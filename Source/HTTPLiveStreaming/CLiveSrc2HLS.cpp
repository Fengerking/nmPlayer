#include <stdio.h>
#include <stdlib.h>

#include "CLiveSrc2HLS.h"
#include "voOSFunc.h"
#include "cmnFile.h"
#include "voLog.h"
#include "vompType.h"

#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

_CrtMemState s1_v2;
_CrtMemState s2_v2;
_CrtMemState s3_v2;
char OutputStr_v2[256];
#endif	//_WIN32


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CLiveSrc2HLS::CLiveSrc2HLS()
:m_pHLS (NULL)
{
#ifdef _WIN32
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtMemCheckpoint( &s1_v2 );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
#endif

    m_pHLS = new vo_http_live_streaming_new();
}

CLiveSrc2HLS::~CLiveSrc2HLS()
{
	if (m_pHLS != NULL)
	{
		m_pHLS->Uninit_HLS();
		delete m_pHLS;
		m_pHLS = NULL;
	}
#ifdef _WIN32
	memset(OutputStr_v2, 0, 256);
	_CrtMemCheckpoint( &s2_v2 );
	if ( _CrtMemDifference( &s3_v2, &s1_v2, &s2_v2) )
		_CrtMemDumpStatistics( &s3_v2 );
	sprintf(OutputStr_v2,"ThreadID: %d\n",GetCurrentThreadId());
	OutputDebugStringA(OutputStr_v2);
#endif
}

VO_U32 CLiveSrc2HLS::Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    if((pInitParam != NULL) && (pInitParam->pInitParam != NULL))
    {
        SetParam(pInitParam->uFlag, pInitParam->pInitParam);
    }

	ulRet = m_pHLS->Init_HLS(pSource, nFlag, pInitParam);

    return ulRet;
}

VO_U32 CLiveSrc2HLS::Uninit()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}


	ulRet = m_pHLS->Uninit_HLS();

    return ulRet;
}

VO_U32 CLiveSrc2HLS::Open()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}


	ulRet = m_pHLS->Open_HLS();
    return ulRet;
}

VO_U32 CLiveSrc2HLS::Close()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pHLS->Close_HLS();
    return ulRet;
}
VO_U32 CLiveSrc2HLS::Start()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pHLS->Start_HLS();
    return ulRet;
}
	
VO_U32 CLiveSrc2HLS::Pause()
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pHLS->Pause_HLS();
    return ulRet;
}
	
VO_U32 CLiveSrc2HLS::Stop()
{
	VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pHLS->Stop_HLS();
    return ulRet;
}
	
VO_U32 CLiveSrc2HLS::Seek(VO_U64* pTimeStamp)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pHLS->Seek_HLS(pTimeStamp);
    return ulRet;
}
	
VO_U32 CLiveSrc2HLS::GetDuration(VO_U64 * pDuration)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pHLS->GetDuration_HLS(pDuration);
    return ulRet;
}
	
VO_U32 CLiveSrc2HLS::GetSample(VO_U32 nOutPutType , VO_PTR pSample )
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;
	
	if (m_pHLS == NULL)
	{
	    return ulRet;
	}

	ulRet = m_pHLS->GetSample_HLS(nOutPutType, pSample );
    return ulRet;
}
	
VO_U32 CLiveSrc2HLS::GetProgramCount(VO_U32 *pProgramCount)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = m_pHLS->GetProgramCounts_HLS(pProgramCount);

	return ulRet;
}
	
VO_U32 CLiveSrc2HLS::GetProgramInfo(VO_U32 nProgramID, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pHLS->GetProgramInfo_HLS(nProgramID, ppProgramInfo);
	return ulRet;
}
	
VO_U32 CLiveSrc2HLS::GetCurSelTrackInfo(VO_U32 nOutPutType , VO_SOURCE2_TRACK_INFO **ppTrackInfo)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	m_pHLS->GetCurSelTrackInfo_HLS(nOutPutType, ppTrackInfo);

	return ulRet;
}

VO_U32 CLiveSrc2HLS::SelectProgram(VO_U32 nProgramID )
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pHLS->SelectProgram_HLS(nProgramID);
	return ulRet;
}

VO_U32 CLiveSrc2HLS::SelectStream(VO_U32 nStreamID )
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pHLS->SelectStream_HLS(nStreamID);
	return ulRet;
}

	
VO_U32 CLiveSrc2HLS::SelectTrack(VO_U32 nTrackID )
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = m_pHLS->SelectTrack_HLS(nTrackID);
	return ulRet;
}
	
VO_U32 CLiveSrc2HLS::GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	return ulRet;
}


VO_U32 CLiveSrc2HLS::SendBuffer(const VO_SOURCE2_SAMPLE& buffer)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	return VO_RET_SOURCE2_NOIMPLEMENT;
}

VO_U32 CLiveSrc2HLS::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;

	if (m_pHLS == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}


	return ulRet;
}
	
VO_U32 CLiveSrc2HLS::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32   ulRet = VO_RET_SOURCE2_OK;


    VOLOGI("the nParamID:%d", nParamID);
	if (m_pHLS == NULL || pParam == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	switch(nParamID)
	{
	    case VO_PID_SOURCE2_LIBOP:
		{
			m_pHLS->set_libop((void *) pParam);
			break;
		}

        case VO_PID_SOURCE2_DRMCALLBACK:
        {
            VOLOGI("set the VO_PID_SOURCE2_DRMCALLBACK");
			m_pHLS->set_DRM((void *) pParam);
            break;
        }

        case VO_PID_SOURCE2_IO:
        {
            VOLOGI("set the VO_PID_SOURCE2_IO");
            break;
        }

        case VO_PID_COMMON_LOGFUNC:
        {
            m_pHLS->SetParamForLog((VO_VOID*) pParam);
            VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
            vologInit (pVologCB->pUserData, pVologCB->fCallBack);
            m_pHLS->SetParamToParser();
            break;
        }

        case VO_PID_SOURCE2_CPUINFO:
        {
            if(pParam != NULL)
            {
                m_pHLS->setCpuInfo(pParam);
            }
            
            break;
        }

        case VO_PID_SOURCE2_BACAP:
        {
            if(pParam != NULL)
            {
                m_pHLS->setCapInfo(pParam);
            }
            
            break;
        }
        case VO_PID_SOURCE2_DOHTTPVERIFICATION:
        {            
            VOLOGI("set the HTTPVERIFICATION");
            if(pParam == NULL)
            {
                return VO_RET_SOURCE2_EMPTYPOINTOR;
            }
            m_pHLS->SetVerificationInfo(pParam);
            
            break;
        }

        case VO_PID_SOURCE2_BA_STARTCAP:
        {
            if(pParam == NULL)
            {
                return VO_RET_SOURCE2_EMPTYPOINTOR;
            }
            VOLOGI("set the Start Bitrate, the string");
            m_pHLS->setStartCap((VO_VOID*) pParam);
            
            break;
        }
        case VO_PID_SOURCE2_EVENTCALLBACK:
        {
            if(pParam == NULL)
            {
                VOLOGI("the eventcallback is NULL!");
            }
            else
            {   
                VOLOGI("the eventcallback is ok");
                VO_SOURCE2_EVENTCALLBACK*   pEventCallback = (VO_SOURCE2_EVENTCALLBACK*)pParam;
                m_pHLS->set_eventcallbackForManifest(pEventCallback);
                m_pHLS->set_eventcallback(pEventCallback->pUserData, pEventCallback->SendEvent);
            }
            
            break;
        }
        case VO_PID_SOURCE2_DOWNLOAD_FAIL_MAX_TOLERANT_COUNT:
        {
            m_pHLS->setTheMaxDownloadFailTolerantCount(pParam);
            break;
        }
        case VO_PID_SOURCE2_CPU_BA_WORKMODE:
        {
            m_pHLS->setCPUWorkMode(pParam);
            break;
        }
		default:
		{
			break;
		}
	}

	return ulRet;
}
