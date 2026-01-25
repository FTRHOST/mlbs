# Clean Mod Menu Template (C++)

This is a clean, safe, and modular template for creating Android game mods using C++, ImGui, and Dobby Hook. It is designed to be a solid foundation, free of active cheat features (like ESP or Unlockers) that are easily detected.

## Features

*   **Clean Architecture:** Separated Core, UI, SDK, and Utils.
*   **ImGui Integrated:** Ready-to-use ImGui interface with touch support.
*   **Safe Hooking:** Uses Dobby for safe function hooking (EGL, Input).
*   **Passive Logic:** Focuses on reading data rather than writing (safer).
*   **Multi-Arch Support:** Builds for `arm64-v8a` and `armeabi-v7a`.
*   **Automated Builds:** GitHub Actions workflow included.

## How to Build

### Option 1: Automatic Build (GitHub Actions)

1.  **Fork** or **Push** this repository to GitHub.
2.  Go to the **Actions** tab in your repository.
3.  Select the **Build Android Mod Library** workflow.
4.  If it doesn't run automatically on push, click **Run workflow**.
5.  Once finished, download the artifacts (`libMyCleanMod-so`) from the workflow summary.

### Option 2: Manual Build

**Prerequisites:**
*   Android NDK (r25c recommended) installed.
*   `ndk-build` added to your PATH or `ANDROID_NDK_HOME` environment variable set.

**Windows:**
Double-click `build.bat` or run it from Command Prompt.

**Linux / macOS / Termux:**
Run the build script:
```bash
chmod +x build.sh
./build.sh
```

### Output
The compiled `.so` files will be located in:
`src/main/jni/libs/`

## Installation

1.  Extract the built `.so` file (e.g., `libMyCleanMod.so`).
2.  Inject it into the target game APK (put in `lib/arm64-v8a/` or `lib/armeabi-v7a/`).
3.  Load the library using `System.loadLibrary("MyCleanMod");` in the game's Java code (e.g., in `MainActivity` or `SplashActivity`).

## Structure

*   `src/main/jni/main.cpp`: Entry point, Thread management, and Hooks.
*   `src/main/jni/Config.h`: Configuration.
*   `src/main/jni/Core/`: Core logic (GameReader, Hooks).
*   `src/main/jni/UI/`: ImGui menu drawing logic.
*   `src/main/jni/SDK/`: Game structs and offsets.
*   `src/main/jni/Utils/`: Utility functions (Logger, Obfuscation).
