#include "StreamCache.h"
#include <memory.h>
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef _DumpPC
	FILE *file = NULL;
#endif // _DumpPC

CStreamCache::CStreamCache(void)
: m_StateCache(Cache_Stop)
, iSizeBuf(1024 * 1024 * 3)
, iSizeCaching(1024 * 1024)
, m_pBuf(NULL)
, m_llFileSize(-1)
, m_llPosBegin(0)
, m_llPosEnd(0)
, m_llPosCur(0)
{
	m_pBuf = new VO_BYTE[iSizeBuf];
}

CStreamCache::~CStreamCache(void)
{
	delete []m_pBuf;

#ifdef _DumpPC
	if (file)
		fclose(file);
#endif // _DumpPC
}

VO_VOID CStreamCache::start()
{
#ifdef _DumpPC
	if (NULL == file)
		file = fopen("F:\\BC", "w+b");
#endif // _DumpPC

	VOLOGR("start");
	m_StateCache = Cache_Running;

	m_EventRead.Reset();
	m_EventWrite.Reset();
}

VO_VOID CStreamCache::close()
{
#ifdef _DumpPC
	if (file)
	{
		fclose(file);
		file = fopen("F:\\BC", "w+b");
	}
#endif // _DumpPC

	VOLOGR("close");
	m_StateCache = Cache_Stop;

	m_llPosBegin = m_llPosCur = m_llPosEnd = 0;
	m_llFileSize = -1;

	m_EventRead.Signal();
	m_EventWrite.Signal();
}

VO_VOID CStreamCache::pause()
{
	VOLOGR("pause");
	m_StateCache = Cache_Stop;

	m_EventRead.Signal();
	m_EventWrite.Signal();
}

VO_VOID CStreamCache::setFileSize(VO_S64 llSize)
{
	m_llFileSize = llSize;
}

VO_S64 CStreamCache::FileSize()
{
	return m_llFileSize;
}

VO_S32 CStreamCache::setFileEnd()
{
	setFileSize(m_llPosEnd);

	m_EventRead.Signal();

	return 0;
}

VO_S32 CStreamCache::read(VO_BYTE *toBuf, VO_U32 uiBufLen)
{
	//VOLOGR("PosBegin %llx PosCur %llx NeedLen %x", m_llPosBegin, m_llPosCur, uiBufLen);

	if (-1 != m_llFileSize && m_llPosCur >= m_llFileSize)
		return -1;

	VO_U32 uiRead = 0;
	while (Cache_Running == m_StateCache)
	{
		VO_S32 iRet = doread(toBuf + uiRead, uiBufLen - uiRead);
		if (iRet == -1) {
			VOLOGE("!doread");
			return uiRead;
		}

		m_EventWrite.Signal();

		if (iRet >= 0)
		{
			uiRead += iRet;
			if (uiRead == uiBufLen || m_llPosCur == m_llFileSize) {
				//VOLOGR("Done %x", uiRead);
				return uiRead;
			}
		}

		m_EventRead.Wait();
	};

	return uiRead;
}

VO_S32 CStreamCache::doread(VO_BYTE *toBuf, VO_U32 uiBufLen)
{
	voCAutoLock lock( &m_lock );

	if (m_llPosCur < m_llPosBegin - (iSizeCaching <= m_llPosBegin ? iSizeCaching : m_llPosBegin)
		|| m_llPosCur > m_llPosBegin + iSizeBuf - iSizeCaching)
	{
		m_StateCache = Cache_End;

		return -1;
	}

	VO_S32 iUsableLen = static_cast<VO_S32>(m_llPosEnd - m_llPosCur);
	if (iUsableLen <= 0)
		return 0;

	VO_S32 iActualSize = static_cast<VO_S32>(uiBufLen) < iUsableLen ? static_cast<VO_S32>(uiBufLen) : iUsableLen;
	VO_S32 iPos = m_llPosCur % iSizeBuf;
	VO_S32 iTrail = iSizeBuf - iPos;

	if (iTrail < iActualSize)
	{
		memcpy(toBuf, m_pBuf + iPos, iTrail);
		memcpy(toBuf + iTrail, m_pBuf, iActualSize - iTrail);
	}
	else
	{
		memcpy(toBuf, m_pBuf + iPos, iActualSize);
	}

	m_llPosCur += iActualSize;

	if (m_llPosBegin < m_llPosCur)
		m_llPosBegin = m_llPosCur;

	return iActualSize;
}

VO_S64 CStreamCache::seek(VO_S64 llOffset, VO_FILE_POS origin)
{
	//VOLOGR("stream_seek from %llx", m_llPosCur);

	switch (origin)
	{
	case VO_FILE_BEGIN:
		{
			m_llPosCur = llOffset;
			//VOLOGR(" to %llx", m_llPosCur);
		}
		break;

	case VO_FILE_CURRENT:
		{
			m_llPosCur += llOffset;
			//VOLOGR(" by %llx to %llx", llOffset, m_llPosCur);
		}
		break;

	case VO_FILE_END:
		{
			//VOLOGR("stream_seek form End!!");

			if ( -1 != FileSize() )
				m_llPosCur = FileSize() + llOffset;
			else
				return -1;
		}
		break;

	default:
		return -1;
	}

	if (m_llFileSize != -1 && m_llPosCur > m_llFileSize) //overflow
	{
		VOLOGW("seek overflow m_llPosCur %lld, m_llFileSize %lld", m_llPosCur, m_llFileSize);
		return -1;
	}
	else
		return m_llPosCur;
}

VO_S32 CStreamCache::write(const VO_BYTE *fromBuf, VO_U32 uiBufLen)
{
	//VOLOGR("%u", uiBufLen);
	if (0 == uiBufLen)
		return setFileEnd();

#ifdef _DumpPC
	fwrite(fromBuf, 1, uiBufLen, file);
	fflush(file);
#endif // _DumpPC

	VO_S32 iWritten = 0;
	
	while (Cache_Running == m_StateCache)
	{
		VO_S32 iRet = dowrite(fromBuf + iWritten, uiBufLen - iWritten);

		m_EventRead.Signal();

		if (iRet)
		{
			iWritten += iRet;
			if (static_cast<VO_S32>(uiBufLen) == iWritten)
			{
				//VOLOGR("Write %d Done", iWritten);
				return iWritten;
			}
		}

		m_EventWrite.Wait();
	};

	return iWritten;
}

VO_S32 CStreamCache::dowrite(const VO_BYTE *fromBuf, VO_U32 uiBufLen)
{
	voCAutoLock lock( &m_lock );

	VO_S32 iFreeSize = iSizeBuf  - iSizeCaching - static_cast<VO_S32>(m_llPosEnd - m_llPosBegin); //actually, iCaching <= nCaching_Size
	VO_S32 iActualSize = static_cast<VO_S32>(uiBufLen) > iFreeSize ? iFreeSize : static_cast<VO_S32>(uiBufLen);

	if (iActualSize > 0)
	{
		VO_S32 iEnd = m_llPosEnd % iSizeBuf;

		if (iSizeBuf - iEnd < iActualSize)
		{
			memcpy(m_pBuf + iEnd, fromBuf, iSizeBuf - iEnd);
			memcpy(m_pBuf, fromBuf + iSizeBuf - iEnd, iActualSize - iSizeBuf + iEnd);
		}
		else
		{
			memcpy(m_pBuf + iEnd, fromBuf, iActualSize);
		}

		m_llPosEnd += iActualSize;
	}

	return iActualSize;
}


VO_S32 CStreamCache::peek(VO_BYTE *toBuf, VO_U32 uiBufLen)
{
	VO_U32 uiPeek = 0;
	while (Cache_Running == m_StateCache)
	{
		VO_S32 iRet = dopeek(toBuf + uiPeek, uiBufLen - uiPeek);
		if (iRet == -1) {
			//VOLOGE("!dopeek");
			return -1;
		}

		uiPeek += iRet;

		if (uiPeek == uiBufLen || m_llPosCur == m_llFileSize)
			return uiPeek;

		m_EventRead.Wait();
	};

	return -1;
}

VO_S32 CStreamCache::dopeek(VO_BYTE *toBuf, VO_U32 uiBufLen)
{
	//voCAutoLock lock( &m_lock );

	VO_S32 iCaching = iSizeCaching <= m_llPosBegin ? iSizeCaching : static_cast<VO_S32>(m_llPosBegin);

	if (m_llPosCur < m_llPosBegin - iCaching)
		return -1;

	VO_S32 iUsableLen = static_cast<VO_S32>(m_llPosEnd - m_llPosCur);
	VO_S32 iActualSize = static_cast<VO_S32>(uiBufLen) < iUsableLen ? static_cast<VO_S32>(uiBufLen) : iUsableLen;

	if (iActualSize > 0)
	{
		VO_S32 iPos = m_llPosCur % iSizeBuf;
		VO_S32 iTrail = iSizeBuf - iPos;

		if (iTrail < iActualSize)
		{
			memcpy(toBuf, m_pBuf + iPos, iTrail);
			memcpy(toBuf + iTrail, m_pBuf, iActualSize - iTrail);
		}
		else
		{
			memcpy(toBuf, m_pBuf + iPos, iActualSize);
		}
	}

	return iActualSize;
}