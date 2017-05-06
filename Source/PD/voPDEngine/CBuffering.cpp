#include <voPDPort.h>
#ifdef _HTC
#include "common_sdk_PD.h"
#else	//_HTC
#include "voStreaming.h"
#endif	//_HTC
#include "CNetGet.h"
#include "CBuffering.h"

CBuffering::CBuffering(CNetGet2* pNetGet)
	: m_pNetGet(pNetGet)
	, m_Type(BT_NONE)
	, m_nStart(0)
	, m_nEnd(0)
	, m_nPercent(0)
{
	CREATE_MUTEX(m_csLock);//=VOCPFactory::CreateOneMutex();
}

CBuffering::~CBuffering()
{
	DELETE_MUTEX(m_csLock);
}

bool CBuffering::Start(int nStart, int nEnd, bool bForceSend, BUFFERINGTYPE btType, int nCurrFilePos)
{
	CAutoLock lock(m_csLock);
#define MAX_FILE_SIZE2 0x4fffffff
#define MIN_FILE_POS2 0x0000ffff
	if (nEnd>MAX_FILE_SIZE2)
	{
		voLog(LL_TRACE,"buffering.txt", "buffering:change end from %d to %d\n",nEnd,nStart+MIN_FILE_POS2);
		nEnd=nStart+MIN_FILE_POS2;
	}
	voLog(LL_TRACE,"buffering.txt", "buffering:begin=%d,end=%d,force=%d,type=%d,curFilepos=%d",nStart,nEnd,bForceSend,btType,nCurrFilePos);
	if(btType == BT_SEEK)
	{
		if(BT_NONE != m_Type)
		{
#ifdef _HTC
			m_pNetGet->NotifyEvent(HS_EVENT_BUFFERING_END, NULL);
#else	//_HTC
			m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_END, NULL);
#endif	//_HTC
			m_Type = BT_NONE;
		}
	}
	else		//BT_PLAY
	{
		if(BT_NONE != m_Type)
			return false;
	}

	CDFInfo* pDfInfo = m_pNetGet->GetDFInfo();
	CDFInfo2* pDfInfo2 = m_pNetGet->GetDFInfo2();

	IVOMutex* pCritSec = NULL;
	if(pDfInfo)
		pCritSec = pDfInfo->GetCritSec();
	else if(pDfInfo2)
		pCritSec = pDfInfo2->GetCritSec();

	if(!pCritSec)
		return false;

	if(bForceSend)
	{
#ifdef _HTC
		m_pNetGet->NotifyEvent(HS_EVENT_BUFFERING_BEGIN, NULL);
#else	//_HTC
		m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_BEGIN, NULL);
#endif	//_HTC

		CAutoLock lock(pCritSec);
		if(pDfInfo)
		{
			if(pDfInfo->CanRead(nStart, nEnd - nStart, false))
			{
#ifdef _HTC
				m_pNetGet->NotifyEvent(HS_EVENT_BUFFERING_END, NULL);
#else	//_HTC
				m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_END, NULL);
#endif	//_HTC
				return true;
			}
		}
		else if(pDfInfo2)
		{
			DWORD dwContStart = 0;
			if(pDfInfo2->CanRead(nStart, nEnd - nStart, dwContStart, false))
			{
#ifdef _HTC
				m_pNetGet->NotifyEvent(HS_EVENT_BUFFERING_END, NULL);
#else	//_HTC
				m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_END, NULL);
#endif	//_HTC
				return true;
			}
		}
	}
	else
	{
		CAutoLock lock(pCritSec);
		if(pDfInfo)
		{
			if(pDfInfo->CanRead(nStart, nEnd - nStart, false))
				return true;
		}
		else if(pDfInfo2)
		{
			DWORD dwContStart = 0;
			if(pDfInfo2->CanRead(nStart, nEnd - nStart, dwContStart, false))
				return true;
		}

#ifdef _HTC
		m_pNetGet->NotifyEvent(HS_EVENT_BUFFERING_BEGIN, NULL);
#else	//_HTC
		m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_BEGIN, NULL);
#endif	//_HTC
	}

	m_Type = btType;
	m_nStart = nStart;

	m_nEnd = nEnd;
	if(nCurrFilePos >= 0)
		m_nPercent = (nCurrFilePos - m_nStart) * 100 / (m_nEnd - m_nStart);
	else
		m_nPercent = 0;
	//voLog(LL_TRACE,"buffering.txt", "curType=%d\n", m_Type);

	return true;
}

void CBuffering::Stop(bool bSendBufferingEnd /* = false */)
{
	CAutoLock lock(m_csLock);

	if(BT_NONE != m_Type && bSendBufferingEnd)//
	{
#ifdef _HTC
		m_pNetGet->NotifyEvent(HS_EVENT_BUFFERING_END, NULL);
#else	//_HTC
		m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_END, NULL);
#endif	//_HTC
	}

	m_Type = BT_NONE;
	m_nStart = 0;
	m_nEnd = 0;
	m_nPercent = 0;
}

BUFFERINGTYPE CBuffering::GetBufferingType()
{
	return m_Type;
}

int CBuffering::GetPercent()
{
	return (BT_NONE != m_Type) ? m_nPercent : 100;
}

bool CBuffering::SetCurrentDownloadFilePos(int nFilePos)
{
	if(BT_NONE == m_Type)
		return false;
	
	CDFInfo* pDfInfo = m_pNetGet->GetDFInfo();
	if(pDfInfo)
	{
		if(nFilePos >= m_nEnd || pDfInfo->CanRead(m_nStart, m_nEnd - m_nStart))		//buffering end
		{
#ifdef _HTC
			m_pNetGet->NotifyEvent(HS_EVENT_BUFFERING_END, NULL);
#else	//_HTC
			m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_END, NULL);
#endif	//_HTC

			m_nPercent = 100;
			m_Type = BT_NONE;
		}
		else
			m_nPercent = (nFilePos - m_nStart) * 100 / (m_nEnd - m_nStart);
	}
	else
	{
		if(nFilePos >= m_nEnd)		//buffering end
		{
#ifdef _HTC
			m_pNetGet->NotifyEvent(HS_EVENT_BUFFERING_END, NULL);
#else	//_HTC
			m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_END, NULL);
#endif	//_HTC

			m_nPercent = 100;
			m_Type = BT_NONE;
		}
		else
			m_nPercent = (nFilePos - m_nStart) * 100 / (m_nEnd - m_nStart);
	}

	if (PDGlobalData::data.version==1)
	{
		static long percent=0;
		if (percent+5<m_nPercent)
		{
			percent = m_nPercent;
			m_pNetGet->NotifyEvent(VO_EVENT_BUFFERING_PERCENT,percent);
		}
		if(m_nPercent==100)
			percent=0;
	}
	return true;
}
