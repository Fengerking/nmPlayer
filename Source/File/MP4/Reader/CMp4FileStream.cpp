#include "CMp4FileStream.h"
#include "fMacros.h"
#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
#ifdef _SUPPORT_PUSHMODE
CMp4PullFileStream::CMp4PullFileStream(VO_FILE_OPERATOR* pFileOp)
	: CMp4BaseFileStream(pFileOp)
#else
CMp4FileStream::CMp4FileStream(VO_FILE_OPERATOR* pFileOp)
	: CvoBaseFileOpr(pFileOp)
	, m_hFile(VO_NULL)
	, m_lFilePos(0)
#endif
{
}

#ifdef _SUPPORT_PUSHMODE
CMp4PullFileStream::~CMp4PullFileStream()
#else
CMp4FileStream::~CMp4FileStream()
#endif
{
	Close();
}
#ifdef _SUPPORT_PUSHMODE
VO_BOOL CMp4PullFileStream::Open(VO_FILE_SOURCE* pFileSource)
#else
VO_BOOL CMp4FileStream::Open(VO_FILE_SOURCE* pFileSource)
#endif
{
	Close();

	pFileSource->nMode = VO_FILE_READ_ONLY;
	m_hFile = FileOpen(pFileSource);
	if(!m_hFile)
	{
		VOLOGI("Failed to open the file and the file handle from I/O: %p",m_hFile);
		return VO_FALSE;
	}

	m_lFilePos = 0;
	return (VO_NULL != m_hFile) ? VO_TRUE : VO_FALSE;
}
#ifdef _SUPPORT_PUSHMODE
VO_VOID CMp4PullFileStream::Close()
#else
VO_VOID CMp4FileStream::Close()
#endif
{
	if(m_hFile)
	{
		FileClose(m_hFile);
		m_hFile = VO_NULL;
	}
}
#ifdef _SUPPORT_PUSHMODE
ABSPOS CMp4PullFileStream::Position()
#else
ABSPOS CMp4FileStream::Position()
#endif
{
	return m_lFilePos;
}
#ifdef _SUPPORT_PUSHMODE
bool CMp4PullFileStream::SetPosition(ABSPOS lPosition)
#else
bool CMp4FileStream::SetPosition(ABSPOS lPosition)
#endif
{
	if(!m_hFile)
		return false;

#if 0
	VOLOGR("SetPosition: %d", lPosition);
	if (lPosition >= 89505)
		return false;
#endif
	VO_S64 nRes = FileSeek(m_hFile, lPosition, VO_FILE_BEGIN);
	VOLOGR("Mp4FileStream. SetPosition. Pos:%lld, Res:%lld",lPosition, nRes);
	if(nRes >= 0)
	{
		m_lFilePos = lPosition;
		return true;
	}

	if(nRes == -2)
		SetErrorCode(MPXERR_DATA_DOWNLOADING);

	return false;
}
#ifdef _SUPPORT_PUSHMODE
bool CMp4PullFileStream::Move(RELPOS lDelta)
#else
bool CMp4FileStream::Move(RELPOS lDelta)
#endif
{
	if(!m_hFile)
		return false;

	VO_S64 nRes = FileSeek(m_hFile, lDelta, VO_FILE_CURRENT);
	VOLOGR("Mp4FileStream. Move. Pos:%lld, Res:%lld",lDelta, nRes);
	if(nRes >= 0)
	{
		m_lFilePos += lDelta;
		return true;
	}

	if(nRes == -2)
		SetErrorCode(MPXERR_DATA_DOWNLOADING);

	return false;
}
#ifdef _SUPPORT_PUSHMODE
bool CMp4PullFileStream::Read(void* pData, uint32 uSize)
#else
bool CMp4FileStream::Read(void* pData, uint32 uSize)
#endif
{
	if(!m_hFile)
		return false;

	VO_S32 nRes = FileRead(m_hFile, pData, uSize);

	VOLOGR("Mp4FileStream. FileRead. Size:%d, Res:%d",uSize, nRes);
	if(nRes > 0)
	{
		m_lFilePos += nRes;
		return true;
	}

	if(nRes == -2)
		SetErrorCode(MPXERR_DATA_DOWNLOADING);

	return false;
}
#ifdef _SUPPORT_PUSHMODE
bool CMp4PullFileStream::Write(const void* pData, uint32 uSize)
#else
bool CMp4FileStream::Write(const void* pData, uint32 uSize)
#endif
{
	return false;
}
#ifdef _SUPPORT_PUSHMODE
//long CMp4PullFileStream::Size()
VO_U64 CMp4PullFileStream::Size()
#else
//long CMp4FileStream::Size()
VO_U64 CMp4FileStream::Size()
#endif
{
	return (!m_hFile) ? -1 : FileSize(m_hFile);
}