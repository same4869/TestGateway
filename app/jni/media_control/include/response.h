#ifndef response_h_
#define response_h_

#include "header.h"


class WENBA_COMMON_API IResponse
{
public:
	virtual void OnLogin(const gwLoginRsp &rsp, UInt32 seq) = 0;

	virtual void OnStartRelogin() = 0;

	virtual void OnReloginSuccess() = 0;

	virtual void OnLogout(const gwLogoutRsp &rsp, UInt32 seq) = 0;

	virtual void OnDisconnect(const gwDisconnectInfo &info) = 0;

	virtual void OnRecvMsg(const char* data, UInt32 dataLen, UInt32 operId, UInt32 seq) = 0;

	virtual void Log(const char* msg) = 0;

	virtual void OnSendMsgError(UInt32 seq) = 0;

};

#endif
