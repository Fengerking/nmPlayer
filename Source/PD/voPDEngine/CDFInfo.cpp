/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CDFInfo.cpp

Contains:	CDFInfo class file

Written by:	East Zhou

Change History (most recent first):
2007-10-17		East		Create file

*******************************************************************************/
#include "CDFInfo.h"
#include "PDHeaderDataStruct.h"
#include "VOUtility.h"
using namespace VOUtility; 
CDFInfo::CDFInfo()
	: m_pFragHead(NULL)
	, m_dwFileSize(MAXDWORD)
	, m_pFragCurr(NULL)
	, m_dwCurrPlayPos(0)
{
	CREATE_MUTEX(m_csFrag);//==VOCPFactory::CreateOneMutex();
}

CDFInfo::~CDFInfo()
{
	DELETE_MUTEX(m_csFrag);
	ClearFragments();
}

bool CDFInfo::ReadIn(PBYTE pSrc, DWORD dwFrags)
{
	if(!pSrc)
		return false;

	CAutoLock lock(m_csFrag);
	PBYTE pTmpFrag = pSrc;
	PDHFileFragment pff;
	for(DWORD i = 0; i < dwFrags; i++)
	{
		memcpy(&pff, pTmpFrag, sizeof(PDHFileFragment));
		if(!AddFragment(pff.start, pff.length, false))
			return false;
		pTmpFrag += sizeof(PDHFileFragment);
	}

	m_pFragCurr = m_pFragHead;

	return true;
}

bool CDFInfo::WriteOut(PBYTE pDst)
{
	if(!pDst)
		return false;

	PDFFrag pTmp = m_pFragHead;
	PDHFileFragment pff;
	PBYTE pTmpFrag = pDst;
	while(pTmp)
	{
		pff.start = pTmp->start;
		pff.length = pTmp->len;

		memcpy(pTmpFrag, &pff, sizeof(PDHFileFragment));

		pTmp = pTmp->next;
		pTmpFrag += sizeof(PDHFileFragment);
	}

	return true;
}

DWORD CDFInfo::GetFragmentNum()
{
	CAutoLock lock(m_csFrag);
	DWORD dwNum = 0;
	PDFFrag pTmp = m_pFragHead;
	while(pTmp)
	{
		dwNum++;
		pTmp = pTmp->next;
	}

	return dwNum;
}

bool CDFInfo::AddFragment(DWORD dwStart, DWORD dwLen, bool bLock /* = true */)
{
	if(bLock)
		m_csFrag->Lock();

	if(!m_pFragHead)
	{
		m_pFragHead = new(MEM_CHECK) DFFrag(dwStart, dwLen);
		if(!m_pFragHead)
		{
			if(bLock)
				m_csFrag->UnLock();
			return false;
		}
		m_pFragCurr = m_pFragHead;
	}
	else
	{
		//find previous fragment
		PDFFrag pPrev = NULL;
		PDFFrag pTmp = m_pFragHead;
		while(pTmp)
		{
			if(pTmp->start < dwStart)
				pPrev = pTmp;
			pTmp = pTmp->next;
		}

		//find next fragment
		PDFFrag pNext = pPrev ? pPrev->next : m_pFragHead;

		//process
		if(!pPrev || pPrev->start + pPrev->len < dwStart)
		{
			if(!pNext || pNext->start > dwStart + dwLen)
			{
				PDFFrag pNew = new(MEM_CHECK) DFFrag(dwStart, dwLen);
				if(!pNew)
				{
					if(bLock)
						m_csFrag->UnLock();
					return false;
				}
				pPrev->next = pNew;
				pNew->next = pNext;
				m_pFragCurr = pNew;
			}
			else
			{
				pNext->len = pNext->start + pNext->len - dwStart;
				pNext->start = dwStart;
				m_pFragCurr = pNext;
			}
		}
		else
		{
			if(!pNext || pNext->start > dwStart + dwLen)
			{
				pPrev->len = dwStart + dwLen - pPrev->start;
				m_pFragCurr = pPrev;
			}
			else
			{
				pPrev->len = pNext->start + pNext->len - pPrev->start;
				pPrev->next = pNext->next;
				delete pNext;
				m_pFragCurr = pPrev;
			}
		}
	}

	if(bLock)
		m_csFrag->UnLock();
	return true;
}

bool CDFInfo::CanRead(DWORD dwStart, DWORD dwLen, bool bLock /* = true */)
{
	if(bLock)
		m_csFrag->Lock();

	bool bCan = false;

	PDFFrag pTmp = m_pFragHead;
	while(pTmp)
	{
		if(pTmp->start > dwStart)
			break;

		if(pTmp->start + pTmp->len >= dwStart + dwLen)
		{
			bCan = true;
			m_dwCurrPlayPos = dwStart;
			break;
		}

		pTmp = pTmp->next;
	}

	if(bLock)
		m_csFrag->UnLock();

	return bCan;
}

void CDFInfo::ClearFragments()
{
	CAutoLock lock(m_csFrag);

	if(m_pFragHead)
	{
		PDFFrag pTmp = m_pFragHead;
		PDFFrag pDel = NULL;
		while(pTmp)
		{
			pDel = pTmp;
			pTmp = pTmp->next;
			delete pDel;
		}

		m_pFragHead = NULL;
	}
}

bool CDFInfo::GetBlankFragment(DWORD& dwStart, DWORD& dwLen)
{
	CAutoLock lock(m_csFrag);
	if(!m_pFragHead)
	{
		dwStart = 0;
		dwLen = m_dwFileSize;
	}
	else
	{
		if(m_pFragHead->next)
		{
			if(m_pFragCurr && m_pFragCurr->start + m_pFragCurr->len != m_dwFileSize)
			{
				dwStart = m_pFragCurr->start + m_pFragCurr->len;
				dwLen = m_pFragCurr->next ? (m_pFragCurr->next->start - dwStart) : (m_dwFileSize - dwStart);
			}
			else
			{
				dwStart = m_pFragHead->start + m_pFragHead->len;
				dwLen = m_pFragHead->next->start - dwStart;
			}
		}
		else	//only one fragment
		{
			if(m_pFragHead->len == m_dwFileSize)
				return false;	//file is complete
			else
			{
				dwStart = m_pFragHead->start + m_pFragHead->len;
				dwLen = m_dwFileSize - dwStart;
			}
		}
	}

	return (dwLen > 0);
}

bool CDFInfo::GetBlankFragmentByStart(DWORD& dwStart, DWORD& dwLen)
{
	CAutoLock lock(m_csFrag);
	//if in downloading
	if(m_pFragCurr && dwStart >= m_pFragCurr->start && dwStart <= (m_pFragCurr->start + m_pFragCurr->len + RESERVE_BUFFER_SIZE))
		return false;

	if(!m_pFragHead)
		dwLen = m_dwFileSize - dwStart;
	else
	{
		PDFFrag pTmp = m_pFragHead;
		PDFFrag pPrev = m_pFragHead;
		PDFFrag pNext = NULL;
		while(pTmp)
		{
			if(pTmp->start > dwStart)
			{
				pNext = pTmp;
				break;
			}

			pPrev = pTmp;
			pTmp = pTmp->next;
		}

		DWORD dwPrevEnd = pPrev->start + pPrev->len;

		dwStart = dwPrevEnd < dwStart ? dwStart : dwPrevEnd;
		dwLen = (pNext ? pNext->start : m_dwFileSize) - dwStart;
	}

	return (dwLen > 0);
}

DWORD CDFInfo::GetCurrPlayDFEnd()
{
	CAutoLock lock(m_csFrag);

	PDFFrag pTmp = m_pFragHead;
	while(pTmp)
	{
		if(pTmp->start > m_dwCurrPlayPos)
			break;

		if(pTmp->start + pTmp->len >= m_dwCurrPlayPos)
			return (pTmp->start + pTmp->len);

		pTmp = pTmp->next;
	}

	return 0;
}

CDFInfo2::CDFInfo2()
	: m_bOrdinal(true)
	, m_dwContStartPos(0)
	, m_dwContEndPos(0)
	, m_dwContSize(0)
	, m_dwFileOffset(0)
	, m_dwFileSize(0)
{
	CREATE_MUTEX(m_csFrag);//==VOCPFactory::CreateOneMutex();

}

CDFInfo2::~CDFInfo2()
{
	DELETE_MUTEX(m_csFrag);
}
/**
The content size is the buffer size,default is 5M
*/
bool CDFInfo2::SetContSize(DWORD dwContSize)
{
	CAutoLock lock(m_csFrag);
	m_dwContSize = dwContSize;
	return true;
}

bool CDFInfo2::CanRead(DWORD dwFilePos, DWORD dwLen, DWORD& dwContStart, bool bLock /* = true */)
{
	if(bLock)
		m_csFrag->Lock();

	DWORD dwContLen = GetContLenB();
	DWORD dwContFilePos = m_dwContStartPos + m_dwFileOffset;

	if(dwFilePos >= dwContFilePos && dwFilePos + dwLen <= dwContFilePos + dwContLen)
	{
		//update  content start
		dwContStart = m_dwContStartPos + dwFilePos - dwContFilePos;
		if(dwContStart >= m_dwContSize)
			dwContStart -= m_dwContSize;

		if(bLock)
			m_csFrag->UnLock();
		//voLog(LL_TRACE,"DFInfo.txt","read ok:contentFilePos(%d-%d),readPos(%d-%d)\n",dwContFilePos,dwContFilePos+dwContLen,dwFilePos,dwFilePos+dwLen);

		return true;
	}

	if(bLock)
		m_csFrag->UnLock();
	voLog(LL_TRACE,"DFInfo.txt","read fail:contentFilePos(%d-%d),readPos(%d-%d)\n",dwContFilePos,dwContFilePos+dwContLen,dwFilePos,dwFilePos+dwLen);
	return false;
}

bool CDFInfo2::CanWrite(DWORD dwLen, DWORD* pdwContStart)
{
	CAutoLock lock(m_csFrag);

	DWORD dwContLen = GetContLenB();

	if(m_dwContSize - dwContLen >= dwLen)
	{
		if(pdwContStart)
			*pdwContStart = m_dwContEndPos;
		return true;
	}
	voLog(LL_TRACE,"DFInfo.txt","write fail:bufsize=%d,contenLen=%d\n",m_dwContSize,dwContLen);

	return false;
}

bool CDFInfo2::AddFragment(DWORD dwLen)
{
	CAutoLock lock(m_csFrag);

	m_dwContEndPos += dwLen;
	if(m_dwContEndPos >= m_dwContSize)
		m_dwContEndPos -= m_dwContSize;

	m_bOrdinal = (m_dwContEndPos > m_dwContStartPos);
	voLog(LL_TRACE,"DFInfo.txt","AddFragment:%d,fileoffset=%d(%d,%d)\n",dwLen,m_dwFileOffset,m_dwContStartPos,m_dwContEndPos);
	return true;
}

void CDFInfo2::RemoveFragment(DWORD dwFilePos)
{
	CAutoLock lock(m_csFrag);

	DWORD dwContFilePos = m_dwContStartPos + m_dwFileOffset;
	if(dwFilePos <= dwContFilePos)
		return;

	DWORD dwContLen = GetContLenB();

	if(dwFilePos >= dwContFilePos + dwContLen)		//reset
	{
		m_dwContStartPos = 0;
		m_dwContEndPos = 0;
		m_dwFileOffset = dwFilePos;
		m_bOrdinal = true;

	}
	else
	{
		m_dwContStartPos += (dwFilePos - dwContFilePos);
		if(m_dwContStartPos >= m_dwContSize)
		{
			m_dwContStartPos -= m_dwContSize;
			m_dwFileOffset += m_dwContSize;
		}

		m_bOrdinal = (m_dwContEndPos >= m_dwContStartPos);
	}
	voLog(LL_TRACE,"DFInfo.txt","Remove Fragment:pos=%d,fileoffset=%d,contentPos=%d,contenLen=%d(%d,%d)\n",dwFilePos,m_dwFileOffset,dwContFilePos,dwContLen,m_dwContStartPos,m_dwContEndPos);
}


void CDFInfo2::Reset(DWORD dwFilePos)
{
	CAutoLock lock(m_csFrag);

	m_dwContStartPos = 0;
	m_dwContEndPos = 0;
	m_dwFileOffset = dwFilePos;
	m_bOrdinal = true;
	voLog(LL_TRACE,"DFInfo.txt","reset:filepos=%d\n",dwFilePos);

}

bool CDFInfo2::RemoveHeader(DWORD dwFileHeaderSize)
{
	CAutoLock lock(m_csFrag);

	m_dwContStartPos = 0;
	m_dwContEndPos -= dwFileHeaderSize;
	m_dwContSize -= dwFileHeaderSize;
	m_dwFileOffset += dwFileHeaderSize;

	return true;
}

bool CDFInfo2::InContent(DWORD dwFilePos)
{
	CAutoLock lock(m_csFrag);

	DWORD dwContFilePos = m_dwContStartPos + m_dwFileOffset;
	if(dwFilePos < dwContFilePos)
		return false;

	if(dwFilePos < dwContFilePos + GetContLenB())
		return true;

	return false;
}

DWORD CDFInfo2::ContPos2FilePos(DWORD dwContPos)
{
	CAutoLock lock(m_csFrag);

	if(!m_bOrdinal && dwContPos < m_dwContStartPos)
		return dwContPos + m_dwFileOffset + m_dwContSize;
	else
		return dwContPos + m_dwFileOffset;
}

DWORD CDFInfo2::GetContLenB()
{
	return m_bOrdinal ? (m_dwContEndPos - m_dwContStartPos) : 
		(m_dwContSize + m_dwContEndPos - m_dwContStartPos);
}

bool CDFInfo2::CanRestartDownloadThread(DWORD& dwFilePos)
{
	CAutoLock lock(m_csFrag);

	DWORD dwContLen = GetContLenB();
	voLog(LL_TRACE,"DFInfo.txt","dwFilePos=%d,dwContLen=%d,m_dwContSize=%d\n",dwFilePos,dwContLen,m_dwContSize);
	if(dwContLen < m_dwContSize * 3 / 4)
	{
		dwFilePos = m_dwContEndPos + m_dwFileOffset;
		if(m_dwContEndPos < m_dwContStartPos)		//如果结束点已经追赶到循环程度
			dwFilePos += m_dwContSize;
		return true;
	}

	return false;
}

bool CDFInfo2::ReadIn(PBYTE pSrc)
{
	if(!pSrc)
		return false;

	PDHFileMapping2 pfm;
	memcpy(&pfm, pSrc, sizeof(PDHFileMapping2));

	m_dwContSize = pfm.cont_size;
	m_dwContStartPos = pfm.cont_start_pos;
	m_dwContEndPos = pfm.cont_end_pos;
	m_dwFileOffset = pfm.file_offset;
	return true;
}

bool CDFInfo2::WriteOut(PBYTE pDst)
{
	if(!pDst)
		return false;

	PDHFileMapping2 pfm;
	pfm.cont_size = m_dwContSize;
	pfm.cont_start_pos = m_dwContStartPos;
	pfm.cont_end_pos = m_dwContEndPos;
	pfm.file_offset = m_dwFileOffset;

	memcpy(pDst, &pfm, sizeof(PDHFileMapping2));
	return true;
}