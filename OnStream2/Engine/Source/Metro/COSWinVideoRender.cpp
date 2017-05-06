#include "COSWinVideoRender.h"
#include <ppltasks.h>
#include "CMetroVideoRender.h"

using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace concurrency;

COSWinVideoRender::COSWinVideoRender(COSWinVomePlayer * pEngine, Windows::UI::Core::CoreWindow^ windows)
	: m_pEngine (pEngine)
	, m_Windows (windows)
	, m_pVideoRender (NULL)
	, m_pRenderThread (NULL)
	, m_nColorType (VOOSMP_COLOR_RGB565_PACKED)
	, m_nStatus (0)
	, m_bSeeking (false)
	, m_nRenderNum(0)
{
	memset (&m_sVideoFormat, 0, sizeof (VOOSMP_VIDEO_FORMAT));

	memset(&m_rcDraw, 0, sizeof(VOMP_RECT));
	memset(&m_VideoBuffer, 0, sizeof(VOMP_BUFFERTYPE));

	m_pVideoRender = new CMetroVideoRender(NULL, reinterpret_cast<void *>(m_Windows), NULL);
}

COSWinVideoRender::~COSWinVideoRender()
{
	Stop();

	while (m_pRenderThread != NULL)
		voOS_Sleep (10);	

	if(m_pVideoRender)
		delete m_pVideoRender;
}

int	COSWinVideoRender::Start (void)
{
	if (m_bSeeking)
		m_bSeeking = false;

	if (m_nStatus == 1)
		return 0;

	m_nStatus = 1;
	m_nRenderNum = 0;

	if(m_pRenderThread == NULL)
	{
		//VO_U32 ThdID;
		//voThreadCreate (&m_pRenderThread, &ThdID, (voThreadProc)VideoRenderThreadProc, this, 0);

		auto workItemDelegate = [this](IAsyncAction^ workItem)
		{
			VideoRenderThreadProc(this);
		};

		auto workItemHandler = ref new WorkItemHandler(workItemDelegate);
		ThreadPool::RunAsync(workItemHandler , WorkItemPriority::Normal);
		m_pRenderThread = (voThreadHandle)1;
	}

	return 0;
}

int COSWinVideoRender::Stop (void)
{
	m_nStatus = 0;

	while (m_pRenderThread != NULL)
		voOS_Sleep (10);	

	return 	0;
}

int COSWinVideoRender::Pause (void)
{
	m_nStatus = 2;
	
	return 0;
}

int	COSWinVideoRender::SetRect (VOMP_RECT *pRect)
{
	m_rcDraw.bottom = pRect->bottom;
	m_rcDraw.left = pRect->left;
	m_rcDraw.right = pRect->right;
	m_rcDraw.top = pRect->top;

	if(m_pVideoRender != NULL)
		m_pVideoRender->SetDispRect (NULL , (VO_RECT *)&m_rcDraw , VO_COLOR_RGB32_PACKED);

	return 0;
}

int	COSWinVideoRender::SetPos (int nPos)
{
	if (m_nStatus == 2)
	{
		m_bSeeking = true;			
		m_nStatus = 1;
	}

	return 0;
}

int COSWinVideoRender::VideoRenderThreadLoop (void)
{
	if (m_pVideoRender == NULL)
		return VOOSMP_ERR_Pointer;

	VOMP_BUFFERTYPE * pVideoBuffer = NULL;

	int nRC = 0;

	m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
	m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);

	//memset(&m_VideoBuffer, 0, sizeof(VOMP_BUFFERTYPE));

	//pVideoBuffer = &m_VideoBuffer;

	//int nRC = m_pEngine->GetVideoBuffer (&pVideoBuffer);
	//if (nRC == VOOSMP_ERR_FormatChange)
	//{
	//	m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
	//	m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
	//	m_pVideoRender->SetDispRect (m_hWnd, (VO_RECT *)&m_rcDraw);		

	//	if(pVideoBuffer == NULL)
	//		return 0;
	//}
	//else if(nRC != VOOSMP_ERR_None)
	//{
	//	voOS_Sleep (2);
	//	return nRC;
	//}

	//if ((m_sVideoFormat.Width == 0 || m_sVideoFormat.Height == 0) && (nRC == VOOSMP_ERR_None || nRC == VOOSMP_ERR_FormatChange))
	//{
	//	m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
	//	m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
	//	m_pVideoRender->SetDispRect (m_hWnd, (VO_RECT *)&m_rcDraw);		
	//	return VOOSMP_ERR_FormatChange;
	//}

	pVideoBuffer = NULL;
	
	nRC = m_pEngine->GetVideoBuffer(&pVideoBuffer);

	if ((m_sVideoFormat.Width == 0 || m_sVideoFormat.Height == 0) && (nRC == VOOSMP_ERR_None || nRC == VOOSMP_ERR_FormatChange))
	{
		m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
		m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
		m_pVideoRender->SetDispRect (reinterpret_cast<void *>(m_Windows), (VO_RECT *)&m_rcDraw , VO_COLOR_RGB32_PACKED);		
		return VOOSMP_ERR_FormatChange;
	}

	if (nRC == VOOSMP_ERR_Retry)
	{
		voOS_Sleep (1);
		return 0;
	}
	else if (nRC == VOOSMP_ERR_FormatChange)
	{
		m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
		m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
		m_pVideoRender->SetDispRect (reinterpret_cast<void *>(m_Windows), (VO_RECT *)&m_rcDraw , VO_COLOR_RGB32_PACKED);		

		if(pVideoBuffer == NULL)
			return 0;
	}
	else if (nRC != VOMP_ERR_None)
	{
		return 0;
	}
	else if (pVideoBuffer == NULL)
	{
		voOS_Sleep (5);
		return 0;
	}

	if(nRC == VOOSMP_ERR_None)
	{
		if(m_nRenderNum == 0)
		{
			m_pEngine->GetParam (VOOSMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
			m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
			m_pVideoRender->SetDispRect (reinterpret_cast<void *>(m_Windows), (VO_RECT *)&m_rcDraw , VO_COLOR_RGB32_PACKED);
		}

		m_nRenderNum++;
	}	

	if (m_pVideoRender != NULL && m_nRenderNum > 1)
	{
		if (pVideoBuffer->nSize == sizeof (VO_VIDEO_BUFFER))
		{
			VO_VIDEO_BUFFER *pvoVideoBuffer = (VO_VIDEO_BUFFER *)pVideoBuffer->pBuffer;
			m_pVideoRender->Render ((VO_VIDEO_BUFFER *)pVideoBuffer->pBuffer, pVideoBuffer->llTime, VO_TRUE);
		}
	}

	return 0;
}

int	COSWinVideoRender::VideoRenderThreadProc (void * pParam)
{
	COSWinVideoRender * pRender = (COSWinVideoRender *)pParam;

	while (pRender->m_nStatus == 1 || pRender->m_nStatus == 2)
	{
		if (pRender->m_nStatus == 2)
		{
			voOS_Sleep(2);
			continue;
		}

		pRender->VideoRenderThreadLoop ();
	}

	pRender->m_pRenderThread = NULL;


	return 0;
}

