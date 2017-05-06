	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoSink.cpp

	Contains:	voCOMXVideoSink class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXOSFun.h"
#include "voOMXFile.h"
#include "voOMXMemory.h"
#include "voCOMXVideoSink.h"

#define LOG_TAG "voCOMXVideoSink"
#include "voLog.h"

#define OMX_COLOR_Format_QcomHW		0X7FA30C00
#define OMX_COLOR_Format_QcomHW2	0X7FA30C03	//for 8660
#define OMX_COLOR_Format_NvidiaHW	0x32315659	// HAL_PIXEL_FORMAT_YV12 YCrCb 4:2:0 Planar
#define OMX_COLOR_Format_SamSuangHW 0x7F000001 // SamSung own color format
#define OMX_COLOR_Format_SamSuangFK 0x7F000010 // Fake SamSung's hardware decoder
 
// STE Proprietary YUV420 MBTiled format
#define OMX_COLOR_FormatSTYUV420PackedSemiPlanarMB (OMX_COLOR_FormatMax - 1)
#define OMX_COLOR_Format_TI_DUCATI1 0x100  // Galaxy Nexus TI's platform

#define VO_VIDEOSINK_MAXDROPFRAME	10

extern VO_TCHAR * g_pvoOneWorkingPath;

voCOMXVideoSink::voCOMXVideoSink(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSink (pComponent)
	, m_pClockPort (NULL)
	, m_pRenderBuffer (NULL)
	, m_pHoldBuffer (NULL)
	, m_pVideoRender (NULL)
	, m_pMemOP (NULL)
	, m_nColorType (VO_COLOR_YUV_PLANAR420)
	, m_nRendFrames (0)
	, m_nTotalRendFrames (0)
	, m_nTotalExecutingTime (0)
	, m_nExecutingStartTime (-1)
	, m_nRenderSpeed (0)
	, m_nFrameDropped (VO_VIDEOSINK_MAXDROPFRAME)
	, m_bDropVideo (VO_TRUE)
	, m_nWrongStepTime (30000)
	, m_bThread (OMX_TRUE)
	, m_llLastFrameTime (0)
	, m_bHoldBuffer (OMX_FALSE)
	, m_bDualCore (OMX_FALSE)
	, m_pDumpInputFile (NULL)
	, m_pFRateMeter(NULL)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);

	strcpy (m_pName, "OMX.VisualOn.Video.Sink");

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;

	m_cbRenderBuffer.pCallBack = NULL;
	m_cbRenderBuffer.pUserData = NULL;

	m_videoFormat.eColorFormat = OMX_COLOR_FormatYUV420Planar;

	memset (&m_rcDisp, 0, sizeof (m_rcDisp));

	VO_TCHAR szCfgFile[256];
	if (g_pvoOneWorkingPath == NULL)
		vostrcpy (szCfgFile, _T("vomeplay.cfg"));
	else
	{
		vostrcpy (szCfgFile, g_pvoOneWorkingPath);
		vostrcat (szCfgFile, _T("vomeplay.cfg"));
	}

	m_pCfgComponent = new CBaseConfig ();
	m_pCfgComponent->Open (szCfgFile);
	if (m_pCfgComponent->GetItemValue (m_pName, (char*)"DropVideo", 1) == 0)
		m_bDropVideo = VO_FALSE;
	if (m_pCfgComponent->GetItemValue (m_pName, (char*)"BufferThread", 1) == 0)
		m_bThread = OMX_FALSE;
	if (m_pCfgComponent->GetItemValue (m_pName, (char*)"DualCore", 0) > 0)
		m_bDualCore = OMX_TRUE;
	m_nWrongStepTime = m_pCfgComponent->GetItemValue (m_pName, (char*)"WrongStepTime", 30000);
	m_nLogLevel = m_pCfgComponent->GetItemValue (m_pName, (char*)"ShowLogLevel", 0);
	m_nDumpRuntimeLog = m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpRuntimeLog", 0);

	char * pFile = m_pCfgComponent->GetItemText (m_pName, (char*)"DumpInputDataFile");
	if (pFile != NULL)
	{
#ifdef _WIN32
		TCHAR szFile[256];
		memset (szFile, 0, sizeof (szFile));
		MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); 
		m_pDumpInputFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_WRITE_ONLY);		
#else
		m_pDumpInputFile = voOMXFileOpen (pFile, VOOMX_FILE_WRITE_ONLY);
#endif // _WIN32
		VOLOGI ("Create Input file %s, The handle is 0X%08X.", pFile, (int)m_pDumpInputFile);
	}

	if (m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpLog", 0) > 0)
	{
		m_nPfmFrameSize = 10240;
		m_pPfmFrameTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmCodecThreadTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmCompnThreadTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmMediaTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmSystemTime = new OMX_U32[m_nPfmFrameSize];
	}

	if (m_pCfgComponent->GetItemValue ((char*)"vomeMF", (char*)"DumpLevel", 0) & 0x200)
	{
		m_pFRateMeter = new FrameRator("Video", FR_MAX_VIDEO_FRAME_RATE);
	}

	m_pCfgCodec = new CBaseConfig ();
#if 0
	m_pCfgCodec->Open (_T("vommcodec.cfg"));
#else //Jason, 7/14/2010
	if (g_pvoOneWorkingPath == NULL)
		vostrcpy (szCfgFile, _T("vommcodec.cfg"));
	else
	{
		vostrcpy (szCfgFile, g_pvoOneWorkingPath);
		vostrcat (szCfgFile, _T("vommcodec.cfg"));
	}
#endif
	m_pCfgCodec->Open (szCfgFile);

#ifdef _WIN32
	m_pVRTimeStamp = (OMX_S32*)voOMXMemAlloc(sizeof(OMX_S32));
	m_pAudioPlayTime = (OMX_S32*)voOMXMemAlloc(sizeof(OMX_S32));
#endif
}

voCOMXVideoSink::~voCOMXVideoSink(void)
{
	if (m_pDumpInputFile != NULL)
		voOMXFileClose (m_pDumpInputFile);

	if (m_pVideoRender != NULL)
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}

	if (m_pFRateMeter != NULL)
	{
		delete m_pFRateMeter;
		m_pFRateMeter = NULL;
	}

	delete m_pCfgCodec;

	voOMXMemFree(m_pVRTimeStamp);
	voOMXMemFree(m_pAudioPlayTime);
}

int OutputOneFrame( VO_VIDEO_BUFFER* par, int w, int h, OMX_PTR outFile)
{
	int i, width,height;
	unsigned char* out_src;
	if (!outFile) {
		return 0;
	}

	if (!par->Buffer[0] || !par->Buffer[1] || !par->Buffer[2]) {
		return 0;
	}

	//printf("\nFrame %d finished, InFrameType:%d, OutFrameType:%d, ", frameNumber, iFrameType, outFormat->Format.Type);

	width  = w;
	height = h;

	/* output decoded YUV420 frame */
	/* Y */
	out_src = par->Buffer[0];
	for( i = 0;i < height; i++ ) {
		voOMXFileWrite(outFile, out_src, width);
		out_src += par->Stride[0];
	}

	/* U */
	out_src = par->Buffer[1];
	for(i = 0; i< height/2; i++ ) {
		voOMXFileWrite(outFile, out_src, width/2);
		out_src += par->Stride[1];
	}

	/* V */
	out_src = par->Buffer[2];
	for( i = 0; i < height/2; i++ ) {
		voOMXFileWrite(outFile, out_src, width/2);
		out_src += par->Stride[2];
	}

	return 0;
}

OMX_ERRORTYPE voCOMXVideoSink::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
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
				pVideoFormat->eCompressionFormat = OMX_VIDEO_CodingUnused;
				pVideoFormat->eColorFormat = OMX_COLOR_FormatYUV420Planar;
				errType = OMX_ErrorNone;
			}
		}
		break;

	case OMX_VO_IndexVideoMemOperator:
		{
			if (m_pVideoRender != NULL)
				m_pVideoRender->GetParam (VO_PID_VIDEO_VIDEOMEMOP, pComponentParameterStructure);
		}
		break;

	case OMX_VO_IndexVideoFPS:
		{
			if(m_nTotalRendFrames <= 0)
				return OMX_ErrorUndefined;

			OMX_U32 dwTotalTime = m_nTotalExecutingTime;
			if((int)m_nExecutingStartTime != -1)
				dwTotalTime += (voOMXOS_GetSysTime() - m_nExecutingStartTime);

			if(dwTotalTime <= 0)
				return OMX_ErrorUndefined;

			*((OMX_S32*)pComponentParameterStructure) = VO_S64(m_nTotalRendFrames) * 100000 / dwTotalTime;
		}
		break;

	default:
		errType = voCOMXCompSink::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXVideoSink::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE		errType = OMX_ErrorNone;
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

// 			pType->nBufferCountActual = 1;
// 			pType->nBufferCountMin = 1;

			OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
			m_pInput->GetPortType (&pPortType);
			pPortType->nBufferCountActual = pType->nBufferCountActual;

			errType = m_ppPorts[pType->nPortIndex]->SetPortType (pType);
			if (errType != OMX_ErrorNone)
				return errType;

			voOMXMemCopy (&m_videoType, &pType->format.video, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));

			m_nYUVWidth = (m_videoType.nFrameWidth + 15) & ~0XF;
			m_nYUVHeight = (m_videoType.nFrameHeight + 15) & ~0XF;

			if (m_pVideoRender != NULL)
			{
				if(isVO_PLANAR420(m_videoFormat.eColorFormat))
					m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUV_PLANAR420);
				else if (m_videoType.eColorFormat == OMX_COLOR_FormatYUV420PackedSemiPlanar)
					m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUV_420_PACK);
				else if (isVO_420_PACK_2(m_videoFormat.eColorFormat))
					m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUV_420_PACK_2);
				else if(m_videoFormat.eColorFormat == OMX_COLOR_FormatSTYUV420PackedSemiPlanarMB)
					m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, (VO_IV_COLORTYPE)(VO_COLOR_TYPE_MAX - 1));
			}
		}
		break;

	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE));
			//if (errType != OMX_ErrorNone)
			//	return errType;

			if (pVideoFormat->nPortIndex == 0)
			{
				VOLOGI ("Color Format is %d (%x)", pVideoFormat->eColorFormat, pVideoFormat->eColorFormat);

				m_videoFormat.eColorFormat = pVideoFormat->eColorFormat;

				if (m_pVideoRender != NULL)
				{
					if(isVO_PLANAR420(m_videoFormat.eColorFormat))
  						m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUV_PLANAR420);
					else if (m_videoType.eColorFormat == OMX_COLOR_FormatYUV420PackedSemiPlanar)
						m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUV_420_PACK);
					else if (isVO_420_PACK_2(m_videoFormat.eColorFormat))
						m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUV_420_PACK_2);
					else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatCbYCrY)
						m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_UYVY422_PACKED);
					else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatCrYCbY)
						m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_VYUY422_PACKED);
					else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatYCbYCr)
						m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUYV422_PACKED);
					else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatYCrYCb)
						m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YVYU422_PACKED);
					else if(m_videoFormat.eColorFormat == OMX_COLOR_FormatSTYUV420PackedSemiPlanarMB)
						m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, (VO_IV_COLORTYPE)(VO_COLOR_TYPE_MAX - 1));	
						
				}

				if(isVO_PLANAR420(pVideoFormat->eColorFormat))
				{
					m_nColorType = VO_COLOR_YUV_PLANAR420;
					return OMX_ErrorNone;
				}

				// TI OMAP4 HW Video Decoder Color Type
				if (pVideoFormat->eColorFormat == OMX_COLOR_FormatYUV420PackedSemiPlanar)
				{
					m_nColorType = VO_COLOR_YUV_420_PACK;
					return OMX_ErrorNone;
				}

				// QCOM HW Video Decoder Color Type
				if (isVO_420_PACK_2(pVideoFormat->eColorFormat))
				{
					m_nColorType = VO_COLOR_YUV_420_PACK_2;
					return OMX_ErrorNone;
				}

				// TI HW Video Decoder Color Type
				if (pVideoFormat->eColorFormat == OMX_COLOR_FormatCbYCrY)
				{
					m_nColorType = VO_COLOR_UYVY422_PACKED;
					return OMX_ErrorNone;
				}

				if (pVideoFormat->eColorFormat == OMX_COLOR_FormatCrYCbY)
				{
					m_nColorType = VO_COLOR_VYUY422_PACKED;
					return OMX_ErrorNone;
				}

				if (pVideoFormat->eColorFormat == OMX_COLOR_FormatYCbYCr)
				{
					m_nColorType = VO_COLOR_YUYV422_PACKED;
					return OMX_ErrorNone;
				}

				if (pVideoFormat->eColorFormat == OMX_COLOR_FormatYCrYCb)
				{
					m_nColorType = VO_COLOR_YVYU422_PACKED;
					return OMX_ErrorNone;
				}
				if(pVideoFormat->eColorFormat == OMX_COLOR_FormatSTYUV420PackedSemiPlanarMB)
				{
					m_nColorType = (VO_IV_COLORTYPE)(VO_COLOR_TYPE_MAX - 1);
					return OMX_ErrorNone;
				}
				errType = OMX_ErrorPortsNotCompatible;
			}
		}
		break;

	case OMX_VO_IndexRenderBufferCallBack:
		{
			OMX_VO_CHECKRENDERBUFFERTYPE * pRenderType = (OMX_VO_CHECKRENDERBUFFERTYPE *)pComponentParameterStructure;
			m_cbRenderBuffer.pCallBack = pRenderType->pCallBack;
			m_cbRenderBuffer.pUserData = pRenderType->pUserData;
		}
		break;

	case OMX_VO_IndexRenderHoldBuffer:
		{
			m_bHoldBuffer = *(OMX_BOOL*)pComponentParameterStructure;
			VOLOGI ("m_bHoldBuffer %d", m_bHoldBuffer);
		}
		break;

	case OMX_VO_IndexVideoDataBuffer:
		{
			OMX_VO_VIDEOBUFFERTYPE * pRenderDataBuffer = (OMX_VO_VIDEOBUFFERTYPE *)pComponentParameterStructure;
			if (m_pVideoRender != NULL)
			{
				VO_VIDEO_BUFFER vidBuffer;
				vidBuffer.Buffer[0] = pRenderDataBuffer->Buffer[0];
				vidBuffer.Buffer[1] = pRenderDataBuffer->Buffer[1];
				vidBuffer.Buffer[2] = pRenderDataBuffer->Buffer[2];
				vidBuffer.Stride[0] = pRenderDataBuffer->Stride[0];
				vidBuffer.Stride[1] = pRenderDataBuffer->Stride[1];
				vidBuffer.Stride[2] = pRenderDataBuffer->Stride[2];
				vidBuffer.ColorType = VO_COLOR_RGB565_PACKED;//pRenderDataBuffer->Color;
				vidBuffer.Time = pRenderDataBuffer->Width;
				vidBuffer.Time = (vidBuffer.Time << 32) + pRenderDataBuffer->Height;

				m_pVideoRender->SetParam (VO_PID_VIDEO_DATABUFFER, &vidBuffer);
			}
		}
		break;

	case OMX_VO_IndexVideoMemOperator:
		{
			if (m_pVideoRender != NULL)
			{
				m_pVideoRender->SetParam (VO_PID_VIDEO_VIDEOMEMOP, pComponentParameterStructure);
			}
		}
		break;

	default:
		errType = voCOMXCompSink::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXVideoSink::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_IN  OMX_PTR pComponentConfigStructure)
{
	switch (nIndex)
	{
	case OMX_IndexConfigTimePosition:
		{
			OMX_COMPONENTTYPE * pComp = m_pClockPort->GetTunnelComp ();
			if (pComp != NULL)
			{
				OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkType;

				pComp->GetConfig (pComp, OMX_IndexConfigTimeActiveRefClock, &clkType);
				if (clkType.eClock == OMX_TIME_RefClockVideo)
				{
					m_pClockPort->StartWallClock (0);
				}
			}
		}
		break;

	case OMX_VO_IndexConfigDisplayArea:
		{
			OMX_VO_DISPLAYAREATYPE * pDisp = (OMX_VO_DISPLAYAREATYPE *)pComponentConfigStructure;

			m_hView = pDisp->hView;

			m_rcDisp.left = pDisp->nX;
			m_rcDisp.top = pDisp->nY;
			m_rcDisp.right = pDisp->nX + pDisp->nWidth;
			m_rcDisp.bottom = pDisp->nY + pDisp->nHeight;

			if (m_pVideoRender != NULL && m_rcDisp.bottom > 0 && m_rcDisp.right > 0)
				m_pVideoRender->SetDispRect (m_hView, &m_rcDisp);

			m_nRendFrames = 0;
			m_nRenderSpeed = 0;
			m_nFrameDropped = VO_VIDEOSINK_MAXDROPFRAME;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXCompSink::SetConfig (hComponent, nIndex, pComponentConfigStructure);

}

OMX_ERRORTYPE voCOMXVideoSink::CreatePorts (void)
{
	if (m_uPorts == 0)
	{
		m_uPorts = 2;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
		if (m_ppPorts == NULL)
			return OMX_ErrorInsufficientResources;
		m_pInput = m_ppPorts[0] = new voCOMXBasePort (this, 0, OMX_DirInput);
		if (m_pInput == NULL)
			return OMX_ErrorInsufficientResources;
		m_pInput->SetCallbacks (m_pCallBack, m_pAppData);

		m_pBufferQueue = m_pInput->GetBufferQueue ();
		m_pSemaphore = m_pInput->GetBufferSem ();

		m_ppPorts[1] = new voCOMXPortClock (this, 1, OMX_DirInput);
		if (m_ppPorts[1] == NULL)
			return OMX_ErrorInsufficientResources;
		m_pClockPort = (voCOMXPortClock *)m_ppPorts[1];
		m_pClockPort->SetCallbacks (m_pCallBack, m_pAppData);
		m_pClockPort->SetClockType (OMX_TIME_RefClockVideo);

		InitPortType ();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoSink::CreateRender (void)
{
	if (m_pVideoRender == NULL)
	{
		m_pVideoRender = new CVideoRender (NULL, m_hView, m_pMemOP);
		m_pVideoRender->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibOP);
		m_pVideoRender->SetWorkPath ((VO_TCHAR *)m_pWorkPath);
		m_pVideoRender->SetConfig (m_pCfgCodec);

		if (m_cbRenderBuffer.pCallBack != NULL)
			m_pVideoRender->SetCallBack (videosinkVideoRenderProc, this);

		VO_VIDEO_FORMAT	fmtVideo;
		fmtVideo.Width = m_videoType.nFrameWidth;
		fmtVideo.Height = m_videoType.nFrameHeight;

		OMX_U32 nRC = m_pVideoRender->Init (&fmtVideo);
		nRC = nRC;
		if (m_rcDisp.right > 0 && m_rcDisp.bottom > 0)
			m_pVideoRender->SetDispRect (m_hView, &m_rcDisp);

		if(isVO_PLANAR420(m_videoFormat.eColorFormat))
			m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUV_PLANAR420);
		else if (m_videoType.eColorFormat == OMX_COLOR_FormatYUV420PackedSemiPlanar)
			m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUV_420_PACK);
		else if (isVO_420_PACK_2(m_videoFormat.eColorFormat))
			m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUV_420_PACK_2);
		else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatCbYCrY)
			m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_UYVY422_PACKED);
		else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatCrYCbY)
			m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_VYUY422_PACKED);
		else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatYCbYCr)
			m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YUYV422_PACKED);
		else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatYCrYCb)
			m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, VO_COLOR_YVYU422_PACKED);
		else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatSTYUV420PackedSemiPlanarMB)
			m_pVideoRender->SetVideoInfo (m_videoType.nFrameWidth, m_videoType.nFrameHeight, (VO_IV_COLORTYPE)(VO_COLOR_TYPE_MAX - 1));
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoSink::SetNewState (OMX_STATETYPE newState)
{
	if (newState == OMX_StateIdle)
	{
		m_llSeekTime = 0;
		if(m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
		{
			if (m_pVideoRender == NULL)
				CreateRender ();

			if (!m_bThread)
				return OMX_ErrorNone;
		}
		else
			m_nRendFrames = 0;
	}

	// Run -> Pause / Pause -> Run, we need not modify it, East 2011/03/04
	if((newState != OMX_StatePause || m_sTrans != COMP_TRANSSTATE_ExecuteToPause) && 
		(newState != OMX_StateExecuting || m_sTrans != COMP_TRANSSTATE_PauseToExecute))
	{
		m_nRendFrames = 0;
		m_llLastFrameTime = 0;
	}

	// We need start refer clock after pause if no audio, East 20110420
	if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_PauseToExecute)
	{
		if(m_pClockPort)
		{
			OMX_COMPONENTTYPE* pComp = m_pClockPort->GetTunnelComp();
			if(pComp)
			{
				OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkType;
				pComp->GetConfig (pComp, OMX_IndexConfigTimeActiveRefClock, &clkType);
				OMX_U32 nAudioStreamStop = 0;
				pComp->GetConfig (pComp, (OMX_INDEXTYPE)OMX_VO_IndexAudioStreamStop, &nAudioStreamStop);
				// reference video or audio stop
				if (clkType.eClock == OMX_TIME_RefClockVideo || nAudioStreamStop != 0)
				{
					m_pClockPort->StartWallClock (1);
					if(m_llLastFrameTime > 0)
						m_pClockPort->UpdateMediaTime(m_llLastFrameTime);
				}
			}
		}
	}

	if (newState == OMX_StateExecuting)
		m_nExecutingStartTime = voOMXOS_GetSysTime();
	else
	{
		if((int)m_nExecutingStartTime != -1)
			m_nTotalExecutingTime += (voOMXOS_GetSysTime() - m_nExecutingStartTime);
		m_nExecutingStartTime = -1;
	}

	OMX_ERRORTYPE errType = voCOMXCompSink::SetNewState (newState);

	// we should return hold buffer after set new state, otherwise check buffer status will timeout for HW codec
	if (newState == OMX_StateIdle)
		ProcHoldBuffer();

	return errType;
}

OMX_ERRORTYPE voCOMXVideoSink::Flush (OMX_U32 nPort)
{
//	voCOMXAutoLock  lock(&m_mutRender);

	ProcHoldBuffer();

	OMX_ERRORTYPE errType = voCOMXCompSink::Flush (nPort);

	m_llSeekTime = 0;
	m_nRendFrames = 0;
	m_llLastFrameTime = 0;

	return errType;
}

OMX_ERRORTYPE voCOMXVideoSink::Disable (OMX_U32 nPort)
{
	ProcHoldBuffer();

	return voCOMXCompSink::Disable (nPort);
}

OMX_ERRORTYPE voCOMXVideoSink::FreeBuffer(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_U32 nPortIndex, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	m_tmHoldBuffer.Lock();
	if(pBuffer == m_pHoldBuffer)
		m_pHoldBuffer = NULL;
	m_tmHoldBuffer.Unlock();

	return voCOMXCompSink::FreeBuffer(hComponent, nPortIndex, pBuffer);
}

OMX_ERRORTYPE voCOMXVideoSink::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_bThread)
	{
		errType = voCOMXCompSink::EmptyThisBuffer (m_pComponent, pBuffer);
		return errType;
	}
	else
	{
		errType = FillBuffer (pBuffer);

		ProcHoldBuffer();

		if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
		{
			if (m_pCallBack != NULL)
			{
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventBufferFlag, 0, pBuffer->nFlags, NULL);
			}
			FinishBuffer(pBuffer);
		}
		else
		{
			if(OMX_FALSE == SaveHoldBuffer(pBuffer))
				FinishBuffer(pBuffer);
		}

		return errType;
	}
}

OMX_U32	voCOMXVideoSink::BufferHandle (void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	m_pSemaphore->Down ();
	OMX_BUFFERHEADERTYPE * pBuffer = (OMX_BUFFERHEADERTYPE *) m_pBufferQueue->Remove ();
	if (pBuffer != NULL)
	{
		if (m_sState <= OMX_StateIdle)
		{
			ProcHoldBuffer();
			FinishBuffer(pBuffer);

			return OMX_ErrorInvalidState;
		}

		if (m_sTrans != COMP_TRANSSTATE_None)
		{
			while (m_sTrans == COMP_TRANSSTATE_IdleToExecute || m_sTrans == COMP_TRANSSTATE_PauseToExecute)
				voOMXOS_Sleep (2);
		}

		if (IsRunning () && !m_pInput->IsFlush ())
			errType = FillBuffer (pBuffer);

		ProcHoldBuffer();

		if ((pBuffer->nFlags & OMX_BUFFERFLAG_EOS))
		{
			if (!m_bEOS)
			{
				VOLOGI ("Name %s, Reach EOS!", m_pName);

				m_bEOS = OMX_TRUE;
				if (m_pCallBack != NULL)
				{
					m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventBufferFlag, 0, pBuffer->nFlags, NULL); 
				}
			}
			
			FinishBuffer(pBuffer);
		}
		else
		{
			if(OMX_FALSE == SaveHoldBuffer(pBuffer))
				FinishBuffer(pBuffer);
		}
	}
	else
	{
		if (!m_pInput->IsFlush())
		{
			if ((m_sState == OMX_StatePause && m_sTrans == COMP_TRANSSTATE_None) || m_sTrans == COMP_TRANSSTATE_ExecuteToPause)
				m_tsState.Down ();
		}
	}

	return errType;
}

OMX_ERRORTYPE voCOMXVideoSink::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	if (pBuffer->nFilledLen <= 0)
		return OMX_ErrorNone;
		
	voCOMXAutoLock lokc (&m_tmStatus);
	
// 	if (m_pDumpInputFile != NULL)
// 		voOMXFileWrite (m_pDumpInputFile, pBuffer->pBuffer, pBuffer->nFilledLen);

	OMX_BUFFERHEADERTYPE * pRenderBuffer = pBuffer;

	if (m_bSetPosInPause)
	{
		if (m_nRendFrames > 0)
		{
			voOMXOS_Sleep (30);
			return OMX_ErrorNone;
		}
	}

	if (pRenderBuffer->nTimeStamp < m_llSeekTime)
	{
		pRenderBuffer->nFilledLen = 0;
		pRenderBuffer->nTimeStamp = 0;
		return OMX_ErrorNone;
	}

	if (m_llLastFrameTime > 0)
	{
		if (pRenderBuffer->nTimeStamp < m_llLastFrameTime &&  pRenderBuffer->nTimeStamp + 30000 > m_llLastFrameTime)
		{
			VOLOGW ("The last frame time is %d, %d, %d. Drop this frame!", (int)m_llLastFrameTime, (int)pRenderBuffer->nTimeStamp, (int)(pRenderBuffer->nTimeStamp - m_llLastFrameTime));
			m_llLastFrameTime = pRenderBuffer->nTimeStamp;

			pRenderBuffer->nFilledLen = 0;
			pRenderBuffer->nTimeStamp = 0;
			return OMX_ErrorNone;
		}
	}

	m_llLastFrameTime = pRenderBuffer->nTimeStamp;

	m_pRenderBuffer = pRenderBuffer;
	if (pRenderBuffer->nFilledLen > 0)
	{
		if (pRenderBuffer->nFilledLen == sizeof (VO_VIDEO_BUFFER))
		{
			VO_VIDEO_BUFFER * pVideoBuffer = (VO_VIDEO_BUFFER *)pRenderBuffer->pBuffer;
			if ((pVideoBuffer->Stride[0] & 0XFFFF0000) > 0)
			{
				VO_U32 nWidth = (pVideoBuffer->Stride[0] & 0XFFFF0000) >> 16;
				VO_U32 nHeight = (pVideoBuffer->Stride[1] & 0XFFFF0000) >> 16;
				if (m_pVideoRender != NULL)
					m_pVideoRender->SetVideoInfo (nWidth, nHeight, VO_COLOR_YUV_PLANAR420);

				pVideoBuffer->Stride[0] = pVideoBuffer->Stride[0] & 0XFFFF;
				pVideoBuffer->Stride[1] = pVideoBuffer->Stride[1] & 0XFFFF;
			}

			memcpy (&m_videoBuffer, pRenderBuffer->pBuffer, sizeof (VO_VIDEO_BUFFER));
		}
		else
		{
			if(isVO_PLANAR420(m_videoFormat.eColorFormat))
			{
				VO_S32 nYUVWidth = (m_videoType.nFrameWidth + 15) & ~15;
				VO_S32 nYUVHeight = m_videoType.nFrameHeight;
				m_videoBuffer.Buffer[0] = pRenderBuffer->pBuffer;
				m_videoBuffer.Buffer[1] = pRenderBuffer->pBuffer + nYUVWidth * nYUVHeight;
				m_videoBuffer.Buffer[2] = pRenderBuffer->pBuffer + nYUVWidth * nYUVHeight * 5 / 4;
				m_videoBuffer.Stride[0] = nYUVWidth;
				m_videoBuffer.Stride[1] = nYUVWidth / 2;
				m_videoBuffer.Stride[2] = nYUVWidth / 2;
				m_videoBuffer.ColorType = VO_COLOR_YUV_PLANAR420;
			}
			else if (m_videoType.eColorFormat == OMX_COLOR_FormatYUV420PackedSemiPlanar)
			{
				m_videoBuffer.Buffer[0] = pRenderBuffer->pBuffer;
				m_videoBuffer.Buffer[1] = pRenderBuffer->pBuffer + m_nYUVWidth * m_nYUVHeight;
				m_videoBuffer.Stride[0] = m_nYUVWidth;
				m_videoBuffer.Stride[1] = m_nYUVWidth;
				m_videoBuffer.ColorType = VO_COLOR_YUV_420_PACK;
			}
			else if(isVO_420_PACK_2(m_videoFormat.eColorFormat))
			{
				m_videoBuffer.Buffer[0] = pRenderBuffer->pBuffer;
				m_videoBuffer.Buffer[1] = pRenderBuffer->pBuffer + m_nYUVWidth * m_nYUVHeight;
				m_videoBuffer.Stride[0] = m_nYUVWidth;
				m_videoBuffer.Stride[1] = m_nYUVWidth;
				m_videoBuffer.ColorType = VO_COLOR_YUV_420_PACK_2;

				m_videoBuffer.Buffer[2] = (VO_PBYTE)pRenderBuffer->pPlatformPrivate;
			}
			else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatYCbYCr || m_videoFormat.eColorFormat == OMX_COLOR_FormatYCrYCb ||
					 m_videoFormat.eColorFormat == OMX_COLOR_FormatCbYCrY || m_videoFormat.eColorFormat == OMX_COLOR_FormatCrYCbY)
			{
				m_videoBuffer.Buffer[0] = pRenderBuffer->pBuffer;
				m_videoBuffer.Stride[0] = m_nYUVWidth * 2;
				m_videoBuffer.ColorType = m_nColorType;
			}
			else if (m_videoFormat.eColorFormat == OMX_COLOR_FormatSTYUV420PackedSemiPlanarMB)
			{
				m_videoBuffer.Buffer[0] = (VO_PBYTE)pRenderBuffer->pBuffer;
				m_videoBuffer.Buffer[1] = (VO_PBYTE)pRenderBuffer->pPlatformPrivate;
				m_videoBuffer.Stride[0] = m_nYUVWidth; // just make the render works fine , nothing means to ste
				m_videoBuffer.ColorType = (VO_IV_COLORTYPE)(VO_COLOR_TYPE_MAX - 1);
			}
			else
			{
				m_videoBuffer.Buffer[0] = pRenderBuffer->pBuffer;
				m_videoBuffer.Stride[0] = m_nYUVWidth * 2;
				m_videoBuffer.ColorType = m_nColorType;
			}
		}

		if (m_pClockPort != NULL && (m_nRendFrames == 0/* || (pRenderBuffer->nFlags & OMX_BUFFERFLAG_STARTTIME)*/))
		{
			OMX_COMPONENTTYPE * pComp = m_pClockPort->GetTunnelComp ();
			if (pComp != NULL)
			{
				OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkType;
				pComp->GetConfig (pComp, OMX_IndexConfigTimeActiveRefClock, &clkType);
				if (clkType.eClock == OMX_TIME_RefClockVideo)
					m_pClockPort->StartWallClock (1);
			}
		}

		if (m_bTranscodeMode)
		{
			RenderVideo (&m_videoBuffer, pRenderBuffer->nTimeStamp, VO_FALSE);
			m_nRenderFrames++;

			pRenderBuffer->nFilledLen = 0;
			pRenderBuffer->nTimeStamp = 0;

			return OMX_ErrorNone;
		}

		OMX_TICKS mediaTime = 0;
		if(m_pClockPort != NULL)
		{
			m_pClockPort->GetMediaTime (&mediaTime);
#ifdef  _WIN32
			if (m_pCallBack != NULL)
			{					
				voOMXMemCopy(m_pAudioPlayTime, &mediaTime, sizeof(OMX_S32));
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_AudioPlayTime, NULL, NULL, m_pAudioPlayTime);
			}
#endif
			VOLOGR ("@#@#@ VR: Get mediaTime %d", (int) mediaTime);
		}

		if (m_nLogLevel > 0)
		{
			if (m_nLogSysStartTime == 0)
				m_nLogSysStartTime = voOMXOS_GetSysTime ();

			OMX_U32 nSysTime = voOMXOS_GetSysTime () - m_nLogSysStartTime;
			VOLOGI ("Sys Time: %d  frame time is %d, Clock Time %d, VMDiff %d, SM Diff %d",(int) nSysTime, (int)pRenderBuffer->nTimeStamp, (int)mediaTime, (int)(mediaTime - pRenderBuffer->nTimeStamp), (int)(nSysTime - mediaTime));
			m_nLogSysCurrentTime = voOMXOS_GetSysTime ();
		}

#ifdef  _WIN32
		//Rogine add for MFW Self-test
		if (m_pCallBack != NULL)
		{	
			//VOLOGI ("@@#@#@ Video Render Time Stamp %d", pBuffer->nTimeStamp);
			voOMXMemCopy(m_pVRTimeStamp, &pBuffer->nTimeStamp, sizeof(OMX_S32));
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_VideoRenderTimeStamp, NULL, NULL, m_pVRTimeStamp);
		}
#endif	
		
		//Rogine add correct dump
		if (m_pDumpInputFile != NULL)
		{
// 			voOMXFileWrite (m_pDumpInputFile, m_videoBuffer.Buffer[0], m_videoBuffer.Stride[0]);
// 			voOMXFileWrite (m_pDumpInputFile, m_videoBuffer.Buffer[1], m_videoBuffer.Stride[1]);
// 			voOMXFileWrite (m_pDumpInputFile, m_videoBuffer.Buffer[2], m_videoBuffer.Stride[2]);
			OutputOneFrame(&m_videoBuffer, 1280, 720, m_pDumpInputFile);
		}

		// detect the video render speed.
		if (m_nRendFrames == 0)
		{
			RenderVideo (&m_videoBuffer, pRenderBuffer->nTimeStamp, VO_FALSE);
		}
		else if (m_nRendFrames > 1 && m_nRendFrames < 12)
		{
			VO_U32 nRenderTime = voOMXOS_GetSysTime ();

			RenderVideo (&m_videoBuffer, pRenderBuffer->nTimeStamp, VO_FALSE);

			m_nRenderSpeed = m_nRenderSpeed + (voOMXOS_GetSysTime () - nRenderTime);

			if (m_nRendFrames == 11)
				VOLOGI ("Render Speed is: %d", (int)m_nRenderSpeed);
		}
		else
		{
   			VOLOGR ("m_bDropVideo %d, m_nFrameDropped %d, mediaTime %d, timestamp %d, offset %d", 
				m_bDropVideo, m_nFrameDropped, (OMX_U32)mediaTime, (OMX_U32)pRenderBuffer->nTimeStamp, OMX_U32(mediaTime - pRenderBuffer->nTimeStamp));

			if (m_bDropVideo)
			{
				VO_S32 nDrop = (VO_S32)((mediaTime - pRenderBuffer->nTimeStamp) / 100);
				if(nDrop > VO_VIDEOSINK_MAXDROPFRAME)
				{
					VOLOGR ("So poor performance mediaTime %d, timestamp %d", (OMX_U32)mediaTime, (OMX_U32)pRenderBuffer->nTimeStamp);
					nDrop = VO_VIDEOSINK_MAXDROPFRAME;
				}

				if(nDrop <= m_nFrameDropped||m_nFrameDropped)//modified by Eric, 2011.09.03, prevent dropping frame successively 
				{
					RenderVideo (&m_videoBuffer, pRenderBuffer->nTimeStamp, VO_FALSE);
					m_nFrameDropped = 0;
				}
				else	// drop frame!!
					m_nFrameDropped++;
			}
			else
				RenderVideo (&m_videoBuffer, pRenderBuffer->nTimeStamp, VO_FALSE);
		}

		if(m_pClockPort)
		{
			if(m_nRendFrames == 0)
			{
				m_pClockPort->UpdateMediaTime(pRenderBuffer->nTimeStamp);
				VOLOGI("Video Time: %d ", (int)pRenderBuffer->nTimeStamp);
			}

			m_pClockPort->SetVideoRenderTime(pRenderBuffer->nTimeStamp);
		}

		if(m_nRendFrames > 0)	// force output first video frame, East 2010/08/31
			WaitForRenderTime(pRenderBuffer->nTimeStamp + 30, NULL);

		m_nRendFrames++;
		m_nTotalRendFrames++;
	}

	m_llPlayTime = pRenderBuffer->nTimeStamp;
	m_nRenderFrames++;

	if (m_pCallBack != NULL)
		m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_VideoRenderFrames, m_nRendFrames, 0, NULL);

	pRenderBuffer->nFilledLen = 0;
	pRenderBuffer->nTimeStamp = 0;

	if (m_nLogLevel > 1)
	{
		VOLOGI ("Render video used time is %d", (int)(voOMXOS_GetSysTime () - m_nLogSysCurrentTime));
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoSink::RenderVideo (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	voCOMXAutoLock lock (&m_mutRender);

	if (m_nPfmFrameIndex < m_nPfmFrameSize)
	{
		m_pPfmFrameTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime ();
		m_pPfmCodecThreadTime[m_nPfmFrameIndex] = voOMXOS_GetThreadTime (NULL);
		m_pPfmCompnThreadTime[m_nPfmFrameIndex] = m_pPfmCodecThreadTime[m_nPfmFrameIndex];
		m_pPfmMediaTime[m_nPfmFrameIndex] = (VO_U32) nStart;
		m_pPfmSystemTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime ();
	}

	if (m_pVideoRender == NULL)
	{
		CreateRender ();
		if (m_pFRateMeter)
			m_pFRateMeter->Start();
	}

	if (m_pVideoRender != NULL)
	{
		m_pVideoRender->Render (pVideoBuffer, nStart, bWait);
		VOLOGR ("Render Video Buffer!");
		if (m_pFRateMeter)
			m_pFRateMeter->AddFrame();
	}

	if (m_nPfmFrameIndex < m_nPfmFrameSize)
	{
		m_pPfmFrameTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime () - m_pPfmFrameTime[m_nPfmFrameIndex];
		m_pPfmCodecThreadTime[m_nPfmFrameIndex] = voOMXOS_GetThreadTime (NULL) - m_pPfmCodecThreadTime[m_nPfmFrameIndex];
	}
	m_nPfmFrameIndex++;

	return OMX_ErrorNone;
}

OMX_BOOL voCOMXVideoSink::WaitForRenderTime (OMX_TICKS nTime, OMX_TICKS* pnCurrMediaTime)
{
	OMX_TICKS mediaTime = 0;
	if(m_pClockPort)
	{
		m_pClockPort->GetMediaTime (&mediaTime);

		while(mediaTime < nTime)
		{
			voOMXOS_Sleep(2);
			m_pClockPort->GetMediaTime(&mediaTime);

			// it is IDLE status
			if(mediaTime == 0)
			{
				OMX_COMPONENTTYPE* pComp = m_pClockPort->GetTunnelComp();
				if(pComp)
				{
					OMX_STATETYPE state = OMX_StateInvalid;
					pComp->GetState(pComp, &state);
					if(state != OMX_StateExecuting)
						return OMX_FALSE;
				}
			}

			// if the time stamp is wrong, sleep and break it.
			if(nTime > mediaTime + 30000 || mediaTime > nTime + 30000)
			{
				voOMXOS_Sleep(30);

				return OMX_FALSE;
			}

			if (m_sTrans == COMP_TRANSSTATE_ExecuteToPause || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle ||
				m_sTrans == COMP_TRANSSTATE_AnyToInvalid)
				return OMX_FALSE;

 			if (m_sTrans == COMP_TRANSSTATE_None && m_sState != OMX_StateExecuting && m_sState != OMX_StatePause)
 				return OMX_FALSE;

			if (m_pInput->IsFlush() || PORT_TRANS_ENA2DIS == m_pInput->m_sStatus)
				return OMX_FALSE;
		}
	}

	if(pnCurrMediaTime)
		*pnCurrMediaTime = mediaTime;

	if(m_nDumpRuntimeLog)
	{
		VOLOGI ("nTime %d, mediaTime %d, systime %d", (int)nTime, (int)mediaTime, (int)voOMXOS_GetSysTime());
	}

	return OMX_TRUE;
}

void voCOMXVideoSink::FinishBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	if (m_pInput->IsTunnel ())
	{
		pBuffer->nOutputPortIndex = m_pInput->GetTunnelPort ();
		pBuffer->nInputPortIndex = 0;
		m_pInput->GetTunnelComp()->FillThisBuffer (m_pInput->GetTunnelComp (), pBuffer);
	}
	else
		m_pCallBack->EmptyBufferDone (m_pComponent, m_pAppData, pBuffer);
}

OMX_BOOL voCOMXVideoSink::SaveHoldBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	voCOMXAutoLock lock(&m_tmHoldBuffer);
	if (OMX_FALSE == m_bHoldBuffer || m_pInput->GetBufferCount () < 2)
		return OMX_FALSE;

	if (m_pInput->IsFlush ())
		return OMX_FALSE;

	m_pHoldBuffer = pBuffer;
	return OMX_TRUE;
}

void voCOMXVideoSink::ProcHoldBuffer (void)
{
	voCOMXAutoLock lock(&m_tmHoldBuffer);
	if (OMX_FALSE == m_bHoldBuffer || m_pInput->GetBufferCount () < 2)
		return;

	if (m_pHoldBuffer != NULL)
	{
		FinishBuffer (m_pHoldBuffer);
		m_pHoldBuffer = NULL;
	}
}

OMX_ERRORTYPE voCOMXVideoSink::InitPortType (void)
{
	m_portParam[OMX_PortDomainVideo].nPorts = 1;
	m_portParam[OMX_PortDomainVideo].nStartPortNumber = 0;

	m_portParam[OMX_PortDomainOther].nPorts = 1;
	m_portParam[OMX_PortDomainOther].nStartPortNumber = 1;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;

	m_pInput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainVideo;
	pPortType->nBufferCountActual = (OMX_TRUE == m_bDualCore) ? 2 : 1;
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

VO_S32 voCOMXVideoSink::videosinkVideoRenderProc (VO_PTR pUserData, VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoFormat, VO_S32 nStart)
{
	voCOMXVideoSink * pVideoSink = (voCOMXVideoSink *)pUserData;

	return pVideoSink->videosinkVideoRender (pVideoBuffer, pVideoFormat, nStart);
}

VO_S32 voCOMXVideoSink::videosinkVideoRender (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoFormat, VO_S32 nStart)
{
	if (pVideoBuffer == NULL)
	{
		m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)pVideoFormat);
		return VO_ERR_FINISH;
	}

	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	if (m_cbRenderBuffer.pCallBack != NULL)
	{
		m_cbVideoBuffer.Buffer[0] = pVideoBuffer->Buffer[0];
		m_cbVideoBuffer.Buffer[1] = pVideoBuffer->Buffer[1];
		m_cbVideoBuffer.Buffer[2] = pVideoBuffer->Buffer[2];
		m_cbVideoBuffer.Stride[0] = pVideoBuffer->Stride[0];
		m_cbVideoBuffer.Stride[1] = pVideoBuffer->Stride[1];
		m_cbVideoBuffer.Stride[2] = pVideoBuffer->Stride[2];

		m_cbVideoBuffer.Width = pVideoFormat->Width;
		m_cbVideoBuffer.Height = pVideoFormat->Height;
		m_cbVideoBuffer.Time = m_pRenderBuffer->nTimeStamp;

		if (pVideoBuffer->ColorType == VO_COLOR_RGB565_PACKED)
			m_cbVideoBuffer.Color = OMX_COLOR_Format16bitRGB565;
		else
			m_cbVideoBuffer.Color = m_videoFormat.eColorFormat;

		if (m_cbVideoBuffer.Buffer[0] == NULL)
			return VO_ERR_FINISH;
		if (m_cbVideoBuffer.Stride[0] == 0)
			return VO_ERR_FINISH;
		errType = m_cbRenderBuffer.pCallBack (this, m_cbRenderBuffer.pUserData, m_pRenderBuffer, (OMX_INDEXTYPE)OMX_VO_IndexVideoBufferType, &m_cbVideoBuffer);

		if (errType == OMX_ErrorNone)
			return VO_ERR_FINISH;
		else
			return VO_ERR_NOT_IMPLEMENT;
	}

	return VO_ERR_FINISH;
}

bool voCOMXVideoSink::isVO_420_PACK_2(OMX_COLOR_FORMATTYPE inColor)
{
	if(inColor == OMX_COLOR_Format_QcomHW 
	   || inColor == OMX_COLOR_Format_QcomHW2 
	   || inColor == OMX_COLOR_Format_NvidiaHW
	   || inColor == OMX_COLOR_FormatYUV420SemiPlanar
	   || inColor == OMX_COLOR_Format_TI_DUCATI1
	   || inColor == OMX_COLOR_Format_SamSuangFK)
	{
		return true;
	}
	return false;
}
bool voCOMXVideoSink::isVO_PLANAR420(OMX_COLOR_FORMATTYPE inColor)
{
	if((inColor == OMX_COLOR_FormatYUV420Planar)
	   || inColor == OMX_COLOR_Format_SamSuangHW)
	{
		return true;
	}
	return false;
}
