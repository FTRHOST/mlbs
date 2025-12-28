#include <jni.h>
#include <pthread.h>
#include <android/input.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <cstring> // for strcmp
#include <array>
#include <unistd.h>
#include <sys/system_properties.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <chrono> // For std::chrono
#include <thread> // For std::thread

#include "xdl.h"
#include "dobby.h"
#include "Include.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_android.h"
#include "Roboto-Regular.h"
#include "Icon.h"
#include "Iconcpp.h"
#include "Vector3.h"

#include "Il2Cpp/il2cpp_dump.h"

#include "include/Hook/KittyMemory/KittyMemory.h"
#include "include/Hook/KittyMemory/MemoryPatch.h"
#include "include/Hook/KittyMemory/KittyScanner.h"
#include "include/Hook/KittyMemory/KittyUtils.h"

#include "obfuscate.h"
#include "Utils.h"

#include "feature/GameClass.h"
#include "feature/ToString.h"
#include "feature/ToString2.h"
#include "Bypass.h"
#include "WebServer.h"
#include "GlobalState.h"
#include "ConfigManager.h"
#include "include/nlohmann/json.hpp" // For JSON serialization

// Instance state global
GlobalState g_State;
bool g_IsWebServerReady = false;

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

std::vector<MemoryPatch> memoryPatches;
std::vector<uint64_t> offsetVector;

bool setup; // Still used by ESP or other features
using namespace ImGui;

#define HOOKAF(ret, func, ...) \
    ret (*orig##func)(__VA_ARGS__); \
    ret my##func(__VA_ARGS__)

HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent*)thiz);
    return;
}

std::string GetProp(const char* key) {
    char value[PROP_VALUE_MAX];
    __system_property_get(key, value);
    return std::string(value);
}

int screenWidth;
int screenHeight;

int32_t (*orig_ANativeWindow_getWidth)(ANativeWindow* window);
int32_t _ANativeWindow_getWidth(ANativeWindow* window) {
	screenWidth = orig_ANativeWindow_getWidth(window);
	return orig_ANativeWindow_getWidth(window);
}

int32_t (*orig_ANativeWindow_getHeight)(ANativeWindow* window);
int32_t _ANativeWindow_getHeight(ANativeWindow* window) {
	screenHeight = orig_ANativeWindow_getHeight(window);
	return orig_ANativeWindow_getHeight(window);
}

bool esp = false; // Still used by ESP
bool drawEnemyBox = false; // Still used by ESP
ImVec4 espLineColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
static float espMaxDistance = 999.0f;
static float linePositionX = 0.5f;
std::vector<Vector3> espPoints;

void (*setpitchyaw)(float, float);
Vector3 (*WorldToScreenPoint)(void* instance, Vector3);
Vector3 (*get_position)(void* instance);

void* (*get_transform)(void* instance);
void (*old_Player_update)(void* player);

struct ESPBox {
    Vector3 top;
    Vector3 bottom;
};
std::vector<ESPBox> espBoxes;

void Player_update(void* player) {
    if (!player) return;

    if (old_Player_update) {
        old_Player_update(player);
    }
}

void DrawESP() {
    if (!esp || espBoxes.empty()) return;

    ImU32 espLineColorU32 = ImGui::ColorConvertFloat4ToU32(espLineColor);
    ImVec2 screenTopLine = ImVec2(screenWidth * linePositionX, 0.0f);
    auto* draw = ImGui::GetBackgroundDrawList();

    for (const auto& box : espBoxes) {
        if (box.bottom.z < 1.0f || box.top.z < 1.0f) continue;

        float boxHeight = fabsf(box.bottom.y - box.top.y);
        float boxWidth = boxHeight * 0.6f;

        ImVec2 boxMin = ImVec2(box.top.x - (boxWidth / 2), box.top.y);
        ImVec2 boxMax = ImVec2(box.top.x + (boxWidth / 2), box.top.y + boxHeight);

        if (drawEnemyBox)
            draw->AddRect(boxMin, boxMax, espLineColorU32, 0.0f, 15, 1.5f);

        draw->AddLine(screenTopLine, ImVec2(box.top.x, box.top.y), espLineColorU32, 2.0f);
    }

    espBoxes.clear();
}

// --- Implementasi Pengumpul Data & Hooks ---

void UpdatePlayerInfo() {
    __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "UpdatePlayerInfo: Starting comprehensive update.");
    auto battlePlayerList = ((monoList<void *> *(*)(uintptr_t))SystemData_GetBattlePlayerInfo)((uintptr_t)0);
    if (!battlePlayerList) {
        std::lock_guard<std::mutex> lock(g_State.stateMutex);
        if (!g_State.players.empty()) g_State.players.clear();
        if (!g_State.draftEvents.empty()) g_State.draftEvents.clear();
        return;
    }
    
    std::lock_guard<std::mutex> lock(g_State.stateMutex);
    g_State.players.clear();
    g_State.draftEvents.clear();
    std::unordered_set<uint32_t> uniqueBans;

    for (int i = 0; i < battlePlayerList->getSize(); i++) {
        void *pawn = battlePlayerList->getItems()[i];
        if (!pawn) continue;

        PlayerData p = {};

        // Helper macro to safely read a value from the pawn offset
        #define READ_FIELD(target, type, offset) \
            if(offset > 0) target = *(type*)((uintptr_t)pawn + offset);

        // Helper macro to safely read a string
        #define READ_STRING(target, offset) \
            if(offset > 0) { \
                MonoString* str = *(MonoString**)((uintptr_t)pawn + offset); \
                if(str) target = str->CString(); \
            }
        
        // Read all fields from RoomData
        READ_FIELD(p.bAutoConditionNew, bool, SystemData_RoomData_bAutoConditionNew);
        READ_FIELD(p.bShowSeasonAchieve, bool, SystemData_RoomData_bShowSeasonAchieve);
        READ_FIELD(p.iStyleBoardId, uint32_t, SystemData_RoomData_iStyleBoardId);
        READ_FIELD(p.iMatchEffectId, uint32_t, SystemData_RoomData_iMatchEffectId);
        READ_FIELD(p.iDayBreakNo1Count, uint32_t, SystemData_RoomData_iDayBreakNo1Count);
        READ_FIELD(p.lUid, uint64_t, SystemData_RoomData_lUid);
        READ_FIELD(p.bUid, uint64_t, SystemData_RoomData_bUid);
        READ_FIELD(p.iCamp, uint32_t, SystemData_RoomData_iCamp);
        READ_FIELD(p.iPos, uint32_t, SystemData_RoomData_iPos);
        READ_FIELD(p.bAutoReadySelect, bool, SystemData_RoomData_bAutoReadySelect);
        READ_STRING(p._sName, SystemData_RoomData__sName);
        READ_FIELD(p.bRobot, bool, SystemData_RoomData_bRobot);
        READ_FIELD(p.heroid, uint32_t, SystemData_RoomData_heroid);
        READ_FIELD(p.heroskin, uint32_t, SystemData_RoomData_heroskin);
        READ_FIELD(p.headID, uint32_t, SystemData_RoomData_headID);
        READ_FIELD(p.uiSex, uint32_t, SystemData_RoomData_uiSex);
        READ_FIELD(p.country, uint32_t, SystemData_RoomData_country);
        READ_FIELD(p.uiZoneId, uint32_t, SystemData_RoomData_uiZoneId);
        READ_FIELD(p.summonSkillId, int32_t, SystemData_RoomData_summonSkillId);
        READ_FIELD(p.runeId, int32_t, SystemData_RoomData_runeId);
        READ_FIELD(p.runeLv, int32_t, SystemData_RoomData_runeLv);
        READ_STRING(p.facePath, SystemData_RoomData_facePath);
        READ_FIELD(p.faceBorder, uint32_t, SystemData_RoomData_faceBorder);
        READ_FIELD(p.bStarVip, bool, SystemData_RoomData_bStarVip);
        READ_FIELD(p.bMCStarVip, bool, SystemData_RoomData_bMCStarVip);
        READ_FIELD(p.bMCStarVipPlus, bool, SystemData_RoomData_bMCStarVipPlus);
        READ_FIELD(p.ulRoomID, uint64_t, SystemData_RoomData_ulRoomID);
        READ_FIELD(p.iConBlackRoomId, uint64_t, SystemData_RoomData_iConBlackRoomId);
        READ_FIELD(p.banHero, uint32_t, SystemData_RoomData_banHero);
        READ_FIELD(p.uiBattlePlayerType, uint32_t, SystemData_RoomData_uiBattlePlayerType);
        READ_STRING(p.sThisLoginCountry, SystemData_RoomData_sThisLoginCountry);
        READ_STRING(p.sCreateRoleCountry, SystemData_RoomData_sCreateRoleCountry);
        READ_FIELD(p.uiLanguage, uint32_t, SystemData_RoomData_uiLanguage);
        READ_FIELD(p.bIsOpenLive, bool, SystemData_RoomData_bIsOpenLive);
        READ_FIELD(p.iTeamId, uint64_t, SystemData_RoomData_iTeamId);
        READ_FIELD(p.iTeamNationId, uint64_t, SystemData_RoomData_iTeamNationId);
        READ_STRING(p._steamName, SystemData_RoomData__steamName);
        READ_STRING(p._steamSimpleName, SystemData_RoomData__steamSimpleName);
        READ_FIELD(p.iCertify, uint32_t, SystemData_RoomData_iCertify);
        READ_FIELD(p.uiRankLevel, uint32_t, SystemData_RoomData_uiRankLevel);
        READ_FIELD(p.uiPVPRank, uint32_t, SystemData_RoomData_uiPVPRank);
        READ_FIELD(p.bRankReview, bool, SystemData_RoomData_bRankReview);
        READ_FIELD(p.iElo, uint32_t, SystemData_RoomData_iElo);
        READ_FIELD(p.uiRoleLevel, uint32_t, SystemData_RoomData_uiRoleLevel);
        READ_FIELD(p.bNewPlayer, bool, SystemData_RoomData_bNewPlayer);
        READ_FIELD(p.iRoad, uint32_t, SystemData_RoomData_iRoad);
        READ_FIELD(p.uiSkinSource, uint32_t, SystemData_RoomData_uiSkinSource);
        READ_FIELD(p.iFighterType, uint32_t, SystemData_RoomData_iFighterType);
        READ_FIELD(p.iWorldCupSupportCountry, uint32_t, SystemData_RoomData_iWorldCupSupportCountry);
        READ_FIELD(p.iHeroLevel, uint32_t, SystemData_RoomData_iHeroLevel);
        READ_FIELD(p.iHeroSubLevel, uint32_t, SystemData_RoomData_iHeroSubLevel);
        READ_FIELD(p.iHeroPowerLevel, uint32_t, SystemData_RoomData_iHeroPowerLevel);
        READ_FIELD(p.iActCamp, uint32_t, SystemData_RoomData_iActCamp);
        READ_FIELD(p.mHeroMission, uint32_t, SystemData_RoomData_mHeroMission);
        READ_FIELD(p.mSkinPaint, uint32_t, SystemData_RoomData_mSkinPaint);
        READ_STRING(p.sClientVersion, SystemData_RoomData_sClientVersion);
        READ_FIELD(p.uiHolyStatue, uint32_t, SystemData_RoomData_uiHolyStatue);
        READ_FIELD(p.uiKamon, uint32_t, SystemData_RoomData_uiKamon);
        READ_FIELD(p.uiUserMapID, uint32_t, SystemData_RoomData_uiUserMapID);
        READ_FIELD(p.iSurviveRank, uint32_t, SystemData_RoomData_iSurviveRank);
        READ_FIELD(p.iDefenceRankID, uint32_t, SystemData_RoomData_iDefenceRankID);
        READ_FIELD(p.iLeagueWCNum, uint32_t, SystemData_RoomData_iLeagueWCNum);
        READ_FIELD(p.iLeagueFCNum, uint32_t, SystemData_RoomData_iLeagueFCNum);
        READ_FIELD(p.iMPLCertifyTime, uint32_t, SystemData_RoomData_iMPLCertifyTime);
        READ_FIELD(p.iMPLCertifyID, uint32_t, SystemData_RoomData_iMPLCertifyID);
        READ_FIELD(p.iHeroUseCount, uint32_t, SystemData_RoomData_iHeroUseCount);
        READ_FIELD(p.iMythPoint, uint32_t, SystemData_RoomData_iMythPoint);
        READ_FIELD(p.bMythEvaled, bool, SystemData_RoomData_bMythEvaled);
        READ_FIELD(p.iDefenceFlag, uint32_t, SystemData_RoomData_iDefenceFlag);
        READ_FIELD(p.iDefenPoint, uint32_t, SystemData_RoomData_iDefenPoint);
        READ_FIELD(p.iDefenceMap, uint32_t, SystemData_RoomData_iDefenceMap);
        READ_FIELD(p.iAIType, uint32_t, SystemData_RoomData_iAIType);
        READ_FIELD(p.iAISeed, uint32_t, SystemData_RoomData_iAISeed);
        READ_STRING(p.sAiName, SystemData_RoomData_sAiName);
        READ_FIELD(p.iWarmValue, uint32_t, SystemData_RoomData_iWarmValue);
        READ_FIELD(p.uiAircraftIDChooose, uint32_t, SystemData_RoomData_uiAircraftIDChooose);
        READ_FIELD(p.uiHeroIDChoose, uint32_t, SystemData_RoomData_uiHeroIDChoose);
        READ_FIELD(p.uiHeroSkinIDChoose, uint32_t, SystemData_RoomData_uiHeroSkinIDChoose);
        READ_FIELD(p.uiMapIDChoose, uint32_t, SystemData_RoomData_uiMapIDChoose);
        READ_FIELD(p.uiMapSkinIDChoose, uint32_t, SystemData_RoomData_uiMapSkinIDChoose);
        READ_FIELD(p.uiDefenceRankScore, uint32_t, SystemData_RoomData_uiDefenceRankScore);
        READ_FIELD(p.bBanChat, bool, SystemData_RoomData_bBanChat);
        READ_FIELD(p.iChatBanFinishTime, uint32_t, SystemData_RoomData_iChatBanFinishTime);
        READ_FIELD(p.iChatBanBattleNum, uint32_t, SystemData_RoomData_iChatBanBattleNum);
        
        // Populate legacy fields for compatibility
        p.name = p._sName;
        p.uid = std::to_string(p.lUid) + "(" + std::to_string(p.uiZoneId) + ")";
        p.rank = RankToString(p.uiRankLevel, p.iMythPoint);
        p.spell = SpellToString(p.summonSkillId);
        p.heroName = HeroToString(p.heroid);
        p.camp = p.iCamp;
        p.heroId = p.heroid;
        p.spellId = p.summonSkillId;
        p.rankLevel = p.uiRankLevel;

        g_State.players.push_back(p);

        // Handle ban event
        if (p.banHero != 0 && uniqueBans.find(p.banHero) == uniqueBans.end()) {
            DraftEvent ev;
            ev.heroName = HeroToString(p.banHero);
            ev.eventType = "BAN";
            ev.playerName = (p.iCamp == 1) ? "Team Blue" : "Team Red";
            g_State.draftEvents.push_back(ev);
            uniqueBans.insert(p.banHero);
        }
    }
}


// --- Logika Inti ---

void MonitorBattleState() {
    void *logicBattleManager = nullptr;
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "LogicBattleManager", "Instance", &logicBattleManager);
    if (!logicBattleManager) return;

    int currentBattleState = GetBattleState(logicBattleManager);

    if (currentBattleState != g_State.battleState) {
        std::lock_guard<std::mutex> lock(g_State.stateMutex);
        g_State.battleState = currentBattleState;
        if(currentBattleState != 2) {
            
        }
    }
    
    if (currentBattleState == 2 || currentBattleState == 3) {
        if (g_State.roomInfoEnabled) {
            UpdatePlayerInfo();
        }
    }
}

// --- UI Mod Menu Utama ---
void DrawModMenu() {
    // Kontrol visibilitas menu sekarang menggunakan state global
    if (!g_State.showMenu) return;

    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    // Berikan state global ke ImGui::Begin, tombol 'X' akan memodifikasinya
    ImGui::Begin("MLBS v0.2 (Diagnostic)", &g_State.showMenu);

    // --- Bagian Main ---
    ImGui::Text("Pengaturan Fitur Utama");
    ImGui::Separator();
    
    // Tambahkan checkbox untuk mengontrol visibilitas menu itu sendiri
    bool showmenu_changed = ImGui::Checkbox("Tampilkan Menu", &g_State.showMenu);

    bool roominfo_changed = ImGui::Checkbox("Aktifkan Room Info", &g_State.roomInfoEnabled);
    ImGui::SameLine();
    ImGui::TextColored(g_State.roomInfoEnabled ? ImVec4(0,1,0,1) : ImVec4(1,0,0,1), g_State.roomInfoEnabled ? "ON" : "OFF");

    bool webserver_changed = ImGui::Checkbox("Aktifkan Web Server", &g_State.webServerEnabled);
    ImGui::SameLine();
    ImGui::TextColored(g_State.webServerEnabled ? ImVec4(0,1,0,1) : ImVec4(1,0,0,1), g_State.webServerEnabled ? "ON" : "OFF");

    if (showmenu_changed || roominfo_changed || webserver_changed) {
        if (webserver_changed) {
            if (g_State.webServerEnabled) {
                StartWebServer();
            } else {
                StopWebServer();
            }
        }
        SaveConfig(g_State);
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spasi

    // --- Bagian Room Info ---
    ImGui::Text("Room Info");
    ImGui::Separator();
    if (!g_State.roomInfoEnabled) {
        ImGui::Text("Fitur 'Room Info' sedang dinonaktifkan. Aktifkan di atas.");
    } else {
        std::lock_guard<std::mutex> lock(g_State.stateMutex);
        ImGui::Text("Jumlah pemain terdeteksi: %zu", g_State.players.size());
        
        // Tampilkan tabel jika ada pemain
        if (!g_State.players.empty()) {
            if (ImGui::BeginTable("PlayersTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("Camp");
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Rank");
                ImGui::TableSetupColumn("Hero");
                ImGui::TableSetupColumn("Spell");
                ImGui::TableSetupColumn("UID");
                ImGui::TableHeadersRow();

                for (const auto& player : g_State.players) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImVec4 camp_color = (player.camp == 1) ? ImVec4(0.2, 0.5, 1, 1) : ImVec4(1, 0.3, 0.3, 1);
                    ImGui::TextColored(camp_color, "%d", player.camp);
                    
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", player.name.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s (%d)", player.rank.c_str(), player.rankLevel);

                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%s (%d)", player.heroName.c_str(), player.heroId);

                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text("%s (%d)", player.spell.c_str(), player.spellId);

                    ImGui::TableSetColumnIndex(5);
                    ImGui::Text("%s", player.uid.c_str());
                }
                ImGui::EndTable();
            }
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spasi

    // --- Bagian Banned Heroes ---
    ImGui::Text("Banned Heroes");
    ImGui::Separator();
    if (!g_State.roomInfoEnabled) {
        ImGui::Text("Fitur 'Room Info' sedang dinonaktifkan. Aktifkan di atas untuk melihat banned heroes.");
    } else {
        std::lock_guard<std::mutex> lock(g_State.stateMutex);
        if (g_State.draftEvents.empty()) {
            ImGui::Text("Tidak ada hero yang di-ban terdeteksi.");
        } else {
            if (ImGui::BeginTable("BannedHeroesTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                ImGui::TableSetupColumn("Camp");
                ImGui::TableSetupColumn("Hero");
                ImGui::TableSetupColumn("Event Type");
                ImGui::TableHeadersRow();

                for (const auto& ev : g_State.draftEvents) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImVec4 camp_color = ImVec4(1, 1, 1, 1); // Default color
                    if (ev.playerName.find("Blue") != std::string::npos) {
                        camp_color = ImVec4(0.2, 0.5, 1, 1);
                    } else if (ev.playerName.find("Red") != std::string::npos) {
                        camp_color = ImVec4(1, 0.3, 0.3, 1);
                    }
                    ImGui::TextColored(camp_color, "%s", ev.playerName.c_str());
                    
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", ev.heroName.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", ev.eventType.c_str());
                }
                ImGui::EndTable();
            }
        }
    }
    ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spasi

    // --- Bagian Server Info ---
    ImGui::Text("Informasi Server & Koneksi");
    ImGui::Separator();
    ImGui::Text("Status Server Web:");
    ImGui::SameLine();
    if (g_IsWebServerReady) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Running on port 2626");
        ImGui::Text("Gunakan skrip atau buka browser di PC/HP lain untuk akses panel.");
    } else {
        if (g_State.webServerEnabled) {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Starting...");
        } else {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "OFF");
        }
    }

    ImGui::End();
}



void SetupImgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    int(*get_width)(void*);
    int(*get_height)(void*);
    get_width = (int (*)(void*)) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Screen", "get_width", 0);
    get_height = (int (*)(void*)) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Screen", "get_height", 0);
	io.DisplaySize = ImVec2((float)get_width(0), (float)get_height(0));
	ImGui::StyleColorsDark();
	ImGuiStyle *style = &ImGui::GetStyle();

	style->Alpha = 1.0f;
	style->WindowTitleAlign = ImVec2(0.5, 0.5);
	style->PopupRounding = 3;
    style->WindowPadding = ImVec2(4, 4);
    style->FramePadding  = ImVec2(2, 2);
    style->ItemSpacing   = ImVec2(2, 2);
    style->ScrollbarSize = 17;
    style->WindowBorderSize = 1;
    style->ChildBorderSize  = 1;
    style->PopupBorderSize  = 3;
    style->FrameBorderSize  = 1;
    style->WindowRounding    = 3;
    style->ChildRounding     = 3;
    style->FrameRounding     = 3;
    style->ScrollbarRounding = 2;
    style->GrabRounding      = 3;
    ImGui_ImplOpenGL3_Init("#version 100");

    ImFontConfig font_cfg;
    io.Fonts->AddFontFromMemoryTTF(&Roboto_Regular, sizeof(Roboto_Regular), 40.0, &font_cfg, io.Fonts->GetGlyphRangesCyrillic());
    ImGui::GetStyle().ScaleAllSizes(3.0f);
}


bool clearMousePos = true;
struct UnityEngine_Vector2_Fields {
    float x;
    float y;
};

struct UnityEngine_Vector2_o {
    UnityEngine_Vector2_Fields fields;
};

enum TouchPhase {
    Began = 0,
    Moved = 1,
    Stationary = 2,
    Ended = 3,
    Canceled = 4
};

struct UnityEngine_Touch_Fields {
    int32_t m_FingerId;
    struct UnityEngine_Vector2_o m_Position;
    struct UnityEngine_Vector2_o m_RawPosition;
    struct UnityEngine_Vector2_o m_PositionDelta;
    float m_TimeDelta;
    int32_t m_TapCount;
    int32_t m_Phase;
    int32_t m_Type;
    float m_Pressure;
    float m_maximumPossiblePressure;
    float m_Radius;
    float m_fRadiusVariance;
    float m_AltitudeAngle;
    float m_AzimuthAngle;
};


EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {

    static bool is_setup = false;
    static bool should_clear_mouse_pos = false;

    if (!is_setup) {
		SetupImgui();
        is_setup = true;
    }

    // Monitor game state and update global state
    MonitorBattleState();

    ImGuiIO &io = ImGui::GetIO();
    int (*TouchCount)(void*) = (int (*)(void*)) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Input", "get_touchCount", 0);
    int touchCount = TouchCount(nullptr);
    if (touchCount > 0) {
        UnityEngine_Touch_Fields touch = ((UnityEngine_Touch_Fields (*)(int)) Il2CppGetMethodOffset("UnityEngine.dll", "UnityEngine", "Input", "GetTouch", 1)) (0);
        float reverseY = io.DisplaySize.y - touch.m_Position.fields.y;

        switch (touch.m_Phase) {
            case TouchPhase::Began:
            case TouchPhase::Stationary:
                io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                io.MouseDown[0] = true;
                break;
            case TouchPhase::Ended:
            case TouchPhase::Canceled:
                io.MouseDown[0] = false;
                should_clear_mouse_pos = true;
                break;
            case TouchPhase::Moved:
                io.MousePos = ImVec2(touch.m_Position.fields.x, reverseY);
                break;
            default:
                break;
        }
    } else {
        io.MouseDown[0] = false;
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
	DrawESP();
	DrawModMenu(); // Call the new main menu UI
	ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();
    if (should_clear_mouse_pos) {
        io.MousePos = ImVec2(-1, -1);
        should_clear_mouse_pos = false;
    }
    return old_eglSwapBuffers(dpy, surface);
}


void *imgui_go(void*) {
    void *handle_egl = xdl_open("libEGL.so", XDL_DEFAULT);
    void *handle_input = xdl_open("libinput.so", XDL_DEFAULT);

    void *xdl_sym_egl = xdl_sym(handle_egl, "eglSwapBuffers", nullptr);
    void *xdl_sym_input = xdl_sym(handle_input, "_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE", nullptr);

	DobbyHook(xdl_sym_egl, (void*)hook_eglSwapBuffers, (void**)&old_eglSwapBuffers);
    DobbyHook(xdl_sym_input, (void*)myInput, (void**)&origInput);

	DobbyHook((void *) DobbySymbolResolver(OBFUSCATE("/system/lib/libandroid.so"), OBFUSCATE("ANativeWindow_getWidth")), (void *) _ANativeWindow_getWidth, (void **) &orig_ANativeWindow_getWidth);
	DobbyHook((void *) DobbySymbolResolver(OBFUSCATE("/system/lib/libandroid.so"), OBFUSCATE("ANativeWindow_getHeight")), (void *) _ANativeWindow_getHeight, (void **) &orig_ANativeWindow_getHeight);

    pthread_exit(nullptr);
}

void *hack_thread(void*) {
	do {
     sleep(1);
    } while (!isLibraryLoaded("libil2cpp.so"));

    Il2CppAttach("libil2cpp.so");
    
	WorldToScreenPoint = (Vector3(*)(void*, Vector3)) getAbsoluteAddress("libil2cpp.so", 0x0);
    get_position = (Vector3 (*)(void *)) getAbsoluteAddress("libil2cpp.so", 0x0);
    get_transform = (void *(*)(void*)) getAbsoluteAddress("libil2cpp.so", 0x0);
    DobbyHook((void *)getAbsoluteAddress("libil2cpp.so", 0x0), (void *) &Player_update, (void **) &old_Player_update);

    
    // Add a delay to prevent race condition on startup
    sleep(3);

    LoadConfig(g_State);
    if (g_State.webServerEnabled) {
        StartWebServer();
    } else {
        __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "Web server is disabled in config. Not starting.");
    }
    
	return nullptr;
}

__attribute__((constructor))
void lib_main() {
    pthread_t hacks;
    pthread_create(&hacks, NULL, imgui_go, NULL);
    pthread_create(&hacks, NULL, hack_thread, NULL);
}

extern "C" jint JNIEXPORT JNI_OnLoad(JavaVM *vm, void *key) {
     jvm = vm;
    JNIEnv *env;
    vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    return JNI_VERSION_1_6;
}
