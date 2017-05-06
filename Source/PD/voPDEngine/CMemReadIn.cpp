#include <voPDPort.h>
#include "CMemReadIn.h"

CMemReadIn::CMemReadIn()
	: CBaseReadIn()
	, m_pBuffer(NULL)
	, m_dwBufferSize(0)
	, m_dwReadPos(0)
{
}

CMemReadIn::~CMemReadIn()
{
	Close();
}

bool CMemReadIn::Open(LPVOID lParam)
{
	PVOMemReadInOpenParam pParam = (PVOMemReadInOpenParam)lParam;
	if(!pParam)
		return false;

	m_pBuffer = pParam->memory_buffer;
	m_dwBufferSize = pParam->memory_buffer_size;

	return (NULL != m_pBuffer);
}

int CMemReadIn::SetReadPos(DWORD dwReadPos, DWORD dwMoveMethod)
{
	switch(dwMoveMethod)
	{
	case FILE_BEGIN:
		m_dwReadPos = dwReadPos;
		break;
	case FILE_CURRENT:
		m_dwReadPos += dwReadPos;
		break;
	case FILE_END:
		m_dwReadPos = (m_dwBufferSize - dwReadPos);
	    break;
	default:
	    return -1;
	}
	return int(m_dwReadPos);
}

bool CMemReadIn::ReadIn(PBYTE pData, int nLen, int& nReaded)
{
	if(!m_pBuffer)
		return false;

	nReaded = nLen;
	if(nReaded > m_dwBufferSize - m_dwReadPos)
		nReaded = m_dwBufferSize - m_dwReadPos;

	if(nReaded <= 0)
		return false;

	memcpy(pData, m_pBuffer + m_dwReadPos, nReaded);
	m_dwReadPos += nReaded;
	return true;
}