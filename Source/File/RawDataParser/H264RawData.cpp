	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		H264RawData.cpp

	Contains:	H264 raw data file parser class file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2011-10-25		Rodney		Create file

*******************************************************************************/
#include "voOSFunc.h"
#include "H264RawData.h"
#include "fCC.h"
#include "fCodec.h"
#include "voSource2.h"
#include "voVideo.h"
#include "CH264Track.h"
#include "voLog.h"

    #define LOG_TAG "H264RawData"
    #include "voLog.h"

#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))

#define READ_LEN 2*1024
#define INBUFFER_SIZE (READ_LEN*1024)


static const VO_U8 VO_UE_VAL[512]={
	31,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
	7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9,10,10,10,10,11,11,11,11,12,12,12,12,13,13,13,13,14,14,14,14,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const VO_U8 VO_BIG_LEN_TAB[256]={
	0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

static signed long vo_big_len(unsigned long val)
{
	int len = 0;
	if (val & 0xffff0000) {
		val >>= 16;
		len += 16;
	}
	if (val & 0xff00) {
		val >>= 8;
		len += 8;
	}
	len += VO_BIG_LEN_TAB[val];

	return len;
}

// public methods
// ----------------------------------------------------------------------------
CH264RawData::CH264RawData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
	: CBaseReader(pFileOp, pMemOp, pLibOP, pDrmCB),
	m_dwNALsPerFrame(1),
	m_dwDurationPerFrame(0),
	m_dwBlockIndex(-1),
	m_dwMaxSampleSize(0),
	m_dwNALCount(0),
	m_dwCurrIndex(0),
	m_dwDuration(0),
	m_fileSize(0),
	m_pStreamBuffer(NULL),
	m_pH264HeadBuffer(NULL),
	m_headdatalen(0),
	m_bIsRepeat(VO_FALSE),
	m_nSysTime (0)
{
	memset(aStreamBlock, 0, STREAM_BUFFER_SIZE);
	memset(m_NALHead, 0, 4);
	m_NALHead[3] = 0x01;
	m_pNALList = (voList*) new voList();
}

CH264RawData::~CH264RawData(void)
{
	if (m_pStreamBuffer) SAFE_MEM_FREE(m_pStreamBuffer);
	if (m_pNALList) {
		delete m_pNALList;
		m_pNALList = NULL;
	}
	if(m_pH264HeadBuffer)
	{
		SAFE_MEM_FREE(m_pH264HeadBuffer);
		m_pH264HeadBuffer = NULL;
	}
}

VO_U32 CH264RawData::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseReader::Load(nSourceOpenFlags, pFileSource);
	if(rc != VO_ERR_SOURCE_OK)
		return rc;
	m_nTracks = 1;
	GetFileSize(&m_fileSize);
    m_pStreamBuffer = NEW_BUFFER(SAMPLE_ALIGNMENT_SIZE);
	m_dwNALCount = ParseH264RawDataFile();
	VOLOGI ("Total frame count = %d", m_dwNALCount);
#ifdef _SOURCE2
	float tDuration = (float)(((float)(1000))/H264_RAW_DATA_FRAME_RATE);
	m_dwDuration = m_dwNALCount*tDuration;//m_dwNALCount*H264_RAW_DATA_FRAME_DURATION;
	InitTracks(0);
	m_nTracks = 1;
#else

	if (m_dwNALCount == VO_ERR_SOURCE_END)
		return VO_ERR_SOURCE_END;

	if (m_dwMaxSampleSize <= SAMPLE_ALIGNMENT_SIZE) {
		m_dwMaxSampleSize = SAMPLE_ALIGNMENT_SIZE;
	} else {
		VO_U32 shang = m_dwMaxSampleSize / SAMPLE_ALIGNMENT_SIZE;
		if (m_dwMaxSampleSize % SAMPLE_ALIGNMENT_SIZE) {
			shang++;
		}
		m_dwMaxSampleSize = shang * SAMPLE_ALIGNMENT_SIZE;
	}
	m_pStreamBuffer = NEW_BUFFER(m_dwMaxSampleSize);

	m_dwNALsPerFrame = GetFramePeriod();
	if (m_dwNALsPerFrame == VO_FALSE)
		return VO_ERR_SOURCE_END;

	m_dwDurationPerFrame = H264_RAW_DATA_FRAME_DURATION / m_dwNALsPerFrame;
	if (H264_RAW_DATA_FRAME_DURATION % m_dwNALsPerFrame) m_dwDurationPerFrame++;
	m_dwDuration = m_dwNALCount * m_dwDurationPerFrame;
#endif
	return VO_ERR_SOURCE_OK;
}

VO_U32 CH264RawData::GetSample(VO_SOURCE_SAMPLE* pSample)
{
#ifdef _SOURCE2
	if (!m_pNALList->Locate(m_dwCurrIndex)) {
		m_dwCurrIndex = 0;
		m_pNALList->Locate(m_dwCurrIndex);
		m_nSysTime = 0;
		return VO_ERR_SOURCE_END;
	}
	PNALUNIT nal = (PNALUNIT)m_pNALList->Get();
	if (!nal) return VO_ERR_SOURCE_END;
	m_chunk.FLocate(nal->NALstart);
	if(nal->numBytesInNAL >= SAMPLE_ALIGNMENT_SIZE)
	{
		SAFE_MEM_FREE(m_pStreamBuffer);
		//m_pStreamBuffer = 0;
		m_pStreamBuffer = NEW_BUFFER(nal->numBytesInNAL + 1);
		if(!m_pStreamBuffer)
			return VO_ERR_SOURCE_OK;
		memset(m_pStreamBuffer, 0, (nal->numBytesInNAL + 1));
	}
	else
	{
		memset(m_pStreamBuffer, 0, SAMPLE_ALIGNMENT_SIZE);
	}
	if (!m_chunk.FRead(m_pStreamBuffer, nal->numBytesInNAL))
	{
		m_dwCurrIndex = 0;
		return VO_ERR_SOURCE_END;
	}

	VO_U32 syncword = 0;
	memcpy(&syncword,m_pStreamBuffer,3);
	if((0x010000 != syncword)&&(0x000000 == syncword))
	{
		memcpy(&syncword,m_pStreamBuffer,4);
		//printf("is not a valid packet");
		if(0x01000000 != syncword)
		{
			printf("is not a valid packet");
		}

	}

	pSample->Buffer = m_pStreamBuffer;
	pSample->Duration = H264_RAW_DATA_FRAME_DURATION;
	if(m_bIsRepeat)
	{
		pSample->Flag = VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
		m_bIsRepeat = VO_FALSE;
	}
	else
	{
		pSample->Flag = 0;
	}
	pSample->Size = nal->numBytesInNAL;
//	pSample->Time = m_dwCurrIndex * pSample->Duration;
	pSample->Time = (m_dwCurrIndex * 1000) / H264_RAW_DATA_FRAME_RATE;
	m_dwCurrIndex++;

	if (m_nSysTime == 0)
		m_nSysTime = voOS_GetSysTime ();

	if ((m_dwCurrIndex % 60) == 59)
	{
		VO_U32 uSysTime = voOS_GetSysTime () - m_nSysTime;
		VOLOGI ("Index % 4d, % 6d, Time % 6d, % 6d, % 6d ", m_dwCurrIndex, pSample->Size,
					uSysTime, (int)pSample->Time, (int)(uSysTime - pSample->Time));
	}

#else
	if (!m_pNALList->Locate(m_dwCurrIndex)) {
		m_dwCurrIndex = 0;
		return VO_ERR_SOURCE_END;
	}
	PNALUNIT nal = (PNALUNIT)m_pNALList->Get();
	if (!nal) return VO_ERR_SOURCE_END;
	m_chunk.FLocate(nal->bsNALstart);
	memset(m_pStreamBuffer, 0, m_dwMaxSampleSize);
	if (!m_chunk.FRead(m_pStreamBuffer, nal->numBytesInBSNAL))
		return VO_ERR_SOURCE_END;

	pSample->Buffer = m_pStreamBuffer;
	pSample->Duration = m_dwDurationPerFrame;
	pSample->Flag = 0;
	pSample->Size = nal->numBytesInBSNAL;
	pSample->Time = m_dwCurrIndex * pSample->Duration;
	m_dwCurrIndex++;
#endif
	return VO_ERR_SOURCE_OK;
}

VO_U32 CH264RawData::SetPos(VO_S64* pPos)
{
#ifndef _SOURCE2
	if ( *pPos < 0 || *pPos >= m_dwDuration ) return VO_ERR_SOURCE_END;
	m_dwCurrIndex = (VO_U32)(*pPos / m_dwDurationPerFrame);
	if (!m_pNALList->Locate(m_dwCurrIndex)) return VO_ERR_SOURCE_SEEKFAIL;
	PNALUNIT nal = (PNALUNIT)m_pNALList->Get();
	if (!nal) return VO_ERR_SOURCE_SEEKFAIL;
	
	VO_S64 rc = FileSeek(m_hFile, nal->bsNALstart, VO_FILE_BEGIN);
	if(rc < 0) {
		return (-2 == rc) ? VO_ERR_SOURCE_NEEDRETRY : VO_ERR_SOURCE_END;
	}
#endif
	m_dwCurrIndex = 0;
	pPos = 0;
	VOLOGI ("To reset the m_dwCurrIndex value when SetPos was called and m_dwCurrIndex = : %d", m_dwCurrIndex);
	//m_bIsRepeat = VO_TRUE;
	return VO_ERR_SOURCE_OK;
}

VO_U32 CH264RawData::SetTrackParameter(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CH264RawData::GetTrackParameter(VO_U32 uID, VO_PTR pParam)
{
	switch(uID)
	{
	case VO_PID_SOURCE_MAXSAMPLESIZE:
		*(VO_U32*)pParam = m_dwMaxSampleSize;
		return VO_ERR_SOURCE_OK;

	case VO_PID_SOURCE_CODECCC:
		return GetCodecCC((VO_U32*)pParam);

	default:
		break;
	}

	return VO_ERR_NOT_IMPLEMENT;
}
VO_U32 CH264RawData::InitTracks(VO_U32 nSourceOpenFlags)
{
	VO_U32 rc = TracksCreate(1);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;
	CH264Track* pNewTrack = new CH264Track(VOTT_VIDEO, 1, this->GetDuration(), this, m_pMemOp);
	VO_U32 i = 0;
	if(pNewTrack)
	{
		m_ppTracks[i] = pNewTrack;
	}
	pNewTrack->SetInUsed(VO_TRUE);
}
VO_U32	CH264RawData::GetHeadData(VO_CODECBUFFER* pHeadData)
{ 
	pHeadData->Buffer = m_pH264HeadBuffer;
	pHeadData->Length = m_headdatalen;//strlen((char*)m_pH264HeadBuffer);
	pHeadData->Time  = 0;
	pHeadData->UserData = 0;
	return 0;
}
VO_U32 CH264RawData::GetTrackInfo(VO_SOURCE_TRACKINFO* pTrackInfo)
{
	if(!pTrackInfo)
		return VO_ERR_INVALID_ARG;

	pTrackInfo->Start = 0;
	pTrackInfo->Duration = GetDuration();
	pTrackInfo->Type = VO_SOURCE_TT_VIDEO;

	VO_U32 nRes = GetCodec(&pTrackInfo->Codec);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	pTrackInfo->HeadData = m_pH264HeadBuffer;
	pTrackInfo->HeadSize = strlen((char*)m_pH264HeadBuffer);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CH264RawData::GetCodecCC(VO_U32* pCC)
{
	*pCC = FOURCC_h264;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CH264RawData::GetCodec(VO_U32* pCodec)
{
	VO_U32 dwH264CC = 0;
	VO_U32 nRes = GetCodecCC(&dwH264CC);
	if(nRes != VO_ERR_SOURCE_OK)
		return nRes;

	if(pCodec)
		*pCodec = fCodecGetVideoCodec(dwH264CC);//VOMP_VIDEO_CodingH264

	return VO_ERR_SOURCE_OK;
}


// protected methods
//-----------------------------------------------------------------------------
VO_U32 CH264RawData::RequestABlock()
{
	VO_U32 readBytes;		// actual read bytes from file.

	memset(aStreamBlock, 0, STREAM_BUFFER_SIZE);
	if (!m_chunk.FRead3(aStreamBlock, STREAM_BUFFER_SIZE, &readBytes))
		return VO_ERR_SOURCE_END;

	m_dwBlockIndex++;

	return readBytes;
}

// next_bits( 24 ) == 0x000001 or next_bits( 32 ) == 0x00000001 ?
VO_BOOL CH264RawData::next_bits(VO_BYTE bitsInBytes, VO_U8 bytes[4])
{
	switch (bitsInBytes) {
	case BITS_24:			// 0x000001
		if (bytes[0] == 0x0 && bytes[1] == 0x0 && bytes[2] == 0x1)
			return VO_TRUE;
		else
			return VO_FALSE;
	case BITS_32:			// 0x00000001
		if (bytes[0] == 0x0 && bytes[1] == 0x0 && bytes[2] == 0x0 && bytes[3] == 0x1)
			return VO_TRUE;
		else
			return VO_FALSE;
	}

	return VO_FALSE;
}

// Is  bytes[3] the boundary 3 zero bytes? true, yes; false, no.
VO_BOOL CH264RawData::IsBoundary3ZeroBytes(VO_U8 bytes[3])
{
	if (bytes[0] || bytes[1] || bytes[2])
		return VO_FALSE;

	return VO_TRUE;
}

// Get frame type(I, P, B).
VO_VIDEO_FRAMETYPE CH264RawData::GetH264FrameType(unsigned char * buffer , int size)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len,value;
	int info_bit;
	int totbitoffset = 0;
	int naluType = buffer[0]&0x0f;

	while(naluType!=1&&naluType!=5)//find next NALU
	{
		//buffer = GetNextFrame(buffer,size)
		unsigned char* p = buffer;  
		unsigned char* endPos = buffer+size;
		for (; p < endPos; p++)
		{
			if (XRAW_IS_ANNEXB(p))
			{
				size  -= p-buffer;
				buffer = p+3;
				naluType = buffer[0]&0x0f;
				break;
			}
			if (XRAW_IS_ANNEXB2(p))
			{
				size  -= p-buffer;
				buffer = p+4;
				naluType = buffer[0]&0x0f;
				break;
			}
		}
		if(p>=endPos)
			return VO_VIDEO_FRAME_NULL; 

	}
	if(naluType==5)
		return VO_VIDEO_FRAME_I;//I_FRAME

	buffer++;
	for(i=0;i<2;i++)
	{
		byteoffset= totbitoffset/8;
		bitoffset= 7-(totbitoffset%8);
		ctr_bit = (buffer[byteoffset] & (0x01<<bitoffset));   // set up control bit

		len=1;
		while (ctr_bit==0)
		{                 // find leading 1 bit
			len++;
			bitoffset-=1;           
			bitcounter++;
			if (bitoffset<0)
			{                 // finish with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}
			ctr_bit=buffer[byteoffset] & (0x01<<(bitoffset));
		}
		// make infoword
		inf=0;                          // shortest possible code is 1, then info is always 0
		for(info_bit=0;(info_bit<(len-1)); info_bit++)
		{
			bitcounter++;
			bitoffset-=1;
			if (bitoffset<0)
			{                 // finished with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}

			inf=(inf<<1);
			if(buffer[byteoffset] & (0x01<<(bitoffset)))
				inf |=1;
		}
		totbitoffset+=len*2-1;
		if(totbitoffset>48)
			return VO_VIDEO_FRAME_NULL;
	}
	//(int)pow(2,(bitsUsed/2))+info-1;//pow(2,x)==1<<x
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if (inf>=5)
	{
		inf-=5;
	}
	if(inf<0||inf>2)
		return VO_VIDEO_FRAME_NULL;
	else if (inf == 0)
		return VO_VIDEO_FRAME_P;
	else if (inf == 1)
		return VO_VIDEO_FRAME_B;
	else
		return VO_VIDEO_FRAME_I;

}

// Get frame period.
VO_U32 CH264RawData::GetFramePeriod()
{
	PNALUNIT nal;
	VO_U32 dwFrameFrequency = 0;
	VO_U32 dwFrameIndex = 0;
	VO_U32 dwNALinFrameIndex = 0;
	VO_U32 dwLastNALinFrameIndex = 0;

	for (VO_U32 i = 0; i < m_pNALList->GetCount(); i++)
	{
		// Get a NAL unit
		if (!m_pNALList->Locate(i)) return VO_FALSE;
		nal = (PNALUNIT)m_pNALList->Get();
		if (!nal) return VO_FALSE;

		m_chunk.FLocate(nal->NALstart);
		memset(m_pStreamBuffer, 0, m_dwMaxSampleSize);
		if (!m_chunk.FRead(m_pStreamBuffer, nal->numBytesInNAL))
			return VO_FALSE;

		// Get its type(I, P, or B) and calculate frame period.
		VO_VIDEO_FRAMETYPE frameType = GetH264FrameType(m_pStreamBuffer, nal->numBytesInNAL);
		if ((frameType == VO_VIDEO_FRAME_I) || (frameType == VO_VIDEO_FRAME_P) || (frameType == VO_VIDEO_FRAME_B))
		{
			dwFrameIndex++;
			if (dwLastNALinFrameIndex == dwNALinFrameIndex) {
				dwFrameFrequency++;
				if (dwFrameFrequency >= LOWEST_FRAME_FREQUENCY_THRESHOLD) break;
			} else {
				dwFrameFrequency = 0;
				dwLastNALinFrameIndex = dwNALinFrameIndex;
			}
			dwNALinFrameIndex = 0;
			//VOLOGI("dwFrameIndex = %d, dwNALinFrameIndex = %d", dwFrameIndex, dwNALinFrameIndex);
		}
		else
		{
			dwNALinFrameIndex++;
			//VOLOGI("dwFrameIndex = %d, dwNALinFrameIndex = %d", dwFrameIndex, dwNALinFrameIndex);
		}
	}	// for

	return dwNALinFrameIndex + 1;
}

// Parse H264 raw data file and find all nal unit in a H264 raw data file.
// Return the total number of nal units in the file, or an error code if some error occurs.
VO_U32 CH264RawData::ParseH264RawDataFile()
{
#ifdef _SOURCE2
     //VO_CODECBUFFER inData;
      char *inputBuf;
      char* nextFrame,*currFrame;
      int leftSize = INBUFFER_SIZE;
      int inSize = 0;
      int framesize=0;
	  int file_pos = 0;

	  //int nIndex = 0;

	  inputBuf = (char *)NEW_BUFFER(INBUFFER_SIZE);   //malloc input buffer
	  nextFrame = inputBuf;

	  //leftSize = fread(nextFrame,1,INBUFFER_SIZE,inFile);
	  leftSize = FileRead(m_hFile,nextFrame,INBUFFER_SIZE);
	  framesize = Get264HeadData(nextFrame,leftSize);

	  //Save the head data
	  if(framesize > 0)
	  {
		  m_pH264HeadBuffer = NEW_OBJS(VO_BYTE,(framesize+1));
		  memset(m_pH264HeadBuffer,0,framesize);
		  memcpy(m_pH264HeadBuffer,nextFrame,framesize);
		  m_pH264HeadBuffer[framesize] = 0;
		  m_headdatalen = framesize;
		  file_pos += framesize;
		  leftSize -= framesize;
	  }
	  else
	  {
		  return -1;//no nul package
	  }
	  nextFrame = nextFrame+framesize;


      //find next nalu head
	  do 
	  {
		framesize = GetNextFrame(nextFrame,leftSize);
		if(framesize < leftSize)
		{
			VO_U32 syncword = 0;
			memcpy(&syncword,nextFrame,3);
			if((0x010000 != syncword)&&(0x000000 == syncword))
			{
				memcpy(&syncword,nextFrame,4);
				//printf("is not a valid packet");
				if(0x01000000 != syncword)
				{
					printf("is not a valid packet");
				}

			}

			nextFrame = nextFrame + framesize;
		  //to contain the frame information
		  PNALUNIT nal = NEW_OBJ(NALUNIT);
		  nal->NALstart = file_pos;
		  nal->numBytesInNAL = framesize;
		  m_pNALList->Add(nal);
		  inSize = framesize;
		  file_pos += framesize;
		  leftSize -= framesize;
		 // break;
		}
		else
		{
		  if(leftSize>2*1024*1024)
		  {
			break;
		  }
		  else  //need to fill inputBuf
		  {
			int readSize;
			memcpy (inputBuf, nextFrame, leftSize);
			nextFrame =inputBuf;
			//memset (inputBuf+leftSize, 0, INBUFFER_SIZE-leftSize);
			//readSize=fread(inputBuf+leftSize,1,INBUFFER_SIZE-leftSize,inFile);
			if(readSize = FileRead(m_hFile,inputBuf+leftSize,INBUFFER_SIZE-leftSize))
				leftSize+=readSize;
			else
				break;
		  }
		}
	  } while(1);

	//free(inputBuf);
	  SAFE_MEM_FREE(inputBuf);

	return m_pNALList->GetCount();
#else
	VO_U32 i = 0;
	VO_U32 dwBlockOffset = 0;
	PNALUNIT nal;

	if (RequestABlock() == VO_ERR_SOURCE_END) return VO_ERR_SOURCE_END;

	// parsing the first byte stream NAL unit.
	nal = NEW_OBJ(NALUNIT);
	nal->bsNALstart = 0;
	while (IsBoundary3ZeroBytes(aStreamBlock + i)) i++;	// skip leading_zero_8bits
	i += 3;
	nal->NALstart = i;
	while (!IsBoundary3ZeroBytes(aStreamBlock + i)) i++;	// skip NAL unit data
	nal->numBytesInNAL = (VO_U32)(i - nal->NALstart);
	while (IsBoundary3ZeroBytes(aStreamBlock + i)) i++;	// skip trailing_zero_8bits
	nal->numBytesInBSNAL = (VO_U32)(i - nal->bsNALstart - 1);
	m_pNALList->Add(nal);

	do {
		nal = NEW_OBJ(NALUNIT);
		nal->bsNALstart = m_dwBlockIndex * STREAM_BUFFER_SIZE + i - 1 - dwBlockOffset;	// includig a zero_byte
		i += 3;										// skip start_code_prefix_one_3bytes
		if (i >= STREAM_BUFFER_SIZE) {
			if (RequestABlock() == VO_ERR_SOURCE_END) {
				SAFE_FREE(nal);					// no byte stream NAL unit anymore.
				return m_pNALList->GetCount();
			}
			i -= STREAM_BUFFER_SIZE;
		}
		nal->NALstart = m_dwBlockIndex * STREAM_BUFFER_SIZE + i - dwBlockOffset;
		while (!IsBoundary3ZeroBytes(aStreamBlock + i)) {
			i++;
			if (i+2 >= STREAM_BUFFER_SIZE) {
				m_chunk.FBack(2);
				if (RequestABlock() == VO_ERR_SOURCE_END) {
					nal->numBytesInNAL = (VO_U32)(m_fileSize - nal->NALstart);
					nal->numBytesInBSNAL = (VO_U32)(m_fileSize - nal->bsNALstart);
					if (!nal->numBytesInNAL) {
						SAFE_FREE(nal);
					} else {
						m_pNALList->Add(nal);
						if (nal->numBytesInBSNAL > m_dwMaxSampleSize)
							m_dwMaxSampleSize = nal->numBytesInBSNAL;
					}
					return m_pNALList->GetCount();
				}
				i = 0;
				dwBlockOffset += 2;
			}
		}
		nal->numBytesInNAL = (VO_U32)(m_dwBlockIndex * STREAM_BUFFER_SIZE + i - nal->NALstart - dwBlockOffset);
		while (IsBoundary3ZeroBytes(aStreamBlock + i)) {	// skip trailing_zero_8bits
			i++;
			if (i+2 >= STREAM_BUFFER_SIZE) {
				m_chunk.FBack(2);
				if (RequestABlock() == VO_ERR_SOURCE_END) {
					nal->numBytesInBSNAL = (VO_U32)(m_fileSize - nal->bsNALstart);
					m_pNALList->Add(nal);
					if (nal->numBytesInBSNAL > m_dwMaxSampleSize)
						m_dwMaxSampleSize = nal->numBytesInBSNAL;
					return m_pNALList->GetCount();
				}
				i = 0;
				dwBlockOffset += 2;
			}
		}
		nal->numBytesInBSNAL = (VO_U32)(m_dwBlockIndex * STREAM_BUFFER_SIZE + i - nal->bsNALstart - 1 - dwBlockOffset);
		m_pNALList->Add(nal);
		if (nal->numBytesInBSNAL > m_dwMaxSampleSize)
			m_dwMaxSampleSize = nal->numBytesInBSNAL;
	} while (1);
#endif
}
VO_S32 CH264RawData::Get264HeadData(char* currPos,int size)
{
	char* endPos = currPos+size-4;
	char* p = currPos+3;
	int ret_size = 0;
	int count = 0;
	for (; p < endPos; p++)
	{
		if (XRAW_IS_ANNEXB(p)||XRAW_IS_ANNEXB2(p))
		{
			if (++count==2)
			break;
			p+=3;
		}
	}
	if(p>=endPos)
		ret_size = size;
	else
		ret_size = p-currPos;
	return ret_size;
}

VO_S32 CH264RawData::GetNextFrame(char* currPos,int size)
{
  char* endPos = currPos+size-4;
#if 1
	  char* p = currPos;	
	  char * tmp_ptr;
	  unsigned int bFind = 0;
	  unsigned int value;
	  int ret_size = 0;
	  do 
	  {
		  for (; p < endPos; p++)
		  {
			  if (XRAW_IS_ANNEXB(p))
			  {
				  tmp_ptr = p + 3;
				  break;
			  }
			  if (XRAW_IS_ANNEXB2(p))
			  {
				  tmp_ptr = p + 4;
				  break;
			  }
		  }
		  if(p>=endPos || tmp_ptr >= endPos)	  //need 5 bytes to parse
			  return size;
		  value = (*tmp_ptr++)&0x1F;
		  if (value == 1 || value == 5)   //slice or IDR
		  {
			  value = (((unsigned int)(*tmp_ptr))<<24) | ((unsigned int)(*(tmp_ptr+1))<<16) | ((unsigned int)(*(tmp_ptr+2))<<8) | ((unsigned int)*(tmp_ptr+3));
			  if(value >=(1<<27))
			  {
				  value >>= 32-9;
				  value = VO_UE_VAL[value];
			  }
			  else
			  {
				  value >>= 2*vo_big_len(value)-31;
				  value--;
			  }
			  if (value == 0) //start_mb_nr = 0, find a new pic
			  {
				  if (bFind)
				  {
					  break;
				  }
				  else
				  {
					  bFind = 1;
				  }
			  }
		  }
		  else if (bFind)
		  {
			  break;
		  }
		  p+=3;
	  } while (1);
#else

  char* p = currPos+3;
  int ret_size = 0;
  for (; p < endPos; p++)
  {
	if (XRAW_IS_ANNEXB(p)||XRAW_IS_ANNEXB2(p))
	  break;
  }
#endif
  if(p>=endPos)
	ret_size = size;
  else
	ret_size = p-currPos;
  return ret_size;
#ifdef OLD
  char* endPos = currPos+size-4;
  char* p = currPos+3;
  int ret_size = 0;
  for (; p < endPos; p++)
  {
	if (XRAW_IS_ANNEXB(p)||XRAW_IS_ANNEXB2(p))
	  break;
  }
  if(p>=endPos)
	ret_size = size;
  else
	ret_size = p-currPos;
  return ret_size;
#endif
}

//VO_S32 CH264RawData::RawDataGetFileFormat(VO_PTCHAR pFilePath)
//{
//	VO_TCHAR szFile[1024*2];
//	vostrcpy(szFile, pFilePath);
//	VO_U32 nLen = vostrlen(szFile);
//	VO_PTCHAR pExt = szFile + nLen - 1;
//	while(*pExt != _T ('.'))
//	{
//		pExt--;
//		if(pExt < szFile)	// some URL has no '.', so protect not crash, East 20111020
//			return VO_FILE_FFUNKNOWN;
//	}
//	pExt++;
//	if(0 == vostricmp(pExt,_T("h264")))
//		return VO_FILE_FFVIDEO_H264;
//	else if(0 == vostricmp(pExt,_T("h265")))
//		return VO_FILE_FFVIDEO_H265;
//}
