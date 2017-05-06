
#include "CBaseSink.h"

CBaseSink::CBaseSink()
	: m_dwSinkPos(0)
	, m_dwFileSize(0)
	, m_dwFileHeaderSize(0)
	, m_dwOffset(0)
	, m_dwErr(VOPD_SINK_SUCCESS)
{
}

CBaseSink::~CBaseSink()
{
	Close();
}

bool CBaseSink::Open(LPVOID lParam)
{
	return true;
}

void CBaseSink::Close()
{
}

bool CBaseSink::Sink(PBYTE pData, int nLen)
{
	return true;
}

bool CBaseSink::SetSinkPos(DWORD dwSinkPos)
{
	m_dwSinkPos = dwSinkPos;
	return true;
}

DWORD CBaseSink::GetSinkPos()
{
	return m_dwSinkPos;
}

DWORD CBaseSink::GetLastErr()
{
	DWORD dwErr = m_dwErr;

	m_dwErr = VOPD_SINK_SUCCESS;

	return dwErr;
}

bool CBaseSink::SetFileHeaderSize(DWORD dwFileHeaderSize)
{
	m_dwFileHeaderSize = dwFileHeaderSize;
	return true;
}

DWORD CBaseSink::GetFileHeaderSize()
{
	return m_dwFileHeaderSize;
}

bool CBaseSink::SetFileSize(DWORD dwFileSize)
{
	m_dwFileSize = dwFileSize;
	return true;
}

DWORD CBaseSink::GetFileSize()
{
	return m_dwFileSize;
}

bool CBaseSink::SetOffet(DWORD dwOffset)
{
	m_dwOffset = dwOffset;
	return true;
}