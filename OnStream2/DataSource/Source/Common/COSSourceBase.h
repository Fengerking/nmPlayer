#ifndef __COSSourceBase_H__
#define __COSSourceBase_H__

#ifdef _LINUX
#include <dlfcn.h>
#include <jni.h>
#include "CJniEnvUtil.h"
#include "vodlfcn.h"
#endif

#include "voOnStreamType.h"
#include "voOnStreamSource.h"
#include "COSBaseSource.h"
#include "voSubtitleType.h"
#include "voString.h"

#include "voDRM2.h"


typedef VO_S32 (VO_API * VOGETDRM2API)(VO_DRM2_API * pDRMHandle, VO_U32 uFlag);
typedef VO_VOID (VO_API * VOGETIOAPI)( VO_SOURCE2_IO_API * pIOAPI);

class COSSourceBase
{
public:
	COSSourceBase ();
	virtual ~COSSourceBase (void);

	virtual int				Init(void* pSource, int nSoureFlag, int nSourceType, void* pInitParam, int nInitParamFlag);
	virtual int				Uninit(void);
	virtual int 			Open(void);
	virtual int 			Run (void);
	virtual int 			Pause (void);
	virtual int 			Stop (void);
	virtual int 			Close (void);
	virtual int 			GetDuration (long long * pDuration);
	virtual int 			SetCurPos (long long *pCurPos);
	virtual int				GetSample(int nTrackType, void* pSample);
	virtual int 			GetProgramCount(unsigned int *pProgreamCount);
	virtual int 			GetProgramInfo(unsigned int nProgram, VOOSMP_SRC_PROGRAM_INFO **ppProgramInfo);
	virtual int 			GetCurTrackInfo(int nTrackType , VOOSMP_SRC_TRACK_INFO ** ppTrackInfo );
	virtual int				SelectProgram(unsigned int nProgram);
	virtual int				SelectStream(unsigned int nStream);
	virtual int				SelectTrack(unsigned int nTrack);
	virtual int				SendBuffer(const VOOSMP_BUFFERTYPE& buffer);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);

	virtual int				GetSubLangNum(int *pNum);
	virtual int				GetSubLangItem(int Index, VOOSMP_SUBTITLE_LANGUAGE** pSubLangItem);
	virtual int				GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** pSubLangInfo);
	virtual int				SelectLanguage(int Index);

	virtual int				GetMediaCount(int ssType);
	virtual int				SelectMediaIndex(int ssType, int nIndex);
	virtual int				IsMediaIndexAvailable(int ssType, int nIndex);
	virtual int				GetMediaProperty(int ssType, int nIndex, VOOSMP_SRC_TRACK_PROPERTY **pProperty);
	virtual int				CommetSelection(int bSelect);
	virtual int				GetCurMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);
	virtual int				GetCurPlayingMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);
	virtual int				GetCurSelectedMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);

	virtual int				SetJavaVM(void *pJavaVM, void* obj);
#ifdef _LINUX
	virtual void*			GetJavaObj(){return m_jOSSource;	};
#endif

	virtual int				HandleEvent(int nID, void * pParam1, void * pParam2);
	virtual int				HandleOnQest(int nID, void * pParam1, void * pParam2);

	virtual VO_U32			doVerifyCallBackFunc(VO_U32 nID, VO_PTR pParam1);
	virtual VO_U32			doHttpIOCallBackFunc(VO_U32 nID , VO_PTR pParam1 , VO_PTR pParam2);

	virtual int				InitDRM();
	virtual int				UnintDRM();

	static VO_S32			OSDRMListener(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	static int				OSSourceListener(void * pUserData, int nID, void * pParam1, void * pParam2);
	static int				OSSourceOnQest(void * pUserData, int nID, void * pParam1, void * pParam2);
	static VO_U32			VerifyCallBackFunc( VO_PTR hHandle , VO_U32 uID , VO_PTR pUserData );
	static VO_U32			OSIOCallBackFunc( VO_PTR hHandle , VO_U32 uID , VO_PTR pParam1 , VO_PTR pParam2 );

protected:
	virtual int				ClearPropertyItem(VOOSMP_SRC_TRACK_PROPERTY *pItemProperty);

	virtual int				InitSourceIO();
	virtual int				UnintSourceIO();
	
protected:
	static void *			vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag);
	static void *			vomtLoadLib (void * pUserData, char * pLibName, int nFlag);
	static int				vomtFreeLib (void * pUserData, void * hLib, int nFlag);

	COSBaseSource*			m_pBaseSource;
	VO_SOURCE2_LIB_FUNC*	m_pLibOP;	
	VO_SOURCE2_LIB_FUNC		m_sLibFunc;

	VOOSMP_LISTENERINFO		m_lSourceListener;
	VOOSMP_LISTENERINFO		m_lAPListener;
	VOOSMP_LISTENERINFO		m_lSourceOnReqest;
	VOOSMP_LISTENERINFO		m_lAPOnReqest;

	voCMutex				m_MutexStatus;

	voCMutex				m_MutexEvent;
	voCMutex				m_MutexOnQuest;
	int						m_nSourceType;
	int						m_nFlag;

	VO_TCHAR				m_szPathLib[1024];

	VO_SOURCE2_EVENTCALLBACK m_cDRMEventCallBack;

	char						m_szDRMFile[1024];
	char						m_szDRMAPI[128];

	char						m_szAdapterDRMFile[1024];
	char						m_szAdapterDRMAPI[128];
	void*					m_pAapterDRMObject;

	void*					m_hDrmLib;
	void*					m_hDrmHandle;
	VO_DRM2_API				m_fDRMCallBack;
	VO_DRM_OPENPARAM		m_sDRMOpenParam;
	VO_SOURCEDRM_CALLBACK2*	m_pDRMCallBack;
	VOGETDRM2API			m_pGetDRM2API;
	void*					m_fThirdAPI;
	char*					m_sDRMUniqueIden;

	int						m_nPrefixEnable;
	int						m_nSuffixEnable;
	char					m_cPrefix[32];
	char					m_cSuffix[32];

	char					m_szIOFile[256];
	char					m_szIOAPI[256];
	void*					m_hIOLib;
	void*					m_hIOHandle;
	VO_SOURCE2_IO_API		m_hIOAPI;

	VO_SOURCE2_CPU_INFO		m_sCPUInfo;
	VO_SOURCE2_CAP_DATA		m_sCapData;

	VO_LOG_PRINT_CB			m_cbVOLOG;

	VO_SOURCE2_IO_HTTP_VERIFYCALLBACK	m_sVerifyCallBack;
	VO_SOURCE2_IO_HTTPCALLBACK			m_sHttpIOCallBack;

	VO_CHAR					m_pProxyHost[128];
	VO_CHAR					m_pProxyPort[128];
	VO_SOURCE2_HTTPPROXY	m_sHTTPPoxy;

	VO_BOOL				m_bIsDRMInit;
	VO_BOOL				m_bNeedUpdateDRM;

#ifdef _LINUX
	JavaVM *				m_JavaVM;
	jobject					m_jOSSource;

	jclass					m_clsChunk;
	jmethodID				m_midChunk;

	jclass					m_clsTimedTag;
	jmethodID				m_midTimedTag;
#endif

	/* 
	  * Following new added class members are for 
	  * Up layer move SetParam call ID values saved in low layer 
	  * OSSource module 
	  */
	int										m_nMinBufferTime;
	char*									m_pLicenseText;
	char*									m_pLicenseFilePath;
	char*									m_pLicenseContent;
	char*									m_pSubtitleFileName;
	int										m_nLowLatencyMode;
	int										m_nPDContentRetryTimes;
	VOOSMP_READBUFFER_FUNC				m_sReadBufferFunction;
	int										m_nCCAutoSwitchDuration;
	VOOSMP_SRC_HTTP_VERIFYCALLBACK		m_sHttpVerifyCallback;
	VOOSMP_SRC_VERIFICATIONINFO			m_sVerificationInfo;
	int										m_nSocketConnectionType;
	char*									m_pCommandString;
	char*									m_pCapTablePath;
	int										m_nMaxDRMDoVerificationCount;
	int										m_nMaxDownloadFaildTolerantCount;
	int										m_nCPUAdaptionOnOff;
	int										m_nStartBufferingTime;
	int										m_nBufferingTime;
	VOOSMP_SRC_DVRINFO					m_sDvrInfo;
	int										m_nBAWorkingMode;
	int										m_nRTSPStats;
	int										m_nBufferMaxSize;
	VOOSMP_SRC_HTTP_HEADER				m_sHTTPHeader;
	int										m_nRTSPConnectionPort;
	int										m_nPresentationDelay;
	VOOSMP_SRC_PERIODTIME_INFO			m_nPerIOToTime;
	VO_SOURCE2_BA_THRESHOLD				m_sBAThreshold;
	int										m_nTotalDownLoadErrorTolerate;
	int										m_nEnableRTSPoverHTTPTunnel;
	int										m_nRTSPOverHTTPConnectPort;
	char*									m_pUpdateURL;
	int										m_nHttpRetryTimeOut;
	char*									m_szDefaultAudioLanguage;
	char*									m_szDefaultSubLanguage;
	void*									m_pSourceIOLib;
	VO_SOURCE2_IO_API						m_pSourceIOAPI;

	/* Following is define for setting values before BaseSource Init */
	bool										m_bNeedSetMinBufferTime;
	bool										m_bNeedSetLicenseText;
	bool										m_bNeedSetLicenseFilePath;
	bool										m_bNeedSetLicenseContent;
	bool										m_bNeedSetPerformanceData;
	bool										m_bNeedSetBAStartCap;
	bool										m_bNeedSetDRMCallback;
	bool										m_bNeedSetDRMThirdPartAPI;
	bool										m_bNeedSetSubtitleFileName;
	bool										m_bNeedSetLowLatencyMode;
	bool										m_bNeedSetPDContentRetryTimes;
	bool										m_bNeedSetReadBufferFunction;
	bool										m_bNeedSetCCAutoSwitchDuration;
	bool										m_bNeedSetHttpVerifyCallback;
	bool										m_bNeedSetVerificationInfo;
	bool										m_bNeedSetSocketConnectionType;
	bool										m_bNeedSetCommandString;
	bool										m_bNeedSetCapTablePath;
	bool										m_bNeedSetMaxDRMDoVerificationCount;
	bool										m_bNeedSetDRMUniqueIdentifier;
	bool										m_bNeedSetMaxDownloadFaildTolerantCount;
	bool										m_bNeedSetCPUAdaptionOnOff;
	bool										m_bNeedSetStartBufferingTime;
	bool										m_bNeedSetBufferingTime;
	bool										m_bNeedSetDvrInfo;
	bool										m_bNeedSetBAWorkingMode;
	bool										m_bNeedSetRTSPStats;
	bool										m_bNeedSetBufferMaxSize;
	bool										m_bNeedSetHTTPHeader;
	bool										m_bNeedSetHttpProxy;
	bool										m_bNeedSetRTSPConnectionPort;
	bool										m_bNeedSetPresentationDelay;
	bool										m_bNeedSetPerIOToTime;
	bool										m_bNeedSetBAThreshold;
	bool										m_bNeedSetTotalDownLoadErrorTolerate;
	bool										m_bNeedSetEnableRTSPoverHTTPTunnel;
	bool										m_bNeedSetRTSPOverHTTPConnectPort;
	bool										m_bNeedSetUpdateURL;
	bool										m_bNeedSetHttpRetryTimeOut;
	bool										m_bNeedSetDefaultAudioLanguage;
	bool										m_bNeedSetDefaultSubLanguage;

	/* TODO */
	/*    VOOSMP_SRC_PID_FUNC_IO     */
	/*	VOOSMP_SRC_PID_DRM_INIT_DATA_RESPONSE    */	
};

#endif // __COSBaseSource_H__
