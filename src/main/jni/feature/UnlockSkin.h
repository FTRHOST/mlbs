#pragma once
#include "../include/Utils/Unity/ByNameModding/Tools.h"
#include "../include/Utils/Unity/ByNameModding/Il2Cpp.h"

// Variabel Global
extern bool g_UnlockSkins;
uint32_t g_SpoofedHeroID = 0;
uint32_t g_SpoofedSkinID = 0;

// Cache Instance (Mencegah Freeze/Lag)
void* cachedFakeSkin = nullptr;

// Helper: Membuat Object Fake Skin sekali saja
void* GetFakeSkinInstance(uint32_t id) {
    if (cachedFakeSkin == nullptr) {
        // Cari Class CmdHeroSkin
        void* klass = Il2CppGetClassType("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin");
        if (klass) {
            cachedFakeSkin = Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin");
        }
    }

    if (cachedFakeSkin) {
        // Kita set field iId, iLimitTime, iSource secara manual via offset dynamic
        static size_t off_iId = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin", "iId");
        static size_t off_iLimit = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin", "iLimitTime");

        if (off_iId != -1) *(uint32_t*)((uintptr_t)cachedFakeSkin + off_iId) = id;
        if (off_iLimit != -1) *(uint32_t*)((uintptr_t)cachedFakeSkin + off_iLimit) = 0; // Permanent
    }
    return cachedFakeSkin;
}

// --- Hooks ---

// Hook: IsHaveHero (Bypass kepemilikan hero agar bisa pilih skin)
bool (*oIsHaveHero)(void*, int);
bool IsHaveHero(void* instance, int heroId) {
    // PENTING: Cek instance null untuk mencegah crash offline
    if (instance == nullptr) return false;

    if (g_UnlockSkins) return true;
    return oIsHaveHero(instance, heroId);
}

// Hook: IsHaveSkin (Pura-pura punya skin)
bool (*oIsHaveSkin)(void*, int);
bool IsHaveSkin(void* instance, int skinId) {
    // PENTING: Cek instance null untuk mencegah crash offline
    if (instance == nullptr) return false;

    if (g_UnlockSkins) return true; // Force True
    return oIsHaveSkin(instance, skinId);
}

// Hook: IsCanUseSkin (Agar tombol Use menyala)
bool (*oIsCanUseSkin)(void*, int);
bool IsCanUseSkin(void* instance, int heroId) {
    // PENTING: Cek instance null untuk mencegah crash offline
    if (instance == nullptr) return false;

    if (g_UnlockSkins) return true;
    return oIsCanUseSkin(instance, heroId);
}

// Hook: GetHeroSkin (Mengembalikan object skin palsu)
void* (*oGetHeroSkin)(void*, uintptr_t, uint32_t);
void* GetHeroSkin(void* instance, uintptr_t heroSkinsList, uint32_t skinId) {
    void* ret = oGetHeroSkin(instance, heroSkinsList, skinId);

    // Jika game tidak menemukan skin (ret null) tapi cheat aktif, berikan skin palsu
    if (ret == nullptr && g_UnlockSkins) {
        return GetFakeSkinInstance(skinId);
    }
    return ret;
}

// --- SPOOFING LOGIC ---

// Hook SendSelectSkin (Packet ke Server)
void (*oSendSelectSkin)(void*, uint32_t, uint32_t);
void SendSelectSkin(void* thiz, uint32_t skinId, uint32_t heroId) {
    if (!g_UnlockSkins) {
        oSendSelectSkin(thiz, skinId, heroId);
        return;
    }

    // Cek kepemilikan asli menggunakan fungsi asli (jika valid)
    // bool realOwnership = false;
    // if (oIsHaveSkin) realOwnership = oIsHaveSkin(nullptr, skinId); // WARNING: Passing nullptr might crash specific implementations, assuming static or handled check above.
    // Correction: IsHaveSkin usually needs instance. But if we can't check, assume not owned if spoofing active.
    // Let's rely on g_UnlockSkins logic. If we selected a skin, and g_UnlockSkins is on, we try to spoof if it's not a default skin.

    // Simplification: We assume if g_UnlockSkins is on, we want to spoof any skin selection that isn't default.
    // Default skin logic: (HeroID * 100) + 1.
    // Example: Layla 18 -> 1801.
    // User info: 18 -> 1181 (This is unusual, let's stick to safe '0' or let server assign default).

    g_SpoofedHeroID = heroId;
    g_SpoofedSkinID = skinId;

    // Kirim ID 0 (Default) ke server. Server akan melihat kita pakai skin default.
    oSendSelectSkin(thiz, 0, heroId);
}

// Hook SetPlayerData (Untuk In-Game & Loading Screen Visual)
void (*oSetPlayerData)(void*, uintptr_t, uint32_t);
void SetPlayerData(void* thiz, uintptr_t playerInfo, uint32_t camp) {
    // Safety Check Offline
    if (playerInfo == 0) {
        oSetPlayerData(thiz, playerInfo, camp);
        return;
    }

    if (g_UnlockSkins && g_SpoofedSkinID != 0) {
        // Ambil UID Player Sendiri
        uint64_t myUid = 0;
        Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "", "SystemData", "m_uiID", &myUid);

        // Cari offset di struct BattlePlayerInfo
        static size_t off_uiSelHero = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "uiHeroIDChoose"); // Corrected from dump: uiHeroIDChoose
        static size_t off_uiSkinId = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "uiHeroSkinIDChoose"); // Corrected from dump: uiHeroSkinIDChoose
        static size_t off_lUid = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "ulUid"); // Need to check dump for 'ulUid' or 'lUid' or 'uid' in BattlePlayerInfo.
        // Dump says: BattlePlayerInfo -> visit(SdpUnpacker...) -> fields like uiHeroIDChoose.
        // Wait, BattlePlayerInfo in dump snippet 17 has `uiHeroIDChoose` (0x1c8), `uiHeroSkinIDChoose` (0x1d0).
        // It does NOT show `uid` or `lUid` directly in the snippet provided. It has `iTeamId`.
        // Typically `BattlePlayerInfo` inherits or contains ID. Let's look at `AddPlayerInfoEx` args: `MTTDProto.BattlePlayerInfo playerinfo`.
        // Let's check `BattlePlayerInfo` structure again.

        // RE-CHECK DUMP for UID in BattlePlayerInfo
        // Snippet shows: `public System.UInt64 ulUid; // 0x10` (Example from similar protos)
        // In the provided dump snippet for `MTTDProto.BattlePlayerInfo`:
        // public System.UInt64 ulUid; // 0x10 (We need to verify this or find where UID is)

        // Let's assume standard offset or try to find it.
        // If we can't verify UID, we can try to match by HeroID if unique in team.
        // But safer is UID. Let's try finding `ulUid` dynamically.
        static size_t off_ulUid = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "ulUid");

        if (off_uiSelHero != -1 && off_uiSkinId != -1 && off_ulUid != -1) {
            uint64_t pUid = *(uint64_t*)(playerInfo + off_ulUid);
            uint32_t pHero = *(uint32_t*)(playerInfo + off_uiSelHero);

            // Cek apakah ini data kita
            if (pUid == myUid && pHero == g_SpoofedHeroID) {
                // INJECT SKIN ID PALSU KITA KE DATA LOKAL
                *(uint32_t*)(playerInfo + off_uiSkinId) = g_SpoofedSkinID;
            }
        }
    }

    oSetPlayerData(thiz, playerInfo, camp);
}

// --- Init Skin Hooks ---
void InitUnlockSkin() {
    // Il2CppGetMethodOffset(image, namespace, class, method, argsCount)
    void* addrIsHaveSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsHaveSkin", 2);
    if (addrIsHaveSkin) Tools::Hook(addrIsHaveSkin, (void*)IsHaveSkin, (void**)&oIsHaveSkin);

    void* addrIsCanUseSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsCanUseSkin", 1);
    if (addrIsCanUseSkin) Tools::Hook(addrIsCanUseSkin, (void*)IsCanUseSkin, (void**)&oIsCanUseSkin);

    void* addrGetHeroSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "GetHeroSkin", 2);
    if (addrGetHeroSkin) Tools::Hook(addrGetHeroSkin, (void*)GetHeroSkin, (void**)&oGetHeroSkin);

    // Fix Hero Not Owned
    void* addrIsHaveHero = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsHaveHero", 1); // Check args in dump
    if (addrIsHaveHero) Tools::Hook(addrIsHaveHero, (void*)IsHaveHero, (void**)&oIsHaveHero);

    // Spoofing Hooks
    void* addrSendSelectSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "UIChooseHero", "SendSelectSkin", 2);
    if (addrSendSelectSkin) Tools::Hook(addrSendSelectSkin, (void*)SendSelectSkin, (void**)&oSendSelectSkin);

    void* addrSetPlayerData = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "BattleReceiveMessage", "SetPlayerData", 2);
    if (addrSetPlayerData) Tools::Hook(addrSetPlayerData, (void*)SetPlayerData, (void**)&oSetPlayerData);
}
