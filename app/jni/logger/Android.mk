#build as necessary
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -fdata-sections -ffunction-sections -fvisibility=hidden -DPLATFORM_ANDROID
LOCAL_CPPFLAGS := -fpermissive -fdata-sections -ffunction-sections -fvisibility=hidden  -std=c++11 -DPLATFORM_ANDROID

LOCAL_MODULE := prof

MY_SRC_FILES := \
	live_log.cpp

LOCAL_SRC_FILES += $(MY_SRC_FILES)

LOCAL_C_INCLUDES += $(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.8/include
LOCAL_C_INCLUDES += $(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/include

LOCAL_LDLIBS :=   -llog 
LOCAL_LDLIBS += $(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/libgnustl_static.a

include $(BUILD_SHARED_LIBRARY)
