
#include "CMemStream.h"
#include "fMacros.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMemStream::CMemStream()
:m_pBuf(NULL)
,m_uBufLen(0)
,m_pBufPointer(NULL)
{

}

CMemStream::~CMemStream()
{

}
VO_U32 CMemStream::Init(VO_PBYTE pData ,VO_U64 uSize)
{
	m_pBuf = pData;
	m_uBufLen = uSize;
	m_pBufPointer = pData;
	return VO_RET_SOURCE2_OK;
}
VO_U32 CMemStream::Read(VO_PBYTE pOutBuf,VO_U64 uReadSize,VO_U64 * pActulReadSize)
{
	if (!pOutBuf)
	{
		return VO_RET_SOURCE2_OK;
	}
	if (m_pBufPointer  + uReadSize > m_pBuf + m_uBufLen)
	{
		return VO_RET_SOURCE2_NEEDRETRY;
	}
	memcpy(pOutBuf,m_pBufPointer,uReadSize);
	m_pBufPointer += uReadSize;
	if (pActulReadSize)
	{
		*pActulReadSize = uReadSize;
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CMemStream::Skip(VO_S64 uDelt)
{
	if (m_pBufPointer  + uDelt > m_pBuf + m_uBufLen)
	{
		return VO_RET_SOURCE2_NEEDRETRY;
	}
	m_pBufPointer += uDelt;
	return VO_RET_SOURCE2_OK;
}

VO_U32 CMemStream::GetPosition(VO_U64 &uCurPos)
{
	uCurPos = m_pBufPointer - m_pBuf;
	return VO_RET_SOURCE2_OK;
}

VO_U32 CMemStream::GetMemSize(VO_U64 &ullSize)
{
	ullSize = m_uBufLen;
	return VO_RET_SOURCE2_OK;
}



CMemFileIO::CMemFileIO()
:m_pBufIO(NULL)
,m_uBufSize(0)
,m_uAllocBufSize(0)
{
}

CMemFileIO::~CMemFileIO()
{
	if (m_uAllocBufSize && m_pBufIO)
	{
		delete m_pBufIO;
	} 
}
VO_U32 CMemFileIO::Open(VO_PBYTE pData ,VO_U64 ullSize)
{
	VO_PBYTE pMemBuf = NULL;
	VO_U64	ullMemSize = 0;
	if (m_uBufSize)///<there is data left from last input
	{
		pMemBuf = m_pBufIO;
		if(m_uBufSize + ullSize > m_uAllocBufSize)
		{
			m_uAllocBufSize = VO_MAX(m_uBufSize + ullSize,MIN_SIZE_MEMORY_BUFFER);
			if (m_uAllocBufSize >= MAX_SIZE_MEMORY_BUFFER)
			{
				VOLOGE("Need too much Buffer to cache the data");
				return VO_RET_SOURCE2_OPENFAIL;
			}
			m_pBufIO =  new VO_BYTE[m_uAllocBufSize];
			memcpy(m_pBufIO,pMemBuf,m_uBufSize);
			delete pMemBuf;
		}		
		memcpy(m_pBufIO+m_uBufSize,pData,ullSize);
		m_uBufSize += ullSize; 

		pMemBuf = m_pBufIO;
		ullMemSize = m_uBufSize;
	}
	else
	{
		pMemBuf = pData;
		ullMemSize = ullSize;
	}
	return m_Mem.Init(pMemBuf,ullMemSize);
}

VO_U32 CMemFileIO::Read(VO_PBYTE pBuffer, VO_U64 uSize,VO_U64 * pActulReadSize)
{
	if (!pBuffer)
	{
		return VO_RET_SOURCE2_OK;
	}
	return m_Mem.Read(pBuffer,uSize,pActulReadSize);
}
VO_U32 CMemFileIO::Write(VO_PBYTE pBuffer, VO_U64 uSize)
{
	return VO_RET_SOURCE2_OK;
}
VO_U32 CMemFileIO::Flush ()
{
	return VO_RET_SOURCE2_OK;
}
VO_U32 CMemFileIO::Seek (VO_S64 llDelta,VO_SOURCE2_IO_POS RelativePos)
{
	if (RelativePos == VO_SOURCE2_IO_POS_CURRENT)
	{
		return m_Mem.Skip(llDelta);
	}
	VO_U64 ullSize = 0;
	m_Mem.GetMemSize(ullSize);

	VO_U64 ullCurPos = 0;
	m_Mem.GetPosition(ullCurPos);

	if (RelativePos == VO_SOURCE2_IO_POS_BEGIN)
	{
		if (llDelta > ullSize)
		{
			return VO_RET_SOURCE2_NEEDRETRY;
		}
		return m_Mem.Skip(llDelta - ullCurPos);
	}
	return m_Mem.Skip(ullSize + llDelta - ullCurPos);
	
}
VO_U32 CMemFileIO::Size (VO_U64 &ullSize)
{
	return m_Mem.GetMemSize(ullSize);
}
VO_U32 CMemFileIO::Save()
{
	return VO_RET_SOURCE2_OK;
}

VO_U32 CMemFileIO::Close ()
{
	VO_U64 ullMemSize = 0;
	VO_U64 ullCurPos = 0;
	m_Mem.GetMemSize(ullMemSize);
	m_Mem.GetPosition(ullCurPos);

	if (ullMemSize < ullCurPos)
	{
		VOLOGE("the ullMemSize less than ullCurPos");
		return VO_RET_SOURCE2_OK;
	}
	if (ullMemSize > ullCurPos)
	{
		if (m_uAllocBufSize < ullMemSize - ullCurPos)
		{
			if (m_uAllocBufSize)
			{
				if (m_pBufIO)
				{
					delete m_pBufIO;
				}
			}
			m_uAllocBufSize = VO_MAX(VO_MAX(m_uAllocBufSize,ullMemSize - ullCurPos),MIN_SIZE_MEMORY_BUFFER);
			m_pBufIO = new VO_BYTE[m_uAllocBufSize];
		}
		VO_U64 ullReadSize = 0;
		VO_U32 ret = VO_RET_SOURCE2_OK;
		if (ret = m_Mem.Read(m_pBufIO,ullMemSize - ullCurPos,&ullReadSize) || ullReadSize != ullMemSize - ullCurPos)
		{
			VOLOGE("Read the left data failed");
		}
	}
	m_uBufSize = ullMemSize - ullCurPos;
	return VO_RET_SOURCE2_OK;
}

VO_U32 CMemFileIO::Position(VO_U64& ullPos)
{
	return m_Mem.GetPosition(ullPos);
}