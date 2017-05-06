#ifndef __CDRM_VERIMATRIX_AES128_OTHER_H__
#define __CDRM_VERIMATRIX_AES128_OTHER_H__
#include "DRMStreaming.h"
#include "voCMutex.h"
#include "ViewRightWebClient3_4.h"
#include "VOOSMPDRM.h"
#include "CSourceIOUtility.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CDRM_Verimatrix_AES128
	:public VOOSMPDRM
{
public:
	CDRM_Verimatrix_AES128(void);
	~CDRM_Verimatrix_AES128(void);

	/*virtual*/
	int init()
	{
		m_bExit = VO_FALSE;
		return VO_OSMP_DRM_ERR_NONE;
	}
	/*virtual*/ int destroy()
	{
		m_bExit = VO_TRUE;
		return VO_OSMP_DRM_ERR_NONE;
	}

	//通常src和des是同一个buffer，若*piDes，请rerutn small并告知piDes; again
	/*virtual*/ int processURI(VO_OSMP_DRM_SOURCE_TYPE eType, const char* szSrcURI, char* pDesURI, int* piDesURI);//VO_OSMP_DRM_ERR_NONE
	/*virtual*/
	int processPlaylist(VO_OSMP_DRM_SOURCE_TYPE eType, const char* szSrcPlaylist, int iSrcPlaylist, char* pDesPlaylist, int* piDesPlaylist)
	{
		return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;        //VO_OSMP_DRM_ERR_NONE
	}
	/*virtual*/ int processHeader(VO_OSMP_DRM_SOURCE_TYPE eType, const unsigned char* pHeader, int iHeaderSize)
	{
		m_eType = eType;        //VO_OSMP_DRM_ERR_NONE
		return VO_OSMP_DRM_ERR_NONE;
	}

	/*virtual*/ int getHeaderLocation(long long* piPosition, int* piSize)
	{
		return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
	}
	/*virtual*/ int getDataLocation(long long* piPosition, int* piSize)
	{
		return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
	}
	/*virtual*/ int getDataFileSize(long long *piSize)
	{
		return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
	}

	/*virtual*/ int beginSegment(int identifer, void* pAdditionalInfo);
	/*virtual*/
	int decryptSegment(int identifer, long long llPosition, unsigned char* pData, int* piSize, bool bLastBlock, void* pAdditionalInfo);
	/*virtual*/
	int decryptSegment(int identifer, long long llPosition, unsigned char* pSrcData, int iSrcDdata, bool bLastBlock, unsigned char* pDesData, int* piDesData, void* pAdditionalInfo);
	/*virtual*/
	int endSegment(int identifer);

	/*virtual*/
	int decryptSample(int iPosition, unsigned char* pData, int* piSize, void* pAdditionalInfo)
	{
		return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
	}
	/*virtual*/ int decryptSample(int iPosition, unsigned char* pSrcData, int iSrcDdata, unsigned char* pDesData, int* piDesData, void* pAdditionalInfo)
	{
		return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
	}
	/*virtual*/ VO_U32	OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
	{
		return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
	}
	/*virtual*/ VO_U32	SetThirdpartyAPI(VO_PTR pParam)
	{
		return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
	}
	virtual VO_U32	SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32  GetParameter(VO_U32 uID, VO_PTR pParam);
	virtual int  GetUDID();

public:
	VO_OSMP_DRM_SOURCE_TYPE	m_eType;

	VO_BOOL				m_bExit;
	int					b_init;  // 0 no init; 1 initing,2 inited, 3 inited failed

	//VO_U32 doInit();

	ViewRightWebClient::VRWebClientError_t CheckDeviceVCASStatus();

	enum STATE_CONTENT
	{
		CONTENT_NORMAL,
		CONTENT_LINK_CHANGED,
		CONTENT_LINK_NEW_SEVER
	};

	struct Chunk_DRM_INFO
	{
		VO_U32	uIdentifer;
		VO_CHAR	szKey[2048];
		VO_BYTE	IV[16];
	};

	Chunk_DRM_INFO			m_DCI[3];

	ViewRightWebClient*		m_pVRWClient;

	STATE_CONTENT			m_eContent;
	VO_CHAR					m_szVCASBootAddress[64];
	VO_CHAR					m_szTempAddress[64];

	voCMutex				m_lock;
	voCMutex				m_lock_shake;

	VO_CHAR					m_szID[64];

	int						m_bINIT;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CDRM_VERIMATRIX_AES128_OTHER_H__
