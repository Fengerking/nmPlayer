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


#ifndef __voMMRecord_H__
#define __voMMRecord_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"
#include "voString.h"
#include "voIVCommon.h"
#include "voAudio.h"
#include "voVideo.h"
#include "voType.h"




/**
 * return error ID.
 */
#define VO_MMR_ERR_BASE		(VO_ERR_BASE | VO_INDEX_MFW_VOMMREC)

 
 
/**
* Audio source sample buffer
*/
typedef struct
{
	VO_BYTE* buffer;			/*!< Buffer pointer */
	VO_U32 length;				/*!< Buffer size in byte */
	VO_U32 start_time;			/*!< Sample start time  */
	VO_U32 end_time;			/*!< Sample end time  */
	VO_U32 disconnect;			/*!< If buffer is continuous */
}
VO_MMR_ASBUFFER;


/**
* Video source sample buffer
*/
typedef struct
{
	VO_VIDEO_BUFFER buffer;		/*!< Video buffer */

	VO_U32		start_time;		/*!< Video sample start time */
	VO_U32		end_time;		/*!< Video sample end time */
	VO_U32		disconnect;		/*!< If buffer is continuous 0:continuous, 1:disconnect*/
}
VO_MMR_VSBUFFER;


/**
* General video source format
*/
typedef struct
{
	VO_U16 width;							/*!< Video width */
	VO_U16 height;							/*!< Video height */
	VO_IV_COLORTYPE video_type;				/*!< Video type */
	VO_IV_RTTYPE rotation_type;				/*!< Video rotation type */
}
VO_MMR_VSFORMAT;


/**
* General audio source format
*/
typedef struct
{
	VO_AUDIO_FORMAT fmt;
}
VO_MMR_ASFORMAT;


/**
* Recording control, must set the value via SetParam() before recording.
*/
typedef enum
{
	ONLY_REC_VIDEO  = 0x0001,	/*!< Record without audio */
	ONLY_REC_AUDIO  = 0x0002,	/*!< Record without video */
	BOTH_REC		= 0x0004	/*!< Record video and audio */
}
VO_MMR_CONTENTCONTROL;



/**
 * Parameter ID.
 */
#define VO_MMR_PMID_BASE	(VO_PID_COMMON_BASE | VO_INDEX_MFW_VOMMREC)
typedef enum
{	
	VO_MMR_PID_AUDIOENCODETYPE		= (VO_MMR_PMID_BASE | 0x0001),		/*!< audio encode type, see VO_AUDIO_CODINGTYPE */
	VO_MMR_PID_VIDEOENCODETYPE		= (VO_MMR_PMID_BASE | 0x0002),		/*!< video encode type, see VO_VIDEO_CODINGTYPE */
	VO_MMR_PID_CONTENTTYPE			= (VO_MMR_PMID_BASE | 0x0003),		/*!< recording control, see VO_MMR_CONTENTCONTROL */
	VO_MMR_PID_MPEG4BITRATE			= (VO_MMR_PMID_BASE | 0x0004),		/*!< mpeg4 encode bit rate */
	VO_MMR_PID_H263BITRATE			= (VO_MMR_PMID_BASE | 0x0005),		/*!< h263 encode bit rate */
	VO_MMR_PID_VIDEOQUALITY			= (VO_MMR_PMID_BASE | 0x0006),		/*!< video encode quality level, see VO_IV_QUALITY*/
	VO_MMR_PID_RECORDTIMELIMIT		= (VO_MMR_PMID_BASE | 0x0007),		/*!< video record time limit*/
	VO_MMR_PID_RECORDSIZELIMIT		= (VO_MMR_PMID_BASE | 0x0008),		/*!< video record size limit*/
	VO_MMR_PID_CURRFILESIZE			= (VO_MMR_PMID_BASE | 0x0009),		/*!< current file size*/
	VO_MMR_PID_VIDEOENCODEFRMRATE	= (VO_MMR_PMID_BASE | 0x000A),		/*!< video encode frame rate */
	VO_MMR_PID_VIDEOSRCFMT			= (VO_MMR_PMID_BASE | 0x000B),		/*!< video source format, see VO_MMR_VSFORMAT */
	VO_MMR_PID_AUDIOSRCFMT			= (VO_MMR_PMID_BASE | 0x000C),		/*!< audio source format, see VO_MMR_ASFORMAT */
	VO_MMR_PID_RECORDTIME			= (VO_MMR_PMID_BASE | 0x000D)		/*!< record duration */
}
VO_MMR_PARAMID;


/**
 * Record notification message type.
 * All the messages will be modified by callback function, see VOMMRecordCallBack definition.
 */

typedef enum
{	
	VO_MMR_MSG_MEMORY_FULL				= (VO_INDEX_MFW_VOMMREC | 0x0001),			/*!< Out of memory */
	VO_MMR_MSG_DISK_FULL				= (VO_INDEX_MFW_VOMMREC | 0x0002),			/*!< Disk is full */
	VO_MMR_MSG_VIDEOENCODE_INIT_FAILED	= (VO_INDEX_MFW_VOMMREC | 0x0003),			/*!< Video encode init failed */
	VO_MMR_MSG_AUDIOENCODE_INIT_FAILED	= (VO_INDEX_MFW_VOMMREC | 0x0004),			/*!< Audio encode init failed */
	VO_MMR_MSG_FILEWRITER_INIT_FAILED	= (VO_INDEX_MFW_VOMMREC | 0x0005),			/*!< File writer init failed */
	VO_MMR_MSG_REC_TIME_EXPIRED			= (VO_INDEX_MFW_VOMMREC | 0x0006),			/*!< Reach to the time limitation of recording */
	VO_MMR_MSG_REC_SIZE_EXPIRED			= (VO_INDEX_MFW_VOMMREC | 0x0007)			/*!< Reach to the file size limitation of recording */
}
VO_MMR_MSG;


/**
 * Call back function
 */
typedef VO_U32 (VO_API * VOMMRecordCallBack) (VO_PTR pUserData, VO_U32 nID, VO_PTR pValue);


/**
 * vommRecord engine function set
 */
typedef struct
{
	VO_U32 (VO_API * Init) (VO_HANDLE * phRec);
	VO_U32 (VO_API * Uninit) (VO_HANDLE hRec);
	VO_U32 (VO_API * SetNotifyCallback) (VO_HANDLE hRec, VOMMRecordCallBack pCallBack, VO_PTR pUserData);
	VO_U32 (VO_API * SetFileName) (VO_HANDLE hRec, VO_TCHAR* pszFileName);
	
	VO_U32 (VO_API * SendAudioSrc) (VO_HANDLE hRec, VO_MMR_ASBUFFER* pBuffer);
	VO_U32 (VO_API * SendVideoSrc) (VO_HANDLE hRec, VO_MMR_VSBUFFER* pBuffer);
	
	VO_U32 (VO_API * Start) (VO_HANDLE hRec);
	VO_U32 (VO_API * Stop) (VO_HANDLE hRec);
	
	VO_U32 (VO_API * SetParam) (VO_HANDLE hRec, VO_U32 nID, VO_PTR pValue);
	VO_U32 (VO_API * GetParam) (VO_HANDLE hRec, VO_U32 nID, VO_PTR pValue);
} VOMM_RECORDAPI;

/**
 * Get voMM Record API interface
 * \param pRecord [IN/OUT] Return the record API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_None Succeeded.
 */
VO_S32 VO_API vommGetRecordAPI (VOMM_RECORDAPI * pRecord, VO_U32 uFlag);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // __voMMRecord_H__


