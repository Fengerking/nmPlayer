
#include <stdio.h>
#include <string.h>
#include <OMX_Video.h>
#include <utils/Log.h>

#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"

#include  "voComxNameDefine.h"
#include "voComSamsVideoDec.h"
#include "SamsVideoDec.h"
#include "SamsVideoMP4Dec.h"
#include "SamsVideoH264Dec.h"


//#define _VOLOG_FUNC
//#define _VOLOG_STAUS
//#define _VOLOG_RUN

#define LOG_TAG "voComSamsVideoDec"

#include "voLog.h"

voComSamsVideoDec::voComSamsVideoDec(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_nCoding (OMX_VIDEO_CodingMPEG4)
	, m_nExtSize (0)
	, m_pExtData (NULL)
	, m_nFrameTime (1)
	, m_nPrevSystemTime (0)
	, m_nStartTime (0)
	, m_pDec(NULL)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, VIDEO_SAMS_VIDEO_DEC);

	m_inBuffer.Length = 0;
	m_outBuffer.Time = 0;

	memset (&m_videoType, 0, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));
	
	VOLOGF ("Name %s", m_pName);
}

voComSamsVideoDec::~voComSamsVideoDec(void)
{
	VOLOGF ("Name %s", m_pName);

	if (m_pExtData != NULL)
	{
		voOMXMemFree (m_pExtData);
		m_pExtData = NULL;
	}
	if(m_pDec)
	{
		m_pDec->deinit();
		delete m_pDec;
		m_pDec = NULL;
	}

	
//	m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);
}


OMX_ERRORTYPE voComSamsVideoDec::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
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


OMX_ERRORTYPE voComSamsVideoDec::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
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

				if (m_nCoding == OMX_VIDEO_CodingH263)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingMPEG4)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingAVC)
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
/*
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
*/
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
			
			//errType = voOMXBase_CheckHeader(pExtData, sizeof(OMX_OTHER_EXTRADATATYPE));
			//if (errType != OMX_ErrorNone)
			//	return errType;

			if (m_pExtData != NULL)
				voOMXMemFree (m_pExtData);
			m_pExtData = NULL;
			m_nExtSize = 0;

			if (pExtData->nDataSize > 0)
			{
				m_nExtSize = pExtData->nDataSize;
				m_pExtData = (OMX_S8*)voOMXMemAlloc (m_nExtSize);
				if (m_pExtData != NULL)
					voOMXMemCopy (m_pExtData, pExtData->data, m_nExtSize);
				else
					m_nExtSize = 0;
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
			if (!strcmp ((char *)pRoleType->cRole, "video_decoder.mpeg4"))
				m_nCoding = OMX_VIDEO_CodingMPEG4;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.h263"))
				m_nCoding = OMX_VIDEO_CodingH263;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.avc"))
				m_nCoding = OMX_VIDEO_CodingAVC;

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

OMX_ERRORTYPE voComSamsVideoDec::ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_OUT OMX_U8 *cRole,
													OMX_IN OMX_U32 nIndex)
{
	strcat((char*)cRole, "\0");
	if (nIndex == 0)
		strcpy ((char*)cRole, "video_decoder.mpeg4");
	else if (nIndex == 1)
		strcpy ((char*)cRole, "video_decoder.h263");
	else if (nIndex == 2)
		strcpy ((char*)cRole, "video_decoder.avc");

	else
		return OMX_ErrorNoMore;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voComSamsVideoDec::SetNewState (OMX_STATETYPE newState)
{
	VOLOGF ("Name: %s. New State %d", m_pName, newState);

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{	
		if (m_pDec == NULL)
		{	
			int nCodec = -1;
			
			if (m_nCoding == OMX_VIDEO_CodingMPEG4)
			{
				nCodec = 0 ; // means MFC_CODEC_MP4D
				m_pDec = new SamsVideoMP4Dec();
			}
			else if (m_nCoding == OMX_VIDEO_CodingH263)
			{
				nCodec = 1;  // means MFC_CODEC_H263D
				m_pDec = new SamsVideoMP4Dec();
			}
			else if( m_nCoding == OMX_VIDEO_CodingAVC)
			{
				nCodec = 2;
				m_pDec = new SamsVideoH264Dec();
			}

			if(nCodec == -1)
			{
				VOLOGE("nCodec can not assign right value");
				return OMX_ErrorInsufficientResources;
			}
			if (m_pDec == NULL && nCodec != -1)
			{
				VOLOGE ("It could not create the video docoder.");
				return OMX_ErrorInsufficientResources;
			}
					
			m_pDec->setCodecType(nCodec);
			
			m_pDec->init((unsigned char*)m_pExtData, m_nExtSize);
		}	
			

	}
	else if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{
		/*
		  if (m_pVideoDec != NULL)
		  m_pVideoDec->Flush ();
		*/
		m_inBuffer.Length = 0;
	}
	else if (newState == OMX_StateIdle)
	{
		/*
		  if (m_pVideoDec != NULL)
		  m_pVideoDec->Flush ();
		*/
		m_inBuffer.Length = 0;
		m_nStartTime = 0;
	}

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voComSamsVideoDec::Flush (OMX_U32	nPort)
{
	VOLOGF ("Name: %s", m_pName);
	/*
	if (m_pVideoDec != NULL)
		m_pVideoDec->Flush ();
	*/	
	OMX_ERRORTYPE errType = voCOMXCompFilter::Flush (nPort);

	m_inBuffer.Length = 0;
	m_nStartTime = 0;

	return errType;
}

OMX_ERRORTYPE voComSamsVideoDec::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	VOLOGR ("Name: %s. Size: %d, Time %d", m_pName, pInput->nFilledLen, (int)pInput->nTimeStamp);

	
	OMX_U32 nRC = 0;

	if (pInput->nFilledLen <= 0)
	{
		*pEmptied = OMX_TRUE;
		return OMX_ErrorNone;
	}
	
	if (m_inBuffer.Length == 0)
	{
		
		m_inBuffer.Buffer = pInput->pBuffer + pInput->nOffset;
		m_inBuffer.Length = pInput->nFilledLen;
		m_inBuffer.Time = pInput->nTimeStamp;

		//nRC = m_pVideoDec->SetInputData (&m_inBuffer);
		nRC = m_pDec->setInputData(m_inBuffer.Buffer, m_inBuffer.Length);
		if (nRC != 1)
		{
			*pEmptied = OMX_TRUE;
			m_inBuffer.Length = 0;

			pOutput->nFilledLen = 0;
			*pFilled = OMX_FALSE;

			VOLOGW ("Set Input Failed! 0X%08X", nRC);
			return OMX_ErrorNone;
		}

		m_nStartTime = pInput->nTimeStamp;
	}

	//VOLOGR ("Name: %s. GetOutputData Video Size: W %d H %d", m_pName, m_videoType.nFrameWidth,  m_videoType.nFrameHeight);

	VO_VIDEO_FORMAT		fmtVideo;
	memset(&fmtVideo, 0, sizeof(VO_VIDEO_FORMAT));
	//nRC = m_pVideoDec->GetOutputData (&m_outBuffer, &fmtVideo);
	m_outBuffer.Time = 0;
	nRC = GetOutputData (&m_outBuffer, &fmtVideo, (VO_U32)pInput->nTimeStamp);

	VOLOGR ("Name: %s. GetOutputData Size: 0X%08X, Result %d. Video Size: W %d H %d", m_pName, m_outBuffer.Buffer[0], nRC, fmtVideo.Width, fmtVideo.Height);

	if (nRC == VO_ERR_NONE && fmtVideo.Width > 0 && fmtVideo.Height > 0)
	{
		OMX_BOOL bChanged = OMX_TRUE;

		if (m_nCoding == OMX_VIDEO_CodingH263)
		{
			if (m_videoType.nFrameWidth == 320 && fmtVideo.Width == 352 && 
				m_videoType.nFrameHeight == 240 && fmtVideo.Height == 288)
			{
				bChanged = OMX_FALSE;
			}
		}

//		if (m_videoType.nFrameWidth != fmtVideo.Width || m_videoType.nFrameHeight != fmtVideo.Height)
		if (bChanged && (m_videoType.nFrameWidth <= fmtVideo.Width - 16 ||  m_videoType.nFrameWidth >= fmtVideo.Width + 16 || 
			m_videoType.nFrameHeight <= fmtVideo.Height - 16 || m_videoType.nFrameHeight >= fmtVideo.Height + 16))
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
		if ((pInput->nFlags & OMX_BUFFERFLAG_DECODEONLY) == OMX_BUFFERFLAG_DECODEONLY)
		{
			if ((pInput->nFlags & OMX_BUFFERFLAG_EOS) == 0)
			{
				if(m_inBuffer.Length <= 0)
					*pEmptied = OMX_TRUE;

				pOutput->nFilledLen = 0;
				*pFilled = OMX_FALSE;

				return  OMX_ErrorNone;
			}
		}

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
		
		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}

	return OMX_ErrorNone;
}

VO_U32 voComSamsVideoDec::GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat, VO_U32 nStart)
{
	VOLOGF ("Name: %s", m_pName);

	if (m_nPfmFrameIndex < m_nPfmFrameSize)
	{
		m_pPfmFrameTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime ();
		m_pPfmCodecThreadTime[m_nPfmFrameIndex] = voOMXOS_GetThreadTime (NULL);
		m_pPfmCompnThreadTime[m_nPfmFrameIndex] = m_pPfmCodecThreadTime[m_nPfmFrameIndex];
		m_pPfmMediaTime[m_nPfmFrameIndex] = (VO_U32) nStart;
		m_pPfmSystemTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime ();
	}


	m_pDec->getVideoInfo(pVideoFormat->Width, pVideoFormat->Height);
	
	unsigned char* pBuffer = NULL;
	unsigned int  nSize = 0;
	pBuffer = m_pDec->getOutputData(&nSize);
	unsigned int nYUVWidth = (pVideoFormat->Width + 15) & ~0xF;
	unsigned int nYUVHeight = (pVideoFormat->Height + 15) & ~0xF;	

	if(pBuffer)
	{
		pOutput->Buffer[0] = pBuffer;
		pOutput->Buffer[1] = pBuffer + nYUVWidth * nYUVHeight;
		pOutput->Buffer[2] = pBuffer + nYUVWidth * nYUVHeight * 5 / 4;

		pOutput->Stride[0] = nYUVWidth;
		pOutput->Stride[1] = pOutput->Stride[2] = nYUVWidth / 2;
		pOutput->ColorType = VO_COLOR_YUV_PLANAR420 ;

		m_inBuffer.Length = 0;
	}
	/*
	VO_U32	nRC = m_pVideoDec->GetOutputData (pOutput, pVideoFormat);

	if (nRC == VO_ERR_NONE && pOutput->Buffer[0] !=  NULL)
	{
		if (m_nPfmFrameIndex < m_nPfmFrameSize)
		{
			m_pPfmFrameTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime () - m_pPfmFrameTime[m_nPfmFrameIndex];
			m_pPfmCodecThreadTime[m_nPfmFrameIndex] = voOMXOS_GetThreadTime (NULL) - m_pPfmCodecThreadTime[m_nPfmFrameIndex];
		}
		m_nPfmFrameIndex++;
	}
	*/
	return VO_ERR_NONE;
	
}


OMX_ERRORTYPE voComSamsVideoDec::InitPortType (void)
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
