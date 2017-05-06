#include "CRealReader.h"
#include "CRealVideoTrack.h"
#include "voRealVideo.h"

#define LOG_TAG "CRealVideoTrack.cpp"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define CHECK_VORV_FORMAT_INFO	if(!m_pInitParam)\
	return VO_ERR_OUTOF_MEMORY;\
	VORV_FORMAT_INFO* pFormat = (VORV_FORMAT_INFO*)m_pInitParam;

void dumpdata(VO_PBYTE data, VO_U32	size)
{
	static int index = 0;
	FILE *input;
	char filename[64];
	sprintf(filename, "d:\\media\\b\\input%d.rmvb", index++);
	input = fopen(filename,"ab");
	fwrite(data, size,1,input);
	fclose(input);
}

CRealVideoTrack::CRealVideoTrack(PRealTrackInfo pTrackInfo, CRealReader *pReader, VO_MEM_OPERATOR* pMemOp)
	: CRealTrack(VOTT_VIDEO, pTrackInfo, pReader, pMemOp)
	, m_dwLastTimeStamp(VO_MAXU32)
	, m_bFirstFrame(VO_TRUE)
	, m_pTempBufHeader(VO_NULL)
	, m_dwPayloadIndex(0)
	, m_dwPayloadOffset(0)
	, m_nMaxEncoderFrameSize(-1)
	, m_pTempEntry(VO_NULL)
	, m_pUnvalidTimeStampEntries(VO_NULL)
	, m_pLastUnvalidTimeStampEntry(VO_NULL)
	, m_wUnvalidTimeStampEntries(0)
{
	ParseExtData(pTrackInfo->pPropBuffer, pTrackInfo->dwPropBufferSize);
	m_TmpFrame.bLastPacket = VO_FALSE;
	m_TmpFrame.usSequenceNum = 0;
}

CRealVideoTrack::~CRealVideoTrack()
{
	if(m_pTempEntry)
	{
		m_pStreamFileIndex->DeleteEntry(m_pTempEntry);
		m_pTempEntry = VO_NULL;
	}

	PBaseStreamMediaSampleIndexEntry pCur = m_pUnvalidTimeStampEntries;
	PBaseStreamMediaSampleIndexEntry pTmp = VO_NULL;
	while(pCur)
	{
		pTmp = pCur;
		pCur = pCur->next;

		m_pStreamFileIndex->DeleteEntry(pTmp);
	}
	m_pUnvalidTimeStampEntries = VO_NULL;
	m_pLastUnvalidTimeStampEntry = VO_NULL;
	m_wUnvalidTimeStampEntries = 0;
}


VO_U32 CRealVideoTrack::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	VO_U32 rc = VO_ERR_SOURCE_OK;

	//for get thumbnail by parameter, we should process it specially
	if(m_IsThumbNail && VO_PID_SOURCE_GETTHUMBNAIL == uID)
	{
		//for thumbnail, we should tell data parser new position and generate sample from the position
		if( m_pReader )
		{
			VO_U64 llfilepos = 0;
			VO_S32 lsize = 0;
			VO_BOOL bret = VO_FALSE;
			//1, get the key frame index info , and retrieve the max size index file pos
			bret = m_pReader->GetDataParser()->GetMaxSizeIframeIndex( llfilepos , lsize );
			if( bret )
			{
				//2,set file i/o position
				m_pReader->GetDataParser()->SetFileIOPos( llfilepos );

				//3,tell data parser pos and size info
				m_pReader->GetDataParser()->SetStepBRangeForthumbnail( lsize );
			}

			m_pReader->Flush();

			//4, tell reader to generate index
			m_pReader->FileGenerateIndex();

			rc = GetSampleN( (VO_SOURCE_SAMPLE *) pParam );
			//if the return is not ok, then we should not try to use the pParam value
			if( rc != VO_ERR_SOURCE_OK)
			{
				return rc;
			}
		}
		else
		{
			return VO_ERR_SOURCE_OPENFAIL;
		}
	}
	else
	{
		rc = CRealTrack::GetParameter(uID , pParam);
		//if the return is not ok, then we should not try to use the pParam value
		if( rc != VO_ERR_SOURCE_OK)
		{
			return rc;
		}
	}
	
	switch(uID)
	{
	case VO_PID_SOURCE_GETTHUMBNAIL:
		{
#ifndef _HARDWARE_DECODER
			VO_SOURCE_SAMPLE * pSample = (VO_SOURCE_SAMPLE*)pParam;
			rv_frame* pFrame = (rv_frame*)pSample->Buffer;
			pFrame->ulTimestamp = (VO_U32)pSample->Time;
			pFrame->pSegment = (rv_segment*)(pSample->Buffer + sizeof(rv_frame));
			pFrame->pData = pSample->Buffer + sizeof(rv_frame) + pFrame->ulNumSegments * sizeof(rv_segment);

			pSample->Size = (sizeof(rv_frame) + pFrame->ulNumSegments * sizeof(rv_segment) + pFrame->ulDataLen) | (pSample->Size & 0x80000000);
#else
			VO_SOURCE_SAMPLE * pSample = (VO_SOURCE_SAMPLE*)pParam;
			rv_frame* pFrame = (rv_frame*)pSample->Buffer;

			pSample->Size = (1 + pFrame->ulNumSegments * sizeof(rv_segment) + pFrame->ulDataLen) | (pSample->Size & 0x80000000);
			pSample->Buffer = pSample->Buffer + sizeof(rv_frame) - 1;
			pSample->Buffer[0] = (VO_U8)(pFrame->ulNumSegments - 1);
#endif
		}
		break;
	case VO_PID_VIDEO_MAXENCODERFRAMESIZE:
		{
			*((int*)pParam) = m_nMaxEncoderFrameSize;
		}
		break;
	default:
		break;
	}

	return rc;
}

VO_U32 CRealVideoTrack::GetSample(VO_SOURCE_SAMPLE* pSample)
{	
	VO_U32 rc = CRealTrack::GetSample(pSample);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	if( pSample->Buffer && pSample->Size > 0)
	{
#ifndef _HARDWARE_DECODER
		rv_frame* pFrame = (rv_frame*)pSample->Buffer;
		pFrame->ulTimestamp = (VO_U32)pSample->Time;
		pFrame->pSegment = (rv_segment*)(pSample->Buffer + sizeof(rv_frame));
		pFrame->pData = pSample->Buffer + sizeof(rv_frame) + pFrame->ulNumSegments * sizeof(rv_segment);

		pSample->Size = (sizeof(rv_frame) + pFrame->ulNumSegments * sizeof(rv_segment) + pFrame->ulDataLen) | (pSample->Size & 0x80000000);

		//if( pSample )
		//{
		//	VOLOGI("[3] pbuffer: 0x%08x , duration: %d , flag: %d , size: %d time: %lld" , pSample->Buffer , pSample->Duration , pSample->Flag , pSample->Size , pSample->Time);
		//}
#else
		rv_frame* pFrame = (rv_frame*)pSample->Buffer;

		//the size should contain 1 more byte, in order to store the segment number.
		pSample->Size = (1 + pFrame->ulNumSegments * sizeof(rv_segment) + pFrame->ulDataLen) | (pSample->Size & 0x80000000);

		//in order to store one more byte , so go back 1 byte.
		pSample->Buffer = pSample->Buffer + sizeof(rv_frame) -1;
		pSample->Buffer[0] = (VO_U8)(pFrame->ulNumSegments - 1);
#endif
		//	dumpdata(pSample->Buffer, pSample->Size & 0X7FFFFFFF);

#if 0
		dumpdata(pSample->Buffer, pSample->Size & 0X7FFFFFFF);
#endif
	}
	else
	{
		pSample->Size = 0;
		pSample->Buffer = 0;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U64 CRealVideoTrack::GetFilePosByFileIndex(VO_U32 dwFileIndex)
{
	return FileIndexGetFilePosByIndex(dwFileIndex);
}

VO_U32 CRealVideoTrack::GetFileIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp)
{
	return FileIndexGetIndexByTime(bForward, llTimeStamp);
}

VO_VOID CRealVideoTrack::Flush()
{
	if(m_pTempEntry)
	{
		m_pStreamFileIndex->DeleteEntry(m_pTempEntry);
		m_pTempEntry = VO_NULL;
	}

	PBaseStreamMediaSampleIndexEntry pCur = m_pUnvalidTimeStampEntries;
	PBaseStreamMediaSampleIndexEntry pTmp = VO_NULL;
	while(pCur)
	{
		pTmp = pCur;
		pCur = pCur->next;

		m_pStreamFileIndex->DeleteEntry(pTmp);
	}
	m_pUnvalidTimeStampEntries = VO_NULL;
	m_pLastUnvalidTimeStampEntry = VO_NULL;
	m_wUnvalidTimeStampEntries = 0;

	m_dwLastTimeStamp = VO_MAXU32;
	m_bFirstFrame = VO_TRUE;

	CRealTrack::Flush();
}

VO_U32 CRealVideoTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	CHECK_VORV_FORMAT_INFO

	if(pdwMaxSampleSize)
		*pdwMaxSampleSize = VO_U32(pFormat->usWidth) * pFormat->usHeight * 3 / 4;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealVideoTrack::GetCodec(VO_U32* pCodec)
{
	*pCodec = VO_VIDEO_CodingRV;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealVideoTrack::GetCodecCC(VO_U32* pCC)
{
	CHECK_VORV_FORMAT_INFO

	*pCC = pFormat->ulSubMOFTag;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealVideoTrack::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	CHECK_VORV_FORMAT_INFO

	pVideoFormat->Width = pFormat->usWidth;
	pVideoFormat->Height = pFormat->usHeight;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealVideoTrack::GetFrameTime(VO_U32* pdwFrameTime)
{
	CHECK_VORV_FORMAT_INFO

	if(!(pFormat->ufFramesPerSecond & 0xFFFF0000))
		return VO_ERR_NOT_IMPLEMENT;

	if(pdwFrameTime)
		*pdwFrameTime = 100000 / ((pFormat->ufFramesPerSecond & 0xFFFF0000) >> 16);

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CRealVideoTrack::ParseExtData(VO_PBYTE pPropBuffer, VO_U32 dwPropBufferSize)
{
	if(pPropBuffer && dwPropBufferSize >= 4)
	{
		VO_PBYTE pSrc = pPropBuffer;
		
		use_big_endian_read

		VO_U32 dwValue;
		read_fcc2(dwValue);
		if(FOURCC_MLTI == dwValue)
		{
			//TODO: process multi-stream header
			return VO_FALSE;
		}
		
		if(dwPropBufferSize >= 26)
		{
			pSrc = pPropBuffer;
			read_dword2(dwValue);

			m_dwInitParamSize = sizeof(VORV_FORMAT_INFO) + dwValue - 26;
			m_pInitParam = NEW_BUFFER(m_dwInitParamSize);
			if(!m_pInitParam)
			{
				return VO_FALSE;
			}

			VORV_FORMAT_INFO* pFormat = (VORV_FORMAT_INFO*)m_pInitParam;
			pFormat->pOpaqueData = VO_PBYTE(pFormat + 1);

			//single substream header length
			pFormat->ulLength = dwValue;

			//media object format, This FourCC field is either 0x5649444F (¡°VIDO¡±) or 0x494D4147 (¡°IMAG¡±).
			read_dword2(pFormat->ulMOFTag);

			//A FourCC that identifies the codec used in this substream.
			//For RealVideo 1, codec_id is 0x52563130 (¡°RV10¡±).
			//For RealVideo G2, codec_id is 0x52563230 (¡°RV20¡±). 
			//For RealVideo 8,codec_id is 0x52563330 (¡°RV30¡±).
			//For RealVideo 9 and 10, codec_id is 0x52563430 (¡°RV40¡±).
			read_dword2(pFormat->ulSubMOFTag);

			if(HX_RVTRVIDEO_ID == pFormat->ulSubMOFTag)
			{
				pFormat->ulSubMOFTag = HX_RV20VIDEO_ID;
			}
			else if(HX_RVTR_RV30_ID == pFormat->ulSubMOFTag)
			{
				pFormat->ulSubMOFTag = HX_RV30VIDEO_ID;
			}

			//Width, in pixels, of a video frame.
			read_word2(pFormat->usWidth);

			//Height, in pixels, of a video frame.
			read_word2(pFormat->usHeight);

			//Number of bits per pixel.
			read_word2(pFormat->usBitCount);

			//Padded width, in pixels, of a video frame. In some cases, codecs need the frame width to be a
			//multiple of a certain number, and therefore the frame width must be padded.
			read_word2(pFormat->usPadWidth);

			//Padded height, in pixels, of a video frame. In some cases, codecs need the frame height to be a
			//multiple of a certain number, and therefore the frame height must be padded.
			read_word2(pFormat->usPadHeight);

			//Frame rate of the video. This is a 32-bit fixed-point value in which the upper 16 bits are the integer
			//part and the lower 16 bits are the fractional part.
			read_dword2(pFormat->ufFramesPerSecond);

			pFormat->ulOpaqueDataSize = pFormat->ulLength - 26;
			if(dwPropBufferSize - (pSrc - pPropBuffer) >= pFormat->ulOpaqueDataSize)
			{
				read_pointer2(pFormat->pOpaqueData, pFormat->ulOpaqueDataSize);
				VO_S32 FrameSize[9] = {0,1,1,2,2,3,3,3,3};
				if( HX_RV30VIDEO_ID == pFormat->ulSubMOFTag )
				{
					VO_S32 numFrameSizes = 1 + ( pFormat->pOpaqueData[1] & 0x7 );
					if(numFrameSizes >= 0 && numFrameSizes < (VO_S32)sizeof(FrameSize))
					{
						m_nMaxEncoderFrameSize = FrameSize[numFrameSizes];
					}
				}
			}

			return VO_TRUE;
		}
	}

	return VO_FALSE;
}

VO_BOOL CRealVideoTrack::OnPacket(CGFileChunk* m_pFileChunk, VO_U8 btFlags, VO_U16 wLen, VO_U32 dwTimeStamp)
{
	//if current content has been parsed, only skipped
	//East 2009/12/07
	if((VO_U64)-1 != m_ullCurrParseFilePos)
	{
		if(m_pFileChunk->FGetFilePos() < m_ullCurrParseFilePos)
		{
			return m_pFileChunk->FSkip(wLen);
		}
		else
		{
			m_ullCurrParseFilePos = -1;
		}
	}

	m_pFileChunk->FStartRecord();

	use_big_endian_read

	VO_U8 btHeader = 0;
	ptr_read_byte(btHeader);
	//frame_type
	VO_U8 btFrameType = (btHeader & 0xC0) >> 6;	//2bit
	switch(btFrameType)
	{
	case RM_FRAMETYPE_PARTIAL:
	case RM_FRAMETYPE_LAST_PARTIAL:
		{
			//num_packets: The number of partial-frame packets into which the RealVideo frame is broken.
			VO_U8 btPayloadNum = (btHeader & 0x3F) << 1;
			ptr_read_byte(btHeader);
			btPayloadNum += (btHeader & 0x80) >> 7;

			//packet_num: The index within the set of partial-frame packets for this RealVideo frame. This number is 1-based,
			//so possible values are in the range [1,num_packets].
			//BYTE bPacketNum = btHeader & 0x7F;						//7bit

			//confirm first complete entry!!
			//East 2010/06/07
			if(m_bFirstFrame)
			{
				if((btHeader & 0x7F) != 1)
				{
					ptr_skip(wLen - m_pFileChunk->FGetLastRead());

					return VO_TRUE;
				}
				else
				{
					m_bFirstFrame = VO_FALSE;
				}
			}

			//broken_up_by_us: Flag that indicates the encoder broke up the frame into partial frames
			//This is usually set to 1 for partial frames and last-partial frames headers.
			VO_U16 wHeader = 0;
			ptr_read_word(wHeader);

			//BYTE bBrokenUpByUs = (wHeader & 0x8000) >> 15;			//1bit

			//entire_frame_size_flag: If this flag is 1, then the entire_frame_size field is 14 bits long. 
			//Otherwise, the entire_frame_size field is 30 bits long.
			VO_U8 btTypeFlag = (wHeader & 0x4000) >> 14;	//1bit
			VO_U32 dwFrameSize = 0;
			if(btTypeFlag)
			{
				//entire_frame_size:In partial frame packets, this field specifies the size, in bytes, of the entire frame from which this
				//partial frame packet was taken.
				dwFrameSize = wHeader & 0x3FFF;
			}
			else
			{
				//entire_frame_size
				dwFrameSize = (wHeader & 0x3FFF) << 16;
				ptr_read_word(wHeader);
				dwFrameSize += wHeader;
			}

			ptr_read_word(wHeader);
			//first bit is reserved
			//partial_frame_offset_flag
			btTypeFlag = (wHeader & 0x4000) >> 14;
			VO_U32 dwPartialFrameOffset = 0;
			if(btTypeFlag)
			{
				//partial_frame_offset
				dwPartialFrameOffset = (wHeader & 0x3FFF);
			}
			else
			{
				//partial_frame_offset:In partial frame packets, this field specifies the offset within the entire frame at which this partial
				//frame is located.
				dwPartialFrameOffset = (wHeader & 0x3FFF) << 16;
				ptr_read_word(wHeader);
				dwPartialFrameOffset += wHeader;
			}

			//skip sequence number
			//sequence_number:An 8-bit number that specifies a sequence index for each frame. Each successive frame in the
			//bitstream has a sequence number that is incremented by one from the last frame. When the
			//sequence number reaches 255, it wraps back around to 0.
			ptr_skip(1);
			//following is frame data
			if(RM_FRAMETYPE_LAST_PARTIAL == btFrameType)
			{
				if(!AddPayloadInfo(m_pFileChunk, btFlags, btPayloadNum, dwPartialFrameOffset, dwTimeStamp))
					return VO_FALSE;

				while(m_pFileChunk->FGetLastRead() < wLen)
				{
					//read_byte(btHeader);
					ptr_skip(1);
					if(!ProcessMultiFrame(m_pFileChunk, btFlags & ~RM_KEYFRAME_FLAG))
						return VO_FALSE;
				}
			}
			else
			{
				if(!AddPayloadInfo(m_pFileChunk, btFlags, btPayloadNum, (VO_U32)(wLen - m_pFileChunk->FGetLastRead()), dwTimeStamp, VO_FALSE))
					return VO_FALSE;
			}

		}
		break;

	case RM_FRAMETYPE_WHOLE:
		{
			//skip sequence number
			ptr_skip(1);

			if(!AddPayloadInfo(m_pFileChunk, btFlags, 1, (VO_U32)(wLen - m_pFileChunk->FGetLastRead()), dwTimeStamp))
				return VO_FALSE;
		}
		break;

	case RM_FRAMETYPE_MULTIPLE:
		{
			if(!ProcessMultiFrame(m_pFileChunk, btFlags))
				return VO_FALSE;

			while(m_pFileChunk->FGetLastRead() < wLen)
			{
				//read_byte(btHeader);
				//frame_type skip
				ptr_skip(1);
				if(!ProcessMultiFrame(m_pFileChunk, btFlags & ~RM_KEYFRAME_FLAG))
					return VO_FALSE;
			}
		}
		break;

	default:
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CRealVideoTrack::ProcessMultiFrame(CGFileChunk* m_pFileChunk, VO_U8 btFlags)
{
	use_big_endian_read

	VO_U16 wHeader;
	ptr_read_word(wHeader);
	VO_U32 dwFrameSize = 0;
	//frame_size_flag
	if(wHeader & 0x4000)
	{
		//frame_size
		dwFrameSize = wHeader & 0x3FFF;
	}
	else
	{
		//frame_size
		dwFrameSize = (wHeader & 0x3FFF) << 16;
		ptr_read_word(wHeader);
		dwFrameSize += wHeader;
	}

	ptr_read_word(wHeader);
	VO_U32 dwTimeStamp = 0;
	//timestamp_flag
	if(wHeader & 0x4000)
	{
		//timestamp
		dwTimeStamp = wHeader & 0x3FFF;
	}
	else
	{
		//timestamp
		dwTimeStamp = (wHeader & 0x3FFF) << 16;
		ptr_read_word(wHeader);
		dwTimeStamp += wHeader;
	}

	//skip sequence number
	ptr_skip(1);

	if(!AddPayloadInfo(m_pFileChunk, btFlags, 1, dwFrameSize, dwTimeStamp))
		return VO_FALSE;

	return VO_TRUE;
}

/*
Data Structure:
rv_frame
some rv_segment
frame data

Note:
for avoid data misalignment, frame data must keep multiple of four!!
*/
VO_BOOL CRealVideoTrack::AddPayloadInfo(CGFileChunk* pFileChunk, VO_U8 btFlags, VO_U8 btPayloadNum, VO_U32 dwLen, VO_U32 dwTimeStamp, VO_BOOL bLastPayload /* = VO_TRUE */)
{
	if(!m_bInUsed)
		return pFileChunk->FSkip(dwLen);

	if(m_pTempEntry)
	{
		if(!m_pGlobeBuffer->HasIdleBuffer(dwLen))	//buffer lack
			return VO_FALSE;

		VO_U32 dwPosInBuffer = m_pGlobeBuffer->Add(pFileChunk, dwLen);
		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;

		m_pTempEntry->Add(dwLen);
	}
	else		//new sample
	{
		m_dwPayloadIndex = 0;
		m_dwPayloadOffset = 0;

		VO_U32 dwHeaderLen = sizeof(rv_frame) + VO_U32(btPayloadNum) * sizeof(rv_segment);

		if(!m_pGlobeBuffer->HasIdleBuffer(dwLen + dwHeaderLen))	//buffer lack
			return VO_FALSE;

		VO_U32 dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite(&m_pTempBufHeader, dwLen + dwHeaderLen);

		m_TmpFrame.ulDataLen = 0;
		m_TmpFrame.usFlags = btFlags & ~RM_KEYFRAME_FLAG;
		m_TmpFrame.ulNumSegments = btPayloadNum;

		if(!m_pGlobeBuffer->DirectWrite(pFileChunk, m_pTempBufHeader, dwHeaderLen, dwLen))
			return VO_FALSE;

		m_pTempEntry = m_pStreamFileIndex->NewEntry(dwTimeStamp, ((btFlags & RM_KEYFRAME_FLAG) ? (0x80000000 | dwPosInBuffer) : dwPosInBuffer), dwLen + dwHeaderLen);
		if(!m_pTempEntry)
			return VO_FALSE;
	}


	m_TmpFrame.ulDataLen += dwLen;

	rv_segment rs;
	rs.bIsValid = VO_TRUE;
	rs.ulOffset = m_dwPayloadOffset;

	m_pGlobeBuffer->DirectWrite2((VO_PBYTE)&rs, m_pTempBufHeader, sizeof(rv_frame) + m_dwPayloadIndex * sizeof(rv_segment), sizeof(rv_segment));

	m_dwPayloadIndex++;
	m_dwPayloadOffset += dwLen;

	if(m_dwPayloadIndex >= btPayloadNum || bLastPayload)	//last payload
	{
		VO_U8 btPadding = m_TmpFrame.ulDataLen & 0x3;
		if(btPadding)	//do padding data to multiple of four
		{
			btPadding = 4 - btPadding;

			if(!m_pGlobeBuffer->HasIdleBuffer(btPadding))	//buffer lack
				return VO_FALSE;

			m_pGlobeBuffer->Use(btPadding);
			m_pTempEntry->Add(btPadding);
		}

		m_pGlobeBuffer->DirectWrite2((VO_PBYTE)&m_TmpFrame, m_pTempBufHeader, 0, sizeof(rv_frame));

		if(VO_MAXU32 == m_dwLastTimeStamp || m_pTempEntry->time_stamp > m_dwLastTimeStamp + 1)
		{
			if(m_pUnvalidTimeStampEntries)
				AddUnvalidTimeStampEntryList(m_pTempEntry->time_stamp);
			m_pStreamFileIndex->Add(m_pTempEntry);
		}
		else
			AddToUnvalidTimeStampEntryList(m_pTempEntry);

		m_dwLastTimeStamp = m_pTempEntry->time_stamp;

		m_pTempEntry = VO_NULL;
	}

	return VO_TRUE;
}

VO_BOOL CRealVideoTrack::SetGlobalBufferExtSize(VO_U32 dwExtSize)
{
	m_dwGBExtSize = sizeof(rv_frame) + sizeof(rv_segment) * VO_MAXU8 + ((dwExtSize + 0x3) & ~0x3);

	return VO_TRUE;
}

VO_VOID CRealVideoTrack::AddToUnvalidTimeStampEntryList(PBaseStreamMediaSampleIndexEntry pEntry)
{
	if(!m_pLastUnvalidTimeStampEntry)
		m_pUnvalidTimeStampEntries = m_pLastUnvalidTimeStampEntry = pEntry;
	else
		m_pLastUnvalidTimeStampEntry = m_pLastUnvalidTimeStampEntry->next = pEntry;
	m_wUnvalidTimeStampEntries++;
}

VO_VOID CRealVideoTrack::AddUnvalidTimeStampEntryList(VO_U32 dwRefTime)
{
	VO_U32 dwStart = m_pUnvalidTimeStampEntries->time_stamp - 1;
	VO_U32 dwTimeStep = (dwRefTime - dwStart) / (m_wUnvalidTimeStampEntries + 1);
	PBaseStreamMediaSampleIndexEntry pTmpEntry = m_pUnvalidTimeStampEntries;
	for(VO_U16 i = 1; i <= m_wUnvalidTimeStampEntries; i++)
	{
		pTmpEntry->time_stamp = dwStart + i * dwTimeStep;
		pTmpEntry = pTmpEntry->next;
	}

	m_pStreamFileIndex->AddEntries(m_pUnvalidTimeStampEntries, m_pLastUnvalidTimeStampEntry);

	m_pUnvalidTimeStampEntries = m_pLastUnvalidTimeStampEntry = VO_NULL;
	m_wUnvalidTimeStampEntries = 0;
}

VO_U32 CRealVideoTrack::GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime)
{
	if(m_dwSeekPoints == 0)
		return VO_ERR_NOT_IMPLEMENT;

	*pllPreviousKeyframeTime = *pllNextKeyframeTime = VO_MAXS64;
	for(VO_U32 i = 0; i < m_dwSeekPoints; i++)
	{
		if(m_pSeekPoints[i].time_stamp <= llTime)
			*pllPreviousKeyframeTime = m_pSeekPoints[i].time_stamp;
		else
		{
			*pllNextKeyframeTime = m_pSeekPoints[i].time_stamp;
			break;
		}
	}

	return VO_ERR_SOURCE_OK;
}
