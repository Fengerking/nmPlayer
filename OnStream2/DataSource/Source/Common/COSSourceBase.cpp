#include "voString.h"

#include "cmnFile.h"
#include "cioFile.h"
#include "CFileFormatCheck.h"
#include "cmnMemory.h"
#include "COSSourceBase.h"
#include "COSAdapStrSource.h"
#include "COSHLSSource.h"
#include "COSDASHSource.h"
#include "COSISSSource.h"
#include "COSRTSPSource.h"
#include "COSPDMSSource.h"
#include "COSCMMBSource.h"
#include "COSLocalSource.h"
#include "voOSFunc.h"
#include "voFile.h"
#include "stdlib.h"
#ifdef _LINUX
#include "CJavaParcelWrapOSMP.h"
#endif
#ifdef _IOS
#include "voLoadLibControl.h"
#endif
#ifdef _MAC_OS
#include "dlfcn.h"
#endif

#define  LOG_TAG    "COSSourceManager"
#include "voLog.h"
#include "voStreaming.h"
#include "voDRM2.h"

typedef VO_VOID ( VO_API * pOSSGetSourceIOAPI)( VO_SOURCE2_IO_API * ptr_api );

COSSourceBase::COSSourceBase()
	: m_pBaseSource(NULL)	
	, m_pLibOP (NULL)
	, m_pAapterDRMObject(NULL)
	, m_hDrmLib (NULL)
	, m_hDrmHandle (NULL)
	, m_pDRMCallBack (NULL)
	, m_pGetDRM2API(NULL)
	, m_fThirdAPI(NULL)
	, m_sDRMUniqueIden(NULL)
	, m_nPrefixEnable(0)
	, m_nSuffixEnable(0)
	, m_hIOLib(0)
	, m_hIOHandle(0)
	, m_bIsDRMInit(VO_FALSE)
	, m_bNeedUpdateDRM(VO_TRUE)
#ifdef _LINUX
	, m_JavaVM(NULL)
	, m_jOSSource(0)
	, m_clsChunk(0)
	, m_midChunk(0)
	, m_clsTimedTag(0)
	, m_midTimedTag(0)
#endif
	,m_nMinBufferTime(0)
	,m_pLicenseText(NULL)
	,m_pLicenseFilePath(NULL)
	,m_pLicenseContent(NULL)
	,m_pSubtitleFileName(NULL)
	,m_nLowLatencyMode(0)
	,m_nPDContentRetryTimes(0)
	,m_nCCAutoSwitchDuration(0)
	,m_nSocketConnectionType(0)
	,m_pCommandString(NULL)
	,m_pCapTablePath(NULL)
	,m_nMaxDRMDoVerificationCount(0)
	,m_nMaxDownloadFaildTolerantCount(0)
	,m_nCPUAdaptionOnOff(0)
	,m_nStartBufferingTime(0)
	,m_nBufferingTime(0)
	,m_nBAWorkingMode(0)
	,m_nRTSPStats(0)
	,m_nBufferMaxSize(0)
	,m_nRTSPConnectionPort(0)
	,m_nPresentationDelay(0)
	,m_nTotalDownLoadErrorTolerate(0)
	,m_nEnableRTSPoverHTTPTunnel(0)
	,m_nRTSPOverHTTPConnectPort(0)
	,m_pUpdateURL(NULL)
	,m_nHttpRetryTimeOut(-1)
	,m_szDefaultAudioLanguage(NULL)
	,m_szDefaultSubLanguage(NULL)
	,m_pSourceIOLib(NULL)
	,m_bNeedSetMinBufferTime(false)
	,m_bNeedSetLicenseText(false)
	,m_bNeedSetLicenseFilePath(false)
	,m_bNeedSetLicenseContent(false)
	,m_bNeedSetPerformanceData(false)
	,m_bNeedSetBAStartCap(false)
	,m_bNeedSetDRMCallback(false)
	,m_bNeedSetDRMThirdPartAPI(false)
	,m_bNeedSetSubtitleFileName(false)
	,m_bNeedSetLowLatencyMode(false)
	,m_bNeedSetPDContentRetryTimes(false)
	,m_bNeedSetReadBufferFunction(false)
	,m_bNeedSetCCAutoSwitchDuration(false)
	,m_bNeedSetHttpVerifyCallback(false)
	,m_bNeedSetVerificationInfo(false)
	,m_bNeedSetSocketConnectionType(false)
	,m_bNeedSetCommandString(false)
	,m_bNeedSetCapTablePath(false)
	,m_bNeedSetMaxDRMDoVerificationCount(false)
	,m_bNeedSetDRMUniqueIdentifier(false)
	,m_bNeedSetMaxDownloadFaildTolerantCount(false)
	,m_bNeedSetCPUAdaptionOnOff(false)
	,m_bNeedSetStartBufferingTime(false)
	,m_bNeedSetBufferingTime(false)
	,m_bNeedSetDvrInfo(false)
	,m_bNeedSetBAWorkingMode(false)
	,m_bNeedSetRTSPStats(false)
	,m_bNeedSetBufferMaxSize(false)
	,m_bNeedSetHTTPHeader(false)
	,m_bNeedSetHttpProxy(false)
	,m_bNeedSetRTSPConnectionPort(false)
	,m_bNeedSetPresentationDelay(false)
	,m_bNeedSetPerIOToTime(false)
	,m_bNeedSetBAThreshold(false)
	,m_bNeedSetTotalDownLoadErrorTolerate(false)
	,m_bNeedSetEnableRTSPoverHTTPTunnel(false)
	,m_bNeedSetRTSPOverHTTPConnectPort(false)
	,m_bNeedSetUpdateURL(false)
	,m_bNeedSetHttpRetryTimeOut(false)
	,m_bNeedSetDefaultAudioLanguage(false)
	,m_bNeedSetDefaultSubLanguage(false)
{
	m_sLibFunc.pUserData = this;
	m_sLibFunc.LoadLib = vomtLoadLib;
	m_sLibFunc.GetAddress = vomtGetAddress;
	m_sLibFunc.FreeLib = vomtFreeLib;

	memset(m_szPathLib, 0, sizeof(VO_TCHAR)*1024);
	memset(m_cPrefix, 0, 32);
	memset(m_cSuffix, 0, 32);

	m_nSourceType = -1;

	m_pLibOP = &m_sLibFunc;
	strcpy (m_szDRMFile, "voDRMAdapter");
	strcpy (m_szDRMAPI, "voGetDRMAPI");
	strcpy (m_szAdapterDRMFile, "");
	strcpy (m_szAdapterDRMAPI, "");
	memset (&m_fDRMCallBack, 0, sizeof (m_fDRMCallBack));
	memset(&m_sDRMOpenParam, 0, sizeof(VO_DRM_OPENPARAM));
	memset(&m_sCPUInfo, 0, sizeof(VO_SOURCE2_CPU_INFO));
	memset(&m_sCapData, 0, sizeof(VO_SOURCE2_CAP_DATA));
	memset(&m_cbVOLOG, 0, sizeof(VO_LOG_PRINT_CB));

	memset(m_szIOFile, 0, 256*sizeof(char));
	memset(m_szIOAPI, 0, 256*sizeof(char));
	memset(&m_hIOAPI, 0, sizeof(VO_SOURCE2_IO_API));

	memset(&m_lAPListener, 0, sizeof(VOOSMP_LISTENERINFO));
	memset(&m_lAPOnReqest, 0, sizeof(VOOSMP_LISTENERINFO));

	m_lSourceListener.pUserData = this;
	m_lSourceListener.pListener = OSSourceListener;

	m_lSourceOnReqest.pUserData = this;
	m_lSourceOnReqest.pListener = OSSourceOnQest;

	m_sVerifyCallBack.hHandle = this;
	m_sVerifyCallBack.HTTP_Callback = VerifyCallBackFunc;

	m_sHttpIOCallBack.hHandle =  this;
	m_sHttpIOCallBack.IO_Callback = OSIOCallBackFunc;

	m_cDRMEventCallBack.pUserData =  this;
	m_cDRMEventCallBack.SendEvent = OSDRMListener;
	m_pProxyHost[0] = 0;
	m_pProxyPort[0] = 0;

	memset (&m_sHTTPPoxy, 0, sizeof (VO_SOURCE2_HTTPPROXY));
	memset (&m_sReadBufferFunction, 0, sizeof( VOOSMP_READBUFFER_FUNC) );
	memset (&m_sHttpVerifyCallback, 0, sizeof (VOOSMP_SRC_HTTP_VERIFYCALLBACK));
	memset (&m_sVerificationInfo, 0, sizeof (VOOSMP_SRC_VERIFICATIONINFO));
	memset (&m_sDvrInfo, 0, sizeof (VOOSMP_SRC_DVRINFO));
	memset (&m_sHTTPHeader, 0, sizeof (VOOSMP_SRC_HTTP_HEADER));
	memset (&m_nPerIOToTime, 0, sizeof (VOOSMP_SRC_PERIODTIME_INFO));
	memset (&m_sBAThreshold, 0, sizeof (VO_SOURCE2_BA_THRESHOLD));
	memset (&m_pSourceIOAPI, 0, sizeof (VO_SOURCE2_IO_API));
}

COSSourceBase::~COSSourceBase ()
{
	UnintDRM();
	Uninit();

	VOLOGUNINIT();

	/* 
	 * NOTE: these class member's live duration is the same with COSSourceBase, not
	 * the same with Init and Uninit
	 */

	if ( m_pSourceIOLib && m_pSourceIOAPI.UnInit)
	{
		m_pSourceIOAPI.UnInit(NULL);
		m_pSourceIOAPI.SetParam(NULL, VO_SOURCE2_IO_PARAMID_DESTROY, NULL);
		m_pLibOP->FreeLib (m_pLibOP->pUserData, m_pSourceIOLib, 0);
	}

	if (m_pLicenseText) delete m_pLicenseText;
	if (m_pLicenseFilePath) delete m_pLicenseFilePath;
	if (m_pLicenseContent) delete m_pLicenseContent;
	if (m_pSubtitleFileName) delete m_pSubtitleFileName;
	if (m_pCommandString) delete m_pCommandString;
	if (m_pCapTablePath) delete m_pCapTablePath;
	if (m_sHTTPHeader.pszHeaderName) delete m_sHTTPHeader.pszHeaderName;
	if (m_sHTTPHeader.pszHeaderValue) delete m_sHTTPHeader.pszHeaderValue;
	if (m_pUpdateURL) delete m_pUpdateURL;
	if (m_szDefaultAudioLanguage) delete m_szDefaultAudioLanguage;
	if (m_szDefaultSubLanguage) delete m_szDefaultSubLanguage;
}

int	COSSourceBase::Init(void* pSource, int nSoureFlag, int nSourceType, void* pInitParam, int nInitParamFlag)
{
	int nRC = VOOSMP_ERR_Unknown;

	m_nSourceType = nSourceType;
	m_nFlag = 0;

	if((nInitParamFlag&VOOSMP_FLAG_INIT_LIBOP) && pInitParam)
	{
		VOOSMP_LIB_FUNC*	pLibOP = (VOOSMP_LIB_FUNC*)((VOOSMP_INIT_PARAM*)pInitParam)->pLibOP;
		
		if(pLibOP)
		{
			m_pLibOP->pUserData = pLibOP->pUserData;
			m_pLibOP->LoadLib = pLibOP->LoadLib;
			m_pLibOP->GetAddress = pLibOP->GetAddress;
			m_pLibOP->FreeLib = pLibOP->FreeLib;
		}
	}

	if((nInitParamFlag&VOOSMP_FLAG_INIT_WORKING_PATH) && pInitParam)
	{
		VO_TCHAR* pWorkpath = (VO_TCHAR *)((VOOSMP_INIT_PARAM*)pInitParam)->pWorkingPath;
		if(pWorkpath)
		{
			vostrcpy(m_szPathLib, pWorkpath);
		}
	}

	VOLOGINIT( m_szPathLib );

	VOLOGI("@@@### Init() call into");
#ifndef _IOS
#ifndef _METRO
	if(m_pLibOP == NULL)
	{
		return VOOSMP_ERR_Unknown;
	}
	else
	{
		void* hCheck = m_pLibOP->LoadLib(m_pLibOP->pUserData, "voVidDec", 0);

		if(hCheck == NULL)
		{
			return VOOSMP_ERR_Unknown;
		}
		else
		{
			m_pLibOP->FreeLib(m_pLibOP->pUserData, hCheck, 0);
		}
	}

#endif //_METRO
#endif //_IOS

	int bUsingIO = 0;
	long long nActualSize = 0;
	VO_FILE_OPERATOR* pFileOP = NULL;
	
	strcpy (m_szIOFile, "voSourceIO");
	strcpy (m_szIOAPI, "voGetSourceIOAPI");

	if(((nInitParamFlag&VOOSMP_FLAG_INIT_IO_FILE_NAME) ||  (nInitParamFlag&VOOSMP_FLAG_INIT_IO_API_NAME)) && pInitParam)
	{
		bUsingIO = 1;

		if(nInitParamFlag&VOOSMP_FLAG_INIT_IO_FILE_NAME)
		{
			strcpy (m_szIOFile, ((VOOSMP_INIT_PARAM*)pInitParam)->pszIOFileName);
		}

		if(nInitParamFlag&VOOSMP_FLAG_INIT_IO_API_NAME)
		{
			strcpy (m_szIOAPI, ((VOOSMP_INIT_PARAM*)pInitParam)->pszIOApiName);
		}
	}

	if((nInitParamFlag&VOOSMP_FLAG_INIT_ACTUAL_FILE_SIZE) && pInitParam)
	{
		bUsingIO = 1;
		nActualSize = ((VOOSMP_INIT_PARAM*)pInitParam)->llFileSize;
	}

	if(bUsingIO)
	{
		VOLOGI("@@@### OSSource use SourceIO");
		UnintSourceIO();
		InitSourceIO();

		cioFileFillPointer(&m_hIOAPI);

		pFileOP = &g_fileIOOP;
	}
	
	if(m_pBaseSource)
	{
		delete m_pBaseSource;
		m_pBaseSource = NULL;
	}

	if(nSoureFlag & VOOSMP_FLAG_SOURCE_OPEN_ASYNC)
		m_nFlag |= VO_SOURCE2_FLAG_OPEN_ASYNC;

	if(nSoureFlag & VOOSMP_FLAG_SOURCE_URL)
		m_nFlag |= VO_SOURCE2_FLAG_OPEN_URL;
	
	if(nSourceType == 0 && (nSoureFlag & VOOSMP_FLAG_SOURCE_URL))
	{
		VO_TCHAR szURL[1024*2];
		vostrcpy (szURL, (VO_TCHAR *)pSource);

#ifdef _WIN32
		if (!vostrnicmp (szURL,_T("RTSP://"), 6) || !vostrnicmp (szURL, _T("rtsp://"), 6) || 
			!vostrnicmp (szURL, _T("MMS://"), 5) || !vostrnicmp (szURL, _T("mms://"), 5))
#else
		if (!strncasecmp (szURL, "RTSP://", 6) || !strncasecmp (szURL, "rtsp://", 6) || 
			!strncasecmp (szURL, "MMS://", 5) || !strncasecmp (szURL, "mms://", 5))
#endif
		{
			m_nSourceType = VOOSMP_SRC_FFSTREAMING_RTSP;
		}
		else if( vostrstr(szURL , _T(".m3u")) || vostrstr(szURL , _T(".M3U")) )
		{
			m_nSourceType = VOOSMP_SRC_FFSTREAMING_HLS;
		}				
		else if( (vostrstr(szURL , _T(".mpd")) || vostrstr(szURL , _T(".MPD"))) || (vostrstr(szURL , _T(".xml")) || vostrstr(szURL , _T(".XML")) ) )
		{
			m_nSourceType = VOOSMP_SRC_FFSTREAMING_DASH;
		}
#ifdef _WIN32
		else if (!vostrnicmp (szURL, _T("HTTP://"), 6) || !vostrnicmp (szURL, _T("http://"), 6) 
				|| !vostrnicmp (szURL, _T("HTTPS://"), 7) || !vostrnicmp (szURL, _T("https://"), 7))
#else
		else if (!strncasecmp (szURL, "HTTP://", 6) || !strncasecmp (szURL, "http://", 6) 
				|| !strncasecmp (szURL, "HTTPS://", 7) || !strncasecmp (szURL, "https://", 7))
#endif
		{
			VO_TCHAR* p = vostrstr(szURL, _T(".sdp"));
			if(!p)
				p = vostrstr(szURL, _T(".SDP"));

			if(p && ((p + 4 - szURL) == (int)vostrlen(szURL) || p[4] == _T('?')))
			{
				m_nSourceType = VOOSMP_SRC_FFSTREAMING_RTSP;
			}
			else
			{				
				if( vostrstr(szURL , _T("/manifest")) || vostrstr (szURL , _T("/MANIFEST")) || vostrstr (szURL , _T("/Manifest")))
				{
					m_nSourceType =  VOOSMP_SRC_FFSTREAMING_SSS;
				}
				else
				{
					m_nSourceType =  VOOSMP_SRC_FFSTREAMING_HTTPPD;
				}
			}
		}
#ifdef _WIN32
		else if (!vostrnicmp (szURL, _T("mtv://cmmb?"), 11))
#else
		else if (!strncasecmp (szURL, "mtv://cmmb?", 11))
#endif
		{
			m_nSourceType = VOOSMP_SRC_FFSTREAMING_CMMB;
		}
		else
		{
			// temporarily for h265 demo, need remove in future
			VO_TCHAR szFile[1024 * 2];
			vostrcpy(szFile, szURL);
			VO_U32 nLen = vostrlen(szFile);
			VO_PTCHAR pExt = szFile + nLen - 1;
			while(*pExt != _T ('.'))
			{
				pExt--;
				if(pExt < szFile)	// some URL has no '.', so protect not crash, East 20111020
					return VOOSMP_SRC_ERR_OPEN_SRC_FAIL;
			}
			pExt++;

			VO_PTCHAR pChar = pExt;
			VO_U32 nChars = vostrlen(pExt);
			for(VO_U32 i = 0; i < nChars; i++)
			{
				if((*pChar) <= _T('z') && (*pChar) >= _T('a'))
					*pChar = *pChar - (_T('a') - _T('A'));
				pChar++;
			}

			if(!vostrcmp(pExt, _T("265")) || !vostrcmp(pExt, _T("H265")))
				m_nSourceType = VOOSMP_SRC_FFVIDEO_H265;
			else if(!vostrcmp(pExt, _T("264")) || !vostrcmp(pExt, _T("H264")))
				m_nSourceType = VOOSMP_SRC_FFVIDEO_H264;
			else if(!vostrcmp(pExt, _T("ISMC")))
				m_nSourceType = VOOSMP_SRC_FFSTREAMING_SSS;
			else
			{
				VO_FILE_FORMAT	nFF = VO_FILE_FFUNKNOWN;
				if( !pFileOP )
				{
					cmnFileFillPointer();
					pFileOP = &g_fileOP;
				}

				cmnMemFillPointer(0x53465053);

				VO_FILE_SOURCE filSource;
				memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
				filSource.nMode = VO_FILE_READ_ONLY;
				if(m_nFlag & VO_SOURCE2_FLAG_OPEN_URL)
					filSource.nFlag = VO_FILE_TYPE_NAME;
				filSource.pSource = pSource;

				CFileFormatCheck filCheck (pFileOP , &g_memOP);
				nFF = filCheck.GetFileFormat(&filSource , ~FLAG_CHECK_AUDIOREADER);

				if (nFF == VO_FILE_FFUNKNOWN)
				{
					/* try again about audio file format */
					nFF = filCheck.GetFileFormat(&filSource , FLAG_CHECK_AUDIOREADER);
				}

				if (nFF == VO_FILE_FFMOVIE_AVI)
				{
					m_nSourceType = VO_FILE_FFMOVIE_AVI;
				}
				else if (nFF == VO_FILE_FFMOVIE_MP4 || nFF == VO_FILE_FFMOVIE_MOV)
				{
					m_nSourceType = VOOSMP_SRC_FFMOVIE_MP4;
				}
				else if (nFF == VO_FILE_FFMOVIE_ASF)
				{
					m_nSourceType = VO_FILE_FFMOVIE_ASF;
				}
				else if (nFF == VO_FILE_FFMOVIE_MPG)
				{
					m_nSourceType = VOOSMP_SRC_FFMOVIE_MPG;
				}
				else if (nFF == VO_FILE_FFMOVIE_TS)
				{
					m_nSourceType = VOOSMP_SRC_FFMOVIE_TS;
				}
				else if (nFF == VO_FILE_FFMOVIE_FLV)
				{
					m_nSourceType = VOOSMP_SRC_FFMOVIE_FLV;
				}
				else if (nFF == VO_FILE_FFMOVIE_REAL)
				{
					m_nSourceType = VOOSMP_SRC_FFMOVIE_REAL;
				}
				else if( nFF == VO_FILE_FFMOVIE_MKV )
				{
					m_nSourceType = VOOSMP_SRC_FFMOVIE_MKV;
				}
				else if( nFF == VO_FILE_FFVIDEO_H264)
				{
					m_nSourceType = VOOSMP_SRC_FFVIDEO_H264;
				}
				else if( nFF == VO_FILE_FFVIDEO_H265)
				{
					m_nSourceType = VOOSMP_SRC_FFVIDEO_H265;
				}
				else if ( nFF == VO_FILE_FFAUDIO_AAC)
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_AAC;
				}
				else if ( nFF == VO_FILE_FFAUDIO_AMR )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_AMR;
				}
				else if ( nFF == VO_FILE_FFAUDIO_AWB )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_AWB;
				}
				else if ( nFF == VO_FILE_FFAUDIO_MP3 )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_MP3;
				}
				else if ( nFF == VO_FILE_FFAUDIO_QCP )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_QCP;
				}
				else if ( nFF == VO_FILE_FFAUDIO_WAV )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_WAV;
				}
				else if ( nFF == VO_FILE_FFAUDIO_WMA )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_WMA;
				}
				else if (nFF == VO_FILE_FFAUDIO_MIDI)
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_MIDI;
				}
				else if (nFF == VO_FILE_FFAUDIO_OGG)
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_OGG;
				}
				else if ( nFF == VO_FILE_FFAUDIO_FLAC )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_FLAC;
				}
				else if ( nFF == VO_FILE_FFAUDIO_AU )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_AU;
				}
				else if ( nFF == VO_FILE_FFAUDIO_APE )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_APE;
				}
				else if ( nFF == VO_FILE_FFAUDIO_ALAC )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_ALAC;
				}
				else if ( nFF == VO_FILE_FFAUDIO_AC3 )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_AC3;
				}
				else if( nFF == VO_FILE_FFAUDIO_PCM)
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_PCM;
				}
				else if ( nFF == VO_FILE_FFAUDIO_DTS )
				{
					m_nSourceType = VOOSMP_SRC_FFAUDIO_DTS;
				}
				else
				{
					m_nSourceType = VOOSMP_SRC_FFMOVIE_MP4;
				}
			}
		}
	}
	VOLOGI("@@@### File format check result m_nSourceType = 0x%08X", m_nSourceType);

	if(m_nSourceType == VOOSMP_SRC_FFSTREAMING_RTSP)
		m_pBaseSource = new COSRTSPSource(m_pLibOP);
	else if(m_nSourceType == VOOSMP_SRC_FFSTREAMING_HLS)
		m_pBaseSource = new COSAdapStrSource(m_pLibOP);
	else if(m_nSourceType == VOOSMP_SRC_FFSTREAMING_DASH)
		m_pBaseSource = new COSAdapStrSource(m_pLibOP);
	else if(m_nSourceType ==  VOOSMP_SRC_FFSTREAMING_SSS)
		m_pBaseSource = new COSAdapStrSource(m_pLibOP);
	else if(m_nSourceType == VOOSMP_SRC_FFSTREAMING_HTTPPD)
		m_pBaseSource = new COSPDMSSource(m_pLibOP);
  else if(m_nSourceType == VOOSMP_SRC_FFSTREAMING_CMMB)
    m_pBaseSource = new COSCMMBSource(m_pLibOP);
	else 
		m_pBaseSource = new COSLocalSource(m_pLibOP, m_nSourceType);

	if(nInitParamFlag & VOOSMP_FLAG_SOURCE_READBUFFER)
		m_nFlag |= VO_SOURCE2_FLAG_OPEN_SENDBUFFER;
	else if(nInitParamFlag & VOOSMP_FLAG_SOURCE_HANDLE)
		m_nFlag |= VO_SOURCE2_FLAG_OPEN_HANDLE;
	
	if(m_pBaseSource)
	{
		nRC = m_pBaseSource->SetParam(VOOSMP_PID_PLAYER_PATH, m_szPathLib);
		nRC = m_pBaseSource->SetParam(VOOSMP_PID_COMMON_LOGFUNC, &m_cbVOLOG);
		nRC = m_pBaseSource->SetParam(VOOSMP_PID_LISTENER, &m_lSourceListener);
		nRC = m_pBaseSource->SetParam(VOOSMP_PID_ONREQUEST_LISTENER, &m_lSourceOnReqest);		
#ifdef _LINUX	
		CJniEnvUtil	env(m_JavaVM);

		m_pBaseSource->SetParam(ID_SET_JAVA_ENV, env.getEnv());		
#endif
		
		VO_SOURCE2_COMMONINITPARAM *pSourceInitParam = NULL;
		nInitParamFlag = 0;

		VO_SOURCE2_COMMONINITPARAM sInitParam;
		memset(&sInitParam, 0, sizeof(VO_SOURCE2_COMMONINITPARAM));

		if(m_hIOLib)
		{
			sInitParam.pIO = &m_hIOAPI;
			pSourceInitParam = &sInitParam;
			nInitParamFlag |= VO_SOURCE2_FLAG_INIT_IO;
		}

		if(nActualSize)
		{
			nInitParamFlag |= VO_SOURCE2_FLAG_INIT_ACTUALFILESIZE;
			sInitParam.ullActualFileSize = nActualSize;
			pSourceInitParam = &sInitParam;
		}

		nRC = m_pBaseSource->Init(pSource, m_nFlag, pSourceInitParam, nInitParamFlag);

		if ((m_pSourceIOLib == NULL) && (VOOSMP_SRC_FFSTREAMING_HLS <= m_nSourceType && m_nSourceType <= VOOSMP_SRC_FFSTREAMING_DASH))
		{
			m_pSourceIOLib = m_pLibOP->LoadLib (m_pLibOP->pUserData, "voSourceIO", 0);
			if (m_pSourceIOLib != NULL)
			{
				pOSSGetSourceIOAPI api = (pOSSGetSourceIOAPI) m_pLibOP->GetAddress (m_pLibOP->pUserData, m_pSourceIOLib, "voGetSourceIOAPI", 0);
				if (api)  api (&m_pSourceIOAPI);
			}
			else
			{
				VOLOGW ("OSSource load sourceIO failed");
			}
		}
		else
		{
			VOLOGI ("OSSource already loaded sourceIO or no need use sourceIO");
		}
		if (m_pSourceIOAPI.Init)
			m_pBaseSource->SetParam(VO_PID_SOURCE2_SOURCE_IO_API, &m_pSourceIOAPI);
		m_pBaseSource->SetParam(VOOSMP_SRC_PID_HTTPVERIFICATIONCALLBACK, &m_sVerifyCallBack);
		m_pBaseSource->SetParam(VO_PID_SOURCE2_HTTPVERIFICATIONCALLBACK, &m_sVerifyCallBack);
		m_pBaseSource->SetParam(VO_PID_SOURCE2_IO_CBFUNC, &m_sHttpIOCallBack);

		if (m_bNeedSetHTTPHeader)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_HTTPHEADER, &m_sHTTPHeader);	
			if(m_hIOHandle && m_hIOAPI.SetParam)
			{
				m_hIOAPI.SetParam(m_hIOHandle, VO_SOURCE2_IO_PARAMID_HTTPHEADINFO, &m_sHTTPHeader);
			}
		}
		if (m_bNeedSetHttpProxy)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_HTTPPROXYINFO, &m_sHTTPPoxy);			
			if(m_hIOHandle && m_hIOAPI.SetParam)
			{
				m_hIOAPI.SetParam(m_hIOHandle, VO_SOURCE2_IO_PARAMID_HTTPPROXYINFO, &m_sHTTPPoxy);
			}
		}
		if(m_bNeedSetDRMCallback)
		{
			m_pBaseSource->SetParam (VO_PID_SOURCE2_DRMCALLBACK, m_pDRMCallBack);
		}
		if (m_bNeedSetDRMThirdPartAPI && m_fDRMCallBack.SetThirdpartyAPI)
		{
			m_fDRMCallBack.SetThirdpartyAPI(m_hDrmHandle, m_fThirdAPI);
		}
		if(m_bNeedSetMaxDRMDoVerificationCount && m_hDrmHandle && m_fDRMCallBack.SetParameter)
		{
			m_fDRMCallBack.SetParameter(m_hDrmHandle, VO_PID_SOURCE2_DOHTTPVERIFICATION, &m_nMaxDRMDoVerificationCount);
		}
		if(m_bNeedSetDRMUniqueIdentifier && m_hDrmHandle && m_fDRMCallBack.SetParameter)
		{
			m_fDRMCallBack.SetParameter(m_hDrmHandle, VO_PID_DRM2_UNIQUE_IDENTIFIER, m_sDRMUniqueIden);
		}
		if (m_bNeedSetMinBufferTime)
		{
			m_pBaseSource->SetParam(VOOSMP_PID_MIN_BUFFTIME, &m_nMinBufferTime);
		}
		if (m_bNeedSetLicenseText)
		{
			m_pBaseSource->SetParam(VOOSMP_PID_LICENSE_TEXT, m_pLicenseText);
		}
		if (m_bNeedSetLicenseFilePath)
		{
			m_pBaseSource->SetParam(VOOSMP_PID_LICENSE_FILE_PATH, m_pLicenseFilePath);
		}
		if (m_bNeedSetLicenseContent)
		{
			m_pBaseSource->SetParam(VOOSMP_PID_LICENSE_CONTENT, m_pLicenseContent);
		}
		if (m_bNeedSetSubtitleFileName)
		{
			m_pBaseSource->SetParam(VOOSMP_PID_SUBTITLE_FILE_NAME, m_pSubtitleFileName);
		}
		if (m_bNeedSetReadBufferFunction)
		{
			m_pBaseSource->SetParam(VOOSMP_SRC_PID_FUNC_READ_BUF, &m_sReadBufferFunction);
		}
		if (m_bNeedSetCCAutoSwitchDuration)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_CC_AUTO_SWITCH_DURATION, &m_nCCAutoSwitchDuration);
		}
		if (m_bNeedSetPerformanceData)
		{
			m_pBaseSource->SetParam(VOOSMP_PID_PERFORMANCE_DATA, &m_sCapData);
		}
		if (m_bNeedSetBAStartCap)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_BA_STARTCAP, &m_sCapData);
		}
		if (m_bNeedSetLowLatencyMode)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_LOWLATENCYMODE, &m_nLowLatencyMode);
		}
		if (m_bNeedSetPDContentRetryTimes)
		{
			m_pBaseSource->SetParam(VOID_STREAMING_PDCONNECTRETRY_TIMES, &m_nPDContentRetryTimes);
		}
		if (m_bNeedSetVerificationInfo)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_DOHTTPVERIFICATION, &m_sVerificationInfo);
		}
		if (m_bNeedSetSocketConnectionType)
		{
			m_pBaseSource->SetParam(VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE, &m_nSocketConnectionType);
		}
		if (m_bNeedSetCommandString)
		{
			m_pBaseSource->SetParam(VOOSMP_SRC_PID_COMMAND_STRING, m_pCommandString);
		}
		if (m_bNeedSetCapTablePath)
		{
			m_pBaseSource->SetParam(VOOSMP_SRC_PID_CAP_TABLE_PATH, m_pCapTablePath);
		}
		if (m_bNeedSetMaxDownloadFaildTolerantCount)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_DOWNLOAD_FAIL_MAX_TOLERANT_COUNT, &m_nMaxDownloadFaildTolerantCount);
		}
		if (m_bNeedSetCPUAdaptionOnOff)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_BA_CPUBADISABLE, &m_nCPUAdaptionOnOff);
		}
		if (m_bNeedSetStartBufferingTime)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_BUFFER_STARTBUFFERINGTIME, &m_nStartBufferingTime);
		}
		if (m_bNeedSetBufferingTime)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_BUFFER_BUFFERINGTIME, &m_nBufferingTime);
		}
		if (m_bNeedSetDvrInfo)
		{
			m_pBaseSource->SetParam(VOOSMP_SRC_PID_DVRINFO, &m_sDvrInfo);
		}
		if (m_bNeedSetBAWorkingMode)
		{
			m_pBaseSource->SetParam(VOOSMP_SRC_PID_BA_WORKMODE, &m_nBAWorkingMode);
		}
		if (m_bNeedSetRTSPStats)
		{
			m_pBaseSource->SetParam(VOOSMP_SRC_PID_RTSP_STATS, &m_nRTSPStats);
		}
		if (m_bNeedSetBufferMaxSize)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_BUFFER_MAXBUFFERSIZE, &m_nBufferMaxSize);
		}
		if (m_bNeedSetRTSPConnectionPort)
		{
			m_pBaseSource->SetParam(VOOSMP_SRC_PID_RTSP_CONNECTION_PORT, &m_nRTSPConnectionPort);
		}
		if (m_bNeedSetPresentationDelay)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_LIVELATENCY, &m_nPresentationDelay);
		}
		if (m_bNeedSetPerIOToTime)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_PERIOD2TIME, &m_nPerIOToTime);
		}
		if (m_bNeedSetBAThreshold)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_BITRATE_THRESHOLD, &m_sBAThreshold);
		}
		if (m_bNeedSetTotalDownLoadErrorTolerate)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_TOLERANT_ERROR_NUMBER, &m_nTotalDownLoadErrorTolerate);
		}
		if (m_bNeedSetEnableRTSPoverHTTPTunnel)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_ENABLE_RTSP_HTTP_TUNNELING, &m_nEnableRTSPoverHTTPTunnel);
		}
		if (m_bNeedSetRTSPOverHTTPConnectPort)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_RTSP_OVER_HTTP_CONNECTION_PORT, &m_nRTSPOverHTTPConnectPort);
		}
		if (m_bNeedSetUpdateURL)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_UPDATE_SOURCE_URL, m_pUpdateURL);
		}
		if (m_bNeedSetHttpRetryTimeOut)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_HTTP_RETRY_TIMEOUT, &m_nHttpRetryTimeOut);
		}
		if (m_bNeedSetDefaultAudioLanguage)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_DEFAULT_AUDIO_LANGUAGE, m_szDefaultAudioLanguage);
		}
		if (m_bNeedSetDefaultSubLanguage)
		{
			m_pBaseSource->SetParam(VO_PID_SOURCE2_DEFAULT_SUBTITLE_LANGUAGE, m_szDefaultSubLanguage);
		}
	}

	return nRC;
}

int	COSSourceBase::Uninit()
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_pBaseSource)
		nRC = m_pBaseSource->Uninit();

	if(m_pBaseSource)
	{
		delete m_pBaseSource;
		m_pBaseSource = NULL;
	}

	if(m_sDRMUniqueIden)
	{
		delete []m_sDRMUniqueIden;
		m_sDRMUniqueIden = NULL;
	}

#ifdef _LINUX
	//CJniEnvUtil	env(m_JavaVM);
	//if(m_clsChunk)
	//{
	//	env.getEnv()->DeleteGlobalRef(m_clsChunk);
	//	m_clsChunk = 0;
	//}

	//if(m_clsTimedTag)
	//{
	//	env.getEnv()->DeleteGlobalRef(m_clsTimedTag);
	//	m_clsTimedTag = 0;
	//}
#endif

	UnintSourceIO();

	return nRC;
}

int COSSourceBase::Open (void)
{
	int nRC = VOOSMP_ERR_Implement;

	if (!m_bIsDRMInit && strlen (m_szDRMFile) > 0)
	{
		UnintDRM();
		InitDRM();
	}

	VO_BOOL drm_running = VO_TRUE;
	if(m_hDrmHandle && m_fDRMCallBack.SetParameter)
		m_fDRMCallBack.SetParameter(m_hDrmHandle, VO_PID_DRM2_RUNNING, &drm_running);

	if(m_hDrmHandle && m_fDRMCallBack.SetParameter)
	{
		VO_BOOL drm_stop = VO_FALSE;
		nRC = m_fDRMCallBack.SetParameter(m_hDrmHandle,VO_PID_DRM2_EXIT,&drm_stop);
		VOLOGI("Drm Set VO_PID_DRM2_EXIT(FALSE) return 0x%x",nRC);
	}
	if(m_pBaseSource)
	{
		if (m_pDRMCallBack != NULL)
		{
			VOLOGI ("Set DRM Call Back Function %p.", m_pDRMCallBack);
			m_pBaseSource->SetParam (VO_PID_SOURCE2_DRMCALLBACK, m_pDRMCallBack);

			if(m_hIOHandle && m_hIOAPI.SetParam)
			{
				m_hIOAPI.SetParam(m_hIOHandle, VO_SOURCE2_IO_PARAMID_DRMPOINTOR, m_pDRMCallBack);
			}
		}

		nRC = m_pBaseSource->Open();
	}

	return nRC;
}

int COSSourceBase::Run (void)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_pBaseSource)
		nRC = m_pBaseSource->Run();

	return nRC;
}

int COSSourceBase::Pause (void)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_pBaseSource)
		nRC = m_pBaseSource->Pause();

	return nRC;
}

int COSSourceBase::Stop (void)
{
	int nRC = VOOSMP_ERR_Implement;

	//as Jacky said,drm exit should be called before call source stop
	if(m_hDrmHandle && m_fDRMCallBack.SetParameter)
	{
		VO_BOOL drm_stop = VO_TRUE;
		nRC = m_fDRMCallBack.SetParameter(m_hDrmHandle,VO_PID_DRM2_EXIT,&drm_stop);
		VOLOGI("Drm Set VO_PID_DRM2_EXIT return 0x%x",nRC);
	}
	
	if(m_pBaseSource)
		nRC = m_pBaseSource->Stop();

	return nRC;
}

int COSSourceBase::Close (void)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_pBaseSource)
		nRC = m_pBaseSource->Close();
	VO_BOOL drm_running = VO_FALSE;
	if(m_hDrmHandle && m_fDRMCallBack.SetParameter)
		m_fDRMCallBack.SetParameter(m_hDrmHandle, VO_PID_DRM2_RUNNING, &drm_running);

	return nRC;
}

int COSSourceBase::GetDuration (long long * pDuration)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_pBaseSource)
		nRC = m_pBaseSource->GetDuration(pDuration);

	VOLOGI("GetDuration, Duration = %d", (int)(*pDuration));

	return nRC;
}

int COSSourceBase::GetSample(int nTrackType, void* pSample)
{
	int nRC = VOOSMP_ERR_Implement;
	if(m_pBaseSource)
	{
		nRC = m_pBaseSource->GetSample((VO_SOURCE2_TRACK_TYPE)nTrackType, pSample);
	}
	return nRC;
}

int COSSourceBase::GetProgramCount(unsigned int *pProgreamCount)
{
	int nRC = VOOSMP_ERR_Implement;
	//remove this lock as GetProgramCount cause it is always be 1
	//voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(m_pBaseSource)
	{
		nRC = m_pBaseSource->GetProgramCount(pProgreamCount);
	}
	return nRC;
}

int COSSourceBase::GetProgramInfo(unsigned int nProgram, VOOSMP_SRC_PROGRAM_INFO **ppProgramInfo)
{
	int nRC = VOOSMP_ERR_Implement;
	//remove this lock casue when async IO send playlist download error event in another
	//thread will cause it dead lock,now our GetprogramInfo do memcpy in OSSource
	//voCAutoLock lockVideoSrc (&m_MutexStatus);
	if(m_pBaseSource)
	{
		nRC = m_pBaseSource->GetProgramInfo(nProgram, (VO_SOURCE2_PROGRAM_INFO **)ppProgramInfo);
	}
	return nRC;
}

int COSSourceBase::GetCurTrackInfo(int nTrackType , VOOSMP_SRC_TRACK_INFO ** ppTrackInfo )
{
	int nRC = VOOSMP_ERR_Implement;
	VOLOGI("GetCurTrackInfo+++,time %d",voOS_GetSysTime());
	voCAutoLock lockVideoSrc (&m_MutexStatus);
	VOLOGI("enter GetCurTrackInfo lockVideoSrc,time %d",voOS_GetSysTime());
	if(m_pBaseSource)
	{
		nRC = m_pBaseSource->GetCurTrackInfo((VO_SOURCE2_TRACK_TYPE)nTrackType, (VO_SOURCE2_TRACK_INFO **)ppTrackInfo);
	}
	VOLOGI("GetCurTrackInfo---,time %d",voOS_GetSysTime());
	return nRC;
}

int COSSourceBase::SelectProgram(unsigned int nProgram)
{
	int nRC = VOOSMP_ERR_Implement;
	VOLOGI("SelectProgram+++,time %d",voOS_GetSysTime());
	voCAutoLock lockVideoSrc (&m_MutexStatus);
	VOLOGI("enter SelectProgram lockVideoSrc,time %d",voOS_GetSysTime());

	if(m_pBaseSource)
	{
		nRC = m_pBaseSource->SelectProgram(nProgram);
	}
	VOLOGI("SelectProgram---,time %d",voOS_GetSysTime());
	return nRC;
}

int COSSourceBase::SelectStream(unsigned int nStream)
{
	int nRC = VOOSMP_ERR_Implement;
	VOLOGI("SelectStream+++,time %d",voOS_GetSysTime());
	voCAutoLock lockVideoSrc (&m_MutexStatus);
	VOLOGI("enter SelectStream lockVideoSrc,time %d",voOS_GetSysTime());
	if(m_pBaseSource)
	{
		nRC = m_pBaseSource->SelectStream(nStream);
	}
	VOLOGI("SelectStream---,time %d",voOS_GetSysTime());
	return nRC;
}

int COSSourceBase::SelectTrack(unsigned int nTrack)
{
	int nRC = VOOSMP_ERR_Implement;
	VOLOGI("SelectTrack+++,time %d",voOS_GetSysTime());
	voCAutoLock lockVideoSrc (&m_MutexStatus);
	VOLOGI("enter SelectTrack lockVideoSrc,time %d",voOS_GetSysTime());
	if(m_pBaseSource)
	{
		nRC = m_pBaseSource->SelectTrack(nTrack);
	}
	VOLOGI("SelectTrack---,time %d",voOS_GetSysTime());
	return nRC;
}

int COSSourceBase::SendBuffer(const VOOSMP_BUFFERTYPE& buffer)
{
	int nRC = VOOSMP_ERR_Implement;

	return nRC;
}

int COSSourceBase::SetCurPos (long long* pCurPos)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_pBaseSource)
		nRC = m_pBaseSource->SetCurPos(pCurPos);

	return nRC;
}

int COSSourceBase::GetParam (int nID, void * pValue)
{
	int nRC = VOOSMP_ERR_Implement;
	int mID = nID;

	voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(nID == VOOSMP_SRC_PID_FUNC_READ_BUF)
	{
		if(m_pBaseSource)
			*((void **)pValue) = m_pBaseSource->GetReadBufPtr();
		else 
			*((void **)pValue) = 0;

		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER)
	{
		if(m_hDrmHandle && m_fDRMCallBack.GetParameter)
		{
			return   m_fDRMCallBack.GetParameter(m_hDrmHandle, VO_PID_DRM2_UNIQUE_IDENTIFIER, pValue);
		}
		else
		{
			return VOOSMP_ERR_Implement;
		}
	}
	else if(nID == VOOSMP_SRC_PID_BA_WORKMODE)
	{
		mID = VO_PID_SOURCE2_BA_WORKMODE;
	}


	if(m_pBaseSource)
	{
		nRC = m_pBaseSource->GetParam(mID, pValue);
	}
	VOLOGI("GetParam---,time %d",voOS_GetSysTime());
	return nRC;
}

int COSSourceBase::SetParam (int nID, void * pValue)
{
	VOLOGI("@@@### SetParm to OSSource:  ID = 0x%08X", nID);
	int nRC = VOOSMP_ERR_None;

	if(nID == VOOSMP_SRC_PID_HTTP_HEADER)
	{
		VOOSMP_SRC_HTTP_HEADER* pHTTPHeader = (VOOSMP_SRC_HTTP_HEADER*)pValue;
		unsigned char* pHeaderName = pHTTPHeader->pszHeaderName;
		unsigned char* pHeaderValue = pHTTPHeader->pszHeaderValue;

		if (m_sHTTPHeader.pszHeaderName) delete m_sHTTPHeader.pszHeaderName;
		if (m_sHTTPHeader.pszHeaderValue) delete m_sHTTPHeader.pszHeaderValue;
		m_sHTTPHeader.pszHeaderName = new unsigned char[ strlen((char*)pHeaderName) + 1];
		m_sHTTPHeader.pszHeaderValue = new unsigned char[ strlen((char*)pHeaderValue) + 1];
		strcpy ((char*)m_sHTTPHeader.pszHeaderName, (char*)pHeaderName);
		strcpy ((char*)m_sHTTPHeader.pszHeaderValue, (char*)pHeaderValue);
		m_sHTTPHeader.uFlag = pHTTPHeader->uFlag;
		m_sHTTPHeader.pFlagData = pHTTPHeader->pFlagData;

		if(m_pBaseSource)
		{
			nRC = m_pBaseSource->SetParam(VO_PID_SOURCE2_HTTPHEADER, &m_sHTTPHeader);			
		}
		else
		{
			m_bNeedSetHTTPHeader = true;
		}

		if(m_hIOHandle && m_hIOAPI.SetParam)
		{
			nRC = m_hIOAPI.SetParam(m_hIOHandle, VO_SOURCE2_IO_PARAMID_HTTPHEADINFO, &m_sHTTPHeader);
		}
		else
		{
			m_bNeedSetHTTPHeader = true;
		}

		if (m_bNeedSetHTTPHeader) return VOOSMP_ERR_None;

		return nRC;
	}
	else if (nID == VOOSMP_SRC_PID_HTTP_PROXY_INFO)
	{
		VOOSMP_SRC_HTTP_PROXY* pHTTPProxy = (VOOSMP_SRC_HTTP_PROXY*)pValue;

		if (strlen( (char*)pHTTPProxy->pszProxyHost ) > 0)
			strcpy (m_pProxyHost,  (char*)pHTTPProxy->pszProxyHost);
		else
			m_pProxyHost[0] = 0;

		sprintf(m_pProxyPort, "%d", pHTTPProxy->nProxyPort);
		m_sHTTPPoxy.szProxyHost = m_pProxyHost;
		m_sHTTPPoxy.szProxyPort = m_pProxyPort;
		
		if(m_pBaseSource)
		{
			nRC = m_pBaseSource->SetParam(VO_PID_SOURCE2_HTTPPROXYINFO, &m_sHTTPPoxy);			
		}
		else
		{
			m_bNeedSetHttpProxy = true;
		}

		if(m_hIOHandle && m_hIOAPI.SetParam)
		{
			nRC = m_hIOAPI.SetParam(m_hIOHandle, VO_SOURCE2_IO_PARAMID_HTTPPROXYINFO, &m_sHTTPPoxy);
		}
		else
		{
			m_bNeedSetHttpProxy = true;
		}

		if (m_bNeedSetHttpProxy) return VOOSMP_ERR_None;
		return nRC;
	}

	voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(pValue == NULL)
		return VOOSMP_ERR_Pointer;
	VOLOGI("SetParam: %d",nID);

	if(nID == VOOSMP_PID_COMMON_LOGFUNC)
	{
		memcpy(&m_cbVOLOG, pValue, sizeof(VO_LOG_PRINT_CB));
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_PLAYER_PATH)
	{
		vostrcpy(m_szPathLib, (VO_TCHAR *)pValue);
		VOLOGI("m_szPathLib: %s",m_szPathLib);

		nRC = VOOSMP_ERR_None;

		if(m_pBaseSource)
			nRC = m_pBaseSource->SetParam(nID, pValue);

		return nRC;
	}
	else if(nID == VOOSMP_PID_FUNC_LIB)
	{
		m_pLibOP = (VO_SOURCE2_LIB_FUNC*)pValue;

		nRC = VOOSMP_ERR_None;

		if(m_pBaseSource)
			nRC = m_pBaseSource->SetParam(nID, pValue);

		return nRC;
	}
	else if(nID == VOOSMP_SRC_PID_DRM_FILE_NAME)
	{
		if ( strcmp(m_szDRMFile, (char *)pValue) != 0)
			m_bNeedUpdateDRM = VO_TRUE;

		strcpy(m_szDRMFile, (char *)pValue);
		VOLOGI("m_szDRMFile: %s",m_szDRMFile);

		if ( strlen (m_szDRMAPI) > 0 && m_bNeedUpdateDRM)
		{
			UnintDRM();
			InitDRM();
			m_bNeedUpdateDRM = VO_FALSE;
		}
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_SRC_PID_DRM_API_NAME)
	{
		if ( strcmp(m_szDRMAPI, (char *)pValue) != 0)
			m_bNeedUpdateDRM = VO_TRUE;

		strcpy(m_szDRMAPI, (char *)pValue);
		VOLOGI("m_szDRMAPI: %s",m_szDRMAPI);

		if ( strlen (m_szDRMFile) > 0 && m_bNeedUpdateDRM)
		{
			UnintDRM();
			InitDRM();
			m_bNeedUpdateDRM = VO_FALSE;
		}
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_SRC_PID_DRM_ADAPTER_FILE_NAME)
	{
		strcpy(m_szAdapterDRMFile, (char *)pValue);
		VOLOGI("m_szAdapterDRMFile: %s",m_szAdapterDRMFile);

		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_SRC_PID_DRM_ADAPTER_API_NAME)
	{
		strcpy(m_szAdapterDRMAPI, (char *)pValue);
		VOLOGI("m_szAdapterDRMAPI: %s",m_szAdapterDRMAPI);

		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT)
	{
		#ifdef _WIN32	
		m_pAapterDRMObject = (void *)(pValue);
		#else
		m_pAapterDRMObject = (void *)(*((int *)pValue));
		#endif
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_LISTENER)
	{
		if(pValue)
		{
			m_lAPListener.pUserData = ((VOOSMP_LISTENERINFO *)pValue)->pUserData;
			m_lAPListener.pListener = ((VOOSMP_LISTENERINFO *)pValue)->pListener;
			m_lAPOnReqest.pUserData = ((VOOSMP_LISTENERINFO *)pValue)->pUserData;
			m_lAPOnReqest.pListener = ((VOOSMP_LISTENERINFO *)pValue)->pListener;

		}

		return VOOSMP_ERR_None; 
	}
	else if(nID == VOOSMP_SRC_PID_DRM_CALLBACK_FUNC)
	{
		m_pDRMCallBack = (VO_SOURCEDRM_CALLBACK2*)pValue;

		if(m_pBaseSource)
			m_pBaseSource->SetParam (VO_PID_SOURCE2_DRMCALLBACK, m_pDRMCallBack);
		else
			m_bNeedSetDRMCallback = true;

		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_SRC_PID_DRM_THIRDPARTY_FUNC_SET)
	{
		m_fThirdAPI = (void *)(*((int *)pValue));
		VOLOGI("@@@### m_fThirdAPI = %d", m_fThirdAPI);
		if (m_fDRMCallBack.SetThirdpartyAPI)
			m_fDRMCallBack.SetThirdpartyAPI(m_hDrmHandle, m_fThirdAPI);
		else
			m_bNeedSetDRMThirdPartAPI = true;
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_ONREQUEST_LISTENER)
	{
		if(pValue)
		{
			m_lAPOnReqest.pUserData = ((VOOSMP_LISTENERINFO *)pValue)->pUserData;
			m_lAPOnReqest.pListener = ((VOOSMP_LISTENERINFO *)pValue)->pListener;
		}

		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_LIB_NAME_PREFIX)
	{
		m_nPrefixEnable = 1;
		strcpy(m_cPrefix, (char *)pValue);
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_LIB_NAME_SUFFIX)
	{
		m_nSuffixEnable = 1;
		strcpy(m_cSuffix, (char *)pValue);
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_SRC_PID_DODRMVERIFICATION)
	{
		m_nMaxDRMDoVerificationCount = (int)( *( (int*)pValue ) );
		if(m_hDrmHandle && m_fDRMCallBack.SetParameter)
		{
			int nRet =  m_fDRMCallBack.SetParameter(m_hDrmHandle, VO_PID_SOURCE2_DOHTTPVERIFICATION, pValue);
			if (nRet != VO_ERR_DRM2_OK) return VOOSMP_SRC_ERR_CONTENT_ENCRYPT;
			return nRet;
		}
		else
		{
			m_bNeedSetMaxDRMDoVerificationCount = true;
			return VOOSMP_ERR_Status;
		}
	}
	else if(nID == VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER)
	{
		if(m_hDrmHandle && m_fDRMCallBack.SetParameter)
		{
			int nRet =  m_fDRMCallBack.SetParameter(m_hDrmHandle, VO_PID_DRM2_UNIQUE_IDENTIFIER, pValue);
			if (nRet != VO_ERR_DRM2_OK) return VOOSMP_SRC_ERR_CONTENT_ENCRYPT;
			return nRet;
		}
		else
		{
			if(m_sDRMUniqueIden)
			{
				delete []m_sDRMUniqueIden;
				m_sDRMUniqueIden = NULL;
			}

			int len = strlen((char *)pValue);
			m_sDRMUniqueIden =  new char[len+ 3];
			memset(m_sDRMUniqueIden, 0, sizeof(len+ 3));
			strcpy(m_sDRMUniqueIden, (char *)pValue);

			m_bNeedSetDRMUniqueIdentifier = true;

			return VOOSMP_ERR_None;
		}
	}
	else if(nID == VOOSMP_SRC_PID_DRM_FUNC_SET)
	{
		m_pGetDRM2API = *((VOGETDRM2API *)pValue);
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_PID_MIN_BUFFTIME)
	{
		m_nMinBufferTime = (int)( *( (int*)pValue ) );
		if (!m_pBaseSource) m_bNeedSetMinBufferTime = true;
	}
	else if (nID == VOOSMP_PID_LICENSE_TEXT)
	{
		char* pLisValue = (char*)pValue;
		int nStrLength = strlen(pLisValue);
		if (m_pLicenseText) delete m_pLicenseText;
		m_pLicenseText = new char[ nStrLength + 1];
		if (m_pLicenseText)
		{
			strcpy (m_pLicenseText, pLisValue);
		}
		if (!m_pBaseSource) m_bNeedSetLicenseText = true;
	}
	else if (nID == VOOSMP_PID_LICENSE_FILE_PATH)
	{
		char* pLisFilePathValue = (char*)pValue;
		int nStrLength = strlen( pLisFilePathValue );
		if (m_pLicenseFilePath) delete m_pLicenseFilePath;
		m_pLicenseFilePath = new char[ nStrLength + 1];
		if (m_pLicenseFilePath)
		{
			strcpy (m_pLicenseFilePath, pLisFilePathValue);
		}
		if (!m_pBaseSource) m_bNeedSetLicenseFilePath = true;
	}
	else if (nID == VOOSMP_PID_LICENSE_CONTENT)
	{
		char* pLisContent = (char*)pValue;
		int nStrLength = strlen( pLisContent );
		if (m_pLicenseContent) delete m_pLicenseContent;
		m_pLicenseContent = new char[ nStrLength + 1];
		if (m_pLicenseContent)
		{
			strcpy (m_pLicenseContent, pLisContent);
		}
		if (!m_pBaseSource ) m_bNeedSetLicenseContent = true;
	}
	else if (nID == VOOSMP_PID_SUBTITLE_FILE_NAME)
	{
		char* pSubTitleFileName = (char*)pValue;
		int nStrLength = strlen( pSubTitleFileName );
		if (m_pSubtitleFileName) delete m_pSubtitleFileName;
		m_pSubtitleFileName = new char[ nStrLength + 1];
		if (m_pSubtitleFileName)
		{
			strcpy (m_pSubtitleFileName, pSubTitleFileName);
		}
		if (!m_pBaseSource) m_bNeedSetSubtitleFileName = true;
	}
	else if (nID == VOOSMP_SRC_PID_FUNC_READ_BUF)
	{
		memcpy (&m_sReadBufferFunction, pValue, sizeof (VOOSMP_READBUFFER_FUNC));
		if (!m_pBaseSource) m_bNeedSetReadBufferFunction = true;
	}
	else if (nID == VOOSMP_SRC_PID_CC_AUTO_SWITCH_DURATION)
	{
		m_nCCAutoSwitchDuration = (int)( *( (int*)pValue ) );
		if (!m_pBaseSource) m_bNeedSetCCAutoSwitchDuration = true;
	}
	else if(nID == VOOSMP_PID_PERFORMANCE_DATA)
	{
		VOOSMP_PERF_DATA* pOSCapData = (VOOSMP_PERF_DATA *)pValue;
		m_sCapData.nCodecType = pOSCapData->nCodecType;
		m_sCapData.nBitRate = pOSCapData->nBitRate;
		m_sCapData.nVideoWidth = pOSCapData->nVideoWidth;
		m_sCapData.nVideoHeight = pOSCapData->nVideoHeight;
		m_sCapData.nProfileLevel = pOSCapData->nProfileLevel;
		m_sCapData.nFPS = pOSCapData->nFPS;
		if (!m_pBaseSource) m_bNeedSetPerformanceData = true;
	}
	else if(nID == VOOSMP_SRC_PID_BA_STARTCAP)
	{
		VOOSMP_PERF_DATA* pOSCapData = (VOOSMP_PERF_DATA *)pValue;
		m_sCapData.nCodecType = pOSCapData->nCodecType;
		m_sCapData.nBitRate = pOSCapData->nBitRate;
		m_sCapData.nVideoWidth = pOSCapData->nVideoWidth;
		m_sCapData.nVideoHeight = pOSCapData->nVideoHeight;
		m_sCapData.nProfileLevel = pOSCapData->nProfileLevel;
		m_sCapData.nFPS = pOSCapData->nFPS;
		if (!m_pBaseSource) m_bNeedSetBAStartCap = true;
	}
	else if(nID == VOOSMP_SRC_PID_LOW_LATENCY_MODE)
	{
		m_nLowLatencyMode =  (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetLowLatencyMode = true;
	}
	else if(nID == VOOSMP_SRC_PID_PD_CONNECTION_RETRY_TIMES)
	{
		m_nPDContentRetryTimes =  (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetPDContentRetryTimes = true;
	}
	else if (nID == VOOSMP_SRC_PID_HTTPVERIFICATIONCALLBACK)
	{
		memcpy (&m_sHttpVerifyCallback, pValue, sizeof (VOOSMP_SRC_HTTP_VERIFYCALLBACK));
	}
	else if (nID == VOOSMP_SRC_PID_DOHTTPVERIFICATION)
	{
		memcpy (&m_sVerificationInfo, pValue, sizeof (VOOSMP_SRC_VERIFICATIONINFO));
		if (!m_pBaseSource) m_bNeedSetVerificationInfo = true;
	}
	else if (nID == VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE)
	{
		m_nSocketConnectionType = (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetSocketConnectionType = true;
	}
	else if (nID == VOOSMP_SRC_PID_COMMAND_STRING)
	{
		char* pCmdString = (char*)pValue;
		int nStrLength = strlen( pCmdString );
		if (m_pCommandString) delete m_pCommandString;
		m_pCommandString = new char[ nStrLength + 1];
		if (m_pCommandString)
		{
			strcpy (m_pCommandString, pCmdString);
		}
		if (!m_pBaseSource) m_bNeedSetCommandString = true;
	}
	else if (nID == VOOSMP_SRC_PID_CAP_TABLE_PATH)
	{
		char* pCapTablePath = (char*)pValue;
		int nStrLength = strlen( pCapTablePath );
		if (m_pCapTablePath) delete m_pCapTablePath;
		m_pCapTablePath = new char[ nStrLength + 1];
		if (m_pCapTablePath)
		{
			strcpy (m_pCapTablePath, pCapTablePath);
		}
		if (!m_pBaseSource) m_bNeedSetCapTablePath = true;
	}
	else if (nID == VOOSMP_SRC_PID_DOWNLOAD_FAIL_MAX_TOLERANT_COUNT)
	{
		m_nMaxDownloadFaildTolerantCount =  (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetMaxDownloadFaildTolerantCount = true;
	}
	else if (nID == VOOSMP_SRC_PID_DISABLE_CPU_ADAPTION)
	{
		m_nCPUAdaptionOnOff =  (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetCPUAdaptionOnOff = true;
	}
	else if (nID == VOOSMP_SRC_PID_BUFFER_START_BUFFERING_TIME)
	{
		m_nStartBufferingTime = (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetStartBufferingTime = true;
	}
	else if (nID == VOOSMP_SRC_PID_BUFFER_BUFFERING_TIME)
	{
		m_nBufferingTime =  (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetBufferingTime = true;
	}
	else if (nID == VOOSMP_SRC_PID_DVRINFO)
	{
		memcpy (&m_sDvrInfo, pValue, sizeof (VOOSMP_SRC_DVRINFO));
		if (!m_pBaseSource) m_bNeedSetDvrInfo = true;
	}
	else if (nID == VOOSMP_SRC_PID_BA_WORKMODE)
	{
		m_nBAWorkingMode = (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetBAWorkingMode = true;
	}
	else if (nID == VOOSMP_SRC_PID_RTSP_STATS)
	{
		m_nRTSPStats =  (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetRTSPStats = true;
	}
	else if (nID == VOOSMP_SRC_PID_BUFFER_MAX_SIZE)
	{
		m_nBufferMaxSize =  (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetBufferMaxSize = true;
	}
	else if (nID == VOOSMP_SRC_PID_RTSP_CONNECTION_PORT)
	{
		m_nRTSPConnectionPort = (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetRTSPConnectionPort = true;
	}
	else if (nID == VOOSMP_SRC_PID_PRESENTATION_DELAY)
	{
		m_nPresentationDelay = (int)(*(int*)pValue);
		if (!m_pBaseSource) m_bNeedSetPresentationDelay = true;
	}
	else if (nID == VOOSMP_SRC_PID_PERIOD2TIME)
	{
		memcpy (&m_nPerIOToTime, pValue, sizeof (VOOSMP_SRC_PERIODTIME_INFO));
		if (!m_pBaseSource) m_bNeedSetPerIOToTime = true;
	}
	else if (nID == VOOSMP_SRC_PID_BITRATE_THRESHOLD)
	{
		VOOSMP_SRC_BA_THRESHOLD* pBAThreshold = (VOOSMP_SRC_BA_THRESHOLD*)pValue;
		m_sBAThreshold.nLower = pBAThreshold->nLower;
		m_sBAThreshold.nUpper = pBAThreshold->nUpper;
		VOLOGI("@@@### Set bitrate threshold: Upper = %d,  Lower = %d", pBAThreshold->nUpper, pBAThreshold->nLower);
		if (!m_pBaseSource) m_bNeedSetBAThreshold = true;
	}
	else if (nID == VOOSMP_SRC_PID_SEGMENT_DOWNLOAD_RETRY_COUNT)
	{
		m_nTotalDownLoadErrorTolerate = (int)(*(int*)pValue);
		VOLOGI("@@@### Set total download error tolerate:  %d", m_nTotalDownLoadErrorTolerate);
		if (!m_pBaseSource) m_bNeedSetTotalDownLoadErrorTolerate = true;
	}
	else if (nID == VOOSMP_SRC_PID_ENABLE_RTSP_HTTP_TUNNELING)
	{
		m_nEnableRTSPoverHTTPTunnel = (int)(*(int*)pValue);
		VOLOGI("@@@### Set RTSP voer HTTP tunnel enable:  %d", m_nEnableRTSPoverHTTPTunnel);
		if (!m_pBaseSource) m_bNeedSetEnableRTSPoverHTTPTunnel = true;
	}
	else if (nID == VOOSMP_SRC_PID_RTSP_OVER_HTTP_CONNECTION_PORT)
	{
		m_nRTSPOverHTTPConnectPort = (int)(*(int*)pValue);
		VOLOGI("@@@### Set RTSP voer HTTP ConnetctPort:  %d", m_nRTSPOverHTTPConnectPort);
		if (!m_pBaseSource) m_bNeedSetRTSPOverHTTPConnectPort = true;
	}
	else if (nID == VOOSMP_SRC_PID_UPDATE_SOURCE_URL)
	{
		VOLOGI("@@@### Set Update URL :  %s", (char*)pValue);
		if (pValue && !m_pBaseSource)
		{
			char* pTmpURL = (char*)pValue;
			m_pUpdateURL = new char[ strlen (pTmpURL) + 1];
			strcpy (m_pUpdateURL, pTmpURL);
			m_bNeedSetUpdateURL = true;
		}
	}
	else if (nID == VOOSMP_SRC_PID_HTTP_RETRY_TIMEOUT)
	{
		m_nHttpRetryTimeOut = (int)(*(int*)pValue);
		VOLOGI("@@@### Set HTTP retry time out:  %d", m_nHttpRetryTimeOut);
		if (!m_pBaseSource) m_bNeedSetHttpRetryTimeOut = true;
	}
	else if (nID == VOOSMP_SRC_PID_DEFAULT_AUDIO_LANGUAGE)
	{
		char* pLanguage = (char*)pValue;
		VOLOGI("@@@### Set Dufatult audio language: %s", pLanguage);
		int nStrLength = strlen( pLanguage );
		if (m_szDefaultAudioLanguage) delete m_szDefaultAudioLanguage;
		m_szDefaultAudioLanguage = new char[ nStrLength + 1];
		if (m_szDefaultAudioLanguage)
		{
			strcpy (m_szDefaultAudioLanguage, pLanguage);
			if (!m_pBaseSource) m_bNeedSetDefaultAudioLanguage = true;
		}
	}
	else if (nID == VOOSMP_SRC_PID_DEFAULT_SUBTITLE_LANGUAGE)
	{
		char* pLanguage = (char*)pValue;
		VOLOGI("@@@### Set Dufatult subtitle language: %s", pLanguage);
		int nStrLength = strlen( pLanguage );
		if (m_szDefaultSubLanguage) delete m_szDefaultSubLanguage;
		m_szDefaultSubLanguage = new char[ nStrLength + 1];
		if (m_szDefaultSubLanguage)
		{
			strcpy (m_szDefaultSubLanguage, pLanguage);
			if (!m_pBaseSource) m_bNeedSetDefaultSubLanguage = true;
		}
	}
	else
	{
		nRC = VOOSMP_ERR_Implement;
	}

	if(m_pBaseSource)
	{
		int vonID = nID;
		void *vopValue = pValue;
		
		if(nID == VOOSMP_SRC_PID_CC_AUTO_SWITCH_DURATION)
		{
			vonID = VO_PID_SOURCE2_CC_AUTO_SWITCH_DURATION;
		}
		else if(nID == VOOSMP_PID_COMMAND_STRING)
		{
			vonID = VO_PID_SOURCE2_CONFIGSTRING;
		}
		else if(nID == VOOSMP_PID_CPU_INFO)
		{
			vonID = VO_PID_SOURCE2_CPUINFO;
			VOOSMP_CPU_INFO* pOSCPUInfo = (VOOSMP_CPU_INFO *)pValue;
			
			m_sCPUInfo.nCoreCount = pOSCPUInfo->nCoreCount;
			m_sCPUInfo.nCPUType = pOSCPUInfo->nCPUType;
			m_sCPUInfo.nFrequency = pOSCPUInfo->nFrequency;
			m_sCPUInfo.llReserved = pOSCPUInfo->llReserved;

			vopValue = (void *)(&m_sCPUInfo);
		}
		else if(nID == VOOSMP_PID_PERFORMANCE_DATA)
		{
			vonID = VO_PID_SOURCE2_BACAP;
			vopValue = (void *)(&m_sCapData);
		}
		else if(nID == VOOSMP_SRC_PID_BA_STARTCAP)
		{
			vonID = VO_PID_SOURCE2_BA_STARTCAP;
			vopValue = (void *)(&m_sCapData);
		}
		else if(nID == VOOSMP_SRC_PID_DOHTTPVERIFICATION)
		{
			vonID = VO_PID_SOURCE2_DOHTTPVERIFICATION;
		}
		else if(nID == VOOSMP_SRC_PID_DOWNLOAD_FAIL_MAX_TOLERANT_COUNT)
		{
			vonID = VO_PID_SOURCE2_DOWNLOAD_FAIL_MAX_TOLERANT_COUNT;
		}
		else if(nID == VOOSMP_SRC_PID_BUFFER_START_BUFFERING_TIME)
		{
			vonID = VO_PID_SOURCE2_BUFFER_STARTBUFFERINGTIME;
		}
		else if(nID == VOOSMP_SRC_PID_BUFFER_BUFFERING_TIME)
		{
			vonID =  VO_PID_SOURCE2_BUFFER_BUFFERINGTIME;
		}
		else if(nID == VOOSMP_SRC_PID_LOW_LATENCY_MODE)
		{
			vonID =  VO_PID_SOURCE2_LOWLATENCYMODE;
		}
		else if(nID == VOOSMP_SRC_PID_DISABLE_CPU_ADAPTION)
		{
			vonID =  VO_PID_SOURCE2_BA_CPUBADISABLE;
		}
		else if(nID == VOOSMP_SRC_PID_BUFFER_MAX_SIZE)
		{
			vonID =  VO_PID_SOURCE2_BUFFER_MAXBUFFERSIZE;
		}
		else if(nID == VOOSMP_SRC_PID_PRESENTATION_DELAY)
		{
			vonID = VO_PID_SOURCE2_LIVELATENCY;
		}
		else if(nID == VOOSMP_SRC_PID_PD_CONNECTION_RETRY_TIMES)
		{
			vonID = VOID_STREAMING_PDCONNECTRETRY_TIMES;
		}
		else if (nID == VOOSMP_SRC_PID_PERIOD2TIME)
		{
			vonID = VO_PID_SOURCE2_PERIOD2TIME;
		}
		else if (nID == VOOSMP_SRC_PID_APPLICATION_SUSPEND)
		{
			vonID = VO_PID_SOURCE2_APPLICATION_SUSPEND;
		}
		else if (nID == VOOSMP_SRC_PID_APPLICATION_RESUME)
		{
			vonID = VO_PID_SOURCE2_APPLICATION_RESUME;
		}
		else if (nID == VOOSMP_PID_AUDIO_PLAYBACK_SPEED)
		{
			vonID = VO_PID_SOURCE2_PLAYBACK_SPEED;
		}
		else if (nID == VOOSMP_SRC_PID_BITRATE_THRESHOLD)
		{
			vonID = VO_PID_SOURCE2_BITRATE_THRESHOLD;
		}
		else if (nID == VOOSMP_SRC_PID_SEGMENT_DOWNLOAD_RETRY_COUNT)
		{
			vonID = VO_PID_SOURCE2_TOLERANT_ERROR_NUMBER;
		}
		else if (nID == VOOSMP_SRC_PID_ENABLE_RTSP_HTTP_TUNNELING)
		{
			vonID = VO_PID_SOURCE2_ENABLE_RTSP_HTTP_TUNNELING;
		}
		else if (nID == VOOSMP_SRC_PID_RTSP_OVER_HTTP_CONNECTION_PORT)
		{
			vonID = VO_PID_SOURCE2_RTSP_OVER_HTTP_CONNECTION_PORT;
		}
		else if (nID == VOOSMP_SRC_PID_UPDATE_SOURCE_URL)
		{
			vonID = VO_PID_SOURCE2_UPDATE_SOURCE_URL;
		}
		else if (nID == VOOSMP_SRC_PID_HTTP_RETRY_TIMEOUT)
		{
			vonID = VO_PID_SOURCE2_HTTP_RETRY_TIMEOUT;
		}
		else if (nID == VOOSMP_SRC_PID_DEFAULT_AUDIO_LANGUAGE)
		{
			vonID = VO_PID_SOURCE2_DEFAULT_AUDIO_LANGUAGE;
		}
		else if (nID == VOOSMP_SRC_PID_DEFAULT_SUBTITLE_LANGUAGE)
		{
			vonID = VO_PID_SOURCE2_DEFAULT_SUBTITLE_LANGUAGE;
		}

		return m_pBaseSource->SetParam(vonID, vopValue);
	}

	return nRC;
}

int COSSourceBase::SetJavaVM(void *pJavaVM, void* jobj)
{
#ifdef _LINUX	
	m_JavaVM = (JavaVM *)pJavaVM;
	m_jOSSource = (jobject)jobj;


	CJniEnvUtil	env(m_JavaVM);

	if(env.getEnv() == NULL)
		return VOOSMP_ERR_Unknown;

	//if(m_clsChunk)
	//{
	//	env.getEnv()->DeleteGlobalRef(m_clsChunk);
	//	m_clsChunk = 0;
	//}

	//if(m_clsTimedTag)
	//{
	//	env.getEnv()->DeleteGlobalRef(m_clsTimedTag);
	//	m_clsTimedTag = 0;
	//}

	//m_clsChunk = env.getEnv()->FindClass ("com/visualon/OSMPUtils/voOSChunkInfoImpl");
	//if (m_clsChunk == NULL)
	//{
	//	VOLOGE("Get com.visualon.OSMPUtils/voOSChunkInfoImpl class error");
	//}

	//m_midChunk = env.getEnv()->GetMethodID (m_clsChunk, "<init>", "(ILjava/lang/String;Ljava/lang/String;JJJII)V");
	//if (m_midChunk == NULL)
	//{
	//	VOLOGE("Get construct of com.visualon.OSMPUtils/voOSChunkInfoImpl error!");
	//}


	//m_clsTimedTag = env.getEnv()->FindClass ("com/visualon/OSMPUtils/voOSTimedTagImpl");
	//if (m_clsTimedTag == NULL)
	//{
	//	VOLOGE("Get com.visualon.OSMPUtils/voOSTimedTagImpl class error");
	//}

	//m_midTimedTag = env.getEnv()->GetMethodID (m_clsTimedTag, "<init>", "(JI[BILjava/lang/Object;)V");
	//if (m_midTimedTag == NULL)
	//{
	//	VOLOGE("Get construct of com.visualon.OSMPUtils/voOSTimedTagImpl error!");
	//}

#endif

	return VOOSMP_ERR_None;
}

void * COSSourceBase::vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag)
{
#ifdef _LINUX
	void * ptr = voGetAddressSymbol (hLib, pFuncName);
	return ptr;
#elif defined  _WIN32
	void* pFunc = NULL;

#ifndef WINCE
	pFunc = GetProcAddress ((HMODULE)hLib, pFuncName);
#else //WINCE
	TCHAR		szLibName[1024];

	memset (szLibName, 0, 1024 * sizeof(TCHAR));
	::MultiByteToWideChar (CP_ACP, 0, pFuncName, -1, szLibName, sizeof (szLibName));
	pFunc = GetProcAddress ((HMODULE)hLib, szLibName);
#endif //WINCE

	return pFunc;
#elif defined  _IOS
    voGetModuleRealAPI pFunction = (voGetModuleRealAPI)voGetModuleAdapterFunc(pFuncName);
    
    if (pFunction) {
        return pFunction();
    }
    
    return NULL;
#elif defined _MAC_OS
    if ((NULL == hLib) || (NULL == pFuncName))
		return NULL;
    
    return dlsym (hLib, pFuncName);
#else
	return 0;
#endif
}

int COSSourceBase::vomtFreeLib (void * pUserData, void * hLib, int nFlag)
{
#ifdef _LINUX
	voFreeLibrary (hLib);
#elif defined _WIN32
	FreeLibrary ((HMODULE)hLib);
#elif defined _MAC_OS
    if (hLib != NULL)
		dlclose (hLib);
#endif
	return 0;
}

void * COSSourceBase::vomtLoadLib (void * pUserData, char * pLibName, int nFlag)
{
#ifdef _IOS
    VOLOGI("Load lib name: %s", pLibName);
    // Return a no-zero value to continue load on iOS
    return (void *)1;
#endif
    
	COSSourceBase *pSource = (COSSourceBase *)pUserData;
#ifdef _LINUX
	char szFile[1024];
	char tempFile[1024];

	strcpy (szFile, pSource->m_szPathLib);
//	strcat (szFile, "lib/lib");

	memset(tempFile, 0, sizeof(tempFile));
	strcpy(tempFile, "/data/local/tmp/OnStreamPlayer/lib/");
	if (strstr(pLibName, "lib") == 0)
	{
		strcat(szFile, "lib");
		strcat(tempFile, "lib");
	}

	strcat(szFile, pLibName); 
	strcat(tempFile, pLibName);

	VOLOGI("Load lib name: %s",szFile);

    if( strstr( szFile , ".so" ) == 0 )
    {
       strcat( szFile , ".so" );
	   strcat( tempFile, ".so" );
    }

	int n;
	if(pSource->m_nPrefixEnable)
	{
		char PreFile[1024];
		char *p = strstr(szFile, "libvo");

		if(p)
		{
			p = p + 5;
			n = (int)(p - szFile);
			memset(PreFile, 0, 1024);
			strncpy(PreFile, szFile, n);
			strcat(PreFile, pSource->m_cPrefix);
			strcat(PreFile, p);
			strcpy(szFile, PreFile);
		}

		p = strstr(tempFile, "libvo");
		if(p)
		{
			p = p + 5;
			n = (int)(p - tempFile);
			memset(PreFile, 0, 1024);
			strncpy(PreFile, tempFile, n);
			strcat(PreFile, pSource->m_cPrefix);
			strcat(PreFile, p);

			strcpy(tempFile, PreFile);
		}
	}

	if(pSource->m_nSuffixEnable)
	{
		char SufFile[1024];
		char *p = strstr(szFile, ".so");

		if(p)
		{
			n = (int)(p - szFile);
			memset(SufFile, 0, 1024);
			strncpy(SufFile, szFile, n);
			strcat(SufFile, pSource->m_cSuffix);
			strcat(SufFile, p);
			strcpy(szFile, SufFile);
		}

		p = strstr(tempFile, ".so");
		if(p)
		{
			n = (int)(p - tempFile);
			memset(SufFile, 0, 1024);
			strncpy(SufFile, tempFile, n);
			strcat(SufFile, pSource->m_cSuffix);
			strcat(SufFile, p);

			strcpy(tempFile, SufFile);
		}
	}

	void * hDll = NULL;
		
	if(voOS_EnableDebugMode(0))
	{
		hDll = voLoadLibrary (tempFile, RTLD_NOW);
	}

	if (hDll == NULL)
	{
		hDll = voLoadLibrary (szFile, RTLD_NOW);

		if (hDll == NULL)
		{
			strcpy (szFile, pSource->m_szPathLib);
			strcat (szFile, "lib/");
			strcat (szFile, pLibName);

	        if( strstr( szFile , ".so" ) == 0 )
	            strcat( szFile , ".so" );

			hDll = voLoadLibrary (szFile, RTLD_NOW);
		}
	}

	return hDll;
#elif defined _METRO
	HMODULE hDll = NULL;

	TCHAR	szDll[1024];
	TCHAR	szLibName[1024];

	memset (szDll, 0, 1024);
	memset (szLibName, 0, 1024);
	::MultiByteToWideChar (CP_ACP, 0, pLibName, -1, szLibName, sizeof (szLibName));

	if (hDll == NULL)
	{
		_tcscat (szDll, szLibName); 
		hDll = LoadPackagedLibrary(szDll , 0);
		if (hDll == NULL)
		{
			_tcscat (szDll, _T(".Dll"));
			hDll = LoadPackagedLibrary (szDll , 0);
		}
	}

	return hDll;

#elif defined _WIN32
	void* hDll = NULL;

	TCHAR	szDll[1024];
	TCHAR	szPath[1024];
	TCHAR		szLibName[1024];

	memset (szLibName, 0, 1024);
	::MultiByteToWideChar (CP_ACP, 0, pLibName, -1, szLibName, sizeof (szLibName));

	vostrcpy (szDll, pSource->m_szPathLib);
	vostrcat (szDll, szLibName);
	hDll = LoadLibrary (szDll);

	if (hDll == NULL)
	{
		voOS_GetModuleFileName (NULL, szPath, sizeof (szPath));

		TCHAR * pPos = _tcsrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;

			_tcscpy (szDll, szPath);
			_tcscat (szDll, szLibName);
			hDll = LoadLibrary (szDll);
		}
	}

	if (hDll == NULL)
	{
		::voOS_GetModuleFileName (NULL, szPath, sizeof (szPath));
		TCHAR * pPos = _tcsrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;

			_tcscpy (szDll, szPath);
			_tcscat (szDll, szLibName);
			_tcscat (szDll, _T(".Dll"));
			hDll = LoadLibrary (szDll);
		}
	}

	if (hDll == NULL)
	{
		_tcscpy (szDll, szPath);
		_tcscat (szDll, _T("Dll\\"));
		_tcscat (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	if (hDll == NULL)
	{
		_tcscpy (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	return hDll;
    
#elif _MAC_OS
    
    if (NULL == pLibName) {
        return NULL;
    }
    
    VO_TCHAR libname[256] = {0};
    VO_TCHAR PathAndName [1024] = {0};
    
    if (0 != strncasecmp (pLibName, "lib", 3))
    {
        vostrcat(libname, "lib");
    }
    vostrcat(libname, pLibName);
    if (vostrstr(libname, ".dylib") == 0)
    {
        vostrcat(libname, ".dylib");
    }
    
	bool bAPP = false;
	VOLOGI ("dllfile: %s, workpath: %s!", libname, pSource->m_szPathLib);
    
    void* hDll = NULL;
    
	if (hDll == NULL && strcmp(pSource->m_szPathLib, ""))
	{
		bAPP = true;
        
		vostrcpy(PathAndName, pSource->m_szPathLib);
        
        if (pSource->m_szPathLib[strlen(pSource->m_szPathLib) - 1] != '/') {
            vostrcat(PathAndName, "/");
        }
        
		vostrcat(PathAndName, libname);
        
		hDll = dlopen (PathAndName, RTLD_NOW);
	}
    
	VOLOGI ("bAPP: %d", bAPP);
    
    if (!bAPP && hDll == NULL)
	{
        voOS_GetAppFolder(PathAndName, sizeof(PathAndName));
        vostrcat(PathAndName, libname);
        hDll = dlopen (PathAndName, RTLD_NOW);
        if (NULL == hDll)
        {
            voOS_GetAppFolder(PathAndName, sizeof(PathAndName));
            vostrcat(PathAndName, "lib/");
            vostrcat(PathAndName, libname);
            hDll = dlopen (PathAndName, RTLD_NOW);
        }
        
        if (NULL == hDll)
        {
            voOS_GetPluginModuleFolder(PathAndName, sizeof(PathAndName));
            vostrcat(PathAndName, libname);
            hDll = dlopen (PathAndName, RTLD_NOW);
        }
        
        if (NULL == hDll)
        {
            vostrcpy(PathAndName, "/Library/Internet Plug-Ins/");
            vostrcat(PathAndName, libname);
            hDll = dlopen (PathAndName, RTLD_NOW);
        }
        
        if (NULL == hDll)
        {
            hDll = dlopen(libname, RTLD_NOW);
        }
        
        if (NULL == hDll)
        {
            if (strrchr(libname, '/') == NULL)
                vostrcpy(PathAndName, "/usr/lib/");
            
            vostrcat (PathAndName, libname);
            hDll = dlopen (PathAndName, RTLD_NOW);
        }
	}
    
	if (hDll)
	{
		VOLOGI("load: %s ok", PathAndName);
	}
    else {
        VOLOGI("load: %s error", libname);
    }
    
    return hDll;
#endif
}

VO_U32 COSSourceBase::OSIOCallBackFunc( VO_PTR hHandle , VO_U32 uID , VO_PTR pParam1 , VO_PTR pParam2 )
{
	COSSourceBase *pSourceBase = (COSSourceBase *)hHandle;
	
	return pSourceBase->doHttpIOCallBackFunc(uID, pParam1, pParam2);
}

VO_U32 COSSourceBase::doHttpIOCallBackFunc(VO_U32 nID , VO_PTR pParam1 , VO_PTR pParam2)
{
	VOLOGI("@@@###  callback nID = 0x%08X", nID);
	int nRC = 0;
	int vonID = nID;
	void* ospPamam1 = pParam1;
	void* ospParam2 = NULL;

	VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION sParam2;
	memset(&sParam2, 0, sizeof(VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION));

	ospParam2 = (void *)(&sParam2);

	switch(nID)
	{
	case VO_SOURCE2_IO_HTTP_STARTDOWNLOAD:
		vonID = VOOSMP_SRC_CB_IO_HTTP_Start_Download;
		break;

	case VO_SOURCE2_IO_HTTP_DOWNLOADFAILED:
		vonID = VOOSMP_SRC_CB_IO_HTTP_Download_Failed;

		if(pParam2)
		{
			VO_SOURCE2_IO_FAILEDREASON_DESCRIPTION *pDesp = (VO_SOURCE2_IO_FAILEDREASON_DESCRIPTION *)pParam2;

			if(pDesp->reason == VO_SOURCE2_IO_HTTP_CONNECT_FAILED)
				sParam2.reason = VOOSMP_IO_HTTP_CONNECT_FAILED;
			else if(pDesp->reason == VO_SOURCE2_IO_HTTP_INVALID_RESPONSE)
				sParam2.reason = VOOSMP_IO_HTTP_INVALID_RESPONSE;
			else if(pDesp->reason == VO_SOURCE2_IO_HTTP_CLIENT_ERROR)
				sParam2.reason = VOOSMP_IO_HTTP_CLIENT_ERROR;
			else if(pDesp->reason == VO_SOURCE2_IO_HTTP_SERVER_ERROR)
				sParam2.reason = VOOSMP_IO_HTTP_SERVER_ERROR;

			VOLOGE("@@@###  Reason = %d, Response = %s, strSize = %d", pDesp->reason, pDesp->szResponse, pDesp->uResponseSize);

			sParam2.pszResponse = pDesp->szResponse;
			sParam2.nResponseSize = pDesp->uResponseSize;
		}

		break;
	}

	voCAutoLock lockVideoSrc (&m_MutexOnQuest);
#ifdef _LINUX
	CJniEnvUtil	env(m_JavaVM);

	if(env.getEnv() == NULL)
		return VOOSMP_ERR_Unknown;

	jobject jobj = NULL;

	CJavaParcelWrapOSMP parc(env.getEnv());
	parc.fillParcelData((VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION *)ospParam2, (char *)ospPamam1);
	jobj = parc.getParcel();

	jclass clazz = env.getEnv()->GetObjectClass(m_jOSSource);
	if (clazz == NULL) {
		return VOOSMP_ERR_Unknown;
	}

	jmethodID RequestCallBack = env.getEnv()->GetMethodID(clazz, "RequestCallBack", "(IIILjava/lang/Object;)I");
	if (RequestCallBack == NULL) {
		return VOOSMP_ERR_Unknown;
	}

	nRC = env.getEnv()->CallIntMethod(m_jOSSource, RequestCallBack, vonID, 0, 0, jobj);

	env.getEnv()->DeleteLocalRef(clazz);
#else
	if(m_lAPOnReqest.pListener)
		nRC = m_lAPOnReqest.pListener(m_lAPOnReqest.pUserData, vonID, ospPamam1, ospParam2);
#endif
	
	if((unsigned int)nRC == VOOSMP_ERR_Retry)
		return VO_SOURCE2_IO_RETRY;
	else if((unsigned int)nRC == VOOSMP_ERR_Unknown)
		return VO_SOURCE2_IO_FAIL;
	else if((unsigned int)nRC == VOOSMP_ERR_None)
		return VO_SOURCE2_IO_OK;

	return nRC;
}

int COSSourceBase::OSSourceListener(void * pUserData, int nID, void * pParam1, void * pParam2)
{
	int nRC = 0;
	COSSourceBase *pSourceBase = (COSSourceBase *)pUserData;
	voCAutoLock lockVideoSrc (&pSourceBase->m_MutexEvent);
	nRC = pSourceBase->HandleEvent(nID, pParam1, pParam2);
	return nRC;
}

int COSSourceBase::HandleOnQest(int nID, void * pParam1, void * pParam2)
{
	int nRC = 0;
#ifdef _LINUX
	int nParam1 = 0;
	if(pParam1) nParam1 = *(((int *)pParam1));
	int nParam2 = 0;
	if(pParam2) nParam2 = *(((int *)pParam2));

	CJniEnvUtil	env(m_JavaVM);

	if(env.getEnv() == NULL)
		return VOOSMP_ERR_Unknown;

	jclass clazz = env.getEnv()->GetObjectClass(m_jOSSource);
	if (clazz == NULL) {
		return VOOSMP_ERR_Unknown;
	}

	jmethodID RequestCallBack = env.getEnv()->GetMethodID(clazz, "RequestCallBack", "(IIILjava/lang/Object;)I");
	if (RequestCallBack == NULL) {
		return VOOSMP_ERR_Unknown;
	}

	nRC = env.getEnv()->CallIntMethod(m_jOSSource, RequestCallBack, nID, nParam1, nParam2, NULL);


	env.getEnv()->DeleteLocalRef(clazz);
#else
	if(m_lAPOnReqest.pListener)
		nRC = m_lAPOnReqest.pListener(m_lAPOnReqest.pUserData, nID, pParam1, pParam2);
#endif

	return nRC;	
}

int COSSourceBase::OSSourceOnQest(void * pUserData, int nID, void * pParam1, void * pParam2)
{
	COSSourceBase *pSourceBase = (COSSourceBase *)pUserData;
	
	voCAutoLock lockVideoSrc (&pSourceBase->m_MutexOnQuest);

	return pSourceBase->HandleOnQest(nID, pParam1, pParam2);
}

VO_S32 COSSourceBase::OSDRMListener(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	COSSourceBase *pSourceBase = (COSSourceBase *)pUserData;
	voCAutoLock lockVideoSrc (&pSourceBase->m_MutexEvent);	

	int nID_DRM = nID;

	if(nID == VO_EVENTID_SOURCE2_ERR_DRMFAIL)
	{
		nID_DRM = VOOSMP_SRC_CB_DRM_Fail;
	}
	else if(nID == VO_EVENTID_DRM2_ERR_NOT_SECURE)
	{
		nID_DRM = VOOSMP_SRC_CB_DRM_Not_Secure;
	}
	else if(nID == VO_EVENTID_DRM2_ERR_POLICY_FAIL)
	{
		nID_DRM = VOOSMP_SRC_CB_DRM_AV_Out_Fail;
	}

	return pSourceBase->HandleEvent(nID_DRM, &nParam1, &nParam2);
}

int COSSourceBase::HandleEvent(int nID, void * pParam1, void * pParam2)
{
	int nRC = 0;
#ifdef _LINUX	
	int nParam1 = 0;
	if(pParam1) nParam1 = *(((int *)pParam1));
	int nParam2 = 0;
	if(pParam2) nParam2 = *(((int *)pParam2));

	CJniEnvUtil	env(m_JavaVM);

	if(env.getEnv() == NULL)
		return VOOSMP_ERR_Unknown;

	jobject jobj = NULL;
	int async = 0;

	if(nID == VOOSMP_SRC_CB_Adaptive_Streaming_Info)
	{
		async = 1;
		if( nParam1 == VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD ||
			nParam1 == VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED ||
			nParam1 == VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK)
		{
			VOOSMP_SRC_CHUNK_INFO* pChunInfo = (VOOSMP_SRC_CHUNK_INFO*)nParam2;

			//if (m_clsChunk == NULL)
			//{
			//	VOLOGE("Get com.visualon.OSMPUtils/voOSChunkInfoImpl class error");
			//	return VOOSMP_ERR_Unknown;
			//}

			//if (m_midChunk == NULL)
			//{
			//	VOLOGE("Get construct of com.visualon.OSMPUtils/voOSChunkInfoImpl error!");
			//	return VOOSMP_ERR_Unknown;
			//}

			CJavaParcelWrapOSMP parc(env.getEnv());
			parc.fillParcelData(pChunInfo);
			jobj = parc.getParcel();
			if(pChunInfo!=NULL && jobj==NULL)
			{
				VOLOGE("VOOSMP_SRC_CB_Adaptive_Streaming_Info pChunInfo!=NULL && jobj==NULL");
			}

			//jobj = env.getEnv()->NewObject (m_clsChunk, m_midChunk, pChunInfo->nType, env.getEnv()->NewStringUTF((const char*)pChunInfo->szRootUrl), 
			//	env.getEnv()->NewStringUTF((const char*)pChunInfo->szUrl),
			//	pChunInfo->ullStartTime, pChunInfo->ullDuration, pChunInfo->ullTimeScale, pChunInfo->uReserved1, pChunInfo->uReserved2);			
		}
		else if( nParam1 == VOOSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_DISCONTINUE_SAMPLE)
			{
			VOOSMP_SRC_CHUNK_SAMPLE* pChunSample = (VOOSMP_SRC_CHUNK_SAMPLE*)nParam2;
			CJavaParcelWrapOSMP parc(env.getEnv());
			parc.fillParcelData(pChunSample);
			jobj = parc.getParcel();
			if(pChunSample!=NULL && jobj==NULL)
			{
				VOLOGE("VOOSMP_SRC_CB_Adaptive_Streaming_Info pChunSample!=NULL && jobj==NULL");
			}
		}
		else if(nParam1 == VO_SOURCE2_ADAPTIVESTREAMING_INFOEVENT_PLAYLIST_DOWNLOADOK)
		{
			// Please do the VOOSMP_SRC_ADAPTIVESTREAMING_PLAYLIST_DATA Parcel
		}
	}
	else if(nID == VOOSMP_SRC_CB_Adaptive_Streaming_Error)
	{
		async = 1;
	}
	else if(nID == VOOSMP_SRC_CB_Adaptive_Stream_Warning)
	{
		async = 1;

		if(nParam1 == VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DOWNLOADERROR)
		{
			VOOSMP_SRC_CHUNK_INFO* pChunInfo = (VOOSMP_SRC_CHUNK_INFO*)nParam2;
			CJavaParcelWrapOSMP parc(env.getEnv());
			parc.fillParcelData(pChunInfo);
			jobj = parc.getParcel();
			if(pChunInfo!=NULL && jobj==NULL)
				VOLOGE("VOOSMP_SRC_CB_Adaptive_Streaming_Info pChunInfo!=NULL && jobj==NULL");
		}
		if(nParam1 == VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_CHUNK_DRMERROR)
		{
			VOOSMP_SRC_CHUNK_INFO* pChunInfo = (VOOSMP_SRC_CHUNK_INFO*)nParam2;
			CJavaParcelWrapOSMP parc(env.getEnv());
			parc.fillParcelData(pChunInfo);
			jobj = parc.getParcel();
			if(pChunInfo!=NULL && jobj==NULL)
				VOLOGE("VOOSMP_SRC_CB_Adaptive_Streaming_Info pChunInfo!=NULL && jobj==NULL");
		}
		else if(nParam1 == VOOSMP_SRC_ADAPTIVE_STREAMING_WARNING_EVENT_PLAYLIST_DOWNLOADERROR)
		{
			// Please do the VOOSMP_SRC_ADAPTIVESTREAMING_PLAYLIST_DATA Parcel
		}
	}
	else if(nID == VOOSMP_SRC_CB_Customer_Tag)
	{
		if(nParam1 == VOOSMP_SRC_CUSTOMERTAGID_TIMEDTAG)
		{
			VOOSMP_SRC_CUSTOMERTAG_TIMEDTAG *pTimeTag = (VOOSMP_SRC_CUSTOMERTAG_TIMEDTAG*)nParam2;
 
			if(pTimeTag->pReserve)
			{
				VOLOGE("pTimeTag->pReserve not finished now,wait for defined");
			}

			//if (m_clsTimedTag == NULL)
			//{
			//	VOLOGE("Get com.visualon.OSMPUtils/voOSTimedTagImpl class error");
			//	return VOOSMP_ERR_Unknown;
			//}

			//if (m_midTimedTag == NULL)
			//{
			//	VOLOGE("Get construct of com.visualon.OSMPUtils/voOSTimedTagImpl error!");
			//	return VOOSMP_ERR_Unknown;
			//}

			CJavaParcelWrapOSMP parc(env.getEnv());
			parc.fillParcelData(pTimeTag);
			jobj = parc.getParcel();
			if(pTimeTag!=NULL && jobj==NULL)
				VOLOGE("VOOSMP_SRC_CB_Customer_Tag pTimeTag!=NULL && jobj==NULL");

			//jcharArray     jData = 0; 
			//jData =   env.getEnv()-> NewCharArray(pTimeTag->uSize); 

   //         if(pTimeTag->pData && jData)
			//	env.getEnv()-> SetCharArrayRegion(jData,   0,   pTimeTag->uSize, (const jchar *)pTimeTag->pData); 

			//jobj = env.getEnv()->NewObject (m_clsTimedTag, m_midTimedTag, pTimeTag->ullTimeStamp, pTimeTag->uSize, jData, pTimeTag->uFlag, jobj1);

			async = 1;
		}
	}

	jclass clazz = env.getEnv()->GetObjectClass(m_jOSSource);
	if (clazz == NULL) {
		return VOOSMP_ERR_Unknown;
	}

	if(async)
	{
		jmethodID RequestCallBack = env.getEnv()->GetMethodID(clazz, "RequestCallBack", "(IIILjava/lang/Object;)I");
		if (RequestCallBack == NULL) {
			return VOOSMP_ERR_Unknown;
		}

		nRC = env.getEnv()->CallIntMethod(m_jOSSource, RequestCallBack, nID, nParam1, nParam2, jobj);
	}
	else
	{
		jmethodID eventCallBack = env.getEnv()->GetMethodID(clazz, "osmpCallBack", "(IIILjava/lang/Object;)V");
		if (eventCallBack == NULL) {
			return VOOSMP_ERR_Unknown;
		}

		env.getEnv()->CallVoidMethod(m_jOSSource, eventCallBack, nID, nParam1, nParam2, jobj);
	}

	env.getEnv()->DeleteLocalRef(clazz);
#endif

  if(m_lAPListener.pListener)
    nRC = m_lAPListener.pListener(m_lAPListener.pUserData, nID, pParam1, pParam2);

	return nRC;
}

VO_U32 COSSourceBase::VerifyCallBackFunc(VO_PTR hHandle , VO_U32 uID, VO_PTR pUserData )
{
	COSSourceBase *pSourceBase = (COSSourceBase *)hHandle;

	return pSourceBase->doVerifyCallBackFunc(uID, pUserData);
}

VO_U32 COSSourceBase::doVerifyCallBackFunc(VO_U32 uID , VO_PTR pUserData)
{
	int nRC = 0;
	int nParam1 = uID;
	int nParam2 = 0;
	int mID = VOOSMP_SRC_CB_Authentication_Request;

	voCAutoLock lockVideoSrc (&m_MutexOnQuest);
		
	if(uID == VO_SOURCE2_CALLBACKIDBASE_DRM)
	{
		nParam1 = VOOSMP_SRC_AUTHENTICATION_DRM_REQUEST_SERVER_INFO;
	}
	else if(uID == VO_SOURCE2_IO_HTTP_BEGIN)
	{
		nParam1 = VOOSMP_SRC_AUTHENTICATION_HTTP_BEGIN;
	}
	else if(uID == VO_SOURCE2_IO_HTTP_SOCKETCONNECTED)
	{
		nParam1 = VOOSMP_SRC_AUTHENTICATION_HTTP_SOCKET_CONNECTED;
	}
	else if(uID == VO_SOURCE2_IO_HTTP_REQUESTPREPARED)
	{
		nParam1 = VOOSMP_SRC_AUTHENTICATION_HTTP_REQUEST_PREPARED;
	}
	else if(uID == VO_SOURCE2_IO_HTTP_REQUESTSEND)
	{
		nParam1 = VOOSMP_SRC_AUTHENTICATION_HTTP_REQUEST_SEND;
	}
	else if(uID == VO_SOURCE2_IO_HTTP_RESPONSERECVED)
	{
		nParam1 = VOOSMP_SRC_AUTHENTICATION_HTTP_RESPONSE_RECVED;
	}
	else if(uID == VO_SOURCE2_IO_HTTP_RESPONSEANALYSED)
	{
		nParam1 = VOOSMP_SRC_AUTHENTICATION_HTTP_RESPONSE_ANALYSED;
	}
	else if(uID == VO_SOURCE2_CB_DRM_INITDATA )
	{
		mID = VOOSMP_SRC_CB_DRM_Init_Data;
		nParam1 = (int)pUserData;
	}
 
#ifdef _LINUX
	CJniEnvUtil	env(m_JavaVM);
    jobject jobj = NULL;

	if(uID == VO_SOURCE2_CB_DRM_INITDATA)
	{
		CJavaParcelWrapOSMP parc(env.getEnv());
		parc.fillParcelData((VOOSMP_SRC_DRM_INIT_DATA_RESPONSE *)pUserData);
		jobj = parc.getParcel();
	}

	if(env.getEnv() == NULL)
		return VOOSMP_ERR_Unknown;

	jclass clazz = env.getEnv()->GetObjectClass(m_jOSSource);
	if (clazz == NULL) {
		return VOOSMP_ERR_Unknown;
	}

	jmethodID RequestCallBack = env.getEnv()->GetMethodID(clazz, "RequestCallBack", "(IIILjava/lang/Object;)I");
	if (RequestCallBack == NULL) {
		return VOOSMP_ERR_Unknown;
	}

	nRC = env.getEnv()->CallIntMethod(m_jOSSource, RequestCallBack, mID, nParam1, nParam2, jobj);


	env.getEnv()->DeleteLocalRef(clazz);
#else
	if(m_lAPOnReqest.pListener)
	{
		nRC = m_lAPOnReqest.pListener(m_lAPOnReqest.pUserData, mID, &nParam1, &nParam2);
	}
#endif

	return nRC;
}

int COSSourceBase::InitDRM()
{
	VOLOGI("here init drm");
	if(m_pLibOP == NULL)
		return VOOSMP_ERR_Implement;
	
	int nRC = VOOSMP_ERR_Implement;

	VOGETDRM2API pGetDRM2API = NULL;
	VOLOGI ("Try to call DRM module: %s.", m_szDRMFile);
	m_hDrmLib = m_pLibOP->LoadLib (m_pLibOP->pUserData, m_szDRMFile, 0);
	if (m_hDrmLib != NULL)
	{
		VOLOGI ("Try to call DRM API Function: %s.", m_szDRMAPI);
		pGetDRM2API = (VOGETDRM2API) m_pLibOP->GetAddress (m_pLibOP->pUserData, m_hDrmLib, m_szDRMAPI, 0);
	}

	if(m_pGetDRM2API)
		pGetDRM2API = m_pGetDRM2API;

	if (pGetDRM2API == NULL)
		return VOOSMP_ERR_Implement;

	pGetDRM2API (&m_fDRMCallBack, 0);

	if(m_fDRMCallBack.Init == NULL)
		return VOOSMP_ERR_Implement;

	m_sDRMOpenParam.nFlag = 0;
	m_sDRMOpenParam.pLibOP = (VO_LIB_OPERATOR *)m_pLibOP;

	nRC = m_fDRMCallBack.Init(&m_hDrmHandle, &m_sDRMOpenParam);
	VOLOGI ("DRM Call Back Fucntion Init: %p.", m_fDRMCallBack.Init);

	if(nRC == VOOSMP_ERR_None)
	{
		m_fDRMCallBack.SetParameter(m_hDrmHandle, VO_PID_COMMON_LOGFUNC, &m_cbVOLOG);
		if (m_fThirdAPI)
			m_fDRMCallBack.SetThirdpartyAPI(m_hDrmHandle, m_fThirdAPI);

		if (m_pAapterDRMObject)
		{
			m_fDRMCallBack.SetParameter(m_hDrmHandle, VO_PID_DRM2_VODRM_COMMON_API,  m_pAapterDRMObject);	
		}
		m_fDRMCallBack.GetInternalAPI(m_hDrmHandle, (VO_PTR *)&m_pDRMCallBack);
		m_fDRMCallBack.SetParameter(m_hDrmHandle,VO_PID_SOURCE2_EVENTCALLBACK, &m_cDRMEventCallBack);
		m_fDRMCallBack.SetParameter(m_hDrmHandle,VO_PID_SOURCE2_HTTPVERIFICATIONCALLBACK, &m_sVerifyCallBack);
		m_fDRMCallBack.SetParameter(m_hDrmHandle,VO_PID_DRM2_PackagePath, m_szPathLib);	

		if(m_sDRMUniqueIden)
		{
			m_fDRMCallBack.SetParameter(m_hDrmHandle, VO_PID_DRM2_UNIQUE_IDENTIFIER, m_sDRMUniqueIden);
		}
		m_bIsDRMInit = VO_TRUE;
	}

	return VOOSMP_ERR_Implement;
}
	
int COSSourceBase::UnintDRM()
{
	if(m_hDrmHandle && m_fDRMCallBack.Uninit)
	{
		m_fDRMCallBack.Uninit(m_hDrmHandle);
		m_hDrmHandle = 0;
	}

	if (m_hDrmLib != NULL)
	{
		m_pLibOP->FreeLib (m_pLibOP->pUserData, m_hDrmLib, 0);
		m_hDrmLib = NULL;
	}		

	memset (&m_fDRMCallBack, 0, sizeof (m_fDRMCallBack));
	memset(&m_sDRMOpenParam, 0, sizeof(VO_DRM_OPENPARAM));

	m_bIsDRMInit = VO_FALSE;
	return VOOSMP_ERR_None;
}

int COSSourceBase::InitSourceIO()
{
	if(m_pLibOP == NULL)
		return VOOSMP_ERR_Implement;
	
	VOGETIOAPI pGetIOAPI = NULL;
	VOLOGI ("Try to call IO module: %s.", m_szIOFile);
	m_hIOLib = m_pLibOP->LoadLib (m_pLibOP->pUserData, m_szIOFile, 0);
	if (m_hIOLib != NULL)
	{
		VOLOGI ("Try to call IO API Function: %s.", m_szIOAPI);
		pGetIOAPI = (VOGETIOAPI) m_pLibOP->GetAddress (m_pLibOP->pUserData, m_hIOLib, m_szIOAPI, 0);
	}

	if (pGetIOAPI == NULL)
		return VOOSMP_ERR_Implement;

	pGetIOAPI (&m_hIOAPI);

	return 0;
}

int COSSourceBase::UnintSourceIO()
{
	if (m_hIOLib != NULL)
	{
		m_pLibOP->FreeLib (m_pLibOP->pUserData, m_hIOLib, 0);
		m_hIOLib = NULL;
	}		

	memset (&m_hIOAPI, 0, sizeof (VO_SOURCE2_IO_API));

	return VOOSMP_ERR_None;
}

int COSSourceBase::GetSubLangNum(int *pNum)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_pBaseSource)
		nRC = m_pBaseSource->GetSubLangNum(pNum);

	return nRC;
}

int COSSourceBase::GetSubLangItem(int Index, VOOSMP_SUBTITLE_LANGUAGE** pSubLangItem)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_pBaseSource)
		nRC = m_pBaseSource->GetSubLangItem(Index, pSubLangItem);

	return nRC;
}

int COSSourceBase::GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** pSubLangInfo)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_pBaseSource)
		nRC = m_pBaseSource->GetSubLangInfo(pSubLangInfo);

	return nRC;
}

int COSSourceBase::SelectLanguage(int Index)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_pBaseSource)
		nRC = m_pBaseSource->SelectLanguage(Index);

	return nRC;
}

int COSSourceBase::GetMediaCount(int ssType)
{
	voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(m_pBaseSource)
		return m_pBaseSource->GetMediaCount(ssType);
	else
		return VOOSMP_ERR_Implement;
}

int COSSourceBase::SelectMediaIndex(int ssType, int nIndex)
{
	voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(m_pBaseSource)
		return m_pBaseSource->SelectMediaIndex(ssType, nIndex);
	else
		return VOOSMP_ERR_Implement;
}
	
int COSSourceBase::IsMediaIndexAvailable(int ssType, int nIndex)
{
	voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(m_pBaseSource)
		return m_pBaseSource->IsMediaIndexAvailable(ssType, nIndex);
	else
		return VOOSMP_ERR_Implement;
}

int COSSourceBase::GetMediaProperty(int ssType, int nIndex, VOOSMP_SRC_TRACK_PROPERTY **pProperty)
{
	voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(m_pBaseSource)
		return m_pBaseSource->GetMediaProperty(ssType, nIndex, pProperty);
	else
		return VOOSMP_ERR_Implement;
}

int COSSourceBase::CommetSelection(int bSelect)
{
	voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(m_pBaseSource)
		return m_pBaseSource->CommetSelection(bSelect);
	else
		return VOOSMP_ERR_Implement;
}

int COSSourceBase::GetCurMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(m_pBaseSource)
		return m_pBaseSource->GetCurMediaTrack(pCurIndex);
	else
		return VOOSMP_ERR_Implement;
}

int COSSourceBase::GetCurPlayingMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(m_pBaseSource)
		return m_pBaseSource->GetCurPlayingMediaTrack(pCurIndex);
	else
		return VOOSMP_ERR_Implement;
}

int COSSourceBase::GetCurSelectedMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	voCAutoLock lockVideoSrc (&m_MutexStatus);

	if(m_pBaseSource)
		return m_pBaseSource->GetCurSelectedMediaTrack(pCurIndex);
	else
		return VOOSMP_ERR_Implement;
}

int COSSourceBase::ClearPropertyItem(VOOSMP_SRC_TRACK_PROPERTY *pItemProperty)
{
	if(pItemProperty == NULL)
		return VOOSMP_ERR_None;

	if(pItemProperty->nPropertyCount > 0 && pItemProperty->ppItemProperties)
	{
		int n;

		for(n = 0; n < pItemProperty->nPropertyCount; n++)
		{
			if(pItemProperty->ppItemProperties[n])
			{
				if(pItemProperty->ppItemProperties[n]->pszProperty)
				{
					delete []pItemProperty->ppItemProperties[n]->pszProperty;
					pItemProperty->ppItemProperties[n]->pszProperty = NULL;
				}

				delete []pItemProperty->ppItemProperties[n];

				pItemProperty->ppItemProperties[n] = NULL;
			}
		}

		delete []pItemProperty->ppItemProperties;
		pItemProperty->ppItemProperties = NULL;
	}

	memset(pItemProperty, 0, sizeof(VOOSMP_SRC_TRACK_PROPERTY));

	return 0;
}
