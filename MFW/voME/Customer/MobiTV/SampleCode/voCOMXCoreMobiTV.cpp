	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreMobiTV.cpp

	Contains:	voCOMXCoreMobiTV class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "voCOMXCoreMobiTV.h"
#include "voOMXOSFun.h"
#include "voOMXMemory.h"

#include "voComponentEntry.h"
#include "voCOMXRTSPSource.h"

#include "voLog.h"

voCOMXCoreMobiTV::voCOMXCoreMobiTV(void)
{
	VOLOGF ();

	strcpy (m_pObjName, __FILE__);

	OMX_U32 i = 0;
	for (i = 0; i < VOOMX_COMPONENT_NUM; i++)
		m_szCompName[i] = (OMX_STRING) voOMXMemAlloc (128);

	strcpy (m_szCompName[0], "OMX.MobiTV.RTSP.Source");

	m_nCompRoles[0] = 1;		// Audio Decoder

	for (i = 0; i < VOOMX_COMPONENT_NUM; i++)
	{
		m_szCompRoles[i] = (OMX_STRING *) voOMXMemAlloc (m_nCompRoles[i] * 4);
		for (OMX_U32 j = 0; j < m_nCompRoles[i]; j++)
		{
			m_szCompRoles[i][j] = (OMX_STRING) voOMXMemAlloc (128);
		}
	}

	strcpy (m_szCompRoles[0][0], "RTSP.Source");
}

voCOMXCoreMobiTV::~voCOMXCoreMobiTV(void)
{
	VOLOGF ();

	OMX_U32 i = 0;
	for (i = 0; i < VOOMX_COMPONENT_NUM; i++)
		voOMXMemFree (m_szCompName[i]);

	for (i = 0; i < VOOMX_COMPONENT_NUM; i++)
	{
		for (OMX_U32 j = 0; j < m_nCompRoles[i]; j++)
		{
			voOMXMemFree (m_szCompRoles[i][j]);
		}

		voOMXMemFree (m_szCompRoles[i]);
	}
}

OMX_ERRORTYPE voCOMXCoreMobiTV::Init (OMX_PTR hInst)
{
	VOLOGF ();

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	return errType;
}

OMX_STRING voCOMXCoreMobiTV::GetName (OMX_U32 nIndex)
{
	VOLOGF ();

	if (nIndex < 0 || nIndex >= VOOMX_COMPONENT_NUM)
		return NULL;

	return m_szCompName[nIndex];
}

OMX_ERRORTYPE voCOMXCoreMobiTV::LoadComponent (OMX_COMPONENTTYPE * pHandle, OMX_STRING pName)
{
	VOLOGF ();

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	voCOMXBaseComponent * pComponent = NULL;

	if (!strcmp (pName, m_szCompName[0]))
		pComponent = new voCOMXRTSPSource (pHandle);
	else
		return OMX_ErrorInvalidComponentName;

	if (pComponent == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pComponent;

	APIComponentFillInterface (pHandle);

	return errType;
}

OMX_ERRORTYPE voCOMXCoreMobiTV::FreeComponent (OMX_COMPONENTTYPE * pHandle)
{
	VOLOGF ();

	pHandle->ComponentDeInit (pHandle);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCoreMobiTV::GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8  **compNames)
{
	VOLOGF ();

	if (role == NULL || pNumComps == NULL)
		return OMX_ErrorBadParameter;

	if (compNames == NULL)
	{
		*pNumComps = 1;
		return OMX_ErrorNone;
	}

	OMX_U32		i, j;
	OMX_BOOL	bFound = OMX_FALSE;
	for (i = 0; i < VOOMX_COMPONENT_NUM; i++)
	{
		for (j = 0; j < m_nCompRoles[i]; j++)
		{
			if (!strcmp (role, m_szCompRoles[i][j]))
			{
				strcpy ((char *)compNames[0], m_szCompName[i]);
				bFound = OMX_TRUE;
				break;
			}
		}
	}

	if (bFound)
		return OMX_ErrorNone;

	return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE voCOMXCoreMobiTV::GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles)
{
	VOLOGF ();

	if (compName == NULL)
		return OMX_ErrorInvalidComponentName;
	if (pNumRoles == NULL)
		return OMX_ErrorBadParameter;

	OMX_U32 i = 0;
	OMX_S32 nIndex = -1;
	for (i = 0; i < VOOMX_COMPONENT_NUM; i++)
	{
		if (!strcmp (compName, m_szCompName[i]))
		{
			nIndex = i;
			break;
		}
	}

	if (nIndex < 0)
		return OMX_ErrorInvalidComponentName;

	if (roles == NULL)
	{
		*pNumRoles = m_nCompRoles[nIndex];
		return OMX_ErrorNone;
	}

	OMX_U32 nCount = *pNumRoles;
	if (nCount >= m_nCompRoles[nIndex])
		nCount = m_nCompRoles[nIndex];

	for (i = 0; i < nCount; i++)
		strcpy ((char *)roles[i], m_szCompRoles[nIndex][i]);

	return OMX_ErrorNone;
}
