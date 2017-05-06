#ifndef _ISOM_WR_H
#define _ISOM_WR_H

#include "isombox.h"
//#include "isombox.h"
#include "isomlogi.h"
#include "voString.h"

//using namespace ISOMW;

//#if defined(LINUX)
//#  include "Lin2Win.h"
//#endif 
// time unit is 1/timescale s, 8/6/2007

//use sample time or duration
#define _USE_SAMPLE_TIME
#define _USE_BLOCK_FILES

enum FileFormat
{
	FileFormat3GP = 1,
	FileFormatMP4 = 2,
	FileFormat3G2 = 4,
	FileFormatAVC = 8,
};

//namespace ISOM {

struct FilePathNode
{
	VO_TCHAR path[260];
	VO_BYTE *	  pBuf;
	FilePathNode* next;
};

class TempFilesStream : public Stream
{
public:
	TempFilesStream(VO_PTCHAR tempdir, int blocksize);

	virtual ABSPOS Position();
	virtual bool SetPosition(long pos);
	virtual bool Move(long delta);
	virtual bool Read(void* data, uint32 size);
	virtual bool Write(const void* data, uint32 size);

protected:
	virtual void ReleaseNode(FilePathNode* node);
	virtual FilePathNode* AddNode();

protected:
	VO_TCHAR _tempDir[260];
	int _blockSize;
	int _total;
	int _current;
	FilePathNode* _headNode;
	FilePathNode* _currentNode;
};


class ReserveSpace
{
public:
	ReserveSpace(int size=65536);
	virtual ~ReserveSpace();

	virtual bool SetFile(VO_PTCHAR filename);
	virtual bool SetSize(int size);
	virtual void Release();

	bool IncSize(int delta) { return SetSize(_size + delta); }

protected:
	VO_TCHAR _filename[260]; //file for reserve space
	int _size;
	//HANDLE _hfile;
};


const int DEFAULT_MAX_CHUNK_SIZE = 65536;

class BufferOutStream : public Stream
{
public:
	BufferOutStream(Stream* target, uint32 size=MAX_IO_BLOCK_SIZE);
	BufferOutStream(Stream* target, MemStream* buffer);
	~BufferOutStream();

	virtual long Position();
	virtual bool SetPosition(long pos);
	virtual bool Move(long delta);
	virtual bool Read(void* data, uint32 size);
	virtual bool Write(const void* data, uint32 size);
	virtual bool Flush();

protected:
	uint32 GetBufSize() const {
		if(bs)
			return bs->GetSize();

		return 0;
	}

private:
	uint8* buf;
	MemStream* bs; //buffer stream
	Stream* ts; //target stream

private:
	ReserveSpace* reserveSpace;
protected:
	void PrepareRetry() {
		if(reserveSpace)
		{
			reserveSpace->Release();
			reserveSpace = 0;
		}
	}
public:
	void SetReserveSpace(ReserveSpace* s) { reserveSpace = s; }
};


class BufferWrite
{
public:
	BufferWrite();
	~BufferWrite();

	Writer* GetWriter() { return _bufwriter; }
	bool Copy(Writer& dstwriter) const;

	bool InitMemoryBuffer(int size, bool bMSB=true);
	bool InitFileBuffer(VO_PTCHAR tempPath, bool bMSB=true);
	bool InitBufferOutFileBuffer(VO_PTCHAR tempPath, ReserveSpace* reserveSpace, int size=MAX_IO_BLOCK_SIZE, bool bMSB=true);
	void Release();

protected:
	uint32 GetWrittenSize() const 
	{ 
		if(_outstream)
			return _outstream->Position() - _startpos; 

		return 0;
	}

	bool InitMemoryStream(int size);
	bool InitFileStream(VO_PTCHAR tempPath);
	bool InitWriter(Stream* stream, bool bMSB);

private:
	Writer* _bufwriter;
	uint32 _startpos;
	Stream* _instream;
	Stream* _outstream;

	MemStream* _memstream;
	uint8* _buffer;

#ifdef _USE_BLOCK_FILES
	TempFilesStream* _filestream;
#else //_USE_BLOCK_FILES
	MPx::FileStream* _filestream;
	TCHAR _filename[260];
#endif //_USE_BLOCK_FILES

	BufferOutStream* _bufoutstream;
};


class ChunkOffsetBoxEx : public ChunkOffsetBox, public BufferWrite
{
public:
	ChunkOffsetBoxEx(uint8 v=0, uint32 f=0) : ChunkOffsetBox(v, f) {}
	virtual ~ChunkOffsetBoxEx() {}

	virtual bool DumpExt(Writer& w) const;
	bool AddEntry(uint32 chunk_offset);
};


class SampleSizeBoxEx : public SampleSizeBox, public BufferWrite
{
public:
	SampleSizeBoxEx(uint8 v=0, uint32 f=0) : SampleSizeBox(v, f) {}
	virtual ~SampleSizeBoxEx() {}

	virtual bool DumpExt(Writer& w) const;
	bool AddEntry(uint32 size);
	bool EndAdd();
};

class SyncSampleBoxEx : public SyncSampleBox, public BufferWrite
{
public:
	SyncSampleBoxEx(uint8 v=0, uint32 f=0) : SyncSampleBox(v, f) {}
	virtual ~SyncSampleBoxEx() {}

	virtual bool DumpExt(Writer& w) const;
	bool AddEntry(uint32 index);
};

class TimeToSampleBoxEx : public TimeToSampleBox, public BufferWrite
{
public:
	TimeToSampleBoxEx(uint8 v=0, uint32 f=0) : TimeToSampleBox(v, f), _current_count(0)
#ifdef _USE_SAMPLE_TIME
		, _csample(0) 
#endif //_USE_SAMPLE_TIME
	{
	}
	virtual ~TimeToSampleBoxEx() {}

	virtual bool DumpExt(Writer& w) const;
	bool AddEntry(uint32 delta);
	bool EndAdd();
	
private:
	uint32 _current_delta;
	uint32 _current_count;

#ifdef _USE_SAMPLE_TIME

public:
	bool AddEntry2(uint32 time);
	bool EndAdd2();
	uint32 GetCurrentTime() const { return _current_time; }
private:
	uint32 _current_time;
	uint32 _csample;

#endif //_USE_SAMPLE_TIME
};


class SampleToChunkBoxEx : public SampleToChunkBox, public BufferWrite
{
public:
	SampleToChunkBoxEx(uint8 v=0, uint32 f=0) : SampleToChunkBox(v, f), _first_chunk(0), _samples_per_chunk(0), _sample_description_index(1) {}
	virtual ~SampleToChunkBoxEx() {}

	virtual bool DumpExt(Writer& w) const;
	bool AddEntry(uint32 chunk, uint32 samples_per_chunk);
	bool EndAdd();
	
private:
	uint32 _first_chunk;
	uint32 _samples_per_chunk;
	uint32 _sample_description_index;
};


struct BufferSample
{
public:
	uint8* buffer;
	uint32 size;
#ifdef _USE_SAMPLE_TIME
	uint32 time;
#else //_USE_SAMPLE_TIME
	uint32 duration;
#endif //_USE_SAMPLE_TIME
	bool sync;
};

class BufferChunk
{
public:
	BufferChunk();
	~BufferChunk();

	bool Init(uint32 maxsize);
	void Reset();
	bool AddSample(const BufferSample* sample);

public:
	const uint8* GetBuffer() const { return _buffer; }
	uint32 GetSize() const { return _size; }
	int GetSamples()  const { return _csample; }
	int GetMaxSize() const { return _maxsize; }

private:
	uint8* _buffer;
	uint32 _size;
	int _csample;
	uint32 _maxsize;
};

class MovieWriter;

class TrackWriter
{
public:
	TrackWriter(MovieWriter* mw);
	~TrackWriter();

	bool SetMaxChunkSize(int maxsize);
	bool AddSample( BufferSample* sample);
	bool Open(Stream* stream);
	bool Close();
	bool Flush();

	Track* GetTrack() { return &_track; }

protected:
	bool AddChunk(const BufferChunk* chunk);
	bool BuildBoxes();
	bool UpdateBoxes();
	SampleEntry* BuildSampleEntry();
	int GetMaxChunkSize() const { return _chunk.GetMaxSize(); }

public:
	uint32 GetDuration() const { return _track.GetDuration(); }
	void SetDuration(uint32 dur) { _track.SetDuration(dur); }
	uint32 GetDataSize() const { return _datasize; }
	bool IsClosed() const { return _sttsbox == NULL; }
	int GetMaxBufferDataSize() const;

	void SetIndexIn(bool bValue);
	void SetChunkDuration(uint32 nDuration){mnChunkDurtion = nDuration; }


protected:
	void SetClosed() { _sttsbox = NULL; }

private:
	MovieWriter* _moviewr;
	Track _track; //only use properties

	Writer _mdatwr;
	BufferChunk _chunk; //current chunk

	int _csample; //count samples
	int _cchunk; //count chunks
	uint32 _datasize; //mdat box size

	//boxes
	Box* _trakbox;
	TrackHeaderBox* _tkhdbox;
	MediaHeaderBox* _mdhdbox;
	ChunkOffsetBoxEx* _stcobox;
	SampleToChunkBoxEx* _stscbox;
	SampleSizeBoxEx* _stszbox;
	TimeToSampleBoxEx* _sttsbox;
	SyncSampleBoxEx* _stssbox;
	SampleDescriptionBox* _stsdbox;
	Box* _minfbox;
	HandlerBox* _hdlrbox;
	EditListBox* _elstbox; //EditList Support, 090104

	//add by matao for writer Index first
	bool mbIndexIn;
	bool mbIndexFirst;

	//add by matao for write a/v average 
	uint32   mnWriteTrunkTime;
	uint32	 mnChunkDurtion;
public:
	uint32 GetErrorCode() const { return _mdatwr.GetErrorCode(); }
};


class MovieWriter
{
public:
	MovieWriter();
	~MovieWriter();

	bool Open(int type, Stream* stream, uint32 timescale);
	bool Close();
	bool Reset();

	TrackWriter* AddTrack();
	bool RemoveTrack(TrackWriter* track);

	Box* GetMoovBox() { return _moovbox; }
	Box* GetFtypBox() { return _ftypbox; }
	uint32 GetFileSize();
	uint32 GetMoovSize() {return _moovboxSize;}
	void	SetMoovSize(uint32 nsize){_moovboxSize = nsize;}

	uint32 GetmdatPos() {return mnmdatPos;}
	void   SetmdatPos(uint32 nPos ){mnmdatPos = nPos;}
private:
	WriterMSB _writer;

	//box
	FileTypeBox* _ftypbox;
	Box* _mdatbox;
	Box* _moovbox;
	MovieHeaderBox* _mvhdbox;

	uint32 _timescale;
	uint32 _duration;
	
	//add by matao for write index to header of file
	uint32 _moovboxSize;
	uint32 mnmdatPos;

protected:
	uint32 GetTimeScale() const { return _timescale; }
	void SetTimeScale(uint32 ts) { _timescale = ts; }
	void SetFileFormat(int fmt);

private:
	enum 
	{
		MAX_TRACKS = 16
	};

	TrackWriter* _tracks[MAX_TRACKS];
	int _ctrack;
	int _trackid;
	int FindTrack(TrackWriter* track);

public:
	int GetTrackCount() const { return _ctrack; }
	TrackWriter* GetTrackByID(uint32 id);

	bool SetTempDirectory(VO_PTCHAR path);
	VO_PTCHAR GetTempDirectory() const { return (VO_PTCHAR)_tempDir; }
private:
	VO_TCHAR _tempDir[260];

private:
	ReserveSpace * _reserveSpace;
public:
	ReserveSpace* GetReserveSpace() { return _reserveSpace; }
	void ReleaseReserveSpace() {
		if(_reserveSpace )
			_reserveSpace->Release(); 
	}
	bool IncReserveSpace(int delta) {
		return true;
	}

public:
	uint32 GetErrorCode() const { return _writer.GetErrorCode(); }
};




//} //namespace ISOM

#endif // _ISOM_WR_H
