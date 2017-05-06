#include "COMXALEngine.h"
#include "voOSFunc.h"
#include <string.h>

#define LOG_TAG "COMXALEngine"
#include "voLog.h"

COMXALEngine::COMXALEngine()
:m_hDll(NULL)
,m_hWrap(NULL)
,m_pLibFunc (NULL)
,m_pbVOLOG(NULL)
,m_pPlayerPath(NULL)
{
	memset(&m_NPFuncSet, 0, sizeof(m_NPFuncSet));
}

COMXALEngine::~COMXALEngine()
{
	if(m_hDll != NULL && m_pLibFunc != NULL)
	{
		m_pLibFunc->FreeLib (m_pLibFunc->pUserData, m_hDll, 0);
		memset(&m_NPFuncSet, 0, sizeof(m_NPFuncSet));
		m_hDll = NULL;
	}
}

int COMXALEngine::Init(void* pInitParam)
{
	m_pLibFunc = (VONP_LIB_FUNC*)pInitParam;
	
	int nRet = LoadDll();
	
	if(VONP_ERR_None != nRet)
	{
		return nRet;
	}

	nRet = m_NPFuncSet.SetParam(m_hWrap, VONP_PID_COMMON_LOGFUNC, m_pbVOLOG);
	nRet = m_NPFuncSet.SetParam(m_hWrap, VONP_PID_PLAYER_PATH, m_pPlayerPath);
	
	nRet = m_NPFuncSet.Init(&m_hWrap, pInitParam);
	
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return nRet;
}

int COMXALEngine::Uninit()
{
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	int nRet =  m_NPFuncSet.Uninit(m_hWrap);
	m_hWrap = NULL;
	return nRet;
}

int COMXALEngine::SetView(void* pView)
{
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return m_NPFuncSet.SetView(m_hWrap, pView);
}

int COMXALEngine::Open(void* pSource, int nFlag)
{
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return m_NPFuncSet.Open(m_hWrap, pSource, nFlag);
}

int COMXALEngine::Close()
{
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return m_NPFuncSet.Close(m_hWrap);
}

int COMXALEngine::Run()
{
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return m_NPFuncSet.Run(m_hWrap);
}

int COMXALEngine::Pause()
{
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return m_NPFuncSet.Pause(m_hWrap);
}

int COMXALEngine::Stop()
{
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return m_NPFuncSet.Stop(m_hWrap);
}

int COMXALEngine::GetPos()
{
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return m_NPFuncSet.GetPos(m_hWrap);
}

int COMXALEngine::SetPos(int nPos)
{
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return m_NPFuncSet.SetPos(m_hWrap, nPos);
}

int COMXALEngine::GetParam(int nParamID, void* pValue)
{
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return m_NPFuncSet.GetParam(m_hWrap, nParamID, pValue);
}

int COMXALEngine::SetParam(int nParamID, void* pValue)
{
	if(nParamID == VONP_PID_COMMON_LOGFUNC)
	{
		m_pbVOLOG = (VO_LOG_PRINT_CB *)pValue;
		return VONP_ERR_None;
	}
  else if(VONP_PID_PLAYER_PATH == nParamID)
  {
    m_pPlayerPath = (VO_TCHAR*)pValue;
		return VONP_ERR_None;
  }
	
	if(!m_hWrap)
		return VONP_ERR_Pointer;
	
	return m_NPFuncSet.SetParam(m_hWrap, nParamID, pValue);
}

int COMXALEngine::LoadDll (void)
{
	if (!m_pLibFunc)
		return VONP_ERR_Pointer;
	
	if (m_hDll != 0) 
	{
		m_pLibFunc->FreeLib (m_pLibFunc->pUserData, m_hDll, 0);
		memset(&m_NPFuncSet, 0, sizeof(m_NPFuncSet));
	}
	
	m_hDll = m_pLibFunc->LoadLib(m_pLibFunc->pUserData, (char*)"voOMXALWrap" , 0 );
	if (!m_hDll) 
	{
		return VONP_ERR_Unknown;
	}
	
	VONPWRAPPERAPI	pGetFuncSet = NULL;
#ifdef _IOS
    pGetFuncSet	= (VONPWRAPPERAPI) m_pLibFunc->GetAddress(m_pLibFunc->pUserData, m_hDll, (char*)("voGetHLSWrapperAPI"), 0);
#else
	pGetFuncSet	= (VONPWRAPPERAPI) m_pLibFunc->GetAddress(m_pLibFunc->pUserData, m_hDll, (char*)("voOMXALWrapperAPI"), 0);
#endif
    
	if(pGetFuncSet == NULL)	
		return VONP_ERR_Pointer;
	
	pGetFuncSet(&m_NPFuncSet);
	
	if (m_NPFuncSet.Init == 0) 
		return VONP_ERR_Pointer;
	
	return VONP_ERR_None;
}
