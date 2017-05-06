#include "framechk.h"
#include "mp4cfg.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


//------------------------------------------------------------------------

/*buffer:the data  at the beginning of  NALU
  		1 : I frame
  		0 : P frame or other NALU
*/
int IsH264IntraFrameForTS(uint8 * buffer, int /* bufLen */)
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
	if(naluType==5)
		return 1;
	if (naluType == 7) //sequence header, tream as I frame, Jason Gao, 3/24/2008
		return 1;
	if(naluType==1)//need continuous check
		buffer++;
	else//the nalu type is params info
		return 0;
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
			return 0;
	}
	//(int)pow(2,(bitsUsed/2))+info-1;//pow(2,x)==1<<x
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if(inf==2||inf==7)//I frame
		return 1;
	else if (inf!=0&&inf!=5)//P frame
	{
		//parsing error!!!!!
		//printf("Parsing the IsIntraH264 data fail\n");
		return 0;
	}

	return 0;           
}



/*buffer:the data  at the beginning of  NALU
2 : I frame
0 : P frame
1 : B frame
-1: error or other NALU
*/
int GetFrameFormat (uint8* buffer, int nBufSize)
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
int GetFrameType_I (uint8* pInData, int nInSize)
{
	long	m_lH264Format = 0;//VOH264_ANNEXB=0;
	uint32	m_dwNALWord = 0X01000000;
	uint8*	pBuffer = pInData;
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

			if (nFormat >= 0)
				break;
			else if (nFormat == -1)
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


// return -1 Unknown 0, I, 1, P, 2 B,
int GetFrameType2 (uint8* pInData, int nInSize)
{
	long	m_lH264Format = 0;//VOH264_ANNEXB=0;
	uint32	m_dwNALWord = 0X010000;
	uint8*	pBuffer = pInData;
	int		nBufSize = nInSize;

	int		nNALSize = 3;

	int nFormat = -1;
	int nFrameCount = 0;

	while (nBufSize > nNALSize)
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
			while (pBuffer - pInData < nInSize - nNALSize)
			{
				if (!memcmp (pBuffer, &m_dwNALWord, nNALSize))
					break;
				pBuffer++;
			}

			pBuffer += nNALSize;
			nBufSize -= nNALSize;
			nFormat = GetFrameFormat (pBuffer, nBufSize);

			if (nFormat >= 0)
				break;
			else if (nFormat == -1)
			{
				while (pBuffer - pInData < nInSize - nNALSize)
				{
					if (!memcmp (pBuffer, &m_dwNALWord, nNALSize))
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





using namespace MP4;

class FrameCheckerAVC : public FrameChecker
{
public:
	FrameCheckerAVC(int naluSizeLen)
		: _naluSizeLen(naluSizeLen)
	{
	}

	virtual int Split(uint8* pData, int cData, int* sizes, uint8*& pStart)
	{
		uint8* p = pStart = pData;
		int size = 0;
		for (int i = 0; i < _naluSizeLen; i++)
		{
			size <<= 8;
			size += *p++;
		}
#ifdef _DEBUG
		if (size + _naluSizeLen == cData)
			return 1;
		else
			return 0;
#else //_DEBUG
		return size + _naluSizeLen == cData ? 1 : 0;
#endif //_DEBUG
	}

	virtual bool IsSync(uint8* pData, int cData)
	{
		return 1 == IsH264IntraFrameForTS(pData + _naluSizeLen, cData - _naluSizeLen);
	}

private:
	int _naluSizeLen;
};

class FrameCheckerH264 : public FrameChecker
{
public:
	virtual bool IsSync(uint8* pData, int cData)
	{
		// tag: 20100501
		//return 0 == GetFrameType(pData, cData);
		int ret = GetFrameType_I(pData, cData);
		if(0 != ret)
			ret = GetFrameType2(pData, cData);
		return 0==ret;
		//end 

		// modified by gtxia 2007-9-15 
		if(pData)
		{
//			return 1 == IsH264IntraFrame(pData + 3, cData - 3);
			int nRC = 0;
			if (memcmp (pData, &m_lSyncWord, 4))
				nRC = IsH264IntraFrameForTS(pData + 3, cData - 3);
			else
				nRC = IsH264IntraFrameForTS(pData + 4, cData - 4);

			return nRC == 1;
		}
		else
			return false;
	}

/*
	virtual int Split(uint8* pData, int cData, int* sizes, uint8*& pStart)
	{
		uint8* pCurLast = pData;
		uint8* pEnd = pData+cData;
		uint8* pCur = pCurLast;
		int   iFrameInterval = 0;
        int   iSyncWordLen = 4;

		//    the size array's max size is 128
        int   iMaxFrameCount = 126;
		pStart = pData;

		PatternFinder m_finder(0x00000109, 0xffffffff);



		while(pCur != pEnd)
		{
			pCur = m_finder.Find(pCurLast, pEnd);
			if(pCur == NULL)
			{
				if(iFrameInterval == 0)
				{
					//have no  00 00 01 09
					sizes[iFrameInterval] = cData;
				}
				else
				{
					//include the 00 00 01 09
					sizes[iFrameInterval] = pEnd - pCurLast + iSyncWordLen; 
				}

				return (iFrameInterval+1); 
			}
			if(pCur == (pStart+4))  //at the Frame header
			{
				pCurLast = pCur;
                iSyncWordLen = 4;
			}
			else
			{
				if(pCur == (pStart+5))
				{
					pCurLast = pCur;
					iSyncWordLen = 5;
				}
				else
				{
					if(iFrameInterval == 0)
					{
						sizes[iFrameInterval] = pCur - pStart - iSyncWordLen;
					}
					else
					{
						sizes[iFrameInterval] = pCur - pCurLast;
					}

					pCurLast = pCur;
					iFrameInterval++;

					if(iFrameInterval >= iMaxFrameCount)
					{
						return iFrameInterval;
					}
				}
			}
		}
	}
*/

};


class FrameCheckerAdtsAAC : public FrameChecker
{

};

class FrameCheckerH265 : public FrameChecker
{
public:
	virtual bool IsSync(uint8* pData, int cData)
	{
        uint8     uNalType = 0xff;
		uint8*    pCur = pData;
        uint8*    pEnd = pData+cData;
		uint8*    pNalHeader = NULL;
		PatternFinder sfinder;
        sfinder.SetPattern(0x000001, 0xffffff);
        sfinder.Reset();
		while(pCur != NULL && pCur != pEnd)
        {
			pNalHeader = sfinder.Find(pCur, pEnd);
			if(pNalHeader != NULL)
            {
				uNalType = (((*pNalHeader)>>1) & (0x3f));
				if(uNalType >= 0 && uNalType <= 15)
                {
					return false;
				}

				if(uNalType >= 16 && uNalType <= 21)
				{
					return true;
				}

				pCur = pNalHeader+3;
			}
			else
			{
				return false;
			}
        }

		return false;
	}
};

class FrameCheckerBSAC : public FrameChecker
{
public:
	virtual int Split(uint8* pData, int cData, int* sizes, uint8*& pStart)
	{
		int split = 0;
		uint8* p = pStart = pData;
		int count = cData;
		while (count > 0)
		{
			int size = *p++;
			size <<= 3;
			size += *p-- >> 5;
			sizes[split++] = size;
			count -= size;
			p += size;
		}
		if (count == 0)
			return split;
		else
			return 0;
	}
};


class FrameCheckerAC3 : public FrameChecker
{
public:
	virtual int Split(uint8* pData, int cData, int* sizes, uint8*& pStart)
	{

	const short TAB_FRAME_SIZE[3][38] =
	{	{	64, 64, 80, 80, 96, 96, 112, 112, 
			128, 128, 160, 160, 192, 192, 224, 224,
			256, 256, 320, 320, 384, 384, 448, 448,
			512, 512, 640, 640, 768, 768, 896, 896,
			1024, 1024, 1152, 1152, 1280, 1280 },
		{	69, 70, 87, 88, 104, 105, 121, 122,
			139, 140, 174, 175, 208, 209, 243, 244,
			278, 279, 348, 349, 417, 418, 487, 488,
			557, 558, 696, 697, 835, 836, 975, 976,
			1114, 1115, 1253, 1254, 1393, 1394 },
		{	96, 96, 120, 120, 144, 144, 168, 168,
			192, 192, 240, 240, 288, 288, 336, 336,
			384, 384, 480, 480, 576, 576, 672, 672,
			768, 768, 960, 960, 1152, 1152, 1344, 1344,
			1536, 1536, 1728, 1728, 1920, 1920 } };

		int split = 0;
		uint8* p = pStart = pData;
		int count = cData;
		while (count > 0)
		{
			bit16 sync = *p++;
			sync <<= 8;
			sync |= *p++;
			if (sync != 0x0b77)
				break;
			p++;
			p++; //CRC
			bit2 fscode = *p >> 6;
			bit6 frmsizecod = *p & 0x3f;
			if ((fscode >= 3) || (frmsizecod >= 38)) //error
				break;
			int size = TAB_FRAME_SIZE[fscode][frmsizecod] * 2;
			sizes[split++] = size;
			count -= size;
			p += size - 4;
#ifdef _DEBUG
			if (size > 2048)
				break;
#endif //_DEBUG
		}
		if (count == 0)
			return split;
		else
			return 0;
	}
};

class FrameCheckerMPEG2 : public FrameChecker
{
public:
	virtual int Split(uint8* pData, int cData, int* sizes, uint8*& pStart)
	{
		// 00 00 01
		uint8* p = pStart = pData;
		if (*p++)
			return 0;
		if (*p++)
			return 0;
		if (*p++ != 0x01)
			return 0;
		return 1;
	}

	virtual bool IsSync(uint8* pData, int cData)
	{
		uint8 b4 = *(pData + 3);
		if (b4 == 0xb3)
			return true;
		//00 00 01 00 skip10 bits 3bits(1:Iframe 2:Pframe 3:Bframe) 
		if (b4) //must 01 00
			return false;
		uint8 b6 = *(pData + 5);
		return (b6 & 0x38) == 0x08;
	}
};

class FrameCheckerMPEG4 : public FrameChecker
{
public:
	virtual int Split(uint8* pData, int cData, int* sizes, uint8*& pStart)
	{
		pStart	= pData;
		*sizes	= 1;

		return 1;
	}

	virtual bool IsSync(uint8* pData, int cData)
	{
		PatternFinder finder;
		finder.SetPattern(0x000001B6);
		uint8 * pTmp = finder.Find(pData,pData+cData);
		if (!pTmp)
		{
			return false;
		}

		if(!((*pTmp) & 0xc0))
		{
			return true;
		}
		return false;
	}
};


FrameChecker* FrameChecker::CreateChecker(int dcdObjectType, uint8* dsiData, int dsiSize)
{
	switch (dcdObjectType)
	{
	case 0x21: //AVC
		{
			AVCDecoderConfigurationRecord cfg;
			if (cfg.Load(dsiData, dsiSize))
				return new FrameCheckerAVC(cfg.lengthSizeMinusOne + 1);
		}
		break;

	case 0x40: //AAC
		{
			AudioSpecificConfig cfg;
			if (cfg.Load(dsiData, dsiSize))
				if (cfg.audioObjectType == 22)
					return new FrameCheckerBSAC();
		}
		break;

	default:
		break;
	}

	return new FrameChecker();
}


class FrameCheckerLATM : public FrameChecker
{
public:
	FrameCheckerLATM(CLATMParser* parser)
		: _parser(parser)
	{
	}

	~FrameCheckerLATM()
	{
		delete _parser;
	}

	virtual int Split(uint8* pData, int cData, int* sizes, uint8*& pStart)
	{
		if (_parser->Parse(pData, cData))
			return 0; //error
		int n = _parser->GetFramesInfo(sizes, pStart);
		return n;
	}

private:
	CLATMParser* _parser;
};


FrameChecker* FrameChecker::CreateCheckerH264()
{
	return new FrameCheckerH264();
}

FrameChecker* FrameChecker::CreateCheckerH265()
{
    return new FrameCheckerH265();
}

FrameChecker* FrameChecker::CreateCheckerAdtsAAC()
{
	return new FrameCheckerAdtsAAC();
}

FrameChecker* FrameChecker::CreateCheckerAVC(int naluLengthSize)
{
	return new FrameCheckerAVC(naluLengthSize);
}

FrameChecker* FrameChecker::CreateCheckerBSAC()
{
	return new FrameCheckerBSAC();
}

FrameChecker* FrameChecker::CreateCheckerDefault()
{
	return new FrameChecker();
}

FrameChecker* FrameChecker::CreateCheckerMPEG2()
{
	return new FrameCheckerMPEG2();
}
FrameChecker* FrameChecker::CreateCheckerMPEG4()
{
	return new FrameCheckerMPEG4();
}
FrameChecker* FrameChecker::CreateCheckerAC3()
{
	return new FrameCheckerAC3();
}

FrameChecker* FrameChecker::CreateCheckerLATM(CLATMParser* parser)
{
	return new FrameCheckerLATM(parser);
}

int FrameChecker::Split(uint8* pData, int cData, int* sizes, uint8*& pStart) 
{ 
	// tag: 20100413
	pStart	= pData;
	*sizes	= 1;
	//end

	return 1; 
}
