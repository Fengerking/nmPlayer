#include "CGBuffer.h"
#include "fMacros.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CGBuffer::CGBuffer(VO_MEM_OPERATOR* pMemOp)
	: CvoBaseMemOpr(pMemOp)
	, m_pBuffer(VO_NULL)
	, m_dwSize(0)
	, m_dwStart(0)
	, m_dwEnd(0)
	, m_dwIdle(0)
{
}

CGBuffer::~CGBuffer()
{
	Release();
}

VO_BOOL CGBuffer::Init(VO_U32 dwBufferSize)
{
	if(m_pBuffer)
		MemFree(m_pBuffer);

	//force multiple of four
	dwBufferSize = (dwBufferSize + 0x3) & ~0x3;

	m_pBuffer = NEW_BUFFER(dwBufferSize);
	if(!m_pBuffer)
		return VO_FALSE;

	m_dwSize = dwBufferSize;
	m_dwStart = 0;
	m_dwEnd = 0;
	m_dwIdle = m_dwSize;

	return VO_TRUE;
}

VO_VOID CGBuffer::Release()
{
	SAFE_MEM_FREE(m_pBuffer);
	m_dwSize = 0;
}

VO_VOID CGBuffer::Flush()
{
	m_dwStart = 0;
	m_dwEnd = 0;
	m_dwIdle = m_dwSize;
}

VO_U32 CGBuffer::Add(CGFileChunk* pFileChunk, VO_U32 dwLen)
{
	if(m_dwEnd + dwLen >= m_dwSize)
	{
		if(!pFileChunk->FRead(m_pBuffer + m_dwEnd, m_dwSize - m_dwEnd))
			return VO_MAXU32;

		if(!pFileChunk->FRead(m_pBuffer, dwLen + m_dwEnd - m_dwSize))
			return VO_MAXU32;
	}
	else
	{
		if(!pFileChunk->FRead(m_pBuffer + m_dwEnd, dwLen))
			return VO_MAXU32;
	}

	return Use(dwLen);
}

VO_U32 CGBuffer::Use(VO_U32 dwLen)
{
	if(dwLen == 0)
		return m_dwEnd;

	VO_U32 dwPos = m_dwEnd;

	m_dwEnd += dwLen;
	if(m_dwEnd >= m_dwSize)
		m_dwEnd -= m_dwSize;

	m_dwIdle = (m_dwEnd > m_dwStart) ? (m_dwSize + m_dwStart - m_dwEnd) : (m_dwStart - m_dwEnd);

	return dwPos;
}

VO_U32 CGBuffer::GetForDirectWrite(VO_PBYTE* ppPtr, VO_U32 dwLen)
{
	*ppPtr = m_pBuffer + m_dwEnd;

	return Use(dwLen);
}

VO_BOOL CGBuffer::DirectWrite(CGFileChunk* pFileChunk, VO_PBYTE pPtr, VO_U32 dwOffset, VO_U32 dwLen)
{
	VO_PBYTE pWritePtr = pPtr + dwOffset;
	if(pWritePtr >= m_pBuffer + m_dwSize)
		pWritePtr -= m_dwSize;

	if(pWritePtr + dwLen >= m_pBuffer + m_dwSize)
	{
		if(!pFileChunk->FRead(pWritePtr, m_pBuffer + m_dwSize - pWritePtr))
			return VO_FALSE;

		if(!pFileChunk->FRead(m_pBuffer, pWritePtr + dwLen - m_pBuffer - m_dwSize))
			return VO_FALSE;
	}
	else
	{
		if(!pFileChunk->FRead(pWritePtr, dwLen))
			return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CGBuffer::DirectWrite2(VO_PBYTE pSource, VO_PBYTE pPtr, VO_U32 dwOffset, VO_U32 dwLen)
{
	VO_PBYTE pWritePtr = pPtr + dwOffset;
	if(pWritePtr >= m_pBuffer + m_dwSize)
		pWritePtr -= m_dwSize;

	if(pWritePtr + dwLen >= m_pBuffer + m_dwSize)
	{
		MemCopy(pWritePtr, pSource, m_pBuffer + m_dwSize - pWritePtr);
		MemCopy(m_pBuffer, pSource + (m_pBuffer + m_dwSize - pWritePtr), pWritePtr + dwLen - m_pBuffer - m_dwSize);
	}
	else
		MemCopy(pWritePtr, pSource, dwLen);

	return VO_TRUE;
}

VO_VOID CGBuffer::RemoveFrom(VO_U32 dwEnd)
{
	m_dwEnd = dwEnd;
	m_dwIdle = (m_dwEnd > m_dwStart) ? (m_dwSize + m_dwStart - m_dwEnd) : (m_dwStart - m_dwEnd);
}

VO_VOID CGBuffer::RemoveTo(VO_U32 dwStart)
{
	m_dwStart = dwStart;
	m_dwIdle = (m_dwEnd >= m_dwStart) ? (m_dwSize + m_dwStart - m_dwEnd) : (m_dwStart - m_dwEnd);
}

VO_VOID CGBuffer::RemoveTo2(VO_U32 dwPos, VO_U32 dwLen)
{
	VO_U32 dwStart = (dwPos + dwLen > m_dwSize) ? (dwPos + dwLen - m_dwSize) : (dwPos + dwLen);
	RemoveTo(dwStart);
}

VO_S32 CGBuffer::Get(VO_PBYTE* ppPtr, VO_PBYTE pPtr, VO_U32 dwPos, VO_U32 dwLen)
{
	m_dwStart = dwPos;
	m_dwIdle = (m_dwEnd >= m_dwStart) ? (m_dwSize + m_dwStart - m_dwEnd) : (m_dwStart - m_dwEnd);

	if(dwPos + dwLen > m_dwSize)
	{
		MemCopy(pPtr, m_pBuffer + dwPos, m_dwSize - dwPos);
		MemCopy(pPtr + m_dwSize - dwPos, m_pBuffer, dwPos + dwLen - m_dwSize);

		return 2;
	}

	*ppPtr = m_pBuffer + dwPos;
	return 1;
}

VO_S32 CGBuffer::Peek(VO_PBYTE* ppPtr, VO_PBYTE pPtr, VO_U32 dwPos, VO_U32 dwLen)
{
	if(dwPos + dwLen > m_dwSize)
	{
		MemCopy(pPtr, m_pBuffer + dwPos, m_dwSize - dwPos);
		MemCopy(pPtr + m_dwSize - dwPos, m_pBuffer, dwPos + dwLen - m_dwSize);

		return 2;
	}

	*ppPtr = m_pBuffer + dwPos;
	return 1;
}

VO_BOOL CGBuffer::HasIdleBuffer(VO_U32 dwLen)
{
	return (m_dwIdle < dwLen) ? VO_FALSE : VO_TRUE;
}