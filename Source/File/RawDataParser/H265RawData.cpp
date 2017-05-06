	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		H265RawData.cpp

	Contains:	H265 raw data file parser class file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2011-10-25		Rodney		Create file

*******************************************************************************/
#include "voOSFunc.h"
#include "H265RawData.h"
#include "fCC.h"
#include "fCodec.h"
#include "voSource2.h"
#include "voVideo.h"
#include "CH265Track.h"


    #define LOG_TAG "H265RawData"
    #include "voLog.h"

#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))

#define NEXT24BITS(x) (x[0]<<2)|(x[1]<<1)|(x[2])
#define INPUT_FRAME_NUM   100000    /* default input frames*/
#define IN_BUFF_LEN 1024*1024*10

// public methods
// ----------------------------------------------------------------------------
CH265RawData::CH265RawData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB)
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
	m_pH265HeadBuffer(NULL),
	m_bytespeed(0),
	m_headdatalen(0),
	m_bIsRepeat(VO_FALSE),
	m_nSysTime (0)
{
	memset(aStreamBlock, 0, H265_STREAM_BUFFER_SIZE);
	memset(m_NALHead, 0, 4);
	m_NALHead[3] = 0x01;
	m_pNALList = (voList*) new voList();
}

CH265RawData::~CH265RawData(void)
{
	if (m_pStreamBuffer) SAFE_MEM_FREE(m_pStreamBuffer);
	if (m_pNALList) {
		delete m_pNALList;
		m_pNALList = NULL;
	}
	if(m_pH265HeadBuffer)
	{
		SAFE_MEM_FREE(m_pH265HeadBuffer);
		m_pH265HeadBuffer = NULL;
	}
}
VO_U32 CH265RawData::InitTracks(VO_U32 nSourceOpenFlags)
{
	VO_U32 rc = TracksCreate(1);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;
	CH265Track* pNewTrack = new CH265Track(VOTT_VIDEO, 1, this->GetDuration(), this, m_pMemOp);
	VO_U32 i = 0;
	if(pNewTrack)
	{
		m_ppTracks[i] = pNewTrack;
	}
	pNewTrack->SetInUsed(VO_TRUE);
	return VO_ERR_SOURCE_OK;
}
VO_U32 CH265RawData::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseReader::Load(nSourceOpenFlags, pFileSource);
	if(rc != VO_ERR_SOURCE_OK)
		return rc;
//	m_nTracks = 1;
	GetFileSize(&m_fileSize);

	m_dwNALCount = ParseH265RawDataFile();

	m_pStreamBuffer = NEW_BUFFER(H265_SAMPLE_ALIGNMENT_SIZE);
	//m_dwDuration = m_dwNALCount*((1*1000)/H265_RAW_DATA_FRAME_DURATION);
	//m_bytespeed = m_dwDuration/m_fileSize;
	float tDuration = (float)(((float)(1000))/H265_RAW_DATA_FRAME_RATE);
	m_dwDuration = m_dwNALCount*tDuration;//m_dwNALCount*H265_RAW_DATA_FRAME_DURATION;
	InitTracks(0);
	m_nTracks = 1;
	return VO_ERR_SOURCE_OK;
}

VO_U32 CH265RawData::GetSample(VO_SOURCE_SAMPLE* pSample)
{
#ifdef H264
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
	if (!m_pNALList->Locate(m_dwCurrIndex)) {
		m_dwCurrIndex = 0;
		m_pNALList->Locate(m_dwCurrIndex);
		m_nSysTime = 0;
		return VO_ERR_SOURCE_END;
	}
	PH265NALUNIT nal = (PH265NALUNIT)m_pNALList->Get();
	if (!nal) 
		return VO_ERR_SOURCE_END;

	m_chunk.FLocate(nal->NALstart);
	if(nal->numBytesInNAL >= H265_SAMPLE_ALIGNMENT_SIZE)
	{
		SAFE_MEM_FREE(m_pStreamBuffer);
		//m_pStreamBuffer = 0;
		m_pStreamBuffer = NEW_BUFFER(nal->numBytesInNAL + 1);
		if(!m_pStreamBuffer)
			return VO_ERR_SOURCE_OK;
	}
	memset(m_pStreamBuffer, 0, H265_SAMPLE_ALIGNMENT_SIZE);
	if (!m_chunk.FRead(m_pStreamBuffer, nal->numBytesInNAL))
	{
		m_dwCurrIndex = 0;
		return VO_ERR_SOURCE_END;
	}
	
	pSample->Buffer = m_pStreamBuffer;
	pSample->Duration = H265_RAW_DATA_FRAME_DURATION; //m_bytespeed*nal->numBytesInNAL;//H265_RAW_DATA_FRAME_DURATION;
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
//	pSample->Time = m_dwCurrIndex * pSample->Duration;//m_bytespeed*nal->NALstart;//m_dwCurrIndex * pSample->Duration;
	pSample->Time = (m_dwCurrIndex * 1000) / H265_RAW_DATA_FRAME_RATE;
	m_dwCurrIndex++;

	if (m_nSysTime == 0)
		m_nSysTime = voOS_GetSysTime ();

	if ((m_dwCurrIndex % 60) == 59)
	{
		VO_U32 uSysTime = voOS_GetSysTime () - m_nSysTime;
		VOLOGI ("Index % 4d, % 6d, Time % 6d, % 6d, % 6d ", m_dwCurrIndex, pSample->Size,
					uSysTime, (int)pSample->Time, (int)(uSysTime - pSample->Time));
	}
	return VO_ERR_SOURCE_OK;
}

VO_U32 CH265RawData::SetPos(VO_S64* pPos)
{
#ifdef H264
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
	//if(0 == *pPos)
	m_dwCurrIndex = 0;//reset the sample index
	pPos = 0;
	//m_bIsRepeat = VO_TRUE;
	VOLOGI ("To reset the m_dwCurrIndex value when SetPos was called and m_dwCurrIndex = : %d", m_dwCurrIndex);
	return VO_ERR_SOURCE_OK;
}

VO_U32 CH265RawData::SetTrackParameter(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CH265RawData::GetTrackParameter(VO_U32 uID, VO_PTR pParam)
{
#ifdef H264
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
#endif
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CH265RawData::GetTrackInfo(VO_SOURCE_TRACKINFO* pTrackInfo)
{
	if(!pTrackInfo)
		return VO_ERR_INVALID_ARG;

	pTrackInfo->Start = 0;
	pTrackInfo->Duration = GetDuration();
	pTrackInfo->Type = VO_SOURCE_TT_VIDEO;

	VO_U32 nRes = GetCodec(&pTrackInfo->Codec);
	if(VO_ERR_SOURCE_OK != nRes)
		return nRes;

	pTrackInfo->HeadData = m_pH265HeadBuffer;
	pTrackInfo->HeadSize = strlen((char*)m_pH265HeadBuffer);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CH265RawData::GetCodecCC(VO_U32* pCC)
{
	*pCC = FOURCC_h265;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CH265RawData::GetCodec(VO_U32* pCodec)
{
	VO_U32 dwH265CC = 0;
	VO_U32 nRes = GetCodecCC(&dwH265CC);
	if(nRes != VO_ERR_SOURCE_OK)
		return nRes;

	if(pCodec)
		*pCodec = fCodecGetVideoCodec(dwH265CC);//VOMP_VIDEO_CodingH265

	return VO_ERR_SOURCE_OK;
}


// protected methods
//-----------------------------------------------------------------------------
VO_U32 CH265RawData::RequestABlock()
{
	VO_U32 readBytes;		// actual read bytes from file.

	memset(aStreamBlock, 0, H265_STREAM_BUFFER_SIZE);
	if (!m_chunk.FRead3(aStreamBlock, H265_STREAM_BUFFER_SIZE, &readBytes))
		return VO_ERR_SOURCE_END;

	m_dwBlockIndex++;

	return readBytes;
}

// next_bits( 24 ) == 0x000001 or next_bits( 32 ) == 0x00000001 ?
VO_BOOL CH265RawData::next_bits(VO_BYTE bitsInBytes, VO_U8 bytes[4])
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
VO_BOOL CH265RawData::IsBoundary3ZeroBytes(VO_U8 bytes[3])
{
	if (bytes[0] || bytes[1] || bytes[2])
		return VO_FALSE;

	return VO_TRUE;
}

// Get frame type(I, P, B).
VO_VIDEO_FRAMETYPE CH265RawData::GetH265FrameType(unsigned char * buffer , int size)
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
VO_U32 CH265RawData::GetFramePeriod()
{
#ifdef H264
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
		VO_VIDEO_FRAMETYPE frameType = GetH265FrameType(m_pStreamBuffer, nal->numBytesInNAL);
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
#endif
	return 1;
}

// Parse H265 raw data file and find all nal unit in a H265 raw data file.
// Return the total number of nal units in the file, or an error code if some error occurs.
VO_U32 CH265RawData::ParseH265RawDataFile()
{

    VO_S32 left_buf_len, used_buf_len, file_len , nalu_len;
    char *p_input_buf, *p_cur_buf_pos;
    
    int file_pos = 0;//to note the file position relate to the operation
	int nFrameCount = 0;

 	GetFileSize((VO_S64*)&file_len);
    /* Allocate input buffer*/
    //p_input_buf = (char *)malloc(IN_BUFF_LEN * sizeof (char));
	p_input_buf = (char *)NEW_BUFFER(IN_BUFF_LEN);

    
    left_buf_len    = 0;
	p_cur_buf_pos   = NULL;
	nalu_len        = -1;/* not ready */
    if (nalu_len < 0) {
		//left_buf_len += fread(p_input_buf + left_buf_len, 1, (IN_BUFF_LEN - left_buf_len), raw_data_file);
		left_buf_len += FileRead(m_hFile,(p_input_buf + left_buf_len),(IN_BUFF_LEN - left_buf_len));
		p_cur_buf_pos = p_input_buf;
    }
	/* Get header data*/
	nalu_len = Get265HeadData(p_cur_buf_pos, left_buf_len);
	if(nalu_len > 0)//get the head data
	{
		m_pH265HeadBuffer = NEW_OBJS(VO_BYTE,(nalu_len + 1));
		m_headdatalen = nalu_len;
		memset(m_pH265HeadBuffer,0,nalu_len + 1);
		memcpy(m_pH265HeadBuffer,p_cur_buf_pos,nalu_len);
		m_pH265HeadBuffer[nalu_len] = 0;

		p_cur_buf_pos += nalu_len;
	    left_buf_len  -= nalu_len;
		file_len      -= nalu_len;
		file_pos      += nalu_len;
	}

	PH265NALUNIT nal = NULL;
    while(file_len > 0){
		VO_S32 nal_type;

        if (left_buf_len == IN_BUFF_LEN && nalu_len < 0) {/*nalu not ready, but input buffer is full*/
            break;
        }

        /* Fill more data */
        if (nalu_len < 0) {
			//left_buf_len += fread(p_input_buf + left_buf_len, 1, (IN_BUFF_LEN - left_buf_len), raw_data_file);
			VO_S32 readlen = 0;
			if(readlen = FileRead(m_hFile,(p_input_buf + left_buf_len),(IN_BUFF_LEN - left_buf_len)))
			{
				left_buf_len += readlen;
			}
			else
			{
				break;//end of the file
			}
			p_cur_buf_pos = p_input_buf;
        }

        /* Get nalu*/
		nalu_len = get_nalu(p_cur_buf_pos, left_buf_len, &nal_type);
		if(nalu_len > 0){
			nal = NEW_OBJ(H265NALUNIT);
			nal->NALstart = file_pos;
			nal->numBytesInNAL = nalu_len;
			file_pos += nalu_len;
			m_pNALList->Add(nal);
			nFrameCount++;
		}
        else{// didn't find nalu
              continue;
        }
        /* Update file_length and used buffer */
		p_cur_buf_pos += nalu_len;
	    left_buf_len  -= nalu_len;
		file_len      -= nalu_len;
        
    }
	SAFE_MEM_FREE(p_input_buf);

	  VOLOGI ("The frame count is %d", nFrameCount);

	return m_pNALList->GetCount();


#ifdef USELENTH
	VO_U32 nSIZE = 0;
	if(m_chunk.FRead(&nSIZE,sizeof(VO_U32)))
	{
		m_pH265HeadBuffer = NEW_OBJS(VO_BYTE,nSIZE);
		memset(m_pH265HeadBuffer,0,nSIZE);
		m_chunk.FRead(m_pH265HeadBuffer,nSIZE);
	}
	PH265NALUNIT nal = NULL;
	while(m_chunk.FRead(&nSIZE,sizeof(VO_U32)))
	{
		nal = NEW_OBJ(H265NALUNIT);
		nal->NALstart = m_chunk.FGetFilePos();
		nal->numBytesInNAL = nSIZE;
		m_pNALList->Add(nal);
		//m_chunk.SeekFileB(SIZE,VO_FILE_CURRENT);
		m_chunk.FLocate(nal->NALstart + nSIZE);
	}
	return m_pNALList->GetCount();
#endif
#ifdef H264
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
VO_U32	CH265RawData::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Buffer = m_pH265HeadBuffer;
	pHeadData->Length = m_headdatalen;
	pHeadData->Time  = 0;
	pHeadData->UserData = 0;
	return VO_ERR_SOURCE_OK;
}

VO_S32 CH265RawData::Get265HeadData(char *p_input, VO_S32 input_len)
{
	VO_S32 nal_type;
	VO_S32 head_len = 0;

	do{
		VO_S32 nalu_len;

		nalu_len = get_nalu(p_input, input_len, &nal_type);

		if (nalu_len < 0) {
			return -1;
		}

		if (nal_type < 31) {
			break;
		}

		head_len  += nalu_len;
		p_input   += nalu_len;
		input_len -= nalu_len;

	}while(input_len > 0);
	
    return head_len;
}
VO_S32 CH265RawData::get_nalu(char *p_input, VO_S32 input_len, VO_S32 *nal_tyle)
{
   /*find 0x000001 */
    char *head = p_input, *end = p_input + input_len;

	if (input_len <= 0)
		return -1;

	while ((end > head) && (NEXT24BITS(head) != 0x000001)) {
		head++;
	}

	if (end == head)
		return -1;//no sc

	head += 3;

	*nal_tyle = ((head[0] >> 1) & 0x3f);

	while ((end > head) && (NEXT24BITS(head) !=  0x000001)) {
		head++;
	}
	
	return (VO_S32)(head - p_input);
}
