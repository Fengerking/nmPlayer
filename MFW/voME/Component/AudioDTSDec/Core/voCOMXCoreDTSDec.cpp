	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreDTS.cpp

	Contains:	voCOMXCoreDTS class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "voString.h"

#include "voOMXOSFun.h"
#include "voOMXMemory.h"

#include "voCOMXCoreDTSDec.h"
#include "voComponentEntry.h"
#include "voCOMXAudioDTSDec.h"

#include "voLog.h"

voCOMXCoreDTSDec::voCOMXCoreDTSDec(void)
{
	//VOLOGF ();

	strcpy (m_pObjName, __FILE__);

	OMX_U32 i = 0;
	m_szCompName = (OMX_STRING) voOMXMemAlloc (128);

	strcpy (m_szCompName, "OMX.VisualOn.Audio.Decoder.DTS");

	m_nCompRoles = 1;		// Audio Dobly Effect

	m_szCompRoles = (OMX_STRING *) voOMXMemAlloc (m_nCompRoles * 4);
		
	for (OMX_U32 i = 0; i < m_nCompRoles; i++)
	{
		m_szCompRoles[i] = (OMX_STRING) voOMXMemAlloc (128);
	}

	// DTS decoder
	strcpy (m_szCompRoles[0], "audio_decoder.dts");
}

voCOMXCoreDTSDec::~voCOMXCoreDTSDec(void)
{
	//VOLOGF ();

	OMX_U32 i = 0;
	voOMXMemFree (m_szCompName);

	for (i = 0; i < m_nCompRoles; i++)
	{
		voOMXMemFree (m_szCompRoles[i]);
	}
	
	voOMXMemFree (m_szCompRoles);
}

OMX_ERRORTYPE voCOMXCoreDTSDec::Init (OMX_PTR hInst)
{
	//VOLOGF ();

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	return errType;
}

OMX_STRING voCOMXCoreDTSDec::GetName (OMX_U32 nIndex)
{
	//VOLOGF ();

	if (nIndex < 0 || nIndex >= 1)
		return NULL;

	return m_szCompName;
}

OMX_ERRORTYPE voCOMXCoreDTSDec::LoadComponent (OMX_COMPONENTTYPE * pHandle, OMX_STRING pName)
{
	//VOLOGF ();

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	voCOMXBaseComponent * pComponent = NULL;

	if (!strcmp (pName, m_szCompName))
		pComponent = new voCOMXAudioDTSDec (pHandle);
	else
		return OMX_ErrorInvalidComponentName;

	if (pComponent == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pComponent;

	APIComponentFillInterface (pHandle);

	return errType;
}

OMX_ERRORTYPE voCOMXCoreDTSDec::FreeComponent (OMX_COMPONENTTYPE * pHandle)
{
	//VOLOGF ();

	pHandle->ComponentDeInit (pHandle);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCoreDTSDec::GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8  **compNames)
{
	//VOLOGF ();

	if (role == NULL || pNumComps == NULL)
		return OMX_ErrorBadParameter;

	if (compNames == NULL)
	{
		*pNumComps = 1;
		return OMX_ErrorNone;
	}

	OMX_U32		i;
	OMX_BOOL	bFound = OMX_FALSE;
	for (i = 0; i < m_nCompRoles; i++)
	{
		if (!strcmp (role, m_szCompRoles[i]))
		{
			strcpy ((char *)compNames[0], m_szCompName);
			bFound = OMX_TRUE;
			break;
		}
	}

	if (bFound)
		return OMX_ErrorNone;

	return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE voCOMXCoreDTSDec::GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles)
{
	//VOLOGF ();

	OMX_U32		i;

	if (compName == NULL)
		return OMX_ErrorInvalidComponentName;
	if (pNumRoles == NULL)
		return OMX_ErrorBadParameter;

	if (strcmp (compName, m_szCompName))
	{
		return OMX_ErrorInvalidComponentName;
	}

	if (roles == NULL)
	{
		*pNumRoles = m_nCompRoles;
		return OMX_ErrorNone;
	}

	OMX_U32 nCount = *pNumRoles;
	if (nCount >= m_nCompRoles)
		nCount = m_nCompRoles;

	for (i = 0; i < nCount; i++)
		strcpy ((char *)roles[i], m_szCompRoles[i]);

	return OMX_ErrorNone;
}
