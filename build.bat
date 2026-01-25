@echo off
setlocal

echo Starting Build Process...

where ndk-build >nul 2>nul
if %errorlevel% equ 0 (
    set NDK_BUILD=ndk-build
) else (
    if defined ANDROID_NDK_HOME (
        if exist "%ANDROID_NDK_HOME%\ndk-build.cmd" (
            set NDK_BUILD="%ANDROID_NDK_HOME%\ndk-build.cmd"
        ) else (
             echo Error: ndk-build not found in ANDROID_NDK_HOME.
             goto :error
        )
    ) else (
        echo Error: ndk-build not found in PATH and ANDROID_NDK_HOME is not set.
        goto :error
    )
)

echo Using NDK: %NDK_BUILD%

cd src\main\jni

call %NDK_BUILD% NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=Android.mk NDK_APPLICATION_MK=Application.mk

if %errorlevel% equ 0 (
    echo Build finished successfully. Output files are in src\main\jni\libs\
) else (
    echo Build failed.
    goto :error
)

goto :eof

:error
echo Build Failed.
exit /b 1
