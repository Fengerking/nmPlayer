#include "CSampleData.h"

#if defined(DMEMLEAK)
#  include "dmemleak.h"
#endif 

CSampleData::CSampleData(void)
	: m_pData (NULL)
	, m_nDataSize (0)
	, m_nDataLen (0)
	, m_dwStartTime (0)
	, m_dwEndTime (0)
	, m_bSyncPoint (false)
	, m_pNext (NULL)
	, m_pExtData (NULL)
	, m_nExtDataLen (0)
{
}

CSampleData::~CSampleData(void)
{
	FreeData ();
}

bool CSampleData::AllocBuf (int nSize)
{
	if (m_nDataSize == nSize)
		return true;

	FreeData();

#ifdef _DEBUG
	//OutputDebugString("alloc Buf\n");
#endif

	m_pData = new VO_BYTE[nSize];
	if (m_pData == NULL)
		return false;

	m_nDataSize = nSize;

	return true;
}

int	CSampleData::GetBuffer (VO_PBYTE * ppBuffer)
{
	if (m_pData == NULL)
		return 0;

	if (ppBuffer == NULL)
		return 0;

	*ppBuffer = m_pData;
	return m_nDataSize;
}

bool CSampleData::FreeData (void)
{
	if (m_pData != NULL)
	{
#ifdef _DEBUG
	//OutputDebugString("free Buf\n");
#endif
		delete []m_pData;
	}
		

	m_pData = NULL;
	m_nDataSize = 0;
	m_nDataLen = 0;

	m_pExtData = NULL;
	m_nExtDataLen = 0;

	return true;
}

bool CSampleData::SetData (VO_PBYTE pData, int nLen)
{
	if (nLen > m_nDataSize)
	{
		if (!AllocBuf (nLen))
			return false;
	}

	memcpy (m_pData, pData, nLen);
	m_nDataLen = nLen;

	return true;
}


bool CSampleData::AddData (VO_PBYTE pData, int nLen)
{
	if (m_nDataLen + nLen > m_nDataSize)
	{
		int nOldDataLen = m_nDataLen;
		VO_PBYTE pOldData = new VO_BYTE[nOldDataLen];
		memcpy (pOldData, m_pData, nOldDataLen);

		if (!AllocBuf (m_nDataLen + nLen))
			return false;

		memcpy (m_pData, pOldData, nOldDataLen);
		m_nDataLen = nOldDataLen;

		delete []pOldData;
	}

	memcpy (m_pData + m_nDataLen, pData, nLen);
	m_nDataLen += nLen;

	return true;
}

int	CSampleData::GetData (VO_PBYTE * ppData)
{
	if (ppData == NULL)
		return 0;

	if (m_pExtData != NULL)
	{
		*ppData = m_pExtData;
		return m_nExtDataLen;
	}

	*ppData = m_pData;

	return m_nDataLen;
}

bool CSampleData::SetSample (VO_PBYTE pData, int nLen, VO_U32 dwStart, VO_U32 dwEnd, bool bSync)
{
	if (!SetData (pData, nLen))
		return false;

	m_dwStartTime = dwStart;
	m_dwEndTime = dwEnd;

	m_bSyncPoint = bSync;

	return true;
}

bool CSampleData::SetNext (CSampleData * pSampleData)
{
	m_pNext = pSampleData;

	return true;
}

CSampleData * CSampleData::GetNext (void)
{
	return m_pNext;
}

bool CSampleData::GetTime (VO_U32 & dwStart, VO_U32 & dwEnd)
{
	dwStart = m_dwStartTime;
	dwEnd = m_dwEndTime;

	return true;
}

bool CSampleData::SetTime (VO_U32 dwStart, VO_U32 dwEnd)
{
	m_dwStartTime = dwStart;
	m_dwEndTime = dwEnd;

	return true;
}

bool CSampleData::SetSyncPoint (bool bSync)
{
	m_bSyncPoint = bSync;

	return true;
}

bool CSampleData::GetSyncPoint (void)
{
	return m_bSyncPoint;
}

bool CSampleData::SetExtData (VO_PBYTE pExtData, int nExtLen)
{
	m_pExtData = pExtData;
	m_nExtDataLen = nExtLen;

	return true;
}


// Sample data chain
CSampleChain::CSampleChain(void)
	: m_pHead (NULL)
	, m_pTail (NULL)
	, m_nSampleCount (0)
{
}

CSampleChain::~CSampleChain(void)
{
}

bool CSampleChain::Init (int nDataSize, int nSampleCount)
{
	if (nSampleCount <= 0 || nDataSize <= 0)
		return false;

	for (int i = 0; i < nSampleCount; i++)
	{
		CSampleData * pSample = new CSampleData ();

#ifdef _DEBUG
	//OutputDebugString("new Init CSampleData\n");
#endif

		if (pSample == NULL)
			return false;

		pSample->AllocBuf (nDataSize);

		AddTail (pSample);
	}

	return true;
}

bool CSampleChain::AddTail (CSampleData * pSample)
{
	voCAutoLock lock (&m_csSample);

	if (pSample == NULL)
		return false;

	if (m_pTail == NULL)
	{
		m_pHead = pSample;
		m_pTail = pSample;

		m_pHead->SetNext (NULL);
		m_pTail->SetNext (NULL);
	}
	else if (m_pTail == m_pHead)
	{
		m_pHead->SetNext (pSample);
		m_pTail = pSample;
	}
	else
	{
		m_pTail->SetNext (pSample);
		m_pTail = pSample;
	}

	pSample->SetNext (NULL);

	m_nSampleCount++;

	return true;
}


CSampleData * CSampleChain::RemoveHead (void)
{
	voCAutoLock lock (&m_csSample);

	CSampleData * pSample = NULL;
	if (m_pHead == NULL)
	{
		return NULL;
	}
	else if (m_pHead->GetNext () == NULL)
	{
		pSample = m_pHead;

		m_pHead = NULL;
		m_pTail = NULL;
	}
	else
	{
		pSample = m_pHead;
		m_pHead = m_pHead->GetNext ();
	}

	m_nSampleCount--;

	return pSample;
}

CSampleData * CSampleChain::RemoveTail (void)
{
	voCAutoLock lock (&m_csSample);

	CSampleData * pTail = NULL;

	if (m_pTail == NULL)
	{
		return NULL;
	}
	else if(m_pHead == NULL)
		return NULL;
	else
	{
		CSampleData * pSample = m_pHead;
		CSampleData * pPrevSample = NULL;

		int nCount = 0;
		while (pSample != NULL)
		{
			nCount++;
			if(nCount == m_nSampleCount-1)
			{
				pSample->SetNext(NULL);
				
				pTail = m_pTail;
				m_pTail = pSample;
				break;
			}
				
			pSample = pSample->GetNext ();
		}
	}

	m_nSampleCount--;

	return pTail;
}

bool CSampleChain::Remove(CSampleData* pRemove)
{
	voCAutoLock lock (&m_csSample);

	CSampleData * pPrev = NULL;
	CSampleData* pSample = m_pHead;

	while(pSample)
	{
		if(pRemove == pSample)
		{
			if(pRemove == m_pHead)
			{
				m_pHead = m_pHead->GetNext();
			}
			else
			{
				pPrev->SetNext(pRemove->GetNext());

				if(pRemove == m_pTail)
					m_pTail = pPrev;
			}
			
			m_nSampleCount--;

			if(0 == m_nSampleCount)
				m_pHead = m_pTail = NULL;
			else if(1 == m_nSampleCount)
				m_pTail = m_pHead;

			return true;
		}
		else
		{
			pPrev = pSample;
			pSample = pSample->GetNext();
		}
	}

	return false;
}


int CSampleChain::GetSampleCount (void)
{
//	return m_nSampleCount;

	int nSampleCount = 0;

	CSampleData * pSample = m_pHead;
	while (pSample != NULL)
	{
		nSampleCount ++;
		pSample = pSample->GetNext ();
	}

	return nSampleCount;
}

VO_U32 CSampleChain::GetDuration (void)
{
	voCAutoLock lock (&m_csSample);

	if (m_pHead == NULL)
		return 0;

	VO_U32 dwChainStart = 0;

	VO_U32 dwStart, dwEnd;
	m_pHead->GetTime (dwStart, dwEnd);
	dwChainStart = dwStart;

	m_pTail->GetTime (dwStart, dwEnd);

	return dwEnd - dwChainStart;
}

bool CSampleChain::Release (void)
{
	voCAutoLock lock (&m_csSample);

	CSampleData * pSample = m_pHead;
	CSampleData * pDelSample = NULL;

	while (pSample != NULL)
	{
		pDelSample = pSample;

		pSample = pSample->GetNext ();

		delete pDelSample;
	}

	m_pHead = NULL;
	m_pTail = NULL;

	return true;
}
