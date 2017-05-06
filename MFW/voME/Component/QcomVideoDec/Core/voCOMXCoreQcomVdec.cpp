	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreQcomVdec.cpp

	Contains:	voCOMXCoreQcomVdec class file

	Written by:	East Zhou

	Change History (most recent first):
	2010-06-18		East		Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "voString.h"

#include "voOMXOSFun.h"
#include "voOMXMemory.h"

#include "voCOMXCoreQcomVdec.h"
#include "voComponentEntry.h"
#include "voCOMXQcomVideoDec.h"

#define LOG_TAG "voCOMXCoreQcomVdec"
#include "voLog.h"

//VisualOn Video Decoder Component which use QCOM HW codec
//Totally support mpeg4/h263/divx/avc/vc1
voCOMXCoreQcomVdec::voCOMXCoreQcomVdec()
	: m_szCompName(NULL)
	, m_nCompRoles(5)
	, m_szCompRoles(NULL)
{
	strcpy(m_pObjName, __FILE__);

	m_szCompName = (OMX_STRING)voOMXMemAlloc(128);
	strcpy(m_szCompName, "OMX.VOQCOM.Video.Decoder.XXX");

	m_szCompRoles = (OMX_STRING*)voOMXMemAlloc(m_nCompRoles * sizeof(OMX_STRING));
	for(OMX_U32 i = 0; i < m_nCompRoles; i++)
		m_szCompRoles[i] = (OMX_STRING)voOMXMemAlloc(128);

	strcpy(m_szCompRoles[0], "video_decoder.mpeg4");
	strcpy(m_szCompRoles[1], "video_decoder.h263");
	strcpy(m_szCompRoles[2], "video_decoder.divx");
	strcpy(m_szCompRoles[3], "video_decoder.avc");
	strcpy(m_szCompRoles[4], "video_decoder.vc1");
}

voCOMXCoreQcomVdec::~voCOMXCoreQcomVdec()
{
	voOMXMemFree(m_szCompName);

	for(OMX_U32 i = 0; i < m_nCompRoles; i++)
		voOMXMemFree(m_szCompRoles[i]);
	voOMXMemFree(m_szCompRoles);
}

OMX_ERRORTYPE voCOMXCoreQcomVdec::Init(OMX_PTR hInst)
{
	return OMX_ErrorNone;
}

OMX_STRING voCOMXCoreQcomVdec::GetName(OMX_U32 nIndex)
{
	return (nIndex == 0) ? m_szCompName : NULL;
}

OMX_ERRORTYPE voCOMXCoreQcomVdec::LoadComponent(OMX_COMPONENTTYPE* pHandle, OMX_STRING pName)
{
	if(strcmp(pName, m_szCompName))
		return OMX_ErrorInvalidComponentName;

	voCOMXBaseComponent* pComponent = new voCOMXQcomVideoDec(pHandle);
	if(pComponent == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pComponent;
	APIComponentFillInterface(pHandle);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCoreQcomVdec::FreeComponent(OMX_COMPONENTTYPE* pHandle)
{
	pHandle->ComponentDeInit(pHandle);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCoreQcomVdec::GetComponentsOfRole(OMX_STRING role, OMX_U32* pNumComps, OMX_U8** compNames)
{
	if(role == NULL || pNumComps == NULL)
		return OMX_ErrorBadParameter;

	if(compNames == NULL)
	{
		*pNumComps = 1;
		return OMX_ErrorNone;
	}

	for(OMX_U32 i = 0; i < m_nCompRoles; i++)
	{
		if(!strcmp(role, m_szCompRoles[i]))
		{
			strcpy((char *)compNames[0], m_szCompName);

			return OMX_ErrorNone;
		}
	}

	return OMX_ErrorComponentNotFound;
}

OMX_ERRORTYPE voCOMXCoreQcomVdec::GetRolesOfComponent(OMX_STRING compName, OMX_U32* pNumRoles, OMX_U8** roles)
{
	if(pNumRoles == NULL)
		return OMX_ErrorBadParameter;

	if(compName == NULL || strcmp(compName, m_szCompName))
		return OMX_ErrorInvalidComponentName;

	if(roles == NULL)
	{
		*pNumRoles = m_nCompRoles;
		return OMX_ErrorNone;
	}

	OMX_U32 nCount = *pNumRoles;
	if(nCount >= m_nCompRoles)
		nCount = m_nCompRoles;

	for(OMX_U32 i = 0; i < nCount; i++)
		strcpy((OMX_STRING)roles[i], m_szCompRoles[i]);

	return OMX_ErrorNone;
}
