	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDrmEngine.h

	Contains:	CDrmEngine header file

	Written by:	East Zhou

	Change History (most recent first):
	2010-03-12		East		Create file

*******************************************************************************/

#ifndef __CDrmEngine_H__
#define __CDrmEngine_H__

#include "voYYDef_Common.h"
#include <voDRM.h>
#include "CDllLoad.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CDrmEngine : public CDllLoad
{
public:
	CDrmEngine(VO_DRM_TYPE nType);
	virtual ~CDrmEngine();

public:
	virtual VO_U32		Open();
	virtual VO_U32		Close();
	virtual VO_U32		SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		SetDrmInfo(VO_U32 nFlag, VO_PTR pDrmInfo);
	virtual VO_U32		GetDrmFormat(VO_DRM_FORMAT* pDrmFormat);
	virtual VO_U32		CheckCopyRightResolution(VO_U32 nWidth, VO_U32 nHeight);
	virtual VO_U32		Commit();
	virtual VO_U32		DecryptData(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pData, VO_U32 nSize);
	virtual VO_U32		DecryptData2(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE* ppDstData, VO_U32* pnDstSize);
	virtual VO_DRM_TYPE GetDrmType() { return m_nType; }

	//add by MaTao at 2011.4.13
	virtual void		SetDrmAPI(VO_DRM_API *drmApi) {mpDrmAPI = drmApi ;}
public:
	virtual VO_U32		LoadLib(VO_HANDLE hInst);

protected:
	VO_DRM_API			m_funDec;
	VO_DRM_API *		mpDrmAPI;

	VO_HANDLE			m_hDrm;
	VO_DRM_TYPE			m_nType;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CDrmEngine_H__
