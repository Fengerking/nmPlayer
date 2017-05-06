#include "PD.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif



/**
* Open the source and return source handle
* \param ppHandle [OUT] Return the source operator handle
* \param pName	[IN] The source name
* \param pParam [IN] The source open param
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDOpen(VO_PTR * ppHandle, VO_SOURCE_OPENPARAM * pParam)
{
	VOLOGI( "VisualOn Progressive Downloader Version: 3.0.1.9"  );
	vo_PD_manager * ptr_PD = new vo_PD_manager();
	if( !ptr_PD )
	{
		VOLOGE( "new obj failed" );
		*ppHandle = 0;
		return VO_ERR_SOURCE_OPENFAIL;
	}

	if( !ptr_PD->open( pParam ) )
	{
		delete ptr_PD;
		*ppHandle = 0;
		return VO_ERR_SOURCE_OPENFAIL;
	}

	*ppHandle = ptr_PD;

	return 0;
}

/**
* Close the opened source.
* \param pHandle [IN] The handle which was create by open function.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDClose(VO_PTR pHandle)
{
	vo_PD_manager * ptr_PD = (vo_PD_manager*)pHandle;
	VO_U32 ret = ptr_PD->close();
	delete ptr_PD;
	return ret;
}

/**
* Get the source information
* \param pHandle [IN] The handle which was create by open function.
* \param pSourceInfo [OUT] The structure of source info to filled.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetSourceInfo(VO_PTR pHandle, VO_SOURCE_INFO * pSourceInfo)
{
	vo_PD_manager * ptr_PD = (vo_PD_manager*)pHandle;
	return ptr_PD->get_sourceinfo( pSourceInfo );
}

/**
* Get the track information
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pTrackInfo [OUT] The track info to filled..
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetTrackInfo(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
{
	vo_PD_manager * ptr_PD = (vo_PD_manager*)pHandle;
	return ptr_PD->get_trackinfo( nTrack , pTrackInfo );
}

/**
* Get the track buffer
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pSample [OUT] The sample info was filled.
*		  Audio. It will fill the next frame audio buffer automatically.
*		  Video  It will fill the frame data depend on the sample time. if the next key frmae time
*				 was less than the time, it will fill the next key frame data, other, it will fill
*				 the next frame data.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetSample(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	vo_PD_manager * ptr_PD = (vo_PD_manager*)pHandle;
	return ptr_PD->get_sample( nTrack , pSample );
}

/**
* Set the track read position.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pPos [IN/OUT] The new pos will be set, and it will reset with previous key frame time if it is video track.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDSetPos(VO_PTR pHandle, VO_U32 nTrack, VO_S64 * pPos)
{
	vo_PD_manager * ptr_PD = (vo_PD_manager*)pHandle;

	if( -1 == ptr_PD->get_firstseektrack() )
	{
		ptr_PD->set_firstseektrack( nTrack );
	}

	if( nTrack == (VO_U32)ptr_PD->get_firstseektrack() )
	{
		ptr_PD->moveto( *pPos );
	}

	return ptr_PD->set_pos( nTrack , pPos );
}

/**
* Set source param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDSetSourceParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{

	vo_PD_manager * ptr_PD = (vo_PD_manager*)pHandle;
	return ptr_PD->set_sourceparam( uID , pParam );
}

/**
* Get source param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param uID [IN] The param ID.
* \param pParam [Out] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetSourceParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	vo_PD_manager * ptr_PD = (vo_PD_manager*)pHandle;
	return ptr_PD->get_sourceparam( uID , pParam );
}

/**
* Set track param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDSetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	vo_PD_manager * ptr_PD = (vo_PD_manager*)pHandle;
	return ptr_PD->set_trackparam( nTrack , uID , pParam );
}

/**
* Get track param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	vo_PD_manager * ptr_PD = (vo_PD_manager*)pHandle;
	return ptr_PD->get_trackparam( nTrack , uID , pParam );
}

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

VO_S32 VO_API voGetPDReadAPI (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag)
{
	if(pReadHandle)
	{
		pReadHandle->Close			=	voPDClose;
		pReadHandle->GetSample		=	voPDGetSample;
		pReadHandle->GetSourceInfo	=	voPDGetSourceInfo;
		pReadHandle->GetSourceParam	=	voPDGetSourceParam;
		pReadHandle->GetTrackInfo	=	voPDGetTrackInfo;
		pReadHandle->GetTrackParam	=	voPDGetTrackParam;
		pReadHandle->Open			=	voPDOpen;
		pReadHandle->SetPos			=	voPDSetPos;
		pReadHandle->SetSourceParam	=	voPDSetSourceParam;
		pReadHandle->SetTrackParam	=	voPDSetTrackParam;

	}
	return 0;
}


#ifdef _PD_SOURCE2

#include "voSource2.h"
#include "voSource2PDWrapper.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif


	VO_U32 Source2_PD_Init(VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pParam )
	{
		VOLOGI( "[trunk+]VisualOn Progressive Downloader Version: 3.0.1.9"  );
		voSource2PDWrapper * ptr_obj = new voSource2PDWrapper;
		if( !ptr_obj )
		{
			VOLOGE( "new obj failed" );
			*phHandle = 0;
			return VO_RET_SOURCE2_FAIL;
		}
		*phHandle = ptr_obj;
		return ptr_obj->Init( pSource , nFlag , pParam );

	}

	VO_U32 Source2_PD_Uninit(VO_HANDLE hHandle)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		if( ptr_obj )
		{
			VO_U32 ret = ptr_obj->Uninit();
			delete ptr_obj;
			ptr_obj = NULL;
			return ret;
		}
		else
		{
			return VO_RET_SOURCE2_OK;
		}
	}

	VO_U32 Source2_PD_Open( VO_HANDLE hHandle )
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->Open();
	}

	VO_U32 Source2_PD_Close( VO_HANDLE hHandle )
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->Close();
	}

	VO_U32 Source2_PD_Start(VO_HANDLE hHandle)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->Start();
	}

	VO_U32 Source2_PD_Pause(VO_HANDLE hHandle)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->Pause();
	}

	VO_U32 Source2_PD_Stop(VO_HANDLE hHandle)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->Stop();
	}

	VO_U32 Source2_PD_Seek(VO_HANDLE hHandle, VO_U64* pTimeStamp)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->Seek( pTimeStamp );
	}

	VO_U32 Source2_PD_GetDuration(VO_PTR hHandle, VO_U64 * pDuration)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->GetDuration( pDuration );
	}

	VO_U32 Source2_PD_GetSample( VO_HANDLE hHandle , VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample )
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->GetSample( nOutPutType , pSample );
	}

	VO_U32 Source2_PD_GetProgramCount(VO_HANDLE hHandle, VO_U32 *pProgramCount)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->GetProgramCount( pProgramCount );
	}

	VO_U32 Source2_PD_GetProgramInfo(VO_HANDLE hHandle, VO_U32 nStream, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->GetProgramInfo( nStream , ppProgramInfo );
	}

	VO_U32 Source2_PD_GetCurTrackInfo( VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nOutPutType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->GetCurTrackInfo( nOutPutType , ppTrackInfo );
	}

	VO_U32 Source2_PD_SelectProgram(VO_HANDLE hHandle, VO_U32 nProgram)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->SelectProgram( nProgram );
	}

	VO_U32 Source2_PD_SelectStream(VO_HANDLE hHandle, VO_U32 nStream)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->SelectStream( nStream );
	}

	VO_U32 Source2_PD_SelectTrack(VO_HANDLE hHandle, VO_U32 nTrack)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->SelectTrack( nTrack );
	}

	VO_U32 Source2_PD_GetDRMInfo(VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->GetDRMInfo( ppDRMInfo );
	}

	VO_U32 Source2_PD_SendBuffer(VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer )
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->SendBuffer( buffer );
	}

	VO_U32 Source2_PD_GetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
	{
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->GetParam( nParamID , pParam );
	}

	VO_U32 Source2_PD_SetParam(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
	{
		if (nParamID == VO_PID_COMMON_LOGFUNC)
		{
			//vologInit (pVologCB->pUserData, pVologCB->fCallBack);
		}
		if(!hHandle)
			return VO_ERR_BASE;
		voSource2PDWrapper * ptr_obj = (voSource2PDWrapper *)hHandle;
		return ptr_obj->SetParam( nParamID , pParam );
	}

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
	VO_EXPORT_FUNC VO_S32 VO_API voGetPD2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag)
	{
		pReadHandle->Init = Source2_PD_Init;
		pReadHandle->Uninit = Source2_PD_Uninit;
		pReadHandle->Open = Source2_PD_Open;
		pReadHandle->Close = Source2_PD_Close;
		pReadHandle->Start = Source2_PD_Start;
		pReadHandle->Pause = Source2_PD_Pause;
		pReadHandle->Stop = Source2_PD_Stop;
		pReadHandle->Seek = Source2_PD_Seek;
		pReadHandle->GetDuration = Source2_PD_GetDuration;
		pReadHandle->GetSample = Source2_PD_GetSample;
		pReadHandle->GetProgramCount = Source2_PD_GetProgramCount;
		pReadHandle->GetProgramInfo = Source2_PD_GetProgramInfo;
		pReadHandle->GetCurTrackInfo = Source2_PD_GetCurTrackInfo;
		pReadHandle->SelectProgram = Source2_PD_SelectProgram;
		pReadHandle->SelectStream = Source2_PD_SelectStream;
		pReadHandle->SelectTrack = Source2_PD_SelectTrack;
		pReadHandle->GetDRMInfo = Source2_PD_GetDRMInfo;
		pReadHandle->SendBuffer = Source2_PD_SendBuffer;
		pReadHandle->GetParam = Source2_PD_GetParam;
		pReadHandle->SetParam = Source2_PD_SetParam;
		return VO_ERR_SOURCE_OK;
	}

	

#endif //_SOURCE2
