/*
 * util_JNI.h
 *
 *  Created on: Dec 4, 2014
 *      Author: lqp
 */

#ifndef UTIL_JNI_H_
#define UTIL_JNI_H_
#include"jni.h"
#include <android/log.h>

//common def
#define ADD_TO_TEXT __attribute__((section (".text")))
#define JNI_SUCCESS 0
#define JNI_FAIL -1

//branch for debug or cancelAll
extern int ANTI_DEBUG;

#define JNI_DEBUG 0

#if JNI_DEBUG

#define WENBA_JNI_LOG(tag, fmtMsg, args...) \
	__android_log_print(ANDROID_LOG_INFO, tag, fmtMsg, ##args)

#define  ENCRYPT_TAG "lqp"

#else

#define WENBA_JNI_LOG(tag, fmtMsg, args...)
#define  ENCRYPT_TAG

#endif

//Util structs
#define COVER_ENTRY(entry) {entry, sizeof(entry)}

typedef struct {
	char* data;
	int len;
}SecString;

#endif /* UTIL_JNI_H_ */
