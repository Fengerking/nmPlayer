	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCEditPlayer.cpp

	Contains:	voCEditPlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifdef __SYMBIAN32__
#include <stdlib.h>
#elif defined _IOS
#include <stdlib.h>
#elif defined _MAC_OS
#include <stdlib.h>
#else
#include <malloc.h>
#endif // __SYMBIAN32__

#include <string.h>
#include <stdio.h>

#include "voOSFunc.h"
#include "voCEditPlayer.h"

#include "voCMediaPlayer.h"

#define LOG_TAG "voCEditPlayer"
#include "voLog.h"

voCEditPlayer::voCEditPlayer(VOMPListener pListener, void * pUserData)
	: voCBasePlayer (pListener, pUserData)
	, m_ppItems (NULL)
	, m_ppURL (NULL)
	, m_ppPlayer (NULL)
	, m_nIndex (0)
	, m_bNextReady (false)
	, m_nDuration (0)
	, m_bAudioNewFile (false)
	, m_bVideoNewFile (false)
	, m_bAudioInLoop (true)
	, m_bVideoInLoop (true)
	, m_nSeekMode (0)
	, m_fThreadCreate (NULL)
	, m_pLibFunc (NULL)
	, m_pDrmCB (NULL)
	, m_nDisableDropVideoFrame (0)
	, m_nCheckFastPerformance (0)
	, m_nGetThumbnail (0)
	, m_nColorType (VOMP_COLOR_YUV_PLANAR420)
{
	memset (&m_sSource, 0, sizeof (VOMP_EDITOR_SOURCE));
}

voCEditPlayer::~voCEditPlayer(void)
{
	if (m_ppPlayer != NULL)
	{
		for (int i = 0; i < m_sSource.nCount; i++)
		{
			if (m_ppPlayer[i] != NULL)
			{
				m_ppPlayer[i]->Stop ();
				delete m_ppPlayer[i];
				m_ppPlayer[i] = NULL;
			}
		}

		delete []m_ppPlayer;
		m_ppPlayer = NULL;
	}

	if (m_ppItems != NULL)
	{
		for (int i = 0; i < m_sSource.nCount; i++)
		{
			if (m_ppItems[i] != NULL)
			{
				delete m_ppItems[i];
				m_ppItems[i] = NULL;
			}
		}

		delete []m_ppItems;
		m_ppItems = NULL;
	}

	if (m_ppURL != NULL)
	{
		for (int i = 0; i < m_sSource.nCount; i++)
		{
			if (m_ppURL[i] != NULL)
			{
				delete m_ppURL[i];
				m_ppURL[i] = NULL;
			}
		}

		delete []m_ppURL;
		m_ppURL = NULL;
	}
}

int voCEditPlayer::SetDataSource (void * pSource, int nFlag)
{
	CheckSource ((VOMP_EDITOR_SOURCE *)pSource);

	if (m_sSource.nCount <= 0)
		return VOMP_ERR_ParamID;

	if (CreatePlayer (0) < 0)
		return VOMP_ERR_OutMemory;

	m_nIndex = 0;
	int nSourceFlag = (m_ppItems[m_nIndex]->nFlag & 0XFFFFFFF0) | VOMP_FLAG_SOURCE_SYNC;

	int nRC = m_ppPlayer[m_nIndex]->SetDataSource (m_ppItems[m_nIndex]->pSource, nSourceFlag);

	m_ppPlayer[m_nIndex]->SetParam (VOMP_PID_START_POS, &m_ppItems[m_nIndex]->nStartPos);
	m_ppPlayer[m_nIndex]->SetParam (VOMP_PID_STOP_POS, &m_ppItems[m_nIndex]->nStopPos);

	m_ppPlayer[m_nIndex]->GetParam (VOMP_PID_AUDIO_FORMAT, &m_sAudioFormat);
	m_ppPlayer[m_nIndex]->GetParam (VOMP_PID_VIDEO_FORMAT, &m_sVideoFormat);

	return nRC;
}

int voCEditPlayer::GetVideoBuffer (VOMP_BUFFERTYPE ** ppBuffer)
{
	m_bVideoInLoop = true;
	voCAutoLock lock (&m_mtPlayer);

	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;
	if (m_ppPlayer[m_nIndex] == NULL)
		return VOMP_ERR_Status;

	if (m_bVideoNewFile)
	{
		m_bVideoNewFile = false;
		VO_VIDEO_FORMAT	sVideoFormat;
		memset (&sVideoFormat, 0, sizeof (VO_VIDEO_FORMAT));
		m_ppPlayer[m_nIndex]->GetParam (VOMP_PID_VIDEO_FORMAT, &sVideoFormat);
		if (memcmp (&sVideoFormat, &m_sVideoFormat, sizeof (VO_VIDEO_FORMAT)))
		{
			m_ppPlayer[m_nIndex]->GetParam (VOMP_PID_VIDEO_FORMAT, &m_sVideoFormat);
			return VOMP_ERR_FormatChange;
		}
	}

	int nRC = m_ppPlayer[m_nIndex]->GetVideoBuffer (ppBuffer);
	m_bVideoInLoop = false;

	if (nRC == VOMP_ERR_EOS)
	{
		return VOMP_ERR_Retry;
	}

	return nRC;
}

int voCEditPlayer::GetAudioBuffer (VOMP_BUFFERTYPE ** ppBuffer)
{
	m_bAudioInLoop = true;
	voCAutoLock lock (&m_mtPlayer);

	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;
	if (m_ppPlayer[m_nIndex] == NULL)
		return VOMP_ERR_Status;

	if (m_bAudioNewFile)
	{
		m_bAudioNewFile = false;

		VO_AUDIO_FORMAT	sAudioFormat;
		memset (&sAudioFormat, 0, sizeof (VO_AUDIO_FORMAT));
		m_ppPlayer[m_nIndex]->GetParam (VOMP_PID_AUDIO_FORMAT, &sAudioFormat);
		if (memcmp (&sAudioFormat, &m_sAudioFormat, sizeof (VO_AUDIO_FORMAT)))
		{
			m_ppPlayer[m_nIndex]->GetParam (VOMP_PID_AUDIO_FORMAT, &m_sAudioFormat);
			return VOMP_ERR_FormatChange;
		}
	}

	int nRC = m_ppPlayer[m_nIndex]->GetAudioBuffer (ppBuffer);
	if (nRC == VOMP_ERR_None && *ppBuffer != NULL)
	{
		if ((*ppBuffer)->llTime + m_ppItems[m_nIndex]->nStartPos + 10000 >= m_ppItems[m_nIndex]->nStopPos && !m_bNextReady)
			LoadNextSource (false);
	}
	m_bAudioInLoop = false;

	if (nRC == VOMP_ERR_EOS)
	{
		return VOMP_ERR_Retry;
	}

	return nRC;
}

int voCEditPlayer::Run (void)
{
	voCAutoLock lock (&m_mtPlayer);

	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;
	if (m_ppPlayer[m_nIndex] == NULL)
		return VOMP_ERR_Status;

	return m_ppPlayer[m_nIndex]->Run ();
}

int voCEditPlayer::Pause (void)
{
	voCAutoLock lock (&m_mtPlayer);

	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;
	if (m_ppPlayer[m_nIndex] == NULL)
		return VOMP_ERR_Status;

	return m_ppPlayer[m_nIndex]->Pause ();
}

int voCEditPlayer::Stop (void)
{
	voCAutoLock lock (&m_mtPlayer);

	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;
	if (m_ppPlayer[m_nIndex] == NULL)
		return VOMP_ERR_Status;

	return m_ppPlayer[m_nIndex]->Stop ();
}

int voCEditPlayer::Flush (void)
{
	voCAutoLock lock (&m_mtPlayer);

	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;
	if (m_ppPlayer[m_nIndex] == NULL)
		return VOMP_ERR_Status;

	return m_ppPlayer[m_nIndex]->Flush ();
}

int voCEditPlayer::GetStatus (VOMP_STATUS * pStatus)
{
	voCAutoLock lock (&m_mtPlayer);

	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;
	if (m_ppPlayer[m_nIndex] == NULL)
		return VOMP_ERR_Status;

	return m_ppPlayer[m_nIndex]->GetStatus (pStatus);
}

int voCEditPlayer::GetDuration (int * pDuration)
{
	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;
	if (m_ppPlayer[m_nIndex] == NULL)
		return VOMP_ERR_Status;

	*pDuration = (int)m_nDuration;

	return VOMP_ERR_None;
}

int voCEditPlayer::GetCurPos (int * pCurPos)
{
	voCAutoLock lock (&m_mtPlayer);

	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;
	if (m_ppPlayer[m_nIndex] == NULL)
		return VOMP_ERR_Status;

	VO_U64 nTotalPos = 0;
	for (int i = 0; i < m_nIndex; i++)
		nTotalPos = nTotalPos + (m_ppItems[i]->nStopPos - m_ppItems[i]->nStartPos);

	int nCurPos = 0;
	int nRC = m_ppPlayer[m_nIndex]->GetCurPos (&nCurPos);

	*pCurPos = (int)(nTotalPos + (VO_U64)nCurPos);

	return nRC;
}

int voCEditPlayer::SetCurPos (int nCurPos)
{
	voCAutoLock lock (&m_mtPlayer);

	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;

	VO_U64 nTotalPos = 0;
	int nIndex = 0;
	for (int i = 0; i < m_sSource.nCount; i++)
	{
		if (nCurPos >= nTotalPos && nCurPos < nTotalPos + (m_ppItems[i]->nStopPos - m_ppItems[i]->nStartPos))
			break;

		nTotalPos = nTotalPos + (m_ppItems[i]->nStopPos - m_ppItems[i]->nStartPos);
		nIndex++;
	}

	if (nIndex >= m_sSource.nCount)
		return 0;

	if (nIndex == m_nIndex)
		return m_ppPlayer[m_nIndex]->SetCurPos (nCurPos - (int)nTotalPos);

	while (!m_bVideoInLoop || !m_bAudioInLoop)
		voOS_Sleep (10);

	if (m_ppPlayer[m_nIndex] != NULL)
	{
		m_ppPlayer[m_nIndex]->CloseSource (true);
	}

	if (CreatePlayer (nIndex) < 0)
		return VOMP_ERR_OutMemory;

	m_nIndex = nIndex;
	int nSourceFlag = (m_ppItems[m_nIndex]->nFlag & 0XFFFFFFF0) | VOMP_FLAG_SOURCE_SYNC;

	int nRC = 0;

	nRC = m_ppPlayer[m_nIndex]->SetDataSource (m_ppItems[m_nIndex]->pSource, nSourceFlag);

	m_ppPlayer[m_nIndex]->SetParam (VOMP_PID_START_POS, &m_ppItems[m_nIndex]->nStartPos);
	m_ppPlayer[m_nIndex]->SetParam (VOMP_PID_STOP_POS, &m_ppItems[m_nIndex]->nStopPos);

	m_ppPlayer[m_nIndex]->Run ();

	m_bAudioNewFile = true;
	m_bVideoNewFile = true;

	return 0;
}

int voCEditPlayer::GetParam (int nID, void * pValue)
{
	if (nID == VOMP_PID_AUDIO_SAMPLE)
		return GetAudioBuffer ((VOMP_BUFFERTYPE **)pValue);
	else if (nID == VOMP_PID_VIDEO_SAMPLE)
		return GetVideoBuffer ((VOMP_BUFFERTYPE **)pValue);

	voCAutoLock lock (&m_mtPlayer);

	if (m_ppPlayer == NULL)
		return VOMP_ERR_Status;
	if (m_ppPlayer[m_nIndex] == NULL)
		return VOMP_ERR_Status;

	return m_ppPlayer[m_nIndex]->GetParam (nID, pValue);
}

int voCEditPlayer::SetParam (int nID, void * pValue)
{
	voCAutoLock lock (&m_mtPlayer);

	if (nID == VOMP_PID_SEEK_MODE)
	{
		m_nSeekMode = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_THREAD_CREATE)
	{
		m_fThreadCreate = (VOTDThreadCreate)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_FUNC_LIB)
	{
		m_pLibFunc = (VOMP_LIB_FUNC *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_FUNC_DRM)
	{
		m_pDrmCB = (VOMP_SOURCEDRM_CALLBACK *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_DISABLE_DROPFRAME)
	{
		m_nDisableDropVideoFrame = *(int *)pValue;
		if (m_nDisableDropVideoFrame > 0)
			m_nVideoDropRender = 0;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_DISABLE_DROPRENDER)
	{
		m_nVideoDropRender = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_CHECK_PERFORMANCE)
	{
		m_nCheckFastPerformance = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_GET_THUMBNAIL)
	{
		m_nGetThumbnail = *(int *)pValue;
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_PID_DRAW_COLOR) 
	{
		m_nColorType = *(int *)pValue;
		VOLOGI ("m_nColorType = %d", m_nColorType);

		if (m_ppPlayer != NULL && m_ppPlayer[m_nIndex] != NULL)
			m_ppPlayer[m_nIndex]->SetParam (VOMP_PID_DRAW_COLOR, &m_nColorType);

		return VOMP_ERR_None;
	}

	return VOMP_ERR_Implement;
}

int voCEditPlayer::CheckSource (VOMP_EDITOR_SOURCE * pSource)
{
	if (pSource == NULL || pSource->nCount <= 0)
		return VOMP_ERR_ParamID;

	m_nDuration = 0;

	m_ppItems = new VOMP_EDITOR_ITEM *[pSource->nCount];
	if(!m_ppItems)
		return VOMP_ERR_OutMemory;
	memset (m_ppItems, 0, sizeof (VOMP_EDITOR_ITEM *) * pSource->nCount);
	m_ppURL = new VO_TCHAR *[pSource->nCount];
	if(!m_ppURL)
		return VOMP_ERR_OutMemory;
	memset (m_ppURL, 0, sizeof (VO_CHAR *) * pSource->nCount);

	int nFlag = 0;
	int nIndex = 0;
	voCMediaPlayer * pPlayer = new voCMediaPlayer (m_pListener, m_pUserData);
	if(!pPlayer)
		return VOMP_ERR_OutMemory;
	pPlayer->SetInstance (m_hInst);
	for (int i = 0; i < pSource->nCount; i++)
	{
		if (pSource->ppItems[i] == NULL)
			continue;

		nFlag = (pSource->ppItems[i]->nFlag & 0XFFFFFFF0) | VOMP_FLAG_SOURCE_SYNC | VOMP_FLAG_SOURCE_METADATA;
		if (pPlayer->SetDataSource (pSource->ppItems[i]->pSource, nFlag) == VOMP_ERR_None)
		{
			m_ppItems[nIndex] = new VOMP_EDITOR_ITEM;
			if(!m_ppItems[nIndex])
				return VOMP_ERR_OutMemory;
			memcpy (m_ppItems[nIndex], pSource->ppItems[i], sizeof (VOMP_EDITOR_ITEM));
			if (pSource->ppItems[i]->nFlag & VOMP_FLAG_SOURCE_URL)
			{
				m_ppURL[i] = new VO_TCHAR[vostrlen ((VO_TCHAR *)pSource->ppItems[i]->pSource) + 1];
				if(!m_ppURL[i])
					return VOMP_ERR_OutMemory;
				vostrcpy (m_ppURL[i], (VO_TCHAR *)pSource->ppItems[i]->pSource);
				m_ppItems[nIndex]->pSource = m_ppURL[i];
			}

			if (pSource->ppItems[i]->nStopPos > pSource->ppItems[i]->nStartPos)
			{
				pPlayer->SetParam (VOMP_PID_START_POS, &pSource->ppItems[i]->nStartPos);
				pPlayer->SetParam (VOMP_PID_STOP_POS, &pSource->ppItems[i]->nStopPos);

				pPlayer->GetParam (VOMP_PID_START_POS, &m_ppItems[nIndex]->nStartPos);
				pPlayer->GetParam (VOMP_PID_STOP_POS, &m_ppItems[nIndex]->nStopPos);
			}
			else
			{
				pPlayer->GetDuration (&m_ppItems[nIndex]->nStopPos);
			}

			m_nDuration += (m_ppItems[nIndex]->nStopPos - m_ppItems[nIndex]->nStartPos);

			nIndex++;
		}
	}
	delete pPlayer;
	pPlayer = NULL;

	m_sSource.nCount = nIndex;
	m_ppPlayer = new voCMediaPlayer *[m_sSource.nCount];
	if(!m_ppPlayer)
		return VOMP_ERR_OutMemory;
	memset (m_ppPlayer, 0, sizeof (voCMediaPlayer *) * m_sSource.nCount);

	return VOMP_ERR_None;
}

int voCEditPlayer::CreatePlayer (int nIndex)
{
	if (m_ppPlayer[nIndex] != NULL)
	{
		m_ppPlayer[nIndex]->Stop ();
		delete m_ppPlayer[nIndex];
		m_ppPlayer[nIndex] = NULL;
	}

	m_ppPlayer[nIndex] = new voCMediaPlayer (OnListener, this);
	if (m_ppPlayer[nIndex] == NULL)
		return VOMP_ERR_OutMemory;

	m_ppPlayer[nIndex]->SetInstance (m_hInst);

	int	nPlayMode = VOMP_PULL_MODE;
	m_ppPlayer[nIndex]->SetParam (VOMP_PID_AUDIO_PLAYMODE, &nPlayMode);
	m_ppPlayer[nIndex]->SetParam (VOMP_PID_VIDEO_PLAYMODE, &nPlayMode);

	int nRC = m_ppPlayer[nIndex]->SetParam (VOMP_PID_SEEK_MODE, &m_nSeekMode);
	nRC = m_ppPlayer[nIndex]->SetParam (VOMP_PID_THREAD_CREATE, (void *)m_fThreadCreate);
	nRC = m_ppPlayer[nIndex]->SetParam (VOMP_PID_FUNC_LIB, m_pLibFunc);
	nRC = m_ppPlayer[nIndex]->SetParam (VOMP_PID_FUNC_DRM, m_pDrmCB);
	nRC = m_ppPlayer[nIndex]->SetParam (VOMP_PID_DISABLE_DROPFRAME, &m_nDisableDropVideoFrame);
	nRC = m_ppPlayer[nIndex]->SetParam (VOMP_PID_DISABLE_DROPRENDER, &m_nVideoDropRender);
	nRC = m_ppPlayer[nIndex]->SetParam (VOMP_PID_CHECK_PERFORMANCE, &m_nCheckFastPerformance);
	nRC = m_ppPlayer[nIndex]->SetParam (VOMP_PID_GET_THUMBNAIL, &m_nGetThumbnail);
	nRC = m_ppPlayer[nIndex]->SetParam (VOMP_PID_DRAW_COLOR, &m_nColorType);

	VOLOGI ("m_nColorType = %d", m_nColorType);

	return VOMP_ERR_None;
}

int voCEditPlayer::LoadNextSource (bool bSync)
{
	m_bNextReady = true;

	int nIndex = m_nIndex + 1;
	if (nIndex >= m_sSource.nCount)
		nIndex = 0;

	if (CreatePlayer (nIndex) < 0)
		return VOMP_ERR_OutMemory;

	int nSourceFlag = 0;
	if (nIndex == 0 || bSync)
		nSourceFlag = (m_ppItems[nIndex]->nFlag & 0XFFFFFFF0) | VOMP_FLAG_SOURCE_SYNC;
	else
		nSourceFlag = (m_ppItems[nIndex]->nFlag & 0XFFFFFFF0) | VOMP_FLAG_SOURCE_ASYNC;

	int nRC = m_ppPlayer[nIndex]->SetDataSource (m_ppItems[nIndex]->pSource, nSourceFlag);

	return nRC;
}

int	voCEditPlayer::OnListener (void * pUserData, int nID, void * pParam1, void * pParam2)
{
	voCEditPlayer * pPlayer = (voCEditPlayer *)pUserData;

	return pPlayer->HandleEvent (nID, pParam1, pParam2);
}

int	voCEditPlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	if (nID == VOMP_CB_OpenSource)
	{
		voCAutoLock lock (&m_mtPlayer);
		int nIndex = m_nIndex + 1;
		if (m_nIndex + 1 >= m_sSource.nCount)
			nIndex = 0;
		if (m_ppPlayer[nIndex] != NULL)
		{
			m_ppPlayer[nIndex]->SetParam (VOMP_PID_START_POS, &m_ppItems[nIndex]->nStartPos);
			m_ppPlayer[nIndex]->SetParam (VOMP_PID_STOP_POS, &m_ppItems[nIndex]->nStopPos);
		}
		return VOMP_ERR_None;
	}
	else if (nID == VOMP_CB_PlayComplete)
	{
		voCAutoLock lock (&m_mtPlayer);

		m_bNextReady = false;
		m_bAudioNewFile = true;
		m_bVideoNewFile = true;

		if (m_sSource.nCount <= 1)
		{
			if (m_pListener != NULL)
				return m_pListener (m_pUserData, nID, pParam1, pParam2);
		}

		while (!m_bVideoInLoop || !m_bAudioInLoop)
			voOS_Sleep (10);

		if (m_nIndex + 1 < m_sSource.nCount)
		{
			m_ppPlayer[m_nIndex]->CloseSource (false);

			if (m_ppPlayer[m_nIndex + 1] == NULL)
				LoadNextSource (true);

			m_nIndex++;
			if (m_ppPlayer[m_nIndex] != NULL)
				 m_ppPlayer[m_nIndex]->Run ();
	
			return VOMP_ERR_None;
		}
		else
		{
			m_ppPlayer[m_nIndex]->CloseSource (true);

			if (m_ppPlayer[0] == NULL)
				LoadNextSource (true);

			m_nIndex = 0;
			if (m_ppPlayer[m_nIndex] != NULL)
				 m_ppPlayer[m_nIndex]->Run ();
		}
	}

	if (m_pListener != NULL)
		return m_pListener (m_pUserData, nID, pParam1, pParam2);

	return 0;
}
