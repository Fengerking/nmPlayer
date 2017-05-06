#include "CMp4PushFileStream.h"
#include "fMacros.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMp4PushFileStream::CMp4PushFileStream(VO_FILE_OPERATOR* pFileOp)
	: CMp4BaseFileStream(pFileOp)
	,m_hFile(VO_NULL)
	,m_pBuffer(VO_NULL)
	,m_lFilePos(0)
	,m_nSize(0)
{
}

CMp4PushFileStream::~CMp4PushFileStream()
{
	Close();
}
VO_BOOL CMp4PushFileStream::Open(VO_FILE_SOURCE* pFileSource)
{
	return VO_FALSE;
}
VO_BOOL CMp4PushFileStream::Open(void* pBuffer, int64 nSize)
{
	Close();
	m_pBuffer =(VO_CHAR*) pBuffer;
	m_nSize = nSize;
	return (VO_NULL != m_pBuffer || m_nSize > 0) ? VO_TRUE : VO_FALSE;
}

VO_VOID CMp4PushFileStream::Close()
{
	m_pBuffer = NULL;
}

ABSPOS CMp4PushFileStream::Position()
{
	return m_lFilePos;
}

bool CMp4PushFileStream::SetPosition(ABSPOS lPosition)
{
	if(!m_pBuffer)
		return false;
	
	if(lPosition <=m_nSize)
	{
		m_lFilePos = lPosition;
		return true;
	}
	return false;
}

bool CMp4PushFileStream::Move(RELPOS lDelta)
{
	if(!m_pBuffer)
		return false;

	if((m_lFilePos + lDelta) <=m_nSize)
	{
		m_lFilePos += lDelta;
		if(m_lFilePos<= 0) m_lFilePos = 0 ;
		return true;
	}
	return false;
}

bool CMp4PushFileStream::Read(void* pData, uint32 uSize)
{
	if(!m_pBuffer)
		return false;
	if(m_lFilePos + uSize > m_nSize)
		return false;
	VO_CHAR* buffer = m_pBuffer + m_lFilePos;
	memcpy(pData, buffer, uSize);
	m_lFilePos += uSize;

	return true;
}

bool CMp4PushFileStream::Write(const void* pData, uint32 uSize)
{
	return false;
}

//long CMp4PushFileStream::Size()
VO_U64 CMp4PushFileStream::Size()
{
	return VO_U64((!m_pBuffer) ? -1 : m_nSize);
}