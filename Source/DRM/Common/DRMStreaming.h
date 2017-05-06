#ifndef __CDRM_HLS_H__
#define __CDRM_HLS_H__
#include "DRM.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

	struct PIFF_SUB_ENCRYPTION_ENTRIES {
		VO_U16	hBytesOfClearData; // the number of bytes of clear data at the beginning of this sub sample encryption entry
		VO_U32	uBytesOfEncrytedData; // the number of bytes of encrypted data following the clear data
	};

	struct PIFF_SAMPLE_ENCRYPTION_INFO
	{
		PIFF_SAMPLE_ENCRYPTION_INFO() 
			: uAlgorithmID(0)
			, uSizeIV(0)
			, pIV(NULL)
			, hNumberOfEntries(0)
			, pEntries(NULL)
		{}

		~PIFF_SAMPLE_ENCRYPTION_INFO() {
			if (pIV)
				delete []pIV;

			if (pEntries)
				delete []pEntries;
		}

		VO_U32		uAlgorithmID; // 3 Bytes. 0x0 Not encrypted; 0x1 AES 128-bit in CTR; 0x2 AES 128-bit in CBC;
		VO_U32		uSizeIV; // 1 Byte
		VO_BYTE		KID[16];

		//VO_U32	uSampleCount;

		VO_BYTE*	pIV; // InitializationVector;

		VO_U16							hNumberOfEntries;
		PIFF_SUB_ENCRYPTION_ENTRIES*	pEntries;
	};


class CDRMStreaming :
	public CDRM
{
public:
	CDRMStreaming(void);
	virtual ~CDRMStreaming(void);

protected:
	// HLS
	virtual VO_U32	getKeyURI_HLS(VO_CHAR* pCurURL, VO_CHAR* pKeyString, VO_CHAR* pKeyURI, VO_PTR pReserved);
	virtual VO_U32	getIV_HLS(VO_CHAR* pKeyString, VO_U32 uSequenceNum, VO_BYTE* pIV, VO_PTR pReserved);

	// SSTR
	virtual VO_U32	getProtectionHeader_SSTR(VO_CHAR *pManifest, VO_U32 uManifest, VO_BYTE **ppDrmHeader, VO_U32 *pnDrmHeader, VO_BOOL bDecodeBase64);
	virtual VO_U32	getIV_SSTR(VO_BYTE *pSampleEncryptionBox, VO_U32 uSampleEncryptionBox, VO_U32 uIndexSample, PIFF_SAMPLE_ENCRYPTION_INFO *pEncryptionInfo);


	VO_BOOL			IsEncrypted() { return m_bEncrypted; };

private:
	VO_VOID	str2IV_HLS(VO_PBYTE ptr_key, VO_CHAR * str_key);

	VO_U32	read_SSTR(VO_BYTE* pInput, VO_U32 uInput, VO_VOID* pOutput, VO_U32 uNeedRead);

	VO_BOOL m_bEncrypted;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CDRM_HLS_H__
