#pragma once
#include "../Include.h"
#include "../include/Utils/Unity/ByNameModding/Tools.h"
#include "../include/Utils/Unity/ByNameModding/Il2Cpp.h"

// Undefine macros to avoid conflicts with Logger.h
#ifdef LOGD
#undef LOGD
#endif
#ifdef LOGE
#undef LOGE
#endif
#ifdef LOGI
#undef LOGI
#endif
#ifdef LOGW
#undef LOGW
#endif

#include "../include/Utils/Logger.h"

// ================= GLOBAL CONFIG =================
bool g_UnlockAll = false;        // Combined Unlock Skin & Unreleased
uint32_t g_MyFakeHeroID = 0;     // Hero we selected
uint32_t g_MyFakeSkinID = 0;     // Premium skin we selected

// Cache Instance (Anti Lag)
void* cachedFakeSkin = nullptr;

// ================= HELPER: FAKE SKIN OBJECT =================
void* GetFakeSkinInstance(uint32_t id) {
    if (cachedFakeSkin == nullptr) {
        cachedFakeSkin = Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin");
        LOGI("UnlockSkin: Created Fake Skin Instance");
    }
    
    if (cachedFakeSkin) {
        // Set ID and LimitTime (0 = Permanent)
        static size_t off_iId = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin", "iId");
        static size_t off_iLimit = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin", "iLimitTime");
        
        if (off_iId > 0) *(uint32_t*)((uintptr_t)cachedFakeSkin + off_iId) = id;
        if (off_iLimit > 0) *(uint32_t*)((uintptr_t)cachedFakeSkin + off_iLimit) = 0; 
    }
    return cachedFakeSkin;
}

// ================= CORE HOOKS (UNLOCK) =================

// 1. UNLOCK HERO & SKIN OWNERSHIP
// bool IsHaveHero(void* instance, int heroId)
bool (*oIsHaveHero)(void*, int);
bool IsHaveHero(void* instance, int heroId) {
    if (instance == nullptr) return false; // Anti-Crash Offline
    if (g_UnlockAll) return true;          // Force Unlock
    return oIsHaveHero(instance, heroId);
}

// bool IsHaveSkin(void* instance, int skinId)
void* (*oIsHaveSkin)(void*, int);
void* IsHaveSkin(void* instance, int skinId) {
    if (instance == nullptr) return nullptr;
    
    // Check real ownership first (so real skins work normally)
    void* res = oIsHaveSkin(instance, skinId);
    if (res != nullptr) return res;

    // If not owned, but mod active -> Fake It
    if (g_UnlockAll) return GetFakeSkinInstance(skinId);
    
    return nullptr;
}

// 2. FILL SKIN DATA (Prevent Empty List / Hidden Unreleased)
// CmdHeroSkin GetHeroSkin(void* instance, List list, uint skinId)
void* (*oGetHeroSkin)(void*, void*, uint32_t);
void* GetHeroSkin(void* instance, void* list, uint32_t skinId) {
    void* ret = oGetHeroSkin(instance, list, skinId);
    
    // If game doesn't find skin (e.g. unreleased or error), inject fake data
    if (ret == nullptr && g_UnlockAll) {
        return GetFakeSkinInstance(skinId);
    }
    return ret;
}

// 3. UNLOCK SELECT/USE BUTTONS
// bool IsCanUseSkin(void* instance, int heroId)
bool (*oIsCanUseSkin)(void*, int);
bool IsCanUseSkin(void* instance, int heroId) {
    if (g_UnlockAll) return true;
    return oIsCanUseSkin(instance, heroId);
}

// bool IsCanUseHero(void* instance, int heroId)
bool (*oIsCanUseHero)(void*, int);
bool IsCanUseHero(void* instance, int heroId) {
    if (g_UnlockAll) return true;
    return oIsCanUseHero(instance, heroId);
}

// ================= SPOOFING LOGIC (RANK/CLASSIC FIX) =================

// Logic: When "SELECT" button is pressed in Lobby
// void SendSelectSkin(void* thiz, uint32_t skinId, uint32_t heroId)
void (*oSendSelectSkin)(void*, uint32_t, uint32_t);
void SendSelectSkin(void* thiz, uint32_t skinId, uint32_t heroId) {
    if (!g_UnlockAll) {
        oSendSelectSkin(thiz, skinId, heroId);
        return;
    }

    // Save our choice in memory
    g_MyFakeHeroID = heroId;
    g_MyFakeSkinID = skinId;

    // Check if we really own this skin
    bool isOwned = false;
    if (oIsHaveSkin) {
        void* res = oIsHaveSkin(nullptr, skinId); // Pass nullptr safely if static check allows, or rely on spoof
        if (res != nullptr) isOwned = true;
    }

    // In this "UnlockAll" mode, we assume everything is spoofed if we want safety, 
    // but the user logic tries to distinguish. 
    // Let's assume if it returns a FakeInstance (which we can't easily check here without casting), it's fake.
    // Simpler: Just spoof if g_UnlockAll is on.
    
    LOGI("UnlockSkin: Selecting Skin %d for Hero %d", skinId, heroId);

    // SPOOF: Send ID 0 (Default) to server to avoid Ban/Verification Error
    oSendSelectSkin(thiz, 0, heroId);
    
    // VISUAL LOCAL: Force UI to change to premium skin
    uint64_t myUid = 0;
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &myUid);
    
    if (myUid > 0) {
        // Call local UI method: BatttleSelectSkin(uid, skinId)
        void* method = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "UIChooseHero", "BatttleSelectSkin", 2);
        if (!method) method = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "UIChooseHero", "BattleSelectSkin", 2);
        
        if (method) {
            typedef void (*FuncType)(void*, uint64_t, uint32_t);
            FuncType func = (FuncType)method;
            func(thiz, myUid, skinId);
            LOGI("UnlockSkin: Forced Local UI Update");
        }
    }
}

// Logic: During Loading / Game Entry (Battle Data)
// Server sends data about who uses what skin. We must manipulate this INSTANTLY.
// void SetPlayerData(void* thiz, uintptr_t playerInfo, uint32_t camp)
void (*oSetPlayerData)(void*, uintptr_t, uint32_t);
void SetPlayerData(void* thiz, uintptr_t playerInfo, uint32_t camp) {
    
    // Call original first to fill struct with server data
    oSetPlayerData(thiz, playerInfo, camp);

    // Safety Offline Check
    if (playerInfo == 0 || !g_UnlockAll) return;

    // Get Player UID from struct
    static size_t off_lUid = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "lUid");
    static size_t off_uiSelHero = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "uiSelHero");
    static size_t off_uiSkinId = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "uiSkinId");

    if (off_lUid == 0 || off_uiSkinId == 0) return;

    uint64_t pUid = *(uint64_t*)(playerInfo + off_lUid);
    
    // Get My UID
    uint64_t myUid = 0;
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &myUid);

    // === OUR OWN VISUAL LOGIC ===
    if (pUid == myUid) {
        uint32_t pHero = *(uint32_t*)(playerInfo + off_uiSelHero);
        // If hero matches what we selected
        if (pHero == g_MyFakeHeroID && g_MyFakeSkinID != 0) {
            // OVERWRITE Default Skin from server with our Premium Skin
            *(uint32_t*)(playerInfo + off_uiSkinId) = g_MyFakeSkinID;
            LOGI("UnlockSkin: Overwrote PlayerData SkinID to %d", g_MyFakeSkinID);
        }
    }
}

// Hook RefreshSkinDic to ensure it doesn't reset during loading (Classic Fix)
// void RefreshSkinDic(void* thiz, uint32_t heroId, uint32_t skinId, uint64_t uid)
void (*oRefreshSkinDic)(void*, uint32_t, uint32_t, uint64_t);
void RefreshSkinDic(void* thiz, uint32_t heroId, uint32_t skinId, uint64_t uid) {
    
    if (g_UnlockAll) {
        uint64_t myUid = 0;
        Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &myUid);
        
        if (uid == myUid && heroId == g_MyFakeHeroID && g_MyFakeSkinID != 0) {
            // Force refresh using our Fake Skin ID
            LOGI("UnlockSkin: Blocked Skin Reset, Enforcing SkinID %d", g_MyFakeSkinID);
            return oRefreshSkinDic(thiz, heroId, g_MyFakeSkinID, uid);
        }
    }
    oRefreshSkinDic(thiz, heroId, skinId, uid);
}

// ================= INITIALIZATION =================

void InitUnlockSkin() {
    LOGI("UnlockSkin: Initializing Merged Logic...");

    // Ownership Logic
    void* addrIsHaveHero = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsHaveHero", 1);
    if (addrIsHaveHero) Tools::Hook(addrIsHaveHero, (void*)IsHaveHero, (void**)&oIsHaveHero);

    void* addrIsHaveSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsHaveSkin", 1);
    if (addrIsHaveSkin) Tools::Hook(addrIsHaveSkin, (void*)IsHaveSkin, (void**)&oIsHaveSkin);

    void* addrGetHeroSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "GetHeroSkin", 2);
    if (addrGetHeroSkin) Tools::Hook(addrGetHeroSkin, (void*)GetHeroSkin, (void**)&oGetHeroSkin);

    void* addrIsCanUseSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsCanUseSkin", 1);
    if (addrIsCanUseSkin) Tools::Hook(addrIsCanUseSkin, (void*)IsCanUseSkin, (void**)&oIsCanUseSkin);

    void* addrIsCanUseHero = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsCanUseHero", 1);
    if (addrIsCanUseHero) Tools::Hook(addrIsCanUseHero, (void*)IsCanUseHero, (void**)&oIsCanUseHero);


    // Spoofing Logic (Lobby & Server Communication)
    void* addrSendSelectSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "UIChooseHero", "SendSelectSkin", 2);
    if (addrSendSelectSkin) Tools::Hook(addrSendSelectSkin, (void*)SendSelectSkin, (void**)&oSendSelectSkin);
    
    // Visual Override (Loading & In-Game)
    void* addrSetPlayerData = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "BattleReceiveMessage", "SetPlayerData", 2);
    if (addrSetPlayerData) Tools::Hook(addrSetPlayerData, (void*)SetPlayerData, (void**)&oSetPlayerData);

    // Refresh Fix
    void* addrRefreshSkinDic = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "UIChooseHero", "RefreshSkinDic", 3);
    if (addrRefreshSkinDic) Tools::Hook(addrRefreshSkinDic, (void*)RefreshSkinDic, (void**)&oRefreshSkinDic);

    LOGI("UnlockSkin: Initialization Complete");
}
