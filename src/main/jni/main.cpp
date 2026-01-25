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

#define LOG_TAG "StealthMod"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using json = nlohmann::json;

// --- Global Config Variables ---
// Default values
bool g_UnlockSkins = false;
bool g_DebugMode = false;

// Path Konfigurasi
const std::string CONFIG_PATH = "/storage/emulated/0/Android/data/com.mobile.legends/files/config.json";

// --- Config Reader ---
void ReadConfig() {
    std::ifstream configFile(CONFIG_PATH);
    if (configFile.is_open()) {
        try {
            json j;
            configFile >> j;

            // Parsing JSON dengan aman
            if (j.contains("UnlockCustomSkin")) g_UnlockSkins = j["UnlockCustomSkin"].get<bool>();
            if (j.contains("DebugMode")) g_DebugMode = j["DebugMode"].get<bool>();

            // LOGI jika debug aktif
            if (g_DebugMode) {
                LOGI("Config Loaded: UnlockSkin=%d", g_UnlockSkins);
            }
        } catch (json::parse_error& e) {
            LOGE("JSON Parse Error: %s", e.what());
        }
        configFile.close();
    } else {
        // Jika file tidak ada, tidak melakukan apa-apa (Stealth)
        if (g_DebugMode) {
            LOGE("Config file not found at: %s", CONFIG_PATH.c_str());
        }
    }
}

// --- Features Logic ---
void ApplyFeatures() {
    // Pastikan il2cpp sudah ter-attach
    if (!Il2CppIsAssembliesLoaded()) return;

    // --- FITUR 1: Unlock Skin (Client Side - Custom Mode) ---
    // Target Class: Guide_Battle
    // Field: m_RobotGuideCanSelectSkin

    // Variabel statis untuk menghindari pencarian string berulang
    static bool hasInitOffsets = false;
    static uintptr_t targetFieldOffset = 0;

    if (!hasInitOffsets) {
        // Get Static Field Offset
        targetFieldOffset = Il2CppGetStaticFieldOffset("Assembly-CSharp.dll", "", "Guide_Battle", "m_RobotGuideCanSelectSkin");
        if (targetFieldOffset != 0 && targetFieldOffset != (uintptr_t)-1) {
             hasInitOffsets = true;
             if (g_DebugMode) LOGI("Found Offset: Guide_Battle.m_RobotGuideCanSelectSkin = %lx", targetFieldOffset);
        } else {
             // Reset if failed, to try again next time (maybe il2cpp not fully ready despite check)
             targetFieldOffset = 0;
        }
    }

    if (hasInitOffsets && g_UnlockSkins) {
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
