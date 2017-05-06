#ifndef __CADAPTIVESTREAMINGDRM_H__
#define __CADAPTIVESTREAMINGDRM_H__

#include "voDRM2.h"
#include "voAdaptiveStreamParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAdaptiveStreamingDRM
{
public:
	CAdaptiveStreamingDRM(VO_SOURCEDRM_CALLBACK2* pDrmCB, VO_SOURCE2_IO_API* pIO, VO_PTR pReserved);
	~CAdaptiveStreamingDRM(void);

	VO_U32 PreprocessURL(const VO_CHAR* urlSrc, VO_CHAR* urlDes, VO_PTR pReserved);

	VO_U32 Info(VO_CHAR* szManifestURL, VO_BYTE* pManifest, VO_U32 uSizeManifest, VO_PTR pReserved);

	VO_U32 DataBegin(VO_U32 uIdentifer, VO_PTR pInfo);

	VO_U32 DataProcess_Chunk(VO_U32 uIdentifer, VO_U64 nOffset, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_BOOL bChunkEnd, VO_PTR pAdditionalInfo);	//File Chunk DRM
	VO_U32 DataEnd(VO_U32 uIdentifer, VO_PTR pInfo);

	VO_U32 Info_FR(VO_PTR pInfo, VO_DRM2_INFO_TYPE eInfoType, VO_PTR pReserved);
	VO_U32 DataProcess_FR(VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_DRM2_DATATYPE eDataType, VO_DRM2_DATAINFO_TYPE eInfoType, VO_PTR pAdditionalInfo);		//Media Format DRM 

	VO_U32 SetParameter(VO_U32 uID, VO_PTR pParam);
	VO_U32 GetParameter(VO_U32 uID, VO_PTR pParam);

	VO_U32 PreprocessPlaylist(VO_BYTE* pPlaylist, VO_U32 uSizeBuffer, VO_U32* puSizePlaylist, VO_PTR pReserved);

private:
	VO_SOURCEDRM_CALLBACK2*	m_pDRMCB;

	VO_SOURCE2_IO_API*		m_pIO;

	VO_DRM2_SOURCE_FORMAT	m_eDRMSource;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CADAPTIVESTREAMINGDRM_H__