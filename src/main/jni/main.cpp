#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <android/log.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

// Include JSON Library
#include "include/nlohmann/json.hpp"

// Include Modding Utils
#include "include/Utils/Unity/ByNameModding/Il2Cpp.h"
#include "include/Utils/Unity/ByNameModding/Tools.h"

// Include Feature Modules
#include "feature/UnlockSkin.h"
#include "feature/GameMaster.h"

// Undefine macros from Includes.h to avoid warnings
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#ifdef LOGI
#undef LOGI
#endif
#ifdef LOGE
#undef LOGE
#endif

#define LOG_TAG "StealthMod"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using json = nlohmann::json;

// --- Global Config Variables ---
// g_UnlockSkin defined in UnlockSkin.h
bool g_DebugMode = false;
bool g_SeeUnreleasedSkins = false;
bool g_SpoofZoneId = false;
int g_CustomZoneId = 50001;
bool g_EnableDebugMenu = false;

// Path Konfigurasi
const std::string CONFIG_PATH = "/storage/emulated/0/Android/data/com.mobile.legends/files/config.json";

// --- Hooks for Methods ---
bool (*orig_IsForbidSkin)(uint32_t, bool);
bool My_IsForbidSkin(uint32_t skinId, bool filterLuaCheck) {
    // Return false to allow seeing/using unreleased skins
    return false;
}

// --- Config Reader ---
void ReadConfig() {
    std::ifstream configFile(CONFIG_PATH);
    if (configFile.is_open()) {
        try {
            json j;
            configFile >> j;

            // Parsing JSON dengan aman
            if (j.contains("UnlockCustomSkin")) g_UnlockSkin = j["UnlockCustomSkin"].get<bool>();
            if (j.contains("DebugMode")) g_DebugMode = j["DebugMode"].get<bool>();
            if (j.contains("SeeUnreleasedSkins")) g_SeeUnreleasedSkins = j["SeeUnreleasedSkins"].get<bool>();
            if (j.contains("SpoofZoneId")) g_SpoofZoneId = j["SpoofZoneId"].get<bool>();
            if (j.contains("CustomZoneId")) g_CustomZoneId = j["CustomZoneId"].get<int>();
            if (j.contains("EnableDebugMenu")) g_EnableDebugMenu = j["EnableDebugMenu"].get<bool>();

            // LOGI jika debug aktif
            if (g_DebugMode) {
                LOGI("Config Loaded: UnlockSkin=%d, SeeUnreleasedSkins=%d, SpoofZoneId=%d, CustomZoneId=%d, EnableDebugMenu=%d",
                     g_UnlockSkin, g_SeeUnreleasedSkins, g_SpoofZoneId, g_CustomZoneId, g_EnableDebugMenu);
            }
        } catch (json::parse_error& e) {
            LOGE("JSON Parse Error: %s", e.what());
        }
        configFile.close();
    } else {
        // Jika file tidak ada, buat file default (Agar user tidak bingung)
        LOGE("Config file not found. Creating default at: %s", CONFIG_PATH.c_str());

        std::ofstream outFile(CONFIG_PATH);
        if (outFile.is_open()) {
            json j;
            j["UnlockCustomSkin"] = true;
            j["SeeUnreleasedSkins"] = true;
            j["DebugMode"] = true; // Enable debug by default for new file
            j["SpoofZoneId"] = false;
            j["CustomZoneId"] = 50001;
            j["EnableDebugMenu"] = true;

            outFile << j.dump(4);
            outFile.close();

            // Apply immediately
            g_UnlockSkin = true;
            g_SeeUnreleasedSkins = true;
            g_DebugMode = true;
            g_SpoofZoneId = false;
            g_CustomZoneId = 50001;
            g_EnableDebugMenu = true;

            LOGI("Default config created successfully.");
        } else {
            LOGE("Failed to create config file. Check permissions.");
        }
    }
}

// --- Features Logic ---
void ApplyFeatures() {
    // Pastikan il2cpp sudah ter-attach
    if (!Il2CppIsAssembliesLoaded()) return;

    // --- FITUR 1: Unlock Skin (Client Side - Custom Mode) ---
    // Sekarang ditangani oleh UnlockSkin.h via InitUnlockSkin()

    // Variabel statis untuk menghindari pencarian string berulang
    static bool hasInitOffsets = false;
    static uintptr_t targetFieldOffset = 0;
    static void* isForbidSkinAddr = nullptr;
    static bool isHooked = false;

    if (!hasInitOffsets) {
        // 1. Get Static Field Offset for Guide_Battle (Masih dipakai untuk Guide)
        targetFieldOffset = Il2CppGetStaticFieldOffset("Assembly-CSharp.dll", "", "Guide_Battle", "m_RobotGuideCanSelectSkin");

        // 2. Get Method Addresses
        isForbidSkinAddr = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "SystemData", "IsForbidSkin", 2);

        if (targetFieldOffset != 0 && targetFieldOffset != (uintptr_t)-1) {
             hasInitOffsets = true;
             if (g_DebugMode) {
                 LOGI("Found Offset: Guide_Battle.m_RobotGuideCanSelectSkin = %lx", (unsigned long)targetFieldOffset);
                 if (isForbidSkinAddr) LOGI("Found Method: SystemData.IsForbidSkin = %p", isForbidSkinAddr);
             }
        } else {
             // Reset if failed, to try again next time (maybe il2cpp not fully ready despite check)
             targetFieldOffset = 0;
        }
    }

    // Apply Field Patch
    if (hasInitOffsets && g_UnlockSkin) {
        bool currentValue = false;
        // Use Tools::ReadAddr as per "Safe Logic" suggestion
        if (Tools::ReadAddr((void*)targetFieldOffset, &currentValue, sizeof(bool))) {
            if (!currentValue) { // Hanya tulis jika nilai masih false
                bool trueValue = true;
                if (Tools::WriteAddr((void*)targetFieldOffset, &trueValue, sizeof(bool))) {
                     if (g_DebugMode) LOGI("Activated Skin Select Logic (Set to true)");
                }
            }
        }
    }

    // Apply Method Hooks
    if (!isHooked) {
        if (g_SeeUnreleasedSkins && isForbidSkinAddr) {
            Tools::Hook(isForbidSkinAddr, (void*)My_IsForbidSkin, (void**)&orig_IsForbidSkin);
            if (g_DebugMode) LOGI("Hooked SystemData.IsForbidSkin");
        }

        // Init Feature Modules (Hanya sekali)
        InitUnlockSkin();
        InitGameMaster();

        isHooked = true;
    }
}

// --- Main Loop (Ghost Thread) ---
void *StealthThread(void *) {
    LOGI("Stealth Service Started...");

    // Tunggu game load library il2cpp
    sleep(5);

    // Attach Il2Cpp Helper
    Il2CppAttach();

    while (true) {
        // 1. Baca Config
        ReadConfig();

        // 2. Terapkan Fitur
        ApplyFeatures();

        // 3. Tidur 3 detik (Hemat CPU & Mengurangi deteksi polling agresif)
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    return nullptr;
}

// --- Entry Point ---
__attribute__((constructor))
void Initializer() {
    pthread_t pt;
    pthread_create(&pt, NULL, StealthThread, NULL);
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    return JNI_VERSION_1_6;
}
