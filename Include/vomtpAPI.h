/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/


#ifndef __VO_MTP_API_H_
#define __VO_MTP_API_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voString.h"
#include "voType.h"
#include "voIndex.h"
#include "voIVCommon.h"
#include "voVideo.h"
#include "voAudio.h"



/**
* Error code
*/
#define VO_ERR_MTP_OK				VO_ERR_NONE
#define VO_ERR_MTP_BASE				0x88000000
#define VO_ERR_MTP_OUTOFMEMORY		(VO_ERR_MTP_BASE | 0x0001)		/*!< out of memory */
#define VO_ERR_MTP_NULLPOINTER		(VO_ERR_MTP_BASE | 0x0002)		/*!< null pointer */
#define VO_ERR_MTP_INVALIDARG		(VO_ERR_MTP_BASE | 0x0003)		/*!< invalid argument */
#define VO_ERR_MTP_SRC_NOT_READY	(VO_ERR_MTP_BASE | 0x0004)		/*!< source not open */
#define VO_ERR_MTP_NO_CHANNEL		(VO_ERR_MTP_BASE | 0x0005)		/*!< source has no channel found */
#define VO_ERR_MTP_NO_DATA			(VO_ERR_MTP_BASE | 0x0006)		/*!< source has no data output*/
#define VO_ERR_MTP_TIMEOUT			(VO_ERR_MTP_BASE | 0x0007)		/*!< timeout */
#define VO_ERR_MTP_SEEK_FAILED		(VO_ERR_MTP_BASE | 0x0008)		/*!< seek failed */
#define VO_ERR_MTP_NOT_IMPLEMENT	(VO_ERR_MTP_BASE | 0x0009)		/*!< feature not implemented */
#define VO_ERR_MTP_NO_ESG			(VO_ERR_MTP_BASE | 0x000A)		/*!< no ESG info */
#define VO_ERR_MTP_PARAM_ID_ERR		(VO_ERR_MTP_BASE | 0x000B)		/*!< invalid param ID */
#define VO_ERR_MTP_FAIL				(VO_ERR_MTP_BASE | 0x000C)      /*!< general error */
#define VO_ERR_MTP_LOAD_DRV_FAIL	(VO_ERR_MTP_BASE | 0x000E)      /*!< load source drive failed */


/**
* Parameter ID, used by vomtpSetParam,vomtpGetParam
*/
#define	VO_MTP_PID_BASE				0x41000000						/*!< The base param ID for Mobile TV Player */
#define	VO_MTP_PID_COLOR_TYPE		(VO_MTP_PID_BASE | 0X0001)		/*!< The color type of video, refer to VO_IV_COLORTYPE */
#define	VO_MTP_PID_AUDIO_SINK_BUFFTIME	(VO_MTP_PID_BASE | 0X0002)	/*!< Set audio sink buffer time int * ms */

/**
* Status update code, used by VOMTPSTATUS callback function
*/
#define VO_MTP_STATUS_BASE					0x51000000						/*!< the base status code for source modules */
#define	VO_MTP_STATUS_SCAN_PROCESS			(VO_MTP_STATUS_BASE | 0x0001)	/*!< nParam1 is scanned frequence percent, 0 - 100 */
#define VO_MTP_STATUS_DEVICE_ERROR			(VO_MTP_STATUS_BASE | 0x0002)	/*!< device error */
#define VO_MTP_STATUS_NO_SIGNAL				(VO_MTP_STATUS_BASE | 0x0003)	/*!< no signal */ 


/**
* Task code, used by vomtpCancel()
*/
#define VO_MTP_TASK_BASE		0x52000000
#define	VO_MTP_TASK_SCAN		(VO_MTP_TASK_BASE | 0x0001)	 /*!< the scan task ID */


/**
* MTP data output type,used by VOMTPSENDDATA callback function
*/
typedef enum
{
	VO_MTP_OUTPUT_AUDIO		=	0,	/*!<  Output audio, see MTP_AUDIO_BUFFER */
	VO_MTP_OUTPUT_VIDEO		=	1,	/*!<  Output video, see MTP_VIDEO_BUFFER */
	VO_MTP_OUTPUT_DATABLOCK	=	2,	/*!<  Output data service,just like stock info,traffic info..., see VO_MTP_DATA_SERVICE */
}VO_MTP_OUTPUT_TYPE;

typedef struct
{
	VO_VIDEO_BUFFER		buf;
	VO_VIDEO_FORMAT*	fmt;
}MTP_VIDEO_BUFFER;

typedef struct
{
	VO_BYTE*	 		pBuffer;			/*!< Buffer pointer */
	VO_U32				nSize;				/*!< Buffer total size */
	VO_U64				llTime;				/*!< The time of the buffer */

	VO_AUDIO_FORMAT*	fmt;
}MTP_AUDIO_BUFFER;


/**
* Data service,used by VOMTPSENDDATA callback function
*
*/
typedef struct  
{
	VO_PTR pData;						/*!< pointer to raw data,just like stock info,traffic info... */
	VO_U32 nDataLen;					/*!< raw data length */
	VO_U32 nReserved;					/*!< reserved */
}VO_MTP_DATA_SERVICE;


/**
* MTP source format
*/
typedef enum
{
	VO_MTP_FT_FILE		= 0x00000000,	/*!< Unknown format */
	VO_MTP_FT_TDMB		= 0x00000001,	/*!< T-DMB */
	VO_MTP_FT_DAB		= 0x00000002,	/*!< DAB */
	VO_MTP_FT_DVBH		= 0x00000003,	/*!< DVB-H stream */
	VO_MTP_FT_ISDBT		= 0x00000004,	/*!< ISDB-T stream */
	VO_MTP_FT_DVBT		= 0x00000005,	/*!< DVB-T stream */
	VO_MTP_FT_DMBT		= 0x00000006,	/*!< DMB-T stream */
	VO_MTP_FT_DVBHTS	= 0x00000007,	/*!< DVB-H stream with TS payload */
	VO_MTP_FT_ATSCMH	= 0x00000008,	/*!< ATSC-MH stream */
	VO_MTP_FT_CMMB		= 0x00000009,	/*!< CMMB stream */
	VO_MTP_FT_HTTP_LS	= 0x0000000A,	/*!< HTTP Live Streaming */
}VO_MTP_SOURCE_FORMAT;


/**
* ESG information
*
*/
typedef struct  
{
	VO_PTR					pEsgInfo;		/*!< pointer to ESG struct depend on VO_MTP_SOURCE_FORMAT */
	VO_MTP_SOURCE_FORMAT	tSourceFormat;	/*!< source format */
}VO_MTP_ESG_INFO;


/**
* Scan parameter
*
*/
typedef struct  
{
	VO_PTR					pParam;			/*!< pointer to VO_MTP_CMMB_SCANPARAM,VO_MTP_DVBT_SCANITEM,VO_MTP_ISDBT_SCANPARAM, depend on VO_MTP_SOURCE_FORMAT*/
	VO_MTP_SOURCE_FORMAT	tSourceFormat;	/*!< source format */
}VO_MTP_SCAN_PARAMEPTERS;


/**
* Channel Information
*/
typedef struct
{
	VO_U32					nChannelID;			/*!< the channel ID */
	VO_TCHAR				szName[256];		/*!< the channel Name */
	VO_PTR					hIcon;				/*!< the channel Icon */
	VO_U32					nCurrent;			/*!< 1 is current channel 0 is not*/
	VO_MTP_SOURCE_FORMAT	nType;				/*!< live source type (CMMB, DVB-T, T-DMB, DVB-H..... */
	VO_PTR					pData;				/*!< if CMMB, pointer to VO_MTP_CHANNELINFOEX_CMMB struct */
}VO_MTP_CHANNELINFO;


/**
* Callback function. The MTP will notify client via this function for some events.
* \param pUserData [in] The user data which was set by vomtpOpen().
* \param nID [in] The status type, refer to VO_MTP_STATUS_BASE.
* \param nParam1 [in] status specific parameter 1.
* \param nParam2 [in] status specific parameter 2.
*/
typedef VO_VOID (VO_API * VOMTPSTATUS) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);


/**
* Callback function. The MTP will send the data out, basically raw data service,like traffic info
* \param pUserData [in] The user data which was set by vomtpOpen().
* \param nOutputType [in] The output data type, refer to VO_MTP_OUTPUT_TYPE
* \param pData [in] The data pointer, it MUST be converted to correct struct pointer according nType, refer to VO_MTP_OUTPUT_TYPE
* \param nParam [in] data specific parameter.
*/
typedef VO_VOID (VO_API * VOMTPSENDDATA) (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData, VO_U32 nParam);


typedef enum
{
	MTP_PUSH_MODE		= 0,		/*!<Push Mode.The AV play thread will create in engine */
	MTP_PULL_MODE		= 1,		/*!<Pull Mode.The client call GetParam to get AV data, currently not support*/
}MTP_AV_TRANSFER_MODE;

/**
* MTP Open parameters, used by vomtpOpen
*/
typedef struct
{
	VO_HANDLE		hDrawWnd;			/*!< The window's handle to draw video */
	VO_PTR			pUserData;			/*!< The user data. It will use in call back function */
	VOMTPSTATUS		fStatus;			/*!< Status callback function. */
	VOMTPSENDDATA	fSendData;			/*!< Send data callback function. */
	VO_U16			nSourceFormat;		/*!< The source format, refer to VO_MTP_SOURCE_FORMAT */
	VO_U16			nTransferMode;		/*!< The Audio Video transer mode, refer to MTP_AV_TRANSFER_MODE */
}VO_MTP_OPEN_PARAM;

/**
* MTP function set
*/
typedef struct
{
	/**
	* Initialize a Mobile TV Player session.
	* This function should be called first for a session.
	* \param phMTP [out] MTP handle.
	* \param pOpenParam [in] MTP open parameters
	*/
	VO_U32 (VO_API * vomtpOpen) (VO_HANDLE* phMTP, VO_MTP_OPEN_PARAM* pOpenParam);

	/**
	* Close the MTP session.
	* This function should be called last of the session.
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	*/
	VO_U32 (VO_API * vomtpClose) (VO_HANDLE hMTP);


	/**
	* Set data source type
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param pSource [in] The source. Normally it should be URL for HTTP Live Streaming.
	* \param nSourceFormat [in]. It can indicate the source type, refer to VO_MTP_SOURCE_FORMAT.
	*/
	VO_U32 (VO_API * vomtpSetDataSource) (VO_HANDLE hMTP, VO_PTR pSource, VO_U32 nSourceFormat);

	/**
	* Set the video draw area in the view window
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param nLeft [in] The left position of draw video area
	* \param nTop [in] The top position of draw video area
	* \param nRight [in] The right position of draw video area
	* \param nBottom [in] The bottom position of draw video area
	*/
	VO_U32 (VO_API * vomtpSetDrawArea) (VO_HANDLE hMTP, VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom);


	/**
	* Scan the channels
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param pParam [in] It was structure depend on the live source type... 
	*/
	VO_U32 (VO_API * vomtpScan) (VO_HANDLE hMTP, VO_MTP_SCAN_PARAMEPTERS* pParam);


	/**
	* Cancel the current task
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param nID [in] The task ID. It only support scan task now..
	*/
	VO_U32 (VO_API * vomtpCancel) (VO_HANDLE hMTP, VO_U32 nID);


	/**
	* Data source seek, Normally it should be used by HTTP Live Streaming
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param nPosition [in]. timestamp to seek to.
	*/
	VO_U32 (VO_API * vomtpSeek) (VO_HANDLE hMTP, VO_U64 nPosition);


	/**
	* Get current playing position, Normally it should be used by HTTP Live Streaming
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param pPosition [in]. current playing timestamp.
	*/
	VO_U32 (VO_API * vomtpGetPosition) (VO_HANDLE hMTP, VO_U64* pPosition);


	/**
	* start to play
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	*/
	VO_U32 (VO_API * vomtpStart) (VO_HANDLE hMTP);


	/**
	* stop playing
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	*/
	VO_U32 (VO_API * vomtpStop) (VO_HANDLE hMTP);


	/**
	* Get Channel information
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param pCount [out] The channel count
	* \param ppChannelInfo [out] The channel info pointer
	*/
	VO_U32 (VO_API * vomtpGetChannel) (VO_HANDLE hMTP, VO_U32* pCount, VO_MTP_CHANNELINFO** ppChannelInfo);


	/**
	* select the channel
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param nChannelID [in] The channel ID
	*/
	VO_U32 (VO_API * vomtpSetChannel) (VO_HANDLE hMTP, VO_S32 nChannelID);


	/**
	* Get ESG information
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param nChannelID [in] The channel ID
	* \param pESGInfo [out] The ESG info pointer
	*/
	VO_U32 (VO_API * vomtpGetESG) (VO_HANDLE hMTP, VO_S32 nChannelID, VO_MTP_ESG_INFO* pESGInfo);


	/**
	* Suspend or resume the audio playback
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param bSuspend [in] Audio control flag.
	*/
	VO_U32 (VO_API * vomtpAudioControl) (VO_HANDLE hMTP, VO_BOOL bSuspend);


	/**
	* Get audio volume.
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param pVolume [out] Current volume. The volume should be 0 - 100
	* \param pMaxVolume [out] Maximal volume. Set the pointer to VO_NULL if you do not care it.
	*/
	VO_U32 (VO_API * vomtpGetVolume) (VO_HANDLE hMTP, VO_U16* pVolume, VO_U16* pMaxVolume);


	/**
	* Set audio volume.
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param nVolume [in] The expected volume.
	*/
	VO_U32 (VO_API * vomtpSetVolume) (VO_HANDLE hMTP, VO_U16 nVolume);


	/**
	* Start to record the data with MP4 file format
	* \param hMTP [in] The MTP engine handle. Opened by voMTPOpen().
	* \param pRecFile [in] The MP4 file name. If it is VO_NULL, it will use default directory and name..
	*/
	VO_U32 (VO_API * vomtpRecStart) (VO_HANDLE hMTP, TCHAR* pRecFile);


	/**
	* Stop recording file
	* \param hMTP [in] The MTP engine handle. Opened by voMTPOpen().
	*/
	VO_U32 (VO_API * vomtpRecStop) (VO_HANDLE hMTP);


	/**
	* Get the special value from param ID
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param nParamID [in] The param ID
	* \param pParam [out] The return value depend on the param ID.
	*/
	VO_U32 (VO_API * vomtpGetParam) (VO_HANDLE hMTP, VO_U32 nParamID, VO_PTR pParam);


	/**
	* Set the special value from param ID
	* \param hMTP [in] The MTP handle. Opened by vomtpOpen().
	* \param nParamID [in] The param ID
	* \param pParam [in] The set value depend on the param ID.
	*/
	VO_U32 (VO_API * vomtpSetParam) (VO_HANDLE hMTP, VO_U32 nParamID, VO_PTR pParam);
} VO_MTP_API;


/**
* Get Mobile TV Player API interface
* \param pHandle [IN/OUT] Return the mobile TV API handle.
* \retval VO_ERR_MTP_OK Succeeded.
*/
VO_S32 VO_API voGetmtpAPI(VO_MTP_API* pHandle);

typedef VO_U32 (VO_API * VOGETMTPAPI) (VO_MTP_API* pHandle);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_MTP_API_H_ */


