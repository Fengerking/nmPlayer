	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCAudioRenderBufferMgr.cpp

	Contains:	voCAudioRenderBufferMgr class file

	Written by:	East Zhou

	Change History (most recent first):
	2013-11-25		East		Create file

*******************************************************************************/
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include "voCAudioRenderBufferMgr.h"

#define LOG_TAG "voCAudioRenderBufferMgr"
#include "voLog.h"

voCARBList::voCARBList()
	: m_ppBuffers(NULL)
	, m_nBuffers(0)
	, m_nCount(0)
{
}

voCARBList::~voCARBList()
{
	if(m_ppBuffers)
		delete [] m_ppBuffers;
}

VO_BOOL voCARBList::create(VO_U32 nBuffers)
{
	if(m_ppBuffers)
		delete [] m_ppBuffers;

	m_nBuffers = 0;
	m_nCount = 0;

	if(nBuffers > 0)
	{
		m_ppBuffers = new VOMP_AUDIORENDERBUFFER *[nBuffers];
		if(NULL == m_ppBuffers)
			return VO_FALSE;

		memset(m_ppBuffers, 0, nBuffers * sizeof(VOMP_AUDIORENDERBUFFER *));
	}

	m_nBuffers = nBuffers;

	return VO_TRUE;
}

VO_BOOL voCARBList::add(VOMP_AUDIORENDERBUFFER * pBuffer)
{
	for(VO_U32 i = 0; i < m_nBuffers; i++)
	{
		if(m_ppBuffers[i] == NULL)
		{
			m_ppBuffers[i] = pBuffer;
			m_nCount++;

			return VO_TRUE;
		}
	}

	return VO_FALSE;
}

VO_BOOL voCARBList::remove(VOMP_AUDIORENDERBUFFER * pBuffer)
{
	VO_U32 nDelete = -1;
	VO_U32 i = 0;
	for(i = 0; i < m_nBuffers; i++)
	{
		if(m_ppBuffers[i] == pBuffer)
		{
			nDelete = i;
			break;
		}
	}

	if(-1 == nDelete)
	{
		VOLOGW("the buffer 0x%08X which we want to remove is not in list!!!", pBuffer);
		return VO_FALSE;
	}

	for(i = nDelete; i < m_nBuffers - 1; i++)
	{
		m_ppBuffers[i] = m_ppBuffers[i + 1];
		if(m_ppBuffers[i + 1] == NULL)
			break;
	}

	if(i == m_nBuffers - 1)
		m_ppBuffers[i] = NULL;

	m_nCount--;
	return VO_TRUE;
}

void voCARBList::removeAll()
{
	for(VO_U32 i = 0; i < m_nBuffers; i++)
		m_ppBuffers[i] = NULL;

	m_nCount = 0;
}


voCAudioRenderBufferMgr::voCAudioRenderBufferMgr(VO_U32 nBufferCount)
	: m_pARBs(NULL)
	, m_nBufferCount(nBufferCount)
{
	m_pARBs = new VOMP_AUDIORENDERBUFFER[m_nBufferCount];
	memset(m_pARBs, 0, m_nBufferCount * sizeof(VOMP_AUDIORENDERBUFFER));

	m_listIdle.create(m_nBufferCount);
	m_listValid.create(m_nBufferCount);

	for(VO_U32 i = 0; i < m_nBufferCount; i++)
		m_listIdle.add(&m_pARBs[i]);
}

voCAudioRenderBufferMgr::~voCAudioRenderBufferMgr()
{
	if(m_pARBs)
	{
		for(VO_U32 i = 0; i < m_nBufferCount; i++)
		{
			if(m_pARBs[i].pData)
			{
				delete [] m_pARBs[i].pData;
				m_pARBs[i].pData = NULL;
			}
		}

		delete [] m_pARBs;
		m_pARBs = NULL;
	}
}

VO_U32 voCAudioRenderBufferMgr::getWritableBuffer(VOMP_AUDIORENDERBUFFER ** ppARB, VO_AUDIO_FORMAT * pFormat)
{
	VOMP_AUDIORENDERBUFFER * pARB = m_listIdle.getHead();
	if(!pARB)
		return VO_ERR_RETRY;

	if(memcmp(&pARB->sFormat, pFormat, sizeof(VO_AUDIO_FORMAT)))
	{
		// audio format changed, we need re-allocate
		if(pARB->pData)
			delete [] pARB->pData;

		pARB->nSize = pFormat->SampleRate * pFormat->Channels * pFormat->SampleBits / 8;
		pARB->pData = new VO_BYTE[pARB->nSize];
		if(!pARB->pData)
			return VO_ERR_OUTOF_MEMORY;
		memcpy(&pARB->sFormat, pFormat, sizeof(VO_AUDIO_FORMAT));
	}

	*ppARB = pARB;

	return VO_ERR_NONE;
}

void voCAudioRenderBufferMgr::finishWriting(VOMP_AUDIORENDERBUFFER * pARB)
{
	m_listIdle.remove(pARB);
	m_listValid.add(pARB);
}

VO_U32 voCAudioRenderBufferMgr::getPCMData(VO_PBYTE pToGet, VO_U32 nToGet, VO_U32 * pnGot, VO_S64 * pllTime, VO_AUDIO_FORMAT * pFormat)
{
	if(!pFormat)
		return VO_ERR_INVALID_ARG;

	VOMP_AUDIORENDERBUFFER * pARB = m_listValid.getHead();
	if(!pARB)
		return VO_ERR_RETRY;

	if(memcmp(pFormat, &pARB->sFormat, sizeof(VO_AUDIO_FORMAT)))
	{
		// audio format not same
		memcpy(pFormat, &pARB->sFormat, sizeof(VO_AUDIO_FORMAT));
		return VO_ERR_FINISH;
	}

	if(pToGet)
		memcpy(pToGet, pARB->pData, pARB->nLength);
	if(pnGot)
		*pnGot = pARB->nLength;
	if(pllTime)
		*pllTime = pARB->llTime;

	m_listValid.remove(pARB);
	m_listIdle.add(pARB);

	return VO_ERR_NONE;
}

void voCAudioRenderBufferMgr::flush()
{
	VOMP_AUDIORENDERBUFFER * pARB = m_listValid.getHead();
	while(pARB)
	{
		m_listValid.remove(pARB);
		m_listIdle.add(pARB);

		pARB = m_listValid.getHead();
	}
}
