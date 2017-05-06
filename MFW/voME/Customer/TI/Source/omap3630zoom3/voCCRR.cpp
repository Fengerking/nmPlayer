	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCCRR.cpp

	Contains:	voCCRR class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#define LOG_TAG "voCCRR"
#include "utils/Log.h"

#include <media/MediaPlayerInterface.h>
#ifndef __VONJ_FROYO__
#include <ui/Surface.h>
#else
#include <surfaceflinger/Surface.h>
#endif

#include <malloc.h>
#include <string.h>
#include <dlfcn.h>

#include "voCCRR.h"
#include "voinfo.h"

namespace android {

typedef VO_S32 (VO_API * VOMMGETAPI) (VO_PTR pFunc, VO_U32 uFlag);

voCCRR::voCCRR(void)
	: m_hModule (NULL)
	, m_hCCRR (NULL)
	, m_nInWidth (0)
	, m_nInHeight (0)
	, m_nInLeft (0)
	, m_nInTop (0)
	, m_nInRight (0)
	, m_nInBottom (0)
	, m_nOutWidth (0)
	, m_nOutHeight (0)
	, m_nOutLeft (0)
	, m_nOutTop (0)
	, m_nOutRight (0)
	, m_nOutBottom (0)
	, m_nOutColor (VO_COLOR_RGB565_PACKED)
	, m_nAngle (0)
	, m_nRotate (VO_RT_DISABLE)
{
	memset (&m_ccrrFunc, 0, sizeof (VO_VIDEO_CCRRRAPI));
	LoadLib ();
}

voCCRR::~voCCRR ()
{
	if (m_hCCRR != NULL)
		m_ccrrFunc.Uninit (m_hCCRR);
	if (m_hModule != NULL)
		dlclose (m_hModule);
}

int voCCRR::SetInputSize (int width, int height, int left, int top, int right, int bottom)
{
	if (m_hCCRR == NULL)
		return -1;

	if (m_nInWidth != width || m_nInHeight != height || m_nInLeft != left || m_nInTop != top || m_nInBottom != bottom)
	{
		m_nInWidth = width;
		m_nInHeight = height;
		m_nInLeft = left;
		m_nInTop = top;
		m_nInRight = right;
		m_nInBottom = bottom;

		if (m_nOutRight > 0 && m_nOutBottom > 0)
			return UpdateSize ();
	}

	return 0;
}

int voCCRR::SetOutputSize (int width, int height, int left, int top, int right, int bottom)
{
	if (m_hCCRR == NULL)
		return -1;

    //LOGW("voCCRR::SetOutputSize(%dx%d  %d,%d - %d,)", m_nOutWidth, m_nOutHeight, m_nOutLeft, m_nOutTop, m_nOutBottom);
    //LOGW("inR/B: %d, %d", m_nInRight, m_nInBottom);

	if (m_nOutWidth != width || m_nOutHeight != height || m_nOutLeft != left || m_nOutTop != top || m_nOutBottom != bottom)
	{
		m_nOutWidth = width;
		m_nOutHeight = height;
		m_nOutLeft = left;
		m_nOutTop = top;
		m_nOutRight = right;
		m_nOutBottom = bottom;

		if (m_nInRight > 0 && m_nInBottom > 0)
			return UpdateSize ();
	}

	return 0;
}

int	voCCRR::ProcessRGB565 (int format, VO_VIDEO_BUFFER* pIn, VO_VIDEO_BUFFER* pOut, int angle)
{
#ifndef __VONJ_FROYO__
  if (format != PIXEL_FORMAT_YCbCr_420_P || m_hCCRR == NULL)
#else
  if (format != VO_COLOR_YUV_PLANAR420 || m_hCCRR == NULL)
#endif
		return -1;

	if (pOut->ColorType != m_nOutColor)
	{
		m_nOutColor = pOut->ColorType;
		int nRC = m_ccrrFunc.SetColorType (m_hCCRR, VO_COLOR_YUV_PLANAR420, m_nOutColor);
		LOGW ("#################################  rc %d, color  %d \n", nRC, m_nOutColor);
	}

	if (angle != m_nAngle)
	{
		m_nAngle = angle;

		if (m_nAngle == 0)
			m_nRotate = VO_RT_DISABLE;
		else if ((m_nAngle == 90))
			m_nRotate = VO_RT_90R;
		else if ((m_nAngle == 270))
			m_nRotate = VO_RT_90L;
		else
			m_nRotate = VO_RT_DISABLE;

		if (m_nInRight > 0 && m_nInBottom > 0)
		{
			if (UpdateSize () < 0)
				return -1;
		}
	}

//	LOGW ("CCRR Buffer %d, %d, %d %d, %d", m_inBuffer.Buffer[0], m_inBuffer.Buffer[1], m_inBuffer.Buffer[2], m_outBuffer.Buffer[0], angle);

//	LOGW ("CCRR Stride %d, %d, %d %d", m_inBuffer.Stride[0], m_inBuffer.Stride[1], m_inBuffer.Stride[2], m_outBuffer.Stride[0]);

	VO_U32 nRC = m_ccrrFunc.Process (m_hCCRR, pIn, pOut, 0, VO_TRUE);

	if (nRC == 0)
		return 0;

	return -1;
}

int voCCRR::SetSize (int nInWidth, int nInHeight, int nOutWidth, int nOutHeight, int angle)
{
	if (m_hCCRR == NULL)
		return -1;

    m_nAngle = angle;

	return 0;
}


int voCCRR::UpdateSize (void)
{
	if (m_hCCRR == NULL)
		return -1;

    LOGW("voCCRR::UpdateSize()");
	VO_U32 nInWidth = m_nInRight - m_nInLeft;
	VO_U32 nInHeight = m_nInBottom - m_nInTop;
	VO_U32 nOutWidth = m_nOutRight - m_nOutLeft;
	VO_U32 nOutHeight = m_nOutBottom - m_nOutTop;

	nInWidth = nInWidth & ~3;
	nInHeight = nInHeight & ~1;

	nOutWidth = nOutWidth & ~3;
	nOutHeight = nOutHeight & ~1;

	LOGW ("CCRR process %d, %d, %d %d, %d", nInWidth, nInHeight, nOutWidth, nOutHeight, m_nRotate);

	if (m_nAngle == 90)
	{
		VOINFO("SetCCRRSize(%dx%d ==> %dx%d @%d)", nInWidth, nInHeight, nOutHeight, nOutWidth, m_nRotate);
		m_ccrrFunc.SetCCRRSize (m_hCCRR, &nInWidth, &nInHeight, &nOutHeight, &nOutWidth, m_nRotate);
	}
	else
	{
		VOINFO("SetCCRRSize(%dx%d ==> %dx%d @%d)", nInWidth, nInHeight, nOutWidth, nOutHeight, m_nRotate);
		m_ccrrFunc.SetCCRRSize (m_hCCRR, &nInWidth, &nInHeight, &nOutWidth, &nOutHeight, m_nRotate);
	}

	return 0;
}

VO_U32 voCCRR::LoadLib (void)
{
	char s[256];
	strcpy(s, "libvoMMCCRRS.so");
	m_hModule = dlopen (s, RTLD_NOW);

	if (m_hModule == NULL)
		m_hModule = dlopen ("/data/local/voOMXPlayer/lib/libvoMMCCRRS.so", RTLD_NOW);

	if (m_hModule == NULL)
		m_hModule = dlopen ("libvoMMCCRRS.so", RTLD_NOW);

	VOINFO("here....");
	if (m_hModule == NULL)
	{
		LOGE ("@@@@@@ Load Module Error %s \n", dlerror ());
		return -1;
	}

	VOINFO("here....");
	VOMMGETAPI pAPI = (VOMMGETAPI) dlsym(m_hModule, "voGetVideoCCRRRAPI");
	if (pAPI == NULL)
	{
		LOGE ("@@@@@@ Get function address Error %s \n", dlerror ());
		return -1;
	}

	VOINFO("here....");
	pAPI (&m_ccrrFunc, 0);
	if (m_ccrrFunc.Init == NULL)
		return VO_ERR_FAILED;

	VOINFO("here....");
	VO_U32 nRC = m_ccrrFunc.Init (&m_hCCRR, NULL, NULL, 0);
	if (nRC == VO_ERR_NONE)
		m_ccrrFunc.SetColorType (m_hCCRR, VO_COLOR_YUV_PLANAR420, VO_COLOR_RGB565_PACKED);

	VOINFO("OK....");
	return 0;
}

// ---------------------------------------------------------------------------
}; // namespace android

