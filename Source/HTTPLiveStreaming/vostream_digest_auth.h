#pragma once
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2007				*
*																		*
************************************************************************/
/*******************************************************************************
File:		vostream_digest_auth.h

Contains:	handle Digest access authentication

Written by:	Stony

Change History (most recent first):
2012-03-20		Stony		Create file
2012-03-22		Thomas		Change it for general usage of RTSP and HLS

*******************************************************************************/

#ifndef __VOSTREAM_DIGEST_AUTH_H__
#define __VOSTREAM_DIGEST_AUTH_H__

#include "voType.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


typedef enum __QOPType
{
	DIGEST_QOP_NONE = 0,
	DIGEST_QOP_AUTH,
	DIGEST_QOP_AUTHINT,
}DigestQOPType;

//implementation Digest access authentication
//refer to: http://en.wikipedia.org/wiki/Digest_access_authentication  and
////http://tools.ietf.org/html/rfc2617
class voStreamDigestAuth
{
public:
	voStreamDigestAuth(void);
	~voStreamDigestAuth(void);

	VO_VOID Reset();
	
	VO_VOID SetUserInfo(VO_CHAR *usr,VO_U16 usrLen, VO_CHAR *passwd, VO_U16 passwdLen);
	VO_VOID SetNonce(VO_CHAR *nonce,VO_U16 len);
	VO_VOID	SetMethod(VO_CHAR *method,VO_U16 len);
	VO_VOID	SetRealm(VO_CHAR *realm,VO_U16 len);
	VO_VOID	SetURI(VO_CHAR *uri,VO_U16 len);
	VO_VOID	SetQOP(DigestQOPType type);
	VO_VOID SetQOPString(VO_CHAR *pQop,VO_U16 len);
	VO_VOID	SetClientNonce(VO_CHAR *pClientNonce,VO_U16 len);
	VO_VOID	SetClientNonceCount(VO_CHAR *pClientNonceCount,VO_U16 len);

	VO_BOOL	GetResponse(VO_CHAR *buf, unsigned int len);
	

private:
	VO_BOOL GetHA1();
	VO_BOOL GetHA2(DigestQOPType type);
	VO_BOOL GetHA2None();
	VO_BOOL GetHA2AuthInt();

	VO_BOOL	GetRespNone(VO_CHAR *buf, unsigned int len);
	VO_BOOL	GetRespAuth(VO_CHAR *buf, unsigned int len);
	
private:
	VO_CHAR  		m_strHA1[33];
	VO_CHAR  		m_strHA2[33];

    VO_CHAR         m_strClientNonce[32];
	VO_U32          m_ulstrClientNonceLen;
    VO_CHAR         m_strClientNonceCount[32];
    VO_U32          m_ulstrClientNonceCountLen;

	VO_CHAR *		m_strQop;
	VO_U16   		m_strQopLen;

	VO_CHAR *		m_strUser;
	VO_U16   		m_strUserLen;
	
	VO_CHAR *		m_strPasswd;
	VO_U16   		m_strPasswdLen;
	
	VO_CHAR *		m_strNonce;
	VO_U16   		m_strNonceLen;

	VO_CHAR *		m_strMethod;
	VO_U16   		m_strMethodLen;

	VO_CHAR *		m_strRealm;
	VO_U16   		m_strRealmLen;
	
	VO_CHAR *		m_strURI;
	VO_U16   		m_strURILen;

	DigestQOPType 	m_qopType;	
};

#ifdef _VONAMESPACE
}
#endif


#endif //__VOSTREAM_DIGEST_AUTH_H__
