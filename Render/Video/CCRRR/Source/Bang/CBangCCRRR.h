	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBangCCRRR.h

	Contains:	CBangCCRRR header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CBangCCRRR_H__
#define __CBangCCRRR_H__

#include "../CBaseCCRRR.h"

#include "CCCRRMng.h"

class CBangCCRRR : public CBaseCCRRR
{
public:
	// Used to control the image drawing
	CBangCCRRR (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	~CBangCCRRR (void);

	VO_U32 GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	VO_U32 GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);

	VO_U32 SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor);
	VO_U32 SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate);

	VO_U32 Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait);

protected:
	CCCRRMng		m_ccrr;
};

#endif // __CBangCCRRR_H__
