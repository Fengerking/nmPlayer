#include "VOAES128_CBC.h"
#include "VOOSMPDRM.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

int CheckEncrypted_HLS(char* pKeyString)
{
	if (NULL == pKeyString || strlen(pKeyString) == 0)
	{
		return VO_OSMP_DRM_ERR_UNENCRYPT;//clr
	}

	VO_CHAR * p = strstr(pKeyString, "METHOD=");
	if (NULL == p) {
		VOLOGE("NO METHOD=");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	p += strlen("METHOD=");
	if (memcmp(p, "NONE", strlen("NONE") ) == 0)
	{
		return VO_OSMP_DRM_ERR_UNENCRYPT;//clr
	}
	else if (memcmp(p, "AES-128", strlen("AES-128") ) != 0) {
		VOLOGE("NOT AES-128");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	return VO_OSMP_DRM_ERR_NONE;
}

VO_U32 getKeyURI_HLS(VO_CHAR* pCurURL, VO_CHAR* pKeyString, VO_CHAR* pKeyURI, VO_PTR pReserved)
{
	if (NULL == pKeyString || strlen(pKeyString) == 0)
		return VO_OSMP_DRM_ERR_UNENCRYPT;

	VO_CHAR * p = strstr(pKeyString, "METHOD=");
	if (NULL == p) {
		VOLOGE("NO METHOD=");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	p += strlen("METHOD=");
	if (memcmp(p, "NONE", strlen("NONE") ) == 0)
	{
		return VO_OSMP_DRM_ERR_UNENCRYPT;
	}
	else if (memcmp(p, "AES-128", strlen("AES-128") ) != 0) {
		VOLOGE("NOT AES-128");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	//m_bEncrypted = VO_TRUE;

	p = strstr(pKeyString , "URI=");
	if (NULL == p) {
		VOLOGE("NO URI=");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}
	p += strlen("URI=\"");

	VO_CHAR* pEnd = strchr(p, '\"');

	VO_S32 iLen = 0;
	if (pEnd)
		iLen = pEnd - p;
	else
		iLen = strlen(p);

	VO_CHAR szURL[2048] = {0};
	memcpy(szURL, p, iLen);

	VO_U32 uRet = GetTheAbsolutePath(pKeyURI, szURL, pCurURL);
	if (0 != uRet) {
		VOLOGE("!getKeyURI 0x%x", uRet);
		return uRet;
	}

	return VO_OSMP_DRM_ERR_NONE;
}

VO_VOID str2IV_HLS(VO_PBYTE ptr_key , VO_CHAR * pStr)
{
	if (NULL == ptr_key || NULL == pStr)
	{
		VOLOGE("empty pointor");
		return;
	}

	VO_CHAR szIV[33];
	memset(szIV, '0', 33);

	VO_CHAR *p = szIV + 32 - strlen(pStr);
	strcpy(p, pStr);

	for (VO_S32 i = 0; i < 16; i++)
	{
		VO_CHAR temp[3];

		temp[0] = szIV[2*i];
		temp[1] = szIV[2*i + 1];
		temp[2] = '\0';

		int value;

		sscanf(temp, "%x", &value);

		ptr_key[i] = value;
	}
}

VO_U32 getIV_HLS(VO_CHAR* pKeyString, VO_U32 uSequenceNum, VO_BYTE* pIV, VO_PTR pReserved)
{
	if (NULL == pKeyString || NULL == pIV)
	{
		VOLOGE("empty pointor");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	VO_CHAR * pBuffer = strstr(pKeyString , "IV=");

	VO_CHAR Buffer[1024] = {0};
	if (pBuffer)
	{
		pBuffer += strlen( "IV=" );
		VO_CHAR * pEnd = strchr(pBuffer, ',');
		if (pEnd)
		{
			strncpy(Buffer, pBuffer, pEnd - pBuffer);
		}
		else
		{
			strcpy(Buffer, pBuffer);
		}

		pBuffer = Buffer;
	}

	if (pBuffer)
	{
		if ( '0' == pBuffer[0] &&
			('x' == pBuffer[1] || 'X' == pBuffer[1]) )
		{
			pBuffer += strlen("0x");

			str2IV_HLS(pIV, pBuffer);
		}
		else
		{
			strcpy( (char *)pIV, pBuffer );
		}
	}
	else
	{
		pIV[15] = (VO_BYTE)((uSequenceNum)&0x000000ff);
		pIV[14] = (VO_BYTE)(((uSequenceNum)>>8)&0x000000ff);
		pIV[13] = (VO_BYTE)(((uSequenceNum)>>16)&0x000000ff);
		pIV[12] = (VO_BYTE)(((uSequenceNum)>>24)&0x000000ff);
	}

	return VO_OSMP_DRM_ERR_NONE;
}

#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

VOAES128_CBC::VOAES128_CBC(void)
: m_pDelegate(NULL)
, m_identifer(0xFFFFFFFF)
, m_bNeedDecrypt(false)
, m_iLen(0)
{
	memset(m_Buf, 0, 752);
	memset(m_PreKeyURI, 0, 2048);
}

VOAES128_CBC::~VOAES128_CBC(void)
{
}

int VOAES128_CBC::Open(VOAES128_CBC::Delegate *pDelegate, int identifer, void* pAdditionalInfo)
{
	if (0xFFFFFFFF != m_identifer)
		return VO_OSMP_DRM_ERR_STATUS;

	m_identifer = identifer;
	m_pDelegate = pDelegate;
	m_bNeedDecrypt = false;
	m_iLen = 0;

	VO_DRM2_HSL_PROCESS_INFO* pDRMHLSProcess = static_cast<VO_DRM2_HSL_PROCESS_INFO*>(pAdditionalInfo);
	VOLOGR("szCurURL %s", pDRMHLSProcess->szCurURL);
	VOLOGR("szKeyString %s", pDRMHLSProcess->szKeyString);
	VOLOGR("uSequenceNum %d", pDRMHLSProcess->uSequenceNum);

	int iRet = CheckEncrypted_HLS(pDRMHLSProcess->szKeyString);
	if (iRet == VO_OSMP_DRM_ERR_NONE)
		m_bNeedDecrypt = true;

	if (false == m_bNeedDecrypt)
		return VO_OSMP_DRM_ERR_NONE;

	char szKeyURI[2048] = {0};
	iRet = getKeyURI_HLS(pDRMHLSProcess->szCurURL, pDRMHLSProcess->szKeyString, szKeyURI, NULL);
	if (0 != iRet) {
		VOLOGE("!getKeyURI 0x%x", iRet);
		return iRet;
	}

	if (strcmp(szKeyURI, m_PreKeyURI) != 0)
	{
		unsigned char aKey[16] = {0};
		int iSize = 16;
		iRet = m_pDelegate->getKey(szKeyURI, aKey, &iSize);
		if (0 != iRet) {
			VOLOGE("!getKey 0x%x", iRet);
			return VO_OSMP_DRM_ERR_STATUS;
		}
		else if (16 != iSize)
		{
			VOLOGW("key length %d", iSize);
			return VO_OSMP_DRM_ERR_STATUS;
		}

		strcpy(m_PreKeyURI, szKeyURI);

		VOLOGR("key: %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X", aKey[0], aKey[1], aKey[2], aKey[3], aKey[4], aKey[5], aKey[6], aKey[7], aKey[8], aKey[9], aKey[10], aKey[11], aKey[12], aKey[13], aKey[14], aKey[15]);
		iRet = setKey(aKey, iSize);
		if (iRet) {
			VOLOGE("!setKey");
			return VO_OSMP_DRM_ERR_BASE;
		}
	}

	unsigned char aIV[16] = {0};
	iRet = getIV_HLS(pDRMHLSProcess->szKeyString, pDRMHLSProcess->uSequenceNum, aIV, NULL);
	if (0 != iRet) {
		VOLOGE("!getIV 0x%x", iRet);
		return iRet;
	}

	VOLOGR("iv: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", aIV[0], aIV[1], aIV[2], aIV[3], aIV[4], aIV[5], aIV[6], aIV[7], aIV[8], aIV[9], aIV[10], aIV[11], aIV[12], aIV[13], aIV[14], aIV[15]);
	iRet = setIV(aIV, 16);
	if (iRet) {
		VOLOGE("!setIV");
		return VO_OSMP_DRM_ERR_BASE;
	}

	return VO_OSMP_DRM_ERR_NONE;
}

int VOAES128_CBC::Decrypt(long long llPosition, unsigned char* pSrcData, int iSrcDdata, bool bLastBlock, unsigned char* pDesData, int* piDesData, void* pAdditionalInfo)
{
	if (false == m_bNeedDecrypt)
		return VO_OSMP_DRM_ERR_NONE;

	if (iSrcDdata == 0)
		return VO_OSMP_DRM_ERR_NONE;

	VO_U32 dec_size = 0;

	if (m_iLen + iSrcDdata > 752)
	{
		VO_BYTE temp[752] = {0};

		VO_U32 iTotalSize = m_iLen + iSrcDdata;
		VO_U32 iNeedDecrypt = (iTotalSize / 752) * 752;
		VO_U32 iLeftSize = iTotalSize - iNeedDecrypt;

		if (0 == iLeftSize)
		{
			iLeftSize = 752;
			iNeedDecrypt -= 752;
		}

		//if (iNeedDecrypt > *piDesData)
		//{
		//	return DRM_ERR_OUTPUT_BUF_SMALL;//small
		//}

		memcpy(temp, pSrcData + iSrcDdata - iLeftSize, iLeftSize);

		VO_U32 uLen = 752 - m_iLen;
		if (uLen)
			memcpy(m_Buf + m_iLen, pSrcData, uLen);

		VO_U32 uSizeOutput = 0;
		VO_U32 uRet = decryptData(m_Buf, 752, m_Buf, &uSizeOutput, VO_FALSE);
		if (0 != uRet) {
			VOLOGE("!decryptData 0x%x", uRet);
			return uRet;
		}
		dec_size += uSizeOutput;

		uSizeOutput = 0;
		uRet = decryptData(pSrcData + uLen, iSrcDdata - uLen - iLeftSize, pSrcData + uLen, &uSizeOutput, VO_FALSE);
		if (0 != uRet) {
			VOLOGE("!decryptData 0x%x", uRet);
			return uRet;
		}
		dec_size += uSizeOutput;

		memmove(pSrcData + 752, pSrcData + uLen, iSrcDdata - uLen - iLeftSize);

		memcpy(pSrcData, m_Buf, 752);

		memcpy(m_Buf, temp, iLeftSize);
		m_iLen = iLeftSize;
	}
	else
	{
		memcpy(m_Buf + m_iLen, pSrcData, iSrcDdata);
		m_iLen += iSrcDdata;						
	}

	if (bLastBlock)
	{
		if (NULL == pDesData)
		{
			VOLOGE("empty pointor");
			return VO_OSMP_DRM_ERR_PARAMETER;
		}

		if (m_iLen % 16)
		{
			m_iLen = 0;
			VOLOGI("m_iLen %d", m_iLen);
			return VO_OSMP_DRM_ERR_DRMDATA;
		}

		VO_U32 uRet = decryptData(m_Buf, m_iLen, m_Buf, (VO_U32*)&m_iLen, VO_TRUE);
		if (0 != uRet) {
			m_iLen = 0;
			VOLOGE("!decryptData 0x%x", uRet);
			return uRet;
		}

		if (m_iLen > 0 && m_iLen <= 752)
		{
			memcpy(pSrcData + dec_size, m_Buf, m_iLen);
			dec_size += m_iLen ; 
		}
		else
		{
			VOLOGW("Size is %d", m_iLen);
		}	

		m_iLen = 0;
	}

	*piDesData = dec_size;

	return VO_OSMP_DRM_ERR_NONE;
}

int VOAES128_CBC::Close()
{
	m_identifer = 0xFFFFFFFF;

	return VO_OSMP_DRM_ERR_NONE;
}