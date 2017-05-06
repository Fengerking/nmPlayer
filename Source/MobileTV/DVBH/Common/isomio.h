/**
 * ISO Media I/O
 * @created 12/17/2004
 */
#ifndef _ISOMIO_H
#define _ISOMIO_H

#include <stdio.h>
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
#   include <objidl.h>
#   include <tchar.h>
#elif defined(LINUX)
#   include "vocrstypes.h"
#endif

#include "vobstype.h"

namespace ISOM {

//====================================================================

class Stream
{
public:
	virtual long Position() = 0;
	virtual bool SetPosition(long pos) = 0;
	virtual bool Move(long delta) = 0;
	virtual bool Read(void* data, uint32 size) = 0;
	virtual bool Write(const void* data, uint32 size) = 0;
};


//====================================================================

class Reader
{
public:
	Reader(Stream* s);
	
	long Position()
	{
		return stream->Position();
	}

	bool SetPosition(long pos)
	{
		return stream->SetPosition(pos);
	}

	bool Move(long delta)
	{
		return stream->Move(delta);
	}

	bool Read(void* data, uint32 size)
	{
		return stream->Read(data, size);
	}

	bool Read(int8* val);
	virtual bool Read(int16* val);
	virtual bool Read(int32* val);
	virtual bool Read(int64* val);
	bool Read(uint8* val);
	virtual bool Read(uint16* val);
	virtual bool Read(uint32* val);
	virtual bool Read(uint64* val);

public:
	char* ReadString();
	char* ReadString(int maxsize);

protected:
	Stream* stream;

public:
	Stream* GetStream() const { return stream; }
	void SetStream(Stream* s) { stream = s; }

protected:
	uint8 bitsbuf;
	int8 bitspos;

public:
	template <class T>
	bool ReadBits(T* val, uint8 size)
	{
		if (size > sizeof(T) * 8)
			return false;
		*val = 0;
		while (size)
		{
			if (bitspos < 0)
			{
				if (!Read(&bitsbuf, 1))
					return false;
				bitspos = 7;
			}
			*val <<= 1;
			if (bitsbuf & 0x80)
				*val |= 1;
			bitsbuf <<= 1;
			--bitspos;
			--size;
		}
		return true;
	}

	void Align() // byte align
	{
		bitspos = -1; //discard remain bits
	}
};


//====================================================================


class ReaderMSB : public Reader
{
public:
	ReaderMSB(Stream* s);

	bool Read(int8* val) { return Reader::Read(val); }
	virtual bool Read(int16* val);
	virtual bool Read(int32* val);
	virtual bool Read(int64* val);
	bool Read(uint8* val) { return Reader::Read(val); }
	virtual bool Read(uint16* val);
	virtual bool Read(uint32* val);
	virtual bool Read(uint64* val);
};


//====================================================================

class Writer
{
public:
	Writer(Stream* s);
	
	long Position()
	{
		return stream->Position();
	}

	bool SetPosition(long pos)
	{
		return stream->SetPosition(pos);
	}

	bool Move(long delta)
	{
		return stream->Move(delta);
	}

	bool Write(const void* data, uint32 size)
	{
		return stream->Write(data, size);
	}

	virtual bool Write(int8 val);
	virtual bool Write(int16 val);
	virtual bool Write(int32 val);
	virtual bool Write(int64 val);
	virtual bool Write(uint8 val);
	virtual bool Write(uint16 val);
	virtual bool Write(uint32 val);
	virtual bool Write(uint64 val);

public:
	bool WriteString(const char* str);

protected:
	Stream* stream;

protected:
	uint8 bitsbuf;
	int8 bitspos;

public:
	template <class T>
	bool WriteBits(T val, uint8 size)
	{
		if (size > sizeof(T) * 8)
			return false;
		T mask = 1 << (size - 1);
		while (mask)
		{
			bitsbuf <<= 1;
			if (val & mask) bitsbuf |= 1;
			++bitspos;
			if (bitspos == 8)
			{
				if (!Write(&bitsbuf, 1))
					return false;
				bitsbuf = 0;
				bitspos = 0;
			}
			mask >>= 1;
		}
		return true;
	}
};


//====================================================================


class WriterMSB : public Writer
{
public:
	WriterMSB(Stream* s);

	//virtual bool Write(int8 val);
	virtual bool Write(int16 val);
	virtual bool Write(int32 val);
	virtual bool Write(int64 val);
	//virtual bool Write(uint8 val);
	virtual bool Write(uint16 val);
	virtual bool Write(uint32 val);
	virtual bool Write(uint64 val);
};


//====================================================================


class FileStream : public Stream
{
public:
	FileStream();
	~FileStream();

	bool Open(const _TCHAR *filename, bool forwrite=false);
	void Close();
	
	virtual long Position();
	virtual bool SetPosition(long pos);
	virtual bool Move(long delta);
	virtual bool Read(void* data, uint32 size);
	virtual bool Write(const void* data, uint32 size);

protected:
	FILE* file;

public:
	long Size();
};

//====================================================================

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
class StreamOfIStream : public Stream
{
public:
	StreamOfIStream();
	~StreamOfIStream();

	bool Attach(IStream* p);
	void Detach();
	
	virtual long Position();
	virtual bool SetPosition(long pos);
	virtual bool Move(long delta);
	virtual bool Read(void* data, uint32 size);
	virtual bool Write(const void* data, uint32 size);

protected:
	IStream* pI;
};

#endif // .WIN_OS
//====================================================================

class MemStream : public Stream
{
public:
	MemStream(uint8* data, uint32 size);
	~MemStream();

	virtual long Position();
	virtual bool SetPosition(long pos);
	virtual bool Move(long delta);
	virtual bool Read(void* data, uint32 size);
	virtual bool Write(const void* data, uint32 size);

protected:
	uint8* d;
	uint32 s;
	uint8* p;
};


} //namespace ISOM
#endif //._ISOMIO_H
