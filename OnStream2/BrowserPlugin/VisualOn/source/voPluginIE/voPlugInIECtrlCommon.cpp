#ifndef WINDOWLESS
#include "..\voPlugInIE\stdafx.h"
#include "..\voPlugInIE\voPlugInIECtrl.h"
#else
#include "..\voPlugInIEWinless\stdafx.h"
#include "..\voPlugInIEWinless\voPlugInIECtrl.h"
#endif

#include "voOSFunc.h"
#include "vompType.h"
#include "voOnStreamSourceType.h"
#include "..\Common\voJSON.h"

/******************* below: To make the plugin is a Safe obj  ******************************/
ULONG FAR EXPORT CvoPlugInIECtrl::XMyObjSafe::AddRef()
{  
  METHOD_PROLOGUE(CvoPlugInIECtrl, MyObjSafe) 
    return pThis->ExternalAddRef();
}  

ULONG FAR EXPORT CvoPlugInIECtrl::XMyObjSafe::Release()
{
	METHOD_PROLOGUE(CvoPlugInIECtrl, MyObjSafe)
	VOLOGI("here do release....");

	return pThis->ExternalRelease();
}

HRESULT FAR EXPORT CvoPlugInIECtrl::XMyObjSafe::QueryInterface(REFIID iid, void FAR* FAR* ppvObj)
{
  METHOD_PROLOGUE(CvoPlugInIECtrl, MyObjSafe)
    return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

//make the obj safe
const DWORD g_dwSupportedBits = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
const DWORD g_dwNotSupportedBits = ~g_dwSupportedBits;
HRESULT FAR EXPORT CvoPlugInIECtrl::XMyObjSafe::GetInterfaceSafetyOptions(REFIID riid,DWORD __RPC_FAR *pdwSupportedOptions, DWORD __RPC_FAR *pdwEnabledOptions)
{  
  METHOD_PROLOGUE(CvoPlugInIECtrl, MyObjSafe)
    *pdwSupportedOptions = *pdwEnabledOptions = g_dwSupportedBits;
  return S_OK;
}

HRESULT FAR EXPORT CvoPlugInIECtrl::XMyObjSafe::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
{
  METHOD_PROLOGUE(CvoPlugInIECtrl, MyObjSafe)

    if (dwOptionSetMask & g_dwNotSupportedBits)
    {
      return CO_E_NOT_SUPPORTED;
    }

    return S_OK;
}
/******************* up: To make the plugin is a Safe obj  ******************************/

/******************* below: Plugin Callback functions  ******************************/
int  PluginCallBack(void * pUserData, int nID, void * pParam1, void * pParam2)
{
  CvoPlugInIECtrl * plugin=(CvoPlugInIECtrl*)pUserData;
  if (plugin != NULL)
    plugin->notify(nID,pParam1,pParam2);
  return 0;
}

void ParseVOOSMP_SRC_CB_IO_HTTP_Download_Failed(OUT VOCBMSG &cbMsg,IN void * pParam1,IN void * pParam2)
{
    cbMsg.nID = VOOSMP_SRC_CB_IO_HTTP_Download_Failed;
    cbMsg.nValue1 = 0;
    cbMsg.nValue2 = 0;
    cbMsg.setJSONObject(NULL);
    if (pParam1==NULL || pParam2 == NULL)
    {
        return;
    }

    VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION *description = (VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION *)pParam2;

    voJSON *pvi = voJSON_CreateObject();
    if (pvi == NULL)
        return;

    voJSON_AddStringToObject(pvi, "URL", (CHAR *)pParam1);
    voJSON_AddNumberToObject(pvi, "reason", description->reason);
    voJSON_AddStringToObject(pvi, "response", description->pszResponse);

    char* szval = voJSON_Print(pvi);
    cbMsg.setJSONObject(szval);

    free(szval);
    voJSON_Delete(pvi);
}

void ParseVOOSMP_SRC_CB_Adaptive_Streaming_Info(OUT VOCBMSG &cbMsg,IN void * pParam1,IN void * pParam2)
{
    cbMsg.nID = VOOSMP_SRC_CB_Adaptive_Streaming_Info;
    if(pParam1)
        cbMsg.nValue1 = *(int*)pParam1;
    if(pParam2)
        cbMsg.nValue2 = *(int*)pParam2;
    cbMsg.setJSONObject(NULL);
    if (pParam1==NULL || pParam2 == NULL)
    {
        return;
    }

    switch(cbMsg.nValue1)
    {
    case VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD:
    case VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED:
    case VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK:
        {
            cbMsg.nValue2 = 0;

            VOOSMP_SRC_CHUNK_INFO *chunkInfo = (VOOSMP_SRC_CHUNK_INFO *)pParam2;
            voJSON *pvi = voJSON_CreateObject();
            if (pvi == NULL)
                return;

            voJSON_AddNumberToObject(pvi, "type", chunkInfo->nType);
            voJSON_AddStringToObject(pvi, "rootURL", chunkInfo->szRootUrl);
            voJSON_AddStringToObject(pvi, "URL", chunkInfo->szUrl);
            voJSON_AddNumberToObject(pvi, "startTime", chunkInfo->ullStartTime);
            voJSON_AddNumberToObject(pvi, "duration", chunkInfo->ullDuration);
            voJSON_AddNumberToObject(pvi, "timeScale", chunkInfo->ullTimeScale);

            char* szval = voJSON_Print(pvi);
            cbMsg.setJSONObject(szval);

            free(szval);
            voJSON_Delete(pvi);
        }
        break;
    default:
        break;
    }
}

void CvoPlugInIECtrl::ParseToMsg(OUT VOCBMSG &cbMsg,IN int nID,IN void * pParam1,IN void * pParam2)
{
    cbMsg.nID = nID;
    if(pParam1)
        cbMsg.nValue1 = *(int*)pParam1;
    if(pParam2)
        cbMsg.nValue2 = *(int*)pParam2;

    switch(nID)
    {
    case VOOSMP_SRC_CB_IO_HTTP_Download_Failed:
        {
            ParseVOOSMP_SRC_CB_IO_HTTP_Download_Failed(cbMsg,pParam1,pParam2);
        }
        break;
    case VOOSMP_SRC_CB_Adaptive_Streaming_Info:
        {
            ParseVOOSMP_SRC_CB_Adaptive_Streaming_Info(cbMsg,pParam1,pParam2);
        }
        break;
    default:
        break;
    }
}

void CvoPlugInIECtrl::notify(int nID, void * pParam1, void * pParam2)
{
#ifdef WINDOWLESS
  if(IsWindowLess())
  {
    if(nID == VOMP_CB_VideoReadyToRender)
    {
      if(m_bDDRraw)
        return;

      ::PostMessage(m_hwndRenderMsg,WM_USERDEFNOTIFY,NULL,NULL);
      return;
    }
  }
#endif

  VOCBMSG cbMsg;
  ParseToMsg(cbMsg,nID,pParam1,pParam2);

  voCAutoLock lock (&m_mtMsgQueue);
  if (cbMsg.nID == VO_OSMP_CB_WINDOWED_UI_COMMAND || cbMsg.nID == VOOSMP_CB_VideoRenderStart || cbMsg.nID == VOOSMP_CB_AudioRenderStart)
	  m_msgQueue.insert2Head(cbMsg);
  else
	  m_msgQueue.enqueue(cbMsg);

  return;
}

int	CvoPlugInIECtrl::msgHandlerProc(VO_PTR pParam) {

	if (pParam == NULL)
		return 0;

	CvoPlugInIECtrl* p = (CvoPlugInIECtrl*)pParam;
	p->msgHandlerLoop();

	return 0;
}

int	CvoPlugInIECtrl::msgHandlerLoop() 
{

  int localcounter = 0;
  bool bSaverForbidden = false;

  while (!m_bExitMsgLoop) {

    localcounter++;

    // detect proxy per 2s
    if (localcounter >= 20) {

      if (m_pOSMPEngnWrap) 
	  {
        m_pOSMPEngnWrap->detectProxy();
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
    if (0==size)
    {
      voOS_Sleep(100);
      continue;
    }

    if (m_msgQueue.getCount() && NULL!=m_pDispatch)
    {
#ifndef WINDOWLESS
        ::PostMessage(m_hWnd,WM_NOTIFY_CALLBACK,0,0);
#else
        ::PostMessage(m_hwndRenderMsg,WM_NOTIFY_CALLBACK,0,0);
#endif
    }

    voOS_Sleep(100);
  }

  VOLOGI("plugin message loop thread exit normally.");
  return 0;
}

void CvoPlugInIECtrl::procNotifyCallBackMsg()
{
      //if (m_bNeedClearMsgQueue)
      //{
      //  voCAutoLock lock (&m_mtMsgQueue);
      //  m_msgQueue.clearAll();
      //  m_bNeedClearMsgQueue = FALSE;
      //  continue;
      //}

      VOCBMSG msg;
      int ret = VOQU_OK;
      {
        voCAutoLock lock (&m_mtMsgQueue);
        ret = m_msgQueue.dequeue(&msg);
      }
      if (VOQU_ERR != ret && (msg.nID == VO_OSMP_CB_WINDOWED_UI_COMMAND || m_nStatus != 5))
        NotifyCallbackJS(msg);
    }

void CvoPlugInIECtrl::NotifyCallbackJS(VOCBMSG &msg)
{
  if(NULL == m_pDispatch)
    return;

  VARIANTARG varPara[4];
  varPara[0].vt   = VT_BSTR;
  varPara[0].bstrVal = NULL;
  varPara[1].vt   = VT_UI4; 
  varPara[1].ulVal = msg.nValue2;
  varPara[2].vt   = VT_UI4;   
  varPara[2].ulVal = msg.nValue1;
  varPara[3].vt   = VT_UI4;  
  varPara[3].ulVal = msg.nID;

  CString strResult;
  if (msg.getJSONObject())
  {
      strResult = msg.getJSONObject();
      varPara[0].bstrVal = strResult.AllocSysString();
  }

  if (m_pDispatch)
  {
    DISPPARAMS dpNoArgs = {varPara, NULL, 4, 0};
    CComVariant varDisp;
    HRESULT hr = m_pDispatch->Invoke((DISPID)DISPID_VALUE,IID_NULL,0,DISPATCH_METHOD,&dpNoArgs,&varDisp,NULL,NULL);

    if(SUCCEEDED(hr))
      return;
  }
}
/******************* up: Plugin Callback functions  ******************************/

/****************  below: Wrap common functions    *******************************/
void CvoPlugInIECtrl::InitMembersValues()
{
  m_pOSMPEngnWrap=NULL;
  TCHAR szVersion[MAX_PATH] = _T("");
  GetVersion(szVersion,MAX_PATH,VOOSMP_MODULE_TYPE_SDK);
  m_Version = szVersion;

  m_hwndBrowser = NULL;

  m_bExitMsgLoop = FALSE;
  m_hMsgLoop     = NULL;
  m_msgQueue.setCapability(MAX_MSG_COUNT);
  m_msgQueue.clearAll();
  m_bNeedClearMsgQueue = FALSE;

  m_nStatus = 0;
}

COSMPEngnWrap * CvoPlugInIECtrl::GetOSMPEngnWrap()
{
  return m_pOSMPEngnWrap;
}

HWND CvoPlugInIECtrl::GetBrowserHwnd()
{
  HWND hWnd = NULL;

  if(IsWindowLess() && m_pClientSite)
  {
    IOleInPlaceSite *pOleInPlaceSite = NULL;
    if (S_OK == m_pClientSite->QueryInterface(IID_IOleInPlaceSite, (LPVOID *)&pOleInPlaceSite))
    {
      pOleInPlaceSite->GetWindow(&hWnd);
      pOleInPlaceSite->Release();
    }
  }
  else
  {
    hWnd = GetSafeHwnd();
  }

  VOLOGI("the browser window is %d", hWnd);

  return hWnd;
}

BOOL CvoPlugInIECtrl::InitOSMPEngnWrap()
{
  TCHAR szAppPath[MAX_PATH] = _T("");
  GetModuleFileName(AfxGetInstanceHandle(),szAppPath, MAX_PATH);
  VO_PTCHAR pPos = vostrrchr(szAppPath, _T('/'));
  if (pPos == NULL)
    pPos = vostrrchr(szAppPath, _T('\\'));
  VO_S32 nPos = pPos - szAppPath;
  szAppPath[nPos+1] = _T('\0');

  HWND hWnd = GetBrowserHwnd();
  if (m_pOSMPEngnWrap && m_hwndBrowser == hWnd)
  {
    return TRUE;
  }

  m_hwndBrowser = hWnd;

  VO_PLUGINWRAP_INIT_PARAM voInitParam;
  voInitParam.pListener = PluginCallBack;
  voInitParam.pUserData = this;
  voInitParam.hView = m_hwndBrowser;
  voInitParam.bWindowless = IsWindowLess();

  _tcscpy(voInitParam.szWorkPath,szAppPath);

  if(NULL==m_pOSMPEngnWrap)
    m_pOSMPEngnWrap = new COSMPEngnWrap ();

  if(m_pOSMPEngnWrap)
    m_pOSMPEngnWrap->Init(&voInitParam);

  return m_pOSMPEngnWrap!=NULL;
}

void CvoPlugInIECtrl::UnInitOSMPEngnWrap()
{
    if (NULL == m_pOSMPEngnWrap)
    {
        return;
    }

  m_bNeedClearMsgQueue = TRUE;
  m_bExitMsgLoop = true;
  voOS_Sleep(100);
  VO_U32 uRC=0;
  if(m_hMsgLoop)
    voThreadClose(m_hMsgLoop,uRC);
  if (uRC==0)
  {
    m_hMsgLoop = NULL;
  }

  if (m_pOSMPEngnWrap != NULL)
  {
    m_pOSMPEngnWrap->Uninit();

    delete m_pOSMPEngnWrap;
    m_pOSMPEngnWrap = NULL;
  }

  SetThreadExecutionState(ES_CONTINUOUS);
}
/****************  up: Wrap common functions           ***************************/

/****************  below: Wrap handle event functions  ***************************/
ULONG CvoPlugInIECtrl::open(LPCTSTR cstrUrl, ULONG flag, ULONG sourceType, LPCTSTR openParam)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (NULL==cstrUrl || _tcslen(cstrUrl)<=0)
    {
      return VO_OSMP_ERR_POINTER;
    }

    //must make sure the browser window set into MP, before open a url, the webpage will show the object.
    if(NULL == m_pOSMPEngnWrap || m_hwndBrowser==NULL)
      InitOSMPEngnWrap();


    if( !IsWindowLess() && (GetSafeHwnd()!=m_hwndBrowser) )//ActiveX atl window maybe changed
    {
      VOLOGI("ATL Window changed!");
      InitOSMPEngnWrap();
    }

    m_bNeedClearMsgQueue = TRUE;
	m_nStatus = 1;
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;    

    // TODO: Add your dispatch handler code here
    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_NONE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->Open (cstrUrl,(VO_OSMP_SRC_FLAG)flag,(VO_OSMP_SRC_FORMAT)sourceType,0);

	if (iRet == VO_OSMP_ERR_NONE)
		m_nStatus = 2;

  m_bNeedClearMsgQueue = FALSE;
				
    return (ULONG)iRet;
}

ULONG CvoPlugInIECtrl::start(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

  // TODO: Add your dispatch handler code here

	VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_NONE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->Start();

  if (iRet ==  VO_OSMP_ERR_NONE)
	  m_nStatus = 3;
	
  return iRet;
}

ULONG CvoPlugInIECtrl::pause(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    // TODO: Add your dispatch handler code here
	VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_NONE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->Pause();
	
	if (iRet == VO_OSMP_ERR_NONE)
		m_nStatus = 4;

    return iRet;
}

LONG CvoPlugInIECtrl::getDuration(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    // TODO: Add your dispatch handler code here
    LONG lDuration = 0;
	if (m_pOSMPEngnWrap)
		lDuration = m_pOSMPEngnWrap->GetDuration();

    return lDuration;
}

ULONG CvoPlugInIECtrl::close(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    // TODO: Add your dispatch handler code here
    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_NONE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->Close();

  m_nStatus = 0;
  m_bNeedClearMsgQueue = TRUE;

  if(IsWindowLess())
  {
    Refresh();
  }

  return iRet;
}

ULONG CvoPlugInIECtrl::stop(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    // TODO: Add your dispatch handler code here
	VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_NONE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->Stop();

  m_nStatus = 5;
  m_bNeedClearMsgQueue = TRUE;

  if(IsWindowLess())
  {
    Refresh();
  }

  return iRet;
}

LONG CvoPlugInIECtrl::setPosition(LONG lMSec)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    // TODO: Add your dispatch handler code here
    LONG lCurPos = 0;
	if (m_pOSMPEngnWrap)
		lCurPos = m_pOSMPEngnWrap->SetPosition(lMSec);

    return lCurPos;
}

LONG CvoPlugInIECtrl::getPosition(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    // TODO: Add your dispatch handler code here
    LONG lCurPos = 0;
	if (m_pOSMPEngnWrap)
		lCurPos = m_pOSMPEngnWrap->GetPosition();

    return lCurPos;
}

ULONG CvoPlugInIECtrl::getPlayerStatus(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    // TODO: Add your dispatch handler code here
    VO_OSMP_STATUS nStatus = VO_OSMP_STATUS_MAX;
	if (m_pOSMPEngnWrap)
		nStatus = m_pOSMPEngnWrap->GetPlayerStatus();
    return (ULONG)nStatus;
}

VARIANT_BOOL CvoPlugInIECtrl::canBePaused(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return FALSE;

    // TODO: Add your dispatch handler code here
	VARIANT_BOOL bpaused = VARIANT_FALSE;
	if (m_pOSMPEngnWrap)
		bpaused = m_pOSMPEngnWrap->CanBePaused();
    return bpaused;
}

ULONG CvoPlugInIECtrl::setVolume(FLOAT fLeftVol, FLOAT fRightVol)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    // TODO: Add your dispatch handler code here
    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->SetVolume(fLeftVol,fRightVol);

    return iRet;
}

ULONG CvoPlugInIECtrl::FullScreen(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    // TODO: Add your dispatch handler code here
    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->FullScreen();

  if (IsWindowLess())
  {
    ::PostMessage(m_hwndBrowser,WM_KEYDOWN,VK_F11,0X00570001);
    ::PostMessage(m_hwndBrowser,WM_KEYUP,VK_F11,0XC0570001);
  }

    return iRet;
}


ULONG CvoPlugInIECtrl::mute(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->Mute();

    return iRet;
}

ULONG CvoPlugInIECtrl::unmute(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->UnMute();

    return iRet;
}

ULONG CvoPlugInIECtrl::setVideoAspectRatio(ULONG ar)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->SetVideoAspectRatio((VO_OSMP_ASPECT_RATIO)ar);

    return iRet;
}

ULONG CvoPlugInIECtrl::setLicenseContent(LPCTSTR data)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet= m_pOSMPEngnWrap->SetLicenseContent(data);

    return iRet;
}

ULONG CvoPlugInIECtrl::setPreAgreedLicense(LPCTSTR str)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    CHAR szMuChar[MAX_PATH] = "";
    WideCharToMultiByte(CP_ACP,NULL,str,-1,szMuChar,MAX_PATH,NULL,FALSE);

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
    if (m_pOSMPEngnWrap)
      iRet = m_pOSMPEngnWrap->SetPreAgreedLicense(szMuChar);

    return iRet;
}

ULONG CvoPlugInIECtrl::setLicenseFilePath(LPCTSTR path)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->SetLicenseFilePath(path);

    return iRet;
}

ULONG CvoPlugInIECtrl::enableDeblock(VARIANT_BOOL bEnable)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->EnableDeblock(bEnable);

    return iRet;
}

ULONG CvoPlugInIECtrl::setDeviceCapabilityByFile(LPCTSTR filePath)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->SetDeviceCapabilityByFile(filePath);

    return iRet;
}

ULONG CvoPlugInIECtrl::setInitialBitrate(ULONG bitrate)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->SetInitialBitrate(bitrate);

    return iRet;
}

LONG CvoPlugInIECtrl::getMinPosition(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    LONG lMinPos = 0;
	if (m_pOSMPEngnWrap)
		lMinPos = m_pOSMPEngnWrap->GetMinPosition();

    return lMinPos;
}

LONG CvoPlugInIECtrl::getMaxPosition(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    LONG lMaxPos = 0;
	if (m_pOSMPEngnWrap)
		lMaxPos = m_pOSMPEngnWrap->GetMaxPosition();

    return lMaxPos;
}

ULONG CvoPlugInIECtrl::getVideoCount(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    ULONG lNum = 0;
	if (m_pOSMPEngnWrap)
		lNum = m_pOSMPEngnWrap->GetVideoCount();

    return lNum;
}

ULONG CvoPlugInIECtrl::getAudioCount(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    ULONG lNum = 0;
	if (m_pOSMPEngnWrap)
		lNum = m_pOSMPEngnWrap->GetAudioCount();

    return lNum;
}

ULONG CvoPlugInIECtrl::getSubtitleCount(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    ULONG lNum = 0;
	if (m_pOSMPEngnWrap)
		lNum = m_pOSMPEngnWrap->GetSubtitleCount();

    return lNum;
}

ULONG CvoPlugInIECtrl::selectVideo(ULONG index)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->SelectVideo(index);

    return iRet;
}

ULONG CvoPlugInIECtrl::selectAudio(ULONG index)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->SelectAudio(index);

    return iRet;
}

ULONG CvoPlugInIECtrl::selectSubtitle(ULONG index)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->SelectSubtitle(index);

    return iRet;
}

VARIANT_BOOL CvoPlugInIECtrl::isVideoAvailable(ULONG index)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return FALSE;

    BOOL bRet = FALSE;
	if (m_pOSMPEngnWrap)
		bRet = m_pOSMPEngnWrap->IsVideoAvailable(index);

    return bRet;
}

VARIANT_BOOL CvoPlugInIECtrl::isAudioAvailable(ULONG index)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return FALSE;

    BOOL bRet = FALSE;
	if (m_pOSMPEngnWrap)
		bRet = m_pOSMPEngnWrap->IsAudioAvailable(index);

    return bRet;
}

VARIANT_BOOL CvoPlugInIECtrl::isSubtitleAvailable(ULONG index)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return FALSE;

    BOOL bRet = FALSE;
	if (m_pOSMPEngnWrap)
		bRet = m_pOSMPEngnWrap->IsSubtitleAvailable(index);

    return bRet;
}

ULONG CvoPlugInIECtrl::commitSelection(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->CommitSelection();

    return iRet;
}

ULONG CvoPlugInIECtrl::clearSelection(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return VO_OSMP_ERR_UNINITIALIZE;

    VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
	if (m_pOSMPEngnWrap)
		iRet = m_pOSMPEngnWrap->ClearSelection();

    return iRet;
}

BSTR CvoPlugInIECtrl::getVideoProperty(ULONG index)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult;

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return strResult.AllocSysString();

    JSON jsonPrpy = _T("");
	if (m_pOSMPEngnWrap)
		jsonPrpy = m_pOSMPEngnWrap->GetVideoProperty(index);
    strResult = jsonPrpy;

    return strResult.AllocSysString();
}

BSTR CvoPlugInIECtrl::getAudioProperty(ULONG index)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult;

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return strResult.AllocSysString();

    JSON jsonPrpy = _T("");
	if (m_pOSMPEngnWrap)
		jsonPrpy = m_pOSMPEngnWrap->GetAudioProperty(index);
    strResult = jsonPrpy;

    return strResult.AllocSysString();
}

BSTR CvoPlugInIECtrl::getSubtitleProperty(ULONG index)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult;

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return strResult.AllocSysString();

    JSON jsonPrpy = _T("");
	if (m_pOSMPEngnWrap)
		jsonPrpy = m_pOSMPEngnWrap->GetSubtitleProperty(index);
    strResult = jsonPrpy;

    return strResult.AllocSysString();
}

BSTR CvoPlugInIECtrl::getPlayingAsset()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult;

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return strResult.AllocSysString();

    JSON jsonPrpy = _T("");
	if (m_pOSMPEngnWrap)
		jsonPrpy = m_pOSMPEngnWrap->GetPlayingAsset();
    strResult = jsonPrpy;

    return strResult.AllocSysString();
}

ULONG CvoPlugInIECtrl::getVolume()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (NULL == m_pOSMPEngnWrap)
        return 0;

	ULONG lvol = 0;
	if (m_pOSMPEngnWrap)
		lvol = m_pOSMPEngnWrap->GetVolume();

    return lvol;
}

ULONG CvoPlugInIECtrl::setSubtitlePath(LPCTSTR filePath)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitlePath(filePath);

  return iRet;
}

ULONG CvoPlugInIECtrl::enableSubtitle(VARIANT_BOOL value)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->EnableSubtitle(value);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontColor(ULONG color)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontColor(color);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontOpacity(ULONG alpha)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontOpacity(alpha);
  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontSizeScale(ULONG scale)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;
  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontSizeScale(scale);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontBackgroundColor(ULONG color)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontBackgroundColor(color);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontBackgroundOpacity(ULONG alpha)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;

  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontBackgroundOpacity(alpha);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleWindowBackgroundColor(ULONG color)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleWindowBackgroundColor(color);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleWindowBackgroundOpacity(ULONG alpha)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleWindowBackgroundOpacity(alpha);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontItalic(VARIANT_BOOL enable)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontItalic(enable);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontBold(VARIANT_BOOL enable)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontBold(enable);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontUnderline(VARIANT_BOOL enable)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontUnderline(enable);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontName(LPCTSTR name)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontName(name);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontEdgeType(ULONG type)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontEdgeType(type);

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontEdgeOpacity(ULONG type)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;


  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontEdgeOpacity(type);

  return iRet;
}

ULONG CvoPlugInIECtrl::resetSubtitleParameter(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->ResetSubtitleParameter();

  return iRet;
}

ULONG CvoPlugInIECtrl::setSubtitleFontEdgeColor(ULONG color)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return VO_OSMP_ERR_UNINITIALIZE;

  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
	  iRet = m_pOSMPEngnWrap->SetSubtitleFontEdgeColor(color);

  return iRet;
}


void CvoPlugInIECtrl::GetVersion(LPTSTR szVersion, int iStrLength, int moduleType)
{
	TCHAR szAppPath[MAX_PATH] = _T("");
	GetModuleFileName(AfxGetInstanceHandle(),szAppPath, MAX_PATH);
	VO_PTCHAR pPos = vostrrchr(szAppPath, _T('/'));
	if (pPos == NULL)
		pPos = vostrrchr(szAppPath, _T('\\'));
	VO_S32 nPos = pPos - szAppPath;
	szAppPath[nPos+1] = _T('\0');

	VOOSMP_MODULE_VERSION version;
	version.pszVersion = NULL;
	version.nModuleType = moduleType;
	int nRet = COSMPEngnWrap::GetVersion(&version, szAppPath);

	if(nRet!=0 || !version.pszVersion)
		return;

	_tcscpy(szVersion, (TCHAR*)version.pszVersion);
}

ULONG CvoPlugInIECtrl::setHTTPHeader(LPCTSTR headerName, LPCTSTR headerValue)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
  {
    CHAR szHeaderName[MAX_PATH] = "";
    WideCharToMultiByte(CP_ACP,NULL,headerName,-1,szHeaderName,MAX_PATH,NULL,FALSE);
    CHAR szHeaderValue[MAX_PATH] = "";
    WideCharToMultiByte(CP_ACP,NULL,headerValue,-1,szHeaderValue,MAX_PATH,NULL,FALSE);
    iRet = m_pOSMPEngnWrap->SetHTTPHeader(szHeaderName,szHeaderValue);
  }

  return iRet;
}

ULONG CvoPlugInIECtrl::setHTTPProxy(LPCTSTR proxy)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
    iRet = m_pOSMPEngnWrap->SetHTTPProxy(proxy);

  return iRet;
}

ULONG CvoPlugInIECtrl::EnableAnalytics(LONG cacheTime)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
    iRet = m_pOSMPEngnWrap->EnableAnalytics(cacheTime);

  return iRet;
}

ULONG CvoPlugInIECtrl::StartAnalyticsNotification(LONG interval, LPCTSTR filter)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
  {
    CHAR szTemp[MAX_PATH] = "";
    WideCharToMultiByte(CP_ACP,NULL,filter,-1,szTemp,MAX_PATH,NULL,FALSE);
    iRet = m_pOSMPEngnWrap->StartAnalyticsNotification(interval,szTemp);
  }

  return iRet;
}

ULONG CvoPlugInIECtrl::StopAnalyticsNotification(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
    iRet = m_pOSMPEngnWrap->StopAnalyticsNotification();

  return iRet;
}

BSTR CvoPlugInIECtrl::GetAnalytics(LPCTSTR filter)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  CString strResult;

  // TODO: Add your dispatch handler code here
  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
  {
    CHAR szTemp[MAX_PATH] = "";
    WideCharToMultiByte(CP_ACP,NULL,filter,-1,szTemp,MAX_PATH,NULL,FALSE);
    strResult = m_pOSMPEngnWrap->GetAnalytics(szTemp);
  }

  return strResult.AllocSysString();
}

ULONG CvoPlugInIECtrl::setAudioPlaybackSpeed(FLOAT fSpeed)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  VO_OSMP_RETURN_CODE iRet = VO_OSMP_ERR_UNINITIALIZE;
  if (m_pOSMPEngnWrap)
    iRet = m_pOSMPEngnWrap->SetAudioPlaybackSpeed(fSpeed);

  return iRet;
}

VARIANT_BOOL CvoPlugInIECtrl::isLiveStreaming(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
    return m_pOSMPEngnWrap->IsLiveStreaming();

  return FALSE;
}

VARIANT_BOOL CvoPlugInIECtrl::isFullScreen(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
    return m_pOSMPEngnWrap->IsFullScreen();

  return FALSE;
}

ULONGLONG CvoPlugInIECtrl::setPresentationDelay(ULONGLONG time)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if(m_pOSMPEngnWrap)
    return m_pOSMPEngnWrap->SetPresentationDelay(time);

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONGLONG CvoPlugInIECtrl::toggleOverlayUI(VARIANT_BOOL shown)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here

#ifdef WINDOWLESS
  if (FALSE == m_bHybridMode)
  {
    VOLOGE("do nothing!");
    return VO_OSMP_ERR_NONE;
  }

/*
  if (m_pOSMPEngnWrap->GetIsCreateDDdrawFailed())
  {
    return 0;
  }
*/

  m_bDDRraw = shown!=0 ? FALSE : TRUE;

  if(shown==0)
  {
    VOLOGI("post message to toggle UI: %d", shown);
    ::PostMessage(m_hwndRenderMsg,WM_CHANGE_TO_DDDRAW,NULL,NULL);
    return 0;
  }
#endif

  int nRC = VO_OSMP_ERR_UNINITIALIZE;
  if(m_pOSMPEngnWrap)
    nRC = m_pOSMPEngnWrap->ToggleOverlayUI(shown);

  return nRC;
}

ULONG CvoPlugInIECtrl::registerEventHandler(IDispatch* pDispatch)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  m_pDispatch = pDispatch;


  if(NULL == m_hMsgLoop)
  {
    VO_U32 tID = 0;
    voThreadCreate(&m_hMsgLoop, &tID, (voThreadProc)msgHandlerProc, this, 0);
    VOLOGI("m_hMsgLoop: %p", m_hMsgLoop);
  }

  return 0;
}


ULONG CvoPlugInIECtrl::setOnEventHandler(IDispatch* pDispatch)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  return registerEventHandler(pDispatch);
}

BSTR CvoPlugInIECtrl::getCurrentSelection(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  CString strResult;

  // TODO: Add your dispatch handler code here
  if (NULL == m_pOSMPEngnWrap)
    return strResult.AllocSysString();

  JSON jsonPrpy = _T("");
  if (m_pOSMPEngnWrap)
    jsonPrpy = m_pOSMPEngnWrap->GetCurrentSelection();
  strResult = jsonPrpy;

  return strResult.AllocSysString();
}

ULONGLONG CvoPlugInIECtrl::setAnewBufferingTime(LONG milliseconds)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  int nRC = VO_OSMP_ERR_UNINITIALIZE;
  if(m_pOSMPEngnWrap)
    nRC = m_pOSMPEngnWrap->SetAnewBufferingTime(milliseconds);

  return nRC;
}


ULONG CvoPlugInIECtrl::enableAudioStream(VARIANT_BOOL bEnable)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
    return m_pOSMPEngnWrap->EnableAudioStream(bEnable);

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::enableAudioEffect(VARIANT_BOOL bEnable)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
    return m_pOSMPEngnWrap->EnableAudioEffect(bEnable);

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::setDRMVerificationInfo(LPCTSTR info)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if(m_pOSMPEngnWrap)
  {
    char szMulitPath[MAX_PATH];
    WideCharToMultiByte(CP_ACP,NULL,info,-1,szMulitPath,MAX_PATH,NULL,FALSE);
    m_pOSMPEngnWrap->SetDRMVerificationInfo(szMulitPath,_tcslen(info));
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}

BSTR CvoPlugInIECtrl::getDRMUniqueIndentifier(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  CString strResult;

  // TODO: Add your dispatch handler code here
  if(m_pOSMPEngnWrap)
  {
    strResult = m_pOSMPEngnWrap->GetDRMUniqueIndentifier();
  }

  return strResult.AllocSysString();
}

ULONG CvoPlugInIECtrl::setDRMUniqueIndentifier(LPCTSTR pid)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if(m_pOSMPEngnWrap)
  {
    char szMulitPath[MAX_PATH];
    WideCharToMultiByte(CP_ACP,NULL,pid,-1,szMulitPath,MAX_PATH,NULL,FALSE);
    return m_pOSMPEngnWrap->SetDRMUniqueIndentifier(szMulitPath);
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::setDRMUniqueIdentifier(LPCTSTR pid)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if(m_pOSMPEngnWrap)
  {
    char szMulitPath[MAX_PATH];
    WideCharToMultiByte(CP_ACP,NULL,pid,-1,szMulitPath,MAX_PATH,NULL,FALSE);
    return m_pOSMPEngnWrap->SetDRMUniqueIndentifier(szMulitPath);
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}

BSTR CvoPlugInIECtrl::getDRMUniqueIdentifier(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  CString strResult;

  // TODO: Add your dispatch handler code here
  if(m_pOSMPEngnWrap)
  {
    strResult = m_pOSMPEngnWrap->GetDRMUniqueIndentifier();
  }

  return strResult.AllocSysString();
}

ULONG CvoPlugInIECtrl::setDRMLibrary(LPCTSTR libName, LPCTSTR libApiName)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    char szLibName[MAX_PATH];
    WideCharToMultiByte(CP_ACP,NULL,libName,-1,szLibName,MAX_PATH,NULL,FALSE);
    char szApiName[MAX_PATH];
    WideCharToMultiByte(CP_ACP,NULL,libApiName,-1,szApiName,MAX_PATH,NULL,FALSE);
    return m_pOSMPEngnWrap->SetDRMLibrary(szLibName,szApiName);
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}


ULONG CvoPlugInIECtrl::getParameter(ULONG nID, BYTE* pParam)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::setParameter(ULONG nID, BYTE* pParam)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::init(ULONG engineType, LPCTSTR initParam)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::setView(BYTE* pView)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::setDisaplayArea(BYTE* pArea)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::suspend(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::resume(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::enableLiveStreamingDVRPosition(VARIANT_BOOL enable)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    return m_pOSMPEngnWrap->EnableLiveStreamingDVRPosition(enable);
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}

BSTR CvoPlugInIECtrl::getAudioDecodingBitrate(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  CString strResult;

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    strResult = m_pOSMPEngnWrap->GetAudioDecodingBitrate();
  }

  return strResult.AllocSysString();
}

BSTR CvoPlugInIECtrl::getVideoDecodingBitrate(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  CString strResult;

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    strResult = m_pOSMPEngnWrap->GetVideoDecodingBitrate();
  }

  return strResult.AllocSysString();
}

ULONG CvoPlugInIECtrl::setInitialBufferTime(LONG time)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    return m_pOSMPEngnWrap->SetInitialBufferTime(time);
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::enableSEI(ULONG flag)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    return m_pOSMPEngnWrap->EnableSEI((VO_OSMP_SEI_INFO_FLAG)flag);
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::startSEINotification(LONG interval)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    return m_pOSMPEngnWrap->StartSEINotification(interval);
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::stopSEINotification(void)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    return m_pOSMPEngnWrap->StopSEINotification();
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}

BSTR CvoPlugInIECtrl::getSEIInfo(LONG time, ULONG flag)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  CString strResult;

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    JSON jsonPrpy = _T("");
    if (m_pOSMPEngnWrap)
      jsonPrpy = m_pOSMPEngnWrap->GetSEIInfo(time, (VO_OSMP_SEI_INFO_FLAG)flag);;
    strResult = jsonPrpy;
  }

  return strResult.AllocSysString();
}

ULONG CvoPlugInIECtrl::enableCPUAdaptation(VARIANT_BOOL bEnable)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    return m_pOSMPEngnWrap->EnableCPUAdaptation(bEnable);
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::setBitrateThreshold(LONG upper, LONG lower)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // TODO: Add your dispatch handler code here
  if (m_pOSMPEngnWrap)
  {
    return m_pOSMPEngnWrap->SetBitrateThreshold(upper, lower);
  }

  return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::updateSourceURL(LPCTSTR url)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (m_pOSMPEngnWrap)
    {
        return m_pOSMPEngnWrap->UpdateSourceURL(url);
    }

    return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::enableDolbyLibrary(VARIANT_BOOL bEnable)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (m_pOSMPEngnWrap)
    {
        return m_pOSMPEngnWrap->EnableDolbyLibrary(bEnable);
    }

    return VO_OSMP_ERR_UNINITIALIZE;
}

BSTR CvoPlugInIECtrl::getVersion(LONG moduleType)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strResult;

    // TODO: Add your dispatch handler code here
    TCHAR szVersion[MAX_PATH] = _T("");
    GetVersion(szVersion,MAX_PATH,moduleType);
    strResult = szVersion;

    return strResult.AllocSysString();
}

ULONG CvoPlugInIECtrl::setHTTPRetryTimeout(LONG iRetryTime)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (m_pOSMPEngnWrap)
    {
        return m_pOSMPEngnWrap->SetHTTPRetryTimeout(iRetryTime);
    }

    return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::setDefaultAudioLanguage(LPCTSTR type)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (m_pOSMPEngnWrap)
    {
        char szTemp[MAX_PATH];
        WideCharToMultiByte(CP_ACP,NULL,type,-1,szTemp,MAX_PATH,NULL,FALSE);
        return m_pOSMPEngnWrap->SetDefaultAudioLanguage(szTemp);
    }

    return VO_OSMP_ERR_UNINITIALIZE;
}

ULONG CvoPlugInIECtrl::setDefaultSubtitleLanguage(LPCTSTR type)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    // TODO: Add your dispatch handler code here
    if (m_pOSMPEngnWrap)
    {
        char szTemp[MAX_PATH];
        WideCharToMultiByte(CP_ACP,NULL,type,-1,szTemp,MAX_PATH,NULL,FALSE);
        return m_pOSMPEngnWrap->SetDefaultSubtitleLanguage(szTemp);
    }

    return VO_OSMP_ERR_UNINITIALIZE;
}