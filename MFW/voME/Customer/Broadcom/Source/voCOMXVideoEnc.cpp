	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoEnc.cpp

	Contains:	voCOMXVideoEnc class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"

#include "voCOMXVideoEnc.h"

#include "voLog.h"

voCOMXVideoEnc::voCOMXVideoEnc(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_pFormatData (NULL)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Video.Encoder.XXX");

	m_videoType.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
	m_videoType.nFrameWidth        = 352;
	m_videoType.nFrameHeight       = 288;
	m_videoType.eColorFormat       = OMX_COLOR_FormatYUV420PackedPlanar;

	VOLOGF ();
}

voCOMXVideoEnc::~voCOMXVideoEnc(void)
{
	VOLOGF ();

	if (m_pFormatData != NULL)
	{
		voOMXMemFree (m_pFormatData);
		m_pFormatData = NULL;
	}
}


OMX_ERRORTYPE voCOMXVideoEnc::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s. The Param Index is 0X%08X", m_pName, nParamIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nParamIndex)
	{
	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			if (pVideoFormat->nPortIndex == 0)
			{
				pVideoFormat->eCompressionFormat = m_videoType.eCompressionFormat;
				pVideoFormat->eColorFormat       = m_videoType.eColorFormat;
			}
			else if (pVideoFormat->nPortIndex == 1)
			{
				pVideoFormat->eCompressionFormat = m_videoType.eCompressionFormat;
				pVideoFormat->eColorFormat       = OMX_COLOR_FormatUnused;
			}
		}
		break;

	default:
		errType = voCOMXCompFilter::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXVideoEnc::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s. The Param Index is 0X%08X", m_pName, nIndex);

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{
	case OMX_IndexParamPortDefinition:
		{
			OMX_PARAM_PORTDEFINITIONTYPE * pType = (OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParameterStructure;
			if (pType->eDomain != OMX_PortDomainVideo)
				return OMX_ErrorPortsNotCompatible;

			errType = CheckParam (pComp, pType->nPortIndex, pType, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pType->nPortIndex == 0)
			{
				errType = m_pInput->SetPortType (pType);
/*
				m_videoType.cMIMEType             = pType->format.video.cMIMEType;
				m_videoType.pNativeRender         = pType->format.video.pNativeRender;
				m_videoType.nFrameWidth           = pType->format.video.nFrameWidth;
				m_videoType.nFrameHeight          = pType->format.video.nFrameHeight;
				m_videoType.nStride               = pType->format.video.nStride;
				m_videoType.nSliceHeight          = pType->format.video.nSliceHeight;
				m_videoType.nBitrate              = pType->format.video.nBitrate;
				m_videoType.xFramerate            = pType->format.video.xFramerate;
				m_videoType.bFlagErrorConcealment = pType->format.video.bFlagErrorConcealment;
				m_videoType.eColorFormat          = pType->format.video.eColorFormat;
				m_videoType.pNativeWindow         = pType->format.video.pNativeWindow;


				OMX_PARAM_PORTDEFINITIONTYPE * pInputType;
				m_pInput->GetPortType (&pInputType);

				pInputType->format.video.nFrameWidth  = m_videoType.nFrameWidth;
				pInputType->format.video.nFrameHeight = m_videoType.nFrameHeight;
				pInputType->format.video.nStride      = m_videoType.nFrameWidth;
				pInputType->format.video.nBitrate     = m_videoType.nBitrate;
				pInputType->format.video.xFramerate   = m_videoType.xFramerate;

				pInputType->format.video.eColorFormat = m_videoType.eColorFormat;

				pInputType->nBufferSize = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;

				switch(m_videoType.eColorFormat)
				{
				case OMX_COLOR_FormatYUV420Planar:
					m_inBuffer.ColorType  = VO_COLOR_YUV_PLANAR420;
					break;

				case OMX_COLOR_FormatYUV420PackedPlanar:
					m_inBuffer.ColorType  = VO_COLOR_YUV_420_PACK_2;
					break;

				default:
					break;
				}
*/
				// Output
				OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
				m_pOutput->GetPortType (&pPortType);
				pPortType->format.video.nFrameWidth = m_videoType.nFrameWidth;
				pPortType->format.video.nFrameHeight = m_videoType.nFrameHeight;

			}
		}
		break;

	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;

			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof(OMX_VIDEO_PARAM_PORTFORMATTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex == 0)
			{
				errType = OMX_ErrorNone;
			}
			else if (pVideoFormat->nPortIndex == 1)
			{
				m_videoType.eCompressionFormat = pVideoFormat->eCompressionFormat;

				VOLOGI("Video Codec %d ", m_videoType.eCompressionFormat );

				if (m_videoType.eCompressionFormat == OMX_VIDEO_CodingH263)
				{
					errType = OMX_ErrorNone;
				}
				else if (m_videoType.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
				{
					errType = OMX_ErrorNone;
				}
				else 
				{
					VOLOGW ("Codec %d.", m_videoType.eCompressionFormat);
					errType = OMX_ErrorComponentNotFound;
				}
			}
		}
		break;

	case OMX_IndexParamStandardComponentRole:
		{
			OMX_PARAM_COMPONENTROLETYPE * pRoleType = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;

			errType = voOMXBase_CheckHeader(pRoleType, sizeof(OMX_PARAM_COMPONENTROLETYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			m_videoType.eCompressionFormat = OMX_VIDEO_CodingUnused;
			if (!strcmp ((char *)pRoleType->cRole, "video_encoder.mpeg4"))
				m_videoType.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
			else if (!strcmp ((char *)pRoleType->cRole, "video_encoder.h263"))
				m_videoType.eCompressionFormat = OMX_VIDEO_CodingH263;

			if (m_videoType.eCompressionFormat != OMX_VIDEO_CodingUnused)
				errType = OMX_ErrorNone;
			else
				errType = OMX_ErrorInvalidComponent;
		}
		break;


	default:
		errType = voCOMXCompFilter::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXVideoEnc::ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_OUT OMX_U8 *cRole,
													OMX_IN OMX_U32 nIndex)
{
	strcat((char*)cRole, "\0");
	if (nIndex == 0)
		strcpy ((char*)cRole, "video_encoder.mpeg4");
	else if (nIndex == 1)
		strcpy ((char*)cRole, "video_encoder.h263");
	else 
		return OMX_ErrorNoMore;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoEnc::SetNewState (OMX_STATETYPE newState)
{
	VOLOGF ("Name: %s. The new state is %d", m_pName, newState);

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{

	}

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXVideoEnc::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	VOLOGR ("Name: %s. Size: %d, Time %d", m_pName, pInput->nFilledLen, (int)pInput->nTimeStamp);

	OMX_U32 nRC = 0;


	*pEmptied = OMX_TRUE;
	*pFilled = OMX_TRUE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoEnc::InitPortType (void)
{
	VOLOGF ("Name: %s.", m_pName);

	m_portParam[OMX_PortDomainVideo].nPorts = 2;
	m_portParam[OMX_PortDomainVideo].nStartPortNumber = 0;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;

	// Input
	m_pInput->GetPortType (&pPortType);
	pPortType->eDomain             = OMX_PortDomainVideo;
	pPortType->nBufferCountActual  = 1;
	pPortType->nBufferCountMin     = 1;
	pPortType->nBufferSize         = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;
	pPortType->bBuffersContiguous  = OMX_FALSE;
	pPortType->nBufferAlignment    = 1;

	pPortType->format.video.nFrameWidth           = m_videoType.nFrameWidth;
	pPortType->format.video.nFrameHeight          = m_videoType.nFrameHeight;
	pPortType->format.video.nStride               = m_videoType.nFrameWidth;
	pPortType->format.video.nSliceHeight          = 16;
	pPortType->format.video.nBitrate              = 0;
	pPortType->format.video.xFramerate            = 0;
	pPortType->format.video.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.video.eCompressionFormat    = OMX_VIDEO_CodingUnused;
	pPortType->format.video.eColorFormat          = m_videoType.eColorFormat;
	pPortType->format.video.pNativeWindow         = NULL;
	pPortType->format.video.pNativeRender         = NULL;

	// Output
	m_pOutput->GetPortType (&pPortType);
	pPortType->eDomain             = OMX_PortDomainVideo;
	pPortType->nBufferCountActual  = 1;
	pPortType->nBufferCountMin     = 1;
	pPortType->nBufferSize         = 102400;
	pPortType->bBuffersContiguous  = OMX_FALSE;
	pPortType->nBufferAlignment    = 1;

	pPortType->format.video.nFrameWidth           = m_videoType.nFrameWidth;
	pPortType->format.video.nFrameHeight          = m_videoType.nFrameHeight;
	pPortType->format.video.nStride               = m_videoType.nFrameWidth;
	pPortType->format.video.nSliceHeight          = 16;
	pPortType->format.video.nBitrate              = 0;
	pPortType->format.video.xFramerate            = 0;
	pPortType->format.video.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.video.eCompressionFormat    = m_videoType.eCompressionFormat;
	pPortType->format.video.eColorFormat          = OMX_COLOR_FormatUnused;
	pPortType->format.video.pNativeWindow         = NULL;
	pPortType->format.video.pNativeRender         = NULL;

	return OMX_ErrorNone;
}
