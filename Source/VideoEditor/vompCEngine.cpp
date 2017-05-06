/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		vompCEngine.cpp

Contains:	vompCEngine class file

Written by:	Bangfei Jin
Modify by:   Leon Huang

Change History (most recent first):
2008-04-30		JBF			Create file
2011-03-03		Leon        Modify for Linux and modify the way of loading dll 
*******************************************************************************/
#include "vompCEngine.h"

#define LOG_TAG "CVideoEditorEngine"
#include "voLog.h"

typedef VO_S32 (VO_API * VOGETVOMPFUNAPI) (VOMP_FUNCSET * pVompHandle);

vompCEngine::vompCEngine (VO_PTR hInst,VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	,m_hEnc(NULL)
{
	//LoadDll ();

}

vompCEngine::~vompCEngine ()
{
	Uninit();
}
VO_U32 vompCEngine::Init ()
{
	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;

		Uninit ();

		if (LoadLib (m_hInst) == 0)
		{
			VOLOGE ("LoadLib was failed!");
			return VO_ERR_FAILED;
		}

		nRC = m_funEnc.Init (&m_hEnc,0,NULL,this);
	return nRC;
}
VO_U32 vompCEngine::Uninit (void)
{
	VOLOGF ();

	if (m_hEnc != NULL)
	{
		m_funEnc.Uninit (m_hEnc);
		m_hEnc = NULL;
	}

	FreeLib ();

	return 0;
}
int vompCEngine::SetDataSource (void * pSource, int nFlag)
{

	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;
	return m_funEnc.SetDataSource(m_hEnc, pSource, nFlag);
//	return m_fSetSource (m_hPlay, pSource, nFlag);
}

int vompCEngine::SendBuffer (bool bVideo, VOMP_BUFFERTYPE * pBuffer)
{
	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;

	return m_funEnc.SendBuffer (m_hEnc, bVideo, pBuffer);
}

int vompCEngine::Run (void)
{
	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;

	return m_funEnc.Run(m_hEnc);
}

int vompCEngine::Pause (void)
{
	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;

	return m_funEnc.Pause(m_hEnc);
}

int vompCEngine::Stop (void)
{
	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;

	return m_funEnc.Stop(m_hEnc);
}

int vompCEngine::Flush (void)
{
	if (m_hEnc == NULL )
		return VOMP_ERR_Pointer;

	return m_funEnc.Flush(m_hEnc);
}

int vompCEngine::GetStatus (VOMP_STATUS * pStatus)
{
	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;

	return m_funEnc.GetStatus (m_hEnc, pStatus);
}

int vompCEngine::GetDuration (int * pDuration)
{
	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;

	return m_funEnc.GetDuration (m_hEnc, pDuration);
}

int vompCEngine::GetCurPos (int * pCurPos)
{
	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;

	return m_funEnc.GetCurPos(m_hEnc, pCurPos);
}

int vompCEngine::SetCurPos (int nCurPos)
{
	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;

	return m_funEnc.SetCurPos(m_hEnc, nCurPos);
}

int vompCEngine::GetParam (int nID, void * pValue)
{
	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;

	return m_funEnc.GetParam (m_hEnc, nID, pValue);
}

int vompCEngine::SetParam (int nID, void * pValue)
{
	if (m_hEnc == NULL)
		return VOMP_ERR_Pointer;

	return m_funEnc.SetParam (m_hEnc, nID, pValue);
}
//
//int vompCEngine::LoadDll (void)
//{
//	if (m_hDll != NULL)
//		FreeLibrary (m_hDll);
//
//	_tcscpy (m_szDllFile, _T("vompEngn.Dll"));
//
//	TCHAR szDll[MAX_PATH];
//	TCHAR szPath[MAX_PATH];
//	GetModuleFileName (NULL, szPath, sizeof (szPath));
//	TCHAR * pPos = _tcsrchr (szPath, _T('\\'));
//	*(pPos + 1) = 0;
//
//	_tcscpy (szDll, szPath);
//	_tcscat (szDll, m_szDllFile);
//	m_hDll = LoadLibrary (szDll);
//
//	if (m_hDll == NULL)
//		m_hDll = LoadLibrary (m_szDllFile);
//
//	if (m_hDll == NULL)
//		return 0;
//
//	m_fInit			= (VOMPINIT) GetProcAddress (m_hDll, ("vompInit"));
//	m_fSetSource	= (VOMPSETDATASOURCE) GetProcAddress (m_hDll, ("vompSetDataSource"));
//	m_fSendBuffer	= (VOMPSENDBUFFER) GetProcAddress (m_hDll, ("vompSendBuffer"));
//	m_fRun			= (VOMPRUN) GetProcAddress (m_hDll, ("vompRun"));
//	m_fPause		= (VOMPPAUSE) GetProcAddress (m_hDll, ("vompPause"));
//	m_fStop			= (VOMPSTOP) GetProcAddress (m_hDll, ("vompStop"));
//	m_fFlush		= (VOMPFLUSH) GetProcAddress (m_hDll, ("vompFlush"));
//	m_fGetStatus	= (VOMPGETSTATUS) GetProcAddress (m_hDll, ("vompGetStatus"));
//	m_fGetDuration	= (VOMPGETDURATION) GetProcAddress (m_hDll, ("vompGetDuration"));
//	m_fGetPos		= (VOMPGETCURPOS) GetProcAddress (m_hDll, ("vompGetCurPos"));
//	m_fSetPos		= (VOMPSETCURPOS) GetProcAddress (m_hDll, ("vompSetCurPos"));
//	m_fGetParam		= (VOMPGETPARAM) GetProcAddress (m_hDll, ("vompGetParam"));
//	m_fSetParam		= (VOMPSETPARAM) GetProcAddress (m_hDll, ("vompSetParam"));
//	m_fUninit		= (VOMPUNINIT) GetProcAddress (m_hDll, ("vompUninit"));
//
//	if (m_fInit == NULL)
//		return -1;
//
//	m_fInit (&m_hPlay, 0, OnListener, this);
//	if (m_hPlay == NULL)
//		return -1;
//
//	return 1;
//}

int	vompCEngine::OnListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
	vompCEngine * pPlayer = (vompCEngine *)pUserData;

	return pPlayer->HandleEvent (nID, pParam1, pParam2);
}

int	vompCEngine::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	return 0;
}


VO_U32 vompCEngine::LoadLib (VO_HANDLE hInst)
{
	VOLOGF ();

	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	if (pDllFile == NULL)
		m_bVOUsed = VO_TRUE;
//
//#if defined _WIN32
//	if (pDllFile != NULL && !m_bVOUsed)
//	{
//		memset (m_szDllFile, 0, sizeof (m_szDllFile));
//		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));
//	}
//	vostrcat (m_szDllFile, _T(".Dll"));
//
//	if (pApiName != NULL && !m_bVOUsed)
//	{
//		memset (m_szAPIName, 0, sizeof (m_szAPIName));
//		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
//	}

	vostrcpy (m_szDllFile, _T("vompEngn"));
	vostrcpy (m_szAPIName, _T("vompGetFuncSet"));
#if defined _WIN32
	vostrcat (m_szDllFile, _T(".Dll"));
#elif defined LINUX
// 	if (pDllFile != NULL && !m_bVOUsed)
// 		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".so"));
	//vostrcpy (m_szDllFile, _T("libvompEngn.so"));
//	if (pApiName != NULL && !m_bVOUsed)
	//	vostrcpy (m_szAPIName, pApiName);
#endif

	VOLOGI ("File %s, API %s .", m_szDllFile, m_szAPIName);

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOGETVOMPFUNAPI pAPI = (VOGETVOMPFUNAPI) m_pAPIEntry;
	pAPI (&m_funEnc);

	return 1;
}
