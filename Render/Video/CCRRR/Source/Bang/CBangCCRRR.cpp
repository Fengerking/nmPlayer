	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include <string.h>
#include "CBangCCRRR.h"

CBangCCRRR::CBangCCRRR(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseCCRRR (hInst, hView, pMemOP)
{
	strcpy ((VO_PCHAR) m_prop.szName, "Bang");
}

CBangCCRRR::~CBangCCRRR ()
{
}

VO_U32 CBangCCRRR::GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	if (nIndex == 0)
		*pColorType = VO_COLOR_YUV_PLANAR420;
	else if (nIndex == 1)
		*pColorType = VO_COLOR_RGB565_PACKED;

	if (nIndex >= 0 && nIndex < 2)
		return VO_ERR_NONE;
	else
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CBangCCRRR::GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	if (nIndex == 0)
		*pColorType = VO_COLOR_RGB565_PACKED;

	if (nIndex == 0)
		return VO_ERR_NONE;
	else
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CBangCCRRR::SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor)
{
	CBaseCCRRR::SetColorType (nInputColor, nOutputColor);

	m_ccrr.SetVideoType (nInputColor, nOutputColor);

	return VO_ERR_NONE;
}

VO_U32 CBangCCRRR::SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate)
{
	m_nInputWidth = *pInWidth;
	m_nInputHeight = *pInHeight;

	m_nOutputWidth = *pOutWidth;
	m_nOutputHeight = *pOutHeight;
	m_nRotate = nRotate;

	VO_U32 nAngle = 0;

	if (m_nRotate == VO_RT_DISABLE)
		nAngle = 0;
	else if (m_nRotate == VO_RT_90L)
		nAngle = 90;
	else if (m_nRotate == VO_RT_90R)
		nAngle = 270;
	else if (m_nRotate == VO_RT_180)
		nAngle = 180;

	m_ccrr.SetVideoSize (m_nInputWidth, m_nInputHeight, m_nInputWidth, m_nInputHeight, m_nOutputWidth, m_nOutputHeight, nAngle);

	return VO_ERR_NONE;
}

VO_U32 CBangCCRRR::Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	m_ccrr.CCRRData (pVideoBuffer, pOutputBuffer, NULL);

	return VO_ERR_NONE;
}

