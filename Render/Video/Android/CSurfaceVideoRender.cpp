
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

#include "CSurfaceVideoRender.h"

#ifndef LOG_TAG
#define LOG_TAG "CSurfaceVideoRender"
#endif // LOG_TAG

#include "voLog.h"

namespace android {

CSurfaceVideoRender::CSurfaceVideoRender(void * pSurface, int nFlag)
	:m_nFlag (nFlag)
	, m_nBytesPixel (0)
	, m_nLockNum (0)
	, m_nRendNum (0)
{
	if (pSurface != NULL)
		m_pSurface = (Surface *)pSurface;
}

CSurfaceVideoRender::~CSurfaceVideoRender()
{
}

int CSurfaceVideoRender::Lock (VOMP_VIDEO_BUFFER * pVideoInfo, VOMP_RECT * pVideoSize)
{
	if (m_pSurface == NULL)
	{
		VOLOGW ("The surface pointer is NULL!");
		return -1;
	}
	if (!m_pSurface->isValid())
	{
		VOLOGW ("The surface is not valid!");
		return -1;
	}

	status_t s =  m_pSurface->lock(&m_siInfo, true);
	if (s)
	{
		VOLOGW ("surface lock failed: 0x%x", s);
		return s;
	}
	m_nLockNum++;

	if (m_siInfo.format == GGL_PIXEL_FORMAT_RGBA_8888)
	{
		pVideoInfo->ColorType = VOMP_COLOR_ARGB32_PACKED;
		m_nBytesPixel = 4;
	}
	else if (m_siInfo.format == GGL_PIXEL_FORMAT_RGBX_8888)
	{
		pVideoInfo->ColorType = VOMP_COLOR_RGB32_PACKED;
		m_nBytesPixel = 4;
	}
	else if (m_siInfo.format ==  GGL_PIXEL_FORMAT_RGB_888)
	{
		pVideoInfo->ColorType = VOMP_COLOR_RGB888_PACKED;
		m_nBytesPixel = 3;
	}
	else
	{
		pVideoInfo->ColorType = VOMP_COLOR_RGB565_PACKED;
		m_nBytesPixel = 2;
	}

#if defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__ || defined __VONJ_GINGERBREAD__
	pVideoInfo->Stride[0] = m_siInfo.s * m_nBytesPixel;
	if (m_nRendNum == 0)
		VOLOGI ("m_nBytesPixel %d, Width %d, Height %d, Stride: %d, Usage: %d, format %d, Buffer %p", m_nBytesPixel, m_siInfo.w, m_siInfo.h, m_siInfo.s, m_siInfo.usage, m_siInfo.format, m_siInfo.bits);
#elif defined __VONJ_CUPCAKE__
	pVideoInfo->Stride[0] = m_siInfo.bpr * m_nBytesPixel;
#elif defined __VONJ_DONUT__
	// Mot clear why need to divid with 2. It maybe the system bug.
	pVideoInfo->Stride[0] = m_siInfo.bpr;
	if (m_nRendNum == 0)
		VOLOGI ("m_nBytesPixel %d, Width %d, Height %d, Stride: %d, format %d, Buffer %p", m_nBytesPixel, m_siInfo.w, m_siInfo.h, m_siInfo.bpr, m_siInfo.format, m_siInfo.bits);
#endif

	pVideoInfo->Buffer[0] = (char*) m_siInfo.bits;

	if (pVideoSize != NULL)
	{
		pVideoSize->left = 0;
		pVideoSize->top = 0;
		pVideoSize->right = m_siInfo.w;
		pVideoSize->bottom = m_siInfo.h;
	}

	m_nRendNum++;

	return 0;
}

int CSurfaceVideoRender::UnLock (void)
{
	if (m_nLockNum <= 0)
		return 0;

	m_pSurface->unlockAndPost();
	m_nLockNum--;

	return 0;
}

int CSurfaceVideoRender::GetParam (int nID, void * pValue)
{
	return -1;
}

int CSurfaceVideoRender::SetParam (int nID, void * pValue)
{
	return -1;
}

} // android
