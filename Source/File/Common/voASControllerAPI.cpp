

#include "voSource2.h"
#include "CASController.h"
#include "voASControllerAPI.h"
#include "memoryinfo.h"
#include "voLog.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#if defined __cplusplus
	extern "C" {
#endif
#endif	// _VONAMESPACE

#ifdef _IOS
#if defined _ISS_SOURCE_
using namespace _ISS;
#define StreamInit					ISSStreamInit
#define StreamUninit                ISSStreamUninit
#define StreamOpen                  ISSStreamOpen
#define StreamClose                 ISSStreamClose
#define StreamStart                 ISSStreamStart
#define StreamPause                 ISSStreamPause
#define StreamStop                  ISSStreamStop
#define StreamSeek                  ISSStreamSeek
#define StreamGetDuration           ISSStreamGetDuration
#define StreamGetSample             ISSStreamGetSample
#define StreamGetProgramCount		ISSStreamGetProgramCount
#define StreamGetProgramInfo        ISSStreamGetProgramInfo
#define StreamGetCurTrackInfo       ISSStreamGetCurTrackInfo
#define StreamSelectProgram         ISSStreamSelectProgram
#define StreamSelectStream          ISSStreamSelectStream
#define StreamSelectTrack           ISSStreamSelectTrack
#define StreamSendBuffer            ISSStreamSendBuffer
#define StreamGetDRMInfo            ISSStreamGetDRMInf
#define StreamGetParam              ISSStreamGetParam
#define StreamSetParam              ISSStreamSetParam

#elif defined _DASH_SOURCE_
using namespace _DASH;
#define StreamInit					DashStreamInit
#define StreamUninit                DashStreamUninit
#define StreamOpen                  DashStreamOpen
#define StreamClose                 DashStreamClose
#define StreamStart                 DashStreamStart
#define StreamPause                 DashStreamPause
#define StreamStop                  DashStreamStop
#define StreamSeek                  DashStreamSeek
#define StreamGetDuration           DashStreamGetDuration
#define StreamGetSample             DashStreamGetSample
#define StreamGetProgramCount		DashStreamGetProgramCount
#define StreamGetProgramInfo        DashStreamGetProgramInfo
#define StreamGetCurTrackInfo       DashStreamGetCurTrackInfo
#define StreamSelectProgram         DashStreamSelectProgram
#define StreamSelectStream          DashStreamSelectStream
#define StreamSelectTrack           DashStreamSelectTrack
#define StreamSendBuffer            DashStreamSendBuffer
#define StreamGetDRMInfo            DashStreamGetDRMInf
#define StreamGetParam              DashStreamGetParam
#define StreamSetParam              DashStreamSetParam
#endif
#endif	// end of _IOS

VO_U32 StreamInit( VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam )
{

	CASController * ptr_obj = new CASController;
	*phHandle = ptr_obj;

	return ptr_obj->Init( pSource , nFlag , pInitParam );

}

VO_U32 StreamUninit( VO_HANDLE hHandle )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	ptr_obj->Uninit();
	delete ptr_obj;
#ifdef _LINUX
	memStatus("/data/local/dump/after.dat");
#endif
	return VO_RET_SOURCE2_OK;
}

VO_U32 StreamOpen( VO_HANDLE hHandle )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->Open();
}

VO_U32 StreamClose( VO_HANDLE hHandle )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->Close();
}

VO_U32 StreamStart( VO_HANDLE hHandle )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->Start();
}

VO_U32 StreamPause( VO_HANDLE hHandle )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->Pause();
}

VO_U32 StreamStop( VO_HANDLE hHandle )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->Stop();
}

VO_U32 StreamSeek( VO_HANDLE hHandle, VO_U64* pTimeStamp )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->Seek( pTimeStamp );
}

VO_U32 StreamGetDuration( VO_PTR hHandle, VO_U64 * pDuration )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->GetDuration( pDuration );
}

VO_U32 StreamGetSample( VO_HANDLE hHandle , VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->GetSample( (VO_U32)nOutPutType , pSample );
}


VO_U32 StreamGetProgramCount(VO_HANDLE hHandle, VO_U32 *pProgramCount)
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->GetProgramCount( pProgramCount );
}

VO_U32 StreamGetProgramInfo(VO_HANDLE hHandle, VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->GetProgramInfo( nProgram , pProgramInfo );
}

VO_U32 StreamGetCurTrackInfo( VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->GetCurTrackInfo( nTrackType , ppTrackInfo );
}

VO_U32 StreamSelectProgram(VO_HANDLE hHandle, VO_U32 nProgram)
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->SelectProgram( nProgram );
}

VO_U32 StreamSelectStream(VO_HANDLE hHandle, VO_U32 nStream)
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->SelectStream( nStream );
}

VO_U32 StreamSelectTrack(VO_HANDLE hHandle, VO_U32 nTrack)
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->SelectTrack( nTrack );
}



VO_U32 StreamGetDRMInfo( VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->GetDRMInfo( ppDRMInfo );
}

VO_U32 StreamGetParam( VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->GetParam( nParamID , pParam );
}

VO_U32 StreamSetParam( VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam )
{
	if(nParamID == VO_PID_COMMON_LOGFUNC)
	{
		VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
		vologInit (pVologCB->pUserData, pVologCB->fCallBack);
	}
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return ptr_obj->SetParam( nParamID , pParam );
}

VO_U32 StreamSendBuffer(VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer )
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;
	CASController * ptr_obj = ( CASController * )hHandle;
	return VO_RET_SOURCE2_NOIMPLEMENT;
}

#ifdef _VONAMESPACE
}
#else
#if defined __cplusplus
  }
#endif
#endif	// _VONAMESPACE


#ifdef _VONAMESPACE
  using namespace _VONAMESPACE;
#endif
#if defined __cplusplus
  extern "C" {
#endif
#if defined _ISS_SOURCE_
VO_U32 voGetASCISSAPI( VO_SOURCE2_API * pAPI )
#elif defined _DASH_SOURCE_
VO_U32 voGetASCDASHAPI( VO_SOURCE2_API * pAPI )
#endif
{
	if(!pAPI )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	pAPI->Init = StreamInit;
	pAPI->Uninit = StreamUninit;
	pAPI->Open = StreamOpen;
	pAPI->Close = StreamClose;
	pAPI->Start = StreamStart;
	pAPI->Pause = StreamPause;
	pAPI->Stop = StreamStop;
	pAPI->Seek = StreamSeek;
	pAPI->GetDuration = StreamGetDuration;
	pAPI->GetSample = StreamGetSample;


	pAPI->GetProgramCount = StreamGetProgramCount;
	pAPI->GetProgramInfo = StreamGetProgramInfo;
	pAPI->GetCurTrackInfo = StreamGetCurTrackInfo;
	pAPI->SelectProgram = StreamSelectProgram;
	pAPI->SelectStream = StreamSelectStream;
	pAPI->SelectTrack = StreamSelectTrack;


	pAPI->SendBuffer = StreamSendBuffer;
	pAPI->GetDRMInfo = StreamGetDRMInfo;
	pAPI->GetParam = StreamGetParam;
	pAPI->SetParam = StreamSetParam;

	return VO_RET_SOURCE2_OK;
}
#if defined __cplusplus
  }
#endif

