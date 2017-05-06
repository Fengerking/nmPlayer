	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CPlugInUIWnd.h

	Contains:	CPlugInUIWnd header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-01-30		JBF			Create file

*******************************************************************************/

#ifndef __CPlugInUIWnd_H__
#define __CPlugInUIWnd_H__

#include <windows.h>
#include "voCSemaphore.h"
#include "voThread.h"
#include "voPlugInUI.h"
#include "vo_main_view.h"
#include "..\Common\CMsgQueue.h"
#include "atlconv.h"
#include "atlimage.h"

#define ID_BUTTON_PLAY			1001
#define ID_BUTTON_PAUSE			1002
#define ID_BUTTON_STOP			1003
#define ID_BUTTON_FULLSCREEN	1004
#define ID_CONTROL_POS			1005
#define ID_CONTROL_AUDIO		1006
#define ID_BUTTON_CLEAR_SELECTION 1007
#define ID_BUTTON_COMMIT_SELECTION 1008

#define ID_MENU_SEL_AUDIO  2000 //the value from 2000 to 2000+AudioCount
#define ID_MENU_SEL_VIDEO  3000 //the value from 3000 to 2000+VideoCount
#define ID_MENU_SEL_SUBTITLE  4000 //the value from 4000 to 2000+SubtitleCount

#include "CFullscreenBtn.h"

class CPlugInUIWnd
{
public:
	static LRESULT CALLBACK ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	// Used to control the image drawing
	CPlugInUIWnd (VOPUI_INIT_PARAM * pParam);
	virtual ~CPlugInUIWnd (void);

	virtual void *      GetView (void);
	virtual bool  	    ShowFullScreen (void);
	virtual int         ShowControls (bool bShow);
	virtual int         SetParam (int nID, void * pParam);
	virtual int         GetParam (int nID, void * pParam);
	void                OnPosChanged(int nPos, bool bTimeSlider);
	void                OnCommand(int wmId);
	void                UpdatePlayPauseButton();
	virtual bool  	    IsFullScreen (void);
	RECT                GetFullScreenRect();

	void                OnShowMenuTrackInfo(int wmID);
	void                OnSelectTrack(int wmId);

	BOOL                IsWndTop();

protected:
	virtual bool  	    CreateWnd (void);
	virtual bool  	    IsControlsVisible (void);
	void                SetWndTopmostIfFullScreen();
	virtual bool  	    UpdateBackground (HDC hDC);
	void                EraseBackGround(HDC hDC);
	virtual bool  	    UpdateSize ();

	int				    msgHandlerLoop();
	static int			msgHandlerProc(VO_PTR pParam);

private:
	int                 GetMenuShowTextByIndex(OUT LPSTR szMenuText, IN VOOSMP_SOURCE_STREAMTYPE sType, IN int nIndex);
	void                UpdateVolumeSlide();
	void                UpdateControlSlide();

	LONG                GetDuration();
	LONG                GetPos();

	void                ShowErrorMsg(VOCBMSG cbEventInfo);

	void                UpdateSeekEnableStatus();
	//wait buffer cursor
	bool                InitWaitCursor();
	void                UnInitWaitCursor();
	void                ShowWaitCursor(VOOSMP_VR_USERCALLBACK_TYPE* pVr = NULL);
	void                UpdateCursorPosition();

	VOOSMP_STATUS       GetPlayerStatus();

	void                cbHandleEvent(VOCBMSG eventInfo);

	void                ShowMouseCursor(BOOL bShow);

	void                procOnMouseMove(WPARAM wParam, LPARAM lParam);
	void                procOnTimer(WPARAM wParam, LPARAM lParam);
	void                procOnDispalyChange(WPARAM wParam, LPARAM lParam);
	void                procOnLButtonDown(WPARAM wParam, LPARAM lParam);
	void                procOnKeyDown(WPARAM wParam, LPARAM lParam);
	void                procOnSize(WPARAM wParam, LPARAM lParam);
	void                procOnAvtivateApp(WPARAM wParam, LPARAM lParam);

	void				getNormalFullBtnRect(RECT&);
	void				traceMouseMove();

    void                ControlFullBtn(BOOL bShow);
protected:
	VOPUI_INIT_PARAM		m_initParam;

	HWND					      m_hWndView;
	HWND					      m_hWndParent;
	HWND				m_hBrowserContextWnd;
		TCHAR					      m_szClassName[64];
	TCHAR					      m_szWindowName[64];

	RECT					      m_rcView;
	HBRUSH				      m_brhBackground;

	//child window
	vo_main_view*			  m_pControlWindow;
	int						      m_nSkipTimes;

	bool                m_bPopupMenu;
	bool                m_bForceUpdateTrackSelInfo;

	bool                m_bFullScreen;

	BOOL                m_bEnableLiveSeek;

	BOOL                m_bVideoRenderStart;

	BOOL                m_bPureAudio;

	//wait buffer cursor
	BOOL                m_bBuffering;//if the buffering icon showing
	BOOL                m_bBufferBegin;
	HBITMAP		      m_hBmpList[12];
	DWORD               m_dwTickStartBuffering;
	int                 m_dwWaitBufferingTime;


	//to support png
	CImage*		          m_image;
	Gdiplus::Bitmap*		  m_pGDIPlusimage;
	CImage*		          m_imageFull;
	Gdiplus::Bitmap*		  m_pGDIPlusimageFull;
	HBITMAP				  m_hBackBitmap;

	int                 m_iBMPIndex;
	int			            m_nBmpWidth;
	int			            m_nBmpHeight;
	int			            m_BmpX;
	int			            m_BmpY;
	HDC			            m_hdc;
	HDC			            m_hdcMem;
	DWORD			          m_dwTickCountLastShow;

	DWORD               m_dwLastClickLButtonTime;
	POINT               m_ptLastMousePos;

	BOOL                m_bControlbarShow;
	BOOL                m_bCursorShow;
	DWORD               m_dwTickLastKeyEvent;
	DWORD               m_dwTickLastMouseMove;

	int                 m_dwHideButtonTime;

	CFullscreenBtn*     m_pFullscreenBtn;
	//voCSemaphore			m_semAnimation;
	BOOL					m_bExit;
	voThreadHandle		m_hAnimationThd;
	BOOL					m_bMouseTracking;

	BOOL                  m_bEanblePlugin;//now, do not allow multi-instance
	BOOL					m_bActive;

	HMONITOR              m_hMonitorFullscreen;

	CMsgQueue				      m_msgQueue;
	voCMutex				      m_mtMsgQueue;
	BOOL					m_bShowFullBtn;
};

#endif // __CPlugInUIWnd_H__