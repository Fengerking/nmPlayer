	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBasePort.cpp

	Contains:	CBasePort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include <string.h>
#include "voString.h"

#include "CBasePort.h"
#include "CBaseComp.h"
#include "voOMXOSFun.h"

#if defined LOG_TAG
#undef LOG_TAG
#define LOG_TAG "CBasePort"
#endif
#include "voLog.h"

CBasePort::CBasePort(CBaseComp * pComp, OMX_U32 nIndex)
	: m_pComp (pComp)
	, m_nIndex (nIndex)
	, m_bSupplier (OMX_FALSE)
	, m_nBuffCount (0)
	, m_ppBuffHead (NULL)
	, m_ppDataBuffer (NULL)
	, m_pOutputItem (NULL)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
	, m_nPadWidth(0)
	, m_nPadHeight(0)
	, m_nSampleRate (44100)
	, m_nChannels (2)
	, m_nSampleBits (16)
	, m_nInputNum (0)
	, m_nOutputNum (0)
	, m_nLastBufferTime (0)
{
	memset((void*)&m_deminfo, 0, sizeof(VO_VIDEO_DECDIMENSION));
	FillPortType ();

	m_pComponent = m_pComp->GetComponentHandle ();
}

CBasePort::~CBasePort()
{
	if (m_ppBuffHead != NULL)
		delete []m_ppBuffHead;

	if (m_ppDataBuffer != NULL)
	{
		for (OMX_U32 i = 0; i < m_nBuffCount; i++)
		{
			if (m_ppDataBuffer[i] != NULL)
				delete []m_ppDataBuffer[i];
		}
		delete []m_ppDataBuffer;
	}

	VOLOGI ("Free port!");
}

VO_U32 CBasePort::SetVideoInputData(VO_CODECBUFFER * pInput, VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_BOOL bHeader)
{
	voCOMXAutoLock lock (&m_muxBuffer);

	CBufferItem *	pItem = m_lstFree.RemoveBufferItem();
	if (pItem == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	OMX_BUFFERHEADERTYPE *	pEmptyBuffer = pItem->m_pBufferHead;
	if (pEmptyBuffer == NULL)
	{
		m_lstFree.AddBufferItem (pItem);
		return VO_ERR_NOT_IMPLEMENT;
	}

	if (pHeadData != NULL)
	{
		memcpy (pEmptyBuffer->pBuffer, pHeadData, nHeadSize);
		memcpy (pEmptyBuffer->pBuffer + nHeadSize, pInput->Buffer, pInput->Length);
		pInput->Length = pInput->Length + nHeadSize;
	}
	else
	{
		memcpy (pEmptyBuffer->pBuffer, pInput->Buffer, pInput->Length);
	}
	pEmptyBuffer->nOffset = 0;
	pEmptyBuffer->nFilledLen = pInput->Length;
	pEmptyBuffer->nTimeStamp = pInput->Time;
	
	if(bHeader)
		pEmptyBuffer->nFlags |=  OMX_BUFFERFLAG_ENDOFFRAME | OMX_BUFFERFLAG_CODECCONFIG;
	else
		pEmptyBuffer->nFlags |=  OMX_BUFFERFLAG_ENDOFFRAME;

	// log out the input information
	OMX_U32 nIndex = 0;
	for (nIndex = 0; nIndex < m_nBuffCount; nIndex++)
	{
		if (pEmptyBuffer == m_ppBuffHead[nIndex])
			break;
	}
//	VOLOGI ("Index % 4d  Time % 6d,    % 6d", nIndex, (int)pEmptyBuffer->nTimeStamp, (int)(pEmptyBuffer->nTimeStamp - m_nLastBufferTime));
	m_nLastBufferTime = pEmptyBuffer->nTimeStamp;


	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType = m_pComponent->EmptyThisBuffer (m_pComponent, pEmptyBuffer);
	if (errType == OMX_ErrorNone)
		m_lstFull.AddBufferItem (pItem);
	else
		VOLOGE ("EmptyThisBuffer return error %08X", errType);

	return VO_ERR_NONE;
}

VO_U32 CBasePort::FillOutputBuffer()
{
	voCOMXAutoLock lock (&m_muxBuffer);

	if (m_pOutputItem != NULL)
	{
		OMX_ERRORTYPE errType = OMX_ErrorNone;

		errType = m_pComponent->FillThisBuffer (m_pComponent, m_pOutputItem->m_pBufferHead);
		if (errType == OMX_ErrorNone)
			m_lstFull.AddBufferItem (m_pOutputItem);
		else
			VOLOGE ("FillThisBuffer return error %08X", errType);

		m_pOutputItem = NULL;
	}
	return VO_ERR_NONE;

}
VO_U32 CBasePort::GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
	voCOMXAutoLock lock (&m_muxBuffer);

	m_pOutputItem = m_lstFree.RemoveBufferItem ();
	if (m_pOutputItem == NULL)
		return VO_ERR_RETRY;

	OMX_BUFFERHEADERTYPE * pFilledBuffer = m_pOutputItem->m_pBufferHead;
	if (pFilledBuffer->nFilledLen  == 0)
	{
		return VO_ERR_INPUT_BUFFER_SMALL;
	}
	else if (pFilledBuffer->nFilledLen == sizeof (VO_VIDEO_BUFFER))
	{
		VO_VIDEO_BUFFER * pVideoBuffer = (VO_VIDEO_BUFFER *)pFilledBuffer->pBuffer;
		memcpy (pOutBuffer, pVideoBuffer, sizeof (VO_VIDEO_BUFFER));
	}
	else
	{
		if (m_nColorType == VO_COLOR_YUV_420_PACK_2)
		{
			pOutBuffer->Buffer[0] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset;
			pOutBuffer->Buffer[1] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset +  m_nVideoWidth * m_nVideoHeight;

			pOutBuffer->Stride[0] = m_nVideoWidth;
			pOutBuffer->Stride[1] = m_nVideoWidth;

			pOutBuffer->Buffer[2] = (VO_PBYTE)pFilledBuffer->pPlatformPrivate;
		}
		else if(m_nColorType == VO_COLOR_YUV_420_PACK)
		{
			pOutBuffer->Buffer[0] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset;
			pOutBuffer->Buffer[1] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset + m_nPadWidth * m_nPadHeight;
			pOutBuffer->Stride[0] = m_nPadWidth;
			pOutBuffer->Stride[1] = m_nPadWidth;
			pOutBuffer->ColorType = VO_COLOR_YUV_420_PACK;	
		}
		else
		{
			pOutBuffer->Buffer[0] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset;
			pOutBuffer->Buffer[1] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset +  m_nVideoWidth * m_nVideoHeight;
			pOutBuffer->Buffer[2] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset +  m_nVideoWidth * m_nVideoHeight * 5 / 4;

			pOutBuffer->Stride[0] = m_nVideoWidth;
			pOutBuffer->Stride[1] = m_nVideoWidth / 2;
			pOutBuffer->Stride[2] = m_nVideoWidth / 2;

			pOutBuffer->ColorType = VO_COLOR_YUV_PLANAR420;

			pOutInfo->Flag = 0;
			pOutInfo->Format.Width = m_nVideoWidth;
			pOutInfo->Format.Height = m_nVideoHeight;
		}
	}

	pOutBuffer->Time = pFilledBuffer->nTimeStamp;

/*
	// log out the input information
	int nIndex = 0;
	for (nIndex = 0; nIndex < m_nBuffCount; nIndex++)
	{
		if (pFilledBuffer == m_ppBuffHead[nIndex])
			break;
	}
	VOLOGI ("Index % 4d  Time % 6d,  % 6d, Free %d", nIndex, (int)pFilledBuffer->nTimeStamp, (int)(pFilledBuffer->nTimeStamp - m_nLastBufferTime), m_lstFree.GetCount ());
	m_nLastBufferTime = pFilledBuffer->nTimeStamp;
*/
	return VO_ERR_NONE;
}

VO_U32 CBasePort::SetAudioInputData(VO_CODECBUFFER * pInput)
{
	return SetVideoInputData(pInput, NULL, 0, VO_FALSE);
}

VO_U32 CBasePort::GetAudioOutputData(VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
/*
	if (m_pOutputBuffer != NULL)
	{
		OMX_ERRORTYPE errType = OMX_ErrorNone;
		errType = m_pComponent->FillThisBuffer (m_pComponent, m_pOutputBuffer);
		if (errType == OMX_ErrorNone)
			UpdateBufferStatus (m_pOutputBuffer, OMX_FALSE);
		else
			VOLOGE ("FillThisBuffer return error %08X", errType);

		m_pOutputBuffer = NULL;
	}

	m_muxBuffer.Lock ();
	VO_S64	nTime = 0X7FFFFFFF;
	for (OMX_U32 i = 0; i < m_nBuffCount; i++)
	{
		if (m_ppBuffEmpty[i] != NULL)
		{
			if (m_ppBuffEmpty[i]->nTimeStamp < nTime)
			{
				m_pOutputBuffer = m_ppBuffEmpty[i];
				nTime = m_pOutputBuffer->nTimeStamp;
			}
		}
	}
	m_muxBuffer.Unlock ();

	if (m_pOutputBuffer == NULL)
		return VO_ERR_INPUT_BUFFER_SMALL;

	if (m_pOutputBuffer->nFilledLen  == 0)
	{
		return VO_ERR_INPUT_BUFFER_SMALL;
	}
	else
	{
		memcpy (pOutBuffer->Buffer, m_pOutputBuffer->pBuffer + m_pOutputBuffer->nOffset, m_pOutputBuffer->nFilledLen);
		pOutBuffer->Length = m_pOutputBuffer->nFilledLen;
	}
*/
	return VO_ERR_NONE;
}

VO_U32	CBasePort::SetVideoRenderInput(VO_VIDEO_BUFFER * pInput)
{
	voCOMXAutoLock lock (&m_muxBuffer);

	CBufferItem *	pItem = m_lstFree.RemoveBufferItem ();
	if (pItem == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	OMX_BUFFERHEADERTYPE *	pEmptyBuffer = pItem->m_pBufferHead;
	if (pEmptyBuffer == NULL)
	{
		m_lstFree.AddBufferItem (pItem);
		return VO_ERR_NOT_IMPLEMENT;
	}


	VO_U32 nLoops = m_nVideoHeight;
	VO_U32 nCopySize = m_nVideoWidth;
	VO_PBYTE pDst = pEmptyBuffer->pBuffer;
	VO_PBYTE pSrc = pInput->Buffer[0];

	for (OMX_U32 i = 0 ; i < nLoops ; i++)
	{
		memcpy(pDst , pSrc , nCopySize);
		pDst += nCopySize;
		pSrc += pInput->Stride[0];
	}

	nLoops = m_nVideoHeight / 2;
	nCopySize = m_nVideoWidth / 2;
	pSrc = pInput->Buffer[1];

	for (OMX_U32 i = 0 ; i < nLoops ; i++)
	{
		memcpy(pDst , pSrc , nCopySize);
		pDst += nCopySize;
		pSrc += pInput->Stride[1];
	}

	nLoops = m_nVideoHeight / 2;
	nCopySize = m_nVideoWidth / 2;
	pSrc = pInput->Buffer[2];

	for (OMX_U32 i = 0 ; i < nLoops ; i++)
	{
		memcpy(pDst , pSrc , nCopySize);
		pDst += nCopySize;
		pSrc += pInput->Stride[2];
	}
	
	pEmptyBuffer->nOffset = 0;
	pEmptyBuffer->nFilledLen = m_nVideoHeight * m_nVideoWidth * 3 / 2;
	pEmptyBuffer->nTimeStamp = 0;


	// log out the input information
	OMX_U32 nIndex = 0;
	for (nIndex = 0; nIndex < m_nBuffCount; nIndex++)
	{
		if (pEmptyBuffer == m_ppBuffHead[nIndex])
			break;
	}
	
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType = m_pComponent->EmptyThisBuffer (m_pComponent, pEmptyBuffer);
	if (errType == OMX_ErrorNone)
		m_lstFull.AddBufferItem (pItem);
	else
		VOLOGE ("EmptyThisBuffer return error %08X", errType);

	return VO_ERR_NONE;
}


OMX_ERRORTYPE CBasePort::AllocBuffer (void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	FillPortType ();
	CreateBufferHead ();

	if (m_portType.eDir == OMX_DirOutput)
	{
		VOLOGI ("Output port m_bSupplier is %d", m_bSupplier);
	}
	else
	{
		VOLOGI ("Input port m_bSupplier is %d", m_bSupplier);
	}

	voCOMXAutoLock lock (&m_muxBuffer);
	if (m_bSupplier)
	{
		if (m_ppBuffHead == NULL)
			return OMX_ErrorNotReady;

		for (OMX_U32 i = 0; i < m_nBuffCount; i++)
		{
			errType = m_pComponent->AllocateBuffer (m_pComponent, &m_ppBuffHead[i], m_nIndex, this, m_portType.nBufferSize);
			if (errType != OMX_ErrorNone)
				return errType;

		//	memset (m_ppBuffHead[i]->pBuffer, 0, m_nVideoWidth * m_nVideoHeight);
		//	memset (m_ppBuffHead[i]->pBuffer + m_nVideoWidth * m_nVideoHeight, 127, m_nVideoWidth * m_nVideoHeight / 2);

			m_lstFree.AddBufferHead (m_ppBuffHead[i]);
		}
	}
	else
	{
		if (m_ppDataBuffer == NULL)
			m_ppDataBuffer = new OMX_U8*[m_nBuffCount];

		for (OMX_U32 i = 0; i < m_nBuffCount; i++)
			m_ppDataBuffer[i] = new OMX_U8[m_portType.nBufferSize];

		for (OMX_U32 i = 0; i < m_nBuffCount; i++)
		{
			errType = m_pComponent->UseBuffer (m_pComponent, &m_ppBuffHead[i], m_nIndex, this, m_portType.nBufferSize, m_ppDataBuffer[i]);
			if (errType != OMX_ErrorNone)
				return errType;

			m_lstFree.AddBufferHead (m_ppBuffHead[i]);
		}
	}

	return errType;
}

OMX_ERRORTYPE CBasePort::FreeBuffer (void)
{
	VOLOGI ("");

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	voCOMXAutoLock lock (&m_muxBuffer);
	for (OMX_U32 i = 0; i < m_nBuffCount; i++)
	{
		errType = m_pComponent->FreeBuffer (m_pComponent, m_nIndex, m_ppBuffHead[i]);
		if (errType != OMX_ErrorNone)
			return errType;

		m_ppBuffHead[i] = NULL;
	}

	m_lstFree.ResetItems ();
	m_lstFull.ResetItems ();

	return errType;
}

OMX_ERRORTYPE CBasePort::StartBuffer (void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (m_ppBuffHead == NULL)
		return OMX_ErrorNotImplemented;

	if (m_portType.eDir == OMX_DirOutput)
	{
		voCOMXAutoLock lock (&m_muxBuffer);
		CBufferItem *	pItem = m_lstFree.RemoveBufferItem ();
		while (pItem != NULL)
		{
			OMX_BUFFERHEADERTYPE *	pEmptyBuffer = pItem->m_pBufferHead;
			if (pEmptyBuffer != NULL)
			{
				errType = m_pComponent->FillThisBuffer (m_pComponent, pEmptyBuffer);
				if (errType != OMX_ErrorNone)
				{
					VOLOGE ("FillThisBuffer return error %08X", errType);
					return errType;
				}
			}

			m_lstFull.AddBufferItem (pItem);
			pItem = m_lstFree.RemoveBufferItem ();
		}
	}

	return errType;
}

OMX_ERRORTYPE CBasePort::Stop (void)
{
	voCOMXAutoLock lock (&m_muxBuffer);
	if (m_pOutputItem != NULL)
	{
		m_lstFull.AddBufferItem (m_pOutputItem);
		m_pOutputItem = NULL;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CBasePort::EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
	voCOMXAutoLock lock (&m_muxBuffer);
	CBufferItem * pItem = m_lstFull.RemoveBufferHead (pBuffer);

	if (pItem != NULL)
		m_lstFree.AddBufferItem (pItem);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CBasePort::FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
/*
	// log out the input information
	int nIndex = 0;
	for (nIndex = 0; nIndex < m_nBuffCount; nIndex++)
	{
		if (pBuffer == m_ppBuffHead[nIndex])
			break;
	}
	VOLOGI ("Index % 4d  Time % 6d,    % 6d", nIndex, (int)pBuffer->nTimeStamp, (int)(pBuffer->nTimeStamp - m_nLastBufferTime));
	m_nLastBufferTime = pBuffer->nTimeStamp;
*/
	voCOMXAutoLock lock (&m_muxBuffer);
	CBufferItem * pItem = m_lstFull.RemoveBufferHead (pBuffer);
	if (pItem != NULL)
		m_lstFree.AddBufferItem (pItem);

	return OMX_ErrorNone;
}
#define MAX(a, b) (a) > (b) ? (a):(b)
OMX_ERRORTYPE CBasePort::SetVideoInfo (VO_VIDEO_CODINGTYPE nCoding, VO_IV_COLORTYPE nColorType,
									   OMX_U32  nWidth, OMX_U32 nHeight)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	m_nVideoWidth = (nWidth + 15) & ~0X0F;
	m_nVideoHeight = (nHeight + 15) & ~0X0F;
	m_nColorType = nColorType;

	errType = m_pComp->GetParameter(OMX_IndexParamPortDefinition, &m_portType);

	m_portType.format.video.nFrameWidth = m_nVideoWidth;
	m_portType.format.video.nFrameHeight = m_nVideoHeight;

	if (!strcmp (m_pComp->GetCompName(), "OMX.Nvidia.std.iv_renderer.overlay.yuv420"))
		m_portType.nBufferSize = MAX(m_nVideoWidth * m_nVideoHeight * 3 / 2, m_portType.nBufferSize);
	else
		m_portType.nBufferSize = MAX(m_nVideoWidth * m_nVideoHeight, m_portType.nBufferSize);

	VO_COMPWRAP_CODECTYPE ComWrapType = m_pComp->GetComWrapType();
	if(ComWrapType == VO_COMPWRAP_CODEC_VideoDec)
	{
		if(m_pComp->GetThumbMode()== VO_TRUE)
			m_portType.nBufferCountActual = m_portType.nBufferCountMin;

		if (m_portType.eDir == OMX_DirInput)
		{
			if (nCoding == VO_VIDEO_CodingMPEG2)
				m_portType.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG2;
			else if (nCoding == VO_VIDEO_CodingH263)
				m_portType.format.video.eCompressionFormat = OMX_VIDEO_CodingH263;
			else if (nCoding == VO_VIDEO_CodingMPEG4)
				m_portType.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
			else if (nCoding == VO_VIDEO_CodingWMV)
				m_portType.format.video.eCompressionFormat = OMX_VIDEO_CodingWMV;
			else if (nCoding == VO_VIDEO_CodingRV)
				m_portType.format.video.eCompressionFormat = OMX_VIDEO_CodingRV;
			else if (nCoding == VO_VIDEO_CodingH264)
				m_portType.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
			else if (nCoding == VO_VIDEO_CodingMJPEG)
				m_portType.format.video.eCompressionFormat = OMX_VIDEO_CodingMJPEG;

			errType = m_pComp->SetParameter (OMX_IndexParamPortDefinition, &m_portType);
			if (errType != OMX_ErrorNone)
				return errType;

			if(nCoding == VO_VIDEO_CodingH264)
			{
				OMX_VIDEO_PARAM_AVCTYPE h264type;
				m_pComp->SetHeader(&h264type, sizeof(OMX_VIDEO_PARAM_AVCTYPE));
				h264type.nPortIndex = m_nIndex;

				errType = m_pComponent->GetParameter(m_pComponent, OMX_IndexParamVideoAvc,&h264type);
				if(errType == OMX_ErrorNone)
				{
					OMX_VIDEO_PARAM_AVCTYPE srcAVCtype;
					memset(&srcAVCtype, 0, sizeof(OMX_VIDEO_PARAM_AVCTYPE));
					VO_U32 err= VO_ERR_NONE;
					err = m_pComp->GetSourceParam(VO_PID_VIDEO_ParamVideoAvc, &srcAVCtype);
					if(err == VO_ERR_NONE)
						h264type.nRefFrames = srcAVCtype.nRefFrames;

					errType = m_pComponent->SetParameter(m_pComponent, OMX_IndexParamVideoAvc,&h264type);
				}				
			}

			OMX_VIDEO_PARAM_PORTFORMATTYPE fmtVideo;
			m_pComp->SetHeader (&fmtVideo, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE));
			fmtVideo.nPortIndex = m_nIndex;
			errType = m_pComponent->GetParameter (m_pComponent, OMX_IndexParamVideoPortFormat, &fmtVideo);			
			fmtVideo.eCompressionFormat = m_portType.format.video.eCompressionFormat;
			fmtVideo.eColorFormat = OMX_COLOR_FormatUnused;
			errType = m_pComponent->SetParameter (m_pComponent, OMX_IndexParamVideoPortFormat, &fmtVideo);			
		}
		else
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE fmtVideo;
			m_pComp->SetHeader (&fmtVideo, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE));
			fmtVideo.nPortIndex = m_nIndex;

			errType = m_pComponent->GetParameter (m_pComponent, OMX_IndexParamVideoPortFormat, &fmtVideo);
			fmtVideo.eColorFormat = m_portType.format.video.eColorFormat;
			fmtVideo.eCompressionFormat = OMX_VIDEO_CodingUnused;
			errType = m_pComponent->SetParameter (m_pComponent, OMX_IndexParamVideoPortFormat, &fmtVideo);
			// update output port color format, to be added...
			switch (fmtVideo.eColorFormat)
			{
			case OMX_COLOR_FormatYUV420Planar:
				m_nColorType = VO_COLOR_YUV_PLANAR420;
				break;
			case OMX_COLOR_FormatYUV420PackedSemiPlanar:
				m_nColorType = VO_COLOR_YUV_420_PACK;
				break;
			default:
				break;
			}

			//configure h/w related parameters
			if(!strncmp(m_pComp->GetComponentName(), "OMX.TI.DUCATI1.VIDEO.DECODER", 29))
			{
				// if allocate buffer by client, need to set following parameter
	/*			if(m_bSupplier = OMX_FALSE)
				{
					OMX_TI_PARAM_BUFFERPREANNOUNCE bufPreAnnouce;
					m_pComp->SetHeader(&bufPreAnnouce, sizeof(OMX_TI_PARAM_BUFFERPREANNOUNCE));
					bufPreAnnouce.nPortIndex = m_nIndex;
					errType = m_pComponent->GetParameter(m_pComponent,(OMX_INDEXTYPE)OMX_TI_IndexParamBufferPreAnnouncement, &bufPreAnnouce);
					bufPreAnnouce.bEnabled = OMX_FALSE;
					errType = m_pComponent->SetParameter(m_pComponent,(OMX_INDEXTYPE)OMX_TI_IndexParamBufferPreAnnouncement, &bufPreAnnouce);
				}*/				

				OMX_CONFIG_RECTTYPE cr;
				m_pComp->SetHeader(&cr, sizeof(OMX_CONFIG_RECTTYPE));
				cr.nPortIndex = m_nIndex;
				errType = m_pComponent->GetParameter(m_pComponent, (OMX_INDEXTYPE)OMX_TI_IndexParam2DBufferAllocDimension, &cr);
				
				m_portType.format.video.nStride		= cr.nWidth;
				m_portType.format.video.nSliceHeight= cr.nHeight;
				m_deminfo.nTop = cr.nTop;
				m_deminfo.nLeft = cr.nLeft;
				m_deminfo.nWidth = cr.nWidth;
				m_deminfo.nHeight = cr.nHeight;

				m_nPadWidth = cr.nWidth > 0 ? cr.nWidth : m_nVideoWidth;
				m_nPadHeight = cr.nHeight > 0 ? cr.nHeight : m_nVideoWidth;

				//OMX_CONFIG_RECTTYPE rect;
				//m_pComp->SetHeader(&rect, sizeof(OMX_CONFIG_RECTTYPE));
    //            rect.nPortIndex = m_nIndex;
    //            errType = m_pComponent->GetParameter(m_pComponent, (OMX_INDEXTYPE)OMX_IndexConfigCommonOutputCrop, &rect);

				if (m_portType.format.video.nStride < 4096)
					m_portType.format.video.nStride = 4096;
			}	

			errType = m_pComp->SetParameter (OMX_IndexParamPortDefinition, &m_portType);
			if (errType != OMX_ErrorNone)
				return errType;

			if(!strncmp(m_pComp->GetComponentName(), "OMX.TI.DUCATI1.VIDEO.DECODER", 29))
			{
				OMX_CONFIG_RECTTYPE cr;
				m_pComp->SetHeader(&cr, sizeof(OMX_CONFIG_RECTTYPE));
				cr.nPortIndex = m_nIndex;
				errType = m_pComponent->GetParameter(m_pComponent, (OMX_INDEXTYPE)OMX_TI_IndexParam2DBufferAllocDimension, &cr);
				
				m_portType.format.video.nStride		= cr.nWidth;
				m_portType.format.video.nSliceHeight= cr.nHeight;
				m_deminfo.nTop = cr.nTop;
				m_deminfo.nLeft = cr.nLeft;
				m_deminfo.nWidth = 4096;//cr.nWidth;
				m_deminfo.nHeight = cr.nHeight;

				m_nPadWidth = cr.nWidth > 0 ? cr.nWidth : m_nVideoWidth;
				m_nPadHeight = cr.nHeight > 0 ? cr.nHeight : m_nVideoWidth;
			}
		}			

	}
	else if(ComWrapType == VO_COMPWRAP_CODEC_VideoEnc)
	{

	}
	

	return errType;
}

OMX_ERRORTYPE CBasePort::SetAudioInfo (VO_AUDIO_CODINGTYPE nCoding, VO_U32 nSampleRate, VO_U32 nChannels, VO_U32 nSampleBits)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	m_nSampleRate = nSampleRate;
	m_nChannels = nChannels;
	m_nSampleBits = nSampleBits;

	if (m_portType.eDir != OMX_DirInput)
		return errType;

	if (nCoding == VO_AUDIO_CodingMP3)
		m_portType.format.audio.eEncoding = OMX_AUDIO_CodingMP3;
	else if (nCoding == VO_AUDIO_CodingAAC)
		m_portType.format.audio.eEncoding = OMX_AUDIO_CodingAAC;
	else if (nCoding == VO_AUDIO_CodingWMA)
		m_portType.format.audio.eEncoding = OMX_AUDIO_CodingWMA;
	else if (nCoding == VO_AUDIO_CodingAMRNB)
		m_portType.format.audio.eEncoding = OMX_AUDIO_CodingAMR;

	errType = m_pComp->SetParameter (OMX_IndexParamPortDefinition, &m_portType);
	if (errType != OMX_ErrorNone)
		return errType;

	OMX_AUDIO_PARAM_PORTFORMATTYPE tpAudioFormat;
	m_pComp->SetHeader (&tpAudioFormat, sizeof (OMX_AUDIO_PARAM_PORTFORMATTYPE));
	tpAudioFormat.nPortIndex = m_nIndex;
	tpAudioFormat.eEncoding = m_portType.format.audio.eEncoding;
	errType = m_pComp->SetParameter (OMX_IndexParamAudioPortFormat, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	// how to set the audio format information?

	return errType;
}

OMX_ERRORTYPE CBasePort::GetPortDefinition(OMX_PTR pParam) {

	if (pParam == NULL) return OMX_ErrorBadParameter;

    OMX_PARAM_PORTDEFINITIONTYPE *pdef = (OMX_PARAM_PORTDEFINITIONTYPE*)pParam;
    pdef->nPortIndex = m_nIndex;
    OMX_ERRORTYPE errType = m_pComp->GetParameter(OMX_IndexParamPortDefinition, pdef);
	if (errType != OMX_ErrorNone) return OMX_ErrorUndefined;

	return errType;
}


OMX_ERRORTYPE CBasePort::CreateBufferHead (void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (m_ppBuffHead != NULL)
		delete []m_ppBuffHead;

	m_nBuffCount = m_portType.nBufferCountActual;
	if (m_nBuffCount <= 0)
		return errType;

	m_ppBuffHead = new OMX_BUFFERHEADERTYPE * [m_nBuffCount];
	memset (m_ppBuffHead, 0, m_nBuffCount * 4);

	return errType;
}

OMX_ERRORTYPE CBasePort::FillPortType (void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	m_pComp->SetHeader (&m_portType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	m_portType.nPortIndex = m_nIndex;

	errType = m_pComp->GetParameter (OMX_IndexParamPortDefinition, &m_portType);
	if (errType != OMX_ErrorNone)
		return errType;

	if (m_portType.eDir == OMX_DirOutput)
	{
		if (m_portType.eDomain == OMX_PortDomainVideo)
		{
			m_nVideoWidth = m_portType.format.video.nFrameWidth;
			m_nVideoHeight = m_portType.format.video.nFrameHeight;
		}
	}

	if (m_portType.eDir == OMX_DirOutput)
		m_bSupplier = OMX_TRUE;
	else
		m_bSupplier = OMX_FALSE;

	OMX_PARAM_BUFFERSUPPLIERTYPE bufSupplier;
	m_pComp->SetHeader (&bufSupplier, sizeof (OMX_PARAM_BUFFERSUPPLIERTYPE));

#ifdef _CHECK_SUPPLIER
	errType = m_pComp->GetParameter (OMX_IndexParamCompBufferSupplier, &bufSupplier);
	if (errType == OMX_ErrorNone)
	{
		if (bufSupplier.eBufferSupplier == OMX_BufferSupplyInput && m_portType.eDir == OMX_DirInput)
			m_bSupplier = OMX_TRUE;
		else if (bufSupplier.eBufferSupplier == OMX_BufferSupplyOutput && m_portType.eDir == OMX_DirOutput)
			m_bSupplier = OMX_TRUE;
		else
			m_bSupplier = OMX_FALSE;
	}
#endif // _CHECK_SUPPLIER

	if (!strcmp (m_pComp->GetCompName(), "OMX.Nvidia.std.iv_renderer.overlay.yuv420"))
	{
		m_bSupplier = OMX_TRUE;
	}

	return OMX_ErrorNone;
}

VO_U32	CBasePort::GetVideoDecDimension (void* pdeminfo) {

	if (pdeminfo == NULL) return VO_ERR_INVALID_ARG;

	memcpy(pdeminfo, (void*)&m_deminfo, sizeof(m_deminfo));
	
	return 0;
 }
