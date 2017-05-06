#include "CAnalyseH264.h"
#include <stdio.h>
#include <string.h>
#include "ParseSPS.h"

#define LOG_TAG "CAnalyseH264"
#include "voLog.h"

CAnalyseH264::CAnalyseH264(void)
: mnSPSLen(0)
, mnPPSLen(0)
, mnStreamType(0)
, mpData(NULL)
, mnDataLen(0)
, mnWritedLen(0)
, mnNestDeep(0)
{
}

CAnalyseH264::~CAnalyseH264(void)
{
	if(mpData != NULL)
	{
		delete mpData;
		mpData = NULL;
	}
}

void   CAnalyseH264::SetFormat(void *pFmt)
{
	VO_VIDEO_FORMAT *pVF = (VO_VIDEO_FORMAT *)pFmt;
	mVideoInfoHeader.bmiHeader.biWidth = pVF->Width;
	mVideoInfoHeader.bmiHeader.biHeight = pVF->Height;
}

VO_BOOL	CAnalyseH264::AnalyseData(VO_BYTE *pData , VO_U32 nDataLen)
{
	if( (pData[0] & 0x0f) == 0x07 && nDataLen < 50)
	{
		mpSeqHeader = new VO_BYTE[100];

		memset(mpSeqHeader , 0 , 100);
		mpSeqHeader[3] = 1;
		memcpy(mpSeqHeader + 4  , pData , nDataLen);
		mnSeqLen = nDataLen + 4;

		return VO_FALSE;
	}
	else if((pData[0] & 0x0F) == 0x08 && nDataLen < 50)
	{
		mpSeqHeader[mnSeqLen + 3] = 1;
		memcpy(mpSeqHeader + mnSeqLen + 4 , pData , nDataLen);
		mnSeqLen += nDataLen + 4;

		mnStreamType = ST_RAW;

		return VO_TRUE;
	}

	VO_BYTE *pOrgP = pData;
	VO_U32 nSeekLen = 400;
	if(nDataLen < nSeekLen)
	{
		nSeekLen = nDataLen;
	}

	VO_BYTE *	pSequenceSets = NULL;
	VO_BYTE *	pPictureSets = NULL;
	VO_BYTE *	pSyncPoint = NULL;

	while(nSeekLen > 4)
	{
		pSyncPoint = FindSyncPos(pData , nSeekLen);
		if(pSyncPoint == NULL)
			break;

		nSeekLen -= (pSyncPoint - pData) ;

		if((pSyncPoint[mnSyncLen] & 0x0f) == 0x07)
		{
			pSequenceSets = pSyncPoint ;
		}
		else if((pSyncPoint[mnSyncLen]  & 0x0f) == 0x08)
		{
			pPictureSets = pSyncPoint;
		}

		if((pSyncPoint[mnSyncLen]  & 0x0f) != 0x07 && (pSyncPoint[mnSyncLen]  & 0x0f) != 0x08 && pPictureSets != NULL)
			break;

		nSeekLen -= mnSyncLen;
		pData = pSyncPoint + mnSyncLen;
		pSyncPoint = NULL;
	}

	if(pSyncPoint != NULL && pSequenceSets != NULL)
	{
		mnSeqLen = pSyncPoint - pSequenceSets;
	}
	else if(pSequenceSets != NULL)
	{
		mnSeqLen = nDataLen - (pSequenceSets - pOrgP);
	}
	else 
	{
		return VO_FALSE;
	}
	

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
	VO_BOOL	bIsKey = IsKeyFrm(pData , nDataLen);

	//ChangeDataHeader(pData , nDataLen);

	return bIsKey;
}

//change ST_H264 to ST_AVC4
void	CAnalyseH264::STH264ToAVC(VO_BYTE **ppData , VO_U32 &nDataLen)
{
#if 1
	int nPadLen = 0;
	VO_BYTE * pData = *ppData;
	VO_U32  nTempLen = nDataLen;
	VO_BYTE *pSync0 = FindSyncPos(pData , nTempLen);
	if(pSync0 == NULL)
		return;

	nPadLen = pSync0 - pData;
	nTempLen -= nPadLen;
	VO_U32 nPackSize = 0;
	bool	bUseInnerBuf = false;

	pData += (mnSyncLen + 1);
	nTempLen -= (mnSyncLen + 1);
	
	VO_BYTE *pSync1 = NULL;
	do 
	{
		pSync1 = FindSyncPos(pData , nTempLen);
		if(pSync1 != NULL)
		{
			nPackSize = pSync1 - pSync0;

			pData = pSync1 + mnSyncLen + 1;
			nTempLen = nDataLen - (pData -  *ppData);

			if(nPackSize <= 6 && pSync0 == *ppData + nPadLen)
			{
				nPadLen += nPackSize;
			}
			else
			{
				if(mnSyncLen == 4)
				{
					WriteDataLen(pSync0 , nPackSize - mnSyncLen );
				}
				else
				{
					bUseInnerBuf = true;
					WriteRawDataToBuf(pSync0 + mnSyncLen , nPackSize - mnSyncLen);
				}
			}

			pSync0 = pSync1;
		}
		else
		{
			nPackSize = nDataLen - (pSync0 - *ppData) ;
			if(mnSyncLen == 4)
			{
				WriteDataLen(pSync0 , nPackSize - mnSyncLen );
			}
			else
			{
				bUseInnerBuf = true;
				WriteRawDataToBuf(pSync0 + mnSyncLen , nPackSize - mnSyncLen);
			}
		}
	} while (pSync1 != NULL);

	if(bUseInnerBuf)
	{
		*ppData = mpData;
		nDataLen = mnWritedLen;

		mnWritedLen = 0;
	}
	else
	{
		nDataLen -= nPadLen;
		*ppData = *ppData + nPadLen;
	}

#else
	VO_BYTE * pData = *ppData;
	VO_U32   nTempLen = nDataLen;

	VO_BYTE  *pTemp = FindSyncPos(pData , nTempLen);
	if(pTemp == NULL)
		return ;

	if(pTemp[mnSyncLen] == 0x67)
	{
		VO_BYTE *	pSequenceSets = pTemp;
		VO_BYTE *	pPictureSets = NULL;

		pData = pTemp + mnSyncLen;
		nTempLen = nDataLen - (pTemp - *ppData) - mnSyncLen;

		pTemp = FindSyncPos(pData , nTempLen);
		if(pTemp == NULL)
			return ;
		
		int nSeqLen = pTemp - pSequenceSets;
		nSeqLen -= mnSyncLen;
			
		WriteDataLen(pSequenceSets , nSeqLen);

		if(pTemp[mnSyncLen] == 0x68)
		{
			pPictureSets = pTemp;

			pData = pTemp + mnSyncLen;
			nTempLen = nDataLen - (pTemp - *ppData) - mnSyncLen;

			pTemp = FindSyncPos(pData , nTempLen);
		}

		if(pTemp == NULL)
			return ;

		if(pPictureSets != NULL)
		{
			int nPicLen = pTemp - pPictureSets;
			nPicLen -= mnSyncLen;

			WriteDataLen(pPictureSets , nPicLen);
		}
	}

	int nPadLen = pTemp - *ppData;
	WriteDataLen(pTemp , nDataLen - nPadLen - mnSyncLen );
	nDataLen -= nPadLen;
	*ppData = *ppData + nPadLen;
#endif 
}

void	CAnalyseH264::WriteDataLen(VO_BYTE *pData , VO_U32 nDataLen)
{
	pData[3] = (VO_BYTE)(nDataLen & 0xFF);
	pData[2] = (VO_BYTE)((nDataLen & 0xFF00) >> 8);
	pData[1] = (VO_BYTE)((nDataLen & 0xFF0000) >> 16);
	pData[0] = (VO_BYTE)((nDataLen & 0xFF000000) >> 24);
}

bool	CAnalyseH264::AllocFrameBuf()
{
	if(mpData == NULL)
	{
		if(mVideoInfoHeader.bmiHeader.biWidth != 0)
			mnDataLen = mVideoInfoHeader.bmiHeader.biWidth * mVideoInfoHeader.bmiHeader.biHeight / 2;
		else 
			mnDataLen = 512 * 1024;

		mpData = new VO_BYTE[mnDataLen];
		if(mpData == NULL)
			return false;
	}

	return true;
}

bool	CAnalyseH264::WriteRawDataToBuf(VO_BYTE *pData , VO_U32 nDataLen)
{
	if(mpData == NULL)
	{
		if(!AllocFrameBuf())
			return false;
	}

	WriteDataLen(mpData + mnWritedLen , nDataLen);

	if(nDataLen + mnWritedLen + 4 > mnDataLen)
	{
		//VOLOGI("Allocate buffer too small buffer size : %d  need buffer : %d " , (int)mnDataLen , (int)(nDataLen + mnWritedLen + 4));
		return false;
	}

	memcpy(mpData + mnWritedLen + 4 , pData , nDataLen);
	mnWritedLen += (nDataLen +4);

	return true;
}

void    CAnalyseH264::STRawToAVC(VO_BYTE **ppData , VO_U32 &nDataLen)
{
	if(!WriteRawDataToBuf(*ppData , nDataLen))
		return ;

	*ppData = mpData;
	nDataLen += 4;

	mnWritedLen = 0;
}


VO_U32 CAnalyseH264::TrimSample(VO_BYTE **ppData , VO_U32 &nDataLen)
{
	if(mnStreamType == 0)
	{
		DetectStreamType(*ppData , nDataLen);
	}

	switch(mnStreamType)
	{
	case ST_AVC4:
		{
			break;
		}
	case ST_AVC2:
		{
			break;
		}
	case ST_H264:
		{
			STH264ToAVC(ppData , nDataLen);
			break;
		}
	case ST_RAW:
		{
			STRawToAVC(ppData , nDataLen);
			break;
		}
	}

	return 0;
}

void CAnalyseH264::DetectStreamType(VO_BYTE *pData , VO_U32 nDataLen)
{
	mnNestDeep ++;
	if(pData[0] == 0x0 && pData[1] == 0x0 && (pData[2] == 0x1 || (pData[2] == 0x0 && pData[3] == 0x1)))
	{
		mnStreamType = ST_H264;
		mnNestDeep --;
		return;
	}

	VO_U32 nLen = pData[0];
	nLen = (nLen << 8) + pData[1];

	VO_U32 nLen1 = (nLen << 16) + (pData[2] << 8) + pData[3];


	if(nLen == nDataLen - 2)
	{
		mnNestDeep --;
		mnStreamType = ST_AVC2;
		return;
	}
	else if(nLen1 ==  nDataLen - 4)
	{
		mnNestDeep --;
		mnStreamType = ST_AVC4;
		return;
	}
	else if(nLen > nDataLen)
	{
		mnNestDeep --;
		mnStreamType = ST_RAW;
		return ;
	}
	else 
	{
		DetectStreamType(pData + 2 + nLen , nDataLen - 2 - nLen);
		if(mnStreamType == ST_RAW)
			DetectStreamType(pData + 4 + nLen , nDataLen - 4 - nLen);
	}
}

VO_BOOL		CAnalyseH264::IsKeyFrm(VO_BYTE *pData , VO_U32 nDataLen)
{
	int nFrameType = GetFrameType((char *)pData , nDataLen);
	if(nFrameType == 0)
		return VO_TRUE;

	return VO_FALSE;

	//if(mnStreamType == 0)
	//{
	//	DetectStreamType(pData , nDataLen);
	//}

	//if(mnStreamType == ST_H264)
	//{
	//	VO_BYTE  *pTemp = FindSyncPos(pData , nDataLen);
	//	if(pTemp == NULL)
	//		return VO_FALSE;

	//	if(pTemp[mnSyncLen] == 0x67)
	//		return VO_TRUE;

	//	nDataLen -= (pTemp - pData);
	//	nDataLen -= mnSyncLen;

	//	pData = pTemp + mnSyncLen;
	//}
	//else if(mnStreamType == ST_AVC2)
	//{
	//	pData += 2;
	//}
	//else if(mnStreamType == ST_AVC4)
	//{
	//	pData += 4;
	//}

	//int inf,i;
	//long byteoffset;      // byte from start of buffer
	//int bitoffset;      // bit from start of byte
	//int ctr_bit=0;      // control bit for current bit posision
	//int bitcounter=1;
	//int len;
	//int info_bit;
	//int totbitoffset = 0;
	//int naluType = pData[0]&0x0f;
	//int frameType=0;
	//if(naluType==5)
	//	return VO_TRUE;
	//if(naluType==1)//need continuous check
	//	pData++;
	//else//the nalu type is params info
	//	return VO_FALSE;
	//for(i=0;i<2;i++)
	//{
	//	byteoffset= totbitoffset/8;
	//	bitoffset= 7-(totbitoffset%8);
	//	ctr_bit = (pData[byteoffset] & (0x01<<bitoffset));   // set up control bit

	//	len=1;
	//	while (ctr_bit==0)
	//	{                 // find leading 1 bit
	//		len++;
	//		bitoffset-=1;           
	//		bitcounter++;
	//		if (bitoffset<0)
	//		{                 // finish with current byte ?
	//			bitoffset=bitoffset+8;
	//			byteoffset++;
	//		}
	//		ctr_bit=pData[byteoffset] & (0x01<<(bitoffset));
	//	}
	//	// make infoword
	//	inf=0;                          // shortest possible code is 1, then info is always 0
	//	for(info_bit=0;(info_bit<(len-1)); info_bit++)
	//	{
	//		bitcounter++;
	//		bitoffset-=1;
	//		if (bitoffset<0)
	//		{                 // finished with current byte ?
	//			bitoffset=bitoffset+8;
	//			byteoffset++;
	//		}

	//		inf=(inf<<1);
	//		if(pData[byteoffset] & (0x01<<(bitoffset)))
	//			inf |=1;
	//	}
	//	totbitoffset+=len*2-1;
	//	if(totbitoffset>48)
	//		return VO_FALSE;
	//}
	//len = (len*2-1)/2;
	//inf = (1<<len)+inf-1;
	//if(inf>=5)
	//	inf-=5;

	//if(inf == 2)
	//	return VO_TRUE;
	//else
	//	return VO_FALSE;
}

