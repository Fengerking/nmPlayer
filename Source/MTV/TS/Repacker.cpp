#include "Repacker.h"
#include "CDumper.h"
#include "adtshead.h"
#include "voLog.h"
#include "voVideoParser.h"
#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK
#if (defined _DEBUG) && (!defined _WIN32_WCE)
FILE* _flog = NULL;
#endif //_DEBUG


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif



// 20100610
//#define INVALID_TIMESTAMP 0
#define INVALID_TIMESTAMP 0xFFFFFFFFFFFFFFFFLL
#define INVALID_PESTS INVALID_TIMESTAMP // invalid time stamp of PES

// CBaseRepacker
//==============================================================================

CBaseRepacker::CBaseRepacker(VOSTREAMPARSELISTENER* listener)
: m_pListener(listener)
//, m_bDTS(false)
{
#if (defined _DEBUG) && (!defined _WIN32_WCE)
	_flog = fopen("E:\\Temp\\parsets.csv", "wb");

	//add by qichaoshen @2011-10-25
	if(NULL != _flog)
	{
		fprintf(_flog, "%3s, %4s, %9s, %9s, %7s\n", 
		    "ID", //escfg->elementary_PID,
		    "Flag", //escfg->context, 
		    //"Use", //(escfg->context & 0x01) ? "DTS" : "PTS",
		    "DTS", "PTS", //(int)(packet->DTS / 90), (int)(packet->PTS / 90),
		    "Payload"); //packet->payloadsize);
	}
	//add by qichaoshen @2011-10-25
#endif //_DEBUG

#ifdef FAKE_TIMESTAMP
	m_tsPES1 = m_tsPES2 = INVALID_PESTS;
	m_tsDelta = DEFAULT_DELTA;
	m_cFramesOfPES = 0;
#endif //FAKE_TIMESTAMP
}

CBaseRepacker::~CBaseRepacker()
{
#if (defined _DEBUG) && (!defined _WIN32_WCE)
	if (_flog)
		fclose(_flog);
#endif //_DEBUG
}

void CBaseRepacker::OnPESHead(ESConfig* escfg, PESPacket* packet)
{
	VOSTREAMPARSEFRAMEINFO frameInfo;
	frameInfo.length = packet->payloadsize;

	frameInfo.timestamp.mul = packet->PTS;

	frameInfo.timestamp.div = 90000;

#if (defined _DEBUG) && (!defined _WIN32_WCE)

	//add by qichaoshen @ 2011-10-25
	if(NULL != _flog)
	{
	    fprintf(_flog, "%3d, %4d, %9d, %9d, %7d\n", 
		    escfg->elementary_PID,
		    packet->PTS_DTS_flags,
		    (int)(packet->DTS / 90), 
		    (int)(packet->PTS / 90),
		    packet->payloadsize);
	}
	
	//add by qichaoshen @ 2011-10-25

#endif //_DEBUG

#ifdef FAKE_TIMESTAMP
	if (m_tsPES1 == INVALID_PESTS)
	{
		m_tsPES1 = frameInfo.timestamp.mul;
		m_cFramesOfPES = 0;
		m_tsDelta = DEFAULT_DELTA;
	}
	else
	{
		if (m_cFramesOfPES > 0)
		{
			m_tsPES2 = frameInfo.timestamp.mul;
			m_tsDelta = (m_tsPES2 - m_tsPES1) / m_cFramesOfPES;
#if 1 //reset to use short term average, otherwise use long term average
			m_tsPES1 = m_tsPES2;
			m_cFramesOfPES = 0;
#endif
		}
	}
#endif //FAKE_TIMESTAMP

	m_pListener->OnNewFrame(escfg->elementary_PID, &frameInfo);
	m_pListener->OnFrameData(escfg->elementary_PID, packet->data, packet->datasize);
}

void CBaseRepacker::OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData)
{
	m_pListener->OnFrameData(escfg->elementary_PID, pData, cbData);
}

// CPrivateDataRepacker
//==============================================================================


CPrivateDataRepacker::CPrivateDataRepacker(VOSTREAMPARSELISTENER* listener)
: CBaseRepacker(listener)
{
	m_frameInfo.length = 0;
	m_frameInfo.timestamp.mul = INVALID_TIMESTAMP;
    m_frameInfo.timestamp.div = 90000;
	m_iCurrentDataSize = 0;
}

CPrivateDataRepacker::~CPrivateDataRepacker()
{

}

void CPrivateDataRepacker::OnPESHead(ESConfig* escfg, PESPacket* packet)
{
	m_frameInfo.length = packet->payloadsize;
	m_frameInfo.timestamp.mul = packet->PTS;
	m_frameInfo.timestamp.div = 90000;

	m_iCurrentDataSize += packet->datasize;
	m_pListener->OnNewFrame(escfg->elementary_PID, &m_frameInfo);
	m_pListener->OnFrameData(escfg->elementary_PID, packet->data, packet->datasize);
   
	if(	m_frameInfo.length != 0)
    {

        if(m_iCurrentDataSize == m_frameInfo.length)
	    {
            m_pListener->OnNewFrame(escfg->elementary_PID, &m_frameInfo);
		    m_iCurrentDataSize = 0;
        }
	    else
        {
		    if(m_iCurrentDataSize > m_frameInfo.length)
		    {
				VOLOGI("metadata size is illegal, larger than the size in desc:%d", m_frameInfo.length);
                m_pListener->OnNewFrame(escfg->elementary_PID, &m_frameInfo);
                m_iCurrentDataSize = 0;
		    }
        }
    }
}


void  CPrivateDataRepacker::OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData)
{
	m_pListener->OnFrameData(escfg->elementary_PID, pData, cbData);
	m_iCurrentDataSize += cbData;
    
	if(m_frameInfo.length != 0)
	{
		if(m_iCurrentDataSize == m_frameInfo.length)
		{
			m_pListener->OnNewFrame(escfg->elementary_PID, &m_frameInfo);
			m_iCurrentDataSize = 0;
		}
		else
		{
			if(m_iCurrentDataSize > m_frameInfo.length)
			{
                VOLOGI("metadata size is illegal, larger than the size in desc:%d", m_frameInfo.length);
				m_pListener->OnNewFrame(escfg->elementary_PID, &m_frameInfo);
				m_iCurrentDataSize = 0;
            }
        }	
    }
}

void CPrivateDataRepacker::Reset()
{
    m_frameInfo.length			= 0;
    m_frameInfo.timestamp.mul	= INVALID_TIMESTAMP;
    m_iCurrentDataSize = 0;
}

// CPatternRepacker
//==============================================================================

CPatternRepacker::CPatternRepacker(VOSTREAMPARSELISTENER* listener, uint32 pattern, uint32 mask, int patternSize,bool beNeedSyncWord)
: CBaseRepacker(listener)
, m_finder(pattern, mask)
, m_patternSize(patternSize)
, m_pendingMatched(0)
, m_countFrame(0)
, m_NeedSyncWord(beNeedSyncWord)
{
	m_frameInfo.length = 0;
	m_frameInfo.timestamp.mul = INVALID_TIMESTAMP;
	m_frameInfo.timestamp.div = 90000;


#ifdef FAKE_TIMESTAMP
	m_tsPES1 = m_tsPES2 = INVALID_PESTS;
	m_tsDelta = DEFAULT_DELTA;
	m_cFramesOfPES = 0;
#endif //FAKE_TIMESTAMP

    //add by qichaoshen @2011-12-06
	m_bPesPackMode = 0;
	m_iPesCount = 1;
	//add by qichaoshen @2011-12-06
}

CPatternRepacker::~CPatternRepacker()
{
}


void CPatternRepacker::SetPESPackMode(bool bPesMode)
{
	m_bPesPackMode = bPesMode;
}

void CPatternRepacker::OnPESHead(ESConfig* escfg, PESPacket* packet)
{
#if (defined _DEBUG) && (!defined _WIN32_WCE)


	//add by qichaoshen @2011-10-25
	if(NULL != _flog)
	{
		fprintf(_flog, "%3d, %4d, %9d, %9d, %7d\n", 
		    escfg->elementary_PID,
		    packet->PTS_DTS_flags,
		    (int)(packet->DTS / 90), 
		    (int)(packet->PTS / 90),
		    packet->payloadsize);

	}
	//add by qichaoshen @2011-10-25
#endif //_DEBUG

	//CDumper::WriteLog("CPatternRepacker::OnPESData");


	//if (m_frameInfo.length < 0)
	//{
	//	m_frameInfo.length = 0; //packet->payloadsize; //!!! the size makes no sense, do not use it
	//	//UseDTS(true); //packet->PTS_DTS_flags == 1);
	//}
	//if (packet->PTS_DTS_flags & 0x2)
	//{
	//	m_frameInfo.timestamp.mul = packet->PTS;
	//}
	//else if(packet->PTS_DTS_flags & 0x3)
	//{
	//	if((packet->PTS < 0xfff) && (packet->DTS > 0x1fffff000))
	//	{
	//	    m_frameInfo.timestamp.mul = packet->PTS; //something wrong, T-DMB go here
	//	}
	//	else
	//	{
	//		m_frameInfo.timestamp.mul = packet->DTS;
	//	}
	//}

	if(packet->PTS == 0x1FFFFFFFFLL)
	{
		m_frameInfo.timestamp.mul = 0xFFFFFFFFFFFFFFFFLL;
	}
	else
	{
		m_frameInfo.timestamp.mul = packet->PTS;
	}


	//else... otherwise, don't change it
	//m_frameInfo.timestamp.div = 90000;

#ifdef FAKE_TIMESTAMP
	if (m_tsPES1 == INVALID_PESTS)
	{
		m_tsPES1 = m_frameInfo.timestamp.mul;
		m_cFramesOfPES = 0;
		m_tsDelta = DEFAULT_DELTA;
	}
	else
	{
		if (m_cFramesOfPES > 0)
		{
			m_tsPES2 = m_frameInfo.timestamp.mul;
			m_tsDelta = (m_tsPES2 - m_tsPES1) / m_cFramesOfPES;
#if 1 //reset to use short term average, otherwise use long term average
			m_tsPES1 = m_tsPES2;
			m_cFramesOfPES = 0;
#endif
		}
	}
#endif //FAKE_TIMESTAMP

	m_iPesCount++;
	OnPESData(escfg, packet->data, packet->datasize);
}



/*buffer:the data  at the beginning of  NALU
2 : I frame
0 : P frame
1 : B frame
-1: error or other NALU
*/
/*
int GetFrameFormat (LPBYTE buffer, int nBufSize)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len;
	int info_bit;
	int totbitoffset = 0;

	int naluType = buffer[0]&0x0f;

	if(naluType>0&&naluType<=5)//need continuous check
		buffer++;
	else//the nalu type is params info
		return -1;
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
			return -1;
	}
	//(int)pow(2,(bitsUsed/2))+info-1;//pow(2,x)==1<<x
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if(inf>4)
		inf-=5;
	if(inf>4)
		return -1;//error

	return inf;
}


// return -1 Unknown 0, I, 1, P, 2 B,
int GetFrameType (LPBYTE pInData, int nInSize)
{
	long	m_lH264Format = 0;//VOH264_ANNEXB=0;
	DWORD	m_dwNALWord = 0X01000000;
	LPBYTE	pBuffer = pInData;
	int		nBufSize = nInSize;

	int		nNALSize = 4;

	int nFormat = -1;
	int nFrameCount = 0;

	while (nBufSize > 4)
	{
		if (m_lH264Format != 0)//VOH264_ANNEXB
		{
			int		nFrameSize = 0;
			for (int i = 0; i < nNALSize; i++)
			{
				nFrameSize <<= 8;
				nFrameSize += *pBuffer++;
			}
			nBufSize -= nNALSize;

			nFormat = GetFrameFormat (pBuffer, nFrameSize);
			pBuffer += nFrameSize;
			nBufSize -= nFrameSize;

			nFrameCount++;

			if (nFormat >= 0)
			{
				if (nFormat == 0 && nBufSize > 4)
					continue;
				break;
			}

		}
		else
		{
			pBuffer += 4;
			nBufSize -= 4;
			nFormat = GetFrameFormat (pBuffer, nBufSize);

			if (nFormat > 0)
				break;
			else if (nFormat == 0)
			{
				while (pBuffer - pInData < nInSize - 4)
				{
					if (!memcmp (pBuffer, &m_dwNALWord, 4))
						break;
					pBuffer++;
				}

				nBufSize = nInSize - (pBuffer - pInData);
			}

			nFrameCount++;
		}
	}


	if (nFormat == 2) // I Frame
		return 0;
	else if (nFormat == 1) // B Frame
		return 2;
	else if (nFormat == 0) // P Frame
		return 1;

	return -1;
}
*/

void CPatternRepacker::OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData)
{
	uint8* pStart = pData;
	uint8* pEnd = pData + cbData;
	uint8* pFinderPos = pStart;
	int size = 0;
    
	if(	m_frameInfo.timestamp.mul == INVALID_TIMESTAMP)
    {
		return;
	}

	if(!m_bPesPackMode)
	{
		while (pFinderPos <= pEnd)  // user <= instead of < for pattern itself
		{
			pFinderPos = m_finder.Find(pFinderPos, pEnd);
			if (pFinderPos == NULL)
			{
				CheckPendingMatched(escfg);
				m_pendingMatched = m_patternSize - m_finder.Unmatched();
				size = pEnd - pStart - m_pendingMatched;
				if (size > 0) //4/30/2009
				{
					m_pListener->OnFrameData(escfg->elementary_PID, pStart, size);
					m_countFrame += size;
				}
				return;
			}

			// 20100617
			//m_finder.Reset();

			uint8* pPatternStart = pFinderPos - m_patternSize;
			if (pPatternStart >= pStart) //padding belongs to prev frame
			{
				CheckPendingMatched(escfg);
			}
			else
				pPatternStart = pStart;
			size = pFinderPos - m_patternSize - pStart;

			if (IsNewFrame(pFinderPos, pEnd, m_countFrame + (size > 0 ? size : 0),escfg->elementary_PID))
			{
				if (size > 0)
				{
					m_pListener->OnFrameData(escfg->elementary_PID, pStart, size);
					m_countFrame += size;
				}

				// 20100610 , open this condition check
				if (m_frameInfo.timestamp.mul != INVALID_TIMESTAMP)
				{
					m_pListener->OnNewFrame(escfg->elementary_PID, &m_frameInfo);
				}


				m_countFrame = 0; 
				if (!m_NeedSyncWord)
				{
					pStart = pFinderPos;
					m_pendingMatched = 0;
				}
				else
				{
					CheckPendingMatched(escfg);
					pStart = pPatternStart;
				}
			}
			else // try next byte
			{
				if (!m_NeedSyncWord)
				{
					pStart = pFinderPos;
					m_pendingMatched = 0;
				}
			}
			//	pFinderPos = pPatternStart + 1;
		}
	}
	else
	{
		//need to collect the PES Data
		if(m_iPesCount == 1)
		{
			m_pListener->OnFrameData(escfg->elementary_PID, pStart, cbData);
			m_countFrame += cbData;
			return;
		}
		else
		{
			//need to issue a frame, the PES packet count is 2
			pFinderPos = m_finder.Find(pFinderPos, pEnd);
			if (pFinderPos == NULL)
			{
				CheckPendingMatched(escfg);
				m_pendingMatched = m_patternSize - m_finder.Unmatched();
				size = pEnd - pStart - m_pendingMatched;
				if (size > 0) //4/30/2009
				{
					m_pListener->OnFrameData(escfg->elementary_PID, pStart, size);
					m_countFrame += size;
				}
				return;
			}

			// 20100617
			//m_finder.Reset();

			uint8* pPatternStart = pFinderPos - m_patternSize;
			if (pPatternStart >= pStart) //padding belongs to prev frame
			{
				CheckPendingMatched(escfg);
			}
			else
			{
				pPatternStart = pStart;
			}

			size = pFinderPos - m_patternSize - pStart;
			if(size < 0)
			{
				//int i = m_finder.Unmatched();
			}

			if (IsNewFrame(pFinderPos, pEnd, m_countFrame + (size > 0 ? size : 0),escfg->elementary_PID))
			{
				if (size > 0)
				{
					m_pListener->OnFrameData(escfg->elementary_PID, pStart, size);
					m_countFrame += size;
				}

				// 20100610 , open this condition check
				if (m_frameInfo.timestamp.mul != INVALID_TIMESTAMP)
				{
					m_pListener->OnNewFrame(escfg->elementary_PID, &m_frameInfo);
				}

				//the frame is issued 
				m_iPesCount--;

				m_countFrame = 0; 
				if (!m_NeedSyncWord)
				{
					pStart = pFinderPos;
					m_pendingMatched = 0;
				}
				else
				{
					CheckPendingMatched(escfg);
					pStart = pPatternStart;
				}


				if(size<0)
				{
					size = cbData;
					pStart = pData;
				}
				else
				{
					size = cbData - size;
				}

				//for h264
				m_finder.Reset();
				//for h264

				m_pListener->OnFrameData(escfg->elementary_PID, pStart, size);
				m_countFrame += size;
			}
			else
			{
				//do nothing
			}
		}
	}
}

void CPatternRepacker::Reset()
{
	m_frameInfo.length			= 0;
	m_frameInfo.timestamp.mul	= INVALID_TIMESTAMP;
}


// CMpeg4Repacker
//==============================================================================

CMpeg4Repacker::CMpeg4Repacker(VOSTREAMPARSELISTENER* listener, uint32 pattern, uint32 mask, int patternSize)
: CPatternRepacker(listener, pattern, mask, patternSize)
{
	beSequenceHead = VO_FALSE;
}

CMpeg4Repacker::~CMpeg4Repacker()
{
}

bool CMpeg4Repacker::IsNewFrame(uint8* pStart, uint8* pEnd, int countFrame,int nStreamId) 
{ 
	if (*pStart == 0xB6)
	{
		if (beSequenceHead)///<we should merge the head data and next frame to one frame for HW decoder
		{
			beSequenceHead = VO_FALSE;
			return false;
		}
		return true;
	}
	else if (*pStart == 0xB0)
	{
		beSequenceHead = VO_TRUE;
		return true;
	}

	return false;
}




// CPatternRepackerFixedSize
//==============================================================================

CPatternRepackerFixedSize::CPatternRepackerFixedSize(VOSTREAMPARSELISTENER* listener, uint32 pattern, uint32 mask, int patternSize, int frameSize,bool beNeedSyncWord)
: CPatternRepacker(listener, pattern, mask, patternSize,beNeedSyncWord)
, m_frameSize(frameSize)
{
}

CPatternRepackerFixedSize::~CPatternRepackerFixedSize()
{
}

bool CPatternRepackerFixedSize::IsNewFrame(uint8* pStart, uint8* pEnd, int countFrame,int nStreamId) 
{ 
#ifdef _DEBUG
	printf("%3d - %02X %02X %02X %02X %02X %02X %02X %02X\n", countFrame, 
		pStart[0], pStart[1], pStart[2], pStart[3],
		pStart[4], pStart[5], pStart[6], pStart[7]);
#endif //_DEBUG
	//return countFrame >= m_frameSize;
	return true;
}


// CH264Repacker
//==============================================================================

CH264Repacker::CH264Repacker(VOSTREAMPARSELISTENER* listener)
: CPatternRepacker(listener, 0x000001, 0xffffff, 3)
{
	m_pVideoParser = NULL;
    m_VideoParserHandle = NULL;
    m_iPesCount = 0;

    memset(m_aTimeStamp, 0, sizeof(VO_S64)*MAX_FRAME_COUNT_IN_CACHE);
	memset(m_aPesLength, 0, sizeof(VO_U32)*MAX_FRAME_COUNT_IN_CACHE);
    m_ulIndexHeader = m_ulIndexTailer = 0;

    m_pWorkBuffer = new VO_BYTE[512*1024];
    if(m_pWorkBuffer == NULL)
    {
        return;
    }

	m_ulMaxSize = 512*1024;
    m_ulCurSize = 0;
    InitVideoParser();
}

CH264Repacker::~CH264Repacker()
{
    if(m_pWorkBuffer != NULL)
    {
		delete[] m_pWorkBuffer;
	}
    UInitVideoParser();
}


void CH264Repacker::InitVideoParser()
{
	VO_VIDEO_PARSERAPI*   pVideoParser = NULL;
    VO_U32  ulRet = 0;
    m_pVideoParser = new VO_VIDEO_PARSERAPI;
    if(m_pVideoParser == NULL)
    {
		VOLOGI("Can't get the VideoParser");
		return;
    }
	memset(m_pVideoParser, 0, sizeof(VO_VIDEO_PARSERAPI));
	pVideoParser = (VO_VIDEO_PARSERAPI*)m_pVideoParser;

    yyGetVPSRFunc (pVideoParser,VO_VIDEO_CodingH264);
	ulRet = pVideoParser->Init(&m_VideoParserHandle);
	if(ulRet != 0)
    {
        delete pVideoParser;
		m_pVideoParser = NULL;
		return;
    }

	ulRet = pVideoParser->SetParam(m_VideoParserHandle,VO_PID_VIDEOPARSER_GET_FRAME,NULL);
    if(ulRet != 0)
    {
        UInitVideoParser();
    }
}

void CH264Repacker::UInitVideoParser()
{
	VO_VIDEO_PARSERAPI*   pVideoParser = NULL;
	pVideoParser = (VO_VIDEO_PARSERAPI*)m_pVideoParser;

	if(m_pVideoParser != NULL && m_VideoParserHandle != NULL)
    {
		pVideoParser->Uninit(m_VideoParserHandle);
    }

	if(m_pVideoParser !=NULL)
    {
		delete pVideoParser;
		m_pVideoParser = NULL;
    }
}

void CH264Repacker::Reset()
{
    m_ulCurSize = 0;
    m_iPesCount = 0;
    m_ulIndexHeader = m_ulIndexTailer = 0;
    memset(m_aPesLength, 0, sizeof(VO_U32)*MAX_FRAME_COUNT_IN_CACHE);
    memset(m_aTimeStamp, 0, sizeof(VO_S64)*MAX_FRAME_COUNT_IN_CACHE);
}


VO_U32 CH264Repacker::GetFrame(ESConfig* escfg)
{
    VO_U32     ulLoop = 0;
    VO_U32     ulTotalOffset = 0;
    VO_U32     ulBufferEnd;
    VO_BYTE*   pNalStart = NULL;
	VO_BYTE*   pNalEnd = NULL;
    VO_U32     ulNalRealSize = 0;
    VO_S64     illTimeStamp = 0;

	VO_VIDEO_PARSERAPI*   pVideoParser = NULL;
	pVideoParser = (VO_VIDEO_PARSERAPI*)m_pVideoParser;
    
    if(pVideoParser == NULL)
    {
		return 1;
    }

    VO_U32   ulRet = 0;
	VO_CODECBUFFER pInData;
	VO_FRAME_Params frame_info;
	VO_FRAME_Params* pframe_info = NULL;
	pInData.Buffer = m_pWorkBuffer;
	pInData.Length = m_ulCurSize;
	pInData.UserData = &frame_info;

    ulRet = pVideoParser->Process(m_VideoParserHandle,&pInData);
	if(ulRet == 0)
	{
        pframe_info = (VO_FRAME_Params*)pInData.UserData;
		illTimeStamp = GetTimeStamp(pframe_info->frame_size);
		m_pListener->SetNewTimeStamp(escfg->elementary_PID, illTimeStamp);
		//the volibVideoParser will guarantee the nal offset info is nal count +1
        for(ulLoop=0; ulLoop<pframe_info->total_nal; ulLoop++)
        {
			pNalStart = m_pWorkBuffer + pframe_info->nal_start_offset[ulLoop];
			pNalEnd = m_pWorkBuffer + pframe_info->nal_start_offset[ulLoop+1]-1;
			while (pNalEnd > (pNalStart+1) && *pNalEnd == 0x00) 
			{
				--pNalEnd;
			}
			ulNalRealSize = pNalEnd-pNalStart+1;
			m_pListener->OnFrameData(escfg->elementary_PID, pNalStart, ulNalRealSize);
		}
        
		memmove(m_pWorkBuffer, m_pWorkBuffer+pframe_info->frame_size, m_ulCurSize-pframe_info->frame_size);
		m_ulCurSize = m_ulCurSize-pframe_info->frame_size;
		return 0;
	}
	else
    {
		return 1;
    }
}

VO_S64 CH264Repacker::GetTimeStamp(VO_U32 ulFrameLength)
{
    VO_U32 ulIndex = 0;
    VO_U32 ulLeftSize = ulFrameLength;
    VO_S64 illTimeStamp = m_aTimeStamp[m_ulIndexHeader];
	
	do
	{
		if(ulLeftSize < m_aPesLength[m_ulIndexHeader])
		{
			m_aPesLength[m_ulIndexHeader] -= ulLeftSize;
			break;
		}
		else
		{
			if(ulLeftSize == m_aPesLength[m_ulIndexHeader])
			{
				m_ulIndexHeader = (m_ulIndexHeader+1)%(MAX_FRAME_COUNT_IN_CACHE);
				m_iPesCount--;
				break;
			}
			else
			{
				m_iPesCount--;
				ulLeftSize -= m_aPesLength[m_ulIndexHeader];
				m_ulIndexHeader = (m_ulIndexHeader+1)%(MAX_FRAME_COUNT_IN_CACHE);
			}
		}
	}while(ulLeftSize>0);
    
    return illTimeStamp;
}

void CH264Repacker::PurgeFrame()
{
    return;
}


void CH264Repacker::OnPESHead(ESConfig* escfg, PESPacket* packet)
{
	if(packet->PTS == 0x1FFFFFFFFLL)
	{
		m_frameInfo.timestamp.mul = 0;
	}
	else
	{
		m_frameInfo.timestamp.mul = packet->PTS;
	}


	m_iPesCount++;
    m_aTimeStamp[m_ulIndexTailer] = (packet->PTS*1000)/90000;
    m_aPesLength[m_ulIndexTailer] = 0;
	m_ulIndexTailer = (m_ulIndexTailer+1)%(MAX_FRAME_COUNT_IN_CACHE);
	OnPESData(escfg, packet->data, packet->datasize);
}
void CH264Repacker::OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData)
{
    VO_U32    ulRet = 0;
    VO_BYTE*  pNewBuffer = NULL;
	VO_U32    ulLastPesIndex = (m_ulIndexTailer-1)%(MAX_FRAME_COUNT_IN_CACHE);

    if(m_pWorkBuffer == NULL)
    {
		VOLOGI("Lack of memory!");
		return;
    }

    if(m_ulMaxSize > (m_ulCurSize+cbData))
    {
        memcpy(m_pWorkBuffer+m_ulCurSize, pData, cbData);
		m_ulCurSize += cbData;
		m_aPesLength[ulLastPesIndex] +=cbData;
    }
	else
	{
		pNewBuffer = new VO_BYTE[2*(m_ulCurSize+cbData)];
        if(pNewBuffer == NULL)
		{
			delete[] m_pWorkBuffer;
			m_pWorkBuffer = NULL;
			VOLOGI("Lack of memory!");
			return;
		}

		memset(pNewBuffer, 0, 2*(m_ulCurSize+cbData));
		m_ulMaxSize = 2*(m_ulCurSize+cbData);
        memcpy(pNewBuffer, m_pWorkBuffer, m_ulCurSize);
        memcpy(pNewBuffer+m_ulCurSize, pData, cbData);
        m_ulCurSize += cbData;
		m_ulMaxSize = 2*(m_ulCurSize+cbData);
		if(m_pWorkBuffer != NULL)
		{
			delete[] m_pWorkBuffer;
		}
		m_pWorkBuffer = pNewBuffer;
	}

    if(m_iPesCount >1)
	{
		ulRet = GetFrame(escfg);
		if(ulRet == 0)
        {
            m_pListener->OnNewFrame(escfg->elementary_PID, &m_frameInfo);
        }
	}
	else
    {
		return;
    }
}

// CADTSAACRepacker
//==============================================================================
#define ADTS_HEADER_MIN_HEADER_FOR_LENGTH 3

CADTSAACRepacker::CADTSAACRepacker(VOSTREAMPARSELISTENER* listener,uint32 pattern, uint32 mask, int patternSize)
: CPatternRepacker(listener, pattern, mask, patternSize)
,m_nCntCacheBuffe(0)
,m_FrameLength(-1)
{
	m_pHeaderCacheBuffer = new uint8[ADTS_HEADER_MIN_HEADER_FOR_LENGTH];
}

CADTSAACRepacker::~CADTSAACRepacker()
{
	if(m_pHeaderCacheBuffer != NULL)
	{
		delete m_pHeaderCacheBuffer;
		m_pHeaderCacheBuffer = NULL;
	}
}


void CADTSAACRepacker::Reset()
{
    m_nCntCacheBuffe = 0;
    m_FrameLength = -1;
	m_frameInfo.length = 0;
	m_frameInfo.timestamp.mul = INVALID_TIMESTAMP;
}

bool CADTSAACRepacker::IsNewFrame(uint8* pStart, uint8* pEnd, int countFrame,int nStreamId)
{
//	static int lastframelen = -1;
	static int coutn111 = 0;

	coutn111++;
	if (m_FrameLength == -1)
	{
		m_FrameLength = 0;
		if (pEnd-pStart>0)
		{
			if (pEnd-pStart>1)
			{
				pStart -= 1;
				m_FrameLength = ((pStart[0] & 0x3) << 11) + (pStart[1] << 3) + (pStart[2] >> 5);
			}		
		}
		return true;
	}
	
	if (m_FrameLength == 0)///first check if the frame buffer has enough data to judge the frame length
	{
		int nFrameBufferSize = m_pListener->GetBufferSize(nStreamId);
		uint8 *pFrameBufferStart = m_pListener->GetFrameBuf(nStreamId);
		uint8 * pFinderPos = m_finder.Find(pFrameBufferStart,pFrameBufferStart+nFrameBufferSize);
		if (pFinderPos == NULL)
		{
			m_nCntCacheBuffe = 0;
			return false;
		}
		if ((pFrameBufferStart + nFrameBufferSize - pFinderPos)> 1)
		{
			m_nCntCacheBuffe = 0;
			pFinderPos -= 1;
			m_FrameLength = ((pFinderPos[0] & 0x3) << 11) + (pFinderPos[1] << 3) + (pFinderPos[2] >> 5);
		}
		else if (pEnd-pStart>0)///if frame buffer has no enough data,so the other data must be in PES packet
		{	
			int nCountFrameInBufferNoPattern = nFrameBufferSize-(pFinderPos - pFrameBufferStart);
			int j = 0;
			pFinderPos -= 1;
			while (j < nCountFrameInBufferNoPattern+1 && m_nCntCacheBuffe < 3)
			{
				m_pHeaderCacheBuffer[m_nCntCacheBuffe++] = pFinderPos[j++];
			}

			int nCountFrameInPES = countFrame - j - (m_patternSize-1);///<data size in PES packet
			int i = 0;
			uint8 *pTmpStart = pStart - nCountFrameInPES- m_patternSize;///<start pointer of data in PES packet

			while (i < nCountFrameInPES && m_nCntCacheBuffe < 3)
			{
				m_pHeaderCacheBuffer[m_nCntCacheBuffe++] = pTmpStart[i++];
			}
			if (m_nCntCacheBuffe == 3)///if all the infor get finded,finish this cache
			{
				m_FrameLength = ((m_pHeaderCacheBuffer[0] & 0x3) << 11) + (m_pHeaderCacheBuffer[1] << 3) + (m_pHeaderCacheBuffer[2] >> 5);
				m_nCntCacheBuffe = 0;
			}
			else//if not all the infor get finded,so all preview cache data should be put into  frame buffer or in PES packet,we can Get those data next time
			{
				m_nCntCacheBuffe = 0;
				return false;
			}
		}
		else
		{
			m_nCntCacheBuffe = 0;
			return false;
		}

		
	}
	if (countFrame < m_FrameLength)
	{
		return false;
	}
	if (countFrame > m_FrameLength)
	{
		VOLOGI("coutn111 =%d,countFrame=%d,m_FrameLength=%d",coutn111,countFrame,m_FrameLength);
		m_FrameLength = 0;
	//	return true;
	}
	if (pEnd-pStart>1)
	{
		pStart -= 1;
		m_FrameLength = ((pStart[0] & 0x3) << 11) + (pStart[1] << 3) + (pStart[2] >> 5);
	}
	else
		m_FrameLength = 0;
	return true;
}





// CLATMRepacker
//==============================================================================

CLATMRepacker::CLATMRepacker(VOSTREAMPARSELISTENER* listener)
: CPatternRepacker(listener, 0x56e0, 0xffe0, 2)
, m_frameSize(0)
{
}

CLATMRepacker::~CLATMRepacker()
{
}

bool CLATMRepacker::IsNewFrame(uint8* pStart, uint8* pEnd, int countFrame,int nStreamId)
{
	bool b = true; //default
	if (m_frameSize)
		if (countFrame != m_frameSize) //compare with previous frame size
		{
			if (countFrame > m_frameSize)
			{
				m_countFrame = 0; //reset it
				m_frameSize = 0; // reset
			}
			return false;
		}
	if (b)
		m_frameSize = ((m_finder.GetCurrent() & 0x1f) << 8) + *pStart + 3;  //13 bits

	return b;
}
