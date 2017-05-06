/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#include "stdafx.h"
#include <CommDlg.h>
#include <commctrl.h>
#include "voOnStreamMPTest.h"
#include "CvoOnStreamMP.h"
#include "voSource2.h"
#include "CViewWindow.h"
#include "vompType.h"

#define MAX_LOADSTRING 100
#define IDC_SLIDER_POS		2003
#define IDC_TEXT_INPUTURL		3003

#define IDC_SELECT_VIDEO		5003
#define IDC_SELECT_AUDIO		5004
#define IDC_SELECT_SUBTITLE		5005

#define IDC_EDIT_URL1			6003
#define IDC_EDIT_URL2			6004
#define IDC_EDIT_URL3			6005

#define IDC_BUTTON_PLAYURL1		6103
#define IDC_BUTTON_PLAYURL2		6104
#define IDC_BUTTON_PLAYURL3		6105

#define	WM_TIMER_POS			101
#define WM_TIMER_PLAYTIME		102
#define IDC_STATIC_SHOWTIME		2004

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HWND        g_hWndShowTime = NULL; // static text to show play time info
HWND				g_hWndSldPos = NULL;		// slider posision handle
HWND				g_hInputURL = NULL;		//input url handle;
CvoOnStreamMP		*mpPlayer = NULL;
HWND				mhWnd = NULL;
TCHAR				g_szFile[1024*2];
long long			gDuration = 0;

TCHAR				g_szFile1[1024*2];
TCHAR				g_szFile2[1024*2];
TCHAR				g_szFile3[1024*2];

#ifdef SHELL_AYGSHELL
HWND				g_hWndMenuBar = NULL;		// menu bar handle
#else // SHELL_AYGSHELL
HWND				g_hWndCommandBar = NULL;	// command bar handle
#endif // SHELL_AYGSHELL
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	InputURL(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	TrackSelect(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ChannelSwitch(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	OptionSetting(HWND, UINT, WPARAM, LPARAM);

int     m_nSelAudioIndex;
int     m_nSelVedioIndex;
int     m_nSelSubtitleIndex;
void	PlayURL(TCHAR* pUrl);
void	InitTrackSelInfo();
void	OnShowTrackInfo(HWND hDlg);
void	OnShowMenuTrackInfo(VOOSMP_SOURCE_STREAMTYPE trackType);
int		GetMenuShowTextByIndex(OUT LPSTR szMenuText, IN VOOSMP_SOURCE_STREAMTYPE sType, IN int nIndex);

int		OnListener (void * pUserData, int nID, void * pParam1, void * pParam2);
#define	WM_USER_PLAYCOMPLETE WM_USER+101

void OnShowUrlSelect(HWND hDlg);

CViewWindow gViewVindow;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_VOONSTREAMMPTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VOONSTREAMMPTEST));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
#ifndef WINCE
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VOONSTREAMMPTEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_VOONSTREAMMPTEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
#else //WINCE
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VOONSTREAMMPTEST));
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
#endif //WINCE
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE | WS_THICKFRAME,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   SetWindowPos (hWnd, NULL, 640, 0, 800, 600+100, 0);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   RECT rcView;
   GetClientRect (hWnd, &rcView);

   rcView.bottom = rcView.bottom - 24;
   rcView.bottom = rcView.bottom / 2 * 2;

	SetRect (&rcView, rcView.left, rcView.bottom, rcView.right, rcView.bottom + 50);
	g_hWndSldPos = CreateWindow (_T("msctls_trackbar32"), _T(""), WS_VISIBLE | WS_CHILD | TBS_BOTH | TBS_NOTICKS, 
							rcView.left+100, rcView.top, rcView.right - rcView.left-100, rcView.bottom - rcView.top,
							hWnd, (HMENU)IDC_SLIDER_POS, hInstance, NULL);

  g_hWndShowTime = CreateWindow (_T("Static"), _T(" 0:0:0/0:0:0"), WS_VISIBLE | WS_CHILD, 
      rcView.left, rcView.top, 100, rcView.bottom - rcView.top,
      hWnd, (HMENU)IDC_STATIC_SHOWTIME, hInstance, NULL);


  _tcscpy(g_szFile1, _T("http://qatest.visualon.com:8082/hls/Customers/Ericsson/multi_audio/index_live.m3u8"));
  _tcscpy(g_szFile2, _T("http://qatest.visualon.com:8082/hls/Customers/Ericsson/xaa/index.m3u8"));
  _tcscpy(g_szFile3, _T("http://hls-iis.visualon.com:8082/hls/v8/bipbop_16x9_variant1.m3u8"));

#ifdef WINCE	
#ifndef SHELL_AYGSHELL
   if (g_hWndCommandBar)
   {
	   CommandBar_Show(g_hWndCommandBar, TRUE);
   }
#endif // !SHELL_AYGSHELL
#endif //WINCE

   mhWnd = hWnd;

   GetClientRect (hWnd, &rcView);

   rcView.bottom = rcView.bottom - 24;
   rcView.bottom = rcView.bottom / 2 * 2;

   gViewVindow.CreateWnd(mhWnd, rcView);

   return TRUE;
}

void	PlayURL(TCHAR* pUrl)
{
	if(pUrl == NULL)
		return ;

	if(mpPlayer == NULL)
	{
		TCHAR	szPath[1024];
		memset(szPath, 0, 1024*sizeof(TCHAR));
		GetModuleFileName (NULL, szPath, sizeof (szPath));

		TCHAR * pPos = _tcsrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;
		}

		mpPlayer = new CvoOnStreamMP(szPath);
		if(mpPlayer == NULL)
			return;
	}
	else
	{
		if(mhWnd != NULL)
		{
			KillTimer(mhWnd , WM_TIMER_PLAYTIME);
			KillTimer(mhWnd , WM_TIMER_POS);
		}
		
		mpPlayer->Stop();
		mpPlayer->Close();
		mpPlayer->Uninit();
	}

	mpPlayer->SetListener (NULL, OnListener);

	int nRC = mpPlayer->Init();
	if(nRC < 0)
	{
		MessageBox(mhWnd , _T("Init player failed !") , _T("Error") , MB_OK);
	}

	mpPlayer->SetView(gViewVindow.GetWnd());
	//mpPlayer->SetView(mhWnd);

	nRC = mpPlayer->Open((void *)pUrl , VOOSMP_FLAG_SOURCE_URL , VOOSMP_SRC_AUTO_DETECT);
	if(nRC < 0)
	{
		MessageBox(mhWnd , _T("Open file failed !") , _T("Error") , MB_OK);
	}
	int val = VOOSMP_RENDER_TYPE_GDI;
	mpPlayer->SetParam(VOOSMP_PID_VIDEO_RENDER_TYPE,&val);
	val = 1;
	mpPlayer->SetParam(VOOSMP_PID_COMMON_CCPARSER,&val);
	//TCHAR ch[1024];
	//_tcscpy(ch,_T("http://dozeu380nojz8.cloudfront.net/uploads/video/subtitle_file/271/dfxp_4738496760_aitraaz.xml"));
	//mpPlayer->SetParam(VOOSMP_PID_SUBTITLE_FILE_NAME,ch);
	//voSubTitleFormatSetting* sett = NULL;
	//(voSubTitleFormatSetting*)mpPlayer->GetParam(VOOSMP_PID_CLOSED_CAPTION_SETTINGS, &sett);
	//if(sett!=NULL)
	//{
	//	sett->setBackgroundColor(RGB(0,120,0));
	//	sett->setFontColor(RGB(120,120,0));
	//}

	gDuration = mpPlayer->GetDuration ();
	SendMessage (g_hWndSldPos, TBM_SETRANGE, TRUE, MAKELONG (0, gDuration / 1000));

	SetTimer(mhWnd,WM_TIMER_PLAYTIME,500,NULL);
	SetTimer(mhWnd,WM_TIMER_POS,500,NULL);

	mpPlayer->Run();

	ShowWindow(g_hWndSldPos,SW_SHOW);
}

void OpenMediaFile (HWND hWnd)
{
	DWORD				dwID = 0;
	OPENFILENAME		ofn;
	memset (g_szFile, 0, 1024*2*sizeof(TCHAR));

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hWnd;

	ofn.lpstrFilter = TEXT("Media File (*.*)\0*.*\0");	
	ofn.lpstrFile = g_szFile;
	ofn.nMaxFile = MAX_PATH;

	ofn.lpstrTitle = TEXT("Open Media File");
	ofn.Flags = OFN_EXPLORER;

	if(mpPlayer != NULL && mhWnd != NULL)
	{
		KillTimer(mhWnd,WM_TIMER_PLAYTIME);
		KillTimer(mhWnd,WM_TIMER_POS);
		mpPlayer->Stop();
	}

	if (GetOpenFileName(&ofn))	
	{
		PlayURL(g_szFile);
	}
}

void GetShowPlayTimeText(IN OUT LPTSTR szText, IN CvoOnStreamMP* gPlayer)
{
    if (NULL==szText || NULL==gPlayer)
    {
        return;
    }

    //whole play time
    static TCHAR szWholeTime[MAX_PATH] = _T("");
    static int nLastDuration = 0;
    int nDuration = gPlayer->GetDuration ();
    if (nDuration != nLastDuration)
    {
        _tcscpy(szWholeTime,_T(""));
        nLastDuration = nDuration;

        int nHourWhole = nDuration/1000/60/60;
        int nMinute = nDuration/1000/60%60;
        int nSecond = nDuration/1000%60;

        TCHAR szTemp[10] = _T("");
        _itot(nHourWhole,szTemp,10);
        _tcscat(szWholeTime,szTemp);
        _tcscat(szWholeTime,_T(":"));
        _itot(nMinute,szTemp,10);
        _tcscat(szWholeTime,szTemp);
        _tcscat(szWholeTime,_T(":"));
        _itot(nSecond,szTemp,10);
        _tcscat(szWholeTime,szTemp);
    }

    //cur play time
    TCHAR szCurTime[MAX_PATH] = _T("");
    int nCurTime = gPlayer->GetPos();
    if (nCurTime>nDuration)
    {
        nCurTime = nDuration;
    }
    int nHourWhole = nCurTime/1000/60/60;
    int nMinute = nCurTime/1000/60%60;
    int nSecond = nCurTime/1000%60;

    TCHAR szTemp[10] = _T("");
    _itot(nHourWhole,szTemp,10);
    _tcscat(szCurTime,szTemp);
    _tcscat(szCurTime,_T(":"));
    _itot(nMinute,szTemp,10);
    _tcscat(szCurTime,szTemp);
    _tcscat(szCurTime,_T(":"));
    _itot(nSecond,szTemp,10);
    _tcscat(szCurTime,szTemp);

    _tcscpy(szText,_T(""));
    _tcscat(szText,szCurTime);
    _tcscat(szText,_T("/"));
    _tcscat(szText,szWholeTime);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		if (mpPlayer && wmId >= ID_MENU_SEL_AUDIO && wmId<(ID_MENU_SEL_AUDIO + mpPlayer->GetAudioCount()))
		{
			m_nSelAudioIndex = wmId - ID_MENU_SEL_AUDIO;
			if (mpPlayer)
			{
				mpPlayer->SelectAudio(m_nSelAudioIndex);
			}
			break;
		}
		else if (mpPlayer && wmId >= ID_MENU_SEL_VIDEO && wmId<=(ID_MENU_SEL_VIDEO+mpPlayer->GetVideoCount()))
		{
			m_nSelVedioIndex = wmId - ID_MENU_SEL_VIDEO;
			if (m_nSelVedioIndex == mpPlayer->GetVideoCount())
			{
				m_nSelVedioIndex = -1;
			}
			if (mpPlayer)
			{
				mpPlayer->SelectVideo(m_nSelAudioIndex);
			}
			break;
		}
		else if (mpPlayer && wmId >= ID_MENU_SEL_SUBTITLE && wmId<(ID_MENU_SEL_SUBTITLE+mpPlayer->GetSubtitleCount()))
		{
			m_nSelSubtitleIndex = wmId - ID_MENU_SEL_SUBTITLE;
			if (mpPlayer)
			{
				mpPlayer->SelectSubtitle(m_nSelSubtitleIndex);
			}
			break;
		}

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FILE_OPENFILE:
			OpenMediaFile(hWnd);
			break;
		case ID_FILE_OPENURL:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_INPUT), hWnd, InputURL);
			break;
		case ID_PLAY_PAUSE:
			{
				if(mpPlayer != NULL)
				{
					mpPlayer->Pause();
				}
			}
			break;
		case ID_PLAY_RUN:
			{
				if(mpPlayer != NULL)
				{
					SetTimer(hWnd,WM_TIMER_PLAYTIME,500,NULL);
					SetTimer(hWnd,WM_TIMER_POS,500,NULL);
					mpPlayer->Run();
				}
			}
			break;
		case ID_PLAY_STOP:
			{
				if(mpPlayer != NULL)
				{
					KillTimer(hWnd,WM_TIMER_PLAYTIME);
					KillTimer(hWnd,WM_TIMER_POS);
					mpPlayer->Stop();
				}
			}
			break;
		case ID_TOOLS_SELECTTRACK:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SELECTED), hWnd, TrackSelect);
			break;
		case ID_TOOLS_CHANNELSWITCH:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SWITCHCHANNEL), hWnd, ChannelSwitch);
			break;
		case ID_TOOLS_OPTIONS:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_OPTIONS), hWnd, OptionSetting);
			break;
		case ID_TRACKSELECT_VEDIO:
			//OnShowMenuTrackInfo(VOOSMP_SS_VIDEO);
			break;
		case ID_TRACKSELECT_AUDIO:
			//OnShowMenuTrackInfo(VOOSMP_SS_AUDIO);
			break;
		case ID_TRACKSELECT_SUBTITLE:
			//OnShowMenuTrackInfo(VOOSMP_SS_SUBTITLE);
			break;
		case ID_TRACKSELECT_CLEARSELECTION:
			//if (mpPlayer)
			//{
			//	mpPlayer->ClearSelection();
			//}
			break;
		case ID_TRACKSELECT_COMMITSELECTION:
			//if (mpPlayer)
			//{
			//	mpPlayer->CommitSelection();
			//}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			if(mpPlayer != NULL)
			{
				mpPlayer->SetView(mhWnd);
			}

			RECT rcClient;
			GetClientRect(hWnd, &rcClient);

			HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
			FillRect(GetDC(hWnd), &rcClient, hBrush);
			DeleteObject(hBrush);

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_TIMER:
		{
			if (NULL == mpPlayer)
			{
				break;
			}

			VOOSMP_STATUS nStatus = VOOSMP_STATUS_MAX;
			int nRC = mpPlayer->GetParam(VOOSMP_PID_STATUS,&nStatus);
			if(nStatus != VOOSMP_STATUS_RUNNING)
				break;

			if (wParam == WM_TIMER_POS)
			{
				int nPos = 0;
				if(mpPlayer != NULL)
				{
					nPos = mpPlayer->GetPos ();

					int nDuration = mpPlayer->GetDuration ();
					if(nDuration != gDuration)
					{
						SendMessage (g_hWndSldPos, TBM_SETRANGE, TRUE, MAKELONG (0, nDuration / 1000));
					}
				}
				//				VOLOGI ("Pos is %d", nPos);
				SendMessage (g_hWndSldPos, TBM_SETPOS, TRUE, nPos / 1000);
			}
			else if (wParam == WM_TIMER_PLAYTIME)
			{
				TCHAR szText[MAX_PATH] = _T("");
				GetShowPlayTimeText(szText,mpPlayer);
				SetWindowText(g_hWndShowTime,szText);
			}
		}
		break;
	case WM_CREATE:
#ifdef WINCE
#ifndef SHELL_AYGSHELL
		g_hWndCommandBar = CommandBar_Create(hInst, hWnd, 1);
		CommandBar_InsertMenubar(g_hWndCommandBar, hInst, IDC_VOONSTREAMMPTEST, 0);
		CommandBar_AddAdornments(g_hWndCommandBar, 0, 0);
#endif // !SHELL_AYGSHELL
#ifdef SHELL_AYGSHELL
		SHMENUBARINFO mbi;

		memset(&mbi, 0, sizeof(SHMENUBARINFO));
		mbi.cbSize     = sizeof(SHMENUBARINFO);
		mbi.hwndParent = hWnd;
		mbi.nToolBarId = IDR_MENU;
		mbi.hInstRes   = g_hInst;

		if (!SHCreateMenuBar(&mbi)) 
		{
			g_hWndMenuBar = NULL;
		}
		else
		{
			g_hWndMenuBar = mbi.hwndMB;
		}

		// Initialize the shell activate info structure
		memset(&s_sai, 0, sizeof (s_sai));
		s_sai.cbSize = sizeof (s_sai);
#endif // SHELL_AYGSHELL
#endif //WINCE
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		if(mpPlayer != NULL)
		{
			mpPlayer->Stop();
			mpPlayer->Close();
			mpPlayer->Uninit();
			delete mpPlayer;
			mpPlayer = NULL;
		}

		break;
	case WM_HSCROLL:
		{
			int nPos = SendMessage (g_hWndSldPos, TBM_GETPOS, 0, 0) * 1000;
			if(mpPlayer != NULL)
				mpPlayer->SetPos (nPos);
		}
		break;
	case WM_SIZE:
		{
			RECT rcView;
			GetClientRect (hWnd, &rcView);

			rcView.bottom = rcView.bottom - 24;
			rcView.bottom = rcView.bottom / 2 * 2;

			SetRect (&rcView, rcView.left, rcView.bottom, rcView.right, rcView.bottom + 50);

			if(g_hWndSldPos != NULL)
				SetWindowPos (g_hWndSldPos, NULL, rcView.left+100, rcView.top, rcView.right - rcView.left-100, rcView.bottom - rcView.top, 0);

			if(g_hWndShowTime != NULL)
				SetWindowPos (g_hWndShowTime, NULL, rcView.left, rcView.top, 100, rcView.bottom - rcView.top, 0);

			GetClientRect (hWnd, &rcView);

			rcView.bottom = rcView.bottom - 24;
			rcView.bottom = rcView.bottom / 2 * 2;

			SetWindowPos (gViewVindow.GetWnd(), NULL, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);

			if(mpPlayer != NULL)
			{
				mpPlayer->SetView(gViewVindow.GetWnd());
			}
		}
		break;

	case WM_USER_PLAYCOMPLETE:
		if(mpPlayer != NULL)
		{
			mpPlayer->Stop();
			mpPlayer->Close();
			mpPlayer->Uninit();
			delete mpPlayer;
			mpPlayer = NULL;
		}

		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int	OnListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
	PostMessage (mhWnd, WM_USER_PLAYCOMPLETE, 0, 0);

	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for select box.
INT_PTR CALLBACK TrackSelect(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		OnShowTrackInfo(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			if(mpPlayer)
			{
				VOOSMP_SRC_CURR_TRACK_INDEX curIndex;
				mpPlayer->GetCurrTrackSelection(&curIndex);
				int nRt = mpPlayer->CommitSelection();
				if(nRt == 0)
				{
					if (m_nSelAudioIndex != curIndex.nCurrAudioIdx || m_nSelSubtitleIndex != curIndex.nCurrSubtitleIdx)
					{
						int lCurPos = mpPlayer->GetPos();
						mpPlayer->SetPos(lCurPos);
					}

					EndDialog(hDlg, LOWORD(wParam));
				}
			}
			else
			{
				EndDialog(hDlg, LOWORD(wParam));
			}

			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDCANCEL)
		{
			if(mpPlayer)
			{
				mpPlayer->ClearSelection();
			}

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDC_SELECT_VIDEO)
		{
			if ( HIWORD(wParam) == CBN_SELCHANGE) 
			{
				int selection = (int)SendMessage(GetDlgItem(hDlg,IDC_SELECT_VIDEO), CB_GETCURSEL, 0, 0);

				if ( selection != CB_ERR ) 
				{
					if(mpPlayer)
					{
						if(mpPlayer->IsVideoAvailable(selection - 1))
						{
							m_nSelVedioIndex = selection - 1;
							mpPlayer->SelectVideo(m_nSelVedioIndex);
						}
						else
						{
							if(mpPlayer->GetVideoCount() > 0)
							{
								SendMessage(GetDlgItem(hDlg,IDC_SELECT_VIDEO), CB_SETCURSEL, (WPARAM)(m_nSelVedioIndex+1), (LPARAM)0);
							}
						}
					}
				}
			}

			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDC_SELECT_AUDIO)
		{
			if ( HIWORD(wParam) == CBN_SELCHANGE) 
			{
				int selection = (int)SendMessage(GetDlgItem(hDlg,IDC_SELECT_AUDIO), CB_GETCURSEL, 0, 0);

				if ( selection != CB_ERR ) 
				{
					if(mpPlayer)
					{
						if(mpPlayer->IsAudioAvailable(selection))
						{
							m_nSelAudioIndex = selection;
							mpPlayer->SelectAudio(m_nSelAudioIndex);
						}
						else
						{
							if(mpPlayer->GetAudioCount() > 0)
							{
								SendMessage(GetDlgItem(hDlg,IDC_SELECT_AUDIO), CB_SETCURSEL, (WPARAM)(m_nSelAudioIndex), (LPARAM)0);
							}
						}
					}
				}
			}

			return (INT_PTR)TRUE;
		}

		if (LOWORD(wParam) == IDC_SELECT_SUBTITLE)
		{
			if ( HIWORD(wParam) == CBN_SELCHANGE) 
			{
				int selection = (int)SendMessage(GetDlgItem(hDlg,IDC_SELECT_SUBTITLE), CB_GETCURSEL, 0, 0);

				if ( selection != CB_ERR ) 
				{
					if(mpPlayer)
					{
						if(mpPlayer->IsSubtitleAvailable(selection))
						{
							m_nSelSubtitleIndex = selection;
							mpPlayer->SelectSubtitle(m_nSelSubtitleIndex);
						}
						else
						{
							if(mpPlayer->GetSubtitleCount() > 0)
							{
								SendMessage(GetDlgItem(hDlg,IDC_SELECT_SUBTITLE), CB_SETCURSEL, (WPARAM)(m_nSelSubtitleIndex), (LPARAM)0);
							}
						}
					}
				}
			}

			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void OnShowUrlSelect(HWND hDlg)
{
	SetWindowPos (hDlg, NULL, 640, 100, 640, 320, 0);

	RECT rcView;
	RECT rcViewUrl;
	GetClientRect (hDlg, &rcView);

	HWND hEdit = NULL;

	SetRect (&rcViewUrl, rcView.left+10, rcView.top + 20, rcView.right - 120, rcView.top + 80);

	hEdit = CreateWindow (_T("Edit"), _T(""), WS_VISIBLE | ES_MULTILINE | WS_CHILD | TBS_BOTH | TBS_NOTICKS, 
		rcViewUrl.left, rcViewUrl.top, rcViewUrl.right - rcViewUrl.left, rcViewUrl.bottom - rcViewUrl.top,
		hDlg, (HMENU)IDC_EDIT_URL1, NULL, NULL);

	SetRect (&rcViewUrl, rcView.right - 110, rcView.top + 30, rcView.right - 20, rcView.top + 60);

	CreateWindow (_T("Button"), _T("Play"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 
		rcViewUrl.left, rcViewUrl.top, rcViewUrl.right - rcViewUrl.left, rcViewUrl.bottom - rcViewUrl.top,
		hDlg, (HMENU)IDC_BUTTON_PLAYURL1, NULL, NULL);

	SetWindowText(hEdit, g_szFile1);

	SetRect (&rcViewUrl, rcView.left+10, rcView.top + 90, rcView.right - 120, rcView.top + 150);

	hEdit = CreateWindow (_T("Edit"), _T(""), WS_VISIBLE | ES_MULTILINE | WS_CHILD | TBS_BOTH | TBS_NOTICKS, 
		rcViewUrl.left, rcViewUrl.top, rcViewUrl.right - rcViewUrl.left, rcViewUrl.bottom - rcViewUrl.top,
		hDlg, (HMENU)IDC_EDIT_URL2, NULL, NULL);

	SetRect (&rcViewUrl, rcView.right - 110, rcView.top + 100, rcView.right - 20, rcView.top + 130);

	CreateWindow (_T("Button"), _T("Play"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 
		rcViewUrl.left, rcViewUrl.top, rcViewUrl.right - rcViewUrl.left, rcViewUrl.bottom - rcViewUrl.top,
		hDlg, (HMENU)IDC_BUTTON_PLAYURL2, NULL, NULL);

	SetRect (&rcViewUrl, rcView.left+10, rcView.top + 160, rcView.right - 120, rcView.top + 220);

	SetWindowText(hEdit, g_szFile2);

	hEdit = CreateWindow (_T("Edit"), _T(""), WS_VISIBLE | ES_MULTILINE | WS_CHILD | TBS_BOTH | TBS_NOTICKS, 
		rcViewUrl.left, rcViewUrl.top, rcViewUrl.right - rcViewUrl.left, rcViewUrl.bottom - rcViewUrl.top,
		hDlg, (HMENU)IDC_EDIT_URL3, NULL, NULL);

	SetRect (&rcViewUrl, rcView.right - 110, rcView.top + 170, rcView.right - 20, rcView.top + 200);

	CreateWindow (_T("Button"), _T("Play"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP, 
		rcViewUrl.left, rcViewUrl.top, rcViewUrl.right - rcViewUrl.left, rcViewUrl.bottom - rcViewUrl.top,
		hDlg, (HMENU)IDC_BUTTON_PLAYURL3, NULL, NULL);

	SetWindowText(hEdit, g_szFile3);

	UpdateWindow(hDlg);

	ShowWindow(hDlg,SW_SHOW);
}

// Message handler for select box.
INT_PTR CALLBACK ChannelSwitch(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		OnShowUrlSelect(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL1), g_szFile1, 2048);
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL2), g_szFile2, 2048);
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL3), g_szFile3, 2048);	

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		if(LOWORD(wParam) == IDC_BUTTON_PLAYURL1)
		{
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL1), g_szFile1, 2048);
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL2), g_szFile2, 2048);
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL3), g_szFile3, 2048);	

			PlayURL(g_szFile1);

			return (INT_PTR)TRUE;
		}

		if(LOWORD(wParam) == IDC_BUTTON_PLAYURL2)
		{
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL1), g_szFile1, 2048);
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL2), g_szFile2, 2048);
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL3), g_szFile3, 2048);	

			PlayURL(g_szFile2);
			return (INT_PTR)TRUE;
		}

		if(LOWORD(wParam) == IDC_BUTTON_PLAYURL3)
		{
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL1), g_szFile1, 2048);
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL2), g_szFile2, 2048);
			GetWindowText(GetDlgItem(hDlg,IDC_EDIT_URL3), g_szFile3, 2048);	

			PlayURL(g_szFile3);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for select box.
INT_PTR CALLBACK OptionSetting(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK InputURL(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:

		SetWindowPos (hDlg, NULL, 640, 100, 640, 320, 0);

		RECT rcView;
		GetClientRect (hDlg, &rcView);

		SetRect (&rcView, rcView.left+10, rcView.top + 20, rcView.right - 10, rcView.bottom - 50);

		g_hInputURL = CreateWindow (_T("Edit"), _T(""), WS_VISIBLE | ES_MULTILINE | WS_CHILD | TBS_BOTH | TBS_NOTICKS, 
			rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top - 50,
			hDlg, (HMENU)IDC_TEXT_INPUTURL, NULL, NULL);

		UpdateWindow(hDlg);

		ShowWindow(hDlg,SW_SHOW);

		SetTimer(mhWnd,WM_TIMER_PLAYTIME,500,NULL);
		SetTimer(mhWnd,WM_TIMER_POS,500,NULL);

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			UpdateWindow(hDlg);
			memset (g_szFile, 0, 1024*2*sizeof(TCHAR));

			GetWindowText(g_hInputURL, g_szFile, 2048);

			int nLen = wcslen(g_szFile);
			if(nLen > 4)
			{
				PlayURL(g_szFile);
			}

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if( LOWORD(wParam) == IDCANCEL)
		{
			memset (g_szFile, 0, 1024*2*sizeof(TCHAR));
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void OnShowTrackInfo(HWND hDlg)
{
	SetWindowPos (hDlg, NULL, 640, 100, 640, 200, 0);

	RECT rcView;
	RECT rcViewTrack;
	GetClientRect (hDlg, &rcView);

	UpdateWindow(hDlg);

	ShowWindow(hDlg,SW_SHOW);

	if (NULL == mpPlayer)
	{
		return;
	}

	InitTrackSelInfo(); //?? when the selected index will change?

	int  i;

	SetRect (&rcViewTrack, rcView.left+20, rcView.top + 20, rcView.left + 190, rcView.bottom - 10);

	CreateWindow(_T("COMBOBOX"), _T("Video Track"),
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,        
		rcViewTrack.left, rcViewTrack.top, rcViewTrack.right - rcViewTrack.left, rcViewTrack.bottom - rcViewTrack.top - 50,
		hDlg,
		(HMENU) IDC_SELECT_VIDEO,
		NULL,
		NULL);

	if(mpPlayer->GetVideoCount() > 0)
	{
		CHAR szMenuText[MAX_PATH] = "Auto Select";
		SendMessageA(GetDlgItem(hDlg,IDC_SELECT_VIDEO), CB_ADDSTRING, 0, (LPARAM) szMenuText);
	}

	for (i=0; i< mpPlayer->GetVideoCount(); ++i)
	{
		CHAR szMenuText[MAX_PATH] = "";

		GetMenuShowTextByIndex(szMenuText,VOOSMP_SS_VIDEO, i);

       SendMessageA(GetDlgItem(hDlg,IDC_SELECT_VIDEO), CB_ADDSTRING, 0, (LPARAM) szMenuText);
	}

	if(mpPlayer->GetVideoCount() > 0)
	{
		SendMessage(GetDlgItem(hDlg,IDC_SELECT_VIDEO), CB_SETCURSEL, (WPARAM)(m_nSelVedioIndex+1), (LPARAM)0);
	}

	SetRect (&rcViewTrack, rcView.left+230, rcView.top + 20, rcView.left + 400, rcView.bottom - 10);

	CreateWindow(_T("COMBOBOX"), _T("Audio Track"),
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,        
		rcViewTrack.left, rcViewTrack.top, rcViewTrack.right - rcViewTrack.left, rcViewTrack.bottom - rcViewTrack.top - 50,
		hDlg,
		(HMENU) IDC_SELECT_AUDIO,
		NULL,
		NULL);

	for (i=0; i< mpPlayer->GetAudioCount(); ++i)
	{
		CHAR szMenuText[MAX_PATH] = "";
		GetMenuShowTextByIndex(szMenuText,VOOSMP_SS_AUDIO, i);

		SendMessageA(GetDlgItem(hDlg,IDC_SELECT_AUDIO), CB_ADDSTRING, 0, (LPARAM) szMenuText);
	}

	if(mpPlayer->GetAudioCount() > 0)
	{
		SendMessage(GetDlgItem(hDlg,IDC_SELECT_AUDIO), CB_SETCURSEL, (WPARAM)m_nSelAudioIndex, (LPARAM)0);
	}

	SetRect (&rcViewTrack, rcView.left+440, rcView.top + 20, rcView.left + 610, rcView.bottom - 10);

	CreateWindow(_T("COMBOBOX"), _T("Subtitle Track"),
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,        
		rcViewTrack.left, rcViewTrack.top, rcViewTrack.right - rcViewTrack.left, rcViewTrack.bottom - rcViewTrack.top - 50,
		hDlg,
		(HMENU) IDC_SELECT_SUBTITLE,
		NULL,
		NULL);

	for ( i=0; i< mpPlayer->GetSubtitleCount(); ++i)
	{
		CHAR szMenuText[MAX_PATH] = "";
		GetMenuShowTextByIndex(szMenuText,VOOSMP_SS_SUBTITLE, i);

		SendMessageA(GetDlgItem(hDlg,IDC_SELECT_SUBTITLE), CB_ADDSTRING, 0, (LPARAM) szMenuText);
	}

	if(mpPlayer->GetSubtitleCount() > 0)
	{
		SendMessage(GetDlgItem(hDlg,IDC_SELECT_SUBTITLE), CB_SETCURSEL, (WPARAM)m_nSelSubtitleIndex, (LPARAM)0);
	}
}

void OnShowMenuTrackInfo(VOOSMP_SOURCE_STREAMTYPE trackType)
{
  if (NULL == mpPlayer)
  {
    return;
  }

  POINT point;
  GetCursorPos(&point);

  //InitTrackSelInfo(); //?? when the selected index will change?

  switch(trackType)
  {
  case VOOSMP_SS_VIDEO:
    {
      HMENU hMenuSelVideo = CreatePopupMenu();

      for (int i=0; i< mpPlayer->GetVideoCount(); ++i)
      {
        CHAR szMenuText[MAX_PATH] = "";

        GetMenuShowTextByIndex(szMenuText,VOOSMP_SS_VIDEO, i);

        UINT uFlags = mpPlayer->IsVideoAvailable(i)==TRUE ? MF_ENABLED : MF_GRAYED;
        AppendMenuA(hMenuSelVideo, MF_STRING|uFlags,ID_MENU_SEL_VIDEO+i,szMenuText);
      }

      //add "auto select"
      AppendMenuA(hMenuSelVideo, MF_STRING,ID_MENU_SEL_VIDEO+mpPlayer->GetVideoCount(),"Auto select");

      int nCheckIndex = m_nSelVedioIndex;
      if (m_nSelVedioIndex==-1)
      {
        nCheckIndex = mpPlayer->GetVideoCount();
      }
      CheckMenuItem(hMenuSelVideo,ID_MENU_SEL_VIDEO+nCheckIndex,MF_BYCOMMAND|MF_CHECKED);

      TrackPopupMenu(hMenuSelVideo,TPM_TOPALIGN,point.x,point.y,0,mhWnd,NULL);
    }
    break;
  case VOOSMP_SS_AUDIO:
    {
      HMENU hMenuSelAudio = CreatePopupMenu();

      for (int i=0; i< mpPlayer->GetAudioCount(); ++i)
      {
        CHAR szMenuText[MAX_PATH] = "";
        GetMenuShowTextByIndex(szMenuText,VOOSMP_SS_AUDIO, i);

        UINT uFlags = mpPlayer->IsAudioAvailable(i)==TRUE ? MF_ENABLED : MF_GRAYED;
        AppendMenuA(hMenuSelAudio, MF_STRING|uFlags,ID_MENU_SEL_AUDIO+i,szMenuText);
      }

      CheckMenuItem(hMenuSelAudio,ID_MENU_SEL_AUDIO+m_nSelAudioIndex,MF_BYCOMMAND|MF_CHECKED);

      TrackPopupMenu(hMenuSelAudio,TPM_TOPALIGN,point.x,point.y,0,mhWnd,NULL);
    }
    break;
  case VOOSMP_SS_SUBTITLE:
    {
      HMENU hMenuSelSubtitle = CreatePopupMenu();

      for (int i=0; i< mpPlayer->GetSubtitleCount(); ++i)
      {
        CHAR szMenuText[MAX_PATH] = "";
        GetMenuShowTextByIndex(szMenuText,VOOSMP_SS_SUBTITLE, i);

        UINT uFlags = mpPlayer->IsSubtitleAvailable(i)==TRUE ? MF_ENABLED : MF_GRAYED;
        AppendMenuA(hMenuSelSubtitle, MF_STRING|uFlags,ID_MENU_SEL_SUBTITLE+i,szMenuText);
      }

      CheckMenuItem(hMenuSelSubtitle,ID_MENU_SEL_SUBTITLE+m_nSelSubtitleIndex,MF_BYCOMMAND|MF_CHECKED);

      TrackPopupMenu(hMenuSelSubtitle,TPM_TOPALIGN,point.x,point.y,0,mhWnd,NULL);
    }
    break;
  default:
    break;
  }
}

int GetMenuShowTextByIndex(OUT LPSTR szMenuText, IN VOOSMP_SOURCE_STREAMTYPE sType, IN int nIndex)
{
  if (NULL == mpPlayer)
  {
    return VOOSMP_ERR_Pointer;
  }
  VOOSMP_SRC_TRACK_PROPERTY *pProperty = NULL;
  int nRC = VOOSMP_ERR_None;

  switch(sType)
  {
  case VOOSMP_SS_VIDEO:
    mpPlayer->GetVideoProperty(nIndex,&pProperty);
    break;
  case VOOSMP_SS_AUDIO:
    mpPlayer->GetAudioProperty(nIndex,&pProperty);
    break;
  case VOOSMP_SS_SUBTITLE:
    mpPlayer->GetSubtitleProperty(nIndex,&pProperty);
    break;
  default:
    break;
  }  

  if (NULL == pProperty)
  {
    strcpy(szMenuText,"Property error!");
    return VOOSMP_ERR_Pointer;
  }

  for (int j=0; pProperty && j<pProperty->nPropertyCount; ++j)
  {
    VOOSMP_SRC_TRACK_ITEM_PROPERTY* pItemProperties = pProperty->ppItemProperties[j];

    char szProperty[MAX_PATH] = "";
    strcpy(szProperty,pItemProperties->pszProperty);
    if (strstr(pItemProperties->szKey, ("language")))
    {
      for (int m=0; m<strlen(pItemProperties->pszProperty); ++m)
      {
        if (pItemProperties->pszProperty[m] == '-')
        {
          szProperty[m] = '\0';
          break;
        }

        szProperty[m] = pItemProperties->pszProperty[m];
      }
    }

    strcat(szMenuText,szProperty);

    if (j<pProperty->nPropertyCount-1)
    {
      strcat(szMenuText," | ");
    }                                        
  }
  return 0;
}

void InitTrackSelInfo()
{
  if(mpPlayer)
  {
		VOOSMP_SRC_CURR_TRACK_INDEX curIndex;
		int nRC = mpPlayer->GetCurrTrackSelection(&curIndex);

		if(nRC == VOOSMP_ERR_None)
		{
			m_nSelAudioIndex = curIndex.nCurrAudioIdx;
			m_nSelVedioIndex = curIndex.nCurrVideoIdx;
			m_nSelSubtitleIndex = curIndex.nCurrSubtitleIdx;
		}
  }
}
