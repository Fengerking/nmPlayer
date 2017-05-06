#include "mpxio.h"
#include "mpxutil.h"

//using namespace MPx;

// Stream
//====================================================================
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
Stream::~Stream()
{
}

bool Stream::SyncRead(ABSPOS pos, void* data, uint32 size)
{
	bool b = SetPosition(pos);
	if (b)
		b = Read(data, size);
	return b;
}


// Reader
//====================================================================

Reader::Reader(Stream* s, uint64 _length)
: stream(s)
{
	bitspos =-1;
	length = _length;
	//ClearErrorCode();
}

bool Reader::Read(int8* val)
{
	return stream->Read(val, 1);
}

bool Reader::Read(int16* val)
{
	return stream->Read(val, 2);
}
 
bool Reader::Read(int32* val)
{
	return stream->Read(val, 4);
}

bool Reader::Read(int64* val)
{
	return stream->Read(val, 8);
}

bool Reader::Read(uint8* val)
{
	return stream->Read(val, 1);
}

bool Reader::Read(uint16* val)
{
	return stream->Read(val, 2);
}

bool Reader::Read(uint32* val)
{
	return stream->Read(val, 4);
}

bool Reader::Read(uint64* val)
{
	return stream->Read(val, 8);
}

char* Reader::ReadString()
{
	uint8 len;
	if (!Read(&len))
		return 0;
	char* buf = new char[len + 1];
	if (!Read(buf, len))
	{
		delete[] buf;
		return 0;
	}
	buf[len] = 0;
	return buf;
}

char* Reader::ReadString(int maxsize)
{
	if (maxsize == 0)
		return 0;
	char buf[1024];
	if (!Read(buf, maxsize))
		return 0;
	char* str = new char[maxsize];
	uint8 len = (uint8)buf[0];
	if ((len + 1 == maxsize) && (buf[len] != 0))
	{
		memcpy(str, buf + 1, len);
		str[len] = 0;
	}
	else
	{
		memcpy(str, buf, maxsize);
	}
	return str;
}

// Reader2
//====================================================================

Reader2::Reader2(Stream* s, uint64 _length)
: Reader(s, _length)
{
}

bool Reader2::Read(int16* val)
{
	bool b = stream->Read(val, 2);
	Util::Swap16(val);
	return b;
}

bool Reader2::Read(int32* val)
{
	bool b = stream->Read(val, 4);
	Util::Swap32(val);
	return b;
}

bool Reader2::Read(int64* val)
{
	bool b = stream->Read(val, 8);
	Util::Swap64(val);
	return b;
}

bool Reader2::Read(uint16* val)
{
	bool b = stream->Read(val, 2);
	Util::Swap16(val);
	return b;
}

bool Reader2::Read(uint32* val)
{
	bool b = stream->Read(val, 4);
	Util::Swap32(val);
	return b;
}

bool Reader2::Read(uint64* val)
{
	bool b = stream->Read(val, 8);
	Util::Swap64(val);
	return b;
}

// Writer
//====================================================================

Writer::Writer(Stream* s)
: stream(s)
{ 
	bitspos = 0;
	bitsbuf =0;
}

bool Writer::Write(int8 val)
{
	return stream->Write(&val, 1);
}

bool Writer::Write(int16 val)
{
	return stream->Write(&val, 2);
}

bool Writer::Write(int32 val)
{
	return stream->Write(&val, 4);
}

bool Writer::Write(int64 val)
{
	return stream->Write(&val, 8);
}

bool Writer::Write(uint8 val)
{
	return stream->Write(&val, 1);
}

bool Writer::Write(uint16 val)
{
	return stream->Write(&val, 2);
}

bool Writer::Write(uint32 val)
{
	return stream->Write(&val, 4);
}

bool Writer::Write(uint64 val)
{
	return stream->Write(&val, 8);
}

bool Writer::WriteString(const char* str)
{
	uint8 len = (uint8)strlen(str);
	if (!Write(len))
		return false;
	if (!Write(str, len))
		return false;
	return true;
}


// Writer2
//====================================================================

Writer2::Writer2(Stream* s)
: Writer(s)
{
}

bool Writer2::Write(int16 val)
{
	Util::Swap16(&val);
	bool b = stream->Write(&val, 2);
	return b;
}

bool Writer2::Write(int32 val)
{
	Util::Swap32(&val);
	bool b = stream->Write(&val, 4);
	return b;
}

bool Writer2::Write(int64 val)
{
	Util::Swap64(&val);
	bool b = stream->Write(&val, 8);
	return b;
}

bool Writer2::Write(uint16 val)
{
	Util::Swap16(&val);
	bool b = stream->Write(&val, 2);
	return b;
}

bool Writer2::Write(uint32 val)
{
	Util::Swap32(&val);
	bool b = stream->Write(&val, 4);
	return b;
}

bool Writer2::Write(uint64 val)
{
	Util::Swap64(&val);
	bool b = stream->Write(&val, 8);
	return b;
}


// FileStream
//====================================================================

FileStream::FileStream()
: m_nPreallocSize(0)
, m_nPreallocEndPos(0)
, m_nActualFileSize(0)
{
}

FileStream::~FileStream()
{
}

bool FileStream::Flush()
{
	return true;
}

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
// StreamOfIStream
//====================================================================

StreamOfIStream::StreamOfIStream()
: pI(0)
{
}

StreamOfIStream::~StreamOfIStream()
{
	Detach();
}

bool StreamOfIStream::Attach(IStream* p)
{
	Detach();
	pI = p;
	pI->AddRef();
	return true;
}

void StreamOfIStream::Detach()
{
	if (pI)
	{
		pI->Release();
		pI = 0;
	}
}

ABSPOS StreamOfIStream::Position()
{
	ULARGE_INTEGER ret;
	LARGE_INTEGER dst;
	dst.QuadPart = 0;
	pI->Seek(dst, STREAM_SEEK_CUR, &ret);
	return (ABSPOS)ret.QuadPart;
}

bool StreamOfIStream::SetPosition(ABSPOS pos)
{
	LARGE_INTEGER dst;
	dst.QuadPart = pos;
	HRESULT hr = pI->Seek(dst, STREAM_SEEK_SET, NULL);
	return hr == S_OK;
}

bool StreamOfIStream::Move(RELPOS delta)
{
	LARGE_INTEGER dst;
	dst.QuadPart = delta;
	HRESULT hr = pI->Seek(dst, STREAM_SEEK_CUR, NULL);
	return hr == S_OK;
}

bool StreamOfIStream::Read(void* data, uint32 size)
{
	HRESULT hr = pI->Read(data, size, NULL);
	return hr == S_OK;
}

bool StreamOfIStream::Write(const void* data, uint32 size)
{
	HRESULT hr = pI->Write(data, size, NULL);
	return hr == S_OK;
}

#endif // windows speical 
// MemStream
//====================================================================

MemStream::MemStream(uint8* data, uint32 size)
: d(data),
  s(size),
  p(data)
{
}

MemStream::~MemStream()
{
}

ABSPOS MemStream::Position()
{
	return (ABSPOS)(p - d);
}

bool MemStream::SetPosition(ABSPOS pos)
{
	if (pos < 0 || pos >= (ABSPOS)s)
		return false;
	p = d + pos;
	return true;
}

bool MemStream::Move(RELPOS delta)
{
	p += delta;
#if 1
	return true;
#else
	if ((p - d < 0) || ((int32)(p - d - s) > 0))
	{
		p -= delta;
		return false;
	}
	return true;	
#endif
}

bool MemStream::Read(void* data, uint32 size)
{
	if (p + size > d + s)
		return false;
	memcpy(data, p, size);
	p += size;
	return true;
}

bool MemStream::Write(const void* data, uint32 size)
{
	if (p + size > d + s)
		return false;
	memcpy(p, data, size);
	p += size;
	return true;
}


//====================================================================


#ifdef _MY_LOG


bool MyLogOpen(LPCTSTR filename)
{
	TCHAR path[MAX_PATH];
	_stprintf(path, _T("%s.log"), filename);
	MyLogFile = _tfopen(path, _T("w"));
	return MyLogFile != NULL;
}

void MyLogClose()
{
	fclose(MyLogFile);
}


// must at last for we declare it in header file
FILE* MyLogFile = NULL;

#endif //_MY_LOG
