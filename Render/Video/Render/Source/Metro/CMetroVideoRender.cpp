

#include "CMetroVideoRender.h"
#include "voLog.h"



CMetroVideoRender::CMetroVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	:CBaseVideoRender(hInst , hView , pMemOP),
	m_pBitmapBuffer(NULL),
	mbUpdate(TRUE)
{
	m_Windows = reinterpret_cast<Windows::UI::Core::CoreWindow^>(hView);
	m_nScreenWidth = m_Windows->Bounds.Width;
	m_nScreenHeight = m_Windows->Bounds.Height;

	mpD2DRender = ref new CD2DVideoRender();
	mpD2DRender->Initialize(m_Windows , 96.0f);
}

CMetroVideoRender::~CMetroVideoRender ()
{

}


VO_U32 CMetroVideoRender::Start (void)
{
	return VO_ERR_NONE;
}

VO_U32 CMetroVideoRender::Pause (void)
{
	return VO_ERR_NONE;
}

VO_U32 CMetroVideoRender::Stop (void)
{
	return VO_ERR_NONE;
}

VO_U32 	CMetroVideoRender::SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
	VO_U32 nRC = CBaseVideoRender::SetVideoInfo(nWidth , nHeight , nColor);

	if(mpD2DRender != nullptr)
	{
		mpD2DRender->SetVideoInfo(m_nShowWidth , m_nShowHeight , m_nShowWidth * 4);
	}

	mbUpdate = TRUE;
	return nRC;
}

VO_U32 	CMetroVideoRender::SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio)
{
	mbUpdate = TRUE;
	return CBaseVideoRender::SetDispType(nZoomMode , nRatio);
}

VO_U32 	CMetroVideoRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor)
{
	mbUpdate = TRUE;
	VO_U32 nRC = CBaseVideoRender::SetDispRect(hView , pDispRect , nColor);

	if(mpD2DRender != nullptr)
	{
		mpD2DRender->SetDispRect(m_nDrawLeft , m_nDrawTop , m_nDrawLeft + m_nDrawWidth , m_nDrawTop + m_nDrawHeight);
	}

	return nRC;
}


VO_U32 CMetroVideoRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	if(mbUpdate == TRUE)
	{
		if(m_pBitmapBuffer != NULL)
		{
			delete [] m_pBitmapBuffer;
			m_pBitmapBuffer = NULL;
		}

		if (m_pCCRRR != NULL)
			m_pCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nShowWidth, &m_nShowHeight, VO_RT_DISABLE);
		if (m_pSoftCCRRR != NULL)
			m_pSoftCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nShowWidth, &m_nShowHeight, VO_RT_DISABLE);


		mbUpdate = FALSE;
		//VOLOGI ("SetCCRRSize %d, %d, %d, %d  %d", (int)m_nShowWidth, (int)m_nShowHeight, (int)m_nDrawWidth, (int)m_nDrawHeight, 0);
	}

	if(m_pBitmapBuffer == NULL)
	{
		m_pBitmapBuffer = new BYTE[m_nShowWidth * m_nShowHeight * 4];
	}

	m_outBuffer.Buffer[0] = m_pBitmapBuffer;
	m_outBuffer.Stride[0] = m_nShowWidth * 4;

	if (!ConvertData (pVideoBuffer, &m_outBuffer, nStart, bWait))
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

	mpD2DRender->Render(m_pBitmapBuffer);
	
	return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
}

