#include "CCCRRRRender.h"

CCCRRRRender::CCCRRRRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
: CBaseVideoRender(hInst , hView , pMemOP)
{
	m_hWinDC = GetDC ((HWND) m_hView);
}

CCCRRRRender::~CCCRRRRender(void)
{
	if (m_hWinDC != NULL)
	{
		ReleaseDC ((HWND)m_hView, m_hWinDC);
		m_hWinDC = NULL;
	}
}

VO_U32 CCCRRRRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	if(m_pCCRRR != NULL)
	{
		return m_pCCRRR->Process(pVideoBuffer , NULL , nStart , bWait);
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 	CCCRRRRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor)
{
	return CBaseVideoRender::SetDispRect(hView , pDispRect , nColor);
}

VO_U32 CCCRRRRender::UpdateSize (void)
{
	CBaseVideoRender::UpdateSize ();

	EraseBackGround();
	return 0;
}

void	CCCRRRRender::EraseBackGround()
{
	int wndWidth = m_rcDisplay.right - m_rcDisplay.left;
	int wndHeight = m_rcDisplay.bottom - m_rcDisplay.top;

	HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));

	RECT drawRect;
	if(wndWidth > m_nDrawWidth)
	{
		int nDrawWidth = (wndWidth - m_nDrawWidth) / 2 ; 
		nDrawWidth = (nDrawWidth + 1) / 2 * 2;
		if(nDrawWidth > 0)
		{
			drawRect.top = m_rcDisplay.top;
			drawRect.bottom = m_rcDisplay.bottom;
			drawRect.left = m_rcDisplay.left;
			drawRect.right = drawRect.left + nDrawWidth + 2;

			FillRect((HDC)m_hWinDC, &drawRect, hBrush);

			drawRect.left = m_rcDisplay.right - nDrawWidth - 2;
			drawRect.right = m_rcDisplay.right;

			FillRect((HDC)m_hWinDC, &drawRect, hBrush);
		}
	}

	if(wndHeight > m_nDrawHeight)
	{
		int nDrawHeight = (wndHeight - m_nDrawHeight) / 2;
		nDrawHeight = (nDrawHeight + 1) / 2 * 2;
		if(nDrawHeight > 0)
		{
			drawRect.top = m_rcDisplay.top;
			drawRect.bottom =  drawRect.top + nDrawHeight + 2;
			drawRect.left = m_rcDisplay.left;
			drawRect.right = m_rcDisplay.right;

			FillRect((HDC)m_hWinDC, &drawRect, hBrush);

			drawRect.top =m_rcDisplay.bottom -  nDrawHeight -2 ;
			drawRect.bottom =  m_rcDisplay.bottom;

			FillRect((HDC)m_hWinDC, &drawRect, hBrush);
		}
	}

	DeleteObject(hBrush);
}
