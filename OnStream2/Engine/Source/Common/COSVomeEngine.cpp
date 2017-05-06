#include "voIndex.h"

#include "vompType.h"
#include "vompAPI.h"
#include "COSVomeEngine.h"
#include "voOSFunc.h"

#define LOG_TAG "COSVomeEngine"
#include "voLog.h"

typedef int (* VOMPGETFUNCSET) (VOMP_FUNCSET * pFuncSet);

COSVomeEngine::COSVomeEngine()
	: COSBasePlayer ()
	, m_hDll(NULL)
	, m_hPlay(NULL)
	, m_pLibFunc (NULL)
{
	memset(&m_VomeFuncSet, 0, sizeof(m_VomeFuncSet));
}

COSVomeEngine::~COSVomeEngine ()
{
	Uninit();
}

int COSVomeEngine::Init()
{
	Uninit();

	int nRC = VOMP_ERR_None;
	nRC = LoadDll();
	if(nRC) return nRC;

	if(m_VomeFuncSet.Init == NULL)
		return VOMP_ERR_Pointer;
	
	if (m_VomeFuncSet.SetParam != NULL)
		m_VomeFuncSet.SetParam (NULL, VO_PID_COMMON_LOGFUNC, &m_cbVOLOG);
	nRC = m_VomeFuncSet.Init (&m_hPlay, VOMP_PLAYER_MEDIA, vomtCallBack, this);

	COSBasePlayer::Init();
	
	return nRC;		
}

int COSVomeEngine::Uninit()
{
	if(m_hPlay != NULL && m_VomeFuncSet.Uninit != NULL) 
	{
		m_VomeFuncSet.Uninit (m_hPlay);
		m_hPlay = NULL;
	}

	if(m_hDll != NULL && m_pLibFunc != NULL)
	{
		m_pLibFunc->FreeLib (m_pLibFunc->pUserData, m_hDll, 0);
		memset(&m_VomeFuncSet, 0, sizeof(m_VomeFuncSet));
		m_hDll = 0;
	}

	 return COSBasePlayer::Uninit();
}

int COSVomeEngine::SetDataSource (void * pSource, int nFlag)
{
	if (m_hPlay == 0 || m_VomeFuncSet.SetDataSource == 0)
		return VOMP_ERR_Pointer;

	int nRC = COSBasePlayer::SetDataSource (pSource, nFlag);
	if(nRC) return VOMP_ERR_Status;

	m_nChanging = 1;	
	nRC = m_VomeFuncSet.SetDataSource (m_hPlay, pSource, nFlag);
	m_nChanging = 0;

	return nRC;
}

int COSVomeEngine::SetView(void* pView)
{
	return VOMP_ERR_Implement; 
}

int COSVomeEngine::SetJavaVM(void *pJavaVM, void* obj)
{
	return VOMP_ERR_Implement; 
}

void* COSVomeEngine::GetJavaObj()
{
	return 0; 
}

int COSVomeEngine::GetVideoBuffer(VOMP_BUFFERTYPE ** ppBuffer)
{
	if (m_hPlay == 0 || m_VomeFuncSet.GetVideoBuffer == 0)
		return VOMP_ERR_Pointer;
	
	return m_VomeFuncSet.GetVideoBuffer (m_hPlay, ppBuffer);
}

int COSVomeEngine::GetAudioBuffer(VOMP_BUFFERTYPE ** ppBuffer)
{
	if (m_hPlay == 0 || m_VomeFuncSet.GetAudioBuffer == 0)
		return VOMP_ERR_Pointer;
	
	return m_VomeFuncSet.GetAudioBuffer (m_hPlay, ppBuffer);
}

int COSVomeEngine::SendBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer)
{
	if(m_hPlay == 0 || m_VomeFuncSet.SendBuffer == 0)
		return VOMP_ERR_Pointer; 

	return m_VomeFuncSet.SendBuffer (m_hPlay, nSSType, pBuffer);
}

int COSVomeEngine::Run (void)
{
	if (m_hPlay == 0 || m_VomeFuncSet.Run == 0)
		return VOMP_ERR_Pointer;

	int nRC = COSBasePlayer::Run ();
	if(nRC) return VOMP_ERR_Status;

	m_nChanging = 1;		
	nRC = m_VomeFuncSet.Run (m_hPlay);
	m_nChanging = 0;

	return nRC;
}

int COSVomeEngine::Pause (void)
{
	if (m_hPlay == 0 || m_VomeFuncSet.Pause == 0)
		return VOMP_ERR_Pointer;

	m_nChanging = 1;
	int nRC = m_VomeFuncSet.Pause (m_hPlay);
	m_nChanging = 0;

	nRC |= COSBasePlayer::Pause ();

	return nRC;

}

int COSVomeEngine::Stop (void)
{
	if (m_hPlay == 0 || m_VomeFuncSet.Stop == 0)
		return VOMP_ERR_Pointer;

	m_nChanging = 1;
	int nRC = m_VomeFuncSet.Stop (m_hPlay);
	m_nChanging = 0;

	nRC |= COSBasePlayer::Stop ();

	return nRC;
}

int COSVomeEngine::Flush (void)
{
	if (m_hPlay == 0 || m_VomeFuncSet.Flush == 0)
		return VOMP_ERR_Pointer;

	m_nChanging = 1;
	int nRC = m_VomeFuncSet.Flush (m_hPlay);
	m_nChanging = 0;

	nRC |= COSBasePlayer::Flush ();

	return nRC;
}

int COSVomeEngine::GetStatus (int * pStatus)
{
	if (m_hPlay == 0 || m_VomeFuncSet.GetStatus == 0)
		return VOMP_ERR_Pointer;

	VOMP_STATUS	voStatus;
	int nRC = m_VomeFuncSet.GetStatus (m_hPlay, &voStatus);
	*pStatus = (int)voStatus;
	return nRC;
}

int COSVomeEngine::GetDuration (int * pDuration)
{
	if (m_hPlay == 0 || m_VomeFuncSet.GetDuration == 0)
		return VOMP_ERR_Pointer;

	return m_VomeFuncSet.GetDuration (m_hPlay, pDuration);
}

int COSVomeEngine::GetPos (int * pCurPos)
{
	if (m_hPlay == 0 || m_VomeFuncSet.GetCurPos == 0)
		return VOMP_ERR_Pointer;

	return m_VomeFuncSet.GetCurPos (m_hPlay, pCurPos);
}

int COSVomeEngine::SetPos (int nCurPos)
{
	if (m_hPlay == 0 || m_VomeFuncSet.SetCurPos == 0)
		return VOMP_ERR_Pointer;

	int nRC = m_VomeFuncSet.SetCurPos (m_hPlay, nCurPos);

	nRC |= COSBasePlayer::SetPos (nCurPos);
	
	return nRC;
}

int COSVomeEngine::GetParam (int nID, void * pValue)
{
	if (m_hPlay == 0 || m_VomeFuncSet.GetParam == 0)
		return VOMP_ERR_Pointer;

	return m_VomeFuncSet.GetParam (m_hPlay, nID, pValue);
}

int COSVomeEngine::SetParam (int nID, void * pValue)
{
	if (m_hPlay == 0 || m_VomeFuncSet.SetParam == 0)
		return VOMP_ERR_Pointer;
	
	return m_VomeFuncSet.SetParam (m_hPlay, nID, pValue);
}

int COSVomeEngine::LoadDll (void)
{
	if  (!m_pLibFunc) return VOMP_ERR_Pointer;

	if (m_hDll != 0) 
	{
		m_pLibFunc->FreeLib (m_pLibFunc->pUserData, m_hDll, 0);
		memset(&m_VomeFuncSet, 0, sizeof(m_VomeFuncSet));
	}

	m_hDll = m_pLibFunc->LoadLib(m_pLibFunc->pUserData, "vompEngn" , 0 );
	if (!m_hDll) 
	{
		return VOMP_ERR_Unknown;
	}

	VOMPGETFUNCSET	pGetFuncSet = NULL;

	pGetFuncSet	= (VOMPGETFUNCSET) m_pLibFunc->GetAddress(m_pLibFunc->pUserData, m_hDll, ("vompGetFuncSet"), 0);
	
	if(pGetFuncSet == NULL)	
		return VOMP_ERR_Pointer;

	pGetFuncSet(&m_VomeFuncSet);

	if (m_VomeFuncSet.Init == 0) 
		return VOMP_ERR_Pointer;

	return VOMP_ERR_None;
}

int COSVomeEngine::vomtCallBack (void * pUserData, int event_id, void * pParam1, void * pParam2)
{
	COSVomeEngine * pPlayer = (COSVomeEngine *)pUserData;

	voCAutoLock lock( &pPlayer->m_EventLock );

	int nEvent = pPlayer->ChangEventIDEvent(event_id, pParam1, pParam2);

  if(VOOSMP_CB_PCM_OUTPUT == nEvent && NULL != pParam2)
  {
    if(VOMP_FLAG_BUFFER_NEW_FORMAT == *((int*)pParam1)){
      return 0; // audio format changed;
    }

    VOMP_BUFFERTYPE * VOMP_PCM_Buffer = (VOMP_BUFFERTYPE*)pParam2;
    VOOSMP_PCMBUFFER VOOSMP_PCM_Buffer;
    memset(&VOOSMP_PCM_Buffer, 0, sizeof(VOOSMP_PCMBUFFER));
    VOOSMP_PCM_Buffer.nTimestamp = VOMP_PCM_Buffer->llTime;
    VOOSMP_PCM_Buffer.nBufferSize = VOMP_PCM_Buffer->nSize;
    VOOSMP_PCM_Buffer.pBuffer = VOMP_PCM_Buffer->pBuffer;

    return pPlayer->HandleEvent(nEvent, &VOOSMP_PCM_Buffer, NULL);
  }

	return pPlayer->HandleEvent(nEvent, pParam1, pParam2);
}

int	COSVomeEngine::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	COSBasePlayer::HandleEvent(nID, pParam1, pParam2);
	return 0;
}