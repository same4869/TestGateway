/*
 * live_log.h
 *
 *  Created on: Jul 29, 2015
 *      Author: lqp
 */

#ifndef LIVE_LOG_H_
#define LIVE_LOG_H_
#include "stdlib.h"

#ifdef __cplusplus
extern "C" {
void liveLog_LogV(const char* tag, const char* fmt, va_list vl);
void liveLog_Log(const char* tag, const char* fmt, ...);
int live_log_init(JNIEnv *env);
void live_log_start(JNIEnv *env);
void live_log_stop(JNIEnv *env);
}
#else
void liveLog_LogV(const char* tag, const char* fmt, va_list vl);
void liveLog_Log(const char* tag, const char* fmt, ...);
int live_log_init(JNIEnv *env);
void live_log_start(JNIEnv *env);
void live_log_stop(JNIEnv *env);
#endif

#endif /* LIVE_LOG_H_ */
