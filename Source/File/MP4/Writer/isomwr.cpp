#include "isomwr.h"
#include "mp4cfg.h"
#include "mp4desc.h"
#include "voFile.h"
#include "CTempFileStream.h"
#include "CReserveSpace2.h"
#include "CTempBufStream.h"

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#  include <windows.h>
#elif defined(LINUX)
#  include <string.h>
#  include <time.h>
#endif

//using namespace ISOM;

extern VO_FILE_OPERATOR *	gpFileOperator;
//====================================================================

#define SAFEDEL(x) if (x) { delete x; x = NULL; }

// TempFilesStream
//====================================================================

VO_BOOL CreateTempFileName(VO_PTCHAR strDir , VO_PTCHAR strPrefixString , VO_U32 nUnique , VO_PTCHAR strFileName)
{
	if(nUnique == 0)
	{
		static VO_U32 sNum = 0;
		sNum ++;

		nUnique = sNum;
	}

	_stprintf(strFileName , _T("%s%s%x.tmp") , strDir , strPrefixString , (unsigned int)nUnique);
	return VO_TRUE;
}

TempFilesStream::TempFilesStream(VO_PTCHAR tempdir, int blocksize)
: _blockSize(blocksize)
, _total(0)
, _current(0)
, _headNode(NULL)
, _currentNode(NULL)
{
	_tcscpy(_tempDir, tempdir);
}

void TempFilesStream::ReleaseNode(FilePathNode* node)
{
	if(!node)
		return;

#ifdef _WIN32
	DeleteFile(node->path);
#elif defined _LINUX || defined LINUX
	remove(node->path);
#endif 
	
	delete node;
}

FilePathNode* TempFilesStream::AddNode()
{
	FilePathNode* node = new FilePathNode();
	/*VO_BOOL rc = */CreateTempFileName(_tempDir, (VO_TCHAR *)(_T("mpw")), 0, node->path);

	return node;
}

long TempFilesStream::Position()
{
	return _blockSize * _current;
}

bool TempFilesStream::SetPosition(long pos)
{
	if (pos)
		return false;
	_current = 0;
	_currentNode = _headNode;
	return true;
}

bool TempFilesStream::Move(long delta)
{
	return false;
}

bool TempFilesStream::Read(void* data, uint32 size)
{
	return false;
	//if (size != _blockSize)
	//	return false;
	//if (_currentNode == NULL)
	//	return false;
	//HANDLE hfile = CreateFile(_currentNode->path, 
	//	GENERIC_READ,
	//	FILE_SHARE_READ,
	//	NULL,
	//	OPEN_EXISTING,
	//	0,
	//	NULL);
	//if (INVALID_HANDLE_VALUE == hfile)
	//	return false;
	//DWORD dw;
	//BOOL b = ReadFile(hfile, data, size, &dw, NULL);
	//CloseHandle(hfile);
	//if (b)
	//{
	//	FilePathNode* tmp = _currentNode;
	//	_currentNode = _currentNode->next;
	//	_current++;
	//	ReleaseNode(tmp);
	//}
	//return b;
}

bool TempFilesStream::Write(const void* data, uint32 size)
{
	return false;
	//if (size != _blockSize)
	//	return false;
	//FilePathNode* node = AddNode();
	//if (node == NULL)
	//	return false;
	//HANDLE hfile = CreateFile(node->path, 
	//	GENERIC_WRITE,
	//	FILE_SHARE_READ,
	//	NULL,
	//	CREATE_ALWAYS,
	//	0,
	//	NULL);
	//if (INVALID_HANDLE_VALUE == hfile)
	//	return false;
	//DWORD dw;
	//BOOL b = WriteFile(hfile, data, size, &dw, NULL);
	//if (dw != size)
	//	b = false;
	//CloseHandle(hfile);
	//if (b)
	//{
	//	if (_currentNode)
	//		_currentNode->next = node;
	//	else
	//		_headNode = node;
	//	_currentNode = node;
	//	_current++;
	//	_total++;
	//}
	//else
	//	ReleaseNode(node);
	//return b;
}


// ReserveSpace
//====================================================================

ReserveSpace::ReserveSpace(int size)
//: _hfile(NULL)
: _size(size)
{
}

ReserveSpace::~ReserveSpace()
{
	Release();
}

void ReserveSpace::Release()
{
	//if (_hfile)
	//{
	//	CloseHandle(_hfile);
	//	_hfile = NULL;
	//	DeleteFile(_filename);
	//}
}

bool ReserveSpace::SetFile(VO_PTCHAR filename)
{
	return false;
	//Release();
	//HANDLE hfile = CreateFile(filename, 
	//	GENERIC_READ | GENERIC_WRITE,
	//	FILE_SHARE_READ,
	//	NULL,
	//	CREATE_ALWAYS,
	//	0,
	//	NULL);
	//if (INVALID_HANDLE_VALUE == hfile)
	//	return false;
	//_hfile = hfile;
	//_tcscpy(_filename, filename);
	//return SetSize(_size);
}

bool ReserveSpace::SetSize(int size)
{
	_size = size;
	//if (_hfile)
	//{
	//	if (INVALID_SET_FILE_POINTER == SetFilePointer(_hfile, _size, NULL, FILE_BEGIN))
	//		return false;
	//	return SetEndOfFile(_hfile);
	//}
	return true;
}


// BufferOutStream
//====================================================================

BufferOutStream::BufferOutStream(Stream* target, uint32 size)
: buf(0)
, bs(0)
, ts(target)
, reserveSpace(NULL)
{
	buf = new uint8[size];
	if (buf)
		bs = new MemStream(buf, size);
}

BufferOutStream::BufferOutStream(Stream* target, MemStream* buffer)
: buf(0)
, bs(buffer)
, ts(target)
, reserveSpace(NULL)
{
}

BufferOutStream::~BufferOutStream()
{
	if (buf)
	{
		delete[] buf;
		if (bs)
			delete bs;
	}
	//otherwise it is a external buffer stream, don't delete it.
}

long BufferOutStream::Position()
{
	//return ts->Position() + bs->Position();
	if(ts)
		return ts->Position();

	return 0;
}

bool BufferOutStream::SetPosition(long pos)
{
	return false;
}

bool BufferOutStream::Move(long delta)
{
	return false;
}

bool BufferOutStream::Read(void* data, uint32 size)
{
	return false;
}

bool BufferOutStream::Write(const void* data, uint32 size)
{
	if(!bs || !ts)
		return false;

	bool b = true;

	//try write directly
	if (bs->Write(data, size))
		return true;

	//buffer must be overflow, then just fill the buffer
	uint32 blocksize = GetBufSize();
	uint32 freesize = blocksize - bs->Position();
	const uint8* p = (const uint8*) data;
	if (!bs->Write(p, freesize))
		return false;
	p += freesize;

	//write buffer to target
	if (!ts->Write(bs->GetData(), blocksize))
	{
		b = false;
		PrepareRetry();
		ts->Write(bs->GetData(), blocksize);
	}
	if (!ts->Flush()) //force write to device 
	{
		b = false;
		PrepareRetry();
		ts->Flush();
	}
	bs->SetPosition(0); //reset buffer

	//write left data directly first...
	uint32 leftsize = size - freesize;
	while (leftsize >= blocksize)
	{
		//write directly
		if (!ts->Write(p, blocksize))
		{
			b = false;
			PrepareRetry();
			ts->Write(p, blocksize);
		}
		p += blocksize;
		leftsize -= blocksize;
	}

	//then write to buffer
	bs->Write(p, leftsize);

	return b;
}


bool BufferOutStream::Flush()
{
	if(!bs || !ts)
		return false;

	uint32 leftsize = bs->Position();
	bool b = ts->Write(bs->GetData(), leftsize);
	bs->SetPosition(0);
	return b;
}

//BufferWrite
//==============================================================================

BufferWrite::BufferWrite()
: _bufwriter(NULL) 
, _memstream(NULL)
, _buffer(NULL)
, _filestream(NULL)
, _bufoutstream(NULL)
{
}

BufferWrite::~BufferWrite()
{
	Release();
}

void BufferWrite::Release()
{
	SAFEDEL(_bufwriter)
	SAFEDEL(_buffer)
	SAFEDEL(_memstream)
	if (_filestream)
	{
		delete _filestream;
#ifndef _USE_BLOCK_FILES
#ifdef _WIN32
		DeleteFile(_filename);
#elif defined _LINUX || defined LINUX
		remove(_filename);
#endif
#endif //_USE_BLOCK_FILES
	}
	SAFEDEL(_bufoutstream)
}

bool BufferWrite::InitWriter(Stream* stream, bool bMSB)
{
	if(stream == NULL)
		return false;

	_outstream = stream;
	_startpos = _outstream->Position();
	if (bMSB)
		_bufwriter = new WriterMSB(_outstream);
	else
		_bufwriter = new WriterLSB(_outstream);
	return _bufwriter != 0;
}

bool BufferWrite::InitMemoryStream(int size)
{
	_buffer = new uint8[size];
	if (_buffer == NULL)
		return false;
	_memstream = new MemStream(_buffer, size);
	return _memstream != 0;
}

bool BufferWrite::InitFileStream(VO_PTCHAR tempPath)
{
#ifdef _USE_BLOCK_FILES
#ifdef _WINDOWS
	_filestream = new CTempFileStream(gpFileOperator , tempPath, MAX_IO_BLOCK_SIZE);
#else //_WINDOWS
	_filestream = new CTempBufStream(tempPath, MAX_IO_BLOCK_SIZE);
#endif //_WINDOWS
	return true;
#else //_USE_BLOCK_FILES
	 CreateTempFileName(tempPath, _T("mpw"), 0, _filename);
	_filestream = new FileStream();
	if(_filestream == NULL)
		return false;

	return _filestream->Open(_filename, true);
#endif //_USE_BLOCK_FILES
}

bool BufferWrite::InitMemoryBuffer(int size, bool bMSB)
{
	if (!InitMemoryStream(size))
		return false;
	_instream = _memstream;
	return InitWriter(_memstream, bMSB);
}

bool BufferWrite::InitFileBuffer(VO_PTCHAR tempPath, bool bMSB)
{
	if (!InitFileStream(tempPath))
		return false;
	_instream = _filestream;
	return InitWriter(_filestream, bMSB);	
}

bool BufferWrite::InitBufferOutFileBuffer(VO_PTCHAR tempPath, ReserveSpace* reserveSpace, int size, bool bMSB)
{
	if (!InitMemoryStream(size))
		return false;
	if (!InitFileStream(tempPath))
		return false;
	_bufoutstream = new BufferOutStream(_filestream, _memstream);
	if(_bufoutstream == NULL)
		return false;

	_bufoutstream->SetReserveSpace(reserveSpace);
	_instream = _filestream;
	return InitWriter(_bufoutstream, bMSB);	
}

bool BufferWrite::Copy(Writer& dstwriter) const
{
	if(_instream == NULL)
		return false;

	const uint32 blocksize = MAX_IO_BLOCK_SIZE;
	uint8* buf = new uint8[blocksize];
	bool b  = true;
	uint32 size = GetWrittenSize();
	_instream->SetPosition(_startpos);
	while (size >= blocksize)
	{
		b = _instream->Read(buf, blocksize);
		if (b)
		{
			b = dstwriter.Write(buf, blocksize);
			if (b)
				b = dstwriter.Flush();
		}
		if (!b)
			break;
		size -= blocksize;
	}
	if (size > 0)
	{
		b = _instream->Read(buf, size);
		if (b)
			b = dstwriter.Write(buf, size);
	}
	delete buf;

	if (_bufoutstream) //write data in memory buffer since we do not flush it to file
	{
		b &= dstwriter.Write(_buffer,  _memstream->Position());
	}

	b &= dstwriter.Flush();
	//Release(); // delete the temp file to save space
	return b;
}


//ChunkOffsetBoxEx
//==============================================================================

bool ChunkOffsetBoxEx::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	return b && Copy(w);
}

bool ChunkOffsetBoxEx::AddEntry(uint32 chunk_offset)
{
	Writer *writer =  GetWriter();
	if(!writer)
		return false;

	bool b = writer->Write(chunk_offset);
	entry_count++;
	return b;
}


//SampleSizeBoxEx
//==============================================================================

const uint32 DIFFERENT_SAMPLE_SIZE_FLAG = 0xffffffffL; //don't use 0 for safe

bool SampleSizeBoxEx::DumpExt(Writer& w) const
{
	bool b = w.Write(sample_size);
	b &= w.Write(sample_count);
	if (sample_size == 0) // not same size
		b &= Copy(w);
	return b;
}

bool SampleSizeBoxEx::EndAdd()
{
	if (sample_size == DIFFERENT_SAMPLE_SIZE_FLAG)
		sample_size = 0;
	return true;
}

bool SampleSizeBoxEx::AddEntry(uint32 size)
{
	bool b = true;
	if (sample_count)
	{
		Writer* pWriter = GetWriter();
		if(!pWriter)
			return false;

		if (sample_size == DIFFERENT_SAMPLE_SIZE_FLAG)
			b &= pWriter->Write(size);
		else
		{
			//check if different or not
			if (size != sample_size)
			{
				for (uint32 i = 0; i < sample_count; i++)
					b &= pWriter->Write(sample_size);
				sample_size = DIFFERENT_SAMPLE_SIZE_FLAG;
				b &= pWriter->Write(size);
			}
			//do nothing if same
		}
	}
	else
	{
		sample_size = size;
		// do not write first sample right now
	}
	sample_count++;
	return b;
}


//SyncSampleBoxEx
//==============================================================================

bool SyncSampleBoxEx::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	return b && Copy(w);
}

bool SyncSampleBoxEx::AddEntry(uint32 index)
{
	Writer* pWriter = GetWriter();
	if(!pWriter)
		return false;

	bool b = pWriter->Write(index);
	entry_count++;
	return b;
}

//TimeToSampleBoxEx
//==============================================================================

bool TimeToSampleBoxEx::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	return b && Copy(w);
}

bool TimeToSampleBoxEx::AddEntry(uint32 delta)
{
	bool b = true;
	if (delta != _current_delta)
	{
		if (_current_count)
		{
			Writer* pWriter = GetWriter();
			if(!pWriter)
				return false;

			b = pWriter->Write(_current_count);
			b &= pWriter->Write(_current_delta);
			entry_count++;
		}
		_current_count = 1;
		_current_delta = delta;
	}
	else
		_current_count++;
	
	return b;
}

bool TimeToSampleBoxEx::EndAdd()
{
	bool b = true;
	if (_current_count)
	{
		Writer* pWriter = GetWriter();
		if(!pWriter)
			return false;

		b = pWriter->Write(_current_count);
		b &= pWriter->Write(_current_delta);
		entry_count++;
		_current_count = 0;
	}
	return b;
}

#ifdef _USE_SAMPLE_TIME

bool TimeToSampleBoxEx::AddEntry2(uint32 time)
{
	bool b = true;
	if (_csample)
	{
		b = AddEntry(time - _current_time);
	}
	_current_time = time;
	++_csample;
	return b;
}

bool TimeToSampleBoxEx::EndAdd2()
{
	// add last sample
	if (_csample > 1)
		AddEntry2(_current_time + _current_time / (_csample - 1));
	return EndAdd();
}

#endif //_USE_SAMPLE_TIME


//SampleToChunkBoxEx
//==============================================================================

bool SampleToChunkBoxEx::DumpExt(Writer& w) const
{
	bool b = w.Write(entry_count);
	return b && Copy(w);
}

bool SampleToChunkBoxEx::AddEntry(uint32 chunk, uint32 samples_per_chunk)
{
	bool b = true;
	
	if (samples_per_chunk != _samples_per_chunk)
	{
		if (_first_chunk)
		{
			Writer* pWriter = GetWriter();
			if(!pWriter)
				return false;

			b = pWriter->Write(_first_chunk);
			b &= pWriter->Write(_samples_per_chunk);
			b &= pWriter->Write(_sample_description_index);
			entry_count++;
		}
		_first_chunk = chunk;
		_samples_per_chunk = samples_per_chunk;
	}

	return b;
}

bool SampleToChunkBoxEx::EndAdd()
{
	bool b = true;
	if (_first_chunk)
	{
		Writer* pWriter = GetWriter();
		if(!pWriter)
			return false;

		b = pWriter->Write(_first_chunk);
		b &= pWriter->Write(_samples_per_chunk);
		b &= pWriter->Write(_sample_description_index);
		entry_count++;
		_first_chunk = 0; //for call EndAdd multi time
	}
	return b;
}


//BufferChunk
//==============================================================================

BufferChunk::BufferChunk()
: _buffer(NULL)
, _size(0)
, _csample(0)
, _maxsize(0)
{
}

BufferChunk::~BufferChunk()
{
	if (_buffer)
		delete _buffer;
}

bool BufferChunk::Init(uint32 maxsize)
{
	if (_buffer)
	{
		delete _buffer;
		_buffer = NULL;
	}
	_buffer = new uint8[maxsize];
	if (_buffer == NULL)
		return false;
	_maxsize = maxsize;
	Reset();
	return true;
}

void BufferChunk::Reset()
{
	_size = 0;
	_csample = 0;
}

bool BufferChunk::AddSample(const BufferSample* sample)
{
	if(!sample)
		return false;

	if (_size + sample->size > _maxsize)
	{
		return false; // chunk ready
	}

	memcpy(_buffer + _size, sample->buffer, sample->size);
	_size += sample->size;
	_csample++;
	return true;
}


//TrackWriter
//==============================================================================

TrackWriter::TrackWriter(MovieWriter* mw)
: _moviewr(mw)
, _mdatwr(NULL)
, _csample(0)
, _cchunk(0)
, _datasize(0)
, _trakbox(0)
, _sttsbox(0)
, mbIndexIn(false)
, mbIndexFirst(false)
, mnWriteTrunkTime(0)
, mnChunkDurtion(0)
{
}

TrackWriter::~TrackWriter()
{
	if (_trakbox)
	{
		if(_moviewr && _moviewr->GetMoovBox())
		{
			_moviewr->GetMoovBox()->RemoveChild(_trakbox); //let I (not parent) manage it
			delete _trakbox;
		}
	}
}

bool TrackWriter::SetMaxChunkSize(int maxsize)
{
	if(!_moviewr)
		return false;

	int oldsize = _chunk.GetMaxSize();
	if (!_moviewr->IncReserveSpace(maxsize - oldsize))
		return false;
	if (_chunk.GetSize())
	{
		// save current data
		if (!AddChunk(&_chunk))
			return false;
	}
	return _chunk.Init(maxsize);
}

bool TrackWriter::AddSample(BufferSample* sample)
{
	if(!sample)
		return false;

	if (_csample == 0)
		BuildBoxes();

	bool b = true;
	if ( (mnChunkDurtion != 0 && (sample->time - mnWriteTrunkTime > mnChunkDurtion))  || false == _chunk.AddSample(sample))
	{
		// here we get a chunk
		if (!AddChunk(&_chunk))
			b = false;
		_chunk.Reset();
		if (!_chunk.AddSample(sample))
		{
			_chunk.Init(sample->size);
			if (!_chunk.AddSample(sample))
				return false; //cannot get a chunk here
		}

		mnWriteTrunkTime = sample->time;
	}

	++_csample; //sample index
	_datasize += sample->size;
#ifdef _USE_SAMPLE_TIME
	SetDuration(sample->time);
#else //_USE_SAMPLE_TIME
	uint32 dur = GetDuration();
	dur += sample->duration;
	SetDuration(dur);
#endif //_USE_SAMPLE_TIME

	if(_sttsbox == NULL)
		return false;

	//write sample time stamp
#ifdef _USE_SAMPLE_TIME
	//if (!_sttsbox->AddEntry2((uint64)sample->time * TIME_DIVISOR / _track.GetTimeScale()))
	if (!_sttsbox->AddEntry2(sample->time)) //unit: 1/timescale s
#else //_USE_SAMPLE_TIME
	//if (!_sttsbox->AddEntry((uint64)sample->duration * TIME_DIVISOR / _track.GetTimeScale()))
	if (!_sttsbox->AddEntry(sample->time)) //unit: 1/timescale s
#endif //_USE_SAMPLE_TIME
		b = false; 

	//write sample size
	if (_stszbox && !_stszbox->AddEntry(sample->size))
		b = false;

	//write sync sample index
	if (sample->sync)
		if (_stssbox && !_stssbox->AddEntry(_csample))
			b = false;

	return b;
}

bool TrackWriter::AddChunk(const BufferChunk* chunk)
{
	if(!chunk)
		return false;

	bool b = true;
	uint32 pos = 0;

	if(mbIndexIn)
	{
		pos = _moviewr->GetmdatPos();
		if(pos == 0)
		{
			pos = _mdatwr.Position();
		}

		_moviewr->SetmdatPos(pos + chunk->GetSize());
	}
	else
	{
		//remember chunk offset
		pos = _mdatwr.Position();
		if(chunk->GetSize() == 0)
			return true;
	
		//write chunk data
		if (!_mdatwr.Write(chunk->GetBuffer(), chunk->GetSize()))
		{
			b = false;

			if(_moviewr)
				_moviewr->ReleaseReserveSpace(); //retry
			_mdatwr.Write(chunk->GetBuffer(), chunk->GetSize());
		}
		//force write to device
		if (!_mdatwr.Flush())
		{
			b = false;
			if(_moviewr)
				_moviewr->ReleaseReserveSpace(); //retry
			_mdatwr.Flush();
		}
	}

	//write chunk offset
	if (_stcobox && !_stcobox->AddEntry(pos))
	{
		b = false;
	}

	++_cchunk; //chunk index

	//write samples per chunk
	if (_stscbox && !_stscbox->AddEntry(_cchunk, chunk->GetSamples()))
	{
		b = false;
	}

	return b;
}

bool TrackWriter::Open(Stream* stream)
{
	_mdatwr.SetStream(stream);
	/*bool b = */SetMaxChunkSize(DEFAULT_MAX_CHUNK_SIZE);
	_csample = 0;
	_cchunk = 0;
	_datasize = 0; 
	SetDuration(0);

	// create root box
	_trakbox = new Box(Box::btTRAK);
	_tkhdbox = new TrackHeaderBox();
	_stcobox = new ChunkOffsetBoxEx();
	_stszbox = new SampleSizeBoxEx();
	_stscbox = new SampleToChunkBoxEx();
	_sttsbox = new TimeToSampleBoxEx();
	_stssbox = new SyncSampleBoxEx();
	_mdhdbox = new MediaHeaderBox();
	_hdlrbox = new HandlerBox();
	_minfbox = new Box(Box::btMINF);
	_stsdbox = new SampleDescriptionBox();

	_elstbox = new EditListBox(); //EditList Support, 090104
	_elstbox->SetEntryCount(1); //EditList Support, 090104
	Box* edtsbox = new Box(Box::btEDTS); //EditList Support, 090104

	Box* mdiabox = new Box(Box::btMDIA);
	Box* dinfbox = new Box(Box::btDINF);
	DataReferenceBox* drefbox = new DataReferenceBox();
	FullBox* urlbox = new FullBox(Box::btURL, 0, 0x000001);
	Box* stblbox = new Box(Box::btSTBL);

	_moviewr->GetMoovBox()->AddChild(_trakbox);
	_trakbox->AddChild(_tkhdbox);
	_trakbox->AddChild(edtsbox);  //EditList Support, 090104
	_trakbox->AddChild(mdiabox);
	edtsbox->AddChild(_elstbox);  //EditList Support, 090104
	mdiabox->AddChild(_mdhdbox);
	mdiabox->AddChild(_hdlrbox);
	mdiabox->AddChild(_minfbox);
	_minfbox->AddChild(dinfbox);
	dinfbox->AddChild(drefbox);
	drefbox->AddChild(urlbox);
	_minfbox->AddChild(stblbox);
	stblbox->AddChild(_stsdbox);
	stblbox->AddChild(_sttsbox);
	stblbox->AddChild(_stssbox);
	stblbox->AddChild(_stcobox);
	stblbox->AddChild(_stscbox);
	stblbox->AddChild(_stszbox);

	drefbox->SetEntryCount(1);

	ReserveSpace* reserveSpace = _moviewr->GetReserveSpace();
	if (!_stcobox->InitBufferOutFileBuffer(_moviewr->GetTempDirectory(), reserveSpace, MAX_IO_BLOCK_SIZE))
	{
		return false;
	}
	if (!_stszbox->InitBufferOutFileBuffer(_moviewr->GetTempDirectory(), reserveSpace, MAX_IO_BLOCK_SIZE))
	{
		return false;
	}
	if (!_stscbox->InitBufferOutFileBuffer(_moviewr->GetTempDirectory(), reserveSpace, MAX_IO_BLOCK_SIZE))
	{
		return false;
	}
	if (!_sttsbox->InitBufferOutFileBuffer(_moviewr->GetTempDirectory(), reserveSpace, MAX_IO_BLOCK_SIZE))
	{
		return false;
	}
	if (!_stssbox->InitBufferOutFileBuffer(_moviewr->GetTempDirectory(), reserveSpace, MAX_IO_BLOCK_SIZE))
	{
		return false;
	}

	return true;
}

int TrackWriter::GetMaxBufferDataSize() const
{
	return MAX_IO_BLOCK_SIZE * 5 + GetMaxChunkSize();
}

bool TrackWriter::Flush()
{
	AddChunk(&_chunk);
	_chunk.Reset();

	return true;
}


bool TrackWriter::Close()
{
	if (IsClosed()) //already closed
		return true;

	if(_moviewr)
		_moviewr->ReleaseReserveSpace();

	bool b = false;

	if(_stszbox)
		b = _stszbox->EndAdd();

#ifdef _USE_SAMPLE_TIME
	if(_sttsbox)
		b &= _sttsbox->EndAdd2();
#else //_USE_SAMPLE_TIME
	if (_csample > 2)
	{
		uint32 dur = GetDuration();
		dur += dur / (_csample - 1);
		SetDuration(dur);
	}
	if(_sttsbox)
		b &= _sttsbox->EndAdd();
#endif //_USE_SAMPLE_TIME

	SetClosed();  //closed flag

	if (_chunk.GetSize())
	{
		b &= AddChunk(&_chunk);
		_chunk.Reset();
	}
	if(_stscbox)
		b &= _stscbox->EndAdd();
	if (_csample == 0)
		BuildBoxes();
	else
		UpdateBoxes();
	return b;
}

bool TrackWriter::UpdateBoxes()
{
	
	if(_tkhdbox)
	{
		//_tkhdbox->SetDuration((uint64)_track.GetDuration() * _track.GetTimeScale() / TIME_DIVISOR); 
		_tkhdbox->SetDuration(_track.GetDuration()); 
		_tkhdbox->UpdateTimeStamp();
	}

	
	if(_elstbox)
		_elstbox->SetEntry(0, _track.GetDuration(), 0); //EditList Support, 090104

	if(_mdhdbox)
	{
		//_mdhdbox->SetDuration((uint64)_track.GetDuration() * _track.GetTimeScale() / TIME_DIVISOR, 
		_mdhdbox->SetDuration(_track.GetDuration(), _track.GetTimeScale()); 
		_mdhdbox->UpdateTimeStamp();
	}
	
	if(_trakbox)
		_trakbox->UpdateSize();	

	return true;
}

bool TrackWriter::BuildBoxes()
{
	if(!_tkhdbox)
		return false;

	_tkhdbox->SetTrackID(_track.GetID());
	if (_track.IsAudio())
	{
		_tkhdbox->SetVolume(0x0100);
		_tkhdbox->SetVideoSize(0, 0);
	}
	else
	{
		_tkhdbox->SetVolume(0);
		_tkhdbox->SetVideoSize(_track.GetWidth(), _track.GetHeight());
	}
	//_tkhdbox->SetDuration((uint64)_track.GetDuration() * TIME_DIVISOR / _track.GetTimeScale()); 
	_tkhdbox->SetDuration(_track.GetDuration()); 
	_tkhdbox->UpdateTimeStamp();

	if(_elstbox)
		_elstbox->SetEntry(0, _track.GetDuration(), 0); //EditList Support, 090104

	if(_mdhdbox)
	{
		//_mdhdbox->SetDuration((uint64)_track.GetDuration() * TIME_DIVISOR / _track.GetTimeScale(), 
		_mdhdbox->SetDuration(_track.GetDuration(), _track.GetTimeScale()); 
		_mdhdbox->UpdateTimeStamp();
	}
	
	if(_hdlrbox)
		_hdlrbox->SetHandlerType(_track.GetHandlerType());
		
	switch (_track.GetType())
	{
	case Track::ttVideo:
	{
		VideoMediaHeaderBox* vmhdbox = new VideoMediaHeaderBox();
		_minfbox->AddChild(vmhdbox);
		break;
	}
		
	case Track::ttAudio:
	{
		SoundMediaHeaderBox* smhdbox = new SoundMediaHeaderBox();
		_minfbox->AddChild(smhdbox);
		break;
	}
		
	default:
		break;
	}
	
	SampleEntry* entry = BuildSampleEntry();
	if (entry)
	{
		uint32 dt = _track.GetDescriptorType();
		if (dt)
		{
			if (dt == FOURCC2_esds)
			{
				UnknownFullBox* desc = new UnknownFullBox(dt);
				desc->SetBody(_track.GetDescriptorData(), _track.GetDescriptorSize());
				entry->AddChild(desc);
			}
			else
			{
				DescriptorBox* desc = new DescriptorBox(dt);
				desc->SetBody(_track.GetDescriptorData(), _track.GetDescriptorSize());
				entry->AddChild(desc);
			}
		}

		if(_stsdbox)
		{
			_stsdbox->AddChild(entry);
			_stsdbox->SetEntryCount(1);
		}
		
	}
	if(_trakbox)
		_trakbox->UpdateSize();
	return true;
}

SampleEntry* TrackWriter::BuildSampleEntry()
{
	switch (_track.GetType())
	{
	case Track::ttVideo:
	{
		VisualSampleEntry* entry = new VisualSampleEntry(_track.GetSampleType()); //('vide');
		entry->SetWidth(_track.GetWidth());
		entry->SetHeight(_track.GetHeight());
		return entry;
	}
	
	case Track::ttAudio:
	{
		AudioSampleEntry* entry = new AudioSampleEntry(_track.GetSampleType()); //('soun');
		entry->SetSampleRate((uint16)_track.GetSampleRate());
		entry->SetChannelCount(_track.GetChannelCount()); //20050203
		entry->SetSampleSize(_track.GetSampleBits()); //20050203
		return entry;
	}
	
	default:
		break;
	}
	
	return 0;
}

void TrackWriter::SetIndexIn(bool bValue)
{
	bool bOld = mbIndexIn;
	mbIndexIn = bValue;

	if(bOld && !mbIndexIn)
	{
		mbIndexFirst = true;
	}
}



//MovieWriter
//==============================================================================

//namespace ISOM 
//{
	const int BASE_RESERVE_SIZE = 44; // + MAX_IO_BLOCK_SIZE * 16; //6/24/2008
//}

MovieWriter::MovieWriter()
: _writer(NULL)
, _ftypbox(NULL)
, _mdatbox(NULL)
, _moovbox(NULL)
, _mvhdbox(NULL)
, _moovboxSize(0)
, mnmdatPos(0)
, _ctrack(0)
, _trackid(0)
, _reserveSpace(NULL)
{
	_reserveSpace = new CReserveSpace2(gpFileOperator , BASE_RESERVE_SIZE + MAX_IO_BLOCK_SIZE);
	_tempDir[0] = 0; //default is empty
}

MovieWriter::~MovieWriter()
{
	// release tracks
	while (_ctrack)
	{
		delete _tracks[--_ctrack];
	}
	// release boxes
	SAFEDEL(_ftypbox);
	SAFEDEL(_mdatbox);
	SAFEDEL(_moovbox);
	SAFEDEL(_reserveSpace);

	//RemoveDirectory(GetTempDirectory());
}

bool MovieWriter::Reset()
{
	// release tracks
	while (_ctrack)
	{
		delete _tracks[--_ctrack];
	}
	// release boxes
	SAFEDEL(_ftypbox);
	SAFEDEL(_mdatbox);
	SAFEDEL(_moovbox);
	SAFEDEL(_reserveSpace);

	return true;
}

bool MovieWriter::Open(int type, Stream* stream, uint32 timescale)
{
	_ftypbox = new FileTypeBox();
	_moovbox = new Box(Box::btMOOV);
	_mdatbox = new Box(Box::btMDAT);
	_mvhdbox = new MovieHeaderBox();
	_moovbox->AddChild(_mvhdbox);
	
	SetFileFormat(FileFormat3GP); // 4/5/2006, default
	//SetTimeScale(90000);  // 4/5/2006, default time scale
	SetTimeScale(timescale);

	_writer.SetStream(stream);

	_ftypbox->Dump(_writer);

	if(_moovboxSize > 0)
	{
		FreeBox *pfree = new FreeBox();
		pfree->SetSize(_moovboxSize);
		pfree->Dump(_writer);

		delete pfree;
	}

	_mdatbox->SetAddress(_writer.Position());
	_mdatbox->Dump(_writer);

	//SetLastError(0);
	return true;
}


void MovieWriter::SetFileFormat(int fmt)
{
	if(!_ftypbox)
		return ;

	_ftypbox->ClearCompatibleBrands();
	
	switch (fmt & ~FileFormatAVC)
	{
	case FileFormat3GP:
		_ftypbox->SetMajorBrand(FOURCC2_3gp6);
		_ftypbox->AddCompatibleBrand(FOURCC2_3gp6);
		_ftypbox->AddCompatibleBrand(FOURCC2_3gp5);
		_ftypbox->AddCompatibleBrand(FOURCC2_3gp4);
		break;
	
	case FileFormatMP4:
		_ftypbox->SetMajorBrand(FOURCC2_mp42);
		_ftypbox->AddCompatibleBrand(FOURCC2_mp42);
		_ftypbox->AddCompatibleBrand(FOURCC2_mp41);
		_ftypbox->AddCompatibleBrand(FOURCC2_isom);
		break;
		
	case FileFormat3G2:
		_ftypbox->SetMajorBrand(FOURCC2_3g2a);
		_ftypbox->AddCompatibleBrand(FOURCC2_3g2a);
		_ftypbox->AddCompatibleBrand(FOURCC2_mp42);
		break;
	
	default:
		_ftypbox->SetMajorBrand(FOURCC2_3gp6);
		_ftypbox->AddCompatibleBrand(FOURCC2_3gp6);
		_ftypbox->AddCompatibleBrand(FOURCC2_3gp5);
		_ftypbox->AddCompatibleBrand(FOURCC2_3gp4);
		break;
	}
		
	if (fmt & FileFormatAVC)
		_ftypbox->AddCompatibleBrand(FOURCC2_avc1);

	_ftypbox->UpdateSize();
}

TrackWriter* MovieWriter::AddTrack()
{
	if (_ctrack >= MAX_TRACKS)
	{
		return NULL;
	}
	TrackWriter* tw = new TrackWriter(this);
	if (!tw)
	{
		return NULL;
	}
	if (!tw->Open(_writer.GetStream()))
	{
		delete tw;
		return NULL;
	}
	if (!IncReserveSpace(tw->GetMaxBufferDataSize()))
	{
		delete tw;
		return NULL;
	}
	tw->GetTrack()->SetID(++_trackid);
	tw->GetTrack()->SetTimeScale(GetTimeScale());
	_tracks[_ctrack++] = tw;
	return tw;
}

bool MovieWriter::RemoveTrack(TrackWriter* track)
{
	if(!track)
		return true;

	int target = FindTrack(track);
	if (target < 0)
		return false;
	if (!IncReserveSpace(-track->GetMaxBufferDataSize()))
		return false;
	//remove it from array
	for (int i = target+1; i < _ctrack; i++)
	{
		_tracks[i - 1] = _tracks[i];
	}
	--_ctrack;
	delete track;
	return true;
}

TrackWriter* MovieWriter::GetTrackByID(uint32 id)
{
	int i = _ctrack;
	while (--i >= 0)
	{
		if (id == _tracks[i]->GetTrack()->GetID())
			return _tracks[i];
	}
	return NULL;
}

int MovieWriter::FindTrack(TrackWriter* track)
{
	int i = _ctrack;
	while (--i >= 0)
	{
		if (track == _tracks[i])
			return i;
	}
	return -1;
}

bool MovieWriter::Close()
{
	bool b = true;
	//ReleaseReserveSpace(); //release in TrackWriter::Close

	//MessageBox(NULL, _T("MovieWriter::Close"), _T("voMP4FW"), MB_OK);

	_duration = 0;
	int i = _ctrack;
	while (--i >= 0)
	{
		TrackWriter* tw = _tracks[i];
		b = tw->Close(); //must close first
		if (_duration < tw->GetDuration())
			_duration  = tw->GetDuration();
	}

	//MessageBox(NULL, _T("MovieWriter::Close After close tracks"), _T("voMP4FW"), MB_OK);

	if(!_mdatbox)
		return false;

	// calc & write mdat size
	uint32 pos1 = _mdatbox->GetAddress();
 	uint32 pos2 = _writer.Position();
	if(mnmdatPos != 0)
		pos2 = mnmdatPos;

	_mdatbox->SetSize(pos2 - pos1);
	_writer.SetPosition(pos1);
	_mdatbox->Dump(_writer);
	_writer.SetPosition(pos2);

	// write moov box

	if(!_mvhdbox)
		return false;

	_mvhdbox->SetNextTrackID(++_trackid);
	//_mvhdbox->SetDuration((uint64)_duration * TIME_DIVISOR / _timescale, _timescale); 
	_mvhdbox->SetDuration(_duration, _timescale); 
	_mvhdbox->UpdateTimeStamp();

	if(!_moovbox)
		return false;

	if(_moovboxSize > 0)
	{
		uint32 nSize = _ftypbox->GetSize();
		_writer.SetPosition(nSize);
	}

	_moovbox->UpdateSize();

	uint32 nFreeSize = 0;
	if(_moovboxSize == 0)
		_moovboxSize = _moovbox->GetSize();
	else
	{
		uint32 nSize = _moovbox->GetSize();
		if(nSize > _moovboxSize)
		{
			_moovboxSize = 0;
		}
		else
		{
			nFreeSize = _moovboxSize - nSize;
		}
	}

	//OutputDebugString(_T("MovieWriter::Close Before _moovbox->Dump\n"));

#ifdef _DEBUG
	//DWORD t1 = GetTickCount();
#endif //_DEBUG
#if 1
	_moovbox->Dump(_writer);
#else
	BufferOutStream bos(_writer.GetStream(), MAX_IO_BLOCK_SIZE);
	WriterMSB wr(&bos);
	b = _moovbox->Dump(wr);
	bos.Flush();
#endif
#ifdef _DEBUG
	//DWORD t2 = GetTickCount();
	//DWORD t = t2 - t1;
#endif //_DEBUG

	if(nFreeSize > 0)
	{
		FreeBox *pfree = new FreeBox();
		pfree->SetSize(nFreeSize);
		pfree->Dump(_writer);

		delete pfree;
	}


	//OutputDebugString(_T("MovieWriter::Close Quit\n"));

	return b;
}

uint32 MovieWriter::GetFileSize() 
{
	if(!_ftypbox || !_moovbox)
		return 0;

	uint32 filesize = _ftypbox->GetSize() + 8; //mdat header size
	_moovbox->UpdateSize();
	filesize += _moovbox->GetSize();
	for (int i = 0; i < _ctrack; i++)
	{
		TrackWriter* tw = _tracks[i];
		filesize += tw->GetDataSize();
		if (!tw->IsClosed())
			filesize += BASE_RESERVE_SIZE;
	}
	return filesize;
}

bool MovieWriter::SetTempDirectory(VO_PTCHAR path)
{
	//BOOL b = CreateDirectory(path, NULL);
	bool b = true;
	if (b)
	{
		vostrcpy(_tempDir, path);
		int len = vostrlen(_tempDir);
#ifdef WIN32
		if (_tempDir[len - 1] != VO_TCHAR('\\'))
		{
			_tempDir[len] = VO_TCHAR('\\');
#else
		if (_tempDir[len - 1] != VO_TCHAR('/'))
		{
			_tempDir[len] = VO_TCHAR('/');
#endif 
			_tempDir[len + 1] = VO_TCHAR('\0');
		}
	}
	if (b)
	{
		VO_TCHAR path[260];
		CreateTempFileName(_tempDir, (VO_TCHAR *)(_T("mpw")), 0, path);

		if(_reserveSpace)
			b = _reserveSpace->SetFile(path);
	}
	return b;
}
