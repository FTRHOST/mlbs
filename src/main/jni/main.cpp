#include <jni.h>
#include <pthread.h>
#include "include/zygisk.h"

#include <chrono> // For std::chrono
#include <thread> // For std::thread
#include <unistd.h> // For pipe() and write()

// Function to safely read from a memory address.
// Returns true on success, false on failure (e.g., unmapped memory).
template<typename T>
bool safe_read(uintptr_t address, T& out_value) {
    if (address == 0) {
        return false;
    }
    // A static pipe is used to check memory readability.
    // This is a common technique to avoid crashing on invalid reads.
    static int p[2] = {-1, -1};
    if (p[0] == -1) {
        if (pipe(p) == -1) {
            // If pipe creation fails, we can't use this check.
            // Failing is safer than falling back to a direct read.
            return false;
        }
    }
    // The write() syscall will return EFAULT if the address is invalid,
    // instead of causing a SIGSEGV crash.
    if (write(p[1], (void*)address, sizeof(T)) == sizeof(T)) {
        // If the write was successful, read the value back from the pipe.
        if (read(p[0], &out_value, sizeof(T)) == sizeof(T)) {
            return true;
        }
    }
    // read/write failed, likely due to an invalid address (EFAULT).
    return false;
}


#include "xdl.h"
#include "dobby.h"
#include "Include.h"



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
#include "feature/BattleData.h"
#include "include/nlohmann/json.hpp" // For JSON serialization

// Instance state global
GlobalState g_State;
bool g_IsWebServerReady = false;

// Software-based battle timer variables
std::chrono::steady_clock::time_point g_battleStartTime;
std::chrono::duration<float> g_elapsedBattleTime(0);
std::atomic<bool> g_isBattleTimerRunning(false);







// --- Implementasi Pengumpul Data & Hooks ---

void UpdatePlayerInfo() {
    __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "UpdatePlayerInfo: Starting comprehensive update.");
    auto battlePlayerList = ((monoList<void *> *(*)(uintptr_t))SystemData_GetBattlePlayerInfo)((uintptr_t)0);
    if (!battlePlayerList) {
        std::lock_guard<std::mutex> lock(g_State.stateMutex);
        if (!g_State.players.empty()) g_State.players.clear();
        return;
    }
    
    std::lock_guard<std::mutex> lock(g_State.stateMutex);
    g_State.players.clear();


    for (int i = 0; i < battlePlayerList->getSize(); i++) {
        void *pawn = battlePlayerList->getItems()[i];
        if (!pawn) continue;

        PlayerData p = {};

        // Helper macro to safely read a value from the pawn offset using safe_read
        #define READ_FIELD(target, type, offset) \
            if (offset > 0) { \
                safe_read<type>(((uintptr_t)pawn + offset), target); \
            }

        // Helper macro to safely read a string using safe_read
        #define READ_STRING(target, offset) \
            if (offset > 0) { \
                MonoString* str_ptr = nullptr; \
                if (safe_read<MonoString*>((uintptr_t)pawn + offset, str_ptr) && str_ptr) { \
                    target = str_ptr->CString(); \
                } \
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
    }
}

// New function to read battle stats
BattleStats GetBattleStats() {
    BattleStats stats = {}; // Initialize with zeros
    void* showFightDataInstance = nullptr;

    // Get the singleton instance of ShowFightData
    Il2CppGetStaticFieldValue(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("ShowFightData"), OBFUSCATE("Instance"), &showFightDataInstance);

    if (showFightDataInstance) {
        // Cast the instance to our layout struct
        auto* pData = static_cast<ShowFightDataTiny_Layout*>(showFightDataInstance);
        
        // Copy the data into our clean struct
        stats.iCampAKill = pData->m_iCampAKill;
        stats.iCampBKill = pData->m_iCampBKill;
        stats.CampAGold = pData->m_CampAGold;
        stats.CampBGold = pData->m_CampBGold;
        stats.CampAExp = pData->m_CampAExp;
        stats.CampBExp = pData->m_CampBExp;
        stats.CampAKillTower = pData->m_CampAKillTower;
        stats.CampBKillTower = pData->m_CampBKillTower;
        stats.CampAKillLingZhu = pData->m_CampAKillLingZhu;
        stats.CampBKillLingZhu = pData->m_CampBKillLingZhu;
        stats.CampAKillShenGui = pData->m_CampAKillShenGui;
        stats.CampBKillShenGui = pData->m_CampBKillShenGui;
    }

    return stats;
    }

float GetBattleTime() {
    if (g_isBattleTimerRunning) {
        // If the timer is running, calculate current elapsed time
        return std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - g_battleStartTime).count();
    } else {
        // If the timer is stopped, return the final recorded duration
        return g_elapsedBattleTime.count();
    }
}

// --- Logika Inti ---

// --- Logika Inti ---
void MonitorBattleState() {
    void *logicBattleManager = nullptr;
    Il2CppGetStaticFieldValue(OBFUSCATE("Assembly-CSharp.dll"), OBFUSCATE(""), OBFUSCATE("LogicBattleManager"), OBFUSCATE("Instance"), &logicBattleManager);
    if (!logicBattleManager) return;

    int currentBattleState = GetBattleState(logicBattleManager);

    if (currentBattleState != g_State.battleState) {
        // --- BATTLE TIMER LOGIC ---
        // If the battle is starting (state 6) and the timer is not already running
        if (currentBattleState == 6 && !g_isBattleTimerRunning) {
            g_isBattleTimerRunning = true;
            g_battleStartTime = std::chrono::steady_clock::now();
            g_elapsedBattleTime = std::chrono::duration<float>(0);
            __android_log_print(ANDROID_LOG_INFO, "MLBS_TIMER", "Battle timer started.");
        } 
        // If the battle has just ended (state 7) and the timer was running
        else if (currentBattleState == 7 && g_isBattleTimerRunning) {
            g_isBattleTimerRunning = false;
            g_elapsedBattleTime = std::chrono::steady_clock::now() - g_battleStartTime;
            __android_log_print(ANDROID_LOG_INFO, "MLBS_TIMER", "Battle timer stopped. Final time: %f", g_elapsedBattleTime.count());
        }
        // --- END BATTLE TIMER LOGIC ---

        std::lock_guard<std::mutex> lock(g_State.stateMutex);
        g_State.battleState = currentBattleState;
        if(currentBattleState != 2) {
            // draftEvents was removed, this is no longer needed.
        }
    }
    
    if (currentBattleState == 2 || currentBattleState == 3) {
        if (g_State.roomInfoEnabled) {
            UpdatePlayerInfo();
        }
    }
}
















void *hack_thread(void*) {
	do {
     sleep(1);
    } while (!isLibraryLoaded(OBFUSCATE("libil2cpp.so")));

    Il2CppAttach(OBFUSCATE("libil2cpp.so"));
    


    
    // Load Config & Start Web Server
    LoadConfig(g_State);
    if (g_State.webServerEnabled) {
        StartWebServer();
    } else {
        __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "Web server is disabled in config. Not starting.");
    }
    
    // Loop Pembacaan Pasif (Looping lambat agar aman)
    while (true) {
        MonitorBattleState(); // Baca data memori
        sleep(1); // Jeda 1 detik (PENTING: Jangan spamming read setiap frame)
    }
	return nullptr;
}


// ===============================================================


// Zygisk Module Implementation (API v4)


// ===============================================================





class MyModule : public zygisk::ModuleBase {


public:


    void onLoad(zygisk::Api *api, JNIEnv *env) override {


        this->api = api;


        this->env = env;


    }





    void postAppSpecialize(const zygisk::AppSpecializeInfo *info) override;





private:


    zygisk::Api *api;


    JNIEnv *env;


};





void MyModule::postAppSpecialize(const zygisk::AppSpecializeInfo* info) {


    const char* process_name = info->nice_name;


    if (process_name == nullptr) {


        return;


    }





    if (strcmp(process_name, "com.mobile.legends") == 0) {


        __android_log_print(ANDROID_LOG_INFO, "MLBS_ZYGISK", "In target process '%s'. Starting hack_thread...", process_name);


        pthread_t hacks;


        pthread_create(&hacks, NULL, hack_thread, NULL);


    }


}





REGISTER_ZYGISK_MODULE(MyModule)





extern "C" jint JNIEXPORT JNI_OnLoad(JavaVM *vm, void *key) {


    jvm = vm;


    return JNI_VERSION_1_6;


}

