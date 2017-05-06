/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voGetSmilAPI.cpp

	Contains:	voGetSmilAPI class file

	Written by:	Aiven

	Change History (most recent first):
	2012-08-14		Aiven			Create file

*******************************************************************************/
#include "CBS_SMIL.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SOURCE_CLASS CBS_SMIL


//#define _DEBUG
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
_CrtMemState s1;
_CrtMemState s2;
_CrtMemState s3;
#endif

VO_U32 vosrcInit(VO_HANDLE * phHandle, VO_U32 nFlag, VO_PTR pParam)
{
	VO_U32	 ulRet = 0;
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF );
	_CrtMemCheckpoint( &s1 );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
//	_CrtSetBreakAlloc(78);
#endif

	SOURCE_CLASS * pSrc = new SOURCE_CLASS();


	ulRet = pSrc->Init(nFlag, pParam);

	*phHandle = (VO_HANDLE)pSrc;
	return ulRet;
}


VO_U32 vosrcUninit(VO_HANDLE hHandle)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = pSrc->Uninit();
	delete	pSrc;

#ifdef _DEBUG
	_CrtMemCheckpoint( &s2 );
	if ( _CrtMemDifference( &s3, &s1, &s2) )
		_CrtMemDumpStatistics( &s3 );
	_CrtDumpMemoryLeaks();	
#endif	
	return VO_RET_SOURCE2_OK;
}

VO_U32 vosrcParse(VO_HANDLE hHandle, VO_PBYTE pData, VO_U32 nSize, VO_PTR* ppSmilStruct)
{
	VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	if(pSrc->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	ulRet = pSrc->Parse(pData, nSize, ppSmilStruct);
	return ulRet;
}


VO_S32 VO_API voGetSMILAPI(VO_SMIL_API* pHandle)
{
	pHandle->Init	= vosrcInit;
	pHandle->Uninit	= vosrcUninit;	
	pHandle->Parse	= vosrcParse;
	return VO_RET_SOURCE2_OK;
} 

#ifdef __cplusplus
}
#endif

