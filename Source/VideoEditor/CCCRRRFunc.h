	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CCCRRRFunc.h

	Contains:	CCCRRRFunc header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CCCRRRFunc_H__
#define __CCCRRRFunc_H__

#include "voCCRRR.h"
#include "CDllLoad.h"
#include "CBaseConfig.h"

class CCCRRRFunc : public CDllLoad
{
public:
	// Used to control the image drawing
	CCCRRRFunc (VO_BOOL bSoft, CBaseConfig * pConfig);
	virtual ~CCCRRRFunc (void);

	VO_U32 	Init (VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag);
	VO_U32 	GetProperty (VO_CCRRR_PROPERTY * pProperty);
	VO_U32 	GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	VO_U32 	GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	VO_U32 	SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor);
	VO_U32 	SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate);
	VO_U32 	Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait);
	VO_U32 	WaitDone (void);
	VO_U32 	SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
	VO_U32 	GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);
	VO_U32 	SetParam (VO_U32 nID, VO_PTR pValue);
	VO_U32 	GetParam (VO_U32 nID, VO_PTR pValue);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);
	VO_VIDEO_CCRRRAPI *	GetAPI (void) {return &m_ccrrFunc;}
	VO_CCRRR_PROPERTY *	GetProp (void) {return &m_prop;}

protected:
	CBaseConfig *		m_pConfig;
	VO_BOOL				m_bSoft;

	VO_VIDEO_CCRRRAPI	m_ccrrFunc;
	VO_HANDLE			m_hCCRR;
	VO_CCRRR_PROPERTY	m_prop;
};

#endif // __CCCRRRFunc_H__
