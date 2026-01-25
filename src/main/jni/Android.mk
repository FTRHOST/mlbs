LOCAL_PATH := $(call my-dir)

# --- Library Dobby Prebuilt ---
include $(CLEAR_VARS)
LOCAL_MODULE := dobby
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# --- Main Module ---
include $(CLEAR_VARS)
LOCAL_MODULE := MyCleanMod

# Flag Build
LOCAL_CFLAGS += -Wall -Wno-unused-variable -Wno-unused-function -fvisibility=hidden
LOCAL_CPPFLAGS += -fexceptions -frtti -std=c++17

# Sertakan File Source
LOCAL_SRC_FILES := main.cpp \
                   include/Utils/Unity/ByNameModding/Il2Cpp.cpp \
                   include/Utils/Unity/ByNameModding/Tools.cpp \
                   include/Utils/Unity/ByNameModding/fake_dlfcn.cpp

# Include Directories
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Hook/Dobby

# Libraries
LOCAL_LDLIBS := -llog -landroid
LOCAL_STATIC_LIBRARIES := dobby

include $(BUILD_SHARED_LIBRARY)
