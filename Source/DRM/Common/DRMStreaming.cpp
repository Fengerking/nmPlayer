#include "DRMStreaming.h"
#include "CSourceIOUtility.h"
#include "base64.h"
#include "voToolUtility.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

// For UTF-16 convert to UTF-8
#if !defined __VO_UTF8_H__
#define __VO_UTF8_H__
#define BYTE_1_REP          0x80
#define BYTE_2_REP          0x800
#define BYTE_3_REP          0x10000
#define BYTE_4_REP          0x200000
#endif

#define VO_PIFF_OVERRIDE_TRACKENCRYPTIONBOX									0X000001
#define VO_PIFF_USE_SUBSAMPLE_ENCRYPTION									0X000002

CDRMStreaming::CDRMStreaming(void)
: m_bEncrypted(VO_FALSE)
{
}

CDRMStreaming::~CDRMStreaming(void)
{
}

VO_U32 CDRMStreaming::getKeyURI_HLS(VO_CHAR* pCurURL, VO_CHAR* pKeyString, VO_CHAR* pKeyURI, VO_PTR pReserved)
{
	m_bEncrypted = VO_FALSE;

	if (NULL == pKeyString || strlen(pKeyString) == 0)
	{
		return VO_ERR_DRM2_OK;
	}

	VO_CHAR * p = strstr(pKeyString, "METHOD=");
	if (NULL == p) {
		VOLOGE("NO METHOD=");
		return VO_ERR_DRM2_BADPARAMETER;
	}

	p += strlen("METHOD=");
	if (memcmp(p, "NONE", strlen("NONE") ) == 0)
	{
		return VO_ERR_DRM2_OK;
	}
	else if (memcmp(p, "AES-128", strlen("AES-128") ) != 0) {
		VOLOGE("NOT AES-128");
		return VO_ERR_DRM2_BADPARAMETER;
	}

	m_bEncrypted = VO_TRUE;

	p = strstr(pKeyString , "URI=");
	if (NULL == p) {
		VOLOGE("NO URI=");
		return VO_ERR_DRM2_BADPARAMETER;
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

	return VO_ERR_DRM2_OK;
}

VO_U32 CDRMStreaming::getIV_HLS(VO_CHAR* pKeyString, VO_U32 uSequenceNum, VO_BYTE* pIV, VO_PTR pReserved)
{
	if (NULL == pKeyString || NULL == pIV)
	{
		VOLOGE("empty pointor");
		return VO_ERR_DRM2_BADPARAMETER;
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

	return VO_ERR_DRM2_OK;
}


VO_U32 CDRMStreaming::getProtectionHeader_SSTR(VO_CHAR *pManifest, VO_U32 uManifest, VO_BYTE **ppDrmHeader, VO_U32 *puDrmHeader, VO_BOOL bDecodeBase64)
{
	if (NULL == pManifest || NULL == ppDrmHeader)
	{
		VOLOGE("empty pointor");
		return VO_ERR_DRM2_BADPARAMETER;
	}

	VO_CHAR	*pTemp	= pManifest;
	VO_U32	uTemp	= uManifest;

	VO_BOOL bNeedDel = VO_FALSE;

	m_bEncrypted = VO_FALSE;

	if (0xff == (VO_BYTE)pManifest[0] && 0xfe == (VO_BYTE)pManifest[1])
	{
		VOLOGR("convert characters code");

		bNeedDel = VO_TRUE;

#if defined _WIN32
		uTemp = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pManifest, -1, NULL, NULL, NULL, NULL);

		pTemp = new VO_CHAR[uTemp + 1];
		if (NULL == pTemp) {
			VOLOGE("can't get memory of %d", uTemp + 1);
			return VO_ERR_OUTOF_MEMORY;
		}

		memset(pTemp, 0, uTemp + 1);

		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pManifest, -1, pTemp, uTemp, NULL, NULL);
#else
		pTemp = new VO_CHAR[uTemp + 1];
		if (NULL == pTemp) {
			VOLOGE("can't get memory of %d", uTemp + 1);
			return VO_ERR_OUTOF_MEMORY;
		}

		memset(pTemp, 0, uTemp + 1);

		VO_UnicodeToUTF8( (VO_U16 *)pManifest, vowcslen( (VO_U16 *)pManifest ), pTemp, uTemp );
#endif
	}

	VO_CHAR *pStrart = strstr(pTemp, "<ProtectionHeader");
	if (pStrart)
	{
		pStrart = strstr(pStrart, "SystemID=\"");
		if (pStrart)
		{
			pStrart += strlen("SystemID=\"");
			while (*pStrart < '0' || *pStrart > '9')
				pStrart++;

#if defined _WIN32
			if (_strnicmp(pStrart, "9a04f079-9840-4286-ab92-e65be0885f95", strlen("9a04f079-9840-4286-ab92-e65be0885f95") ) == 0)
#else
			if (strncasecmp(pStrart, "9a04f079-9840-4286-ab92-e65be0885f95", strlen("9a04f079-9840-4286-ab92-e65be0885f95") ) == 0)
#endif
			{
				pStrart += strlen("9a04f079-9840-4286-ab92-e65be0885f95");
				while (*pStrart != '>')
					pStrart++;
				pStrart++;

				VO_CHAR *pEnd = strstr(pStrart, "</ProtectionHeader");
				if (pEnd)
				{
					*puDrmHeader = pEnd - pStrart;
				}
			}
		}
	}

	if (0 == *puDrmHeader) {
		VOLOGW("NO ProtectionHeader");

		if (bNeedDel && pTemp)
			delete []pTemp;

		return VO_ERR_DRM2_OK;
	}

	m_bEncrypted = VO_TRUE;

	*ppDrmHeader = new VO_BYTE[*puDrmHeader + 1];
	if (NULL == *ppDrmHeader) {
		VOLOGE("can't get memory of %d", *puDrmHeader + 1);
		return VO_ERR_OUTOF_MEMORY;
	}

	memset(*ppDrmHeader, 0, *puDrmHeader + 1);
	memcpy(*ppDrmHeader, pStrart, *puDrmHeader);

	if (bNeedDel && pTemp)
		delete []pTemp;

	if (bDecodeBase64)
	{
		VO_U32 uSize = *puDrmHeader;
		unsigned char *decoder = Base64Decode( (char*)*ppDrmHeader, uSize, VO_FALSE);

		delete []*ppDrmHeader;
		*ppDrmHeader = NULL;

		*ppDrmHeader = decoder;
		*puDrmHeader = uSize;
	}

	return VO_ERR_DRM2_OK;
}

VO_U32 CDRMStreaming::getIV_SSTR(VO_BYTE *pSampleEncryptionBox, VO_U32 uSampleEncryptionBox, VO_U32 uIndexSample, PIFF_SAMPLE_ENCRYPTION_INFO *pEncryptionInfo)
{
	if (uSampleEncryptionBox < 24 || NULL == pSampleEncryptionBox || NULL == pEncryptionInfo)
		return VO_ERR_DRM2_BADPARAMETER;

	VO_BYTE	*pTemp	= pSampleEncryptionBox + 4 + 4 + 16; //int size; "uuid"; GUID = {0xA2394F52-5A9B-4f14-A244-6C427C648DF4}
	VO_U32	uTemp	= uSampleEncryptionBox - 4 - 4 - 16;

	const unsigned char uuidPlayReady[] = {0xA2, 0x39, 0x4F, 0x52, 0x5A, 0x9B, 0x4F, 0x14, 0xA2, 0x44, 0x6C, 0x42, 0x7C, 0x64, 0x8D, 0xF4};
	if (memcmp(pSampleEncryptionBox + 8, uuidPlayReady, 16) != 0)
		return VO_ERR_DRM2_BADPARAMETER;

	VO_U32 uVersion_Flags = 0;
	if (read_SSTR(pTemp, uTemp, &uVersion_Flags, 4) != 0)
		return VO_ERR_DRM2_BADPARAMETER;
	pTemp += 4;
	uTemp -= 4;

	int uFlags = uVersion_Flags & 0x3f;
	if (uFlags & VO_PIFF_OVERRIDE_TRACKENCRYPTIONBOX)
	{
		VO_U32 uAlgorithmID = 0;
		if (read_SSTR(pTemp, uTemp, &uAlgorithmID, 3) != 0)
			return VO_ERR_DRM2_BADPARAMETER;
		pTemp += 3;
		uTemp -= 3;

		memcpy(&pEncryptionInfo->uSizeIV, pTemp, 1);
		pTemp++;
		uTemp--;

		unsigned char KID[16] = {0};

		memcpy(KID, pTemp, 16);
		pTemp += 16;
		uTemp -= 16;
	}

	if(0 == pEncryptionInfo->uSizeIV)
		pEncryptionInfo->uSizeIV = 8;

	VO_U32 uSampleCount = 0;
	if (read_SSTR(pTemp, uTemp, &uSampleCount, 4) != 0)
		return VO_ERR_DRM2_BADPARAMETER;
	pTemp += 4;
	uTemp -= 4;

	for (VO_U32 i = 0; i < uSampleCount; i++)
	{
		if (i == uIndexSample)
		{
			/*get IV */
			if (pEncryptionInfo->pIV)
				delete []pEncryptionInfo->pIV;

			pEncryptionInfo->pIV = new VO_BYTE[pEncryptionInfo->uSizeIV + 1];
			if (NULL == pEncryptionInfo->pIV) {
				VOLOGE("can't get memory of %d", pEncryptionInfo->uSizeIV + 1);
				return VO_ERR_OUTOF_MEMORY;
			}

			memset(pEncryptionInfo->pIV, 0, pEncryptionInfo->uSizeIV + 1);

			memcpy(pEncryptionInfo->pIV, pTemp, pEncryptionInfo->uSizeIV);

			VOLOGR("IV size: %d; 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", pEncryptionInfo->uSizeIV,
				pEncryptionInfo->pIV[0], pEncryptionInfo->pIV[1], pEncryptionInfo->pIV[2], pEncryptionInfo->pIV[3], pEncryptionInfo->pIV[4], pEncryptionInfo->pIV[5], pEncryptionInfo->pIV[6], pEncryptionInfo->pIV[7]);
		}

		pTemp += pEncryptionInfo->uSizeIV;
		uTemp -= pEncryptionInfo->uSizeIV;

		/* */
		if (uFlags & VO_PIFF_USE_SUBSAMPLE_ENCRYPTION)
		{
			VO_U32 uNumberOfEntries = 0;
			if (read_SSTR(pTemp, uTemp, &uNumberOfEntries, 2) != 0)
				return VO_ERR_DRM2_BADPARAMETER;
			pTemp += 2;
			uTemp -= 2;

			if (i != uIndexSample)
			{
				pTemp += uNumberOfEntries * (2 + 4);
				uTemp -= uNumberOfEntries * (2 + 4);
			}
			else
			{
				pEncryptionInfo->hNumberOfEntries = uNumberOfEntries;
				VOLOGR("\tEntries size: %d", pEncryptionInfo->hNumberOfEntries);

				if (pEncryptionInfo->pEntries)
					delete []pEncryptionInfo->pEntries;

				pEncryptionInfo->pEntries = new PIFF_SUB_ENCRYPTION_ENTRIES[uNumberOfEntries];
				if (NULL == pEncryptionInfo->pEntries) {
					VOLOGE("can't get memory of %d", uNumberOfEntries);
					return VO_ERR_OUTOF_MEMORY;
				}

				memset(pEncryptionInfo->pEntries, 0, sizeof(PIFF_SUB_ENCRYPTION_ENTRIES) * uNumberOfEntries);

				for (VO_U32 j = 0; j < uNumberOfEntries; j++)
				{
					if (read_SSTR(pTemp, uTemp, &pEncryptionInfo->pEntries[j].hBytesOfClearData, 2) != 0)
						return VO_ERR_DRM2_BADPARAMETER;
					pTemp += 2;
					uTemp -= 2;

					if (read_SSTR(pTemp, uTemp, &pEncryptionInfo->pEntries[j].uBytesOfEncrytedData, 4) != 0)
						return VO_ERR_DRM2_BADPARAMETER;
					pTemp += 4;
					uTemp -= 4;

					VOLOGR("hBytesOfClearData %d, uBytesOfEncrytedData %d", pEncryptionInfo->pEntries[j].hBytesOfClearData, pEncryptionInfo->pEntries[j].uBytesOfEncrytedData);
				}
			}
		}

		if (i == uIndexSample)
			return VO_ERR_DRM2_OK;
	}

	return VO_ERR_DRM2_BADDRMINFO;
}

VO_VOID CDRMStreaming::str2IV_HLS(VO_PBYTE ptr_key , VO_CHAR * pStr)
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

VO_U32 CDRMStreaming::read_SSTR(VO_BYTE* pInput, VO_U32 uInput, VO_VOID* pOutput, VO_U32 uNeedRead)
{
	if (uInput < uNeedRead)
	{
		VOLOGE("No Enough Data");
		return VO_ERR_DRM2_BADDRMINFO;
	}

	VO_BYTE *p = (VO_BYTE *)pOutput;
	for (VO_U32 i = 0; i < uNeedRead; i++)
		memcpy(p + i, pInput + uNeedRead - 1 - i, 1);

	return VO_ERR_DRM2_OK;
}