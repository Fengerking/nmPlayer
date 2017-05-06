/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAacParser.cpp

	Contains:	CAacParser class file

	Written by:	Aiven

	Change History (most recent first):
	2012-04-11		Aiven			Create file

*******************************************************************************/
#include "CAacParser.h"
#include "fMacros.h"
#include "voString.h"
#include "voLog.h"
#include "voAAC2.h"
#include <stdio.h>

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define D_VOLOG_INFO
#define MAX_PUSH_BLOCK		6
#define MAX_PUSH_SIZE		1024					//1KB
#define MAX_SAMPLE_SIZE		(MAX_PUSH_SIZE*64)		//64KB

DEFINE_USE_AAC_GLOBAL_VARIABLE(CAacParser)


CAacParser::CAacParser()
:m_pID3Parser(NULL)
,m_bIsNeedToParseHeadData(VO_TRUE)
,m_btChannels(0)
,m_btSampleRateIndex(0)
,m_btProfile(0)
,m_dSampleTime(0)
,m_dTimeStamp(0)
//,m_dwMaxSampleSize(MAX_SAMPLE_SIZE)
{
	VOLOGI( "CAacParser");
	m_pID3Parser = new CID3Parser();
}


CAacParser::~CAacParser()
{
	VOLOGI( "~CAacParser");

	Reset();
	
	if(m_pID3Parser){
		delete m_pID3Parser;
	}

}



VO_U32 CAacParser::Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_SAMPLECALLBACK * pCallback )
{
	VOLOGI("Init");
	CBasePushPaser::Init(pSource, nFlag, pCallback);
	m_pID3Parser->Init(pSource, nFlag, pCallback);
	return VO_RET_SOURCE2_OK;
}

VO_U32 CAacParser::Uninit()
{
	VOLOGI( "Uninit");
	m_pID3Parser->Uninit();

	return VO_RET_SOURCE2_OK;
}

VO_VOID CAacParser::Reset()
{
	VOLOGR( "Reset");

	CBasePushPaser::Reset();
		
	m_btChannels = 0;
	m_btSampleRateIndex = 0;
	m_btProfile = 0;
	m_dSampleTime = 0;
	m_dTimeStamp = 0;
	m_bIsNeedToParseHeadData = VO_TRUE;

	if(m_pID3Parser){
		m_pID3Parser->Reset();
	}
}
/*
VO_U32 CAacParser::ReceiveData(VO_PBYTE pbuffer, VO_U32 nSize)
{
	VO_PBYTE pTmpbuffer = pbuffer;
	VO_U32 nBufferSize = nSize;
	VO_U32 nTempSize = 0;
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	while(nBufferSize){

		nTempSize = (nBufferSize > (MAX_PUSH_BLOCK*MAX_PUSH_SIZE))? (MAX_PUSH_BLOCK*MAX_PUSH_SIZE): nBufferSize;
		nBufferSize -= nTempSize;

		nResult = ReceiveInternalData(pTmpbuffer, nTempSize);	
		if(VO_RET_SOURCE2_FAIL == nResult){
			VOLOGE("ReceiveData----FAIL");
			break;
		}
		pTmpbuffer += nTempSize; 
	}

	return nResult;
}

VO_U32 CAacParser::ReceiveInternalData(VO_PBYTE pbuffer, VO_U32 nSize)
{	

	HEADPOINT	headpoint = {0};

	PushTheDataIntoBufferPool(pbuffer, nSize);

	VO_PBYTE pHeaderpos = m_pBuffer + m_nStartPos;
	VO_U32 nDatalengh = m_nLengh;
	VOLOGR( "ReceiveInternalData---start---nSize=%lu" , nSize );
	
	while(FindHeadInBuffer(pHeaderpos, nDatalengh, &headpoint))
	{	
		switch(CheckHeadPoint(&headpoint))
		{
		case ERR_Not_Enough_Data:
			{

			}
			return VO_RET_SOURCE2_INPUTDATASMALL;
		case ERR_Invalid_Data:
			{
//				m_nLengh--;
//				m_nStartPos++;
				m_nLengh += m_nStartPos;
				m_nStartPos = headpoint.nStartPos+1;
				m_nLengh -= m_nStartPos;

				pHeaderpos = m_pBuffer + m_nStartPos;
				nDatalengh = m_nLengh;
				memset(&headpoint, 0x0, sizeof(headpoint));
			}
			break;
		case ERR_Valid_Data:
			{
				DealWithTheData(&headpoint);

				m_nLengh -= (headpoint.nStartPos - m_nStartPos + headpoint.nLength);
				m_nStartPos = headpoint.nStartPos + headpoint.nLength;
				
				nDatalengh = m_nLengh;
				pHeaderpos = m_pBuffer + m_nStartPos;			
				memset(&headpoint, 0x0, sizeof(headpoint));
			}
			break;
		}
	}
	return VO_RET_SOURCE2_OK;
}

VO_BOOL CAacParser::PushTheDataIntoBufferPool(VO_PBYTE pbuffer, VO_U32 nlength)
{
	VOLOGR( "PushTheDataIntoBufferPool---nlength=%d", nlength);

	PrintBufferReceived(pbuffer, nlength);
	//if the buffer is out of memory, clean the buffer.
	if((m_nStartPos+m_nLengh+nlength) > MAX_SAMPLE_SIZE)
	{	
		if((m_nLengh+ nlength) < MAX_SAMPLE_SIZE){
			memcpy(m_pBuffer, m_pBuffer + m_nStartPos, m_nLengh);
			m_nStartPos = 0;

		}else{
			memset(m_pBuffer, 0x0, MAX_SAMPLE_SIZE);
			m_nLengh = 0;
			m_nStartPos = 0;
			return VO_FALSE;			
		}
		
	}
	VOLOGR( "PushTheDataIntoBufferPool---start_to_copy");

	VO_PBYTE pChar = VO_NULL;

	pChar = m_pBuffer + m_nStartPos+ m_nLengh;
	memcpy(pChar, pbuffer, nlength);
	m_nLengh += nlength;
	
	return VO_TRUE;
}


VO_VOID CAacParser::PrintBufferReceived(VO_PBYTE pBuffer, VO_U32 nSize)
{
#ifdef _DUMP_DEBUG_INFO_
	int result = 0;

	if(pBuffer == NULL && nSize == 0){
		return;
	}
	
	if(!m_pFileHandle){
#ifdef _WIN32
		m_pFileHandle = fopen("G:\\Debug\\aacfile.aac", "wb");
#else
		m_pFileHandle = fopen("/mnt/sdcard/aacfile.aac", "wb");
#endif
	}
	
	result = fwrite(pBuffer,1,nSize, m_pFileHandle);

#endif

}


ERR_TYPE CAacParser::CheckHeadPoint(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)
{
	VO_PBYTE pHeaderpos = m_pBuffer + pPoint->nStartPos;
	VO_U32 nDatalengh = pPoint->nLength;
	HEADPOINT	headpoint = {0};
	ERR_TYPE	nRet = ERR_Valid_Data;
	pHeaderpos += nDatalengh;
	nDatalengh = m_nStartPos+m_nLengh - (pPoint->nStartPos + pPoint->nLength);

	VOLOGR( "CheckHeadPoint---pPoint->nType=%d",pPoint->nType);

	if((m_nStartPos+m_nLengh) < (pPoint->nStartPos + pPoint->nLength + ID3V2_HEADER_LEN)){
		nRet = ERR_Not_Enough_Data;
	}else{
		if(FindEndInBuffer(pHeaderpos, nDatalengh, &headpoint)){
			if(headpoint.nStartPos != (pPoint->nStartPos + pPoint->nLength)){
				nRet = ERR_Invalid_Data;
			}
		}else{
			nRet = ERR_Invalid_Data;
		}
	}

	
	VOLOGR( "CheckHeadPoint---nRet=%d",nRet);
	return nRet;
}



*/


VO_BOOL CAacParser::FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)
{
	VO_PBYTE pHeaderpos = pBuffer;
	VO_U32 nDatalengh = dwLen;
	HEADPOINT	headpoint1 = {0};
	HEADPOINT	headpoint2 = {0};
	HEADPOINT*	tmpheadpoint = NULL;

	VOLOGR( "FindHeadInBuffer---start");
	_CHECK_NULL_POINTER_(pBuffer)

	if(m_bIsNeedToParseHeadData){
		if(FindAdtsHeadInBuffer(pHeaderpos, nDatalengh, &headpoint1)){			
			tmpheadpoint = &headpoint1;
			tmpheadpoint->nType = TYPE_HEADDATE;
		}
	}
	else
	{
		if(FindAdtsHeadInBuffer(pHeaderpos, nDatalengh, &headpoint1)){
			
			VOLOGR( "FindHeadInBuffer---FindAdtsHeadInBuffer==TRUE");
			tmpheadpoint = &headpoint1;
		}
		
		if(m_pID3Parser->FindID3HeadInBuffer(pHeaderpos, nDatalengh, &headpoint2)){
			VOLOGR( "FindHeadInBuffer---FindID3HeadInBuffer==TRUE");
			
			if(tmpheadpoint){
				tmpheadpoint = (headpoint1.nStartPos < headpoint2.nStartPos)? (&headpoint1): (&headpoint2);
			}else{
				tmpheadpoint = &headpoint2;
			}
			
		}
	}


	if(!tmpheadpoint){
		return VO_FALSE;
	}


	pHeadData->nStartPos= tmpheadpoint->nStartPos;
	pHeadData->nLength = tmpheadpoint->nLength;
	pHeadData->nType = tmpheadpoint->nType;
	VOLOGR( "FindHeadInBuffer---nStartOffset=%d, nLength=%d, nType=%d" , pHeadData->nStartPos, pHeadData->nLength, pHeadData->nType);

	return VO_TRUE;
}

VO_BOOL CAacParser::FindEndInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)
{
	VO_BOOL bRet = VO_FALSE;
	HEADPOINT pEndPoint = {0};
//	VOLOGR( "FindEndInBuffer--start");
	_CHECK_NULL_POINTER_(pBuffer)

	bRet = FindHeadInBuffer(pBuffer, dwLen, &pEndPoint);

	if(bRet){
		pHeadData->nStartPos = pEndPoint.nStartPos;
		pHeadData->nLength = pEndPoint.nLength;
		pHeadData->nType = pEndPoint.nType;
	//	VOLOGR( "FindEndInBuffer---nStartPos=%d, nLength=%d, nType=%d" , pHeadData->nStartPos, pHeadData->nLength, pHeadData->nType);
	}
	return bRet;
}

VO_BOOL CAacParser::DealWithTheData(VO_PBYTE pBuffer, HEADPOINT* pPoint)
{
	VO_BOOL	nResult = VO_FALSE;
	_CHECK_NULL_POINTER_(pBuffer)
	VOLOGR( "DealWithTheData---pPoint->nType=%d",pPoint->nType);

		
	switch(pPoint->nType)
	{
	case TYPE_ID3v2:
		nResult = m_pID3Parser->ParseTheID3Data(pPoint, pBuffer);
		break;	
	case TYPE_ADTS:
		nResult = AssembleTheAdtsFrame(pBuffer, pPoint);
		break;
	case TYPE_HEADDATE:
		nResult = GenerateTheAACHeadData(pBuffer, pPoint);
		ResetPos();
		break;
	default:
		break;
	}

	return nResult;

}

VO_BOOL CAacParser::GenerateTheAACHeadData(VO_PBYTE pBuffer, HEADPOINT* pPoint)
{
	_CHECK_NULL_POINTER_(pBuffer)

	//we need to generate the AACHeadData before send the data.
	if(!m_bIsNeedToParseHeadData){
		return VO_FALSE;
	}

	if(TYPE_HEADDATE!=pPoint->nType){
		return VO_FALSE;
	}

	VO_SOURCE2_SAMPLE 	Sample = {0};
	VO_SOURCE2_TRACK_INFO TrackInfo = {0};

	if(pPoint->nStartPos >= MAX_SAMPLE_SIZE){
		VOLOGR( "AssembleTheAdtsFrame-out of memory---nStartPos=%d, nLength=%d", pPoint->nStartPos, pPoint->nLength);
		return VO_FALSE;
	}

	VOLOGR( "GenerateTheAACHeadData-start");

	Sample.uFlag=VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
	Sample.pFlagData= &TrackInfo;

	TrackInfo.pHeadData = pBuffer + pPoint->nStartPos;
	TrackInfo.uHeadSize = 7;
	TrackInfo.uCodec = VO_AUDIO_CodingAAC;

	m_nCallback.SendData(m_nCallback.pUserData, VO_SOURCE2_TT_AUDIO, (VO_PTR)&Sample);
	VOLOGI( "GenerateTheAACHeadData-send_success");
	
	m_bIsNeedToParseHeadData = VO_FALSE;

	//start from position 0 to send the frame
	pPoint->nLength = 0;
	pPoint->nStartPos = 0;
	return VO_TRUE;

}

VO_BOOL CAacParser::IsHeader(VO_PBYTE pHeader)
{
	/*Fixed Header of ADTS, not change from frame to frame
		0		 1		  2 	   3		4		 5		  6
	AAAAAAAA AAAABCCD EEFFFFGH HHIJKLMM MMMMMMMM MMMNNNNN NNNNNNOO
	11111111 11111111 11111111 11110000 00000000 00000000 00000000
	*/
	if(!pHeader){
		return VO_FALSE;
	}
	
	VOLOGR( "IsHeader");

	return ((pHeader[0] == 0xFF) && ((pHeader[1] & 0xF0) == 0xF0)) ? VO_TRUE: VO_FALSE;
}

VO_BOOL CAacParser::FindAdtsHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)
{
	_CHECK_NULL_POINTER_(pBuffer)

	if(dwLen < 6)
		return VO_FALSE;

	VOLOGR( "FindAdtsHeadInBuffer");

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen - 5;
	while(pHead < pTail)
	{
		if(0xFF != pHead[0])
		{
			pHead++;
			continue;
		}

		//0x11111111 found!!
		if(0xF0 != (pHead[1] & 0xF0))
		{
			pHead += 2;
			continue;
		}
		VO_U32 pdwFrameSize = 0;
		ParseAdtsHeader(pHead, &pdwFrameSize);
		if(pdwFrameSize<=0){			
			pHead+=3;
			continue;
		}
		pHeadData->nStartPos= pHead - pBuffer;
		pHeadData->nLength = pdwFrameSize;		
		pHeadData->nType = TYPE_ADTS;			
		return VO_TRUE; 				
	}

	return VO_FALSE;

}

ERR_TYPE CAacParser::CheckAdtsHead(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)
{
	return ERR_Valid_Data;

}
VO_BOOL CAacParser::AssembleTheAdtsFrame(VO_PBYTE pBuffer,HEADPOINT* pPoint)
{
	_CHECK_NULL_POINTER_(pBuffer)

	VO_SOURCE2_SAMPLE pSample = {0};
	VO_PBYTE pHeader = pBuffer + pPoint->nStartPos;

	if(pPoint->nStartPos >= MAX_SAMPLE_SIZE){
		VOLOGR( "AssembleTheAdtsFrame-out of memory---nStartPos=%d, nLength=%d", pPoint->nStartPos, pPoint->nLength);
		return VO_FALSE;
	}

	VOLOGR( "AssembleTheAdtsFrame-start");

	ParseAdtsHeader(pHeader, NULL, VO_TRUE);

	pSample.uTime = (VO_U64)m_dTimeStamp;
	pSample.uDuration = (VO_U32)m_dSampleTime;
	pSample.uFlag = VO_FALSE;
	pSample.uSize = pPoint->nLength;
	if(pSample.uSize){
		pSample.pBuffer = pBuffer + pPoint->nStartPos;
	}


	VOLOGI( "AssembleTheAdtsFrame-start_to_send_buffer--uDuration=%lu,uTime = %llu,uSize=%lu",pSample.uDuration, pSample.uTime, pSample.uSize);	
	m_nCallback.SendData(m_nCallback.pUserData, VO_SOURCE2_TT_AUDIO, (VO_PTR)&pSample);

	m_dTimeStamp += m_dSampleTime;

	return VO_TRUE;
}




VO_BOOL CAacParser::ParseAdtsHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize, VO_BOOL bInit /* = VO_FALSE */)
{
	/*	0		 1		  2 	   3		4		 5		  6
	AAAAAAAA AAAABCCD EEFFFFGH HHIJKLMM MMMMMMMM MMMNNNNN NNNNNNOO
	//Fixed Header of ADTS, not change from frame to frame
	A 12	Frame sync (all bits set) 
	B 1 	ID, MPEG identifier, set to ¡®1¡¯.
	C 2 	Layer, Indicates which layer is used. Set to ¡®00¡¯.
	D 1 	Protection absent, Indicates whether error_check() data is present or not.
	E 2 	Profile
	F 4 	Sampling rate frequency index
	G 1 	Private bit
	H 3 	Channel configuration 
	I 1 	Original/copy
	J 1 	home

	//1.2.2 Variable Header of ADTS, maybe change from frame to frame
	K 1 	copyright_identification_bit
	L 1 	copyright_identification_start
	M 13	frame_length
	N 11	adts_buffer_fullness
	O 2 	number_of_raw_data_blocks_in_frame
	*/
	VOLOGR( "ParseAdtsHeader");
	_CHECK_NULL_POINTER_(pHeader)

	if(!IsHeader(pHeader))
		return VO_FALSE;

	if(bInit)
	{
		//check ID
//		if(!(pHeader[1] & 0x8))
//			return VO_FALSE;

		//check Layer
//		if(pHeader[1] & 0x6)
//			return VO_FALSE;

		//check Profile
		m_btProfile = pHeader[2] >> 6;
		//if(0x3 == m_btProfile)
			//return VO_FALSE;

		//check Sampling rate frequency index
		m_btSampleRateIndex = (pHeader[2] >> 2) & 0xF;
		//if(m_btSampleRateIndex > 0xB)
		//	return VO_FALSE;

		//check Channel configuration
		m_btChannels = s_btChannels[((pHeader[2] << 2) | (pHeader[3] >> 6)) & 0x07];

		m_dSampleTime = double(1024) * 1000 / s_dwSamplingRates[m_btSampleRateIndex];
	}
	
	if(pdwFrameSize)
	{
		*pdwFrameSize = ((pHeader[3] & 0x03) << 11) | (pHeader[4] << 3) | (pHeader[5] >> 5);
/*		
   		*pdwFrameSize =((pHeader[3] & 0x3) << 11) + (pHeader[4] << 3) + (pHeader[5] >> 5);//Get the frame Length		
		if(*pdwFrameSize > m_dwMaxSampleSize)
			m_dwMaxSampleSize = *pdwFrameSize;
		VOLOGR( "ParseAdtsHeader---m_dwMaxSampleSize=%d",*pdwFrameSize);
*/		
	}

	return VO_TRUE;
}


