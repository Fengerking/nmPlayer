#include <memory.h>
#include "PacketBuffer.h"

#if defined LINUX
#include <string.h>
#endif

CPacketBuffer::CPacketBuffer(void)
:m_id(INVALID_ID)
, m_nMaxSize(0)
, m_cSize(0)
, m_pBuffer(0)
{
}

CPacketBuffer::~CPacketBuffer(void)
{
	Release();
}

bool CPacketBuffer::SetMaxSize(int size)
{
	Release();
	if (size <= 0)
		return true;
	m_pBuffer = new unsigned char[size];
	if (m_pBuffer == 0)
		return false;
	m_nMaxSize = size;
	return true;
}

void CPacketBuffer::Release()
{
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = 0;
		m_cSize = 0;
		m_nMaxSize = 0;
	}
}

bool CPacketBuffer::AddData(const unsigned char * pData, int size)
{
	if (m_cSize + size > m_nMaxSize)
		return false;
	memcpy(m_pBuffer + m_cSize, pData, size);
	m_cSize += size;
	return true;
}
