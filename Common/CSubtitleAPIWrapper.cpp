/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CSubtitleAPIWrapper.cpp

Contains:	CSubtitleAPIWrapper class file

Written by:	 Tom Yu Wei

Change History (most recent first):
2010-11-11		YW			Create file

*******************************************************************************/

#define LOG_TAG "CSubtitleAPIWrapper"

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include "CSubtitleAPIWrapper.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CSubtitleAPIWrapper::CSubtitleAPIWrapper(void)
: m_hSubtitle(NULL)
, m_hDll(NULL)
{	
}

CSubtitleAPIWrapper::~CSubtitleAPIWrapper(void)
{
	Release();
}

void CSubtitleAPIWrapper::Release()
{
	if (m_hSubtitle != NULL)
	{
		m_APISubtitle.vostUninit(m_hSubtitle);
		m_hSubtitle = NULL;
	}

#ifdef _WIN32
	if (m_hDll != NULL)
		FreeLibrary ((HMODULE)m_hDll);
#endif // _WIN32
#if defined LINUX
	if (m_hDll != NULL)
		dlclose(m_hDll);
#endif
	m_hDll = NULL;
}

int CSubtitleAPIWrapper::Init (char* chPath, void* jniEnv)
{
	Release();

	if (LoadDll (chPath) == -1)
		return Subtitle_ErrorUnknown;

	m_APISubtitle.vostInit(&m_hSubtitle);
	if (m_hSubtitle == NULL)
	{
		return Subtitle_ErrorUnknown;
	}

	m_APISubtitle.vostSetParameter(m_hSubtitle, ID_SET_JAVA_ENV, (long)jniEnv);

	return Subtitle_ErrorNone;
}


int CSubtitleAPIWrapper::LoadDll(char* chPath)
{
#ifndef _WIN32
	char szFilePath[1024];
	if(chPath == NULL)
		sprintf(szFilePath,  "/system/lib/libvo_subtitle_v22.so");
	else
		sprintf(szFilePath,  "%s/libvoSubtitleParser.so", chPath);

	m_hDll = dlopen (szFilePath, RTLD_NOW);
	if (m_hDll == NULL)
	{
		VOLOGE("Failed to open so file");
		return -1;
	}
	VOLOGI("Subtitle lib Path %s", szFilePath);
	VOGETSUBTITLE3API pAPI = (VOGETSUBTITLE3API) dlsym(m_hDll, "voGetSubtitle3API");
	pAPI (&m_APISubtitle, 0);
#endif
	return 0;
}

bool CSubtitleAPIWrapper::SetMediaFile(VO_TCHAR* szMediaFilePath)
{
	if (m_hSubtitle == NULL)
		return false;
    VOLOGI("SetMediaFile OK");
	return m_APISubtitle.vostSetMediaFile(m_hSubtitle, szMediaFilePath) > 0;
}

bool CSubtitleAPIWrapper::SetParameter(long lID, long lValue)
{
	if (m_hSubtitle == NULL)
		return false;

	return m_APISubtitle.vostSetParameter(m_hSubtitle, lID, lValue) == Subtitle_ErrorNone;
}

bool CSubtitleAPIWrapper::GetParameter(long lID, long *plValue)
{
	if (m_hSubtitle == NULL)
		return false;

	return (m_APISubtitle.vostGetParameter(m_hSubtitle, lID, plValue) == Subtitle_ErrorNone);
}
bool CSubtitleAPIWrapper::GetSubtitleObject (long nTimeCurrent, voSubtitleInfo ** pSubtitle, int nTrackIndex)
{
	if (m_hSubtitle == NULL)
		return false;
	m_APISubtitle.vostGetSubtitleObject(m_hSubtitle, nTimeCurrent, pSubtitle, nTrackIndex);
	return true;
}


