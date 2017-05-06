	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.h

	Contains:	CVideoWindow header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-12-02		JBF			Create file

*******************************************************************************/

#ifndef __CVideoWindow_H__
#define __CVideoWindow_H__

class CFilterOneRender;
class CVOWPlayer;

class CVideoWindow : public CBaseControlWindow, public CBaseControlVideo
{
public:
	CVideoWindow(TCHAR * pName,	LPUNKNOWN pUnk,	HRESULT * phr, 
				 CCritSec * pInterfaceLock, CFilterOneRender *pRenderer);
	virtual ~CVideoWindow();

	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);

	// Overriden from CBaseWindow return our window and class styles
	LPTSTR GetClassWindowStyles(DWORD *pClassStyles, DWORD *pWindowStyles, DWORD *pWindowStylesEx);
	// Method that gets all the window messages
	LRESULT OnReceiveMessage(HWND hwnd,	UINT uMsg, WPARAM wParam,	LPARAM lParam);
    virtual HRESULT PrepareWindow();
    virtual HRESULT DoneWithWindow();

	// Pure virtual methods for the IBasicVideo interface
	HRESULT IsDefaultTargetRect();
	HRESULT SetDefaultTargetRect();
	HRESULT SetTargetRect(RECT *pTargetRect);
	HRESULT GetTargetRect(RECT *pTargetRect);
	HRESULT IsDefaultSourceRect();
	HRESULT SetDefaultSourceRect();
	HRESULT SetSourceRect(RECT *pSourceRect);
	HRESULT GetSourceRect(RECT *pSourceRect);
	HRESULT GetStaticImage(long *pBufferSize,long *pDIBImage);

	// IVideoWindow methods
	// Override to implement full screen mode
	STDMETHODIMP get_FullScreenMode(long * FullScreenMode);
	STDMETHODIMP put_FullScreenMode(long FullScreenMode);
	STDMETHODIMP GetMinIdealImageSize(long *pWidth, long *pHeight);
	STDMETHODIMP GetMaxIdealImageSize(long *pWidth, long *pHeight);

    STDMETHODIMP put_Left(long Left);
    STDMETHODIMP put_Width(long Width);
    STDMETHODIMP put_Top(long Top);
    STDMETHODIMP put_Height(long Height);
    STDMETHODIMP put_Owner(OAHWND Owner);
    STDMETHODIMP SetWindowForeground(long Focus);
    STDMETHODIMP SetWindowPosition (long Left,long Top,long Width,long Height);

    HRESULT DoCreateWindow();

	// video render function
	CCritSec *				GetFullScreenCritSec (void) {return &m_csFullScreen;}
	CFilterOneRender *	GetFilter (void) {return m_pRenderer;}

protected:
	virtual bool					SetFullScreen (bool bFullScreen);
	virtual BOOL					OnSize(LONG Width, LONG Height);
	virtual VIDEOINFOHEADER *		GetVideoFormat();

protected:
	CFilterOneRender *			m_pRenderer;
	CVOWPlayer *				m_pPlayer;

	HBRUSH						m_hBGBrush;

	VIDEOINFOHEADER *			m_pVideoFormat;
	CCritSec					m_csFullScreen;

	// IBasicVideo Interface Parameters
	bool						m_bDefaultTarget;
	RECT						m_rcTarget;

	bool						m_bDefaultSource;
	RECT						m_rcSource;

	// Support fullscreen mode
	long						m_lFullScreen;
	long						m_nOldLeft;   // Save the old position of video window before entering full screen
	long						m_nOldTop;
	long						m_nOldWidth;
	long						m_nOldHeight;
	OAHWND						m_pOldParent; // Save the old parent window
};

#endif // __CVideoWindow_H__