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

// ================= GLOBAL VARS =================
bool g_UnlockSkin = false;       // Activate this for Unlock All
uint32_t g_SpoofHeroID = 0;      // Stored when hero is selected
uint32_t g_SpoofSkinID = 0;      // Stored when skin is selected

// Cache Instance (Prevents Freeze/Lag)
void* cachedFakeSkin = nullptr;

// ================= HELPERS =================

// Helper: Create fake skin object once
void* GetFakeSkinInstance(uint32_t id) {
    if (cachedFakeSkin == nullptr) {
        cachedFakeSkin = Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin");
        LOGI("UnlockSkin: Created Fake Skin Instance");
    }
    
    // Set fields dynamically
    if (cachedFakeSkin) {
        static size_t off_iId = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin", "iId");
        static size_t off_iLimit = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin", "iLimitTime");
        
        if (off_iId > 0) *(uint32_t*)((uintptr_t)cachedFakeSkin + off_iId) = id;
        if (off_iLimit > 0) *(uint32_t*)((uintptr_t)cachedFakeSkin + off_iLimit) = 0; // 0 = Permanent
    }
    return cachedFakeSkin;
}

// ================= CORE HOOKS (HERO & SKIN) =================

// 1. BYPASS HERO OWNERSHIP (IsHaveHero)
// Signature: bool IsHaveHero(void* instance, int heroId)
bool (*oIsHaveHero)(void*, int);
bool IsHaveHero(void* instance, int heroId) {
    if (instance == nullptr) return false;
    if (g_UnlockSkin) return true; // Force True: We have all heroes
    return oIsHaveHero(instance, heroId);
}

// 2. BYPASS HERO USABILITY (CanUseHero)
// Signature: bool CanUseHero(void* instance, int heroId)
bool (*oCanUseHero)(void*, int);
bool CanUseHero(void* instance, int heroId) {
    if (g_UnlockSkin) return true; // Force True: We can use any hero
    return oCanUseHero(instance, heroId);
}

// 3. BYPASS SKIN OWNERSHIP (IsHaveSkin)
// Signature: CmdHeroSkin* IsHaveSkin(void* instance, int skinId)
void* (*oIsHaveSkin)(void*, int);
void* IsHaveSkin(void* instance, int skinId) {
    if (instance == nullptr) return nullptr;
    
    // Check original ownership
    void* res = oIsHaveSkin(instance, skinId);
    if (res != nullptr) return res; // Actually owned

    // If not owned but unlock active, return fake skin
    if (g_UnlockSkin) return GetFakeSkinInstance(skinId);
    
    return nullptr;
}

// 4. GET HERO SKIN DATA (GetHeroSkin)
// Signature: CmdHeroSkin GetHeroSkin(void* instance, List<CmdHeroSkin> list, uint skinId)
void* (*oGetHeroSkin)(void*, void*, uint32_t);
void* GetHeroSkin(void* instance, void* list, uint32_t skinId) {
    void* ret = oGetHeroSkin(instance, list, skinId);
    
    // If game says "no skin data" (ret == null), give fake data
    if (ret == nullptr && g_UnlockSkin) {
        return GetFakeSkinInstance(skinId);
    }
    return ret;
}

// 5. UNLOCK ALL BUTTONS (IsCanUseSkin)
// Signature: bool IsCanUseSkin(void* instance, int heroId)
bool (*oIsCanUseSkin)(void*, int);
bool IsCanUseSkin(void* instance, int heroId) {
    if (instance == nullptr) return false;
    if (g_UnlockSkin) return true;
    return oIsCanUseSkin(instance, heroId);
}

// ================= SPOOFING LOGIC (SAFE FOR RANK) =================

// Logic: When "Select" is clicked in Lobby (SendSelectSkin)
void (*oSendSelectSkin)(void*, uint32_t, uint32_t);
void SendSelectSkin(void* thiz, uint32_t skinId, uint32_t heroId) {
    if (!g_UnlockSkin) {
        oSendSelectSkin(thiz, skinId, heroId);
        return;
    }

    g_SpoofHeroID = heroId;
    g_SpoofSkinID = skinId;

    LOGI("UnlockSkin: Spoofing Select - HeroID: %d, DesiredSkin: %d -> Sending Skin 0 to Server", heroId, skinId);

    // --- LOGIC SPOOFING ---
    // Send ID 0 (Default Skin) to Server. 
    oSendSelectSkin(thiz, 0, heroId); 

    // --- FORCE LOCAL UI UPDATE ---
    uint64_t myUid = 0;
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &myUid);
    
    if (myUid > 0) {
        // Find Method BatttleSelectSkin
        void* method = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "UIChooseHero", "BatttleSelectSkin", 2);
        if (!method) method = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "UIChooseHero", "BattleSelectSkin", 2);
        
        if (method) {
            LOGI("UnlockSkin: Forcing Local UI Update for UID: %llu", (unsigned long long)myUid);
            typedef void (*FuncType)(void*, uint64_t, uint32_t);
            FuncType func = (FuncType)method;
            func(thiz, myUid, skinId);
        } else {
            LOGE("UnlockSkin: Failed to find BatttleSelectSkin method");
        }
    } else {
        LOGE("UnlockSkin: Failed to get m_uiID");
    }
}

// Logic: Loading Screen / In-Game (SetPlayerData)
void (*oSetPlayerData)(void*, uintptr_t, uint32_t);
void SetPlayerData(void* thiz, uintptr_t playerInfo, uint32_t camp) {
    if (playerInfo == 0) {
        oSetPlayerData(thiz, playerInfo, camp);
        return;
    }

    if (g_UnlockSkin && g_SpoofSkinID != 0) {
        static size_t off_lUid = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "lUid");
        static size_t off_uiSelHero = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "uiSelHero");
        static size_t off_uiSkinId = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "uiSkinId");

        if (off_lUid > 0 && off_uiSelHero > 0 && off_uiSkinId > 0) {
            uint64_t pUid = *(uint64_t*)(playerInfo + off_lUid);
            
            uint64_t myUid = 0;
            Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &myUid);

            if (pUid == myUid) {
                uint32_t pHero = *(uint32_t*)(playerInfo + off_uiSelHero);
                if (pHero == g_SpoofHeroID) {
                    LOGI("UnlockSkin: Injecting Visual Skin ID %d for Hero %d (UID Matched)", g_SpoofSkinID, pHero);
                    *(uint32_t*)(playerInfo + off_uiSkinId) = g_SpoofSkinID;
                }
            }
        }
    }
    
    oSetPlayerData(thiz, playerInfo, camp);
}

// ================= INITIALIZATION =================

void InitUnlockSkin() {
    LOGI("UnlockSkin: Initializing Hooks...");
    
    // 1. Hook SystemData (Hero & Skin Ownership)
    void* addrIsHaveHero = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsHaveHero", 1);
    if (addrIsHaveHero) Tools::Hook(addrIsHaveHero, (void*)IsHaveHero, (void**)&oIsHaveHero);

    void* addrIsHaveSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsHaveSkin", 1);
    if (addrIsHaveSkin) Tools::Hook(addrIsHaveSkin, (void*)IsHaveSkin, (void**)&oIsHaveSkin);

    void* addrGetHeroSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "GetHeroSkin", 2);
    if (addrGetHeroSkin) Tools::Hook(addrGetHeroSkin, (void*)GetHeroSkin, (void**)&oGetHeroSkin);

    void* addrIsCanUseSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsCanUseSkin", 1);
    if (addrIsCanUseSkin) Tools::Hook(addrIsCanUseSkin, (void*)IsCanUseSkin, (void**)&oIsCanUseSkin);
    
    // 2. Hook ChooseHeroMgr (Hero Usability)
    void* addrCanUseHero = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "ChooseHeroMgr", "CanUseHero", 1);
    if (addrCanUseHero) Tools::Hook(addrCanUseHero, (void*)CanUseHero, (void**)&oCanUseHero);

    // 3. Hook UI & Networking
    void* addrSendSelectSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "UIChooseHero", "SendSelectSkin", 2);
    if (addrSendSelectSkin) Tools::Hook(addrSendSelectSkin, (void*)SendSelectSkin, (void**)&oSendSelectSkin);
    
    // 4. Hook In-Game Data
    void* addrSetPlayerData = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "BattleReceiveMessage", "SetPlayerData", 2);
    if (addrSetPlayerData) Tools::Hook(addrSetPlayerData, (void*)SetPlayerData, (void**)&oSetPlayerData);
    
    LOGI("UnlockSkin: Initialization Complete");
}
