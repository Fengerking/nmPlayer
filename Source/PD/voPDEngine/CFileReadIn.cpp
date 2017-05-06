#include <voPDPort.h>
#include <stdio.h>
#include "CFileReadIn.h"

void DumpErrLog(LPCSTR pLogInfo);

CFileReadIn::CFileReadIn()
	: CBaseReadIn()
	, m_hFile(NULL)
{
}

CFileReadIn::~CFileReadIn()
{
	Close();
}

bool CFileReadIn::Open(LPVOID lParam)
{
	//m_hFile = CreateFile((LPCTSTR)lParam, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
	//	NULL, OPEN_EXISTING, 0, NULL);
#ifdef UNICODE
	wcstombs(m_fileName,(LPCTSTR)lParam,MAX_FNAME_LEN);
#else
	tcscpy(m_fileName,(LPCTSTR)lParam);
#endif
	m_hFile = fopen(m_fileName,"rb");
	//if(INVALID_HANDLE_VALUE == m_hFile)
	//	m_hFile = NULL;

	return (NULL != m_hFile);
}

void CFileReadIn::Close()
{
	if(m_hFile)
	{
		//CloseHandle(m_hFile);
		fclose(m_hFile);
		m_hFile = NULL;
	}
}

int CFileReadIn::SetReadPos(DWORD dwReadPos, DWORD dwMoveMethod)
{
	if(!m_hFile)
		return false;

	//DWORD dwFilePos = SetFilePointer(m_hFile, dwReadPos, NULL, dwMoveMethod);
	//if(0xFFFFFFFF == dwFilePos)
	if(fseek(m_hFile,dwReadPos,dwMoveMethod))//note:the seek method in filebase.h is greater than stdio.h by one 
	{
		char sz[256];
		sprintf(sz, "[CFileReadIn::SetReadPos]SetFilePointer fail. dwReadPos: %d, dwMoveMethod: %d \r\n", dwReadPos, dwMoveMethod);
		DumpErrLog(sz);
		return -1;
	}

	return int(ftell(m_hFile));
}

bool CFileReadIn::ReadIn(PBYTE pData, int nLen, int& nReaded)
{
	if(!m_hFile)
		return false;
	DWORD dwReaded = 0;
	//if(!ReadFile(m_hFile, pData, nLen, &dwReaded, NULL))
	dwReaded=fread(pData,1,nLen,m_hFile);
	if(ferror(m_hFile))
	{
		char sz[256];
		sprintf(sz, "[CFileReadIn::ReadIn]ReadFile fail\r\n");
		DumpErrLog(sz);
		return false;
	}

	nReaded = dwReaded;
	return true;
}
