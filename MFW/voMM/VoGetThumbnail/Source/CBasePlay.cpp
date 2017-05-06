	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBasePlay.cpp

	Contains:	CBasePlay class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-31		JBF			Create file

*******************************************************************************/
#include "cmnMemory.h"
#include "CBaseConfig.h"

#include "CBasePlay.h"

CBasePlay::CBasePlay(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP)
	: m_hInst (hInst)
	, m_pMemOP (pMemOP)
	, m_pFileOP (pFileOP)
	, m_pCallBack (NULL)
	, m_pUserData (NULL)
	, m_hView (NULL)
	, m_pConfig (NULL)
{
	g_fileOP.Open = NULL;

	if (m_pMemOP == NULL)
	{
		cmnMemFillPointer (VO_INDEX_MFW_VOMMPLAY);
		m_pMemOP = &g_memOP;
	}

	if (m_pFileOP == NULL)
	{
		if (g_fileOP.Open == NULL)
			cmnFileFillPointer ();
		m_pFileOP = &g_fileOP;
	}

	m_rcView.left = 0;
	m_rcView.top = 0;
	m_rcView.right = 0;
	m_rcView.bottom = 0;
}

CBasePlay::~CBasePlay ()
{
	if (m_pConfig != NULL)
		delete m_pConfig;
	m_pConfig = NULL;
}

VO_U32 CBasePlay::SetCallBack (VOMMPlayCallBack pCallBack, VO_PTR pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;

	return VO_ERR_NONE;
}

VO_U32 CBasePlay::SetViewInfo (VO_PTR hView, VO_RECT * pRect)
{
	m_hView = hView;

	if (pRect != NULL)
	{
		m_rcView.left = pRect->left;
		m_rcView.top = pRect->top;
		m_rcView.right = pRect->right;
		m_rcView.bottom = pRect->bottom;
	}

	return VO_ERR_NONE;
}

VO_U32 CBasePlay::Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBasePlay::Run (void)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBasePlay::Pause (void)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBasePlay::Stop (void)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBasePlay::GetDuration (VO_U32 * pDuration)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBasePlay::GetCurPos (VO_S32 * pPos)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBasePlay::SetCurPos (VO_S32 nPos)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBasePlay::SetParam (VO_U32 nID, VO_PTR pValue)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBasePlay::GetParam (VO_U32 nID, VO_PTR pValue)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBasePlay::SetConfig (CBaseConfig * pConfig)
{
	m_pConfig = pConfig;
	return 0;
}
