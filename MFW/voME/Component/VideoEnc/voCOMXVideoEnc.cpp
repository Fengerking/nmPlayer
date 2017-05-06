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
#include "voOMX_Index.h"
#include "voOMXBase.h"
#include "voMPEG4.h"
#include "voH264.h"
#include "voCOMXVideoEnc.h"

#define LOG_TAG "voCOMXVideoEnc"
#include "voLog.h"

voCOMXVideoEnc::voCOMXVideoEnc(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_pVideoEnc (NULL)
	, m_pInputData (NULL)
	, m_nQuality (VO_ENC_MID_QUALITY)
	, m_nKeyFrameInterval (15)
	, m_nRotateType (VO_RT_DISABLE)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Video.Encoder.XXX");

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;

	m_videoType.eCompressionFormat	= OMX_VIDEO_CodingMPEG4;
	m_videoType.eColorFormat		= OMX_COLOR_FormatYUV420PackedPlanar;
	m_videoType.nFrameWidth			= 352;
	m_videoType.nFrameHeight		= 288;
	m_videoType.nStride				= 352;

	m_inBuffer.ColorType  = VO_COLOR_YUV_PLANAR420;
}

voCOMXVideoEnc::~voCOMXVideoEnc(void)
{
	if (m_pVideoEnc != NULL)
		delete m_pVideoEnc;

	if (m_pInputData != NULL)
		delete []m_pInputData;
}

OMX_ERRORTYPE voCOMXVideoEnc::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
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
	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;
	switch (nIndex)
	{
	case OMX_IndexParamPortDefinition:
		{
			OMX_PARAM_PORTDEFINITIONTYPE * pType = (OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParameterStructure;
			errType = CheckParam (pComp, pType->nPortIndex, pType, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pType->eDomain != OMX_PortDomainVideo)
				return OMX_ErrorPortsNotCompatible;

			errType = m_ppPorts[pType->nPortIndex]->SetPortType (pType);
			if (errType != OMX_ErrorNone)
				return errType;

			if (pType->nPortIndex == 0)
			{
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
				pInputType->format.video.nStride      = m_videoType.nStride;
				pInputType->format.video.nBitrate     = m_videoType.nBitrate;
				pInputType->format.video.xFramerate   = m_videoType.xFramerate;
				pInputType->format.video.eColorFormat = m_videoType.eColorFormat;

				pInputType->nBufferSize = m_videoType.nStride * m_videoType.nFrameHeight * 3 / 2;

				switch(m_videoType.eColorFormat)
				{
				case OMX_COLOR_FormatYUV420Planar:
					m_inBuffer.ColorType  = VO_COLOR_YUV_PLANAR420;
					break;

				case OMX_COLOR_FormatYUV420PackedPlanar:
					m_inBuffer.ColorType  = VO_COLOR_YUV_420_PACK_2;
					break;

				case OMX_COLOR_FormatYCbYCr:
					m_inBuffer.ColorType  = VO_COLOR_YUYV422_PACKED;
					break;

				case OMX_COLOR_FormatCbYCrY:
					m_inBuffer.ColorType  = VO_COLOR_UYVY422_PACKED;
					break;

				case OMX_COLOR_FormatYUV420SemiPlanar:
					m_inBuffer.ColorType  = VO_COLOR_YUV_420_PACK;
					break;

				case OMX_COLOR_FormatYUV420PackedSemiPlanar:
					m_inBuffer.ColorType  = VO_COLOR_YUV_420_PACK;
					break;

				default:
					break;
				}

				// Output
				OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
				m_pOutput->GetPortType (&pPortType);
				pPortType->format.video.nFrameWidth = m_videoType.nFrameWidth;
				pPortType->format.video.nFrameHeight = m_videoType.nFrameHeight;
				pPortType->nBufferSize = m_videoType.nStride * m_videoType.nFrameHeight * 3 / 2;

				VOLOGI ("Name: %s. Input W %d H %d Color %d, CF %d", m_pName, (int)m_videoType.nFrameWidth, (int)m_videoType.nFrameHeight, (int)m_inBuffer.ColorType, (int)m_videoType.eColorFormat);
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
				errType = OMX_ErrorNone;
			else if (pVideoFormat->nPortIndex == 1)
			{
				m_videoType.eCompressionFormat = pVideoFormat->eCompressionFormat;
				if (m_videoType.eCompressionFormat == OMX_VIDEO_CodingH263 || 
					m_videoType.eCompressionFormat == OMX_VIDEO_CodingMPEG4 ||
					m_videoType.eCompressionFormat == OMX_VIDEO_CodingAVC)
				{
					errType = OMX_ErrorNone;
				}
				else
					errType = OMX_ErrorComponentNotFound;
			}
		}
		break;

	case OMX_IndexParamStandardComponentRole:
		{
			OMX_PARAM_COMPONENTROLETYPE * pRoleType = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;
			errType = voOMXBase_CheckHeader(pRoleType, sizeof(OMX_PARAM_COMPONENTROLETYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			VOLOGI ("OMX_IndexParamStandardComponentRole %s", (char*)pRoleType->cRole);

			m_videoType.eCompressionFormat = OMX_VIDEO_CodingUnused;
			if (!strcmp ((char *)pRoleType->cRole, "video_encoder.mpeg4"))
				m_videoType.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
			else if (!strcmp ((char *)pRoleType->cRole, "video_encoder.h263"))
				m_videoType.eCompressionFormat = OMX_VIDEO_CodingH263;
			else if(!strcmp ((char *)pRoleType->cRole, "video_encoder.avc"))
				m_videoType.eCompressionFormat = OMX_VIDEO_CodingAVC;

			if (m_videoType.eCompressionFormat != OMX_VIDEO_CodingUnused)
				errType = OMX_ErrorNone;
			else
				errType = OMX_ErrorInvalidComponent;
		}
		break;

	case OMX_VO_IndexVideoQuality:
		{
			OMX_U32 nQuality = *((OMX_U32 *)pComponentParameterStructure);
			if(nQuality == 0)
				m_nQuality = VO_ENC_LOW_QUALITY;
			else if(nQuality == 1)
				m_nQuality = VO_ENC_MID_QUALITY;
			else if(nQuality == 2)
				m_nQuality = VO_ENC_HIGH_QUALITY;
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
	else if (nIndex == 2)
		strcpy ((char*)cRole, "video_encoder.avc");
	else 
		return OMX_ErrorNoMore;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoEnc::SetNewState (OMX_STATETYPE newState)
{
	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_pVideoEnc == NULL)
		{
			VO_U32 nCodec = VO_VIDEO_CodingUnused;
			if (m_videoType.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
				nCodec = VO_VIDEO_CodingMPEG4;
			else if (m_videoType.eCompressionFormat == OMX_VIDEO_CodingH263)
				nCodec = VO_VIDEO_CodingH263;
			else if(m_videoType.eCompressionFormat == OMX_VIDEO_CodingAVC)
				nCodec = VO_VIDEO_CodingH264;

			m_pVideoEnc = new CVideoEncoder (NULL, nCodec, m_pMemOP);
			if (m_pVideoEnc == NULL)
				return OMX_ErrorResourcesLost;

			m_pVideoEnc->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibOP);
			m_pVideoEnc->SetWorkPath ((VO_TCHAR *)m_pWorkPath);
			VO_U32 nRC = m_pVideoEnc->Init ();
			if (nRC != VO_ERR_NONE)
			{
				VOLOGE ("m_pVideoEnc->Init failed. 0X%08X", (int)nRC);
				return OMX_ErrorResourcesLost;
			}

			// get output port settings
			OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
			m_pOutput->GetPortType (&pPortType);

			float fFrameRate = 29.97f;
			if(pPortType->format.video.xFramerate != 0)
				fFrameRate = float((pPortType->format.video.xFramerate >> 16) & 0xFFFF) / (pPortType->format.video.xFramerate & 0xFFFF);

			OMX_U32 nBitrate = pPortType->format.video.nBitrate;
			if(nBitrate == 0)
			{
				nBitrate = pPortType->format.video.nFrameWidth * pPortType->format.video.nFrameHeight;
				if(m_nQuality == VO_ENC_LOW_QUALITY)
					nBitrate *= 3;
				else if(m_nQuality == VO_ENC_MID_QUALITY)
					nBitrate *= 4;
				else
					nBitrate *= 6;
			}

			if (nCodec == VO_VIDEO_CodingH264)
			{
				VO_H264FILEFORMAT eStreamType = VO_H264_ANNEXB_COMPLETE;
				nRC = m_pVideoEnc->SetParam (VO_ID_H264_STREAMFORMAT,			&eStreamType);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_H264_WIDTH,				&m_videoType.nFrameWidth);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_H264_HEIGHT,			&m_videoType.nFrameHeight);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_H264_BITRATE,			&nBitrate);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_H264_VIDEO_QUALITY,		&m_nQuality);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_H264_FRAMERATE,			&fFrameRate);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_H264_KEYFRAME_INTERVAL,	&m_nKeyFrameInterval);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_H264_INPUT_ROTATION,	&m_nRotateType);
			}
			else if (nCodec == VO_VIDEO_CodingH263 || nCodec == VO_VIDEO_CodingMPEG4)
			{
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_MPEG4_WIDTH,				&m_videoType.nFrameWidth);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_MPEG4_HEIGHT,				&m_videoType.nFrameHeight);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_MPEG4_BITRATE,				&nBitrate);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_MPEG4_VIDEO_QUALITY,		&m_nQuality);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_MPEG4_FRAMERATE,			&fFrameRate);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_MPEG4_KEY_FRAME_INTERVAL,	&m_nKeyFrameInterval);
				nRC = m_pVideoEnc->SetParam (VO_PID_ENC_MPEG4_INPUT_ROTATION,		&m_nRotateType);
			}
		}
	}
	else if (newState == OMX_StateIdle && (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle))
		ResetPortBuffer ();

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXVideoEnc::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	
	voCOMXAutoLock lokc (&m_mutCodec);

	if (m_pVideoEnc == NULL)
		return OMX_ErrorInvalidState;

	if(pInput->nFilledLen == 0)
		return OMX_ErrorNone;

	OMX_U32 nRC = 0;

	if (pInput->nFilledLen == sizeof (VO_VIDEO_BUFFER))
	{
		if (m_pInputData == NULL)
			m_pInputData = new VO_BYTE[m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2];

		VO_VIDEO_BUFFER * pVideoBuf = (VO_VIDEO_BUFFER *)pInput->pBuffer;

		for (OMX_U32 i = 0; i < m_videoType.nFrameHeight; i++)
			memcpy (m_pInputData + m_videoType.nStride * i, pVideoBuf->Buffer[0] + pVideoBuf->Stride[0] * i, m_videoType.nStride);
		for (OMX_U32 i = 0; i < m_videoType.nFrameHeight/2; i++)
			memcpy (m_pInputData + m_videoType.nStride * m_videoType.nFrameHeight + m_videoType.nStride / 2 * i, pVideoBuf->Buffer[1] + pVideoBuf->Stride[1] * i, m_videoType.nStride / 2);
		for (OMX_U32 i = 0; i < m_videoType.nFrameHeight/2; i++)
			memcpy (m_pInputData + (m_videoType.nStride * m_videoType.nFrameHeight * 5 / 4) + m_videoType.nStride / 2 * i, pVideoBuf->Buffer[2] + pVideoBuf->Stride[2] * i, m_videoType.nStride / 2);

		m_inBuffer.Buffer[0] = m_pInputData;
		m_inBuffer.Buffer[1] = m_pInputData + m_videoType.nStride * m_videoType.nFrameHeight;
		m_inBuffer.Buffer[2] = m_inBuffer.Buffer[1] + m_videoType.nStride * m_videoType.nFrameHeight / 4;
		m_inBuffer.Stride[0] = m_videoType.nStride;
		m_inBuffer.Stride[1] = m_videoType.nStride >> 1;
		m_inBuffer.Stride[2] = m_videoType.nStride >> 1;
	}
	else if(m_inBuffer.ColorType == VO_COLOR_YUV_420_PACK_2 || m_inBuffer.ColorType == VO_COLOR_YUV_420_PACK)
	{
		if(pInput->nFilledLen == 4096 * m_videoType.nFrameHeight * 3 / 2)
		{
			m_inBuffer.Buffer[0] = pInput->pBuffer + 0;
			m_inBuffer.Buffer[1] = pInput->pBuffer + 4096 * m_videoType.nFrameHeight;
			m_inBuffer.Buffer[2] = NULL;
			m_inBuffer.Stride[0] = 4096;
			m_inBuffer.Stride[1] = 4096 ;
			m_inBuffer.Stride[2] = 0;
		}
		else if(pInput->nFilledLen > 4096 * m_videoType.nFrameHeight * 3 / 2)
		{
			OMX_U32  nFrameHeight = pInput->nFilledLen * 2 / 3 / 4096;
			m_inBuffer.Buffer[0] = pInput->pBuffer + 0;
			m_inBuffer.Buffer[1] = pInput->pBuffer + 4096 *nFrameHeight;
			m_inBuffer.Buffer[2] = NULL;
			m_inBuffer.Stride[0] = 4096;
			m_inBuffer.Stride[1] = 4096 ;
			m_inBuffer.Stride[2] = 0;

			if (m_pInputData == NULL)
				m_pInputData = new VO_BYTE[m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2];

			VO_BYTE *pDst = m_pInputData;
			VO_BYTE *pSrc = m_inBuffer.Buffer[0];
			for(OMX_U32 i = 0 ; i <  m_videoType.nFrameHeight ; i++)
			{
				memcpy(pDst , pSrc , m_videoType.nFrameWidth);
				pDst += m_videoType.nFrameWidth;
				pSrc += m_inBuffer.Stride[0];
			}

			pSrc = m_inBuffer.Buffer[1];
			for(OMX_U32 i = 0 ; i <  m_videoType.nFrameHeight / 2 ; i++)
			{
				memcpy(pDst , pSrc , m_videoType.nFrameWidth);
				pDst += m_videoType.nFrameWidth;
				pSrc += m_inBuffer.Stride[1];
			}

			m_inBuffer.Buffer[0] = m_pInputData + 0;
			m_inBuffer.Buffer[1] = m_pInputData + m_videoType.nFrameWidth * m_videoType.nFrameHeight;
			m_inBuffer.Stride[0] = m_videoType.nFrameWidth;
			m_inBuffer.Stride[1] = m_videoType.nFrameWidth;
		}
		else
		{
			m_inBuffer.Buffer[0] = pInput->pBuffer + 0;
			m_inBuffer.Buffer[1] = pInput->pBuffer + m_videoType.nFrameWidth * m_videoType.nFrameHeight;
			m_inBuffer.Buffer[2] = NULL;
			m_inBuffer.Stride[0] = m_videoType.nFrameWidth;
			m_inBuffer.Stride[1] = m_videoType.nFrameWidth ;
			m_inBuffer.Stride[2] = 0;
		}
	}
	else if(m_inBuffer.ColorType == VO_COLOR_UYVY422_PACKED)
	{
		m_inBuffer.Buffer[0] = pInput->pBuffer + 0;
		m_inBuffer.Buffer[1] = NULL;
		m_inBuffer.Buffer[2] = NULL;
		m_inBuffer.Stride[0] = m_videoType.nFrameWidth * 2;
		m_inBuffer.Stride[1] = 0;
		m_inBuffer.Stride[2] = 0;
	}
	else
	{
		m_inBuffer.Buffer[0] = pInput->pBuffer;
		m_inBuffer.Buffer[1] = pInput->pBuffer + m_videoType.nStride * m_videoType.nFrameHeight;
		m_inBuffer.Buffer[2] = m_inBuffer.Buffer[1] + m_videoType.nStride * m_videoType.nFrameHeight / 4;
		m_inBuffer.Stride[0] = m_videoType.nStride;
		m_inBuffer.Stride[1] = m_videoType.nStride >> 1;
		m_inBuffer.Stride[2] = m_videoType.nStride >> 1;
	}
	m_outBuffer.Buffer   = pOutput->pBuffer;
	m_outBuffer.Length   = pOutput->nAllocLen;
	
	VO_VIDEO_FRAMETYPE frmType;
	nRC = m_pVideoEnc->Process (&m_inBuffer, &m_outBuffer, &frmType);
	VOLOGR("Name: %s. m_inBuffer.ColorType %d  GetOutputData Size: %d, Result 0x%08X", m_pName, m_inBuffer.ColorType , m_outBuffer.Length, nRC);

	pOutput->nFilledLen = m_outBuffer.Length;
	pOutput->nTimeStamp = pInput->nTimeStamp;
	pOutput->nFlags = (frmType == VO_VIDEO_FRAME_I) ? OMX_BUFFERFLAG_SYNCFRAME : 0;

	*pEmptied = OMX_TRUE;
	*pFilled = OMX_TRUE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoEnc::InitPortType (void)
{
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
