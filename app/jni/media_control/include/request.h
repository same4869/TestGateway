#ifndef request_h_
#define request_h_

#include "header.h"


class IRequest;
class IResponse;

/**< c��ʼ���������ӿ� */
extern "C" WENBA_COMMON_API IRequest *CreateRequest();

//������Ϣ���к�
extern "C" WENBA_COMMON_API UInt32 CreateGwSeq();

/**
* @brief ����������
*
* IRequest�������SDK���еķ�������ӿ�
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
	* ����SDK��ʹ��SDKʱ����Ҫ���ô˺���
	* @param pNotify �ص���ʵ�����ͻ�����Ҫ��IResponse��������ʵ�����еĽӿ�
    * @param encryptType �������紫����ܷ�ʽ��0-������ 1-������
	* @return �ɹ�����true ʧ�ܷ���false
	*/
	virtual bool Start(IResponse *pNotify, int encryptType = 0, char * encryptPassword = 0) = 0;

	/**
	* ֹͣSDK����
	*/
	virtual void Stop(void) = 0;

	/**
	*����sdk�� �ͷ��ڴ�
	*/
	virtual void Destroy() = 0;


	/**
	* ѧ����¼����
	* @param req  ��������
	* @see IWenbaStResponse::OnLogin
	* @return �ɹ�����true ʧ�ܷ���false
	*/
	virtual bool Login(const gwLoginReq &req, const UInt32 seq = 0, const UInt32 timeout = enmTimeoutSeconds) = 0;

	/**
	* ѧ��ע������
	* @param req  ��������
	* @see IWenbaStResponse::OnLogout
	* @return �ɹ�����true ʧ�ܷ���false
	*/
	virtual bool Logout(const gwLogoutReq &req, const UInt32 seq = 0, const UInt32 timeout = enmTimeoutSeconds) = 0;


	virtual bool SendMsg(char* data, UInt32 dataLen, const UInt32 seq, const UInt32 operId) = 0;

virtual bool SendMsgToUser(char* data, UInt32 dataLen, UInt64 receiverId, UInt32 receiverOperId, const UInt32 seq, const UInt32 operId)=0;
	/**
	* ��ȡresponse����
	*/
	virtual IResponse *GetResponse() = 0;

};

#endif
