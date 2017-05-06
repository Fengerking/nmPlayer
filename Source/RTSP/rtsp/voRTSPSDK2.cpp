#include "voLog.h"
#include "utility.h"
#include "macro.h"
#include "voRTSP.h"
#include "voSource2ParserWrapper.h"
/**
Create the Session
/param [IN/OUT] ppRTSPSession: the pointer to a session handle
*/

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

static int instanceNum=0;
/**
* Close the opened source.
* \param pHandle [IN] The handle which was create by open function.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voRTSPClose2(VO_PTR pHandle)
{
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","voRTSPClose2\n");
	while (instanceNum==3)//disable another close
	{
		VOLOGI("voRTSPClose2--instanceNum!=1");
		//return 0;
		IVOThread::Sleep(10);
	}
	if (instanceNum==0)
	{
		VOLOGI("voRTSPClose2--instanceNum==0,the close has been finished by another thread,exit");
		return 0;
	}
	instanceNum=3;//begin close, set the signal as 3
	CVORTSPSource2* source = (CVORTSPSource2*)pHandle;
	SAFE_DELETE(source);
	trace_memLeak();
	instanceNum=0;
	VOLOGI("voRTSPClose2--END");
	return 0;
}
/**
* Open the source and return source handle
* \param ppHandle [OUT] Return the source operator handle
* \param pName	[IN] The source name
* \param pParam [IN] The source open param
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voRTSPOpen2(VO_PTR * ppHandle, VO_SOURCE_OPENPARAM * pParam)
{
	*ppHandle = NULL;
	if (instanceNum!=0)
	{
		return 0;
	}
	instanceNum = 2;//disable another open and close
	CVORTSPSource2* source=new CVORTSPSource2(pParam);TRACE_NEW("CVORTSPSource2",source);
	instanceNum=1;//good to close
	int ret=0;
	
	*ppHandle=source;
	
	return ret;
}

/**
* Get the source information
* \param pHandle [IN] The handle which was create by open function.
* \param pSourceInfo [OUT] The structure of source info to filled.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voRTSPGetSourceInfo2(VO_PTR pHandle, VO_SOURCE_INFO * pSourceInfo)
{
	CVORTSPSource2* source = (CVORTSPSource2*)pHandle;
	return source->GetSourceInfo(pSourceInfo);
}

/**
* Get the track information
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pTrackInfo [OUT] The track info to filled..
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voRTSPGetTrackInfo2(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
{
	CVORTSPSource2* source = (CVORTSPSource2*)pHandle;
	return source->GetTrackInfo(nTrack,pTrackInfo);
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
VO_U32 VO_API voRTSPGetSample2(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	CVORTSPSource2* source = (CVORTSPSource2*)pHandle;
	return source->GetSample(nTrack,pSample);
}

/**
* Set the track read position.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pPos [IN/OUT] The new pos will be set, and it will reset with previous key frame time if it is video track.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voRTSPSetPos2(VO_PTR pHandle, VO_U32 nTrack, VO_S64 * pPos)
{
	CVORTSPSource2* source = (CVORTSPSource2*)pHandle;
	VO_S32 pos=(VO_S32)*pPos;
	VO_U32 ret = source->SetPos(nTrack,&pos);
	if(ret==0)
		*pPos=pos;
	return ret;
}

/**
* Set source param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voRTSPSetSourceParam2(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CVORTSPSource2* source = (CVORTSPSource2*)pHandle;
	return source->SetSourceParam(uID,pParam);
}

/**
* Get source param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param uID [IN] The param ID.
* \param pParam [Out] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voRTSPGetSourceParam2(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CVORTSPSource2* source = (CVORTSPSource2*)pHandle;
	return source->GetSourceParam(uID,pParam);
}

/**
* Set track param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voRTSPSetTrackParam2(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	CVORTSPSource2* source = (CVORTSPSource2*)pHandle;
	return source->SetTrackParam(nTrack,uID,pParam);
}

/**
* Get track param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voRTSPGetTrackParam2(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	CVORTSPSource2* source = (CVORTSPSource2*)pHandle;
	return source->GetTrackParam(nTrack,uID,pParam);
}

VO_S32 VO_API voGetRTSPReadAPI (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag)
{
	if(pReadHandle)
	{
		pReadHandle->Close				 = voRTSPClose2;
		pReadHandle->GetSample		 =	voRTSPGetSample2;
		pReadHandle->GetSourceInfo =	voRTSPGetSourceInfo2;
		pReadHandle->GetSourceParam	=	voRTSPGetSourceParam2;
		pReadHandle->GetTrackInfo		=	voRTSPGetTrackInfo2;
		pReadHandle->GetTrackParam	=	voRTSPGetTrackParam2;
		pReadHandle->Open				 =		voRTSPOpen2;
		pReadHandle->SetPos				=		voRTSPSetPos2;
		pReadHandle->SetSourceParam	=	voRTSPSetSourceParam2;
		pReadHandle->SetTrackParam	=	voRTSPSetTrackParam2;

	}
	return 0;
}

/**
 * Initial a source session and Set the Source( url etc. ) into the module
 * For this function please only copy the pSource param, and cache the callback, do not do anything else
 * This function should be called first for a session.
 * \param phHandle [out] source handle.
 * \param pSource [in] source file description, should be an url or fd etc.
 * \param nFlag [in]. It can indicate the source type..
 * \param pCallback [in] notify call back function pointer.
 */
VO_U32 VO_API voRTSPInitSource2(VO_HANDLE * phHandle, VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam )
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	
	if(instanceNum!=0)
		return ret;

	instanceNum=2;
	voStream2ParserWrapper *source = new voStream2ParserWrapper();
	
	if(!source)
		return ret;

	//char *url = "rtsp://10.2.64.42/source/mp4/h264mpaac/14165743_MPEG4_AVC'M13_AAC'LC_397K'32s543ms'320x240'30f'2c'32KHz.mp4";
	ret = source->Init(pSource, nFlag, pInitParam);

	if(ret == VO_RET_SOURCE2_OK)
		*phHandle= (VO_HANDLE)source;

	instanceNum=1;

	return ret;
}

/**
 * Uninitial a source session
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 VO_API voRTSPUninitSource2(VO_HANDLE hHandle)
{
	if(!hHandle)
		return VO_RET_SOURCE2_FAIL;

	while (instanceNum==3)//disable another close
	{
		VOLOGI("voRTSPUninitSource2--instanceNum!=1");
		IVOThread::Sleep(10);
	}
	
	if (instanceNum==0)
	{
		VOLOGI("voRTSPUninitSource2--instanceNum==0,the close has been finished by another thread,exit");
		return 0;
	}
	instanceNum=3;//begin close, set the signal as 3
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;
	SAFE_DELETE(source);
	instanceNum=0;
	VOLOGI("voRTSPUninitSource2--END");
	return 0;
}

/**
 * Check and analyze the stream, get information
 * For this function, please do not start to parser the media data
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 VO_API voRTSPOpenSource2( VO_HANDLE hHandle )
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	return source->Open();
}

/**
 * Destroy the information of the stream
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 VO_API voRTSPCloseSource2( VO_HANDLE hHandle )
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	return source->Close();
}

/**
 * Start to parse the media data
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 VO_API voRTSPStartSource2(VO_HANDLE hHandle)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->Start();
}

/**
 * Pause
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 VO_API voRTSPPauseSource2(VO_HANDLE hHandle)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->Pause();
}

/**
 * Stop
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 VO_API voRTSPStopSource2(VO_HANDLE hHandle)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->Stop();
}

/**
 * Seek
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pTimeStamp [in/out] The seek pos and return the available pos after seek
 */
VO_U32 VO_API voRTSPSeekSource2(VO_HANDLE hHandle, VO_U64* pTimeStamp)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->Seek(pTimeStamp);
}

/**
 * Get Duration of the stream
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pDuration [out] The duration of the stream
 */
VO_U32 VO_API voRTSPGetDurationSource2(VO_PTR hHandle, VO_U64 * pDuration)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->GetDuration(pDuration);
}

/**
 * Get sample from the source
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nOutPutType [in] The sample type of the stream, it will be audio/video/closed caption
 * \param pSample [out] The sample of the stream, all buffer in the sample( include reserve field ) will be mantained by source, they will be available until you call GetSample again
 *                       the param type is depended on the nOutputType, for VO_SOURCE2_TT_VIDEO & VO_SOURCE2_TT_AUDIO please use VO_SOURCE2_SAMPLE
 *                                                                      for VO_SOURCE2_TT_SUBTITLE please use voSubtitleInfo
 */
VO_U32 VO_API voRTSPGetSampleSource2( VO_HANDLE hHandle , VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample )
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->GetSample(nTrackType,pSample);
}

/**
 * It retrieve how many streams in the live stream
 * For Stream, it should be defined as: all sub streams that share the same angle for video and same language for audio
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pProgramCount [out] The number of the streams
 */
VO_U32 VO_API voRTSPGetProgrammCountSource2(VO_HANDLE hHandle, VO_U32 *pStreamCount)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}
	
	return source->GetProgrammCount(pStreamCount);
}
/**
 * Enum all the stream info based on stream count
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nProgram  [in] The Stream sequence based on the stream counts
 * \param pProgramInfo [out] return the info of the Stream, source will keep the info structure available until you call close
 */
VO_U32 VO_API  voRTSPGetProgrammInfoSource2(VO_HANDLE hHandle, VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;
	
	if(!source)
		return VO_RET_SOURCE2_FAIL;
	if(source->GetLicState() == VO_FALSE)
	{
		VOLOGI("****************License check fail when GetProgrammInfo");
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->GetProgrammInfo(nProgram,pProgramInfo);
}

/**
 * Get the track info we selected
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nTrackType [in] The sample type of the stream, it will be audio/video/closed caption
 * \param pTrackInfo [out] The trackinfo of the selected track. The trackinfo memory will be maintained in source until you call close
 */
VO_U32 VO_API voRTSPGetCurTrackInfoSource2( VO_HANDLE hHandle, VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->GetCurTrackInfo(nTrackType,ppTrackInfo);
}
/**
 * Select the Program
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nProgram [in] Program ID
 */
VO_U32 VO_API voRTSPSelectProgramSource2(VO_HANDLE hHandle, VO_U32 nProgram)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->SelectProgram(nProgram);
}
/**
 * Select the Stream
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nStream [in] Stream ID
 */
VO_U32 VO_API voRTSPSelectStreamSource2(VO_HANDLE hHandle, VO_U32 nStream)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->SelectStream(nStream);
}
/**
 * Select the Track
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nTrack [in] Track ID
 */
VO_U32 VO_API  voRTSPSelectTrackSource2(VO_HANDLE hHandle, VO_U32 nTrack)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->SelectTrack(nTrack);
}
/**
 * Get the DRM info
 * \param hHandle [in] The source handle. Opened by Init().
 * \param VO_SOURCE2_DRM_INFO [out] return the drm info, source will keep the info structure available until you call close
 */
VO_U32 VO_API  voRTSPGetDRMInfoSource2(VO_HANDLE hHandle, VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}
	return VO_RET_SOURCE2_FAIL;
}
/**
* Send the buffer into the source, if you want to use this function, you should use flag VO_SOURCE2_FLAG_OPEN_SENDBUFFER
* \param hHandle [in] The source handle. Opened by Init().
* \param buffer [in] The buffer send to the source
*/
VO_U32 VO_API voRTSPSendBufferSource2(VO_HANDLE hHandle, const VO_SOURCE2_SAMPLE& buffer )
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return VO_RET_SOURCE2_FAIL;
}

/**
 * Get the special value from param ID
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \
 */
VO_U32 VO_API voRTSPGetParamSource2(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;
	if(!source)
		return VO_RET_SOURCE2_FAIL;

	if(source->GetLicState() == VO_FALSE)
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->GetParam(nParamID,pParam);
}

/**
 * Set the special value from param ID
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
VO_U32 VO_API voRTSPSetParamSource2(VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)hHandle;

	if(!source)
		return VO_RET_SOURCE2_FAIL;
	
	if(source->IsAddLicCheck() && (source->GetLicState() == VO_FALSE))
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	return source->SetParam(nParamID,pParam);
}

VO_S32 VO_API voGetRTSP2ReadAPI (VO_SOURCE2_API * pReadHandle, VO_U32 uFlag)
{
	if(pReadHandle)
	{
		pReadHandle->Init				=			voRTSPInitSource2;
		pReadHandle->Uninit 			=			voRTSPUninitSource2;
		pReadHandle->Open				=			voRTSPOpenSource2;
		pReadHandle->Close				=			voRTSPCloseSource2;
		pReadHandle->Start				=			voRTSPStartSource2;
		pReadHandle->Pause				=			voRTSPPauseSource2;
		pReadHandle->Stop				=			voRTSPStopSource2;
		pReadHandle->Seek				=			voRTSPSeekSource2;
		pReadHandle->GetDuration		=			voRTSPGetDurationSource2;
		pReadHandle->GetSample			=			voRTSPGetSampleSource2;
		pReadHandle->GetProgramCount	=			voRTSPGetProgrammCountSource2;
		pReadHandle->GetProgramInfo		=			voRTSPGetProgrammInfoSource2;
		pReadHandle->GetCurTrackInfo	=			voRTSPGetCurTrackInfoSource2;
		pReadHandle->SelectProgram		=			voRTSPSelectProgramSource2;
		pReadHandle->SelectStream		=			voRTSPSelectStreamSource2;
		pReadHandle->SelectTrack		=			voRTSPSelectTrackSource2;
		pReadHandle->GetDRMInfo			=			voRTSPGetDRMInfoSource2;
		pReadHandle->SendBuffer			=			voRTSPSendBufferSource2;
		pReadHandle->GetParam			=			voRTSPGetParamSource2;
		pReadHandle->SetParam			=			voRTSPSetParamSource2;
	}
	return VO_RET_SOURCE2_OK;
}



