/*
 * scoped_utf_char.h

 *
 *  Created on: Aug 28, 2015
 *      Author: lqp
 */

#ifndef WEBRTC_SCOPED_UTF_CHAR_H_
#define WEBRTC_SCOPED_UTF_CHAR_H_

#include <stddef.h>
#include "jni.h"

class ScopedUtfChars {
public:
	ScopedUtfChars(JNIEnv* env, jstring str) {
		pEnv = env;
		javaString = str;

		if (javaString) {
			utfChars = env->GetStringUTFChars(str, NULL);
		}else {
			utfChars = NULL;
		}

	}

	~ScopedUtfChars() {
		if (javaString) {
			pEnv->ReleaseStringUTFChars(javaString, utfChars);
		}
	}

	int length() const {
		if (utfChars) {
			return strlen(utfChars);
		}

		return 0;
	}

	const char *get() {
		return utfChars;
	}

private :
	JNIEnv *pEnv;
	jstring javaString;
	const char *utfChars;

	ScopedUtfChars(const ScopedUtfChars&);
    void operator=(const ScopedUtfChars&);
};


static inline JNIEnv* wenba_get_env(JavaVM *vm) {
	JNIEnv *env = NULL;

	vm->AttachCurrentThread(&env, NULL);

	return env;
}

static inline void wenba_put_env(JavaVM *vm, JNIEnv *env) {
	vm->DetachCurrentThread();
}

#endif /* WEBRTC_SCOPED_UTF_CHAR_H_ */
