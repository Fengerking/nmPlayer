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

#ifndef __VOMTVENGINE_H__
#define __VOMTVENGINE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"
#include "voString.h"
#include "voMem.h"

#include "voAudio.h"
#include "voVideo.h"
#include "voMTVBase.h"

/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_MTVENGINE_BASE			0x4A000000							/*!< the base param ID for source modules */
#define	VO_PID_MTVENGINE_SIGNAL			(VO_PID_MTVENGINE_BASE | 0x0001)		/*!< <G> get the signal strength. The value is 0 - 100 */
#define VO_PID_MTVENGINE_PROGRAMGUIDE	(VO_PID_MTVENGINE_BASE | 0x0002)		/*!< <G> Not used for now */
#define VO_PID_MTVENGINE_EB				(VO_PID_MTVENGINE_BASE | 0x0003)		/*!< <G> Not used for now */
#define VO_PID_MTVENGINE_CAID			(VO_PID_MTVENGINE_BASE | 0x0004)		/*!< <G> get the CA card ID, pValue is TCHAR pointer */
#define VO_PID_MTVENGINE_KDAVERSION		(VO_PID_MTVENGINE_BASE | 0x0005)		/*!< <G> get the KDA version, pValue is TCHAR pointer */
#define VO_PID_MTVENGINE_PROGRAMINFO	(VO_PID_MTVENGINE_BASE | 0x0006)		/*!< <G> get the program info array, pValue is pointer to VOENGPROGRAMEINFO_CMMB, return value is total program number */
#define VO_PID_MTVENGINE_RECORDSTATUS	(VO_PID_MTVENGINE_BASE | 0x0007)		/*!< <G> get recording status */
#define VO_PID_MTVENGINE_STREAMINFO		(VO_PID_MTVENGINE_BASE | 0x0008)		/*!< <G> get the stream information and codec type */
#define	VO_PID_MTVENGINE_CACARDNAME		(VO_PID_MTVENGINE_BASE | 0x0011)		/*!< <S> set the CA card device name, nValue is a TCHAR string pointer */
#define VO_PID_MTVENGINE_CHDATASAVEPATH	(VO_PID_MTVENGINE_BASE | 0x0012)		/*!< <S> set the file path (w/ name) to save channel info, return channel number, Should be called immediately after voMTPOpen */
#define VO_PID_MTVENGINE_CACARDREMOVED	(VO_PID_MTVENGINE_BASE | 0x0013)		/*!< <S> notify that CA card is removed */
#define VO_PID_MTVENGINE_CACARDINSERTED	(VO_PID_MTVENGINE_BASE | 0x0014)		/*!< <S> notify that CA card is inserted */
#define VO_PID_MTVENGINE_DATAEXCALLBACK	(VO_PID_MTVENGINE_BASE | 0x0015)		/*!< <S> set callback pointer (VOENGSENDDATAEX)for data service */
#define VO_PID_MTVENGINE_EBQUERY		(VO_PID_MTVENGINE_BASE | 0x0016)		/*!< <S> check EB status (only for two channel CMMB chips) */
#define VO_PID_MTVENGINE_STARTRECORD	(VO_PID_MTVENGINE_BASE | 0x0017)		/*!< <S> start program data recording */
#define VO_PID_MTVENGINE_ENDRECORD		(VO_PID_MTVENGINE_BASE | 0x0018)		/*!< <S> end program data recording */

/**
* Error code
*/
#define VO_ERR_MTVENGINE_OK				VO_ERR_NONE
#define VO_ERR_MTVENGINE_BASE			0x80000000
#define VO_ERR_MTVENGINE_FAIL				VO_ERR_MTVENGINE_BASE
#define VO_ERR_MTVENGINE_OUTOFMEMORY		(VO_ERR_MTVENGINE_BASE | 0x0001)		/*!< out of memory */
#define VO_ERR_MTVENGINE_NULLPOINTER		(VO_ERR_MTVENGINE_BASE | 0x0002)		/*!< null pointer */
#define VO_ERR_MTVENGINE_INVALIDARG			(VO_ERR_MTVENGINE_BASE | 0x0003)		/*!< invalid argument */
#define VO_ERR_MTVENGINE_LOADDRIVERFAILED	(VO_ERR_MTVENGINE_BASE | 0x0004)		/*!< load driver failed */
#define VO_ERR_MTVENGINE_NOCHANNEL			(VO_ERR_MTVENGINE_BASE | 0x0005)		/*!< no channel */
#define VO_ERR_MTVENGINE_NO_DATA			(VO_ERR_MTVENGINE_BASE | 0x0006)		/*!< no data */

/**
* Status update code
*/
#define VO_MTVENGINE_STATUS_BASE				0x41000000
#define	VO_MTVENGINE_STATUS_SCAN_PROCESS		(VO_MTVENGINE_STATUS_BASE | 0x0001)	 /*!< nParam1 is scanned channels, nParam2 is total channels */
#define	VO_MTVENGINE_STATUS_CHANNEL_STOP		(VO_MTVENGINE_STATUS_BASE | 0x0002)	 /*!< nParam1 is old type, nParam2 is new type */
#define	VO_MTVENGINE_STATUS_CHANNEL_START		(VO_MTVENGINE_STATUS_BASE | 0x0003)	 /*!< nParam1 is old type, nParam2 is new type */
#define VO_MTVENGINE_STATUS_DEVICE_NOTREADY		(VO_MTVENGINE_STATUS_BASE | 0x0004)	 
#define VO_MTVENGINE_STATUS_DEVICE_ERROR		(VO_MTVENGINE_STATUS_BASE | 0x0005)
#define VO_MTVENGINE_STATUS_NO_SIGNAL           (VO_MTVENGINE_STATUS_BASE | 0x0006)
#define VO_MTVENGINE_STATUS_DEVICE_INIT_OK      (VO_MTVENGINE_STATUS_BASE | 0x0007)
#define VO_MTVENGINE_STATUS_CA_ACCESS_CHANGE	(VO_MTVENGINE_STATUS_BASE | 0x0008)	/*!< Abandoned */
#define VO_MTVENGINE_STATUS_RECEIVING_ESG		(VO_MTVENGINE_STATUS_BASE | 0x0009)
#define VO_MTVENGINE_STATUS_CMMB_CA_INVALIDCARD	(VO_MTVENGINE_STATUS_BASE | 0x000A)
#define VO_MTVENGINE_STATUS_CMMB_CA_NOPERMIT	(VO_MTVENGINE_STATUS_BASE | 0x000B)
#define VO_MTVENGINE_STATUS_EB_RECEIVED			(VO_MTVENGINE_STATUS_BASE | 0x000C)	/*!< nParam1 is Update ID, nParam2 is struct pointer */
#define VO_MTVENGINE_STATUS_PROGRAMGUIDE_RECEIVED	(VO_MTVENGINE_STATUS_BASE | 0x000D)	/*!< nParam1 is Update ID, nParam2 is struct pointer */
#define VO_MTVENGINE_STATUS_ESG_NEEDUPDATE			(VO_MTVENGINE_STATUS_BASE | 0x000E)	
#define VO_MTVENGINE_STATUS_PROGRAM_CHANGED			(VO_MTVENGINE_STATUS_BASE | 0x000F)	 /*!< nParam1 is old program ID, nParam2 is new program ID */

/**
* Task code
*/
#define VO_MTVENGINE_TASK_BASE		0x42000000
#define	VO_MTVENGINE_TASK_SCAN		(VO_MTVENGINE_TASK_BASE | 0x0001)	 /*!< the scan task ID */

/**
* Various maximum values
*/
#define	VO_MTVENGINE_MAX_TITLE_LEN		128
#define	VO_MTVENGINE_MAX_DIGEST_LEN		1024
#define	VO_MTVENGINE_MAX_KEYWORD_LEN	256
#define	VO_MTVENGINE_MAX_EB_LEN			4096

/**
* Codec Format flag 
*/
#define VO_MTVENGINE_AUDIOFRAME			1
#define VO_MTVENGINE_VIDEOFRAME			2
#define VO_MTVENGINE_DATABLOCK			3

/**
 * Engine format type
 */
typedef enum
{
	VO_MTVENGINE_FT_FILE =		0x00000000, /*!< Unknown format */
	VO_MTVENGINE_FT_TDMB =		0x00000001, /*!< T-DMB */
	VO_MTVENGINE_FT_DAB  =		0x00000002, /*!< DAB */
	VO_MTVENGINE_FT_DVBH =		0x00000003, /*!< DVB-H stream */
	VO_MTVENGINE_FT_ISDBT =		0x00000005, /*!< ISDB-T stream */
	VO_MTVENGINE_FT_DVBT =		0x00000006,  /*!< DVB-T stream */
	VO_MTVENGINE_FT_DMBT =		0x00000007,  /*!< DMB-T stream */
	VO_MTVENGINE_FT_DVBHTS =	0x00000008, /*!< DVB-H stream with TS payload */
	VO_MTVENGINE_FT_CMMB =		0x00000010, /*!< CMMB stream */
}VO_MTVENGINE_FORMATTYPE;

/**
 * DVB-T scan item
 */
typedef struct
{
	VO_U32 			nFrequency; /*!< in kHz */
	VO_U32 			nBandwidth; /*!< in kHz */
}VO_MTVENGINE_DVBT_SCANITEM;

/**
 * DVB-T scan parameters
 */
typedef struct
{
	VO_U32 					nCount;
	VO_MTVENGINE_DVBT_SCANITEM *pItems;
}VO_MTVENGINE_DVBT_SCANPARAM;

/**
 * T-DMB scan item parameter
 */
typedef struct
{
	VO_U32 			nFrequency; /*!< in kHz */
}VO_MTVENGINE_TDMB_SCANITEM;

/**
* T-DMB scan parameters
*/
typedef struct
{
	VO_U32 			nCount;
	VO_MTVENGINE_TDMB_SCANITEM *pItems;
}VO_MTVENGINE_TDMB_SCANPARAM;

/**
 * ISDB-T scan parameters
 */
typedef struct
{
	VO_U32 			nStartFrequency;
	VO_U32 			nEndFrequency;
}VO_MTVENGINE_ISDBT_SCANPARAM;

/**
* DVB-H scan parameters
*/
typedef struct
{
	VO_U32 			nSize;
	VO_PBYTE		pBuffer;	/* sdp content */
}VO_MTVENGINE_DVBH_SCANPARAM;

/**
 * File scan parameters
 */
typedef struct
{
	VO_TCHAR *			pFolder;
	VO_TCHAR *			pExt;
	VO_U32				nBitrate;
	VO_U32				nPackSize;
}VO_MTVENGINE_FILE_SCANPARAM;

/**
 * CMMB scan parameters
 */
typedef struct
{
	VO_U32				nStartFreq;
	VO_U32				nEndFreq;
}VO_MTVENGINE_CMMB_SCANPARAM;

/**
* Language Type 
*/
typedef enum
{
	VO_MTVENGINE_LANG_CHINESE			= 0,
	VO_MTVENGINE_LANG_ENGLISH			= 1,
	VO_MTVENGINE_LANG_MAX				= 10
}VO_MTVENGINE_LANGTYPE;

/**
 * Channel Information
 */
typedef struct
{
	VO_U32					nChannelID;     /*!< the channel ID */
	VO_TCHAR				szName[VO_MTVENGINE_MAX_TITLE_LEN];	/*!< the channel Name */
	VO_PTR					hIcon;			/*!< the channel Icon */
	VO_U32					nCurrent;		/*!< 1 is current channel 0 is not*/
	VO_MTVENGINE_FORMATTYPE		nType;			/*!< Engine type (CMMB, DVB-T, T-DMB, DVB-H..... */
	VO_PTR					pData;			/*!< if CMMB, pointer to VO_MTVENGINE_CHANNELINFOEX_CMMB struct */
}VO_MTVENGINE_CHANNELINFO;

/**
 * CMMB Channel Extension Information
 */
typedef struct  
{
	VO_U32					nServiceID;
	VO_U32					nClass;
	VO_U32					nGenre;
	VO_U32					nParaID;
	VO_TCHAR				szName[VO_MTVENGINE_MAX_TITLE_LEN];
	VO_BOOL					bForFree;
	VO_MTVENGINE_LANGTYPE		nLang;
}VO_MTVENGINE_CMMB_CHANNELINFOEX;

/**
 * ESG Information
 */
typedef struct
{
	VO_U32			nChannelID;     /*!< the channel ID */
	VO_U32			nIndex;			/*!< the index content */
	VO_TCHAR		szName[VO_MTVENGINE_MAX_TITLE_LEN];	/*!< the content Name */
	VODATETIME		nStartTime;
	VO_U32			nDuration;
	VO_PTR			pData;			/*!< if CMMB, pointer to VO_MTVENGINE_ESGINFOEX_CMMB struct*/
}VO_MTVENGINE_ESGINFO;

/**
 * CMMB ESG Extension Information 
 */
typedef struct  
{
	VO_U32				nScheduleID;
	VO_U32				nServiceID;
	VO_U32				nContentID;
	VO_TCHAR			szTitle[VO_MTVENGINE_MAX_TITLE_LEN];
	VO_MTVENGINE_LANGTYPE		nLang;
	VODATETIME			nStartTime;
	VO_BOOL				bForFree;
	VO_BOOL				bLive;
	VO_BOOL				bRepeat;
}VO_MTVENGINE_CMMB_ESGINFOEX;

/**
 * CMMB Program Information
 */
typedef struct  
{
	VO_U32				nContentID;
	VO_U32				nClass;
	VO_U32				nGenre;
	VO_U32				nDuration;
	VO_TCHAR			szTitle[VO_MTVENGINE_MAX_TITLE_LEN];
	VO_TCHAR			szDigestInfo[VO_MTVENGINE_MAX_DIGEST_LEN];
	VO_TCHAR			szKeyword[VO_MTVENGINE_MAX_KEYWORD_LEN];
	VO_MTVENGINE_LANGTYPE	nAudioLang;
	VO_MTVENGINE_LANGTYPE	nSubtitleLang;
	VO_MTVENGINE_LANGTYPE	nLang;
	VO_U32				nServiceParaID;
	VO_TCHAR			szExtentInfo[128];
}VO_MTVENGINE_CMMB_PROGRAMEINFO;

/**
 * CMMB EB Information
 */
typedef struct
{
	VO_U32			nType;
	VO_U32			nLevel;
	VO_TCHAR 		szText[VO_MTVENGINE_MAX_EB_LEN];
}VO_MTVENGINE_CMMB_EBINFO;

/**
* Codec ID
*/
typedef enum
{
	VO_MTVENGINE_CODEC_UNKNOWN = 0,
	VO_MTVENGINE_CODEC_H264 = 0x10, /*!< H264 or AVC */
	VO_MTVENGINE_CODEC_MPEG2 = 0x20, /*!< Mpeg2 video */
	VO_MTVENGINE_CODEC_AAC = 0x01, /*!< AAC+ or BSAC */
	VO_MTVENGINE_CODEC_MP3 = 0x02, /*!< MPEG 1, layer 3, or layer 2 */
	VO_MTVENGINE_CODEC_AC3 = 0x03, /*!< AC3 */
	VO_MTVENGINE_CODEC_DRA = 0x04, /*!< DRA */
}VO_MTVENGINE_CODECID;

typedef struct  
{
	VO_MTVENGINE_CODECID	videoCodec;
	VO_MTVENGINE_CODECID	audioCodec;
}VO_MTVENGINE_STREAMINFO;

/**
* Record status type
*/
typedef enum
{
	VO_MTVENGINE_RECORD_INVALID = 0,	/*!< Unknown format */
	VO_MTVENGINE_RECORD_ONGOING = 1 /*!< T-DMB */
}VO_MTVENGINE_RECORDSTATUSTYPE;

/**
 * Callback function. The engine will notify client via this function for some events.
 * \param pUserData [in] The user data which was set by voengOpen().
 * \param nID [in] The status type.
 * \param nParam1 [in] status specific parameter 1.
 * \param nParam2 [in] status specific parameter 2.
 */
typedef VO_VOID (VO_API * VOENGSTATUS) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

/**
 * Callback function. The engine will send the data out..
 * \param pUserData [in] The user data which was set by voengOpen().
 * \param nChannelID [in] The channel ID.
 * \param pData [in] The data pointer. Set pData as -1 before change channel, the video buffer will be flushed.
 * \param nDataLen [in] The data size..
 */
typedef VO_VOID (VO_API * VOENGSENDDATA) (VO_PTR pUserData, VO_U32 nChannelID, VO_PBYTE pData, VO_U32 nDataLen);

/**
* Callback function. The engine will send out the special data which is retrieved from data service..
* \param pUserData [in] The user data which was set by voengOpen().
* \param pData [in] The data pointer. 
* \param nDataLen [in] The data size.
* \param nParam [in] The specific parameter.
*/
typedef VO_VOID (VO_API * VOENGSENDDATAEX) (VO_PTR pUserData, VO_PBYTE pData, VO_U32 nDataLen, VO_U32 nParam);

/**
* MTV Engine function set
*/
typedef struct
{
/**
 * Initialize a MT source session.
 * This function should be called first for a session.
 * \param phMTEng [out] MT source engine handle.
 * \param pUserData [in] The user data. It will use in call back function
 * \param fStatus [in] status notify call back function pointer.
 * \param fSendData [in] send data call back function pointer.
 * Return Value: 0 it didn't scan before, > 0 it scaned before and return the channels
 */
VO_U32 (VO_API * Open) (VO_HANDLE * phMTEng, VO_PTR pUserData, VOENGSTATUS	fStatus, VOENGSENDDATA fSendData);

/**
 * Close the MT source session.
 * This function should be called last of the session.
 * \param hMTEng [in] The MT source engine handle. Opened by voengOpen().
 */
VO_U32 (VO_API * Close) (VO_HANDLE hMTEng);

/**
 * Scan the channels
 * \param hMTEng [in] The MT source engine handle. Opened by voengOpen().
  * \param pParam [in] It was structure depend on the engine type..
 */
VO_U32 (VO_API * Scan) (VO_HANDLE hMTEng, VO_PTR pParam);

/**
 * Cancel the current task
 * \param hMTEng [in] The MT source engine handle. Opened by voengOpen().
  * \param nID [in] The tast ID. It only support scan task now..
*/
VO_U32 (VO_API * Cancel) (VO_HANDLE hMTEng, VO_U32 nID);

/**
 * start to send data out
 * \param hMTEng [in] The MT source engine handle. Opened by voengOpen().
 */
VO_U32 (VO_API * Start) (VO_HANDLE hMTEng);

/**
 * stop to send data out
 * \param hMTEng [in] The MT source engine handle. Opened by voengOpen().
 */
VO_U32 (VO_API * Stop) (VO_HANDLE hMTEng);

/**
 * Get Channel information
 * \param hMTEng [in] The MT source engine handle. Opened by voengOpen().
 * \param pCount [out] The channel count
 * \param ppChannelInfo [out] The channel info pointer
 */
VO_U32 (VO_API * GetChannel) (VO_HANDLE hMTEng, VO_U32 * pCount, VO_MTVENGINE_CHANNELINFO ** ppChannelInfo);

/**
 * select the channel
 * \param hMTEng [in] The MT source engine handle. Opened by voengOpen().
 * \param nChannelID [in] The channel ID
 */
VO_U32 (VO_API * SetChannel) (VO_HANDLE hMTEng, VO_S32 nChannelID);

/**
 * Get ESG information
 * \param hMTEng [in] The MT source engine handle. Opened by voengOpen().
 * \param nChannelID [in] The channel ID
 * \param pCount [out] The ESG count
 * \param ppESGInfo [out] The ESG info pointer
 */
VO_U32 (VO_API * GetESG) (VO_HANDLE hMTEng, VO_S32 nChannelID, VO_U32 * pCount, VO_MTVENGINE_ESGINFO ** ppESGInfo);

/**
 * Get the special value from param ID
 * \param hMTEng [in] The MT source engine handle. Opened by voengOpen().
 * \param nParamID [in] The param ID
 * \param pValue [out] The return value depend on the param ID.
 */
VO_U32 (VO_API * GetParam) (VO_HANDLE hMTEng, VO_U32 nParamID, VO_S32 * pParam);

/**
 * Set the special value from param ID
 * \param hMTEng [in] The MT source engine handle. Opened by voengOpen().
 * \param nParamID [in] The param ID
 * \param nValue [in] The set value depend on the param ID.
 */
VO_U32 (VO_API * SetParam) (VO_HANDLE hMTEng, VO_U32 nParamID, VO_PTR pParam);
} VO_MTVENGINE_API;

/**
* Get MTVEngine API interface
* \param pHandle [IN/OUT] Return the MTVEngine API handle.
* \retval VO_ERR_MTVENGINE_OK Succeeded.
*/
VO_S32 VO_API voGetMTVEngineAPI(VO_MTVENGINE_API* pHandle);

typedef VO_U32 (VO_API * VOGETMTVENGINEAPI) (VO_MTVENGINE_API* pHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VOMTVENGINE_H__
