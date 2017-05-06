#include "StreamCache.h"
#include <memory.h>
#include "voLog.h"
#include "voOSFunc.h"

#define nBuf_Size (1024 * 800)
#define nCaching_Size (1024 * 100)

CStreamCache::CStreamCache(void)
	: m_llPosBegin(0)
	, m_llPosEnd(0)
	, m_llPosCur(0)
	, m_bRunning(VO_TRUE)
	, m_llFileSize(-1)
	, m_pBuf(NULL)
	, m_pFreedRanges(NULL)
{
	m_pBuf = new VO_BYTE[nBuf_Size];
}

CStreamCache::~CStreamCache(void)
{
	delete []m_pBuf;

	if (NULL == m_pFreedRanges)
		delete m_pFreedRanges;
}

VO_VOID CStreamCache::open()
{
	m_llPosBegin = m_llPosCur = m_llPosEnd = 0;
	m_llFileSize = -1;

	m_bRunning = VO_TRUE;

	m_event.Reset();
}

VO_VOID CStreamCache::close()
{
	m_bRunning = VO_FALSE;

	m_event.Signal();

	voOS_Sleep(10);

	if (NULL == m_pFreedRanges) {
		delete m_pFreedRanges;
		m_pFreedRanges = NULL;
	}
}

VO_VOID CStreamCache::setFileSize(VO_S64 llSize)
{
	voCAutoLock lock( &m_lock );

	m_llFileSize = llSize;
}

VO_S64 CStreamCache::FileSize()
{
	voCAutoLock lock( &m_lock );

	return m_llFileSize;
}

VO_S32 CStreamCache::setFileEnd()
{
	setFileSize(m_llPosEnd);

	m_event.Signal();

	return 0;
}

VO_S32 CStreamCache::read(VO_BYTE *toBuf, VO_U32 uiBufLen)
{
	VOLOGI("PosBegin %llx PosCur %llx NeedLen %x\t\n", m_llPosBegin, m_llPosCur, uiBufLen);

	VO_U32 uiRead = 0;
	while (m_bRunning)
	{
		VO_S32 iRet = doread(toBuf + uiRead, uiBufLen - uiRead);
		if (iRet == -1) {
			VOLOGI("!doread");
			return uiRead;
		}

		uiRead += iRet;

		m_event.Signal();

		if (uiRead == uiBufLen || m_llPosCur == m_llFileSize)
			return uiRead;

		m_event.Reset();
		m_event.Wait();
	};

	return 0;
}

VO_S32 CStreamCache::doread(VO_BYTE *toBuf, VO_U32 uiBufLen)
{
	voCAutoLock lock( &m_lock );

	VO_S32 iCaching = nCaching_Size <= m_llPosBegin ? nCaching_Size : m_llPosBegin;

	if (m_llPosCur < m_llPosBegin - iCaching || m_llPosCur > m_llPosBegin + nBuf_Size - nCaching_Size)
	{
		m_bRunning = VO_FALSE;

		return -1;
	}

	VO_S32 iLen = m_llPosEnd - m_llPosCur;

	if ( iLen > 0)
		return readBuf(toBuf, uiBufLen < iLen ? uiBufLen : iLen);

	return 0;
}

VO_S32 CStreamCache::readBuf(VO_BYTE *toBuf, VO_U32 uiBufLen)
{
	VO_S32 iPos = m_llPosCur % nBuf_Size;
	VO_S32 iTrail = nBuf_Size - iPos;

	if (iTrail < uiBufLen)
	{
		memcpy(toBuf, m_pBuf + iPos, iTrail);
		memcpy(toBuf + iTrail, m_pBuf, uiBufLen - iTrail);
	}
	else
	{
		memcpy(toBuf, m_pBuf + iPos, uiBufLen);
	}

	LogFreedRange(uiBufLen);

	return uiBufLen;
}

VO_VOID CStreamCache::LogFreedRange(VO_U32 uiBufLen)
{
	FreedRange *pPreFreed = NULL;
	FreedRange *pFreed = m_pFreedRanges;

	if (m_llPosCur <= m_llPosBegin)	//remove
	{
		m_llPosCur += uiBufLen;

		while (NULL != pFreed)
		{
			if (pFreed->iBegin == m_llPosCur)
			{
				m_llPosCur += pFreed->iLen;

				if (pPreFreed)
					pPreFreed->pNextFreedRange = pFreed->pNextFreedRange;
				else
					m_pFreedRanges = pFreed->pNextFreedRange;

				pFreed->pNextFreedRange = NULL;
				delete pFreed;

				pPreFreed = NULL;	//from begin
				pFreed = m_pFreedRanges;
			}
			else
			{
				pPreFreed = pFreed;
				pFreed = pFreed->pNextFreedRange;
			}
		};

		m_llPosBegin = m_llPosCur;
	}
	else	//add
	{
		while (NULL != pFreed)
		{
			if (pFreed->iBegin + pFreed->iLen == m_llPosCur)
				break;

			pPreFreed = pFreed;
			pFreed = m_pFreedRanges->pNextFreedRange;
		};

		if (NULL == pFreed)//need new
		{
			if (pPreFreed)
			{
				pPreFreed->pNextFreedRange = new FreedRange();

				pFreed = pPreFreed->pNextFreedRange;
			}
			else
			{
				m_pFreedRanges = new FreedRange();

				pFreed = m_pFreedRanges;
			}

			pFreed->iBegin = m_llPosCur;
		}

		pFreed->iLen += uiBufLen;

		m_llPosCur += uiBufLen;
	}
}

VO_S64 CStreamCache::seek(VO_S64 llOffset, VO_FILE_POS origin)
{
	VOLOGI("stream_seek from %llx", m_llPosCur);

	switch (origin)
	{
	case VO_FILE_BEGIN:
		{
			m_llPosCur = llOffset;
			VOLOGI(" to %llx", m_llPosCur);
			break;
		}
	case VO_FILE_CURRENT:
		{
			m_llPosCur += llOffset;
			VOLOGI(" by %llx to %llx", llOffset, m_llPosCur);

			break;
		}
	case VO_FILE_END:
		{
			VOLOGI("stream_seek from End!!", m_llPosCur);

			if ( -1 != FileSize() )
				m_llPosCur = FileSize() + llOffset;
			else
				return -1;

			break;
		}
	}

	if (m_llFileSize != -1 && m_llPosCur > m_llFileSize) //overflow
		return -1;
	else
		return m_llPosCur;
}

VO_S32 CStreamCache::write(const VO_BYTE *fromBuf, VO_U32 uiBufLen)
{
	if (0 == uiBufLen)
		return setFileEnd();

	VO_S32 iWritten = 0;
	while (m_bRunning)
	{
		VO_S32 iRet = dowrite(fromBuf + iWritten, uiBufLen - iWritten);

		iWritten += iRet;

		m_event.Signal();

		if (iWritten == uiBufLen)
			return iWritten;

		m_event.Reset();
		m_event.Wait();
	};

	return 0;
}

VO_S32 CStreamCache::dowrite(const VO_BYTE *fromBuf, VO_U32 uiBufLen)
{
	voCAutoLock lock( &m_lock );

	VO_S32 iEmpty = nBuf_Size  - nCaching_Size - (m_llPosEnd - m_llPosBegin); //actually, iCaching <= nCaching_Size
	if (iEmpty > 0)
		return writeBuf(fromBuf, uiBufLen > iEmpty ? iEmpty : uiBufLen);

	return 0;
}

VO_S32 CStreamCache::writeBuf(const VO_BYTE *fromBuf, VO_U32 uiBufLen)
{
	VO_S32 iEnd = m_llPosEnd % nBuf_Size;

	if (nBuf_Size - iEnd < uiBufLen)
	{
		memcpy(m_pBuf + iEnd, fromBuf, nBuf_Size - iEnd);
		memcpy(m_pBuf, fromBuf + nBuf_Size - iEnd, uiBufLen - nBuf_Size + iEnd);
	}
	else
	{
		memcpy(m_pBuf + iEnd, fromBuf, uiBufLen);
	}

	m_llPosEnd += uiBufLen;

	return uiBufLen;
}

VO_S32 CStreamCache::peek(VO_BYTE *toBuf, VO_U32 uiBufLen)
{
	VO_U32 uiPeek = 0;
	while (m_bRunning)
	{
		VO_S32 iRet = dopeek(toBuf + uiPeek, uiBufLen - uiPeek);
		if (iRet == -1) {
			VOLOGI("!doread");
			return -1;
		}

		uiPeek += iRet;

		m_event.Signal();

		if (uiPeek == uiBufLen || m_llPosCur == m_llFileSize)
			return uiPeek;

		m_event.Reset();
		m_event.Wait();
	};

	return -1;
}

VO_S32 CStreamCache::dopeek(VO_BYTE *toBuf, VO_U32 uiBufLen)
{
	voCAutoLock lock( &m_lock );

	VO_S32 iCaching = nCaching_Size <= m_llPosBegin ? nCaching_Size : m_llPosBegin;

	if (m_llPosCur < m_llPosBegin - iCaching)
		return -1;

	VO_S32 iLen = m_llPosEnd - m_llPosCur;

	if ( iLen > 0)
		return peekBuf(toBuf, uiBufLen < iLen ? uiBufLen : iLen);

	return 0;
}

VO_S32 CStreamCache::peekBuf(VO_BYTE *toBuf, VO_U32 uiBufLen)
{
	VO_S32 iPos = m_llPosCur % nBuf_Size;
	VO_S32 iTrail = nBuf_Size - iPos;

	if (iTrail < uiBufLen)
	{
		memcpy(toBuf, m_pBuf + iPos, iTrail);
		memcpy(toBuf + iTrail, m_pBuf, uiBufLen - iTrail);
	}
	else
	{
		memcpy(toBuf, m_pBuf + iPos, uiBufLen);
	}

	return uiBufLen;
}
