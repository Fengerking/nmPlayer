	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAacParser.h

	Contains:	CAacParser head file

	Written by:	Aiven

	Change History (most recent first):
	2012-04-11		Aiven			Create file

*******************************************************************************/
#pragma once
#ifndef _CAacParser_H
#define _CAacParser_H
//#include "vo_type.h"
#include "voSource2.h"
#include "AacFileDataStruct.h"
#include "CvoBaseMemOpr.h"
#include "voAAC2.h"
#include "CID3Parser.h"
#include "CPushModeType.h"
#include "CBasePushPaser.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#undef _DUMP_DEBUG_INFO_
class CAacParser:public CBasePushPaser
{
public:
	CAacParser();
	~CAacParser();

	VO_U32	Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_SAMPLECALLBACK * pCallback );
	VO_U32	Uninit();
	VO_VOID Reset();	
//	VO_BOOL AssambleSample(VO_SOURCE2_SAMPLE* pSample);
//	VO_U32	ReceiveData(VO_PBYTE pbuffer, VO_U32 nSize);

protected:
//	VO_U32 ReceiveInternalData(VO_PBYTE pbuffer, VO_U32 nSize);	
//	VO_BOOL FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);
	VO_BOOL FindEndInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);	
	VO_BOOL FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);
	VO_BOOL DealWithTheData(VO_PBYTE pBuffer, HEADPOINT* pPoint);
	
	VO_BOOL GenerateTheAACHeadData(VO_PBYTE pBuffer,HEADPOINT* pPoint);	
	VO_BOOL IsHeader(VO_PBYTE pHeader);
	VO_BOOL FindAdtsHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);
	ERR_TYPE CheckAdtsHead(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);
	VO_BOOL AssembleTheAdtsFrame(VO_PBYTE pBuffer,HEADPOINT* pPoint);
//	VO_BOOL PushTheDataIntoBufferPool(VO_PBYTE pbuffer, VO_U32 nlength);
	VO_BOOL ParseAdtsHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize, VO_BOOL bInit = VO_FALSE);
//	VO_VOID PrintBufferReceived(VO_PBYTE pBuffer, VO_U32 nSize);
private:
	DECLARE_USE_AAC_GLOBAL_VARIABLE
		
	VO_BOOL			m_bIsNeedToParseHeadData;
	
	CID3Parser*		m_pID3Parser;
//	VO_SOURCE2_SAMPLECALLBACK  m_nCallback;
	
//	VO_U32			m_dwAvgBytesPerSec;

//	VO_BYTE			m_btHeader[4];
	
	VO_U8			m_btChannels;
	VO_U8			m_btSampleRateIndex;
	VO_U8			m_btProfile;
	double			m_dSampleTime;			//aac sample time is not integer

	double			m_dTimeStamp;			//aac TimeStamp is not integer

};

#ifdef _VONAMESPACE
}
#endif

#endif //_CAacParser_H


