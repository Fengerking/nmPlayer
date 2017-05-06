#include "ape_reader.h"
#include "fCC.h"
    #define LOG_TAG "ape_reader"
    #include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

ape_reader::ape_reader(VO_FILE_OPERATOR* pFileOp, 
					   VO_MEM_OPERATOR* pMemOp, 
					   VO_LIB_OPERATOR* pLibOP, 
					   VO_SOURCEDRM_CALLBACK* pDrmCB)
:CBaseAudioReader(pFileOp, pMemOp, pLibOP, pDrmCB)
,m_apedata_pos(0)
,m_ptr_headerdata(0)
,m_headerdata_size(0)
,m_SeekTableLen(0)
,m_index(0)
{
}

ape_reader::~ape_reader(void)
{
	if( m_ptr_headerdata )
	{
		delete []m_ptr_headerdata;
		m_ptr_headerdata = 0;
	}

	if( m_ptr_frames )
	{
		delete []m_ptr_frames;
		m_ptr_frames = 0;
	}
}

VO_U32 ape_reader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseAudioReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	if( !analyze_header() )
		return VO_ERR_SOURCE_OPENFAIL;

	NewReadSampleBuffer();

	return VO_ERR_SOURCE_OK;
}

VO_U32 ape_reader::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	if( m_index >= m_info.header.total_frames )
		return VO_ERR_SOURCE_END;

	m_chunk.FLocate( m_ptr_frames[m_index].pos );
	memcpy(m_pBuf, (void*)&m_ptr_frames[m_index].offset, 4);

	VO_U32 readedBytes;
	if (!m_chunk.FRead3( m_pBuf+4, (VO_U32)m_ptr_frames[m_index].size, &readedBytes)) {
		//VOLOGI("Read Error, m_index = %d", m_index);
		return VO_ERR_SOURCE_END;
	}
	//VOLOGI("Readed bytes = 0x%x", readedBytes);

	pSample->Buffer = m_pBuf;
	pSample->Duration = 1;
	pSample->Size = readedBytes;//m_ptr_frames[m_index].size + 4;
	pSample->Time = m_ptr_frames[m_index].timestamp;

	m_index++;

	return 0;
}

VO_U32 ape_reader::SetPos(VO_S64* pPos)
{
	VO_S32 index = 0;

	for( VO_U32 i = 0 ; i < m_info.header.total_frames ; i++ )
	{
		if( m_ptr_frames[i].timestamp > *pPos )
		{
			index = i - 1;
			break;
		}
	}

	if( index < 0 )
		index = 0;

	*pPos = m_ptr_frames[index].timestamp;

	m_index = index;

	return 0;
}

VO_U32 ape_reader::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	if( !m_ptr_headerdata )
	{
		m_ptr_headerdata = new VO_BYTE[ m_headerdata_size ];
		m_chunk.FLocate( m_apedata_pos );
		m_chunk.FRead( m_ptr_headerdata , m_headerdata_size );

	}

	pHeadData->Buffer = m_ptr_headerdata;
	pHeadData->Length = m_headerdata_size;

	return VO_ERR_SOURCE_OK;
}

VO_U32 ape_reader::GetCodecCC(VO_U32* pCC)
{
	*pCC = AudioFlag_APE;
	return VO_ERR_SOURCE_OK;
}

VO_U32 ape_reader::GetBitrate(VO_U32* pdwBitrate)
{
	VO_U64 audiosize;

	if( m_info.descriptor.audiodata_length_low || m_info.descriptor.audiodata_length_high )
	{
		audiosize = m_info.descriptor.audiodata_length_high;
		audiosize = (audiosize << 32) + m_info.descriptor.audiodata_length_low;
	}
	else
	{
		audiosize = m_ptr_frames[ m_info.header.total_frames - 1 ].pos - m_ptr_frames[0].pos + m_ptr_frames[ m_info.header.total_frames - 1 ].size;
	}

	*pdwBitrate = (VO_U32)(audiosize * 8 * 1000. / m_dwDuration);

	return 0;
}

VO_U32 ape_reader::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = m_info.header.channels;
	pAudioFormat->SampleBits = m_info.header.bps;
	pAudioFormat->SampleRate = m_info.header.sample_rate;

	return 0;
}

VO_BOOL ape_reader::analyze_header()
{
	m_chunk.FLocate( 0 );

	memset( &m_info , 0 , sizeof(m_info) );

	m_chunk.FRead( m_info.descriptor.tag , 4 );

	if( m_info.descriptor.tag[0] != 'M' ||
		m_info.descriptor.tag[1] != 'A' ||
		m_info.descriptor.tag[2] != 'C' ||
		m_info.descriptor.tag[3] != ' ' )
		return VO_FALSE;

	m_chunk.FRead( &m_info.descriptor.file_version, 2 );

	if( m_info.descriptor.file_version < 3950 ||
		m_info.descriptor.file_version > 3990 )
		return VO_FALSE;

	if( m_info.descriptor.file_version >= 3980 )
	{
		m_chunk.FSkip( 2 );
		m_chunk.FRead( &m_info.descriptor.descriptor_length , 4 );
		m_chunk.FRead( &m_info.descriptor.header_length , 4 );
		m_chunk.FRead( &m_info.descriptor.seektable_length , 4 );
		m_chunk.FRead( &m_info.descriptor.wavheader_length , 4 );
		m_chunk.FRead( &m_info.descriptor.audiodata_length_low , 4 );
		m_chunk.FRead( &m_info.descriptor.audiodata_length_high , 4 );
		m_chunk.FRead( &m_info.descriptor.wavtail_length , 4 );
		m_chunk.FRead( &m_info.descriptor.md5 , 16 );

		if( m_info.descriptor.descriptor_length > 52 )
			m_chunk.FSkip( m_info.descriptor.descriptor_length - 52 );

		m_chunk.FRead( &m_info.header.compression_type , 2 );
		m_chunk.FRead( &m_info.header.format_flag , 2 );
		m_chunk.FRead( &m_info.header.blocks_per_frame , 4 );
		m_chunk.FRead( &m_info.header.finalframeblocks , 4 );
		m_chunk.FRead( &m_info.header.total_frames , 4 );
		m_chunk.FRead( &m_info.header.bps , 2 );
		m_chunk.FRead( &m_info.header.channels , 2 );
		m_chunk.FRead( &m_info.header.sample_rate , 4 );
	}
	else
	{
		m_info.descriptor.descriptor_length = 0;
		m_info.descriptor.header_length = 32;

		m_chunk.FRead( &m_info.header.compression_type , 2 );
		m_chunk.FRead( &m_info.header.format_flag , 2 );
		m_chunk.FRead( &m_info.header.channels , 2 );
		m_chunk.FRead( &m_info.header.sample_rate , 4 );
		m_chunk.FRead( &m_info.descriptor.wavheader_length , 4 );
		m_chunk.FRead( &m_info.descriptor.wavtail_length , 4 );
		m_chunk.FRead( &m_info.header.total_frames , 4 );
		m_chunk.FRead( &m_info.header.finalframeblocks , 4 );

		if( m_info.header.format_flag & 4 )	//has peak level
		{
			m_chunk.FSkip( 4 );
			m_info.descriptor.header_length += 4;
		}

		if( m_info.header.format_flag & 16 ) //has seek table
		{
			m_chunk.FRead( &m_info.descriptor.seektable_length , 4 );
			m_info.descriptor.header_length += 4;
			m_info.descriptor.seektable_length *= 4;
		}
		else
			m_info.descriptor.seektable_length = m_info.header.total_frames * 4;

		if ( m_info.header.format_flag & 1 )
			m_info.header.bps = 8;
		else if ( m_info.header.format_flag & 8 )
			m_info.header.bps = 24;
		else
			m_info.header.bps = 16;

		if ( m_info.descriptor.file_version >= 3950 )
			m_info.header.blocks_per_frame = 73728 * 4;
		else if ( m_info.descriptor.file_version >= 3900 || ( m_info.descriptor.file_version >= 3800  && m_info.header.compression_type >= 4000))
			m_info.header.blocks_per_frame = 73728;
		else
			m_info.header.blocks_per_frame = 9216;

		if (!(m_info.header.format_flag & 32))
			m_chunk.FSkip( m_info.descriptor.wavheader_length );
	}

	m_ptr_frames = new APE_FRAME[m_info.header.total_frames];

	//VO_U32 waveHeaderSize = (m_info.header.format_flag & MAC_WITH_WAV_HEADER) ? sizeof(WAVE_HEADER) : m_info.descriptor.wavheader_length;
	//m_ptr_frames[0].pos = m_apedata_pos + m_info.descriptor.descriptor_length + m_info.descriptor.header_length + m_info.descriptor.seektable_length + waveHeaderSize;

	m_ptr_frames[0].pos = m_apedata_pos + m_info.descriptor.descriptor_length + m_info.descriptor.header_length + m_info.descriptor.seektable_length + m_info.descriptor.wavheader_length;
	m_ptr_frames[0].blocks = m_info.header.blocks_per_frame;
	m_ptr_frames[0].offset = 0;


	// m_info.descriptor.seektable_length / 4 = Number of SeekTableElements
	//if( m_info.descriptor.seektable_length <= 0 || m_info.descriptor.seektable_length / 4 != m_info.header.total_frames )
	if( m_info.descriptor.seektable_length <= 0 )
		return VO_FALSE;

	VO_U32 firstframe_pos;
	m_chunk.FRead( &firstframe_pos , 4 );

	VO_S64 delta = m_ptr_frames[0].pos - firstframe_pos;

	for( VO_U32 i = 1 ; i < m_info.header.total_frames ; i++  )
	{
		VO_U32 pos;
		m_chunk.FRead( &pos , 4 );
		m_ptr_frames[i].pos = pos + delta;
	}

	for( VO_U32 i = 1 ; i < m_info.header.total_frames ; i++ )
	{
		m_ptr_frames[i].blocks = m_info.header.blocks_per_frame;
		m_ptr_frames[i - 1].size = m_ptr_frames[i].pos - m_ptr_frames[i - 1].pos;
		m_ptr_frames[i].offset = (m_ptr_frames[i].pos - m_ptr_frames[0].pos) & 3;
	}

	//m_ptr_frames[m_info.header.total_frames - 1].size    = m_info.header.finalframeblocks * 4;
	m_ptr_frames[m_info.header.total_frames - 1].size = m_ullFileSize - m_ptr_frames[m_info.header.total_frames - 1].pos + 1;
	m_ptr_frames[m_info.header.total_frames - 1].blocks = m_info.header.finalframeblocks;

	m_dwMaxSampleSize = (VO_U32)m_ptr_frames[0].size;

	VO_U64 timestamp = 0;
	for (VO_U32 i = 0; i < m_info.header.total_frames; i++) {
		if(m_ptr_frames[i].offset){
			m_ptr_frames[i].pos  -= m_ptr_frames[i].offset;
			m_ptr_frames[i].size += m_ptr_frames[i].offset;
		}
		m_ptr_frames[i].size = (m_ptr_frames[i].size + 3) & ~3;
		m_ptr_frames[i].timestamp = timestamp;

		if( m_dwMaxSampleSize < m_ptr_frames[i].size )
			m_dwMaxSampleSize = (VO_U32)m_ptr_frames[i].size;

		timestamp = timestamp + (VO_U64)(m_info.header.blocks_per_frame * 1000. / m_info.header.sample_rate);
		//VOLOGI("frame[%d] = {pos = 0x%x\toffset = 0x%x\tsize = 0x%x\tblocks = 0x%x\ttimestamp = 0x%x\n", i, m_ptr_frames[i].pos, m_ptr_frames[i].offset, m_ptr_frames[i].size, m_ptr_frames[i].blocks, m_ptr_frames[i].timestamp);
	}

	m_headerdata_size = m_info.descriptor.descriptor_length + m_info.descriptor.header_length;
	m_SeekTableLen = m_info.descriptor.seektable_length + m_info.descriptor.wavheader_length;


	m_dwDuration = (VO_U32)(( m_info.header.blocks_per_frame * ( m_info.header.total_frames - 1 ) + m_info.header.finalframeblocks ) * 1000. / m_info.header.sample_rate);

	return VO_TRUE;
}
