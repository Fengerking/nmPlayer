#include "voHLS2.h"
#include "CLiveSrc2HLS.h"
#define SOURCE_CLASS CLiveSrc2HLS



#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


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

#define MODULEVOSRCINIT2(name) vosrcInit2##name
#define vosrcInit2(name) MODULEVOSRCINIT2(name)

#define MODULEVOSRCUNINIT2(name) vosrcUninit2##name
#define vosrcUninit2(name) MODULEVOSRCUNINIT2(name)

#define MODULEVOSRCOPEN2(name) vosrcOpen2##name
#define vosrcOpen2(name) MODULEVOSRCOPEN2(name)

#define MODULEVOSRCCLOSE2(name) vosrcClose2##name
#define vosrcClose2(name) MODULEVOSRCCLOSE2(name)

#define MODULEVOSRCSTART2(name) vosrcStart2##name
#define vosrcStart2(name) MODULEVOSRCSTART2(name)

#define MODULEVOSRCPAUSE2(name) vosrcPause2##name
#define vosrcPause2(name) MODULEVOSRCPAUSE2(name)

#define MODULEVOSRCSTOP2(name) vosrcStop2##name
#define vosrcStop2(name) MODULEVOSRCSTOP2(name)

#define MODULEVOSRCSEEK2(name) vosrcSeek2##name
#define vosrcSeek2(name) MODULEVOSRCSEEK2(name)

#define MODULEVOSRCGETDURATION2(name) vosrcGetDuration2##name
#define vosrcGetDuration2(name) MODULEVOSRCGETDURATION2(name)

#define MODULEVOSRCGETSAMPLE2(name) vosrcGetSample2##name
#define vosrcGetSample2(name) MODULEVOSRCGETSAMPLE2(name)

#define MODULEVOSRCGETPROGRAMCOUNT2(name) vosrcGetProgramCount2##name
#define vosrcGetProgramCount2(name) MODULEVOSRCGETPROGRAMCOUNT2(name)

#define MODULEVOSRCGETPROGRAMINFO2(name) vosrcGetProgramInfo2##name
#define vosrcGetProgramInfo2(name) MODULEVOSRCGETPROGRAMINFO2(name)

#define MODULEVOSRCGETCURTRACKINFO2(name) vosrcGetCurTrackInfo2##name
#define vosrcGetCurTrackInfo2(name) MODULEVOSRCGETCURTRACKINFO2(name)

#define MODULEVOSRCSELECTPROGRAM2(name) vosrcSelectProgram2##name
#define vosrcSelectProgram2(name) MODULEVOSRCSELECTPROGRAM2(name)

#define MODULEVOSRCSELECTSTREAM2(name) vosrcSelectStream2##name
#define vosrcSelectStream2(name) MODULEVOSRCSELECTSTREAM2(name)

#define MODULEVOSRCSELECTTRACK2(name) vosrcSelectTrack2##name
#define vosrcSelectTrack2(name) MODULEVOSRCSELECTTRACK2(name)

#define MODULEVOSRCGETDRMINFO2(name) vosrcGetDRMInfo2##name
#define vosrcGetDRMInfo2(name) MODULEVOSRCGETDRMINFO2(name)

#define MODULEVOSRCSENDBUFFER2(name) vosrcSendBuffer2##name
#define vosrcSendBuffer2(name) MODULEVOSRCSENDBUFFER2(name)

#define MODULEVOSRCGETPARAM2(name) vosrcGetParam2##name
#define vosrcGetParam2(name) MODULEVOSRCGETPARAM2(name)

#define MODULEVOSRCSETPARAM2(name) vosrcSetParam2##name
#define vosrcSetParam2(name) MODULEVOSRCSETPARAM2(name)

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


VO_U32 vosrcInit2(MODULENAME) (VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam )
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = new SOURCE_CLASS();
	if (pSrc == NULL)
    {
        return VO_ERR_LIVESRC_OUTOFMEMORY;
	}

	pSrc->Init(pSource, nFlag, pInitParam);

	*phHandle = (VO_HANDLE)pSrc;
	return VO_ERR_LIVESRC_OK;
}


VO_U32 vosrcUninit2(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_ERR_LIVESRC_NULLPOINTER;
	}
	
	ulRet = pSrc->Uninit();
	delete  pSrc;
	return VO_ERR_LIVESRC_OK;
}


VO_U32 vosrcOpen2(MODULENAME) (VO_HANDLE hHandle)
{   
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_ERR_LIVESRC_NULLPOINTER;
	}
	
	ulRet = pSrc->Open();
	return ulRet;
}

VO_U32 vosrcClose2(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_ERR_LIVESRC_NULLPOINTER;
	}
	
	ulRet = pSrc->Close();
	return ulRet;
}


VO_U32 vosrcStart2(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_ERR_LIVESRC_NULLPOINTER;
	}
	
	ulRet = pSrc->Start();
	return ulRet;
}

VO_U32 vosrcPause2(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_ERR_LIVESRC_NULLPOINTER;
	}
	
	ulRet = pSrc->Pause();
	return ulRet;
}

VO_U32 vosrcStop2(MODULENAME) (VO_HANDLE hHandle)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_ERR_LIVESRC_NULLPOINTER;
	}
	
	ulRet = pSrc->Stop();
	return ulRet;
}

VO_U32 vosrcSeek2(MODULENAME) (VO_HANDLE hHandle, VO_U64* pTimeStamp)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_ERR_LIVESRC_NULLPOINTER;
	}
	
	ulRet = pSrc->Seek(pTimeStamp);
	return ulRet;
}

VO_U32 vosrcGetDuration2(MODULENAME) (VO_HANDLE hHandle,VO_U64 * pDuration)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_ERR_LIVESRC_NULLPOINTER;
	}
	
	ulRet = pSrc->GetDuration(pDuration);
	return ulRet;
}

VO_U32 vosrcGetSample2(MODULENAME) (VO_HANDLE hHandle , VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample)
{
    VO_U32   ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
    {
        return VO_ERR_LIVESRC_NULLPOINTER;
	}
	
	ulRet = pSrc->GetSample((VO_U32)nOutPutType, pSample);
	return ulRet;
}

VO_U32 vosrcGetProgramCount2(MODULENAME) (VO_HANDLE hHandle, VO_U32 *pProgramCount)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_ERR_LIVESRC_NULLPOINTER;
	}
		
	ulRet = pSrc->GetProgramCount(pProgramCount);
	return ulRet;
}

VO_U32 vosrcGetProgramInfo2(MODULENAME) (VO_HANDLE hHandle, VO_U32 nStream, VO_SOURCE2_PROGRAM_INFO **ppStreamInfo)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_ERR_LIVESRC_NULLPOINTER;
	}
		
	ulRet = pSrc->GetProgramInfo(nStream, ppStreamInfo);
	return ulRet;
}

VO_U32 vosrcGetCurTrackInfo2(MODULENAME) (VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nOutPutType , VO_SOURCE2_TRACK_INFO **ppTrackInfo )
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_ERR_LIVESRC_NULLPOINTER;
	}
		
	ulRet = pSrc->GetCurSelTrackInfo((VO_U32)nOutPutType, ppTrackInfo);
	return ulRet;
}

VO_U32 vosrcSelectProgram2(MODULENAME) (VO_HANDLE hHandle, VO_U32 nProgramID)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_ERR_LIVESRC_NULLPOINTER;
	}
		
	ulRet = pSrc->SelectProgram(nProgramID);
	return ulRet;
}

VO_U32 vosrcSelectStream2(MODULENAME) (VO_HANDLE hHandle, VO_U32 nStreamID)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_ERR_LIVESRC_NULLPOINTER;
	}
		
	ulRet = pSrc->SelectStream(nStreamID);
	return ulRet;
}

VO_U32 vosrcSelectTrack2(MODULENAME) (VO_HANDLE hHandle, VO_U32 nTrackID)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_ERR_LIVESRC_NULLPOINTER;
	}
		
	ulRet = pSrc->SelectTrack(nTrackID);
	return ulRet;
}


VO_U32  vosrcGetDRMInfo2(MODULENAME) (VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_ERR_LIVESRC_NULLPOINTER;
	}
		
	ulRet = pSrc->GetDRMInfo(ppDRMInfo);
	return ulRet;
}


VO_U32  vosrcSendBuffer2(MODULENAME) (VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_ERR_LIVESRC_NULLPOINTER;
	}
		
	ulRet = pSrc->SendBuffer(buffer);
	return ulRet;
}


VO_U32 vosrcGetParam2(MODULENAME) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_ERR_LIVESRC_NULLPOINTER;
	}
		
	ulRet = pSrc->GetParam(nParamID, pParam);
	return ulRet;
}

VO_U32 vosrcSetParam2(MODULENAME) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
    VO_U32	 ulRet = 0;
	SOURCE_CLASS * pSrc = (SOURCE_CLASS *)hHandle;
	if (pSrc == NULL)
	{
	    return VO_ERR_LIVESRC_NULLPOINTER;
	}
		
	ulRet = pSrc->SetParam(nParamID, pParam);
	return ulRet;

}

VO_S32 VO_API voGetSrcHLSAPI(VO_SOURCE2_API* pHandle)
{
    pHandle->Init       = vosrcInit2(MODULENAME) ;
    pHandle->Uninit     = vosrcUninit2(MODULENAME) ;	
	pHandle->Open		= vosrcOpen2(MODULENAME) ;
	pHandle->Close		= vosrcClose2(MODULENAME) ;
	pHandle->Start		= vosrcStart2(MODULENAME) ;
	pHandle->Pause		= vosrcPause2(MODULENAME) ;
	pHandle->Stop		= vosrcStop2(MODULENAME) ;
	pHandle->Seek		= vosrcSeek2(MODULENAME) ;
    pHandle->GetDuration = vosrcGetDuration2(MODULENAME) ;
	pHandle->GetSample	 = vosrcGetSample2(MODULENAME) ;
	pHandle->GetProgramCount	= vosrcGetProgramCount2(MODULENAME) ;
	pHandle->GetProgramInfo	= vosrcGetProgramInfo2(MODULENAME) ;
	pHandle->GetCurTrackInfo	= vosrcGetCurTrackInfo2(MODULENAME) ;
	pHandle->SelectProgram		= vosrcSelectProgram2(MODULENAME) ;
	pHandle->SelectStream		= vosrcSelectStream2(MODULENAME) ;    
	pHandle->SelectTrack		= vosrcSelectTrack2(MODULENAME) ;
	pHandle->GetDRMInfo = vosrcGetDRMInfo2(MODULENAME) ;
	pHandle->SendBuffer = vosrcSendBuffer2(MODULENAME) ;
	pHandle->GetParam	= vosrcGetParam2(MODULENAME) ;
	pHandle->SetParam	= vosrcSetParam2(MODULENAME) ;

	return VO_RET_SOURCE2_OK;
} 

#if defined __cplusplus
}
#endif
