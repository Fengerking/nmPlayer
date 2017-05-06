#include "CMp4WriterStream.h"
#include "fMacros.h"

#define LOG_TAG "CMp4WriterStream"
#include "voLog.h"

CMp4WriterStream::CMp4WriterStream(VO_FILE_OPERATOR* pFileOp)
	: CvoBaseFileOpr(pFileOp)
	, m_hFile(VO_NULL)
	, m_lFilePos(0)
{
#ifdef _LOCAL_FILE_
	gFile = NULL;
#endif
}

CMp4WriterStream::~CMp4WriterStream()
{
	Close();
}

VO_BOOL CMp4WriterStream::Open(VO_FILE_SOURCE *pFileSource )
{
#ifdef _LOCAL_FILE_
	Close();

	gFile = fopen( szFile , "wb");
	if(gFile == NULL)
	{
		return VO_FALSE;
	}

	return VO_TRUE;
#endif

	/////////////////////////////////////
	if(!pFileSource)
		return VO_FALSE;

	Close();

	m_lFilePos = 0;

	m_hFile = FileOpen(pFileSource);

	if(m_hFile == NULL)
	{
		VOLOGE("FileOpen reutrn NULL");
	}

	return (VO_NULL != m_hFile) ? VO_TRUE : VO_FALSE;
}

VO_VOID CMp4WriterStream::Close()
{
#ifdef _LOCAL_FILE_
	if(gFile == NULL)
		return;

	fclose(gFile);
	gFile = NULL;

	return ;
#endif 

	if(m_hFile)
	{
		FileClose(m_hFile);
		m_hFile = VO_NULL;
	}
}

long CMp4WriterStream::Position()
{
	return m_lFilePos;
}

bool CMp4WriterStream::SetPosition(long lPosition)
{
#ifdef _LOCAL_FILE_
	if(gFile == NULL)
	{
		return false;
	}

	fseek(gFile , lPosition, SEEK_SET);
	m_lFilePos = lPosition;
	return true;
#endif 

	if(!m_hFile)
		return false;

	VO_S64 nRes = FileSeek(m_hFile, lPosition, VO_FILE_BEGIN);
	if(nRes >= 0)
	{
		m_lFilePos = lPosition;
		return true;
	}

	if(nRes == -2)
		SetErrorCode(MPXERR_DATA_DOWNLOADING);

	return false;
}

bool CMp4WriterStream::Move(long lDelta)
{
	if(!m_hFile)
		return false;

	VO_S64 nRes = FileSeek(m_hFile, lDelta, VO_FILE_CURRENT);
	if(nRes >= 0)
	{
		m_lFilePos += lDelta;
		return true;
	}

	if(nRes == -2)
		SetErrorCode(MPXERR_DATA_DOWNLOADING);

	return false;
}

bool CMp4WriterStream::Read(void* pData, uint32 uSize)
{
	if(!m_hFile)
		return false;

	VO_S32 nRes = FileRead(m_hFile, pData, uSize);
	if(nRes > 0)
	{
		m_lFilePos += nRes;
		return true;
	}

	if(nRes == -2)
		SetErrorCode(MPXERR_DATA_DOWNLOADING);

	return false;
}

bool CMp4WriterStream::Write(const void* pData, uint32 uSize)
{
#ifdef _LOCAL_FILE_
	if(!gFile)
		return false;


	VO_S32 nRes = fwrite(pData , uSize , 1 , gFile);
	if(nRes > 0)
	{
		m_lFilePos += uSize;
		return true;
	}

	return false;
#else

	if(!m_hFile)
		return false;


	VO_S32 nRes = FileWrite(m_hFile, (VO_PTR)pData, uSize);
	if(nRes > 0)
	{
		m_lFilePos += nRes;
		return true;
	}

	return false;
#endif 
}

VO_U64 CMp4WriterStream::Size()
{
#ifdef _LOCAL_FILE_
#ifdef _WIN32

#elif defined LINUX

	VO_S32 r0 = 0, r = 0;
	//struct stat st;
	//r = fstat(fileno(gFile), &st);   // not be supported by some devices
	r0 = ftell(gFile);

	r = fseek(gFile, 0L, SEEK_END);
	if (-1 == r){
		return -1;
	}

	r = ftell(gFile);
	if (-1 == r){

		return -1;
	}

	fseek(gFile, r0, SEEK_SET);
	//return st.st_size;
	return r;
#endif
#else //_LOCAL_FILE_
	return (!m_hFile) ? -1 : (long)FileSize(m_hFile);
#endif //_LOCAL_FILE_
}