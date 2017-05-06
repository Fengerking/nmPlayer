#ifndef _ISOM_RD_H
#define _ISOM_RD_H

#include "../MP4Base/mpxobj.h"
#include "../MP4Base/mpxio.h"
#include "fMacros.h"
#include "voType.h"
#include "fCC2.h"

#include "voDrmCallback.h"
#include "voDSDRM.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMp4Reader2;
extern uint64 _time_divisor;


//#define _ERR_TOL //Error Tolerance
#define _CHECK_READ //check return value of read operation
#define ERR_CHECK_READ 0xffffffff


typedef struct  
{
	uint32 nTrackID;
	uint32 nDefaultDuration;
	uint32 nDefaultSize;
}DEFAULT_SAMPLEINFO;



#ifdef _SUPPORT_LANG

#include "mpxutil.h"

#define LANGCODE(a, b, c) ((c - 0x60) | ((b - 0x60) << 5) | ((a - 0x60) << 10))

// see http://www.loc.gov/standards/iso639-2/php/code_list.php
#define LANG_ENG LANGCODE(FOURCC2_e', nFOURCC2_, 'g)  //English
#define LANG_CHI LANGCODE(FOURCC2_c', hFOURCC2_, 'i)  //Chinese
#define LANG_JPN LANGCODE(FOURCC2_j', pFOURCC2_, 'n)  //Japanese
#define LANG_FIN LANGCODE(FOURCC2_f', iFOURCC2_, 'n)  //Finnish

#endif //_SUPPORT_LANG


enum ENUM_MP4RRC
{
	MP4RRC_OK = 0,

	MP4RRC_READ_FAILED,
	MP4RRC_SEEK_FAILED,

	MP4RRC_NO_FTYP,
	MP4RRC_NO_MOOV,
	MP4RRC_NO_MDAT,
	MP4RRC_NO_MVHD,
	MP4RRC_NO_TKHD,
	MP4RRC_NO_MDIA,
	MP4RRC_NO_MDHD,
	MP4RRC_NO_HDLR,
	MP4RRC_NO_MINF,
	MP4RRC_NO_STBL,
	MP4RRC_NO_STSD,
	MP4RRC_NO_STTS,
	MP4RRC_NO_CTTS,
	MP4RRC_NO_STSS,
	MP4RRC_NO_STSC,
	MP4RRC_NO_STSZ,
	MP4RRC_NO_STCO,
	MP4RRC_NO_ESDS,

	MP4RRC_NO_SUCH_TRACK,
	MP4RRC_NO_SUCH_SAMPLE,
	MP4RRC_UNSUPPORTED_SAMPLE_TYPE,
	MP4RRC_UNKNOWN_SAMPLE_TYPE,
	MP4RRC_BAD_MOOV,
	MP4RRC_BAD_DESCRIPTOR,
	MP4RRC_BAD_DATA,

	MP4RC_NO_CHUNK,
	MP4RC_NO_SAMPLE,

	MP4RC_INIT_DATA_STREAM_FAIL,

	MP4RRC_MISS_TRACK,
	MP4RC_INVALID_STTS,
	MP4RC_INVALID_TRAKS,

	MP4RRC_DRMS_SAMPLE_TYPE, // FOURCC2_drms
	//09/01/2011,Leon
	MP4RRC_ENTRY_SAMPLE_TYPE,
	MP4RRC_AUDIO_TRACK,
	MP4RRC_VIDEO_TRACK,
	//12/09/2011,Leon
	MP4RRC_SUBTITLE_TRACK,

	MP4RRC_INVALID_TFRA,
	MP4RRC_INVALID_TRUN,

	MP4RRC_NO_MFRA,
	MP4RRC_NO_TFRA,
	MP4RRC_NO_TRUN,
	MP4RRC_NO_TFHD,
	MP4RRC_NO_MOOF,
	MP4RRC_NO_TRAF,
	MP4RRC_NO_SINF,
	MP4RRC_NO_FRMA,
	MP4RRC_NO_SCHI,

	MP4RRC_NO_SIDX,
	MP4RRC_NO_TREX,
	MP4RRC_NO_SUBSIDX,

	MP4RRC_MAX = 0x7FFFFFFF
};

#if 0
typedef int MP4RRC;
#else
typedef enum ENUM_MP4RRC MP4RRC;
#endif



//09/14/2011,leon
MP4RRC CheckType(uint32 sampletype);


void SetTimeUnit(uint32 divisor=1000);



class BufferInStream : public Stream
{
public:
	BufferInStream();
	~BufferInStream();

	bool Init(Stream* source, uint32 maxsize, uint32 blocksize=MAX_IO_BLOCK_SIZE);

	bool Init1(Stream* source, uint32 blocksize=MAX_IO_BLOCK_SIZE); //called only once
	bool Init2(uint32 maxsize, ABSPOS position);  //can be called multi times

	virtual ABSPOS Position();
	virtual bool SetPosition(ABSPOS pos);
	virtual bool Move(RELPOS delta);
	virtual bool Read(void* data, uint32 size);
	virtual bool Write(const void* data, uint32 size);
	virtual bool Flush();

protected:
	uint32 GetBufSize() const { return _blocksize; }
	uint8* GetBuffer() { return _bs->GetBuffer(); }

private:
	Stream* _ss; //source stream
	MemStream* _bs; //buffer stream
	ABSPOS _offset; // current offset (address)
	ABSPOS _start;  // start offset (address)
	ABSPOS _maxsize; //max datasize can be buffered
	uint32 _blocksize; // size of buffer stream's buffer
};


class BufferInBox
{
public:
	BufferInBox();
	~BufferInBox();

	MP4RRC Init(Stream* stream, uint32 bodySize);

	uint32 GetEntryCount() const { return _entry_count; }
	VO_VOID	SetMaxInitSize(VO_U32 uMaxSize){m_uMaxInitSize = uMaxSize;}
protected:
	Reader* GetReader() { return &_reader; }

private:
	BufferInStream _bs;
	ReaderMSB _reader;
	uint32 _body_size;

protected:
	ABSPOS _data_pos;
	uint32 _entry_count;
	uint32 _current_entry;
	VO_U32 m_uMaxInitSize;	


};


class SttsBox : public BufferInBox
{
public:
	SttsBox();
	MP4RRC Init(Stream* stream, uint32 bodySize);
	uint64 GetSampleTime(int32 index);
	int GetSampleIndex(uint64 time);
	uint32 GetCurrentDelta() const { return _current_delta; }

private:
	uint32 _index_base;
	uint64 _time_base;
	uint32 _current_count;
	uint32 _current_delta;

#ifdef _ERR_TOL

public:
	void SetAvgDuration(uint32 mul, uint32 div)
	{
		_avg_dur_mul = mul;
		_avg_dur_div = div;
	}

private:
	uint32 _avg_dur_mul;
	uint32 _avg_dur_div;

#endif //_ERR_TOL

};

class CttsBox: public BufferInBox
{
public:
	CttsBox();
	MP4RRC Init(Stream* stream, uint32 bodySize);
	int64 GetSampleTime(int32 index);
private:
	uint32 _index_base;
	uint64 _time_base;
	uint32 _current_count;
	int32  _current_delta;///<for QuickTime container,the delta is signed int
};

class StssBox : public BufferInBox
{
public:
	StssBox();
	MP4RRC Init(Stream* stream, uint32 bodySize, uint32 sampleCount);

	uint32 GetSampleSync(int index);
	int GetNextSyncPoint(int index);
	int GetPrevSyncPoint(int index);
private:
	uint32 _sample_count;
	int32 _current_sync_index;
	int32 _next_sync_index;
};

class StscBox : public BufferInBox
{
public:
	MP4RRC Init(Stream* stream, uint32 bodySize, uint32 chunkCount);
	//12/27/2011, leon add
#ifdef _SUPPORT_TTEXT
	MP4RRC Init2(Stream* stream, uint32 bodySize, uint32 chunkCount);
	MP4RRC GetSampleChunk2(int index, int &chunksampleindex, int &chunksamplecount);
#endif

	int GetSampleChunk(int index, int& chunksampleindex, int& chunksamplecount);
	int32 GetSamplesPerChunk(int index);
	uint32 GetSampleIndexBase() const { return _sample_index_base; }
	uint32 GetCurFragmentSampleCount() const { return _sample_current_count; }
private:
	uint32 _last_chunk_index; 
	uint32 _chunk_index_current;
	uint32 _chunk_index_next;
	uint32 _samples_per_chunk;
	uint32 _sample_index_base;
	uint32 _sample_current_count;

 //12/27/2011, leon add
	uint32 m_nCurChunkIndex;
	uint32 m_nNextChunkIndex;
	uint32 m_nCurChunkSampleCount;
	uint32 m_nNextChunkSampleCount;
	uint32 m_nCurSampleIndex;

};


class StszBox : public BufferInBox
{
public:
	MP4RRC Init(Stream* stream, uint32 bodySize);

	uint32 GetSampleCount() const { return GetEntryCount(); }
	uint32 GetSampleSize(int index);
	uint32 GetSampleSize() { return _sample_size; }

private:
	uint32 _sample_size;
};

class StcoBox : public BufferInBox
{
public:
	MP4RRC Init(Stream* stream, uint32 bodySize);

	uint32 GetChunkCount() const { return GetEntryCount(); }
	virtual uint32 GetChunkOffset(int index);

private:
};


class Co64Box : public StcoBox
{
	virtual uint32 GetChunkOffset(int index);
};

class Movie;

class Track
{
public:
	Track();
	virtual ~Track();
	virtual MP4RRC Open(Reader* r);
	virtual MP4RRC Close();
	virtual VO_U32	ResetIndex(){return VO_ERR_NOT_IMPLEMENT;};
public:
	void		SetFileFormat(uint32 format){ _nFileFormat = format;}
	uint32		IsIsml(){ return _nFileFormat == FOURCC2_isml ? 1 : 0; }
	uint32		IsDash(){ return _nFileFormat == FOURCC2_dash ? 1 : 0; }
	uint32		IsMov(){return _nFileFormat == FOURCC2_qt? 1 : 0;}
	VO_BOOL IsCencdrm(){ 
		return m_bIsCencDrm;
	}
//	Movie*		GetMovie(){return _movie;}
	void		SetMovie(Movie* m) { _movie = m; }
	uint32		GetTrackID(){return _nTrack_ID;}
	void		GetMatrix(int32 matrix[9]);
	VO_BOOL		IsPDIsmv(){return m_bPDIsmv;}
	VO_VOID		SetPDIsmv(VO_BOOL bPDIsmv){m_bPDIsmv = bPDIsmv;}
public:
	uint32		GetHandlerType() const { return _handler_type; }
	uint32		GetSampleType() const { return _sample_type; }
	uint32		GetDescriptorType() const { return _descriptor_type; }
	uint32		GetDescriptorSize() const { return _descriptor_size; }
	uint8*		GetDescriptorData() const { return _descriptor_data; }

	virtual uint32	GetSampleCount();
	uint32			GetDuration();
	virtual uint32	GetMaxSampleSize();
	uint32			GetTotalSampleSize();
	virtual	uint32	GetBitrate();

	bool			IsAudio() const { return _handler_type == FOURCC2_soun; }//0x736f756e
	bool			IsVideo() const { return _handler_type == FOURCC2_vide; } //0x76696465
	bool			IsText() const { return _handler_type == FOURCC2_text; } //0x74657874

	uint16			GetWidth() const { return _videofmt.width; }
	uint16			GetHeight() const { return _videofmt.height; }
	uint32			GetSampleRate() const { return _audiofmt.samplerate; }
	/*add by leon, 05/16/2012, #14418*/
	void			SetSampleRate(uint32 samplerate){if(IsMov()) _audiofmt.samplerate = (uint16)samplerate;}
	uint16			GetChannelCount() const { return _audiofmt.channelcount; }
	//2012/03/12, Leon, fix quicktime #11375
	uint16			GetSampleBits()  {	return (uint16)(IsMov() ? _audioqtff.bytesPerSample * _audiofmt.channelcount :_audiofmt.samplesize);}
	//2012/03/12, Leon, fix quicktime #11375
	uint32			GetBytesPerFrame()  {	return _audioqtff.bytesPerFrame ? _audioqtff.bytesPerFrame :0;}

public:
	virtual uint32	GetSampleSize(int index);
#ifndef _SUPPORT_TSOFFSET
	virtual uint64	GetSampleTime(int32 index);
	virtual uint64	GetSampleTime2(int32 index);
	virtual int		FindSampleByTime(uint32 time);
#endif //_SUPPORT_TSOFFSET

#ifdef _SUPPORT_TTEXT
	virtual uint32 GetSampleBuffer(void **ptr) {return  0;}
#endif
	virtual uint32	GetSampleAddress(int index);
	uint32			GetSampleAddressBase(int index);
	uint32			GetSampleAddressEnd(int index);
	virtual uint32	GetSampleSync(int index){return _stssBox.GetEntryCount() ? _stssBox.GetSampleSync(index): 1;}
	virtual uint32	GetSampleData(int index, void* buffer, int buffersize);
	virtual int		GetNextSyncPoint(int index) {return _stssBox.GetEntryCount() ? _stssBox.GetNextSyncPoint(index) : index; } //default is sync
	virtual int		GetPrevSyncPoint(int index) {  return _stssBox.GetEntryCount() ? _stssBox.GetPrevSyncPoint(index) : index;} //default is sync
	virtual int		GetSampleChunk(int index, int& chunksampleindex, int& chunksamplecount) {  return _stscBox.GetSampleChunk(index, chunksampleindex, chunksamplecount);  }

	uint32			GetChunkCount() const { return _stcoBox->GetChunkCount(); }
	uint32			GetChunkOffset(int index) { return _stcoBox->GetChunkOffset(index); }
	
	//10/18/2011 leon, add for push &play
	uint64			GetRealDurationByFilePos(uint64 pos);

	MP4RRC			GetSampleInfo(int index);
	virtual VO_U32	TrackGenerateIndex(VO_U32 uIndex, Reader* pReader);
	virtual VO_BOOL	IsIndexReady(VO_U32 uIndex, VO_U32* pMaxIndex);
	virtual VO_U32	GetMaxTimeByIndex(VO_U32 uIndex,VO_S64& ullTs);
protected:
	MP4RRC			ParseTkhdBox(Reader* r, int bodySize);
	MP4RRC			ParseMdhdBox(Reader* r, int bodySize);
	MP4RRC			ParseElstBox(Reader* r, int bodySize);
	MP4RRC			ParseHdlrBox(Reader* r, int bodySize);
	virtual MP4RRC	ParseStsdBox(Reader* r, int bodySize);
	MP4RRC			ParseAudioSampleEntry(Reader* r, int bodysize);
	MP4RRC			ParseVideoSampleEntry(Reader* r, int bodysize);
	virtual MP4RRC	ParseTextSampleEntry(Reader* r, int bodysize) {return  MP4RRC_OK ;} //12/10/2011, leon for subtitle
	virtual MP4RRC	ParseDescriptor(Reader* r, int bodysize);

public:
	uint32			GetTimeScale() const { return _timescale; }
private:
	uint32			GetMaxSampleSize(int iFrom, int iTo);
	uint32			GetTotalSampleSize(int iFrom, int iTo);

protected:
	uint32			_timescale;
	uint64			_duration;
	uint32			_handler_type;
	uint32			_sample_type;
	uint32			_descriptor_type;
	uint8*			_descriptor_data;
	uint32			_descriptor_size;

	//2012/02/14, add by Leon for quick time audio
	struct 
	{
		uint32 samplesPerPacket;
		uint32 bytesPerFrame;
		uint32 bytesPerSample;
		uint32 bytesPerPacket;
	}_audioqtff;


	union
	{
		struct
		{
			uint16 channelcount;
			uint16 samplesize;
			uint16 compressionID; //QuickTime, qtff.pdf  p.119
			uint16 packetsize; //QuickTime, qtff.pdf  p.119, must 0
			uint16 samplerate; //should be 16.16 fix point number, 11/29/2005
		} 
		_audiofmt;

		struct
		{
			uint16 width;
			uint16 height;
			uint32 horizresolution;
			uint32 vertresolution;
			uint16 frame_count;
			char compressorname[32];
			uint16 depth;
			int16 pre_defined;
		}
		_videofmt;

#ifdef _SUPPORT_TTEXT
		struct
		{
			uint32	displayFlags;
			int8	horizontal_justification;
			int8	vertical_justification;
			int8	background_color_rgba[4];
		}
		_textfmt;
#endif //_SUPPORT_TTEXT
	};


protected:
	SttsBox			_sttsBox;
	CttsBox			_cttsBox;
	StssBox			_stssBox;
	StscBox			_stscBox;
	StszBox			_stszBox;
	StcoBox*		_stcoBox; //may be stco or co64

    char		    *m_pTence;//CENC tence box data



protected:
	VO_S32			_current_sample_chunk;
	VO_S32			_current_sample_index;
	uint64			_current_sample_address;
	uint32			_nFileFormat;
	uint32			_nTrack_ID;
	VO_S32			_prev_sample_chunk;
	BufferInStream	_data_stream;
	uint64			_current_chunk_address;
	uint32			_current_chunk_size;
	int32			_matrix[9];
	Movie*			_movie;
	VO_BOOL			m_bPDIsmv;
	VO_BOOL			m_bIsCencDrm;
#ifdef _SUPPORT_LANG
public:
	// return ISO-639-2/T language code
	uint16			GetLang() const { return _lang; }
	// for debug
	const char*		GetLangName() const { return Util::LanguageText(_lang); }
private:
	uint16			_lang;
#endif //_SUPPORT_LANG

#ifdef _SUPPORT_TSOFFSET
private:
	uint64			_tsoffset; //time stamp offset
public:
	uint64			GetSampleTime(int index) { return _time_divisor * _sttsBox.GetSampleTime(index) / GetTimeScale() + _tsoffset; }
	uint64			GetSampleTime2(int index) { return _time_divisor * _sttsBox.GetSampleTime(index+1) / GetTimeScale() + _tsoffset; }
	int FindSampleByTime(uint32 time) 
	{ 
		return _sttsBox.GetSampleIndex((uint32)(
			((uint64)(time > _tsoffset ? time - _tsoffset : 0) * 
			GetTimeScale() + _time_divisor - 1) / _time_divisor)); 
	}
#endif //_SUPPORT_TSOFFSET
};


class Movie
{
public:
	Movie();
	~Movie();
	MP4RRC			Open(Reader* r);
	MP4RRC			Open(Reader* r, VO_BOOL isStreaming);
	MP4RRC			Close();

	VO_S32			GetTrackCount() const { return _cTracks; }
	MP4RRC			OpenTrack(int index, Track*& track);

	uint32			GetDuration() const {
						return (uint32)(_time_divisor * _duration / GetTimeScale());
					}
	uint64			GetMehdDuration() const{
						return _mehdduration;
					}
	uint32			GetHeaderSize() const { return uint32(_headersize); }
	uint32			GetHeaderPos() const { return uint32(_headerpos); }
	
	MP4RRC			ProgressiveOpen(Reader* r); // open for progressive download
	char*			GetCreateTime(){ return m_strCreationTime;}
	char*			GetGPS(){return m_strGPS;}
	uint32			GetMinorVersion(){return m_nMinorVersion;}
	uint32			GetEVideoContent(){return m_nEVideoContent;}
	uint32			GetDefaultDuration(uint32 nTrackID);
	uint32			GetDefaultSize(uint32 nTrackID);

	VO_BOOL			GetIsStreaming(){return m_bIsStreaming;}

	VO_VOID			DisableBoxType(VO_U32 uBoxType);
	VO_U32			GetBoxType(){return m_uEnableBox;}
	VO_VOID			GetMatrix(int32 matrix[9]);
	uint32			GetTimeScale() const { return _timescale; }
	uint32			IsIsml(){ return _nFileFormat == FOURCC2_isml?1 : 0; }
	uint32			IsDash(){ return _nFileFormat == FOURCC2_dash?1:0; }
	VO_BOOL         IsCencDrm(){ return m_bIsCENCDrm;}
	VO_VOID			SetDrmReader(CMp4Reader2 * pDrmReader){ m_DrmReader = pDrmReader;}
	CMp4Reader2*	GetDrmReader(){ return m_DrmReader; }
	VO_VOID         SetCENCDrmCallback(VO_DATASOURCE_FILEPARSER_DRMCB *pCB){ m_pCencDrmCallbackProc = pCB;};
	VO_DATASOURCE_FILEPARSER_DRMCB *GetCencDrmCallback(){return m_pCencDrmCallbackProc;}
protected:
	MP4RRC			ParseFtypBox(Reader* r,int bodySize);
	MP4RRC			ParseUdtaBox(Reader* r, int bodySize);

	MP4RRC			ParseMvhdBox(Reader* r, int bodySize);
	MP4RRC			ParseTrakBoxes(Reader* r, int totalSize);
	VO_S32			MoveToTrak(Reader* r, int index, bool onlySupported, int totalSize=0); //return the actually index
	MP4RRC			CheckTrackValid(Reader* r);
	MP4RRC			ParseMvexBox(Reader* r, int bodySize);
	MP4RRC			ParseTfraBoxes(Reader* r, int totalSize);
	VO_S32			GetTfraPos(Reader* r, int index, int totalSize);
	
protected:
	Stream*			GetSourceStream() { return m_pReader->GetStream(); }
	Reader*			GetSourceReader() { return m_pReader; }

private:
	VO_S32			CheckTrackType(Reader* r);

private:
	VO_S32			_cTracks;
	ABSPOS			_nTrakPos;
	Reader*			m_pReader;
	VO_BYTE  *      m_pPssh; //cenc drm.
private:
	//2011-12-1 , add by leon,
	char			m_strCreationTime[255];
	char			m_strModificationTime[255];
	//2012-02-15 , add by leon,
	char			m_strGPS[255];
	/*2012/03/05,add by leon*/
	uint32			m_nMinorVersion;
	/*for udta box cypt*/
	uint32			m_nEVideoContent;
	uint32			_timescale;
	uint64			_duration;   //the duration get from mvhd
	uint64			_mehdduration;//the duration get from mehd
	uint64			_headersize;
	uint64			_headerpos;
	DEFAULT_SAMPLEINFO	m_stDefaultInfo[10];
	VO_U32				m_uEnableBox;
	VO_S32				_matrix[9];	

/*09/01/2011,add by leon*/
private:
	uint32				_nFileFormat;
	VO_BOOL				m_bIsStreaming;
	VO_S32				_cTfras;
	ABSPOS				_nTfraPos;
	VO_BOOL				m_bHasFrag;
	CMp4Reader2*		m_DrmReader;
	VO_BOOL             m_bIsCENCDrm;
	VO_DATASOURCE_FILEPARSER_DRMCB *m_pCencDrmCallbackProc;//from controler for cenc drm
};


class TrackS1 : public Track
{
public:
	TrackS1();

	virtual uint32 GetSampleCount();
	virtual uint32 GetSampleSize(int index);
	virtual uint32 GetSampleAddress(int index);
	virtual uint64 GetSampleTime(int32 index);
	virtual uint64 GetSampleTime2(int32 index);
	
	virtual int FindSampleByTime(uint32 time);
	
	virtual uint32 GetSampleSync(int index) { return 1; } //always sync
	virtual int GetNextSyncPoint(int index) { return index; } 
	virtual int GetPrevSyncPoint(int index) { return index; } 

private:

	int _index_cache;
	uint32 _count_real_samples;
	//08/26/2011 add by leon
	uint32 _pre_sample_size;
	int GetPreSampleByteSize();
	//08/26/2011 add by leon
};

#ifdef _VONAMESPACE
}
#endif
#endif	//_ISOM_RD_H
