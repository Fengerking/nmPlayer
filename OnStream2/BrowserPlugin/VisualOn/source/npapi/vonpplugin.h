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

#ifndef __VONPPLUGIN_H__
#define __VONPPLUGIN_H__


#include "COSMPEngnWrap.h"
#include "npapi.h"
#include "npruntime.h"
#include "voIVCommon.h"
#include "voCMutex.h"
#include "CMsgQueue.h"
#include "voThread.h"

#define  VORGB_DEFAULT_BKG   RGB(0,0,0)

#define LIC_CONTENT_LEN (1024 * 64)

typedef struct {
	bool bWindowless;
	int x;
	int y;
	int width;
	int height;
	RECT clip;
	char szmime[32];
} VOPluginParam;
	

typedef void (*InvalidateWindowFunc)(int32_t, uint32_t, uint32_t, uint32_t);

class voNPPlugin
{
public:
	voNPPlugin(NPP pNPInstance);
	~voNPPlugin();

	void init(VOPluginParam*);
	void startEngn();
	bool setWindow(NPWindow* pNPWindow);
	HWND getWindow() { return m_hWnd; }
	void showVersion();
	void clear(void* hdc);
	void setStatus(const int n);
	int  getStatus () { return m_nStatus; }
	void shutdown();

	// for callback
	void notify(int nID, void * pParam1, void * pParam2);
	void notifyBrowserToDraw();
	void JSCallback(void * data);  // for windowed
	void JSCallback2(); // for windowless

	int16_t					handleEvent(void*);
	NPObject*				getScriptableObject();
	void					updateStrmEngn(const int type);
	COSMPEngnWrap*			getStrmEngn();

	void invalidateWindow();
	void invalidateWindow(const int left, const int top, const int right, const int bottom);
	bool isWindowless() { return  m_plgnParam.bWindowless; }
	bool isEngnStarted() { return (m_hMsgLoop != NULL); }

	void refreshOverlayWindow();
	void setJSCallbackObject(NPVariant&);
	void* getJSCallbackOjbect();

private:

	void				npRescheduleToMainThread(void (*func)(void *));

	int					msgHandlerLoop();
	static int			msgHandlerProc(VO_PTR pParam);
	static void			npcbInvalidateWindow(void *);
	static void			npcbJSCallback(void *);

	// for windowed mode
	static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);

	// for windowless mode
	//static VOID CALLBACK DisplayAsync(HWND hwnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult);

private:
	NPP						m_pnpp;
	void*					m_hdc;
	HWND					m_hWnd;

	NPWindow*				m_pnpWindow;
	NPObject*				m_pScriptableObject;
	NPObject*				m_pJSCallbackOjbect;
	COSMPEngnWrap*			m_pOsmpEngn;
	
	int						m_nStatus;   // 0: loaded/closed   1: openning   2: opened/prepared   3: playing   4: paused   5: stopped
	int						m_nCount;
	VO_U32					m_nMainThread;
	voCMutex				m_mtMsgQueue;
	VOPluginParam			m_plgnParam;
	voThreadHandle			m_hMsgLoop;
	bool					m_bExitMsgLoop;
	LONG_PTR				m_lpOldProc;
	CMsgQueue				m_msgQueue;
	HBRUSH					m_hBlackBrush;
	int						m_nToEraseCount;
};

#endif // __VONPPLUGIN_H__
