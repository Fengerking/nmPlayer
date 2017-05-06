	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include <dlfcn.h>

#include "cmnMemory.h"
#include "CSurfaceVideoRender.h"

#include "voLogoData.h"

CSurfaceVideoRender::CSurfaceVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseVideoRender (hInst, hView, pMemOP)
	, m_pSurface (NULL)
	, m_pFrameHeap (NULL)
	, m_nFrameSize (0)
	, m_bInit (VO_FALSE)
	, m_nOffset (0)
	, m_nFrameCount (1)
	, m_nFrameIndex (0)
	, m_pOutBuffer (NULL)
{
	m_pSurface = (ISurface *)m_hView;

	m_nScreenWidth = 480;
	m_nScreenHeight = 320;
}

CSurfaceVideoRender::~CSurfaceVideoRender ()
{
	releaseMemHeap ();
}

VO_U32 CSurfaceVideoRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
//	if (m_fCallBack != NULL)
//		return CBaseVideoRender::Render (pVideoBuffer, nStart, bWait);

	if (m_pSurface == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	sprintf (m_szLog, "CSurfaceVideoRender::Render  0 \r\n");
	m_fCallBack (m_pUserData, NULL, (VO_VIDEO_FORMAT *)m_szLog, 0);

	if (!m_bInit)
		createMemHeap ();

	sprintf (m_szLog, "CSurfaceVideoRender::Render  2 \r\n");
	m_fCallBack (m_pUserData, NULL, (VO_VIDEO_FORMAT *)m_szLog, 0);

	m_OutBuffer.Buffer[0]  = m_pOutBuffer + m_nFrameIndex * m_nFrameSize + m_nOffset;
	if (!ConvertData (pVideoBuffer, &m_OutBuffer, nStart, bWait))
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

	sprintf (m_szLog, "CSurfaceVideoRender::Render  3 \r\n");
	m_fCallBack (m_pUserData, NULL, (VO_VIDEO_FORMAT *)m_szLog, 0);

	for (VO_U32 i = 0; i < VOLOGO_DATA_HEIGHT; i++)
		memcpy (m_OutBuffer.Buffer[0] + m_OutBuffer.Stride[0] * i, voLogoBuffer565 + VOLOGO_DATA_WIDTH * 2 * i, VOLOGO_DATA_WIDTH * 2);

	sprintf (m_szLog, "CSurfaceVideoRender::Render  4 \r\n");
	m_fCallBack (m_pUserData, NULL, (VO_VIDEO_FORMAT *)m_szLog, 0);

	m_pSurface->postBuffer (m_nFrameIndex * m_nFrameSize);

	sprintf (m_szLog, "CSurfaceVideoRender::Render  5 \r\n");
	m_fCallBack (m_pUserData, NULL, (VO_VIDEO_FORMAT *)m_szLog, 0);

	m_nFrameIndex++;
	if (m_nFrameIndex >= m_nFrameCount)
		m_nFrameIndex = 0;

	return VO_ERR_NONE;
}

VO_U32 CSurfaceVideoRender::Redraw (void)
{
	if (m_pSurface != NULL && m_bInit)
		m_pSurface->postBuffer (0);

	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
}

VO_U32 CSurfaceVideoRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect)
{
	m_pSurface = (ISurface *)hView;

	VO_U32	nRC = CBaseVideoRender::SetDispRect (hView, pDispRect);

	return nRC;
}

bool CSurfaceVideoRender::createMemHeap (void)
{
	sprintf (m_szLog, "CSurfaceVideoRender::createMemHeap  0 \r\n");
	m_fCallBack (m_pUserData, NULL, (VO_VIDEO_FORMAT *)m_szLog, 0);

	// release resources if previously initialized
	releaseMemHeap();

	sprintf (m_szLog, "CSurfaceVideoRender::createMemHeap  1 \r\n");
	m_fCallBack (m_pUserData, NULL, (VO_VIDEO_FORMAT *)m_szLog, 0);

 	// YUV420 frames are 1.5 bytes/pixel
    m_nFrameSize = m_nScreenWidth * m_nScreenHeight * 2;
	m_pFrameHeap = new MemoryHeapBase(m_nFrameSize * m_nFrameCount);
    m_BufferHeap = ISurface::BufferHeap(m_nScreenWidth, m_nScreenHeight, m_nScreenWidth, m_nScreenHeight,
										PIXEL_FORMAT_RGB_565, 0, 0, m_pFrameHeap);
	m_pSurface->registerBuffers(m_BufferHeap);

	sprintf (m_szLog, "CSurfaceVideoRender::createMemHeap  2 \r\n");
	m_fCallBack (m_pUserData, NULL, (VO_VIDEO_FORMAT *)m_szLog, 0);

	m_OutBuffer.Buffer[0] = static_cast<unsigned char*>(m_pFrameHeap->base());
	memset (m_OutBuffer.Buffer[0], 0, m_nFrameSize);

	sprintf (m_szLog, "CSurfaceVideoRender::createMemHeap  3 \r\n");
	m_fCallBack (m_pUserData, NULL, (VO_VIDEO_FORMAT *)m_szLog, 0);

	m_OutBuffer.Stride[0] = m_nScreenWidth * 2;
	m_nOffset = m_nDrawLeft * 2 + m_nDrawTop * m_nScreenWidth * 2;
	m_pOutBuffer = m_OutBuffer.Buffer[0];

	sprintf (m_szLog, "CSurfaceVideoRender::createMemHeap  4 \r\n");
	m_fCallBack (m_pUserData, NULL, (VO_VIDEO_FORMAT *)m_szLog, 0);

	m_bInit = VO_TRUE;

	return true;
}

void CSurfaceVideoRender::releaseMemHeap (void)
{
	if (m_bInit)
	{
		m_pSurface->unregisterBuffers();

		m_pFrameHeap.clear();

		m_bInit = VO_FALSE;
	}
}


