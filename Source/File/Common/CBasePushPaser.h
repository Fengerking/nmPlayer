	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBasePushPaser.h

	Contains:	CBasePushPaser head file

	Written by:	Aiven

	Change History (most recent first):
	2012-06-14		Aiven			Create file

*******************************************************************************/
#pragma once
#ifndef _CBasePushParser_H
#define _CBasePushParser_H
//#include "voYYDef_filcmn.h"
#include "vo_type.h"
#include "voSource2.h"
#include "CvoBaseMemOpr.h"
#include "voAAC2.h"
#include "CPushModeType.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

//#define _DUMP_DEBUG_INFO_

typedef struct{
	VO_PBYTE	pBuffer;
	VO_U32		nStartPos;
	VO_U32		nLengh;
	VO_U32		nInUsedLength;
}BufferStruct;


class CBasePushPaser
{
public:
	CBasePushPaser();
	~CBasePushPaser();

	virtual VO_U32	Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_SAMPLECALLBACK * pCallback );
	virtual VO_U32	Uninit();
	virtual VO_VOID Reset();	
	virtual VO_U32	ReceiveData(VO_PBYTE pbuffer, VO_U32 nSize);

protected:
	VO_U32 ResetPos();	
	
	virtual VO_U32 ReceiveInternalData(VO_PBYTE pbuffer, VO_U32 nSize);	
	virtual ERR_TYPE CheckHeadPoint(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);	
	virtual ERR_TYPE CheckHeadPointEx(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData);
	virtual VO_BOOL FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)=0;
	virtual VO_BOOL FindEndInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen, HEADPOINT* pHeadData)=0;	
	virtual VO_BOOL DealWithTheData(VO_PBYTE pbuffer, HEADPOINT* pPoint)=0;
	virtual VO_BOOL PushTheDataIntoBufferPool(VO_PBYTE pbuffer, VO_U32 nlength);
	VO_VOID DumpBufferIn(VO_PBYTE pBuffer, VO_U32 nSize);
	VO_VOID DumpBufferOut(VO_PBYTE pBuffer, VO_U32 nSize);

protected:	
	VO_SOURCE2_SAMPLECALLBACK  m_nCallback;
private:		
	BufferStruct	m_nInfo;

	VO_U32 	m_nHeadlength;
#ifdef _DUMP_DEBUG_INFO_
	FILE* 			m_pFileHandleIn;
	FILE*			m_pFileHandleOut;
#endif
};

#ifdef _VONAMESPACE
}
#endif

#endif //_CAacParser_H

