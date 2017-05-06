	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseGraph.cpp

	Contains:	voCOMXBaseGraph class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include "voOMX_Index.h"
#include "voOMXOSFun.h"
#include "voOMXBase.h"
#include "voFile.h"
#include "voCOMXBaseGraph.h"

#define LOG_TAG "voCOMXBaseGraph"
#include "voLog.h"
 
#define VOME_CONFIG_CORE_FILE _T("vomeCore.cfg")
#define VOME_CONFIG_COMP_FILE _T("vomeComp.cfg")

#define RTLD_FLAGS (RTLD_NOW | RTLD_LOCAL)
#undef MODULE_NAME
#define MODULE_NAME "voOMXME"
#define BUILD_NO 1120
#include "voVersion.c"

OMX_PTR voCOMXBaseGraph::OMX_mmLoadLib (OMX_PTR	pUserData, OMX_STRING pLibName, OMX_S32 nFlag)
{
//	VOLOGI ("lIb Name is %s", pLibName);
	voCOMXBaseGraph *	pGraph = (voCOMXBaseGraph *)pUserData;
	OMX_STRING			pWorkPath = pGraph->GetWorkPath ();;
	OMX_PTR				hDll = NULL;

#ifdef _WIN32
	VO_TCHAR	szDll[MAX_PATH];
	VO_TCHAR	szPath[MAX_PATH];
	TCHAR		szLibName[256];

	memset (szLibName, 0, 256);
	::MultiByteToWideChar (CP_ACP, 0, pLibName, -1, szLibName, sizeof (szLibName));

	if (pWorkPath != NULL)
	{
		vostrcpy (szDll, (VO_TCHAR *)pWorkPath);
		vostrcat (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	if (hDll == NULL)
	{
		GetModuleFileName ((HMODULE)pGraph->m_hInstance, szPath, sizeof (szPath));

		VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;

			vostrcpy (szDll, szPath);
			vostrcat (szDll, szLibName);
			hDll = LoadLibrary (szDll);
		}
	}

	if (hDll == NULL)
	{
		::GetModuleFileName (NULL, szPath, sizeof (szPath));
		VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;

			vostrcpy (szDll, szPath);
			vostrcat (szDll, szLibName);
			hDll = LoadLibrary (szDll);
		}
	}

	if (hDll == NULL)
	{
		vostrcpy (szDll, szPath);
		vostrcat (szDll, _T("Dll\\"));
		vostrcat (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	if (hDll == NULL)
	{
		vostrcpy (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	VOLOGI ("%s", szDll);

#elif defined _LINUX_X86
	VO_TCHAR libname[256];
	VO_TCHAR szPath[256];

	vostrcpy(libname, pLibName);
	voOS_GetAppFolder(szPath, sizeof(szPath));
	vostrcat(szPath, libname);
	hDll = dlopen (szPath, RTLD_FLAGS);
	if (hDll == NULL)
	{
		voOS_GetAppFolder(szPath, sizeof(szPath));
		vostrcat(szPath, "lib/");
		vostrcat(szPath, libname);
		hDll = dlopen (szPath, RTLD_FLAGS);
	}

#elif defined _LINUX_ANDROID
	VO_TCHAR libname[256];
	VO_TCHAR szLibPath[256];

	if (strncmp (pLibName, "lib", 3))
		vostrcpy (libname, pLibName);
	else
		vostrcpy(libname, pLibName + 3);

//	FILE * hFile = fopen ("/data/local/voOMXPlayer/lib/debugvolib.txt", "rb");
//	if (hFile != NULL)
	int nRC = access("/data/local/voOMXPlayer/lib/debugvolib.txt", F_OK);
	if (nRC == 0)
	{
//		fclose (hFile);
		vostrcpy(szLibPath, "/data/local/voOMXPlayer/lib/lib");
		vostrcat(szLibPath, libname);
		hDll = dlopen (szLibPath, RTLD_FLAGS);
		if (hDll == NULL)
		{
			vostrcpy(szLibPath, "/data/local/voOMXPlayer/lib/");
			vostrcat(szLibPath, libname);
			hDll = dlopen (szLibPath, RTLD_FLAGS);
		}
	}

	if (hDll == NULL)
	{
		if (pWorkPath != NULL)
		{
			vostrcpy (szLibPath, pWorkPath);
			vostrcat(szLibPath, "/lib/lib");
			vostrcat (szLibPath, libname);
			hDll = dlopen (szLibPath, RTLD_FLAGS);

			if (hDll == NULL)
			{
				vostrcpy (szLibPath, pWorkPath);
				vostrcat(szLibPath, "/lib/");
				vostrcat (szLibPath, libname);
				hDll = dlopen (szLibPath, RTLD_FLAGS);
			}

			if (hDll == NULL && strncmp (libname, "libvo", 5))
			{
				vostrcpy (szLibPath, libname);
				hDll = dlopen (szLibPath, RTLD_FLAGS);
			}
		}
		else
		{
			vostrcpy(szLibPath, "lib");
			vostrcat(szLibPath, libname);
			hDll = dlopen (szLibPath, RTLD_FLAGS);
			if (hDll == NULL)
			{
				vostrcpy (szLibPath, libname);
				hDll = dlopen (szLibPath, RTLD_FLAGS);
			}
		}
	}

	dlerror ();

	if (hDll != NULL)
	{
		VOLOGI ("Load %s ... %p", szLibPath, hDll);
        voShowModuleVersion(hDll);
	}
	else
	{
		VOLOGE ("Load %s failed!", pLibName);
	}

#endif // _WIN32

	return hDll;
}

OMX_PTR voCOMXBaseGraph::OMX_mmGetAddress (OMX_PTR	pUserData, OMX_PTR hLib, OMX_STRING pFuncName, OMX_S32 nFlag)
{
	if (hLib == NULL)
		return NULL;

//	VOLOGI ("%s", pFuncName);

	OMX_PTR pFunc = NULL;

#ifdef _LINUX
	pFunc = dlsym (hLib, pFuncName);
#endif // _LINUX

#ifdef _WIN32
#ifdef _WIN32_WCE
	TCHAR szFuncName[128];
	memset (szFuncName, 0, 256);
	::MultiByteToWideChar (CP_ACP, 0, pFuncName, -1, szFuncName, sizeof (szFuncName));

	pFunc = GetProcAddress ((HMODULE)hLib, szFuncName);
#else
	pFunc = GetProcAddress ((HMODULE)hLib, pFuncName);
#endif //_WIN32_WCE
#endif // _WIN32

	return pFunc;
}

OMX_S32 voCOMXBaseGraph::OMX_mmFreeLib (OMX_PTR	pUserData, OMX_PTR hLib, OMX_S32 nFlag)
{
	if (hLib == NULL)
		return -1;

    VOLOGI("Free library %p", hLib);

#ifdef _LINUX
	dlclose (hLib);
#endif // _LINUX

#ifdef _WIN32
	FreeLibrary ((HMODULE)hLib);
#endif // _WIN32

	return 0;
}

voCOMXBaseGraph::voCOMXBaseGraph()
	: m_pConfig (NULL)
	, m_sStatus (VOME_STATUS_Unknown)
	, m_pChain (NULL)
	, m_bInit (OMX_FALSE)
	, m_pWorkPath (NULL)
	, m_pCompCfgFile (NULL)
	, m_pLicenseText (NULL)
	, m_pExtLibOP (NULL)
	, m_nSeekTime (0)
	, m_nSeekPos (0)
	, m_nStartRunTime (0)
	, m_bTunnelMode (OMX_TRUE)
	, m_nPlayFlag (3)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
	, m_nVideoRotation (0)
#ifdef _VO_NDK_CheckLicense
	, m_hVidDec (NULL)
	, m_hLcsCheck (NULL)
	, m_fUninitCheck (NULL)
	, m_fCheckImage (NULL)
#endif // _VO_NDK_CheckLicense
	, m_pCallBack (NULL)
	, m_pUserData (NULL)
	, m_pFilePipe (NULL)
	, m_hMsgThread (NULL)
	, m_nMsgThreadID (0)
	, m_bMsgThreadStop (OMX_TRUE)
{
    voShowModuleVersion(NULL);
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);

	m_cbVideoRender.pUserData = NULL;
	m_cbVideoRender.pCallBack = NULL;

	m_cbAudioRender.pUserData = NULL;
	m_cbAudioRender.pCallBack = NULL;

	memset (&m_dispArea, 0, sizeof (OMX_VO_DISPLAYAREATYPE));

	m_libOP.pUserData = this;
	m_libOP.LoadLib = OMX_mmLoadLib;
	m_libOP.GetAddress = OMX_mmGetAddress;
	m_libOP.FreeLib = OMX_mmFreeLib;
//	m_pExtLibOP = &m_libOP;

	FILE * hFile = fopen("/proc/self/cmdline", "rb");
	if (hFile != NULL)
	{
		char szPackageName[992];
		memset(szPackageName, 0, sizeof(szPackageName));
		fgets(szPackageName, 992, hFile);
		if (strstr (szPackageName, "com.") != NULL)
		{
			m_pWorkPath = new char[1024];
			sprintf(m_pWorkPath, "/data/data/%s/", szPackageName);
			VOLOGI ("Package name is %s", m_pWorkPath);
		}
		fclose(hFile);
	}

	m_bMsgThreadStop = OMX_FALSE;
	voOMXThreadCreate (&m_hMsgThread, &m_nMsgThreadID, (voOMX_ThreadProc)voGraphMessageHandleProc, this, 0);
}

voCOMXBaseGraph::~voCOMXBaseGraph()
{
	Close ();

	voCOMXAutoLock lock (&m_tmState);

#ifdef _VO_NDK_CheckLicense
	if (m_hVidDec != NULL)
	{
		if (m_hLcsCheck != NULL && m_fUninitCheck != NULL)
			m_fUninitCheck (m_hLcsCheck);

		if (m_pExtLibOP != NULL)
			m_pExtLibOP->FreeLib (m_pExtLibOP->pUserData, m_hVidDec, 0);
		else
			m_libOP.FreeLib (m_libOP.pUserData, m_hVidDec, 0);
		m_hVidDec = NULL;
	}
#endif // _VO_NDK_CheckLicense

	if (m_pChain != NULL)
	{
		delete m_pChain;
		m_pChain = NULL;
	}

	m_bMsgThreadStop = OMX_TRUE;
	if (m_tsMessage.Count () == 0)
		m_tsMessage.Up ();

	OMX_U32 nTryNum = 0;
	while (m_hMsgThread != NULL && nTryNum < 500)
	{
		voOMXOS_Sleep (1);
		nTryNum++;
	}

	if (m_pConfig != NULL)
		delete m_pConfig;

	if (m_pWorkPath != NULL)
		delete []m_pWorkPath;

	if (m_pCompCfgFile != NULL)
		delete []m_pCompCfgFile;

	if (m_pLicenseText != NULL)
		delete []m_pLicenseText;
}

OMX_ERRORTYPE	voCOMXBaseGraph::Init (OMX_PTR hInst)
{
	voCOMXAutoLock lock (&m_tmState);
	m_bInit = OMX_TRUE;
	m_hInstance = hInst;

	if (m_pWorkPath != NULL)
	{
#ifdef _WIN32
		TCHAR szCfgFile[256];
		_tcscpy (szCfgFile, (TCHAR *)m_pWorkPath);
		_tcscat (szCfgFile, VOME_CONFIG_CORE_FILE);
		if (!m_cfgCore.Open ((OMX_STRING)szCfgFile))
		{
			VOLOGW ("Open vomeCore.cfg file error!");
			// return OMX_ErrorUndefined;
		}

		_tcscpy (szCfgFile, (TCHAR *)m_pWorkPath);
		if (m_pCompCfgFile != NULL)
			_tcscat (szCfgFile, (TCHAR *)m_pCompCfgFile);
		else
			_tcscat (szCfgFile, VOME_CONFIG_COMP_FILE);
		if (!m_cfgComp.Open ((OMX_STRING)szCfgFile))
		{
			VOLOGW ("Open vomeComp.cfg file error!");
			//return OMX_ErrorUndefined;
		}
#else
		char szCfgFile[256];
		strcpy (szCfgFile, (TCHAR *)m_pWorkPath);
		strcat (szCfgFile, VOME_CONFIG_CORE_FILE);
		if (!m_cfgCore.Open ((OMX_STRING)szCfgFile))
		{
			VOLOGW ("Open Core Config file %s was failed!", szCfgFile);
			//return OMX_ErrorUndefined;
		}

		OMX_BOOL bCfgOpen = OMX_FALSE;
		strcpy (szCfgFile, (TCHAR *)m_pWorkPath);
		if (m_pCompCfgFile != NULL)
		{
			strcat (szCfgFile, m_pCompCfgFile);
			bCfgOpen = m_cfgComp.Open ((OMX_STRING)szCfgFile);
		}

		if (!bCfgOpen)
		{
			strcpy (szCfgFile, (TCHAR *)m_pWorkPath);
			strcat (szCfgFile, VOME_CONFIG_COMP_FILE);
			if (!m_cfgComp.Open ((OMX_STRING)szCfgFile))
			{
				VOLOGW ("Open Component Config file %s was failed!", szCfgFile);
				//return OMX_ErrorUndefined;
			}
		}

		m_pConfig = new voCOMXBaseConfig ();
		if (m_pConfig != NULL)
		{
			strcpy (szCfgFile, (TCHAR *)m_pWorkPath);
			strcat (szCfgFile, "vomeplay.cfg");

			m_pConfig->Open (szCfgFile);
			m_bTunnelMode = (OMX_BOOL)m_pConfig->GetItemValue ((OMX_STRING)"vomeMF", (OMX_STRING)"TunnelMode", OMX_TRUE);
			m_nPlayFlag = m_pConfig->GetItemValue ((OMX_STRING)"vomeMF", (OMX_STRING)"PlayFlag", 3);
		}
#endif // _WIN32
	}
	else
	{
		m_pConfig = new voCOMXBaseConfig ();
		if (m_pConfig != NULL)
		{
			m_pConfig->Open ((OMX_STRING)_T("vomeplay.cfg"));
			m_bTunnelMode = (OMX_BOOL)m_pConfig->GetItemValue ((OMX_STRING)"vomeMF", (OMX_STRING)"TunnelMode", OMX_TRUE);
			m_nPlayFlag = m_pConfig->GetItemValue ((OMX_STRING)"vomeMF", (OMX_STRING)"PlayFlag", 3);
		}

		if (!m_cfgCore.Open ((OMX_STRING)VOME_CONFIG_CORE_FILE))
		{
			VOLOGW ("Open vomeCore.cfg file error!");
			//return OMX_ErrorUndefined;
		}

		OMX_BOOL bCfgOpen = OMX_FALSE;
		if (m_pCompCfgFile != NULL)
			bCfgOpen = m_cfgComp.Open (m_pCompCfgFile);
		if (!bCfgOpen)
			m_cfgComp.Open ((OMX_STRING)VOME_CONFIG_COMP_FILE);
	}
	m_cmpList.SetLibOperator (m_pExtLibOP);
	if (m_cmpList.Init (&m_cfgCore, m_pWorkPath) != OMX_ErrorNone)
	{
		VOLOGE ("m_cmpList.Init error!");
		return OMX_ErrorUndefined;
	}

	if (m_pChain != NULL)
		delete m_pChain;
	m_pChain = new voCOMXCompBaseChain (this, &m_cmpList, &m_cfgCore, &m_cfgComp);
	if (m_pChain == NULL)
	{
		VOLOGE ("Create voCOMXCompBaseChain error!");
		return OMX_ErrorInsufficientResources;
	}

	m_pChain->SetCallBack (m_pCallBack, m_pUserData);
	m_pChain->m_pWorkPath = m_pWorkPath;

	return OMX_ErrorNone;
}

void voCOMXBaseGraph::SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;

	m_cmpList.SetCallBack (pCallBack, pUserData);
}

OMX_ERRORTYPE voCOMXBaseGraph::SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay)
{
	voCOMXAutoLock lock (&m_tmState);

	voOMXMemCopy (&m_dispArea,  pDisplay, sizeof (OMX_VO_DISPLAYAREATYPE));

	if (m_pChain != NULL && m_pChain->m_pVideoSink != NULL)
	{
		m_pChain->m_pVideoSink->SetConfig ((OMX_INDEXTYPE)OMX_VO_IndexConfigDisplayArea, &m_dispArea);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE  voCOMXBaseGraph::GetParam (OMX_S32 nID, OMX_PTR pValue)
{
	voCOMXAutoLock lock (&m_tmState);

	OMX_ERRORTYPE errType = OMX_ErrorBadParameter;

	if (nID == VOME_PID_AudioFormat)
	{
		if (m_pChain != NULL && m_pChain->m_pAudioSink != NULL)
		{
			OMX_AUDIO_PARAM_PCMMODETYPE tpPCMFormat;
			m_pChain->m_pAudioSink->SetHeadder (&tpPCMFormat, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
			tpPCMFormat.nPortIndex = 0;

			errType = m_pChain->m_pAudioSink->GetParameter (OMX_IndexParamAudioPcm, &tpPCMFormat);
			if (errType == OMX_ErrorNone)
			{
				VOME_AUDIOFORMATTYPE * pAudioFormat = (VOME_AUDIOFORMATTYPE *)pValue;
				pAudioFormat->nSampleRate = tpPCMFormat.nSamplingRate;
				pAudioFormat->nChannels = tpPCMFormat.nChannels;
				pAudioFormat->nSampleBits = tpPCMFormat.nBitPerSample;

				return OMX_ErrorNone;
			}
		}
	}
	else if (nID == VOME_PID_VideoSize)
	{
		if (m_pChain != NULL && m_pChain->m_pVideoSink != NULL)
		{
			VOME_VIDEOSIZETYPE * pVideoSize = (VOME_VIDEOSIZETYPE *)pValue;

			pVideoSize->nWidth = m_nVideoWidth;
			pVideoSize->nHeight = m_nVideoHeight;

			return OMX_ErrorNone;
		}
	}
	else if (nID == VOME_PID_VideoRotation)
	{
		if (m_pChain != NULL && m_pChain->m_pVideoSink != NULL)
		{
			*((OMX_S32*)pValue) = m_nVideoRotation;
			return OMX_ErrorNone;
		}
	}
	else if (nID == VOME_PID_GetTrackCount)
	{
		VOLOGI("Ferry VOME_PID_GetTrackCount");
		if(m_pChain != NULL && m_pChain->m_pSource != NULL)
		{
			m_pChain->m_pSource->GetParameter((OMX_INDEXTYPE)OMX_VO_IndexTrackCount, pValue);
		}
		return OMX_ErrorNone;
	}
	else if (nID == VOME_PID_GetTrackInfo)
	{
		VOLOGI("Ferry VOME_PID_GetTrackInfo");
		if(m_pChain != NULL && m_pChain->m_pSource != NULL)
		{
			m_pChain->m_pSource->GetParameter((OMX_INDEXTYPE)OMX_VO_IndexTrackInfo, pValue);
		}
		return OMX_ErrorNone;
	}

	return errType;
}

OMX_ERRORTYPE  voCOMXBaseGraph::SetParam (OMX_S32 nID, OMX_PTR pValue)
{
	if (nID == VOME_PID_WorkingPath)
	{	
		if (m_pWorkPath != NULL)
			delete []m_pWorkPath;
		int nLen = 0;
#ifdef _WIN32
		nLen = (_tcslen ((TCHAR *)pValue) + 4) * sizeof (TCHAR);
		m_pWorkPath = (OMX_STRING) new unsigned char[nLen];
		_tcscpy ((TCHAR *)m_pWorkPath, (TCHAR *)pValue);
		TCHAR * pWorkPath = (TCHAR *) m_pWorkPath;
		if (pWorkPath[_tcslen (pWorkPath) - 1] != _T('/') && pWorkPath[_tcslen (pWorkPath) - 1] != _T('\\'))
			_tcscat (pWorkPath, _T("/"));
#else
		nLen = (strlen ((char *)pValue) + 4);
		m_pWorkPath = (OMX_STRING) new unsigned char[nLen];
		strcpy ((char *)m_pWorkPath, (char *)pValue);
		if (m_pWorkPath[strlen (m_pWorkPath) - 1] != _T('/'))
			vostrcat (m_pWorkPath, _T("/"));
#endif // _WIN32

		return OMX_ErrorNone;
	}
	else if (nID == VOME_PID_CompConfigFile)
	{
		if (m_pCompCfgFile != NULL)
			delete []m_pCompCfgFile;
		int nLen = 0;
#ifdef _WIN32
		nLen = (_tcslen ((TCHAR *)pValue) + 4) * sizeof (TCHAR);
		m_pCompCfgFile = (OMX_STRING) new unsigned char[nLen];
		_tcscpy ((TCHAR *)m_pCompCfgFile, (TCHAR *)pValue);
#else
		nLen = (strlen ((char *)pValue) + 4);
		m_pCompCfgFile = (OMX_STRING) new unsigned char[nLen];
		strcpy ((char *)m_pCompCfgFile, (char *)pValue);
#endif // _WIN32

		return OMX_ErrorNone;
	}

	if (!m_bInit)
		Init (NULL);

	voCOMXAutoLock lock (&m_tmState);
	if (m_pChain == NULL)
		return OMX_ErrorResourcesLost;

	if (nID == VOME_PID_VideoRenderBuffer)
	{
		OMX_VO_CHECKRENDERBUFFERTYPE * pRender = (OMX_VO_CHECKRENDERBUFFERTYPE *) pValue;
		m_cbVideoRender.pCallBack = pRender->pCallBack;
		m_cbVideoRender.pUserData = pRender->pUserData;

		if (m_pChain->m_pVideoSink != NULL)
			m_pChain->m_pVideoSink->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexRenderBufferCallBack, &m_cbVideoRender);

		return OMX_ErrorNone;
	}
	else if (nID == VOME_PID_AudioRenderBuffer)
	{
		OMX_VO_CHECKRENDERBUFFERTYPE * pRender = (OMX_VO_CHECKRENDERBUFFERTYPE *) pValue;
		m_cbAudioRender.pCallBack = pRender->pCallBack;
		m_cbAudioRender.pUserData = pRender->pUserData;

		if (m_pChain->m_pAudioSink != NULL)
			m_pChain->m_pAudioSink->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexRenderBufferCallBack, &m_cbAudioRender);

		return OMX_ErrorNone;
	}
	else if (nID == VOME_PID_VideoDataBuffer)
	{
		if (m_pChain->m_pVideoSink != NULL)
		{
			m_pChain->m_pVideoSink->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexVideoDataBuffer, pValue);
		}

		return OMX_ErrorNone;
	}
	else if (nID == VOME_PID_VideoMemOperator)
	{
		if (m_pChain->m_pVideoDec != NULL)
		{
			m_pChain->m_pVideoDec->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexVideoMemOperator, pValue);
		}

		return OMX_ErrorNone;
	}
	else if (nID == VOME_PID_Flush)
	{
		return m_pChain->FlushAll ();
	}
	else if (nID == OMX_VO_IndexSourceDrm)
	{
		m_pChain->m_pDrmCB = (OMX_VO_SOURCEDRM_CALLBACK *)pValue;

		return OMX_ErrorNone;
	}
	else if (nID == OMX_VO_IndexSetTranscodeMode)
	{
		for (OMX_U32 i = 0; i < VOMAX_BOX_NUM;  i++)
		{
			if (m_pChain->m_aBox[i] != NULL)
			{
				m_pChain->m_aBox[i]->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexSetTranscodeMode, pValue);
			}
		}

		return OMX_ErrorNone;
	}
	else if (nID == 0X7F000422)
	{
		if (m_pLicenseText != NULL)
			delete []m_pLicenseText;
		m_pLicenseText = (OMX_STRING) new char[256];
		strcpy ((char *)m_pLicenseText, (char *)pValue);
#ifdef _VO_NDK_CheckLicense
		if (m_hLcsCheck != NULL)
		{
			VO_VIDEO_BUFFER	bufImg;

			bufImg.ColorType = (VO_IV_COLORTYPE)20050309;
			bufImg.Buffer[0] = (VO_PBYTE) m_pLicenseText;

			m_fCheckImage (m_hLcsCheck, &bufImg, NULL);
		}
#endif // _VO_NDK_CheckLicense
		return OMX_ErrorNone;
	}
	else if (nID == OMX_VO_IndexLibraryOperator)
	{
		m_pExtLibOP = (OMX_VO_LIB_OPERATOR *)pValue;
		if (m_pChain != NULL)
			m_pChain->m_pLibOP = m_pExtLibOP;

		return OMX_ErrorNone;
	}
	else if (nID == VOME_PID_ChangeSurface)
	{
		voCOMXAutoLock lock (&m_tmState);

		VOME_UPDATESURFACECALLBACK * pCallBack = (VOME_UPDATESURFACECALLBACK *)pValue;
		return m_pChain->ChangeSurface(pCallBack->fCallBack, pCallBack->pUserData);
	}
	else if(nID == VOME_PID_SelectTrack)
	{
		VOME_TRACKINFO * trackInfo = (VOME_TRACKINFO *)pValue;
		return m_pChain->SelectTrack(trackInfo->nIndex,trackInfo->nType);
	}

	return OMX_ErrorBadParameter;
}

OMX_ERRORTYPE  voCOMXBaseGraph::Playback (VOME_SOURCECONTENTTYPE * pSource)
{
	if (!m_bInit)
		Init (NULL);
	if (m_pChain == NULL)
		return OMX_ErrorResourcesLost;

	InitLicenseCheck ();

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType = Stop ();
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("The result is 0X%08X from Stop!", errType);
		return errType;
	}

	voCOMXAutoLock lock (&m_tmState);
	errType = m_pChain->ReleaseBoxes ();
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("The result is 0X%08X from m_pChain->ReleaseBoxes!", errType);
		return errType;
	}

	voCOMXBaseBox * pSourceBox = m_pChain->AddSource (pSource);
	if (pSourceBox == NULL)
	{
		VOLOGE ("m_pChain->AddSource!");
		return OMX_ErrorFormatNotDetected;
	}

	if (0x10 == (pSource->nFlag & 0x10))
		return OMX_ErrorNone;

	if ((pSource->nFlag & 0xF) == 1)
		m_nPlayFlag = 1;
	if ((pSource->nFlag & 0xF) == 2)
		m_nPlayFlag = 2;

	OMX_U32			uClockPort = 0;
	voCOMXBaseBox * pClockBox = m_pChain->AddClock ();
	if (pClockBox == NULL)
	{
		VOLOGE ("m_pChain->AddClock!");
		return OMX_ErrorUndefined;
	}
	pClockBox->CreateHoles ();

	OMX_PARAM_PORTDEFINITIONTYPE *	pPortType = NULL;

	voCOMXBaseHole *				pHole = NULL;
	voCOMXBaseHole *				pOutputHole = NULL;
	voCOMXBaseHole *				pInputHole = NULL;

	voCOMXBaseBox *					pAudioDec = NULL;
	voCOMXBaseBox *					pVideoDec = NULL;

	OMX_STRING						pCompName = NULL;

	OMX_BOOL						bIsPCM = OMX_FALSE;

	OMX_S32							nPlayMode = 0;
	if (m_pConfig != NULL)
		nPlayMode = m_pConfig->GetItemValue ((OMX_STRING)"vomeMF", (OMX_STRING)"PlayMode", 0);

	for (OMX_U32 i = 0; i < pSourceBox->GetHoleCount (); i++)
	{
		pHole = pSourceBox->GetHole (i);
		pPortType = pHole->GetPortType ();

		if (pPortType->eDomain == OMX_PortDomainOther && pPortType->eDir == OMX_DirInput)
		{
			pOutputHole = pClockBox->GetHole (uClockPort);
			uClockPort++;

			pInputHole = pSourceBox->GetHole (i);
			errType = m_pChain->Connect (pOutputHole, pInputHole, OMX_TRUE);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("The result is 0X%08X from m_pChain->Connect!", errType);
				return errType;
			}
		}
		else if (pPortType->eDomain == OMX_PortDomainAudio && pPortType->eDir == OMX_DirOutput && (m_nPlayFlag & 0X02))
		{
			pOutputHole = pSourceBox->GetHole (i);

			OMX_AUDIO_PARAM_PORTFORMATTYPE tpAudioFormat;
			pSourceBox->SetHeadder (&tpAudioFormat, sizeof (OMX_AUDIO_PARAM_PORTFORMATTYPE));
			tpAudioFormat.nPortIndex = pOutputHole->GetPortIndex ();

			errType = pSourceBox->GetParameter (OMX_IndexParamAudioPortFormat, &tpAudioFormat);
			if (errType != OMX_ErrorNone)
			{
				if (nPlayMode > 0)
					continue;

				VOLOGE ("The result is 0X%08X from pSourceBox->GetParameter OMX_IndexParamAudioPortFormat !", errType);
				return errType;
			}

			if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingPCM)
			{
				bIsPCM = OMX_TRUE;
				pOutputHole = pSourceBox->GetHole (i);
			}
			else
			{
				OMX_U32 nTryTimes = 0;
				OMX_BOOL bExit = OMX_FALSE;
				while (nTryTimes < 5)
				{
					pAudioDec = TryConnectAudioDec (pOutputHole, nTryTimes, bExit);
					if (pAudioDec != NULL || bExit)
						break;
					nTryTimes++;
				}

				if (pAudioDec == NULL)
				{
					if (nPlayMode > 0)
						continue;

					VOLOGE ("Connect the audio decoder component!");
					NotifyCallbackMsg (VOME_CID_PLAY_UNSUPPORTEDCODEC, 0, 0);
					return OMX_ErrorComponentNotFound;
				}

				pOutputHole = pAudioDec->GetHole (1);
			}

			m_pChain->m_pAudioDec = pAudioDec;

			pOutputHole = AddEffectComp (pOutputHole, OMX_TRUE);
			if (pOutputHole == NULL)
			{
				VOLOGE ("It could not add the audio effect component(s)!");
				return OMX_ErrorComponentNotFound;
			}

			pCompName = m_cfgCore.GetAudioSinkCompName (1);
			if (pCompName == NULL)
			{
				VOLOGE ("m_cfgCore.GetAudioSinkCompName!");
				return OMX_ErrorComponentNotFound;
			}

			voCOMXBaseBox * pAudioRender = m_pChain->AddBox (pCompName);
			if (pAudioRender == NULL)
			{
				VOLOGE ("m_pChain->AddBox. The Name is %s!", pCompName);
				return OMX_ErrorComponentNotFound;
			}
			pAudioRender->CreateHoles ();

			for (OMX_U32 j = 0; j < pAudioRender->GetHoleCount (); j++)
			{
				pInputHole = pAudioRender->GetHole (j);
				pPortType = pInputHole->GetPortType ();

				if (pPortType->eDomain  == OMX_PortDomainOther)
				{
					voCOMXBaseHole * pClockHole = pClockBox->GetHole (uClockPort);
					uClockPort++;

					errType = m_pChain->Connect (pClockHole, pInputHole, OMX_TRUE);
					if (errType != OMX_ErrorNone)
					{
						VOLOGE ("m_pChain->Connect (Audio Render to Clock Component). The result is 0X%08X.", errType);
						return OMX_ErrorUndefined;
					}
				}
				else
				{
					if (!strncmp (pOutputHole->GetParentBox ()->GetComponentName (), "OMX_PV", 6))
						errType = m_pChain->Connect (pOutputHole, pInputHole, OMX_FALSE);
					else
						errType = m_pChain->Connect (pOutputHole, pInputHole, m_bTunnelMode);
						// errType = m_pChain->Connect (pOutputHole, pInputHole, OMX_FALSE);

					if (errType != OMX_ErrorNone)
					{
						VOLOGE ("m_pChain->Connect (Audio Decoder to Render Component). The result is 0X%08X.", errType);
						return OMX_ErrorUndefined;
					}
				}
			}

			m_pChain->m_bEndOfAudio = OMX_FALSE;
			m_pChain->m_pAudioSink = pAudioRender;
			if (m_cbAudioRender.pCallBack != NULL)
				m_pChain->m_pAudioSink->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexRenderBufferCallBack, &m_cbAudioRender);
		}
		else if (pPortType->eDomain == OMX_PortDomainVideo && pPortType->eDir == OMX_DirOutput && (m_nPlayFlag & 0X01))
		{
			m_nVideoWidth = pPortType->format.video.nFrameWidth;
			m_nVideoHeight = pPortType->format.video.nFrameHeight;

			errType = pSourceBox->GetParameter((OMX_INDEXTYPE)OMX_VO_IndexVideoRotation, &m_nVideoRotation);

			OMX_U32 nTryTimes = 0;
			OMX_BOOL bExit = OMX_FALSE;
			pOutputHole = pSourceBox->GetHole (i);
			while (nTryTimes < 5)
			{
				pVideoDec = TryConnectVideoDec (pOutputHole, nTryTimes, bExit);
				if (pVideoDec != NULL || bExit)
					break;
				nTryTimes++;
			}
			if (pVideoDec == NULL)
			{
				if (nPlayMode > 0)
					continue;

				VOLOGE ("Try to connect the video decoer component was failed!");
				NotifyCallbackMsg (VOME_CID_PLAY_UNSUPPORTEDCODEC, 0, 0);
				return OMX_ErrorComponentNotFound;
			}
			m_pChain->m_pVideoDec = pVideoDec;
			pVideoDec->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexSourceDrm, m_pChain->m_pDrmCB);
			pOutputHole = pVideoDec->GetHole (1);
			pOutputHole = AddEffectComp (pOutputHole, OMX_FALSE);
			if (pOutputHole == NULL)
			{
				VOLOGE ("It could not add the audio effect component(s)!");
				return OMX_ErrorComponentNotFound;
			}

			pCompName = m_cfgCore.GetVideoSinkCompName (1);
			if (pCompName == NULL)
			{
				VOLOGE ("It could not find the video render component!");
				return OMX_ErrorComponentNotFound;
			}

			voCOMXBaseBox * pVideoRender = m_pChain->AddBox (pCompName);
			if (pVideoRender == NULL)
			{
				VOLOGE ("It could not add the video render component!");
				return OMX_ErrorComponentNotFound;
			}
			pVideoRender->CreateHoles ();
			for (OMX_U32 j = 0; j < pVideoRender->GetHoleCount (); j++)
			{
				pInputHole = pVideoRender->GetHole (j);
				pPortType = pInputHole->GetPortType ();

				if (pPortType->eDomain  == OMX_PortDomainOther)
				{
					pOutputHole = pClockBox->GetHole (uClockPort);
					uClockPort++;

					errType = m_pChain->Connect (pOutputHole, pInputHole, OMX_TRUE);
					if (errType != OMX_ErrorNone)
					{
						VOLOGE ("It could not connect the clock with video render component!");
						return OMX_ErrorUndefined;
					}
				}
				else
				{
					if (!strncmp (pOutputHole->GetParentBox ()->GetComponentName (), "OMX_PV", 6))
						errType = m_pChain->Connect (pOutputHole, pInputHole, OMX_FALSE);
					else
						errType = m_pChain->Connect (pOutputHole, pInputHole, m_bTunnelMode);
					//	errType = m_pChain->Connect (pOutputHole, pInputHole, OMX_FALSE);
					if (errType != OMX_ErrorNone)
					{
						VOLOGE ("It could not connect the video decoder with render component!");
						return OMX_ErrorUndefined;
					}
				}
			}

			m_pChain->m_bEndOfVideo = OMX_FALSE;
			m_pChain->m_pVideoSink = pVideoRender;
			OMX_PARAM_PORTDEFINITIONTYPE ptVideoRender;
			m_pChain->m_pVideoSink->SetHeadder (&ptVideoRender, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
			errType = m_pChain->m_pVideoSink->GetParameter (OMX_IndexParamPortDefinition, &ptVideoRender);
			ptVideoRender.format.video.nFrameWidth = m_nVideoWidth;
			ptVideoRender.format.video.nFrameHeight = m_nVideoHeight;
			errType = m_pChain->m_pVideoSink->SetParameter (OMX_IndexParamPortDefinition, &ptVideoRender);

			m_pChain->m_pVideoSink->SetConfig ((OMX_INDEXTYPE)OMX_VO_IndexConfigDisplayArea, &m_dispArea);

			if (m_cbVideoRender.pCallBack != NULL)
				m_pChain->m_pVideoSink->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexRenderBufferCallBack, &m_cbVideoRender);
		}
	}

	if (nPlayMode > 0)
	{
		if (pAudioDec == NULL && pVideoDec == NULL)
			return OMX_ErrorComponentNotFound;
	}
	else
	{
		if (errType != OMX_ErrorNone)
			return errType;
	}

	if (pClockBox != NULL)
	{
		OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkRef;
		pClockBox->SetHeadder (&clkRef, sizeof (OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE));

		if (pAudioDec != NULL || bIsPCM == OMX_TRUE)
			clkRef.eClock = OMX_TIME_RefClockAudio;
		else
			clkRef.eClock = OMX_TIME_RefClockVideo;
		pClockBox->SetConfig (OMX_IndexConfigTimeActiveRefClock, &clkRef);
	}
	m_sStatus = VOME_STATUS_Loaded;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseGraph::Transcode (VOME_TRANSCODETYPE * pTranscode)
{
	if (!m_bInit)
		Init (NULL);
	if (m_pChain == NULL)
		return OMX_ErrorResourcesLost;

	InitLicenseCheck ();

	OMX_ERRORTYPE errType = Stop ();
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("The result is 0X%08X from Stop!", errType);
		return errType;
	}

	voCOMXAutoLock lock (&m_tmState);
	errType = m_pChain->ReleaseBoxes ();
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("The result is 0X%08X from m_pChain->ReleaseBoxes!", errType);
		return errType;
	}

	VOME_SOURCECONTENTTYPE sSourceContent;
	memset(&sSourceContent, 0, sizeof(sSourceContent));
	sSourceContent.nSize = sizeof(VOME_SOURCECONTENTTYPE);
	sSourceContent.nVersion = pTranscode->nVersion;
	sSourceContent.pSource = pTranscode->pSource;
	sSourceContent.nType = pTranscode->nSourceType;
	sSourceContent.nOffset = pTranscode->nSourceOffset;
	sSourceContent.nLength = pTranscode->nSourceLength;
	sSourceContent.pPipe = pTranscode->pFileOP;
	sSourceContent.pDrmCB = pTranscode->pDrmCB;
	sSourceContent.pLibOP = pTranscode->pLibOP;
	voCOMXBaseBox * pSourceBox = m_pChain->AddSource (&sSourceContent);
	if (pSourceBox == NULL)
	{
		VOLOGE ("Create source component fail!");
		return OMX_ErrorFormatNotDetected;
	}

	OMX_S32 nPlayMode = 0;
	if (m_pConfig != NULL)
		nPlayMode = m_pConfig->GetItemValue ((OMX_STRING)"vomeMF", (OMX_STRING)"PlayMode", 0);

	OMX_PARAM_PORTDEFINITIONTYPE *	pPortType = NULL;
	voCOMXBaseHole *				pHole = NULL;
	voCOMXBaseHole *				pOutputHole = NULL;
	voCOMXBaseHole *				pInputHole = NULL;

	voCOMXBaseBox *					pAudioDec = NULL;
	voCOMXBaseBox *					pVideoDec = NULL;

	voCOMXBaseBox *					pAudioEnc = NULL;
	voCOMXBaseBox *					pVideoEnc = NULL;

	voCOMXBaseBox *					pVideoRsz = NULL;

	voCOMXBaseBox *					pFileSink = NULL;

	OMX_U32							nTryTimes = 0;
	OMX_BOOL						bExit = OMX_FALSE;
	for (OMX_U32 i = 0; i < pSourceBox->GetHoleCount (); i++)
	{
		pHole = pSourceBox->GetHole (i);
		pPortType = pHole->GetPortType ();

		if (pPortType->eDomain == OMX_PortDomainOther && pPortType->eDir == OMX_DirInput)
		{
			// source component clock input port
		}
		else if (pPortType->eDomain == OMX_PortDomainAudio && pPortType->eDir == OMX_DirOutput && (m_nPlayFlag & 0X02))
		{
			// source component audio output port
			pOutputHole = pHole;

			OMX_AUDIO_PARAM_PORTFORMATTYPE tpAudioFormat;
			pSourceBox->SetHeadder (&tpAudioFormat, sizeof (OMX_AUDIO_PARAM_PORTFORMATTYPE));
			tpAudioFormat.nPortIndex = i;
			errType = pSourceBox->GetParameter (OMX_IndexParamAudioPortFormat, &tpAudioFormat);
			if (errType != OMX_ErrorNone)
			{
				if (nPlayMode > 0)
					continue;

				VOLOGE ("The result is 0X%08X from pSourceBox->GetParameter OMX_IndexParamAudioPortFormat !", errType);
				return errType;
			}

			if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingPCM)
				pOutputHole = pHole;
			else
			{
				nTryTimes = 0;
				bExit = OMX_FALSE;
				while (nTryTimes < 5)
				{
					pAudioDec = TryConnectAudioDec (pOutputHole, nTryTimes, bExit);
					if (pAudioDec != NULL || bExit)
						break;
					nTryTimes++;
				}

				if (pAudioDec == NULL)
				{
					if (nPlayMode > 0)
						continue;

					VOLOGE ("Connect to the audio decoder component fail!");
					NotifyCallbackMsg (VOME_CID_PLAY_UNSUPPORTEDCODEC, 0, 0);
					return OMX_ErrorComponentNotFound;
				}

				pOutputHole = pAudioDec->GetHole (1);
			}

			OMX_U32 nBitrate = pTranscode->nAudioBitrate;
			if(nBitrate == 0)
			{
				OMX_AUDIO_PARAM_PORTFORMATTYPE tpAudioFormat;
				voOMXBase_SetHeader (&tpAudioFormat, sizeof (OMX_AUDIO_PARAM_PORTFORMATTYPE));
				tpAudioFormat.nPortIndex = i;
				errType = pSourceBox->GetParameter (OMX_IndexParamAudioPortFormat, &tpAudioFormat);
				if (errType != OMX_ErrorNone)
					return errType;

				if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingMP3)
				{
					OMX_AUDIO_PARAM_MP3TYPE tpMP3Format;
					voOMXBase_SetHeader (&tpMP3Format, sizeof (OMX_AUDIO_PARAM_MP3TYPE));
					tpMP3Format.nPortIndex = i;
					errType = pSourceBox->GetParameter (OMX_IndexParamAudioMp3, &tpMP3Format);
					if (errType == OMX_ErrorNone)
						nBitrate = tpMP3Format.nBitRate;
				}
				else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingAAC)
				{
					OMX_AUDIO_PARAM_AACPROFILETYPE tpAACFormat;
					voOMXBase_SetHeader (&tpAACFormat, sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));
					tpAACFormat.nPortIndex = i;
					errType = pSourceBox->GetParameter (OMX_IndexParamAudioAac, &tpAACFormat);
					if (errType == OMX_ErrorNone)
						nBitrate = tpAACFormat.nBitRate;
				}
				else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingAMR)
				{
					OMX_AUDIO_PARAM_AMRTYPE tpAmrFormat;
					voOMXBase_SetHeader (&tpAmrFormat, sizeof (OMX_AUDIO_PARAM_AMRTYPE));
					tpAmrFormat.nPortIndex = i;
					errType = pSourceBox->GetParameter (OMX_IndexParamAudioAmr, &tpAmrFormat);
					if (errType == OMX_ErrorNone)
						nBitrate = tpAmrFormat.nBitRate;
				}
				else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingWMA)
				{
					OMX_AUDIO_PARAM_WMATYPE tpWMAFormat;
					voOMXBase_SetHeader (&tpWMAFormat, sizeof (OMX_AUDIO_PARAM_WMATYPE));
					tpWMAFormat.nPortIndex = i;
					errType = pSourceBox->GetParameter (OMX_IndexParamAudioWma, &tpWMAFormat);
					if (errType == OMX_ErrorNone)
						nBitrate = tpWMAFormat.nBitRate;
				}
				else if(tpAudioFormat.eEncoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingDTS)
				{
					OMX_VO_AUDIO_PARAM_DTSTYPE tpDTSFormat;
					voOMXBase_SetHeader (&tpDTSFormat, sizeof (OMX_VO_AUDIO_PARAM_DTSTYPE));
					errType = pSourceBox->GetParameter ((OMX_INDEXTYPE)OMX_VO_IndexAudioParamDTS, &tpDTSFormat);
					if (errType == OMX_ErrorNone)
						nBitrate = tpDTSFormat.nBitRate;
				}
				else if(tpAudioFormat.eEncoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAC3 || tpAudioFormat.eEncoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingEAC3)
				{
					OMX_VO_AUDIO_PARAM_AC3TYPE tpAC3Format;
					voOMXBase_SetHeader (&tpAC3Format, sizeof (OMX_VO_AUDIO_PARAM_AC3TYPE));
					errType = pSourceBox->GetParameter ((OMX_INDEXTYPE)OMX_VO_IndexAudioParamAC3, &tpAC3Format);
					if (errType == OMX_ErrorNone)
						nBitrate = tpAC3Format.nBitRate;
				}
			}
			if(nBitrate == 0)
				nBitrate = 0x20000;	// 128kbps

			// now it is audio PCM data
			nTryTimes = 0;
			bExit = OMX_FALSE;
			while (nTryTimes < 5)
			{
				pAudioEnc = TryConnectAudioEnc (pOutputHole, pTranscode->nAudioCoding, nBitrate, pTranscode->nDestFormat, nTryTimes, bExit);
				if (pAudioEnc != NULL || bExit)
					break;
				nTryTimes++;
			}

			if (pAudioEnc == NULL)
			{
				if (nPlayMode > 0)
					continue;

				VOLOGE ("Connect to the audio encoder component fail!");
				NotifyCallbackMsg (VOME_CID_PLAY_UNSUPPORTEDCODEC, 0, 0);
				return OMX_ErrorComponentNotFound;
			}
		}
		else if (pPortType->eDomain == OMX_PortDomainVideo && pPortType->eDir == OMX_DirOutput && (m_nPlayFlag & 0X01))
		{
			m_nVideoWidth = pPortType->format.video.nFrameWidth;
			m_nVideoHeight = pPortType->format.video.nFrameHeight;

			errType = pSourceBox->GetParameter((OMX_INDEXTYPE)OMX_VO_IndexVideoRotation, &m_nVideoRotation);

			pOutputHole = pHole;

			nTryTimes = 0;
			bExit = OMX_FALSE;
			while (nTryTimes < 5)
			{
				pVideoDec = TryConnectVideoDec (pOutputHole, nTryTimes, bExit);
				if (pVideoDec != NULL || bExit)
					break;
				nTryTimes++;
			}
			if (pVideoDec == NULL)
			{
				if (nPlayMode > 0)
					continue;

				VOLOGE ("Try to connect the video decoer component was failed!");
				NotifyCallbackMsg (VOME_CID_PLAY_UNSUPPORTEDCODEC, 0, 0);
				return OMX_ErrorComponentNotFound;
			}
			m_pChain->m_pVideoDec = pVideoDec;

			pVideoDec->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexSourceDrm, m_pChain->m_pDrmCB);
			pOutputHole = pVideoDec->GetHole (1);

			// now it is video YUV data
			pVideoRsz = TryConnectVideoRsz (pOutputHole, pTranscode->nVideoWidth, pTranscode->nVideoHeight);
			if(NULL != pVideoRsz)
				pOutputHole = pVideoRsz->GetHole(1);

			OMX_U32 nBitrate = pTranscode->nVideoBitrate;
			if(nBitrate == 0)
				nBitrate = OMX_U32(pPortType->format.video.nBitrate * \
				(pTranscode->nVideoWidth * pTranscode->nVideoHeight / (double(pPortType->format.video.nFrameWidth * pPortType->format.video.nFrameHeight))));
			if(nBitrate == 0)
			{
				nBitrate = pTranscode->nVideoWidth * pTranscode->nVideoHeight;
				if(pTranscode->nVideoQuality == 0)	// normal
					nBitrate *= 3;
				else if(pTranscode->nVideoQuality == 1)	// middle
					nBitrate *= 4;
				else	// high
					nBitrate *= 6;
			}

			OMX_U32 xFramerate = pTranscode->xVideoFrameRate;
			if(xFramerate == 0)
				xFramerate = pPortType->format.video.xFramerate;
			if(xFramerate == 0)
				xFramerate = (30000 << 16) | 1001;	// 29.97

			VOLOGI("Transcode Bitrate %d, Transcode FrameRate 0x%08X, Transcode Quality %d, Transcode Width %d, Transcode Height %d", 
				(int)pTranscode->nVideoBitrate, (unsigned int)pTranscode->xVideoFrameRate, (int)pTranscode->nVideoQuality, (int)pTranscode->nVideoWidth, (int)pTranscode->nVideoHeight);

			VOLOGI("Source Bitrate %d, Source FrameRate 0x%08X, Source Width %d, Source Height %d", 
				(int)pPortType->format.video.nBitrate, (unsigned int)pPortType->format.video.xFramerate, (int)pPortType->format.video.nFrameWidth, (int)pPortType->format.video.nFrameHeight);

			VOLOGI("Bitrate %d, FrameRate 0x%08X", (int)nBitrate, (unsigned int)xFramerate);

			nTryTimes = 0;
			bExit = OMX_FALSE;
			while (nTryTimes < 5)
			{
				pVideoEnc = TryConnectVideoEnc (pOutputHole, pTranscode->nVideoCoding, nBitrate, xFramerate, pTranscode->nVideoQuality, nTryTimes, bExit);
				if (pVideoEnc != NULL || bExit)
					break;
				nTryTimes++;
			}
			if (pVideoEnc == NULL)
			{
				if (nPlayMode > 0)
					continue;

				VOLOGE ("Try to connect the video encoder component was failed!");
				NotifyCallbackMsg (VOME_CID_PLAY_UNSUPPORTEDCODEC, 0, 0);
				return OMX_ErrorComponentNotFound;
			}
		}
	}

	if (nPlayMode > 0)
	{
		if (pAudioEnc == NULL && pVideoEnc == NULL)
			return OMX_ErrorComponentNotFound;
	}
	else
	{
		if (errType != OMX_ErrorNone)
			return errType;
	}

	// connect to file sink component
	pFileSink = m_pChain->AddBox((OMX_STRING)"OMX.VisualOn.FileSink");
	if (pFileSink == NULL)
	{
		VOLOGE ("It could not add the file sink component!");
		return OMX_ErrorComponentNotFound;
	}
	pFileSink->CreateHoles();

	if(pAudioEnc)
	{
		pOutputHole = pAudioEnc->GetHole(1);
		pInputHole = pFileSink->GetHole(0);
		errType = m_pChain->Connect (pOutputHole, pInputHole, m_bTunnelMode);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("It could not connect the audio encoder with file sink component!");
			return OMX_ErrorUndefined;
		}
		m_pChain->m_bEndOfAudio = OMX_FALSE;
	}

	if(pVideoEnc)
	{
		pOutputHole = pVideoEnc->GetHole(1);
		pInputHole = pFileSink->GetHole(1);
		errType = m_pChain->Connect (pOutputHole, pInputHole, m_bTunnelMode);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("It could not connect the video encoder with file sink component!");
			return OMX_ErrorUndefined;
		}
		m_pChain->m_bEndOfVideo = OMX_FALSE;
	}

	errType = pFileSink->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexFileFormat, &pTranscode->nDestFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("File Sink OMX_VO_IndexFileFormat fail 0x%08X!", errType);
		return OMX_ErrorUndefined;
	}

	errType = pFileSink->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexFileMaxAVDiffTime, &pTranscode->nMaxAVDiffTime);

	if(VOME_TRANSCODE_DEST_File == pTranscode->nDestType)
	{
		VO_TCHAR * pURI = (VO_TCHAR *)pTranscode->pDest;
		OMX_U32 uSize = (vostrlen(pURI) + 1) * sizeof(VO_TCHAR);
		OMX_PARAM_CONTENTURITYPE * pContent = (OMX_PARAM_CONTENTURITYPE *)voOMXMemAlloc(uSize + 8);
		voOMXMemCopy(pContent->contentURI, pURI, uSize);
		pContent->nSize = uSize + 8;
		errType = pFileSink->SetParameter(OMX_IndexParamContentURI, pContent);
		voOMXMemFree(pContent);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("File Sink OMX_IndexParamContentURI fail 0x%08X!", errType);
			return OMX_ErrorUndefined;
		}
	}
	else if(VOME_TRANSCODE_DEST_Callback == pTranscode->nDestType)
	{
		VOLOGE ("File Sink Callback not implemented!");
		return OMX_ErrorUndefined;
	}
	else
	{
		VOLOGE ("Unknown destination type %d not implemented!", pTranscode->nDestType);
		return OMX_ErrorUndefined;
	}

	m_sStatus = VOME_STATUS_Loaded;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseGraph::Record (VOME_RECORDERFILETYPE * pRecFile)
{
	return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE voCOMXBaseGraph::Snapshot (VOME_RECORDERIMAGETYPE * pRecImage)
{
	return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE voCOMXBaseGraph::Close (void)
{
	if (m_pChain == NULL)
		return OMX_ErrorNotImplemented;

	if (m_pChain->m_pSeekBox != NULL)
	{
		if (m_pChain->m_pSeekBox->GetCompState () == OMX_StateLoaded)
		{
			VOLOGI ("Send Command to Source Comp to loaded status!");
			m_pChain->m_pSeekBox->GetComponent ()->SendCommand (m_pChain->m_pSeekBox->GetComponent (), OMX_CommandStateSet, OMX_StateLoaded, NULL);
		}
	}

	voCOMXAutoLock lock (&m_tmState);

	return m_pChain->ReleaseBoxes ();
}

OMX_ERRORTYPE  voCOMXBaseGraph::Run (void)
{
	voCOMXAutoLock lock (&m_tmState);

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pChain == NULL)
		return OMX_ErrorNotImplemented;

	errType = m_pChain->Run ();
	if (errType == OMX_ErrorNone)
		m_sStatus = VOME_STATUS_Running;

	m_nStartRunTime = voOMXOS_GetSysTime ();

	return errType;
}

OMX_ERRORTYPE  voCOMXBaseGraph::Pause (void)
{
	voCOMXAutoLock lock (&m_tmState);

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pChain == NULL)
		return OMX_ErrorNotImplemented;

	errType = m_pChain->Pause ();
	if (errType == OMX_ErrorNone)
		m_sStatus = VOME_STATUS_Paused;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE  voCOMXBaseGraph::Stop (void)
{
	voCOMXAutoLock lock (&m_tmState);

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pChain == NULL)
		return OMX_ErrorNotImplemented;

	errType = m_pChain->Stop ();
	if (errType == OMX_ErrorNone)
		m_sStatus = VOME_STATUS_Stopped;

	m_nSeekPos = 0;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE  voCOMXBaseGraph::GetStatus (VOMESTATUS * pStatus)
{
	voCOMXAutoLock lock (&m_tmState);

	if (pStatus == NULL)
		return OMX_ErrorBadParameter;

	*pStatus = m_sStatus;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE  voCOMXBaseGraph::GetDuration (OMX_S32 * pDuration)
{
	voCOMXAutoLock lock (&m_tmState);

	if (m_pChain != NULL)
	{
		*pDuration = m_pChain->GetDuration ();
		return OMX_ErrorNone;
	}
	else
	{
		return OMX_ErrorUndefined;
	}
}

OMX_ERRORTYPE  voCOMXBaseGraph::GetCurPos (OMX_S32 * pCurPos)
{
	voCOMXAutoLock lock (&m_tmState);

	if (m_pChain != NULL)
	{
		*pCurPos = m_pChain->GetPos ();
		return OMX_ErrorNone;
	}
	else
	{
		return OMX_ErrorUndefined;
	}
}

OMX_ERRORTYPE  voCOMXBaseGraph::SetCurPos (OMX_S32 nCurPos)
{
	voCOMXAutoLock lock (&m_tmState);

/*
	if (m_nStartRunTime > 0)
	{
		if (voOMXOS_GetSysTime () - m_nStartRunTime < 2000)
		{
			VOLOGI("voOMXOS_GetSysTime () - m_nStartRunTime < 2000");
			return OMX_ErrorNotImplemented;
		}
	}
	m_nStartRunTime = 0;
*/
/*
	if (nCurPos != 0)
	{
		if (voOMXOS_GetSysTime () - m_nSeekTime < 1000)
		{
			VOLOGI("voOMXOS_GetSysTime () - m_nSeekTime < 1000");
			return OMX_ErrorNotImplemented;
		}
	}
*/
	if (m_nSeekPos != 0)
	{
		if (m_nStartRunTime > 0)
		{
			if (voOMXOS_GetSysTime () - m_nStartRunTime < 1000)
			{
				if ((nCurPos >  m_nSeekPos - 300) && (nCurPos < m_nSeekPos + 300))
				{
					VOLOGI("(nCurPos >  m_nSeekPos - 300) && (nCurPos < m_nSeekPos + 300)");
					return OMX_ErrorNone;
				}
			}
		}
	}
	m_nStartRunTime = voOMXOS_GetSysTime ();;

	if (m_pChain != NULL)
	{
		OMX_S32		nRC = 0;
		nRC = m_pChain->SetPos (nCurPos);
		if (nRC < 0)
		{
			return OMX_ErrorInvalidState;
		}
		else
		{
			m_nSeekPos = nCurPos;
			m_nSeekTime = voOMXOS_GetSysTime ();
			return OMX_ErrorNone;
		}
	}
	else
	{
		VOLOGE("m_pChain == NULL");
		return OMX_ErrorNotImplemented;
	}
}

OMX_ERRORTYPE voCOMXBaseGraph::SetCompCallBack (OMX_CALLBACKTYPE * pCompCallBack)
{
	if (m_pChain != NULL)
	{
		m_pChain->SetCompCallBack (pCompCallBack);
		return OMX_ErrorNone;
	}
	else
	{
		return OMX_ErrorUndefined;
	}
}

OMX_ERRORTYPE voCOMXBaseGraph::EnumComponentName (OMX_STRING pCompName, OMX_U32 nIndex)
{
	if (!m_bInit)
		Init (NULL);
	if (m_pChain == NULL)
		return OMX_ErrorResourcesLost;

	return m_cmpList.GetComponentName (pCompName, nIndex);
}

OMX_ERRORTYPE voCOMXBaseGraph::GetRolesOfComponent (OMX_STRING pCompName, OMX_U32 * pNumRoles, OMX_U8 ** ppRoles)
{
	return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE voCOMXBaseGraph::GetComponentsOfRole (OMX_STRING pRole, OMX_U32 * pNumComps, OMX_U8 ** ppCompNames)
{
	return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE voCOMXBaseGraph::AddComponent (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent)
{
	if (!m_bInit)
		Init (NULL);
	if (m_pChain == NULL)
		return OMX_ErrorResourcesLost;

	InitLicenseCheck ();

//	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (m_pChain == NULL)
		return OMX_ErrorNotReady;

	voCOMXBaseBox * pBox = m_pChain->AddBox (pName);
	if (pBox == NULL)
		return OMX_ErrorInvalidComponentName;

	pBox->CreateHoles ();

	*ppComponent = pBox->GetComponent ();

	if (!strcmp (pName, "OMX.VisualOn.Clock.Time"))
		m_pChain->m_pClockBox = pBox;
	else if (!strcmp (pName, "OMX.VisualOn.Video.Sink"))
	{
		m_pChain->m_bEndOfVideo = OMX_FALSE;
		m_pChain->m_pVideoSink = pBox;
		m_pChain->m_pVideoSink->SetConfig ((OMX_INDEXTYPE)OMX_VO_IndexConfigDisplayArea, &m_dispArea);
		if (m_cbVideoRender.pCallBack != NULL)
			m_pChain->m_pVideoSink->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexRenderBufferCallBack, &m_cbVideoRender);
	}
	else if (!strcmp (pName, "OMX.VisualOn.Audio.Sink"))
	{
		m_pChain->m_bEndOfAudio = OMX_FALSE;
		m_pChain->m_pAudioSink = pBox;
		if (m_cbAudioRender.pCallBack != NULL)
			m_pChain->m_pAudioSink->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexRenderBufferCallBack, &m_cbAudioRender);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseGraph::RemoveComponent (OMX_COMPONENTTYPE * pComponent)
{
	if (m_pChain == NULL)
		return OMX_ErrorNotReady;

	return m_pChain->RemoveComp (pComponent);
}

OMX_ERRORTYPE voCOMXBaseGraph::ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
											 OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel)
{
	if (m_pChain == NULL)
		return OMX_ErrorNotReady;

	return m_pChain->ConnectPorts (pOutputComp, nOutputPort, pInputComp, nInputPort, bTunnel);
}

OMX_ERRORTYPE voCOMXBaseGraph::GetComponentByName (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent)
{
	if (m_pChain == NULL)
		return OMX_ErrorNotReady;

	if (pName == NULL || ppComponent == NULL)
		return OMX_ErrorBadParameter;
	
	*ppComponent = NULL;

	OMX_ERRORTYPE errType = OMX_ErrorInvalidComponentName;

	voCOMXBaseBox ** ppBox = m_pChain->GetBoxList ();
	for (int i = 0; i < VOMAX_BOX_NUM; i++)
	{
		if (ppBox[i] != NULL)
		{
			if (!strcmp (ppBox[i]->GetComponentName (), pName))
			{
				*ppComponent = ppBox[i]->GetComponent ();
				errType = OMX_ErrorNone;
				break;
			}
		}
	}

	return errType;
}

OMX_ERRORTYPE voCOMXBaseGraph::GetComponentByIndex (OMX_U32 nIndex, OMX_COMPONENTTYPE ** ppComponent)
{
	if (m_pChain == NULL)
		return OMX_ErrorNotReady;

	if (ppComponent == NULL || nIndex >= VOMAX_BOX_NUM)
		return OMX_ErrorBadParameter;
	
	*ppComponent = NULL;

	OMX_ERRORTYPE	errType = OMX_ErrorInvalidComponent;
	OMX_U32			nCompCount = 0;
	voCOMXBaseBox ** ppBox = m_pChain->GetBoxList ();
	for (int i = 0; i < VOMAX_BOX_NUM; i++)
	{
		if (ppBox[i] != NULL)
		{
			if (nCompCount == nIndex)
			{
				*ppComponent = ppBox[i]->GetComponent ();
				errType = OMX_ErrorNone;
				break;
			}
			nCompCount++;
		}
	}

	return errType;
}

OMX_ERRORTYPE voCOMXBaseGraph::SaveGraph (OMX_STRING pFile)
{
	return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE voCOMXBaseGraph::OpenGraph (OMX_STRING pFile)
{
	return OMX_ErrorNotImplemented;
}

voCOMXBaseBox * voCOMXBaseGraph::TryConnectAudioDec (voCOMXBaseHole * pOutputHole, OMX_U32 nTimes, OMX_BOOL& bExit)
{
	bExit = OMX_FALSE;
	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	OMX_STRING		pAudioCompName = NULL;
	OMX_AUDIO_PARAM_PORTFORMATTYPE tpAudioFormat;

	voCOMXBaseBox * pSourceBox = pOutputHole->GetParentBox ();
	pSourceBox->SetHeadder (&tpAudioFormat, sizeof (OMX_AUDIO_PARAM_PORTFORMATTYPE));
	tpAudioFormat.nPortIndex = pOutputHole->GetPortIndex ();

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = pOutputHole->GetPortType ();
	if (pPortType->format.audio.cMIMEType != NULL && strlen (pPortType->format.audio.cMIMEType) > 0)
	{
		voCOMXBaseBox * pSourceBox = pOutputHole->GetParentBox ();
		OMX_U32 nFourCC = pSourceBox->GetAudioFourCC();
		pAudioCompName = m_cfgComp.GetCodecCompNameByMIME (OMX_PortDomainAudio, 0, pPortType->format.audio.cMIMEType, nFourCC , nTimes);
	}
	if (pAudioCompName == NULL)
	{
		errType = pSourceBox->GetParameter (OMX_IndexParamAudioPortFormat, &tpAudioFormat);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("The result is 0X%08X GetParameter (OMX_IndexParamAudioPortFormat)", errType);
			return NULL;
		}

		pAudioCompName = m_cfgComp.GetCodecCompNameByCoding (OMX_PortDomainAudio, 0, tpAudioFormat.eEncoding, nTimes);
	}
	if (pAudioCompName == NULL)
	{
		VOLOGE ("It could not find the audio decoder component at times %d.", (int)nTimes);
		return NULL;
	}

	if(!strcmp(pAudioCompName, "OMX.VisualOn.Forbid"))
	{
		bExit = OMX_TRUE;
		return NULL;
	}

	voCOMXBaseBox * pAudioDec = m_pChain->AddBox (pAudioCompName);
	if (pAudioDec == NULL)
	{
		VOLOGE ("It could not add the audio decoder component %s.", pAudioCompName);
		return NULL;
	}

	// set the role for component
	if (strstr (pAudioCompName, "VisualOn") == NULL)
	{
		char	szAudioDecRole[128];
		if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingADPCM)
			strcpy (szAudioDecRole, "audio_decoder.adpcm");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingAMR)
			strcpy (szAudioDecRole, "audio_decoder.amr");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingGSMFR)
			strcpy (szAudioDecRole, "audio_decoder.gsmfr");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingGSMEFR)
			strcpy (szAudioDecRole, "audio_decoder.gsmefr");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingGSMHR)
			strcpy (szAudioDecRole, "audio_decoder.gsmmhr");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingPDCFR)
			strcpy (szAudioDecRole, "audio_decoder.pdcfr");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingPDCEFR)
			strcpy (szAudioDecRole, "audio_decoder.pdcefr");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingPDCHR)
			strcpy (szAudioDecRole, "audio_decoder.pdchr");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingTDMAFR)
			strcpy (szAudioDecRole, "audio_decoder.tdmafr");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingTDMAEFR)
			strcpy (szAudioDecRole, "audio_decoder.tdmaefr");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingQCELP8)
			strcpy (szAudioDecRole, "audio_decoder.qcelp8");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingQCELP13)
			strcpy (szAudioDecRole, "audio_decoder.qcelp13");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingEVRC)
			strcpy (szAudioDecRole, "audio_decoder.evrc");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingSMV)
			strcpy (szAudioDecRole, "audio_decoder.smv");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingG711)
			strcpy (szAudioDecRole, "audio_decoder.g711");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingG723)
			strcpy (szAudioDecRole, "audio_decoder.g723");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingG726)
			strcpy (szAudioDecRole, "audio_decoder.g726");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingG729)
			strcpy (szAudioDecRole, "audio_decoder.g729");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingAAC)
			strcpy (szAudioDecRole, "audio_decoder.aac");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingMP3)
			strcpy (szAudioDecRole, "audio_decoder.mp3");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingSBC)
			strcpy (szAudioDecRole, "audio_decoder.src");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingVORBIS)
			strcpy (szAudioDecRole, "audio_decoder.ogg");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingWMA)
			strcpy (szAudioDecRole, "audio_decoder.wma");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingRA)
			strcpy (szAudioDecRole, "audio_decoder.ra");
		else if (pPortType->format.audio.eEncoding == OMX_AUDIO_CodingMIDI)
			strcpy (szAudioDecRole, "audio_decoder.midi");
		else
			strcpy (szAudioDecRole, "audio_decoder.unknown");

		OMX_U8		szRole[OMX_MAX_STRINGNAME_SIZE];
		OMX_STRING	pRole = (OMX_STRING)szRole;
		OMX_U32		nIndex = 0;
		errType = OMX_ErrorNone;
		while (errType == OMX_ErrorNone && pAudioDec->GetComponent ()->ComponentRoleEnum != NULL)
		{
			memset (szRole, 0, 128);
			errType = pAudioDec->GetComponent ()->ComponentRoleEnum (pAudioDec->GetComponent (), szRole, nIndex++);
			if (errType != NULL)
				break;
			
			if (!strcmp (pRole, szAudioDecRole))
			{
				OMX_PARAM_COMPONENTROLETYPE rolType;
				pAudioDec->SetHeadder (&rolType, sizeof (OMX_PARAM_COMPONENTROLETYPE));
				memcpy (rolType.cRole, szRole, OMX_MAX_STRINGNAME_SIZE);
				pAudioDec->SetParameter (OMX_IndexParamStandardComponentRole, &rolType);
				break;
			}
		}
	}

	pAudioDec->CreateHoles ();
	if (pAudioDec->GetHoleCount () != 2)
	{
		m_pChain->RemoveBox (pAudioDec);
		VOLOGE ("The component was not filter component.");
		return NULL;
	}

	voCOMXBaseHole * pInput = pAudioDec->GetHole (0);

	// Update the port buffer count for source component
	if (strstr (pAudioCompName, "VisualOn") == NULL)
	{
		OMX_PARAM_PORTDEFINITIONTYPE * pInputType = pInput->GetPortType ();
		pPortType->nBufferCountActual = pInputType->nBufferCountActual;
		pPortType->nBufferCountMin = pInputType->nBufferCountMin;
		pOutputHole->GetParentBox ()->SetParameter (OMX_IndexParamPortDefinition, pPortType);
		pOutputHole->FillPortType ();
	}

	if (!strncmp (pAudioDec->GetComponentName (), "OMX.PV", 6))
		errType = m_pChain->Connect (pOutputHole, pInput, OMX_FALSE);
	else
		errType = m_pChain->Connect (pOutputHole, pInput, m_bTunnelMode);
	if (errType != OMX_ErrorNone)
	{
		m_pChain->RemoveBox (pAudioDec);
		VOLOGE ("It could not connect the file source and audio decoder components. The result is 0X%08X.", errType);
		return NULL;
	}

	return pAudioDec;
}

voCOMXBaseBox * voCOMXBaseGraph::TryConnectAudioEnc (voCOMXBaseHole * pOutputHole, OMX_AUDIO_CODINGTYPE nCoding, OMX_U32 nBitrate, OMX_VO_FILE_FORMATTYPE nFormat, OMX_U32 nTimes, OMX_BOOL& bExit)
{
	bExit = OMX_FALSE;

	OMX_STRING pAudioCompName = m_cfgComp.GetCodecCompNameByCoding (OMX_PortDomainAudio, 1, nCoding, nTimes);
	if (pAudioCompName == NULL)
	{
		VOLOGE ("It could not find the audio encoder component at times %d.", (int)nTimes);
		return NULL;
	}

	if(!strcmp(pAudioCompName, "OMX.VisualOn.Forbid"))
	{
		bExit = OMX_TRUE;
		return NULL;
	}

	voCOMXBaseBox * pAudioEnc = m_pChain->AddBox (pAudioCompName);
	if (pAudioEnc == NULL)
	{
		VOLOGE ("It could not add the audio encoder component %s.", pAudioCompName);
		return NULL;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	// set the role for component
//	if (strstr (pAudioCompName, "VisualOn") == NULL)
	{
		char szAudioDecRole[128];
		if (nCoding == OMX_AUDIO_CodingAMR)
			strcpy (szAudioDecRole, "audio_encoder.amr");
		else if (nCoding == OMX_AUDIO_CodingQCELP13)
			strcpy (szAudioDecRole, "audio_encoder.qcelp13");
		else if (nCoding == OMX_AUDIO_CodingEVRC)
			strcpy (szAudioDecRole, "audio_encoder.evrc");
		else if (nCoding == OMX_AUDIO_CodingAAC)
			strcpy (szAudioDecRole, "audio_encoder.aac");
		else if (nCoding == OMX_AUDIO_CodingMP3)
			strcpy (szAudioDecRole, "audio_encoder.mp3");
		else
			strcpy (szAudioDecRole, "audio_encoder.unknown");

		OMX_U8 szRole[OMX_MAX_STRINGNAME_SIZE];
		OMX_STRING pRole = (OMX_STRING)szRole;
		OMX_U32	nIndex = 0;
		errType = OMX_ErrorNone;
		while (errType == OMX_ErrorNone && pAudioEnc->GetComponent ()->ComponentRoleEnum != NULL)
		{
			memset (szRole, 0, sizeof(szRole));
			errType = pAudioEnc->GetComponent ()->ComponentRoleEnum (pAudioEnc->GetComponent (), szRole, nIndex++);
			if (errType != OMX_ErrorNone)
				break;

			if (!strcmp (pRole, szAudioDecRole))
			{
				OMX_PARAM_COMPONENTROLETYPE rolType;
				pAudioEnc->SetHeadder (&rolType, sizeof (OMX_PARAM_COMPONENTROLETYPE));
				memcpy (rolType.cRole, szRole, OMX_MAX_STRINGNAME_SIZE);
				pAudioEnc->SetParameter (OMX_IndexParamStandardComponentRole, &rolType);
				break;
			}
		}
	}

	pAudioEnc->CreateHoles ();
	if (pAudioEnc->GetHoleCount () != 2)
	{
		m_pChain->RemoveBox (pAudioEnc);
		VOLOGE ("The audio encoder component is not filter component!");
		return NULL;
	}

	voCOMXBaseHole * pInput = pAudioEnc->GetHole (0);

	// Update the port buffer count from previous component
	if (strstr (pAudioCompName, "VisualOn") == NULL)
	{
		OMX_PARAM_PORTDEFINITIONTYPE * pInputType = pInput->GetPortType ();
		OMX_PARAM_PORTDEFINITIONTYPE * pOutputType = pOutputHole->GetPortType ();
		pOutputType->nBufferCountActual = pInputType->nBufferCountActual;
		pOutputType->nBufferCountMin = pInputType->nBufferCountMin;
		pOutputHole->GetParentBox ()->SetParameter (OMX_IndexParamPortDefinition, pOutputType);
		pOutputHole->FillPortType ();
	}

	errType = m_pChain->Connect (pOutputHole, pInput, m_bTunnelMode);
	if (errType != OMX_ErrorNone)
	{
		m_pChain->RemoveBox (pAudioEnc);
		VOLOGE ("It could not connect audio encoder components. The result is 0X%08X.", errType);
		return NULL;
	}

	if (nCoding == OMX_AUDIO_CodingAAC)
	{
		OMX_AUDIO_PARAM_AACPROFILETYPE tpAACFormat;
		voOMXBase_SetHeader (&tpAACFormat, sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));
		tpAACFormat.nPortIndex = 1;
		errType = pAudioEnc->GetParameter (OMX_IndexParamAudioAac, &tpAACFormat);
		if (errType != OMX_ErrorNone)
		{
			m_pChain->RemoveBox (pAudioEnc);
			VOLOGE ("It could not get OMX_IndexParamAudioAac to audio encoder components. The result is 0X%08X.", errType);
			return NULL;
		}

		tpAACFormat.nBitRate = nBitrate;
		if(nFormat == OMX_VO_FILE_FormatTS)
			tpAACFormat.eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP2ADTS;
		else
			tpAACFormat.eAACStreamFormat = OMX_AUDIO_AACStreamFormatRAW;

		errType = pAudioEnc->SetParameter (OMX_IndexParamAudioAac, &tpAACFormat);
		if (errType != OMX_ErrorNone)
		{
			m_pChain->RemoveBox (pAudioEnc);
			VOLOGE ("It could not set OMX_IndexParamAudioAac to audio encoder components. The result is 0X%08X.", errType);
			return NULL;
		}
	}

	return pAudioEnc;
}

voCOMXBaseBox * voCOMXBaseGraph::TryConnectVideoDec (voCOMXBaseHole * pOutputHole, OMX_U32 nTimes, OMX_BOOL& bExit)
{
	bExit = OMX_FALSE;
	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	OMX_STRING		pVideoCompName = NULL;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = pOutputHole->GetPortType ();	
	OMX_U32 nFourCC = 0;
	if (pPortType->format.video.cMIMEType != NULL && strlen (pPortType->format.video.cMIMEType) > 0)
	{		
		voCOMXBaseBox * pSourceBox = pOutputHole->GetParentBox ();
		nFourCC = pSourceBox->GetVideoFourCC();
		pVideoCompName = m_cfgComp.GetCodecCompNameByMIME (OMX_PortDomainVideo, 0, pPortType->format.video.cMIMEType ,nFourCC , nTimes, 
			pPortType->format.video.nFrameWidth, pPortType->format.video.nFrameHeight, pPortType->format.video.nBitrate);
		
	}
	if (pVideoCompName == NULL)
	{
		voCOMXBaseBox * pSourceBox = pOutputHole->GetParentBox ();

		OMX_VIDEO_PARAM_PORTFORMATTYPE tpVideoFormat;
		pSourceBox->SetHeadder (&tpVideoFormat, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE));
		tpVideoFormat.nPortIndex = pOutputHole->GetPortIndex ();
		errType = pSourceBox->GetParameter (OMX_IndexParamVideoPortFormat, &tpVideoFormat);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("The result is 0X%08X GetParameter (OMX_IndexParamVideoPortFormat)", errType);
			return NULL;
		}

		pVideoCompName = m_cfgComp.GetCodecCompNameByCoding (OMX_PortDomainVideo, 0, tpVideoFormat.eCompressionFormat, nTimes, 
			pPortType->format.video.nFrameWidth, pPortType->format.video.nFrameHeight, pPortType->format.video.nBitrate);
	}
	//following code is workaround for Samsung h/w wmv decoder, it cannot support all kinds of codec
	if(pVideoCompName&& !strncmp(pVideoCompName, "OMX.SEC.",8))
	{
		if((!strncmp(pPortType->format.video.cMIMEType,"WMV", 3) &&
			memcmp((char*)&nFourCC, (char*)"WMV3", 4) && 
			memcmp((char*)&nFourCC, (char*)"wmv3", 4) &&
			memcmp((char*)&nFourCC, (char*)"WMVA", 4) &&
			memcmp((char*)&nFourCC, (char*)"WVC1", 4) &&
			memcmp((char*)&nFourCC, (char*)"wvc1", 4))
			||!strncmp(pPortType->format.video.cMIMEType,"MPV1", 4))
		{
			bExit = OMX_TRUE;
			return NULL;		
		}
	}
	if (pVideoCompName == NULL)
	{
		VOLOGE ("It could not find the video decoder component at times %d.", (int)nTimes);
		return NULL;
	}

	if(!strcmp(pVideoCompName, "OMX.VisualOn.Forbid"))
	{
		bExit = OMX_TRUE;
		return NULL;
	}
	voCOMXBaseBox * pVideoDec = m_pChain->AddBox (pVideoCompName);
	if (pVideoDec == NULL)
	{
		VOLOGE ("It could not add the video decoder component %s.", pVideoCompName);
		return NULL;
	}
	// set the role for component
	if (strstr (pVideoCompName, "VisualOn") == NULL)
	{
		char	szVideoDecRole[128];
		if (pPortType->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG2)
			strcpy (szVideoDecRole, "video_decoder.mpeg2");
		else if (pPortType->format.video.eCompressionFormat == OMX_VIDEO_CodingH263)
			strcpy (szVideoDecRole, "video_decoder.h263");
		else if (pPortType->format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4)
			strcpy (szVideoDecRole, "video_decoder.mpeg4");
		else if (pPortType->format.video.eCompressionFormat == OMX_VIDEO_CodingWMV)
		{
			if(!strncmp(pVideoCompName,  "OMX.Freescale.std.",18))
				strcpy (szVideoDecRole, "video_decoder.wmv9");
			else
				strcpy (szVideoDecRole, "video_decoder.wmv");
		}
		else if (pPortType->format.video.eCompressionFormat == OMX_VIDEO_CodingRV)
			strcpy (szVideoDecRole, "video_decoder.rv");
		else if (pPortType->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
			strcpy (szVideoDecRole, "video_decoder.avc");
		else if (pPortType->format.video.eCompressionFormat == OMX_VIDEO_CodingMJPEG)
			strcpy (szVideoDecRole, "video_decoder.mjpeg");
		else if (pPortType->format.video.eCompressionFormat == (OMX_VIDEO_CODINGTYPE)OMX_VO_VIDEO_CodingVP8)
			strcpy (szVideoDecRole, "video_decoder.vp8");
		else
			strcpy (szVideoDecRole, "video_decoder.unknown");

		OMX_U8		szRole[OMX_MAX_STRINGNAME_SIZE];
		OMX_STRING	pRole = (OMX_STRING)szRole;
		OMX_U32		nIndex = 0;
		errType = OMX_ErrorNone;
		
		const char* pTIComp = "OMX.TI.DUCATI1.VIDEO.DECODER" ;
		// added by gtxia for fix STE component don't implement ComponentRoleEnum function issue 2011-7-1
		if((!strncmp(pVideoCompName,"OMX.ST.VFM", 10) || !strncmp(pVideoCompName,pTIComp, strlen(pTIComp)) 
			|| !strncmp(pVideoCompName,  "OMX.Freescale.std.",18) ) 
		   && strncmp(szVideoDecRole,"video_decoder.unknown", 21))
		{
			OMX_PARAM_COMPONENTROLETYPE rolType;
			pVideoDec->SetHeadder (&rolType, sizeof (OMX_PARAM_COMPONENTROLETYPE));
			memcpy (rolType.cRole, szVideoDecRole, OMX_MAX_STRINGNAME_SIZE);
		
			errType = pVideoDec->SetParameter (OMX_IndexParamStandardComponentRole, &rolType);

			// do special configure for Freescale VC1/WMV9 decoder
			const char* pFsl_wmv = "OMX.Freescale.std.video_decoder.wmv9.hw-based";
			if(!strncmp(pVideoCompName, pFsl_wmv ,strlen(pFsl_wmv)))
			{
				OMX_VIDEO_PARAM_WMVTYPE wmvType;
				pVideoDec->SetHeadder (&wmvType, sizeof (OMX_VIDEO_PARAM_WMVTYPE));
				wmvType.nPortIndex = 0;
				wmvType.eFormat = OMX_VIDEO_WMVFormat9;
				errType = pVideoDec->SetParameter(OMX_IndexParamVideoWmv, &wmvType);
			}
			
		}
		else
		{
			while (errType == OMX_ErrorNone && pVideoDec->GetComponent ()->ComponentRoleEnum != NULL)
			{
				memset (szRole, 0, OMX_MAX_STRINGNAME_SIZE);
				errType = pVideoDec->GetComponent ()->ComponentRoleEnum (pVideoDec->GetComponent (), szRole, nIndex++);
				if (errType != NULL)
					break;
				
				if (!strcmp (pRole, szVideoDecRole))
				{
					OMX_PARAM_COMPONENTROLETYPE rolType;
					pVideoDec->SetHeadder (&rolType, sizeof (OMX_PARAM_COMPONENTROLETYPE));
					memcpy (rolType.cRole, szRole, OMX_MAX_STRINGNAME_SIZE);
					pVideoDec->SetParameter (OMX_IndexParamStandardComponentRole, &rolType);
					break;
				}
			}
		}
	}
	else
	{
		//add by Eric, 2011.08.19, fix RTSP video freeze issue 7015
		if(pPortType->format.video.eCompressionFormat == OMX_VIDEO_CodingAVC)
		{
			voCOMXBaseBox * pSourceBox = pOutputHole->GetParentBox ();
			VO_FILE_FORMAT nFileFormat;
			errType = pSourceBox->GetParameter((OMX_INDEXTYPE)OMX_VO_IndexFileFormat, (VO_PTR)&nFileFormat);
			if(errType == OMX_ErrorNone && nFileFormat == VO_FILE_FFSTREAMING_RTSP)
			{
				OMX_U32 nSourceType = 0;
				errType = pVideoDec->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexSourceType, &nSourceType);
			}
		}
	}
	pVideoDec->CreateHoles ();

	if (pVideoDec->GetHoleCount () != 2)
	{
		m_pChain->RemoveBox (pVideoDec);

		VOLOGE ("The component was not filter component.");
		return NULL;
	}

	voCOMXBaseHole * pInput = pVideoDec->GetHole (0);

	// Update the port buffer count for source component
	if (strstr (pVideoCompName, "VisualOn") == NULL)
	{
		OMX_PARAM_PORTDEFINITIONTYPE * pInputType = pInput->GetPortType ();
		pPortType->nBufferCountActual = pInputType->nBufferCountActual;
		pPortType->nBufferCountMin = pInputType->nBufferCountMin;
		pOutputHole->GetParentBox ()->SetParameter (OMX_IndexParamPortDefinition, pPortType);
		pOutputHole->FillPortType ();
	}

	if (!strncmp (pVideoDec->GetComponentName (), "OMX.PV", 6))
		errType = m_pChain->Connect (pOutputHole, pInput, OMX_FALSE);
	else if(!strncmp (pVideoDec->GetComponentName (), "OMX.ST.VFM.", 11))
		errType = m_pChain->Connect (pOutputHole, pInput, OMX_FALSE);
	else if(!strncmp (pVideoDec->GetComponentName (), "OMX.hantro", 10))
		errType = m_pChain->Connect (pOutputHole, pInput, OMX_FALSE);
	else
		errType = m_pChain->Connect (pOutputHole, pInput, m_bTunnelMode);
	if (errType != OMX_ErrorNone)
	{
		m_pChain->RemoveBox (pVideoDec);
		VOLOGE ("It could not connect the file source and video decoder components. The result is 0X%08X.", errType);
		return NULL;
	}

	return pVideoDec;
}

voCOMXBaseBox * voCOMXBaseGraph::TryConnectVideoEnc (voCOMXBaseHole * pOutputHole, OMX_VIDEO_CODINGTYPE nCoding, OMX_U32 nBitrate, OMX_U32 xFramerate, OMX_U32 nQuality, OMX_U32 nTimes, OMX_BOOL& bExit)
{
	bExit = OMX_FALSE;

	OMX_STRING pVideoCompName = m_cfgComp.GetCodecCompNameByCoding (OMX_PortDomainVideo, 1, nCoding, nTimes);
	if (pVideoCompName == NULL)
	{
		VOLOGE ("It could not find the video encoder component at times %d.", (int)nTimes);
		return NULL;
	}

	if(!strcmp(pVideoCompName, "OMX.VisualOn.Forbid"))
	{
		bExit = OMX_TRUE;
		return NULL;
	}

	voCOMXBaseBox * pVideoEnc = m_pChain->AddBox (pVideoCompName);
	if (pVideoEnc == NULL)
	{
		VOLOGE ("It could not add the video encoder component %s.", pVideoCompName);
		return NULL;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	// set the role for component
//	if (strstr (pVideoCompName, "VisualOn") == NULL)
	{
		char szVideoEncRole[128];
		if (nCoding == OMX_VIDEO_CodingH263)
			strcpy (szVideoEncRole, "video_encoder.h263");
		else if (nCoding == OMX_VIDEO_CodingMPEG4)
			strcpy (szVideoEncRole, "video_encoder.mpeg4");
		else if (nCoding == OMX_VIDEO_CodingAVC)
			strcpy (szVideoEncRole, "video_encoder.avc");
		else
			strcpy (szVideoEncRole, "video_encoder.unknown");

		OMX_U8 szRole[OMX_MAX_STRINGNAME_SIZE];
		OMX_STRING pRole = (OMX_STRING)szRole;
		OMX_U32 nIndex = 0;
		errType = OMX_ErrorNone;
		
		if((!strncmp(pVideoCompName,  "OMX.Freescale.std.",18))
		   && strncmp(szVideoEncRole,"video_decoder.unknown", 21))
		{
			OMX_PARAM_COMPONENTROLETYPE rolType;
			pVideoEnc->SetHeadder (&rolType, sizeof (OMX_PARAM_COMPONENTROLETYPE));
			memcpy (rolType.cRole, szVideoEncRole, OMX_MAX_STRINGNAME_SIZE);
			errType = pVideoEnc->SetParameter (OMX_IndexParamStandardComponentRole, &rolType);
		}
		else
		{
			while (errType == OMX_ErrorNone && pVideoEnc->GetComponent ()->ComponentRoleEnum != NULL)
			{
				memset (szRole, 0, sizeof(szRole));
				errType = pVideoEnc->GetComponent ()->ComponentRoleEnum (pVideoEnc->GetComponent (), szRole, nIndex++);
				if (errType != OMX_ErrorNone)
					break;

				if (!strcmp (pRole, szVideoEncRole))
				{
					OMX_PARAM_COMPONENTROLETYPE rolType;
					pVideoEnc->SetHeadder (&rolType, sizeof (OMX_PARAM_COMPONENTROLETYPE));
					memcpy (rolType.cRole, szRole, OMX_MAX_STRINGNAME_SIZE);
					pVideoEnc->SetParameter (OMX_IndexParamStandardComponentRole, &rolType);
					break;
				}
			}
		}
	}

	pVideoEnc->CreateHoles ();
	if (pVideoEnc->GetHoleCount () != 2)
	{
		m_pChain->RemoveBox (pVideoEnc);
		VOLOGE ("The video encoder component was not filter component!");
		return NULL;
	}

	voCOMXBaseHole * pInput = pVideoEnc->GetHole (0);

	// Update the port buffer count for source component
	if (strstr (pVideoCompName, "VisualOn") == NULL)
	{
		OMX_PARAM_PORTDEFINITIONTYPE * pInputType = pInput->GetPortType ();
		OMX_PARAM_PORTDEFINITIONTYPE * pOutputType = pOutputHole->GetPortType();
		pOutputType->nBufferCountActual = pInputType->nBufferCountActual;
		pOutputType->nBufferCountMin = pInputType->nBufferCountMin;
		pOutputHole->GetParentBox ()->SetParameter (OMX_IndexParamPortDefinition, pOutputType);
		pOutputHole->FillPortType ();
	}

	errType = m_pChain->Connect (pOutputHole, pInput, m_bTunnelMode);
	if (errType != OMX_ErrorNone)
	{
		m_pChain->RemoveBox (pVideoEnc);
		VOLOGE ("It could not connect the video encoder components. The result is 0X%08X.", errType);
		return NULL;
	}

	// video encoder setting
	voCOMXBaseHole * pOutput = pVideoEnc->GetHole(1);
	pOutput->FillPortType();
	OMX_PARAM_PORTDEFINITIONTYPE * pVideoEncOutputType = pOutput->GetPortType();
	pVideoEncOutputType->format.video.nBitrate = nBitrate;
	pVideoEncOutputType->format.video.xFramerate = xFramerate;
	pVideoEnc->SetParameter(OMX_IndexParamPortDefinition, pVideoEncOutputType);
	pOutput->FillPortType();

	errType = pVideoEnc->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexVideoQuality, &nQuality);

	return pVideoEnc;
}

voCOMXBaseBox * voCOMXBaseGraph::TryConnectVideoRsz(voCOMXBaseHole * pOutputHole, OMX_U32 nWidth, OMX_U32 nHeight)
{
	voCOMXBaseBox * pVideoRsz = m_pChain->AddBox ((OMX_STRING)"OMX.VisualOn.Video.Resize");
	if (pVideoRsz == NULL)
	{
		VOLOGW ("It could not add the video resize component!");
		return NULL;
	}

	pVideoRsz->CreateHoles ();
	if (pVideoRsz->GetHoleCount () != 2)
	{
		VOLOGW ("The video encoder component was not filter component!");
		m_pChain->RemoveBox (pVideoRsz);
		return NULL;
	}

	voCOMXBaseHole * pInput = pVideoRsz->GetHole(0);
	OMX_ERRORTYPE errType = m_pChain->Connect (pOutputHole, pInput, m_bTunnelMode);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("It could not connect the video resize components. The result is 0X%08X.", errType);
		m_pChain->RemoveBox (pVideoRsz);
		return NULL;
	}

	OMX_PARAM_PORTDEFINITIONTYPE sPortType;
	voOMXBase_SetHeader (&sPortType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	sPortType.nPortIndex = 1;
	errType = pVideoRsz->GetParameter(OMX_IndexParamPortDefinition, &sPortType);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("Video resize component GetParameter OMX_IndexParamPortDefinition was failed. 0X%08X", errType);
		m_pChain->RemoveBox (pVideoRsz);
		return NULL;
	}

	sPortType.format.video.nFrameWidth = nWidth;
	sPortType.format.video.nFrameHeight = nHeight;
	errType = pVideoRsz->SetParameter(OMX_IndexParamPortDefinition, &sPortType);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("Video resize component SetParameter OMX_IndexParamPortDefinition was failed. 0X%08X", errType);
		m_pChain->RemoveBox (pVideoRsz);
		return NULL;
	}

	OMX_VIDEO_PARAM_PORTFORMATTYPE tpOutoutVideoFormat;
	voOMXBase_SetHeader (&tpOutoutVideoFormat, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE));
	tpOutoutVideoFormat.nIndex = 0;
	tpOutoutVideoFormat.nPortIndex = 0;
	errType = pVideoRsz->GetParameter (OMX_IndexParamVideoPortFormat, &tpOutoutVideoFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("Video resize component GetParameter OMX_IndexParamVideoPortFormat was failed. 0X%08X", errType);
		m_pChain->RemoveBox (pVideoRsz);
		return NULL;
	}

	OMX_COLOR_FORMATTYPE eColorFormat = tpOutoutVideoFormat.eColorFormat;

	tpOutoutVideoFormat.nPortIndex = 1;
	errType = pVideoRsz->GetParameter (OMX_IndexParamVideoPortFormat, &tpOutoutVideoFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("Video resize component GetParameter OMX_IndexParamVideoPortFormat was failed. 0X%08X", errType);
		m_pChain->RemoveBox (pVideoRsz);
		return NULL;
	}

	tpOutoutVideoFormat.eColorFormat = eColorFormat;
	errType = pVideoRsz->SetParameter (OMX_IndexParamVideoPortFormat, &tpOutoutVideoFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("Video resize component SetParameter OMX_IndexParamVideoPortFormat was failed. 0X%08X", errType);
		m_pChain->RemoveBox (pVideoRsz);
		return NULL;
	}
	
	return pVideoRsz;
}

voCOMXBaseHole*	voCOMXBaseGraph::AddEffectComp (voCOMXBaseHole * pOutputHole, OMX_BOOL bAudioEffect)
{
	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	voCOMXBaseHole*	pEffectHole = pOutputHole;
	OMX_STRING		pCompName = NULL;
	OMX_U32			nIndex = 0;

	while (errType == OMX_ErrorNone)
	{
		if (bAudioEffect)
			pCompName = m_cfgComp.GetCodecCompNameByMIME (OMX_PortDomainAudio, 0, (OMX_STRING)"Effect", 0 ,  nIndex++);
		else
			pCompName = m_cfgComp.GetCodecCompNameByMIME (OMX_PortDomainVideo, 0, (OMX_STRING)"Effect", 0 ,  nIndex++);
		if (pCompName == NULL)
			break;

		voCOMXBaseBox * pEffectComp = m_pChain->AddBox (pCompName);
		if (pEffectComp == NULL)
		{
			VOLOGW ("It could not add the audio effect component %s.", pCompName);
			continue;
		}
		pEffectComp->CreateHoles ();
		if (pEffectComp->GetHoleCount () != 2)
		{
			m_pChain->RemoveBox (pEffectComp);
			VOLOGW ("The component was not effect component.");
			continue;
		}

		voCOMXBaseHole * pInput = pEffectComp->GetHole (0);
		errType = m_pChain->Connect (pOutputHole, pInput, m_bTunnelMode);
		// errType = m_pChain->Connect (pOutputHole, pInput, OMX_FALSE);
		if (errType != OMX_ErrorNone)
		{
			m_pChain->RemoveBox (pEffectComp);
			VOLOGW ("It could not connec the file source and video decoder commponents. The result is 0X%08X.", errType);
			continue;
		}

		pOutputHole = pEffectComp->GetHole (1);
		pEffectHole = pOutputHole;
	}

	return pEffectHole;
}

OMX_ERRORTYPE voCOMXBaseGraph::NotifyCallbackMsg (OMX_U32 nType, OMX_PTR pData1, OMX_PTR pData2)
{
	GRAPH_MESSAGE_TYPE * pMsg = (GRAPH_MESSAGE_TYPE *) voOMXMemAlloc (sizeof (GRAPH_MESSAGE_TYPE));
	if (pMsg != NULL)
	{
		pMsg->uType = GRAPH_MESSAGE_TYPE_CALLBACK;
		pMsg->sInfo.callback.uType = nType;
		pMsg->sInfo.callback.pData1 = pData1;
		pMsg->sInfo.callback.pData2 = pData2;

		m_tqMessage.Add (pMsg);
		m_tsMessage.Up ();
	}

	return OMX_ErrorNone;
}

OMX_U32 voCOMXBaseGraph::voGraphMessageHandleProc (OMX_PTR pParam)
{
	voCOMXBaseGraph *	pGraph = (voCOMXBaseGraph *)pParam;

#ifdef _LINUX
	voSetThreadName("GraphMsgHandle");
#endif

	pGraph->voGraphMessageHandle ();

	return 0;
}

OMX_U32 voCOMXBaseGraph::voGraphMessageHandle (void)
{
	while (!m_bMsgThreadStop)
	{
		m_tsMessage.Down ();

		GRAPH_MESSAGE_TYPE * pMsg = (GRAPH_MESSAGE_TYPE *)m_tqMessage.Remove ();
		if (pMsg != NULL)
		{
			if (pMsg->uType == GRAPH_MESSAGE_TYPE_CALLBACK)
			{
				if (pMsg->sInfo.callback.uType == VOME_CID_PLAY_FINISHED)
					VOLOGI ("VOME_CID_PLAY_FINISHED!");

				if (pMsg->sInfo.callback.uType == VOME_CID_PLAY_BUFFERING || pMsg->sInfo.callback.uType == VOME_CID_PLAY_BUFFERSTART || 
					pMsg->sInfo.callback.uType == VOME_CID_PLAY_BUFFERSTOP || pMsg->sInfo.callback.uType == VOME_CID_PLAY_DOWNLOADPOS)
				{
					if (m_pCallBack != NULL)
					{
						OMX_U32 nBuffer = (OMX_U32)pMsg->sInfo.callback.pData1;
						m_pCallBack (m_pUserData, pMsg->sInfo.callback.uType, &nBuffer, 0);
					}
				}
				else if (pMsg->sInfo.callback.uType == VOME_CID_PLAY_CODECPASSABLEERR || 
					pMsg->sInfo.callback.uType == VOME_CID_PLAY_MEDIACONTENTTYPE || 
					pMsg->sInfo.callback.uType == VOME_CID_PLAY_STREAMERROREVENT)
				{
					if (m_pCallBack)
						m_pCallBack (m_pUserData, pMsg->sInfo.callback.uType, pMsg->sInfo.callback.pData1, 0);
				}
				else if (pMsg->sInfo.callback.uType == VOME_CID_PLAY_MEDIAFOURCC || 
					pMsg->sInfo.callback.uType == VOME_CID_PLAY_STREAMBITRATETHROUGHPUT)
				{
					if (m_pCallBack)
						m_pCallBack (m_pUserData, pMsg->sInfo.callback.uType, pMsg->sInfo.callback.pData1, pMsg->sInfo.callback.pData2);
				}
				else if (pMsg->sInfo.callback.uType == VOME_CID_PLAY_ARTIMESTAMP || 
					pMsg->sInfo.callback.uType == VOME_CID_PLAY_VRTIMESTAMP ||
					pMsg->sInfo.callback.uType == VOME_CID_PLAY_AUDIOPLAYTIME)
				{
					if (m_pCallBack)
						m_pCallBack (m_pUserData, pMsg->sInfo.callback.uType, pMsg->sInfo.callback.pData1, 0);
				}
				else	//default no additional parameter
				{
					if (m_pCallBack)
						m_pCallBack (m_pUserData,pMsg->sInfo.callback.uType, 0, 0);
				}

			}
			
			voOMXMemFree (pMsg);
		}
	}

	m_hMsgThread = NULL;

	return 0;
}

OMX_BOOL voCOMXBaseGraph::InitLicenseCheck (void)
{
	if (m_pChain != NULL)
		m_pChain->m_pLibOP = m_pExtLibOP;

#ifdef _VO_NDK_CheckLicense
	if (m_pWorkPath != NULL && m_hVidDec == NULL)
	{
		OMX_VO_LIB_OPERATOR * pLibOP = m_pExtLibOP;
		if (pLibOP == NULL)
			pLibOP = &m_libOP;
#ifdef _WIN32
		m_hVidDec = (HMODULE)pLibOP->LoadLib (pLibOP->pUserData, "voVidDec.Dll", 0);
#else 
		m_hVidDec = pLibOP->LoadLib (pLibOP->pUserData, (OMX_STRING)"voVidDec.so", RTLD_FLAGS);
#endif // _WIN32

		if (m_hVidDec != NULL)
		{
			VOCHECKDLLINIT	fInit = (VOCHECKDLLINIT)pLibOP->GetAddress (pLibOP->pUserData, m_hVidDec, (OMX_STRING)("voCheckDllInit"), 0);
			m_fCheckImage = (VOCHECKDLLCHECKIMAGE)pLibOP->GetAddress (pLibOP->pUserData, m_hVidDec, (OMX_STRING)("voCheckDllCheckImage"), 0);
			m_fUninitCheck = (VOCHECKDLLUNINIT)pLibOP->GetAddress (pLibOP->pUserData, m_hVidDec,(OMX_STRING)("voCheckDllUninit"), 0);

			int i = 0;
			PLicenseBuffer pBuffer = (PLicenseBuffer)malloc (sizeof (LicenseBuffer));
			for (i = 0; i < 4; i++)
			{
				pBuffer->m_pSource[i] = (unsigned char *) malloc (1024);
				pBuffer->m_pOutput[i] = (unsigned char *) malloc (1024);
				pBuffer->m_pKey[i] = (unsigned char *) malloc (16);

				pBuffer->m_pOutput[i][128] = 16;
			}

			VO_VIDEO_BUFFER	bufImg;
			//VO_U32 nRC = fInit (&m_hLcsCheck, 0, 0, (char *)pBuffer + 20040924);
			fInit (&m_hLcsCheck, 0, 0, (char *)pBuffer + 20040924);
			
			if (m_hLcsCheck != NULL)
			{
				bufImg.ColorType = (VO_IV_COLORTYPE)20050422;
				bufImg.Buffer[0] = (VO_PBYTE) m_pWorkPath;

				m_fCheckImage (m_hLcsCheck, &bufImg, NULL);

				if (m_pLicenseText != NULL)
				{
					bufImg.ColorType = (VO_IV_COLORTYPE)20050309;
					bufImg.Buffer[0] = (VO_PBYTE) m_pLicenseText;

					m_fCheckImage (m_hLcsCheck, &bufImg, NULL);
				}
			}

			for (i = 0; i < 4; i++)
			{
				free (pBuffer->m_pSource[i]);
				free (pBuffer->m_pOutput[i]);
				free (pBuffer->m_pKey[i]);
			}
			free (pBuffer);
		}
	}
#endif // _VO_NDK_CheckLicense

	return OMX_TRUE;
}
