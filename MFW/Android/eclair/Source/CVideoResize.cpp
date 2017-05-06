	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioResample.cpp

	Contains:	CAudioResample class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#if defined LINUX
#include <dlfcn.h>
#include "voOSFunc.h"
#endif

#include "CVideoResize.h"

#define LOG_TAG "CVideoResize"
#include "voLog.h"

//#pragma warning (disable : 4996)

CVideoResize::CVideoResize(void)
	: m_hResize (NULL)
{
	LoadDll ();

	memset (&m_ccInfo, 0, sizeof (m_ccInfo));
}

CVideoResize::~CVideoResize ()
{
	Release ();
}

OMX_U32 CVideoResize::ResizeBuffer (ClrConvData * pVideoInfo)
{
	if (m_hResize == NULL)
		return -1;

	if (m_ccInfo.nInWidth == 0)
	{
		memcpy (&m_ccInfo, pVideoInfo, sizeof (m_ccInfo));
		m_rsAPI.CCSetParam (m_hResize, VO_PID_CC_INIINSTANCE, &m_ccInfo);
	}

	m_rsAPI.CCProcess (m_hResize, pVideoInfo);

	return 0;
}

typedef VO_S32 (VO_API * VOVIDEORESIZEGEEAPI) (VO_CLRCONV_DECAPI * pFunc, OMX_U32 uFlag);

OMX_U32 CVideoResize::LoadDll (void)
{
	vostrcpy(m_szDllFile, _T("voColorConversion"));
	vostrcpy(m_szAPIName, _T("voGetClrConvAPI"));

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, _T(".so"));
#endif

	if(CDllLoad::LoadLib(NULL) == 0)
	{
		VOLOGI ("LoadLib fail");
		return -1;
	}

	VOVIDEORESIZEGEEAPI pAPI = (VOVIDEORESIZEGEEAPI) m_pAPIEntry;
	if (pAPI == NULL)
		return -1;

	pAPI (&m_rsAPI, 0);

	VO_CODEC_INIT_USERDATA	initInfo;
	//initInfo.memData = &m_memVideo;
	memset (&initInfo, 0, sizeof (VO_CODEC_INIT_USERDATA));
	initInfo.memflag = VO_IMF_USERMEMOPERATOR;
	initInfo.memData = NULL;

	m_rsAPI.CCInit (&m_hResize, NULL);//&initInfo);

	return 1;
}

OMX_U32 CVideoResize::Release (void)
{
	if (m_hResize != NULL)
	{
		m_rsAPI.CCUninit (m_hResize);
		m_hResize = NULL;
	}

	return 1;
}
