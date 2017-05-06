#include "DRMToolUtility.h"
#include "voToolUtility.h"
#include "voDRM2.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define VO_PIFF_OVERRIDE_TRACKENCRYPTIONBOX		0X000001
#define VO_PIFF_USE_SUBSAMPLE_ENCRYPTION		0X000002

VO_U32 memcpyr(VO_VOID* dst, const VO_VOID* src, VO_U32 count)
{
	src = (char *)src + count - 1;
	while (count--)
	{
		*(char *)dst = *(char *)src;
		dst = (char *)dst + 1;
		src = (char *)src - 1;
	}

	return VO_ERR_DRM2_OK;
}


VO_U32 parseProtectionSystemSpecificHeaderBox(const VO_BYTE* pProtectionSystemSpecificHeaderBox, VO_U32 uProtectionSystemSpecificHeaderBox, ProtectionSystemSpecificHeader* pHeader)
{
	if (uProtectionSystemSpecificHeaderBox < 0x20 || NULL == pProtectionSystemSpecificHeaderBox || NULL == pHeader) {
		VOLOGE("input data %d", uProtectionSystemSpecificHeaderBox);
		return VO_ERR_DRM2_BADPARAMETER;
	}

	VO_BYTE* p = const_cast<VO_BYTE*>(pProtectionSystemSpecificHeaderBox);

	if (memcmp("uuid", p + 4, 4) == 0)
	{
		const unsigned char uuidProtectionSystemSpecificHeader[] = {0xD0, 0x8A, 0x4F, 0x18, 0x10, 0xF3, 0x4A, 0x82, 0xB6, 0xC8, 0x32, 0xD8, 0xAB, 0xA1, 0x83, 0xD3};
		if ( memcmp(p + 8, uuidProtectionSystemSpecificHeader, 16) )
		{
			VOLOGE("Protection System Specific Header Box");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		p += 16;
	}
	else if (memcmp("pssh", p + 4, 4) != 0)
	{
		VOLOGE("not recgnize");
		return VO_ERR_DRM2_BADPARAMETER;
	}
	p += 12;

	memcpy(pHeader->uuidSystem, p, 16);
	p += 16;

	memcpyr(&pHeader->uSize, p, 4);
	p += 4;

	if (pHeader->uSize + 32 > uProtectionSystemSpecificHeaderBox)
		memcpy(&pHeader->uSize, p, 4);

	if (pHeader->uSize + 32 > uProtectionSystemSpecificHeaderBox)
	{
		VOLOGE("data size %d", pHeader->uSize);
		return VO_ERR_DRM2_BADPARAMETER;
	}

	pHeader->pData = p;

	return VO_ERR_DRM2_OK;
}

VO_U32 parseTrackEncryptionBox(const VO_BYTE *pTrackEncryptionBox, VO_U32 uTrackEncryptionBox, TrackEncryption *pEncryptionInfo)
{
	if (uTrackEncryptionBox < 0x20 || NULL == pTrackEncryptionBox || NULL == pEncryptionInfo)
		return VO_ERR_DRM2_BADPARAMETER;

	VO_BYTE* p = const_cast<VO_BYTE*>(pTrackEncryptionBox);

	if (memcmp("uuid", p + 4, 4) == 0)
	{
		//0x8974dbce-7be7-4c51-84f9-7148f9882554
		const unsigned char uuidTrackEncryption[] = {0x89, 0x74, 0xdb, 0xce, 0x7b, 0xe7, 0x4c, 0x51, 0x84, 0xf9, 0x71, 0x48, 0xf9, 0x88, 0x25, 0x54};
		if ( memcmp(p + 8, uuidTrackEncryption, 16) )
		{
			VOLOGE("Protection System Specific Header Box");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		p += 16;
	}
	else if (memcmp("tenc", p + 4, 4) != 0)
	{
		VOLOGE("not recgnize");
		return VO_ERR_DRM2_BADPARAMETER;
	}
	p += 12;

	memcpyr(&pEncryptionInfo->uAlgorithmID, p, 3);
	p += 3;

	memcpy(&pEncryptionInfo->uSizeIV, p, 1);
	p++;

	memcpy(pEncryptionInfo->KID, p, 16);

	return VO_ERR_DRM2_OK;
}

VO_U32 parseSampleEncryptionBox(const VO_BYTE *pSampleEncryptionBox, VO_U32 uSampleEncryptionBox, VO_U32 uIndexSample, SampleEncryption *pEncryptionInfo)
{
	if (uSampleEncryptionBox < 24  || NULL == pSampleEncryptionBox || NULL == pEncryptionInfo)
		return VO_ERR_DRM2_BADPARAMETER;

	VO_BYTE* p = const_cast<VO_BYTE*>(pSampleEncryptionBox);

	if (memcmp("uuid", p + 4, 4) == 0)
	{
		const unsigned char uuidSampleEncryption[] = {0xA2, 0x39, 0x4F, 0x52, 0x5A, 0x9B, 0x4F, 0x14, 0xA2, 0x44, 0x6C, 0x42, 0x7C, 0x64, 0x8D, 0xF4};
		if ( memcmp(p + 8, uuidSampleEncryption, 16) )
		{
			VOLOGE("Protection System Specific Header Box");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		p += 16;
	}
	else if (memcmp("senc", p + 4, 4) != 0)
	{
		VOLOGE("not recgnize");
		return VO_ERR_DRM2_BADPARAMETER;
	}
	p += 8;

	VO_U32 uVersion_Flags = 0;
	if (memcpyr(&uVersion_Flags, p, 4) != 0)
		return VO_ERR_DRM2_BADPARAMETER;
	p += 4;

	int uFlags = uVersion_Flags & 0x3f;
	if (uFlags & VO_PIFF_OVERRIDE_TRACKENCRYPTIONBOX)
	{
		if (memcpyr(&pEncryptionInfo->s_TrackEncryption.uAlgorithmID, p, 3) != 0)
			return VO_ERR_DRM2_BADPARAMETER;
		p += 3;

		memcpy(&pEncryptionInfo->s_TrackEncryption.uSizeIV, p, 1);
		p++;

		memcpy(pEncryptionInfo->s_TrackEncryption.KID, p, 16);
		p += 16;
	}

	if(0 == pEncryptionInfo->s_TrackEncryption.uSizeIV)
		pEncryptionInfo->s_TrackEncryption.uSizeIV = 8;

	VO_U32 uSampleCount = 0;
	if (memcpyr(&uSampleCount, p, 4) != 0)
		return VO_ERR_DRM2_BADPARAMETER;
	p += 4;

	for (VO_U32 i = 0; i < uSampleCount; i++)
	{
		if (i == uIndexSample)
		{
			/*get IV */
			memset(pEncryptionInfo->IV, 0, 16);
			memcpy(pEncryptionInfo->IV, p, pEncryptionInfo->s_TrackEncryption.uSizeIV);

			VOLOGR("IV size: %d; 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", pEncryptionInfo->s_TrackEncryption.uSizeIV,
				pEncryptionInfo->IV[0], pEncryptionInfo->IV[1], pEncryptionInfo->IV[2], pEncryptionInfo->IV[3], pEncryptionInfo->IV[4], pEncryptionInfo->IV[5], pEncryptionInfo->IV[6], pEncryptionInfo->IV[7]);
		}

		p += pEncryptionInfo->s_TrackEncryption.uSizeIV;

		/* */
		if (uFlags & VO_PIFF_USE_SUBSAMPLE_ENCRYPTION)
		{
			VO_U32 uNumberOfEntries = 0;
			if (memcpyr(&uNumberOfEntries, p, 2) != 0)
				return VO_ERR_DRM2_BADPARAMETER;
			p += 2;

			if (i != uIndexSample)
			{
				p += uNumberOfEntries * (2 + 4);
			}
			else
			{
				pEncryptionInfo->hNumberOfEntries = uNumberOfEntries;
				VOLOGR("\tEntries size: %d", pEncryptionInfo->hNumberOfEntries);

				if (pEncryptionInfo->pSubEntries)
					delete []pEncryptionInfo->pSubEntries;

				pEncryptionInfo->pSubEntries = new SampleEncryption::SubSampleEncryptionEntry[uNumberOfEntries];
				if (NULL == pEncryptionInfo->pSubEntries) {
					VOLOGE("can't get memory of %d", uNumberOfEntries);
					return VO_ERR_OUTOF_MEMORY;
				}

				memset(pEncryptionInfo->pSubEntries, 0, sizeof(SampleEncryption::SubSampleEncryptionEntry) * uNumberOfEntries);

				for (VO_U32 j = 0; j < uNumberOfEntries; j++)
				{
					if (memcpyr(&pEncryptionInfo->pSubEntries[j].hBytesOfClearData, p, 2) != 0)
						return VO_ERR_DRM2_BADPARAMETER;
					p += 2;

					if (memcpyr(&pEncryptionInfo->pSubEntries[j].uBytesOfEncrytedData, p, 4) != 0)
						return VO_ERR_DRM2_BADPARAMETER;
					p += 4;

					VOLOGR("hBytesOfClearData %hd, uBytesOfEncrytedData %d", pEncryptionInfo->pSubEntries[j].hBytesOfClearData, pEncryptionInfo->pSubEntries[j].uBytesOfEncrytedData);
				}
			}
		}

		if (i == uIndexSample)
			return VO_ERR_DRM2_OK;
	}

	return VO_ERR_DRM2_BADDRMINFO;
}

VO_U32 parseCencSampleAuxiliaryDataFormat(const VO_BYTE *pSampleEncryption, VO_U32 uSampleEncryption, SampleEncryption *pEncryptionInfo)
{
	if (uSampleEncryption < 8  || NULL == pSampleEncryption || NULL == pEncryptionInfo)
		return VO_ERR_DRM2_BADPARAMETER;

	VO_BYTE* p = const_cast<VO_BYTE*>(pSampleEncryption);

	memset(pEncryptionInfo->IV, 0, 16);
	memcpy(pEncryptionInfo->IV, p, pEncryptionInfo->s_TrackEncryption.uSizeIV);
	p += pEncryptionInfo->s_TrackEncryption.uSizeIV;

	VOLOGR("IV size: %d; 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", pEncryptionInfo->s_TrackEncryption.uSizeIV,
		pEncryptionInfo->IV[0], pEncryptionInfo->IV[1], pEncryptionInfo->IV[2], pEncryptionInfo->IV[3], pEncryptionInfo->IV[4], pEncryptionInfo->IV[5], pEncryptionInfo->IV[6], pEncryptionInfo->IV[7]);

	if (uSampleEncryption > pEncryptionInfo->s_TrackEncryption.uSizeIV)
	{
		VO_U32 uNumberOfEntries = 0;
		if (memcpyr(&uNumberOfEntries, p, 2) != 0)
			return VO_ERR_DRM2_BADPARAMETER;
		p += 2;

		pEncryptionInfo->hNumberOfEntries = uNumberOfEntries;
		VOLOGR("\tEntries size: %d", pEncryptionInfo->hNumberOfEntries);

		if (pEncryptionInfo->pSubEntries)
			delete []pEncryptionInfo->pSubEntries;

		pEncryptionInfo->pSubEntries = new SampleEncryption::SubSampleEncryptionEntry[uNumberOfEntries];
		if (NULL == pEncryptionInfo->pSubEntries) {
			VOLOGE("can't get memory of %d", uNumberOfEntries);
			return VO_ERR_OUTOF_MEMORY;
		}

		memset(pEncryptionInfo->pSubEntries, 0, sizeof(SampleEncryption::SubSampleEncryptionEntry) * uNumberOfEntries);

		for (VO_U32 j = 0; j < uNumberOfEntries; j++)
		{
			if (memcpyr(&pEncryptionInfo->pSubEntries[j].hBytesOfClearData, p, 2) != 0)
				return VO_ERR_DRM2_BADPARAMETER;
			p += 2;

			if (memcpyr(&pEncryptionInfo->pSubEntries[j].uBytesOfEncrytedData, p, 4) != 0)
				return VO_ERR_DRM2_BADPARAMETER;
			p += 4;

			VOLOGR("hBytesOfClearData %hd, uBytesOfEncrytedData %d", pEncryptionInfo->pSubEntries[j].hBytesOfClearData, pEncryptionInfo->pSubEntries[j].uBytesOfEncrytedData);
		}
	}

	return VO_ERR_DRM2_OK;
}

VO_U32 SSTR_findProtectionHeader(VO_CHAR *pManifest, VO_U32 uManifest, VO_BYTE **ppDRMHeader, VO_U32 *puDRMHeader)
{
	VO_CHAR *pStrart = strstr(pManifest, "<ProtectionHeader");
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
					*puDRMHeader = pEnd - pStrart;

					*ppDRMHeader = new VO_BYTE[*puDRMHeader + 1];
					if (NULL == *ppDRMHeader) {
						VOLOGE("can't get memory of %d", *ppDRMHeader + 1);
						return VO_ERR_OUTOF_MEMORY;
					}
					memset(*ppDRMHeader, 0, *puDRMHeader + 1);

					memcpy(*ppDRMHeader, pStrart, *puDRMHeader);

					return VO_ERR_DRM2_OK;
				}
			}
		}
	}

	return VO_ERR_DRM2_NOTENCRYPTION;
}

VO_U32 SSTR_getProtectionHeader(VO_CHAR *pManifest, VO_U32 uManifest, VO_BYTE **ppDRMHeader, VO_U32 *puDRMHeader)
{
	if (NULL == pManifest) {
		VOLOGE("empty pointor");
		return VO_ERR_DRM2_BADPARAMETER;
	}

	if (0xff == (VO_BYTE)pManifest[0] && 0xfe == (VO_BYTE)pManifest[1])
	{
		VOLOGR("convert characters code");

#if defined _WIN32
		VO_U32 uSize = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pManifest, -1, NULL, 0, NULL, NULL);

		VO_CHAR* pData = new VO_CHAR[uSize + 1];
		if (NULL == pData) {
			VOLOGE("can't get memory of %d", uSize + 1);
			return VO_ERR_OUTOF_MEMORY;
		}
		memset(pData, 0, uSize + 1);

		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pManifest, -1, pData, uSize, NULL, NULL);
#else
		VO_U32		uSize	= uManifest;
		VO_CHAR*	pData = new VO_CHAR[uSize + 1];
		if (NULL == pData) {
			VOLOGE("can't get memory of %d", uSize + 1);
			return VO_ERR_OUTOF_MEMORY;
		}
		memset(pData, 0, uSize + 1);

		VO_UnicodeToUTF8( (VO_U16 *)pManifest, vowcslen( (VO_U16 *)pManifest ), pData, uSize );
#endif

		VO_U32 uRet = SSTR_findProtectionHeader(pData, uSize, ppDRMHeader, puDRMHeader);

		delete []pData;

		return uRet;
	}

	return SSTR_findProtectionHeader(pManifest, uManifest, ppDRMHeader, puDRMHeader);;
}


VO_U32 combine(VO_BYTE *pData, VO_U32* puSize, const VO_BYTE* pAppend, VO_U32 uAppend) //the max size of buf = *puSize + 1024
{
	//if (2 * sizeof(VO_U32) + 2 + uAppend > 1024) {
	//	VOLOGE("output buf small %d", uAppend);
	//	return VO_ERR_DRM2_OUTPUT_BUF_SMALL;
	//}

	VO_BYTE* pTemp = pData + *puSize;

	memcpy( pTemp, puSize, sizeof(VO_U32) );
	pTemp += sizeof(VO_U32);
	memset(pData + *puSize + sizeof(VO_U32), 1, 1);//ID 1 = main data
	pTemp += 1;

	memcpy(pTemp, pAppend, uAppend);
	pTemp += uAppend;
	memcpy( pTemp, &uAppend, sizeof(VO_U32) );
	pTemp += sizeof(VO_U32);
	memset(pTemp, 2, 1);//ID 2 = append data
	pTemp += 1;

	*puSize += (2 * sizeof(VO_U32) + 2 + uAppend);

	return VO_ERR_DRM2_OK;
}

VO_U32 depart(VO_BYTE *pBuf, VO_U32 uBuf, VO_BYTE** ppData, VO_U32* puSize, VO_BYTE** ppAppend, VO_U32* puAppend)
{
	VO_BYTE* pTemp = pBuf + uBuf - 1;
	if (*pTemp != 2) {
		VOLOGE("Bad Data 0x%x", *pTemp);
		return VO_ERR_DRM2_BADDRMDATA;
	}

	pTemp -= sizeof(VO_U32);
	memcpy( puAppend, pTemp, sizeof(VO_U32) );
	*ppAppend = pTemp - *puAppend;

	pTemp = *ppAppend - 1;
	if (*pTemp != 1) {
		VOLOGE("Bad Data 0x%x", *pTemp);
		return VO_ERR_DRM2_BADDRMDATA;
	}

	pTemp -= sizeof(VO_U32);
	memcpy( puSize, pTemp, sizeof(VO_U32) );
	*ppData = pTemp - *puSize;

	if (*ppData != pBuf) {
		VOLOGE("Bad Data 0x%x", *pTemp);
		return VO_ERR_DRM2_BADDRMDATA;
	}

	return VO_ERR_DRM2_OK;
}

VO_U32 departWithSampleEncryption(VO_BYTE *pBuf, VO_U32 uBuf, VO_BYTE** ppData, VO_U32* puSize, SampleEncryption **ppEncryptionInfo)
{
	VO_BYTE* pAppend = NULL;
	VO_U32   uAppendLen = 0;
	VO_U32 uRet = depart(pBuf, uBuf, ppData, puSize, &pAppend, &uAppendLen);
	if (uRet) {
		VOLOGE("depart 0x%x", uRet);
		return uRet;
	}

	*ppEncryptionInfo = reinterpret_cast<SampleEncryption*>(pAppend);
	(*ppEncryptionInfo)->pSubEntries = (SampleEncryption::SubSampleEncryptionEntry*)((VO_BYTE*)(*ppEncryptionInfo) + sizeof(SampleEncryption));

	return VO_ERR_DRM2_OK;
}


#ifdef _VONAMESPACE
}
#endif //_VONAMESPACE