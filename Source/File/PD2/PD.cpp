#include "PD.h"
#include "voLog.h"

#define LOG_TAG "voCOMXAudioSink"

#if defined __cplusplus
extern "C" {
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
	vo_PD_manager * ptr_PD = new vo_PD_manager();

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

	if( nTrack == ptr_PD->get_firstseektrack() )
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

#if defined __cplusplus
}
#endif