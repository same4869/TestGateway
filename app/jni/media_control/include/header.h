#ifndef _header_h__
#define _header_h__


#ifdef WENBA_COMMON_EXPORTS
#define WENBA_COMMON_API __declspec(dllexport)
#else
#define WENBA_COMMON_API
#endif


#include "Poco/Types.h"
using namespace Poco;

/// ���ȶ����
#define GW_USER_LEN                                256  ///< �û�������
#define GW_PWD_LEN                                 256  ///< ���볤��
#define GW_VERSION_LEN                             64   ///< �ͻ��˰汾�ų���
#define GW_EXT_DATA_LEN								4096 ///< ��չ���ݳ���
#define GW_ERR_MSG_LEN                             128  ///< ������Ϣ����
#define GW_HOST_LEN									128



#define GW_TOKEN_LEN				                128	 ///< ���Ƴ���;
#define GW_PHONE_LEN			                    20   ///< �绰����
#define GW_REASON_LEN                              128  ///< ԭ�򳤶�
#define GW_ERRMSG_LEN                              128  ///< ������Ϣ����
#define GW_AVATAR_LEN                              512  ///< �û�ͷ��url����


#pragma pack(1)
typedef struct { //gateway to client header
	UInt32 totalLen;
	UInt16 oobLen;
	UInt32 seqId;
	UInt32 operId;
	UInt8  crypt;
	UInt16 headerCrc;
	UInt16 bodyCrc;
	char	   oobData[0];
} gwPackHeader;

typedef struct {
	char		userName[GW_USER_LEN];
	char		password[GW_PWD_LEN];
	char		host[GW_HOST_LEN];
	UInt32	port;
	char		clientVersion[GW_VERSION_LEN];
	char		extData[GW_EXT_DATA_LEN];
	UInt32 operId;
} gwLoginReq;

typedef struct {
	UInt32	errCode;
	char		userName[GW_USER_LEN];
	UInt64	userId;
	UInt32  loginType;
	char		errMsg[GW_ERR_MSG_LEN];
	UInt32  resendTimes;
	UInt32  heartBeatInterval;
	UInt32  offlineDetectTime;
	UInt64 sessionId;
	UInt32 contextId;
} gwLoginRsp;

typedef struct {
	UInt64 user_id;
	UInt32 oper_id;
}gwLogoutReq;

typedef struct {
	UInt32	errCode;
	char	errMsg[GW_ERR_MSG_LEN];
}gwLogoutRsp;

typedef struct
{
	UInt32 code;                                ///< ������
	char errorMsg[GW_ERRMSG_LEN];                  ///< ������Ϣ
}gwDisconnectInfo;


#pragma pack()

#endif

