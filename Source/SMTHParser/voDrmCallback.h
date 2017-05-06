
#ifndef __VO_DRM_CALLBACK_H__
#define __VO_DRM_CALLBACK_H__


//#define	_TEST

#include "CvoBaseDrmCallback.h"
#include "vo_http_stream.h"
#include "voPlayReady.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CvoDrmCallback :public CvoBaseDrmCallback
{

public:
	CvoDrmCallback(VO_PTR pDrmCB);
	virtual ~CvoDrmCallback();

public:
	//VO_ERR_DRM_OK mean supported, otherwise mean not supported
	//virtual VO_U32		DRMIsSupported(VO_DRM_TYPE nType);

	//virtual VO_U32		DRMInfo_DivX(VO_PBYTE pStrdChunk, VO_U32 nStrdChunk);
	//virtual VO_U32		DRMInfo_WM(VO_PBYTE pContentEncryptionObject, VO_U32 nContentEncryptionObject, VO_PBYTE pExtendedContentEncryptionObject, VO_U32 nExtendedContentEncryptionObject);
	virtual VO_U32		DRMInfo_PlayReady(VO_PBYTE *ppDrmHeader, VO_U32 &nDrmHeader, int isNeedConver = 1);

public:
	//virtual VO_U32		DRMData_DivX_Video(VO_PBYTE pData, VO_U32 nSize, VO_PBYTE pDdInfo);
	//virtual VO_U32		DRMData_DivX_Audio(VO_PBYTE pData, VO_U32 nSize);
	//virtual VO_U32		DRMData_WM_PacketData(VO_PBYTE pData, VO_U32 nSize);
	// pAESCTRInfo(VO_PLAYREADY_AESCTR_INFO*)
	virtual VO_U32		DRMData_PlayRead_PacketData(VO_PBYTE pData, VO_U32 nSize, VO_PBYTE pInitializationVector,
																					VO_U32    nIVsize,VO_U32    nBlockOffset,VO_U8		btByteOffset);

	VO_U32	DRMData_PlayRead_PacketData_for_Discretix(VO_PBYTE pData, VO_U32 nSize, VO_PBYTE pSampleEncryptionBox,
										VO_U32    uSampleEncryptionBoxSize,VO_U32    uSampleIndex,VO_U32		uTrackId);
	VO_U32  DRMInfo_PlayReady_for_Discretix(VO_PBYTE pBuffer, VO_U32 nSize);


protected:
	VOSOURCEDRMCALLBACK		m_fCallback;
	VO_PTR					m_pUserData;
	
	static VO_U32 VO_API Get_License_Response(VO_PBYTE pChallenge, VO_U32 nChallenge, VO_PCHAR pcUrl, VO_U32 nUrl, VO_PCHAR pcHttpHeader, VO_U32 nHttpHeader, 
		VO_PCHAR* ppcMimetype, VO_PBYTE* ppResponse, VO_U32* pnResponse, VO_BOOL* pbCancel,VO_PTR pUseData);

private:
	VO_BYTE *m_pResponseBuf;
	VO_CHAR *m_pcMimetype;
	VO_U32 m_nResponseSize;
	VO_BOOL m_bMediaStop;
	vo_http_stream m_http;
};


#ifdef _VONAMESPACE
}
#endif

#endif
