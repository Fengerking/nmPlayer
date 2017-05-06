	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CCCRRMng.cpp

	Contains:	CCCRRMng class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-07-08	JBF			Create file

*******************************************************************************/
#include "CCCRRMng.h"

#include "CRGB565RR.h"
#include "CYUV420RGB565MB.h"

CCCRRMng::CCCRRMng(void)
	: m_pCCRR (NULL)
	, m_bResized (true)
{
}

CCCRRMng::~CCCRRMng(void)
{
	if (m_pCCRR != NULL)
	{
		delete m_pCCRR;
		m_pCCRR = NULL;
	}
}

bool CCCRRMng::SetVideoType (VO_IV_COLORTYPE inType, VO_IV_COLORTYPE outType)
{
	if (m_pCCRR != NULL)
		delete m_pCCRR;

	if(inType == VO_COLOR_RGB565_PACKED && outType == VO_COLOR_RGB565_PACKED)
		m_pCCRR = new CRGB565RR;
	else if(inType == VO_COLOR_YUV_PLANAR420 && outType == VO_COLOR_RGB565_PACKED)
		m_pCCRR = new CYUV420RGB565MB;
	else
		return false;

	if(m_pCCRR == NULL)
		return false;

	return true;
}

bool CCCRRMng::SetVideoSize (int nInWidth, int nInHeight, int nVideoWidth, int nVideoHeight, 
							 int nOutWidth, int nOutHeight, int nAngle)
{
	if(m_pCCRR == NULL)
		return false;

	m_pCCRR->SetVideoSize (nInWidth, nInHeight, nVideoWidth, nVideoHeight, nOutWidth, nOutHeight, nAngle);

	return true;
}

bool CCCRRMng::CCRRData (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB)
{
	if (m_pCCRR == NULL)
		return false;

	if (m_bResized)
		m_pCCRR->CCRRData (inData, outData, NULL);
	else
		m_pCCRR->CCRRData (inData, outData, pMB);

	m_bResized = false;

	return true;
}

bool CCCRRMng::SetBrightness (int nBrightness)
{
	if (m_pCCRR == NULL)
		return false;
	
	return m_pCCRR->SetBrightness (nBrightness);
}
