#include <voPDPort.h>
#include <stdio.h>
#include "CFileSink.h"
#include "VOUtility.h"
using namespace VOUtility;
extern int g_nLog;
void DumpLog(LPCSTR pLogInfo);

CFileSink::CFileSink()
	: CBaseSink()
	, m_hFile(NULL)
{
}

CFileSink::~CFileSink()
{
	Close();
}

bool CFileSink::Open(LPVOID lParam)
{
	Close();

	PVOFileSinkOpenParam pParam = (PVOFileSinkOpenParam)lParam;
	if(!pParam)
		return false;
#ifdef UNICODE
	wcstombs(m_fileName,pParam->file_path,MAX_FNAME_LEN);
#else
	tcscpy(m_fileName,(const char*)pParam->file_path);
#endif
#ifdef LINUX
	m_hFile =  fopen(m_fileName,"w+");	
#else//LINUX
	m_hFile =  fopen(m_fileName,"wb");//CreateFile(pParam->file_path, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
#endif//LINUX		
	if(!m_hFile)
	{
		voLog(LL_DEBUG,"PDSource.txt","open file fail file=%s,err=%d\n",m_fileName,GetLastError2());
		m_dwErr = VOPD_SINK_ERR_CREATE_FAIL;
		return false;
	}

#ifdef _HTC
	//HTC want their downloaded file is hidden!!
	if(pParam->if_create)
		SetFileAttributes(pParam->file_path, FILE_ATTRIBUTE_HIDDEN);
#endif	//_HTC

	m_dwFileSize = pParam->file_size;


	return true;
}

void CFileSink::Close()
{
	if(m_hFile)
	{
		fclose(m_hFile);
		m_hFile = NULL;
	}
}

bool CFileSink::Sink(PBYTE pData, int nLen)
{
	if(m_hFile)
	{
		DWORD dwWrited = 0;
		bool bWriteOK=true;
		if(m_dwOffset + m_dwSinkPos + nLen > m_dwFileSize)
		{
			DWORD dwToWrite = m_dwFileSize - (m_dwOffset + m_dwSinkPos);
			//if(!WriteFile(m_hFile, pData, dwToWrite, &dwWrited, NULL))
			dwWrited=fwrite( pData, 1,dwToWrite, m_hFile);
			bWriteOK=dwToWrite==dwWrited;
			if(!bWriteOK)
			{
				if(g_nLog)
				{
					char sz[256];
					sprintf(sz, "[sink error1-1]file position: %d, len: %d,actual=%d\r\n", m_dwSinkPos, dwToWrite,dwWrited);
					DumpLog(sz);
				}
				m_dwErr = VOPD_SINK_ERR_WRITE_FAIL;
				return false;
			}

			dwToWrite = m_dwOffset + m_dwSinkPos + nLen - m_dwFileSize;
			m_dwSinkPos = m_dwFileHeaderSize;
			
			//if(0xFFFFFFFF == SetFilePointer(m_hFile, m_dwSinkPos + m_dwOffset, NULL, FILE_BEGIN))
			if(fseek(m_hFile,m_dwSinkPos + m_dwOffset,SEEK_SET))
			{
				if(g_nLog)
				{
					char sz[256];
					sprintf(sz, "[sink error1-2]seek position: %d\r\n", m_dwFileHeaderSize);
					DumpLog(sz);
				}
				m_dwErr = VOPD_SINK_ERR_SEEK_FAIL;
				return false;
			}

			//if(!WriteFile(m_hFile, pData + dwWrited, dwToWrite, &dwWrited, NULL))
			dwWrited=fwrite( pData + dwWrited, 1,dwToWrite, m_hFile);
			bWriteOK=dwToWrite==dwWrited;
			if(!bWriteOK)
			{
				if(g_nLog)
				{
					char sz[256];
					sprintf(sz, "[sink error1-3]file position: %d, len: %d\r\n", m_dwSinkPos, dwToWrite);
					DumpLog(sz);
				}
				m_dwErr = VOPD_SINK_ERR_WRITE_FAIL;
				return false;
			}
			m_dwSinkPos += dwWrited;
		}
		else
		{
			//if(!WriteFile(m_hFile, pData, nLen, &dwWrited, NULL))
#if 0//test
			//fclose(m_hFile);
			//fopen("c:/visualon/vopd/shit.mp4","w");
			int pos1=ftell(m_hFile);
			dwWrited=fwrite( pData, 1,nLen, m_hFile);
			fflush(m_hFile);
			int pos2=ftell(m_hFile);
			int size2=pos2-pos1;
			
#else
			dwWrited=fwrite( pData, 1,nLen, m_hFile);
			fflush(m_hFile);
#endif
			bWriteOK=nLen==dwWrited;
			if(!bWriteOK)
			{
				if(g_nLog)
				{
					char sz[256];
					sprintf(sz, "[sink error2]file position: %d, len: %d actual=%d\r\n", m_dwSinkPos, nLen,dwWrited);
					DumpLog(sz);
				}
				m_dwErr = VOPD_SINK_ERR_WRITE_FAIL;
				return false;
			}
			m_dwSinkPos += dwWrited;
		}

		return true;
	}

	return false;
}

bool CFileSink::SetSinkPos(DWORD dwSinkPos)
{
	if(!m_hFile)
		return false;

	//if(0xFFFFFFFF == SetFilePointer(m_hFile, dwSinkPos + m_dwOffset, NULL, FILE_BEGIN))
	if(fseek(m_hFile,dwSinkPos + m_dwOffset,SEEK_SET))
	{
		m_dwErr = VOPD_SINK_ERR_SEEK_FAIL;
		return false;
	}

	return CBaseSink::SetSinkPos(dwSinkPos);
}