#include "CGFileChunk.h"
#include "fMacros.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CGFileChunk::CGFileChunk(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp)
	: CvoBaseFileOpr(pFileOp)
	, CvoBaseMemOpr(pMemOp)
	, m_hFile(VO_NULL)
	, m_llActualFilePos(0)
	, m_bFileHandleToShare(VO_FALSE)
	, m_bFileHandleShared(VO_FALSE)
	, m_pContent(VO_NULL)
	, m_nSize(0)
	, m_dwChunkSize(VO_FILE_CHUNK_SIZE)
	, m_dwReadPos(0)
	, m_ullFilePos(0)
	, m_ullRecordPos(-1)
	, m_llFileSize(0)
{
}

CGFileChunk::~CGFileChunk()
{
	FDestroy();
}

VO_VOID CGFileChunk::SetShareFileHandle(VO_BOOL bValue)
{
	m_bFileHandleToShare = bValue;
	if(m_bFileHandleToShare)
		m_bFileHandleShared = VO_TRUE;
}

VO_BOOL CGFileChunk::FCreate(VO_PTR hFile, VO_U64 ullFilePos /* = 0 */, VO_U32 dwChunkSize /* = VO_FILE_CHUNK_SIZE */)
{
	if(!hFile)
		return VO_FALSE;

	FDestroy();

	m_hFile = hFile;
	m_llActualFilePos = 0;
	m_bFileHandleToShare = VO_FALSE;
	m_bFileHandleShared = VO_FALSE;
	m_nSize = 0;
	m_dwReadPos = 0;
	m_ullFilePos = 0;
	m_dwChunkSize = dwChunkSize;
	m_ullRecordPos = -1;
	m_llFileSize = FileSize(hFile); 

	m_pContent = NEW_BUFFER(m_dwChunkSize);
	if(!m_pContent)
		return VO_FALSE;

	return FLocate(ullFilePos);
}

VO_VOID CGFileChunk::FDestroy()
{
	SAFE_MEM_FREE(m_pContent);
	m_hFile = VO_NULL;
}

VO_BOOL CGFileChunk::FLocate(VO_U64 ullFilePos, VO_BOOL bForceMove /* = VO_FALSE */)
{
	if(bForceMove || ullFilePos + m_dwReadPos > m_ullFilePos + m_nSize || ullFilePos + m_dwReadPos < m_ullFilePos)
	{
		if(!SeekFileB(ullFilePos, VO_FILE_BEGIN))
			return VO_FALSE;

		m_dwReadPos = 0;
		m_ullFilePos = ullFilePos;

		if(!ReadFileB(m_pContent, m_dwChunkSize, &m_nSize))
			return VO_FALSE;
	}
	else
	{
		m_dwReadPos = (VO_U32)((ullFilePos + m_dwReadPos) - m_ullFilePos);
		m_ullFilePos = ullFilePos;
	}

	return VO_TRUE;
}

VO_PBYTE CGFileChunk::FGetFileContentPtr(VO_U32 dwLen)
{
	VO_U32 dwReadPos = m_dwReadPos + dwLen;
	if(dwReadPos > (VO_U32)m_nSize)
		return VO_NULL;
	else
	{
		VO_PBYTE pReturn = m_pContent + m_dwReadPos;
		m_dwReadPos = dwReadPos;
		m_ullFilePos += dwLen;
		return pReturn;
	}
}

VO_BOOL CGFileChunk::FRead(VO_PTR pData, VO_U32 dwLen)
{
	if(0 == dwLen)
		return VO_TRUE;

	VO_U32 dwReadPos = m_dwReadPos + dwLen;
	if(dwReadPos > (VO_U32)m_nSize)
	{
		dwReadPos -= m_nSize;
		if(dwLen < dwReadPos)
			return VO_FALSE;

		VO_PBYTE pTmp = (VO_PBYTE)pData;
		MemCopy(pTmp, m_pContent + m_dwReadPos, dwLen - dwReadPos);
		pTmp += (dwLen - dwReadPos);

		if(dwReadPos > m_dwChunkSize)
		{
			VO_S32 nToRead = m_dwChunkSize * ((dwReadPos - 1) / m_dwChunkSize);
			VO_S32 nReaded = 0;
			if(!ReadFileB(pTmp, nToRead, &nReaded))
				return VO_FALSE;

			if(nReaded < nToRead)
				return VO_FALSE;

			pTmp += nReaded;
			dwReadPos -= nReaded;
		}

		m_dwReadPos = dwReadPos;
		if(!ReadFileB(m_pContent, m_dwChunkSize, &m_nSize))
			return VO_FALSE;

		if((VO_U32)m_nSize < m_dwReadPos)
			return VO_FALSE;

		MemCopy(pTmp, m_pContent, m_dwReadPos);
	}
	else
	{
		MemCopy(pData, m_pContent + m_dwReadPos, dwLen);
		m_dwReadPos = dwReadPos;
	}

	m_ullFilePos += dwLen;
	return VO_TRUE;
}

VO_BOOL CGFileChunk::FRead2(VO_PTR pData, VO_U32 dwLen, VO_U64 ullFilePos)
{
	if(0 == dwLen)
		return VO_TRUE;

	if(ullFilePos != m_ullFilePos)
	{
		if(!FLocate(ullFilePos))
			return VO_FALSE;
	}

	return FRead(pData, dwLen);
}

VO_BOOL CGFileChunk::FRead3(VO_PTR pData, VO_U32 dwLen, VO_U32* pdwReaded)
{
	if(0 == dwLen)
		return VO_TRUE;

	VO_U32 dwReadPos = m_dwReadPos + dwLen;
	VO_U32 dwReaded = 0;
	if(dwReadPos > (VO_U32)m_nSize)
	{
		dwReadPos -= m_nSize;
		if(dwLen < dwReadPos)
			return VO_FALSE;

		VO_PBYTE pTmp = (VO_PBYTE)pData;
		MemCopy(pTmp, m_pContent + m_dwReadPos, dwLen - dwReadPos);
		dwReaded += (dwLen - dwReadPos);
		pTmp = VO_PBYTE(pData) + dwReaded;

		if(dwReadPos > m_dwChunkSize)
		{
			VO_S32 nToRead = m_dwChunkSize * ((dwReadPos - 1) / m_dwChunkSize);
			VO_S32 nReaded = 0;
			if(!ReadFileB(pTmp, nToRead, &nReaded))
			{
				m_ullFilePos += dwReaded;
				if(pdwReaded)
					*pdwReaded = dwReaded;
				return (dwReaded > 0) ? VO_TRUE : VO_FALSE;
			}

			dwReaded += nReaded;
			pTmp = VO_PBYTE(pData) + dwReaded;
			dwReadPos -= nReaded;
		}

		m_dwReadPos = dwReadPos;
		if(!ReadFileB(m_pContent, m_dwChunkSize, &m_nSize))
		{
			m_ullFilePos += dwReaded;
			if(pdwReaded)
				*pdwReaded = dwReaded;
			return (dwReaded > 0) ? VO_TRUE : VO_FALSE;
		}

		VO_U32 dwToCopy = VO_MIN(m_dwReadPos, (VO_U32)m_nSize);
		MemCopy(pTmp, m_pContent, dwToCopy);
		dwReaded += dwToCopy;

		m_ullFilePos += dwReaded;
		if(pdwReaded)
			*pdwReaded = dwReaded;
		return (dwReaded > 0) ? VO_TRUE : VO_FALSE;
	}
	else
	{
		MemCopy(pData, m_pContent + m_dwReadPos, dwLen);
		m_dwReadPos = dwReadPos;
		m_ullFilePos += dwLen;
		if(pdwReaded)
			*pdwReaded = dwLen;
		return VO_TRUE;
	}
}

VO_BOOL CGFileChunk::FRead4(VO_PTR pData, VO_U32 dwLen, VO_U64 ullFilePos, VO_U32* pdwReaded)
{
	if(0 == dwLen)
		return VO_TRUE;

	if(ullFilePos != m_ullFilePos)
	{
		if(!FLocate(ullFilePos))
			return VO_FALSE;
	}

	return FRead3(pData, dwLen, pdwReaded);
}

VO_BOOL CGFileChunk::FSkip(VO_U64 ullSkip)
{
	if(0 == ullSkip)
		return VO_TRUE;

	VO_U64 ullReadPos = m_dwReadPos + ullSkip;
	if(ullReadPos > (VO_U32)m_nSize)
	{
		ullReadPos -= m_nSize;
		if(ullReadPos > m_dwChunkSize)
		{
			VO_U64 ullReaded = ((ullReadPos - 1) / m_dwChunkSize) * m_dwChunkSize;
			if(!SeekFileB(ullReaded, VO_FILE_CURRENT))
				return VO_FALSE;

			ullReadPos -= ullReaded;
		}

 		if(!ReadFileB(m_pContent, m_dwChunkSize, &m_nSize))
			return VO_FALSE;
	}

	m_dwReadPos = (VO_U32)(ullReadPos);
	m_ullFilePos += ullSkip;

	return VO_TRUE;
}

VO_BOOL CGFileChunk::FBack(VO_U64 ullBack)
{
	if(0 == ullBack)
		return VO_TRUE;

	if(m_dwReadPos >= ullBack) //chunk internal
	{
		m_ullFilePos -= ullBack;
		m_dwReadPos -= (VO_U32)(ullBack);
		
		return VO_TRUE;
	}
	else
		return FLocate(m_ullFilePos - ullBack);
}

VO_BOOL CGFileChunk::ReadFileB(VO_PBYTE pBuffer, VO_S32 nToRead, VO_S32* pnReaded)
{
	if(m_bFileHandleShared)
	{
		if(!SeekFileB(0, VO_FILE_CURRENT))
			return VO_FALSE;
	}

	VO_S32 nRes = -2;
	while(-2 == nRes)
		nRes = FileRead(m_hFile, pBuffer, nToRead);

	if(nRes > 0)
	{
		if(pnReaded)
			*pnReaded = nRes;

		m_llActualFilePos += nRes;

		return VO_TRUE;
	}
	else
	{
		if(pnReaded)
			*pnReaded = 0;

		return VO_FALSE;
	}
}

VO_BOOL CGFileChunk::SeekFileB(VO_S64 llFilePos, VO_FILE_POS fsPos)
{
	if(m_llFileSize > 0 && llFilePos > m_llFileSize) return VO_FALSE;

	if(m_bFileHandleShared)
	{
		if(VO_FILE_CURRENT == fsPos)
		{
			fsPos = VO_FILE_BEGIN;
			llFilePos = m_llActualFilePos + llFilePos;
		}

		if(!m_bFileHandleToShare)
			m_bFileHandleShared = VO_FALSE;
	}

	VO_S64 nRes = -2;
	while(-2 == nRes)
		nRes = FileSeek(m_hFile, llFilePos, fsPos);

	if(nRes < 0)
		return VO_FALSE;
	
	m_llActualFilePos = nRes;

	return VO_TRUE;
}
