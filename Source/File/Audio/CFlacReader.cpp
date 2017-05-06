	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CFlacReader.cpp

Contains:	CFlacReader class file

Written by:	East

Change History (most recent first):
2006-12-12		East			Create file

*******************************************************************************/
#include "CFlacReader.h"
#include "fCC.h"

//#define LOG_TAG	"CFlacReader"
//#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#if defined(WIN32) || defined(WINCE)
	#define CaseInsensitiveCompare		_strnicmp
#else
	#define CaseInsensitiveCompare		strncasecmp
#endif


CFlacReader::CFlacReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseAudioReader(pFileOp, pMemOp, pLibOP, pDrmCB)
	, m_dwSamplesPerSec(44100)
	, m_wChannel(2)
	, m_wBitsPerSample(16)
	, m_llSampleCounts(0)
	, m_wFixedBlockSize(0)
	, m_magicByte1(0)
	, m_magicByte2(0)
	, m_framesPerSecond(0)
	, m_dwSeekPoints(0)
	, m_pSeekPoints(VO_NULL)
	, m_pLastSeekPoint(VO_NULL)
	, m_pCurrSeekPoint(VO_NULL)
	, m_dwCurrPosInSeekPoint(0)
	, m_pImage(VO_NULL)
{
	MemSet(m_btStreamInfo, 0, FLAC_STREAM_INFO_BLOCK_SIZE);
	MemSet(m_title , 0 , sizeof(m_title) );
	MemSet(m_album , 0 , sizeof(m_album) );
	MemSet(m_discNumber , 0 , sizeof(m_discNumber) );
	MemSet(m_trackNumber , 0 , sizeof(m_trackNumber) );
	MemSet(m_artist , 0 , sizeof(m_artist) );
	MemSet(m_composer , 0 , sizeof(m_composer) );
	MemSet(m_performer , 0 , sizeof(m_performer) );
	MemSet(m_genre , 0 , sizeof(m_genre) );
	MemSet(m_date , 0 , sizeof(m_date) );
	MemSet(m_year , 0 , sizeof(m_year) );
	m_pImage = (PMetaDataImage)NEW_BUFFER(sizeof(MetaDataImage));
	MemSet(m_pImage, 0, sizeof(MetaDataImage));
}

CFlacReader::~CFlacReader()
{
	SAFE_MEM_FREE(m_pImage->pBuffer);
	SAFE_MEM_FREE(m_pImage);
	ReleaseSeekPoints();
}

VO_U32 CFlacReader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseAudioReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	if(!ParseFileHeader())
		return VO_ERR_SOURCE_OPENFAIL;

	m_ullFileHeadSize = (VO_U32)(m_chunk.FGetFilePos());
	m_dwDuration = (VO_U32)(m_llSampleCounts * 1000 / m_dwSamplesPerSec);

	if(m_pSeekPoints)
	{
		PVOFlacSeekPoint pCurr = m_pSeekPoints;
		PVOFlacSeekPoint pNext = VO_NULL;
		while(pCurr)
		{
			pNext = pCurr->next;
			if(pNext)
			{
				pCurr->duration = pNext->time_stamp - pCurr->time_stamp;
				pCurr->length = pNext->file_pos - pCurr->file_pos;
				pCurr->file_pos += (VO_U32)m_ullFileHeadSize;
			}
			else
			{
				pCurr->duration = (VO_U32)(m_llSampleCounts * 1000 / m_dwSamplesPerSec - pCurr->time_stamp);
				pCurr->file_pos += (VO_U32)m_ullFileHeadSize;
				pCurr->length = (VO_U32)(m_ullFileSize - pCurr->file_pos);
			}
			pCurr = pNext;
		}
	}
	else
	{
		//some file has no seek points, so we generate a seek pointer for them!!
		m_pSeekPoints = NEW_OBJ(VOFlacSeekPoint);
		m_pSeekPoints->time_stamp = 0;
		m_pSeekPoints->file_pos = (VO_U32)m_ullFileHeadSize;

		m_pSeekPoints->next = VO_NULL;
		m_pSeekPoints->duration = (VO_U32)(m_llSampleCounts * 1000 / m_dwSamplesPerSec);
		m_pSeekPoints->length = (VO_U32)(m_ullFileSize - m_ullFileHeadSize);
	}

	// since file handle will be used by both FileChunk and FileXXX API, so we need set share file handle.
	m_chunk.SetShareFileHandle(VO_TRUE);
	if(-1 == FileSeek(m_hFile, m_ullFileHeadSize, VO_FILE_BEGIN))
		return VO_ERR_SOURCE_OPENFAIL;

	VO_U32 uAvgSampleSize = 0;
	if (m_dwDuration)///<some audio sample duration is too small,so we need give at least 200ms data once.
	{
		uAvgSampleSize =  (m_ullFileSize - m_ullFileHeadSize)*20/m_dwDuration;
	}
	if (m_dwMaxSampleSize < uAvgSampleSize)
	{
		m_dwMaxSampleSize = uAvgSampleSize;
	}

	m_pBuf = NEW_BUFFER(m_dwMaxSampleSize);
	m_pCurrSeekPoint = m_pSeekPoints;
	m_dwCurrPosInSeekPoint = 0;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlacReader::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_S32 nRes = FileRead(m_hFile, m_pBuf, m_dwMaxSampleSize);
	if(nRes <= 0)
		return (-2 == nRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;

	pSample->Buffer = m_pBuf;
	pSample->Size = nRes;
	pSample->Time = GetTimeStampByCurPos();
	pSample->Duration = 1;

	m_dwCurrPosInSeekPoint += pSample->Size;
	if(m_dwCurrPosInSeekPoint > m_pCurrSeekPoint->length)
	{
		m_dwCurrPosInSeekPoint -= m_pCurrSeekPoint->length;
		m_pCurrSeekPoint = m_pCurrSeekPoint->next;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlacReader::SetPos(VO_S64* pPos)
{
	PVOFlacSeekPoint pCur = m_pSeekPoints;
	PVOFlacSeekPoint pFound = VO_NULL;

	while(pCur)
	{
		if(pCur->time_stamp > *pPos)
			break;

		pFound = pCur;
		pCur = pCur->next;
	}

	if(!pFound || pFound->time_stamp + pFound->duration < *pPos)
		return VO_ERR_SOURCE_END;

	m_pCurrSeekPoint = pFound;

#if 1
	VO_U32 uDelDuration = (*pPos - m_pCurrSeekPoint->time_stamp);
	if (!m_dwDuration)
	{
		return VO_ERR_SOURCE_SEEKFAIL;
	}
	VO_U64 uDelPosition = (m_ullFileSize - m_ullFileHeadSize) * uDelDuration / m_dwDuration;

	if(!m_chunk.FLocate(m_pCurrSeekPoint->file_pos + uDelPosition))
		return VO_ERR_SOURCE_END;

	if(!FindFrameHeaderInFile())
		return VO_ERR_SOURCE_END;

	m_dwCurrPosInSeekPoint = (VO_U32)(m_chunk.FGetFilePos() - m_pCurrSeekPoint->file_pos - 4);

#else
	VO_U32 dwSkipFrames = 0;
	if(m_wFixedBlockSize) {
		dwSkipFrames = (VO_U32)((*pPos - m_pCurrSeekPoint->time_stamp) / 1000);
		if ((*pPos - m_pCurrSeekPoint->time_stamp) % 1000)
			dwSkipFrames++;
		dwSkipFrames *= m_dwSamplesPerSec;
		if (dwSkipFrames % m_wFixedBlockSize)
			dwSkipFrames = dwSkipFrames / m_wFixedBlockSize + 1;
		else
			dwSkipFrames = dwSkipFrames / m_wFixedBlockSize;

		//dwSkipFrames = ((*pPos - m_pCurrSeekPoint->time_stamp) * m_dwSamplesPerSec / m_wFixedBlockSize) / 1000;
	}

	if(dwSkipFrames > 0)
	{
		if(!m_chunk.FLocate(m_pCurrSeekPoint->file_pos))
			return VO_ERR_SOURCE_END;

		for(VO_U32 i = 0; i < dwSkipFrames; i++)
		{
			if(!FindFrameHeaderInFile())
				return VO_ERR_SOURCE_END;
		}

		m_dwCurrPosInSeekPoint = (VO_U32)(m_chunk.FGetFilePos() - m_pCurrSeekPoint->file_pos - 4);
	}
	else
		m_dwCurrPosInSeekPoint = 0;
#endif

	VO_S64 nRes = FileSeek(m_hFile, m_pCurrSeekPoint->file_pos + m_dwCurrPosInSeekPoint, VO_FILE_BEGIN);
	if(nRes < 0)
		return (-2 == nRes) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;

	* pPos = GetTimeStampByCurPos();
	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlacReader::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Buffer = m_btStreamInfo;
	pHeadData->Length = FLAC_STREAM_INFO_BLOCK_SIZE;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlacReader::GetCodecCC(VO_U32* pCC)
{
	*pCC = AudioFlag_FLAC;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlacReader::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = m_wChannel;
	pAudioFormat->SampleRate = m_dwSamplesPerSec;
	pAudioFormat->SampleBits = m_wBitsPerSample;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlacReader::GetBitrate(VO_U32* pdwBitrate)
{
	//if( m_llSampleCounts )
	//	*pdwBitrate = (m_ullFileSize - m_ullFileHeadSize) * m_dwSamplesPerSec / m_llSampleCounts;
	*pdwBitrate = (VO_U32)((m_ullFileSize - m_ullFileHeadSize) * 8000 / m_dwDuration);

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CFlacReader::ParseFileHeader()
{
	VO_U32 fcc;
	read_fcc(fcc);
	if(FOURCC_fLaC != fcc)
		return VO_FALSE;

	VO_BYTE btBlockType = 0;
	VO_U32 dwBlockSize = 0;
	use_big_endian_read
	while(!(btBlockType & 0x80))	//not reach last meta data block!!
	{
		read_byte(btBlockType);
		read_3byte_dword(dwBlockSize);

		switch(btBlockType & 0x7F)
		{
		case FLAC_BLOCKTYPE_STREAMINFO:
			{
				if(!ParseBlock_StreamInfo(dwBlockSize))
					return VO_FALSE;
			}
			break;

		case FLAC_BLOCKTYPE_PADDING:
			{
				if(!ParseBlock_Padding(dwBlockSize))
					return VO_FALSE;
			}
			break;

		case FLAC_BLOCKTYPE_APPLICATION:
			{
				if(!ParseBlock_Application(dwBlockSize))
					return VO_FALSE;
			}
			break;

		case FLAC_BLOCKTYPE_SEEKTABLE:
			{
				if(!ParseBlock_SeekTable(dwBlockSize))
					return VO_FALSE;
			}
			break;

		case FLAC_BLOCKTYPE_VORBISCOMMENT:
			{
				if(!ParseBlock_VorbisComment(dwBlockSize))
					return VO_FALSE;
			}
			break;

		case FLAC_BLOCKTYPE_CUESHEET:
			{
				if(!ParseBlock_Cuesheet(dwBlockSize))
					return VO_FALSE;
			}
			break;

		case FLAC_BLOCKTYPE_PICTURE:
			{
				if(!ParseBlock_Picture(dwBlockSize))
					return VO_FALSE;
			}
			break;

		case FLAC_BLOCKTYPE_INVALID:
			return VO_FALSE;

		default:
			{
				if(!ParseBlock_Other(dwBlockSize))
					return VO_FALSE;
			}
			break;
		}
	}

	return VO_TRUE;
}

VO_BOOL CFlacReader::ParseBlock_StreamInfo(VO_U32 dwBlockSize)
{
	if(FLAC_STREAM_INFO_BLOCK_SIZE != dwBlockSize)
		return VO_FALSE;

	read_pointer(m_btStreamInfo, FLAC_STREAM_INFO_BLOCK_SIZE);

	/*	<16>  The minimum block size (in samples) used in the stream.  
	<16>  The maximum block size (in samples) used in the stream. (Minimum blocksize == maximum blocksize) implies a fixed-blocksize stream.  
	<24>  The minimum frame size (in bytes) used in the stream. May be 0 to imply the value is not known.  
	<24>  The maximum frame size (in bytes) used in the stream. May be 0 to imply the value is not known.  
	<20>  Sample rate in Hz. Though 20 bits are available, the maximum sample rate is limited by the structure of frame headers to 655350Hz. Also, a value of 0 is invalid.  
	<3>  (number of channels)-1. FLAC supports from 1 to 8 channels  
	<5>  (bits per sample)-1. FLAC supports from 4 to 32 bits per sample. Currently the reference encoder and decoders only support up to 24 bits per sample.  
	<36>  Total samples in stream. 'Samples' means inter-channel sample, i.e. one second of 44.1Khz audio will have 44100 samples regardless of the number of channels. A value of zero here means the number of total samples is unknown.  
	<128>  MD5 signature of the unencoded audio data. This allows the decoder to determine if an error exists in the audio data even when the error does not result in an invalid bitstream.  */
	VO_U16 wMinBlockSize = 0, wMaxBlockSize = 0;
	wMinBlockSize = VO_U16(m_btStreamInfo[0]) << 8 | m_btStreamInfo[1];
	wMaxBlockSize = VO_U16(m_btStreamInfo[2]) << 8 | m_btStreamInfo[3];
	if(wMinBlockSize == wMaxBlockSize) {
		m_wFixedBlockSize = wMinBlockSize;
	} else {
		m_wFixedBlockSize = (wMinBlockSize + wMaxBlockSize) / 2;
	}

	m_dwMinFrameSize = VO_U32(m_btStreamInfo[4]) << 16 | VO_U32(m_btStreamInfo[5]) << 8 | m_btStreamInfo[6];
	m_dwMaxFrameSize = VO_U32(m_btStreamInfo[7]) << 16 | VO_U32(m_btStreamInfo[8]) << 8 | m_btStreamInfo[9];
	
	m_dwSamplesPerSec = (VO_U32(m_btStreamInfo[10]) << 12) | (VO_U16(m_btStreamInfo[11]) << 4) | (m_btStreamInfo[12] >> 4);
	m_wChannel = ((m_btStreamInfo[12] & 0xE) >> 1) + 1;
	m_wBitsPerSample = ((m_btStreamInfo[12] & 1) << 4 | (m_btStreamInfo[13]) >> 4) + 1;
	m_llSampleCounts = VO_S64(m_btStreamInfo[13] & 0xF) << 32 | VO_U32(m_btStreamInfo[14]) << 24 | VO_U32(m_btStreamInfo[15]) << 16 | VO_U16(m_btStreamInfo[16]) << 8 | m_btStreamInfo[17];

	m_magicByte1 = 0;
	switch (m_wFixedBlockSize) {
	case 576:
		m_magicByte1 = 2;
		break;
	case 1152:
		m_magicByte1 = 3;
		break;
	case 2304:
		m_magicByte1 = 4;
		break;
	case 4608:
		m_magicByte1 = 5;
		break;
	case 256:
		m_magicByte1 = 8;
		break;
	case 512:
		m_magicByte1 = 9;
		break;
	case 1024:
		m_magicByte1 = 0xA;
		break;
	case 2048:
		m_magicByte1 = 0xB;
		break; 
	case 4096:
		m_magicByte1 = 0xC;
		break;
	case 8192:
		m_magicByte1 = 0xD;
		break; 
	case 16384:
		m_magicByte1 = 0xE;
		break;
	case 32768:
		m_magicByte1 = 0xF;
		break;
	default:
		break;
	}
	m_magicByte1 <<= 4;
	switch (m_dwSamplesPerSec) {
	case 88200:
		m_magicByte1 |= 1;
		break;
	case 176400:
		m_magicByte1 |= 2;
		break;
	case 192000:
		m_magicByte1 |= 3;
		break;
	case 8000:
		m_magicByte1 |= 4;
		break;
	case 16000:
		m_magicByte1 |= 5;
		break;
	case 22050:
		m_magicByte1 |= 6;
		break;
	case 24000:
		m_magicByte1 |= 7;
		break;
	case 32000:
		m_magicByte1 |= 8;
		break; 
	case 44100:
		m_magicByte1 |= 9;
		break;
	case 48000:
		m_magicByte1 |= 0xA;
		break; 
	case 96000:
		m_magicByte1 |= 0xB;
		break;
	default:
		break;
	}

	m_magicByte2 = 0;
	if (m_wChannel != 2) 
		m_magicByte2 = (VO_BYTE)(m_wChannel-1) << 4;
	switch(m_wBitsPerSample) {
	case 8:
		m_magicByte2 |= 2;
		break;
	case 12:
		m_magicByte2 |= 4;
		break;
	case 16:
		m_magicByte2 |= 8;
		break;
	case 20:
		m_magicByte2 |= 0xA;
		break;
	case 24:
		m_magicByte2 |= 0xC;
		break;
	default:
		break;
	}
	
	if (m_dwSamplesPerSec % m_wFixedBlockSize)
		m_framesPerSecond = m_dwSamplesPerSec / m_wFixedBlockSize + 1;
	else
		m_framesPerSecond = m_dwSamplesPerSec / m_wFixedBlockSize;

	return VO_TRUE;
}

VO_BOOL CFlacReader::ParseBlock_Padding(VO_U32 dwBlockSize)
{
	skip(dwBlockSize);

	return VO_TRUE;
}

VO_BOOL CFlacReader::ParseBlock_Application(VO_U32 dwBlockSize)
{
	skip(dwBlockSize);

	return VO_TRUE;
}

VO_BOOL CFlacReader::ParseBlock_SeekTable(VO_U32 dwBlockSize)
{
	ReleaseSeekPoints();

	VO_U32 dwSeekPoints = dwBlockSize / 18;
	if(dwSeekPoints > 0)
	{
		use_big_endian_read

		VO_U32 i = 0;
		VO_S64 llSampleNumber = 0, llOffsets = 0;
		for(; i < dwSeekPoints; i++)
		{
			read_qword(llSampleNumber);
			read_qword(llOffsets);
//			read_word(wSamples);
			skip(2);
			if(llSampleNumber >= m_llSampleCounts)
			{
				skip((dwSeekPoints - i - 1) * 18);
				break;
			}

			if(!AddSeekPoint((VO_U32)(llSampleNumber * 1000 / m_dwSamplesPerSec), (VO_U32)llOffsets))
				return VO_FALSE;
		}
	}

	return VO_TRUE;
}

VO_BOOL CFlacReader::ParseBlock_VorbisComment(VO_U32 dwBlockSize)
{
	//skip(dwBlockSize);
	char * ptr_buffer = new char[dwBlockSize+1];
	memset( ptr_buffer , 0 , dwBlockSize + 1 );

	VO_S32 ret = m_chunk.FRead(ptr_buffer , dwBlockSize);
	//VO_S32 ret = FileRead( m_hFile , ptr_buffer , dwBlockSize );

	if(ret <= 0)
		return VO_FALSE;

	VO_PBYTE ptr_cur = (VO_PBYTE)ptr_buffer;
	VO_S32 size = dwBlockSize;

	if( size <= 4 )
		return VO_FALSE;

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
			return VO_FALSE;

		VO_PCHAR ptr_str = (VO_PCHAR)ptr_cur;
		ptr_cur = ptr_cur + *ptr_len;

		if( CaseInsensitiveCompare(ptr_str , "TITLE=" , strlen("TITLE=")) == 0 )
		{
			ptr_str += strlen( "TITLE=" );
			VO_U32 len = *ptr_len - strlen( "TITLE=" );

			VO_U32 cpylen = len <= sizeof( m_title ) - 1 ? len : sizeof( m_title ) - 1;

			strncpy( m_title , ptr_str , cpylen );
		}
		else if( CaseInsensitiveCompare(ptr_str , "ALBUM=" , strlen("ALBUM=")) == 0 )
		{
			ptr_str += strlen( "ALBUM=" );
			VO_U32 len = *ptr_len - strlen( "ALBUM=" );

			VO_U32 cpylen = len <= sizeof( m_album ) - 1 ? len : sizeof( m_album ) - 1;

			strncpy( m_album , ptr_str , cpylen );
		}
		else if( CaseInsensitiveCompare(ptr_str , "DISCNUMBER=" , strlen("DISCNUMBER=")) == 0 )
		{
			ptr_str += strlen( "DISCNUMBER=" );
			VO_U32 len = *ptr_len - strlen( "DISCNUMBER=" );

			VO_U32 cpylen = len <= sizeof( m_discNumber ) - 1 ? len : sizeof( m_discNumber ) - 1;

			strncpy( m_discNumber , ptr_str , cpylen );
		}
		else if( CaseInsensitiveCompare(ptr_str , "TRACKNUMBER=" , strlen("TRACKNUMBER=")) == 0 )
		{
			ptr_str += strlen( "TRACKNUMBER=" );
			VO_U32 len = *ptr_len - strlen( "TRACKNUMBER=" );

			VO_U32 cpylen = len <= sizeof( m_trackNumber ) - 1 ? len : sizeof( m_trackNumber ) - 1;

			strncpy( m_trackNumber , ptr_str , cpylen );
		}
		else if( CaseInsensitiveCompare(ptr_str , "ARTIST=" , strlen("ARTIST=")) == 0 )
		{
			ptr_str += strlen( "ARTIST=" );
			VO_U32 len = *ptr_len - strlen( "ARTIST=" );

			VO_U32 cpylen = len <= sizeof( m_artist ) - 1 ? len : sizeof( m_artist ) - 1;

			strncpy( m_artist , ptr_str , cpylen );
		}
		else if( CaseInsensitiveCompare(ptr_str , "PERFORMER=" , strlen("PERFORMER=")) == 0 )
		{
			ptr_str += strlen( "PERFORMER=" );
			VO_U32 len = *ptr_len - strlen( "PERFORMER=" );

			VO_U32 cpylen = len <= sizeof( m_performer ) - 1 ? len : sizeof( m_performer ) - 1;

			strncpy( m_performer , ptr_str , cpylen );
		}
		else if( CaseInsensitiveCompare(ptr_str , "COMPOSER=" , strlen("COMPOSER=")) == 0 )
		{
			ptr_str += strlen( "COMPOSER=" );
			VO_U32 len = *ptr_len - strlen( "COMPOSER=" );

			VO_U32 cpylen = len <= sizeof( m_composer ) - 1 ? len : sizeof( m_composer ) - 1;

			strncpy( m_composer , ptr_str , cpylen );
		}
		else if( CaseInsensitiveCompare(ptr_str , "GENRE=" , strlen("GENRE=")) == 0 )
		{
			ptr_str += strlen( "GENRE=" );
			VO_U32 len = *ptr_len - strlen( "GENRE=" );

			VO_U32 cpylen = len <= sizeof( m_genre ) - 1 ? len : sizeof( m_genre ) - 1;

			strncpy( m_genre , ptr_str , cpylen );
		}
		else if( CaseInsensitiveCompare(ptr_str , "DATE=" , strlen("DATE=")) == 0 )
		{
			ptr_str += strlen( "DATE=" );
			VO_U32 len = *ptr_len - strlen( "DATE=" );

			VO_U32 cpylen = len <= sizeof( m_date ) - 1 ? len : sizeof( m_date ) - 1;

			strncpy( m_date , ptr_str , cpylen );
		}
		else if( CaseInsensitiveCompare(ptr_str , "YEAR=" , strlen("YEAR=")) == 0 )
		{
			ptr_str += strlen( "YEAR=" );
			VO_U32 len = *ptr_len - strlen( "YEAR=" );

			VO_U32 cpylen = len <= sizeof( m_year ) - 1 ? len : sizeof( m_year ) - 1;

			strncpy( m_year , ptr_str , cpylen );
		}
		else if( CaseInsensitiveCompare(ptr_str , "ANDROID_LOOP=" , strlen("ANDROID_LOOP=")) == 0 )
		{
			/*ptr_str += strlen( "ANDROID_LOOP=" );

			if( strncmp( ptr_str , "true" , strlen("true") ) == 0 )
				m_needloop = VO_TRUE;
			else
				m_needloop = VO_FALSE;*/
		}
		else if( CaseInsensitiveCompare(ptr_str, "METADATA_BLOCK_PICTURE=" , strlen("METADATA_BLOCK_PICTURE=")) == 0 )
		{
			/*ptr_str += strlen("METADATA_BLOCK_PICTURE=");
			VO_U32 len = *ptr_len - strlen("METADATA_BLOCK_PICTURE=");

			VO_U32 nContent = 0;
			if(base64_decode((VO_PBYTE)ptr_str, len, NULL, nContent) == VO_TRUE)
			{
				VO_PBYTE pContent = new VO_BYTE[nContent];
				base64_decode((VO_PBYTE)ptr_str, len, pContent, nContent);
				parser_frontcover( pContent );
				delete []pContent;
			}*/
		}
	}

	return VO_TRUE;
}

VO_BOOL CFlacReader::ParseBlock_Cuesheet(VO_U32 dwBlockSize)
{
	skip(dwBlockSize);

	return VO_TRUE;
}

VO_BOOL CFlacReader::ParseBlock_Picture(VO_U32 dwBlockSize)
{
	VO_S32 nRes;
	VO_U32 size;
	VO_U32 picHeaderSize = 32;

	use_big_endian_read

	skip(4);	// skip picture type

	read_dword(size);
	picHeaderSize += size;
	if (picHeaderSize >= dwBlockSize)
		return VO_FALSE;
	
	VO_PBYTE pMimeType = NEW_BUFFER(size);
	//pMimeType[size] = 0;
	nRes = m_chunk.FRead(pMimeType, size);
	if(!nRes) {
		SAFE_MEM_FREE(pMimeType);
		return VO_FALSE;
	}

	if(MemCompare(pMimeType, (VO_PTR)"image/", 6) && MemCompare(pMimeType, (VO_PTR)"IMAGE/", 6)) {
		SAFE_MEM_FREE(pMimeType);
		return VO_FALSE;
	}

	VO_PBYTE p = pMimeType + 6;
	VO_METADATA_IMAGETYPE imageType;
	if((!MemCompare(p, (VO_PTR)"jpg", 3) || !MemCompare(p, (VO_PTR)"JPG", 3)))
	{
		imageType = VO_METADATA_IT_JPEG;
	}
	else if((!MemCompare(p, (VO_PTR)"jpeg", 4) || !MemCompare(p, (VO_PTR)"JPEG", 4)))
	{
		imageType = VO_METADATA_IT_JPEG;
	}
	else if((!MemCompare(p, (VO_PTR)"bmp", 3) || !MemCompare(p, (VO_PTR)"BMP", 3)))
	{
		imageType = VO_METADATA_IT_BMP;
	}
	else if((!MemCompare(p, (VO_PTR)"png", 3) || !MemCompare(p, (VO_PTR)"PNG", 3)))
	{
		imageType = VO_METADATA_IT_PNG;
	}
	else if((!MemCompare(p, (VO_PTR)"gif", 3) || !MemCompare(p, (VO_PTR)"GIF", 3)))
	{
		imageType = VO_METADATA_IT_GIF;
	}
	else {
		SAFE_MEM_FREE(pMimeType);
		return VO_FALSE;
	}
	SAFE_MEM_FREE(pMimeType);

	read_dword(size);
	picHeaderSize += size;
	if (picHeaderSize >= dwBlockSize)
		return VO_FALSE;
	
	skip(size);	// skip the description of the picture, in UTF-8.
	skip(20);	// skip other fields

	m_pImage->nImageType = imageType;
	m_pImage->dwBufferSize = dwBlockSize - picHeaderSize;
	m_pImage->pBuffer = NEW_BUFFER(m_pImage->dwBufferSize);
	nRes = m_chunk.FRead(m_pImage->pBuffer, m_pImage->dwBufferSize);
	if(!nRes) {
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CFlacReader::ParseBlock_Other(VO_U32 dwBlockSize)
{
	skip(dwBlockSize);

	return VO_TRUE;
}

VO_BOOL CFlacReader::AddSeekPoint(VO_U32 dwTimeStamp, VO_U32 dwFilePos)
{
	PVOFlacSeekPoint pNew = NEW_OBJ(VOFlacSeekPoint);
	if(!pNew)
		return VO_FALSE;

	pNew->time_stamp = dwTimeStamp;
	pNew->file_pos = dwFilePos;
	pNew->next = VO_NULL;

	if(!m_pLastSeekPoint)
		m_pSeekPoints = m_pLastSeekPoint = pNew;
	else
		m_pLastSeekPoint = m_pLastSeekPoint->next = pNew;

	m_dwSeekPoints++;

	return VO_TRUE;
}

VO_VOID CFlacReader::ReleaseSeekPoints()
{
	SAFE_RELEASE_CHAIN(VOFlacSeekPoint, m_pSeekPoints);

	m_dwSeekPoints = 0;
	m_pLastSeekPoint = VO_NULL;
}

VO_BOOL CFlacReader::FindFrameHeaderInFile()
{
	VO_BYTE byte;

	while(1)
	{
		if (!m_chunk.FRead(&byte, 1)) return VO_FALSE;
		if (byte != 0xFF) continue;
		while(1)
		{
			if (!m_chunk.FRead(&byte, 1)) return VO_FALSE;
			if (byte != 0xFF) break;
		}
		if (byte>>2 != 0x3E) {	// match the last 6 bits of sync code
			continue;
		}
		if (!m_chunk.FRead(&byte, 1)) return VO_FALSE;
		if (byte != m_magicByte1) continue;
		if (!m_chunk.FRead(&byte, 1)) return VO_FALSE;
		if (m_wChannel == 2) {
			if (((byte>>4) & 1) == 1 || ((byte>>4) & 8) == 8) {	// is 2 channels
				if((m_magicByte2 & 0xF) == (byte & 0xF))
					break;
				else
					continue;
			} else {	// is not 2 channels
				continue;
			}
		} else {
			if (byte != m_magicByte2) continue;
		}
		
		//if((m_magicByte2 & 0xF) != (byte & 0xF))
		//	continue;
		break;
	}

	return VO_TRUE;
}

VO_U32 CFlacReader::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch ( uID )
	{
	case VO_PID_METADATA_TITLE:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_title;
			ptr_str->dwBufferSize = strlen( m_title );
		}
		break;
	case VO_PID_METADATA_ALBUM:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_album;
			ptr_str->dwBufferSize = strlen( m_album );
		}
		break;
	case  VO_PID_METADATA_TRACK:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_trackNumber;
			ptr_str->dwBufferSize = strlen( m_trackNumber );
		}
		break;
	case VO_PID_METADATA_DISK:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_discNumber;
			ptr_str->dwBufferSize = strlen( m_discNumber );
		}
		break;
	case VO_PID_METADATA_ARTIST:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_artist;
			ptr_str->dwBufferSize = strlen( m_artist );
		}
		break;
	case  VO_PID_METADATA_COMPOSER:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			if (*m_composer) {
				ptr_str->pBuffer = (VO_PBYTE)m_composer;
				ptr_str->dwBufferSize = strlen( m_composer );
			} else {
				ptr_str->pBuffer = (VO_PBYTE)m_performer;
				ptr_str->dwBufferSize = strlen( m_performer );
			}
		}
		break;
	case VO_PID_METADATA_GENRE:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_genre;
			ptr_str->dwBufferSize = strlen( m_genre );
		}
		break;
	case VO_PID_SOURCE_DATE:
		pParam = m_year;
		break;
	case VO_PID_METADATA_YEAR:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_date;
			ptr_str->dwBufferSize = strlen( m_date );
		}
		break;
	case VO_PID_METADATA_FRONTCOVER:
		{
			PMetaDataImage pImage = (PMetaDataImage)pParam;
			pImage->nImageType = m_pImage->nImageType;
			pImage->pBuffer = m_pImage->pBuffer;
			pImage->dwBufferSize = m_pImage->dwBufferSize;
		}
		break;
	default:
		return CBaseReader::GetParameter( uID , pParam );
	}

	return 0;
}


VO_S64 CFlacReader::GetTimeStampByCurPos()
{
	return m_pCurrSeekPoint->time_stamp + VO_S64(m_dwCurrPosInSeekPoint) * m_pCurrSeekPoint->duration / m_pCurrSeekPoint->length;
}