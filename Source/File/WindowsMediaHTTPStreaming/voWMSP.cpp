#include "voWMSP.h"
#include "WMSPManager.h"
#include "voLog.h"
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
VO_U32 VO_API voWMSPOpen(VO_PTR * ppHandle, VO_SOURCE_OPENPARAM * pParam)
{
	CWMSPManager * pManager = new CWMSPManager();

	if( !pManager->Open(pParam) )
	{
		delete pManager;
		*ppHandle = 0;
		return VO_ERR_SOURCE_OPENFAIL;
	}

	*ppHandle = pManager;

	return VO_ERR_SOURCE_OK;
}

/**
* Close the opened source.
* \param pHandle [IN] The handle which was create by open function.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voWMSPClose(VO_PTR pHandle)
{
	CWMSPManager * pManager = (CWMSPManager*)pHandle;
	VO_U32 uiRet = pManager->Close();
	delete pManager;

	return uiRet;
}

/**
* Get the source information
* \param pHandle [IN] The handle which was create by open function.
* \param pSourceInfo [OUT] The structure of source info to filled.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voWMSPGetSourceInfo(VO_PTR pHandle, VO_SOURCE_INFO * pSourceInfo)
{
	CWMSPManager * pManager = (CWMSPManager*)pHandle;

	return pManager->GetSourceInfo(pSourceInfo);
}

/**
* Get the track information
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pTrackInfo [OUT] The track info to filled..
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voWMSPGetTrackInfo(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
{
	CWMSPManager * pManager = (CWMSPManager*)pHandle;

	return pManager->GetTrackInfo(nTrack, pTrackInfo);
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
VO_U32 VO_API voWMSPGetSample(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	CWMSPManager * pManager = (CWMSPManager*)pHandle;

	return pManager->GetSample(nTrack, pSample);
}

/**
* Set the track read position.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pPos [IN/OUT] The new pos will be set, and it will reset with previous key frame time if it is video track.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voWMSPSetPos(VO_PTR pHandle, VO_U32 nTrack, VO_S64 * pPos)
{
	CWMSPManager * pManager = (CWMSPManager*)pHandle;

	//if( -1 == ptr_PD->get_firstseektrack() )
	//{
	//	ptr_PD->set_firstseektrack( nTrack );
	//}

	//if( nTrack == ptr_PD->get_firstseektrack() )
	//{
	//	ptr_PD->moveto( *pPos );
	//}

	return pManager->SetPos(nTrack, pPos);
}

/**
* Set source param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voWMSPSetSourceParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{

	CWMSPManager * pManager = (CWMSPManager*)pHandle;

	return pManager->SetSourceParam(uID, pParam);
}

/**
* Get source param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param uID [IN] The param ID.
* \param pParam [Out] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voWMSPGetSourceParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CWMSPManager * pManager = (CWMSPManager*)pHandle;

	return pManager->GetSourceParam(uID, pParam);
}

/**
* Set track param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voWMSPSetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	CWMSPManager * pManager = (CWMSPManager*)pHandle;

	return pManager->SetTrackParam(nTrack , uID, pParam);
}

/**
* Get track param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voWMSPGetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	CWMSPManager * pManager = (CWMSPManager*)pHandle;

	return pManager->GetTrackParam(nTrack , uID, pParam);
}

VO_S32 VO_API voGetWMSPReadAPI (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag)
{
	if(pReadHandle)
	{
		pReadHandle->Close			=	voWMSPClose;
		pReadHandle->GetSample		=	voWMSPGetSample;
		pReadHandle->GetSourceInfo	=	voWMSPGetSourceInfo;
		pReadHandle->GetSourceParam	=	voWMSPGetSourceParam;
		pReadHandle->GetTrackInfo	=	voWMSPGetTrackInfo;
		pReadHandle->GetTrackParam	=	voWMSPGetTrackParam;
		pReadHandle->Open			=	voWMSPOpen;
		pReadHandle->SetPos			=	voWMSPSetPos;
		pReadHandle->SetSourceParam	=	voWMSPSetSourceParam;
		pReadHandle->SetTrackParam	=	voWMSPSetTrackParam;
	}

	return VO_ERR_SOURCE_OK;
}

#if defined __cplusplus
}
#endif