#include "isomio.h"
#include <memory.h>
#include <string.h>
#include "isomutil.h"

#if defined(LINUX)
#   include "vocrsapis.h"
#endif 

using namespace ISOM;

// Reader
//====================================================================

Reader::Reader(Stream* s)
: stream(s),
  bitspos(-1)
{
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


// ReaderMSB
//====================================================================

ReaderMSB::ReaderMSB(Stream* s)
: Reader(s)
{
}

bool ReaderMSB::Read(int16* val)
{
	bool b = stream->Read(val, 2);
	Util::Swap16(val);
	return b;
}

bool ReaderMSB::Read(int32* val)
{
	bool b = stream->Read(val, 4);
	Util::Swap32(val);
	return b;
}

bool ReaderMSB::Read(int64* val)
{
	bool b = stream->Read(val, 8);
	Util::Swap64(val);
	return b;
}

bool ReaderMSB::Read(uint16* val)
{
	bool b = stream->Read(val, 2);
	Util::Swap16(val);
	return b;
}

bool ReaderMSB::Read(uint32* val)
{
	bool b = stream->Read(val, 4);
	Util::Swap32(val);
	return b;
}

bool ReaderMSB::Read(uint64* val)
{
	bool b = stream->Read(val, 8);
	Util::Swap64(val);
	return b;
}

// Writer
//====================================================================

Writer::Writer(Stream* s)
: stream(s),
  bitspos(0),
  bitsbuf(0)
{
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


// WriterMSB
//====================================================================

WriterMSB::WriterMSB(Stream* s)
: Writer(s)
{
}

bool WriterMSB::Write(int16 val)
{
	Util::Swap16(&val);
	bool b = stream->Write(&val, 2);
	return b;
}

bool WriterMSB::Write(int32 val)
{
	Util::Swap32(&val);
	bool b = stream->Write(&val, 4);
	return b;
}

bool WriterMSB::Write(int64 val)
{
	Util::Swap64(&val);
	bool b = stream->Write(&val, 8);
	return b;
}

bool WriterMSB::Write(uint16 val)
{
	Util::Swap16(&val);
	bool b = stream->Write(&val, 2);
	return b;
}

bool WriterMSB::Write(uint32 val)
{
	Util::Swap32(&val);
	bool b = stream->Write(&val, 4);
	return b;
}

bool WriterMSB::Write(uint64 val)
{
	Util::Swap64(&val);
	bool b = stream->Write(&val, 8);
	return b;
}


// FileStream
//====================================================================

FileStream::FileStream()
: file(0)
{
}

FileStream::~FileStream()
{
	Close();
}

bool FileStream::Open(const _TCHAR *filename, bool forwrite)
{
	Close();
	file = _tfopen(filename, forwrite ? __T("wb") : __T("rb"));
	return file != 0;
}

void FileStream::Close()
{
	if (file)
	{
		fclose(file);
		file = 0;
	}
}

long FileStream::Position()
{
	return ftell(file);
}

bool FileStream::SetPosition(long pos)
{
	return fseek(file, pos, SEEK_SET) == 0;
}

bool FileStream::Move(long delta)
{
	return fseek(file, delta, SEEK_CUR) == 0;
}

bool FileStream::Read(void* data, uint32 size)
{
	return fread(data, 1, size, file) == size;
}

bool FileStream::Write(const void* data, uint32 size)
{
	return fwrite(data, 1, size, file) == size;
}

long FileStream::Size()
{
	//CE does not support <io.h> & _filelength
	//long sz = _filelength(_fileno(file));
	long p1 = ftell(file);
	fseek(file, 0, SEEK_END);
	long p2 = ftell(file);
	fseek(file, p1, SEEK_SET);
	return p2;
}

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
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

long StreamOfIStream::Position()
{
	ULARGE_INTEGER ret;
	LARGE_INTEGER dst;
	dst.QuadPart = 0;
	pI->Seek(dst, STREAM_SEEK_CUR, &ret);
	return (long)ret.QuadPart;
}

bool StreamOfIStream::SetPosition(long pos)
{
	LARGE_INTEGER dst;
	dst.QuadPart = pos;
	HRESULT hr = pI->Seek(dst, STREAM_SEEK_SET, NULL);
	return hr == S_OK;
}

bool StreamOfIStream::Move(long delta)
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

#endif // .IStream for windows only
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

long MemStream::Position()
{
	return (long)(p - d);
}

bool MemStream::SetPosition(long pos)
{
	if (pos < 0 || pos >= s)
		return false;
	p = d + pos;
	return true;
}

bool MemStream::Move(long delta)
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



