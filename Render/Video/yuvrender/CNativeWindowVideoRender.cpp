
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
	File:		CSurfaceVideoRender.cpp

	Contains:	CSurfaceVideoRender class file

	Written by:	Tom Yu Wei

	Change History (most recent first):
	2010-09-25		Bang			Create file

*******************************************************************************/
#include <utils/Log.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <ui/Rect.h>
#include "CNativeWindowVideoRender.h"

#ifndef LOG_TAG
#define LOG_TAG "CNativeWindowVideoRender"
#endif // LOG_TAG

#include "voLog.h"

#define VO_RENDER_PID_YUVBUFFERINFO		9

// YCrCb 4:2:0 Planar
#define ANDROID_HAL_PIXEL_FORMAT_YV12				0x32315659

// https://gitorious.org/replicant/hardware_libhardware/source/2bb7c9c2d785f634c7d2338847ac3fa369fc8680:include/hardware/gralloc.h
#define SAMSUNG_GRALLOC_USAGE_HW_FIMC1				0x01000000
#define SAMSUNG_GRALLOC_USAGE_HW_ION				0x02000000
#define SAMSUNG_GRALLOC_USAGE_HWC_HWOVERLAY         0x20000000

namespace android
{
CNativeWindowVideoRender::CNativeWindowVideoRender(void * pSurface, int nFlag)
	: m_piNativeWindow(NULL)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_pBuf(NULL)
	, m_eBoardPlatformType(VO_BOARDPLATFORM_OTHERS)
#ifdef _VOLOG_INFO
	, m_bFirstLock(true)
#endif	// _VOLOG_INFO
{
	property_get("ro.product.brand", m_szProductBrand, "");
	property_get("ro.product.model", m_szProductModel, "");
	property_get("ro.board.platform", m_szBoardPlatform, "");

	if(!strcmp(m_szBoardPlatform, "exynos4"))
		m_eBoardPlatformType = VO_BOARDPLATFORM_EXYNOS4;

	if(!strcmp(m_szBoardPlatform, "rk30xx"))
		m_eBoardPlatformType = VO_BOARDPLATFORM_RK30XX;

	char szProp[PROPERTY_VALUE_MAX];
	property_get("ro.build.version.release", szProp, "");
	memset(&m_nVersionNumber, 0, sizeof(m_nVersionNumber));
	char szTmp[PROPERTY_VALUE_MAX];
	char * pStart = szProp;
	char * pTmp = strchr(pStart, '.');
	VO_S32 nIndex = 0;
	while(pTmp)
	{
		memset(szTmp, 0, sizeof(szTmp));
		memcpy(szTmp, pStart, pTmp - pStart);
		szTmp[pTmp - szProp] = '\0';

		if(nIndex < 3)
			m_nVersionNumber[nIndex] = atoi(szTmp);
		nIndex++;

		pStart = pTmp + 1;
		if(nIndex >= 3 || pStart >= szProp + strlen(szProp))
			break;

		pTmp = strchr(pStart, '.');
	}

	if(nIndex < 3 && pStart < szProp + strlen(szProp))
		m_nVersionNumber[nIndex] = atoi(pStart);

	VOLOGI("brand: %s, model %s, board platform %s, version number %d %d %d", 
		m_szProductBrand, m_szProductModel, m_szBoardPlatform, m_nVersionNumber[0], m_nVersionNumber[1], m_nVersionNumber[2]);

	status_t err = OK;
	if(NULL != m_piNativeWindow.get())
	{
		err = native_window_api_disconnect(m_piNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);
		if(OK != err)
		{
			VOLOGW("failed to native_window_api_disconnect %s (%d)", strerror(-err), -err);
		}
	}

	if(pSurface != NULL)
		m_piNativeWindow = (ANativeWindow *)pSurface;

	if(NULL == m_piNativeWindow.get())
	{
		VOLOGE("null native window");
	}


	err = native_window_api_connect(m_piNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);
}

CNativeWindowVideoRender::~CNativeWindowVideoRender()
{
	native_window_api_disconnect(m_piNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);
}

int CNativeWindowVideoRender::Lock (VOMP_VIDEO_BUFFER * pVideoInfo, VOMP_RECT * pVideoSize)
{
	if(m_piNativeWindow == NULL)
	{
		VOLOGW ("The surface pointer is NULL!");
		return -1;
	}

	status_t err = OK;
	bool bFormatChanged = false;
	int nWidth = 0;
	int nHeight = 0;

	if(VO_BOARDPLATFORM_RK30XX == m_eBoardPlatformType)
	{
		nWidth = (pVideoSize->right - pVideoSize->left + 1) & ~1;
		nHeight = (pVideoSize->bottom - pVideoSize->top + 31) & ~31;
	}
	else
	{
		nWidth = (pVideoSize->right - pVideoSize->left + 1) & ~1;
		nHeight = (pVideoSize->bottom - pVideoSize->top + 1) & ~1;
	}

	if(nWidth != m_nWidth || nHeight != m_nHeight)
	{
		VOLOGI("video size changed OW %d OH %d NW %d NH %d", m_nWidth, m_nHeight, nWidth, nHeight);

		bFormatChanged = true;

		m_nWidth = nWidth;
		m_nHeight = nHeight;

		int nMinUndequeuedBuffers = 0;
		err = m_piNativeWindow->query(m_piNativeWindow.get(), NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS, &nMinUndequeuedBuffers);
		VOLOGI("NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS %d", nMinUndequeuedBuffers);

		err = native_window_set_buffer_count(m_piNativeWindow.get(), nMinUndequeuedBuffers + 1);
		VOLOGI("native_window_set_buffer_count 0x%08X", err);

		int nUsage = GRALLOC_USAGE_SW_READ_NEVER | GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP;
		if(VO_BOARDPLATFORM_EXYNOS4 == m_eBoardPlatformType)	// exynos4 need use extension flag
			nUsage |= SAMSUNG_GRALLOC_USAGE_HW_FIMC1;
		err = native_window_set_usage(m_piNativeWindow.get(), nUsage);
		VOLOGI("native_window_set_usage 0x%08X", err);

		err = native_window_set_scaling_mode(m_piNativeWindow.get(), NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW);
		VOLOGI("native_window_set_scaling_mode 0x%08X", err);

		err = native_window_set_buffers_geometry(m_piNativeWindow.get(), m_nWidth, m_nHeight, ANDROID_HAL_PIXEL_FORMAT_YV12);
		//err = native_window_set_buffers_geometry(m_piNativeWindow.get(), m_nWidth, m_nHeight, 0x13);
		VOLOGI("native_window_set_buffers_geometry 0x%08X", err);

		if(VO_BOARDPLATFORM_RK30XX == m_eBoardPlatformType)
		{
			android_native_rect_t sNativeRect;
			sNativeRect.left = 0;
			sNativeRect.top = 0;
			sNativeRect.right = pVideoSize->right - pVideoSize->left;    
			sNativeRect.bottom = pVideoSize->bottom - pVideoSize->top;
			VOLOGI("Current set crop %d %d %d %d",sNativeRect.left, sNativeRect.top, sNativeRect.right, sNativeRect.bottom);
			err = native_window_set_crop(m_piNativeWindow.get(), &sNativeRect);
			VOLOGI("native_window_set_crop 0x%08X", err);
		}

	}

#ifdef _JB43
	err = native_window_dequeue_buffer_and_wait(m_piNativeWindow.get(), &m_pBuf);
#else
	err = m_piNativeWindow->dequeueBuffer(m_piNativeWindow.get(), &m_pBuf);
#endif
	if(0 != err)
	{
		VOLOGE("failed to dequeueBuffer 0x%08X", err);
		return err;
	}

#ifndef _JB43
	m_piNativeWindow->lockBuffer(m_piNativeWindow.get(), m_pBuf);
#endif

#ifdef _VOLOG_INFO
	if(m_bFirstLock)
	{
		VOLOGI("width %d height %d buffer stride %d buffer height %d", m_nWidth, m_nHeight, m_pBuf->stride, m_pBuf->height);
		m_bFirstLock = false;
	}
#endif	// _VOLOG_INFO

	GraphicBufferMapper &GBMapper = GraphicBufferMapper::get();

	Rect rcBounds(m_nWidth, m_nHeight);

	unsigned char* pBuffer;
	GBMapper.lock(m_pBuf->handle, GRALLOC_USAGE_SW_WRITE_OFTEN, rcBounds, (void**)&pBuffer);

	pVideoInfo->Buffer[0] = (char *)pBuffer;

	if(bFormatChanged && pVideoSize != NULL)
	{
		pVideoSize->left = 0;
		pVideoSize->top = 0;
		pVideoSize->right = m_nWidth;
		pVideoSize->bottom = m_nHeight;
	}

	return 0;
}

int CNativeWindowVideoRender::UnLock (void)
{
	GraphicBufferMapper &GBMapper = GraphicBufferMapper::get();
	GBMapper.unlock(m_pBuf->handle);

	status_t err = OK;
#ifdef _JB43
	err = m_piNativeWindow->queueBuffer(m_piNativeWindow.get(), m_pBuf, -1);
#else
	err = m_piNativeWindow->queueBuffer(m_piNativeWindow.get(), m_pBuf);
#endif
	if(OK != err)
	{
		VOLOGE("failed to queueBuffer 0x%08X", err);
	}

	return 0;
}

int CNativeWindowVideoRender::GetParam (int nID, void * pValue)
{
	if(NULL == pValue)
		return -1;

	switch(nID)
	{
	case VO_RENDER_PID_YUVBUFFERINFO:
		{
			// must call it between Lock and UnLock
			VOMP_VIDEO_BUFFER * pInfo = (VOMP_VIDEO_BUFFER *)pValue;

			pInfo->Stride[0] = m_pBuf->stride;
			if(VO_BOARDPLATFORM_EXYNOS4 == m_eBoardPlatformType)
				pInfo->Stride[1] = pInfo->Stride[2] = m_pBuf->stride / 2;	// exynos4 doesn't need 16 bytes alignment for UV
			else
				pInfo->Stride[1] = pInfo->Stride[2] = (m_pBuf->stride / 2 + 15) & ~15;

			// Buffer[0] is input, we need use it to get Buffer pointers
//			pInfo->Buffer[0] = pInfo->Buffer[0];
			if(VO_BOARDPLATFORM_EXYNOS4 == m_eBoardPlatformType)
			{
				// exynos4 need height 16 bytes alignment and UV 16 bytes alignment
				pInfo->Buffer[2] = pInfo->Buffer[0] + m_pBuf->stride * ((m_pBuf->height + 15) & ~15);
				pInfo->Buffer[1] = pInfo->Buffer[2] + ((pInfo->Stride[2] + 15) & ~15) * ((m_pBuf->height / 2 + 15) & ~15);
			}
			else
			{
				pInfo->Buffer[2] = pInfo->Buffer[0] + m_pBuf->stride * m_pBuf->height;
				pInfo->Buffer[1] = pInfo->Buffer[2] + pInfo->Stride[2] * m_pBuf->height / 2;
			}

			return 0;
		}
		break;

	default:
		break;
	}

	return -1;
}

int CNativeWindowVideoRender::SetParam (int nID, void * pValue)
{
	return -1;
}

} // android
