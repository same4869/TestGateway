#ifndef request_h_
#define request_h_

#include "header.h"


class IRequest;
class IResponse;

extern "C" WENBA_COMMON_API IRequest *CreateRequest();

extern "C" WENBA_COMMON_API UInt32 CreateGwSeq();


class WENBA_COMMON_API IRequest
{
public:
	enum
	{
		enmTimeoutSeconds = 60,
	};

public:

    /**
	* 启动SDK，使用SDK时首先要调用此函数
	* @param pNotify 回调类实例，客户端需要从IWenbaStResponse类派生并实现所有的接口
	* @return 成功返回true 失败返回false
	*/
	virtual bool Start(IResponse *pNotify) = 0;

	/**
	* 停止SDK服务
	*/
	virtual void Stop(void) = 0;

	virtual void Destroy() = 0;


	/**
	* 学生登录请求
    * @param req  请求数据
    * @see IWenbaStResponse::OnLogin
    * @return 消息序列码，用来关联请求与响应，等于0表示失败,大于0表示成功
    */
	virtual bool Login(const gwLoginReq &req, const UInt32 seq = 0, const UInt32 timeout = enmTimeoutSeconds) = 0;

	/**
    * 学生注销请求
    * @param req  请求数据
    * @see IWenbaStResponse::OnLogout
    * @return 消息序列码，用来关联请求与响应，等于0表示失败,大于0表示成功
	*/
	virtual bool Logout(const gwLogoutReq &req, const UInt32 seq = 0, const UInt32 timeout = enmTimeoutSeconds) = 0;

	virtual bool SendMsg(char* data, UInt32 dataLen, const UInt32 seq, const UInt32 operId) = 0;

    virtual bool SendMsgToUser(char* data, UInt32 dataLen, UInt64 receiverId, UInt32 receiverOperId, const UInt32 seq, const UInt32 operId)=0;

	virtual IResponse *GetResponse() = 0;

};

#endif
