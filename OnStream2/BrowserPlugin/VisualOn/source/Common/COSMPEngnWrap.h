#ifndef __COSMPEngnWrap_H__
#define __COSMPEngnWrap_H__


#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <tchar.h>
#include <vector>
#include "voDRM2.h"
#include "CvoOnStreamMP.h"

#include "voType.h"
#include "voString.h"
#include "COverlayUI.h"
#include "voCMutex.h"
#include "voPluginCBType.h"


#define	VOOSMP_PLUGIN_CB_UICommand			0X02000005		/*!<The UI Command. pParam1 int *, Command ID, pParam2 int * value of command  */
#define	VOOSMP_PLUGIN_CB_SRCCommand			0X02000006		/*!<The Source Command. pParam1 int, Command ID, pParam2 int * value of command  */
#define VOSEMA_MULINSTANCE_NAME				_T("voPluginIE_SingleInstance_Control")
#define VOSEMA_VERIMATRIXDRMINIT_NAME			_T("voPluginIE_VerimatrixDRM_Init_Control")
#define VOSEMA_VERIMATRIXDRMSHAKE_NAME			_T("voPluginIE_VerimatrixDRM_Shake_Control")
#define JSON LPCTSTR

typedef struct voPluginInitParam
{
    // Call back function
    VOOSMPListener	pListener;
    // The user data
    void *			pUserData;
    // The view handle
    void *			hView;
    // The Run Work Path
    TCHAR      szWorkPath[MAX_PATH];
	
	BOOL		bWindowless;
	RECT		rcDraw;

    voPluginInitParam()
    {
        pListener = NULL;
        pUserData = NULL;
        hView = NULL;
		bWindowless = false;
		memset(&rcDraw, 0, sizeof(RECT));
        _tcscpy(szWorkPath,_T(""));
    }
    voPluginInitParam& operator=( const voPluginInitParam& voPParam )
    { 
        pListener = voPParam.pListener;
        pUserData = voPParam.pUserData;
        hView     = voPParam.hView;
        _tcscpy(szWorkPath,voPParam.szWorkPath);
		bWindowless = voPParam.bWindowless;
		rcDraw = voPParam.rcDraw;
        return *this; 
    } 
} VO_PLUGINWRAP_INIT_PARAM;

typedef struct __voFirefoxProxy__ {

	int type;   // 0: no proxy; 1: manual configure; 2: automatic configuration url; 3: use system setting; 4: auto detect
	bool share_proxy_setting;

	char ftp[16];
	int	 ftp_port;

	char http[16];
	int  http_port;

	char socks[16];
	int  socks_port;

	char ssl[16];
	int  ssl_port;

	char* autoconfig_url;
} VOFFPROXY;

class COSMPEngnWrap
{
public:
    // Used to control the image drawing
    COSMPEngnWrap (void);
    virtual   ~COSMPEngnWrap (void);

    BOOL                    Init (VO_PLUGINWRAP_INIT_PARAM * pParam);
    void                    Uninit ();

    CvoOnStreamMP *         GetOSMPPlayer(){return m_pOSMPPlayer;};

    //Player control
    VO_OSMP_RETURN_CODE     Open(LPCTSTR cstrUrl, VO_OSMP_SRC_FLAG voSourceFlag,VO_OSMP_SRC_FORMAT voSourceType, void* openParam);
    VO_OSMP_RETURN_CODE     Close();
    VO_OSMP_RETURN_CODE     Start();
    VO_OSMP_RETURN_CODE     Pause();    
    VO_OSMP_RETURN_CODE     Stop();
    LONG                    SetPosition(LONG lMSec);
    LONG                    GetPosition();
    LONG                    GetDuration();
    VO_OSMP_RETURN_CODE     SetVolume(FLOAT fLeftVol, FLOAT fRightVol);
    LONG                    GetVolume();
    VO_OSMP_RETURN_CODE     Mute();
    VO_OSMP_RETURN_CODE     UnMute();
    VO_OSMP_RETURN_CODE     SetVideoAspectRatio(VO_OSMP_ASPECT_RATIO ar);
    VO_OSMP_STATUS          GetPlayerStatus();
    BOOL                    CanBePaused();

    //Player Configuration
    VO_OSMP_RETURN_CODE     SetLicenseContent(LPCTSTR data);
    VO_OSMP_RETURN_CODE     SetPreAgreedLicense(LPCSTR str);
    VO_OSMP_RETURN_CODE     SetLicenseFilePath(LPCTSTR path);
    VO_OSMP_RETURN_CODE     EnableDeblock(BOOL bEnable);
    VO_OSMP_RETURN_CODE     SetDeviceCapabilityByFile(LPCTSTR filePath);
    VO_OSMP_RETURN_CODE     SetInitialBitrate(int bitrate);
    VO_OSMP_RETURN_CODE     SetAnewBufferingTime(int);
    LONG                    GetMinPosition();
    LONG                    GetMaxPosition();
    VO_OSMP_RETURN_CODE     EnableAudioEffect(BOOL bEnable);
    VO_OSMP_RETURN_CODE     EnableAudioStream(BOOL bEnable);
    VO_OSMP_RETURN_CODE     SetDRMVerificationInfo(char*, int,BOOL bSetByWebPage=TRUE);
    char*                   GetDRMUniqueIndentifier();
    VO_OSMP_RETURN_CODE     SetDRMUniqueIndentifier(char*);

    //Player Asset Selection
    INT                     GetVideoCount();
    INT                     GetAudioCount();
    INT                     GetSubtitleCount();
    VO_OSMP_RETURN_CODE     SelectVideo(INT index);
    VO_OSMP_RETURN_CODE     SelectAudio(INT index);
    VO_OSMP_RETURN_CODE     SelectSubtitle(INT index);
    BOOL                    IsVideoAvailable(INT index);
    BOOL                    IsAudioAvailable(INT index);
    BOOL                    IsSubtitleAvailable(INT index);
    VO_OSMP_RETURN_CODE     CommitSelection();
    VO_OSMP_RETURN_CODE     ClearSelection();
    JSON                    GetVideoProperty(INT index);
    JSON                    GetAudioProperty(INT index);
    JSON                    GetSubtitleProperty(INT index);
    JSON                    GetPlayingAsset();
    JSON                    GetCurrentSelection();

    //Player Subtitle
    VO_OSMP_RETURN_CODE     SetSubtitlePath(LPCTSTR filePath);
    VO_OSMP_RETURN_CODE     EnableSubtitle(bool value);
    VO_OSMP_RETURN_CODE     SetSubtitleFontColor(COLORREF color);
    VO_OSMP_RETURN_CODE     SetSubtitleFontOpacity(int alpha);
    VO_OSMP_RETURN_CODE     SetSubtitleFontSizeScale(int scale);
    VO_OSMP_RETURN_CODE     SetSubtitleFontBackgroundColor(COLORREF color);
    VO_OSMP_RETURN_CODE     SetSubtitleFontBackgroundOpacity(int alpha);
    VO_OSMP_RETURN_CODE     SetSubtitleWindowBackgroundColor(COLORREF color);
    VO_OSMP_RETURN_CODE     SetSubtitleWindowBackgroundOpacity(int alpha);
    VO_OSMP_RETURN_CODE     SetSubtitleFontItalic(bool enable);
    VO_OSMP_RETURN_CODE     SetSubtitleFontBold(bool enable);
    VO_OSMP_RETURN_CODE     SetSubtitleFontUnderline(bool enable);
    VO_OSMP_RETURN_CODE     SetSubtitleFontName(LPCTSTR name);
    VO_OSMP_RETURN_CODE     SetSubtitleFontEdgeType(int type);
    VO_OSMP_RETURN_CODE     SetSubtitleFontEdgeColor(COLORREF color);
    VO_OSMP_RETURN_CODE     SetSubtitleFontEdgeOpacity(int type);
    VO_OSMP_RETURN_CODE     ResetSubtitleParameter();

    VO_OSMP_RETURN_CODE     SetHTTPHeader(CHAR* headerName, CHAR* headerValue);
    VO_OSMP_RETURN_CODE     SetHTTPProxy(JSON proxy);

    VO_OSMP_RETURN_CODE     SetAudioPlaybackSpeed(FLOAT fSpeed);
    VO_OSMP_RETURN_CODE     SetInitialBufferTime(int time);


    VO_OSMP_RETURN_CODE     FullScreen();
    BOOL  	                IsFullScreen ();
    VO_OSMP_RETURN_CODE     ToggleOverlayUI(BOOL bShow);

    BOOL                    IsLiveStreaming();

    VO_OSMP_RETURN_CODE     EnableAnalytics(const int cacheTime);
    VO_OSMP_RETURN_CODE     StartAnalyticsNotification(const int interval, const char* filter);
    VO_OSMP_RETURN_CODE     StopAnalyticsNotification();
    const char*	            GetAnalytics(const char* filter);

    VO_OSMP_RETURN_CODE     Redraw(BOOL bDraw = TRUE);
    VO_OSMP_RETURN_CODE     updateViewRegion(RECT&);
    VO_OSMP_RETURN_CODE     SetDC(HDC hdc);
    VO_OSMP_RETURN_CODE     SetPresentationDelay(int time);

    VO_OSMP_RETURN_CODE     EnableLiveStreamingDVRPosition(BOOL bEnable);
    const char*             GetVideoDecodingBitrate();
    const char*             GetAudioDecodingBitrate();

    VO_OSMP_RETURN_CODE     EnableSEI(VO_OSMP_SEI_INFO_FLAG flag);
    VO_OSMP_RETURN_CODE     StartSEINotification(int interval);
    VO_OSMP_RETURN_CODE     StopSEINotification(void);
    JSON                    GetSEIInfo(LONG time, VO_OSMP_SEI_INFO_FLAG flag);

    VO_OSMP_RETURN_CODE     EnableCPUAdaptation(BOOL bEnable);

    VO_OSMP_RETURN_CODE     SetBitrateThreshold(int upper, int lower);
    VO_OSMP_RETURN_CODE     UpdateSourceURL(LPCTSTR url);
    VO_OSMP_RETURN_CODE     EnableDolbyLibrary(BOOL bEnable);

    VO_OSMP_RETURN_CODE     SetHTTPRetryTimeout(int iRetryTime);
    VO_OSMP_RETURN_CODE     SetDefaultAudioLanguage(CHAR* type,BOOL bSetByWebPage=TRUE);
    VO_OSMP_RETURN_CODE     SetDefaultSubtitleLanguage(CHAR* type,BOOL bSetByWebPage=TRUE);

public:
    static int	            OnNotifyUICommand (void * pUserData, int nID, void * pValue1, void * pValue2);//windowed modele, UI notify web

    static	int	            OnOSMPListener (void * pUserData, int nID, void * pParam1, void * pParam2);
    int                     OSMPCommonHandleEvent(int nID, void * pParam1, void * pParam2);
    int	                    OSMPWndHandleEvent (int nID, void * pParam1, void * pParam2);
    int	                    OSMPWndlessHandleEvent (int nID, void * pParam1, void * pParam2);

    int	                    notifyWebPageCommand (int nID, void * pParam1, void * pParam2);//notify webpage
    void                    notifyUIEventInfo(int nID, void * pParam1, void * pParam2);//some event need to notify UI

    static VO_OSMP_RETURN_CODE		GetVersion(VOOSMP_MODULE_VERSION* pVersion, TCHAR* pszWorkingPath);

    void                    postMessage2UI(UINT msg, WPARAM wParam, LPARAM lParam);
    void                    detectProxy();
    void                    ActiveBrowserWindow();

    VO_OSMP_RETURN_CODE     SetDRMLibrary(LPCSTR libName, LPCSTR libApiName,BOOL bSetByWebPage=TRUE);
    void                    InitVerimatrixDrmEngn();
    void                    UninitVerimatrixDrmEngn();

    void                    detectExceptions();
    BOOL                    isUrlInExceptions(LPCTSTR szUrl);
    void                    setSystemState(const int);

private:
    void                    InitInterMemberValue();		//such as:m_bMute m_nAudioVolume etc.
    BOOL                    InitOSMPPlayer();
    void                    UnInitOSMPPlayer();
    BOOL                    InitPluginUI();
    void                    UnInitPluginUI();
    void                    EraseBackGround();

    VOOSMP_SRC_PROGRAM_TYPE GetProgramType();

    void                    BeginBuffering(int nID = VOOSMP_CB_VideoStartBuff);
    void                    EndBuffering(int nID = VOOSMP_CB_VideoStopBuff);
    void                    detectFFConfigureFile();
    int                     internalSetHttpProxy();

    //some functions for callback    
    int                     cbMediaTypeChanged(void * pParam1, void * pParam2);//VOOSMP_CB_MediaTypeChanged
    int                     cbSrcAuthenticationRequest(void * pParam1, void * pParam2);

    int                     internalShakeHandWithDrmServer();
    static int              shakeHandWithDrmServerProc(VO_PTR pParam);

    void                    internalSetDrmVerfication();
    void                    internalSetPreAgreedLicense();

    //Process msg
    static LRESULT CALLBACK msgWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT	                OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    BOOL                    CreateProcessMsgWnd();

	void					getSingleInstanceController(const int sema = 1);

private:
    CvoOnStreamMP *             m_pOSMPPlayer;

    VOOSMP_LISTENERINFO         m_Listener;

    VOPUI_INIT_PARAM            m_paramUI;

    COverlayUI *                m_pOverlayUI;

    VO_PLUGINWRAP_INIT_PARAM    m_voPlugInitParam;
    HWND                        m_hWndView;

    BOOL                        m_bMute;
    INT                         m_nAudioVolume;

    voCMutex                    m_Mutex;
    char                        m_szAnalyticsInfo[1024];
    TCHAR                       m_szJSONString[1024];

    TCHAR                       m_szLicensePath[MAX_PATH];
    CHAR                        m_szLicenseString[MAX_PATH];
    CHAR                        m_szUserAgentHeaderValue[MAX_PATH];

    int                         m_nBrowserType;  // 0: IE; 1: Chrome; 2: Firefox;
    int                         m_nTrackDisabled;  // 0: none disabled; 0x001: subtitle disabled;  0x010: audio disabled;  0x100:  video disabled, 0x1000: audio effect disabled
	
	//add for stop->play, can re-play
    TCHAR                       m_szOpenUrl[2048];
    BOOL                        m_bStopped;
    VO_OSMP_SRC_FLAG            m_OpenFlag;
    VO_OSMP_SRC_FORMAT          m_OpenType;

    VOOSMP_RENDER_TYPE          m_typeRender;
    BOOL                        m_bProxySetup;
    VOOSMP_SRC_HTTP_PROXY       m_proxy;

    CString                     m_cfgFirefox;
    VOFFPROXY                   m_ffProxy;

    CStringA                    m_strDRMLibName;
    CStringA                    m_strDRMApiName;

	//add workaround code to load voDRM_Verimatrix_AES128.dll
    HMODULE                     m_hVODRM;
    char                        m_szDRMVerificationInfo[MAX_PATH];

    VO_PTR                      m_pDrmAdapter;
    VO_DRM2_API                 m_apiDRM;

    int                         m_nDrmShakingHand;
    //CVoQueue<int>             m_drmThreadQueue;
    static	TCHAR               m_szVersion[MAX_PATH];

    std::vector<CString>        m_vecStrExceptions;

    HPOWERNOTIFY                m_hPowerNotify;
    HWND                        m_hwndProcessMsg;

	// removed temporarily
	BOOL						m_bSemaphoreGotten; 
	HANDLE						m_hSemaSingleInstanceCtrl;
    HANDLE                      m_hSemaVerimatrixDrmInitCrl;
    HANDLE                      m_hSemaVerimatrixDrmShakeCrl;
    int                         m_iHttpRetryTime;

    CHAR                        m_szDefAudioLan[32];
    CHAR                        m_szDefSubLan[32];
};

#endif // __COSMPEngnWrap_H__

