MY_LOCAL_PATH := $(call my-dir)

TMP_TARGET := default # or liblog, should maually copy to logger dir

ifeq ($(strip $(TMP_TARGET)), default)

include logger/prebuild.mk
include media_control/Android.mk

else

include logger/Android.mk

endif

