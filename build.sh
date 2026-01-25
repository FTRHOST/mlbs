#!/bin/bash

# Function to check if a command exists
command_exists() {
  command -v "$1" >/dev/null 2>&1
}

echo "Starting Build Process..."

# Check for ndk-build
if command_exists ndk-build; then
    NDK_BUILD="ndk-build"
elif [ -n "$ANDROID_NDK_HOME" ] && [ -f "$ANDROID_NDK_HOME/ndk-build" ]; then
    NDK_BUILD="$ANDROID_NDK_HOME/ndk-build"
else
    echo "Error: ndk-build not found. Please add it to your PATH or set ANDROID_NDK_HOME."
    exit 1
fi

echo "Using NDK: $NDK_BUILD"

# Navigate to jni directory
cd src/main/jni || exit

# Run build
"$NDK_BUILD" NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk NDK_APPLICATION_MK=Application.mk

if [ $? -eq 0 ]; then
    echo "Build finished successfully. Output files are in src/main/jni/libs/"
else
    echo "Build failed."
    exit 1
fi
