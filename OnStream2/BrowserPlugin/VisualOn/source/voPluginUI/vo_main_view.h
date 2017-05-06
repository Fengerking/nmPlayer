#pragma once
#include "vo_playback_control_top_layer.h"
class CPlugInUIWnd;

class vo_main_view
{
public:
	static LRESULT CALLBACK ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	vo_main_view(void);
	virtual ~vo_main_view(void);

  HWND GetWindow(){return m_hWndView;};
	void resize(int cx, int cy);
	void ShowWindow(bool bShow);
	bool IsWindowVisible();
	bool GetSliderPos(int nControl, int& nPos);
	bool SetSliderPos(int nControl, int nPos);
	void SetParentObject(CPlugInUIWnd*	pParent){m_pParent = pParent;};
	void UpdatePlayButtonStatus(bool bRunning);

  void SetCanSeek(bool bEnable);
	void SetDuration(const int);

protected:
	vo_playback_control_top_layer g_playback_control;
	HWND					m_hWndView;
	HWND					m_hWndParent;
	TCHAR					m_szClassName[64];
	TCHAR					m_szWindowName[64];

	CPlugInUIWnd*			m_pParent;

public:
	virtual bool	CreateWnd (HWND hwndParent, HINSTANCE hInst);
};
