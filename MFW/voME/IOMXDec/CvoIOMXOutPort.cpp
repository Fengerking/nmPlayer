	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoIOMXOutPort.cpp

	Contains:	Android IOMX component output port source file

	Written by:	East Zhou

	Change History (most recent first):
	2012-04-19	East		Create file

*******************************************************************************/
#include "voIOMXDec.h"
#include "CvoIOMXComponent.h"
#include "voOMXOSFun.h"
#include "CvoIOMXOutPort.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "CvoIOMXOutPort"
#endif

#include "voLog.h"

#ifdef _GB
#define OMX_TI_IndexParam2DBufferAllocDimension		((OMX_INDEXTYPE)(OMX_IndexVendorStartUnused + 87))
#endif	// _GB

static const int QOMX_COLOR_FormatYVU420SemiPlanar = 0x7FA30C00;
static const int QOMX_COLOR_FormatYVU420PackedSemiPlanar32m4ka = 0x7FA30C01;
static const int QOMX_COLOR_FormatYUV420PackedSemiPlanar64x32Tile2m8ka = 0x7FA30C03;

CvoIOMXOutPort::CvoIOMXOutPort(CvoIOMXComponent * pComponent, OMX_U32 nPortIndex)
	: CvoIOMXPort(pComponent, nPortIndex)
#ifdef _ICS
	, m_nMinUndequeuedBuffers(1)
#endif	// _ICS
{
}

CvoIOMXOutPort::~CvoIOMXOutPort()
{
}

OMX_ERRORTYPE CvoIOMXOutPort::SetPortSettings(OMX_U32 nWidth, OMX_U32 nHeight)
{
	VOLOGI("Port %d, Width %d, Height %d", m_nIndex, nWidth, nHeight);

	OMX_ERRORTYPE errType = UpdatePortFormat();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to UpdatePortFormat 0x%08X", errType);
		return errType;
	}

// 	if(COMP_SAMSUNG == m_pComponent->m_nType)
// 	{
// 		// for Samsung chipset, OMX_COLOR_FormatYUV420Planar for invalid native window, OMX_COLOR_FormatYUV420SemiPlanar for valid native window, East 20130110
// 		// 1478 line of http://git.insignal.co.kr/samsung/exynos/android/platform/frameworks/base/tree/media/libstagefright/OMXCodec.cpp?h=exynos-ics
// 		m_sPortFormat.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
// 	}

	// we need call it, some devices (like Galaxy Note 10.1) need it for OMX_PARAM_PORTDEFINITIONTYPE::format.video.eColorFormat, East 20121230
	errType = m_pComponent->SetParameter(OMX_IndexParamVideoPortFormat, &m_sPortFormat, sizeof(m_sPortFormat));
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_IndexParamVideoPortFormat 0x%08X", errType);
		return errType;
	}

	errType = UpdatePortDefinition();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to UpdatePortDefinition 0x%08X", errType);
		return errType;
	}

	VOLOGI("output port buffer min count %d, count %d, buffer size %d, width %d, height %d, stride %d, slice height %d, color %d", 
		m_sPortDefinition.nBufferCountMin, m_sPortDefinition.nBufferCountActual, m_sPortDefinition.nBufferSize, 
		m_sPortDefinition.format.video.nFrameWidth, m_sPortDefinition.format.video.nFrameHeight, m_sPortDefinition.format.video.nStride, m_sPortDefinition.format.video.nSliceHeight, 
		m_sPortDefinition.format.video.eColorFormat);

	m_sPortDefinition.format.video.nFrameWidth = nWidth;
	m_sPortDefinition.format.video.nFrameHeight = nHeight;

#if defined (_GB) || defined (_FRY)
	// TI OMAP need this workaround for froyo and Gingerbread, ICS no need, East 20130122
	// #define NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE    2	(OMAP4)
	// #define NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE    3	(OMAP3)
	// def.nBufferCountActual = def.nBufferCountMin + (2 * NUM_BUFFERS_TO_BE_QUEUED_FOR_OPTIMAL_PERFORMANCE);
	if(COMP_TI_OMAP4 == m_pComponent->m_nType)
		m_sPortDefinition.nBufferCountActual = m_sPortDefinition.nBufferCountMin + 2 * 2;
	else if(COMP_TI_OMAP3 == m_pComponent->m_nType)
		m_sPortDefinition.nBufferCountActual = m_sPortDefinition.nBufferCountMin + 2 * 3;
#endif	// _GB, _FRY

	errType = m_pComponent->SetParameter(OMX_IndexParamPortDefinition, &m_sPortDefinition, sizeof(m_sPortDefinition));
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_IndexParamPortDefinition 0x%08X", errType);
		return errType;
	}

#ifdef _ICS

#ifdef _KK
	//	For Android 4.4, we need call PrepareForAdaptivePlayback before enable graphic buffer.
	//	1280 * 720 comes from Android source code.
	//	Some devices have no such feature enabled and return Error, but we should ignore return value.
	errType = m_pComponent->PrepareForAdaptivePlayback(m_nIndex, OMX_TRUE, 1280, 720);
	if(OMX_ErrorNone != errType)
	{
		VOLOGI("failed to PrepareForAdaptivePlayback 0x%08X", errType);
	}
#endif // _KK
	// follow StageFright strictly, East 20130111
	// Enable use of a GraphicBuffer as the output for this node.
	// This must happen before getting the IndexParamPortDefinition parameter because it will affect the pixel format that the node reports.
	errType = m_pComponent->EnableGraphicBuffers(m_nIndex, OMX_TRUE);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to EnableGraphicBuffers 0x%08X", errType);
		return errType;
	}
#endif	// _ICS

	// get final port definition
	errType = UpdatePortDefinition();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to UpdatePortDefinition 0x%08X", errType);
		return errType;
	}

	VOLOGI("output port buffer count %d, buffer size %d, width %d, height %d, stride %d, slice height %d, color %d", 
		m_sPortDefinition.nBufferCountActual, m_sPortDefinition.nBufferSize, 
		m_sPortDefinition.format.video.nFrameWidth, m_sPortDefinition.format.video.nFrameHeight, m_sPortDefinition.format.video.nStride, m_sPortDefinition.format.video.nSliceHeight, 
		m_sPortDefinition.format.video.eColorFormat);

	return errType;
}

OMX_ERRORTYPE CvoIOMXOutPort::AllocateBufferHandle()
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

#ifdef _ICS
	ANativeWindow * pNativeWindow = m_pComponent->m_piNativeWindow.get();
	if(!pNativeWindow)
	{
		VOLOGE("native window is NULL!");
		return OMX_ErrorNotReady;
	}

	status_t err = native_window_set_scaling_mode(pNativeWindow, NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW);
	if(OK != err) 
	{
		VOLOGE("failed to native_window_set_scaling_mode %s (%d)", strerror(-err), -err);
		return OMX_ErrorUndefined;
	}

	// differnt platform will use color format specially, note here!!
	OMX_COLOR_FORMATTYPE eColor = m_sPortDefinition.format.video.eColorFormat;
	if(COMP_QCOM == m_pComponent->m_nType)	// for QCOM chipset
	{
		if(QOMX_COLOR_FormatYUV420PackedSemiPlanar64x32Tile2m8ka == eColor)
		{
			// JB need use QOMX_COLOR_FormatYUV420PackedSemiPlanar64x32Tile2m8ka directly
			// So just ICS need convert color format, East 20130128
			if(m_pComponent->m_nVersionNumber[0] == 4 && m_pComponent->m_nVersionNumber[1] == 0)
				eColor = OMX_COLOR_FORMATTYPE(0x108);	// "\spec\Hardware_Integration\ICS\gralloc_priv.htm" HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED     = 0x108
		}
	}
	else if(COMP_SAMSUNG == m_pComponent->m_nType)	// for Samsung chipset
	{
		/* Samsung define different HAL_PIXEL_FORMAT_XXX, get following information from exynos_format.h, East 20121230
		HAL_PIXEL_FORMAT_YCbCr_422_P         = 0x100,
		HAL_PIXEL_FORMAT_YCbCr_420_P         = 0x101,
		HAL_PIXEL_FORMAT_YCbCr_420_I         = 0x102,
		HAL_PIXEL_FORMAT_CbYCrY_422_I        = 0x103,
		HAL_PIXEL_FORMAT_CbYCrY_420_I        = 0x104,
		HAL_PIXEL_FORMAT_YCbCr_420_SP        = 0x105,
		HAL_PIXEL_FORMAT_YCrCb_422_SP        = 0x106,
		HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED  = 0x107,
		*/
		if(OMX_COLOR_FormatYUV420Planar == eColor)	// Galaxy S II
			eColor = OMX_COLOR_FORMATTYPE(0x101);	// HAL_PIXEL_FORMAT_YCbCr_420_P
		else if(OMX_COLOR_FormatYUV420SemiPlanar == eColor)	// Galaxy Notes II
			eColor = OMX_COLOR_FORMATTYPE(0x105);	// HAL_PIXEL_FORMAT_YCbCr_420_SP
	}

	VOLOGI("native window buffer geometry width %d, height %d, color %d", m_sPortDefinition.format.video.nFrameWidth, m_sPortDefinition.format.video.nFrameHeight, eColor);
	err = native_window_set_buffers_geometry(pNativeWindow, m_sPortDefinition.format.video.nFrameWidth, m_sPortDefinition.format.video.nFrameHeight, eColor);
	if(OK != err)
	{
		VOLOGE("failed to native_window_set_buffers_geometry %s (%d)", strerror(-err), -err);
		return OMX_ErrorUndefined;
	}

	// for Samsung chipset, update crop here to avoid mosaic at border, East 20130122
	// at least Galaxy S III won't notify OMX_IndexConfigCommonOutputCrop event
	if(COMP_SAMSUNG == m_pComponent->m_nType)
		m_pComponent->UpdateCrop();

	OMX_U32 nUsage = 0;
	errType = m_pComponent->GetGraphicBufferUsage(m_nIndex, &nUsage);
	if(OMX_ErrorNone != errType)
		nUsage = 0;

	VOLOGI("graphic buffer usage from component: 0x%08X", nUsage);
	err = native_window_set_usage(pNativeWindow, nUsage | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP);
	if(OK != err)
	{
		VOLOGE("failed to native_window_set_usage %s (%d)", strerror(-err), -err);
		return OMX_ErrorUndefined;
	}

	err = m_pComponent->m_piNativeWindow->query(pNativeWindow, NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS, (int*)&m_nMinUndequeuedBuffers);
	if(OK != err)
	{
		VOLOGW("failed to get NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS %s (%d)", strerror(-err), -err);
		m_nMinUndequeuedBuffers = 1;
	}

	if(m_sPortDefinition.nBufferCountActual < m_sPortDefinition.nBufferCountMin + m_nMinUndequeuedBuffers)
	{
		VOLOGW("m_sPortDefinition.nBufferCountActual(%d) < m_sPortDefinition.nBufferCountMin(%d) + m_nMinUndequeuedBuffers(%d)", 
			m_sPortDefinition.nBufferCountActual, m_sPortDefinition.nBufferCountMin, m_nMinUndequeuedBuffers);

		m_sPortDefinition.nBufferCountActual = m_sPortDefinition.nBufferCountMin + m_nMinUndequeuedBuffers;

		errType = m_pComponent->SetParameter(OMX_IndexParamPortDefinition, &m_sPortDefinition, sizeof(m_sPortDefinition));
		if(OMX_ErrorNone != errType)
		{
			VOLOGW("failed to set OMX_IndexParamPortDefinition 0x%08X", errType);
//			return errType;
		}

		errType = UpdatePortDefinition();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to get OMX_IndexParamPortDefinition 0x%08X", errType);
			return errType;
		}
	}

	err = native_window_set_buffer_count(pNativeWindow, m_sPortDefinition.nBufferCountActual);
	if(OK != err)
	{
		VOLOGE("failed to native_window_set_buffer_count %d %s (%d)", m_sPortDefinition.nBufferCountActual, strerror(-err), -err);
		return OMX_ErrorUndefined;
	}

	if(COMP_QCOM == m_pComponent->m_nType)
	{
		VOLOGR("ICS QCOM need perform NATIVE_WINDOW_SET_BUFFERS_SIZE!");

		if(m_pComponent->m_nVersionNumber[0] == 4 && m_pComponent->m_nVersionNumber[1] == 0)
		{
			/*
			Qcom specific Native Window perform operations, East 20130104
			/platform/hardware/qcom/display/libqdutils/qcom_ui.h
			enum {
				NATIVE_WINDOW_SET_BUFFERS_SIZE        = 0x10000000, 
				NATIVE_WINDOW_UPDATE_BUFFERS_GEOMETRY = 0x20000000, 
				NATIVE_WINDOW_SET_S3D_FORMAT          = 0x40000000, 
				NATIVE_WINDOW_SET_PIXEL_ASPECT_RATIO  = 0x80000000, 
			};
			*/

			err = pNativeWindow->perform(pNativeWindow, 0x10000000, m_sPortDefinition.nBufferSize);
		}
		else
		{
			/* from QCOM 8960 4.1.1 /window.h
			enum {
				NATIVE_WINDOW_SET_USAGE                 =  0,
				NATIVE_WINDOW_CONNECT                   =  1,
				NATIVE_WINDOW_DISCONNECT                =  2,
				NATIVE_WINDOW_SET_CROP                  =  3,
				NATIVE_WINDOW_SET_BUFFER_COUNT          =  4,
				NATIVE_WINDOW_SET_BUFFERS_GEOMETRY      =  5,
				NATIVE_WINDOW_SET_BUFFERS_TRANSFORM     =  6,
				NATIVE_WINDOW_SET_BUFFERS_TIMESTAMP     =  7,
				NATIVE_WINDOW_SET_BUFFERS_DIMENSIONS    =  8,
				NATIVE_WINDOW_SET_BUFFERS_FORMAT        =  9,
				NATIVE_WINDOW_SET_SCALING_MODE          = 10,
				NATIVE_WINDOW_LOCK                      = 11,
				NATIVE_WINDOW_UNLOCK_AND_POST           = 12,
				NATIVE_WINDOW_API_CONNECT               = 13,
				NATIVE_WINDOW_API_DISCONNECT            = 14,
				NATIVE_WINDOW_SET_BUFFERS_USER_DIMENSIONS = 15,
				NATIVE_WINDOW_SET_POST_TRANSFORM_CROP   = 16,
				NATIVE_WINDOW_SET_BUFFERS_SIZE          = 17,
				NATIVE_WINDOW_UPDATE_BUFFERS_GEOMETRY   = 18,
			};
			4.2 remove NATIVE_WINDOW_SET_BUFFERS_SIZE and NATIVE_WINDOW_UPDATE_BUFFERS_GEOMETRY, so 4.2 QCOM (Nexus 4) can't support interlaced video
			*/

			err = pNativeWindow->perform(pNativeWindow, 17, m_sPortDefinition.nBufferSize);
		}

		if(OK != err)
		{
			// new QCOM platform no need call it, so we need warning and not return error, East 20130122
			VOLOGW("failed to perform NATIVE_WINDOW_SET_BUFFERS_SIZE %d %s (%d)", m_sPortDefinition.nBufferSize, strerror(-err), -err);
		}
	}
#endif	// _ICS

	m_nBufferCount = m_sPortDefinition.nBufferCountActual;

#ifdef OMAP_ENHANCEMENT
	if(m_pComponent->m_piOMXRenderer.get() == NULL)
		m_pComponent->CreateVideoRenderer(m_nBufferCount);

	Vector< sp<IMemory> > piMemorys = m_pComponent->GetBuffers(m_pComponent->m_piOMXRenderer);
#endif	// OMAP_ENHANCEMENT

	m_pBuffers = new voIOMXPortBuffer[m_nBufferCount];
	if(!m_pBuffers)
	{
		VOLOGE("failed to allocate buffer pointer!");
		return OMX_ErrorInsufficientResources;
	}
	memset(m_pBuffers, 0, m_nBufferCount * sizeof(voIOMXPortBuffer));

	if(OMX_FALSE == m_listBufferEmpty.Create(m_nBufferCount))
	{
		VOLOGE("failed to create empty buffer list!");
		return OMX_ErrorInsufficientResources;
	}

	for(OMX_U32 i = 0; i < m_nBufferCount; i++)
	{
		IOMX::buffer_id hBuffer = NULL;
#ifdef _ICS
		ANativeWindowBuffer * pNativeWindowBuffer;
		// from 4.2, Android support native_window_dequeue_buffer_and_wait, but it seems old API can work fine, East 20130116
#ifdef _JB43
		err = native_window_dequeue_buffer_and_wait(pNativeWindow, &pNativeWindowBuffer);
#else
		err = m_pComponent->m_piNativeWindow->dequeueBuffer(pNativeWindow, &pNativeWindowBuffer);
#endif
		if(OK != err)
		{
			VOLOGE("failed to dequeueBuffer %d %s (%d)", i, strerror(-err), -err);
			break;
		}

		sp<GraphicBuffer> piGraphicBuffer(new GraphicBuffer(pNativeWindowBuffer, false));
		errType = m_pComponent->UseGraphicBuffer(m_nIndex, piGraphicBuffer, &hBuffer);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to UseGraphicBuffer 0x%08X", errType);
			m_bGraphicBufferAvaible = OMX_FALSE;
			break;
		}

		m_pBuffers[i].piGraphicBuffer = piGraphicBuffer;
		m_pBuffers[i].bDequeued = OMX_TRUE;
#else	// _ICS
#ifdef OMAP_ENHANCEMENT
		errType = m_pComponent->UseBuffer(m_nIndex, piMemorys[i], &hBuffer, m_sPortDefinition.nBufferSize);
#else	// OMAP_ENHANCEMENT
		void * pBuffer = NULL;
		errType = m_pComponent->AllocateBuffer(m_nIndex, m_sPortDefinition.nBufferSize, &hBuffer, &pBuffer);
#endif	// OMAP_ENHANCEMENT
#endif	// _ICS
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to AllocateBuffer 0x%08X", errType);
			return errType;
		}

		m_pBuffers[i].hBuffer = hBuffer;
		m_pBuffers[i].bFilled = OMX_FALSE;
		m_listBufferEmpty.Add(m_pBuffers + i);

#ifdef _ICS
		VOLOGR("AllocateBuffer 0x%08X, graphic buffer 0x%08X, 0x%08X", hBuffer, piGraphicBuffer.get(), errType);
#else	// _ICS
		VOLOGR("AllocateBuffer 0x%08X, 0x%08X", hBuffer, errType);
#endif	// _ICS
	}

	return errType;
}

OMX_ERRORTYPE CvoIOMXOutPort::FillBuffer(IOMX::buffer_id hBuffer, OMX_BOOL bCancel /* = OMX_FALSE */)
{
	voIOMXPortBuffer * pFillBuffer = NULL;
#ifndef _ICS
	OMX_U32	nTryTimes = 0;
	while(nTryTimes < 500)
	{
		{
			voCOMXAutoLock lock(&m_tmBuffer);
			if(COMP_QCOM == m_pComponent->m_nType && OMX_FALSE == bCancel)
				pFillBuffer = NULL;
			else
				pFillBuffer = m_listBufferEmpty.GetByBuffer(hBuffer);

			if(NULL == pFillBuffer)
				pFillBuffer = m_listBufferEmpty.GetHead();
		}

		if(NULL != pFillBuffer)
			break;

		voOMXOS_Sleep(2);
		nTryTimes++;

		if(OMX_FALSE == CanBuffersWork())
			return OMX_ErrorNone;
	}
#endif	// _ICS

	if(OMX_FALSE == CanBuffersWork())
		return OMX_ErrorNone;

	voCOMXAutoLock lock(&m_tmBuffer);
#ifdef _ICS
	pFillBuffer = m_pComponent->NativeWindow_PrepareBuffer(m_pBuffers, m_nBufferCount);
#endif	// _ICS
	if(NULL != pFillBuffer)
	{
		VOLOGR("FillBuffer 0x%08X", pFillBuffer->hBuffer);

		m_listBufferEmpty.Remove(pFillBuffer);
		OMX_ERRORTYPE errType = m_pComponent->FillBuffer(pFillBuffer->hBuffer);
		if(OMX_ErrorNone != errType)
		{
			VOLOGW("failed to FillBuffer 0x%08X", errType);
			m_listBufferEmpty.Add(pFillBuffer);
		}
	}
	else
	{
		VOLOGW("failed to get valid buffer");
		return OMX_ErrorTimeout;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CvoIOMXOutPort::StartBuffer()
{
	voCOMXAutoLock lock(&m_tmBuffer);

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_U32 i = 0;
#ifdef _ICS
	// native window will show last frame when power on/off
	// for showing frame, if we not call queueBuffer, call lockBuffer to it will cause hang
	// showing frame always at the last of buffer list, so we will use it after queueBuffer
	// East guess and debug 20110903
	for(i = 0; i < m_nBufferCount; i++)
		m_pComponent->NativeWindow_CancelBuffer(&m_pBuffers[m_nBufferCount - i - 1]);
#endif	// _ICS

	if(OMX_TRUE == m_bGraphicBufferAvaible)
	{
		for(i = 0; i < m_nBufferCount; i++)
		{
			if(PORT_TRANS_Ena2Dis == m_eTransStatus)
				break;

#ifdef _ICS
			OMX_S32 nMinUndequeuedBuffers = m_nMinUndequeuedBuffers;
			if(nMinUndequeuedBuffers < 2 && COMP_SAMSUNG != m_pComponent->m_nType)
				nMinUndequeuedBuffers = 2;

			if(i >= m_nBufferCount - nMinUndequeuedBuffers)
				break;

			voIOMXPortBuffer * pBuffer = m_pComponent->NativeWindow_PrepareBuffer(m_pBuffers, m_nBufferCount);
#else	// _ICS
			if(COMP_QCOM == m_pComponent->m_nType)
			{
				// QCOM need keep 3 buffers
				if(i >= m_nBufferCount - 3)
				{
					VOLOGR("OMX.qcom.video. %d %d", i, m_nBufferCount);
						break;
				}
			}

			voIOMXPortBuffer * pBuffer = m_pBuffers + i;
#endif	// _ICS

			if(NULL == pBuffer)
			{
				continue;
			}

			VOLOGR("FillBuffer 0x%08X", pBuffer->hBuffer);
			errType = m_pComponent->FillBuffer(pBuffer->hBuffer);
			if(OMX_ErrorNone != errType)
			{
				VOLOGE("failed to FillBuffer 0x%08X", errType);
				return errType;
			}

			m_listBufferEmpty.Remove(pBuffer);
		}
	}

	return errType;
}

void CvoIOMXOutPort::GetDecodeResolution(OMX_U32 * pnWidth, OMX_U32 * pnHeight)
{
#ifdef _GB
	if(COMP_TI_OMAP4 == m_pComponent->m_nType)
	{
		OMX_CONFIG_RECTTYPE sRect;
		m_pComponent->SetHeader(&sRect, sizeof(sRect));
		sRect.nPortIndex = m_nIndex;
		OMX_ERRORTYPE errType = m_pComponent->GetParameter(OMX_TI_IndexParam2DBufferAllocDimension, &sRect, sizeof(sRect));
		if(OMX_ErrorNone == errType)
		{
			if(pnWidth)
				*pnWidth = sRect.nWidth;
			if(pnHeight)
				*pnHeight = sRect.nHeight;

			return;
		}
	}
#endif	// _GB

	voCOMXAutoLock lock(&m_tmPortDefinition);

	if(pnWidth)
		*pnWidth = m_sPortDefinition.format.video.nStride;
	if(pnHeight)
		*pnHeight = m_sPortDefinition.format.video.nSliceHeight;
}

OMX_ERRORTYPE CvoIOMXOutPort::GetFilledBuffer(voIOMXPortBuffer ** ppBuffer)
{
	voCOMXAutoLock lock(&m_tmBuffer);
	for(OMX_U32 i = 0; i < m_listBufferEmpty.Count(); i++)
	{
		voIOMXPortBuffer * pTemp = m_listBufferEmpty.GetByIndex(i);
		if(pTemp->bFilled == OMX_TRUE)
		{
			pTemp->bFilled = OMX_FALSE;
			*ppBuffer = pTemp;
			return OMX_ErrorNone;
		}
	}

	VOLOGR("failed to get filled buffer");
	return (OMX_ERRORTYPE)VO_ERR_IOMXDEC_NeedRetry;
}

void CvoIOMXOutPort::SetFlushing(OMX_BOOL bFlushing)
{
	CvoIOMXPort::SetFlushing(bFlushing);

	if(!bFlushing)
	{
		for(OMX_U32 i = 0; i < m_nBufferCount; i++)
			m_pBuffers[i].bFilled = OMX_FALSE;
	}
}

void CvoIOMXOutPort::OnFillBufferDone(IOMX::buffer_id hBuffer, OMX_U32 nOffset, OMX_U32 nLength, OMX_U32 nFlags, OMX_TICKS nTimeStamp, OMX_PTR pPlatformPrivate, OMX_PTR pData)
{
	VOLOGR("FillBufferDone 0x%08X flags 0x%08X, time %d", hBuffer, nFlags, (int)(nTimeStamp / 1000));

	// to find associated wrap buffer
	voIOMXPortBuffer * pBuffer = NULL;
	for(OMX_U32 i = 0; i < m_nBufferCount; i++)
	{
		if(m_pBuffers[i].hBuffer == hBuffer)
		{
			pBuffer = m_pBuffers + i;
			break;
		}
	}

	if(NULL == pBuffer)
	{
		VOLOGW("failed to find this buffer 0x%08X", hBuffer);
		return;
	}

	// we need ignore EOS buffer from output port, East 20130118
	if(OMX_FALSE == m_pComponent->m_bPortSettingsChanging && OMX_TRUE == CanBuffersWork() && OMX_FALSE == m_pComponent->m_bOMXError && !(nFlags & OMX_BUFFERFLAG_EOS))
	{
		pBuffer->bFilled = OMX_TRUE;
		pBuffer->nOffset = nOffset;
		pBuffer->nLength = nLength;
		pBuffer->nFlags = nFlags;
		pBuffer->nTimeStamp = nTimeStamp / 1000;
		pBuffer->pPlatformPrivate = pPlatformPrivate;
		pBuffer->pData = pData;
	}
	else
	{
		VOLOGI("m_bPortSettingsChanging %d, m_pComponent->m_bOMXError %d", (int)m_pComponent->m_bPortSettingsChanging, (int)m_pComponent->m_bOMXError);
		pBuffer->bFilled = OMX_FALSE;
	}

	voCOMXAutoLock lock(&m_tmBuffer);
	m_listBufferEmpty.Add(pBuffer);

#ifdef _ICS
	// we need use m_tmBuffer to lock this operation, otherwise it will make m_listBufferEmpty mess up, East 20120104
	if(OMX_FALSE == pBuffer->bFilled)
		m_pComponent->NativeWindow_CancelBuffer(pBuffer);
#endif	// _ICS
}
