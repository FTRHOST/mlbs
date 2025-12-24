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
    __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "UpdatePlayerInfo: Starting.");
    void *logicBattleManager = nullptr;
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "LogicBattleManager", "Instance", &logicBattleManager);
    if (!logicBattleManager) {
        __android_log_print(ANDROID_LOG_WARN, "MLBS_HOOK", "UpdatePlayerInfo: LogicBattleManager not found.");
        return;
    }

    auto battlePlayerList = ((MonoList<void **> *(*)(uintptr_t))SystemData_GetBattlePlayerInfo)((uintptr_t)0);
    if (!battlePlayerList) {
        std::lock_guard<std::mutex> lock(g_State.stateMutex);
        if (!g_State.players.empty()) {
            g_State.players.clear();
        }
        return;
    }
    
    int playerListSize = battlePlayerList->getSize();
    
    std::lock_guard<std::mutex> lock(g_State.stateMutex);
    g_State.players.clear();

    for (int i = 0; i < playerListSize; i++) {
        void *pawn = battlePlayerList->getItems()[i];
        if (!pawn) continue;

        // Safety check to prevent crash from race condition in bot matches
        auto _sName = *(MonoString **)((uintptr_t)pawn + 0x40);
        if (!_sName) continue;
        
        const char* playerName = _sName->CString();
        __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "UpdatePlayerInfo: Processing player %d: %s", i, playerName ? playerName : "NULL");

        auto lUid = *(uint64_t *)((uintptr_t)pawn + 0x20);
        auto uiZoneId = *(uint32_t *)((uintptr_t)pawn + 0x60);
        auto uiRankLevel = *(uint32_t *)((uintptr_t)pawn + 0x128);
        auto iMythPoint = *(uint32_t *)((uintptr_t)pawn + 0x1cc);
        auto summonSkillId = *(int *)((uintptr_t)pawn + 0x64);
        auto heroid = *(uint32_t *)((uintptr_t)pawn + 0x4c);
        auto iCamp = *(int *)((uintptr_t)pawn + 0x30);

        PlayerData p;
        p.name = playerName;
        p.uid = std::to_string(lUid) + "(" + std::to_string(uiZoneId) + ")";
        p.rank = RankToString(uiRankLevel, iMythPoint);
        p.spell = SpellToString(summonSkillId);
        p.heroName = HeroToString(heroid);
        p.camp = iCamp;
        // Store raw data
        p.rankLevel = uiRankLevel;
        p.spellId = summonSkillId;
        p.heroId = heroid;
        g_State.players.push_back(p);
    }
}

void (*origOnChangeHeroConfirm)(void*, int, int, int, int, int, int, void*);
void myOnChangeHeroConfirm(void* instance, int param, int heroId, int skinId, int summonSkillId, int runeId, int runeLevel, void* mapRune) {
    if (origOnChangeHeroConfirm) {
        origOnChangeHeroConfirm(instance, param, heroId, skinId, summonSkillId, runeId, runeLevel, mapRune);
    }
    
    if (g_State.battleState == 2) {
        std::lock_guard<std::mutex> lock(g_State.stateMutex);
        DraftEvent ev;
        ev.playerName = "Unknown Player"; 
        ev.heroName = HeroToString(heroId);
        ev.eventType = "PICK";
        g_State.draftEvents.push_back(ev);
    }
}

// Hook for Ban Event
void (*origOnChangeHeroBan)(void*, int, int);
void myOnChangeHeroBan(void* instance, int campId, int heroId) {
    if (origOnChangeHeroBan) {
        origOnChangeHeroBan(instance, campId, heroId);
    }

    if (g_State.battleState == 2) {
        std::lock_guard<std::mutex> lock(g_State.stateMutex);
        DraftEvent ev;
        ev.playerName = (campId == 1) ? "Team Blue" : "Team Red";
        ev.heroName = HeroToString(heroId);
        ev.eventType = "BAN";
        g_State.draftEvents.push_back(ev);
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
            g_State.draftEvents.clear();
        }
    }
    
    if (currentBattleState == 2 || currentBattleState == 3) {
        if (g_State.roomInfoEnabled) {
            UpdatePlayerInfo();
        }
    }
}

// --- UI minimalis ---
void DrawConnectionInfo() {
    static bool show_menu = true;
    if (!show_menu) return;

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.7f);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    
    std::string device_ip = "127.0.0.1"; // Placeholder

    if (ImGui::Begin("MLBS API Server", &show_menu, window_flags)) {
        ImGui::Text("API Server Status:");
        ImGui::SameLine();
        if (g_IsWebServerReady) {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Running!");
            ImGui::Text("Access Panel:");
            ImGui::Text("http://%s:8080/panel", device_ip.c_str());
            ImGui::Separator();
            ImGui::Checkbox("Enable Cheat Bypass", &g_State.bypassEnabled);
        } else {
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "Starting...");
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

    // Apply bypass logic
    ApplyBypass();

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
	DrawConnectionInfo(); // Call the new minimal UI
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

    // Hook untuk event draft pick
    // Nama kelasnya adalah "BattleReceiveMessage" berdasarkan pencarian sebelumnya
    void* onChangeHeroConfirmAddr = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "BattleReceiveMessage", "OnChangeHeroConfirm", 7);
    if (onChangeHeroConfirmAddr) {
        DobbyHook(onChangeHeroConfirmAddr, (void*)myOnChangeHeroConfirm, (void**)&origOnChangeHeroConfirm);
        __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "Hooked OnChangeHeroConfirm");
    } else {
        __android_log_print(ANDROID_LOG_WARN, "MLBS_HOOK", "Failed to find OnChangeHeroConfirm address.");
    }

    // Hook untuk event draft ban
    void* onChangeHeroBanAddr = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "BattleReceiveMessage", "OnChangeHeroBan", 2);
    if (onChangeHeroBanAddr) {
        DobbyHook(onChangeHeroBanAddr, (void*)myOnChangeHeroBan, (void**)&origOnChangeHeroBan);
        __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "Hooked OnChangeHeroBan");
    } else {
        __android_log_print(ANDROID_LOG_WARN, "MLBS_HOOK", "Failed to find OnChangeHeroBan address.");
    }
    
    // Add a delay to prevent race condition on startup
    sleep(3);

    LoadConfig(g_State);
    StartWebServer();
    
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