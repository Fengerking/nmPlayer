#include "windows.h"
#include <tchar.h>

#include "voSubTitleWindowlessRender.h"
#include "voSubtitleType.h"
#include "voSubTitleManager.h"
#include "CBaseBitmap.h"



voSubTitleWindowlessRender::voSubTitleWindowlessRender(voSubTitleManager* manager)
	: voSubTitleRenderBase(manager)
{
	m_bBltAfterUpdated = false;
	ResetetTransparentColor();
	m_bEnableCreatRegion = false;
	if(m_rectDisplay == NULL)
	{
		m_rectDisplay = new RECT();
		m_rectDisplay->left = 400;
		m_rectDisplay->right = 800;
		m_rectDisplay->top = 400;
		m_rectDisplay->bottom = 600;
	}
	if(m_rectNeedUpdated == NULL)
	{
		m_rectNeedUpdated = new RECT();
		ResetRectNeedUpdate();
	}

	
}

voSubTitleWindowlessRender::~voSubTitleWindowlessRender(void)
{
}
void voSubTitleWindowlessRender::Show(bool bShow)
{
	m_bShow = bShow;
}

bool voSubTitleWindowlessRender::Update(bool bForce)
{
	if(!bForce)
	{//check window position or size
		//return false;
	}
	else
		m_bForceRedrawAll = true;
	bool b = false;
	if(this->m_pManager)
	{
		HDC dc =	NULL;
		if(m_pManager->IsID3Picture() && m_pManager->GetTypeRender() == VOOSMP_RENDER_TYPE_DDRAW)
			dc = ::GetDC(m_pManager->GetWindow());
		//b = OnDraw(dc);
		//::ReleaseDC(m_pManager->GetWindow(), dc);
		//HDC dc;
		//if(m_pManager->GetWindowlessDC())
		//	dc = m_pManager->GetWindowlessDC();
		//else
		//	dc = ::GetDC(m_pManager->GetWindow());
		b = OnDraw((m_pManager->IsID3Picture() && m_pManager->GetTypeRender() == VOOSMP_RENDER_TYPE_DDRAW)?dc:m_pManager->GetWindowlessDC());
		//if(m_pManager->GetWindowlessDC() == NULL)
		if(m_pManager->IsID3Picture() && m_pManager->GetTypeRender() == VOOSMP_RENDER_TYPE_DDRAW)
			::ReleaseDC(m_pManager->GetWindow(), dc);
	}

	return b;
}
bool voSubTitleWindowlessRender::CreateWnd (HWND hParent)
{
	if (m_hWnd != NULL)
	{
		SetParent (m_hWnd, hParent);
		return true;
	}
#ifdef _WIN32_WCE
	HINSTANCE hInst = NULL;
#else
	HINSTANCE hInst = (HINSTANCE )GetWindowLong (hParent, GWL_HINSTANCE);
#endif //_WIN32_WCE

	RECT rcView;
	::GetClientRect(hParent, &rcView);

	WNDCLASS wcex;

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ViewWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)NULL;
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= m_szClassName;

	RegisterClass(&wcex);


#ifndef WINCE
	m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);
	SetWindowPos (m_hWnd, NULL, rcView.left, rcView.top,1,1,0);// rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
#else
	m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);
	SetWindowPos (m_hWnd, HWND_TOP, rcView.left , rcView.top ,1,1,0);// rcView.right - rcView.left , rcView.bottom - rcView.top , 0);
#endif 
	

	if (m_hWnd == NULL)
		return false;

	LONG lRC = SetWindowLong (m_hWnd, GWL_USERDATA, (LONG)this);

	return true;
}

bool voSubTitleWindowlessRender::OnDraw(HDC dc, bool bOnlyBitblt)
{
	if(!m_bShow)
		return false;
	if(m_pManager==NULL )
		return false;
	RECT r;
	r.left = 100;
	r.top = 100;
	r.bottom = 300;
	r.right = 300;
	if(this->m_rectDisplay)
		r=*m_rectDisplay;
	//bool bNull = false;
	//if(dc == NULL )
	//{
	//	bNull = true;
	//	dc = ::GetDC(m_pManager->GetWindow());
	//}
	//if(dc!=NULL)
	OnDrawToDC( dc, r, bOnlyBitblt);
	//if(bNull)
	//	::ReleaseDC(m_pManager->GetWindow(), dc);
	return true;

}
void voSubTitleWindowlessRender::BitbltToDC(HDC dc, HDC hMemDC, bool bRecreateBitmap)
{
	if(m_pManager->GetEnableDrawValue() == 0)
		return;
	if(m_pManager->GetEnableDrawValue()>1)
	{
		if(::GetTickCount()-m_pManager->GetEnableDrawValue()<2000)
			return;
		else
			m_pManager->SetEnableDrawValue(1);
	}
	if(m_rectDisplay && m_pManager)
	{
		int nLeft = m_rectDisplay->left;
		int nTop = m_rectDisplay->top;
		int nW = m_width;
		int nH = m_height;
		//int nLeftSrc = 0;
		//int nTopSrc = 0;
		if(m_pManager->GetTypeRender() == VOOSMP_RENDER_TYPE_DDRAW)
		{
			nLeft = -m_rectVaild.left;
			nTop = -m_rectVaild.top;
		}
		if(m_rectNeedUpdated)
		{
			nLeft+=(m_rectNeedUpdated->left);
			nTop+=(m_rectNeedUpdated->top);
			nW = m_rectNeedUpdated->right - m_rectNeedUpdated->left;//300;//
			nH = m_rectNeedUpdated->bottom - m_rectNeedUpdated->top;
		}

		if(dc == NULL)
			return;
		if(bRecreateBitmap && !m_pManager->IsID3Picture())
			return;
		//	m_img.Draw(dc,nLeft,nTop,nW,nH,0,0,nW,nH);

		if(m_pManager->GetTypeRender() != VOOSMP_RENDER_TYPE_DDRAW && !m_pManager->IsID3Picture()){
		BLENDFUNCTION bf;

		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 0xff;
		bf.AlphaFormat = AC_SRC_ALPHA;
		BOOL bResult = ::AlphaBlend( dc, nLeft,nTop,nW,nH, 
			hMemDC, m_rectNeedUpdated->left,m_rectNeedUpdated->top,nW,nH, bf );
								//char ch[1024];
								//sprintf(ch,"AlphaBlend, l=%d, t=%d, r=%d, b=%d, string=%s\r\n",nLeft,nTop,nW,nH,"hh");
								//::OutputDebugStringA(ch);
		}
		else{
		BOOL bResult = ::TransparentBlt( dc, nLeft,nTop,nW,nH,
			hMemDC, m_rectNeedUpdated->left,m_rectNeedUpdated->top,nW,nH, RGB(1,1,1) );
								//char ch[1024];
								//sprintf(ch,"AlphaBlend 222, l=%d, t=%d, r=%d, b=%d, string=%s\r\n",nLeft,nTop,nW,nH,"hh");
								//::OutputDebugStringA(ch);
		}
	}
	else
		::BitBlt(dc,0,0,this->m_width,this->m_height,hMemDC,0,0,SRCCOPY);
}

void voSubTitleWindowlessRender::Draw(HDC hdc, void* pParam)
{
	//m_pBufferIn = pParam;
	VOOSMP_VR_USERCALLBACK_TYPE* pVr = (VOOSMP_VR_USERCALLBACK_TYPE*)pParam;
	if(pVr){
		if(pVr->pRect)
		{
			RECT * r = (RECT *)pVr->pRect;
			float f = -1.0f;
			if(r->bottom - r->top > 0)
				f = (r->right - r->left)/(float)(r->bottom - r->top);
			if(m_pManager->GetXyRate()-f < -0.001 || m_pManager->GetXyRate()-f>0.001)
				m_pManager->SetXYRate(f);
		}
	}
	if(this->m_rectDisplay!=NULL)
		if(m_width != m_rectDisplay->right - m_rectDisplay->left  || m_height != m_rectDisplay->bottom - m_rectDisplay->top)
		{
			m_bForceRedrawAll = true;
			voCAutoLock lock(m_pManager->GetMutexObject());
			OnDraw(hdc,false);
			m_bForceRedrawAll = false;

			//when pause on switch to full screen, redraw
			OnDraw(hdc,true);
			return;
		}


	if(m_bForceRedrawAll)
		return;
	voCAutoLock lock(m_pManager->GetMutexObject());

	HDC hdc2 = NULL;
	if(m_pManager && (m_pManager->IsID3Picture() && m_pManager->GetTypeRender() == VOOSMP_RENDER_TYPE_DDRAW))
		hdc2 = ::GetDC(m_pManager->GetWindow());
	OnDraw(hdc2==NULL?hdc:hdc2,true);
	if(m_pManager && (m_pManager->IsID3Picture() && m_pManager->GetTypeRender() == VOOSMP_RENDER_TYPE_DDRAW))
		::ReleaseDC(m_pManager->GetWindow(), hdc2);
}
void voSubTitleWindowlessRender::UpdateBack (HDC dc, RECT rcDisplay, COLORREF clr, BYTE nTransparent)
{
#ifndef _WIN32_WCE
	Gdiplus::Graphics gs(dc);
	Gdiplus::Rect rf(rcDisplay.left, rcDisplay.top, rcDisplay.right-rcDisplay.left, rcDisplay.bottom - rcDisplay.top);
	Gdiplus::Color clr2(nTransparent,GetRValue(clr),GetGValue(clr),GetBValue(clr));
	Gdiplus::SolidBrush brh(clr2);
    gs.FillRectangle(&brh,
                         rf);
	//gs.DrawString(lpchText, cchText,&Gdiplus::Font(hdc,m_font),rf,&fmt,&Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(nClr),GetGValue(nClr),GetBValue(nClr))));
	//CheckRectNeedUpdate(lprc);
#else
	HBRUSH  bh = CreateSolidBrush(clr);//clr
	FillRect(dc,&rcDisplay,bh);
	::DeleteObject(bh);
#endif // _WIN32_WCE
}
void voSubTitleWindowlessRender::DrawString(HDC hdc, TCHAR* lpchText, int cchText, LPRECT lprc)
{
#ifndef _WIN32_WCE
	Gdiplus::Graphics gs(hdc);
	Gdiplus::RectF rf((Gdiplus::REAL)lprc->left, (Gdiplus::REAL)(lprc->top+3), (Gdiplus::REAL)(lprc->right-lprc->left), (Gdiplus::REAL)(lprc->bottom - lprc->top));
	Gdiplus::StringFormat fmt;
	fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
	COLORREF nClr = ::GetTextColor(hdc);

	gs.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	gs.DrawString(lpchText, cchText,&Gdiplus::Font(hdc,m_font),rf,&fmt,&Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(nClr),GetGValue(nClr),GetBValue(nClr))));
	CheckRectNeedUpdate(lprc);
#endif // _WIN32_WCE
}


bool voSubTitleWindowlessRender::ResizeWindow (void)
{
	return true;
}

LRESULT voSubTitleWindowlessRender::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_ERASEBKGND) 
	{
		return (LRESULT) 1;
	}
	else if (uMsg == WM_CLOSE) 
	{
		return 1;
	}
	else if (uMsg == WM_PAINT)
	{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
	}

	return	__super::OnReceiveMessage( hwnd, uMsg, wParam, lParam);//DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int voSubTitleWindowlessRender::Start()
{
	int nRtn = __super::Start();
	if(this->m_pManager)
	{
		//if(m_pManager->GetTypeRender() == VOOSMP_RENDER_TYPE_DDRAW)
		{
				if(m_pManager->GetListenerInfo()->pListener!=NULL)
				{
								char ch[1024];
								sprintf(ch,"voSubTitleWindowlessRender::Start \r\n");
								::OutputDebugStringA(ch);
					int nP1 = VOOSMP_FLAG_SUBTITLE_VR_USERCALLBACK, nP2 = 1;
					m_pManager->GetListenerInfo()->pListener(m_pManager->GetListenerInfo()->pUserData , VOOSMP_PID_CLOSED_CAPTION_NOTIFY_EVENT , &nP1 , &nP2);
				}
		}
	}
	return nRtn;
	//return VOOSMP_ERR_None;
}
int voSubTitleWindowlessRender::Stop()
{
	int nRtn = __super::Stop();
	//if(this->m_pManager)
	//{
	//	//if(m_pManager->GetTypeRender() == VOOSMP_RENDER_TYPE_DDRAW)
	//	{
	//			if(m_pManager->GetListenerInfo()->pListener!=NULL)
	//			{
	//							char ch[1024];
	//							sprintf(ch,"voSubTitleWindowlessRender::Stop \r\n");
	//							::OutputDebugStringA(ch);
	//				int nP1 = VOOSMP_FLAG_SUBTITLE_VR_USERCALLBACK, nP2 = 0;
	//				m_pManager->GetListenerInfo()->pListener(m_pManager->GetListenerInfo()->pUserData , VOOSMP_PID_CLOSED_CAPTION_NOTIFY_EVENT , &nP1 , &nP2);
	//			}
	//	}
	//}
	return nRtn;
}



