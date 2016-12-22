LOCAL_PATH := $(call my-dir)
MY_DIR := $(LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -fdata-sections -ffunction-sections -fvisibility=hidden -DPLATFORM_ANDROID
LOCAL_CPPFLAGS := -fpermissive -fdata-sections -ffunction-sections -fvisibility=hidden  -std=c++11 -DPLATFORM_ANDROID

LOCAL_MODULE := media_control

MY_SRC_FILES := \
	media_jni.cpp

LOCAL_SRC_FILES += $(MY_SRC_FILES)

LOCAL_C_INCLUDES += $(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.9/include
LOCAL_C_INCLUDES += $(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/include
LOCAL_C_INCLUDES += $(MY_DIR)/include
LOCAL_C_INCLUDES += include logger

LOCAL_LDLIBS :=   -llog -Wl,-rpath=. -Llogger -lprof
LOCAL_LDLIBS += -L$(MY_DIR)/libs -lgateway -lprotobuf -lPocoNet -lPocoFoundation
LOCAL_LDLIBS += $(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi/libgnustl_static.a

include $(BUILD_SHARED_LIBRARY)
