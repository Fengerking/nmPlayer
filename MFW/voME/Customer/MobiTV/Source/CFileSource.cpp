	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#ifdef _LINUX
#include <dlfcn.h>
#endif // _LINUX

#include "CFileSource.h"

#include "voLog.h"

typedef VO_S32 (VO_API * VOGETFILEREADAPI) (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag);

CFileSource::CFileSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP)
	: m_hInst (hInst)
	, m_hFile (NULL)
	, m_bForceClosed (VO_FALSE)
	, m_pFirstSample (NULL)
{
	memset (&m_Source, 0, sizeof (VO_FILE_SOURCE));

	m_paramOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR | VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE;
	m_paramOpen.pSource = &m_Source;
	m_paramOpen.pSourceOP = pFileOP;
	m_paramOpen.pMemOP = pMemOP;
}

CFileSource::~CFileSource ()
{
	CloseSource ();

	if (m_pFirstSample != NULL)
		delete []m_pFirstSample;

	FreeLib ();
}

VO_U32 CFileSource::LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	return VO_ERR_NONE;
}

VO_U32 CFileSource::CloseSource (void)
{
	return VO_ERR_NONE;
}

VO_U32 CFileSource::GetSourceInfo (VO_PTR pSourceInfo)
{
	VOLOGF ();

	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_SOURCE_INFO * pFileInfo = (VO_SOURCE_INFO *)pSourceInfo;

	return 	m_funFileRead.GetSourceInfo (m_hFile, pFileInfo);
}

VO_U32 CFileSource::GetTrackInfo (VO_U32 nTrack, VO_PTR pTrackInfo)
{
	VOLOGF ();

	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funFileRead.GetTrackInfo (m_hFile, nTrack, (VO_SOURCE_TRACKINFO *)pTrackInfo);

	return nRC;
}

VO_U32 CFileSource::GetAudioFormat (VO_U32 nTrack, VO_AUDIO_FORMAT * pAudioFormat)
{
	VOLOGF ();

	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funFileRead.GetTrackParam (m_hFile, nTrack, VO_PID_AUDIO_FORMAT, pAudioFormat);

	//permit 8 bit!!
	if(8 != pAudioFormat->SampleBits)
		pAudioFormat->SampleBits = 16;
 
	return nRC;
}

VO_U32 CFileSource::GetVideoFormat (VO_U32 nTrack, VO_VIDEO_FORMAT * pVideoFormat)
{
	VOLOGF ();

	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funFileRead.GetTrackParam (m_hFile, nTrack, VO_PID_VIDEO_FORMAT, pVideoFormat);

	return nRC;
}

VO_U32 CFileSource::GetFrameType (VO_U32 nTrack)
{
	return 0;
}

VO_U32 CFileSource::GetTrackData (VO_U32 nTrack, VO_PTR pTrackData)
{
	return 0;
}

VO_U32 CFileSource::SetTrackPos (VO_U32 nTrack, VO_S64 * pPos)
{
	return 0;
}

VO_U32 CFileSource::GetSourceParam (VO_U32 nID, VO_PTR pValue)
{
	VOLOGF ();

	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funFileRead.GetSourceParam (m_hFile, nID, pValue);
}

VO_U32 CFileSource::SetSourceParam (VO_U32 nID, VO_PTR pValue)
{
	VOLOGF ();

	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funFileRead.SetSourceParam (m_hFile, nID, pValue);
}

VO_U32 CFileSource::GetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue)
{
	VOLOGF ();

	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funFileRead.GetTrackParam (m_hFile, nTrack, nID, pValue);

	return nRC;
}

VO_U32 CFileSource::SetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue)
{
	VOLOGF ();

	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funFileRead.SetTrackParam (m_hFile, nTrack, nID, pValue);
}

VO_U32 CFileSource::Start(void)
{
	return 0;
}

VO_U32 CFileSource::Pause(void)
{
	return 0;
}

VO_U32 CFileSource::Stop(void)
{
	return 0;
}

VO_U32 CFileSource::SetCallBack (VO_PTR pCallBack, VO_PTR pUserData)
{
	return 0;
}

VO_U32 CFileSource::LoadLib (VO_HANDLE hInst)
{
	FreeLib ();

	vostrcpy (m_szDllFile, _T("voSrcRTSP"));
	vostrcpy (m_szAPIName, _T("voGetRTSPReadAPI"));

#if defined _WIN32
	vostrcat (m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat (m_szDllFile, _T(".so"));
#endif

#ifdef _WIN32
	VO_TCHAR szDll[MAX_PATH];
	VO_TCHAR szPath[MAX_PATH];
	GetModuleFileName ((HMODULE)hInst, szPath, sizeof (szPath));

	VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
	*(pPos + 1) = 0;

	vostrcpy (szDll, szPath);
	vostrcat (szDll, m_szDllFile);
	m_hDll = LoadLibrary (szDll);

	if (m_hDll == NULL)
	{
		::GetModuleFileName (NULL, szPath, sizeof (szPath));
		VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
		*(pPos + 1) = 0;

		vostrcpy (szDll, szPath);
		vostrcat (szDll, m_szDllFile);
		m_hDll = LoadLibrary (szDll);
	}

	if (m_hDll == NULL)
	{
		vostrcpy (szDll, szPath);
		vostrcat (szDll, _T("Dll\\"));
		vostrcat (szDll, m_szDllFile);
		m_hDll = LoadLibrary (szDll);
	}

	if (m_hDll == NULL)
		m_hDll = LoadLibrary (m_szDllFile);

#elif defined _LINUX
	VO_TCHAR libname[128];
	vostrcpy(libname, m_szDllFile);

	vostrcpy(m_szDllFile, "/data/local/voOMXPlayer/lib/");
	vostrcat(m_szDllFile, libname);
	m_hDll = dlopen (m_szDllFile, RTLD_NOW);
	if (m_hDll == NULL)
	{
		vostrcpy(m_szDllFile, "/data/local/voOMXPlayer/lib/lib");
		vostrcat(m_szDllFile, libname);
		m_hDll = dlopen (m_szDllFile, RTLD_NOW);
	}
	if (m_hDll == NULL)
	{
		vostrcpy (m_szDllFile, libname);
		m_hDll = dlopen (m_szDllFile, RTLD_NOW);
		if (m_hDll == NULL)
		{
			vostrcpy(m_szDllFile, "lib");
			vostrcat(m_szDllFile, libname);
			m_hDll = dlopen (m_szDllFile, RTLD_NOW);
		}
	}
	if (m_hDll == NULL)
	{
		vostrcpy(m_szDllFile, "/data/data/com.visualon.vome/lib/");
		vostrcat(m_szDllFile, libname);
		m_hDll = dlopen (m_szDllFile, RTLD_NOW);
		if (m_hDll == NULL)
		{
			vostrcpy(m_szDllFile, "/data/data/com.visualon.vome/lib/lib");
			vostrcat(m_szDllFile, libname);
			m_hDll = dlopen (m_szDllFile, RTLD_NOW);
		}
	}
#endif // _WIN32

	if (m_hDll == NULL)
	{
#ifdef LINUX
		VOLOGE ("Load module %s was failed. Error: %s", m_szDllFile, dlerror());
#endif // _LINUX
		return 0;
	}
	VOLOGI ("Load module %s!", m_szDllFile);

	if (vostrlen (m_szAPIName) > 0)
	{
#ifdef _WIN32
#ifdef _WIN32_WCE
		m_pAPIEntry = GetProcAddress (m_hDll, m_szAPIName);
#else
		char szAPI[128];
		memset (szAPI, 0, 128);
		WideCharToMultiByte (CP_ACP, 0, m_szAPIName, -1, szAPI, 128, NULL, NULL);
		m_pAPIEntry = GetProcAddress (m_hDll, szAPI);
#endif // _WIN32_WCE
#elif defined _LINUX
		m_pAPIEntry = dlsym (m_hDll, m_szAPIName);
#endif // _WIN32
	}

	if (m_pAPIEntry == NULL)
	{
#ifdef LINUX
		VOLOGE ("Get function %s was failed. Error: %s", m_szAPIName, dlerror());
#endif // _LINUX
		return 0;
	}

	VOGETFILEREADAPI pAPI = (VOGETFILEREADAPI) m_pAPIEntry;
	pAPI (&m_funFileRead, 0);

	return 1;
}

VO_U32 CFileSource::FreeLib (void)
{
	VOLOGF ();

	if (m_hDll != NULL)
#ifdef _WIN32
		FreeLibrary (m_hDll);
#elif defined _LINUX
		dlclose (m_hDll);
#endif // _WIN32

	if (m_hDll != NULL)
		VOLOGI ("Free module %s!", m_szDllFile);
	m_hDll = NULL;

	return VO_ERR_NONE;
}


