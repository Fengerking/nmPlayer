	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CID3Parser.h

	Contains:	CID3Parser head file

	Written by:	Aiven

	Change History (most recent first):
	2013-06-16		Aiven			Create file

*******************************************************************************/
#pragma once
#ifndef _CID3Parser_H_
#define _CID3Parser_H_

#include "ID3v2DataStruct.h"
#include "CPushModeType.h"
#include "voAAC2.h"
#include "CBasePushPaser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


typedef struct{
	VO_U32			dwFrameCount;
	PID3v2Frame 		pFrameHead;
	PID3v2Frame 		pFrameTail;
}FrameBlock;

class CID3Parser:public CBasePushPaser{
public:
	CID3Parser();
	~CID3Parser();
	VO_U32	Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_SAMPLECALLBACK * pCallback );
	VO_U32	Uninit();
	VO_VOID	Reset();

	
	ERR_TYPE Parse(VO_PBYTE* ppBuffer, VO_U32 nSize);
	VO_BOOL	FindFirstFrame(PID3v2Frame* pFrame);
	VO_U32	GetFrameCount();
	
	VO_BOOL FindID3HeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);	
	VO_BOOL ParseTheID3Data(HEADPOINT* pPoint, VO_PBYTE pBuffer);
	
protected:
	VO_BOOL FindEndInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);	
	VO_BOOL FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);
	VO_BOOL DealWithTheData(VO_PBYTE pBuffer, HEADPOINT* pPoint);
	VO_VOID FrameReset();


	
	/* return value:
	0:	error
	-1: error(beyond max frame size)
	>0: success, return frame size
	*/
	VO_S32	ParseFrame(VO_PBYTE* ppBuffer, VO_U32 nSize, PID3v2Frame pFrame, VO_U32 dwMaxFrameSize);
	VO_VOID 	ReleaseFrames();

	ERR_TYPE CheckID3Head(HEADPOINT* pPoint);
	VO_U32 CheckID3FrameData(ID3Frame* pID3Frame); 

	VO_U32 ParseTXXXFrame(ID3Frame* pID3Frame);
	VO_U32 ParseAPICFrame(ID3Frame* pID3Frame);
	VO_U32 ParsePRIVFrame(ID3Frame* pID3Frame);
	VO_U32 ConvertHeadToFlag(VO_BYTE* pFrameHead, VO_U32* pFalg);
	
protected:
	//headers
	ID3v2Header 		m_pHeader;
	ID3v2ExtHeader	m_pExtHeader;

	//frames
	FrameBlock		m_pFrameBlock;
};

#ifdef _VONAMESPACE
}
#endif

#endif//_CID3Parser_H_
