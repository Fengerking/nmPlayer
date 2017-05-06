#include "CMKVVideoTrack.h"
#include "fCC.h"
#include "voLog.h"
#include "voVideoParser.h"
#include "fCodec.h"

typedef VO_S32 (*ptr_voGetVideoParserAPI)(VO_VIDEO_PARSERAPI * pParserHandle, VO_VIDEO_CODINGTYPE vType);

typedef struct tagMPEG2VIDEOINFO {
	VO_VIDEOINFOHEADER    hdr;
	VO_U32               dwStartTimeCode;   
	VO_U32               cbSequenceHeader;     
	VO_U32               dwProfile;     
	VO_U32               dwLevel;            
	VO_U32               dwFlags;            
	VO_U32               dwSequenceHeader[1];     
} MPEG2VIDEOINFO;

struct AVCSH
{
	VO_BYTE reserve1	: 8;
	VO_BYTE profile		: 8;
	VO_BYTE reserve2	: 8;
	VO_BYTE level		: 8;
	VO_BYTE reserve3	: 6;
	VO_BYTE s_NALU		: 2;
	VO_BYTE reserve4	: 3;
	VO_BYTE s_SPS		: 5;
};

//VO_VOID BigEndianLittleEndianExchange( VO_VOID * pData , VO_S32 DataSize );


CMKVVideoTrack::CMKVVideoTrack( VO_U8 btStreamNum, VO_U32 dwDuration, TracksTrackEntryStruct * pVideoInfo , VO_MEM_OPERATOR* pMemOp )
:CMKVTrack( VOTT_VIDEO , btStreamNum , dwDuration , pVideoInfo , pMemOp )
,m_pHeadData(VO_NULL)
,m_HeadSize(0)
,m_lastrelativetimecode(-1)
,m_bcontain_undetermined_keyframe(VO_FALSE)
,m_nExistThumnnailCnt(0)
{
}

CMKVVideoTrack::~CMKVVideoTrack()
{
	if( m_pHeadData )
		MemFree( m_pHeadData );
}

VO_BOOL CMKVVideoTrack::CheckHeadData(VO_PBYTE pHeadData, VO_U32 nSize, VO_U32 nCodec)
{
	VO_BOOL  nResult = VO_TRUE;
	if(pHeadData == NULL && nSize <= 0){
		return VO_FALSE;
	}

	if(VO_VIDEO_CodingMPEG4 == nCodec)
	{
			VO_S32 nRet = 0;
			VO_HANDLE hCodec = NULL;
			VO_VIDEO_PARSERAPI parserAPI ={0};
			VO_CODECBUFFER headData ={0};
			VO_CODECBUFFER parserBuf = {0};
			parserBuf.Buffer = pHeadData;
			parserBuf.Length = nSize;
			
#ifdef WIN32
			HMODULE m_hDll;
			m_hDll = LoadLibrary(_T("voVideoParser.dll"));
#ifndef WINCE
			ptr_voGetVideoParserAPI pEngAPI = (ptr_voGetVideoParserAPI) GetProcAddress (m_hDll, "voGetVideoParserAPI");
#else //WINCE
			ptr_voGetVideoParserAPI pEngAPI = (ptr_voGetVideoParserAPI) GetProcAddress (m_hDll, _T("voGetVideoParserAPI"));
#endif  //WINCE
			pEngAPI (&parserAPI, VO_VIDEO_CodingMPEG4);
#else			
			voGetVideoParserAPI(&parserAPI, VO_VIDEO_CodingMPEG4);
#endif
			parserAPI.Init(&hCodec);
			if(parserBuf.Length){
				nRet = parserAPI.Process(hCodec, &parserBuf);
			}
		
			if(nRet == VO_RETURN_SQHEADER || nRet == VO_RETURN_SQFMHEADER)
			{
		
				if(VO_ERR_NONE != parserAPI.GetParam(hCodec, VO_PID_VIDEOPARSER_HEADERDATA, &headData))
				{
					nResult = VO_FALSE;
				}
		
				if(VO_TRUE == nResult && memcmp(parserBuf.Buffer, headData.Buffer, headData.Length))
				{
					nResult = VO_FALSE;
				}
			}else{
				nResult = VO_FALSE;
			}
			parserAPI.Uninit(hCodec);

	}

	return nResult;
}


VO_U32 CMKVVideoTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	static VO_S32 is_firsttime = VO_TRUE;
	VO_U32 nCodec = 0;
	GetCodec(&nCodec);

	if(is_firsttime && !CheckHeadData((VO_PBYTE)m_pTrackInfo->pCodecPrivate, m_pTrackInfo->CodecPrivateSize, nCodec))
	{
		VO_SOURCE_SAMPLE sample;
		MemSet(&sample, 0, sizeof(VO_SOURCE_SAMPLE));
		if(VO_ERR_SOURCE_OK == GetFirstFrame(&sample))
		{
			VO_U32 dwCodec = 0;
			GetCodec(&dwCodec);
		
			VO_U32 dwSeqHeadPos = 0xFFFF;
			VO_U32 dwSeqHeadSize = 0;

			if(VO_VIDEO_CodingMPEG4 == nCodec){
				VO_S32 nRet = 0;
				VO_HANDLE hCodec;
				VO_VIDEO_PARSERAPI parserAPI;
				VO_CODECBUFFER headData ={0};
				VO_CODECBUFFER parserBuf = {0};
				parserBuf.Buffer = sample.Buffer;
				parserBuf.Length = sample.Size & 0x7FFFFFFF;
#ifdef WIN32
				HMODULE m_hDll;
				m_hDll = LoadLibrary(_T("voVideoParser.dll"));
#ifndef WINCE
				ptr_voGetVideoParserAPI pEngAPI = (ptr_voGetVideoParserAPI) GetProcAddress (m_hDll, "voGetVideoParserAPI");
#else //WINCE
				ptr_voGetVideoParserAPI pEngAPI = (ptr_voGetVideoParserAPI) GetProcAddress (m_hDll, _T("voGetVideoParserAPI"));
#endif  //WINCE
				pEngAPI (&parserAPI, VO_VIDEO_CodingMPEG4);
#else			
				voGetVideoParserAPI(&parserAPI, VO_VIDEO_CodingMPEG4);
#endif
				parserAPI.Init(&hCodec);
				if(parserBuf.Length){
					nRet = parserAPI.Process(hCodec, &parserBuf);
				}

				if(VO_RETURN_SQHEADER == nRet || VO_RETURN_SQFMHEADER == nRet)
				{
					if(VO_ERR_NONE == parserAPI.GetParam(hCodec, VO_PID_VIDEOPARSER_HEADERDATA, &headData)){
						m_HeadSize = headData.Length;
						m_pHeadData = NEW_BUFFER(m_HeadSize);
						if(m_pHeadData)
							MemCopy(m_pHeadData, headData.Buffer, m_HeadSize);
						
						pHeadData->Length = m_HeadSize;
						pHeadData->Buffer = m_pHeadData;
					}

				}
				parserAPI.Uninit(hCodec);

			}
			else if(VO_TRUE == GetVideoHeadDataFromBuffer(dwCodec, sample.Buffer, sample.Size & 0x7FFFFFFF, &dwSeqHeadPos, &dwSeqHeadSize))
			{
				if(dwSeqHeadPos != 0xFFFF)
				{					
					m_HeadSize = dwSeqHeadSize;
					m_pHeadData = NEW_BUFFER(m_HeadSize);
					if(m_pHeadData)
						MemCopy(m_pHeadData, sample.Buffer + dwSeqHeadPos, m_HeadSize);
					
					pHeadData->Length = m_HeadSize;
					pHeadData->Buffer = m_pHeadData;
				}
			}
		}

	}

	if( m_pHeadData )
	{
		pHeadData->Length = m_HeadSize;
		pHeadData->Buffer = m_pHeadData;
	}
	else if(m_pTrackInfo->pCodecPrivate)
	{
		pHeadData->Length = m_pTrackInfo->CodecPrivateSize;
		pHeadData->Buffer = (VO_PBYTE)m_pTrackInfo->pCodecPrivate;	
	}
	else
	{
		pHeadData->Length = 0;
		pHeadData->Buffer = NULL;
	}

	is_firsttime = VO_FALSE;

	return VO_ERR_SOURCE_OK;
}


VO_U32 CMKVVideoTrack::GetFirstFrame(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 result = 0;
	result = m_pSampleList->GetSampleByTime(0, pSample);

	return result;
 }

VO_U32 CMKVVideoTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	*pdwMaxSampleSize = (VO_U32)(m_pTrackInfo->PixelHeight * m_pTrackInfo->PixelWidth * 4. / 5.);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMKVVideoTrack::GetCodecCC(VO_U32* pCC)
{
	// http://wiki.multimedia.cx/index.php?title=Matroska
	/*
	V_DIRAC: Dirac [1]
	V_MS/VFW/FOURCC: VfW compatibility mode
	V_MPEG1: MPEG-1
	V_MPEG2: MPEG-2
	V_MPEG4/ISO/SP: MPEG-4 part 2 Simple Profile (divx4)
	V_MPEG4/ISO/ASP: MPEG-4 part 2 Advanced Simple Profile
	V_MPEG4/ISO/AP: MPEG-4 part 10 Advanced Profile (H.264)
	V_MPEG4/MS/V3: Divx3
	V_REAL/RV10: RealVideo 1
	V_REAL/RV20: RealVideo G2
	V_REAL/RV30: RealVideo 3
	V_REAL/RV40: RealVideo 4
	V_SNOW: FFmpeg Snow
	V_THEORA: Xiph Theora
	V_QUICKTIME: Quicktime compatiblity mode (mainly for SVQ3)
	V_UNCOMPRESSED: Uncompressed, format defined by KaxCodecColourSpace
	V_VC1: Deprecated; Use V_MS/VFW/FOURCC with WMV3 or WVC1 instead [2]
	V_VP8: VP8 [3]
	*/


	STRCODECID2FOURCC("V_MS/VFW/FOURCC", ((VO_BITMAPINFOHEADER *)m_pTrackInfo->pCodecPrivate)->biCompression);
	STRCODECID2FOURCC("V_MPEG1", FOURCC_MPG1);
	STRCODECID2FOURCC("V_MPEG2", FOURCC_MPG2);
	STRCODECID2FOURCC("V_VC1", FOURCC_WMV3);
	STRCODECID2FOURCC("V_MPEG4/MS/V3", FOURCC_DIV3);
	STRCODECID2FOURCC("V_MPEG4/ISO/SP", FOURCC_DIV4);
	STRCODECID2FOURCC("V_MPEG4/ISO/ASP", FOURCC_MP4V);
	STRCODECID2FOURCC("V_MPEG4/ISO/", FOURCC_AVC1);
	STRCODECID2FOURCC("V_REAL/RV10", FOURCC_REAL);
	STRCODECID2FOURCC("V_REAL/RV20", FOURCC_REAL);
	STRCODECID2FOURCC("V_REAL/RV30", FOURCC_REAL);
	STRCODECID2FOURCC("V_REAL/RV40", FOURCC_REAL);
	STRCODECID2FOURCC("V_VP8", FOURCC_VP80);
	STRCODECID2FOURCC("V_MJPEG", FOURCC_MJPG);
	
	return VO_ERR_SOURCE_FORMATUNSUPPORT;
}

VO_U32 CMKVVideoTrack::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	pVideoFormat->Height = m_pTrackInfo->PixelHeight;
	pVideoFormat->Width = m_pTrackInfo->PixelWidth;
	return VO_ERR_SOURCE_OK;
}
/*
VO_BOOL CMKVVideoTrack::NewFrameArrived( MKVFrame * pFrame )
{
	static VO_U32 flag = 0;
	if(1){//(flag ==1 ||(pFrame->TimeCode + pFrame->RelativeTimeCode) > 6000000){
		flag = 1;
//		VOLOGI("NewFrameArrived---time =%lld,TrackNumber=%d",(pFrame->TimeCode + pFrame->RelativeTimeCode), pFrame->TrackNumber);
	}

	if(0xFFFFFFFFFFFFFFFFULL != m_ullCurrParseFilePos)
	{
		if(pFrame->pFileChunk->FGetFilePos() < m_ullCurrParseFilePos)
		{
			pFrame->pFileChunk->FSkip((VO_U32)pFrame->framesize);
			return VO_FALSE;
		}
		else
			m_ullCurrParseFilePos = 0xFFFFFFFFFFFFFFFFULL;
	}

//	VO_S64 timecode = 0; 

	VO_U32 dwPosInBuffer = 0;

	if( m_pTrackInfo->ptr_ContentCompSettings )
	{
		VO_PBYTE buffer;
		dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite( &buffer , m_pTrackInfo->ContentCompSettingsSize );

		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;

		m_pGlobeBuffer->DirectWrite2( m_pTrackInfo->ptr_ContentCompSettings , buffer , 0 , m_pTrackInfo->ContentCompSettingsSize );

		m_pGlobeBuffer->Add( pFrame->pFileChunk, (VO_U32)pFrame->framesize );

		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;

		pFrame->framesize = pFrame->framesize + m_pTrackInfo->ContentCompSettingsSize;
	}
	else
	{
		dwPosInBuffer = m_pGlobeBuffer->Add( pFrame->pFileChunk, (VO_U32)pFrame->framesize );

		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;
	}

	//MKVMediaSampleIndexEntry* pNew = new MKVMediaSampleIndexEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | dwPosInBuffer) : dwPosInBuffer), (VO_U32)pFrame->framesize);
	MKVMediaSampleIndexEntry* pNew = ((CMKVStreamFileIndex*)m_pStreamFileIndex)->NewEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | dwPosInBuffer) : dwPosInBuffer), (VO_U32)pFrame->framesize);
	if(!pNew)
		return VO_FALSE;

	if( m_pTempFrameIndexListHead == NULL && m_pTempFrameIndexListTail == NULL )
	{
		m_pTempFrameIndexListHead = m_pTempFrameIndexListTail = pNew;
	}
	else
	{
		m_pTempFrameIndexListTail->next = pNew;
		pNew->pre = m_pTempFrameIndexListTail;
		m_pTempFrameIndexListTail = pNew;
	}

	if( pNew->cluster_timecode != -1 )
	{
#ifndef _HARDWARE_DECODER
		RerangeFrameTimeCode( pNew );
#endif
		m_FramesAvailable++;
	}

    m_bcontain_undetermined_keyframe = VO_TRUE;

	return VO_TRUE;
}

VO_VOID CMKVVideoTrack::RerangeFrameTimeCode( MKVMediaSampleIndexEntry * pEntry )
{
	VO_S64 timecode = pEntry->time_stamp;
	pEntry = (MKVMediaSampleIndexEntry *)pEntry->pre;

	while( pEntry )
	{
		if( pEntry->time_stamp <= timecode )
			break;
		else
		{
			pEntry->next->time_stamp = pEntry->time_stamp;
			pEntry->time_stamp = (VO_U32)timecode;
		}

		pEntry = (MKVMediaSampleIndexEntry *)pEntry->pre;
	}
}

VO_VOID CMKVVideoTrack::NewClusterStarted()
{
	;
}

VO_VOID CMKVVideoTrack::NewBlockStarted()
{
	m_BlocksInCache++;
}

VO_VOID CMKVVideoTrack::NewBlockGroupStarted()
{
	m_pTempFrameLastGroupPointer = m_pTempFrameIndexListTail;
    m_bcontain_undetermined_keyframe = VO_FALSE;
}

VO_VOID CMKVVideoTrack::NewReferenceBlockArrived()
{
	if( !m_pTempFrameLastGroupPointer )
	{
        if( m_bcontain_undetermined_keyframe )
        {
            MKVMediaSampleIndexEntry *pEntry = m_pTempFrameIndexListHead; 
            while( pEntry )
            {
                pEntry->SetKeyFrame( VO_FALSE );
                pEntry = (MKVMediaSampleIndexEntry *)pEntry->next;
            }
        }
		return;
	}

	m_pTempFrameLastGroupPointer = (MKVMediaSampleIndexEntry *)m_pTempFrameLastGroupPointer->next;

	while( m_pTempFrameLastGroupPointer )
	{
		m_pTempFrameLastGroupPointer->SetKeyFrame( VO_FALSE );

		m_pTempFrameLastGroupPointer = (MKVMediaSampleIndexEntry *)m_pTempFrameLastGroupPointer->next;
	}
}
VO_VOID CMKVVideoTrack::FileReadEnd()
{
	if( m_pTempFrameIndexListTail )
		m_pTempFrameIndexListTail->time_stamp = (VO_U32)(m_pSegInfo->Duration * m_pSegInfo->TimecodeScale / 1000000.);
	else
	{
		PBaseStreamMediaSampleIndexEntry FileIndex = NULL;
		if (m_pStreamFileIndex)
		{
			m_pStreamFileIndex->GetTail(&FileIndex);
			if(FileIndex)
				FileIndex->time_stamp = (VO_U32)(m_pSegInfo->Duration * m_pSegInfo->TimecodeScale / 1000000.);
		}
	}
}

VO_VOID CMKVVideoTrack::NewClusterTimeCodeArrived( VO_S64 timecode )
{
	MKVMediaSampleIndexEntry * pEntry = FindLastEntryWithoutClusterTimeCode();

	while( pEntry )
	{
		pEntry->set_clustertimecode( timecode );
#ifndef _HARDWARE_DECODER
		RerangeFrameTimeCode( pEntry );
#endif
		pEntry = (MKVMediaSampleIndexEntry *)pEntry->next;
	}
}

VO_VOID BigEndianLittleEndianExchange( VO_VOID * pData , VO_S32 DataSize )
{
	VO_PBYTE pBuffer = (VO_PBYTE)pData;

	for( VO_S32 i = 0 ; i < DataSize / 2 ; i++ )
	{
		VO_BYTE temp = pBuffer[i];
		pBuffer[i] = pBuffer[DataSize-i-1];
		pBuffer[DataSize-i-1] = temp;
	}
}
*/

VO_BOOL CMKVVideoTrack::SampleNotify(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam )
{
	return VO_TRUE;
}

VO_VOID CMKVVideoTrack::Flush()
{
	m_lastrelativetimecode = -1;
	m_bcontain_undetermined_keyframe = VO_FALSE;
	return CMKVTrack::Flush();
}
VO_U32 CMKVVideoTrack::GetFrameTime(VO_U32* pdwFrameTime)
{
	if(pdwFrameTime)
	{
		if(m_pTrackInfo->DefaultDuration != 0)
			*pdwFrameTime = (VO_U32)(m_pTrackInfo->DefaultDuration/10000);
		else
		{
			VO_U32 dwCurrCount = m_pSampleList->GetSampleCount();
			if(dwCurrCount >= 2)
			{
				VO_SOURCE_SAMPLE FirstSample;
				VO_SOURCE_SAMPLE LastSample;
				memset(&FirstSample, 0x0, sizeof(VO_SOURCE_SAMPLE));
				memset(&LastSample, 0x0, sizeof(VO_SOURCE_SAMPLE));
				
				m_pSampleList->GetSampleByIndex(0, &FirstSample);
				m_pSampleList->GetSampleByIndex(0, &LastSample);
				
				*pdwFrameTime = (VO_U32)((LastSample.Time - FirstSample.Time)/(dwCurrCount -1));
			}
			else
			{
				return VO_ERR_NOT_IMPLEMENT;
			}
		}
	}
	return VO_ERR_SOURCE_OK;
}
//VO_U32 CMKVVideoTrack::SetPosN(VO_S64* pPos)
//{
//	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
//	VO_S32 nRes = m_pStreamFileIndex->GetEntryByTime(m_bVideo, *pPos, &pGet);
//	if(-1 == nRes)
//		return VO_ERR_SOURCE_END;
//	else if(1 == nRes)	//retry
//	{
//		VO_BOOL bParseEnd = VO_FALSE;
//		while(1 == nRes && !bParseEnd)	//retry!!
//		{
//			m_pStreamFileIndex->RemoveAll();
//
//			if( m_pGlobeBuffer )
//				m_pGlobeBuffer->RemoveTo( (pGet->pos_in_buffer & 0x7fffffff) + pGet->size );
//
//			if(!TrackGenerateIndex())
//				bParseEnd = VO_TRUE;
//
//			nRes = m_pStreamFileIndex->GetEntryByTime(m_bVideo, *pPos, &pGet);
//			if(-1 == nRes)
//				return VO_ERR_SOURCE_END;
//		}
//
//		if(0 != nRes)
//			return VO_ERR_SOURCE_END;
//
//		*pPos = pGet->time_stamp;
//		m_pStreamFileIndex->RemoveUntil(pGet);
//
//		TrackGenerateIndex();
//	}
//	else	//ok
//	{
//		*pPos = pGet->time_stamp;
//		m_pStreamFileIndex->RemoveUntil(pGet);
//
//		m_pStreamFileReader->FileGenerateIndex();
//	}
//
//	return VO_ERR_SOURCE_OK;
//}

VO_U32 CMKVVideoTrack::GetBitmapInfoHeader(VO_BITMAPINFOHEADER** ppBitmapInfoHeader)
{
	*ppBitmapInfoHeader = (VO_BITMAPINFOHEADER *)m_pTrackInfo->pCodecPrivate;
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMKVVideoTrack::GetMKVProfileLevel_Video(VO_CODECBUFFER *pCodecBuf, VO_VIDEO_PROFILELEVEL *pVideoProfileLevel, VO_VIDEO_CODINGTYPE nCodecType)
{
	VO_S32 nRet = VO_VIDEO_PARSER_ERR_BASE;
	VO_U32 nResult = VO_ERR_SOURCE_ERRORDATA;
	VO_HANDLE hCodec = NULL;
	VO_VIDEO_PARSERAPI parserAPI ={0};
	
#ifdef WIN32
	HMODULE m_hDll;
	m_hDll = LoadLibrary(_T("voVideoParser.dll"));
#ifndef WINCE
	ptr_voGetVideoParserAPI pEngAPI = (ptr_voGetVideoParserAPI) GetProcAddress (m_hDll, "voGetVideoParserAPI");
#else //WINCE
	ptr_voGetVideoParserAPI pEngAPI = (ptr_voGetVideoParserAPI) GetProcAddress (m_hDll, _T("voGetVideoParserAPI"));
#endif  //WINCE
	pEngAPI (&parserAPI, nCodecType);
#else			
	voGetVideoParserAPI(&parserAPI, nCodecType);
#endif
	parserAPI.Init(&hCodec);
	if(pCodecBuf->Length){
		nRet = parserAPI.Process(hCodec, pCodecBuf);
	}

	if(nRet == VO_RETURN_SQHEADER || nRet == VO_RETURN_SQFMHEADER)
	{
		if(VO_ERR_NONE == parserAPI.GetParam(hCodec, VO_PID_VIDEOPARSER_PROFILE, &pVideoProfileLevel->Profile)){
			nResult = VO_ERR_SOURCE_OK;
		}
		
		if(VO_ERR_NONE == parserAPI.GetParam(hCodec, VO_PID_VIDEOPARSER_LEVEL, &pVideoProfileLevel->Level)){
			nResult = VO_ERR_SOURCE_OK;
		}
	}
	
	parserAPI.Uninit(hCodec);

	return nResult;
}

VO_U32 CMKVVideoTrack::GetMKVVideoProfileLevel(VO_VIDEO_PROFILELEVEL* pInfo)
{
	pInfo->Profile = VO_VIDEO_PROFILETYPE_MAX;
	pInfo->Level	= VO_VIDEO_LEVELTYPE_MAX;

	VO_U32 uFourcc = 0;
	VO_U32 nRes = GetCodecCC(&uFourcc);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	switch(uFourcc)//DivX
	{
	case FOURCC_DIV3:
	case FOURCC_DIV4:
		{
			pInfo->Profile	= VO_VIDEO_DivX311;
			pInfo->Level	= VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}
	case FOURCC_DIVX:
	case FOURCC_divx:
		{
			pInfo->Profile	= VO_VIDEO_DivX4;
			pInfo->Level	= VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}
	case FOURCC_DIV5:
	case FOURCC_DX50:
		{
			pInfo->Profile	= VO_VIDEO_DivX5;
			pInfo->Level	= VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}
	case FOURCC_DIV6:
		{
			pInfo->Profile	= VO_VIDEO_DivX6;
			pInfo->Level	= VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}
	case FOURCC_xvid:
		{
			return VO_ERR_INVALID_ARG;
		}
	}

	switch ( fCodecGetVideoCodec(uFourcc) )
	{
	case VO_VIDEO_CodingMPEG4:
		{
			VO_CODECBUFFER codecbuf = {0};
			nRes = GetHeadData(&codecbuf);
			if(VO_ERR_SOURCE_OK == nRes && codecbuf.Length) {
				if (GetMKVProfileLevel_Video(&codecbuf, pInfo, VO_VIDEO_CodingMPEG4) == 0)
					return VO_ERR_SOURCE_OK;
			}

			VO_SOURCE_SAMPLE sample = {0};
			nRes = GetFirstFrame(&sample);
			if (VO_ERR_NONE != nRes || 0 == sample.Size)
				return nRes ? nRes : VO_ERR_BASE;

			codecbuf.Buffer = sample.Buffer;
			codecbuf.Length = sample.Size;

			return GetMKVProfileLevel_Video(&codecbuf, pInfo, VO_VIDEO_CodingMPEG4);
		}
	case VO_VIDEO_CodingH264:
		{
			VO_CODECBUFFER codecbuf = {0};
			nRes = GetHeadData(&codecbuf);
			if(VO_ERR_SOURCE_OK == nRes &&	codecbuf.Length) {
				if (GetMKVProfileLevel_Video(&codecbuf, pInfo, VO_VIDEO_CodingH264) == 0)
					return VO_ERR_SOURCE_OK;
			}

			VO_SOURCE_SAMPLE sample = {0};
			nRes = GetFirstFrame(&sample);
			if (VO_ERR_NONE != nRes || 0 == sample.Size)
				return nRes ? nRes : VO_ERR_BASE;

			codecbuf.Buffer = sample.Buffer;
			codecbuf.Length = sample.Size & 0x7fffffff;

			return GetMKVProfileLevel_Video(&codecbuf, pInfo, VO_VIDEO_CodingH264);
		}
	case VO_VIDEO_CodingWMV:
		{
			if(FOURCC_WMV1 == uFourcc)
				pInfo->Profile = VO_VIDEO_WMVFormat7;
			else if(FOURCC_WMV2 == uFourcc)
				pInfo->Profile = VO_VIDEO_WMVFormat8;
			else
				pInfo->Profile = VO_VIDEO_PROFILETYPE_MAX;
			pInfo->Level = VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}
	case VO_VIDEO_CodingVC1:
		{
			if(FOURCC_WMV3 == uFourcc)
				pInfo->Profile = VO_VIDEO_WMVFormat9;
			else if(FOURCC_WVC1 == uFourcc)
				pInfo->Profile = VO_VIDEO_WMVFormatWVC1;
			else
				pInfo->Profile = VO_VIDEO_PROFILETYPE_MAX;

			pInfo->Level = VO_VIDEO_LEVELTYPE_MAX;

			return VO_ERR_SOURCE_OK;
		}

	case VO_VIDEO_CodingDIVX:
		//break;
	case VO_VIDEO_CodingH263:
		// TODO implement them
	case VO_VIDEO_CodingMPEG2:
	case VO_VIDEO_CodingS263:
	case VO_VIDEO_CodingRV:
	case VO_VIDEO_CodingMJPEG:
	case VO_VIDEO_CodingVP6:
	case VO_VIDEO_CodingVP8:
	default:
		return VO_ERR_NOT_IMPLEMENT;
	}
}



VO_U32 CMKVVideoTrack::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	VO_U32 result = 0;

	switch (uID)
	{
	case VO_PID_VIDEO_UPSIDEDOWN:
		{
			VO_U32 fourcc = 0;
			VO_U32 nRes = GetCodecCC(&fourcc);
			if(VO_ERR_SOURCE_OK != nRes)
				return nRes;

			if (FOURCC_VP60 == fourcc || FOURCC_VP61 == fourcc || FOURCC_VP6F == fourcc || FOURCC_VP62 == fourcc)
				*( (VO_BOOL*)pParam ) = VO_TRUE;
			else
				*( (VO_BOOL*)pParam ) = VO_FALSE;

			result = VO_ERR_SOURCE_OK;
		}
		break;
		
	case VO_PID_SOURCE_VIDEOPROFILELEVEL:
		{
			VO_VIDEO_PROFILELEVEL* pInfo = (VO_VIDEO_PROFILELEVEL*)pParam;
			result = GetMKVVideoProfileLevel(pInfo);
		}
		break;
		
	default:
		result = CMKVTrack::GetParameter(uID, pParam);
	}

	return result;
}
