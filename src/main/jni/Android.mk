LOCAL_PATH := $(call my-dir)

# --- Library Dobby Prebuilt ---
include $(CLEAR_VARS)
LOCAL_MODULE := dobby
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# --- Main Module ---
include $(CLEAR_VARS)
LOCAL_MODULE := MyCleanMod
# ^ Ganti nama lib sesuai keinginan, nanti outputnya libMyCleanMod.so

# Flag Build
LOCAL_CFLAGS += -Wall -Wno-unused-variable -Wno-unused-function -fvisibility=hidden
LOCAL_CPPFLAGS += -fexceptions -frtti -std=c++17

# Sertakan File Source (Hanya yang penting!)
# Hapus WebServer.cpp, ConfigManager.cpp, Unlock.cpp, dll.
LOCAL_SRC_FILES := main.cpp \
                   src/ImGui/imgui.cpp \
                   src/ImGui/imgui_demo.cpp \
                   src/ImGui/imgui_draw.cpp \
                   src/ImGui/imgui_tables.cpp \
                   src/ImGui/imgui_widgets.cpp \
                   src/ImGui/backends/imgui_impl_android.cpp \
                   src/ImGui/backends/imgui_impl_opengl3.cpp \
                   src/xdl/xdl.c \
                   src/xdl/xdl_iterate.c \
                   src/xdl/xdl_linker.c \
                   src/xdl/xdl_lzma.c \
                   src/xdl/xdl_util.c

# Include Directories
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui/backends
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/xdl
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Hook/Dobby

# Libraries
LOCAL_LDLIBS := -llog -lEGL -lGLESv2 -landroid
LOCAL_STATIC_LIBRARIES := dobby

include $(BUILD_SHARED_LIBRARY)
