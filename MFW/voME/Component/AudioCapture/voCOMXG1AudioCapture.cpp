	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXG1AudioCapture.cpp

	Contains:	voCOMXG1AudioCapture class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMXMemory.h"
#include "voOMXBase.h"
#include "voOMXOSFun.h"

#include "voCOMXG1AudioCapture.h"

#define LOG_TAG "voCOMXG1AudioCapture"
#include "voLog.h"

voCOMXG1AudioCapture::voCOMXG1AudioCapture(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSource (pComponent)
	, m_pAudioPort (NULL)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.G1Audio.Capture");
}

voCOMXG1AudioCapture::~voCOMXG1AudioCapture()
{
//	m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);
}

OMX_ERRORTYPE voCOMXG1AudioCapture::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	if (m_pAudioPort != NULL)
	{
		OMX_ERRORTYPE errType = m_pAudioPort->GetParameter (nParamIndex, pComponentParameterStructure);
		if (errType == OMX_ErrorNone)
			return errType;
	}

	return  voCOMXCompSource::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
}


OMX_ERRORTYPE voCOMXG1AudioCapture::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	if (m_pAudioPort != NULL)
	{
		OMX_ERRORTYPE errType = m_pAudioPort->SetParameter (nIndex, pComponentParameterStructure);
		if (errType == OMX_ErrorNone)
			return errType;
	}

	return  voCOMXCompSource::SetParameter (hComponent, nIndex, pComponentParameterStructure);
}

OMX_ERRORTYPE voCOMXG1AudioCapture::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_INOUT OMX_PTR pComponentConfigStructure)
{
	if (m_pAudioPort != NULL)
	{
		OMX_ERRORTYPE errType = m_pAudioPort->GetConfig (nIndex, pComponentConfigStructure);
		if (errType == OMX_ErrorNone)
			return errType;
	}

	return voCOMXCompSource::GetConfig (hComponent, nIndex, pComponentConfigStructure);
}


OMX_ERRORTYPE voCOMXG1AudioCapture::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_IN  OMX_PTR pComponentConfigStructure)
{
	if (m_pAudioPort != NULL)
	{
		OMX_ERRORTYPE errType = m_pAudioPort->SetConfig (nIndex, pComponentConfigStructure);
		if (errType == OMX_ErrorNone)
			return errType;
	}

	return voCOMXCompSource::SetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXG1AudioCapture::CreatePorts (void)
{
	if (m_uPorts == 0)
	{
		m_uPorts = 1;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
		if (m_ppPorts == NULL)
			return OMX_ErrorInsufficientResources;

		m_pAudioPort = new voCOMXG1AudioCapPort (this, 0);
		if (m_pAudioPort == NULL)
			return OMX_ErrorInsufficientResources;

		m_pAudioPort->SetCallbacks (m_pCallBack, m_pAppData);
		m_ppPorts[0] = m_pAudioPort;

		InitPortType ();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXG1AudioCapture::InitPortType (void)
{
	m_portParam[OMX_PortDomainAudio].nPorts = 1;
	m_portParam[OMX_PortDomainAudio].nStartPortNumber = 0;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;

	m_pAudioPort->GetPortType (&pPortType);
	pPortType->eDir    = OMX_DirOutput;
	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 1;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 44100;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;
	pPortType->format.audio.pNativeRender = NULL;
	pPortType->format.audio.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.audio.eEncoding = OMX_AUDIO_CodingPCM;

	return OMX_ErrorNone;
}
