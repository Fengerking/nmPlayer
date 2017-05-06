	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXG1VideoCapture.cpp

	Contains:	voCOMXG1VideoCapture class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMXMemory.h"
#include "voOMXBase.h"
#include "voOMXOSFun.h"

#include "voCOMXG1VideoCapture.h"

#define LOG_TAG "voCOMXG1VideoCapture"
#include "voLog.h"

voCOMXG1VideoCapture::voCOMXG1VideoCapture(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSource (pComponent)
	, m_pVideoPort (NULL)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.G1Video.Capture");
}

voCOMXG1VideoCapture::~voCOMXG1VideoCapture()
{
//	m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);
}

OMX_ERRORTYPE voCOMXG1VideoCapture::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	if (m_pVideoPort != NULL)
	{
		OMX_ERRORTYPE errType = m_pVideoPort->GetParameter (nParamIndex, pComponentParameterStructure);
		if (errType == OMX_ErrorNone)
			return errType;
	}

	return  voCOMXCompSource::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
}


OMX_ERRORTYPE voCOMXG1VideoCapture::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	if (m_pVideoPort != NULL)
	{
		OMX_ERRORTYPE errType = m_pVideoPort->SetParameter (nIndex, pComponentParameterStructure);
		if (errType == OMX_ErrorNone)
			return errType;
	}

	return  voCOMXCompSource::SetParameter (hComponent, nIndex, pComponentParameterStructure);
}

OMX_ERRORTYPE voCOMXG1VideoCapture::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_INOUT OMX_PTR pComponentConfigStructure)
{
	if (m_pVideoPort != NULL)
	{
		OMX_ERRORTYPE errType = m_pVideoPort->GetConfig (nIndex, pComponentConfigStructure);
		if (errType == OMX_ErrorNone)
			return errType;
	}

	return voCOMXCompSource::GetConfig (hComponent, nIndex, pComponentConfigStructure);
}


OMX_ERRORTYPE voCOMXG1VideoCapture::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_IN  OMX_PTR pComponentConfigStructure)
{
	if (m_pVideoPort != NULL)
	{
		OMX_ERRORTYPE errType = m_pVideoPort->SetConfig (nIndex, pComponentConfigStructure);
		if (errType == OMX_ErrorNone)
			return errType;
	}

	return voCOMXCompSource::SetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXG1VideoCapture::CreatePorts (void)
{
	if (m_uPorts == 0)
	{
		m_uPorts = 1;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
		if (m_ppPorts == NULL)
			return OMX_ErrorInsufficientResources;

		m_pVideoPort = new voCOMXG1VideoCapPort (this, 0);
		if (m_pVideoPort == NULL)
			return OMX_ErrorInsufficientResources;

		m_pVideoPort->SetCallbacks (m_pCallBack, m_pAppData);
		m_ppPorts[0] = m_pVideoPort;

		InitPortType ();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXG1VideoCapture::InitPortType (void)
{
	m_portParam[OMX_PortDomainVideo].nPorts = 1;
	m_portParam[OMX_PortDomainVideo].nStartPortNumber = 0;

	return OMX_ErrorNone;
}
