	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoDec.cpp

	Contains:	voCOMXVideoDec class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <OMX_Video.h>

#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"

#include "voCOMXVideoDec.h"

//#define _VOLOG_FUNC
//#define _VOLOG_STAUS
//#define _VOLOG_RUN

//#define LOG_TAG "voCOMXVideoDec"

#include "voLog.h"

voCOMXVideoDec::voCOMXVideoDec(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_nCoding (OMX_VIDEO_CodingAutoDetect)
	, m_nExtSize (0)
	, m_pExtData (NULL)
	, m_nFrameTime (1)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Video.Decoder.XXX");

	memset (&m_videoType, 0, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));

	VOLOGF ("Name %s", m_pName);
}

voCOMXVideoDec::~voCOMXVideoDec(void)
{
	VOLOGF ("Name %s", m_pName);

	if (m_pExtData != NULL)
	{
		voOMXMemFree (m_pExtData);
		m_pExtData = NULL;
	}
}


OMX_ERRORTYPE voCOMXVideoDec::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nParamIndex);

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
				pVideoFormat->eCompressionFormat = m_nCoding;
				pVideoFormat->eColorFormat = OMX_COLOR_FormatUnused;
			}
			else
			{
				pVideoFormat->eCompressionFormat = OMX_VIDEO_CodingUnused;
				pVideoFormat->eColorFormat = OMX_COLOR_FormatYUV420Planar;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoProfileLevelCurrent:
		{
			OMX_VIDEO_PARAM_PROFILELEVELTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PROFILELEVELTYPE *) pComponentParameterStructure;
			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			errType = OMX_ErrorNone;
		}
		break;

	default:
		errType = voCOMXCompFilter::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXVideoDec::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nIndex);

	OMX_ERRORTYPE		errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

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

			errType = m_ppPorts[pType->nPortIndex]->SetPortType (pType);
			if (errType != OMX_ErrorNone)
				return errType;

			if (pType->nPortIndex == 0)
			{
				voOMXMemCopy (&m_videoType, &pType->format.video, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));

				OMX_PARAM_PORTDEFINITIONTYPE * pOutType;
				m_pOutput->GetPortType (&pOutType);
				pOutType->format.video.nFrameWidth = m_videoType.nFrameWidth;
				pOutType->format.video.nFrameHeight = m_videoType.nFrameHeight;
				pOutType->format.video.nStride = m_videoType.nFrameWidth;
				pOutType->format.video.nBitrate = m_videoType.nBitrate;
				pOutType->format.video.xFramerate = m_videoType.xFramerate;

				pOutType->nBufferSize = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;

				if (m_videoType.xFramerate == 0)
					m_nFrameTime = 1;
				else
				{
					if (((m_videoType.xFramerate >> 16) & 0XFFFF) == 0)
						m_nFrameTime = 40;
					else
						m_nFrameTime = 1000 * (m_videoType.xFramerate & 0XFFFF) / ((m_videoType.xFramerate >> 16) & 0XFFFF);
				}
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
				m_nCoding = pVideoFormat->eCompressionFormat;

				if (m_nCoding == OMX_VIDEO_CodingMPEG2)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingH263)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingMPEG4)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingWMV)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingRV)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingAVC)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingMJPEG)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingRV)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VO_VIDEO_CodingS263)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VO_VIDEO_CodingVP6)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VO_VIDEO_CodingDIV3)
					errType = OMX_ErrorNone;
				else
					errType = OMX_ErrorComponentNotFound;
			}
			else
			{
				errType = OMX_ErrorNone;
			}
		}
		break;

	case OMX_IndexParamVideoMpeg4:
		{
			OMX_VIDEO_PARAM_MPEG4TYPE * pVideoFormat = (OMX_VIDEO_PARAM_MPEG4TYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_MPEG4TYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			if (pVideoFormat->bGov == OMX_TRUE)
				return OMX_ErrorUndefined;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoWmv:
		{
			OMX_VIDEO_PARAM_WMVTYPE * pVideoFormat = (OMX_VIDEO_PARAM_WMVTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_WMVTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoProfileLevelQuerySupported:
		{
			OMX_VIDEO_PARAM_PROFILELEVELTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PROFILELEVELTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_PROFILELEVELTYPE));
			if (errType != OMX_ErrorNone)
				return errType;
			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			errType = OMX_ErrorNoMore;
		}
		break;

	case OMX_IndexParamCommonExtraQuantData:
		{
			OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *)pComponentParameterStructure;
			
			if (m_pExtData != NULL)
				voOMXMemFree (m_pExtData);
			m_pExtData = NULL;
			m_nExtSize = 0;

			if (pExtData->nDataSize > 0)
			{
				m_nExtSize = pExtData->nDataSize;
				m_pExtData = (OMX_S8*)voOMXMemAlloc (m_nExtSize);
				voOMXMemCopy (m_pExtData, pExtData->data, m_nExtSize);
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamStandardComponentRole:
		{
			OMX_PARAM_COMPONENTROLETYPE * pRoleType = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;

			errType = voOMXBase_CheckHeader(pRoleType, sizeof(OMX_PARAM_COMPONENTROLETYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			m_nCoding = OMX_VIDEO_CodingUnused;
			if (!strcmp ((char *)pRoleType->cRole, "video_decoder.avc"))
				m_nCoding = OMX_VIDEO_CodingAVC;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.mpeg4"))
				m_nCoding = OMX_VIDEO_CodingMPEG4;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.wmv"))
				m_nCoding = OMX_VIDEO_CodingWMV;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.h263"))
				m_nCoding = OMX_VIDEO_CodingH263;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.mpeg2"))
				m_nCoding = OMX_VIDEO_CodingMPEG2;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.s263"))
				m_nCoding = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingS263;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.vp6"))
				m_nCoding = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingVP6;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.div3"))
				m_nCoding = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingDIV3;

			if (m_nCoding != OMX_VIDEO_CodingUnused)
				errType = OMX_ErrorNone;
			else
				errType = OMX_ErrorInvalidComponent;

			if(NULL == m_pInput)
				CreatePorts();

			OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
			m_pInput->GetPortType (&pPortType);
			pPortType->nBufferCountActual = 2;
			pPortType->nBufferSize = 1024 * 64;

			m_pOutput->GetPortType (&pPortType);
			pPortType->nBufferCountActual = 2;
		}

	default:
		errType = voCOMXCompFilter::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXVideoDec::ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_OUT OMX_U8 *cRole,
													OMX_IN OMX_U32 nIndex)
{
	strcat((char*)cRole, "\0");
	if (nIndex == 0)
		strcpy ((char*)cRole, "video_decoder.avc");
	else if (nIndex == 1)
		strcpy ((char*)cRole, "video_decoder.mpeg4");
	else if (nIndex == 2)
		strcpy ((char*)cRole, "video_decoder.wmv");
	else if (nIndex == 3)
		strcpy ((char*)cRole, "video_decoder.h263");
	else if (nIndex == 4)
		strcpy ((char*)cRole, "video_decoder.rv");
	else if (nIndex == 5)
		strcpy ((char*)cRole, "video_decoder.s263");
	else if (nIndex == 6)
		strcpy ((char*)cRole, "video_decoder.vp6");
	else if (nIndex == 7)
		strcpy ((char*)cRole, "video_decoder.div3");
	else if (nIndex == 8)
		strcpy ((char*)cRole, "video_decoder.mpeg2");
	else 
		return OMX_ErrorNoMore;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoDec::SetNewState (OMX_STATETYPE newState)
{
	VOLOGF ("Name: %s. New State %d", m_pName, newState);

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{

	}

	if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{

	}

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXVideoDec::Flush (OMX_U32	nPort)
{
	VOLOGF ("Name: %s", m_pName);


	OMX_ERRORTYPE errType = voCOMXCompFilter::Flush (nPort);


	return errType;
}

OMX_ERRORTYPE voCOMXVideoDec::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	VOLOGR ("Name: %s. Size: %d, Time %d", m_pName, pInput->nFilledLen, (int)pInput->nTimeStamp);

	OMX_U32 nRC = 0;

/*
	if (nRC == VO_ERR_NONE && fmtVideo.Width > 0 && fmtVideo.Height > 0)
	{
//		if (m_videoType.nFrameWidth != fmtVideo.Width || m_videoType.nFrameHeight != fmtVideo.Height)
		if (m_videoType.nFrameWidth <= fmtVideo.Width - 16 ||  m_videoType.nFrameWidth >= fmtVideo.Width + 16 || 
			m_videoType.nFrameHeight <= fmtVideo.Height - 16 || m_videoType.nFrameHeight >= fmtVideo.Height + 16)
		{
			VOLOGI ("Name: %s. Video size was changed. W: %d  H %d", m_pName, fmtVideo.Width, fmtVideo.Height);

			pOutput->nFilledLen = 0;
			m_pOutput->ReturnBuffer (pOutput);
			m_pOutputBuffer = NULL;
			*pFilled = OMX_FALSE;

			m_videoType.nFrameWidth = fmtVideo.Width;
			m_videoType.nFrameHeight = fmtVideo.Height;

			OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
			m_pOutput->GetPortType (&pPortType);
			pPortType->format.video.nFrameWidth = fmtVideo.Width;
			pPortType->format.video.nFrameHeight = fmtVideo.Height;
			pPortType->nBufferSize = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;

			if (m_pCallBack != NULL)
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventPortSettingsChanged, 1, 0, NULL);

			return OMX_ErrorNone;
		}
	}

	if (nRC == VO_ERR_INPUT_BUFFER_SMALL)
	{
		*pEmptied = OMX_TRUE;
		m_inBuffer.Length = 0;

		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}
	else if (nRC == VO_ERR_NONE && m_outBuffer.Buffer[0] != NULL)
	{
		if (m_pOutput->IsTunnel ())
		{
			pOutput->nFilledLen = sizeof (VO_VIDEO_BUFFER);
			memcpy (pOutput->pBuffer, &m_outBuffer, pOutput->nFilledLen);
		}
		else
		{
			VO_PBYTE pDstBuffer = pOutput->pBuffer;
			fmtVideo.Width = (fmtVideo.Width + 3) & -4;
			for (VO_U32 i = 0; (int)i < fmtVideo.Height; i++)
				memcpy (pDstBuffer + fmtVideo.Width * i, m_outBuffer.Buffer[0] + m_outBuffer.Stride[0] * i, fmtVideo.Width);

			VO_U32 nHalfWidth = fmtVideo.Width / 2;
			pDstBuffer = pOutput->pBuffer + fmtVideo.Width * fmtVideo.Height;
			for (VO_U32 i = 0; (int)i < fmtVideo.Height / 2; i++)
				memcpy (pDstBuffer + nHalfWidth * i, m_outBuffer.Buffer[1] + m_outBuffer.Stride[1] * i, nHalfWidth);

			pDstBuffer = pOutput->pBuffer + fmtVideo.Width * fmtVideo.Height * 5 / 4;
			for (VO_U32 i = 0; (int)i < fmtVideo.Height / 2; i++)
				memcpy (pDstBuffer + nHalfWidth * i, m_outBuffer.Buffer[2] + m_outBuffer.Stride[2] * i, nHalfWidth);

			pOutput->nFilledLen = fmtVideo.Width * fmtVideo.Height * 3 / 2;
		}

	//	if (m_outBuffer.Time == 0)
			pOutput->nTimeStamp = m_nStartTime;
	//	else
	//		pOutput->nTimeStamp = m_outBuffer.Time;

		m_nStartTime = m_nStartTime + m_nFrameTime;

		*pFilled = OMX_TRUE;
		if(m_inBuffer.Length <= 0)
			*pEmptied = OMX_TRUE;
	}
	else
	{
		if (nRC == VO_ERR_NONE && m_inBuffer.Length > 0)
		{
			*pEmptied = OMX_FALSE;
		}
		else
		{
			*pEmptied = OMX_TRUE;
			m_inBuffer.Length = 0;
		}

		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}
*/
	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXVideoDec::InitPortType (void)
{
	VOLOGF ("Name: %s", m_pName);

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
