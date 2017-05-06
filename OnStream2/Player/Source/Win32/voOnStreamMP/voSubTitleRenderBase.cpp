#include "voSubTitleRenderBase.h"
#include "voSubTitleManager.h"
#include "voSubTitleFormatSettingImpl.h"
#include "CBaseBitmap.h"
#include "atlimage.h"
#ifndef _WIN32_WCE
#include "GdiPlusGraphics.h"
#endif // _WIN32_WCE

//#pragma comment(lib, "gdiplus.lib")

#define _TIMER_ID_SUBTITLE 2357
#define _ENABLE_PICTURE_SUBTITLE

//enum VOOSMP_IMAGE_TYPE
//{
//		VOOSMP_IMAGE_Unused   =                       (0x00000000),
//		VOOSMP_IMAGE_RGB565   =                       (0x00000001),
//		VOOSMP_IMAGE_RGB24    =                       (0x00000002),
//		VO_IMAGE_RGBA32   =                       (0x00000003),
//		VOOSMP_IMAGE_ARGB32   =                       (0x00000004),
//		VOOSMP_IMAGE_JPEG     =                       (0x00000005),
//		VOOSMP_IMAGE_BMP      =                       (0x00000006),
//		VOOSMP_IMAGE_PNG      =                       (0x00000007),
//		VOOSMP_IMAGE_MAX      =                       (0x7fffffff)
//};
//
voGraphics::voGraphics()
{
#ifndef _WIN32_WCE
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL); 	
#endif // _WIN32_WCE
	//Graphics gs(NULL);
}

voGraphics::~voGraphics()
{
#ifndef _WIN32_WCE
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
#endif // _WIN32_WCE

}


LRESULT CALLBACK voSubTitleRenderBase::ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	voSubTitleRenderBase * pSubTitleWnd = (voSubTitleRenderBase *)GetWindowLong (hwnd, GWL_USERDATA);
	if (pSubTitleWnd == NULL)
		return(DefWindowProc(hwnd, uMsg, wParam, lParam));
	else
		return pSubTitleWnd->OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

voSubTitleRenderBase::voSubTitleRenderBase(voSubTitleManager* manager)
{
	m_bEnableCreatRegion = false;
	m_bUseDDrawKeyColor = false;
	m_bShow = true;
	m_pManager = manager;
	m_width = 0;
	m_height = 0;
	m_font = NULL;
	m_hMessageWnd = NULL;
	_tcscpy (m_szClassName, _T("voSubTitleWindow"));
	_tcscpy (m_szWindowName, _T("voSubTitleWindow"));
	m_hWnd = (NULL);
	m_clrBack = RGB(0,0,0);
	memset(&m_rectVaild,0,sizeof(m_rectVaild));
	m_bWin8 = false;
	m_rectDisplay = NULL;
	m_bitmapBK = NULL;
	m_bForceRedrawAll = true;
	m_rectNeedUpdated = NULL;
	m_pBufferIn = NULL;
	m_bBltAfterUpdated = true;
	m_bHandleSubtitleData = false;
	m_bRunning = false;
}

voSubTitleRenderBase::~voSubTitleRenderBase(void)
{
	this->Stop();
	voCAutoLock lock(m_pManager->GetMutexObject());

	SetWindowLong (m_hWnd, GWL_USERDATA, (LONG)0);

	if(m_font)
		::DeleteObject(m_font);
	m_font = NULL;
	if(	m_hMessageWnd)
		::DestroyWindow(m_hMessageWnd);
	m_hMessageWnd = NULL;
	if(m_bEnableCreatRegion)
	{
		if(m_hWnd)
		{
			HRGN rgn = NULL;
			::GetWindowRgn(m_hWnd,rgn);//::GetRe
			if(rgn)
				::DeleteObject(rgn);
		}
	}
	if(m_hWnd)
		::DestroyWindow(m_hWnd);
	UnregisterClass (m_szClassName, NULL);
	if(m_rectDisplay)
		delete m_rectDisplay;
	m_rectDisplay = NULL;
	DeleteBitmapBK();
	if(m_rectNeedUpdated)
		delete m_rectNeedUpdated;
	m_rectNeedUpdated = NULL;
	

}
void voSubTitleRenderBase::ResetetTransparentColor()
{
	if(m_bUseDDrawKeyColor)
		m_clrBack = RGB(0,0,0);
	else
		m_clrBack = RGB(1,1,1);
}
HWND voSubTitleRenderBase::GetMessageWindow()
{
	if(m_hMessageWnd==NULL && m_pManager)
	{
//#ifdef _WIN32_WCE
//	HINSTANCE hInst = NULL;
//#else
//	HINSTANCE hInst = (HINSTANCE )GetWindowLong (::GetDesktopWindow(), GWL_HINSTANCE);
//#endif //_WIN32_WCE
//		 m_hMessageWnd = CreateWindow (_T("Static"), NULL, WS_POPUP , 
//					  0, 0, 1, 1,
//					  ::GetDesktopWindow(), (HMENU)NULL, hInst, NULL);
	}
	return m_hMessageWnd;
}
int voSubTitleRenderBase::toRealX(int x, bool bComputeScale,  bool bAddValidLeft)
{
	int x2=x;
	if(bComputeScale && m_pManager)
	{//only for left side
		voSubTitleFormatSettingImpl* sett = m_pManager->GetSettings();
		if(sett)
		{
			if(sett->GetFontSizeEnable())
			{
				float fScale = sett->GetFontSizeMedium();
				if(fScale>1)
				{
					x2 = (int)(x2*fScale);
				}
			}
		}
	}
	return (bAddValidLeft?m_rectVaild.left:0)+(m_rectVaild.right - m_rectVaild.left)*x2/100;
}
int voSubTitleRenderBase::toRealY(int y, bool bComputeScale, bool bAddValidTop)
{
	int x2=y;
	if(bComputeScale && m_pManager)
	{//
		voSubTitleFormatSettingImpl* sett = m_pManager->GetSettings();
		if(sett)
		{
			if(sett->GetFontSizeEnable())
			{
				float fScale = sett->GetFontSizeMedium();
				x2 = (int)(x2*fScale);
			}
		}
	}
	return (bAddValidTop?m_rectVaild.top:0)+(m_rectVaild.bottom - m_rectVaild.top)*x2/100;
}

void voSubTitleRenderBase::reverseString(TCHAR *ch)
{
}

float voSubTitleRenderBase::getFontSize(pvoSubtitleTextInfoEntry textInfo)
{
	float rate2 = 0.75f;
	if(textInfo->stStringInfo.stFontInfo.FontSize == 1)
	{
		rate2 = .7f;
	}
	if(textInfo->stStringInfo.stFontInfo.FontSize == 2)
	{
		rate2 = .8f;
	}
	return rate2;
}


int voSubTitleRenderBase::setTextViewTextInfo(pvoSubtitleTextRowInfo rowInfo, pvoSubtitleTextInfoEntry textInfo, HDC textPaint, HDC strokePaint)
{
	int nFntSize = 0;
	::SetTextColor(textPaint, RGB(textInfo->stStringInfo.stFontInfo.stFontColor.nRed,
			textInfo->stStringInfo.stFontInfo.stFontColor.nGreen, 
			textInfo->stStringInfo.stFontInfo.stFontColor.nBlue));
	voSubTitleFormatSettingImpl* settings = m_pManager->GetSettings();
	if(settings != NULL && settings->GetFontColorEnable())  
	{
		::SetTextColor(textPaint,settings->GetFontColor());
	}
	::SetBkMode(textPaint,TRANSPARENT);
	//if(settings != NULL && settings->GetBackgroundColorEnable())  
	//{
	//	::SetBkColor(textPaint,settings->GetBackgroundColor());
	//}
	//else
	//{
	//	if(rowInfo->stTextRowDes.stDataBox.stRectFillColor.nTransparency >0)
	//		::SetBkColor(textPaint,RGB(rowInfo->stTextRowDes.stDataBox.stRectFillColor.nRed,
	//							rowInfo->stTextRowDes.stDataBox.stRectFillColor.nGreen,
	//							rowInfo->stTextRowDes.stDataBox.stRectFillColor.nBlue ));
	//}
	BOOL italic = FALSE;
	BOOL underline = FALSE;
	int weight = FW_BOLD;//FW_NORMAL;
	if(textInfo->stStringInfo.stFontInfo.FontSize == 103)//to support ttml
		weight = FW_BOLD;
	if(textInfo->stStringInfo.stFontInfo.FontSize == 102)
		weight = FW_NORMAL;
	if(settings != NULL && settings->GetFontBoldEnable())  
	{
		weight = settings->GetFontBold()==0?FW_NORMAL:FW_BOLD;
	}
	
	if(textInfo->stStringInfo.stCharEffect.Italic!=0){
		italic = TRUE;//fontItalic
	}
	if(textInfo->stStringInfo.stCharEffect.Underline!=0){
		underline = TRUE;//fontItalic
	}
	if(settings != NULL && settings->GetFontItalicEnable())  
	{
		italic = settings->GetFontItalic()==0?FALSE:TRUE;
	}
	if(settings != NULL && settings->GetFontUnderlineEnable())  
	{
		underline = settings->GetFontUnderline()==0?FALSE:TRUE;
	}
	TCHAR ch[64];
	_tcscpy(ch,_T(""));
	if(settings)
		settings->ConvertFontName(ch, textInfo->stStringInfo.stFontInfo.FontStyle);
	if(settings != NULL && settings->GetFontName())  
	{
		_tcscpy(ch,settings->GetFontName());
	}

	{
		bool bVertical = rowInfo->stTextRowDes.nPrintDirection>1?true:false;
		
		float rate2 = getFontSize(textInfo);
		float fsz2 = 0;
		if(bVertical)
		{
			//int nLeft = toRealX(rowInfo->stTextRowDes.stDataBox.stRect.nLeft);
			int nWidth = toRealX(rowInfo->stTextRowDes.stDataBox.stRect.nRight-rowInfo->stTextRowDes.stDataBox.stRect.nLeft, true,false);//-nLeft;
			fsz2 = (nWidth*rate2);
		}
		else
		{
			//int nTop = toRealY(rowInfo->stTextRowDes.stDataBox.stRect.nTop);
			int nHeight = toRealY(rowInfo->stTextRowDes.stDataBox.stRect.nBottom-rowInfo->stTextRowDes.stDataBox.stRect.nTop, true,false);//-nTop;
			fsz2 = (float)(nHeight*rate2);//tv.getTextSize();
		}
		
		nFntSize = (int)(fsz2);
	}
	
	if(m_font)
		::DeleteObject(m_font);


	LOGFONT fnt;
	memset(&fnt,0,sizeof(LOGFONT));
	fnt.lfCharSet = DEFAULT_CHARSET;
	fnt.lfHeight = nFntSize;
	_tcscpy(fnt.lfFaceName,ch);// = DEFAULT_CHARSET;
	fnt.lfWeight = weight;
	fnt.lfUnderline = underline;
	fnt.lfItalic = italic;
	fnt.lfQuality = CLEARTYPE_QUALITY;//ANTIALIASED_QUALITY;
	m_font = ::CreateFontIndirect(&fnt);

	return nFntSize;
}
bool voSubTitleRenderBase::CreateWnd (HWND hParent)
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
	if(m_bUseDDrawKeyColor)
	{
		m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);
	}
	else
	{
		if(m_hWnd == NULL)
		{
			m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);
		}
		//else
		//{
		//	m_bWin8 = true;
		//	m_bEnableCreatRegion = true;
		//	ResetetTransparentColor();
		//	DWORD rtn = ::GetLastError();
		//	::SetLayeredWindowAttributes(m_hWnd, m_clrBack, 255,LWA_COLORKEY);//LWA_COLORKEY//LWA_ALPHA|
		//	
		//}
	}
	SetWindowPos (m_hWnd, NULL, rcView.left, rcView.top,1,1,0);// rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
#else
	//SetWindowPos (m_hWnd, HWND_TOP, rcView.left, rcView.top + 24, rcView.right - rcView.left, rcView.bottom - rcView.top - 24, 0);
	m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);
	SetWindowPos (m_hWnd, HWND_TOP, rcView.left , rcView.top ,1,1,0);// rcView.right - rcView.left , rcView.bottom - rcView.top , 0);
#endif 
	

	if (m_hWnd == NULL)
		return false;

	LONG lRC = SetWindowLong (m_hWnd, GWL_USERDATA, (LONG)this);

	return true;
}
int voSubTitleRenderBase::Start()
{
	//Stop();
	if(m_hWnd)
		::SetTimer(m_hWnd, _TIMER_ID_SUBTITLE, 500, NULL);
	m_bRunning = true;

	return VOOSMP_ERR_None;
}
int voSubTitleRenderBase::Stop()
{
	if(m_hWnd)
		::KillTimer(m_hWnd, _TIMER_ID_SUBTITLE);
	int nAll = 0;
	while(m_bHandleSubtitleData && nAll<20)
	{
		Sleep(2);
		nAll++;
	}
	m_bRunning = false;

	return VOOSMP_ERR_None;
}

LRESULT voSubTitleRenderBase::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//voCAutoLock lock(m_pManager->GetMutexObject());
	if (uMsg == WM_LBUTTONUP) 
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);;
	}
	//WM_SUBTITLE_SHOW
	else if (uMsg == WM_SUBTITLE_SHOW) 
	{
		if(this->m_pManager)
			m_pManager->Show(((int)wParam)==0?false:true);
		return	1;//DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	else if (uMsg == WM_DESTROY) 
	{
		this->Stop();
		return	DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	else if (uMsg == WM_TIMER)
	{
		if((int)wParam == _TIMER_ID_SUBTITLE)
		{
			if(m_pManager)
			{
				if(m_pManager->GetListenerInfo()->pListener!=NULL)
				{
					m_bHandleSubtitleData = true;
					int nP1 = 0, nP2 = 0;
					m_pManager->GetListenerInfo()->pListener(m_pManager->GetListenerInfo()->pUserData , VOOSMP_PID_CLOSED_CAPTION_NOTIFY_EVENT , &nP1 , &nP2);
					m_bHandleSubtitleData = false;
				}
			}
		}
	
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return	DefWindowProc(hwnd, uMsg, wParam, lParam);
}
void voSubTitleRenderBase::UpdateBack (HDC dc, RECT rcDisplay, COLORREF clr, BYTE nTransparent)
{
	HBRUSH  bh = CreateSolidBrush(clr);
	FillRect(dc,&rcDisplay,bh);
	::DeleteObject(bh);
}
bool voSubTitleRenderBase::OnDrawToDC(HDC dc,RECT rcClient, bool bOnlyBitblt)
{
	if(!m_bShow)
		return false;
	if(m_pManager == NULL)
		return false;

	SetWidthHeight(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
	if(m_bForceRedrawAll)
	{
		if(m_bitmapBK)
		{
			//if(m_bitmapBK->GetWidth()!=m_width || m_bitmapBK->GetHeight()!=m_height)
				this->DeleteBitmapBK();
		}
	}
	if(m_bitmapBK == NULL && m_width!=0 && m_height!=0 )
	{
		m_bitmapBK = new CBaseBitmap();
		m_bitmapBK->CreateBitmap(dc,32,m_width,m_height);
	}
	if(m_bitmapBK == NULL)
		return false;

	//this->m_width = rcClient.right - rcClient.left;
	//this->m_height = rcClient.bottom - rcClient.top;
	HDC dc2 = NULL;
	if(dc == NULL)
		dc2 = ::GetDC(m_pManager->GetWindow());
	HDC hMemDC = ::CreateCompatibleDC(dc==NULL?dc2:dc);
	if(hMemDC == NULL)	{
		if(dc2 == NULL)
			dc2 = ::GetDC(m_pManager->GetWindow());
		hMemDC = ::CreateCompatibleDC(dc2);
	}

	//CBaseBitmap bitmap;
	//bitmap.CreateBitmap(dc,32,m_width,m_height);
	HBITMAP hBitmapDraw = m_bitmapBK->GetBitmapHandle(dc2==NULL?dc:dc2);//::CreateCompatibleBitmap(dc,m_width,m_height);

	HGDIOBJ hOldBitmapDraw = ::SelectObject(hMemDC,hBitmapDraw);
	HRGN combine_rgn = NULL;
	voSubTitleFormatSettingImpl* settings = m_pManager->GetSettings();

	bool bRecreateBitmap = false;

	TCHAR strText[2048];
	voSubtitleInfo* pInfo = m_pManager->GetSubtitleInfo();
	if(pInfo == NULL || !m_bShow)
		goto _RELEASE_END_LINE;
	//if(!::IsWindowVisible(m_pManager->GetWindow()))
	if(!m_bForceRedrawAll)
	{
		goto _RELEASE_END_LINE;
	}

	if(!bOnlyBitblt)
	{
	voCAutoLock lock(m_pManager->GetMutexObject());

	pvoSubtitleInfoEntry info = pInfo->pSubtitleEntry;
	voSubTitleRenderBase::UpdateBack (hMemDC, rcClient,m_clrBack);
	ResetRectNeedUpdate();
	while(info)
	{
		voSubtitleDisplayInfo& dispInfo = info->stSubtitleDispInfo;
		bool windowDraw = false;
		bRecreateBitmap = true;
		//to draw background
    	{
			voSubtitleRGBAColor color = info->stSubtitleRectInfo.stRectFillColor;//subtitleRectInfo.rectFillColor;
	    	if(settings != NULL)  
	    	{
				color = settings->converColor(color,settings->GetWindowBackgroundColor(),settings->GetWindowBackgroundColorEnable() );
        		color = settings->converColorOpacityRate(color, settings->GetBackgroundColorOpacityRate(), settings->GetWindowBackgroundColorOpacityRateEnable());
	    	}
			if(color.nTransparency!=0)
			{
				UpdateBack (hMemDC, rcClient, RGB(color.nRed, color.nGreen, color.nBlue));//,color.nTransparency
			}
    	}

		//to draw bitmap
		pvoSubtitleImageInfo imageInfo = dispInfo.pImageInfo;
		OnDrawImage( dc2==NULL?dc:dc2, hMemDC, imageInfo, combine_rgn);
		//draw row
		pvoSubtitleTextRowInfo rowInfo = dispInfo.pTextRowInfo;
		int rowInfoSize = 0;
		int nBottomPre = -1;//we regard draw from top to bottom. to define nBottomPre to avoid overlap
		while(rowInfo != NULL)
		{
			rowInfoSize++;
			if(rowInfo->pTextInfoEntry == NULL){
				rowInfo = rowInfo->pNext;
				continue;
			}

			int nLeft = toRealX(rowInfo->stTextRowDes.stDataBox.stRect.nLeft);
			int nRight =  nLeft+toRealX(rowInfo->stTextRowDes.stDataBox.stRect.nRight-rowInfo->stTextRowDes.stDataBox.stRect.nLeft, true,false);
			if(nRight<nLeft){
				int tmp = nLeft;
				nRight = nLeft;
				nLeft = tmp;
			}
			if(nRight>this->m_width)
			{
				int nW = nRight-nLeft;
				nRight = m_width;
				nLeft = nRight-nW;
				if(nLeft<0) nLeft = 0;
			}
			int nTop = toRealY(rowInfo->stTextRowDes.stDataBox.stRect.nTop);//+10
			int nBottom =  nTop+toRealY(rowInfo->stTextRowDes.stDataBox.stRect.nBottom-rowInfo->stTextRowDes.stDataBox.stRect.nTop, true,false);//+10nRight-nLeft,nBottom-nTop);//
			if(nBottom>this->m_height)
				nBottom = m_height;
		
			pvoSubtitleTextInfoEntry textInfo = rowInfo->pTextInfoEntry;

			HFONT font = NULL;

			int textInfoSize = 0;
			int nTextInfo = 1;
			if(textInfo)
				if(textInfo->pNext)
					nTextInfo++;
			while(textInfo != NULL)
			{
				textInfoSize++;
				::SetTextAlign(hMemDC,TA_LEFT);
				//to draw now
				{
					memset(strText,0,sizeof(strText));
					memcpy(strText,textInfo->pString,textInfo->nSize<4000?textInfo->nSize:4000);

			    	//TCHAR* strText = (TCHAR*)textInfo->pString;//stringText;
					bool bVertical = rowInfo->stTextRowDes.nPrintDirection>1?true:false;
					if(rowInfo->stTextRowDes.nPrintDirection == 1 || rowInfo->stTextRowDes.nPrintDirection == 3 )
						reverseString(strText);

					int nFntSize = setTextViewTextInfo(rowInfo, textInfo, hMemDC,NULL);

					HGDIOBJ hOld = NULL;//::SelectObject(hMemDC,m_font);
					if(m_font)
					hOld = ::SelectObject(hMemDC,m_font);

					int desc = 1;
					RECT rtBack;
					int nLen = _tcslen(strText);
			    	if(bVertical)
			    	{
						
						if(nFntSize<nRight -nLeft)
						{
							if(((nRight -nLeft) - nFntSize)>0)
								desc += ((nRight -nLeft) - nFntSize)/3;
						}
						
				    	rtBack.left = nLeft-3;
				    	int rightAdd = 2;
						if(textInfo->stStringInfo.stCharEffect.Italic!=0)
				    		rightAdd+=nFntSize/2;
						int width = (nRight -nLeft);
						rtBack.right = rtBack.left+width+rightAdd;//(int)textPaint.measureText(str)+rightAdd;
						for(int i_char = 0; i_char < nLen; i_char ++){
						    rtBack.top = nTop+width * (i_char);
						    rtBack.bottom = nTop+width * (i_char + 1);
							DrawString(hMemDC,strText+i_char,1,&rtBack);

							//::DrawText(hMemDC,strText+i_char,1,&rtBack,DT_LEFT|DT_VCENTER);
						}
						nTop+=width*nLen;
			    	}
			    	else
			    	{
						if(nFntSize<nBottom -nTop)
						{
							if(((nBottom -nTop) - nFntSize)>0)
								desc += ((nBottom -nTop) - nFntSize)/3;
						}
				    	bool bMoveRect = false;
				    	if(bMoveRect && !windowDraw && nLeft>6)
				    	{
				    		nLeft = nLeft*2/3;
				    	}
				    	int leftNow = nLeft;
				    	int nWidText = 0;
						//rowInfo->stTextRowDes.nHorizontalJustification = 2;
				    	TCHAR sUse2[2048];// = (TCHAR*)strText;
						memset(sUse2,0,sizeof(sUse2));
						if(_tcslen((TCHAR*)strText)<2000)
							_tcscpy(sUse2,(TCHAR*)strText);
						TCHAR* sUse = (TCHAR*)sUse2;

						bool bContinue = true;
						//_tcsstr(
						TCHAR* tmpOld = NULL;
						while(bContinue )
				    	{
							if(nTextInfo==1)
							{
								if(tmpOld)
									sUse = tmpOld+1;
								TCHAR* tmp = _tcsstr(sUse,_T("\n"));
								if(tmp == NULL)
									bContinue = false;
								else
								{
									tmpOld = tmp+1;
									tmp[0]=0;
								}
								
							}
							else
								bContinue = false;

							int nLenStrUse = _tcslen((TCHAR*)sUse);
							if(nLenStrUse<=0)
								break;
							SIZE sz;
							GetTextSize(hMemDC, sUse, nLenStrUse, &sz);
							//::GetTextExtentPoint(hMemDC, sUse, nLenStrUse, &sz);
							nWidText = sz.cx;//(int)textPaint.measureText(sUse);
				    		//to simply logic
							//rowInfo->stTextRowDes.nHorizontalJustification = 2;
							if(textInfoSize == 1 && textInfo->pNext == NULL)
					    	{
								if(rowInfo->stTextRowDes.nHorizontalJustification == 1)//right
					    			leftNow = nRight - nWidText-5;
								if(rowInfo->stTextRowDes.nHorizontalJustification == 2)//center
					    			leftNow = ((nRight+nLeft)/2 - nWidText/2)-5;
								if(leftNow<0)
									leftNow = 0;
								
								
					    	}
					    	rtBack.left = leftNow;
					    	int rightAdd = 0;

							if(textInfo->stStringInfo.stCharEffect.Italic!=0)
					    		rightAdd+=nFntSize/2;
					    	
							if(textInfoSize==1)
							{
								if(nBottomPre<nTop)
								{
					    			rtBack.top = nTop;
					    			rtBack.bottom = nBottom;
								}
								else
								{
					    			rtBack.top = nBottomPre;
					    			rtBack.bottom = rtBack.top + nBottom - nTop;
								}
							}
					    	rtBack.right = rtBack.left+nWidText+rightAdd;

							bool bDrawed = false;
							if(rowInfoSize == 1 && rowInfo->pNext == NULL
								&& textInfoSize == 1 && textInfo->pNext == NULL)
							{
								int nChar = _tcslen(sUse);
								if(nChar>32)
								{
									int nNowHigh = nChar/32;
									if(nChar%32>0)
										nNowHigh++;
								}
								//int nNowHigh = nChar/32+1;
								//nBottom = nTop+nNowHigh*(nBottom-nTop);
							}

							RECT rtBackDraw = rtBack;
							rtBackDraw.right+=6;
							rtBackDraw.bottom++;

							if(!bDrawed && nLen>0)
							{
								combine_rgn = CombineDrawedRgn(combine_rgn, rtBackDraw.left , rtBackDraw.top , rtBackDraw.right , rtBackDraw.bottom );
								//voSubTitleFormatSettingImpl* settings = m_pManager->GetSettings();
								COLORREF * clrBack = NULL;
								BYTE nAlpha = 0;
								if(settings != NULL && settings->GetBackgroundColorEnable())  
								{
									clrBack = new COLORREF;
									*clrBack = (settings->GetBackgroundColor());
									if(settings->GetBackgroundColorOpacityRateEnable())
										nAlpha = settings->GetBackgroundColorOpacityRate();
									else
										nAlpha = 255;
								}
								else
								{
									if(rowInfo->stTextRowDes.stDataBox.stRectFillColor.nTransparency >0){
										clrBack = new COLORREF;
										nAlpha = rowInfo->stTextRowDes.stDataBox.stRectFillColor.nTransparency;
										*clrBack = (RGB(rowInfo->stTextRowDes.stDataBox.stRectFillColor.nRed,
															rowInfo->stTextRowDes.stDataBox.stRectFillColor.nGreen,
															rowInfo->stTextRowDes.stDataBox.stRectFillColor.nBlue ));
									}
								}
								if(clrBack)
								{
									this->UpdateBack(hMemDC,rtBackDraw,*clrBack,nAlpha);
									delete clrBack;
								}
								DrawString(hMemDC,sUse,nLen,&rtBackDraw);
								//char ch[1024];
								//sprintf(ch,"l=%d, t=%d, r=%d, b=%d, string=%s, cy=%d\r\n",rtBack.left,rtBack.top,rtBack.right,rtBack.bottom,sUse,sz.cy);
								//::OutputDebugStringA(ch);
							}

							nBottomPre = rtBack.bottom;

							if(nTextInfo>1)
							{
								if(textInfo->pNext)
					    			nLeft+=nWidText;
							}
				    	}
					}

					if(m_font)
						::SelectObject(hMemDC,hOld);
				}
				//end
				textInfo = textInfo->pNext;
			}
			//end
			rowInfo = rowInfo->pNext;
		}

		//end
		info = info->pNext;

	}
	}
	m_bForceRedrawAll = false;
_RELEASE_END_LINE:
	if(m_bEnableCreatRegion)
	{
		if(!bOnlyBitblt)
		{
			if(m_hWnd)
			{
				HRGN rgn = NULL;
				::GetWindowRgn(m_hWnd,rgn);//::GetRe
				if(rgn)
					::DeleteObject(rgn);
			}
			if(combine_rgn == NULL)
				combine_rgn = CreateRectRgn( 0 , 0 , 0 , 0 );
			SetWindowRgn(m_hWnd,combine_rgn,TRUE);
		}
		//if(combine_rgn)
		//	DeleteObject( combine_rgn );
	}
	int end = 0;

	//if(bOnlyBitblt)
	BitbltToDC( dc2==NULL?dc:dc2, hMemDC, bRecreateBitmap);
	//::BitBlt(dc,0,0,this->m_width,this->m_height,hMemDC,0,0,SRCCOPY);
	::SelectObject(hMemDC,hOldBitmapDraw);
	::DeleteDC(hMemDC);

	if(dc2 != NULL)
		::ReleaseDC(m_pManager->GetWindow(), dc2);

	//if(m_bWin8)
	//{
	//	if(settings)
	//	{
	//		if(settings->GetBackgroundColorEnable())
	//		{
	//			::SetLayeredWindowAttributes(m_hWnd, m_clrBack, settings->GetBackgroundColorOpacityRate(),LWA_ALPHA);//LWA_COLORKEY//LWA_ALPHA|
	//		}
	//	}
	//}
	//::DeleteObject(hBitmapDraw);
	return true;
}
void voSubTitleRenderBase::GetTextSize(HDC hMemDC, TCHAR* sUse, int nLenStrUse,SIZE *sz)
{
#ifndef WINCE
	Gdiplus::Font font(hMemDC,m_font);
    //Gdiplus::GraphicsPath graphicsPathObj;
    //Gdiplus::FontFamily fontfamily;
    //font.GetFamily(&fontfamily);
	//Gdiplus::StringFormat strFormat;
	//strFormat.SetAlignment(Gdiplus::StringAlignmentNear);
 //   graphicsPathObj.AddString(sUse,-1,&fontfamily,font.GetStyle(),font.GetSize(),
 //                             Gdiplus::PointF(0,0),&strFormat);
 //   Gdiplus::Rect rcBound;
 //   graphicsPathObj.GetBounds(&rcBound);
	//sz->cx = int(rcBound.Width*1.15)+3;
	//if(nLenStrUse<=12)
	//	sz->cx+=6;
	//if(this->m_width>=1024)//full screen mode, or display size is large, we need adjust width to display whole string.
	//	sz->cx+=2*rcBound.Height;
	//sz->cy = rcBound.Height;

	Gdiplus::Graphics g(hMemDC);
	g.SetPageUnit(Gdiplus::Unit::UnitPixel);
	g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	Gdiplus::StringFormat strFormat;
	strFormat.SetAlignment(Gdiplus::StringAlignmentNear);
    Gdiplus::RectF rcBound;
	Gdiplus::PointF pt(0.0, 0.0);
	g.MeasureString(sUse,_tcslen(sUse),&font,pt,&rcBound);
	sz->cx = int(rcBound.Width);
	sz->cy = rcBound.Height;
    //Gdiplus::SizeF sizeF = g.MeasureString(sUse, font, 500, sf);
#else
	::GetTextExtentPoint(hMemDC, sUse, nLenStrUse, sz);
#endif
}
void  voSubTitleRenderBase::CheckRectNeedUpdate(RECT* rtIn)
{
	if(m_rectNeedUpdated == NULL)
		return;
	if(m_rectNeedUpdated->left == 0 && m_rectNeedUpdated->right == 0)
	{
		*m_rectNeedUpdated = *rtIn;
	}
	else
	{
		if(m_rectNeedUpdated->left > rtIn->left)
			m_rectNeedUpdated->left = rtIn->left;
		if(m_rectNeedUpdated->top > rtIn->top)
			m_rectNeedUpdated->top = rtIn->top;
		if(m_rectNeedUpdated->right < rtIn->right)
			m_rectNeedUpdated->right = rtIn->right;
		if(m_rectNeedUpdated->bottom < rtIn->bottom)
			m_rectNeedUpdated->bottom = rtIn->bottom;
	}
}

void  voSubTitleRenderBase::ResetRectNeedUpdate()
{
	if(m_rectNeedUpdated != NULL)
	{
		m_rectNeedUpdated->left = 0;
		m_rectNeedUpdated->right = 0;
		m_rectNeedUpdated->top = 0;
		m_rectNeedUpdated->bottom = 0;
	}
}

void voSubTitleRenderBase::BitbltToDC(HDC dc, HDC hMemDC, bool bRecreateBitmap)
{
	if(dc == NULL)
		return;
	int nLeft = 0;
	int nTop = 0;
	if(m_rectDisplay)
	{
		nLeft = m_rectDisplay->left;
		nTop = m_rectDisplay->top;
	}
	::BitBlt(dc,nLeft,nTop,this->m_width,this->m_height,hMemDC,0,0,SRCCOPY);
}
void voSubTitleRenderBase::DrawString(HDC hdc, TCHAR* lpchText, int cchText, LPRECT lprc)
{
#ifndef _WIN32_WCE
	Gdiplus::Graphics gs(hdc);
	Gdiplus::RectF rf((Gdiplus::REAL)lprc->left, (Gdiplus::REAL)(lprc->top+3), (Gdiplus::REAL)(lprc->right-lprc->left), (Gdiplus::REAL)(lprc->bottom - lprc->top));
	Gdiplus::StringFormat fmt;
	fmt.SetAlignment(Gdiplus::StringAlignmentNear);
	COLORREF nClr = ::GetTextColor(hdc);

	gs.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	gs.DrawString(lpchText, cchText,&Gdiplus::Font(hdc,m_font),rf,&fmt,&Gdiplus::SolidBrush(Gdiplus::Color(GetRValue(nClr),GetGValue(nClr),GetBValue(nClr))));

	CheckRectNeedUpdate(lprc);
#else
	::DrawText(hdc,lpchText,cchText,DT_LEFT|DT_VCENTER| DT_WORDBREAK );
#endif
}

bool voSubTitleRenderBase::OnDrawImage(HDC dc, HDC hMemDC, pvoSubtitleImageInfo imageInfo, HRGN& combine_rgn)
{
	while(imageInfo)
	{
		if( imageInfo->stImageData.pPicData )
		{
			//imageInfo.stImageDesp.
			if (imageInfo->stImageData.nType== VO_IMAGE_RGBA32 )//||
			//		imageInfo->stImageData.nType == VOOSMP_IMAGE_ARGB32
			{
				//to check top, bottom, left, right
				//
				int topMin = imageInfo->stImageData.nHeight;
				int leftMin = imageInfo->stImageData.nWidth;
				int rightMax = 0;
				int bottomMax = 0;//imageInfo->stImageData.nHeight-1;
				BYTE* pByteSrc = imageInfo->stImageData.pPicData;

					//HANDLE h = ::CreateFile(_T("D:\\dump_data.pict"),GENERIC_WRITE, FILE_SHARE_READ, 
					//	NULL, CREATE_ALWAYS, 
					//	FILE_ATTRIBUTE_NORMAL, NULL);
					//if(h!=INVALID_HANDLE_VALUE)
					//{
					//	int len = imageInfo->stImageData.nSize;
					//	DWORD nWrite = 0;
					//	::WriteFile(h,pByteSrc,len,&nWrite,NULL);
					//	::CloseHandle(h);
					//}

				int nRowLen = imageInfo->stImageData.nWidth*4;
				int iTransparent = 0;
				if(imageInfo->stImageData.nType== VO_IMAGE_RGBA32)
					iTransparent = 3;
				for(int i=0;i<(int)(imageInfo->stImageData.nHeight);i++)
				{
					int* pIntSrc = (int* )pByteSrc;
					for(int j = 0;j<(int)(imageInfo->stImageData.nWidth);j++)
					{
						BYTE* pB = (BYTE*)(pIntSrc+j);
						if(pB[iTransparent] != 0)
						{
							if(topMin>i)
								topMin = i;
							if(bottomMax<i)
								bottomMax = i;
							if(leftMin>j)
								leftMin = j;
							if(rightMax<j)
								rightMax = j;
						}
						else
						{
							if(imageInfo->stImageData.nType == VO_IMAGE_RGBA32)
							{
								//if(pB[iTransparent] != 0){
								//BYTE bTemp = pB[0];
								//pB[0] = pB[3];
								//pB[3] = pB[2];
								//pB[2] = pB[1];
								//pB[1] = bTemp;
								//}
								//else{
								pB[0] = 0;
								pB[3] = 0;
								pB[2] = 0;
								pB[1] = 0;
								//}
							}
						}
					}
					pByteSrc+=nRowLen;
				}

				//add edge
				if(leftMin>=1)
					leftMin--;
				if(topMin>=1)
					topMin--;
				if(bottomMax<(int)(imageInfo->stImageData.nHeight)-2)
					bottomMax++;
				if(rightMax<(int)(imageInfo->stImageData.nWidth)-2)
					rightMax++;

				int wid = imageInfo->stImageData.nWidth;
				int hgt = imageInfo->stImageData.nHeight;

				//copy bitmap data
				CBaseBitmap bitmapDraw;
				BYTE* pByte  = NULL;
				if(rightMax-leftMin>0 && bottomMax-topMin>0)
				{
					bitmapDraw.CreateBitmap(dc,32,rightMax-leftMin+1,bottomMax-topMin+1);
					int nWidByteBitmap = bitmapDraw.GetBmpDataWidth();
					pByte = bitmapDraw.GetBitmapBuffer();
					pByteSrc = imageInfo->stImageData.pPicData+leftMin*4+topMin*nRowLen;
					for(int i=topMin;i<bottomMax+1;i++)
					{
						if(pByte != NULL && pByteSrc != NULL)
							memcpy(pByte,pByteSrc,nWidByteBitmap);
						if(imageInfo->stImageData.nType== VO_IMAGE_RGBA32)
						{
							for(int jk = 0;jk<nWidByteBitmap; jk+=4){
								BYTE pb = pByte[jk];
								pByte[jk] = pByte[jk+2];
								pByte[jk+2] = pb;
							}
						}
						pByte+=nWidByteBitmap;
						pByteSrc+=nRowLen;
					}
				}
				else
				{
					bitmapDraw.CreateBitmap(dc,32,wid,hgt);
					int nWidByteBitmap = bitmapDraw.GetBmpDataWidth();
					pByte = bitmapDraw.GetBitmapBuffer();
					pByteSrc = imageInfo->stImageData.pPicData;
					for(int i=0;i<(int)imageInfo->stImageData.nHeight;i++)
					{
						if(pByte != NULL && pByteSrc != NULL)
							memcpy(pByte,pByteSrc,nRowLen);
						pByte+=nWidByteBitmap;
						pByteSrc+=nRowLen;
					}
				}


				//COMPUTE DISPLAY POSITION
				int nLeft1 = imageInfo->stImageDesp.stImageRectInfo.stRect.nLeft;
				int nRight1 = imageInfo->stImageDesp.stImageRectInfo.stRect.nRight;
				int nTop1 = imageInfo->stImageDesp.stImageRectInfo.stRect.nTop;
				int nBottom1 = imageInfo->stImageDesp.stImageRectInfo.stRect.nBottom;
				float fRate = 0.0f;
				if(hgt>0 && wid>0)
				{
					fRate = (bottomMax-topMin)/(float)hgt;
					int real = (int)(fRate*(nBottom1-nTop1));
					fRate = (topMin)/(float)hgt;
					int real2 = (int)((nBottom1-nTop1)*fRate);//*(nTop1);
					nTop1 = nTop1+real2;//((nBottom1-nTop1)-real)/2;
					nBottom1 = nTop1+real;
					fRate = (rightMax-leftMin)/(float)wid;
					real = (int)(fRate*(nRight1-nLeft1));
					fRate = (leftMin)/(float)wid;
					real2 = (int)(fRate*(nRight1-nLeft1));
					nLeft1 += real2;//nLeft1+((nRight1-nLeft1)-real)/2;
					nRight1 = nLeft1+real;
				}

				int nLeft = toRealX(nLeft1);
				int nRight = toRealX(nRight1);
				int nTop = toRealY(nTop1);
				int nBottom = toRealY(nBottom1);

				combine_rgn = CombineDrawedRgn(combine_rgn,nLeft , nTop ,nRight , nBottom );

				HDC hMemDCImage = ::CreateCompatibleDC(dc);
				HGDIOBJ hOldBitmap2 = ::SelectObject(hMemDCImage, bitmapDraw.GetBitmapHandle(dc));
				CImage img;
				img.Create(bitmapDraw.GetWidth(),bitmapDraw.GetHeight(),32,CImage::createAlphaChannel);
				BYTE* bt = (BYTE*)img.GetBits();
				int nPitch = img.GetPitch();
				{
					BYTE * pByte = bitmapDraw.GetBitmapBuffer();
					for(int l = 0;l<bitmapDraw.GetHeight();l++)
					{
						BYTE* bt2 = (BYTE*)img.GetPixelAddress(0,l);
						memcpy(bt2,pByte,bitmapDraw.GetBmpDataWidth());
						pByte+=bitmapDraw.GetBmpDataWidth();
					}
				}

				//fmt.
				
				//memcpy(bitmapDraw.GetBitmapBuffer(),bt,bitmapDraw.GetBmpDataWidth()*bitmapDraw.GetHeight());
				img.Draw(hMemDC, nLeft, nTop, nRight-nLeft, nBottom - nTop,0,0,bitmapDraw.GetWidth(),bitmapDraw.GetHeight());
				//::StretchBlt(hMemDC, nLeft, nTop, nRight-nLeft, nBottom - nTop, hMemDCImage,0,0,
				//	bitmapDraw.GetWidth(),bitmapDraw.GetHeight(), SRCCOPY );
				::SelectObject(hMemDCImage, hOldBitmap2);
				::DeleteDC(hMemDCImage);
				RECT rIn;
				rIn.left = nLeft;
				rIn.right = nRight;
				rIn.top = nTop;
				rIn.bottom = nBottom;
				CheckRectNeedUpdate(&rIn);
				//xx
			}
#ifdef _ENABLE_PICTURE_SUBTITLE
			else//draw general picture
			{
				if(imageInfo->stImageData.nSize>0)
				{
					//HANDLE h = ::CreateFile(_T("d:\\seek.png"),GENERIC_READ, FILE_SHARE_READ, NULL,
					//	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					//if(h!=INVALID_HANDLE_VALUE)
					//{
					//	DWORD len = 0;
					//	len = ::GetFileSize(h,NULL);
					//	if(imageInfo->stImageData.nSize>=len)
					//	{
					//		imageInfo->stImageData.nSize = len;
					//		DWORD nRead = 0;
					//		::ReadFile(h,imageInfo->stImageData.pPicData,len,&nRead,NULL);
					//		nRead = nRead;
					//	}
					//	::CloseHandle(h);
					//}
					//::CoInitialize(NULL);
					HGLOBAL hGlobal  =  GlobalAlloc(GMEM_MOVEABLE, imageInfo->stImageData.nSize); 
					void *  pData  =  GlobalLock(hGlobal); 
					memcpy(pData, imageInfo->stImageData.pPicData, imageInfo->stImageData.nSize); 
					GlobalUnlock(hGlobal);
					IStream *  pStream  =  NULL; 
					IPicture* pPicture = NULL;
					CImage img;
					//img.Load(_T("d:\\seek.png"));
					bool bOk = false;
					if  (CreateStreamOnHGlobal(hGlobal, TRUE,  & pStream)  ==  S_OK) 
					{ 
						//HRESULT hrn = OleLoadPicture( pStream, imageInfo->stImageData.nSize, TRUE, IID_IPicture, ( LPVOID * )&pPicture );
						// if ( hrn ==S_OK ) 
						//	 bOk = true;

						 if (SUCCEEDED(img.Load(pStream)))
						 {
							 bOk = true;
							 //if(img.GetBits()!=NULL)
							 //memset(img.GetBits(), 255, img.GetHeight()*img.GetWidth());
						 }
					} 
					if(bOk && img.GetWidth() && img.GetHeight())
					{
						int nLeft1 = imageInfo->stImageDesp.stImageRectInfo.stRect.nLeft;
						int nRight1 = imageInfo->stImageDesp.stImageRectInfo.stRect.nRight;
						int nTop1 = imageInfo->stImageDesp.stImageRectInfo.stRect.nTop;
						int nBottom1 = imageInfo->stImageDesp.stImageRectInfo.stRect.nBottom;
						if(nRight1<=0 || nBottom1<=0)
						{
							nLeft1 = 0;
							nRight1 = 100;
							nTop1 = 0;
							nBottom1 = 100;
							TransferRectByAspect(nLeft1, nTop1, nRight1, nBottom1,img.GetWidth()/(float)img.GetHeight());
						}
						else{
							TransferRectByAspect(nLeft1, nTop1, nRight1, nBottom1,img.GetWidth()/(float)img.GetHeight());
						}

						int nLeft = toRealX(nLeft1);
						int nRight = toRealX(nRight1);
						int nTop = toRealY(nTop1);
						int nBottom = toRealY(nBottom1);
						//img.Save(_T("d:\\1.jpg"));
						img.Draw(hMemDC, nLeft, nTop, nRight-nLeft, nBottom - nTop,0,0,img.GetWidth(),img.GetHeight());
						//if(pPicture)
						//{
						//	pPicture->Render(hMemDC,nLeft, nTop, nRight-nLeft, nBottom - nTop,0,0,img.GetWidth(),img.GetHeight(),NULL);
						//	pPicture->Release();
						//}
						RECT rIn;
						rIn.left = nLeft;
						rIn.right = nRight;
						rIn.top = nTop;
						rIn.bottom = nBottom;
						//this->UpdateBack(hMemDC,rIn,RGB(255,0,0),255);
						CheckRectNeedUpdate(&rIn);

					}
					if(pStream)
						pStream -> Release();     

					GlobalFree(hGlobal); 
				}
			}
#endif
		}

		//end
		imageInfo = imageInfo->pNext;
	}
	return true;
}
void voSubTitleRenderBase::TransferRectByAspect(int& leftTarget, int& topTarget, int& rightTarget, int& bottomTarget, float fXYRate)
{
	int nWidth1 = rightTarget-leftTarget;
	int nHeight1 = bottomTarget-topTarget;
	float f = nWidth1/(float)nHeight1;
	if(f>fXYRate)
	{
		nWidth1 = (int) (nHeight1*fXYRate);
	}
	else
	{
		nHeight1 = (int)(nWidth1/fXYRate);
	}
	leftTarget += (( rightTarget-leftTarget)-nWidth1)/2;
	rightTarget = leftTarget+nWidth1;
	topTarget += (( bottomTarget-topTarget)-nHeight1)/2;
	bottomTarget = topTarget+nHeight1;
}

HRGN voSubTitleRenderBase::CombineDrawedRgn(HRGN hSrc, int x, int y, int r, int b)
{
	if(m_bEnableCreatRegion)
	{
		int nLeft = 0;
		int nTop = 0;
		if(m_rectDisplay)
		{
			nLeft = m_rectDisplay->left;
			nTop = m_rectDisplay->top;
		}
		if(hSrc ==NULL)
		{
			hSrc = CreateRectRgn( nLeft+x , nTop+y , nLeft+r , nTop+b);
		}
		else
		{
			HRGN tmp_rgn = CreateRectRgn( nLeft+x , nTop+y , nLeft+r , nTop+b );
			HRGN combine_rgn1 = CreateRectRgn( 0 , 0 , 1 , 1 );
			CombineRgn( combine_rgn1 , hSrc , tmp_rgn , RGN_OR );
			DeleteObject( hSrc );
			hSrc = combine_rgn1;
			DeleteObject( tmp_rgn );
		}
	}
	return hSrc;
}

void voSubTitleRenderBase::ComputeValidRectOfXYRatio(bool bRedraw)
{
	if(m_pManager==NULL)
		return;
	float xyRate = m_pManager->GetXyRate();
	

	int widthmax = m_width;
	int heightmax = m_height;
	if(m_width<=0 || m_height<0 )
		return;

	if(m_width>(int)(m_height*xyRate))
	{
		int width = (int)(m_height*xyRate);
		m_rectVaild.left = (widthmax-width)/2;
		m_rectVaild.right = m_rectVaild.left+width;
		m_rectVaild.top=0;
		m_rectVaild.bottom=m_height;
	}
	else
	{
		int height = (int)(m_width/xyRate);
		m_rectVaild.top=(heightmax-height)/2;
		m_rectVaild.left = 0;
		m_rectVaild.right = m_rectVaild.left+m_width;
		m_rectVaild.bottom=m_rectVaild.top+height;
	}

	if(bRedraw){
		this->Update(true);
	}
}
void voSubTitleRenderBase::DeleteBitmapBK()
{
	if(m_bitmapBK)
		delete m_bitmapBK;
	m_bitmapBK = NULL;
}
void voSubTitleRenderBase::SetWidthHeight(int width, int height)
{
	bool bSetWidth = false;
	if(height!=m_height || m_width!=width)
	{
		m_width = width;
		m_height = height;
		ComputeValidRectOfXYRatio(false);
		bSetWidth = true;
	}
	if(bSetWidth)
	{
		DeleteBitmapBK();
	}
}