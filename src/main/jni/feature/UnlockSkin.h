#pragma once
#include "../include/Utils/Unity/ByNameModding/Tools.h"
#include "../include/Utils/Unity/ByNameModding/Il2Cpp.h"

// Variabel Global untuk Menu (bisa di-extern di main.cpp jika perlu)
extern bool g_UnlockSkins;

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

// Hook: IsHaveSkin (Pura-pura punya skin)
bool (*oIsHaveSkin)(void*, int);
bool IsHaveSkin(void* unk, int skinId) {
    if (g_UnlockSkins) return true; // Force True
    return oIsHaveSkin(unk, skinId);
}

// Hook: IsCanUseSkin (Agar tombol Use menyala)
bool (*oIsCanUseSkin)(void*, int);
bool IsCanUseSkin(void* unk, int heroId) {
    if (g_UnlockSkins) return true;
    return oIsCanUseSkin(unk, heroId);
}

// Hook: GetHeroSkin (Mengembalikan object skin palsu)
void* (*oGetHeroSkin)(void*, uintptr_t, uint32_t);
void* GetHeroSkin(void* instance, uintptr_t heroSkinsList, uint32_t skinId) {
    void* ret = oGetHeroSkin(instance, heroSkinsList, skinId);
    if (ret != nullptr) return ret; // Kalau punya asli, return asli

    if (g_UnlockSkins) {
        return GetFakeSkinInstance(skinId);
    }
    return nullptr;
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
}
