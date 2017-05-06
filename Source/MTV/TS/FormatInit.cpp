#include "FormatInit.h"
#include "mp4cfg.h"
#include "Repacker.h"
#include "LatmParser.h"
#include "CDumper.h"
#include "fVideoHeadDataInfo.h"
#include "adtsparser.h"
#include "voVideo.h"
#include "voAudio.h"
#include "adtshead.h"
#include "voLog.h"
#include "voSubtitleType.h"
#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK

using namespace MP4;

const int MAX_INIT_DATA_SIZE = 512 << 10; //512 K
const int DEFAULT_AAC_FRAME_SIZE = 16 << 10;
const int MAX_ADTS_HEADER_SIZE = 9;
const int MAX_LATM_HEADER_SIZE = 32;

//const int MAX_MPEG2_FRAME_SIZE = 512 << 10;it doesn't make sence,the max frame size should be weight*height*(bit per pixel)
const int MAX_MPEG4_FRAME_SIZE = 512 << 10;
const int MAX_MP3_FRAME_SIZE = 2 << 10;
const int MAX_AC3_FRAME_SIZE = 64 << 10; //???
const int MAX_AAC_FRAME_SIZE = 64 << 10;
const int MAC_DTS_FRAME_SIZE = 12 << 10;
const int MAX_HEVC_FRAME_SIZE = 512 << 10;


bool ParseMP3Head(const void *pbData, int nSize, int& samplerate, int& channels, int& bitrate, int& framelen)
{
	// 1. index = MPEG Version ID; 2. index = sampling rate index
	const int TABLE_SAMPLE_RATE[4][3] = 
	{
		{11025, 12000, 8000,  },	// MPEG 2.5
		{0,     0,     0,     },	// reserved
		{22050, 24000, 16000, },	// MPEG 2
		{44100, 48000, 32000  }		// MPEG 1
	};

	// bitrates: 1. index = LSF, 2. index = Layer, 3. index = bitrate index
	const int TABLE_BITRATE[2][3][15] = 
	{
		{	// MPEG 1
			{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},	// Layer1
			{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},	// Layer2
			{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,}	// Layer3
		},
		{	// MPEG 2, 2.5		
			{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},		// Layer1
			{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},			// Layer2
			{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,}			// Layer3
		}
	};

	// 1. index = LSF, 2. index = Layer
	const int TABLE_COEFFICIENTS[2][3] = 
	{
		{	// MPEG 1
			48,		// Layer1
			144,	// Layer2
			144		// Layer3
		},
		{	// MPEG 2, 2.5
			48,		// Layer1
			144,	// Layer2
			72		// Layer3
		}	
	};

	// padding sizes in bytes for different layers: 1. index = layer
	const int TABLE_PADDING_RATIO[3] = 
	{
		4,	// Layer1
		1,	// Layer2
		1	// Layer3
	};

	const int TABLE_CHANNEL[] = {2, 2, 2, 1};

	const uint8* p = (const uint8*) pbData;
	uint8 b1 = *p++;
	uint8 b2 = *p++;
	uint8 b3 = *p++;
	uint8 b4 = *p++;

	int sync = b1;
	sync <<= 3;
	sync |= b2 >> 5;
	if (sync != 0x7ff)///<11 bits must be 11111111111
		return false;

	int version = (b2 >> 3) & 0x03;
	if (version == 0x01)///<00-MPEG 2.5   01-未定义     10-MPEG 2     11-MPEG 1
	{
		return false;
	}
	int layer = 4 - ((b2 >> 1) & 0x03); ///<00-未定义      01-Layer 3     10-Layer 2      11-Layer 1
	if (layer == 4) //5/20/2009
		return false;

	int bitrateindex = b3 >> 4;
	if (bitrateindex == 15) //5/20/2009
		return false;
	int samplerateindex = (b3 >> 2) & 0x03;
	if (samplerateindex == 0x3) // 5/20/2009
		return false;
	int padding = (b3 >> 1) & 1;
	int channelmode = b4 >> 6;
	int LSF = version == 3 ? 0 : 1;

#if 0 //now we support all
	if ((version != 3) || (layer != 1)) //MPEG1, layer2
		return false;
#endif

	samplerate = TABLE_SAMPLE_RATE[version][samplerateindex];
	bitrate = TABLE_BITRATE[LSF][layer-1][bitrateindex] * 1000;
	channels = TABLE_CHANNEL[channelmode];
	framelen = TABLE_COEFFICIENTS[LSF][layer-1] * bitrate / samplerate + padding * TABLE_PADDING_RATIO[layer-1];
	return true;
}


// CHeadParser
//==============================================================================



CHeadParser::CHeadParser()
: headdata(NULL)
, headsize(0)
{
	m_pPrivate1 = NULL;
	m_iPrivate1Len = 0;
	m_pPrivate2 = NULL;
	m_iPrivate2Len = 0;
}

CHeadParser::~CHeadParser()
{
	if (headdata)
	{
		delete []headdata;
		headdata = NULL;
	}

	if(m_pPrivate1)
	{
		delete []m_pPrivate1;
		m_pPrivate1 = NULL;
	}

	if(m_pPrivate2)
	{
		delete []m_pPrivate2;
		m_pPrivate2 = NULL;
	}
}

// CMPEG2HeadParser
//==============================================================================

class CMPEG2HeadParser : public CHeadParser
{
public:
	CMPEG2HeadParser();
	bool Process(uint8* data, int size);

private:
	PatternFinder finder;
};

const int MAX_MPEG2_HEAD_SIZE = 1024;

CMPEG2HeadParser::CMPEG2HeadParser()
{
	headdata = new uint8[MAX_MPEG2_HEAD_SIZE];
    
    if(headdata == NULL)
    {
		return;
    }

	finder.SetPattern(0x000001B3);
}

bool CMPEG2HeadParser::Process(uint8* data, int size)
{
	uint8* pEnd = data + size;
	uint8* p1 = data;
	if (headsize == 0)
	{
		p1 = finder.Find(data, pEnd);
		if (p1 == NULL)
			return false;
		p1 -= 4; //??? include 00 00 01 B3
		finder.SetPattern(0x00000100, 0xffffff80);
	}
	uint8* p2 = finder.Find(p1, pEnd);
	if (p2 == NULL)
	{
		int tempsize = pEnd - p1;
		if (headsize + tempsize > MAX_MPEG2_HEAD_SIZE)
		{
			headsize = 0; //must error, reset the head data
			finder.SetPattern(0x000001B3);
			return false;
		}
		memcpy(headdata + headsize, p1, tempsize);
		headsize += tempsize;
		return false;
	}
	//hear we got whole head data
	p2 -= 4; //remove 00 00 01 xx
	int tempsize = p2 - p1;
	if (tempsize > 0)
	{
		if (headsize + tempsize > MAX_MPEG2_HEAD_SIZE)
		{
			headsize = 0; //must error, reset the head data
			return false;
		}
		memcpy(headdata + headsize, p1, tempsize);
		headsize += tempsize;
	}

	if(headsize == 0)
	{
		return false;
	}
	return true;
}

// CMpeg4HeadParser
//==============================================================================

class CMpeg4HeadParser : public CHeadParser
{
public:
	CMpeg4HeadParser();
	bool Process(uint8* data, int size);

private:
	PatternFinder finder;
};

const int MAX_MPEG4_HEAD_SIZE = 1024;

CMpeg4HeadParser::CMpeg4HeadParser()
{
	headdata = new uint8[MAX_MPEG4_HEAD_SIZE];
    if(headdata == NULL)
    {
        return;
    }

	finder.SetPattern(0x000001, 0xffffff);
}

bool CMpeg4HeadParser::Process(uint8* data, int size)
{
	uint8* pEnd = data + size;
	uint8* p1 = data;
	if (headsize == 0)
	{
		while (p1 < pEnd)
		{
			p1 = finder.Find(p1, pEnd);
			if (p1 == NULL)
				return false;
			if (p1 == pEnd)
				return false;
			uint8 flag = *p1;
			if (flag == 0xb0) // find sequence head start code
				break;
		}
		// copy 00 00 01
		memcpy(headdata, p1 - 3, 3);
		headsize = 3;
		// continue to find 00 00 01		
	}
	uint8* p2 = p1;
	while (p2)
	{
		p2 = finder.Find(p2, pEnd);
		if (p2)
		{
			uint8 flag = *p2;
			if (flag == 0xb6) //	find next VOP start code
				break;
		}
	}
	if (p2 == NULL)
	{
		int tempsize = pEnd - p1;
		if (headsize + tempsize > MAX_MPEG4_HEAD_SIZE)
		{
			headsize = 0; //must error, reset the head data
			return false;
		}
		memcpy(headdata + headsize, p1, tempsize);
		headsize += tempsize;
		return true;
	}
	//hear we got whole head data
	p2 -= 3; //remove 00 00 01 xx
	int tempsize = p2 - p1;
	if (tempsize > 0)
	{
		if (headsize + tempsize > MAX_MPEG4_HEAD_SIZE)
		{
			headsize = 0; //must error, reset the head data
			return false;
		}
		memcpy(headdata + headsize, p1, tempsize);
		headsize += tempsize;
	}
	return true;
}

// CH264HeadParser
//==============================================================================

class CH264HeadParser : public CHeadParser
{
public:
	CH264HeadParser();
	bool Process(uint8* data, int size);
	void RearangeHeadBuf();
	uint8 nMaskBitCount;
	uint8 m_delimiterNalValue;

private:
	PatternFinder finder;
	bool m_beFindSPS;
	bool m_beFindPPS;
	bool m_beGotSPS;
	bool m_beGotPPS;

	bool m_iWaitSPS_PPSData;
	bool beFindSPS;
	bool beFindPPS;

	uint8   m_uSPSDataLen;
	uint8   m_uPPSDataLen;
};

const int MAX_H264_HEAD_SIZE = 1024;

CH264HeadParser::CH264HeadParser()
{
	m_beFindSPS = false;
	m_beFindPPS = false;
	m_beGotSPS = false;
	m_beGotPPS = false;
	m_iWaitSPS_PPSData = false;

	m_uSPSDataLen = 0;
	m_uPPSDataLen = 0;
	
	m_pPrivate1 = new uint8[256];
    if(m_pPrivate1 == NULL)
	{
		return;
	}

	m_pPrivate2 = new uint8[256];
    if(m_pPrivate2 == NULL)
	{
		delete[] m_pPrivate1;
		m_pPrivate1 = NULL;
		return;
	}
	memset(m_pPrivate1, 0, 256);
	memset(m_pPrivate2, 0, 256);

	headdata = new uint8[MAX_H264_HEAD_SIZE];
    if(headdata == NULL)
    {
		delete[] m_pPrivate1;
		m_pPrivate1 = NULL;

		delete[] m_pPrivate2;
		m_pPrivate2 = NULL;
    }

	memset(headdata, 0, MAX_H264_HEAD_SIZE);
	
	// tag: 20101013 sometimes start code is 00 00 01, sometimes 00 00 00 01, it's better to check 3 bytes
	finder.SetPattern(0x000001, 0xffffff);
	beFindSPS = false;
	beFindPPS = false;
	nMaskBitCount = 0;
	m_delimiterNalValue = 0;
	nMaskBitCount = 3;
	//finder.SetPattern(0x00000001, 0xffffffff);
}

void CH264HeadParser::RearangeHeadBuf()
{
	PatternFinder pNewFinder(0x000001,0xffffff);
	uint8* pHead = GetHeadData();
	uint32 nSize = GetHeadSize();
	uint8* pEnd = pHead+nSize;
	uint8* pUselessDataStart = NULL;
	uint8* pUselessDataEnd = NULL;
	while (pHead < pEnd)
	{
		pHead = pNewFinder.Find(pHead, pEnd);
		if (!pHead || pHead == pEnd)
		{
			break;
		}
		int flag = *pHead & 0x1f;
		if (flag != 0x7 && flag != 0x8)
		{
			if (!pUselessDataStart)
			{
				pUselessDataStart = pHead - nMaskBitCount;
			}
			else
				pUselessDataEnd = pHead;

		}
		else
		{
			pUselessDataEnd = pHead;
		}
	}
	if (!pUselessDataEnd || pUselessDataEnd < pUselessDataStart)
	{
		pUselessDataEnd = pEnd;
	}
	pUselessDataEnd -= nMaskBitCount;///<leave the possible sync of next sps or pps
	if (pUselessDataStart && pUselessDataEnd)
	{
		memcpy(pUselessDataStart,pUselessDataEnd,pEnd - pUselessDataEnd);
		headsize -= (pUselessDataEnd-pUselessDataStart);
	}
}

bool CH264HeadParser::Process(uint8* data, int size)
{
	uint8    auSycArray[4] = {0, 0, 1, 0};
	uint8* pEnd = data + size;
	uint8* p1 = data;
	uint8 flag = 0;
    uint8* pDataStart = p1;


	if(m_beGotSPS == true && m_beGotPPS == true)
	{
		VOLOGI("the header data is error,reset it!");
		headsize = 0;
		m_uSPSDataLen = 0;
		m_uPPSDataLen = 0;
		m_beFindSPS = false;
		m_beGotSPS = false;

		m_beFindPPS = false;
		m_beGotPPS = false;
		m_iWaitSPS_PPSData = false;
	}


	while (p1 < pEnd)
	{
		p1 = finder.Find(pDataStart, pEnd);
		if (p1 == NULL)
		{
			if(m_iWaitSPS_PPSData == true)
			{
			    if(m_beFindSPS == true && m_beGotSPS == false)
			    {
					if((m_uSPSDataLen+(pEnd-pDataStart)) > 256)
					{
						m_beFindSPS = false;
						m_iWaitSPS_PPSData = false;
						break;
					}
					memcpy(m_pPrivate1+m_uSPSDataLen, pDataStart, pEnd-pDataStart);
					m_uSPSDataLen += (pEnd-pDataStart);
			    }

			    if(m_beFindPPS == true && m_beGotPPS == false)
			    {
					if((m_uPPSDataLen+(pEnd-pDataStart)) > 256)
					{
						m_beFindPPS = false;
						m_iWaitSPS_PPSData = false;
						break;
					}
					memcpy(m_pPrivate2+m_uPPSDataLen, pDataStart, pEnd-pDataStart);
					m_uPPSDataLen += (pEnd-pDataStart);
			    }
			}

			break;
		}


		if(m_iWaitSPS_PPSData == true)
		{
			if(m_beFindSPS == true && m_beGotSPS == false)
			{
				m_beGotSPS = true;
				m_iWaitSPS_PPSData = false;
				if((m_uSPSDataLen+(p1-pDataStart - 3)) > 256 || (p1-pDataStart) < 3)
				{
					m_beFindSPS = false;
					m_iWaitSPS_PPSData = false;
					m_beGotSPS = false;
					break;
				}

			    memcpy(m_pPrivate1 + m_uSPSDataLen, pDataStart, p1-pDataStart - 3);//Get the Nal Data
			    m_uSPSDataLen += (p1-pDataStart - 3);
				if(m_pPrivate1[m_uSPSDataLen-1] == 0)
				{
					m_uSPSDataLen--;
				}
			}

			if(m_beFindPPS == true && m_beGotPPS == false)
			{
				m_beGotPPS = true;
				m_iWaitSPS_PPSData = false;
				if((m_uPPSDataLen+(p1-pDataStart - 3)) > 256 || (p1-pDataStart) < 3)
				{
					m_beFindSPS = false;
					m_iWaitSPS_PPSData = false;
					m_beGotPPS = false;
					break;
				}
			    memcpy(m_pPrivate2 + m_uPPSDataLen, pDataStart, p1-pDataStart - 3);//Get the Nal Data
			    m_uPPSDataLen += (p1-pDataStart - 3);
				if(m_pPrivate2[m_uPPSDataLen-1] == 0)
				{
					m_uPPSDataLen--;
				}
			}
		}
		else
		{
            if(m_beGotSPS == true && m_beGotPPS == true)
			{
                break;
			}

			if(p1 == pEnd)
			{
				return false;
			}

		    flag = *p1 & 0x0f;
			if (flag == 0x07 || flag == 0x08)
			{
				if (flag == 0x07 && m_beGotSPS == false)
				{
					m_beFindSPS = true;
					m_uSPSDataLen = 0;
					m_iWaitSPS_PPSData = true;
					if((p1-pDataStart) > 3 && (*(p1-4)) == 0)
					{
						m_pPrivate1[0] = 0;
						m_uSPSDataLen += 1;
					}
					memcpy(m_pPrivate1 + m_uSPSDataLen, auSycArray, 3);
					m_uSPSDataLen += 3;
				}
				else
				{
					if(flag == 0x08 && m_beGotPPS == false)
					{
					    m_beFindPPS = true;
						m_uPPSDataLen = 0;
					    m_iWaitSPS_PPSData = true;
						if((p1-pDataStart) > 3 && (*(p1-4)) == 0)
						{
							m_pPrivate2[0] = 0;
							m_uPPSDataLen += 1;
						}
					    memcpy(m_pPrivate2 + m_uPPSDataLen, auSycArray, 3);
					    m_uPPSDataLen += 3;
					}
				}
			}

			if(flag == 0x09)
		    {
			    m_delimiterNalValue = 0x09;
			}

			pDataStart = p1;
		}
	}

    if(m_beGotSPS == true && m_beGotPPS == true)
	{
		memcpy(headdata + headsize, m_pPrivate1, m_uSPSDataLen);
		headsize += m_uSPSDataLen;

	    memcpy(headdata + headsize, m_pPrivate2, m_uPPSDataLen);
		headsize += m_uPPSDataLen;
	    return true;
	}

	return false;
}


// CHEVCHeadParser
//==============================================================================
class CHEVCHeadParser : public CHeadParser
{
public:
	CHEVCHeadParser();
	~CHEVCHeadParser();

	bool Process(uint8* data, int size);
    bool IsSequenceNalType(uint8*  pData, uint8* pNalType);
    bool CommitSequenceNalType(uint8*  pStart, uint8* pEnd, uint8 uNalType);

private:
	PatternFinder m_finder;
	bool m_bFindSPS;
	bool m_bFindPPS;
	bool m_bFindVPS;

    uint8*   m_pDataForSPS;
    uint32   m_ulDataLengthForSPS;
	uint8*   m_pDataForPPS;
    uint32   m_ulDataLengthForPPS;
	uint8*   m_pDataForVPS;
    uint32   m_ulDataLengthForVPS;

    uint8*   m_pworkbuffer;
    uint32   m_ulworkbufferMaxSize;
	uint32   m_ulworkbufferCurrentSize;
};


CHEVCHeadParser::CHEVCHeadParser()
{
    // tag: 20101013 sometimes start code is 00 00 01, sometimes 00 00 00 01, it's better to check 3 bytes
    m_finder.SetPattern(0x000001, 0xffffff);
    m_finder.Reset();

    m_bFindSPS = false;
    m_bFindPPS = false;
    m_bFindVPS = false;

    m_pDataForSPS = NULL;
    m_ulDataLengthForSPS = 0;
	
    m_pDataForPPS = NULL;
    m_ulDataLengthForPPS = 0;

    m_pDataForVPS = NULL;
    m_ulDataLengthForVPS = 0;

    m_pworkbuffer = new uint8[0x4000];
    if(m_pworkbuffer == NULL)
    {
        VOLOGI("Lack of memory!");
		return;
	}
    
	m_ulworkbufferMaxSize = 0x4000;
	memset(m_pworkbuffer, 0, m_ulworkbufferMaxSize);
    m_ulworkbufferCurrentSize = 0;
}

CHEVCHeadParser::~CHEVCHeadParser()
{
	// tag: 20101013 sometimes start code is 00 00 01, sometimes 00 00 00 01, it's better to check 3 bytes
    if(m_pDataForSPS != NULL)
    {
		delete []m_pDataForSPS;
        m_pDataForSPS = NULL;
    }

	if(m_pDataForPPS != NULL)
	{
		delete []m_pDataForPPS;
		m_pDataForPPS = NULL;
	}

    if(m_pDataForVPS != NULL)
    {
        delete []m_pDataForVPS;
        m_pDataForVPS = NULL;
    }

	if(m_pworkbuffer != NULL)
    {
		delete []m_pworkbuffer;
        m_pworkbuffer = NULL;
    }
}

bool CHEVCHeadParser::Process(uint8* data, int size)
{
	// tag: 20101013 sometimes start code is 00 00 01, sometimes 00 00 00 01, it's better to check 3 bytes
    uint8*     pSyncStart = NULL;
	uint8*     pPreSyncStart = NULL;
    uint8      ulStartSequenceLen = 0;
    uint8*     pSyncEnd = NULL;
	uint8      ulEndSequenceLen = 0;
	uint8*     pEnd = NULL;
    uint8*     pCur = NULL;
    uint8      uNalType = 0;
    uint32     ulSequencesSize = 0;

	if(m_bFindPPS == true && m_bFindSPS == true && m_bFindVPS == true)
    {
        if(headdata == NULL)
        {
            ulSequencesSize = m_ulDataLengthForPPS+m_ulDataLengthForSPS+m_ulDataLengthForVPS;
			headdata = new uint8[ulSequencesSize+8];
			if(headdata == NULL)
			{
				return false;
			}

			headsize = 0;
			memset(headdata, 0, ulSequencesSize+8);

			memcpy(headdata+headsize, m_pDataForVPS, m_ulDataLengthForVPS);
			headsize+=m_ulDataLengthForVPS;

			memcpy(headdata+headsize, m_pDataForSPS, m_ulDataLengthForSPS);
			headsize+=m_ulDataLengthForSPS;

			memcpy(headdata+headsize, m_pDataForPPS, m_ulDataLengthForPPS);
			headsize+=m_ulDataLengthForPPS;
		}

        return true;
	}

    if(data == NULL)
    {
        return false;
    }

    if(size+m_ulworkbufferCurrentSize < m_ulworkbufferMaxSize)
    {
		memcpy(m_pworkbuffer+m_ulworkbufferCurrentSize, data, size);
		m_ulworkbufferCurrentSize += size;
	}
	else
	{
		pCur = m_pworkbuffer;
		pEnd = m_pworkbuffer+m_ulworkbufferCurrentSize;
		pSyncStart = m_finder.Find(pCur, pEnd);
		if(pSyncStart == NULL)
		{
			memmove(m_pworkbuffer, m_pworkbuffer+m_ulworkbufferCurrentSize-4, 4);
			m_ulworkbufferCurrentSize = 4;
			memcpy(m_pworkbuffer+m_ulworkbufferCurrentSize, data, size);
			m_ulworkbufferCurrentSize += size;
		}
		else
		{
			//move the memory 
			while(pSyncStart != NULL && pCur < pEnd)
			{
				pPreSyncStart = pSyncStart;
				pCur = pSyncStart+3;
				pSyncStart = m_finder.Find(pCur, pEnd);
			}

			if(pPreSyncStart != NULL && (pPreSyncStart-m_pworkbuffer)>4)
            {
				memmove(m_pworkbuffer, pPreSyncStart-4, pEnd-pPreSyncStart+4);
				m_ulworkbufferCurrentSize = pEnd-pPreSyncStart+4;

				if((m_ulworkbufferCurrentSize+size)> m_ulworkbufferMaxSize)
				{
					VOLOGI("Nal unit too large!");
					return false;
				}
				else
				{
					memcpy(m_pworkbuffer+m_ulworkbufferCurrentSize, data, size);
					m_ulworkbufferCurrentSize += size;
				}
			}
			else
			{
				m_ulworkbufferCurrentSize = 0;
				memcpy(m_pworkbuffer+m_ulworkbufferCurrentSize, data, size);
				m_ulworkbufferCurrentSize += size;
			}
		}
	}

    //check if exist two sync word
    pCur = m_pworkbuffer;
    pEnd = m_pworkbuffer+m_ulworkbufferCurrentSize;
    
    while (pCur < pEnd)
    {
		pSyncStart = m_finder.Find(pCur, pEnd);
        if(pSyncStart != NULL)
		{
			pSyncEnd = m_finder.Find(pSyncStart, pEnd);
			if(pSyncEnd != NULL)
			{
				if(IsSequenceNalType(pSyncStart, &uNalType))
				{
					CommitSequenceNalType(pSyncStart, pSyncEnd, uNalType);
					pCur = pSyncStart+3;
				}
				else
				{
					pCur = pSyncStart+3;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			memmove(m_pworkbuffer, m_pworkbuffer+m_ulworkbufferCurrentSize-4, 4);
			m_ulworkbufferCurrentSize = 4;
			return false;
		}
    }

    return false;
}

bool CHEVCHeadParser::IsSequenceNalType(uint8*  pData, uint8* pNalType)
{
	uint8   uValue = 0;
	if(pData == NULL)
	{
		return false;
	}

    uValue = *pData;
    uValue = uValue>>1;

	switch(uValue&0x3f)
	{
	    case 32:
		{
			*pNalType = 32;
			return true;
		}
	    case 33:
	    {
			*pNalType = 33;
			return true;
		}
	    case 34:
		{
			*pNalType = 34;
			return true;
		}
	}

    return false;
}

bool CHEVCHeadParser::CommitSequenceNalType(uint8*  pStart, uint8* pEnd, uint8 uNalType)
{
	uint8    ucSequenceHeader[4] = {0, 0, 0, 1};
    uint32   ulEnd = 0;

    if(pStart == NULL || pEnd == NULL)
    {
        return false;
    }

	if((*(pEnd-4)) == 0)
	{
		ulEnd = 4;
	}
	else
	{
		ulEnd = 3;
	}

    switch(uNalType)
	{
		case 32:
		{
			if(m_bFindVPS == true)
			{
				return true;
			}

			m_bFindVPS = true;
			m_pDataForVPS = new uint8[pEnd-pStart+8];

			if(m_pDataForVPS == NULL)
			{
				return false;
			}
			else
			{
				memset(m_pDataForVPS, 0, pEnd-pStart+8);
				memcpy(m_pDataForVPS, ucSequenceHeader, 4);
				memcpy(m_pDataForVPS+4, pStart, pEnd-pStart-ulEnd);
				m_ulDataLengthForVPS = 4+pEnd-pStart-ulEnd;
				m_bFindVPS = true;
			}
			
			return true;				
		}
		case 33:
		{
			if(m_bFindSPS == true)
			{
				return true;
			}

			m_bFindSPS = true;
			m_pDataForSPS = new uint8[pEnd-pStart+8];

			if(m_pDataForSPS == NULL)
			{
				return false;
			}
			else
			{
				memset(m_pDataForSPS, 0, pEnd-pStart+8);
				memcpy(m_pDataForSPS, ucSequenceHeader, 4);
				memcpy(m_pDataForSPS+4, pStart, pEnd-pStart-ulEnd);
				m_ulDataLengthForSPS = 4+pEnd-pStart-ulEnd;
				m_bFindSPS = true;
			}

			return true;				
		}
		case 34:
		{
			if(m_bFindPPS == true)
			{
				return true;
			}

			m_bFindPPS = true;
			m_pDataForPPS = new uint8[pEnd-pStart+8];

			if(m_pDataForPPS == NULL)
			{
				return false;
			}
			else
			{
				memset(m_pDataForPPS, 0, pEnd-pStart+8);
				memcpy(m_pDataForPPS, ucSequenceHeader, 4);
				memcpy(m_pDataForPPS+4, pStart, pEnd-pStart-ulEnd);
				m_ulDataLengthForPPS = 4+pEnd-pStart-ulEnd;
				m_bFindPPS = true;
			}

			return true;				
        }
	}

	return false;
}


// CFormatInitializer
//==============================================================================

CFormatInitializer::CFormatInitializer()
: status(0)
, videohead(NULL)
, audiohead(NULL)
, playbackableES(1)
{
    for(int i=0; i<0x2000; i++)
	{
        m_ppPrivateDataForFormat[i] = NULL;
		m_pulPrivateDataForFormatLen[i] = 0;
	}
	m_uFormatState = 0;
    m_ulReadyStreamCount = 0;
}

CFormatInitializer::~CFormatInitializer()
{
	if (videohead)
	{
		delete videohead;
		videohead = NULL;
	}
	if (audiohead)
	{
		delete audiohead;
		audiohead = NULL;
	}

	for(int i=0; i<0x2000; i++)
	{
		if(m_ppPrivateDataForFormat[i] != NULL)
		{
			delete [](m_ppPrivateDataForFormat[i]);
			m_ppPrivateDataForFormat[i] = NULL;
			m_pulPrivateDataForFormatLen[i] = 0;
		}
	}
}

void CFormatInitializer::OnElementInfo(int total, int playbackable) 
{ 
	playbackableES = playbackable; 
}

void CFormatInitializer::OnElementStream(ESConfig* escfg)
{
	if (escfg->GetDCD())
		InitStreamFromDescriptor(escfg);
	else
		InitStreamFromConfig(escfg);
}

void CFormatInitializer::OnPESHead(ESConfig* escfg, PESPacket* packet)
{
	switch (escfg->stream_type)
	{
	case ST_111722Video:
	case ST_H262Video: //13818-2, 11172-2
		InitMPEG2(escfg, packet->data, packet->datasize);
		break;

	case ST_H264:
		InitH264(escfg, packet->data, packet->datasize);
		break;
	case ST_144962Visual:
		InitMPEG4(escfg, packet->data, packet->datasize);
		break;
	case ST_HEVC:
    case ST_HEVC_IEC_23008_2:
	    {
			InitHEVC(escfg, packet->data, packet->datasize);
			break;
	    }

	default:
		// Try to get new stream format for PES payload since no such info in PAT, 11/20/2007
		InitStreamFromPES(escfg, packet);
		break;
	}
}

void CFormatInitializer::OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData)
{
	PESPacket    sPesOnlyForData = {0};
	sPesOnlyForData.data = pData;
	sPesOnlyForData.datasize = (VO_U16) cbData;

	switch (escfg->stream_type)
	{
	case ST_111722Video:
	case ST_H262Video: //13818-2, 11172-2
		InitMPEG2(escfg, pData, cbData);
		break;

	case ST_H264:
		InitH264(escfg, pData, cbData);
		break;
	case ST_144962Visual:
		InitMPEG4(escfg, pData, cbData);
		break;
    case ST_HEVC:
    case ST_HEVC_IEC_23008_2:
		{
			InitHEVC(escfg, pData, cbData);
			break;
		}
	default:
		// Try to get new stream format for PES payload since no such info in PAT, 11/20/2007
		InitStreamFromPES(escfg, &sPesOnlyForData);
		break;
	}
}


void CFormatInitializer::InitStreamFromDescriptor(ESConfig* escfg)
{
	MP4::DecoderConfigDescriptor* DCD = escfg->GetDCD();
	MP4::DecoderSpecificInfo* DSI = escfg->GetDSI();

	// for T-DMB only
	switch (DCD->objectTypeIndication)
	{
	case 0x21: //AVC
		if (DSI == NULL)
			return;
		NewStreamAVC(escfg->elementary_PID, DSI->GetBodyData(), DSI->GetBodySize());
		escfg->extension = new CBaseRepacker(m_pListener);
		SetVideoReady();
		break;

	case 0x40: //AAC
		if (DSI == NULL)
			return;
		NewStreamAAC(escfg->elementary_PID, DSI->GetBodyData(), DSI->GetBodySize());
		escfg->extension = new CBaseRepacker(m_pListener);
		SetAudioReady();
		break;

	case 0x6c: // Visual ISO/IEC 10918-1, JPEG!
		break;

	default:
		break;
	}
}


void CFormatInitializer::InitStreamFromConfig(ESConfig* escfg)
{
	switch (escfg->stream_type)
	{
	case ST_111722Video:
	case ST_H262Video: //13818-2, 11172-2
		if (videohead == NULL)
			videohead = new CMPEG2HeadParser();
		break;
	
	case ST_H264:
		if (videohead == NULL)
			videohead = new CH264HeadParser();
		break;

	case ST_HEVC:
    case ST_HEVC_IEC_23008_2:
		if (videohead == NULL)
			videohead = new CHEVCHeadParser();
		break;

	case ST_144963AudioWithLATM:
		CDumper::WriteLog((char *)"ST_144963AudioWithLATM");
		break;
	case ST_144962Visual:
		if (videohead == NULL)
			videohead = new CMpeg4HeadParser();
		break;
	case ST_H2220_138181_PESPackets:
		if (escfg->descriptors == NULL)
			break;
		if(escfg->descriptors && escfg->descriptors->FindFirst(0x05))
		{
			registration_descriptor *res_des = (registration_descriptor *)escfg->descriptors->FindFirst(0x05);
			if (res_des->GetFormatType() == 0x44545332)
			{
				break;
			}	
		}

		//define for subtitle descriptor
		if (escfg->descriptors->FindFirst(0x59))
		{
			subtitling_descriptor *subtitle_des = (subtitling_descriptor *)escfg->descriptors->FindFirst(0x59);
			if (subtitle_des->GetLanguageCount() > 0 )
			{
				InitDvbSubTitle(escfg, NULL, 0);
				break;
			}	
		}

		break;
	case ST_META_DATA_IN_PES:
		if(escfg->descriptors && escfg->descriptors->FindFirst(0x26))
		{
			metadata_descriptor *meta_des = (metadata_descriptor *)escfg->descriptors->FindFirst(0x26);
			//the metadata_application_format_identifier is ID3
			if (meta_des->GetMetaDataFormatIdentifier() == 0x49443320) //
			{
				InitMetaDataID3(escfg, NULL, 0);
				break;
			}	
		}
		break;

#if 0
	case ST_H2220_138181_PESPackets:
		if (escfg->descriptors == NULL)
			break;
		if (escfg->descriptors->FindFirst(0x6A))
			InitAC3(escfg, NULL, 0);
		else if (escfg->descriptors->FindFirst(0x56))
			InitTeletext(escfg, NULL, 0);
		break;
#endif
	}

#if 0
	// fix settings for GoldSpotMedia
	BYTE profile = 3;
	BYTE sri = 5;
	BYTE channel = 1;
	BYTE dsi[2];

	switch (escfg->stream_type)
	{
	case ST_144963AudioWithLATM:
		//NewStreamAAC(escfg->elementary_PID, escfg->ES_info, escfg->ES_info_length);
		dsi[0] = (profile << 3) | (sri >> 1);
		dsi[1] = (sri << 7) | (channel << 4);
		NewStreamAAC(escfg->elementary_PID, dsi, 2);
		if (m_repacker == NULL)
			m_repacker = new CBaseRepacker(m_pListener);
		SetAudioReady();
		break;

	case ST_138187AudioWithADTS:
	case ST_H264:
		break;

	default:
		break;
	}
#endif
}


void CFormatInitializer::InitStreamFromPES(ESConfig* escfg, PESPacket* packet)
{
	switch (escfg->stream_type)
	{
	case ST_111723Audio:
		InitMP3(escfg, packet->data, packet->datasize, 0);
		break;

	case ST_138183Audio:
		InitMP3(escfg, packet->data, packet->datasize, 1);
		break;

	case ST_H2220_138181_PESPackets:
		if (escfg->descriptors == NULL)
			break;
		if (escfg->descriptors->FindFirst(0x6A))
			InitAC3(escfg, packet->data, packet->datasize);
		else if (escfg->descriptors->FindFirst(0x56))
			InitTeletext(escfg, packet->data, packet->datasize);
		else if (escfg->descriptors->FindFirst(0x5))
		{
			registration_descriptor *res_des = (registration_descriptor *)escfg->descriptors->FindFirst(0x05);
			if (res_des->GetFormatType() == 0x44545332)
			{
				InitDTS(escfg, packet->data, packet->datasize);
				break;
			}	
		}
        
		//define for subtitle descriptor
		else if (escfg->descriptors->FindFirst(0x59))
		{
			subtitling_descriptor *subtitle_des = (subtitling_descriptor *)escfg->descriptors->FindFirst(0x59);
			if (subtitle_des->GetLanguageCount() > 0 )
			{
				InitDvbSubTitle(escfg, packet->data, packet->datasize);
				break;
			}	
		}

		else if (escfg->descriptors->FindFirst(0x7a))
		{
			//Find the EAC3  desc
			InitEAC3(escfg, packet->data, packet->datasize);
		}

		break;

	case ST_138187AudioWithADTS:
		InitAdtsAAC(escfg, packet->data, packet->datasize);
		break;

	case ST_144963AudioWithLATM:
		InitLatmAAC(escfg, packet->data, packet->datasize);
		break;
	case ST_UserPrivateAC3:///<for system A(ATSC)use the 0x81 to uniquely identify the AC-3,the descriptor may be not exist.
		{
			InitAC3(escfg, packet->data, packet->datasize);
#if 0
			if (!escfg->descriptors)
			{
				break;
			}
			if (escfg->descriptors->FindFirst(0x05))
			{
				registration_descriptor *res_des = (registration_descriptor *)escfg->descriptors;
				if (res_des->GetFormatType() == 0x41432D33)
				{
					InitAC3(escfg, packet->data, packet->datasize);
				}	
			}

			if (escfg->descriptors->FindFirst(0x6A))
			{
				InitAC3(escfg, packet->data, packet->datasize);
			}
#endif
		}
		break;

	case ST_HDMV_DTS_Audio:
		{
			InitDTS(escfg, packet->data, packet->datasize);
			break;
		}
	case ST_AUDIO_DTS_HD:
		{
			InitDTS(escfg, packet->data, packet->datasize);
			break;
		}

	case ST_UserPrivateEAC3:
		{
			InitEAC3(escfg, packet->data, packet->datasize);
			break;
		}


	case ST_UserPrivateMin:
		if (escfg->descriptors && escfg->descriptors->FindFirst(0x05))
		{
			registration_descriptor *res_des = (registration_descriptor *)escfg->descriptors;
			if (res_des->GetFormatType() == 0x48444d56)
			{
				InitM2TSLPCM(escfg, packet->data, packet->datasize);
			}	
		}
		break;

	case ST_META_DATA_IN_PES:
	{
		if(escfg->descriptors && escfg->descriptors->FindFirst(0x26))
		{
			metadata_descriptor *meta_des = (metadata_descriptor *)escfg->descriptors;
			//the metadata_application_format_identifier is ID3
			uint32      formatIdentify = meta_des->GetMetaDataFormatIdentifier();
			if (formatIdentify == 0x49443320) //
			{
				InitMetaDataID3(escfg, packet->data, packet->datasize);
			}	
		}		
		break;
	}
	case ST_H262Video: //13818-2, 11172-2
	default:
		break;
	}
}


void CFormatInitializer::NewStreamAVC(int streamid, void* dsidata, int dsisize)
{
	//TODO: get width & height
	int width = 320;
	int height = 240;

#if 0
	AVCDecoderConfigurationRecord cfg;
	if (!cfg.Load((uint8*)dsidata, dsisize))
		return;

	int num1 = cfg.numOfSequenceParameterSets;
	int num2 = cfg.numOfPictureParameterSets;
	int seqhsize = (num1 + num2) * 2;
	int i;
	for (i = 0; i < num1; i++)
		seqhsize += cfg.sequenceParameterSet[i].len;
	for (i = 0; i < num2; i++)
		seqhsize += cfg.pictureParameterSet[i].len;


	int fmtsize = (sizeof(MPEG2VIDEOINFO) + seqhsize + 3 ) / 4 * 4; //DWORD align

	BYTE* p = (BYTE*) vi->dwSequenceHeader;
	for (i = 0; i < num1; i++)
	{
		int len = cfg.sequenceParameterSet[i].len;
		*p++ = (BYTE)((len >> 8) & 0xff);
		*p++ = (BYTE) len & 0xff;
		memcpy(p, cfg.sequenceParameterSet[i].data, len);
		p += len;
	}
	for (i = 0; i < num2; i++)
	{
		int len = cfg.pictureParameterSet[i].len;
		*p++ = (BYTE)((len >> 8) & 0xff);
		*p++ = (BYTE) len & 0xff;
		memcpy(p, cfg.pictureParameterSet[i].data, len);
		p += len;
	}

	VIDEOINFOHEADER2* vih = &vi->hdr;
	vih->dwInterlaceFlags = 0;
	vih->dwCopyProtectFlags = 0;
	vih->dwPictAspectRatioX = 4;
	vih->dwPictAspectRatioY = 3;

	BITMAPINFOHEADER* bih = &vih->bmiHeader;
	memset(bih, 0, sizeof(BITMAPINFOHEADER));
	bih->biSize = sizeof(BITMAPINFOHEADER);
	bih->biWidth = width;
	bih->biHeight = height;
	bih->biBitCount = 24; 
	bih->biPlanes = 1; 

#endif

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = streamid;
	info.is_video = true;
	info.codec = VO_VIDEO_CodingH264;
	info.video.width = width;
	info.video.height = height;

	info.extra_data = dsidata;
	info.extra_size = dsisize;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_VIDEO;


	info.max_frame_len = 20480; //??
	m_pListener->OnNewStream(&info);
    m_ulReadyStreamCount++;
}

void CFormatInitializer::NewStreamAAC(int streamid, void* dsidata, int dsisize)
{
	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = streamid;
	//info.codec = AC_AAC;
	info.codec = VO_AUDIO_CodingAAC;


	AudioSpecificConfig cfg;
	if (!cfg.Load((uint8*)dsidata, dsisize))
		return;

	info.audio.channels = cfg.channelConfiguration;
	info.audio.sample_rate = cfg.samplingFrequency;
	info.audio.sample_bits = 16;


	info.max_frame_len = DEFAULT_AAC_FRAME_SIZE; //?? increase the size for there are many frames in a packet in Korean T-DMB files.
	info.extra_data = dsidata;
	info.extra_size = dsisize;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_AUDIO;


	m_pListener->OnNewStream(&info);
	m_ulReadyStreamCount++;
}

void CFormatInitializer::InitAdtsAAC(ESConfig* escfg, uint8* pData, uint32 cbData)
{
    int   iLanguageLength = 0;
    VO_U8*    pPrivateForFormat = NULL;
    VO_U32*    pulPrivateDataLength = 0;
	if(escfg != NULL && escfg->extension != NULL)
	{
		VOLOGI("The Stream is ready!");
		return;
	}

    CDumper::WriteLog((char *)"CFormatInitializer::InitAdtsAAC");

    if(escfg->elementary_PID >= 0x2000)
	{
		VOLOGI("Invalid PID!");
        return;
	}

	pPrivateForFormat = m_ppPrivateDataForFormat[escfg->elementary_PID];
    pulPrivateDataLength = &(m_pulPrivateDataForFormatLen[escfg->elementary_PID]);

	if(pPrivateForFormat == NULL)
	{
		pPrivateForFormat = new uint8[0x2000];
		if(pPrivateForFormat == NULL)
		{
			return;
		}
		m_ppPrivateDataForFormat[escfg->elementary_PID] = pPrivateForFormat;
		m_pulPrivateDataForFormatLen[escfg->elementary_PID] = 0;
		pulPrivateDataLength = &(m_pulPrivateDataForFormatLen[escfg->elementary_PID]);
	}

	if(((*pulPrivateDataLength) + cbData) < 0x2000)
	{
		memcpy(pPrivateForFormat+(*pulPrivateDataLength), pData, cbData);
		(*pulPrivateDataLength) += cbData;
	}
	else
	{
        memset(pPrivateForFormat, 0, 0x2000);
		(*pulPrivateDataLength) = 0;
		memcpy(pPrivateForFormat+(*pulPrivateDataLength), pData, cbData);
		(*pulPrivateDataLength) += cbData;
	}

	int   ulOffsetLen = 0;
	uint16   wFindSyncWord = 0;
	PatternFinder finder(0xfff0, 0xfff6); //FFF8 or FFF9
	uint8* pEnd = pData + cbData;
	uint8* p1 = finder.Find(pData, pEnd);
	uint8* pLastSync = NULL;
	if (p1 == NULL)
		return;
	p1 -= 2;
	if (pEnd - p1 < MAX_ADTS_HEADER_SIZE)
		return;

	uint8* p2 = NULL;

	finder.Reset();
	pEnd = pPrivateForFormat+(*pulPrivateDataLength);
	p1 = finder.Find(pPrivateForFormat, pEnd);

	while(	p1 != pEnd)
	{
		if(p1 != NULL && (pEnd-p1)> MAX_ADTS_HEADER_SIZE)
		{
			//get the adts frame length
			if(ulOffsetLen != 0)
			{
				if((p1 - pLastSync) == ulOffsetLen)
				{
					wFindSyncWord = 1;
					break;
				}
				else
				{
					if((p1 - pLastSync) > ulOffsetLen)
					{
						p1 = pLastSync+1;
						ulOffsetLen = 0;
					}
				}
			}
			else
			{
				ulOffsetLen = ((p1[1] & 0x3) << 11) + (p1[2] << 3) + (p1[3] >> 5);//Get the frame Length
				pLastSync = p1;
			}
		}

		if(p1 == NULL)
		{
			break;
		}
		p1 = finder.Find(p1, pEnd);
	}

	if(wFindSyncWord == 0)
	{
		return;
	}


	p2 = pLastSync-2;
	ADTSHeader adtsh;
	if(false == adtsh.FindHead(p2,pEnd - p2,(void **)&p1,true))
		return;


	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(VOSTREAMPARSESTREAMINFO));
	info.is_video = false;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.codec = VO_AUDIO_CodingAAC;
	info.max_frame_len = MAX_AAC_FRAME_SIZE;  
	info.extra_data = p1;
	info.extra_size = MAX_ADTS_HEADER_SIZE;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_AUDIO;
	iLanguageLength = (strlen(escfg->chLanguage)>15)?15:(strlen(escfg->chLanguage));
	memcpy(info.audio.audio_language, escfg->chLanguage, iLanguageLength);

	VO_U32 pattern = (p1[3]&0xFC) | (p1[2]<<8) | (p1[1]<< 16)| (p1[0]<< 24);
	CBaseRepacker* repacker= new CADTSAACRepacker(m_pListener,pattern,0xFFFFFFFC,4);

	VOLOGI("Find Adts Header Data, the %x, %x, %x, %x", *p1,*(p1+1), *(p1+2), *(p1+3));

	escfg->extension = repacker;

	m_pListener->OnNewStream(&info);
	SetAudioReady();
	m_ulReadyStreamCount++;
}

void CFormatInitializer::InitLatmAAC(ESConfig* escfg, uint8* pData, uint32 cbData)
{
    int   iLanguageLength = 0;

	if(escfg != NULL && escfg->extension != NULL)
	{
		VOLOGI("The Stream is ready!");
		return;
	}

	CDumper::WriteLog((char *)"CFormatInitializer::InitLatmAAC");

	PatternFinder finder(0x56e0, 0xffe0); //high 11 bits == 0x2b7
	uint8* pEnd = pData + cbData;
	uint8* p1 = finder.Find(pData, pEnd);
	if (p1 == NULL)
		return;
	p1 -= 2;


	CLATMParser check;
	int len = cbData - (p1 - pData);
	
	if (len < MAX_LATM_HEADER_SIZE)
	{
		CDumper::WriteLog((char *)"data len < MAX_LATM_HEADER_SIZE!");
		return;
	}

	int ret = check.Parse(p1, len);
	if (ret)
	{
		CDumper::WriteLog((char *)"LATM check return, ret = %d...", ret);
		return;
	}
	// end

	if (pEnd - p1 < MAX_LATM_HEADER_SIZE)
	{
		CDumper::WriteLog((char *)"data len > MAX_LATM_HEADER_SIZE!");
		return;
	}
		
	if (*(p1 + 3) & 0x80) //the first bit is 'use_same_mux'
		return;

	CDumper::WriteLog((char *)"CFormatInitializer::InitLatmAAC, Go on");

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
    iLanguageLength = (strlen(escfg->chLanguage)>15)?15:(strlen(escfg->chLanguage));
    memcpy(info.audio.audio_language, escfg->chLanguage, iLanguageLength);

	info.id = escfg->elementary_PID;
	info.is_video = false;
	info.codec = VO_AUDIO_CodingAAC;
	info.max_frame_len = MAX_AAC_FRAME_SIZE;  
	info.extra_data = p1;
	info.extra_size = MAX_LATM_HEADER_SIZE;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_AUDIO;

    escfg->extension = new CBaseRepacker(m_pListener);
    m_pListener->OnNewStream(&info);
    SetAudioReady();
    m_ulReadyStreamCount++;
}

void CFormatInitializer::InitMP3(ESConfig* escfg, uint8* pData, uint32 cbData,uint32 syncword)
{
    int   iLanguageLength = 0;

	if(escfg != NULL && escfg->extension != NULL)
	{
		VOLOGI("The Stream is ready!");
		return;
	}

	CDumper::WriteLog((char *)"CFormatInitializer::InitMP3");

	const int MP3_HEAD_SIZE = 4; //4 byte head, including sync byte
	PatternFinder finder;///<mpeg's syncword is not unique,so must do more strict check to avoid taking normal data as header data 
	if (syncword == 0)
	{
		finder.SetPattern(0xfff0, 0xfff0); //we just check the 12 bits syncword 
	}
	else
	{
		finder.SetPattern(0xffe0, 0xffe0); //we just check the 11 bits syncword 
	}

	uint8* pEnd = pData+cbData;
	uint8* p1 = finder.Find(pData, pEnd);

	if (p1 == NULL)
	{
		return;
	}

	p1 -= 2;
	if (pEnd - p1 < MP3_HEAD_SIZE)
	{
		return;
	}

	int channels, samplerate, bitrate, framelen;
	bool b = ParseMP3Head(p1, MP3_HEAD_SIZE, samplerate, channels, bitrate, framelen);
	if (!b)
	{
		return;
	}

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	//info.codec = AC_MP2;
	info.is_video = false;
	info.codec = VO_AUDIO_CodingMP3;
	info.audio.channels = channels;
	info.audio.sample_bits = 16;
	info.audio.sample_rate = samplerate;
	info.extra_data = p1;
	info.extra_size = MP3_HEAD_SIZE;
    //info.max_frame_len = MAX_MP3_FRAME_SIZE;
	//more than a frame in a sample
	info.max_frame_len = MAX_MP3_FRAME_SIZE*8;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_AUDIO;

	iLanguageLength = (strlen(escfg->chLanguage)>15)?15:(strlen(escfg->chLanguage));
	memcpy(info.audio.audio_language, escfg->chLanguage, iLanguageLength);


#if 0
	CBaseRepacker* repacker = new CBaseRepacker(m_pListener);
#else
	uint32 pattern = (uint32)(*p1);///<once we got the header data,should take header data as pattern,it should be more accurate
	pattern = ((pattern<<8) + (*(p1+1)));
	CPatternRepacker* repacker= new CPatternRepackerFixedSize(m_pListener, pattern, 0xffff, 2, framelen - 1);  //padding 1 byte sometimes

	//For Test  2011-12-07
	repacker->SetPESPackMode(true);
	//For Test  2011-12-07

	//CBaseRepacker* repacker= new CPatternRepacker(m_pListener, 0xfffc, 0xfffe, 2);
	//repacker->UseDTS(packet->PTS_DTS_flags == 1);
#endif
	escfg->extension = repacker;

	m_pListener->OnNewStream(&info);
	SetAudioReady();
    m_ulReadyStreamCount++;
}


void CFormatInitializer::InitTeletext(ESConfig* escfg, uint8* pData, uint32 cbData)
{
	CDumper::WriteLog((char *)"CFormatInitializer::InitTeletext");
}

void CFormatInitializer::InitM2TSLPCM(ESConfig* escfg, uint8* pData, uint32 cbData)
{
    int   iLanguageLength = 0;

    if(escfg != NULL && escfg->extension != NULL)
    {
        VOLOGI("The Stream is ready!");
        return;
	}

	if (cbData < 4)
	{
		return;
	}
	const uint16 sample_bits[4] = { 0, 16, 20, 24 };///<2 bits
	const uint16 channels[16] = {0,1,0,2,3,3,4,4,5,6,7,8,0,0,0,0};///<4 bits

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.is_video = false;
	info.codec = VO_AUDIO_CodingPCM;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_AUDIO;

	info.audio.channels =channels[(pData[2] >> 4)&0x0F]; 
	switch (pData[2]&0x0F)
	{
	case 1:
		info.audio.sample_rate = 48000;
		break;
	case 4:
		info.audio.sample_rate = 96000;
		break;
	case 5:
		info.audio.sample_rate = 192000;
		break;
	default:
		info.audio.sample_rate = 0;
		break;
	}   
	info.audio.sample_bits = sample_bits[(pData[3]>>6)&0x03];
	if (!info.audio.channels || !info.audio.sample_rate || !info.audio.sample_bits)
	{
		return;
	}

	iLanguageLength = (strlen(escfg->chLanguage)>15)?15:(strlen(escfg->chLanguage));
	memcpy(info.audio.audio_language, escfg->chLanguage, iLanguageLength);

	info.extra_data = NULL;
	info.extra_size = 0;
	info.max_frame_len = (info.audio.sample_bits)/8*info.audio.channels*info.audio.sample_rate;

	uint32 pattern = pData[0]<<24 | pData[1]<<16 | pData[2]<<8 | (pData[3]&0xC0);
	uint32 mask = 0xFFFFFFC0;

	escfg->extension = new CPatternRepackerFixedSize(m_pListener, pattern, mask, 4,info.max_frame_len,false);

	m_pListener->OnNewStream(&info);
	SetAudioReady();
	m_ulReadyStreamCount++;	
}
void CFormatInitializer::InitAC3(ESConfig* escfg, uint8* pData, uint32 cbData)
{
    int   iLanguageLength = 0;
	if(escfg != NULL && escfg->extension != NULL)
	{
		VOLOGI("The Stream is ready!");
		return;
	}

	CDumper::WriteLog((char *)"CFormatInitializer::InitAC3");

	PatternFinder finder(0x0b77, 0xffff);
	uint8* p = finder.Find(pData, pData + cbData - 4);
	if (p == NULL)
		return;

	const int TAB_SAMPLE_RATE[] = {48000, 44100, 32000, 0};

	bit2 fscode = p[2] >> 6;
	
	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.is_video = false;
	info.codec = VO_AUDIO_CodingAC3;

#if 0
	info.audio.channels = TAB_CHANNELS[acmod];
#else //because our deocder always returns 2 
	info.audio.channels = 2; 
#endif
	info.audio.sample_rate = TAB_SAMPLE_RATE[fscode];;    
	info.audio.sample_bits = 16;
	iLanguageLength = (strlen(escfg->chLanguage)>15)?15:(strlen(escfg->chLanguage));
	memcpy(info.audio.audio_language, escfg->chLanguage, iLanguageLength);

	info.extra_data = NULL;
	info.extra_size = 0;
	info.max_frame_len = MAX_AC3_FRAME_SIZE;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_AUDIO;


	//modify by qichaoshen @2011-10-25

	//if (pData + 2 == p)  //PES includes whole packets, so we use simple repacker
	//	escfg->extension = new CBaseRepacker(m_pListener);
	//else //otherwise we should split packets
	//{
	//	CDumper::WriteLog("Warning: AC3 packets don't align with PES packets");
	//	escfg->extension = new CPatternRepacker(m_pListener, 0x0b77, 0xffff, 2);
	//}

	CPatternRepacker* pRepacker = new CPatternRepacker(m_pListener, 0x0b77, 0xffff, 2);
	//pRepacker->SetPESPackMode(false);
	pRepacker->SetPESPackMode(true);
	escfg->extension = pRepacker;

	//modify by qichaoshen @2011-10-25

	m_pListener->OnNewStream(&info);
	SetAudioReady();
    m_ulReadyStreamCount++;	
}



void CFormatInitializer::InitEAC3(ESConfig* escfg, uint8* pData, uint32 cbData)
{
    int   iLanguageLength = 0;

	if(escfg != NULL && escfg->extension != NULL)
	{
		VOLOGI("The Stream is ready!");
		return;
	}

    PatternFinder finder(0x0b77, 0xffff);
	uint8* p = finder.Find(pData, pData + cbData - 4);
	if (p == NULL)
		return;

	const int TAB_SAMPLE_RATE[] = {48000, 44100, 32000, 0};

	bit2 fscode = p[2] >> 6;

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.is_video = false;
	info.codec = VO_AUDIO_CodingEAC3;

	iLanguageLength = (strlen(escfg->chLanguage)>15)?15:(strlen(escfg->chLanguage));
	memcpy(info.audio.audio_language, escfg->chLanguage, iLanguageLength);

	info.extra_data = NULL;
	info.extra_size = 0;
	info.max_frame_len = MAX_AC3_FRAME_SIZE;
	info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_AUDIO;


	//modify by qichaoshen @2011-10-25

	//if (pData + 2 == p)  //PES includes whole packets, so we use simple repacker
	//	escfg->extension = new CBaseRepacker(m_pListener);
	//else //otherwise we should split packets
	//{
	//	CDumper::WriteLog("Warning: AC3 packets don't align with PES packets");
	//	escfg->extension = new CPatternRepacker(m_pListener, 0x0b77, 0xffff, 2);
	//}

	CPatternRepacker* pRepacker = new CPatternRepacker(m_pListener, 0x0b77, 0xffff, 2);
	//pRepacker->SetPESPackMode(false);
	pRepacker->SetPESPackMode(true);
	escfg->extension = pRepacker;

	m_pListener->OnNewStream(&info);
	SetAudioReady();
	m_ulReadyStreamCount++;	

}



void CFormatInitializer::InitDTS(ESConfig* escfg, uint8* pData, uint32 cbData)
{
    int   iLanguageLength = 0;

	if(escfg != NULL && escfg->extension != NULL)
	{
		VOLOGI("The Stream is ready!");
		return;
	}

	CDumper::WriteLog((char *)"CFormatInitializer::InitDTS");

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.is_video = false;
	info.codec = VO_AUDIO_CodingDTS;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_AUDIO;


#if 0
	info.audio.channels = TAB_CHANNELS[acmod];
#else //because our deocder always returns 2 
	info.audio.channels = 2; 
#endif
	info.audio.sample_rate = 48000;    
	info.audio.sample_bits = 16;
	iLanguageLength = (strlen(escfg->chLanguage)>15)?15:(strlen(escfg->chLanguage));
	memcpy(info.audio.audio_language, escfg->chLanguage, iLanguageLength);

	info.extra_data = NULL;
	info.extra_size = 0;
	info.max_frame_len = MAC_DTS_FRAME_SIZE;


	//modify by qichaoshen @2011-10-25

	//if (pData + 2 == p)  //PES includes whole packets, so we use simple repacker
	//	escfg->extension = new CBaseRepacker(m_pListener);
	//else //otherwise we should split packets
	//{
	//	CDumper::WriteLog("Warning: AC3 packets don't align with PES packets");
	//	escfg->extension = new CPatternRepacker(m_pListener, 0x0b77, 0xffff, 2);
	//}

	CPatternRepacker* pRepacker = new CPatternRepacker(m_pListener, 0x7ffe8001, 0xffffffff, 4);
	//pRepacker->SetPESPackMode(false);
	pRepacker->SetPESPackMode(true);
	escfg->extension = pRepacker;

	//modify by qichaoshen @2011-10-25

	m_pListener->OnNewStream(&info);
	SetAudioReady();
    m_ulReadyStreamCount++;	
}


void  CFormatInitializer::InitDvbSubTitle(ESConfig* escfg, uint8* pData, uint32 cbData)
{
    subtitling_descriptor *subtitle_des = NULL;
    uint8   uTypeId = 0;
    uint16 wCompositionPageId = 0;
    uint16 wAncillaryPageId = 0;
    uint32 ulISOLanguage = 0;
    
	if(escfg != NULL && escfg->extension != NULL)
	{
		VOLOGI("The Stream is ready!");
		return;
	}

	CDumper::WriteLog((char *)"CFormatInitializer::InitDvbSubTitle");

    if(escfg->descriptors != NULL)
	{
		subtitle_des = (subtitling_descriptor *)escfg->descriptors->FindFirst(0x59);
	}

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.is_video = false;
	info.max_frame_len = 65536;
	info.codec = (uint32)VO_SUBTITLE_CodingDVBSystem;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_SUBTITLE;
    if(subtitle_des != NULL)
	{
        subtitle_des->GetSubTitleItemInfoByIndx(0, &ulISOLanguage, &uTypeId, &wCompositionPageId, &wAncillaryPageId);
		info.subtitle.subtitle_language[0] = (uint8)(ulISOLanguage >> 16);
		info.subtitle.subtitle_language[1] = (uint8)(ulISOLanguage >> 8);
		info.subtitle.subtitle_language[2] = (uint8)(ulISOLanguage);
		info.subtitle.uSubTitleType = uTypeId;
		info.subtitle.ulPreserved1 = wCompositionPageId;
        info.subtitle.ulPreserved2 = wAncillaryPageId;
	}

	escfg->extension = new CPrivateDataRepacker(m_pListener);
	m_pListener->OnNewStream(&info);
	m_ulReadyStreamCount++;

}


void CFormatInitializer::InitMPEG2(ESConfig* escfg, uint8* pData, uint32 cbData)
{
    caption_service_descriptor*    pcaptionDesc = NULL;
	if(escfg != NULL && escfg->extension != NULL)
	{
		VOLOGI("The Stream is ready!");
		return;
	}

	if (videohead == NULL)
		return;
	if (!videohead->Process(pData, cbData))
		return;

	CDumper::WriteLog((char *)"CFormatInitializer::InitMPEG2");

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.is_video = true;
	info.codec = VO_VIDEO_CodingMPEG2;
	info.extra_data = videohead->GetHeadData();
	info.extra_size = videohead->GetHeadSize();
	

	uint8* dsidata = videohead->GetHeadData();
	info.video.width = ((*(dsidata + 4)) << 4) | ((*(dsidata + 5)) >> 4);
	info.video.height = (((*(dsidata + 5)) << 8) | ((*(dsidata + 6)))) & 0x0FFF;
	info.max_frame_len = info.video.width*info.video.height;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_VIDEO;

    if(escfg->descriptors != NULL)
	{
		pcaptionDesc = (caption_service_descriptor *)escfg->descriptors->FindFirst(0x86);
		if(pcaptionDesc != NULL)
		{
			info.pClosedCaptionDescData = pcaptionDesc->GetDescData();
			info.iClosedCaptionDescLen = pcaptionDesc->length;
		}
	}


#if 0
	CBaseRepacker* repacker = new CBaseRepacker(m_pListener);
#else
	CPatternRepacker* repacker= new CPatternRepacker(m_pListener, 0x00000100, 0xffffffff, 4);
	//repacker->UseDTS(packet->PTS_DTS_flags == 1);
#endif

	//
	repacker->SetPESPackMode(false);
    //repacker->SetPESPackMode(true);
	//
	escfg->extension = repacker;

	m_pListener->OnNewStream(&info);
	SetVideoReady();
    m_ulReadyStreamCount++;
}

void CFormatInitializer::InitMPEG4(ESConfig* escfg, uint8* pData, uint32 cbData)
{
	if (videohead == NULL)
		return;
	if (!videohead->Process(pData, cbData))
		return;

	CDumper::WriteLog((char *)"CFormatInitializer::InitMPEG4");

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.is_video = true;
	info.codec = VO_VIDEO_CodingMPEG4;
	info.extra_data = videohead->GetHeadData();
	info.extra_size = videohead->GetHeadSize();
	info.max_frame_len = MAX_MPEG4_FRAME_SIZE;

	uint8* dsidata = videohead->GetHeadData();
	info.video.width = ((*(dsidata + 4)) << 4) | ((*(dsidata + 5)) >> 4);
	info.video.height = (((*(dsidata + 5)) << 8) | ((*(dsidata + 6)))) & 0x0FFF;
	info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_VIDEO;

#if 0
	CBaseRepacker* repacker = new CBaseRepacker(m_pListener);
#else
	// tag: 20100524
	//CBaseRepacker* repacker= new CPatternRepacker(m_pListener, 0x000001, 0xffffff, 3);
	CBaseRepacker* repacker= new CMpeg4Repacker(m_pListener, 0x000001, 0xffffff, 3);
	//repacker->UseDTS(packet->PTS_DTS_flags == 1);
#endif
	escfg->extension = repacker;

	m_pListener->OnNewStream(&info);
	SetVideoReady();
	m_ulReadyStreamCount++;
}

void CFormatInitializer::InitH264(ESConfig* escfg, uint8* pData, uint32 cbData)
{
    caption_service_descriptor*    pcaptionDesc = NULL;

    if(escfg != NULL && escfg->extension != NULL)
    {
        VOLOGI("The Stream is ready!");
        return;
    }

	if (videohead == NULL)
		return;
	if (!videohead->Process(pData, cbData))
		return;

	CDumper::WriteLog((char *)"CFormatInitializer::InitH264");

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.is_video = true;
	info.codec = VO_VIDEO_CodingH264;
	info.max_frame_len = 0; // do not know
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_VIDEO;


#if 0
	info.video.width = 1280; //test
	info.video.height = 720; //test
#endif

	info.extra_data = videohead->GetHeadData();
	info.extra_size = videohead->GetHeadSize();



	VO_CODECBUFFER   varCodecBuffer;
	VO_VIDEO_HEADDATAINFO    varVideoHeaderDataInfo;
	VO_VIDEO_PROFILELEVEL    varVideoProfileLevel;
    
	varCodecBuffer.Buffer = (VO_PBYTE)info.extra_data;
	varCodecBuffer.Length = info.extra_size;


    if (getResolution_H264(&varCodecBuffer, &varVideoHeaderDataInfo) != 0)
	{
		VOLOGI("The  H264 Header is wrong!");
		return;
	}

	if(getProfileLevel_H264(&varCodecBuffer, &varVideoProfileLevel) != 0)
	{
		VOLOGI("The  H264 Header is wrong!");
		return;
	}


    info.video.width = varVideoHeaderDataInfo.Width;
	info.video.height = varVideoHeaderDataInfo.Height;

	if(escfg->descriptors != NULL)
	{
		pcaptionDesc = (caption_service_descriptor *)escfg->descriptors->FindFirst(0x86);
		if(pcaptionDesc != NULL)
		{
			info.pClosedCaptionDescData = pcaptionDesc->GetDescData();
			info.iClosedCaptionDescLen = pcaptionDesc->length;
		}
	}

#if 0
	if (packet->PTS_DTS_flags & 0x02) //??? 6/24/2008
		escfg->context = 0; //use PTS
	else 
		if (packet->PTS_DTS_flags & 0x01)
		escfg->context = 1; //
#endif


    CH264Repacker*  pH264Repacker = new CH264Repacker(m_pListener);
	pH264Repacker->SetPESPackMode(true);
	escfg->extension = pH264Repacker;

	VOLOGI("Find H264 Header Data, Header Data Length:%d", info.extra_size);

	m_pListener->OnNewStream(&info);
	SetVideoReady();
	m_ulReadyStreamCount++;
}


void CFormatInitializer::InitHEVC(ESConfig* escfg, uint8* pData, uint32 cbData)
{
	if(escfg != NULL && escfg->extension != NULL)
	{
		VOLOGI("The Stream is ready!");
		return;
	}

    if (videohead == NULL)
	{	
		return;
	}

    if (!videohead->Process(pData, cbData))
    {	
        return;
    }

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.is_video = true;
	info.codec = VO_VIDEO_CodingH265;
	info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_VIDEO;
	info.extra_data = videohead->GetHeadData();
	info.extra_size = videohead->GetHeadSize();
    info.max_frame_len = MAX_HEVC_FRAME_SIZE;


	escfg->extension = new CBaseRepacker(m_pListener);

	m_pListener->OnNewStream(&info);
    SetVideoReady();
	m_ulReadyStreamCount++;	
}



void CFormatInitializer::InitMetaDataID3(ESConfig* escfg, uint8* pData, uint32 cbData)
{
	if(escfg != NULL && escfg->extension != NULL)
	{
		VOLOGI("The Stream is ready!");
		return;
	}

	CDumper::WriteLog((char *)"CFormatInitializer::InitMetaDataID3");

	VOSTREAMPARSESTREAMINFO info;
	memset(&info, 0, sizeof(info));
	info.id = escfg->elementary_PID;
	info.is_video = false;
    info.max_frame_len = 65536;
    info.codec = VO_AUDIO_Coding_MAX;
    info.eStreamMediaType = VO_TS_STREAM_MEDIA_TYPE_PRIVATE;


	escfg->extension = new CPrivateDataRepacker(m_pListener);

	m_pListener->OnNewStream(&info);
	m_ulReadyStreamCount++;	
}


void  CFormatInitializer::SetOpenFlag(uint32 ulOpenFlag)
{
	m_ulOpenFlag = ulOpenFlag;
}
