/*
 * live_log.cpp
 *
 *  Created on: Jul 29, 2015
 *      Author: lqp
 */

#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <jni.h>
#include <pthread.h>
#include <android/log.h>
#include "live_log.h"
#include <list>

class LogEntry {
public :
	const char* tag; //don't need free
	const char* msg;

	LogEntry() {
		tag = NULL;
		msg = NULL;
	}

	~LogEntry() {
		free((void*)msg);

		tag = NULL;
		msg = NULL;
	}
};

static JavaVM* pJavaVm;
static jclass sLiveLogClass;
static jmethodID sLogMethod;

static volatile bool sIsAlive = false;

static pthread_mutex_t sLock;
static pthread_cond_t sLockCond;

static pthread_t sLogThreadId;
static std::list<LogEntry*>* sLogEntryList;

#define ERR_LOG(fmtMsg, ...) \
	__android_log_print(ANDROID_LOG_INFO, "live_log_jni", fmtMsg, ##__VA_ARGS__)

JNIEnv* JNI_get_env()
{
	JNIEnv* env = NULL;
	//LOGD("JNI_get_env ###################");
	if(pJavaVm->AttachCurrentThread(&env, NULL)<0){
		return NULL;
	}

	return env;
}

void JNI_put_env()
{
	pJavaVm->DetachCurrentThread();
}

#define SAFE_FREE(addr)  \
    if(addr != NULL) {   \
        free(addr);      \
        addr = NULL;     \
    }

static void callLiveLog(JNIEnv *env, const char* pLogTag, const char* pMsg) {
	jstring tagString = env->NewStringUTF(pLogTag);
	jstring msgString = env->NewStringUTF(pMsg);

	env->CallStaticVoidMethod(sLiveLogClass, sLogMethod, tagString, msgString);

	env->DeleteLocalRef(tagString);
	env->DeleteLocalRef(msgString);
}

static void* logger_thread(void *) {
	JNIEnv *env = JNI_get_env();

	while(sIsAlive) {
		pthread_mutex_lock(&sLock);

		while (sLogEntryList->size() == 0) {
			pthread_cond_wait(&sLockCond, &sLock);
		}

		std::list<LogEntry*>::iterator itor = sLogEntryList->begin();

		while(itor != sLogEntryList->end()) {
			LogEntry* entry = *itor;

			callLiveLog(env, entry->tag, entry->msg);

			delete entry;

			itor = sLogEntryList->erase(itor);
		}

		pthread_mutex_unlock(&sLock);
	}

	JNI_put_env();
}



static void add_to_logList(LogEntry* entry) {
	if (entry == NULL) {
		return;
	}

	pthread_mutex_lock(&sLock);
	sLogEntryList->push_back(entry);

	pthread_cond_signal(&sLockCond);
	pthread_mutex_unlock(&sLock);
}

#define BUFFER_MAX 512

extern "C" {

JNIEXPORT void liveLog_LogV(const char* tag, const char* fmt, va_list vl)
{
	if (!sIsAlive) {
		ERR_LOG("liveLog_Log: logger not started");
		return;
	}

	char *buffer = (char*)malloc(BUFFER_MAX);
	if (buffer == NULL) {
		ERR_LOG("liveLog_Log: not enougth memory for buffer");
		return;
	}

    vsnprintf(buffer, BUFFER_MAX, fmt, vl);

    LogEntry *entry = (LogEntry*)malloc(sizeof(LogEntry));
    entry->tag = tag;
    entry->msg = strdup(buffer);

    add_to_logList(entry);

	SAFE_FREE(buffer);
}

JNIEXPORT void liveLog_Log(const char* tag, const char* fmt, ...)
{
	if (!sIsAlive) {
		ERR_LOG("liveLog_Log: logger not started");
		return;
	}

	va_list vl;
	va_start(vl, fmt);
	liveLog_LogV(tag, fmt, vl);
	va_end(vl);
}

JNIEXPORT void live_log_start(JNIEnv *env) {
	if (sIsAlive) {
		ERR_LOG("live_log_start: live_log already stared!!!");
		return;
	}

	pthread_mutex_init(&sLock, NULL);
	pthread_cond_init(&sLockCond, NULL);

	sLogEntryList = new std::list<LogEntry*>();

	int ret = 0;
	ret = pthread_create(&sLogThreadId, NULL, logger_thread, NULL);

	if (ret < 0) {
		ERR_LOG("live_log_start: crate logger thread failed");
	}

	sIsAlive = true;
}

JNIEXPORT void live_log_stop(JNIEnv *env) {
	if (!sIsAlive) {
		ERR_LOG("live_log_stop: logger not stared!!!");
		return;
	}

	pthread_mutex_lock(&sLock);
	sIsAlive = false;
	pthread_cond_broadcast(&sLockCond);
	pthread_mutex_unlock(&sLock);

	pthread_join(sLogThreadId, NULL);

	pthread_mutex_destroy(&sLock);
	pthread_cond_destroy(&sLockCond);

	//clean logs
	std::list<LogEntry*>::iterator itor = sLogEntryList->begin();
	while(itor != sLogEntryList->end()) {
		delete *itor;

		itor = sLogEntryList->erase(itor);
		itor++;
	}

	delete sLogEntryList;
}

JNIEXPORT int live_log_init(JNIEnv *env) {
	const char* liveClazzName = "com/wenba/live/LiveLog";

	jclass liveCls = env->FindClass(liveClazzName);
	if (liveCls == NULL) {
		ERR_LOG("live_log_init: liveCls == NULL");
		return -1;
	}

	sLiveLogClass = (jclass)env->NewGlobalRef(liveCls);

	jmethodID method = env->GetStaticMethodID(liveCls, "e", "(Ljava/lang/String;Ljava/lang/String;)V");
	if (method == NULL) {
		ERR_LOG("live_log_init: method == NULL");
		return -1;
	}

	sLogMethod = method;
	return 0;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		return -1;
	}

	pJavaVm = vm;

	live_log_init(env);
	live_log_start(env);

	return JNI_VERSION_1_4;
}
}
