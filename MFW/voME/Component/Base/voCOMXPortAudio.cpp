	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXPortAudio.cpp

	Contains:	voCOMXPortAudio class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif

#include <voOMXMemory.h>

#include "voOMXBase.h"
#include "voOMXOSFun.h"

#include "voCOMXBaseComponent.h"

#include "voCOMXPortAudio.h"

voCOMXPortAudio::voCOMXPortAudio(voCOMXBaseComponent * pParent, OMX_S32 nIndex, OMX_DIRTYPE dirType)
	: voCOMXBasePort (pParent, nIndex, dirType)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXPortAudio::~voCOMXPortAudio(void)
{
}

OMX_ERRORTYPE voCOMXPortAudio::TunnelRequest (OMX_COMPONENTTYPE * hTunneledComp, OMX_U32 nTunneledPort, 
											 OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	errType = voCOMXBasePort::TunnelRequest (hTunneledComp, nTunneledPort, pTunnelSetup);
	if (errType != OMX_ErrorNone)
		return errType;

	if (m_sType.eDir == OMX_DirOutput)
		return OMX_ErrorNone;

	m_hTunnelComp = NULL;
	m_nTunnelPort = NULL;

	OMX_AUDIO_PARAM_PORTFORMATTYPE tpAudioFormat;
	voOMXBase_SetHeader (&tpAudioFormat, sizeof (OMX_AUDIO_PARAM_PORTFORMATTYPE));
	tpAudioFormat.nPortIndex = nTunneledPort;

	errType = hTunneledComp->GetParameter (hTunneledComp, OMX_IndexParamAudioPortFormat, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	tpAudioFormat.nPortIndex = m_sType.nPortIndex;
	errType = m_pParent->SetParameter (m_pParent->GetComponent (), OMX_IndexParamAudioPortFormat, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingPCM)
	{
		OMX_AUDIO_PARAM_PCMMODETYPE tpPCMFormat;
		voOMXBase_SetHeader (&tpPCMFormat, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
		tpPCMFormat.nPortIndex = nTunneledPort;

		errType = hTunneledComp->GetParameter (hTunneledComp, OMX_IndexParamAudioPcm, &tpPCMFormat);
		if (errType != OMX_ErrorNone)
			return errType;

		tpPCMFormat.nPortIndex = m_sType.nPortIndex;
		errType = m_pParent->SetParameter (m_pParent->GetComponent (), OMX_IndexParamAudioPcm, &tpPCMFormat);
		if (errType != OMX_ErrorNone)
			return errType;
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingMP3)
	{
		OMX_AUDIO_PARAM_MP3TYPE tpMP3Format;
		voOMXBase_SetHeader (&tpMP3Format, sizeof (OMX_AUDIO_PARAM_MP3TYPE));
		tpMP3Format.nPortIndex = nTunneledPort;

		errType = hTunneledComp->GetParameter (hTunneledComp, OMX_IndexParamAudioMp3, &tpMP3Format);
		if (errType != OMX_ErrorNone)
			return errType;

		tpMP3Format.nPortIndex = m_sType.nPortIndex;
		errType = m_pParent->SetParameter (m_pParent->GetComponent (), OMX_IndexParamAudioMp3, &tpMP3Format);
		if (errType != OMX_ErrorNone)
			return errType;
	}

	OMX_U32 uDataSize = 256;
	OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *)voOMXMemAlloc (sizeof (OMX_OTHER_EXTRADATATYPE) + uDataSize);
	voOMXBase_SetHeader (pExtData, sizeof (OMX_OTHER_EXTRADATATYPE) + uDataSize);
	pExtData->nPortIndex = nTunneledPort;
	pExtData->nDataSize = uDataSize;

	errType = hTunneledComp->GetParameter (hTunneledComp, OMX_IndexParamCommonExtraQuantData, pExtData);
	if (errType == OMX_ErrorNone && pExtData->nDataSize > 0)
	{
		pExtData->nPortIndex = m_sType.nPortIndex;
		errType = m_pParent->SetParameter (m_pParent->GetComponent (), OMX_IndexParamCommonExtraQuantData, pExtData);
	}
	else
	{
		errType = OMX_ErrorNone;
	}
	voOMXMemFree (pExtData);

	if (errType == OMX_ErrorNone)
	{
		m_hTunnelComp = hTunneledComp;
		m_nTunnelPort = nTunneledPort;
	}

	return errType;
}
