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
#include "string.h"
#include "cmnMemory.h"
#include "CBaseCCRRR.h"

CBaseCCRRR::CBaseCCRRR(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: m_pMemOP (pMemOP)
	, m_hView (hView)
	, m_nInputWidth (0)
	, m_nInputHeight (0)
	, m_nInputColor (VO_COLOR_YUV_PLANAR420)
	, m_nOutputWidth (0)
	, m_nOutputHeight (0)
	, m_nOutoutColor (VO_COLOR_RGB565_PACKED)
	, m_nRotate (VO_RT_DISABLE)
	, m_fCallBack (NULL)
	, m_pUserData (NULL)
{
	if (m_pMemOP == NULL)
	{
		cmnMemFillPointer (VO_INDEX_SNK_AUDIO);
		m_pMemOP = &g_memOP;
	}

	m_prop.nRender = 0;
	m_prop.nOverlay = 0;
	m_prop.nKeyColor = 0;
	m_prop.nRotate = VO_RT_90L | VO_RT_90R | VO_RT_180;
	m_prop.nOutBuffer = 0;
	m_prop.nFlag = 0;

	strcpy ((VO_PCHAR) m_prop.szName, "voCCRRR");
}

CBaseCCRRR::~CBaseCCRRR ()
{
}

VO_U32 CBaseCCRRR::GetProperty (VO_CCRRR_PROPERTY * pProperty)
{
	if (pProperty == NULL)
		return VO_ERR_INVALID_ARG | VO_INDEX_SNK_CCRRR;

	memcpy (pProperty, &m_prop, sizeof (VO_CCRRR_PROPERTY));

	return VO_ERR_NONE;
}

VO_U32 CBaseCCRRR::GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CBaseCCRRR::GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CBaseCCRRR::SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor)
{
	m_nInputColor = nInputColor;
	m_nOutoutColor = nOutputColor;

	return VO_ERR_NONE;
}

VO_U32 CBaseCCRRR::SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate)
{
	m_nInputWidth = *pInWidth;
	m_nInputHeight = *pInHeight;

	m_nOutputWidth = *pOutWidth;
	m_nOutputHeight = *pOutHeight;
	m_nRotate = nRotate;

	return VO_ERR_NONE;
}

VO_U32 CBaseCCRRR::Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CBaseCCRRR::WaitDone (void)
{
	return VO_ERR_NONE;
}

VO_U32 CBaseCCRRR::SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	m_fCallBack = pCallBack;
	m_pUserData = pUserData;

	return VO_ERR_NONE;
}

VO_U32 CBaseCCRRR::GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	*ppVideoMemOP = NULL;

	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CBaseCCRRR::SetParam (VO_U32 nID, VO_PTR pValue)
{
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_CCRRR;
}

VO_U32 CBaseCCRRR::GetParam (VO_U32 nID, VO_PTR pValue)
{
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_CCRRR;
}

