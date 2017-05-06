	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseNode.cpp

	Contains:	CBaseNode class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-06-06		JBF			Create file

*******************************************************************************/
#include "CBaseNode.h"
#ifndef _LINUX
#pragma warning (disable : 4996)
#endif
CBaseNode::CBaseNode (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: m_hInst (hInst)
	, m_pMemOP (pMemOP)
	, m_pcbVOLOG (NULL)	
	, m_pConfig (NULL)
	, m_bVOUsed (VO_FALSE)
	, m_bForThumbnail (VO_FALSE)
	, m_bOMXComp (VO_FALSE)
	, m_pError (NULL)
	, m_nDumpLogLevel (0)
{
	strcpy (m_szCfgItem, "");
}

CBaseNode::~CBaseNode ()
{
}

VO_U32 CBaseNode::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	return VO_ERR_WRONG_PARAM_ID;
}

VO_U32 CBaseNode::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	return VO_ERR_WRONG_PARAM_ID;
}

VO_U32 CBaseNode::LoadLib (VO_HANDLE hInst)
{
	VO_U32 nRC = CDllLoad::LoadLib (m_hInst);
	if (nRC == 0 && m_pError != NULL)
	{
		if (m_hDll == NULL)
		{
			vostrcpy (m_pError, _T("It could not load the module "));
			vostrcat (m_pError, m_szDllFile);
		}
		else
		{
			vostrcpy (m_pError, _T("It could not find the API "));
			vostrcat (m_pError, m_szAPIName);
		}
	}

	return nRC;
}
