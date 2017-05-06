	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXConfigCore.cpp

	Contains:	voCOMXConfigCore class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "voCOMXConfigCore.h"

voCOMXConfigCore::voCOMXConfigCore(void)
	: voCOMXBaseConfig ()
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXConfigCore::~voCOMXConfigCore(void)
{
}

OMX_STRING voCOMXConfigCore::GetCompName (OMX_U32 nIndex)
{
	if (nIndex >= m_nSectNum)
		return NULL;

	COMXCfgSect *		pSect = m_pFirstSect;
	while (nIndex > 0)
	{
		pSect = pSect->m_pNext;
		nIndex--;

		if (pSect == NULL)
			return NULL;
	}

	if (pSect == NULL)
		return NULL;

	return pSect->m_pName;
}

OMX_STRING voCOMXConfigCore::GetCompFile (OMX_STRING pCompName)
{
	COMXCfgItem * pItem = FindItem (pCompName, "File");
	if (pItem == NULL)
		return NULL;

	return pItem->m_pValue;
}

OMX_STRING voCOMXConfigCore::GetCompAPI (OMX_STRING pCompName)
{
	COMXCfgItem * pItem = FindItem (pCompName, "API");
	if (pItem == NULL)
		return NULL;

	return pItem->m_pValue;
}

OMX_U32 voCOMXConfigCore::GetCompPriority (OMX_STRING pCompName)
{
	COMXCfgItem * pItem = FindItem (pCompName, "GroupPriority");
	if (pItem == NULL)
		return 0;

	return pItem->m_nValue;
}

OMX_U32 voCOMXConfigCore::GetCompGroupID (OMX_STRING pCompName)
{
	COMXCfgItem * pItem = FindItem (pCompName, "GroupID");
	if (pItem == NULL)
		return 0;

	return pItem->m_nValue;
}
