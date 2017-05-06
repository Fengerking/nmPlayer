#pragma once

#include "CBaseDRM.h"
#include "voDivXDRM.h"

#ifdef _DIVX_DRM_HT    
	// Resolution limitation : width <= 1280, height <= 720
	#define VO_MAX_PROTECTED_WIDTH  (1281)
	#define VO_MAX_PROTECTED_HEIGHT (721)
#else 
	// Resolution limitation : width < 640, height < 480
	#define VO_MAX_PROTECTED_WIDTH  (640)
	#define VO_MAX_PROTECTED_HEIGHT (480)
#endif

class CDivXDRM : public CBaseDRM
{
public:
	CDivXDRM();
	virtual ~CDivXDRM();

public:
	virtual VO_U32		Open();
	virtual VO_U32		Close();
	virtual VO_U32		SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		SetDrmInfo(VO_U32 nFlag, VO_PTR pDrmInfo);
	virtual VO_U32		GetDrmFormat(VO_DRM_FORMAT* pDrmFormat);
	virtual VO_U32 		CheckCopyRightResolution(VO_U32 nWidth, VO_U32 nHeight);
	virtual VO_U32		Commit();
	virtual VO_U32		DecryptData(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pData, VO_U32 nSize);
	virtual VO_U32		DecryptData2(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE* ppDstData, VO_U32* pnDstSize);

protected:
	VO_U32				doOpen();

	VO_PBYTE		m_pDrmContext;
	VO_PBYTE		m_pDecryptContext;

	VO_DRM_MODE		m_nMode;
	VO_U8			m_btUseLimit;
	VO_U8			m_btUseCount;
	
	VO_U8			m_btCgmsa;
	VO_U8			m_btAcptb;
	VO_U8			m_btDigitalProtection;

	VO_U32			m_dwRandomSampleCount;

	VO_U32			m_iDRMMode;	//0: 1.5; 1: JIT
};
