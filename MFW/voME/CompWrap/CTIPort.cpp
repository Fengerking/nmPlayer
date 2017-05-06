	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CTIPort.cpp

	Contains:	CTIPort class file

	Written by:	David @ 2011/11/29

*******************************************************************************/
#if defined LOG_TAG
#undef LOG_TAG
#define LOG_TAG "CTIPort"
#endif

#include <string.h>
#include "voString.h"
#include "CTIComp.h"
#include "CTIPort.h"

#include "voOMXOSFun.h"
#include "voomxdefs.h"
#include "voLog.h"

CTIPort::CTIPort(CBaseComp * pComp, OMX_U32 nIndex)
	: CBasePort(pComp, nIndex)
	, m_nMinUndequeuedBuffers(2)
	, m_nOffset(0)
	, m_pBufferState(NULL)
{

	FillPortType ();
	m_pComponent = m_pComp->GetComponentHandle ();
}

CTIPort::~CTIPort()
{
	if (m_pBufferState) 
		delete []m_pBufferState;
}

OMX_ERRORTYPE CTIPort::FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
	voCOMXAutoLock lock (&m_muxBuffer);
	
	CBufferItem * pItem = m_lstFull.RemoveBufferHead (pBuffer);
	if (pItem != NULL)
		m_lstFree.AddBufferItem (pItem);

	return OMX_ErrorNone;
}

VO_U32 CTIPort::GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
	voCOMXAutoLock lock (&m_muxBuffer);

	m_pOutputItem = m_lstFree.RemoveBufferItem();
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
		if (m_nColorType == VO_COLOR_YUV_PLANAR420_NV12 || m_nColorType == VO_COLOR_YUV_420_PACK)
		{
			if (pFilledBuffer->nOffset != m_nOffset) {
				m_nOffset = pFilledBuffer->nOffset;
				OMX_S32 left = m_nOffset % ARM_4K_PAGE_SIZE;
				OMX_S32 top = m_nOffset / ARM_4K_PAGE_SIZE;
				OMX_S32 right = m_nVideoWidth + left;
				OMX_S32 bottom = m_nVideoHeight + top;
				nwSetCrop(left, top, right, bottom);
			}
			pOutBuffer->Buffer[0] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset;
			pOutBuffer->Buffer[1] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset + m_nPadWidth * m_nPadHeight;
			pOutBuffer->Stride[0] = m_nPadWidth;
			pOutBuffer->Stride[1] = m_nPadWidth;
			pOutBuffer->ColorType = m_nColorType;	
			pOutBuffer->Buffer[2] = (VO_PBYTE)pFilledBuffer->pAppPrivate;
		}
		else if (m_nColorType == VO_COLOR_YUV_420_PACK_2)
		{
			pOutBuffer->Buffer[0] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset;
			pOutBuffer->Buffer[1] = pFilledBuffer->pBuffer + pFilledBuffer->nOffset +  m_nVideoWidth * m_nVideoHeight;

			pOutBuffer->Stride[0] = m_nVideoWidth;
			pOutBuffer->Stride[1] = m_nVideoWidth;
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
	return VO_ERR_NONE;
}


OMX_ERRORTYPE CTIPort::AllocBuffer (void)
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

		int ret = nwAllocBuffers();
		if (ret != 0)
			return OMX_ErrorInsufficientResources;

		OMX_U32 cancelStart = m_portType.nBufferCountActual - m_nMinUndequeuedBuffers;
		for (OMX_U32 i = 0; i < cancelStart; i++)
			m_lstFree.AddBufferHead (m_ppBuffHead[i]);
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

OMX_ERRORTYPE CTIPort::FreeBuffer (void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	voCOMXAutoLock lock (&m_muxBuffer);
	for (OMX_U32 i = 0; i < m_nBuffCount; i++)
	{
		if (m_ppBuffHead[i] != NULL)
			nwFreeBuffer(m_ppBuffHead[i]->pAppPrivate);

		errType = m_pComponent->FreeBuffer (m_pComponent, m_nIndex, m_ppBuffHead[i]);
		if (errType != OMX_ErrorNone)
			return errType;

		m_ppBuffHead[i] = NULL;
	}

	m_lstFree.ResetItems ();
	m_lstFull.ResetItems ();

	return errType;
}

OMX_ERRORTYPE CTIPort::StartBuffer (void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (m_ppBuffHead == NULL)
		return OMX_ErrorNotImplemented;

	if (m_portType.eDir == OMX_DirOutput)
	{
		voCOMXAutoLock lock (&m_muxBuffer);
		CBufferItem *	pItem = m_lstFree.RemoveBufferItem();

		while (pItem != NULL)
		//for (OMX_U32 i = 0; i < cancelEnd; ++i) 
		{
			OMX_BUFFERHEADERTYPE *	pEmptyBuffer = pItem->m_pBufferHead;
			//OMX_BUFFERHEADERTYPE *	pEmptyBuffer = nwPrepareBuffer();
			if (pEmptyBuffer != NULL)
			{
				if (nwIsBufferDequeued(pEmptyBuffer->pAppPrivate) == OMX_TRUE) {
					nwLockBuffer(pEmptyBuffer->pAppPrivate);
				}

				pEmptyBuffer->nFlags = 0;
				errType = m_pComponent->FillThisBuffer (m_pComponent, pEmptyBuffer);
				if (errType != OMX_ErrorNone)
				{
					VOLOGE ("FillThisBuffer return error %08X", errType);
					return errType;
				}
			}

			m_lstFull.AddBufferItem (pItem);
			//m_lstFull.AddBufferHead(pEmptyBuffer);
			pItem = m_lstFree.RemoveBufferItem();
		}
	}

	return errType;
}

VO_U32 CTIPort::FillOutputBuffer()
{
	voCOMXAutoLock lock (&m_muxBuffer);
	
	if (m_pOutputItem == NULL) 
		return VO_ERR_RETRY;

	OMX_BUFFERHEADERTYPE* pFillBuff = nwPrepareBuffer();
	if (pFillBuff != NULL)
	{	
		pFillBuff->nFlags = 0;
		OMX_ERRORTYPE errType = OMX_ErrorNone;

		errType = m_pComponent->FillThisBuffer(m_pComponent, pFillBuff);
		if (errType == OMX_ErrorNone) {
			m_lstFull.AddBufferHead(pFillBuff);
		} else {
			VOLOGE ("FillThisBuffer return error %08X", errType);
			nwCancelBuffer(pFillBuff->pAppPrivate);
		}

		pFillBuff = NULL;
	}

	return VO_ERR_NONE;
}

#define MAX(a, b) (a) > (b) ? (a):(b)

OMX_ERRORTYPE CTIPort::SetVideoInfo (VO_VIDEO_CODINGTYPE nCoding, VO_IV_COLORTYPE nColorType, OMX_U32  nWidth, OMX_U32 nHeight)
{
	voCOMXAutoLock lock (&m_muxBuffer);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	m_nVideoWidth = (nWidth + 15) & ~0X0F;
	m_nVideoHeight = (nHeight + 15) & ~0X0F;
	m_nColorType = nColorType;

	errType = m_pComp->GetParameter(OMX_IndexParamPortDefinition, &m_portType);

	m_portType.format.video.nFrameWidth = m_nVideoWidth;
	m_portType.format.video.nFrameHeight = m_nVideoHeight;
	m_portType.nBufferSize = MAX(m_nVideoWidth * m_nVideoHeight, m_portType.nBufferSize);

	VO_COMPWRAP_CODECTYPE ComWrapType = m_pComp->GetComWrapType();
	if(ComWrapType != VO_COMPWRAP_CODEC_VideoDec) return OMX_ErrorNotImplemented;

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

		if (m_portType.nBufferSize < 64 * 1024)
			m_portType.nBufferSize = 64 * 1024;

		m_portType.nBufferCountActual = 4;
		errType = m_pComp->SetParameter (OMX_IndexParamPortDefinition, &m_portType);
		if (errType != OMX_ErrorNone)
			return errType;

		if (nCoding == VO_VIDEO_CodingH264)
		{
			OMX_VIDEO_PARAM_AVCTYPE h264type;
			m_pComp->SetHeader(&h264type, sizeof(OMX_VIDEO_PARAM_AVCTYPE));
			h264type.nPortIndex = m_nIndex;

			errType = m_pComponent->GetParameter(m_pComponent, OMX_IndexParamVideoAvc, &h264type);
			if(errType == OMX_ErrorNone)
			{
				OMX_VIDEO_PARAM_AVCTYPE srcAVCtype;
				memset(&srcAVCtype, 0, sizeof(OMX_VIDEO_PARAM_AVCTYPE));
				VO_U32 err= VO_ERR_NONE;
				err = m_pComp->GetSourceParam(VO_PID_VIDEO_ParamVideoAvc, &srcAVCtype);
				if(err == VO_ERR_NONE) {
					h264type.nRefFrames = srcAVCtype.nRefFrames;

					h264type.eProfile = srcAVCtype.eProfile;
					h264type.eLevel = srcAVCtype.eLevel;
				}
				errType = m_pComponent->SetParameter(m_pComponent, OMX_IndexParamVideoAvc, &h264type);
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
			m_nColorType = VO_COLOR_YUV_PLANAR420_NV12;
			break;
		default:
			break;
		}

		//configure h/w related parameters
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

		// Increase the buffer count by one to allow for the ANativeWindow to hold on one of the buffers.
		// TI is 2 generally
		m_nMinUndequeuedBuffers = nwGetMinUndequeuedBuffers();
		if (m_nMinUndequeuedBuffers < 2) 
			m_nMinUndequeuedBuffers = 2;
		
		//Due to the way mecahnism to update nBufferCountActual is most likely to keep
		//using 22 Output Buffers for every resolution, hence setting
		//nBufferCountActual to minimun allowed.
		m_portType.nBufferCountActual = m_portType.nBufferCountMin;
		if (m_portType.nBufferCountActual < m_portType.nBufferCountMin + m_nMinUndequeuedBuffers)
			m_portType.nBufferCountActual += m_nMinUndequeuedBuffers;
		m_portType.format.video.nStride = ARM_4K_PAGE_SIZE; // hard coded to compliant with TI HW Decoder's feature;
		m_portType.nBufferSize = MAX(ARM_4K_PAGE_SIZE * m_nPadHeight * 3 / 2, m_portType.nBufferSize);

		errType = m_pComp->SetParameter (OMX_IndexParamPortDefinition, &m_portType);
		if (errType != OMX_ErrorNone)
			return errType;

		OMX_CONFIG_RECTTYPE rect;
		m_pComp->SetHeader(&rect, sizeof(OMX_CONFIG_RECTTYPE));
		rect.nVersion.s.nVersionMinor = 1;
		rect.nPortIndex = m_nIndex;
		m_pComponent->GetParameter(m_pComponent, (OMX_INDEXTYPE)OMX_IndexConfigCommonOutputCrop, &rect);
	}			

	return errType;
}

OMX_ERRORTYPE CTIPort::ResetCrop(void) {
	
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_CONFIG_RECTTYPE rect;
	m_pComp->SetHeader(&rect, sizeof(OMX_CONFIG_RECTTYPE));
	rect.nPortIndex = m_nIndex;
	errType = m_pComponent->GetParameter(m_pComponent, (OMX_INDEXTYPE)OMX_IndexConfigCommonOutputCrop, &rect);
	if (errType == OMX_ErrorNone)
		errType = (OMX_ERRORTYPE)nwSetCrop(rect.nLeft, rect.nTop, rect.nLeft + rect.nWidth, rect.nTop + rect.nHeight);
	
	return errType;
}

/*OMX_S32 CTIPort::GetGraphicBufferUsage(OMX_PTR pUserData, OMX_PTR pparam) {
	CTIPort* pHole = (CTIPort*)pUserData;
	return pHole->getGraphicBufferUsage((OMX_U32*)pparam);
}*/

OMX_S32 CTIPort::UseGraphicBuffer(OMX_PTR pUserData, OMX_PTR pparam)
{
	CTIPort* pHole = (CTIPort*)pUserData;
	return pHole->useGraphicBuffer(pparam);
}

VO_S32 CTIPort::useGraphicBuffer(OMX_PTR pparam) {

	voCOMXAutoLock lock (&m_muxBuffer);

	VOGRAPHICBUFFERWRAPPER* pw = (VOGRAPHICBUFFERWRAPPER*)pparam;
	OMX_BUFFERHEADERTYPE** header = (OMX_BUFFERHEADERTYPE**)pw->ppHeader;
	OMX_U8* pBuffer = pw->pBuffer;
	void* bufferMeta = pw->pBufferMeta;

	OMX_PARAM_PORTDEFINITIONTYPE portdef;
	InitOMXParams(&portdef);
	portdef.nVersion.s.nVersionMinor = 1;
	portdef.nPortIndex = m_nIndex;
	OMX_ERRORTYPE err = m_pComponent->GetParameter(m_pComponent, OMX_IndexParamPortDefinition, &portdef);
	if (err != OMX_ErrorNone) {
		VOLOGE("Failed to get OMX_IndexParamPortDefinition!");
	}

	err = m_pComponent->UseBuffer(m_pComponent, header, m_nIndex, bufferMeta, portdef.nBufferSize, pBuffer);
	if (err != OMX_ErrorNone) {
		VOLOGE("OMX_UseAndroidNativeBuffer failed with error 0x%08x", err);
		return err;
	}

	return OMX_ErrorNone;

#if 0  // unsupported by TI hardware of Pasteur
	OMX_INDEXTYPE nIndex;
	err = m_pComponent->GetExtensionIndex(m_pComponent, const_cast<OMX_STRING>("OMX.google.android.index.useAndroidNativeBuffer"), &nIndex);
	if(errType != OMX_ErrorNone)
	{
		VOLOGE("OMX_GetExtensionIndex failed 0x%08X", errType);
		return -1;
	}

	errType = m_pComponent->SetParameter(m_pComponent, nIndex, pparam);
	if(errType != OMX_ErrorNone)
	{
		VOLOGE("OMX_UseAndroidNativeBuffer failed with error 0x%08x", errType);
		return -1;
	}
#endif

    return err;
}
	
OMX_S32 CTIPort::nwAllocBuffers()
{
	VOMP_NATIVEWINDOWALLOCBUFFER sAllocBuffer;
	sAllocBuffer.nPortIndex = m_nIndex;
	sAllocBuffer.nWidth = m_nPadWidth;
	sAllocBuffer.nHeight = m_nPadHeight;
	sAllocBuffer.nColor = 0x100;
	sAllocBuffer.ppBufferHeader = (OMX_PTR*)m_ppBuffHead;
	sAllocBuffer.nBufferCount = m_portType.nBufferCountActual;
	sAllocBuffer.fUseGraphicBuffer = (VOMPUSEGRAPHICBUFFER)CTIPort::UseGraphicBuffer;
	//sAllocBuffer.fGetGraphicBufferUsage = (VOMPGETGRAPHICBUFFERUSAGE)CTIPort::GetGraphicBufferUsage;
	sAllocBuffer.pUserData = this;
	sAllocBuffer.nBufferSize = m_portType.nBufferSize;

	VOMP_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOMP_NATIVEWINDOW_AllocBuffers;
	sCommand.pParam1 = &sAllocBuffer;
	
	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = mycallback(VOMP_CB_NATIVEWINDOW, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOMP_NATIVEWINDOW_AllocBuffers callback error.");

	OMX_U32 cancelStart, cancelEnd;
	if (nRet != 0) {
		// If an error occurred while dequeuing we need to cancel any buffers
		// that were dequeued.
		cancelStart = 0;
		cancelEnd = m_portType.nBufferCountActual;
	} else {
		// Return at least the last two buffers to the native window.
		cancelStart = m_portType.nBufferCountActual - m_nMinUndequeuedBuffers; 
		cancelEnd = m_portType.nBufferCountActual;
	}

	m_pBufferState = new VONWBUFLOCKSTATE[m_portType.nBufferCountActual];
	memset(m_pBufferState, 0, sizeof(VONWBUFLOCKSTATE) * m_portType.nBufferCountActual);
	for (OMX_U32 i = 0; i <  m_portType.nBufferCountActual; i++) {
		m_pBufferState[i].pAppPrivate = m_ppBuffHead[i]->pAppPrivate;
		m_pBufferState[i].bLocked = false;
	}

	for (OMX_U32 i = cancelEnd - 1; i >= cancelStart; i--) {
		OMX_BUFFERHEADERTYPE* pBuffer = m_ppBuffHead[i];
		nwCancelBuffer(pBuffer->pAppPrivate);
	}

	return nRet;
}


OMX_S32 CTIPort::nwCancelBuffer(OMX_PTR pNativeBuffer)
{
	VOMP_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOMP_NATIVEWINDOW_CancelBuffer;
	sCommand.pParam1 = pNativeBuffer;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = mycallback(VOMP_CB_NATIVEWINDOW, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOMP_NATIVEWINDOW_CancelBuffer callback error.");

	for (OMX_U32 i = 0; i <  m_portType.nBufferCountActual; i++) {
		if (m_pBufferState[i].pAppPrivate == pNativeBuffer)
			m_pBufferState[i].bLocked = false;
	}

	return nRet;
}

OMX_S32 CTIPort::nwQueueBuffer(OMX_PTR pNativeBuffer)
{
	VOMP_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOMP_NATIVEWINDOW_QueueBuffer;
	sCommand.pParam1 = pNativeBuffer;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = mycallback(VOMP_CB_NATIVEWINDOW, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOMP_NATIVEWINDOW_QueueBuffer callback error.");

	return nRet;
}

OMX_BUFFERHEADERTYPE* CTIPort::nwPrepareBuffer()
{

	VOMP_NATIVEWINDOWBUFFERHEADERS sBufferHeaders;
	sBufferHeaders.nBufferCount = m_nBuffCount;
	sBufferHeaders.ppBufferHeader = (OMX_PTR*)m_ppBuffHead;

	VOMP_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOMP_NATIVEWINDOW_PrepareBuffer;
	int locktype = 1;
	sCommand.pParam1 = &locktype;
	sCommand.pParam2 = &sBufferHeaders;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = mycallback(VOMP_CB_NATIVEWINDOW, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOMP_NATIVEWINDOW_PrepareBuffer callback error.");

	OMX_BUFFERHEADERTYPE* header = m_ppBuffHead[locktype];
	for (OMX_U32 i = 0; i <  m_portType.nBufferCountActual; i++) {
		if (m_pBufferState[i].pAppPrivate == header->pAppPrivate)
			m_pBufferState[i].bLocked = true;
	}
	
	return header;
}

OMX_S32 CTIPort::nwLockBuffer(OMX_PTR pNativeBuffer)
{
	VOMP_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOMP_NATIVEWINDOW_LockBuffer;
	sCommand.pParam1 = pNativeBuffer;
	int locktype = 1;
	sCommand.pParam2 = &locktype;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = mycallback(VOMP_CB_NATIVEWINDOW, (OMX_PTR)&sCommand, 0);
	if(nRet != 0) {
		VOLOGE("VOMP_NATIVEWINDOW_LockBuffer callback error.");
		return -1;
	}

	for (OMX_U32 i = 0; i <  m_portType.nBufferCountActual; i++) {
		if (m_pBufferState[i].pAppPrivate == pNativeBuffer)
			m_pBufferState[i].bLocked = true;
	}

	return 0;
}

OMX_S32 CTIPort::nwSetCrop(OMX_S32 nLeft, OMX_S32 nTop, OMX_S32 nRight, OMX_S32 nBottom)
{
	VOMP_NATIVEWINDOWCROP sCrop;
	sCrop.nLeft = nLeft;
	sCrop.nTop = nTop;
	sCrop.nRight = nRight;
	sCrop.nBottom = nBottom;

	VOMP_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOMP_NATIVEWINDOW_SetCrop;
	sCommand.pParam1 = &sCrop;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = mycallback(VOMP_CB_NATIVEWINDOW, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOMP_NATIVEWINDOW_SetCrop callback error.");
	return nRet;
}

OMX_S32 CTIPort::nwFreeBuffer(OMX_PTR pNativeBuffer)
{
	VOMP_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOMP_NATIVEWINDOW_FreeBuffer;
	sCommand.pParam1 = pNativeBuffer;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = mycallback(VOMP_CB_NATIVEWINDOW, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOMP_NATIVEWINDOW_FreeBuffer callback error.");

	for (OMX_U32 i = 0; i <  m_portType.nBufferCountActual; i++) {
		if (m_pBufferState[i].pAppPrivate == pNativeBuffer)
			m_pBufferState[i].bLocked = false;
	}
	return nRet;
}

OMX_BOOL CTIPort::nwIsBufferDequeued(OMX_PTR pNativeBuffer)
{
	OMX_U32 nDequeued = 0;

	VOMP_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOMP_NATIVEWINDOW_IsBufferDequeued;
	sCommand.pParam1 = pNativeBuffer;
	sCommand.pParam2 = &nDequeued;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = mycallback(VOMP_CB_NATIVEWINDOW, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOMP_NATIVEWINDOW_IsBufferDequeued callback error.");

	VOLOGR("pNativeBuffer 0x%08X isDequeued %d", nDequeued);
	return (nDequeued == 0) ? OMX_FALSE : OMX_TRUE;
}

OMX_BOOL CTIPort::nwIsBufferLocked(OMX_PTR pNativeBuffer)
{
	OMX_BOOL locked = OMX_FALSE;
	for (OMX_U32 i = 0; i <  m_portType.nBufferCountActual; i++) {
		if (m_pBufferState[i].pAppPrivate == pNativeBuffer) {
			if (nwIsBufferDequeued(pNativeBuffer) == OMX_TRUE)
				locked = (m_pBufferState[i].bLocked ? OMX_TRUE : OMX_FALSE);
			break;
		}
	}

	return locked;
}

OMX_PTR CTIPort::nwDequeueBuffer()
{
	VOMP_NATIVEWINDOWBUFFERHEADERS sBufferHeaders;
	sBufferHeaders.nBufferCount = m_nBuffCount;
	sBufferHeaders.ppBufferHeader = (OMX_PTR*)m_ppBuffHead;

	OMX_PTR pNativeBuffer;
	VOMP_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOMP_NATIVEWINDOW_DequeueBuffer;
	sCommand.pParam1 = &pNativeBuffer;
	sCommand.pParam2 = &sBufferHeaders;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = mycallback(VOMP_CB_NATIVEWINDOW, &sCommand, 0);
	if (nRet != 0)
		VOLOGE("VOMP_NATIVEWINDOW_DequeueBuffer callback error.");

	for (OMX_U32 i = 0; i <  m_portType.nBufferCountActual; i++) {
		if (m_pBufferState[i].pAppPrivate == pNativeBuffer)
			m_pBufferState[i].bLocked = false;
	}

	return pNativeBuffer;
}

OMX_U32 CTIPort::nwGetMinUndequeuedBuffers()
{
	OMX_S32 nMinUndequeuedBuffers = 0;

	VOMP_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOMP_NATIVEWINDOW_GetMinUndequeuedBuffers;
	sCommand.pParam1 = &nMinUndequeuedBuffers;
	sCommand.pParam2 = NULL;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = mycallback(VOMP_CB_NATIVEWINDOW, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
	{
		VOLOGW("get nativewindow min undequeued buffer not supported, so use default value 2");
		return 2;  // by experience
	}

	return nMinUndequeuedBuffers;
}

int CTIPort::mycallback(int nID, void * pParam1, void * pParam2) {

	VOMP_LISTENERINFO* pli = ((CTIComp*)m_pComp)->m_pListener;
	if (pli == NULL)
		return -1;

	VOMPListener listener = pli->pListener;
	VO_PTR pUserData = pli->pUserData;
	int ret = listener(pUserData, nID, pParam1, pParam2);

	return ret;
}

OMX_ERRORTYPE CTIPort::ReturnAllBuffers(void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (m_ppBuffHead == NULL)
		return OMX_ErrorNotImplemented;

	if (m_portType.eDir == OMX_DirOutput)
	{
		voCOMXAutoLock lock (&m_muxBuffer);

		CBufferItem *	pItem = m_lstFree.RemoveBufferItem();
		while (pItem != NULL)
		{
			OMX_BUFFERHEADERTYPE *	pbuf = pItem->m_pBufferHead;
			if (pbuf != NULL && nwIsBufferDequeued(pbuf->pAppPrivate) == OMX_TRUE)
				nwCancelBuffer(pbuf->pAppPrivate);

			pItem = m_lstFree.RemoveBufferItem();
		}

		pItem = m_lstFull.RemoveBufferItem();
		while (pItem != NULL)
		{
			OMX_BUFFERHEADERTYPE *	pbuf = pItem->m_pBufferHead;
			if (pbuf != NULL && nwIsBufferDequeued(pbuf->pAppPrivate) == OMX_TRUE)
				nwCancelBuffer(pbuf->pAppPrivate);

			pItem = m_lstFull.RemoveBufferItem();
		}
	}

	voCOMXAutoLock lock (&m_muxBuffer);
	OMX_U32 cancelEnd = m_portType.nBufferCountActual - m_nMinUndequeuedBuffers;
	for (OMX_U32 i = 0; i < cancelEnd; i++) {
		OMX_PTR pnativebuf = nwDequeueBuffer();
		for (OMX_U32 i = 0; i < m_portType.nBufferCountActual; i++) {
			if (pnativebuf == m_ppBuffHead[i]->pAppPrivate) {
				m_lstFree.AddBufferHead(m_ppBuffHead[i]);
			}
		}
	}

	return errType;
}

