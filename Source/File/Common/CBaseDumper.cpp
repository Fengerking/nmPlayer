#include "CBaseDumper.h"
#include "fMacros.h"
#include "CDumper.h"
#include "voLog.h"

#define MAX_CHUNK_SIZE 188*300


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBaseDumper::CBaseDumper(VO_FILE_OPERATOR* pFileOp)
:CvoBaseFileOpr(pFileOp)
,CvoBaseMemOpr(VO_NULL)
,m_hFile(VO_NULL)
,m_MemPool(VO_NULL)
,m_nChunkSize(MAX_CHUNK_SIZE)
,m_nCurrPos(0)
,m_pChunk(VO_NULL)
{
}

CBaseDumper::~CBaseDumper(void)
{
	Close();
}

VO_U32 CBaseDumper::Open(VO_FILE_SOURCE* pFileSource)
{
	m_hFile = FileOpen(pFileSource);

	if (!m_hFile)
		return VO_ERR_SINK_OPENFAIL;
#ifndef _SINK_TO_MEMORY_FILE
	m_pChunk = (VO_BYTE*)AllocChunkMemory();
	if(!m_pChunk)
		return VO_ERR_SINK_OPENFAIL;
#endif
	m_nCurrPos = 0;

	return VO_ERR_NONE;
}

VO_U32 CBaseDumper::Close()
{
	if (m_hFile)
	{
#ifndef _SINK_TO_MEMORY_FILE
		if(m_nCurrPos > 0)
			FileWrite(m_hFile, m_pChunk, m_nCurrPos);
#endif
		FileClose(m_hFile);
		m_hFile = VO_NULL;
	}

	m_nCurrPos	= 0;
	m_pChunk	= VO_NULL;

	return VO_ERR_NONE;
}

VO_U32 CBaseDumper::Write(VO_BYTE* pData, VO_U32 nLen)
{
	if(!m_hFile)
		return VO_ERR_SINK_OPENFAIL;

	VO_PBYTE pTmp = pData;
	VO_U32	 nLeng = nLen;
	if(m_pChunk)
	{
		// need handle nLen > m_nChunkSize
		// ...

		if(m_nCurrPos+nLen > m_nChunkSize)
		{
			if (m_nCurrPos > 0)
			{
				FileWrite(m_hFile, m_pChunk, m_nCurrPos);
				m_nCurrPos = 0;
			}
			VO_U32 nToWrite = m_nChunkSize * ((nLeng - 1)/m_nChunkSize);
			if (nToWrite)
			{
				FileWrite(m_hFile, pTmp, nToWrite);
				pTmp += nToWrite;
				nLeng -= nToWrite;
			}	
		}

		MemCopy(m_pChunk+m_nCurrPos, pTmp, nLeng);
		m_nCurrPos += nLeng;
	}
	else
		FileWrite(m_hFile, pTmp, nLeng);

	VOLOGR("DumpRawData:size==%d",nLeng);
	CDumper::DumpRawData(pTmp,nLeng);
	return VO_ERR_NONE;
}

VO_U32 CBaseDumper::Flush()
{
	if(!m_hFile)
		return VO_ERR_SINK_OPENFAIL;

	FileFlush(m_hFile);

	return VO_ERR_NONE;
}

VO_PTR CBaseDumper::AllocChunkMemory()
{
	return m_MemPool.alloc(m_nChunkSize);
}

