#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <EGL/egl.h>
#include <android/log.h> // Include log header

#include "include/Hook/Dobby/dobby.h"
#include "include/ImGui/imgui.h"
#include "include/ImGui/backends/imgui_impl_opengl3.h"
#include "include/ImGui/backends/imgui_impl_android.h"
#include "include/xdl/xdl.h"

#define LOG_TAG "CleanMod"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// --- Global Variables ---
bool g_ShowMenu = true;
EGLBoolean (*orig_eglSwapBuffers)(EGLDisplay, EGLSurface);
void (*orig_Input)(void*, void*, void*);

// --- UI Logic ---
void DrawMenu() {
    if (!g_ShowMenu) return;

    // UI Dasar Sederhana
    ImGui::Begin("Clean Mod Menu", &g_ShowMenu);
    ImGui::Text("Status: Safe & Clean");
    ImGui::Separator();
    ImGui::Text("Mod Aktif!");
    ImGui::Text("Versi Arsitektur: %s",
    #if defined(__aarch64__)
        "ARM64-v8a"
    #elif defined(__arm__)
        "ARMv7a"
    #else
        "Unknown"
    #endif
    );
    
    if (ImGui::Button("Test Button")) {
        LOGI("Tombol Test Ditekan");
    }

    ImGui::End();
}

// --- Setup ImGui ---
void SetupImGui() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Setup Style
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(3.0f); // Skala untuk HP

    // Init Backend
    ImGui_ImplOpenGL3_Init("#version 100");
}

// --- Hooks ---
// Hook Input: Agar menu bisa disentuh
void MyInput(void *thiz, void *ex_ab, void *ex_ac) {
    orig_Input(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent*)thiz);
}

// Hook SwapBuffers: Untuk menggambar menu
EGLBoolean MyEglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    static bool isSetup = false;
    if (!isSetup) {
        SetupImGui();
        isSetup = true;
        LOGI("ImGui Initialized");
    }

    ImGuiIO& io = ImGui::GetIO();

    // Update Display Size
    EGLint width, height;
    eglQuerySurface(dpy, surface, EGL_WIDTH, &width);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &height);
    io.DisplaySize = ImVec2((float)width, (float)height);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    DrawMenu(); // Gambar menu kita

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return orig_eglSwapBuffers(dpy, surface);
}

// --- Entry Point ---
void *MainThread(void *) {
    LOGI("MainThread Started. Waiting for library...");
    // Tunggu sampai library sistem siap (opsional, tergantung game)
    sleep(5);

    // 1. Hook Graphics (EGL)
    void *eglSym = xdl_sym(xdl_open("libEGL.so", XDL_DEFAULT), "eglSwapBuffers", nullptr);
    if (eglSym) {
        DobbyHook(eglSym, (void*)MyEglSwapBuffers, (void**)&orig_eglSwapBuffers);
        LOGI("Hooked eglSwapBuffers");
    } else {
        LOGE("Failed to find eglSwapBuffers");
    }

    // 2. Hook Input (libinput.so)
    // Catatan: Symbol input mungkin berbeda tiap versi Android, ini contoh umum
    void *inputSym = xdl_sym(xdl_open("libinput.so", XDL_DEFAULT), "_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE", nullptr);
    if (inputSym) {
        DobbyHook(inputSym, (void*)MyInput, (void**)&orig_Input);
        LOGI("Hooked Input");
    } else {
         LOGE("Failed to find Input symbol");
    }

    LOGI("CleanMod Loaded Successfully");

    return nullptr;
}

__attribute__((constructor))
void Initializer() {
    pthread_t pt;
    pthread_create(&pt, NULL, MainThread, NULL);
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("JNI_OnLoad called - Library loaded via System.loadLibrary");
    return JNI_VERSION_1_6;
}
