	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CCCRRMng.h

	Contains:	CCCRRMng header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-07_08		JBF			Create file

*******************************************************************************/

#ifndef __CCCRRMng_H__
#define __CCCRRMng_H__

#include "CBaseCCRR.h"

class CCCRRMng
{
public:
	CCCRRMng(void);
	~CCCRRMng(void);

public:
	bool	SetVideoType (VO_IV_COLORTYPE inType, VO_IV_COLORTYPE outType);

	bool	SetVideoSize (int nInWidth, int nInHeight, int nVideoWidth, int nVideoHeight, 
						  int nOutWidth, int nOutHeight, int nAngle);

	bool	CCRRData (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB = NULL);

	bool	SetBrightness (int nBrightness);

private:
	CBaseCCRR *			m_pCCRR;
	bool				m_bResized;

};
#endif //__CCCRRMng_H__