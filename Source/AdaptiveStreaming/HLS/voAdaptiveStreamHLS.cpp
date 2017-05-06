#include "voLog.h"
#include "hls_entity.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define SOURCE_CLASS C_HLS_Entity


#define SOUORCE_OBJ ((SOURCE_CLASS*)phHandle)

#ifdef _IOS
    #ifdef _ISS_SOURCE_
 	#define MODULENAME ISS
 	#elif defined _HLS_SOURCE_
 	#define MODULENAME HLS
 	#endif
#else
#define MODULENAME 
#endif

#define MODULEyysrcINIT(name) yysrcInit##name
#define yysrcInit(name) MODULEyysrcINIT(name)

#define MODULEyysrcUNINIT(name) yysrcUninit2##name
#define yysrcUninit(name) MODULEyysrcUNINIT(name)

#define MODULEyysrcOPEN(name) yysrcOpen##name
#define yysrcOpen(name) MODULEyysrcOPEN(name)

#define MODULEyysrcCLOSE(name) yysrcClose##name
#define yysrcClose(name) MODULEyysrcCLOSE(name)

#define MODULEyysrcSTART(name) yysrcStart##name
#define yysrcStart(name) MODULEyysrcSTART(name)

#define MODULEyysrcSTOP(name) yysrcStop##name
#define yysrcStop(name) MODULEyysrcSTOP(name)

#define MODULEyysrcUPDATE(name) yysrcUpdate##name
#define yysrcUpdate(name) MODULEyysrcUPDATE(name)

#define MODULEyysrcGETCHUNCK(name) yysrcGetChunck##name
#define yysrcGetChunck(name) MODULEyysrcGETCHUNCK(name)

#define MODULEyysrcSEEK(name) yysrcSeek##name
#define yysrcSeek(name) MODULEyysrcSEEK(name)

#define MODULEyysrcGETDURATION(name) yysrcGetDuration##name
#define yysrcGetDuration(name) MODULEyysrcGETDURATION(name)

#define MODULEyysrcGETPROGRAMCOUNT(name) yysrcGetProgramCount##name
#define yysrcGetProgramCount(name) MODULEyysrcGETPROGRAMCOUNT(name)

#define MODULEyysrcGETPROGRAMINFO(name) yysrcGetProgramInfo##name
#define yysrcGetProgramInfo(name) MODULEyysrcGETPROGRAMINFO(name)

#define MODULEyysrcGETCURTRACKINFO(name) yysrcGetCurTrackInfo##name
#define yysrcGetCurTrackInfo(name) MODULEyysrcGETCURTRACKINFO(name)

#define MODULEyysrcSELECTPROGRAM(name) yysrcSelectProgram##name
#define yysrcSelectProgram(name) MODULEyysrcSELECTPROGRAM(name)

#define MODULEyysrcSELECTSTREAM(name) yysrcSelectStream##name
#define yysrcSelectStream(name) MODULEyysrcSELECTSTREAM(name)

#define MODULEyysrcSELECTTRACK(name) yysrcSelectTrack##name
#define yysrcSelectTrack(name) MODULEyysrcSELECTTRACK(name)

#define MODULEyysrcGETDRMINFO(name) yysrcGetDRMInfo##name
#define yysrcGetDRMInfo(name) MODULEyysrcGETDRMINFO(name)

#define MODULEyysrcGETPARAM(name) yysrcGetParam##name
#define yysrcGetParam(name) MODULEyysrcGETPARAM(name)

#define MODULEyysrcSETPARAM(name) yysrcSetParam##name
#define yysrcSetParam(name) MODULEyysrcSETPARAM(name)

#if defined __cplusplus
extern "C" {
#endif

#ifndef _LIB
#ifdef _WIN32
#include <Windows.h>
BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	return TRUE;
}
#endif	//_WIN32
#endif // _LIB

VO_U32 yysrcInit(MODULENAME) (VO_HANDLE * phHandle, VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_SOURCE2_EVENTCALLBACK* pCallback, VO_ADAPTIVESTREAMPARSER_INITPARAM * pInitParam)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = new SOURCE_CLASS();
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = pSrc->Init_HLS(pData, pCallback, pInitParam);
	*phHandle = (VO_HANDLE)pSrc;
	return ulRet;
}


VO_U32 yysrcUninit(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->Uninit_HLS();
	delete  pSrc;
	return VO_RET_SOURCE2_OK;
}


VO_U32 yysrcOpen(MODULENAME) (VO_HANDLE hHandle)
{   
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
    if(pSrc->GetLicState() == VO_FALSE)
    {
        return VO_RET_SOURCE2_OPENFAIL;
    }
    
	ulRet = pSrc->Open_HLS();
	return ulRet;
}

VO_U32 yysrcClose(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    if(pSrc->GetLicState() == VO_FALSE)
    {
        return VO_RET_SOURCE2_OPENFAIL;
    }

	ulRet = pSrc->Close_HLS();
	return ulRet;
}

VO_U32 yysrcStart(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    if(pSrc->GetLicState() == VO_FALSE)
    {
        return VO_RET_SOURCE2_OPENFAIL;
    }

	ulRet = pSrc->Start_HLS();
	return ulRet;
}

VO_U32 yysrcStop(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    if(pSrc->GetLicState() == VO_FALSE)
    {
        return VO_RET_SOURCE2_OPENFAIL;
    }

	ulRet = pSrc->Stop_HLS();
	return ulRet;
}

VO_U32 yysrcUpdate(MODULENAME) (VO_HANDLE hHandle, VO_ADAPTIVESTREAM_PLAYLISTDATA * pData)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
    
    if(pSrc->GetLicState() == VO_FALSE)
    {
        return VO_RET_SOURCE2_OPENFAIL;
    }
    
	ulRet = pSrc->Update_HLS(pData);
	return ulRet;
}


VO_U32 yysrcGetChunck(MODULENAME) (VO_HANDLE hHandle, VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk )
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    if(pSrc->GetLicState() == VO_FALSE)
    {
        return VO_RET_SOURCE2_OPENFAIL;
    }

	ulRet = pSrc->GetChunk_HLS(uID, ppChunk);
	return ulRet;
}


VO_U32 yysrcSeek(MODULENAME) (VO_HANDLE hHandle, VO_U64* pTimeStamp, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    if(pSrc->GetLicState() == VO_FALSE)
    {
        return VO_RET_SOURCE2_OPENFAIL;
    }

	ulRet = pSrc->Seek_HLS(pTimeStamp, sSeekMode);
	return ulRet;
}

VO_U32 yysrcGetDuration(MODULENAME) (VO_HANDLE hHandle, VO_U64* pDuration)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

    if(pSrc->GetLicState() == VO_FALSE)
    {
        return VO_RET_SOURCE2_OPENFAIL;
    }

	ulRet = pSrc->GetDuration_HLS(pDuration);
	return ulRet;
}

VO_U32 yysrcGetProgramCount(MODULENAME) (VO_HANDLE hHandle, VO_U32 *pProgramCount)
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

	ulRet = pSrc->GetProgramCounts_HLS(pProgramCount);
	return ulRet;
}

VO_U32 yysrcGetProgramInfo(MODULENAME) (VO_HANDLE hHandle, VO_U32 nProgramId, _PROGRAM_INFO **ppProgramInfo)
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

	ulRet = pSrc->GetProgramInfo_HLS(nProgramId, ppProgramInfo);
	return ulRet;
}

VO_U32 yysrcGetCurTrackInfo(MODULENAME) (VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nOutPutType , _TRACK_INFO **ppTrackInfo )
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
		
	ulRet = pSrc->GetCurTrackInfo_HLS(nOutPutType, ppTrackInfo);
	return ulRet;
}

VO_U32 yysrcSelectProgram(MODULENAME) (VO_HANDLE hHandle, VO_U32 nProgramID)
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
    		
	ulRet = pSrc->SelectProgram_HLS(nProgramID);
	return ulRet;
}

VO_U32 yysrcSelectStream(MODULENAME) (VO_HANDLE hHandle, VO_U32 nStreamID, VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos)
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
	
	ulRet = pSrc->SelectStream_HLS(nStreamID, sPrepareChunkPos);
	return ulRet;
}

VO_U32 yysrcSelectTrack(MODULENAME) (VO_HANDLE hHandle, VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE nType)
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
        
	ulRet = pSrc->SelectTrack_HLS(nType, nTrackID);
	return ulRet;
}


VO_U32  yysrcGetDRMInfo(MODULENAME) (VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO** ppDRMInfo)
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
        
	ulRet = pSrc->GetDRMInfo_HLS(ppDRMInfo);
	return ulRet;
}


VO_U32 yysrcGetParam(MODULENAME) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
		
	ulRet = pSrc->GetParam_HLS(nParamID, pParam);
	return ulRet;
}

VO_U32 yysrcSetParam(MODULENAME) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32	 ulRet = 0;
    if(VO_PID_COMMON_LOGFUNC == nParamID)
    {
        if(pParam != NULL)
        {
        //    VO_LOG_PRINT_CB*  pVologCB = (VO_LOG_PRINT_CB *)pParam;
        //    vologInit (pVologCB->pUserData, pVologCB->fCallBack);
        }
        return 0;
    }
    
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
		
	ulRet = pSrc->SetParam_HLS(nParamID, pParam);
	return ulRet;

}

VO_S32 VO_API yyGetHLSParserFunc(VO_ADAPTIVESTREAM_PARSER_API* pHandle)
{
    pHandle->Init       = yysrcInit(MODULENAME) ;
    pHandle->UnInit     = yysrcUninit(MODULENAME) ;	
	pHandle->Open		= yysrcOpen(MODULENAME) ;
	pHandle->Close		= yysrcClose(MODULENAME) ;
	pHandle->Start		= yysrcStart(MODULENAME) ;
	pHandle->Stop		= yysrcStop(MODULENAME) ;	
    pHandle->Update     = yysrcUpdate(MODULENAME) ;
    pHandle->GetChunk   = yysrcGetChunck(MODULENAME) ;
	pHandle->Seek		= yysrcSeek(MODULENAME) ;
    pHandle->GetDuration = yysrcGetDuration(MODULENAME) ;
	pHandle->GetProgramCount	= yysrcGetProgramCount(MODULENAME) ;
	pHandle->GetProgramInfo	= yysrcGetProgramInfo(MODULENAME) ;
	pHandle->GetCurTrackInfo	= yysrcGetCurTrackInfo(MODULENAME) ;
	pHandle->SelectProgram		= yysrcSelectProgram(MODULENAME) ;
	pHandle->SelectStream		= yysrcSelectStream(MODULENAME) ;    
	pHandle->SelectTrack		= yysrcSelectTrack(MODULENAME) ;
	pHandle->GetDRMInfo = yysrcGetDRMInfo(MODULENAME) ;
	pHandle->GetParam	= yysrcGetParam(MODULENAME) ;
	pHandle->SetParam	= yysrcSetParam(MODULENAME) ;

	return VO_RET_SOURCE2_OK;
} 

#if defined __cplusplus
}
#endif

