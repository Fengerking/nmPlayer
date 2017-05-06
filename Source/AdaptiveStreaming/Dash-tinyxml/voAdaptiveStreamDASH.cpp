#include "C_DASH_Entity.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define SOURCE_CLASS C_DASH_Entity


#define SOUORCE_OBJ ((SOURCE_CLASS*)phHandle)
#ifdef _IOS
    #ifdef _DASH_SOURCE_
 	#define MODULENAME DASH
 	#elif defined _DASH_SOURCE_
 	#define MODULENAME DASH
 	#endif
#else
#define MODULENAME DASH
#endif

#define MODULEVOSRCINIT(name) vosrcInit##name
#define vosrcInit(name) MODULEVOSRCINIT(name)

#define MODULEVOSRCUNINIT(name) vosrcUninit##name
#define vosrcUninit(name) MODULEVOSRCUNINIT(name)

#define MODULEVOSRCSTOP(name) vosrcStop##name
#define vosrcStop(name) MODULEVOSRCSTOP(name)
	
#define MODULEVOSRCOPEN(name) vosrcOpen##name
#define vosrcOpen(name) MODULEVOSRCOPEN(name)

#define MODULEVOSRCSTART(name) vosrcStart##name
#define vosrcStart(name) MODULEVOSRCSTART(name)

#define MODULEVOSRCCLOSE(name) vosrcClose##name
#define vosrcClose(name) MODULEVOSRCCLOSE(name)

#define MODULEVOSRCUPDATE(name) vosrcUpdate##name
#define vosrcUpdate(name) MODULEVOSRCUPDATE(name)

#define MODULEVOSRCGETCHUNK(name) vosrcGetChunk##name
#define vosrcGetChunk(name) MODULEVOSRCGETCHUNK(name)


#define MODULEVOSRCSEEK(name) vosrcSeek##name
#define vosrcSeek(name) MODULEVOSRCSEEK(name)

#define MODULEVOSRCGETDURATION(name) vosrcGetDuration##name
#define vosrcGetDuration(name) MODULEVOSRCGETDURATION(name)

#define MODULEVOSRCGETPROGRAMCOUNT(name) vosrcGetProgramCount##name
#define vosrcGetProgramCount(name) MODULEVOSRCGETPROGRAMCOUNT(name)

#define MODULEVOSRCGETPROGRAMINFO(name) vosrcGetProgramInfo##name
#define vosrcGetProgramInfo(name) MODULEVOSRCGETPROGRAMINFO(name)

#define MODULEVOSRCGETCURTRACKINFO(name) vosrcGetCurTrackInfo##name
#define vosrcGetCurTrackInfo(name) MODULEVOSRCGETCURTRACKINFO(name)

#define MODULEVOSRCSELECTPROGRAM(name) vosrcSelectProgram##name
#define vosrcSelectProgram(name) MODULEVOSRCSELECTPROGRAM(name)

#define MODULEVOSRCSELECTSTREAM(name) vosrcSelectStream##name
#define vosrcSelectStream(name) MODULEVOSRCSELECTSTREAM(name)

#define MODULEVOSRCSELECTTRACK(name) vosrcSelectTrack##name
#define vosrcSelectTrack(name) MODULEVOSRCSELECTTRACK(name)

#define MODULEVOSRCGETDRMINFO(name) vosrcGetDRMInfo##name
#define vosrcGetDRMInfo(name) MODULEVOSRCGETDRMINFO(name)

#define MODULEVOSRCGETPARAM(name) vosrcGetParam##name
#define vosrcGetParam(name) MODULEVOSRCGETPARAM(name)

#define MODULEVOSRCSETPARAM(name) vosrcSetParam##name
#define vosrcSetParam(name) MODULEVOSRCSETPARAM(name)

#if defined __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <Windows.h>
BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	return TRUE;
}
#endif	//_WIN32


VO_U32 vosrcInit(MODULENAME) (VO_HANDLE * phHandle, VO_ADAPTIVESTREAM_PLAYLISTDATA * pData , VO_SOURCE2_EVENTCALLBACK * pCallback)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = new SOURCE_CLASS();
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	pSrc->Init_DASH(pData,pCallback);

	*phHandle = (VO_HANDLE)pSrc;
	return VO_RET_SOURCE2_OK;
}

VO_U32 vosrcStart(MODULENAME) (VO_HANDLE  hHandle)
{
	VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	ulRet = pSrc->Start_DASH();
	return VO_RET_SOURCE2_OK;
}

VO_U32 vosrcStop(MODULENAME) (VO_HANDLE  hHandle)
{
	VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	ulRet = pSrc->Stop_DASH();
	return VO_RET_SOURCE2_OK;
}
VO_U32 vosrcUninit(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->Uninit_DASH();
	delete  pSrc;
	return VO_RET_SOURCE2_OK;
}


VO_U32 vosrcOpen(MODULENAME) (VO_HANDLE hHandle)
{   
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->Open_DASH();
	return ulRet;
}

VO_U32 vosrcClose(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->Close_DASH();
	return ulRet;
}

VO_U32 vosrcUpdate(MODULENAME) (VO_HANDLE hHandle, VO_ADAPTIVESTREAM_PLAYLISTDATA * pData)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->Update_DASH( pData);
	return ulRet;
}

VO_U32 vosrcGetChunk(MODULENAME) (VO_HANDLE hHandle ,VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->GetChunk_DASH(uID, ppChunk);
	//VOLOGE("DASH V2  GetChunk_DASH to %s",(*ppChunk)->szUrl);
	return ulRet;
}


VO_U32 vosrcSeek(MODULENAME) (VO_HANDLE hHandle, VO_U64* pTimeStamp)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	ulRet = pSrc->Seek_DASH(pTimeStamp);
	return ulRet;
}

VO_U32 vosrcGetDuration(MODULENAME) (VO_HANDLE hHandle, VO_U64* pDuration)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	ulRet = pSrc->GetDuration_DASH(pDuration);
	return ulRet;
}


VO_U32 vosrcGetProgramCount(MODULENAME) (VO_HANDLE hHandle, VO_U32 *pProgramCount)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
		
	ulRet = pSrc->GetProgramCounts_DASH(pProgramCount);
	return ulRet;
}

VO_U32 vosrcGetProgramInfo(MODULENAME) (VO_HANDLE hHandle, VO_U32 nProgramId, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
		
	ulRet = pSrc->GetProgramInfo_DASH(nProgramId, ppProgramInfo);
	return ulRet;
}

VO_U32 vosrcGetCurTrackInfo(MODULENAME) (VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nOutPutType , VO_SOURCE2_TRACK_INFO **ppTrackInfo )
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
		
	ulRet = pSrc->GetCurTrackInfo_DASH(nOutPutType, ppTrackInfo);
	return ulRet;
}

VO_U32 vosrcSelectProgram(MODULENAME) (VO_HANDLE hHandle, VO_U32 nProgramID)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
		
	ulRet = pSrc->SelectProgram_DASH(nProgramID);
	return ulRet;
}

VO_U32 vosrcSelectStream(MODULENAME) (VO_HANDLE hHandle, VO_U32 nStreamID)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	ulRet = pSrc->SelectStream_DASH(nStreamID);	
	//ulRet = pSrc->SelectStream_DASH(nStreamID);
	return ulRet;
}

VO_U32 vosrcSelectTrack(MODULENAME) (VO_HANDLE hHandle, VO_U32 nTrackID)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
		
	ulRet = pSrc->SelectTrack_DASH(nTrackID);
	return ulRet;
}


VO_U32  vosrcGetDRMInfo(MODULENAME) (VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO* ppDRMInfo)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
		
	ulRet = pSrc->GetDRMInfo_DASH(ppDRMInfo);
	return ulRet;
}


VO_U32 vosrcGetParam(MODULENAME) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
		
	ulRet = pSrc->GetParam_DASH(nParamID, pParam);
	return ulRet;
}

VO_U32 vosrcSetParam(MODULENAME) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
		
	ulRet = pSrc->SetParam_DASH(nParamID, pParam);
	return ulRet;

}

VO_S32 VO_API voGetAdaptiveStreamDASHAPI(VO_ADAPTIVESTREAM_PARSER_API* pHandle)
{
    pHandle->Init       = vosrcInit(MODULENAME) ;
    pHandle->UnInit     = vosrcUninit(MODULENAME) ;
	pHandle->Start =     vosrcStart(MODULENAME);
	pHandle->Stop  =     vosrcStop(MODULENAME);
	pHandle->Open		= vosrcOpen(MODULENAME) ;
	pHandle->Close		= vosrcClose(MODULENAME) ;
	pHandle->Update     = vosrcUpdate(MODULENAME);
    pHandle->GetChunk    = vosrcGetChunk(MODULENAME);
	pHandle->Seek		= vosrcSeek(MODULENAME) ;
    pHandle->GetDuration = vosrcGetDuration(MODULENAME) ;
	pHandle->GetProgramCount	= vosrcGetProgramCount(MODULENAME) ;
	pHandle->GetProgramInfo	= vosrcGetProgramInfo(MODULENAME) ;
	pHandle->GetCurTrackInfo	= vosrcGetCurTrackInfo(MODULENAME) ;
	pHandle->SelectProgram		= vosrcSelectProgram(MODULENAME) ;
	pHandle->SelectStream		= vosrcSelectStream(MODULENAME) ;    
	pHandle->SelectTrack		= vosrcSelectTrack(MODULENAME) ;
	pHandle->GetParam	= vosrcGetParam(MODULENAME) ;
	pHandle->SetParam	= vosrcSetParam(MODULENAME) ;

	return VO_RET_SOURCE2_OK;
} 

#if defined __cplusplus
}
#endif

