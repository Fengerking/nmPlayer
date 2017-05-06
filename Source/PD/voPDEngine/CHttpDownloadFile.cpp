#include <voPDPort.h>
#include <tchar.h>
#include "CHttpDownloadFile.h"

DWORD CHttpDownloadFile::g_dwOffset = 0;
CNetGet2* CHttpDownloadFile::g_pNetGet = NULL;

CHttpDownloadFile::CHttpDownloadFile()
	: m_dwFilePos(0)
	, m_pReadIn(NULL)
{
}

CHttpDownloadFile::~CHttpDownloadFile()
{
	Close();
}

bool CHttpDownloadFile::Open(LPCTSTR szFile, VOFILEOPMODE opMode)
{
	Close();

	if(!g_pNetGet)
		return false;

	if(!g_pNetGet->GenerateReadIn(szFile, &m_pReadIn) || !m_pReadIn)
		return false;

	//set file start pos!
	Seek(0, FS_BEGIN);

	return true;
}

void CHttpDownloadFile::Close()
{
	if(m_pReadIn)
	{
		m_pReadIn->Close();
		delete m_pReadIn;
		m_pReadIn = NULL;
	}
}

int CHttpDownloadFile::Read(PBYTE pBuffer, DWORD dwToRead, DWORD* pdwReaded)
{
	if(!g_pNetGet || !m_pReadIn)
		return -1;

	CDFInfo* pDfInfo = g_pNetGet->GetDFInfo();
	CDFInfo2* pDfInfo2 = g_pNetGet->GetDFInfo2();
	if(pDfInfo)
	{
		if(!pDfInfo->CanRead(m_dwFilePos - g_dwOffset, dwToRead))
		{
			int fileSize=pDfInfo->GetFileSize();
			int isFinished=g_pNetGet->IsFinish();
			int isOverSize= (fileSize<m_dwFilePos - g_dwOffset + dwToRead||isFinished)&&m_dwFilePos!=0;
			if(isOverSize)
				return -1;
			else if (isFinished)
			{
				goto FINISH_TRY;
			}

			IVOThread::Sleep(1);
			return -2;
		}
FINISH_TRY:
		int nReaded = 0;
		m_pReadIn->ReadIn(pBuffer, dwToRead, nReaded);

		m_dwFilePos += nReaded;
		if(pdwReaded)
			*pdwReaded = nReaded;
		return nReaded;
	}
	else if(pDfInfo2)
	{
		DWORD dwFileHeaderSize = g_pNetGet->GetFileHeaderSize();
		int nReaded = 0;
		if(m_dwFilePos - g_dwOffset + dwToRead <= dwFileHeaderSize)		//read file header
		{
			m_pReadIn->SetReadPos(m_dwFilePos, FILE_BEGIN);
			m_pReadIn->ReadIn(pBuffer, dwToRead, nReaded);

			m_dwFilePos += nReaded;
			if(pdwReaded)
				*pdwReaded = nReaded;
			return nReaded;
		}
		else												//read media data
		{
			DWORD dwContStart = 0;
			if(m_dwFilePos - g_dwOffset < dwFileHeaderSize)				//some data in file header
			{
				if(!pDfInfo2->CanRead(dwFileHeaderSize, m_dwFilePos - g_dwOffset + dwToRead - dwFileHeaderSize, dwContStart))
				{
					if(g_pNetGet->IsFinish())
						return -1;

					IVOThread::Sleep(1);
					return -2;
				}

				//read data in file header
				m_pReadIn->SetReadPos(m_dwFilePos, FILE_BEGIN);
				m_pReadIn->ReadIn(pBuffer, dwToRead, nReaded);

				m_dwFilePos += nReaded;
				if(pdwReaded)
					*pdwReaded = nReaded;

				if(dwFileHeaderSize > 0)
					g_pNetGet->UpdateFilePos(m_dwFilePos - g_dwOffset);
				return nReaded;
			}
			else
			{
				if(!pDfInfo2->CanRead(m_dwFilePos - g_dwOffset, dwToRead, dwContStart))
				{
					if(pDfInfo2->GetFileSize() < m_dwFilePos - g_dwOffset + dwToRead || g_pNetGet->IsFinish())
						return -1;

					IVOThread::Sleep(1);
					return -2;
				}

				m_pReadIn->SetReadPos(g_dwOffset + dwFileHeaderSize + dwContStart, FILE_BEGIN);
				DWORD dwContSize = pDfInfo2->GetContSize();
				if(dwContStart + dwToRead <= dwContSize)
				{
					m_pReadIn->ReadIn(pBuffer, dwToRead, nReaded);

					m_dwFilePos += nReaded;
					if(pdwReaded)
						*pdwReaded = nReaded;

					if(dwFileHeaderSize > 0)
						g_pNetGet->UpdateFilePos(m_dwFilePos - g_dwOffset);
					return nReaded;
				}
				else										//后面的数据在content的前面，所以我们需要定位到前面再读
				{
					PBYTE pTmp = pBuffer;
					//顺序读
					m_pReadIn->ReadIn(pTmp, dwContSize - dwContStart + g_dwOffset, nReaded);

					m_dwFilePos += nReaded;
					if(pdwReaded)
						*pdwReaded = nReaded;

					//定位到content首部继续读
					dwToRead -= nReaded;
					pTmp += nReaded;
					m_pReadIn->SetReadPos(g_dwOffset + dwFileHeaderSize, FILE_BEGIN);
					m_pReadIn->ReadIn(pTmp, dwToRead, nReaded);

					m_dwFilePos += nReaded;
					if(pdwReaded)
						*pdwReaded += nReaded;

					if(dwFileHeaderSize > 0)
						g_pNetGet->UpdateFilePos(m_dwFilePos - g_dwOffset);
					return nReaded + (pTmp - pBuffer);
				}
			}
		}
	}
	else
		return -1;
}

int CHttpDownloadFile::Seek(DWORD dwOffset, VOFILESEEKPOS fsPos)
{
	if(!g_pNetGet || !m_pReadIn)
		return -1;

	CDFInfo* pDfInfo = g_pNetGet->GetDFInfo();
	CDFInfo2* pDfInfo2 = g_pNetGet->GetDFInfo2();
	if(pDfInfo)
	{
		DWORD dwMoveMethod = -1;
		if(fsPos == FS_BEGIN)
		{
			dwMoveMethod = FILE_BEGIN;
			dwOffset += g_dwOffset;
		}
		else if(fsPos == FO_CURRENT)
			dwMoveMethod = FILE_CURRENT;
		else if(fsPos == FO_END)
			dwMoveMethod = FILE_END;

		int nRet = m_pReadIn->SetReadPos(dwOffset, dwMoveMethod);
		if(-1 != nRet)
			m_dwFilePos = nRet;

		return nRet;
	}
	else if(pDfInfo2)
	{
		if(fsPos == FS_BEGIN)
			m_dwFilePos = g_dwOffset + dwOffset;
		else if (fsPos == FO_CURRENT)
			m_dwFilePos += dwOffset;
		else if (fsPos == FO_END)
			m_dwFilePos = g_dwOffset + pDfInfo2->GetFileSize() - dwOffset;

		return int(m_dwFilePos);
	}
	else
		return -1;
}

int CHttpDownloadFile::Size(DWORD* pHighSize)
{
	if(!g_pNetGet || !m_pReadIn)
		return -1;

	CDFInfo* pDfInfo = g_pNetGet->GetDFInfo();
	CDFInfo2* pDfInfo2 = g_pNetGet->GetDFInfo2();
	int filesize=-1;
	if(pDfInfo)
		filesize=pDfInfo->GetFileSize();
	else if(pDfInfo2)
		filesize= pDfInfo2->GetFileSize();
	voLog(LL_DEBUG,"PDSource.txt","filesize=%d\n",filesize);
	return filesize;
}