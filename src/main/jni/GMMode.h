#pragma once

#include "GlobalState.h"
#include "dobby.h"
#include "include/Utils/Unity/ByNameModding/Il2Cpp.h"
#include <android/log.h>

// --- GM Mode Hook ---

// Original function pointer for TestGM::Update()
void (*orig_TestGM_Update)(void* instance);

// Replacement function for TestGM::Update()
void my_TestGM_Update(void* instance) {
    // Call the original function first to avoid breaking game logic
    if (orig_TestGM_Update) {
        orig_TestGM_Update(instance);
    }

    // Only apply GM settings if the feature is enabled AND the game is in a valid state (not initial loading)
    if (instance && g_State.gmModeEnabled && g_State.battleState != 0) {
        // isThread (Offset 0x18) = 2 (On)
        *(int*)((uintptr_t)instance + 0x18) = 2;

        // debugLog (Offset 0x50) = true
        *(bool*)((uintptr_t)instance + 0x50) = true;
    }
}

// --- Crash Report Blocker ---

// Original function pointer for Analytics::ReportCrashData(...)
// Signature: (bool, String, String, int, int, String, String, String)
void (*orig_Analytics_ReportCrashData)(void*, bool, void*, void*, int, int, void*, void*, void*);

// Replacement function to disable crash reporting
void my_Analytics_ReportCrashData(void* __this, bool isLargeData, void* tag, void* data, int lastMemoryWarningCount, int lastMemoryWarningMaxValue, void* lastLoadingRes, void* lastExitGameType, void* lastRestartGameType) {
    __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "Crash report blocked by GM Mode feature.");
    return;
}


// Function to apply all hooks related to GM Mode
void ApplyGMModeHooks() {
    // Hook for GM Mode activation
    void* testGM_UpdateAddr = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "TestGM", "Update", 0);
    if (testGM_UpdateAddr) {
        DobbyHook(testGM_UpdateAddr, (void*)my_TestGM_Update, (void**)&orig_TestGM_Update);
        __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "Hooked TestGM::Update for GM Mode.");
    } else {
        __android_log_print(ANDROID_LOG_WARN, "MLBS_HOOK", "Failed to find TestGM::Update address.");
    }

    // Hook to block crash reports
    // Note: The original method is static, so the first argument 'this' will be null, but we must keep it in the signature for Dobby.
    void* reportCrashAddr = Il2CppGetMethodOffset("Assembly-CSharp.dll", "", "Analytics", "ReportCrashData", 8);
    if (reportCrashAddr) {
        DobbyHook(reportCrashAddr, (void*)my_Analytics_ReportCrashData, (void**)&orig_Analytics_ReportCrashData);
        __android_log_print(ANDROID_LOG_INFO, "MLBS_HOOK", "Hooked Analytics::ReportCrashData to prevent crash reporting.");
    } else {
        __android_log_print(ANDROID_LOG_WARN, "MLBS_HOOK", "Failed to find Analytics::ReportCrashData address.");
    }
}
