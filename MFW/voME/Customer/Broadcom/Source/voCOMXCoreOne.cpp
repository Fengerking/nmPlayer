	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreOne.cpp

	Contains:	voCOMXCoreOne class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "voString.h"

#include "voCOMXCoreOne.h"
#include "voOMXOSFun.h"
#include "voOMXMemory.h"

#include "voComponentEntry.h"

#include "voCOMXAudioDec.h"
#include "voCOMXVideoDec.h"
#include "voCOMXVideoSink.h"

#include "voCOMXVideoEnc.h"
#include "voCOMXAudioEnc.h"

#include "voLog.h"

voCOMXCoreOne::voCOMXCoreOne(void)
{
	VOLOGF ();

	strcpy (m_pObjName, __FILE__);

	OMX_U32 i = 0;
	for (i = 0; i < VOOMX_COMPONENT_NUM; i++)
		m_szCompName[i] = (OMX_STRING) voOMXMemAlloc (128);

	strcpy (m_szCompName[0], "OMX.VisualOn.Audio.Decoder.BC");
	strcpy (m_szCompName[1], "OMX.VisualOn.Video.Decoder.BC");
	strcpy (m_szCompName[2], "OMX.VisualOn.Video.Sink.BC");
	strcpy (m_szCompName[3], "OMX.VisualOn.Video.Encoder.BC");
	strcpy (m_szCompName[4], "OMX.VisualOn.Audio.Encoder.BC");

	m_nCompRoles[0] = 9;		// Audio Decoder
	m_nCompRoles[1] = 9;		// Video Decoder
	m_nCompRoles[2] = 1;		// Video Sink
	m_nCompRoles[3] = 5;		// Audio Encoder
	m_nCompRoles[4] = 2;		// Video Encoder

	for (i = 0; i < VOOMX_COMPONENT_NUM; i++)
	{
		m_szCompRoles[i] = (OMX_STRING *) voOMXMemAlloc (m_nCompRoles[i] * 4);
		for (OMX_U32 j = 0; j < m_nCompRoles[i]; j++)
		{
			m_szCompRoles[i][j] = (OMX_STRING) voOMXMemAlloc (128);
		}
	}

	// audio decoder
	strcpy (m_szCompRoles[0][0], "audio_decoder.aac");
	strcpy (m_szCompRoles[0][1], "audio_decoder.mp3");
	strcpy (m_szCompRoles[0][2], "audio_decoder.wma");
	strcpy (m_szCompRoles[0][3], "audio_decoder.amr");
	strcpy (m_szCompRoles[0][4], "audio_decoder.qcelp");
	strcpy (m_szCompRoles[0][5], "audio_decoder.evrc");
	strcpy (m_szCompRoles[0][6], "audio_decoder.adpcm");
	strcpy (m_szCompRoles[0][7], "audio_decoder.ac3");
	strcpy (m_szCompRoles[0][8], "audio_decoder.amrwbp");

	// video decoder
	strcpy (m_szCompRoles[1][0], "video_decoder.avc");
	strcpy (m_szCompRoles[1][1], "video_decoder.mpeg4");
	strcpy (m_szCompRoles[1][2], "video_decoder.wmv");
	strcpy (m_szCompRoles[1][3], "video_decoder.h263");
	strcpy (m_szCompRoles[1][4], "video_decoder.rv");
	strcpy (m_szCompRoles[1][5], "video_decoder.s263");
	strcpy (m_szCompRoles[1][6], "video_decoder.vp6");
	strcpy (m_szCompRoles[1][7], "video_decoder.div3");
	strcpy (m_szCompRoles[1][8], "video_decoder.mpeg2");

	// video sink
	strcpy (m_szCompRoles[2][0], "video_sink.yuv-rgb");

	// audio encoder
	strcpy (m_szCompRoles[3][0], "audio_encoder.amrnb");
	strcpy (m_szCompRoles[3][1], "audio_encoder.aac");
	strcpy (m_szCompRoles[3][2], "audio_encoder.mp3");
	strcpy (m_szCompRoles[3][3], "audio_encoder.qcelp13");
	strcpy (m_szCompRoles[3][4], "audio_encoder.evrc");

	// video encoder
	strcpy (m_szCompRoles[4][0], "video_encoder.mpeg4");
	strcpy (m_szCompRoles[4][1], "video_encoder.h263");
}

voCOMXCoreOne::~voCOMXCoreOne(void)
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

OMX_ERRORTYPE voCOMXCoreOne::Init (OMX_PTR hInst)
{
	VOLOGF ();

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	return errType;
}

OMX_STRING voCOMXCoreOne::GetName (OMX_U32 nIndex)
{
	VOLOGF ();

	if (nIndex < 0 || nIndex >= VOOMX_COMPONENT_NUM)
		return NULL;

	return m_szCompName[nIndex];
}

OMX_ERRORTYPE voCOMXCoreOne::LoadComponent (OMX_COMPONENTTYPE * pHandle, OMX_STRING pName)
{
	VOLOGF ();

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	voCOMXBaseComponent * pComponent = NULL;

	if (!strcmp (pName, m_szCompName[0]))
		pComponent = new voCOMXAudioDec (pHandle);
	else if (!strcmp (pName, m_szCompName[1]))
		pComponent = new voCOMXVideoDec (pHandle);
	else if (!strcmp (pName, m_szCompName[2]))
		pComponent = new voCOMXVideoSink (pHandle);
	else if (!strcmp (pName, m_szCompName[3]))
		pComponent = new voCOMXAudioEnc (pHandle);
	else if (!strcmp (pName, m_szCompName[4]))
		pComponent = new voCOMXVideoEnc (pHandle);
	else
		return OMX_ErrorInvalidComponentName;

	if (pComponent == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pComponent;

	APIComponentFillInterface (pHandle);

	return errType;
}

OMX_ERRORTYPE voCOMXCoreOne::FreeComponent (OMX_COMPONENTTYPE * pHandle)
{
	VOLOGF ();

	pHandle->ComponentDeInit (pHandle);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCoreOne::GetComponentsOfRole (OMX_STRING role, OMX_U32 *pNumComps, OMX_U8  **compNames)
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

OMX_ERRORTYPE voCOMXCoreOne::GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles)
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
