	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseCCRR.cpp

	Contains:	CBaseCCRR class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-07-08	JBF			Create file

*******************************************************************************/
#include "CBaseCCRR.h"

CBaseCCRR::CBaseCCRR(void)
	: m_nInWidth (0)
	, m_nInHeight (0)
	, m_nOutWidth (0)
	, m_nOutHeight (0)
	, m_nAngle (0)
	, m_nInYUVWidth (0)
	, m_nInYUVHeight (0)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
{
}

CBaseCCRR::~CBaseCCRR(void)
{
}

bool CBaseCCRR::SetVideoSize (int nInWidth, int nInHeight, int nVideoWidth, int nVideoHeight, int nOutWidth, int nOutHeight, int nAngle)
{
	m_nInWidth = nInWidth;
	m_nInHeight = nInHeight;

	m_nInYUVWidth = (nInWidth + 15) & ~0X0F;
	m_nInYUVHeight = (nInHeight + 15) & ~0X0F;

	m_nVideoWidth = (nVideoWidth + 15) & ~0X0F;
	m_nVideoHeight = (nVideoHeight + 15) & ~0X0F;

	m_nOutWidth = nOutWidth;
	m_nOutHeight = nOutHeight;

	m_nAngle = nAngle;

	return true;
}

bool CBaseCCRR::CCRRData (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB)
{
	return false;
}

