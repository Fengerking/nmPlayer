/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBasePushPaser.cpp

	Contains:	CBasePushPaser class file

	Written by:	Aiven

	Change History (most recent first):
	2012-06-14		Aiven			Create file

*******************************************************************************/
#include "CBasePushPaser.h"
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
#define HEADDATA_LENGTH	10

CBasePushPaser::CBasePushPaser()
:m_nHeadlength(HEADDATA_LENGTH)
#ifdef _DUMP_DEBUG_INFO_
,m_pFileHandleIn(NULL)
,m_pFileHandleOut(NULL)
#endif
{
	VOLOGI( "CBasePushPaser");
	
	m_nInfo.nStartPos = 0;
	m_nInfo.nLengh = MAX_SAMPLE_SIZE;
	m_nInfo.nInUsedLength = 0;
 	m_nInfo.pBuffer = new VO_BYTE[m_nInfo.nLengh];
	memset(m_nInfo.pBuffer, 0x0, sizeof(m_nInfo.nLengh));
	
	memset(&m_nCallback, 0x0, sizeof(m_nCallback));
}


CBasePushPaser::~CBasePushPaser()
{
	VOLOGI( "~CBasePushPaser");

	Reset();
	
	if(m_nInfo.pBuffer){
		delete []m_nInfo.pBuffer;
		m_nInfo.pBuffer = NULL;
	}
	memset(&m_nInfo, 0x0, sizeof(m_nInfo));

}



VO_U32 CBasePushPaser::Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_SAMPLECALLBACK* pCallback )
{
	VOLOGI( "Init--pUserData=%d,SendData=%d",pCallback->pUserData, pCallback->SendData);

	m_nCallback.pUserData = pCallback->pUserData;
	m_nCallback.SendData = pCallback->SendData;
	
	return VO_RET_SOURCE2_OK;
}

VO_U32 CBasePushPaser::Uninit()
{
	VOLOGI( "Uninit");

	return VO_RET_SOURCE2_OK;
}

VO_VOID CBasePushPaser::Reset()
{
	VOLOGI( "Reset");

	m_nInfo.nStartPos = 0;
	m_nInfo.nLengh = MAX_SAMPLE_SIZE;
	m_nInfo.nInUsedLength = 0;	
	if(m_nInfo.pBuffer){
		memset(m_nInfo.pBuffer, 0x0, sizeof(m_nInfo.nLengh));
	}

#ifdef _DUMP_DEBUG_INFO_
	if (m_pFileHandleIn)
	{
		fclose(m_pFileHandleIn);
		m_pFileHandleIn = NULL;
	}

	if (m_pFileHandleOut)
	{
		fclose(m_pFileHandleOut);
		m_pFileHandleOut = NULL;
	}
#endif
}

VO_U32 CBasePushPaser::ReceiveData(VO_PBYTE pbuffer, VO_U32 nSize)
{
	VO_PBYTE pTmpbuffer = pbuffer;
	VO_U32 nBufferSize = nSize;
	VO_U32 nTempSize = 0;
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	VOLOGI( "ReceiveData---nSize=%lu" , nSize );

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
	VOLOGI( "ReceiveData---end");

	return nResult;
}

VO_U32 CBasePushPaser::ReceiveInternalData(VO_PBYTE pbuffer, VO_U32 nSize)
{	
	HEADPOINT	headpoint = {0};
	VOLOGI( "ReceiveInternalData---start---nSize=%lu" , nSize );

	PushTheDataIntoBufferPool(pbuffer, nSize);

	VO_PBYTE pHeaderpos = m_nInfo.pBuffer + m_nInfo.nStartPos;
	VO_U32 nDatalengh = m_nInfo.nInUsedLength;
	
	while(FindHeadInBuffer(pHeaderpos, nDatalengh, &headpoint))
	{	
		switch(CheckHeadPoint(pHeaderpos, nDatalengh, &headpoint))
		{
		case ERR_Not_Enough_Data:
			{

			}
			return VO_RET_SOURCE2_INPUTDATASMALL;
		case ERR_Invalid_Data:
			{
				m_nInfo.nStartPos += headpoint.nStartPos+1;
				m_nInfo.nInUsedLength -= headpoint.nStartPos + 1;

				pHeaderpos = m_nInfo.pBuffer + m_nInfo.nStartPos;
				nDatalengh = m_nInfo.nInUsedLength;
				memset(&headpoint, 0x0, sizeof(headpoint));
				VOLOGE("ERR_Invalid_Data---nStartPos=%lu, nInUsedLength=%lu, nLengh=%lu", m_nInfo.nStartPos, m_nInfo.nInUsedLength, m_nInfo.nLengh);
			}
			break;
		case ERR_Valid_Data:
			{
				DealWithTheData(pHeaderpos, &headpoint);

				DumpBufferOut(pHeaderpos+headpoint.nStartPos, headpoint.nLength);

				m_nInfo.nStartPos += headpoint.nStartPos + headpoint.nLength;
				m_nInfo.nInUsedLength -= headpoint.nStartPos + headpoint.nLength;
				
				pHeaderpos = m_nInfo.pBuffer + m_nInfo.nStartPos;
				nDatalengh = m_nInfo.nInUsedLength;	
				memset(&headpoint, 0x0, sizeof(headpoint));
				VOLOGI("ERR_Valid_Data---nStartPos=%lu, nInUsedLength=%lu, nLengh=%lu", m_nInfo.nStartPos, m_nInfo.nInUsedLength, m_nInfo.nLengh);
				
			}
			break;
		}
	}

	VOLOGI( "ReceiveInternalData---end");
	
	return VO_RET_SOURCE2_OK;
}

VO_BOOL CBasePushPaser::PushTheDataIntoBufferPool(VO_PBYTE pbuffer, VO_U32 nlength)
{
	DumpBufferIn(pbuffer, nlength);
	//if the buffer is out of memory, clean the buffer.
	if((m_nInfo.nStartPos+m_nInfo.nInUsedLength+nlength) > m_nInfo.nLengh)
	{	
		if((m_nInfo.nInUsedLength+nlength) < m_nInfo.nLengh){
			memcpy(m_nInfo.pBuffer, m_nInfo.pBuffer + m_nInfo.nStartPos, m_nInfo.nInUsedLength);
			m_nInfo.nStartPos = 0;
		}else{	
			VOLOGE( "PushTheDataIntoBufferPool---Error(out of memory!)");
			Reset();
			return VO_FALSE;			
		}
		
	}

	VO_PBYTE pChar = VO_NULL;

	pChar = m_nInfo.pBuffer + m_nInfo.nStartPos+ m_nInfo.nInUsedLength;
	memcpy(pChar, pbuffer, nlength);
	m_nInfo.nInUsedLength += nlength;
	
	return VO_TRUE;
}

ERR_TYPE CBasePushPaser::CheckHeadPoint(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)
{
	ERR_TYPE	nRet = ERR_Valid_Data;

	if(dwLen < (pHeadData->nStartPos + pHeadData->nLength) || 0 == dwLen){
		nRet = ERR_Not_Enough_Data;
	}else if((dwLen == pHeadData->nLength) && (0 == pHeadData->nStartPos)){
		nRet = ERR_Valid_Data;
	}else{
		nRet = CheckHeadPointEx(pBuffer, dwLen, pHeadData);
	}

	return nRet;
}

ERR_TYPE CBasePushPaser::CheckHeadPointEx(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)
{
	VO_PBYTE pHeaderpos = pBuffer + pHeadData->nStartPos;
	VO_U32 nDatalengh = pHeadData->nLength;
	HEADPOINT	headpoint = {0};
	ERR_TYPE	nRet = ERR_Valid_Data;

	if(dwLen < (pHeadData->nStartPos + pHeadData->nLength + m_nHeadlength)){
		return ERR_Not_Enough_Data;
	}

	pHeaderpos += nDatalengh;
	nDatalengh = dwLen - (pHeadData->nStartPos + pHeadData->nLength);


	if(FindEndInBuffer(pHeaderpos, nDatalengh, &headpoint)){
		if(headpoint.nStartPos != 0){
			nRet = ERR_Invalid_Data;
		}
	}else{
		nRet = ERR_Invalid_Data;
	}

	VOLOGI( "CheckHeadPoint---nType=%lu, nRet=%lu",pHeadData->nType, nRet);
	return nRet;
}



VO_U32 CBasePushPaser::ResetPos()
{
	m_nInfo.nInUsedLength += m_nInfo.nStartPos;
	m_nInfo.nStartPos =0;
	return VO_RET_SOURCE2_OK;
}

VO_VOID CBasePushPaser::DumpBufferIn(VO_PBYTE pBuffer, VO_U32 nSize)
{
#ifdef _DUMP_DEBUG_INFO_
	int result = 0;

	if(pBuffer == NULL && nSize == 0){
		return;
	}
	
	if(!m_pFileHandleIn){
#ifdef _WIN32
		m_pFileHandleIn = fopen("G:\\Debug\\aacfile.aac", "wb");
#else
		m_pFileHandleIn = fopen("/mnt/sdcard/aacfile.aac", "wb");
#endif
	VOLOGI("PrintBufferReceived---m_pFileHandle=%x",m_pFileHandleIn);

	}
	
	result = fwrite(pBuffer,1,nSize, m_pFileHandleIn);

#endif

}

VO_VOID CBasePushPaser::DumpBufferOut(VO_PBYTE pBuffer, VO_U32 nSize)
{
#ifdef _DUMP_DEBUG_INFO_
	int result = 0;

	if(pBuffer == NULL && nSize == 0){
		return;
	}
	
	if(!m_pFileHandleOut){
#ifdef _WIN32
		m_pFileHandleOut = fopen("G:\\Debug\\aacfile_test.aac", "wb");
#else
		m_pFileHandleOut = fopen("/mnt/sdcard/aacfile_test.aac", "wb");
#endif
	VOLOGI("PrintBufferReceived---m_pFileHandleOut=%x",m_pFileHandleOut);

	}
	
	result = fwrite(pBuffer,1,nSize, m_pFileHandleOut);

#endif

}

