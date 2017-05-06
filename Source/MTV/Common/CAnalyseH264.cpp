#include "CAnalyseH264.h"
#include <stdio.h>
#include <string.h>

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
/*buffer:the data  at the beginning of  NALU
2 : I frame
0 : P frame
1 : B frame
-1: error or other NALU
*/
int CAnalyseH264::GetFrameFormat (VO_BYTE* buffer, int nBufSize)
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
int CAnalyseH264::GetFrameType (VO_BYTE* pInData, int nInSize)
{
	long	m_lH264Format = 0;//VOH264_ANNEXB=0;
	VO_U32	m_dwNALWord = 0X01000000;
	VO_BYTE*	pBuffer = pInData;
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
int CAnalyseH264::GetFrameType2 (VO_BYTE* pInData, int nInSize)
{
	long	m_lH264Format = 0;//VOH264_ANNEXB=0;
	VO_U32	m_dwNALWord = 0X010000;
	VO_BYTE*	pBuffer = pInData;
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


CAnalyseH264::CAnalyseH264(void)
{
}

CAnalyseH264::~CAnalyseH264(void)
{
}

VO_BOOL	CAnalyseH264::AnalyseData(VO_BYTE *pData , VO_U32 nDataLen)
{
	VO_PBYTE pBegin = pData;
	VO_U32 nSeekLen = 400;
	if(nDataLen < nSeekLen)
	{
		nSeekLen = nDataLen;
	}

	VO_BYTE *	pSequenceSets = NULL;
	VO_BYTE *	pPictureSets = NULL;
	VO_BYTE *	pSyncPoint = NULL;

	VO_BOOL bHeadEnd = VO_FALSE;
	while(nSeekLen > 4)
	{
		pSyncPoint = FindSyncPos(pData , nSeekLen);
		if(pSyncPoint == NULL)
			return VO_FALSE;

		nSeekLen -= (pSyncPoint - pData) ;

		if((pSyncPoint[mnSyncLen] & 0x1F) == 0x7)
		{
			pSequenceSets = pSyncPoint ;
		}
		else if((pSyncPoint[mnSyncLen]&0x1F) == 0x8)
		{
			pPictureSets = pSyncPoint;
		}

		if((pSyncPoint[mnSyncLen] & 0x1F) != 0x7 && (pSyncPoint[mnSyncLen] & 0x1F) != 0x8)
		{
			bHeadEnd = VO_TRUE;
			break;
		}

		nSeekLen -= mnSyncLen;
		pData = pSyncPoint + mnSyncLen;
	}

	if (!pSequenceSets)
	{
		return VO_FALSE;
	}

	if (bHeadEnd)
	{
		mnSeqLen = pSyncPoint - pSequenceSets;
	}
	else
		mnSeqLen = pBegin + nDataLen - pSequenceSets;
	

	if(mpSeqHeader != NULL)
	{
		delete [] mpSeqHeader;
		mpSeqHeader = NULL;
	}

	mpSeqHeader = new VO_BYTE[mnSeqLen];
	if(mpSeqHeader == NULL)
		return VO_FALSE;

	memcpy(mpSeqHeader , pSequenceSets , mnSeqLen);

	return VO_TRUE;
}

VO_BOOL	CAnalyseH264::IsKeyFrame(VO_BYTE *pData , VO_U32 nDataLen)
{
	if(!pData)
		return VO_FALSE;

	int ret = GetFrameType(pData, nDataLen);
	if(0 != ret)
		ret = GetFrameType2(pData, nDataLen);
	return 0==ret?VO_TRUE:VO_FALSE;


	VO_BYTE* pEnd = pData + nDataLen;
	VO_BYTE  *pTemp = FindSyncPos(pData , nDataLen);

	if (!pTemp)
	{
		return VO_FALSE;
	}

	nDataLen -= (pTemp - pData);
	nDataLen -= mnSyncLen;

	pData = pTemp + mnSyncLen;

	if (!pData || pData>pEnd)
	{
		return VO_FALSE;
	}

	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len;
	int info_bit;
	int totbitoffset = 0;
	int naluType = pData[0]&0x0f;
	if(naluType==5)
		return VO_TRUE;
	if(naluType==1)//need continuous check
		pData++;
// 	else//the nalu type is params info
// 		return VO_FALSE;
	for(i=0;i<2;i++)
	{
		byteoffset= totbitoffset/8;
		bitoffset= 7-(totbitoffset%8);
		ctr_bit = (pData[byteoffset] & (0x01<<bitoffset));   // set up control bit

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
			ctr_bit=pData[byteoffset] & (0x01<<(bitoffset));
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
			if(pData[byteoffset] & (0x01<<(bitoffset)))
				inf |=1;
		}
		totbitoffset+=len*2-1;
		if(totbitoffset>48)
			return VO_FALSE;
	}
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if(inf>=5)
		inf-=5;

	if(inf == 2)
		return VO_TRUE;
	else
		return VO_FALSE;;
}
