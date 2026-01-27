#pragma once
#include "../include/Utils/Unity/ByNameModding/Tools.h"
#include "../include/Utils/Unity/ByNameModding/Il2Cpp.h"

// Variabel Global (di-extern di main.cpp)
extern bool g_EnableDebugMenu;

// Offset Cache agar tidak mencari terus menerus (Performance)
size_t off_isThread = -1;
size_t off_debugMoreInfo = -1;
size_t off_unityTime = -1;

// Hook Update TestGM
void (*oTestGM_Update)(void*);
void TestGM_Update(void* instance) {

    if (g_EnableDebugMenu && instance != nullptr) {
        // Cari offset hanya sekali saat pertama kali jalan
        if (off_isThread == -1) {
            off_isThread = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "TestGM", "isThread");
            off_debugMoreInfo = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "TestGM", "debugMoreInfo");
            off_unityTime = Il2CppGetFieldOffset("Assembly-CSharp.dll", "", "TestGM", "unityTime");
        }

        // --- INI LOGIKA PENGAKTIFAN GM ---
        // Enum TestGMSwitch: Off=1, On=2, SecondGear=3, Disable=4
        // Kita paksa jadi 2 (On)
        if (off_isThread != -1) {
            *(int*)((uintptr_t)instance + off_isThread) = 2;
        }

        // Aktifkan Info Debug Tambahan
        if (off_debugMoreInfo != -1) {
            *(bool*)((uintptr_t)instance + off_debugMoreInfo) = true;
        }

        // (Opsional) Mengaktifkan kontrol waktu unity
        if (off_unityTime != -1) {
            *(bool*)((uintptr_t)instance + off_unityTime) = true;
        }
    }

    // Panggil fungsi asli agar game tidak crash/berhenti
    if (oTestGM_Update) oTestGM_Update(instance);
}

void InitGameMaster() {
    // Hook TestGM.Update untuk menyuntikkan nilai GM Mode
    void* addrTestGMUpdate = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "TestGM", "Update", 0);
    if (addrTestGMUpdate) {
        Tools::Hook(addrTestGMUpdate, (void*)TestGM_Update, (void**)&oTestGM_Update);
    }
}
