#ifndef response_h_
#define response_h_

#include "header.h"


/**
* @brief 接收响应类
*
* IResponse类包含了SDK所有的接受命令接口
*/

class WENBA_COMMON_API IResponse
{
public:
	/**
	* 学生登录响应
	* @param res 返回的响应数据
	* @param seq 消息序列码，关联请求与响应，与请求时发送的一致
	*/
	virtual void OnLogin(const gwLoginRsp &rsp, UInt32 seq) = 0;

	/**
	* 开始重连
	*/
	virtual void OnStartRelogin() = 0;

	/**
	* 重连成功
	*/
	virtual void OnReloginSuccess() = 0;

	/**
	* 学生注销响应
	* @param res 返回的响应数据
	* @param seq 消息序列码，关联请求与响应，与请求时发送的一致
	*/
	virtual void OnLogout(const gwLogoutRsp &rsp, UInt32 seq) = 0;

	/**
	* 网络连接断开
	* @param info 连接断开信息
	*/
	virtual void OnDisconnect(const gwDisconnectInfo &info) = 0;

	/**
	* 收到数据
	* @param data 数据指针
	* @param dataLen 数据长度
	* @param operId  业务id
	* @param seq     序列号
	*/
	virtual void OnRecvMsg(const char* data, UInt32 dataLen, UInt32 operId, UInt32 seq) = 0;

	/**
	* log 
	*/
	virtual void Log(const char* msg) = 0;

	/**
	* @param seq 序列号
	*/
	virtual void OnSendMsgError(UInt32 seq) = 0;

};

#endif
