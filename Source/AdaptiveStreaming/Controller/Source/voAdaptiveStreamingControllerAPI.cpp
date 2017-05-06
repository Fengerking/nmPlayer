#include "voAdaptiveStreamingControllerAPI.h"
#include "voAdaptiveStreamingController.h"
#include "voAdaptiveStreamingClassFactory.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


/*API should make each interface valid when it be called*/
#define __CHECKOPENCOMPLEATE(b)  {if(!b) return VO_RET_SOURCE2_NEEDRETRY; }


VO_U32 AdaptiveStreamController_Init( VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam )
{
	voAdaptiveStreamingController * ptr_obj = 0;

	VO_U32 ret = CreateAdaptiveStreamingController( &ptr_obj );

	if( ret != VO_RET_SOURCE2_OK )
		return ret;

	*phHandle = ptr_obj; 

	return ptr_obj->Init( pSource , nFlag , pInitParam );
}

VO_U32 AdaptiveStreamController_Uninit( VO_HANDLE hHandle)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = ptr_obj->Uninit();

	ret = DestroyAdaptiveStreamingController( ptr_obj );

	return ret;
}

VO_U32 AdaptiveStreamController_Open(  VO_HANDLE hHandle )
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	VO_U32 ret = ptr_obj->Open();

	RecordCostTime( ct, false,"<--- Call Open Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_Close(  VO_HANDLE hHandle )
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	VO_U32 ret = ptr_obj->Close();

	RecordCostTime( ct, false,"<---Call Close Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_Start( VO_HANDLE hHandle)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret = ptr_obj->Start();

	RecordCostTime( ct, false,"Call Start Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_Pause( VO_HANDLE hHandle)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret = ptr_obj->Pause();
	
	RecordCostTime( ct, false,"Call Pause Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_Stop( VO_HANDLE hHandle)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

//	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());
	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	VO_U32 ret = ptr_obj->Stop();

	RecordCostTime( ct, false,"<---Call Stop Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_Seek( VO_HANDLE hHandle, VO_U64* pTimeStamp)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret =  ptr_obj->Seek( pTimeStamp );

	RecordCostTime( ct, false,"Call Seek Cost", 3);
	return ret;
}

VO_U32 AdaptiveStreamController_GetDuration( VO_HANDLE hHandle, VO_U64 * pDuration)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);


	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret =  ptr_obj->GetDuration( pDuration );

	RecordCostTime( ct, false,"Call GetDuration Cost", 3);
	return ret;
	
}

VO_U32 AdaptiveStreamController_GetSample(  VO_HANDLE hHandle , VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample )
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret = ptr_obj->GetSample( nTrackType , pSample );

	return ret;
}

VO_U32 AdaptiveStreamController_GetProgramCount( VO_HANDLE hHandle, VO_U32 *pProgramCount)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret = ptr_obj->GetProgramCount( pProgramCount );

	RecordCostTime( ct, false,"Call GetProgramCount Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_GetProgramInfo( VO_HANDLE hHandle, VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret = ptr_obj->GetProgramInfo( nProgram , pProgramInfo );

	RecordCostTime( ct, false,"Call GetProgramInfo Cost", 3);

	return ret;

}

VO_U32 AdaptiveStreamController_GetCurTrackInfo(  VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret = ptr_obj->GetCurTrackInfo( nTrackType , ppTrackInfo );

	RecordCostTime( ct, false,"Call GetCurTrackInfo Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_SelectProgram( VO_HANDLE hHandle, VO_U32 nProgram)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret = ptr_obj->SelectProgram( nProgram );

	RecordCostTime( ct, false,"Call SelectProgram Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_SelectStream( VO_HANDLE hHandle, VO_U32 nStream)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret = ptr_obj->SelectStream( nStream );

	RecordCostTime( ct, false,"Call SelectStream Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_SelectTrack( VO_HANDLE hHandle, VO_U32 nTrack)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret = ptr_obj->SelectTrack( nTrack );

	RecordCostTime( ct, false,"Call SelectTrack Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_GetDRMInfo( VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ct = 0;
	RecordCostTime( ct, true, NULL);

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	VO_U32 ret = ptr_obj->GetDRMInfo( ppDRMInfo );

	RecordCostTime( ct, false,"Call GetDRMInfo Cost", 3);

	return ret;
}

VO_U32 AdaptiveStreamController_SendBuffer( VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer )
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	__CHECKOPENCOMPLEATE(ptr_obj->CheckIsOpenComplete());

	return ptr_obj->SendBuffer( buffer );
}

VO_U32 AdaptiveStreamController_GetParam( VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	return ptr_obj->GetParam( nParamID , pParam );
}

VO_U32 AdaptiveStreamController_SetParam( VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	voAdaptiveStreamingController * ptr_obj = ( voAdaptiveStreamingController * )hHandle;

	if( !ptr_obj )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	return ptr_obj->SetParam( nParamID , pParam );
}
#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef _IOS

VO_S32 voGetSrcHLSAPI(VO_SOURCE2_API* pHandle)
{
	pHandle->Init = AdaptiveStreamController_Init;
	pHandle->Uninit = AdaptiveStreamController_Uninit;
	pHandle->Open = AdaptiveStreamController_Open;
	pHandle->Close = AdaptiveStreamController_Close;
	pHandle->Start = AdaptiveStreamController_Start;
	pHandle->Pause = AdaptiveStreamController_Pause;
	pHandle->Stop = AdaptiveStreamController_Stop;
	pHandle->Seek = AdaptiveStreamController_Seek;
	pHandle->GetDuration = AdaptiveStreamController_GetDuration;
	pHandle->GetSample = AdaptiveStreamController_GetSample;
	pHandle->GetProgramCount = AdaptiveStreamController_GetProgramCount;
	pHandle->GetProgramInfo = AdaptiveStreamController_GetProgramInfo;
	pHandle->GetCurTrackInfo = AdaptiveStreamController_GetCurTrackInfo;
	pHandle->SelectProgram = AdaptiveStreamController_SelectProgram;
	pHandle->SelectStream = AdaptiveStreamController_SelectStream;
	pHandle->SelectTrack = AdaptiveStreamController_SelectTrack;
	pHandle->GetDRMInfo = AdaptiveStreamController_GetDRMInfo;
	pHandle->SendBuffer = AdaptiveStreamController_SendBuffer;
	pHandle->GetParam = AdaptiveStreamController_GetParam;
	pHandle->SetParam = AdaptiveStreamController_SetParam;

	return 0;
}

VO_S32 voGetASCDASHAPI(VO_SOURCE2_API* pHandle)
{
	pHandle->Init = AdaptiveStreamController_Init;
	pHandle->Uninit = AdaptiveStreamController_Uninit;
	pHandle->Open = AdaptiveStreamController_Open;
	pHandle->Close = AdaptiveStreamController_Close;
	pHandle->Start = AdaptiveStreamController_Start;
	pHandle->Pause = AdaptiveStreamController_Pause;
	pHandle->Stop = AdaptiveStreamController_Stop;
	pHandle->Seek = AdaptiveStreamController_Seek;
	pHandle->GetDuration = AdaptiveStreamController_GetDuration;
	pHandle->GetSample = AdaptiveStreamController_GetSample;
	pHandle->GetProgramCount = AdaptiveStreamController_GetProgramCount;
	pHandle->GetProgramInfo = AdaptiveStreamController_GetProgramInfo;
	pHandle->GetCurTrackInfo = AdaptiveStreamController_GetCurTrackInfo;
	pHandle->SelectProgram = AdaptiveStreamController_SelectProgram;
	pHandle->SelectStream = AdaptiveStreamController_SelectStream;
	pHandle->SelectTrack = AdaptiveStreamController_SelectTrack;
	pHandle->GetDRMInfo = AdaptiveStreamController_GetDRMInfo;
	pHandle->SendBuffer = AdaptiveStreamController_SendBuffer;
	pHandle->GetParam = AdaptiveStreamController_GetParam;
	pHandle->SetParam = AdaptiveStreamController_SetParam;

	return 0;
}

#endif

VO_S32 VO_API voGetAdaptiveStreamControllerAPI( VO_SOURCE2_API* pHandle )
{
	pHandle->Init = AdaptiveStreamController_Init;
	pHandle->Uninit = AdaptiveStreamController_Uninit;
	pHandle->Open = AdaptiveStreamController_Open;
	pHandle->Close = AdaptiveStreamController_Close;
	pHandle->Start = AdaptiveStreamController_Start;
	pHandle->Pause = AdaptiveStreamController_Pause;
	pHandle->Stop = AdaptiveStreamController_Stop;
	pHandle->Seek = AdaptiveStreamController_Seek;
	pHandle->GetDuration = AdaptiveStreamController_GetDuration;
	pHandle->GetSample = AdaptiveStreamController_GetSample;
	pHandle->GetProgramCount = AdaptiveStreamController_GetProgramCount;
	pHandle->GetProgramInfo = AdaptiveStreamController_GetProgramInfo;
	pHandle->GetCurTrackInfo = AdaptiveStreamController_GetCurTrackInfo;
	pHandle->SelectProgram = AdaptiveStreamController_SelectProgram;
	pHandle->SelectStream = AdaptiveStreamController_SelectStream;
	pHandle->SelectTrack = AdaptiveStreamController_SelectTrack;
	pHandle->GetDRMInfo = AdaptiveStreamController_GetDRMInfo;
	pHandle->SendBuffer = AdaptiveStreamController_SendBuffer;
	pHandle->GetParam = AdaptiveStreamController_GetParam;
	pHandle->SetParam = AdaptiveStreamController_SetParam;

	return 0;
}
