#include <voPDPort.h>
#include <stdio.h>
#include "CMemSink.h"

extern int g_nLog;
void DumpLog(LPCSTR pLogInfo);

CMemSink::CMemSink()
	: CBaseSink()
	, m_pBuffer(NULL)
{
}

CMemSink::~CMemSink()
{
	Close();
}

bool CMemSink::Open(LPVOID lParam)
{
	Close();

	PVOMemSinkOpenParam pParam = (PVOMemSinkOpenParam)lParam;
	if(!pParam)
		return false;

	m_dwFileSize = pParam->memory_buffer_size;
	m_pBuffer = pParam->memory_buffer;

	return (NULL != m_pBuffer);
}

void CMemSink::Close()
{
}

bool CMemSink::Sink(PBYTE pData, int nLen)
{
	if(m_pBuffer)
	{
		if(m_dwSinkPos + nLen > m_dwFileSize)
		{
			PBYTE pSrc = pData;
			DWORD dwToWrite = m_dwFileSize - m_dwSinkPos;
			memcpy(m_pBuffer + m_dwSinkPos, pSrc, dwToWrite);
			pSrc += dwToWrite;

			dwToWrite = nLen - dwToWrite;
			m_dwSinkPos = m_dwFileHeaderSize;
			memcpy(m_pBuffer + m_dwSinkPos, pSrc, dwToWrite);
			m_dwSinkPos += dwToWrite;
		}
		else
		{
			memcpy(m_pBuffer + m_dwSinkPos, pData, nLen);
			m_dwSinkPos += nLen;
		}

		return true;
	}

	return false;
}
