#include "VOUtility.h"
#include "voPDPort.h"
#include "CThroughputer.h"

#define THROUGH_PUT_ELEMENT_NUM			5

CThroughputer::CThroughputer()
	: m_pHead(NULL)
	, m_bFull(false)
{
	//::InitializeCriticalSection(&m_cs);
	CREATE_MUTEX(m_cs);
	m_pElems = new ThroughputElem[THROUGH_PUT_ELEMENT_NUM];
	if(m_pElems)
	{
		m_pHead = m_pElems;
		PThroughputElem pTmp = m_pElems;
		for(int i = 0; i < THROUGH_PUT_ELEMENT_NUM - 1; i++)
			m_pElems[i].next = m_pElems + (i + 1);
		m_pElems[THROUGH_PUT_ELEMENT_NUM - 1].next = m_pElems;
	}
}

CThroughputer::~CThroughputer()
{
	if(m_pElems)
		delete [] m_pElems;

	DELETE_MUTEX(m_cs);
}

bool CThroughputer::HasElem()
{
	CAutoLock lock(m_cs);
	bool bHasElem = (m_pHead && m_pHead->bytes & 0x80000000);
	return bHasElem;
}

bool CThroughputer::AddElem(DWORD dwBytes, DWORD dwDuration)
{
	CAutoLock lock(m_cs);
	if(!m_pHead)
	{
		return false;
	}

	if(!m_bFull)
	{
		PThroughputElem pElem = m_pHead;
		for(int i = 0; i < THROUGH_PUT_ELEMENT_NUM; i++)
		{
			if(!(pElem->bytes & 0x80000000))
			{
				pElem->bytes = (dwBytes | 0x80000000);
				pElem->duration = dwDuration;
				return true;
			}
			pElem = pElem->next;
		}

		m_bFull = true;
	}

	m_pHead->bytes = (dwBytes | 0x80000000);
	m_pHead->duration = dwDuration;
	m_pHead = m_pHead->next;
	
	return true;
}

bool CThroughputer::GetThroughput(DWORD& dwThroughput)
{
	CAutoLock lock(m_cs);
	if(!m_pHead || !(m_pHead->bytes & 0x80000000))
	{
		return false;
	}

	DWORD dwBytes = 0;
	DWORD dwDuration = 0;
	PThroughputElem pElem = m_pHead;
	for(int i = 0; i < THROUGH_PUT_ELEMENT_NUM; i++)
	{
		if(pElem->bytes & 0x80000000)
		{
			dwBytes += (pElem->bytes & 0x7FFFFFFF);
			dwDuration += pElem->duration;
		}
		else
			break;

		pElem = pElem->next;
	}

	dwThroughput = (dwDuration > 0) ? LONGLONG(dwBytes) * 1000 / dwDuration : MAXDWORD;
	return true;
}
