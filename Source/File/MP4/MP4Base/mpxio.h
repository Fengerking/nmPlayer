/**
 * ISO Media I/O
 * @created 12/17/2004
 */
#ifndef _MPX_IO_H
#define _MPX_IO_H

#include <stdio.h>
#include "mpxtype.h"

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#   include <objidl.h>
#   include <tchar.h>
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define MAX_IO_BLOCK_SIZE 65536  //32768

#if 0 //defined(_DEBUG) || defined(DEBUG)
#define _MY_LOG
#endif //_DEBUG

#if 1 //Little Endian, Intel CPU...
#define ReaderLSB Reader
#define ReaderMSB Reader2
#define WriterLSB Writer
#define WriterMSB Writer2
#else //Big Endian, Motorola CPU...
#define ReaderLSB Reader2
#define ReaderMSB Reader
#define WriterLSB Writer2
#define WriterMSB Writer
#endif 

// Error Code

#define MPXERR_DATA_DOWNLOADING -1  //data is downloading, may be avaiable later

#if _Writer //old version
typedef  int32 ABSPOS; //absolute file position
typedef  int32 RELPOS;  //relative file position
#else //support 4G files
//typedef uint32 ABSPOS; //absolute file position
//typedef  int32 RELPOS;  //relative file position
typedef int64 ABSPOS; //absolute file position
typedef int64 RELPOS;  //relative file position
#endif

//namespace MPx {

//====================================================================

class Stream
{
public:
	virtual ABSPOS Position() = 0;
	virtual bool SetPosition(ABSPOS pos) = 0;
	virtual bool Move(RELPOS delta) = 0;
	virtual bool Read(void* data, uint32 size) = 0;
	virtual bool Write(const void* data, uint32 size) = 0;
	virtual bool Flush() { return true; }

protected:
	uint32 error;
public:
	Stream() : error(0) {}
	virtual ~Stream();
	uint32 GetErrorCode() const { return error; }
	void SetErrorCode(uint32 err) { if (error == 0) error = err; }
	void ClearErrorCode() { error = 0; }

public:
	bool SyncRead(ABSPOS pos, void* data, uint32 size);

};


//====================================================================

class Reader
{
public:
	Reader(Stream* s, uint64 _length =0 );
	virtual ~Reader() {};
	
	ABSPOS Position()
	{
		return stream->Position();
	}

	bool SetPosition(ABSPOS pos)
	{
		return stream->SetPosition(pos);
	}

	bool Move(RELPOS delta)
	{
		return stream->Move(delta);
	}

	bool Read(void* data, uint32 size)
	{
		bool b = true;
		ABSPOS pos = Position();
#if 1
		while (size > MAX_IO_BLOCK_SIZE)
		{
			b = stream->Read(data, MAX_IO_BLOCK_SIZE);
			if (!b)
				break;
			size -= MAX_IO_BLOCK_SIZE;
			data = (void*)((uint8*)data + MAX_IO_BLOCK_SIZE);
		}
#endif
		if (b)
			b = stream->Read(data, size);
		if (!b)
			SetPosition(pos);
		return b;
	}

	bool Read(int8* val);
	virtual bool Read(int16* val);
	virtual bool Read(int32* val);
	virtual bool Read(int64* val);
	bool Read(uint8* val);
	virtual bool Read(uint16* val);
	virtual bool Read(uint32* val);
	virtual bool Read(uint64* val);
	virtual bool SeekToEnd(){ return SetPosition((ABSPOS)length);}
public:
	char* ReadString();
	char* ReadString(int maxsize);

protected:
	Stream* stream;
	uint64 length;

public:
	Stream* GetStream() { return stream; }
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

public:
	uint32 GetErrorCode() const { return stream->GetErrorCode(); }
	void ClearErrorCode() { stream->ClearErrorCode(); }

};


//====================================================================


class Reader2 : public Reader
{
public:
	Reader2(Stream* s, uint64 _length = 0);

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
	virtual ~Writer() {};
	
	ABSPOS Position()
	{
		return stream->Position();
	}

	bool SetPosition(ABSPOS pos)
	{
		return stream->SetPosition(pos);
	}

	bool Move(RELPOS delta)
	{
		return stream->Move(delta);
	}

	bool Write(const void* data, uint32 size)
	{
		return stream->Write(data, size);
	}

	bool Flush()
	{
		return stream->Flush();
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

public:
	Stream* GetStream() { return stream; }
	void SetStream(Stream* s) { stream = s; }

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

public:
	uint32 GetErrorCode() const { return stream->GetErrorCode(); }
	void ClearErrorCode() { stream->ClearErrorCode(); }
};


//====================================================================


class Writer2 : public Writer
{
public:
	Writer2(Stream* s);
	virtual ~Writer2() {};

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
	
	virtual ABSPOS Position() = 0;
	virtual bool SetPosition(ABSPOS pos) = 0;
	virtual bool Move(RELPOS delta) = 0;
	virtual bool Read(void* data, uint32 size) = 0;
	virtual bool Write(const void* data, uint32 size) = 0;
	//virtual long Size() = 0;
	virtual VO_U64 Size() = 0;
	virtual bool Flush();

protected:
	uint32 m_nPreallocSize;
	uint32 m_nPreallocEndPos;
	uint32 m_nActualFileSize;
};

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
//====================================================================

class StreamOfIStream : public Stream
{
public:
	StreamOfIStream();
	~StreamOfIStream();

	bool Attach(IStream* p);
	void Detach();
	
	virtual ABSPOS Position();
	virtual bool SetPosition(ABSPOS pos);
	virtual bool Move(RELPOS delta);
	virtual bool Read(void* data, uint32 size);
	virtual bool Write(const void* data, uint32 size);

protected:
	IStream* pI;
};
#endif // windows special 

//====================================================================

class MemStream : public Stream
{
public:
	MemStream(uint8* data, uint32 size);
	~MemStream();

	void SetSize(uint32 size) { s = size; }

	uint32 GetSize() const { return s; }
	const uint8* GetData() const { return d; }
	uint8* GetBuffer() { return d; }

	virtual ABSPOS Position();
	virtual bool SetPosition(ABSPOS pos);
	virtual bool Move(RELPOS delta);
	virtual bool Read(void* data, uint32 size);
	virtual bool Write(const void* data, uint32 size);

protected:
	uint8* d;
	uint32 s;
	uint8* p;
};

//====================================================================


//} //namespace MPx


#ifdef _MY_LOG

bool MyLogOpen(LPCTSTR filename);
void MyLogClose();
extern FILE* MyLogFile;


#endif //_MY_LOG
#ifdef _VONAMESPACE
}
#endif

#endif //_MPX_IO_H
