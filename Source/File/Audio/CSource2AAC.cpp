	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CSource2AAC.cpp

	Contains:	Implement the AAC Source2 Interface

	Written by:	Aiven

	Change History (most recent first):
	2012-04-11		Aiven			Create file

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "CSource2AAC.h"
#include "voOSFunc.h"
#include "cmnFile.h"
#include "voLog.h"
#include "vompType.h"
#include "voCheck.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#ifdef WINCE
#elif defined(_SELFTEST)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

_CrtMemState s1;
_CrtMemState s2;
_CrtMemState s3;
char OutputStr[256];
#endif	//_SELFTEST


CSource2AAC::CSource2AAC()
:m_pAacParser (NULL)
,m_hCheck(NULL)
{
#ifdef WINCE
#elif defined(_SELFTEST)
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtMemCheckpoint( &s1 );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
#endif

    m_pAacParser = new CAacParser();
}

CSource2AAC::~CSource2AAC()
{
	if (m_pAacParser != NULL)
	{
		m_pAacParser->Uninit();
		delete m_pAacParser;
		m_pAacParser = NULL;
	}
#ifdef WINCE
#elif defined(_SELFTEST)
	memset(OutputStr, 0, 256);
	_CrtMemCheckpoint( &s2 );
	if ( _CrtMemDifference( &s3, &s1, &s2) )
		_CrtMemDumpStatistics( &s3 );
	sprintf(OutputStr,"ThreadID: %d\n",GetCurrentThreadId());
	OutputDebugStringA(OutputStr);
#endif
}

VO_U32 CSource2AAC::Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_TCHAR *  pWorkingPath)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
//	voCAutoLock autolock(&m_AutoLock);

	VOLOGINIT(pWorkingPath);
	ulRet = voCheckLibInit(&m_hCheck, VO_INDEX_SRC_AUDIO, VO_LCS_WORKPATH_FLAG, 0, pWorkingPath);
	if (ulRet != VO_RET_SOURCE2_OK)
	{
		VOLOGE("CheckLib Fail!!!  pWorkingPath=%s",pWorkingPath);
		return ulRet;
	}

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pAacParser->Init(pSource , nFlag , pCallback );
//	m_pCallback = pCallback;
    return ulRet;
}

VO_U32 CSource2AAC::Uninit()
{
//	voCAutoLock autolock(&m_AutoLock);
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (NULL != m_hCheck)
	{
		ulRet = voCheckLibUninit(m_hCheck);
		m_hCheck = NULL;
	}

	if (m_pAacParser == NULL)
	{
		VOLOGUNINIT();
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}


	ulRet = m_pAacParser->Uninit();
	VOLOGUNINIT();
    return ulRet;
}

VO_U32 CSource2AAC::Open()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    return ulRet;
}

VO_U32 CSource2AAC::Close()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

//	ulRet = m_pAacParser->Close();
    return ulRet;
}
VO_U32 CSource2AAC::Start()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

//	ulRet = m_pAacParser->Start();
    return ulRet;
}
	
VO_U32 CSource2AAC::Pause()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

//	ulRet = m_pAacParser->Pause();
    return ulRet;
}
	
VO_U32 CSource2AAC::Stop()
{
	VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

//	ulRet = m_pAacParser->Stop();
    return ulRet;
}
	
VO_U32 CSource2AAC::Seek(VO_U64* pTimeStamp)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

//	ulRet = m_pAacParser->Seek(pTimeStamp);
    return ulRet;
}
	
VO_U32 CSource2AAC::GetDuration(VO_U64 * pDuration)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

//	ulRet = m_pAacParser->GetDuration(pDuration);
    return ulRet;
}
	
VO_U32 CSource2AAC::GetSample(VO_U32 nOutPutType , VO_PTR pSample )
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
	
	if (m_pAacParser == NULL)
	{
	    return ulRet;
	}

//	ulRet = m_pAacParser->GetSample(nOutPutType, pSample );
    return ulRet;
}
	
VO_U32 CSource2AAC::GetStreamCount(VO_U32 *pStreamCount)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
//	ulRet = m_pAacParser->GetStreamCounts(pStreamCount);

	return ulRet;
}
	
VO_U32 CSource2AAC::GetStreamInfo(VO_U32 nStreamID, VO_SOURCE2_STREAM_INFO **ppStreamInfo)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

//	ulRet = m_pAacParser->GetStreamInfo(nStreamID, ppStreamInfo);
	return ulRet;
}
	
VO_U32 CSource2AAC::GetCurSelTrackInfo(VO_U32 nOutPutType , VO_SOURCE2_TRACK_INFO **ppTrackInfo)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

//	m_pAacParser->GetCurSelTrackInfo(nOutPutType, ppTrackInfo);

	return ulRet;
}
	
VO_U32 CSource2AAC::SelectTrack(VO_U32 nStreamID , VO_U32 nSubStreamID , VO_U32 nTrackID )
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

//	ulRet = m_pAacParser->SelectTrack(nStreamID, nSubStreamID, nTrackID);
	return ulRet;
}
	
VO_U32 CSource2AAC::GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	return ulRet;
}


VO_U32 CSource2AAC::SendBuffer(const VO_SOURCE2_SAMPLE& buffer)
{
	VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pAacParser->ReceiveData(buffer.pBuffer, buffer.uSize);

	return ulRet;
}

VO_U32 CSource2AAC::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pAacParser == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}


	return ulRet;
}
	
VO_U32 CSource2AAC::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32   ulRet = VO_RET_SOURCE2_OK;

	VOLOGI("the nParamID:%d", nParamID);
	if (m_pAacParser == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	switch(nParamID)
	{
	case VO_AAC2_PARAM_RESET:
		m_pAacParser->Reset();
		ulRet = VO_RET_SOURCE2_OK;
		break;
	case VO_PID_COMMON_LOGFUNC:
	{
	//	VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
	//	vologInit (pVologCB->pUserData, pVologCB->fCallBack);
	}
		break;
	default:
		break;
	}
	
	return ulRet;
}




