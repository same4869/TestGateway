#ifndef response_h_
#define response_h_

#include "header.h"


/**
* @brief ������Ӧ��
*
* IResponse�������SDK���еĽ�������ӿ�
*/

class WENBA_COMMON_API IResponse
{
public:
	/**
	* ѧ����¼��Ӧ
	* @param res ���ص���Ӧ����
	* @param seq ��Ϣ�����룬������������Ӧ��������ʱ���͵�һ��
	*/
	virtual void OnLogin(const gwLoginRsp &rsp, UInt32 seq) = 0;

	/**
	* ��ʼ����
	*/
	virtual void OnStartRelogin() = 0;

	/**
	* �����ɹ�
	*/
	virtual void OnReloginSuccess() = 0;

	/**
	* ѧ��ע����Ӧ
	* @param res ���ص���Ӧ����
	* @param seq ��Ϣ�����룬������������Ӧ��������ʱ���͵�һ��
	*/
	virtual void OnLogout(const gwLogoutRsp &rsp, UInt32 seq) = 0;

	/**
	* �������ӶϿ�
	* @param info ���ӶϿ���Ϣ
	*/
	virtual void OnDisconnect(const gwDisconnectInfo &info) = 0;

	/**
	* �յ�����
	* @param data ����ָ��
	* @param dataLen ���ݳ���
	* @param operId  ҵ��id
	* @param seq     ���к�
	*/
	virtual void OnRecvMsg(const char* data, UInt32 dataLen, UInt32 operId, UInt32 seq) = 0;

	/**
	* log 
	*/
	virtual void Log(const char* msg) = 0;

	/**
	* @param seq ���к�
	*/
	virtual void OnSendMsgError(UInt32 seq) = 0;

};

#endif
