	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseCCRR.h

	Contains:	CBaseCCRR header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-07_08		JBF			Create file

*******************************************************************************/

#ifndef __CBaseCCRR_H__
#define __CBaseCCRR_H__	     

#include "voVideo.h"

class CBaseCCRR
{
public:
	CBaseCCRR(void);
	virtual ~CBaseCCRR(void);

	virtual bool				SetVideoSize (int nInWidth, int nInHeight, int nVideoWidth, int nVideoHeight, int nOutWidth, int nOutHeight, int nAngle);
	virtual bool				CCRRData (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB);

	virtual bool				SetBrightness (int nBrightness) {return false;}

	VO_IV_COLORTYPE	virtual		GetInType (void) {return m_vtIn;};
	VO_IV_COLORTYPE	virtual		GetOutType (void) {return m_vtOut;};


protected:
	VO_IV_COLORTYPE	m_vtIn;
	VO_IV_COLORTYPE	m_vtOut;
	int				m_nInWidth;
	int				m_nInHeight;
	int				m_nOutWidth;
	int				m_nOutHeight;
	int				m_nAngle;

	int				m_nInYUVWidth;
	int				m_nInYUVHeight;

	int				m_nVideoWidth;
	int				m_nVideoHeight;
};

#endif //__CBaseCCRR_H__