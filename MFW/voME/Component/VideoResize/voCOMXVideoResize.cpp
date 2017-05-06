	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoResize.cpp

	Contains:	voCOMXVideoResize class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voCOMXVideoResize.h"

#define LOG_TAG "voCOMXVideoResize"
#include "voLog.h"

voCOMXVideoResize::voCOMXVideoResize(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_pVideoRisize (NULL)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Video.Resize");

	memset(&m_inputPortDefinition, 0, sizeof(OMX_VIDEO_PORTDEFINITIONTYPE));
	memset(&m_outputPortDefinition, 0, sizeof(OMX_VIDEO_PORTDEFINITIONTYPE));

	memset(&m_inputFormat, 0, sizeof(VO_VIDEO_FORMAT));
	memset(&m_outputFormat, 0, sizeof(VO_VIDEO_FORMAT));

	memset(&m_inputBuffer, 0, sizeof(VO_VIDEO_BUFFER));
	memset(&m_outputBuffer, 0, sizeof(VO_VIDEO_BUFFER));

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;
}

voCOMXVideoResize::~voCOMXVideoResize(void)
{
	if (m_pVideoRisize != NULL)
		delete m_pVideoRisize;
}

OMX_ERRORTYPE voCOMXVideoResize::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	switch (nParamIndex)
	{
	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			if (pVideoFormat->nPortIndex == 0)
			{
				pVideoFormat->eCompressionFormat = OMX_VIDEO_CodingUnused;
				pVideoFormat->eColorFormat = OMX_COLOR_FormatYUV420Planar;
			}
			else
			{
				pVideoFormat->eCompressionFormat = OMX_VIDEO_CodingUnused;
				pVideoFormat->eColorFormat = OMX_COLOR_FormatYUV420Planar;
			}
		}
		break;

	default:
		errType = voCOMXCompFilter::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXVideoResize::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	switch (nIndex)
	{
	case OMX_IndexParamPortDefinition:
		{
			OMX_PARAM_PORTDEFINITIONTYPE * pType = (OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParameterStructure;
			errType = CheckParam (pComp, pType->nPortIndex, pType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pType->eDomain != OMX_PortDomainVideo)
				return OMX_ErrorPortsNotCompatible;

			if (pType->nPortIndex == 1) // output port
			{
				pType->format.video.nStride = (pType->format.video.nFrameWidth + 15) & ~15;
				pType->nBufferSize = pType->format.video.nStride * pType->format.video.nFrameHeight * 3 / 2;
			}

			errType = m_ppPorts[pType->nPortIndex]->SetPortType (pType);
			if (errType != OMX_ErrorNone)
				return errType;

			if (pType->nPortIndex == 0)
			{
				// input port
				memcpy(&m_inputPortDefinition, &pType->format.video, sizeof(OMX_VIDEO_PORTDEFINITIONTYPE));
				m_inputFormat.Width = m_inputPortDefinition.nFrameWidth;
				m_inputFormat.Height = m_inputPortDefinition.nFrameHeight;
				m_inputBuffer.Stride[0] = (m_inputFormat.Width + 15) & ~15;
				m_inputBuffer.Stride[1] = m_inputBuffer.Stride[2] = m_inputBuffer.Stride[0] / 2;
			}
			else if (pType->nPortIndex == 1)
			{
				// output port
				memcpy(&m_outputPortDefinition, &pType->format.video, sizeof(OMX_VIDEO_PORTDEFINITIONTYPE));
				m_outputFormat.Width = m_outputPortDefinition.nFrameWidth;
				m_outputFormat.Height = m_outputPortDefinition.nFrameHeight;
				m_outputBuffer.Stride[0] = m_outputPortDefinition.nStride;
				m_outputBuffer.Stride[1] = m_outputBuffer.Stride[2] = m_outputBuffer.Stride[0] / 2;
			}
		}
		break;

	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex == 0)
			{
				// input port
				if(pVideoFormat->eCompressionFormat == OMX_VIDEO_CodingUnused)
				{
					if(pVideoFormat->eColorFormat == OMX_COLOR_FormatYUV420Planar)
					{
						m_inputBuffer.Stride[0] = (m_inputFormat.Width + 15) & ~15;
						m_inputBuffer.Stride[1] = m_inputBuffer.Stride[2] = m_inputBuffer.Stride[0] / 2;
						m_inputBuffer.ColorType = VO_COLOR_YUV_PLANAR420;
						errType = OMX_ErrorNone;
					}
					else
					{
						VOLOGE ("Unsupported Color Format %d", pVideoFormat->eColorFormat);
						errType = OMX_ErrorComponentNotFound;
					}
				}
			}
			else if (pVideoFormat->nPortIndex == 1)
			{
				// output port
				if(pVideoFormat->eCompressionFormat == OMX_VIDEO_CodingUnused)
				{
					if(pVideoFormat->eColorFormat == OMX_COLOR_FormatYUV420Planar)
					{
						m_outputBuffer.Stride[0] = m_outputPortDefinition.nStride;
						m_outputBuffer.Stride[1] = m_outputBuffer.Stride[2] = m_outputBuffer.Stride[0] / 2;
						m_outputBuffer.ColorType = VO_COLOR_YUV_PLANAR420;
						errType = OMX_ErrorNone;
					}
					else
					{
						VOLOGE ("Unsupported Color Format %d", pVideoFormat->eColorFormat);
						errType = OMX_ErrorComponentNotFound;
					}
				}
			}
		}
		break;

	default:
		errType = voCOMXCompFilter::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXVideoResize::CreateVideoResize (void)
{
	m_pVideoRisize = new CVideoResize(NULL, m_pMemOP);
	if(m_pVideoRisize == NULL)
	{
		VOLOGE ("Can not create the video resize!");
		return OMX_ErrorInsufficientResources;
	}
	m_pVideoRisize->SetLibOperator((VO_LIB_OPERATOR *)m_pLibOP);
	m_pVideoRisize->SetWorkPath((VO_TCHAR *)m_pWorkPath);
	m_pVideoRisize->SetOMXComp(VO_TRUE);

	VO_U32 nRC = m_pVideoRisize->Init();
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_pVideoRisize->Init failed 0x%08X", (unsigned int)nRC);
		return OMX_ErrorResourcesLost;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoResize::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied, OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	voCOMXAutoLock lock(&m_mutCodec);

	if(m_pVideoRisize == NULL)
		CreateVideoResize();

	if(m_pVideoRisize == NULL)
	{
		VOLOGW ("m_pVideoRisize == NULL");

		*pEmptied = OMX_TRUE;

		*pFilled = OMX_TRUE;
		pOutput->nFilledLen = 0;

		return OMX_ErrorUndefined;
	}

	if(pInput->nFilledLen <= 0)
	{
		*pEmptied = OMX_TRUE;
		return OMX_ErrorNone;
	}

	if(pInput->nFilledLen == sizeof(VO_VIDEO_BUFFER))
		memcpy(&m_inputBuffer, pInput->pBuffer, sizeof(VO_VIDEO_BUFFER));
	else
	{
		m_inputBuffer.Buffer[0] = pInput->pBuffer;
		m_inputBuffer.Buffer[1] = pInput->pBuffer + m_inputBuffer.Stride[0] * m_inputFormat.Height;
		m_inputBuffer.Buffer[2] = m_inputBuffer.Buffer[1] + m_inputBuffer.Stride[1] * (m_inputFormat.Height / 2);
	}

	m_outputBuffer.Buffer[0] = pOutput->pBuffer;
	m_outputBuffer.Buffer[1] = pOutput->pBuffer + m_outputBuffer.Stride[0] * m_outputFormat.Height;
	m_outputBuffer.Buffer[2] = m_outputBuffer.Buffer[1] + m_outputBuffer.Stride[1] * (m_outputFormat.Height / 2);

	VO_U32 nRC = m_pVideoRisize->Process(&m_inputFormat, &m_inputBuffer, &m_outputFormat, &m_outputBuffer);

	*pEmptied = OMX_TRUE;

	if(nRC == VO_ERR_NONE)
	{
		*pFilled = OMX_TRUE;
		pOutput->nFilledLen = m_outputPortDefinition.nStride * m_outputFormat.Height * 3 / 2;
		pOutput->nTimeStamp = pInput->nTimeStamp;
	}
	else
	{
		*pFilled = (pInput->nFlags & OMX_BUFFERFLAG_EOS) ? OMX_TRUE : OMX_FALSE;
		pOutput->nFilledLen = 0;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoResize::InitPortType (void)
{
	m_portParam[OMX_PortDomainVideo].nPorts = 2;
	m_portParam[OMX_PortDomainVideo].nStartPortNumber = 0;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
	m_pInput->GetPortType (&pPortType);

	pPortType->eDomain = OMX_PortDomainVideo;
	pPortType->nBufferCountActual = 1;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 20480;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;
	pPortType->format.video.nFrameWidth = 0;
	pPortType->format.video.nFrameHeight = 0;
	pPortType->format.video.nStride = 0;
	pPortType->format.video.nSliceHeight = 16;
	pPortType->format.video.nBitrate = 0;
	pPortType->format.video.xFramerate = 0;
	pPortType->format.video.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.video.eCompressionFormat = OMX_VIDEO_CodingAutoDetect;
	pPortType->format.video.eColorFormat = OMX_COLOR_FormatUnused;
	pPortType->format.video.pNativeWindow = NULL;
	pPortType->format.video.pNativeRender = NULL;

	m_pOutput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainVideo;
	pPortType->nBufferCountActual = 1;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 0;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;

	pPortType->format.video.nFrameWidth = 0;
	pPortType->format.video.nFrameHeight = 0;
	pPortType->format.video.nStride = 0;
	pPortType->format.video.nSliceHeight = 16;
	pPortType->format.video.nBitrate = 0;
	pPortType->format.video.xFramerate = 0;
	pPortType->format.video.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	pPortType->format.video.eColorFormat = OMX_COLOR_FormatYUV420Planar;
	pPortType->format.video.pNativeWindow = NULL;
	pPortType->format.video.pNativeRender = NULL;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoResize::SetNewState (OMX_STATETYPE newState)
{
	if(newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if(m_pVideoRisize == NULL)
			CreateVideoResize();
	}
	else if (newState == OMX_StateIdle && (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle))
		ResetPortBuffer();

	return voCOMXCompFilter::SetNewState(newState);
}
