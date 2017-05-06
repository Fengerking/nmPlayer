	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseCCRRR.h

	Contains:	CBaseCCRRR header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CBaseCCRRR_H__
#define __CBaseCCRRR_H__

#include "voCCRRR.h"
#include "CvoBaseObject.h"

class CBaseCCRRR : public CvoBaseObject
{
public:
	// Used to control the image drawing
	CBaseCCRRR (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CBaseCCRRR (void);

	virtual VO_U32 GetProperty (VO_CCRRR_PROPERTY * pProperty);
	virtual VO_U32 GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	virtual VO_U32 GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	virtual VO_U32 SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor);
	virtual VO_U32 SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate);
	virtual VO_U32 Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32 WaitDone (void);
	virtual VO_U32 SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
	virtual VO_U32 GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);
	virtual VO_U32 SetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 GetParam (VO_U32 nID, VO_PTR pValue);

protected:
	VO_CCRRR_PROPERTY	m_prop;
	VO_MEM_OPERATOR *	m_pMemOP;
	VO_PTR				m_hView;

	VO_U32				m_nInputWidth;
	VO_U32				m_nInputHeight;
	VO_IV_COLORTYPE		m_nInputColor;

	VO_U32				m_nOutputWidth;
	VO_U32				m_nOutputHeight;
	VO_IV_COLORTYPE		m_nOutoutColor;
	VO_IV_RTTYPE		m_nRotate;

	VOVIDEOCALLBACKPROC	m_fCallBack;
	VO_PTR				m_pUserData;
};

#endif // __CBaseCCRRR_H__