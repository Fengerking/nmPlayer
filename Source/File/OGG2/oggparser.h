
#ifndef __VO_OGGPARSER_H_

#define __VO_OGGPARSER_H_

#include "oggcommon.h"
#include "CvoFileParser.h"
#include "voMetaData.h"


class ogg_header_parser : public CvoFileHeaderParser
{
public:
	ogg_header_parser( CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp );
	virtual ~ogg_header_parser();

    VO_BOOL	ReadFromFile(){ return ReadFromFile(VO_FALSE); }
	VO_BOOL	ReadFromFile( VO_BOOL is_scan_metadata );
	const track_info * get_track_info(OGG_MEDIA_DETAIL media_type);

	VO_U32	get_length();
	VO_CHAR * get_track_number(){ return m_TrackNumber; }
	VO_CHAR * get_title(){ return m_title; }
	VO_CHAR * get_album(){ return m_album; }
	VO_CHAR * get_artist(){ return m_artist; }
	VO_CHAR * get_genre(){ return m_genre; }
	VO_BOOL get_needloop(){ return m_needloop; }
	PMetaDataImage get_frontcover(){ return m_ptr_cover; }

    VO_VOID set_filesize( VO_U64 filesize ){ m_filesize = filesize; }
    VO_U64 Get_filesize(){ return m_filesize; }

private:
	VO_BOOL generate_track_info( VO_BOOL is_scan_metadata );
	track_info * get_track_info( VO_U32 serial_number );
	VO_BOOL	read_page_header( OggPageHead * ptr_head );
	VO_U32 add_track_info( VO_U32 serial_number , const OggPageHead * ptr_ogg_head , const VO_PBYTE ptr_segments );
	track_info* create_track_info();
	VO_VOID add_track_index( track_info * ptr_track , VO_U64 page_pos , VO_U64 granule_pos );
	VO_VOID parse_comment_field( VO_PBYTE ptr_buffer , VO_S32 size );

	VO_VOID prepare_head();

	VO_VOID remove_track_info();


	VO_BOOL parse_vorbis_metadata(track_info * ptr_track_info);
	VO_U32 parse_vorbis_info(track_info * ptr_track, VO_U32 packets, const OggPageHead * ptr_ogg_head, const VO_PBYTE ptr_segments);
	VO_VOID prepare_vorbis_head(track_info * ptr_track);
	VO_BOOL read_vorbis_header( OggIdentificationHeader * ptr_vorbis_header );
	VO_VOID set2lastseveralpage();
	VO_VOID parser_frontcover( VO_PBYTE ptr_buffer );

	
	VO_BOOL parse_theora_metadata(track_info * ptr_track_info);
	VO_U32 parse_theora_info(track_info * ptr_track, VO_U32 packets, const OggPageHead * ptr_ogg_head, const VO_PBYTE ptr_segments);
	VO_VOID prepare_theora_head(track_info * ptr_track);

private:
	list_T<track_info *>  m_trackinfolist;
//	track_info * m_ptr_trackinfo;
	VO_U64 m_last_granule_pos;

	VO_CHAR m_TrackNumber[1024];
	VO_CHAR m_title[1024];
	VO_CHAR m_album[1024];
	VO_CHAR m_artist[1024];
	VO_CHAR m_genre[1024];
	VO_BOOL m_needloop;
	PMetaDataImage m_ptr_cover;

	VO_U32	m_nMode;
    VO_U64 m_filesize;
};

struct ogg_page_property 
{
	VO_BYTE header_flag;
	VO_U64 granule_pos;
	VO_U32 serial_number;
	VO_U32 sequence_number;
	VO_U32 crc_checksum;
	VO_U8 segments;
	VO_BYTE segments_table[256];
	VO_U32 packets;
	VO_U32 sumsize;
};

struct ogg_packet
{
	VO_U32 serial_number;
	VO_U64 file_pos;
	VO_U32 packet_size;
	VO_BOOL b_is_compelete;
	CGFileChunk * ptr_file_chunk;
	VO_U32 page_packet_number;
	VO_U32 packets;
};

typedef VO_BOOL (*pOnNewPageArrived)( VO_PTR pUser, VO_U32 stream_id , VO_U64 granule_pos );
typedef VO_U64 (*pOnFunc)(VO_PTR pUser, VO_U64 pos);

class ogg_data_parser : public CvoFileDataParser
{
public:
	ogg_data_parser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	~ogg_data_parser();

	virtual VO_VOID Init(VO_PTR pParam);

	inline VO_VOID set_new_page_arrive_callback( VO_PTR ptr_caller , pOnNewPageArrived callback ){ m_new_page_receiver = ptr_caller; m_on_new_page_arrived = callback; }

	VO_BOOL seek_to( VO_U64 file_pos,  VO_U64 llTimeStamp, pOnFunc pFunc, VO_PTR param );

	VO_VOID	Flush();

protected:
	virtual VO_BOOL StepB();
	VO_BOOL read_ogg_page_head();
	VO_BOOL read_packet();

private:

	//for stepb
	ogg_page_property m_current_page_property;
	VO_BOOL m_b_is_in_page;
	VO_U32 m_current_segment_index;
	VO_U32 m_current_packet_number_in_page;
	//

	pOnNewPageArrived m_on_new_page_arrived;
	VO_PTR m_new_page_receiver;
};


#endif
