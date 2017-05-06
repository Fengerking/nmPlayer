#include <stdlib.h>
#include <string.h>

#include "vostream_digest_auth.h"
#include "md5.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voStreamDigestAuth::voStreamDigestAuth(void)
{
	memset(m_strHA1,0,33);
	memset(m_strHA2,0,33);

	m_strUser = NULL;
	m_strPasswd = NULL;
	m_strNonce = NULL;
	m_strMethod = NULL;
	m_strRealm = NULL;
	m_strURI = NULL;
	
	m_qopType = DIGEST_QOP_NONE;

}

voStreamDigestAuth::~voStreamDigestAuth(void)
{
	Reset();
}

VO_VOID voStreamDigestAuth::Reset()
{
	memset(m_strHA1,0,33);
	memset(m_strHA2,0,33);

	if(m_strUser)
		delete [] m_strUser;
	m_strUserLen = 0;

	if(m_strPasswd)
		delete [] m_strPasswd;
	m_strPasswdLen = 0;

	if(m_strNonce)
		delete [] m_strNonce;
	m_strNonceLen = 0;

	if(m_strMethod)
		delete [] m_strMethod;
	m_strMethodLen = 0;

	if(m_strRealm)
		delete [] m_strRealm;
	m_strRealmLen = 0;

	if(m_strURI)
		delete [] m_strURI;
	m_strURILen = 0;

	m_qopType = DIGEST_QOP_NONE;	
}

VO_VOID voStreamDigestAuth::SetUserInfo(VO_CHAR *usr,VO_U16 usrLen, VO_CHAR *passwd, VO_U16 passwdLen)
{
	if(!usr || !passwd)
		return;

	if(m_strUser)
		delete [] m_strUser;

	m_strUserLen = usrLen;
	m_strUser = new char[m_strUserLen+1];
	strcpy(m_strUser,usr);

	if(m_strPasswd)
		delete [] m_strPasswd;

	m_strPasswdLen = passwdLen;
	m_strPasswd = new char[m_strPasswdLen+1];
	strcpy(m_strPasswd,passwd);
	
	return;
}

VO_VOID voStreamDigestAuth::SetNonce(VO_CHAR *nonce,VO_U16 len)
{
	if(!nonce)
		return;

	if(m_strNonce)
		delete [] m_strNonce;

	m_strNonceLen = len ;
	m_strNonce = new char[m_strNonceLen+1];
	strcpy(m_strNonce,nonce);
	
	return;
}

VO_VOID	voStreamDigestAuth::SetMethod(VO_CHAR *method,VO_U16 len)
{
	if(!method)
		return;

	if(m_strMethod)
		delete [] m_strMethod;

	m_strMethodLen = len ;
	m_strMethod = new char[m_strMethodLen+1];
	strcpy(m_strMethod,method);
	
	return;
}

VO_VOID	voStreamDigestAuth::SetRealm(VO_CHAR *realm,VO_U16 len)
{
	if(!realm)
		return;

	if(m_strRealm)
		delete [] m_strRealm;

	m_strRealmLen = len ;
	m_strRealm = new char[m_strRealmLen+1];
	strcpy(m_strRealm,realm);
	return;
}

VO_VOID	voStreamDigestAuth::SetURI(VO_CHAR *uri,VO_U16 len)
{
	if(!uri)
		return;

	if(m_strURI)
		delete [] m_strURI;

	m_strURILen = len;
	m_strURI = new char[m_strURILen+1];
	strcpy(m_strURI,uri);
	
	return;
}

VO_VOID	voStreamDigestAuth::SetQOP(DigestQOPType type)
{
	m_qopType = type;
	return;
}

VO_BOOL	voStreamDigestAuth::GetResponse(VO_CHAR *buf, unsigned int len)
{
	VO_BOOL ret = VO_FALSE;

	if((!buf) || (len<32))
		return VO_FALSE;
	
	memset(buf, 0, len);
	
	if(!GetHA1())
		return VO_FALSE;
	
	if(!GetHA2(m_qopType))
		return VO_FALSE;

	switch(m_qopType)
	{
		case DIGEST_QOP_NONE:
			ret = GetRespNone(buf,len);
			break;
			
		case DIGEST_QOP_AUTHINT:
		case DIGEST_QOP_AUTH:
			ret = GetRespAuth(buf,len);
			break;
			
		default:
			break;
	}
	
	return ret;
}

VO_BOOL voStreamDigestAuth::GetHA1()
{
	if(!m_strUser||!m_strPasswd||!m_strRealm)
		return VO_FALSE;

	VO_U16 tempLen = m_strUserLen+m_strRealmLen+m_strPasswdLen+2;
	char *temp = new char[tempLen+1];
	
	memset(temp,0,tempLen);

	strcat(temp,m_strUser);
	temp[m_strUserLen] = ':';

	strcat(temp,m_strRealm);
	temp[m_strUserLen+1+m_strRealmLen] = ':';

	strcat(temp,m_strPasswd);

	MD5Data(m_strHA1,temp,tempLen);

	delete [] temp;
	
	return VO_TRUE;
}

VO_BOOL voStreamDigestAuth::GetHA2(DigestQOPType type)
{
	VO_BOOL ret = VO_FALSE;
	
	switch(m_qopType)
	{
		case DIGEST_QOP_NONE:
		case DIGEST_QOP_AUTH:
			ret = GetHA2None();
			break;
			
		case DIGEST_QOP_AUTHINT:
			ret = GetHA2AuthInt();
			break;
			
		default:
			break;
	}
	return ret;
}

VO_BOOL voStreamDigestAuth::GetHA2None()
{
	if(!m_strMethod||!m_strURI)
		return VO_FALSE;

	VO_U16 tempLen = m_strMethodLen+m_strURILen+1;
	char *temp = new char[tempLen+1];
	
	memset(temp,0,tempLen);

	strcat(temp,m_strMethod);
	temp[m_strMethodLen] = ':';

	strcat(temp,m_strURI);

	MD5Data(m_strHA2,temp,tempLen);

	delete [] temp;
	
	return VO_TRUE;
}

VO_BOOL voStreamDigestAuth::GetHA2AuthInt()
{
	return VO_FALSE;
}

VO_BOOL	voStreamDigestAuth::GetRespNone(VO_CHAR *buf, unsigned int len)
{
	if(!m_strNonce)
		return VO_FALSE ;
	
	VO_U16 tempLen = 32*2+m_strNonceLen+2;
	char *temp = new char[tempLen+1];
	
	memset(temp,0,tempLen);

	strcat(temp,m_strHA1);
	temp[32] = ':';

	strcat(temp,m_strNonce);
	temp[33+m_strNonceLen] = ':';

	strcat(temp,m_strHA2);

	MD5Data(buf,temp,tempLen);

	delete [] temp;
	
	return VO_TRUE;
}

VO_BOOL	voStreamDigestAuth::GetRespAuth(VO_CHAR *buf, unsigned int len)
{
	return VO_FALSE;
}
