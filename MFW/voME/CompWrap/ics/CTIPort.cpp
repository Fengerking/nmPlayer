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
#include "vonwdefs.h"
#include "voLog.h"

using namespace android;

CTIPort::CTIPort(CBaseComp * pComp, OMX_U32 nIndex, sp<IOMX>& omx, void* pn, sp<ANativeWindow>& nw)
		: CBasePort(pComp, nIndex)
		, m_nMinUndequeuedBufs(1)
		, m_nOffset(0)
		, m_nflags(0) 
		//, m_nflags(kIgnoreCodecSpecificData | kUseSecureInputBuffers)
		, m_pomx(omx)
		, m_pnode(pn)
		, m_pnativewindow(nw)
		, m_bLivesLocally(m_pomx->livesLocally(getpid()))
		, m_pdrmengine(NULL)
		, m_pBuffInfo(NULL)
{}

CTIPort::~CTIPort()
{
	if (m_pBuffInfo) {
		delete []m_pBuffInfo;
		m_pBuffInfo = NULL;
	}

	if (m_ppDataBuffer != NULL) {
		delete []m_ppDataBuffer;
		m_ppDataBuffer = 0;
	}
}

OMX_ERRORTYPE CTIPort::EmptyBuffer(OMX_BUFFERHEADERTYPE* pBuffer, OMX_U32 rangeOffset, OMX_U32 rangeLength, OMX_U32 flags, OMX_TICKS timestamp)
{
  if (pBuffer == NULL)
	return OMX_ErrorNotReady;

  return (OMX_ERRORTYPE)m_pomx->emptyBuffer(m_pnode, pBuffer, rangeOffset, rangeLength, flags, timestamp);
}

OMX_ERRORTYPE CTIPort::FillBuffer(OMX_BUFFERHEADERTYPE* pBuffer) {

  return (OMX_ERRORTYPE)m_pomx->fillBuffer(m_pnode, pBuffer);
}

OMX_ERRORTYPE CTIPort::EmptyBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)
{
  voCOMXAutoLock lock (&m_muxBuffer);
  CBufferItem * pItem = m_lstFull.RemoveBufferHead (pBuffer);

  if (pItem != NULL)
	m_lstFree.AddBufferItem (pItem);

  return OMX_ErrorNone;
}

OMX_ERRORTYPE CTIPort::FillBufferDone(OMX_BUFFERHEADERTYPE* pBuffer)
{
  voCOMXAutoLock lock(&m_muxBuffer);

  CBufferItem * pItem = m_lstFull.RemoveBufferHead(pBuffer);
  if (pItem != NULL)
	m_lstFree.AddBufferItem(pItem);

  return OMX_ErrorNone;
}

//static int g_ntotal = 0;
VO_U32 CTIPort::SetVideoInputData(VO_CODECBUFFER* pInput, VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_BOOL bHeader)
{
	voCOMXAutoLock lock (&m_muxBuffer);

	CBufferItem* pItem = m_lstFree.RemoveBufferItem();
	if (pItem == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	OMX_BUFFERHEADERTYPE* peb = pItem->m_pBufferHead;
	if (peb == NULL)
	{
		m_lstFree.AddBufferItem(pItem);
		return VO_ERR_NOT_IMPLEMENT;
	}

	OMX_U32 ndatalen = 0;
	void* pbuf = m_pBuffInfo[findInputBuffer(peb)].pdata;

	if (bHeader)
	{
		memcpy(pbuf, pHeadData, nHeadSize);
		ndatalen = nHeadSize;
	}
	else
	{
		ndatalen = pInput->Length;
		memcpy(pbuf, pInput->Buffer, ndatalen);

		/*FILE *pf = NULL;
		if (g_ntotal == 0) {
			VOLOGI("head data 32 bytes..............................................................");
			ndatalen = 32;
			pf = fopen("/sdcard/seqhead.data", "r");
			if (pf) {
				fread(pbuf, 1, ndatalen, pf);
				fclose(pf);
			}
		} else {
			VOLOGI("first frame data 17015 bytes..............................................................");
			ndatalen = 17015;
			pf = fopen("/sdcard/firstframe.data", "r");
			if (pf) {
				fread(pbuf, 1, ndatalen, pf);
				fclose(pf);
			}
		}*/
	}

	
	OMX_U32 flags = 0;
	if (bHeader)
	//if (g_ntotal == 0)
		flags |=  OMX_BUFFERFLAG_ENDOFFRAME | OMX_BUFFERFLAG_CODECCONFIG;
	else
		flags |=  OMX_BUFFERFLAG_ENDOFFRAME;
	
	//g_ntotal++;
	// log out the input information
	m_nLastBufferTime = pInput->Time;

	OMX_ERRORTYPE errType = EmptyBuffer(peb, 0, ndatalen, flags, pInput->Time);
	if (errType == OMX_ErrorNone)
		m_lstFull.AddBufferItem (pItem);
	else
		VOLOGE ("EmptyThisBuffer return error %08X", errType);

	return VO_ERR_NONE;
}

VO_U32 CTIPort::GetVideoOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
	voCOMXAutoLock lock(&m_muxBuffer);

	m_pOutputItem = m_lstFree.RemoveBufferItem();
	if (m_pOutputItem == NULL)
		return VO_ERR_RETRY;

	OMX_BUFFERHEADERTYPE * pFilledBuffer = m_pOutputItem->m_pBufferHead;
	//VOLOGI("get output buffer head: %p....", pFilledBuffer);
	//VOLOGI("buffer header: %p", pFilledBuffer);

	nwQueueBuffer(pFilledBuffer);

	pOutBuffer->Buffer[0] = (unsigned char*)pFilledBuffer;
	pOutBuffer->Buffer[1] = NULL;//(unsigned char*)pFilledBuffer;
	pOutBuffer->Buffer[2] = NULL;//(unsigned char*)pFilledBuffer;

	pOutBuffer->Stride[0] = m_nVideoWidth;
	pOutBuffer->Stride[1] = m_nVideoWidth / 2;
	pOutBuffer->Stride[2] = m_nVideoWidth / 2;

	pOutBuffer->ColorType = VO_COLOR_YUV_420_PACK;
	pOutInfo->Flag = 0;
	pOutInfo->Format.Width = m_nVideoWidth;
	pOutInfo->Format.Height = m_nVideoHeight;
	pOutBuffer->Time = -1;

	return VO_ERR_NONE;
}

OMX_ERRORTYPE CTIPort::AllocBuffer(OMX_PARAM_PORTDEFINITIONTYPE* pPortDef)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE* pdef;
	if (pPortDef)
		pdef = pPortDef;
	else
		pdef = &m_portType;

	OMX_VIDEO_PORTDEFINITIONTYPE *vdef = &pdef->format.video;
	m_nVideoWidth = (vdef->nFrameWidth + 15) & ~0x0f;
	m_nVideoHeight = (vdef->nFrameHeight + 15) & ~0x0f;

	m_nBuffCount = pdef->nBufferCountActual;
//	VOLOGI("port %d m_bSupplier is %d, buffercount: %d, buffsize: %d, min", m_nIndex, m_bSupplier, m_nBuffCount, pdef->nBufferSize, pdef->nBufferCountMin);
	CreateBufferHead();

	if (m_ppBuffHead == NULL)
		return OMX_ErrorNotReady;

	if (m_pBuffInfo != NULL) 
		delete []m_pBuffInfo;

	m_pBuffInfo = new VOBUFFERINFO[m_nBuffCount];
	memset(m_pBuffInfo, 0, sizeof(VOBUFFERINFO) * m_nBuffCount);

	//VOLOGI("port %d m_bSupplier is %d, buffercount: %d, buffsize: %d", m_nIndex, m_bSupplier, m_nBuffCount, pdef->nBufferSize);
	status_t err;
	voCOMXAutoLock lock(&m_muxBuffer);
	if (m_nIndex == kPortIndexInput) {

		if (m_nflags & kStoreMetaDataInVideoBuffers) {

			err = m_pomx->storeMetaDataInBuffers(m_pnode, m_nIndex, OMX_TRUE);
			if (err != OK) {
				LOGE("Storing meta data in video buffers is not supported");
				return (OMX_ERRORTYPE)err;
			}

		}

		size_t totalSize = pdef->nBufferSize * m_nBuffCount;
		m_pmemdealer = new MemoryDealer(totalSize, "TI DUCATI INPUT PORT");

		if (m_bLivesLocally) {

			if (m_ppDataBuffer != NULL)
				delete []m_ppDataBuffer;

			m_ppDataBuffer = new OMX_U8*[m_nBuffCount];
		}

		for (OMX_U32 i = 0; i < m_nBuffCount; i++) {
			sp<IMemory> mem = m_pmemdealer->allocate(pdef->nBufferSize);
			//VOLOGI("mem.get(): %p", mem.get());

			if (!m_bSupplier) 
				break;

			if (m_nflags & kUseSecureInputBuffers) {

			//VOLOGI("port %d  buffhead[%d]: %p", m_nIndex, i, m_ppBuffHead[i]);
				if (m_bLivesLocally) {
					mem.clear();
					err = m_pomx->allocateBuffer(m_pnode, m_nIndex, pdef->nBufferSize, (void**)&m_ppBuffHead[i], (void**)&m_ppDataBuffer[i]);
				} else {
					err = m_pomx->allocateBufferWithBackup(m_pnode, m_nIndex, mem, (void**)&m_ppBuffHead[i]);
				}

			} else {
				err = m_pomx->allocateBufferWithBackup(m_pnode, m_nIndex, mem, (void**)&m_ppBuffHead[i]);
				VOLOGI("head: %p", m_ppBuffHead[i]);
			}

			if (err != OK) {
				LOGE("allocate_buffer_with_backup failed");
				return (OMX_ERRORTYPE)err;
			}

			m_pBuffInfo[i].pBuffHead = m_ppBuffHead[i];
			m_pBuffInfo[i].pdata = mem->pointer();
			m_pBuffInfo[i].nsize = pdef->nBufferSize;
			//VOLOGI("port %d  buffhead[%d]: %p, buffercount: %d", m_nIndex, i, m_ppBuffHead[i], m_nBuffCount);
			m_lstFree.AddBufferHead(m_ppBuffHead[i]);
		}
	} else if (m_nIndex == kPortIndexOutput) {

		//VOLOGI("Output port m_bSupplier is %d, buffercount: %d", m_bSupplier, m_nBuffCount);


		int ret = nwAllocBuffers();
		if (ret != 0)
			return OMX_ErrorInsufficientResources;

		OMX_U32 cancelStart = pdef->nBufferCountActual - m_nMinUndequeuedBufs;
		for (OMX_U32 i = 0; i < cancelStart; i++)
			m_lstFree.AddBufferHead(m_ppBuffHead[i]);
	}

	return errType;
}

OMX_ERRORTYPE CTIPort::FreeBuffer(void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	voCOMXAutoLock lock(&m_muxBuffer);
	//VOLOGI("buffercount: %d", m_nBuffCount);

	for (OMX_U32 i = 0; i < m_nBuffCount; i++) {

		if (m_portType.eDir == OMX_DirOutput) {
			//VOLOGI("cancel buffer %d  %p", i, m_ppBuffHead[i]);
			nwFreeBuffer(m_ppBuffHead[i]);
		}

	}

	for (OMX_U32 i = 0; i < m_nBuffCount; i++) {

		//VOLOGI("omx free buffer %d %p", i, m_ppBuffHead[i]);
		errType = (OMX_ERRORTYPE)m_pomx->freeBuffer(m_pnode, m_nIndex, m_ppBuffHead[i]);

		if (errType != OMX_ErrorNone)
			return errType;

		m_ppBuffHead[i] = NULL;
	}

	m_lstFree.ResetItems();
	m_lstFull.ResetItems();

	return errType;
}

OMX_ERRORTYPE CTIPort::StartBuffer(void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_ppBuffHead == NULL)
	  return OMX_ErrorNotImplemented;

	//VOLOGI("m_portType.eDir: %d", m_portType.eDir);
	if (m_nIndex == kPortIndexOutput) {

		//VOLOGI("port %d has %d free buffers", m_nIndex, m_lstFree.GetCount());
		voCOMXAutoLock lock(&m_muxBuffer);
		CBufferItem* pItem = m_lstFree.RemoveBufferItem();

		while (pItem != NULL) {

			OMX_BUFFERHEADERTYPE* pe = pItem->m_pBufferHead;

			if (pe != NULL) {

				//VOLOGI("[ port %d buf info ] head: %p", m_nIndex, pe);
				nwLockBuffer(pe);
				errType = FillBuffer(pe);
				if (errType != OMX_ErrorNone) {

					VOLOGE("FillBuffer return error %08X", errType);
					return errType;
				}
		  }

		  m_lstFull.AddBufferItem(pItem);
		  pItem = m_lstFree.RemoveBufferItem();
		}
	} else if (m_nIndex == kPortIndexInput) {

		//VOLOGI("port %d has %d free buffers", m_nIndex, m_lstFree.GetCount());
		/*CBufferItem* pi = m_lstFree.GetHead();
		while (pi) {
			OMX_BUFFERHEADERTYPE* pe = pi->m_pBufferHead;
			VOLOGI("[ port %d buf info ] head: %p", m_nIndex, pe);
			pi = pi->m_pNext;
		}*/
	}

	//VOLOGI("startup buffers over.....");
	return errType;
}

VO_U32 CTIPort::FillOutputBuffer()
{
  voCOMXAutoLock lock(&m_muxBuffer);

  if (m_pOutputItem == NULL) 
	return VO_ERR_RETRY;

  OMX_BUFFERHEADERTYPE* pFillBuff = nwPrepareBuffer();
  if (pFillBuff != NULL)
  {	
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	errType = FillBuffer(pFillBuff);
	if (errType == OMX_ErrorNone) {
	  m_lstFull.AddBufferHead(pFillBuff);
	} else {
	  VOLOGE("FillBuffer return error %08X", errType);
	  nwCancelBuffer(pFillBuff);
	}

	pFillBuff = NULL;
  }

  return VO_ERR_NONE;
}

#define MAX(a, b)(a) >(b) ?(a):(b)

OMX_ERRORTYPE CTIPort::SetVideoInfo(VO_VIDEO_CODINGTYPE nCoding, VO_IV_COLORTYPE nColorType, OMX_U32  nWidth, OMX_U32 nHeight)
{
	voCOMXAutoLock lock(&m_muxBuffer);

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	m_nVideoWidth =(nWidth + 15) & ~0X0F;
	m_nVideoHeight =(nHeight + 15) & ~0X0F;
	m_nColorType = nColorType;
	//VOLOGI(" port %d [ video ] w x h = %d x %d, [ m_nVideo ] w x h = %d x %d", m_nIndex, nWidth, nHeight, m_nVideoWidth, m_nVideoHeight);

	OMX_PARAM_PORTDEFINITIONTYPE def;
	OMX_VIDEO_PORTDEFINITIONTYPE *vdef = &def.format.video;

	if (m_nIndex == kPortIndexInput) {

		OMX_VIDEO_CODINGTYPE compressionFormat = OMX_VIDEO_CodingUnused;

		if (nCoding == VO_VIDEO_CodingMPEG2)
			compressionFormat = OMX_VIDEO_CodingMPEG2;
		else if (nCoding == VO_VIDEO_CodingH263)
			compressionFormat = OMX_VIDEO_CodingH263;
		else if (nCoding == VO_VIDEO_CodingMPEG4)
			compressionFormat = OMX_VIDEO_CodingMPEG4;
		else if (nCoding == VO_VIDEO_CodingH264)
			compressionFormat = OMX_VIDEO_CodingAVC;
		else {
			LOGE("Not a supported video code type: %d", nCoding);
		}

		//VOLOGI("compressionFormat: %d", compressionFormat);
		errType = (OMX_ERRORTYPE)setVideoPortFormatType(m_nIndex, compressionFormat, OMX_COLOR_FormatUnused);
		if (errType != OMX_ErrorNone)
			return errType;

		if (nCoding == VO_VIDEO_CodingH264) {
			OMX_VIDEO_PARAM_AVCTYPE h264type;
			size_t size = sizeof(OMX_VIDEO_PARAM_AVCTYPE);
			InitOMXParams(&h264type);
			h264type.nPortIndex = m_nIndex;

			errType = m_pComp->GetParameter(OMX_IndexParamVideoAvc, &h264type, size);
			if (errType == OMX_ErrorNone) {
				OMX_VIDEO_PARAM_AVCTYPE srcAVCtype;
				memset(&srcAVCtype, 0, sizeof(OMX_VIDEO_PARAM_AVCTYPE));
				VO_U32 err= VO_ERR_NONE;
				err = m_pComp->GetSourceParam(VO_PID_VIDEO_ParamVideoAvc, &srcAVCtype);
				if (err == VO_ERR_NONE) {
					h264type.nRefFrames = 2;//srcAVCtype.nRefFrames;
					h264type.eProfile = OMX_VIDEO_AVCProfileBaseline; //srcAVCtype.eProfile;
					h264type.eLevel = OMX_VIDEO_AVCLevel3; //srcAVCtype.eLevel;
					//VOLOGI("refframes: %d, profile: %d, level: %d", h264type.nRefFrames, h264type.eProfile, h264type.eLevel);
				}
				errType = m_pComp->SetParameter(OMX_IndexParamVideoAvc, &h264type, size);
				//VOLOGI("errType: 0x%08x", errType);
			}				
		}


		GetDefinition(&def);

#if 1
		// XXX Need a (much) better heuristic to compute input buffer sizes.
		const size_t X = 64 * 1024;
		if (def.nBufferSize < X) {
			def.nBufferSize = X;
		}
#endif
		//VOLOGI("[ port %d video ] w x h = %d x %d, buffer size: %d, stride: %d", m_nIndex, m_nVideoWidth, m_nVideoHeight, def.nBufferSize, vdef->nStride, vdef->nSliceHeight);

		vdef->nFrameWidth = m_nVideoWidth;;
		vdef->nFrameHeight = m_nVideoHeight;
		vdef->eCompressionFormat = compressionFormat;
		vdef->eColorFormat = OMX_COLOR_FormatUnused;
		errType = SetDefinition(&def);
		//VOLOGI("err: 0x%08x", errType);
		if (errType != OMX_ErrorNone)
			return errType;

	} else if (m_nIndex == kPortIndexOutput) {

		OMX_VIDEO_PARAM_PORTFORMATTYPE format;
		InitOMXParams(&format);
		format.nPortIndex = m_nIndex;
		format.nIndex = 0;

		errType = m_pComp->GetParameter(OMX_IndexParamVideoPortFormat, &format, sizeof(format));
		if (errType != OMX_ErrorNone)
			return errType;

		GetDefinition(&def);
		//VOLOGI("port %d, format ecolor: 0x%08x, def ecolor: 0x%08x", m_nIndex, format.eColorFormat, vdef->eColorFormat);
		format.eColorFormat = vdef->eColorFormat;
		format.eCompressionFormat = OMX_VIDEO_CodingUnused;

		errType = m_pComp->SetParameter(OMX_IndexParamVideoPortFormat, &format, sizeof(format));
		if (errType != OMX_ErrorNone) {
			//VOLOGI("err: 0x%08x", errType);
			return errType;
		}

		switch (format.eColorFormat)
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

		//VOLOGI("[ frame ] w x h = %d x %d, buffer size: %d", vdef->nFrameWidth, vdef->nFrameHeight, def.nBufferSize);
		vdef->nFrameWidth = m_nVideoWidth;
		vdef->nFrameHeight = m_nVideoHeight;

		EnableGraphicBuffers();
		SetCrop(&def);
		nwInitNativeWindow(&def);

		SetDefinition(&def);
	}

	return errType;
}


OMX_S32 CTIPort::EnableGraphicBuffers() {

  //VOLOGI("nativewindow: %p", m_pnativewindow.get());
  if (m_pnativewindow.get() == NULL)
	return OMX_ErrorNotReady;

  status_t err = m_pomx->enableGraphicBuffers(m_pnode, kPortIndexOutput, OMX_TRUE);
  //VOLOGI("enable graphic buffer return: 0x%08x", err);
  if (err != 0) 
	return err;

  return err;
}

OMX_S32	CTIPort::ReconnectNativeWindow(OMX_PARAM_PORTDEFINITIONTYPE* pdef) {

	status_t err = nwInitNativeWindow(pdef);
	if (err != OK)
		return err;

	err = SetDefinition(pdef);
	return err;
}

OMX_S32	CTIPort::nwInitNativeWindow(OMX_PARAM_PORTDEFINITIONTYPE *pdef) {

	status_t err = OK;

	/*VOLOGI("[ video ] w x h = %d x %d, [ buffer ] size = %d, count = %d, min: %d, eColorformat: 0x%08x",
			pdef->format.video.nFrameWidth, 
			pdef->format.video.nFrameHeight, 
			pdef->nBufferSize, 
			pdef->nBufferCountActual,
			pdef->nBufferCountMin,
			pdef->format.video.eColorFormat);*/

	err = native_window_set_scaling_mode(m_pnativewindow.get(), NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW);
	if (err != OK)  {
		//VOLOGI("err: 0x%08x", err);
		return err;
	}

	err = native_window_set_buffers_geometry(
			m_pnativewindow.get(),
			pdef->format.video.nFrameWidth,
			pdef->format.video.nFrameHeight,
			pdef->format.video.eColorFormat & 0x100);

	if (err != OK) {
		LOGE("native_window_set_buffers_geometry failed: %s (%d)", strerror(-err), -err);
		return err;
	}

	err = applyRotation();
	if (err != OK)
		return err;

	// Set up the native window.
	OMX_U32 usage = 0;
	err = m_pomx->getGraphicBufferUsage(m_pnode, m_nIndex, &usage);
	if (err != 0) {
		LOGW("querying usage flags from OMX IL component failed: %d", err);
		usage = 0;
	}

	getFlags();

	if (m_nflags & kEnableGrallocUsageProtected) 
		usage |= GRALLOC_USAGE_PROTECTED;


	//VOLOGI("usage: %d", usage);

	// Make sure to check whether either Stagefright or the video decoder requested protected buffers.
	if (usage & GRALLOC_USAGE_PROTECTED) {
		// Verify that the ANativeWindow sends images directly to SurfaceFlinger.
		int queuesToNativeWindow = 0;
		err = m_pnativewindow->query(m_pnativewindow.get(), NATIVE_WINDOW_QUEUES_TO_WINDOW_COMPOSER, &queuesToNativeWindow);
		if (err != 0) {
			LOGE("error authenticating native window: %d", err);
			return err;
		}
		if (queuesToNativeWindow != 1) {
			LOGE("native window could not be authenticated");
			return PERMISSION_DENIED;
		}
	}

	//VOLOGI("native_window_set_usage usage: %d", usage);
	err = native_window_set_usage(m_pnativewindow.get(), usage | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP);
	if (err != 0) {
		LOGE("native_window_set_usage failed: %s (%d)", strerror(-err), -err);
		return err;
	}

	err = m_pnativewindow->query(m_pnativewindow.get(), NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS, (int*)&m_nMinUndequeuedBufs);
	if (err != 0) {
		LOGE("NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS query failed: %s (%d)", strerror(-err), -err);
		return err;
	}

	//VOLOGI("[ buffer count ] actual: %d, min: %d, minundequeued: %d", pdef->nBufferCountActual, pdef->nBufferCountMin, m_nMinUndequeuedBufs);
	if (pdef->nBufferCountActual < pdef->nBufferCountMin + m_nMinUndequeuedBufs) {

		pdef->nBufferCountActual = pdef->nBufferCountMin + m_nMinUndequeuedBufs;
		if (err != OK) {
			LOGE("setting nBufferCountActual to %lu failed: %d", pdef->nBufferCountActual, err);
			return err;
		}
	}

	err = native_window_set_buffer_count(m_pnativewindow.get(), pdef->nBufferCountActual);
	//VOLOGI("--------------------------- set nativewindow buffer count: %d, ret: %d --------------------------------", pdef->nBufferCountActual, err);
	if (err != 0) {
		LOGE("native_window_set_buffer_count failed: %s (%d)", strerror(-err), -err);
		return err;
	}

	//VOLOGI("init native window return: 0x%08x", err);
	return err;
}


OMX_S32 CTIPort::nwAllocBuffers()
{

	//VOLOGI("allocating %lu buffers from a native window on output port", m_nBuffCount);

	status_t err = 0;
	OMX_U32 i = 0;
	for (; i < m_nBuffCount; i++) {

		ANativeWindowBuffer* buf;
		err = m_pnativewindow->dequeueBuffer(m_pnativewindow.get(), &buf);
		if (err != 0) {
			LOGE("dequeueBuffer failed: %s (%d)", strerror(-err), -err);
			break;
		}

		sp<GraphicBuffer> gb(new GraphicBuffer(buf, false));
		err = m_pomx->useGraphicBuffer(m_pnode, m_nIndex, gb, (void**)&m_ppBuffHead[i]);
		if (err != 0) {
			LOGE("registering GraphicBuffer with OMX IL component "
					"failed: %d", err);
			break;
		}

		m_pBuffInfo[i].pBuffHead = m_ppBuffHead[i];
		m_pBuffInfo[i].status = OWNED_BY_US;
		m_pBuffInfo[i].pgb = gb;

		//OMX_BUFFERHEADERTYPE *ph = m_ppBuffHead[i];
		//VOLOGI("graphicbuffer: %p, head: %p", gb.get(), ph);
	}

	OMX_U32 cancelStart;
	OMX_U32 cancelEnd;
	if (err != 0) {
		// If an error occurred while dequeuing we need to cancel any buffers
		// that were dequeued.
		cancelStart = 0;
		cancelEnd = i;
	} else {
		// Return the last two buffers to the native window.
		cancelStart = m_nBuffCount - m_nMinUndequeuedBufs;
		cancelEnd = m_nBuffCount;
	}

	for (OMX_U32 j = cancelStart; j < cancelEnd; j++) {
		if (m_pBuffInfo[j].pBuffHead == m_ppBuffHead[j]) {
			//VOLOGI("cancel buffer %d  %p to native window", j, m_ppBuffHead[j]);
			nwCancelBuffer(m_ppBuffHead[j]);
		}
	}

	return err;
}


OMX_S32 CTIPort::nwCancelBuffer(OMX_PTR pb) {

	OMX_S32 err = 0;
	for (OMX_U32 i = 0; i < m_nBuffCount; ++i) {

		if (m_pBuffInfo[i].pBuffHead != pb)
			continue;

		err = m_pnativewindow->cancelBuffer(m_pnativewindow.get(), m_pBuffInfo[i].pgb.get());
		if (err != 0) {
			LOGE("cancelBuffer failed w/ error 0x%08x", (unsigned int)err);

			//setState(ERROR);
			return err;
		}

		m_pBuffInfo[i].status = OWNED_BY_NATIVE_WINDOW;
		break;
	}

	return err;
}

OMX_S32 CTIPort::nwQueueBuffer(OMX_PTR pb)
{
	OMX_S32 err = 0;
	for (OMX_U32 i = 0; i < m_nBuffCount; ++i) {

		if (m_pBuffInfo[i].pBuffHead != pb)
			continue;

		err = m_pnativewindow->queueBuffer(m_pnativewindow.get(), m_pBuffInfo[i].pgb.get());
		if (err != 0) {
			LOGE("cancelBuffer failed w/ error 0x%08x", (unsigned int)err);

			//setState(ERROR);
			return err;
		}

		m_pBuffInfo[i].status = OWNED_BY_NATIVE_WINDOW;
		break;
	}

	return err;
}

OMX_BUFFERHEADERTYPE* CTIPort::nwPrepareBuffer()
{
	OMX_BUFFERHEADERTYPE* pbuf = nwDequeueBuffer();
	if (nwLockBuffer(pbuf) == 0)
		return pbuf;

	return NULL;
}

OMX_S32 CTIPort::nwLockBuffer(OMX_PTR pb)
{
	OMX_S32 err = 0;
	for (OMX_U32 i = 0; i < m_nBuffCount; ++i) {

		if (m_pBuffInfo[i].pBuffHead != pb)
			continue;

		err = m_pnativewindow->lockBuffer(m_pnativewindow.get(), m_pBuffInfo[i].pgb.get());
		if (err != 0) {
			LOGE("lockBuffer failed w/ error 0x%08x", (unsigned int)err);

			//setState(ERROR);
			return err;
		}
		break;
	}

	return err;
}


OMX_S32 CTIPort::nwFreeBuffer(OMX_PTR pb)
{
	return nwCancelBuffer(pb);
}


OMX_BUFFERHEADERTYPE* CTIPort::nwDequeueBuffer() {

	ANativeWindowBuffer* buf;
	int err = m_pnativewindow->dequeueBuffer(m_pnativewindow.get(), &buf);
	if (err != 0) {
		LOGE("dequeueBuffer failed w/ error 0x%08x", err);

		//setState(ERROR);
		return NULL;
	}

	// Determine which buffer we just dequeued.
	VOBUFFERINFO* pbi = 0;
	OMX_U32 i = 0;
	for (i = 0; i < m_nBuffCount; i++) {
		sp<GraphicBuffer> gb = m_pBuffInfo[i].pgb;
		if (gb->handle == buf->handle) {
			pbi = &m_pBuffInfo[i];
			break;
		}
	}

	if (pbi == 0) {
		LOGE("dequeued unrecognized buffer: %p", buf);

		//setState(ERROR);
		return 0;
	}

	if (pbi->status == OWNED_BY_NATIVE_WINDOW) 
		pbi->status = OWNED_BY_US;

	return m_ppBuffHead[i];
}

int CTIPort::mycallback(int nID, void * pParam1, void * pParam2) {

	VOMP_LISTENERINFO* pli =((CTIComp*)m_pComp)->m_pListener;
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

	if (m_ppBuffHead == NULL || m_portType.eDir != OMX_DirOutput)
		return OMX_ErrorNotImplemented;

	
	voCOMXAutoLock lock(&m_muxBuffer);

	OMX_BUFFERHEADERTYPE* pbuf = NULL;
	CBufferItem *	pItem = m_lstFree.RemoveBufferItem();
	while (pItem != NULL)
	{
		pbuf = pItem->m_pBufferHead;
		if (pbuf != NULL)
			nwCancelBuffer(pbuf);

		pItem = m_lstFree.RemoveBufferItem();
	}
	m_lstFree.ResetItems(); 

	pItem = m_lstFull.RemoveBufferItem();
	while (pItem != NULL)
	{
		pbuf = pItem->m_pBufferHead;
		if (pbuf != NULL)
			nwCancelBuffer(pbuf);

		pItem = m_lstFull.RemoveBufferItem();
	}
	m_lstFull.ResetItems(); 

	OMX_U32 cancelEnd = m_nBuffCount - m_nMinUndequeuedBufs;
	for (OMX_U32 i = 0; i < cancelEnd; i++) {
		pbuf = nwDequeueBuffer();
		//VOLOGI("buffer[%d] : %p dequeued", i, pbuf);
		m_lstFree.AddBufferHead(pbuf);
	}

	return errType;
}

status_t CTIPort::applyRotation() {

	int32_t rotationDegrees = 0;
	/*sp<MetaData> meta = mSource->getFormat();

	  if (!meta->findInt32(kKeyRotation, &rotationDegrees)) {
	  rotationDegrees = 0;
	  }*/

	uint32_t transform;
	switch (rotationDegrees) {
		case 0: transform = 0; break;
		case 90: transform = HAL_TRANSFORM_ROT_90; break;
		case 180: transform = HAL_TRANSFORM_ROT_180; break;
		case 270: transform = HAL_TRANSFORM_ROT_270; break;
		default: transform = 0; break;
	}

	status_t err = OK;

	if (transform) {
		err = native_window_set_buffers_transform(m_pnativewindow.get(), transform);
	}

	return err;
}

void CTIPort::SetCrop(OMX_PARAM_PORTDEFINITIONTYPE* pPortDef) {

	OMX_CONFIG_RECTTYPE rect;
	InitOMXParams(&rect);
	rect.nPortIndex = kPortIndexOutput;
	status_t err = (status_t)m_pComp->GetConfig(OMX_IndexConfigCommonOutputCrop, &rect, sizeof(rect));

	if (err == OK) {
		m_diminfo.nTop = rect.nTop;
		m_diminfo.nLeft = rect.nLeft;
		m_diminfo.nWidth = rect.nWidth;
		m_diminfo.nHeight = rect.nHeight;

		//VOLOGI("Crop rect is %ld x %ld @ (%ld, %ld)", rect.nWidth, rect.nHeight, rect.nLeft, rect.nTop);
	} else {
		m_diminfo.nTop = 0;
		m_diminfo.nLeft = 0;
		m_diminfo.nWidth = m_nVideoWidth - 1;
		m_diminfo.nHeight = m_nVideoHeight - 1;
		//VOLOGI("dimension rect is %ld x %ld @ (%ld, %ld)", rect.nWidth, rect.nHeight, rect.nLeft, rect.nTop);
	}

	if (m_pnativewindow != NULL) 
		nwSetCrop();
}

OMX_S32 CTIPort::nwSetCrop() {

	android_native_rect_t crop;
	crop.left = m_diminfo.nLeft;
	crop.top = m_diminfo.nTop;
	crop.right = m_diminfo.nLeft + m_diminfo.nWidth + 1;
	crop.bottom = m_diminfo.nTop + m_diminfo.nHeight + 1;
	/*VOLOGI("[ video ] w x h = %d x %d, [ pad ] w x h = %d x %d, [ crop ] w x h  = %d x %d @ (%d, %d)", m_nVideoWidth, m_nVideoHeight, m_nPadWidth, m_nPadHeight,
	crop.right - crop.left, crop.bottom - crop.top, crop.left, crop.top);*/
	native_window_set_crop(m_pnativewindow.get(), &crop);

	return 0;
}

status_t CTIPort::setVideoPortFormatType(OMX_U32 portIndex, OMX_VIDEO_CODINGTYPE compressionFormat, OMX_COLOR_FORMATTYPE colorFormat) {

	OMX_VIDEO_PARAM_PORTFORMATTYPE format;
	InitOMXParams(&format);
	format.nPortIndex = portIndex;
	bool found = false;

	status_t err = 0;
	OMX_U32 index = 0;
	for (;;) {

		format.nIndex = index;
		//VOLOGI("port index: %d", index);
		err = m_pComp->GetParameter(OMX_IndexParamVideoPortFormat, &format, sizeof(format));

			//VOLOGI("err: 0x%08x", err);
		if (err != OK) {
			//VOLOGI("err: 0x%08x", err);
			return err;
		}

		//VOLOGI("port: %ld, index: %ld, eCompressionFormat=%d eColorFormat=%d", portIndex, index, format.eCompressionFormat, format.eColorFormat);

		if (format.eCompressionFormat == compressionFormat && format.eColorFormat == colorFormat) {
			found = true;
			break;
		}

		++index;
		if (index >= 1000) {
			LOGE("color format %d or compression format %d is not supported", colorFormat, compressionFormat);
			return UNKNOWN_ERROR;
		}
	}

	if (!found) {
		return UNKNOWN_ERROR;
	}

	//VOLOGI("found a match.");
	err = m_pComp->SetParameter(OMX_IndexParamVideoPortFormat, &format, sizeof(format));

	return err;
}

void CTIPort::getFlags() {

#ifdef DEBUG_HDCP
	// For debugging, we allow a system property to control the protected usage.
	// In case of uninitialized or unexpected property, we default to "DRM only".
	bool setProtectionBit = false;
	char value[PROPERTY_VALUE_MAX];
	if (property_get("persist.sys.hdcp_checking", value, NULL)) {
		if (!strcmp(value, "never")) {
			// nop
		} else if (!strcmp(value, "always")) {
			setProtectionBit = true;
		} else if (!strcmp(value, "drm-only")) {
			if (m_pdrmengine != NULL) {
				setProtectionBit = true;
			}
			// property value is empty, or unexpected value
		} else {
			if (m_pdrmengine != NULL) {
				setProtectionBit = true;
			}
		}
		// can' read property value
	} else {
		if (m_pdrmengine != NULL) {
			setProtectionBit = true;
		}
	}
	// note that usage bit is already cleared, so no need to clear it in the "else" case
	if (setProtectionBit) {
		m_nflags |= kEnableGrallocUsageProtected;
	}
#else
	if (m_pdrmengine != NULL) {
		m_nflags |= kEnableGrallocUsageProtected;
	}
#endif

	//VOLOGI("m_nflags: %d", m_nflags);
}

int CTIPort::findInputBuffer(OMX_BUFFERHEADERTYPE* ph) {

	unsigned int i = 0;
	for ( ; i < m_nBuffCount; ++i) 
		if (m_pBuffInfo[i].pBuffHead == ph)
			break;

	return i;
}

