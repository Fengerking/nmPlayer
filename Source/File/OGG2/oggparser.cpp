
#include "oggparser.h"
#include "voString.h"
#include "dmxbasicapi.h"
#include "voLog.h"

#define MAX_PAGE_SIZE	64*1024
#define PAGE_HEAD_OFFSET	27

static const VO_BYTE base64_dec_map[128] =
{
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127,  62, 127, 127, 127,  63,  52,  53,
	54,  55,  56,  57,  58,  59,  60,  61, 127, 127,
	127,  64, 127, 127, 127,   0,   1,   2,   3,   4,
	5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
	25, 127, 127, 127, 127, 127, 127,  26,  27,  28,
	29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
	39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
	49,  50,  51, 127, 127, 127, 127, 127
};

VO_BOOL base64_decode(VO_PBYTE pSrc, VO_U32 nSrc, VO_PBYTE pDst, VO_U32& nDst)
{
	VO_U32 x = 0;
	VO_S32 j = 0, n = 0;
	for(VO_U32 i = 0; i < nSrc; i++)
	{
		if((nSrc - i) >= 2 && pSrc[i] == '\r' && pSrc[i + 1] == '\n')
			continue;

		if(pSrc[i] == '\n')
			continue;

		if(pSrc[i] == '=' && ++j > 2)
			return VO_FALSE;

		if(pSrc[i] > 127 || base64_dec_map[pSrc[i]] == 127)
			return VO_FALSE;

		if(base64_dec_map[pSrc[i]] < 64 && j != 0)
			return VO_FALSE;

		n++;
	}

	if(n == 0)
		return VO_FALSE;

	nDst = ((n * 6) + 7) >> 3;
	if(pDst)
	{
		VO_PBYTE p = pDst;
		j = 3;
		n = 0;
		for(VO_U32 i = 0; i < nSrc; i++)
		{
			if(pSrc[i] == '\r' || pSrc[i] == '\n')
				continue;

			j -= (base64_dec_map[pSrc[i]] == 64);
			x = (x << 6) | ( base64_dec_map[pSrc[i]] & 0x3F);

			if(++n == 4)
			{
				n = 0;
				if(j > 0) *p++ = (VO_BYTE)(x >> 16);
				if(j > 1) *p++ = (VO_BYTE)(x >>  8);
				if(j > 2) *p++ = (VO_BYTE)(x      );
			}
		}
	}

	return VO_TRUE;
}


ogg_header_parser::ogg_header_parser( CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp ) : CvoFileHeaderParser( pFileChunk , pMemOp )
//, m_ptr_trackinfo( VO_NULL )
, m_last_granule_pos(0)
, m_needloop( VO_FALSE )
, m_ptr_cover(0)
, m_nMode(0)
, m_filesize(0)
{
	memset( m_TrackNumber , 0 , sizeof(m_title) );
	memset( m_title , 0 , sizeof(m_title) );
	memset( m_album , 0 , sizeof(m_album) );
	memset( m_artist , 0 , sizeof(m_artist) );
	memset( m_genre , 0 , sizeof(m_genre) );
}

ogg_header_parser::~ogg_header_parser()
{
	remove_track_info();

	if( m_ptr_cover )
	{
		if( m_ptr_cover->pBuffer )
			delete []m_ptr_cover->pBuffer;

		delete m_ptr_cover;
	}
}

VO_BOOL	 ogg_header_parser::ReadFromFile( VO_BOOL is_scan_metadata )
{
	VO_BOOL ret = generate_track_info( is_scan_metadata );

	if( ret ){
		prepare_head();
		get_length();
	}

	return ret;
}


VO_BOOL ogg_header_parser::generate_track_info( VO_BOOL is_scan_metadata )
{
	VO_U32	mode = 0;
	m_pFileChunk->FLocate( 0 );

	OggPageHead header;
	while( read_page_header( &header ) )
	{
		VO_PBYTE ptr_segments = (VO_PBYTE)MemAlloc( header.number_page_segments );

		m_pFileChunk->FRead( ptr_segments , header.number_page_segments );

		VO_U32 ret = add_track_info( header.bitstream_serial_number , &header , ptr_segments );

		MemFree( ptr_segments );

		if( VO_ERR_SOURCE_ERRORDATA == ret )
			return VO_FALSE;

		if( is_scan_metadata )
		{
			track_info * ptr_track_info = NULL;
			ptr_track_info = get_track_info(header.bitstream_serial_number);
			switch(ptr_track_info->media_detail)
			{
				case MD_VORBIS:
					if(parse_vorbis_metadata(ptr_track_info)){
						mode |= 0x01;
					}
					break;
				case MD_THEORA:
					if(parse_theora_metadata(ptr_track_info)){
						mode |= 0x02;
					}
					break;
			}

			if(mode & m_nMode == m_nMode){
				break;
			}
		}
		else
		{
			track_info * ptr_track_info = NULL;
			ptr_track_info = get_track_info(header.bitstream_serial_number);
			switch(ptr_track_info->media_detail)
			{
				case MD_VORBIS:
					if(VO_ERR_SOURCE_END == ret){
						mode |= 0x01;
					}
					break;
				case MD_THEORA:
					if(VO_ERR_SOURCE_END == ret){
						mode |= 0x02;
					}
					break;
			}

			if(mode & m_nMode == m_nMode){
				break;
			}


		}
	}

	return VO_TRUE;
}

VO_VOID ogg_header_parser::prepare_head()
{
	track_info * ptr_track = NULL;

	list_T<track_info *>::iterator iter;
	for(iter=m_trackinfolist.begin(); iter!=m_trackinfolist.end(); ++iter)
	{
		ptr_track = (track_info *)(*iter);

		switch(ptr_track->media_detail)
		{
			case MD_VORBIS:
				prepare_vorbis_head(ptr_track);
				break;
			case MD_THEORA:
				prepare_theora_head(ptr_track);
				break;
			default:
				break;
		}
	}	
}

VO_U32	ogg_header_parser::get_length()
{
	OggPageHead header;
    	VO_U64	end = 0;

    	end = m_filesize > MAX_PAGE_SIZE? m_filesize - MAX_PAGE_SIZE: 0;
	if(!m_pFileChunk->FLocate(end)){
		return 0;
	}

	while( read_page_header( &header ) )
	{
		track_info * ptr_track_info = get_track_info(header.bitstream_serial_number);

		if(ptr_track_info){
			ptr_track_info->last_granule_pos = header.granule_position;
		}
	}

	if(!m_pFileChunk->FLocate(0)){
		return 0;
	}

	int num = m_trackinfolist.size();
	while( num > 0 && read_page_header( &header ) )
	{
		track_info * ptr_track_info = get_track_info(header.bitstream_serial_number);

		if(ptr_track_info && header.granule_position != -1 && header.granule_position != 0){
			ptr_track_info->first_granule_pos = header.granule_position;
			num--;
		}
	}

    return 0;
}

track_info * ogg_header_parser::get_track_info( VO_U32 serial_number )
{
	track_info * ptr_track_info = NULL;

	list_T<track_info *>::iterator iter;
	for(iter=m_trackinfolist.begin(); iter!=m_trackinfolist.end(); ++iter)
	{
		ptr_track_info = (track_info *)(*iter);
		
		if( ptr_track_info->serial_number == serial_number )
			break;

	}	


	return ptr_track_info;
}

VO_BOOL	ogg_header_parser::read_page_header( OggPageHead * ptr_head )
{
	if( !ptr_head )
		return VO_FALSE;

	//VO_BYTE header_flag;
	VO_BYTE header[4];
	VO_BOOL ret = m_pFileChunk->FRead( header , 4 );

	if( ret == VO_FALSE )
		return VO_FALSE;


	//find the sync word in the page header.
	VO_U32 sp = 0;
	VO_U32	index = 0;
	VO_BYTE tmpchar;
	do{

		if (header[sp & 3] == 'O' &&
		header[(sp + 1) & 3] == 'g' &&
		header[(sp + 2) & 3] == 'g' && header[(sp + 3) & 3] == 'S')
		break;

		ret = m_pFileChunk->FRead(&tmpchar , 1 );

		if( ret == VO_FALSE )
			return VO_FALSE;

		header[sp++ & 3] = tmpchar;
	}while (index++ < MAX_PAGE_SIZE);

	if (index >= MAX_PAGE_SIZE){
		VOLOGE("Can't find the page!");
		return VO_FALSE;
	}

	ptr_head->capture_pattern = 0x5367674f;

	ret = m_pFileChunk->FRead( &(ptr_head->stream_structure_version) , 1 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_head->header_type_flag) , 1 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_head->granule_position) , 8 );

	if( !ret )
		return VO_FALSE;

	if( ptr_head->granule_position == 0xffffffffffffffffLL )
		ptr_head->granule_position = m_last_granule_pos;

	m_last_granule_pos = ptr_head->granule_position;

	ret = m_pFileChunk->FRead( &(ptr_head->bitstream_serial_number) , 4 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_head->page_sequence_number) , 4 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_head->CRC_checksum) , 4 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_head->number_page_segments) , 1 );

	if( !ret )
		return VO_FALSE;

	return VO_TRUE;
}

VO_U32 ogg_header_parser::add_track_info( VO_U32 serial_number , 
										  const OggPageHead * ptr_ogg_head , 
										  const VO_PBYTE ptr_segments )
{
	VO_U32 nResult = VO_ERR_SOURCE_OK;

	track_info * ptr_track = get_track_info( serial_number );

	if( !ptr_track )
	{
		ptr_track = create_track_info();
		ptr_track->serial_number = serial_number;
	}

	VO_U32 packets = 0;
	VO_U32 sumsize = 0;
	VO_U64 page_pos = m_pFileChunk->FGetFilePos() - ptr_ogg_head->number_page_segments - PAGE_HEAD_OFFSET;

	for( VO_U32 i = 0 ; i < ptr_ogg_head->number_page_segments ; i++ )
	{
		sumsize += ptr_segments[i];

		if( ptr_segments[i] < 0xff )
			packets++;
	}

	VO_U32 skipsize = sumsize;
	
	if( ptr_track->readpackets == 0 )
	{
		VO_BYTE packet_type;
		VO_BYTE  packet_identifier[6];
		m_pFileChunk->FRead( (VO_PTR)&packet_type , 1 );

		skipsize = skipsize - 1;

		if( packet_type == 1 )
		{
			if( skipsize >= 7 )
			{
				m_pFileChunk->FRead( (VO_PTR)packet_identifier , 6 );

				skipsize = skipsize - 6;

				if( MemCompare( (VO_PTR)packet_identifier , (VO_PTR)OGG_HEAD_PACKET_FLAG , 6 ) == 0 )
				{
					ptr_track->media_detail = MD_VORBIS;
					ptr_track->media_type = MT_AUDIO;
					m_nMode |= 0x01;

					if( !read_vorbis_header( &(ptr_track->info) ) )
						return VO_ERR_SOURCE_ERRORDATA;

					skipsize = skipsize - 23;
				}
				else
					return VO_ERR_SOURCE_ERRORDATA; // to stop parse
			
			}
		}
		else if(packet_type == 0x80)
		{
			if( skipsize >= 7 )
			{
				m_pFileChunk->FRead( (VO_PTR)packet_identifier , 6 );

				skipsize = skipsize - 6;

				if( MemCompare( (VO_PTR)packet_identifier , (VO_PTR)THEORA_HEAD_PACKET_FLAG , 6 ) == 0 )
				{
					ptr_track->media_detail = MD_THEORA;
					ptr_track->media_type = MT_VIDEO;
					m_nMode |= 0x02;

/*
					if( !read_vorbis_header( &(ptr_track->info) ) )
						return VO_ERR_SOURCE_ERRORDATA;

					sumsize = sumsize - 23;
*/					
				}
				else
					return VO_ERR_SOURCE_ERRORDATA; // to stop parse

			}

		}
		else{
			return VO_ERR_SOURCE_ERRORDATA; // to stop parse
		}
	}

	switch(ptr_track->media_detail)
	{
		case MD_VORBIS:
			nResult = parse_vorbis_info(ptr_track, packets, ptr_ogg_head, ptr_segments);	
			break;
		case MD_THEORA:
			nResult = parse_theora_info(ptr_track, packets, ptr_ogg_head, ptr_segments);				
			break;
		default:
			break;
	}


	if(VO_ERR_SOURCE_ERRORDATA != nResult){
		add_track_index( ptr_track , page_pos , ptr_ogg_head->granule_position );
	}
	m_pFileChunk->FSkip( skipsize );

	return nResult;
 }

track_info* ogg_header_parser::create_track_info()
{
	track_info * ptr_temp = new track_info;

	MemSet( ptr_temp , 0 , sizeof( track_info ) );

	ptr_temp->ptr_indexlist = new list_T<track_index *>;
	ptr_temp->media_detail = MD_UNKNOWN;
	ptr_temp->media_type = MT_UNKNOWN;

	m_trackinfolist.push_back(ptr_temp);		

	return ptr_temp;
}

VO_VOID ogg_header_parser::add_track_index( track_info * ptr_track , VO_U64 page_pos , VO_U64 granule_pos )
{
	track_index * ptr_index = VO_NULL;

	if( ptr_track->media_detail == MD_VORBIS )
	{
		//VO_U64 timestamp = (VO_U64)(granule_pos * 1000. / ptr_track->info.audio_sample_rate);

		ptr_index = (track_index*)MemAlloc( sizeof( track_index ) );

		ptr_index->page_pos = page_pos;
		ptr_index->granule_pos = granule_pos;
	}

	if( ptr_index )
	{
		ptr_track->ptr_indexlist->push_back(ptr_index);
	}
}

const track_info * ogg_header_parser::get_track_info(OGG_MEDIA_DETAIL media_type)
{
	track_info * ptr_track_info = NULL;

	list_T<track_info *>::iterator iter;
	for(iter=m_trackinfolist.begin(); iter!=m_trackinfolist.end(); ++iter)
	{
		ptr_track_info = (track_info *)(*iter);
		
		if( ptr_track_info->media_detail == media_type )
			break;
	}	

	return ptr_track_info;
}


VO_VOID ogg_header_parser::remove_track_info()
{
	track_info * ptr_track = NULL;
	
	list_T<track_info *>::iterator iter;
	for(iter=m_trackinfolist.begin(); iter!=m_trackinfolist.end(); ++iter)
	{
		ptr_track = (track_info *)(*iter);
	        if( ptr_track->ptr_header_data ){
	            MemFree( ptr_track->ptr_header_data );
		}

		//free the indexlist
		list_T<track_index *>::iterator indexiter;
		for(indexiter=ptr_track->ptr_indexlist->begin(); indexiter!=ptr_track->ptr_indexlist->end(); ++indexiter){			
			MemFree(*indexiter);
		}
		ptr_track->ptr_indexlist->clear();
			
		delete ptr_track->ptr_indexlist;
		delete ptr_track;
	}
	m_trackinfolist.clear();
}

/***************************Vorbis Start***************************************************/
VO_BOOL ogg_header_parser::parse_vorbis_metadata(track_info * ptr_track_info)
{
	VO_BOOL bResult = VO_FALSE;
		
	if( ptr_track_info->media_detail == MD_VORBIS ){
		return VO_FALSE;
	}
	
	if( ptr_track_info->readpackets < 5 ){
		return VO_FALSE;
	}
	
	set2lastseveralpage();
	bResult = VO_TRUE;
	
	return bResult;
}

VO_U32 ogg_header_parser::parse_vorbis_info(track_info * ptr_track, VO_U32 packets, const OggPageHead * ptr_ogg_head, const VO_PBYTE ptr_segments)
{
	VO_U32 nResult = VO_ERR_SOURCE_OK;
	
	if( ptr_track->media_detail == MD_VORBIS && ptr_track->readpackets < 3 )
	{
		VO_U32 countpackets = ptr_track->readpackets;
		for( VO_U32 i = 0 ; i < ptr_ogg_head->number_page_segments ; i++ )
		{
			if( countpackets < 3 )
				ptr_track->headersize += ptr_segments[i];
			else{
				break;			
			}

			if( ptr_segments[i] < 0xff )
				countpackets++;
		}
	}
	
	ptr_track->readpackets += packets;

	if(ptr_track->readpackets >= 3){
		nResult = VO_ERR_SOURCE_END;
	}

	return nResult;
}

VO_VOID ogg_header_parser::prepare_vorbis_head(track_info * ptr_track)
{
	VO_U32 head_got = 0;
	ptr_track->ptr_header_data = (VO_PBYTE)MemAlloc( ptr_track->headersize + sizeof( VORBISFORMAT2 ) );
	ptr_track->headersize = ptr_track->headersize + sizeof( VORBISFORMAT2 );
	VORBISFORMAT2 * ptr_vorbis_head = (VORBISFORMAT2*)ptr_track->ptr_header_data;
	VO_PBYTE ptr_writer = ptr_track->ptr_header_data + sizeof( VORBISFORMAT2 );

	ptr_vorbis_head->BitsPerSample = 16;
	ptr_vorbis_head->Channels = ptr_track->info.audio_channel;
	ptr_vorbis_head->SamplesPerSec = ptr_track->info.audio_sample_rate;
	ptr_vorbis_head->HeaderSize[0] = 0;
	ptr_vorbis_head->HeaderSize[1] = 0;
	ptr_vorbis_head->HeaderSize[2] = 0;

	VO_U32 packet_index = 0;
	track_index * ptr_index = NULL;

	//get the headdata
	list_T<track_index *>::iterator iter;
	for(iter=ptr_track->ptr_indexlist->begin(); iter!=ptr_track->ptr_indexlist->end(); ++iter)
	{
		ptr_index = (track_index*)(*iter);
		//break the while if we get enough headdata.
		if( head_got >= ptr_track->headersize - sizeof( VORBISFORMAT2 ) ) {
			break;
		}
		
		if( ptr_index )
			m_pFileChunk->FLocate( ptr_index->page_pos );

		m_pFileChunk->FSkip( 26 );
	
		VO_BYTE segnum = 0;
	
		m_pFileChunk->FRead( &segnum , 1 );
	
		VO_PBYTE psegmentslen = (VO_PBYTE)MemAlloc(segnum);
	
		m_pFileChunk->FRead( psegmentslen , segnum );
	
		VO_U32 sumsize = 0;
		for( VO_U32 i = 0 ; i < segnum ; i++ )
		{
			if( packet_index < 3 )
			{
				ptr_vorbis_head->HeaderSize[packet_index] += psegmentslen[i];
				sumsize += psegmentslen[i];
			}
	
			if( psegmentslen[i] < 0xff )
				packet_index++;
		}
	
		m_pFileChunk->FRead( ptr_writer , sumsize );
	
		ptr_writer = ptr_writer + sumsize;
		head_got = head_got + sumsize;
						
		MemFree(psegmentslen);
	}	
	parse_comment_field( ptr_track->ptr_header_data + sizeof( VORBISFORMAT2 ) + ptr_vorbis_head->HeaderSize[0] + 7 , ptr_vorbis_head->HeaderSize[1] );
}


VO_BOOL ogg_header_parser::read_vorbis_header( OggIdentificationHeader * ptr_vorbis_header )
{
	if( !ptr_vorbis_header )
		return VO_FALSE;

	VO_BOOL ret = m_pFileChunk->FRead( &(ptr_vorbis_header->vorbis_version) , 4 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_vorbis_header->audio_channel) , 1 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_vorbis_header->audio_sample_rate) , 4 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_vorbis_header->bitrate_maximum) , 4 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_vorbis_header->bitrate_nominal) , 4 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_vorbis_header->bitrate_minimum) , 4 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_vorbis_header->block_size) , 1 );

	if( !ret )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( &(ptr_vorbis_header->framing_flag) , 1 );

	if( !ret )
		return VO_FALSE;

	return VO_TRUE;
}

VO_VOID ogg_header_parser::parse_comment_field( VO_PBYTE ptr_buffer , VO_S32 size )
{
	VO_PBYTE ptr_cur = ptr_buffer;

	if( size <= 4 )
		return;

	VO_U32 * ptr_vendor_len = (VO_U32 *)ptr_cur;
	ptr_cur = ptr_cur + 4;

	if( *ptr_vendor_len )
		ptr_cur = ptr_cur + *ptr_vendor_len;

	VO_U32 * ptr_comment_count = ( VO_U32 * )ptr_cur;
	ptr_cur = ptr_cur + 4;

	for( VO_U32 i = 0; i < *ptr_comment_count ; i++ )
	{
		VO_U32 * ptr_len = ( VO_U32 * )ptr_cur;
		ptr_cur = ptr_cur + 4;

		if( *ptr_len <= 0 )
			return;

		VO_PCHAR ptr_str = (VO_PCHAR)ptr_cur;
		ptr_cur = ptr_cur + *ptr_len;

#ifndef WIN32
		if( strncasecmp( ptr_str , "TRACKNUMBER=" , strlen("TRACKNUMBER=") ) == 0 )
		{
			ptr_str += strlen( "TRACKNUMBER=" );
			VO_U32 len = *ptr_len - strlen( "TRACKNUMBER=" );

			VO_U32 cpylen = len <= sizeof( m_TrackNumber ) - 1 ? len : sizeof( m_TrackNumber ) - 1;

			strncpy( m_TrackNumber , ptr_str , cpylen );
		}
		else if( strncasecmp( ptr_str , "TITLE=" , strlen("TITLE=") ) == 0 )
		{
			ptr_str += strlen( "TITLE=" );
			VO_U32 len = *ptr_len - strlen( "TITLE=" );

			VO_U32 cpylen = len <= sizeof( m_title ) - 1 ? len : sizeof( m_title ) - 1;

			strncpy( m_title , ptr_str , cpylen );
		}
		else if( strncasecmp( ptr_str , "ALBUM=" , strlen("ALBUM=") ) == 0 )
		{
			ptr_str += strlen( "ALBUM=" );
			VO_U32 len = *ptr_len - strlen( "ALBUM=" );

			VO_U32 cpylen = len <= sizeof( m_album ) - 1 ? len : sizeof( m_album ) - 1;

			strncpy( m_album , ptr_str , cpylen );
		}
		else if( strncasecmp( ptr_str , "ARTIST=" , strlen("ARTIST=") ) == 0 )
		{
			ptr_str += strlen( "ARTIST=" );
			VO_U32 len = *ptr_len - strlen( "ARTIST=" );

			VO_U32 cpylen = len <= sizeof( m_artist ) - 1 ? len : sizeof( m_artist ) - 1;

			strncpy( m_artist , ptr_str , cpylen );
		}
		else if( strncasecmp( ptr_str , "GENRE=" , strlen("GENRE=") ) == 0 )
		{
			ptr_str += strlen( "GENRE=" );
			VO_U32 len = *ptr_len - strlen( "GENRE=" );

			VO_U32 cpylen = len <= sizeof( m_genre ) - 1 ? len : sizeof( m_genre ) - 1;

			strncpy( m_genre , ptr_str , cpylen );
		}
		else if( strncasecmp( ptr_str , "ANDROID_LOOP=" , strlen("ANDROID_LOOP=") ) == 0 )
		{
			ptr_str += strlen( "ANDROID_LOOP=" );

			if( strncmp( ptr_str , "true" , strlen("true") ) == 0 )
				m_needloop = VO_TRUE;
			else
				m_needloop = VO_FALSE;
		}
		else if(strncasecmp(ptr_str, "METADATA_BLOCK_PICTURE=" , strlen("METADATA_BLOCK_PICTURE=")) == 0)
		{
			ptr_str += strlen("METADATA_BLOCK_PICTURE=");
			VO_U32 len = *ptr_len - strlen("METADATA_BLOCK_PICTURE=");

			VO_U32 nContent = 0;
			if(base64_decode((VO_PBYTE)ptr_str, len, NULL, nContent) == VO_TRUE)
			{
				VO_PBYTE pContent = new VO_BYTE[nContent];
				base64_decode((VO_PBYTE)ptr_str, len, pContent, nContent);
				parser_frontcover( pContent );
				delete []pContent;
			}
        }
#else
		if( strncmp( ptr_str , "TRACKNUMBER=" , strlen("TRACKNUMBER=") ) == 0 )
		{
			ptr_str += strlen( "TRACKNUMBER=" );
			VO_U32 len = *ptr_len - strlen( "TRACKNUMBER=" );

			VO_U32 cpylen = len <= sizeof( m_TrackNumber ) - 1 ? len : sizeof( m_TrackNumber ) - 1;

			strncpy( m_TrackNumber , ptr_str , cpylen );
		}
        if( strncmp( ptr_str , "TITLE=" , strlen("TITLE=") ) == 0 )
        {
            ptr_str += strlen( "TITLE=" );
            VO_U32 len = *ptr_len - strlen( "TITLE=" );

            VO_U32 cpylen = len <= sizeof( m_title ) - 1 ? len : sizeof( m_title ) - 1;

            strncpy( m_title , ptr_str , cpylen );
        }
        else if( strncmp( ptr_str , "ALBUM=" , strlen("ALBUM=") ) == 0 )
        {
            ptr_str += strlen( "ALBUM=" );
            VO_U32 len = *ptr_len - strlen( "ALBUM=" );

            VO_U32 cpylen = len <= sizeof( m_album ) - 1 ? len : sizeof( m_album ) - 1;

            strncpy( m_album , ptr_str , cpylen );
        }
        else if( strncmp( ptr_str , "ARTIST=" , strlen("ARTIST=") ) == 0 )
        {
            ptr_str += strlen( "ARTIST=" );
            VO_U32 len = *ptr_len - strlen( "ARTIST=" );

            VO_U32 cpylen = len <= sizeof( m_artist ) - 1 ? len : sizeof( m_artist ) - 1;

            strncpy( m_artist , ptr_str , cpylen );
        }
        else if( strncmp( ptr_str , "GENRE=" , strlen("GENRE=") ) == 0 )
        {
            ptr_str += strlen( "GENRE=" );
            VO_U32 len = *ptr_len - strlen( "GENRE=" );

            VO_U32 cpylen = len <= sizeof( m_genre ) - 1 ? len : sizeof( m_genre ) - 1;

            strncpy( m_genre , ptr_str , cpylen );
        }
        else if( strncmp( ptr_str , "ANDROID_LOOP=" , strlen("ANDROID_LOOP=") ) == 0 )
        {
            ptr_str += strlen( "ANDROID_LOOP=" );

            if( strncmp( ptr_str , "true" , strlen("true") ) == 0 )
                m_needloop = VO_TRUE;
            else
                m_needloop = VO_FALSE;
        }
        else if(strncmp(ptr_str, "METADATA_BLOCK_PICTURE=" , strlen("METADATA_BLOCK_PICTURE=")) == 0)
        {
            ptr_str += strlen("METADATA_BLOCK_PICTURE=");
            VO_U32 len = *ptr_len - strlen("METADATA_BLOCK_PICTURE=");

            VO_U32 nContent = 0;
            if(base64_decode((VO_PBYTE)ptr_str, len, NULL, nContent) == VO_TRUE)
            {
                VO_PBYTE pContent = new VO_BYTE[nContent];
                base64_decode((VO_PBYTE)ptr_str, len, pContent, nContent);
                parser_frontcover( pContent );
                delete []pContent;
            }
        }
#endif
	}
}

VO_VOID ogg_header_parser::set2lastseveralpage()
{
    VO_U64 pos = m_pFileChunk->FGetFilePos();

    VO_U64 planpos = m_filesize > 1024 * 1024 ? m_filesize - 1024 * 1024 : 0;

    if( planpos < pos + 1024 * 1024 )
        return;

    m_pFileChunk->FLocate( planpos );

    VO_U32 tester = 0;
    while( m_pFileChunk->FRead( &tester , 4 ) )
    {
        if( tester == 0x5367674f )
        {
            VO_U32 log_pos = (VO_U32)(m_pFileChunk->FGetFilePos());
            m_pFileChunk->FSkip( 22 );
            VO_BYTE segments;
            m_pFileChunk->FRead( &segments , 1 );

            VO_PBYTE ptr_segments = (VO_PBYTE)MemAlloc( segments );

            m_pFileChunk->FRead( ptr_segments , segments );

            VO_U64 jumpsize = 0;

            for( VO_S32 i = 0 ; i < segments ; i++ )
                jumpsize = jumpsize + ptr_segments[i];

            MemFree( ptr_segments );

            m_pFileChunk->FSkip( jumpsize );

            tester = 0;
            m_pFileChunk->FRead( &tester , 4 );

            if( tester == 0x5367674f )
            {
                m_pFileChunk->FLocate( log_pos - 4 );
                return;
            }
            else
                m_pFileChunk->FLocate( log_pos - 3 );

        }
        else
            m_pFileChunk->FBack( 3 );
    }
}


VO_VOID ogg_header_parser::parser_frontcover( VO_PBYTE ptr_buffer )
{
	//MIME type
	VO_S32 len = 0;
	ptr_buffer += 4;
	((VO_PBYTE)&len)[0] = ptr_buffer[3];
	((VO_PBYTE)&len)[1] = ptr_buffer[2];
	((VO_PBYTE)&len)[2] = ptr_buffer[1];
	((VO_PBYTE)&len)[3] = ptr_buffer[0];
	ptr_buffer += 4;

	m_ptr_cover = new MetaDataImage;
	MemSet( m_ptr_cover , 0 , sizeof(MetaDataImage) );

	if( MemCompare(ptr_buffer, (VO_PTR)"image/", 6) != 0 && MemCompare(ptr_buffer, (VO_PTR)"IMAGE/", 6) != 0 )
		return;

	ptr_buffer +=6;

	if((!MemCompare(ptr_buffer, (VO_PTR)"jpg", 3) || !MemCompare(ptr_buffer, (VO_PTR)"JPG", 3)) && !ptr_buffer[3])
	{
		m_ptr_cover->nImageType = VO_METADATA_IT_JPEG;
	}
	else if((!MemCompare(ptr_buffer, (VO_PTR)"jpeg", 4) || !MemCompare(ptr_buffer, (VO_PTR)"JPEG", 4)) && !ptr_buffer[4])
	{
		m_ptr_cover->nImageType = VO_METADATA_IT_JPEG;
	}
	else if((!MemCompare(ptr_buffer, (VO_PTR)"bmp", 3) || !MemCompare(ptr_buffer, (VO_PTR)"BMP", 3)) && !ptr_buffer[3])
	{
		m_ptr_cover->nImageType = VO_METADATA_IT_BMP;
	}
	else if((!MemCompare(ptr_buffer, (VO_PTR)"png", 3) || !MemCompare(ptr_buffer, (VO_PTR)"PNG", 3)) && !ptr_buffer[3])
	{
		m_ptr_cover->nImageType = VO_METADATA_IT_PNG;
	}
	else if((!MemCompare(ptr_buffer, (VO_PTR)"gif", 3) || !MemCompare(ptr_buffer, (VO_PTR)"GIF", 3)) && !ptr_buffer[3])
	{
		m_ptr_cover->nImageType = VO_METADATA_IT_GIF;
	}
	else
		return;

	ptr_buffer += len - 6;

	((VO_PBYTE)&len)[0] = ptr_buffer[0];
	((VO_PBYTE)&len)[1] = ptr_buffer[1];
	((VO_PBYTE)&len)[2] = ptr_buffer[2];
	((VO_PBYTE)&len)[3] = ptr_buffer[3];
	ptr_buffer +=len + 4;
	ptr_buffer += 16;
	((VO_PBYTE)&len)[0] = ptr_buffer[3];
	((VO_PBYTE)&len)[1] = ptr_buffer[2];
	((VO_PBYTE)&len)[2] = ptr_buffer[1];
	((VO_PBYTE)&len)[3] = ptr_buffer[0];
	ptr_buffer += 4;

	m_ptr_cover->pBuffer = new VO_BYTE[ len ];
	MemCopy( m_ptr_cover->pBuffer , ptr_buffer , len );
	m_ptr_cover->dwBufferSize = len;
	return;
}
/*****************************Vorbis End*************************************************/

/*****************************Theora  Start*************************************************/
VO_BOOL ogg_header_parser::parse_theora_metadata(track_info * ptr_track_info)
{
	VO_BOOL bResult = VO_FALSE;

	return bResult;
}

VO_U32 ogg_header_parser::parse_theora_info(track_info * ptr_track, VO_U32 packets, const OggPageHead * ptr_ogg_head, const VO_PBYTE ptr_segments)
{
	VO_U32	nResult = VO_ERR_SOURCE_OK;

	return nResult;
}

VO_VOID ogg_header_parser::prepare_theora_head(track_info * ptr_track)
{

}
/******************************Theora  End************************************************/

ogg_data_parser::ogg_data_parser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp) : CvoFileDataParser( pFileChunk , pMemOp )
, m_b_is_in_page(VO_FALSE)
, m_current_segment_index(0)
, m_current_packet_number_in_page(0)
{
	MemSet( &m_current_page_property , 0 , sizeof( ogg_page_property ) );
}

ogg_data_parser::~ogg_data_parser()
{
}

VO_VOID ogg_data_parser::Init(VO_PTR pParam)
{
	;
}

VO_BOOL ogg_data_parser::StepB()
{
	if( m_b_is_in_page )
	{
		read_packet();
	}
	else
	{
		VO_BOOL b_is_ok = VO_FALSE;

		while( !b_is_ok )
		{
			VO_BOOL ret = read_ogg_page_head();

			if( ret == VO_FALSE )
			{
				m_b_is_in_page = VO_FALSE;
				return VO_FALSE;
			}

			b_is_ok = m_on_new_page_arrived( m_new_page_receiver , m_current_page_property.serial_number , m_current_page_property.granule_pos );

			if( !b_is_ok )
			{
				ret = m_pFileChunk->FSkip( m_current_page_property.sumsize );
				m_b_is_in_page = VO_FALSE;

				if( ret == VO_FALSE )
					return VO_FALSE;
			}
		}

		read_packet();
	}

	return VO_TRUE;
}

VO_BOOL ogg_data_parser::read_ogg_page_head()
{
	m_b_is_in_page = VO_TRUE;
	m_current_segment_index = 0;
	MemSet( (VO_PTR)&m_current_page_property , 0 , sizeof( ogg_page_property ) );

	//VO_BYTE header_flag;
	VO_BYTE header[4];

	VO_BOOL ret = m_pFileChunk->FRead( header , 4 );

	if( ret == VO_FALSE )
		return VO_FALSE;


	//find the sync word in the page header.
	VO_U32 sp = 0;
	VO_U32	index = 0;
	VO_BYTE tmpchar;
	do{

		if (header[sp & 3] == 'O' &&
		header[(sp + 1) & 3] == 'g' &&
		header[(sp + 2) & 3] == 'g' && header[(sp + 3) & 3] == 'S')
		break;

		ret = m_pFileChunk->FRead(&tmpchar , 1 );

		if( ret == VO_FALSE )
			return VO_FALSE;

		header[sp++ & 3] = tmpchar;
	}while (index++ < MAX_PAGE_SIZE);

	if (index >= MAX_PAGE_SIZE){
		VOLOGE("Can't find the page!");
		return VO_FALSE;
	}

	VO_BYTE structure_version;
	ret = m_pFileChunk->FRead( (VO_PTR)(&structure_version) , 1 );

	if( ret == VO_FALSE )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( (VO_PTR)(&m_current_page_property.header_flag) , 1 );

	if( ret == VO_FALSE )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( (VO_PTR)(&m_current_page_property.granule_pos) , 8 );

	if( ret == VO_FALSE )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( (VO_PTR)(&m_current_page_property.serial_number) , 4 );

	if( ret == VO_FALSE )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( (VO_PTR)(&m_current_page_property.sequence_number) , 4 );
	
	if( ret == VO_FALSE )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( (VO_PTR)(&m_current_page_property.crc_checksum) , 4 );
	
	if( ret == VO_FALSE )
		return VO_FALSE;


	ret = m_pFileChunk->FRead( (VO_PTR)(&m_current_page_property.segments) , 1 );

	if( ret == VO_FALSE )
		return VO_FALSE;

	ret = m_pFileChunk->FRead( (VO_PTR)(&m_current_page_property.segments_table) , m_current_page_property.segments );

	if( ret == VO_FALSE )
		return VO_FALSE;

	for( VO_U32 i = 0 ; i < m_current_page_property.segments ; i++ )
	{
		m_current_page_property.sumsize += m_current_page_property.segments_table[i];

		if( m_current_page_property.segments_table[i] < 0xff )
			m_current_page_property.packets++;
	}

	if( m_current_page_property.segments_table[m_current_page_property.segments - 1] == 0xff )
		m_current_page_property.packets++;

	if( m_current_page_property.header_flag & 0x01 )
	{
		m_current_packet_number_in_page = -1;
		m_current_page_property.packets--;
	}

	return VO_TRUE;
}


VO_BOOL ogg_data_parser::read_packet()
{
	VO_U32 packetsize = 0;
	VO_BOOL b_is_compelete_packet;

	while( m_current_page_property.segments_table[ m_current_segment_index ] == 0xff 
		&& m_current_segment_index < m_current_page_property.segments )
	{
		packetsize = packetsize + 0xff;
		m_current_segment_index++;
	}

	if( m_current_segment_index < m_current_page_property.segments )
	{
		packetsize = packetsize + m_current_page_property.segments_table[ m_current_segment_index ];
		m_current_segment_index++;
		b_is_compelete_packet = VO_TRUE;
	}
	else
		b_is_compelete_packet = VO_FALSE;


	ogg_packet packet;
	packet.b_is_compelete = b_is_compelete_packet;
	packet.file_pos = m_pFileChunk->FGetFilePos();
	packet.packet_size = packetsize;
	packet.ptr_file_chunk = m_pFileChunk;
	packet.serial_number = m_current_page_property.serial_number;
	packet.page_packet_number = m_current_packet_number_in_page;
	packet.packets = m_current_page_property.packets;

	if( CBRT_FALSE == m_fODCallback( m_pODUser , m_pODUserData , (VO_PTR)&packet ) )
	{
		m_pFileChunk->FLocate( packet.file_pos + packet.packet_size );
	}

	m_current_packet_number_in_page++;

	if( m_current_segment_index >= m_current_page_property.segments )
	{
		m_b_is_in_page = VO_FALSE;
		m_current_segment_index = 0;
		m_current_packet_number_in_page = 0;
	}

	return VO_TRUE;
}

VO_BOOL ogg_data_parser::seek_to(VO_U64 file_pos,  VO_U64 llTimeStamp, pOnFunc pFunc, VO_PTR param)
{
	VO_U64	time = 0;
	VO_U64	pos = file_pos;
	VO_U64	prepos = 0;
	m_pFileChunk->FLocate( file_pos );

	//seek to the position precisly
	while(read_ogg_page_head())
	{
		time = pFunc(param, m_current_page_property.granule_pos);

		if(time > llTimeStamp)
		{
			if(!prepos){
				pos = (pos >MAX_PAGE_SIZE) ? (pos - MAX_PAGE_SIZE) : 0;
				m_pFileChunk->FLocate( pos );
			}else{
				break;
			}
		}
		else if(time < llTimeStamp){
			prepos = m_pFileChunk->FGetFilePos() - PAGE_HEAD_OFFSET - m_current_page_property.segments;
			m_pFileChunk->FSkip(m_current_page_property.sumsize);
		}
		else if(time == llTimeStamp){
			prepos = m_pFileChunk->FGetFilePos() - PAGE_HEAD_OFFSET - m_current_page_property.segments;
			break;
		}
	}

	m_pFileChunk->FLocate( prepos );

	if( !read_ogg_page_head() )
		return VO_FALSE;

	if( m_current_page_property.header_flag & 0x01 )
	{
		VO_U32 seek_size = 0;

		while( m_current_page_property.segments_table[ m_current_segment_index ] == 0xff 
			&& m_current_segment_index < m_current_page_property.segments )
		{
			seek_size += 0xff;
			m_current_segment_index++;
		}

		if( m_current_segment_index < m_current_page_property.segments )
		{
			seek_size += m_current_page_property.segments_table[ m_current_segment_index ];
			m_current_segment_index++;
			m_b_is_in_page = VO_TRUE;
		}
		else
		{
			m_b_is_in_page = VO_FALSE;
		}

		m_pFileChunk->FSkip( seek_size );
	}

	return VO_TRUE;
}

VO_VOID	ogg_data_parser::Flush()
{
	m_b_is_in_page = VO_FALSE;
	m_current_segment_index = 0;
	m_current_packet_number_in_page = 0;
}
