	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoEffect.h

	Contains:	CVideoEffect header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-11-30		JBF			Create file

*******************************************************************************/

#ifndef __CVideoEffect_H__
#define __CVideoEffect_H__

#include "voVideo.h"
#include "CBaseNode.h"

typedef enum VO_VIDEO_EFFECTTYPE {
	VO_VE_Null		  = 0,
	VO_VE_CloseCaptionOn,				/**< the closeCaptionOn  */
	VO_VE_MAX		= 0X7FFFFFFF
} VO_VIDEO_EFFECTTYPE;


class CVideoEffect : public CBaseNode
{
public:
	CVideoEffect (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CVideoEffect (void);

	virtual VO_U32		Init (VO_VIDEO_FORMAT * pFormat);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		SetInputData (VO_VIDEO_BUFFER * pInput);
	virtual VO_U32		GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pVideoInfo, VO_PTR pUseData);
	virtual VO_U32		Flush (void);
	virtual VO_U32		Enable (VO_BOOL bEnable);
	virtual VO_U32		SetVideoEffect (VO_VIDEO_EFFECTTYPE ve_type);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	VO_HANDLE			m_hEffect;
	VO_HANDLE			m_funEffect;

	VO_VIDEO_BUFFER		*m_pInputBuffer;	
	VO_VIDEO_FORMAT		m_Format;
	VO_PBYTE			m_pUserBuffer;
	VO_U32				m_nUserLength;

	VO_VIDEO_EFFECTTYPE	m_nEffectType;
	VO_BOOL				m_bEffectOn;
};

#endif // __CVideoEffect_H__
