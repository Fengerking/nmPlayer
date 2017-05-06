#include "CFlvParser.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#define read_time_stamp(dw)\
{\
	tp = (VO_PBYTE)&dw;\
	ptr_read_pointer(tp + 2, 1);\
	ptr_read_pointer(tp + 1, 1);\
	ptr_read_pointer(tp, 1);\
	ptr_read_pointer(tp + 3, 1);\
}

DEFINE_USE_FLV_GLOBAL_VARIABLE(CFlvHeaderParser)

CFlvHeaderParser::CFlvHeaderParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileHeaderParser(pFileChunk, pMemOp)
	, m_btFileTypeFlags(0)
	, m_dwDuration(0)
	, m_ullFileSize(0)
	, m_ullMediaDataFilePos(0)
	, m_dwIndexEntries(0)
	, m_pIndexEntries(VO_NULL)
	, m_bMetaDataReaded(VO_FALSE)
	, m_physical_file_size(0)
	, m_metadata_end_pos(0)
	,m_bThumbNail(VO_FALSE)
{
	MemSet(&m_AudioInfo, 0, sizeof(m_AudioInfo));
	m_AudioInfo.btCodecID = 0xFF;

	MemSet(&m_VideoInfo, 0, sizeof(m_VideoInfo));
	m_VideoInfo.btCodecID = 0xFF;
}

CFlvHeaderParser::~CFlvHeaderParser()
{
	SAFE_MEM_FREE(m_pIndexEntries);

	SAFE_MEM_FREE(m_AudioInfo.ptr_header_data);
	SAFE_MEM_FREE(m_VideoInfo.ptr_header_data);
}

VO_BOOL CFlvHeaderParser::ReadFromFile()
{
	m_bMetaDataReaded = VO_FALSE;

	use_big_endian_read

	//FLV Header
	FlvFileHeader ffh;
	ptr_read_pointer(ffh.btSignature, 3);
	if(0x46 != ffh.btSignature[0] || 0x4c != ffh.btSignature[1] || 0x56 != ffh.btSignature[2])
		return VO_FALSE;

	ptr_read_byte(ffh.btVersion);
	ptr_read_byte(ffh.btTypeFlags);
	ptr_read_dword(ffh.dwDataOffset);

	m_btFileTypeFlags = ffh.btTypeFlags;

	m_pFileChunk->FSkip( ffh.dwDataOffset - 9 );

	VO_U32 tag_count = 0;

	m_ullMediaDataFilePos = m_pFileChunk->FGetFilePos();
	m_metadata_end_pos = m_ullMediaDataFilePos + 15;

	VO_BOOL bThumbNail = VO_TRUE;

	while(bThumbNail && tag_count < 20)
	{
		VO_U64 tmpPos = m_pFileChunk->FGetFilePos();
	
		VO_U32 dwPrevTagSize = 0;
		ptr_read_dword(dwPrevTagSize);

		FlvTag ft;
		ptr_read_byte(ft.btTagType);
		ptr_read_3byte_dword(ft.dwDataSize);
		read_time_stamp(ft.dwTimeStamp);
		ptr_read_3byte_dword(ft.dwStreamID);

		switch( ft.btTagType )
		{
		case 8:
			read_audio_data( ft.dwDataSize );
			break;
		case 9:
			read_video_data( ft.dwDataSize );
			if (m_bThumbNail)
			{
				bThumbNail = VO_FALSE;
				m_pFileChunk->FLocate(tmpPos);
			}
			break;
		case 18:
			m_metadata_end_pos += ft.dwDataSize;
			read_script_data( ft.dwDataSize );
			break;
		default:
			break;
		}

		//if( m_bMetaDataReaded && IsAudioInfoValid() && IsVideoInfoValid() )
			//break;

		tag_count++;
	}

	if( m_dwDuration == 0 )
	{
		m_pFileChunk->FLocate( m_physical_file_size - 4 );
		
		VO_U32 tag_size;
		ptr_read_dword( tag_size );

		if( tag_size < m_physical_file_size )
		{
			m_pFileChunk->FLocate( m_physical_file_size - 4 - tag_size );
			m_pFileChunk->FSkip( 1 );
			VO_U32 data_size = 0;
			ptr_read_3byte_dword( data_size );

			if( data_size + 11 == tag_size )
				read_time_stamp( m_dwDuration );
		}

	}

	if( m_dwDuration == 0 )
	{
		find_duration_info();
	}

	//FLV File Body
	//while(!m_bMetaDataReaded || 
	//	((m_btFileTypeFlags & FLV_FLAG_AUDIO_PRESENT) && !IsAudioInfoValid()) || 
	//	((m_btFileTypeFlags & FLV_FLAG_VIDEO_PRESENT) && !IsVideoInfoValid()))
	//{
	//	VO_U32 dwPrevTagSize = 0;
	//	ptr_read_dword(dwPrevTagSize);

	//	FlvTag ft;
	//	ptr_read_byte(ft.btTagType);
	//	ptr_read_3byte_dword(ft.dwDataSize);
	//	read_time_stamp(ft.dwTimeStamp);
	//	ptr_read_3byte_dword(ft.dwStreamID);

	//	//if audio, video data appear before meta data, we not support!!
	//	switch(ft.btTagType)
	//	{
	//	case 0x8:					//audio
	//		{
	//			VO_U8 btFlag;
	//			ptr_read_byte(btFlag);
	//			ptr_skip(ft.dwDataSize - 1);

	//			m_btFileTypeFlags |= FLV_FLAG_AUDIO_PRESENT;

	//			m_AudioInfo.btCodecID = ((btFlag & 0xF0) >> 4);
	//			m_AudioInfo.nSamplesPerSec = s_nSamplesPerSec[(btFlag & 0xC) >> 2];
	//			m_AudioInfo.wBitsPerSample = (btFlag & 0x2) ? 16 : 8;
	//			m_AudioInfo.nChannels = (btFlag & 0x1) ? 2 : 1;
	//		}
	//		break;

	//	case 0x9:					//video
	//		{
	//			VO_U8 btFlag;
	//			ptr_read_byte(btFlag);
	//			ptr_skip(ft.dwDataSize - 1);

	//			m_btFileTypeFlags |= FLV_FLAG_VIDEO_PRESENT;

	//			m_VideoInfo.btCodecID = (btFlag & 0xF);
	//		}
	//		break;

	//	case 0x12:					//script data
	//		{
	//			if(!ReadScriptData(ft.dwDataSize))
	//				return VO_FALSE;

	//			if(m_bMetaDataReaded)
	//				m_ullMediaDataFilePos = m_pFileChunk->FGetFilePos();
	//		}
	//		break;

	//	default:
	//		ptr_skip(ft.dwDataSize);
	//		break;
	//	}
	//}

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::ReadScriptData(VO_U32 dwDataSize)
{
	m_pFileChunk->FStartRecord();

	if( m_pFileChunk->FGetFilePos() >= m_metadata_end_pos )
	{
		m_pFileChunk->FLocate( m_metadata_end_pos );
		return VO_FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//SCRIPTDATAOBJECT
	VO_U8 btObjNameType;

	//Object Name Type(always 2)
	ptr_read_byte(btObjNameType);
	if(0x2 != btObjNameType)
		return VO_FALSE;

	//Object Name
	VO_CHAR szName[FLV_SCRIPT_DATA_STRING_LENGTH];
	if(!ReadScriptDataString(szName))
		return VO_FALSE;

	//Object Data
	if(!ReadScriptDataValue(szName))
		return VO_FALSE;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//SCRIPTDATAOBJECTEND
	//ObjectEndMarker1(0x02) or ObjectEndMarker2(0x00 0x00 0x09)
	VO_S32 nLeft = dwDataSize - static_cast<VO_S32>( m_pFileChunk->FGetLastRead() );
	if(nLeft <= 0)
		return VO_FALSE;

	VO_U8 btObjEndMarker;
	if(nLeft == 1)
	{
		ptr_read_byte(btObjEndMarker);

		if(0x2 != btObjEndMarker)
			return VO_FALSE;
	}
	else if(nLeft == 3)
	{
		ptr_read_byte(btObjEndMarker);
		if(0x0 != btObjEndMarker)
			return VO_FALSE;

		ptr_read_byte(btObjEndMarker);
		if(0x0 != btObjEndMarker)
			return VO_FALSE;

		ptr_read_byte(btObjEndMarker);
		if(0x9 != btObjEndMarker)
			return VO_FALSE;
	}
	else if(nLeft > 3)
	{
		ptr_skip(nLeft - 3);

		VO_U8 btObjEndMarker;
		ptr_read_byte(btObjEndMarker);
		if(0x0 != btObjEndMarker)
		{
			ptr_skip(1);

			ptr_read_byte(btObjEndMarker);
			if(0x2 != btObjEndMarker)
				return VO_FALSE;
		}
		else
		{
			ptr_read_byte(btObjEndMarker);
			if(0x0 != btObjEndMarker)
				return VO_FALSE;

			ptr_read_byte(btObjEndMarker);
			if(0x9 != btObjEndMarker)
				return VO_FALSE;
		}
	}
	else
		return VO_FALSE;
	//////////////////////////////////////////////////////////////////////////

	if(!strcmp(szName, SDON_ON_META_DATA))
		m_bMetaDataReaded = VO_TRUE;

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::ReadScriptDataString(VO_PCHAR szString)
{
	if( m_pFileChunk->FGetFilePos() >= m_metadata_end_pos )
	{
		m_pFileChunk->FLocate( m_metadata_end_pos );
		return VO_FALSE;
	}

	use_big_endian_read

	VO_U16 wStrLen;
	ptr_read_word(wStrLen);

	if(wStrLen >= FLV_SCRIPT_DATA_STRING_LENGTH || !szString)
	{
		ptr_skip(wStrLen);
	}
	else if (0 == wStrLen)
	{
		memset(szString, 0, FLV_SCRIPT_DATA_STRING_LENGTH);
		m_pFileChunk->FBack(2);
	}
	else
	{
		ptr_read_pointer(szString, wStrLen);
		szString[wStrLen] = '\0';
	}

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::ReadScriptDataValue(const VO_PCHAR szString)
{
	if (strlen(szString) == 0)
		return VO_TRUE;

	if( m_pFileChunk->FGetFilePos() >= m_metadata_end_pos )
	{
		m_pFileChunk->FLocate( m_metadata_end_pos );
		return VO_FALSE;
	}

	VO_U8 btType;
	ptr_read_byte(btType);

	switch(btType)
	{
	case 0:		//Number
		{
			use_big_endian_read

			double dValue;
			ptr_read_qword(dValue);

			if(!strcmp(szString, SDON_AUDIO_DATA_RATE))	//Kb/S -> B/S
			{
				m_btFileTypeFlags |= FLV_FLAG_AUDIO_PRESENT;
				m_AudioInfo.dwBytesPerSec = static_cast<VO_U32>(dValue * 125);
			}
			else if(!strcmp(szString, SDON_AUDIO_SAMPLE_RATE))
			{
				m_btFileTypeFlags |= FLV_FLAG_AUDIO_PRESENT;
				m_AudioInfo.nSamplesPerSec = static_cast<VO_U32>(dValue);
			}
			else if(!strcmp(szString, SDON_AUDIO_SAMPLE_SIZE))
			{
				m_btFileTypeFlags |= FLV_FLAG_AUDIO_PRESENT;
				m_AudioInfo.wBitsPerSample = static_cast<VO_U16>(dValue);
			}
			else if(!strcmp(szString, SDON_AUDIO_DELAY))	//S -> MS
				m_btFileTypeFlags |= FLV_FLAG_AUDIO_PRESENT;
			else if(!strcmp(szString, SDON_FRAME_RATE))	//F/S -> 100ns/F
			{
				m_btFileTypeFlags |= FLV_FLAG_VIDEO_PRESENT;
				if(dValue)
					m_VideoInfo.llAvgTimePerFrame = static_cast<VO_U64>(10000000 / dValue);
			}
			else if(!strcmp(szString, SDON_HEIGHT))	//F/S -> 100ns/F
			{
				m_btFileTypeFlags |= FLV_FLAG_VIDEO_PRESENT;
				m_VideoInfo.wHeight = static_cast<VO_U16>(dValue);
			}
			else if(!strcmp(szString, SDON_WIDTH))	//F/S -> 100ns/F
			{
				m_btFileTypeFlags |= FLV_FLAG_VIDEO_PRESENT;
				m_VideoInfo.wWidth = static_cast<VO_U16>(dValue);
			}
//			else if(!strcmp(szString, SDON_LAST_TIME_STAMP))	//S -> MS
//			else if(!strcmp(szString, SDON_LAST_KEY_FRAME_TIME_STAMP))	//S -> MS
			else if(!strcmp(szString, SDON_AUDIO_CODEC_ID))
			{
				m_btFileTypeFlags |= FLV_FLAG_AUDIO_PRESENT;
				m_AudioInfo.btCodecID = static_cast<VO_U8>(dValue);
			}
			else if(!strcmp(szString, SDON_VIDEO_DATA_RATE))	//Kb/S -> B/S
			{
				m_btFileTypeFlags |= FLV_FLAG_VIDEO_PRESENT;
				m_VideoInfo.dwBytesPerSec = static_cast<VO_U32>(dValue * 125);
			}
			else if(!strcmp(szString, SDON_DURATION))	//S -> MS
				m_dwDuration = static_cast<VO_U32>(dValue * 1000);
			else if(!strcmp(szString, SDON_VIDEO_CODEC_ID))
			{
				m_btFileTypeFlags |= FLV_FLAG_VIDEO_PRESENT;
				m_VideoInfo.btCodecID = static_cast<VO_U8>(dValue);
			}
			else if(!strcmp(szString, SDON_FILE_SIZE))
				m_ullFileSize = static_cast<VO_U64>(dValue);
		}
		break;

	case 1:		//Boolean
		{
			VO_U8 btValue;
			ptr_read_byte(btValue);
			if(!strcmp(szString, SDON_STEREO))
			{
				m_btFileTypeFlags |= FLV_FLAG_AUDIO_PRESENT;
				m_AudioInfo.nChannels = btValue ? 2 : 1;
			}
		}
		break;

	case 2:		//String
		{
			ReadScriptDataString(NULL);
		}
		break;

	case 3:		//Object
		{
			VO_CHAR szName[FLV_SCRIPT_DATA_STRING_LENGTH];
			VO_U8 btObjEndMarker = 0;
			while(1)
			{
				//Object Name
				if(!ReadScriptDataString(szName))
					return VO_FALSE;

				//Object Data
				if(!ReadScriptDataValue(szName))
					return VO_FALSE;

				ptr_read_byte(btObjEndMarker);
				if(2 == btObjEndMarker)		//ObjectEndMarker1
					break;
				else if(0 == btObjEndMarker)
				{
					ptr_read_byte(btObjEndMarker);
					if(0 != btObjEndMarker)
					{
						m_pFileChunk->FBack(2);
						continue;
					}

					ptr_read_byte(btObjEndMarker);
					if(9 != btObjEndMarker)
					{
						m_pFileChunk->FBack(3);
						continue;
					}

					//ObjectEndMarker2
					break;
				}
				else
				{
					m_pFileChunk->FBack(1);
					continue;
				}
			}
		}
		break;

	case 4:		//Movie Clip
	case 5:		//Null
	case 6:		//Undefined
	case 7:		//Reference
		break;

	case 8:		//ECMA array
		{
			use_big_endian_read

			VO_U32 dwArrayLen;
			ptr_read_dword(dwArrayLen);

			VO_CHAR szName[FLV_SCRIPT_DATA_STRING_LENGTH];
			for(VO_U32 i = 0; i < (0 != dwArrayLen ? dwArrayLen : 20); i++)
			{
				//Object Name
				if(!ReadScriptDataString(szName))
					return VO_FALSE;

				//Object Data
				if(!ReadScriptDataValue(szName))
					return VO_FALSE;
			}
		}
		break;

	case 10:	//Strict array
		{
			if(!strcmp(szString, SDON_CUE_POINTS))
			{
				ptr_skip(4);
			}
			else if(!strcmp(szString, SDON_TIMES))
			{
				use_big_endian_read

				VO_U32 dwSeekPoints;
				ptr_read_dword(dwSeekPoints);

				if(dwSeekPoints > m_dwIndexEntries)
				{
					if(m_pIndexEntries)
						MemFree(m_pIndexEntries);

					m_dwIndexEntries = dwSeekPoints;
					m_pIndexEntries = NEW_OBJS(FlvIndexEntry, m_dwIndexEntries);
					if(!m_pIndexEntries)
						return VO_FALSE;
				}

				double dValue;
				for(VO_U32 i = 0; i < dwSeekPoints; i++)
				{
//					ReadScriptDataValue(szString);
					//first byte is Type, we can skip, it must be 0
					ptr_skip(1);

					//double
					ptr_read_qword(dValue);
					m_pIndexEntries[i].dwTimeStamp = static_cast<VO_U32>(dValue * 1000);
				}
			}
			else if(!strcmp(szString, SDON_FILE_POSITIONS))
			{
				use_big_endian_read

				VO_U32 dwSeekPoints;
				ptr_read_dword(dwSeekPoints);

				if(dwSeekPoints > m_dwIndexEntries)
				{
					if(m_pIndexEntries)
						MemFree(m_pIndexEntries);

					m_dwIndexEntries = dwSeekPoints;
					m_pIndexEntries = NEW_OBJS(FlvIndexEntry, m_dwIndexEntries);
					if(!m_pIndexEntries)
						return VO_FALSE;
				}

				double dValue;
				for(VO_U32 i = 0; i < dwSeekPoints; i++)
				{
//					ReadScriptDataValue(szString);
					//first byte is Type, we can skip, it must be 0
					ptr_skip(1);

					//double
					ptr_read_qword(dValue);
					m_pIndexEntries[i].qwFilePos = static_cast<VO_U64>(dValue);
				}
			}
		}
		break;

	case 11:	//Date
		{
			ptr_skip(10);
		}
		break;

	case 12:	//Long String
		break;

	default:
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_U64 CFlvHeaderParser::IndexGetFilePosByTime(VO_S64 llTimeStamp)
{
	if (llTimeStamp == 0)
		return m_pIndexEntries[0].qwFilePos;

	VO_U32 dwIndex = 0;
	for(VO_U32 i = 0; i < m_dwIndexEntries && m_pIndexEntries[i].dwTimeStamp <= llTimeStamp; i++)
		dwIndex = i;

	return m_pIndexEntries[dwIndex].qwFilePos;
}

VO_U64 CFlvHeaderParser::IndexGetFilePosByIndex(VO_U32 dwIndex)
{
	return (dwIndex < m_dwIndexEntries) ? m_pIndexEntries[dwIndex].qwFilePos : -1;
}

VO_U32 CFlvHeaderParser::IndexGetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp)
{
	VO_U32 dwIndex = 0;
	for(VO_U32 i = 0; i < m_dwIndexEntries; i++)
	{
		dwIndex = i;

		if(m_pIndexEntries[i].dwTimeStamp >= llTimeStamp)
			break;
	}

	return dwIndex;
}

VO_S64 CFlvHeaderParser::IndexGetTimeByFilePos( VO_U64 filepos )
{
	VO_S64 timestamp = -1;

	for( VO_U32 i = 0 ; i < m_dwIndexEntries ; i++ )
	{
		if( m_pIndexEntries[i].qwFilePos <= filepos )
			timestamp = m_pIndexEntries[i].dwTimeStamp;
		else
			break;	
	}

	return timestamp;
}

VO_S64 CFlvHeaderParser::GetPrevKeyFrameTime(VO_S64 llTimeStamp)
{
	VO_S64 timestamp = -1;

	for (VO_U32 i = 0 ; i < m_dwIndexEntries ; i++)
	{
		if (m_pIndexEntries[i].dwTimeStamp <= llTimeStamp)
			timestamp = m_pIndexEntries[i].dwTimeStamp;
		else
			break;	
	}

	return timestamp;
}

VO_S64 CFlvHeaderParser::GetNextKeyFrameTime(VO_S64 llTimeStamp)
{
	VO_S64 timestamp = -1;

	for (VO_U32 i = 0 ; i < m_dwIndexEntries ; i++)
	{
		if (m_pIndexEntries[i].dwTimeStamp > llTimeStamp)
		{
			timestamp = m_pIndexEntries[i].dwTimeStamp;
			break;
		}
	}

	return (-1 == timestamp) ? VO_MAXU64 : timestamp;
}

VO_BOOL CFlvHeaderParser::read_audio_data( VO_U32 data_size )
{
	VO_U8 btFlag;
	ptr_read_byte(btFlag);

	m_btFileTypeFlags |= FLV_FLAG_AUDIO_PRESENT;

	m_AudioInfo.btCodecID = ((btFlag & 0xF0) >> 4);
	m_AudioInfo.nSamplesPerSec = s_nSamplesPerSec[(btFlag & 0xC) >> 2];
	m_AudioInfo.wBitsPerSample = (btFlag & 0x2) ? 16 : 8;
	m_AudioInfo.nChannels = (btFlag & 0x1) ? 2 : 1;

	data_size -= 1;

	switch( m_AudioInfo.btCodecID )
	{
	case 1://adpcm
		{
			if (!m_AudioInfo.header_size)
			{
				m_AudioInfo.ptr_header_data = (VO_PBYTE)MemAlloc( 1 );
				MemCopy(m_AudioInfo.ptr_header_data, &btFlag, 1);

				m_AudioInfo.header_size = 1;
			}

			m_pFileChunk->FSkip( data_size );
		}
		//read_audio_data_adpcm( data_size );
		break;
	case 10://aac
		read_audio_data_aac( data_size );
		break;
	default:
		m_pFileChunk->FSkip( data_size );
		break;
	}

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::read_audio_data_aac( VO_U32 data_size )
{
	VO_U8 flag;
	ptr_read_byte(flag);

	data_size -= 1;

	if( flag == 0 )
	{
		if( m_AudioInfo.ptr_header_data )
		{
			VO_PBYTE ptr_tmp = (VO_PBYTE)MemAlloc( data_size + m_AudioInfo.header_size );
			MemCopy( ptr_tmp , m_AudioInfo.ptr_header_data , m_AudioInfo.header_size );
			MemFree( m_AudioInfo.ptr_header_data );
			m_AudioInfo.ptr_header_data = ptr_tmp;
			ptr_tmp = ptr_tmp + m_AudioInfo.header_size;
			m_pFileChunk->FRead( ptr_tmp , data_size );
			m_AudioInfo.header_size = data_size + m_AudioInfo.header_size;
		}
		else
		{
			m_AudioInfo.header_size = data_size;
			m_AudioInfo.ptr_header_data = (VO_PBYTE)MemAlloc( data_size );
			m_pFileChunk->FRead( m_AudioInfo.ptr_header_data , data_size );
		}
	}
	else
	{
		VO_U8 b;
		m_pFileChunk->FRead( &b , 1 );

		if( b == 0xff && m_AudioInfo.header_size == 0 )
			m_AudioInfo.btCodecID = 11;

		m_pFileChunk->FSkip( data_size - 1 );
	}

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::read_audio_data_adpcm( VO_U32 data_size )
{
	if( m_AudioInfo.ptr_header_data )
	{
		VO_PBYTE ptr_tmp = (VO_PBYTE)MemAlloc( data_size + m_AudioInfo.header_size );
		MemCopy( ptr_tmp , m_AudioInfo.ptr_header_data , m_AudioInfo.header_size );
		MemFree( m_AudioInfo.ptr_header_data );
		m_AudioInfo.ptr_header_data = ptr_tmp;
		ptr_tmp = ptr_tmp + m_AudioInfo.header_size;
		m_pFileChunk->FRead( ptr_tmp , data_size );
		m_AudioInfo.header_size = data_size + m_AudioInfo.header_size;
	}
	else
	{
		m_AudioInfo.header_size = data_size;
		m_AudioInfo.ptr_header_data = (VO_PBYTE)MemAlloc( data_size );
		m_pFileChunk->FRead( m_AudioInfo.ptr_header_data , data_size );
	}

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::read_video_data( VO_U32 data_size )
{
	VO_U8 btFlag;
	ptr_read_byte(btFlag);

	m_btFileTypeFlags |= FLV_FLAG_VIDEO_PRESENT;
	m_VideoInfo.btCodecID = (btFlag & 0xF);

	data_size -= 1;

	switch( m_VideoInfo.btCodecID )
	{
	case 1://jpeg not used
		m_pFileChunk->FSkip( data_size );
		break;
	case 2://sorenson h.263
		read_video_data_sorenson263(data_size);
		break;
	case 3://screen video
		read_video_data_screen_video(data_size);
		break;
	case 4://on2 vp6
		read_video_data_vp6(data_size);
		break;
	case 5://on2 vp6 with alpha channel
		read_video_data_vp6_alpha(data_size);
		break;
	case 6://screen video v2
		read_video_data_screen_video2(data_size);
		break;
	case 7://avc
		read_video_data_avc(data_size);
		break;
	default:
		m_pFileChunk->FSkip( data_size );
		break;
	}

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::read_video_data_sorenson263( VO_U32 data_size )
{
	m_pFileChunk->FSkip( 3 );
	data_size -= 3;

	VO_BYTE flag1 , flag2;
	ptr_read_byte(flag1);
	ptr_read_byte(flag2);

	VO_U32 pic_size = 0;

	pic_size = (( flag1 & 0x3 ) << 1) + ( flag2 >> 7 );

	data_size -= 2;

	switch( pic_size )
	{
	case 0:
		{
			VO_BYTE b1 , b2;
			ptr_read_byte(b1);
			ptr_read_byte(b2);

			m_VideoInfo.wWidth = ((VO_BYTE)( flag2 << 1 )) + ((VO_BYTE)( b1 >> 7 ));
			m_VideoInfo.wHeight = ((VO_BYTE)( b1 << 1 )) + ((VO_BYTE)( b2 >> 7 ));

			data_size -= 2;
		}
		break;
	case 1:
		{
			VO_BYTE b1 , b2 , b3 , b4;
			ptr_read_byte(b1);
			ptr_read_byte(b2);
			ptr_read_byte(b3);
			ptr_read_byte(b4);

			VO_U32 width , height;
			width = ((VO_BYTE)( flag2 << 1 )) + ((VO_BYTE)( b1 >> 7 ));
			m_VideoInfo.wWidth = static_cast<VO_U16>( (width << 8) + ( (VO_BYTE)(b1 << 1) ) + ((VO_BYTE)(b2 >> 7)) );

			height = ((VO_BYTE)( b2 << 1 )) + ((VO_BYTE)( b3 >> 7 ));
			m_VideoInfo.wHeight = static_cast<VO_U16>( (height << 8) + ( (VO_BYTE)(b3 << 1) ) + ( (VO_BYTE)(b4 >> 7) ) );

			data_size -= 4;
		}
		break;
	case 2:
		m_VideoInfo.wWidth = 352;
		m_VideoInfo.wHeight = 288;
		break;
	case 3:
		m_VideoInfo.wWidth = 176;
		m_VideoInfo.wHeight = 144;
		break;
	case 4:
		m_VideoInfo.wWidth = 128;
		m_VideoInfo.wHeight = 96;
		break;
	case 5:
		m_VideoInfo.wWidth = 320;
		m_VideoInfo.wHeight = 240;
		break;
	case 6:
		m_VideoInfo.wWidth = 160;
		m_VideoInfo.wHeight = 120;
		break;
	default:
		break;
	}

	m_pFileChunk->FSkip( data_size );

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::read_video_data_screen_video( VO_U32 data_size )
{
	VO_BYTE width_high , width_low , height_high , height_low;
	ptr_read_byte( width_high );
	width_high = width_high & 0xf;
	ptr_read_byte( width_low );
	m_VideoInfo.wWidth = width_high;
	m_VideoInfo.wWidth = ( m_VideoInfo.wWidth << 8 ) + width_low;

	ptr_read_byte( height_high );
	height_high = height_high & 0xf;
	ptr_read_byte( height_low );
	m_VideoInfo.wHeight = height_high;
	m_VideoInfo.wHeight = ( m_VideoInfo.wHeight << 8 ) + height_low;

	data_size -= 4;

	m_pFileChunk->FSkip( data_size );

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::read_video_data_screen_video2( VO_U32 data_size )
{
	return read_video_data_screen_video( data_size );
}

VO_BOOL CFlvHeaderParser::read_video_data_vp6( VO_U32 data_size )
{
	m_pFileChunk->FSkip( data_size );
	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::read_video_data_vp6_alpha( VO_U32 data_size )
{
	m_pFileChunk->FSkip( data_size );
	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::read_video_data_avc( VO_U32 data_size )
{
	use_big_endian_read;

	VO_U8 flag;
	ptr_read_byte(flag);

	VO_U32 time;
	ptr_read_3byte_dword(time);

	data_size -= 4;

	if( flag == 0 )
	{
		if( m_VideoInfo.ptr_header_data )
		{
			VO_PBYTE ptr_tmp = (VO_PBYTE)MemAlloc( data_size + m_VideoInfo.header_size );
			MemCopy( ptr_tmp , m_VideoInfo.ptr_header_data , m_VideoInfo.header_size );
			MemFree( m_VideoInfo.ptr_header_data );
			m_VideoInfo.ptr_header_data = ptr_tmp;
			ptr_tmp = ptr_tmp + m_VideoInfo.header_size;
			m_pFileChunk->FRead( ptr_tmp , data_size );
			m_VideoInfo.header_size = data_size + m_VideoInfo.header_size;
		}
		else
		{
			m_VideoInfo.header_size = data_size;
			m_VideoInfo.ptr_header_data = (VO_PBYTE)MemAlloc( data_size );
			m_pFileChunk->FRead( m_VideoInfo.ptr_header_data , data_size );
		}
	}
	else
	{
		m_pFileChunk->FSkip( data_size );
	}

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::read_script_data( VO_U32 data_size )
{
	if(!ReadScriptData(data_size))
		return VO_FALSE;

	return VO_TRUE;
}

VO_VOID CFlvHeaderParser::find_duration_info()
{
	if( m_physical_file_size <= 1024000 )
	{
		get_duration_by_travel( m_ullMediaDataFilePos );
		return;
	}

	VO_U64 file_pos = m_physical_file_size - 102400;

	while( file_pos > m_ullMediaDataFilePos )
	{
		VO_U64 tag_pos = 0;

		VO_BOOL ret = find_tag_by_pos( file_pos , 102400 , tag_pos );

		if( ret )
		{
			get_duration_by_travel( tag_pos );
			break;
		}
		else
		{
			file_pos = file_pos - 102400;
		}
	}
}

VO_BOOL CFlvHeaderParser::find_tag_by_pos( VO_U64 file_pos , VO_U64 find_range , VO_U64& first_tag_pos )
{
	VO_BYTE value;

	m_pFileChunk->FLocate( file_pos );

	for( VO_U32 offset = 0 ; offset < find_range ; offset++ )
	{
		m_pFileChunk->FRead( &value , 1 );

		switch( value )
		{
		case 0x8:
		case 0x9:
			{
				if( test_is_tag() )
				{
					first_tag_pos = m_pFileChunk->FGetFilePos() - 1;
					return VO_TRUE;
				}
			}
			break;
		}
	}

	return VO_FALSE;
}

VO_BOOL CFlvHeaderParser::get_duration_by_travel( VO_U64 file_pos )
{
	m_pFileChunk->FLocate( file_pos );

	use_big_endian_read
	while( 1 )
	{
		FlvTag ft;
		ptr_read_byte(ft.btTagType);
		ptr_read_3byte_dword(ft.dwDataSize);
		read_time_stamp(ft.dwTimeStamp);
		ptr_read_3byte_dword(ft.dwStreamID);

		if( ft.dwTimeStamp > m_dwDuration )
			m_dwDuration = ft.dwTimeStamp;

		if( !m_pFileChunk->FSkip( ft.dwDataSize ) )
			return VO_FALSE;

		VO_U32 dwPrevTagSize = 0;
		ptr_read_dword(dwPrevTagSize);
	}

	return VO_TRUE;
}

VO_BOOL CFlvHeaderParser::test_is_tag()
{
	file_pos_rollback rollback( m_pFileChunk );

	use_big_endian_read

	VO_U32 tag_size , tag_data_size;
	ptr_read_3byte_dword(tag_data_size);
	m_pFileChunk->FSkip( 7 + tag_data_size );
	ptr_read_dword( tag_size );

	if( tag_data_size + 11 == tag_size )
		return VO_TRUE;
	else
		return VO_FALSE;
}

VO_S32 CFlvHeaderParser::GetTrackCount()
{
	VO_S32 count = 0;

	if( IsAudioInfoValid() )
		count++;

	if( IsVideoInfoValid() )
		count++;

	return count;
}

CFlvDataParser::CFlvDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileDataParser(pFileChunk, pMemOp)
	,m_bGetThumbNail(VO_FALSE)
{
}

CFlvDataParser::~CFlvDataParser()
{
}

VO_VOID CFlvDataParser::Init(VO_PTR pParam)
{
}

VO_BOOL CFlvDataParser::StepB()
{
	VO_U64 tmpPos = m_pFileChunk->FGetFilePos();

	use_big_endian_read

	VO_U32 dwPrevTagSize = 0;
	ptr_read_dword(dwPrevTagSize);

	FlvTag ft;
	ptr_read_byte(ft.btTagType);
	ptr_read_3byte_dword(ft.dwDataSize);
	read_time_stamp(ft.dwTimeStamp);
	ptr_read_3byte_dword(ft.dwStreamID);

	switch(ft.btTagType)
	{
	case 0x8:	//audio
		{
			if(m_bGetThumbNail)
			{
				ptr_skip(ft.dwDataSize);
				break;
			}

			if((m_btBlockStream == ft.btTagType) && (!m_fOBCallback || m_fOBCallback(m_pOBUser, m_pOBUserData, &ft.dwTimeStamp)))
			{
				ptr_skip(ft.dwDataSize);

				return VO_TRUE;
			}

			FlvDataParserSample sample;
			ptr_read_byte(sample.btFlag);
			sample.bVideo = VO_FALSE;

			if( sample.btFlag >> 4 == 0xa )
			{
				m_pFileChunk->FSkip( 1 );
				ft.dwDataSize -= 1;
			}

			sample.dwLen = ft.dwDataSize - 1;
			sample.dwTimeStamp = ft.dwTimeStamp;
			sample.pFileChunk = m_pFileChunk;
			sample.pos = m_pFileChunk->FGetFilePos();
			VO_U8 btRes = m_fODCallback(m_pODUser, m_pODUserData, &sample);

			if(!CBRT_IS_CONTINUABLE(btRes))
				return VO_FALSE;

			if(CBRT_IS_NEEDSKIP(btRes))
				ptr_skip(ft.dwDataSize - 1);
		}
		break;
	case 0x9:	//video
		{
			if((m_btBlockStream == ft.btTagType) && (!m_fOBCallback || m_fOBCallback(m_pOBUser, m_pOBUserData, &ft.dwTimeStamp)))
			{
				ptr_skip(ft.dwDataSize);

				return VO_TRUE;
			}

			VO_S32 datasize = ft.dwDataSize;

			FlvDataParserSample sample;
			ptr_read_byte(sample.btFlag);

			datasize--;

 			if( (sample.btFlag & 0xF) == 7 )
 			{
 				m_pFileChunk->FSkip( 4 );
 				datasize -= 4;
 			}

			if(m_bGetThumbNail && (0x10 != (sample.btFlag&0x10)))
			{
				ptr_skip(datasize);
				return VO_TRUE;
			}

			sample.bVideo = VO_TRUE;
			sample.dwLen = datasize;
			sample.dwTimeStamp = ft.dwTimeStamp;
			sample.pFileChunk = m_pFileChunk;
			sample.pos = m_pFileChunk->FGetFilePos();;
			VO_U8 btRes = m_fODCallback(m_pODUser, m_pODUserData, &sample);

			if(!CBRT_IS_CONTINUABLE(btRes))
				return VO_FALSE;

			if(CBRT_IS_NEEDSKIP(btRes))
				ptr_skip(datasize);
		}
		break;

	case 0x12:					//script data
	default:
		ptr_skip(ft.dwDataSize);
		break;
	}

	return VO_TRUE;
}

VO_BOOL CFlvDataParser::GetFilePosByTime(VO_U64 ullStartFilePos, VO_U32 dwMaxSampleSize, VO_U32 dwMaxTimeStamp, VO_S64 llTimeStamp, VO_U64& ullFilePos)
{
	if(!m_pFileChunk->FLocate(ullStartFilePos))
		return VO_FALSE;

	ullFilePos = VO_MAXU64;

	use_big_endian_read

	VO_U64 ullVideoFlvTagsPos = ullStartFilePos, dwPrevTagSize = 0;
	FlvTag ft;
	while(1)
	{
		ptr_read_byte(ft.btTagType);
		if(0x9 != ft.btTagType)
			continue;

		//0x9 found!!
		ptr_read_3byte_dword(ft.dwDataSize);
		if(ft.dwDataSize > (dwMaxSampleSize + 1) || ft.dwDataSize < 1)
		{
			m_pFileChunk->FBack(3);
			continue;
		}

		read_time_stamp(ft.dwTimeStamp);
		if(ft.dwTimeStamp > dwMaxTimeStamp)
		{
			m_pFileChunk->FBack(7);
			continue;
		}

		ptr_read_3byte_dword(ft.dwStreamID);	//always 0
		if(ft.dwStreamID)
		{
			m_pFileChunk->FBack(10);
			continue;
		}

		ullVideoFlvTagsPos = m_pFileChunk->FGetFilePos() - 11;

		if(m_pFileChunk->FSkip(ft.dwDataSize))
		{
			tp = (VO_PBYTE)&dwPrevTagSize;
			if(m_pFileChunk->FRead(tp + 3, 1) && m_pFileChunk->FRead(tp + 2, 1) && m_pFileChunk->FRead(tp + 1, 1) && m_pFileChunk->FRead(tp, 1) && 
				(dwPrevTagSize != ft.dwDataSize + 11))	//PreviousTagSize not fit!!
			{
				m_pFileChunk->FLocate(ullVideoFlvTagsPos + 1);
				continue;
			}
		}
		break;
	}

	if(!m_pFileChunk->FLocate(ullVideoFlvTagsPos - 4))
		return VO_FALSE;

	VO_BOOL bFindNext = VO_TRUE;
	while(1)
	{
		ptr_read_dword(dwPrevTagSize);

		ptr_read_byte(ft.btTagType);
		ptr_read_3byte_dword(ft.dwDataSize);
		read_time_stamp(ft.dwTimeStamp);
		ptr_read_3byte_dword(ft.dwStreamID);

		if(0x9 == ft.btTagType)
		{
			VO_U8 btFlag;
			ptr_read_byte(btFlag);

			if(ft.dwTimeStamp > llTimeStamp)
			{
				if(VO_MAXU64 != ullFilePos)
					return VO_TRUE;

				bFindNext = VO_FALSE;
			}
			else
			{
				if(0x10 == (btFlag & 0xF0))	//key frame
				{
					ullFilePos = m_pFileChunk->FGetFilePos() - 16;
					if(!bFindNext)
						return VO_TRUE;
				}
			}

			if(bFindNext)
			{
				ptr_skip(ft.dwDataSize - 1);
			}
			else
			{
				m_pFileChunk->FBack(16);
				m_pFileChunk->FBack(dwPrevTagSize + 4);
			}
		}
		else
		{
			if(bFindNext)
			{
				ptr_skip(ft.dwDataSize);
			}
			else
			{
				m_pFileChunk->FBack(15);
				m_pFileChunk->FBack(dwPrevTagSize + 4);
			}
		}
	}

	return VO_TRUE;
}

VO_BOOL CFlvDataParser::GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos)
{
	return m_pFileChunk->FRead2(*ppBuffer, nSize, nPos);
}
