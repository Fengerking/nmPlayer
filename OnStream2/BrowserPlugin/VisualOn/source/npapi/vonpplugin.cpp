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

#include "afxwin.h"

#ifdef XP_MAC
#include <TextEdit.h>
#endif

#ifdef XP_UNIX
#include <string.h>
#endif

#include <stdio.h>
#include "config.h"
#include "voNPPlugin.h"
#include "npfunctions.h"
#include "voFile.h"
#include "CBaseConfig.h"
#include "voThread.h"
#include "voOSFunc.h"
#include "..\voPluginUI\vopluginUI.h"
#include "voPluginCBType.h"
#include "voScriptableObject.h"
#include "vompType.h"
#include "volog.h"


#define WM_USERDEFNOTIFY_CALLBACK WM_USER + 101 //user defined callback notifying message for both windowed & windowless modes

#define VOPLUGIN_DEBUG 0
#if (VOPLUGIN_DEBUG)
static int g_nNotifiedTime = 0;
#endif

extern TCHAR g_lpszPluginPath[MAX_PATH];

static NPClass plugin_ref_obj = {
	NP_CLASS_STRUCT_VERSION,
	voScriptableObject::_Allocate,
	voScriptableObject::_Deallocate,
	voScriptableObject::_Invalidate,
	voScriptableObject::_HasMethod,
	voScriptableObject::_Invoke,
	voScriptableObject::_InvokeDefault,
	voScriptableObject::_HasProperty,
	voScriptableObject::_GetProperty,
	voScriptableObject::_SetProperty,
	voScriptableObject::_RemoveProperty,
	voScriptableObject::_Enumerate,
	voScriptableObject::_Construct,
};


static int  vonpPluginCallback(void * pUserData, int nID, void * pParam1, void * pParam2)
{
	voNPPlugin * plugin=(voNPPlugin*)pUserData;
	if (NULL == plugin)
		return -1;

	if (VOMP_CB_VideoReadyToRender == nID) {

		plugin->notifyBrowserToDraw();

	} else {

		plugin->notify(nID, pParam1, pParam2);
	}

	return 0;
}

voNPPlugin::voNPPlugin(NPP inst) :
	m_pnpp(inst),
	m_hWnd(NULL), 
	m_pnpWindow(NULL),
	m_pScriptableObject(NULL),
	m_pJSCallbackOjbect(NULL),
	m_pOsmpEngn(NULL),
	m_nStatus(0),
	m_hMsgLoop(NULL),
	m_bExitMsgLoop(false),
	m_lpOldProc(NULL),
	m_hBlackBrush(NULL),
	m_nToEraseCount(0)
{
	memset(&m_plgnParam, 0, sizeof (VOPluginParam));
}

voNPPlugin::~voNPPlugin()
{
	shutdown();

	if (m_pScriptableObject) {
		
		int rc = m_pScriptableObject->referenceCount;
		VOLOGI("scriptable object rc: %d", rc);
		//for (int i = 0; i < rc; ++i) 
			NPN_ReleaseObject(m_pScriptableObject);
	}
	m_pScriptableObject = NULL;

	if (m_pJSCallbackOjbect) {

		//for (int i = 0; i < rc; ++i) 
			NPN_ReleaseObject(m_pJSCallbackOjbect);
	}

	//VOLOGI("m_pJSCallbackObject: %p, referenceCount: %d", m_pJSCallbackOjbect, m_pJSCallbackOjbect->referenceCount);
	m_pJSCallbackOjbect = NULL;

	if (m_hBlackBrush) {
		::DeleteObject(m_hBlackBrush);
		m_hBlackBrush = NULL;
	}

	SetThreadExecutionState(ES_CONTINUOUS);
}

void voNPPlugin::init(VOPluginParam* pp) {

	if (pp)
		memcpy(&m_plgnParam, pp, sizeof(VOPluginParam));

	m_nMainThread = voThreadGetCurrentID();

	if (m_plgnParam.bWindowless) {

        NPN_SetValue(m_pnpp, NPPVpluginWindowBool, (void*)false);
		NPError err = NPN_SetValue(m_pnpp, NPPVpluginTransparentBool, (void*)true); // Set transparency to true or false
		VOLOGI("Informed browser I'm windowless!! Then set transparent to true, return: 0x%08x", err);
		//BOOL ret = SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		//VOLOGI("Main thread 0x%08x priority changed to THREAD_PRIORITY_HIGHEST 0x%08x return: %d", GetCurrentThread(), THREAD_PRIORITY_HIGHEST, ret);
	}
}

void voNPPlugin::startEngn() {
	VO_U32 tID = 0;
	m_msgQueue.setCapability(MAX_MSG_COUNT);
	voThreadCreate(&m_hMsgLoop, &tID, (voThreadProc)msgHandlerProc, this, 0);

	if (m_pOsmpEngn == NULL)
		m_pOsmpEngn = new COSMPEngnWrap();

	updateStrmEngn(3);
	VOLOGI("Engine already started up.");
}

void voNPPlugin::setStatus(const int n) {

	m_nStatus = n;

	if (m_nStatus < 2) { // if closed or just opening, then all msg should be abandoned
	
		voCAutoLock lock (&m_mtMsgQueue);
		m_msgQueue.clearAll();
	}

	if (m_plgnParam.bWindowless) {
		invalidateWindow();

		if (m_nStatus < 2) {
			clear(m_hdc);
			invalidateWindow();
		}
	}
}

int16_t voNPPlugin::handleEvent(void* pe)
{
	if (NULL == pe)
		return 0;

	NPEvent* evt(reinterpret_cast<NPEvent*>(pe));

	LRESULT lRes(0);
	if (evt->event == WM_PAINT) {  //special handle drawing, as we need to pass the draw bounds through the layers

		void* hdc = (void*)evt->wParam;
		if (m_hdc != hdc) {
			m_hdc = hdc;
 
			if (m_pOsmpEngn) 
				m_pOsmpEngn->SetDC((HDC)m_hdc);
#if VOPLUGIN_DEBUG
			VOLOGI("DC changed!");
#endif
		}

#if VOPLUGIN_DEBUG
		static int last = voOS_GetSysTime();
		int mid = voOS_GetSysTime();
		int elapsed = mid - g_nNotifiedTime;
#endif
		
		if (m_nStatus == 1 || m_nStatus == 2 || m_nStatus == 5) {

			clear(hdc);			
		} else if (m_nStatus == 3) { // if running view rect changed
			
			if (m_nToEraseCount) {
				clear(hdc);			
				m_nToEraseCount--;
				//VOLOGI("view rect changed, so background erased!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			} 
		}

		if (m_pOsmpEngn)
			m_pOsmpEngn->Redraw();

#if VOPLUGIN_DEBUG
		int now = voOS_GetSysTime();
		VOLOGI("[notified] at: %d, elapsed: %d, [drawing] last: %d, now: %d, draw interval: %d", g_nNotifiedTime, mid - g_nNotifiedTime, last, mid, mid - last);
		last = mid;

		VOLOGI("[rendered] at: %d, cost: %d, since notified: %d", now, now - mid, now - g_nNotifiedTime);
#endif
		return 1;

	} else {
		switch(evt->event) {
			case WM_LBUTTONDBLCLK:
				{
					//VOLOGI("here got left button double clicked...");
					break;
				}
				break;
			case WM_LBUTTONDOWN: 
				{
					//VOLOGI("here got left button down...");
					return 1;
					break;
				}
			case WM_RBUTTONDOWN:
				{
					break;
				}
			case WM_MBUTTONDOWN:
				{
					break;
				}
			case WM_LBUTTONUP: 
				{
					//VOLOGI("here got left button up...");
					return 1;
					break;
				}
			case WM_RBUTTONUP:
				{
					break;
				}
			case WM_MBUTTONUP:
				{
					break;
				}
			case WM_KEYUP:
				{
					break;
				}
			case WM_KEYDOWN:
				{
					switch(evt->lParam)
					{
					case VK_ESCAPE:
						{
							VOCBMSG msg;
							msg.nID = VOOSMP_PLUGIN_CB_UICommand;
							msg.nValue1 = VOUI_CMD_FULLSCREEN;
							JSCallback((void *)&msg);
						}
						return 1;
					}
					break;
				}

			case WM_MOUSEMOVE: 
				{
					//::ShowCursor(TRUE);				
				}
			   break;
			case WM_WINDOWPOSCHANGED:
				{
					//VOLOGI("here got poschanged message!");	
					return 1;
				}
				break;
			case WM_POWERBROADCAST:
				{
					VOLOGI("system power state: 0x08x", evt->wParam);
				}
				break;
			default:
				{
					VOLOGI("unknown event: 0x%04x", evt->event);
					break;
				}
		}
	}

	return 0;
}

bool voNPPlugin::setWindow(NPWindow* pNPWindow)
{
	if (pNPWindow == NULL)
		return false;

	NPWindow *pw = pNPWindow;
	int updated = 0;
	
	HWND browserHWND = NULL;
	if (m_plgnParam.bWindowless) {
		
		NPBool bWindowlessSupported = false;
		NPN_GetValue(m_pnpp, NPNVSupportsWindowless, (void*)&bWindowlessSupported); 
		NPN_GetValue(m_pnpp, NPNVnetscapeWindow, (void*)&browserHWND);
		//VOLOGI("pw->window: %p, browserHWND: %p, type: %d, windowless supported: %d", pw->window, browserHWND, pw->type, bWindowlessSupported);
	}

	if (!m_plgnParam.bWindowless && m_hWnd != pw->window)
	{
#ifdef XP_WIN
		//VOLOGI("window changed!");
		if (m_hWnd != NULL && m_lpOldProc != NULL)
		{
			SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, m_lpOldProc);
		}
#endif
	} 

	if (m_plgnParam.bWindowless) {

		if (m_hWnd != browserHWND) {
			updated = 2;

			m_hWnd = browserHWND;
			m_hdc = pw->window;
		}
		//VOLOGI("browser window: %p, pw->window: %p", m_hWnd, pw->window);
	} else {
		if (m_hWnd != (HWND)pw->window)
			updated = 2;

		m_hWnd = (HWND)pw->window;
		if (m_hWnd == NULL) 
			return false;

		//VOLOGI("windowed handle: %p", m_hWnd);
	}

	if (!m_plgnParam.bWindowless && updated == 2)
	{
		DWORD dwRet = SetClassLong(m_hWnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(BLACK_BRUSH)); 
		m_lpOldProc = SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LPARAM)((WNDPROC)PluginWinProc));
		//VOLOGI("m_lpOldProc: %p", m_lpOldProc);
		// associate window with our voNPPlugin object so we can access it in the window procedure
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
	}

	m_pnpWindow = pw;
	bool bzoomed = false;
	if (m_plgnParam.bWindowless) {
		if (m_plgnParam.x != pw->x || m_plgnParam.y != pw->y || m_plgnParam.width != pw->width || m_plgnParam.height != pw->height) {

		VOLOGI("[old] x: %d, y: %d, w: %d, h: %d, [clip] t: %d, l: %d, b: %d, r: %d", m_plgnParam.x, m_plgnParam.y, m_plgnParam.width, m_plgnParam.height,
			m_plgnParam.clip.top, m_plgnParam.clip.left, m_plgnParam.clip.bottom, m_plgnParam.clip.right);

		VOLOGI("[new] x: %d, y: %d, w: %d, h: %d, [clip] t: %d, l: %d, b: %d, r: %d", pw->x, pw->y, pw->width, pw->height,
			pw->clipRect.top, pw->clipRect.left, pw->clipRect.bottom, pw->clipRect.right);		
			updated += 1;
		}
	}

	if (m_plgnParam.width != pw->width)
		bzoomed = true;

	if (updated || bzoomed) {
		m_plgnParam.x = pw->x;
		m_plgnParam.y = pw->y;
		m_plgnParam.width = pw->width;
		m_plgnParam.height = pw->height;
		m_plgnParam.clip.left = pw->clipRect.left;
		m_plgnParam.clip.top = pw->clipRect.top;
		m_plgnParam.clip.right = pw->clipRect.right;
		m_plgnParam.clip.bottom = pw->clipRect.bottom;
	}
	//VOLOGI("[window region] l = %d, t = %d, w = %d, h = %d; [clip region] l = %d, t = %d, r = %d, b = %d", 
	//	pw->x, pw->y, pw->width, pw->height, pw->clipRect.left, pw->clipRect.top, pw->clipRect.right, pw->clipRect.bottom);

	//VOLOGI("updated: %d, browser hdc: %p, pw->window: %p", updated, m_hdc, pw->window);
	updateStrmEngn(updated);

	if (updated || bzoomed) {
		// due to FF has 2 alternative DCs for a rendering instance, so once view rect changed,
		// make sure these 2 DC have to be erased with black color
		m_nToEraseCount = 2;
	}

	if (!m_plgnParam.bWindowless) {

		if (bzoomed && m_nStatus) {
			//VOLOGI("zoomed! please update window right now!!!");	
			refreshOverlayWindow();
		} else if (m_nStatus == 4) {
			//VOLOGI("if windowed & paused, must be redrawn!");
			if (m_pOsmpEngn)
				m_pOsmpEngn->Redraw();
		}
	} else {

		if (m_nStatus == 4) {
			//VOLOGI("if windowless & paused, must be redrawn for some strange devices!");
			notifyBrowserToDraw();
		}
	}

	return true;
}

void voNPPlugin::shutdown()
{
	VOLOGI("NPP %p m_pScriptableObject %p gonna shutdown...", m_pnpp, m_pScriptableObject);
	if (m_pOsmpEngn != NULL)
	{
		delete m_pOsmpEngn;
		m_pOsmpEngn = NULL;
	}
	m_nStatus = 0;

	if (m_hWnd != NULL && m_lpOldProc != NULL && !m_plgnParam.bWindowless)
	{
		SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, m_lpOldProc);
		m_hWnd = NULL;
	}


	if (m_hMsgLoop) {
		m_bExitMsgLoop = true;
		// waiting for message loop thread exiting from sleeping
		voOS_Sleep(100);
		voThreadClose(m_hMsgLoop, 0);
		m_hMsgLoop = NULL;
	}

	VOLOGI("NPP %p m_pScriptableObject %p was dead.", m_pnpp, m_pScriptableObject);
}


// this will force to draw a version string in the plugin window
void voNPPlugin::showVersion()
{
	const char *ua = NPN_UserAgent(m_pnpp);

	InvalidateRect(m_hWnd, NULL, true);
	UpdateWindow(m_hWnd);

	if (m_pnpWindow) 
	{
		NPRect r ={
					(uint16_t)m_pnpWindow->y, (uint16_t)m_pnpWindow->x,
					(uint16_t)(m_pnpWindow->y + m_pnpWindow->height),
					(uint16_t)(m_pnpWindow->x + m_pnpWindow->width)
				  };

		NPN_InvalidateRect(m_pnpp, &r);
	}
}

// this will clean the plugin window
void voNPPlugin::clear(void* hdc)
{
	RECT r = { m_plgnParam.x, m_plgnParam.y, m_plgnParam.x + m_plgnParam.width, m_plgnParam.y + m_plgnParam.height};
	if (m_hBlackBrush == NULL)
		m_hBlackBrush = (HBRUSH)::CreateSolidBrush (VORGB_DEFAULT_BKG);

	if (hdc)
		::FillRect((HDC)hdc,&r,m_hBlackBrush);
}


NPObject * voNPPlugin::getScriptableObject()
{
	if (NULL == m_pScriptableObject) 
	{
		VOLOGI("gonna create a new scriptable object.");
		m_pScriptableObject = NPN_CreateObject(m_pnpp, &plugin_ref_obj);
	}

	if (m_pScriptableObject) 
	{
		NPN_RetainObject(m_pScriptableObject);
	}

	VOLOGI("NPP: %p, m_pScriptableObject: %p, referenceCount: %d", m_pnpp, m_pScriptableObject, m_pScriptableObject->referenceCount);
	return m_pScriptableObject;
}


 void voNPPlugin::updateStrmEngn(const int type)
 {
	 VO_PLUGINWRAP_INIT_PARAM voInitParam;
	 voInitParam.pListener = vonpPluginCallback;
	 voInitParam.pUserData = this;
	 voInitParam.hView = m_hWnd;

	 //VOLOGI("voInitParam.hView: %p, width: %d", voInitParam.hView, m_plgnParam.width);
	 voInitParam.bWindowless = m_plgnParam.bWindowless;
	 voInitParam.rcDraw.left = m_plgnParam.x;
	 voInitParam.rcDraw.top = m_plgnParam.y;
	 voInitParam.rcDraw.right = voInitParam.rcDraw.left + m_plgnParam.width;
	 voInitParam.rcDraw.bottom = voInitParam.rcDraw.top + m_plgnParam.height;
	 _tcscpy(voInitParam.szWorkPath, g_lpszPluginPath);

	 if (m_pOsmpEngn == NULL) 
		return ;
	 
	 if (type > 1 || (!m_plgnParam.bWindowless && m_nStatus == 0)) {
		 m_pOsmpEngn->Init(&voInitParam);
		 m_nStatus = 0;
	 } else if (type == 1) {
		 RECT r = voInitParam.rcDraw;
		 if (m_plgnParam.bWindowless) {
			 m_pOsmpEngn->updateViewRegion(r);
		 } 
	 }
 }

 COSMPEngnWrap*	voNPPlugin::getStrmEngn() {

	 return m_pOsmpEngn;
 }

void voNPPlugin::notify(int nID, void * pParam1, void * pParam2)
{

	voCAutoLock lock (&m_mtMsgQueue);
	VOCBMSG msg;
	msg.nID = nID;

	if (NULL != pParam1)
		msg.nValue1 = (unsigned int) *((unsigned int*)pParam1);

	if (NULL != pParam2)
		msg.nValue2 = (unsigned int) *((unsigned int*)pParam2);

	//VOLOGI("nID: 0x%08x, webID: 0x%08x, param2: 0x%08x", nID, (int)pParam1, (pParam2 != NULL) ?  *((int*)pParam2) : 0);
	if (msg.nID == VO_OSMP_CB_WINDOWED_UI_COMMAND || msg.nID == VOOSMP_CB_VideoRenderStart || msg.nID == VOOSMP_CB_AudioRenderStart)
		m_msgQueue.insert2Head(msg);
	else
		m_msgQueue.enqueue(msg);
}

void voNPPlugin::notifyBrowserToDraw() {

#if VOPLUGIN_DEBUG
	int now = voOS_GetSysTime();
	//VOLOGI("[notify] last: %d, now: %d, interval: %d", g_nNotifiedTime, now, now - g_nNotifiedTime);
	g_nNotifiedTime = now;
#endif

	//VOLOGI("notified to draw again...");
	npRescheduleToMainThread(npcbInvalidateWindow);
}

void voNPPlugin::JSCallback(void * data)
{
	if (data == NULL || m_pJSCallbackOjbect == NULL)
		return;

	VOCBMSG msg;
	memcpy(&msg, data, sizeof(msg));

	NPVariant args[3];
	DOUBLE_TO_NPVARIANT(msg.nID, args[0]);	

	unsigned int p1 = msg.nValue1;
	DOUBLE_TO_NPVARIANT(p1, args[1]);

	unsigned int p2 = msg.nValue2;
	DOUBLE_TO_NPVARIANT(p2, args[2]);

	NPVariant ret; 
	//VOLOGI("m_pJSCallbackOjbect: %p, [msg] id: 0x%08x, p1: 0x%08x, p2: 0x%08x", m_pJSCallbackOjbect, msg.nID, msg.nValue1, msg.nValue2);
	NPN_InvokeDefault(m_pnpp, m_pJSCallbackOjbect, args, 3, &ret);
	NPN_ReleaseVariantValue(&ret); 
}

void voNPPlugin::JSCallback2()
{
	int size = m_msgQueue.getCount();
	if (!size) return;

	VOCBMSG msg;
	int ret = 0;

	{
		voCAutoLock lock (&m_mtMsgQueue);
		ret = m_msgQueue.dequeue(&msg);
	}

	if (VOQU_ERR != ret && m_nStatus != 5) {

		JSCallback(&msg);
	}
}


void voNPPlugin::npcbInvalidateWindow(void * pp) {

	voNPPlugin* p = (voNPPlugin*)pp;

	if (p) {
		p->invalidateWindow();
	}
}

void voNPPlugin::npcbJSCallback(void * pp) {

	voNPPlugin* p = (voNPPlugin*)pp;

	if (p) {
		p->JSCallback2();
	}
}


void voNPPlugin::invalidateWindow()
{

	NPRect r = {
		(uint16_t)0, (uint16_t)0,
		(uint16_t)( m_plgnParam.height),
		(uint16_t)(m_plgnParam.width)
	};

	NPN_InvalidateRect(m_pnpp, &r);
	NPN_ForceRedraw(m_pnpp);

}

void voNPPlugin::invalidateWindow(const int left, const int top, const int right, const int bottom)
{
    NPRect r = { top, left, bottom, right };
	NPN_InvalidateRect(m_pnpp, &r);
}


int	voNPPlugin::msgHandlerProc(VO_PTR pParam) {

	if (pParam == NULL)
		return 0;

	voNPPlugin* p = (voNPPlugin*)pParam;
	p->msgHandlerLoop();

	return 0;
}

void voNPPlugin::npRescheduleToMainThread(void (*func)(void *)) {

	NPN_PluginThreadAsyncCall(m_pnpp, func, this);
}

int	voNPPlugin::msgHandlerLoop() {

	int localcounter = 0;
	bool bSaverForbidden = false;
	int nSystemState = 1;

	while (!m_bExitMsgLoop) {
		
		localcounter++;

		// detect proxy per 2s
		if (localcounter >= 20) {

			if (m_pOsmpEngn) {
				m_pOsmpEngn->detectProxy();
			}

			if (m_nStatus == 3) {
				
				if (!bSaverForbidden) {
					SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
					bSaverForbidden = true;
				}

			} else {

				if (bSaverForbidden) {
					SetThreadExecutionState(ES_CONTINUOUS);
					bSaverForbidden = false;
				}
			}

			localcounter = 0;
		}

		int size = m_msgQueue.getCount();
		if (!size) {
			voOS_Sleep(100);

			continue;
		} 

		if (m_plgnParam.bWindowless) {

			npRescheduleToMainThread(npcbJSCallback);

		} else {
			voCAutoLock lock (&m_mtMsgQueue);
			VOCBMSG msg;
			int ret = m_msgQueue.dequeue(&msg);
			//VOLOGI("msg list size: %d, msg.nID: 0x%08x, msg.nValue1: 0x%08x", size, msg.nID, msg.nValue1);
			// if stopped, not post any more
			if (msg.nID == VO_OSMP_CB_WINDOWED_UI_COMMAND || m_nStatus != 5)
				::PostMessage(m_hWnd, WM_USERDEFNOTIFY_CALLBACK, 0, (LPARAM)(&msg));
		}

		voOS_Sleep(100);
	}

	VOLOGI("plugin message loop thread exit normally.");
	return 0;
}

void voNPPlugin::refreshOverlayWindow() {

	if (m_pOsmpEngn == NULL)
		return;

	m_pOsmpEngn->postMessage2UI(WM_SIZE, NULL, NULL);
}

LRESULT CALLBACK voNPPlugin::PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		{
			VOLOGI("gonna do resize action...");
		}
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			{
				VOLOGI("HERE got a key down....")
				VOCBMSG msg;
				msg.nID = VOOSMP_PLUGIN_CB_UICommand;
				msg.nValue1 = VOUI_CMD_FULLSCREEN;
				voNPPlugin * p = (voNPPlugin *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				if (p) {
					p->JSCallback(&msg);
				}
			}
			break;
		}
		break;

	case WM_USERDEFNOTIFY_CALLBACK:
		{
			voNPPlugin * p = (voNPPlugin *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (p)
				p->JSCallback((void *)lParam);

		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void voNPPlugin::setJSCallbackObject(NPVariant& npval) {

	m_pJSCallbackOjbect = NPVARIANT_TO_OBJECT(npval);
	if (m_pJSCallbackOjbect)
		NPN_RetainObject(m_pJSCallbackOjbect);

	VOLOGI("NPP: %p, m_pJSCallbackOjbect: %p, referenceCount: %d", m_pnpp, m_pJSCallbackOjbect, m_pJSCallbackOjbect->referenceCount);
}

void* voNPPlugin::getJSCallbackOjbect() {

	if (m_pJSCallbackOjbect) 
	{
		NPN_RetainObject(m_pJSCallbackOjbect);
	}

	VOLOGI("NPP: %p, m_pJSCallbackOjbect: %p, referenceCount: %d", m_pnpp, m_pJSCallbackOjbect, m_pJSCallbackOjbect->referenceCount);
	return static_cast<void *>(m_pJSCallbackOjbect);
}

