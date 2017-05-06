#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "voString.h"

#include "voCOMXCoreOne4Sams.h"
#include "voOMXOSFun.h"
#include "voOMXMemory.h"

#include "voComponentEntry.h"
#include "voComxNameDefine.h"
#include "voComSamsVideoDec.h"
#include "voLog.h"

#define LOG_TAG "voCOMXCoreOne4Sams"

voCOMXCoreOne4Sams::voCOMXCoreOne4Sams(void)
{
	VOLOGF ();

	strcpy (m_pObjName, __FILE__);

	m_uCompNum = VOOMX_SAMS_COMPONENT_NUM;
	
	
	OMX_U32 i = 0;
	for (i = 0; i < VOOMX_SAMS_COMPONENT_NUM; i++)
		m_szCompName[i] = (OMX_STRING) voOMXMemAlloc (128);


	strcpy (m_szCompName[0], VIDEO_SAMS_VIDEO_DEC);
	strcpy (m_szCompName[1], VIDEO_SAMS_VIDEO_ENC);
	
	m_nCompRoles[0] = 3;		// mpeg4 decoder and h263 decoder and h264 decoder
	m_nCompRoles[1] = 3;		// mpeg4 encoder and h263 encoder, h264 encoder
	

	for (i = 0; i < VOOMX_SAMS_COMPONENT_NUM; i++)
	{
		m_szCompRoles[i] = (OMX_STRING *) voOMXMemAlloc (m_nCompRoles[i] * 4);
		if (m_szCompRoles[i] != NULL)
		{
			for (OMX_U32 j = 0; j < m_nCompRoles[i]; j++)
			{
				m_szCompRoles[i][j] = (OMX_STRING) voOMXMemAlloc (128);
			}
		}
	}

	// video decoder
	strcpy (m_szCompRoles[0][0], V_DEC_MP4);
	strcpy (m_szCompRoles[0][1], V_DEC_263);
	strcpy (m_szCompRoles[0][2], V_DEC_AVC);

	// video encoder
	strcpy (m_szCompRoles[1][0], V_ENC_MP4);
	strcpy (m_szCompRoles[1][1], V_ENC_263);
	strcpy (m_szCompRoles[1][2], V_ENC_AVC);
}

voCOMXCoreOne4Sams::~voCOMXCoreOne4Sams(void)
{
	VOLOGF ();

	OMX_U32 i = 0;
	for (i = 0; i < m_uCompNum; i++)
		voOMXMemFree (m_szCompName[i]);

	for (i = 0; i < m_uCompNum; i++)
	{
		for (OMX_U32 j = 0; j < m_nCompRoles[i]; j++)
		{
			voOMXMemFree (m_szCompRoles[i][j]);
		}

		voOMXMemFree (m_szCompRoles[i]);
	}

	//cmnMemShowStatus ();
}

OMX_ERRORTYPE voCOMXCoreOne4Sams::Init (OMX_PTR hInst)
{
	VOLOGF ();

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	return errType;
}

OMX_STRING voCOMXCoreOne4Sams::GetName (OMX_U32 nIndex)
{
	VOLOGF ();

	if (nIndex < 0 || nIndex >= m_uCompNum)
		return NULL;

	return m_szCompName[nIndex];
}

OMX_ERRORTYPE voCOMXCoreOne4Sams::LoadComponent (OMX_COMPONENTTYPE * pHandle, OMX_STRING pName)
{
	VOLOGF ();

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	voCOMXBaseComponent * pComponent = NULL;

	if (!strcmp (pName, VIDEO_SAMS_VIDEO_DEC))
		pComponent = new voComSamsVideoDec (pHandle);
/*
	else if (!strcmp (VIDEO_SAMS_VIDEO_ENC))
		pComponent = new voCOMXAudioDec (pHandle);
*/
 	else
		return OMX_ErrorInvalidComponentName;

	if (pComponent == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pComponent;

	APIComponentFillInterface (pHandle);
	
	return errType;
}

OMX_ERRORTYPE voCOMXCoreOne4Sams::FreeComponent (OMX_COMPONENTTYPE * pHandle)
{
	VOLOGF ();

	pHandle->ComponentDeInit (pHandle);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCoreOne4Sams::GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8  **compNames)
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
	for (i = 0; i < m_uCompNum; i++)
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

OMX_ERRORTYPE voCOMXCoreOne4Sams::GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles)
{
	VOLOGF ();

	if (compName == NULL)
		return OMX_ErrorInvalidComponentName;
	if (pNumRoles == NULL)
		return OMX_ErrorBadParameter;

	OMX_U32 i = 0;
	OMX_S32 nIndex = -1;
	for (i = 0; i < m_uCompNum; i++)
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
