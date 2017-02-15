#ifndef request_h_
#define request_h_

#include "header.h"


class IRequest;
class IResponse;

/**< c初始化请求对象接口 */
extern "C" WENBA_COMMON_API IRequest *CreateRequest();

//生成消息序列号
extern "C" WENBA_COMMON_API UInt32 CreateGwSeq();

/**
* @brief 发送请求类
*
* IRequest类包含了SDK所有的发送命令接口
*/

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
	* @param pNotify 回调类实例，客户端需要从IResponse类派生并实现所有的接口
    * @param encryptType 数据网络传输加密方式，0-不加密 1-异或加密
	* @return 成功返回true 失败返回false
	*/
	virtual bool Start(IResponse *pNotify, int encryptType = 0, char * encryptPassword = 0) = 0;

	/**
	* 停止SDK服务
	*/
	virtual void Stop(void) = 0;

	/**
	*销毁sdk， 释放内存
	*/
	virtual void Destroy() = 0;


	/**
	* 学生登录请求
	* @param req  请求数据
	* @see IWenbaStResponse::OnLogin
	* @return 成功返回true 失败返回false
	*/
	virtual bool Login(const gwLoginReq &req, const UInt32 seq = 0, const UInt32 timeout = enmTimeoutSeconds) = 0;

	/**
	* 学生注销请求
	* @param req  请求数据
	* @see IWenbaStResponse::OnLogout
	* @return 成功返回true 失败返回false
	*/
	virtual bool Logout(const gwLogoutReq &req, const UInt32 seq = 0, const UInt32 timeout = enmTimeoutSeconds) = 0;


	virtual bool SendMsg(char* data, UInt32 dataLen, const UInt32 seq, const UInt32 operId) = 0;

virtual bool SendMsgToUser(char* data, UInt32 dataLen, UInt64 receiverId, UInt32 receiverOperId, const UInt32 seq, const UInt32 operId)=0;
	/**
	* 获取response对象
	*/
	virtual IResponse *GetResponse() = 0;

};

#endif
