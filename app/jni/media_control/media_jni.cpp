#include <jni.h>
#include <stddef.h>
#include <android/log.h>
#include <string.h>
#include "util_JNI.h"
#include "wenba_jni_helper.h"
#include "request.h"
#include "response.h"
#include <map>
#include <iostream>

#include "live_log.h"

using namespace std;

static std::map<int, jobject> sCallbackMap;
static pthread_mutex_t sCallBackLockMutex;

static JavaVM* sJavaVm;

static jobject sMediaInstance = NULL;
static jclass sMediaClazz = NULL;
static IRequest* sRequestManager = NULL;

//helper class
static jclass sJsonClazz;
static jmethodID sJsonInitMethod;
static jmethodID sPutObjectMethod;
static jmethodID sPutIntMethod;
static jmethodID sPutDoubleMethod;
static jmethodID sCallbackMethod;

#define LOG_TAG "media_control_jni"
#define DEBUG_LOG(format, ...) \
		liveLog_Log(LOG_TAG, format, ##__VA_ARGS__)

#define SRC_CAMERA   0x00
#define SRC_HISTORY  0x01
#define SRC_EXERCISE 0x02
#define SRC_FAVORITE  0x03
#define SRC_UNKNOWN  0x0f

JNIEnv* JNI_get_env() {
	JNIEnv* env = NULL;
	//LOGD("JNI_get_env ###################");
	if (sJavaVm->AttachCurrentThread(&env, NULL) < 0) {
		return NULL;
	}

	return env;
}

void JNI_put_env() {
	sJavaVm->DetachCurrentThread();
}

static void destoryRequestManager() {
	sRequestManager = NULL;
}

jclass getMediaClazz(JNIEnv *env) {
	return sMediaClazz;
}

static void lockMediaCallbackMutex() {
	pthread_mutex_lock(&sCallBackLockMutex);
}

static void unlockMediaCallbackMutex() {
	pthread_mutex_unlock(&sCallBackLockMutex);
}

static jobject getRequestCallback(int seq, const char* func,
		bool remove = true) {
	lockMediaCallbackMutex();

	jobject callback = NULL;

	map<int, jobject>::iterator iter = sCallbackMap.find(seq);
	if (iter == sCallbackMap.end()) {
		DEBUG_LOG("getRequestCallback : seq = 0x%08d not has a callback", seq);
		goto bail;
	}

	callback = iter->second;

	if (remove) {
		sCallbackMap.erase(iter);
	}

	DEBUG_LOG("==getReqCb: seq = [%08d] at [%s] cb object = 0x%08x",
			seq, func, callback);

	bail: unlockMediaCallbackMutex();
	return callback;
}

class ResponseHandler: public IResponse {

public:
	void OnLogin(const gwLoginRsp &res, UInt32 seq) {
		jobject callbackObject = getRequestCallback(seq, __FUNCTION__);
		if (callbackObject == NULL) {
			return;
		}

		JNIEnv *env = JNI_get_env();
		if (env == NULL) {
			DEBUG_LOG("ResponseHandler %s: env == NULL", __FUNCTION__);
			return;
		}

		jobject resObject = env->NewObject(sJsonClazz, sJsonInitMethod);
		if (env->ExceptionOccurred()) {
			DEBUG_LOG("ResponseHandler %s: create json exception occured",
					__FUNCTION__);
			return;
		}

		jstring tmpKey = env->NewStringUTF("code");

		env->CallObjectMethod(resObject, sPutIntMethod, tmpKey, res.errCode);
		if (res.errCode == 0) {
			tmpKey = env->NewStringUTF("userId");
			env->CallObjectMethod(resObject, sPutIntMethod, tmpKey, res.userId);
		} else {
			tmpKey = env->NewStringUTF("errorMsg");
			jstring errMsg = env->NewStringUTF(res.errMsg);

			env->CallObjectMethod(resObject, sPutObjectMethod, tmpKey, errMsg);
		}

		invokeCallbackMethod(env, callbackObject, resObject);

		JNI_put_env();
	}

	void OnLogout(const gwLogoutRsp &res, UInt32 seq) {
		jobject callbackObject = getRequestCallback(seq, __FUNCTION__);
		if (callbackObject == NULL) {
			return;
		}

		JNIEnv *env = JNI_get_env();
		if (env == NULL) {
			DEBUG_LOG("ResponseHandler %s: env == NULL", __FUNCTION__);
			return;
		}

		jobject resObject = env->NewObject(sJsonClazz, sJsonInitMethod);
		if (env->ExceptionOccurred()) {
			DEBUG_LOG("ResponseHandler %s: create json exception occured",
					__FUNCTION__);
			return;
		}

		jstring tmpKey = env->NewStringUTF("errCode");

		env->CallObjectMethod(resObject, sPutIntMethod, tmpKey, res.errCode);
		if (res.errCode != 0) {
			tmpKey = env->NewStringUTF("errMsg");
			jstring errMsg = env->NewStringUTF(res.errMsg);
			env->CallObjectMethod(resObject, sPutObjectMethod, tmpKey, errMsg);
		}

		invokeCallbackMethod(env, callbackObject, resObject);

		JNI_put_env();
	}

	void OnDisconnect(const gwDisconnectInfo &info) {
		JNIEnv *env = JNI_get_env();
		if (env == NULL) {
			DEBUG_LOG("ResponseHandler OnDisconnect: env == NULL");
			return;
		}

		jobject resObject = env->NewObject(sJsonClazz, sJsonInitMethod);
		if (env->ExceptionOccurred()) {
			DEBUG_LOG(
					"ResponseHandler OnDisconnect: create json exception occured");
			return;
		}

		jstring tmpKey = env->NewStringUTF("code");

		env->CallObjectMethod(resObject, sPutIntMethod, tmpKey, info.code);

		tmpKey = env->NewStringUTF("errorMsg");
		jstring errMsg = env->NewStringUTF(info.errorMsg);
		env->CallObjectMethod(resObject, sPutObjectMethod, tmpKey, errMsg);

		jclass mediaCls = getMediaClazz(env);
		jmethodID method = env->GetMethodID(mediaCls, "OnDisconnect",
				"(Lorg/json/JSONObject;)V");

		if (sMediaInstance != NULL) {
			env->CallVoidMethod(sMediaInstance, method, resObject);
		} else {
			DEBUG_LOG(
					"**warning : OnDisconnect called when sMediaInstance is null");
		}

		JNI_put_env();
	}

	void OnRecvMsg(const char* data, UInt32 dataLen, UInt32 operId, UInt32 seq) {
        DEBUG_LOG("OnRecvMsg msg --> %s", data);
        JNIEnv *env = JNI_get_env();
       		if (env == NULL) {
       			DEBUG_LOG("ResponseHandler OnRecvMsg: env == NULL");
       			return;
       		}

       		jobject resObject = env->NewObject(sJsonClazz, sJsonInitMethod);
       		if (env->ExceptionOccurred()) {
       			DEBUG_LOG(
       					"ResponseHandler OnRecvMsg: create json exception occured");
       			return;
       		}

       		jstring tmpKey = env->NewStringUTF("data");
       		jstring tmpV = env->NewStringUTF(data);

       		env->CallObjectMethod(resObject, sPutObjectMethod, tmpKey, tmpV);

       		jclass mediaCls = getMediaClazz(env);
       		jmethodID method = env->GetMethodID(mediaCls, "OnRecvMsg",
       				"(Lorg/json/JSONObject;)V");

    		if (sMediaInstance != NULL) {
       			env->CallVoidMethod(sMediaInstance, method, resObject);
       		} else {
       			DEBUG_LOG(
       					"**warning : OnRecvMsg called when sMediaInstance is null");
       		}

       		JNI_put_env();
	}

	void OnSendMsgError(UInt32 seq) {
        DEBUG_LOG("OnSendMsgError seq --> %d", seq);
	}

	void OnReloginSuccess(){
        JNIEnv *env = JNI_get_env();
        if (env == NULL) {
            DEBUG_LOG("ResponseHandler OnReconnectSuccess: env == NULL");
            return;
        }
        jclass mediaCls = getMediaClazz(env);
        jmethodID method = env->GetMethodID(mediaCls, "OnReconnectSuccess",
                "()V");
        if (sMediaInstance != NULL) {
            env->CallVoidMethod(sMediaInstance, method);
        } else {
            DEBUG_LOG(
                    "**warning : OnReconnectSuccess called when sMediaInstance is null");
        }

        JNI_put_env();
	}

	void OnStartRelogin(){
        JNIEnv *env = JNI_get_env();
		if (env == NULL) {
			DEBUG_LOG("ResponseHandler OnStartReconnect: env == NULL");
			return;
		}
		jclass mediaCls = getMediaClazz(env);
		jmethodID method = env->GetMethodID(mediaCls, "OnStartReconnect",
				"()V");
		if (sMediaInstance != NULL) {
			env->CallVoidMethod(sMediaInstance, method);
		} else {
			DEBUG_LOG(
					"**warning : OnStartReconnect called when sMediaInstance is null");
		}

		JNI_put_env();
	}

	void Log(const char* msg){
        DEBUG_LOG("log msg --> %s", msg);
	}

	void SdkLog(const char *logInfo) {
		liveLog_Log(LOG_TAG, "sdk: %s", logInfo);
	}

private:
	void invokeNotifyMethod(JNIEnv *env, const char *funcName,
			jobject jsonObject) {
		jclass mediaCls = getMediaClazz(env);
		jmethodID method = env->GetMethodID(mediaCls, funcName,
				"(Lorg/json/JSONObject;)V");

		DEBUG_LOG("invokeNotifyMethod: %s called ...", funcName);
		if (sMediaInstance != NULL) {
			env->CallVoidMethod(sMediaInstance, method, jsonObject);
		} else {
			DEBUG_LOG("**warning : %s called when sMediaInstance is null",
					funcName);
		}
	}

	void invokeCallbackMethod(JNIEnv* env, jobject callback,
			jobject resObject) {
		env->CallVoidMethod(callback, sCallbackMethod, resObject);
		env->DeleteGlobalRef(callback);
	}
};

static void addRequestCallback(JNIEnv* env, int seqId, jobject callback,
		const char* func) {
	if (callback == NULL) {
		return;
	}

	if (seqId == 0) {
		//invoke callback directly
		jobject resObject = env->NewObject(sJsonClazz, sJsonInitMethod);
		if (env->ExceptionOccurred()) {
			DEBUG_LOG("ResponseHandler OnLogin: create json exception occured");
			return;
		}

		jstring codeKey = env->NewStringUTF("code");
		jstring codeValue = env->NewStringUTF("-1");
		env->CallObjectMethod(resObject, sPutObjectMethod, codeKey, codeValue);

		jstring msgKey = env->NewStringUTF("errorMsg");
		jstring msgValue = env->NewStringUTF("No Network or not login");
		env->CallObjectMethod(resObject, sPutObjectMethod, msgKey, msgValue);

		env->CallVoidMethod(callback, sCallbackMethod, resObject);
		return;
	}

	jobject globaRef = NULL;

	lockMediaCallbackMutex();
	if (sCallbackMap.find(seqId) != sCallbackMap.end()) {
		DEBUG_LOG("addRequestCallback: seqId -> %d is already in callback map",
				seqId);
		goto bail;
	}

	globaRef = env->NewGlobalRef(callback);

	sCallbackMap[seqId] = globaRef;

	DEBUG_LOG("##addReqCb: seq = [%08d] by [%s] cb object = 0x%08x",
			seqId, func, globaRef);

	bail: unlockMediaCallbackMutex();
}

static void JNI_stop(JNIEnv* env, jobject thiz) {
	lockMediaCallbackMutex();
	DEBUG_LOG("JNI_stop call start...");
	IRequest* requestManager = sRequestManager;

	DEBUG_LOG("JNI_stop requestManager->Stop() start...");
	requestManager->Stop();
	DEBUG_LOG("JNI_stop requestManager->Stop() end...");

	env->DeleteGlobalRef(sMediaInstance);

	sMediaInstance = NULL;

	//destory callback Reference
	std::map<int, jobject>::iterator itor = sCallbackMap.begin();
	while (itor != sCallbackMap.end()) {

		env->DeleteGlobalRef(itor->second);

		itor = sCallbackMap.erase(itor);
	}

	DEBUG_LOG("JNI_stop call end...");
	unlockMediaCallbackMutex();
}

static jboolean JNI_start(JNIEnv* env, jobject thiz) {
	if (sMediaInstance != NULL) {
		liveLog_Log(LOG_TAG,
				"JNI_start: sMediaInstance != NULL, call JNI_stop first");
		JNI_stop(env, thiz);
	}

	DEBUG_LOG("JNI_start: call start...");
	lockMediaCallbackMutex();

	bool ret = false;

	if (sMediaInstance != NULL) {
		liveLog_Log(LOG_TAG, "JNI_start: sMediaInstance != NULL: 0x%8x",
				sMediaInstance);
		goto bail;
	}

	sMediaInstance = env->NewGlobalRef(thiz);
	if (!sRequestManager) {
		sRequestManager = CreateRequest();
		//not destoryed
		liveLog_Log(LOG_TAG, "JNI_start: sRequestManager is NULL");
	} else {
		liveLog_Log(LOG_TAG, "JNI_start: sRequestManager is not NULL");
	}

	{
		IResponse* sResponseNotify = new ResponseHandler();
		ret = sRequestManager->Start(sResponseNotify);
	}

	bail: unlockMediaCallbackMutex();
	DEBUG_LOG("JNI_start: call end...");
	return ret;
}

static void JNI_destory(JNIEnv* env, jobject thiz) {
	DEBUG_LOG("JNI_destory start...");
	JNI_stop(env, thiz);

	destoryRequestManager();
	DEBUG_LOG("JNI_destory end...");
}

static void JNI_login(JNIEnv* env, jobject thiz, jstring host, jint port,
		jstring userName, jstring password, jstring clientVer,
		jint operId, jobject callback) {

	IRequest* requestManager = sRequestManager;

	gwLoginReq req;

	ScopedUtfChars scName(env, userName);
	ScopedUtfChars scPassword(env, password);
	ScopedUtfChars scHost(env, host);
	ScopedUtfChars scClientVer(env, clientVer);

	strncpy(req.host, scHost.get(), sizeof(req.host));
	req.port = port;
	req.operId = operId;

	strncpy(req.userName, scName.get(), sizeof(req.userName));
	strncpy(req.password, scPassword.get(), sizeof(req.password));

	strncpy(req.clientVersion, scClientVer.get(), sizeof(req.clientVersion));

	int seqId = CreateGwSeq();

	if (callback != NULL) {
		addRequestCallback(env, seqId, callback, __FUNCTION__);
	}

	DEBUG_LOG("JNI_login, userName:%s, password:%s, host:%s, clientVer:%s, port:%d, operId:%d, seqId:%d", req.userName,req.password,req.host,req.clientVersion,req.port,req.operId,seqId);

	requestManager->Login(req, seqId);
}

static void JNI_logout(JNIEnv* env, jobject thiz, jint userId,
		jobject callback) {
	IRequest* requestManager = sRequestManager;

	gwLogoutReq req;

	req.user_id = userId;

	int seqId = CreateGwSeq();

	if (callback != NULL) {
		addRequestCallback(env, seqId, callback, __FUNCTION__);
	}

	requestManager->Logout(req, seqId);
}

static bool JNI_SendMsg(JNIEnv* env, jobject thiz, jstring message, jint operId,jobject callback) {
    IRequest* requestManager = sRequestManager;
    ScopedUtfChars scMessage(env, message);

    int seqId = CreateGwSeq();
	if (callback != NULL) {
		addRequestCallback(env, seqId, callback, __FUNCTION__);
	}

    DEBUG_LOG("JNI_SendMsg, userscMessage.get():%s, size:%d", scMessage.get(),scMessage.length());
    requestManager->SendMsg(scMessage.get(), scMessage.length(), seqId, operId);
}

static bool JNI_SendMsgToUser(JNIEnv* env, jobject thiz, jstring message, jint receiverId, jint receiverOperId,jint operId,jobject callback) {
    IRequest* requestManager = sRequestManager;
    ScopedUtfChars scMessage(env, message);

    int seqId = CreateGwSeq();
	if (callback != NULL) {
		addRequestCallback(env, seqId, callback, __FUNCTION__);
	}

    DEBUG_LOG("JNI_SendMsgToUser, userscMessage.get():%s, size:%d", scMessage.get(),scMessage.length());
    requestManager->SendMsgToUser(scMessage.get(), scMessage.length(), receiverId, receiverOperId, seqId, operId);
}

static JNINativeMethod gMethods[] =
		{
				//start
				{ "fun_start", "()Z",
						(void*) JNI_start },
				//stop
				{ "fun_stop", "()V",
						(void*) JNI_stop },
				//destory
				{ "fun_destory", "()V",
						(void*) JNI_destory },
				//login
				{ "fun_login",
						"(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;ILtestgateway/xun/com/testgateway/MediaControlManager$ResponseCallback;)V",
						(void*) JNI_login },
				//logout
				{ "fun_logout",
						"(Ltestgateway/xun/com/testgateway/MediaControlManager$ResponseCallback;)V",
						(void*) JNI_logout },
				//sendMsgToUser
                { "fun_sendMsgToUser",
                    "(Ljava/lang/String;IIILtestgateway/xun/com/testgateway/MediaControlManager$ResponseCallback;)V",
                    	(void*) JNI_SendMsgToUser },
                //sendMsg
                { "fun_SendMsg",
                    "(Ljava/lang/String;ILtestgateway/xun/com/testgateway/MediaControlManager$ResponseCallback;)V",
                         (void*) JNI_SendMsg },
				};

static int registerNatives(JNIEnv* jniEnv) {
	jclass clazz;
	clazz = jniEnv->FindClass("testgateway/xun/com/testgateway/MediaControlManager");
	if (clazz == NULL) {
		return -1;
	}

	sMediaClazz = (jclass) jniEnv->NewGlobalRef(clazz);

	if (jniEnv->RegisterNatives(clazz, gMethods,
			sizeof(gMethods) / sizeof(gMethods[0])) < 0) {
		return -1;
	}

	jclass tmpClazz = jniEnv->FindClass("org/json/JSONObject");
	if (tmpClazz == NULL) {
		WENBA_JNI_LOG(LOG_TAG, "registerNatives: json class not found");
		return -1;
	}

	sJsonClazz = (jclass) jniEnv->NewGlobalRef(tmpClazz);

	sJsonInitMethod = jniEnv->GetMethodID(sJsonClazz, "<init>", "()V");
	if (sJsonInitMethod == NULL) {
		WENBA_JNI_LOG(LOG_TAG, "registerNatives: json construstor not found");
		return -1;
	}

	sPutObjectMethod = jniEnv->GetMethodID(sJsonClazz, "put",
			"(Ljava/lang/String;Ljava/lang/Object;)Lorg/json/JSONObject;");
	if (sPutObjectMethod == NULL) {
		WENBA_JNI_LOG(LOG_TAG, "registerNatives: sPutObjectMethod not found");
		return -1;
	}

	sPutIntMethod = jniEnv->GetMethodID(sJsonClazz, "put",
			"(Ljava/lang/String;I)Lorg/json/JSONObject;");
	if (sPutIntMethod == NULL) {
		WENBA_JNI_LOG(LOG_TAG, "registerNatives: sPutIntMethod not found");
		return -1;
	}

	sPutDoubleMethod = jniEnv->GetMethodID(sJsonClazz, "put",
			"(Ljava/lang/String;D)Lorg/json/JSONObject;");
	if (sPutDoubleMethod == NULL) {
		WENBA_JNI_LOG(LOG_TAG, "registerNatives: sPutDoubleMethod not found");
		return -1;
	}

	tmpClazz = jniEnv->FindClass(
			"testgateway/xun/com/testgateway/MediaControlManager$ResponseCallback");
	if (tmpClazz == NULL) {
		WENBA_JNI_LOG(LOG_TAG, "registerNatives: callback class not found");
		return -1;
	}

	sCallbackMethod = jniEnv->GetMethodID(tmpClazz, "onResponse",
			"(Lorg/json/JSONObject;)V");
	if (sCallbackMethod == NULL) {
		WENBA_JNI_LOG(LOG_TAG, "registerNatives:callback method not found");
		return -1;
	}

	return 0;
}

static int media_jni_init(JNIEnv* env) {
	JavaVM *pJvm = NULL;

	WENBA_JNI_LOG(LOG_TAG, "media_jni_init called");

	int res = env->GetJavaVM(&pJvm);
	if (res < 0) {
		WENBA_JNI_LOG(LOG_TAG, "get java vm failed");
		return -1;
	}

	sJavaVm = pJvm;

	res = registerNatives(env);
	if (res != 0) {
		WENBA_JNI_LOG(LOG_TAG, "registerNatives returned : %d", res);
	}

	pthread_mutex_init(&sCallBackLockMutex, NULL);

	return res;
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		return -1;
	}

	media_jni_init(env);

	return JNI_VERSION_1_4;
}
