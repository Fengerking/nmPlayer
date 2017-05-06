/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CID3Parser.cpp

	Contains:	CID3Parser class file

	Written by:	Aiven

	Change History (most recent first):
	2013-06-16		Aiven			Create file

*******************************************************************************/

#include "CID3Parser.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define 	SUBHEAD_HLS_TIMESTAMP_LENGTH				45
#define 	SUBHEAD_CUSTOMER_PIC_JPEG_LENGTH			14
#define 	SUBHEAD_CUSTOMER_PIC_PNG_LENGTH			13
#define 	SUBHEAD_CUSTOMER_PIC_GIF_LENGTH			13

#define	HEAD_LENGTH	4

//static const char* ID3_SUBHEAD[] = {"com.apple.streaming.transportStreamTimestamp", "image/jpeg", "image/png", "image/gif"};

CID3Parser::CID3Parser()
{
	VOLOGI( "CID3Parser");

	memset(&m_pHeader, 0, sizeof(m_pHeader));
	memset(&m_pExtHeader, 0, sizeof(m_pExtHeader));
	memset(&m_pFrameBlock, 0, sizeof(m_pFrameBlock));

}

CID3Parser::~CID3Parser()
{
	VOLOGI( "~CID3Parser");
	ReleaseFrames();
}

VO_U32 CID3Parser::Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_SAMPLECALLBACK * pCallback )
{
	CBasePushPaser::Init(pSource, nFlag, pCallback);
	return VO_RET_SOURCE2_OK;
}

VO_U32 CID3Parser::Uninit()
{
	VOLOGI( "Uninit");
	CBasePushPaser::Uninit();

	return VO_RET_SOURCE2_OK;
}

VO_VOID	CID3Parser::Reset()
{
	CBasePushPaser::Reset();
	FrameReset();
}

VO_VOID	CID3Parser::FrameReset()
{
	ReleaseFrames();
	memset(&m_pHeader, 0, sizeof(m_pHeader));
	memset(&m_pExtHeader, 0, sizeof(m_pExtHeader));
	memset(&m_pFrameBlock, 0, sizeof(m_pFrameBlock));
}


VO_BOOL CID3Parser::FindEndInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)
{
	VO_BOOL bResult = VO_FALSE;
	bResult = FindID3HeadInBuffer(pBuffer, dwLen, pHeadData);

	return bResult;
}
VO_BOOL CID3Parser::FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)
{
	VO_BOOL bResult = VO_FALSE;
	bResult = FindID3HeadInBuffer(pBuffer, dwLen, pHeadData);

	return bResult;

}
		
VO_BOOL CID3Parser::DealWithTheData(VO_PBYTE pBuffer, HEADPOINT* pPoint)
{
	VO_BOOL	nResult = VO_FALSE;
	_CHECK_NULL_POINTER_(pBuffer)
	VOLOGR( "DealWithTheData---pPoint->nType=%d",pPoint->nType);
		
	switch(pPoint->nType)
	{
	case TYPE_ID3v2:
		nResult = ParseTheID3Data(pPoint, pBuffer);
		break;	
	default:
		break;
	}

	return nResult;
}




ERR_TYPE CID3Parser::Parse(VO_PBYTE* ppBuffer, VO_U32 nSize)
{
	VO_PBYTE	pbuffer = *ppBuffer;
	ERR_TYPE nRet = ERR_Valid_Data;

	if(NULL == pbuffer || nSize< ID3V2_HEADER_LEN){
		return ERR_Not_Enough_Data;
	}

	memcpy(&m_pHeader, pbuffer, ID3V2_HEADER_LEN);

	if(m_pHeader.szTag[0]!='I' ||m_pHeader.szTag[1]!='D' ||m_pHeader.szTag[2]!='3')//(VO_PTR)"ID3", 3))
		return ERR_Invalid_Data;

	VO_U32 dwSize = CALCULATE_SIZE_SYNCHSAFE(m_pHeader.btSize);

	if(dwSize ==0){
		return ERR_Invalid_Data;
	}

	if(dwSize+ID3V2_HEADER_LEN > nSize){
		return ERR_Not_Enough_Data;
	}

	VOLOGR( "Parse");
	
	VO_S32 nLeft = dwSize;
	pbuffer = pbuffer + ID3V2_HEADER_LEN;
	
	if(m_pHeader.btFlags & 0x40)	//has ext header!!
	{
		memcpy(&m_pExtHeader, pbuffer, ID3V2_HEADER_LEN);
//		ptr_read_pointer(&m_pExtHeader, sizeof(ID3v2ExtHeader));
		VO_U32 dwExtHeaderSize = CALCULATE_SIZE_SYNCHSAFE(m_pExtHeader.btSize);
//		ptr_skip(dwExtHeaderSize - sizeof(ID3v2ExtHeader));

		nLeft -= dwExtHeaderSize;
		pbuffer = pbuffer + dwExtHeaderSize;
	}

	while(nLeft > 0)
	{
		PID3v2Frame pFrame = new ID3v2Frame;
		if(!pFrame)
			return ERR_Invalid_Data;

		pFrame->pData = NULL;
		pFrame->pNext = NULL;

		VO_S32 nRet = ParseFrame(&pbuffer, nLeft, pFrame, nLeft);
		if(nRet <= 0)	//error
		{
			delete pFrame;

			if(0 == nRet)
				break;
			else
			{
				if(m_pFrameBlock.dwFrameCount == 0)
				{
					ReleaseFrames();

					return ERR_Invalid_Data;
				}
				else
				{
					break;
				}
			}
		}

		nLeft -= nRet;

		if(m_pFrameBlock.pFrameHead)
			m_pFrameBlock.pFrameTail = m_pFrameBlock.pFrameTail->pNext = pFrame;
		else
			m_pFrameBlock.pFrameTail = m_pFrameBlock.pFrameHead = pFrame;

		m_pFrameBlock.dwFrameCount++;
	}	

	return nRet;
}

VO_BOOL CID3Parser::FindFirstFrame(PID3v2Frame* pFrame)
{
	if(!m_pFrameBlock.pFrameHead ||!m_pFrameBlock.dwFrameCount){
		return VO_FALSE;
	}

	*pFrame = m_pFrameBlock.pFrameHead;
	return VO_TRUE;
}	

VO_U32 CID3Parser::GetFrameCount()
{
	return m_pFrameBlock.dwFrameCount;
}



VO_S32 CID3Parser::ParseFrame(VO_PBYTE* ppBuffer, VO_U32 nSize, PID3v2Frame pFrame, VO_U32 dwMaxFrameSize)
{
	if(!(*ppBuffer) || nSize < VO_ID3V2_FRAME_HEADER_LEN){
		return 0;
	}
	VOLOGR( "ParseFrame");

	VO_PBYTE	pbuffer = *ppBuffer;

	VO_U32 dwDataSize = 0, dwFrameSize = 0;
	if(m_pHeader.btVersion[0] > 2)
	{
//		ptr_read_pointer(&pFrame->Header, VO_ID3V2_FRAME_HEADER_LEN);
		memcpy(&pFrame->Header, pbuffer, VO_ID3V2_FRAME_HEADER_LEN);
		pbuffer += VO_ID3V2_FRAME_HEADER_LEN;
		
		if(m_pHeader.btVersion[0] > 3)	//.4.0 later use 32 bit synchsafe integer for frame size
			dwDataSize = CALCULATE_SIZE_SYNCHSAFE(pFrame->Header.btSize);
		else
			dwDataSize = CALCULATE_SIZE(pFrame->Header.btSize);

		dwFrameSize = dwDataSize + VO_ID3V2_FRAME_HEADER_LEN;
	}
/*	
	else
	{
		MemSet(&pFrame->Header, 0, VO_ID3V2_FRAME_HEADER_LEN);
		ptr_read_pointer(pFrame->Header.btID, 3);
		ptr_read_pointer(pFrame->Header.btSize + 1, 3);

		dwDataSize = CALCULATE_SIZE(pFrame->Header.btSize);
		dwFrameSize = dwDataSize + VO_ID3V2_FRAME_HEADER_LEN_20BEFORE;
	}
*/

	if(nSize < dwFrameSize){
		return -1;
	}

//	static VO_BYTE g_btNullID[] = {0, 0, 0, 0};
	if(!dwDataSize && pFrame->Header.btID[0] == 0 && pFrame->Header.btID[1] == 0 &&
		pFrame->Header.btID[2] == 0 && pFrame->Header.btID[3] == 0)//!MemCompare(g_btNullID, pFrame->Header.btID, sizeof(g_btNullID)))
		return 0;

	if(dwFrameSize > dwMaxFrameSize)// && bFirstFrame == VO_FALSE)
		return -1;

	if(dwDataSize > 0)
	{
		pFrame->pData = new VO_BYTE[dwDataSize];//NEW_BUFFER(dwDataSize);
		if(!pFrame->pData)
			return 0;

//		ptr_read_pointer(pFrame->pData, dwDataSize);
		memcpy(pFrame->pData, pbuffer, dwDataSize);
		pbuffer+=dwDataSize;
	}

	*ppBuffer = pbuffer;

	return dwFrameSize;
}


VO_VOID CID3Parser::ReleaseFrames()
{
	VOLOGR( "ReleaseFrames");

	if(m_pFrameBlock.pFrameHead)
	{
		PID3v2Frame pCur = m_pFrameBlock.pFrameHead;
		PID3v2Frame pDel;
		while(pCur)
		{
			pDel = pCur;
			pCur = pCur->pNext;
			if(pDel->pData)
				delete []pDel->pData;
			delete []pDel;
		}
		m_pFrameBlock.pFrameHead = NULL;
	}

	m_pFrameBlock.pFrameTail = NULL;
	m_pFrameBlock.dwFrameCount = 0;
}


VO_BOOL CID3Parser::FindID3HeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)
{	
	if(NULL == pBuffer || NULL == pHeadData){
		return VO_FALSE;
	}

	if(dwLen < ID3V2_HEADER_LEN)
		return VO_FALSE;

	VOLOGR( "FindID3HeadInBuffer");

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen - ID3V2_HEADER_LEN + 1;
	while(pHead < pTail)
	{
		if(0x49 != pHead[0])
		{
			pHead++;
			continue;
		}

		if(0x44 != pHead[1])
		{
			pHead += 1;
			continue;
		}

		if(0x33 != pHead[2])
		{
			pHead += 2;
			continue;
		}

		ID3v2Header* header = (ID3v2Header*)pHead;
		VO_U32 dwSize = CALCULATE_SIZE_SYNCHSAFE(header->btSize);

		pHeadData->nStartPos= pHead - pBuffer;
		pHeadData->nLength = (ID3V2_HEADER_LEN + dwSize);
		pHeadData->nType = TYPE_ID3v2;
		return VO_TRUE;
	}

	return VO_FALSE;	
	
}

VO_BOOL CID3Parser::ParseTheID3Data(HEADPOINT* pPoint, VO_PBYTE	pBuffer)
{
	VOLOGR( "ParseTheID3Data");

	if(NULL == pBuffer || NULL == pPoint){
		return VO_FALSE;
	}
	
	if(pPoint->nType != TYPE_ID3v2){
		return VO_FALSE;
	}

	VO_PBYTE	pbuffer = pBuffer + pPoint->nStartPos;
	VO_U32		nSize = pPoint->nLength;
	PID3v2Frame  pFrame = NULL;
	ID3Frame	ID3Frame;
	
	FrameReset();
	if(ERR_Valid_Data == Parse(&pbuffer, nSize)){
		if(!FindFirstFrame(&pFrame)){
			return VO_FALSE;
		}

		while(pFrame){
			memset(&ID3Frame, 0x0, sizeof(ID3Frame));
			
			memcpy(ID3Frame.pFrameHead, pFrame->Header.btID, sizeof(pFrame->Header.btID));
			ID3Frame.nDataLength = CALCULATE_SIZE_SYNCHSAFE(pFrame->Header.btSize);
			if(ID3Frame.nDataLength){
				ID3Frame.pFrameData = pFrame->pData;
			}
			CheckID3FrameData(&ID3Frame);
			
			VOLOGI( "send ID3Data successfully---nSubHead = %lu, nDataLength = %lu", ID3Frame.nSubHeadFlag, ID3Frame.nDataLength);

			VO_CHAR head[10] = {0};
			VO_CHAR data[1024] = {0};
			memcpy(head, ID3Frame.pFrameHead, 4);
			
			if(!memcmp("TIT2", ID3Frame.pFrameHead, HEAD_LENGTH) ||
				!memcmp("TPE1", ID3Frame.pFrameHead, HEAD_LENGTH)){
				
				memcpy(data, ID3Frame.pFrameData, ID3Frame.nDataLength);
				
			}else if(!memcmp("COMM", ID3Frame.pFrameHead, HEAD_LENGTH)){
				VO_PBYTE	point=ID3Frame.pFrameData;
				VO_U32 length = 0;
				point += 4;
				while ((*point) != 0)
				{
					point++;
				}
				point++;
				length = point - ID3Frame.pFrameData;
				memcpy(data, point, ID3Frame.nDataLength - length);
			}
			VOLOGI( "print the frame data---pFrameHead = %s, pFrameData = %s", head, data);
			
			m_nCallback.SendData(m_nCallback.pUserData, VO_SOURCE2_TT_HINT, (VO_PTR)&ID3Frame);

			//delete the memory created in "CheckID3FrameData"
			if(ID3Frame.pSubStruct){
				free(ID3Frame.pSubStruct);
				ID3Frame.pSubStruct = NULL;
			}
			pFrame = pFrame->pNext;
		}
	}
	
	VOLOGR( "ParseTheID3Data---send_success");

	return VO_TRUE;
}


ERR_TYPE CID3Parser::CheckID3Head(HEADPOINT* pPoint)
{
	
	return ERR_Valid_Data;

}



VO_U32	CID3Parser::ParseTXXXFrame(ID3Frame* pID3Frame)
{
	VO_U32 nResut = VO_RET_SOURCE2_OK;
	_CHECK_NULL_POINTER_EX(pID3Frame, VO_RET_SOURCE2_FAIL)
	
	TxxxStruct* pStruct = (TxxxStruct*)malloc(sizeof(TxxxStruct));

	VO_PBYTE pbuffer = pID3Frame->pFrameData;
	VO_U32 len = 1;

	memcpy(pStruct->pTextEncoding, pbuffer, sizeof(pStruct->pTextEncoding));
	pbuffer += sizeof(pStruct->pTextEncoding);


	pStruct->pDescription = pbuffer;
 	while(0x0 != *pbuffer){
		pbuffer++;
		len++;
	};
 	pStruct->nDescriptionLenght = len;
	pbuffer++;

  	pStruct->pValue = pbuffer;
	pStruct->nValueLength = pID3Frame->nDataLength - sizeof(pStruct->pTextEncoding) - pStruct->nDescriptionLenght;


	pID3Frame->pSubStruct = (VO_PTR)pStruct;
	return nResut;
}

VO_U32	CID3Parser::ParseAPICFrame(ID3Frame* pID3Frame)
{
	VO_U32 nResut = VO_RET_SOURCE2_OK;
	_CHECK_NULL_POINTER_EX(pID3Frame, VO_RET_SOURCE2_FAIL)
	
	ApicStruct* pStruct = (ApicStruct*)malloc(sizeof(ApicStruct));

	VO_PBYTE pbuffer = pID3Frame->pFrameData;
	VO_U32 len = 1;

	memcpy(pStruct->pTextEncoding, pbuffer, sizeof(pStruct->pTextEncoding));
	pbuffer += sizeof(pStruct->pTextEncoding);

	
	pStruct->pMimeType = pbuffer;
 	while(0x0 != *pbuffer){
		pbuffer++;
		len++;
	};
 	pStruct->nMimeTypeLength = len;
	pbuffer++;

	memcpy(pStruct->pPictureType, pbuffer, sizeof(pStruct->pPictureType));
	pbuffer += sizeof(pStruct->pPictureType);


	pStruct->pDescription = pbuffer;
	len = 1;
	while(0x0 != *pbuffer){
		pbuffer++;
		len++;
	};
	pStruct->nDescriptionLength = len;
	pbuffer++;

	pStruct->pPictureData = pbuffer;
	pStruct->nPictureDataLength = pID3Frame->nDataLength - sizeof(pStruct->pTextEncoding) - 
		pStruct->nMimeTypeLength - sizeof(pStruct->pPictureType) - pStruct->nDescriptionLength;

	pID3Frame->pSubStruct = (VO_PTR)pStruct;	
	return nResut;


}

VO_U32	CID3Parser::ParsePRIVFrame(ID3Frame* pID3Frame)
{
	VO_U32 nResut = VO_RET_SOURCE2_OK;

	_CHECK_NULL_POINTER_EX(pID3Frame, VO_RET_SOURCE2_FAIL)
	
	PrivStruct* pStruct = (PrivStruct*)malloc(sizeof(PrivStruct));

	VO_PBYTE pbuffer = pID3Frame->pFrameData;
	VO_U32 len = 1;

	pStruct->pOwnerIdentifier = pbuffer;
 	while(0x0 != *pbuffer){
		pbuffer++;
		len++;
	};
 	pStruct->nOwnerIdentifierLenght = len;
	pbuffer++;
	
  	pStruct->pPrivateData = pbuffer;
	pStruct->nPrivateDataLenght = pID3Frame->nDataLength - pStruct->nOwnerIdentifierLenght;


	pID3Frame->pSubStruct = (VO_PTR)pStruct;
	return nResut;

}

VO_U32 CID3Parser::CheckID3FrameData(ID3Frame* pID3Frame)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	_CHECK_NULL_POINTER_EX(pID3Frame, VO_RET_SOURCE2_FAIL)
	VOLOGR( "CheckID3FrameData");

	ConvertHeadToFlag(pID3Frame->pFrameHead, &pID3Frame->nSubHeadFlag);

	switch(pID3Frame->nSubHeadFlag)
	{
	case SUBHEAD_ID3_PRIV:
		nResult = ParsePRIVFrame(pID3Frame);
		break;
	case SUBHEAD_ID3_APIC:
		nResult = ParseAPICFrame(pID3Frame);
		break;
	case SUBHEAD_ID3_TXXX:
		nResult = ParseTXXXFrame(pID3Frame);
		break;
	default:
		break;
	}

	return nResult;
}

VO_U32 CID3Parser::ConvertHeadToFlag(VO_BYTE* pFrameHead, VO_U32* pFalg)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	_CHECK_NULL_POINTER_EX(pFrameHead, VO_RET_SOURCE2_FAIL)
	_CHECK_NULL_POINTER_EX(pFalg, VO_RET_SOURCE2_FAIL)

	if(!memcmp("PRIV", pFrameHead, HEAD_LENGTH)){
		*pFalg = SUBHEAD_ID3_PRIV;
	}else if(!memcmp("APIC", pFrameHead, HEAD_LENGTH)){
		*pFalg = SUBHEAD_ID3_APIC;
	}else if(!memcmp("TXXX", pFrameHead, HEAD_LENGTH)){
		*pFalg = SUBHEAD_ID3_TXXX;
	}else{
		nResult = VO_RET_SOURCE2_FAIL;
	}

	return nResult;
}

