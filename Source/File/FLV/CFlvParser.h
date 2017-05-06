#pragma once

#include "CvoFileParser.h"
#include "FlvFileDataStruct.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class file_pos_rollback
{
public:
	file_pos_rollback( CGFileChunk * ptr_file_chunk , VO_U64 file_pos )
	{
		m_ptr_file_chunk = ptr_file_chunk;
		m_org_file_pos = file_pos;
	}

	file_pos_rollback( CGFileChunk * ptr_file_chunk )
	{
		m_ptr_file_chunk = ptr_file_chunk;
		m_org_file_pos = ptr_file_chunk->FGetFilePos();
	}

	~file_pos_rollback()
	{
		m_ptr_file_chunk->FLocate( m_org_file_pos );
	}

private:
	CGFileChunk * m_ptr_file_chunk;
	VO_U64 m_org_file_pos;
};

class CFlvHeaderParser : 
	public CvoFileHeaderParser
{
public:
	CFlvHeaderParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CFlvHeaderParser();

public:
	//parse file header to get information
	virtual VO_BOOL			ReadFromFile();

protected:
	virtual VO_BOOL			ReadScriptData(VO_U32 dwDataSize);
	virtual VO_BOOL			ReadScriptDataString(VO_PCHAR szString);
	virtual VO_BOOL			ReadScriptDataValue(const VO_PCHAR szString);

public:
	inline VO_BOOL			IsAudioInfoValid() {return (0xFF != m_AudioInfo.btCodecID && m_AudioInfo.nSamplesPerSec && m_AudioInfo.nChannels && m_AudioInfo.wBitsPerSample) ? VO_TRUE : VO_FALSE;}
	inline VO_BOOL			IsVideoInfoValid() {return (0xFF != m_VideoInfo.btCodecID) ? VO_TRUE : VO_FALSE;}

	PFlvAudioInfo			GetAudioInfo() {return &m_AudioInfo;}
	PFlvVideoInfo			GetVideoInfo() {return &m_VideoInfo;}
	VO_U32					GetDuration() {return m_dwDuration;}
	VO_U64					GetFileSize() {return m_ullFileSize;}
	VO_U64					GetMediaDataFilePos() {return m_ullMediaDataFilePos;}
	VO_S32					GetTrackCount();
	VO_VOID				SetThumbNail(VO_BOOL flag){m_bThumbNail = flag;}

	VO_BOOL					IndexIsValid() {return (m_dwIndexEntries > 0) ? VO_TRUE : VO_FALSE;}

	//if llTimeStamp too big, return last entry's packet number
	VO_U64					IndexGetFilePosByTime(VO_S64 llTimeStamp);
	//if dwIndex too big, return -1
	VO_U64					IndexGetFilePosByIndex(VO_U32 dwIndex);
	//bForward: get the next index near the time stamp
	VO_U32					IndexGetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp);
	
	//add by yyang to support GetMediaTimeByFilePos
	VO_S64					IndexGetTimeByFilePos( VO_U64 filepos );

	VO_S64					GetPrevKeyFrameTime(VO_S64 llTimeStamp);
	VO_S64					GetNextKeyFrameTime(VO_S64 llTimeStamp);

	VO_VOID					set_physical_file_size( VO_U64 file_size ){ m_physical_file_size = file_size; }
	//
protected:
	DECLARE_USE_FLV_GLOBAL_VARIABLE

	VO_U8					m_btFileTypeFlags;

	FlvAudioInfo			m_AudioInfo;
	FlvVideoInfo			m_VideoInfo;

	VO_U32					m_dwDuration;				//<MS>
	VO_U64					m_ullFileSize;

	VO_U64					m_ullMediaDataFilePos;

	VO_U32					m_dwIndexEntries;			//entry count
	PFlvIndexEntry			m_pIndexEntries;			//entry content

	VO_BOOL					m_bMetaDataReaded;

	VO_U64					m_physical_file_size;
	VO_U64					m_metadata_end_pos;
	VO_BOOL				m_bThumbNail;

private:
	VO_BOOL read_audio_data( VO_U32 data_size );
	VO_BOOL read_audio_data_aac( VO_U32 data_size );
	VO_BOOL read_audio_data_adpcm( VO_U32 data_size );
	VO_BOOL read_video_data( VO_U32 data_size );
	VO_BOOL read_video_data_sorenson263( VO_U32 data_size );
	VO_BOOL read_video_data_screen_video( VO_U32 data_size );
	VO_BOOL read_video_data_screen_video2( VO_U32 data_size );
	VO_BOOL read_video_data_vp6( VO_U32 data_size );
	VO_BOOL read_video_data_vp6_alpha( VO_U32 data_size );
	VO_BOOL read_video_data_avc( VO_U32 data_size );
	VO_BOOL read_script_data( VO_U32 data_size );

	VO_VOID find_duration_info();
	VO_BOOL find_tag_by_pos( VO_U64 file_pos , VO_U64 find_range , VO_U64& first_tag_pos );
	VO_BOOL get_duration_by_travel( VO_U64 file_pos ); //do not care about the return value of this function
	VO_BOOL test_is_tag(); //it will not change file pos
};

typedef struct tagFlvDataParserSample
{
	VO_BOOL			bVideo;
	VO_U8			btFlag;
	VO_U32			dwLen;
	VO_U32			dwTimeStamp;
	CGFileChunk*	pFileChunk;
	VO_U64			pos;
} FlvDataParserSample, *PFlvDataParserSample;
class CFlvDataParser : 
	public CvoFileDataParser
{
public:
	CFlvDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CFlvDataParser();

public:
	virtual VO_VOID			Init(VO_PTR pParam);
	VO_BOOL					GetFilePosByTime(VO_U64 ullStartFilePos, VO_U32 dwMaxSampleSize, VO_U32 dwMaxTimeStamp, VO_S64 llTimeStamp, VO_U64& ullFilePos);
	VO_BOOL					GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos);	//doncy 0606
	VO_VOID					SetThumbNailFromFR(VO_BOOL flag){m_bGetThumbNail = flag;}

protected:
	virtual VO_BOOL			StepB();

protected:
		VO_BOOL				m_bGetThumbNail;
};


#ifdef _VONAMESPACE
}
#endif
