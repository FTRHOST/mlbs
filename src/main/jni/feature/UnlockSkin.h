#pragma once
#include "../Include.h"
#include "../include/Utils/Unity/ByNameModding/Tools.h"
#include "../include/Utils/Unity/ByNameModding/Il2Cpp.h"

// ================= GLOBAL VARS =================
// Access globals from main.cpp
extern bool g_UnlockSkins;
extern bool g_SeeUnreleasedSkins;

// Internal spoof vars
uint32_t g_SpoofHeroID = 0;
uint32_t g_SpoofSkinID = 0;

// Cache Instance (PENTING: Mencegah Freeze/Lag saat scroll menu)
void* cachedFakeSkin = nullptr;

// ================= HELPERS =================

// Helper: Membuat objek skin palsu 1x saja (Performance Optimized)
void* GetFakeSkinInstance(uint32_t id) {
    if (cachedFakeSkin == nullptr) {
        void* klass = Il2CppGetClassType("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin");
        if (klass) {
            cachedFakeSkin = Il2CppCreateClassInstance("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin");
        }
    }

    // Set field iId (ID Skin) secara dinamis
    if (cachedFakeSkin) {
        static size_t off_iId = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin", "iId");
        static size_t off_iLimit = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "CmdHeroSkin", "iLimitTime");

        if (off_iId != -1) *(uint32_t*)((uintptr_t)cachedFakeSkin + off_iId) = id;
        if (off_iLimit != -1) *(uint32_t*)((uintptr_t)cachedFakeSkin + off_iLimit) = 0; // 0 = Permanen
    }
    return cachedFakeSkin;
}

// ================= CORE HOOKS (HERO & SKIN) =================

// 1. BYPASS KEPEMILIKAN HERO (FIX UTAMA ANDA)
bool (*oIsHaveHero)(void*, int);
bool IsHaveHero(void* instance, int heroId) {
    // Jika instance null (biasanya saat offline/loading awal), return false biar ga crash
    // For static methods, 'instance' captures the first argument (heroId).
    // If instance (heroId) is 0, it's invalid anyway.
    if (instance == nullptr) return false;

    if (g_UnlockSkins) return true; // Force True: Kita punya semua hero
    return oIsHaveHero(instance, heroId);
}

// 2. BYPASS HERO BISA DIGUNAKAN (Untuk Mode Custom/Draft)
bool (*oIsCanUseHero)(void*, int);
bool IsCanUseHero(void* instance, int heroId) {
    if (g_UnlockSkins) return true;
    return oIsCanUseHero(instance, heroId);
}

// 3. BYPASS KEPEMILIKAN SKIN
// Note: IsHaveSkin likely returns CmdHeroSkin* in newer versions.
// We treat return type as void* to be safe and return FakeSkin if needed.
void* (*oIsHaveSkin)(void*, int);
void* IsHaveSkin(void* instance, int skinId) {
    if (instance == nullptr) return nullptr; // Anti-Crash

    // Cek asli dulu (apakah kita benar-benar punya?)
    void* owned = oIsHaveSkin(instance, skinId);
    if (owned != nullptr) return owned;

    // Kalau tidak punya, tapi fitur aktif -> Ngaku punya
    if (g_UnlockSkins) {
        // Return fake instance to prevent crash if game accesses fields
        return GetFakeSkinInstance(skinId);
    }

    return nullptr;
}

// 4. GET HERO SKIN DATA (Mengisi list skin agar tidak kosong)
void* (*oGetHeroSkin)(void*, uintptr_t, uint32_t);
void* GetHeroSkin(void* instance, uintptr_t list, uint32_t skinId) {
    void* ret = oGetHeroSkin(instance, list, skinId);

    // Jika game bilang "tidak ada data skin ini" (ret == null), kita beri data palsu
    if (ret == nullptr && g_UnlockSkins) {
        return GetFakeSkinInstance(skinId);
    }
    return ret;
}

// 5. MEMBUKA SEMUA TOMBOL (USE/TRIAL)
bool (*oIsCanUseSkin)(void*, int);
bool IsCanUseSkin(void* instance, int heroId) {
    if (g_UnlockSkins) return true;
    return oIsCanUseSkin(instance, heroId);
}

// 6. UNLOCK UNRELEASED HEROES (NEW)
bool (*oIsForbidHeroInChooseHero)(void*, int);
bool IsForbidHeroInChooseHero(void* instance, int dummy) {
    if (g_SeeUnreleasedSkins || g_UnlockSkins) return false; // Don't forbid
    return oIsForbidHeroInChooseHero(instance, dummy);
}

bool (*oIsForbidHeros)(void*, int);
bool IsForbidHeros(void* instance, int dummy) {
    if (g_SeeUnreleasedSkins || g_UnlockSkins) return false; // Don't forbid
    return oIsForbidHeros(instance, dummy);
}

// ================= SPOOFING LOGIC (AMAN UNTUK RANK) =================

// Logic: Saat kita klik "Pilih" di Lobby
void (*oSendSelectSkin)(void*, uint32_t, uint32_t);
void SendSelectSkin(void* thiz, uint32_t skinId, uint32_t heroId) {
    if (!g_UnlockSkins) {
        return oSendSelectSkin(thiz, skinId, heroId);
    }

    // Simpan pilihan kita di variabel global
    g_SpoofHeroID = heroId;
    g_SpoofSkinID = skinId;

    // --- LOGIC SPOOFING ---
    // Kirim ID 0 (Default Skin) ke Server.
    // Server akan melihat kita pakai skin biasa -> Aman dari Ban Invalid Data.
    oSendSelectSkin(thiz, 0, heroId);

    // --- FORCE UPDATE TAMPILAN LOKAL ---
    // Agar di layar HP kita skinnya berubah menjadi skin mahal pilihan kita.
    // Kita panggil method UI internal: UIChooseHero.BatttleSelectSkin(uid, skinId)

    // Cari UID kita (SystemData.m_uiID)
    uint64_t myUid = 0;
    Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "SystemData", "m_uiID", &myUid);

    if (myUid > 0) {
        // Cari Method BatttleSelectSkin (Perhatikan ejaan 'tt' ganda sering terjadi di dump MLBB)
        // Coba cari "BatttleSelectSkin" atau "BattleSelectSkin"
        void* method = Il2CppGetMethodOffset("Assembly-CSharp.dll", "UIChooseHero", "BatttleSelectSkin", 2);
        if (!method) method = Il2CppGetMethodOffset("Assembly-CSharp.dll", "UIChooseHero", "BattleSelectSkin", 2); // Fallback spelling

        if (method) {
            // Kita butuh 'instance' UIChooseHero.
            // Karena kita ada di dalam fungsi SendSelectSkin yg dipanggil oleh UIChooseHero,
            // variabel 'thiz' ADALAH instance UIChooseHero!

            // Cast and Call
            // Args: (this, uid, skinId)
            ((void(*)(void*, uint64_t, uint32_t))method)(thiz, myUid, skinId);
        }
    }
}

// Logic: Saat Loading Screen / Masuk Land of Dawn
void (*oSetPlayerData)(void*, uintptr_t, uint32_t);
void SetPlayerData(void* thiz, uintptr_t playerInfo, uint32_t camp) {
    // Safety check offline
    if (playerInfo == 0) { oSetPlayerData(thiz, playerInfo, camp); return; }

    // Jika fitur aktif dan kita sudah memilih skin spoof
    if (g_UnlockSkins && g_SpoofSkinID != 0) {
        // Ambil data dari struct BattlePlayerInfo
        // Perlu offset yang akurat. Jika struktur berubah, ini bisa crash.
        // Kita gunakan nama field dinamis via Il2Cpp helper

        static size_t off_lUid = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "ulUid"); // Updated based on earlier thought
        static size_t off_uiSelHero = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "uiHeroIDChoose");
        static size_t off_uiSkinId = Il2CppGetFieldOffset("Assembly-CSharp.dll", "MTTDProto", "BattlePlayerInfo", "uiHeroSkinIDChoose");

        if (off_lUid != -1 && off_uiSelHero != -1 && off_uiSkinId != -1) {
            uint64_t pUid = *(uint64_t*)(playerInfo + off_lUid);

            // Ambil UID kita sendiri
            uint64_t myUid = 0;
            Il2CppGetStaticFieldValue("Assembly-CSharp.dll", "SystemData", "m_uiID", &myUid);

            // Jika data ini milik kita (My UID matches)
            if (pUid == myUid) {
                uint32_t pHero = *(uint32_t*)(playerInfo + off_uiSelHero);
                // Dan heronya sesuai dengan yang kita pilih
                if (pHero == g_SpoofHeroID) {
                    // SUNTIKKAN SKIN ID PALSU KITA KE MEMORI LOKAL
                    *(uint32_t*)(playerInfo + off_uiSkinId) = g_SpoofSkinID;
                }
            }
        }
    }

    // Lanjutkan fungsi asli
    oSetPlayerData(thiz, playerInfo, camp);
}

// ================= INITIALIZATION =================

void InitUnlockSkin() {
    // 1. Hook SystemData (Hero & Skin Ownership)
    void* addrIsHaveHero = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SystemData", "IsHaveHero", 1);
    if (addrIsHaveHero) Tools::Hook(addrIsHaveHero, (void*)IsHaveHero, (void**)&oIsHaveHero);

    void* addrIsCanUseHero = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SystemData", "IsCanUseHero", 1);
    if (addrIsCanUseHero) Tools::Hook(addrIsCanUseHero, (void*)IsCanUseHero, (void**)&oIsCanUseHero);

    void* addrIsHaveSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SystemData", "IsHaveSkin", 1);
    if (addrIsHaveSkin) Tools::Hook(addrIsHaveSkin, (void*)IsHaveSkin, (void**)&oIsHaveSkin);

    void* addrGetHeroSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SystemData", "GetHeroSkin", 2);
    if (addrGetHeroSkin) Tools::Hook(addrGetHeroSkin, (void*)GetHeroSkin, (void**)&oGetHeroSkin);

    void* addrIsCanUseSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SystemData", "IsCanUseSkin", 1);
    if (addrIsCanUseSkin) Tools::Hook(addrIsCanUseSkin, (void*)IsCanUseSkin, (void**)&oIsCanUseSkin);

    // 2. Hook UI & Networking untuk Spoofing
    void* addrSendSelectSkin = Il2CppGetMethodOffset("Assembly-CSharp.dll", "UIChooseHero", "SendSelectSkin", 2);
    if (addrSendSelectSkin) Tools::Hook(addrSendSelectSkin, (void*)SendSelectSkin, (void**)&oSendSelectSkin);

    // 3. Hook In-Game Data (Visual Spoof)
    void* addrSetPlayerData = Il2CppGetMethodOffset("Assembly-CSharp.dll", "BattleReceiveMessage", "SetPlayerData", 2);
    if (addrSetPlayerData) Tools::Hook(addrSetPlayerData, (void*)SetPlayerData, (void**)&oSetPlayerData);

    // 4. Hook Unreleased Heroes
    void* addrIsForbidHeroChoose = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SystemData", "IsForbidHeroInChooseHero", 1);
    if (addrIsForbidHeroChoose) Tools::Hook(addrIsForbidHeroChoose, (void*)IsForbidHeroInChooseHero, (void**)&oIsForbidHeroInChooseHero);

    void* addrIsForbidHeros = Il2CppGetMethodOffset("Assembly-CSharp.dll", "SystemData", "IsForbidHeros", 1);
    if (addrIsForbidHeros) Tools::Hook(addrIsForbidHeros, (void*)IsForbidHeros, (void**)&oIsForbidHeros);
}
