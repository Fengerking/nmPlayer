#pragma once
//#include "voToolUtility.h"
#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif


	struct ProtectionSystemSpecificHeader
	{
		VO_BYTE		uuidSystem[16];

		VO_U32		uSize;
		VO_BYTE*	pData;
	};

	struct TrackEncryption
	{
		VO_U32	uAlgorithmID;
		VO_U32	uSizeIV;
		VO_BYTE	KID[16];
	};

	struct SampleEncryption
	{
		TrackEncryption				s_TrackEncryption;

		VO_BYTE						IV[16];

		struct SubSampleEncryptionEntry {
			VO_U16	hBytesOfClearData;		// the number of bytes of clear data at the beginning of this sub sample encryption entry
			VO_U32	uBytesOfEncrytedData;	// the number of bytes of encrypted data following the clear data
		};

		VO_U16						hNumberOfEntries;
		SubSampleEncryptionEntry*	pSubEntries;
	};

	VO_U32 SSTR_getProtectionHeader(VO_CHAR *pManifest, VO_U32 uManifest, VO_BYTE **ppDRMHeader, VO_U32 *puDRMHeader);

	VO_U32 parseProtectionSystemSpecificHeaderBox(const VO_BYTE* pProtectionSystemSpecificHeaderBox, VO_U32 uProtectionSystemSpecificHeaderBox, ProtectionSystemSpecificHeader* pHeader);
	VO_U32 parseTrackEncryptionBox(const VO_BYTE *pTrackEncryptionBox, VO_U32 uTrackEncryptionBox, TrackEncryption *pEncryptionInfo);
	VO_U32 parseSampleEncryptionBox(const VO_BYTE *pSampleEncryptionBox, VO_U32 uSampleEncryptionBox, VO_U32 uIndexSample, SampleEncryption *pEncryptionInfo);

	VO_U32 parseCencSampleAuxiliaryDataFormat(const VO_BYTE *pSampleEncryption, VO_U32 uSampleEncryption, SampleEncryption *pEncryptionInfo);

	VO_U32 combine(VO_BYTE *pData, VO_U32* puSize, const VO_BYTE* pAppend, VO_U32 uAppend);
	VO_U32 depart(VO_BYTE *pBuf, VO_U32 uBuf, VO_BYTE** ppData, VO_U32* puSize, VO_BYTE** ppAppend, VO_U32* puAppend);
	VO_U32 departWithSampleEncryption(VO_BYTE *pBuf, VO_U32 uBuf, VO_BYTE** ppData, VO_U32* puSize, SampleEncryption **ppEncryptionInfo);


#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */