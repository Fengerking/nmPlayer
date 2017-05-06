#include "vomtpAPI.h"
#include "CMTPSDK.h"

#define _CHECK_HANDLE_ if(!hMTP) return VO_ERR_MTP_NULLPOINTER;

/**
* Initialize a Mobile TV Player session.
* This function should be called first for a session.
* \param phMTP [out] MTP handle.
* \param pOpenParam [in] MTP open parameters
*/
VO_U32 vomtpOpen(VO_HANDLE* phMTP, VO_MTP_OPEN_PARAM* pOpenParam)
{
	CMTPSDK* pSdk	= new CMTPSDK;

	if(!pSdk)
		return VO_ERR_MTP_OUTOFMEMORY;

	*phMTP = pSdk;
	return pSdk->Open(pOpenParam);
}

/**
* Close the MTP session.
* This function should be called last of the session.
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
*/
VO_U32 vomtpClose(VO_HANDLE hMTP)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->Close();
}


/**
* Set data source type
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param pSource [in] The source. Normally it should be URL for HTTP Live Streaming.
* \param nSourceFormat [in]. It can indicate the source type, refer to VO_MTP_SOURCE_FORMAT.
*/
VO_U32 vomtpSetDataSource(VO_HANDLE hMTP, VO_PTR pSource, VO_U32 nSourceFormat)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->SetDataSource(pSource, nSourceFormat);
}

/**
* Set the video draw area in the view window
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param nLeft [in] The left position of draw video area
* \param nTop [in] The top position of draw video area
* \param nRight [in] The right position of draw video area
* \param nBottom [in] The bottom position of draw video area
*/
VO_U32 vomtpSetDrawArea(VO_HANDLE hMTP, VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->SetDrawArea(nLeft, nTop, nRight, nBottom);
}


/**
* Scan the channels
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param pParam [in] It was structure depend on the live source type... 
*/
VO_U32 vomtpScan(VO_HANDLE hMTP, VO_MTP_SCAN_PARAMEPTERS* pParam)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->Scan(pParam);
}


/**
* Cancel the current task
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param nID [in] The task ID. It only support scan task now..
*/
VO_U32 vomtpCancel(VO_HANDLE hMTP, VO_U32 nID)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->Cancel(nID);
}


/**
* Data source seek, Normally it should be used by HTTP Live Streaming
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param nPosition [in]. timestamp to seek to.
*/
VO_U32 vomtpSeek(VO_HANDLE hMTP, VO_U64 nPosition)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->Seek(nPosition);
}


/**
* Get current playing position, Normally it should be used by HTTP Live Streaming
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param pPosition [in]. current playing timestamp.
*/
VO_U32 vomtpGetPosition(VO_HANDLE hMTP, VO_U64* pPosition)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->GetPosition(pPosition);
}


/**
* start to play
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
*/
VO_U32 vomtpStart(VO_HANDLE hMTP)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->Start();
}


/**
* stop playing
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
*/
VO_U32 vomtpStop(VO_HANDLE hMTP)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->Stop();
}


/**
* Get Channel information
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param pCount [out] The channel count
* \param ppChannelInfo [out] The channel info pointer
*/
VO_U32 vomtpGetChannel(VO_HANDLE hMTP, VO_U32* pCount, VO_MTP_CHANNELINFO** ppChannelInfo)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->GetChannel(pCount, ppChannelInfo);
}


/**
* select the channel
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param nChannelID [in] The channel ID
*/
VO_U32 vomtpSetChannel(VO_HANDLE hMTP, VO_S32 nChannelID)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->SetChannel(nChannelID);
}


/**
* Get ESG information
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param nChannelID [in] The channel ID
* \param pESGInfo [out] The ESG info pointer
*/
VO_U32 vomtpGetESG(VO_HANDLE hMTP, VO_S32 nChannelID, VO_MTP_ESG_INFO* pESGInfo)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->GetESG(nChannelID, pESGInfo);
}


/**
* Suspend or resume the audio playback
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param bSuspend [in] Audio control flag.
*/
VO_U32 vomtpAudioControl(VO_HANDLE hMTP, VO_BOOL bSuspend)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->AudioControl(bSuspend);
}


/**
* Get audio volume.
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param pVolume [out] Current volume. The volume should be 0 - 100
* \param pMaxVolume [out] Maximal volume. Set the pointer to VO_NULL if you do not care it.
*/
VO_U32 vomtpGetVolume(VO_HANDLE hMTP, VO_U16* pVolume, VO_U16* pMaxVolume)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->GetVolume(pVolume, pMaxVolume);
}


/**
* Set audio volume.
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param nVolume [in] The expected volume.
*/
VO_U32 vomtpSetVolume(VO_HANDLE hMTP, VO_U16 nVolume)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->SetVolume(nVolume);
}


/**
* Start to record the data with MP4 file format
* \param hMTP [in] The MTP engine handle. Opened by voMTPOpen().
* \param pRecFile [in] The MP4 file name. If it is VO_NULL, it will use default directory and name..
*/
VO_U32 vomtpRecStart(VO_HANDLE hMTP, TCHAR* pRecFile)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->RecStart(pRecFile);
}


/**
* Stop recording file
* \param hMTP [in] The MTP engine handle. Opened by voMTPOpen().
*/
VO_U32 vomtpRecStop(VO_HANDLE hMTP)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->RecStop();
}


/**
* Get the special value from param ID
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param nParamID [in] The param ID
* \param pParam [out] The return value depend on the param ID.
*/
VO_U32 vomtpGetParam(VO_HANDLE hMTP, VO_U32 nParamID, VO_PTR pParam)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->GetParam(nParamID, pParam);
}


/**
* Set the special value from param ID
* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
* \param nParamID [in] The param ID
* \param pParam [in] The set value depend on the param ID.
*/
VO_U32 vomtpSetParam(VO_HANDLE hMTP, VO_U32 nParamID, VO_PTR pParam)
{
	_CHECK_HANDLE_
	return ((CMTPSDK*)hMTP)->SetParam(nParamID, pParam);
}


VO_S32 VO_API voGetmtpAPI(VO_MTP_API* pHandle)
{
	pHandle->vomtpOpen			= vomtpOpen;
	pHandle->vomtpClose			= vomtpClose;
	pHandle->vomtpSetDataSource	= vomtpSetDataSource;
	pHandle->vomtpSetDrawArea	= vomtpSetDrawArea;
	pHandle->vomtpScan			= vomtpScan;
	pHandle->vomtpCancel		= vomtpCancel;
	pHandle->vomtpSeek			= vomtpSeek;
	pHandle->vomtpGetPosition	= vomtpGetPosition;
	pHandle->vomtpStart			= vomtpStart;
	pHandle->vomtpStop			= vomtpStop;
	pHandle->vomtpGetChannel	= vomtpGetChannel;
	pHandle->vomtpSetChannel	= vomtpSetChannel;
	pHandle->vomtpGetESG		= vomtpGetESG;
	pHandle->vomtpAudioControl	= vomtpAudioControl;
	pHandle->vomtpGetVolume		= vomtpGetVolume;
	pHandle->vomtpSetVolume		= vomtpSetVolume;
	pHandle->vomtpRecStart		= vomtpRecStart;
	pHandle->vomtpRecStop		= vomtpRecStop;
	pHandle->vomtpGetParam		= vomtpGetParam;
	pHandle->vomtpSetParam		= vomtpSetParam;

	return 0;
}
