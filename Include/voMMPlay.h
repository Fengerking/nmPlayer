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
 
#ifndef __voMMPlay_H__
#define __voMMPlay_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"
#include "voMem.h"
#include "voString.h"
#include "voIVCommon.h"
#include "voAudio.h"
#include "voVideo.h"
#include "voFile.h"
#include "voStreaming.h"

/**
 * return error ID.
 */
#define VO_ERR_VOMMP_FileFormat			(VO_INDEX_MFW_VOMMPLAY | 0x0101)
#define VO_ERR_VOMMP_AudioDecoder		(VO_INDEX_MFW_VOMMPLAY | 0x0102)
#define VO_ERR_VOMMP_AudioRender		(VO_INDEX_MFW_VOMMPLAY | 0x0103)
#define VO_ERR_VOMMP_VideoDecoder		(VO_INDEX_MFW_VOMMPLAY | 0x0104)
#define VO_ERR_VOMMP_VideoRender		(VO_INDEX_MFW_VOMMPLAY | 0x0105)


#define VO_VOMMP_PMID_Redraw			(VO_INDEX_MFW_VOMMPLAY | 0x0201)	//!< Set, Redraw the view, RECT * >
#define VO_VOMMP_PMID_VideoFormat		(VO_INDEX_MFW_VOMMPLAY | 0x0202)	//!< Get, Video Format, VO_VIDEO_FORMAT * >
#define VO_VOMMP_PMID_AudioFormat		(VO_INDEX_MFW_VOMMPLAY | 0x0203)	//!< Get, Audio Format, VO_AUDIO_FORMAT * >
#define VO_VOMMP_PMID_SeekMode			(VO_INDEX_MFW_VOMMPLAY | 0x0204)	//!< Get/Set, Seek Mode, VO_U32 *. 0 Key Frame Pos, 1  accurate pos>
#define VO_VOMMP_PMID_PlaySpeed			(VO_INDEX_MFW_VOMMPLAY | 0x0205)	//!< Get/Set, Play speed. VO_U32 *. 16 / 16 (fraction) >
#define VO_VOMMP_PMID_PlayFlagAV		(VO_INDEX_MFW_VOMMPLAY | 0x0206)	//!< Get/Set, Play AV flag. VO_U32 *. VO_U32 *. 3, AV, 2, A, 1 V) >
#define VO_VOMMP_PMID_LastError			(VO_INDEX_MFW_VOMMPLAY | 0x0207)	//!< Get, the last error information. VO_CHAR **	) >
#define VO_VOMMP_PMID_DrawRect			(VO_INDEX_MFW_VOMMPLAY | 0x0208)	//!< Get, the display rect VO_RECT *	) >
#define VO_VOMMP_PMID_VOSDK				(VO_INDEX_MFW_VOMMPLAY | 0x0209)	//!< Set, Force to use VO SDK VO_BOOL *	) >
#define VO_VOMMP_PMID_FileFormat		(VO_INDEX_MFW_VOMMPLAY | 0x020A)	//!< Get, the file format. VO_U32 * *	) >
#define VO_VOMMP_PMID_FrameRate			(VO_INDEX_MFW_VOMMPLAY | 0x020B)	//!< Get, the video frame rate. VO_U32 * *	) >
#define VO_VOMMP_PMID_ForThumbnail		(VO_INDEX_MFW_VOMMPLAY | 0x0210)	//!< Set, use for thumbnail VO_BOOL *	) >
#define VO_VOMMP_PMID_Status			(VO_INDEX_MFW_VOMMPLAY | 0x0211)	//!< Get, use for get current status VOMMPlAYSTATUS *	) >
#define VO_VOMMP_PMID_NearKeyFrame		(VO_INDEX_MFW_VOMMPLAY | 0x0212)	//!< Get, VO_VOMM_NEARKEYFRAME *	) >
#define VO_VOMMP_PMID_PlayClearestVideo	(VO_INDEX_MFW_VOMMPLAY | 0x0213)	//!< Set, VO_BOOL * ) >
#define VO_VOMMP_PMID_VideoRotation		(VO_INDEX_MFW_VOMMPLAY | 0x0214)	//!< Get, VO_S32 * ) >

#define VO_VOMMP_PMID_VideoRender		(VO_INDEX_MFW_VOMMPLAY | 0x0220)	//!< Set the Video render call back function VO_VOMM_CB_VIDEORENDER *	) >
#define VO_VOMMP_PMID_AudioRender		(VO_INDEX_MFW_VOMMPLAY | 0x0221)	//!< Set the Audio render call back function VO_VOMM_CB_AUDIORENDER *	) >
#define VO_VOMMP_PMID_StreamCallBack	(VO_INDEX_MFW_VOMMPLAY | 0x0222)	//!< Set, Stream Call Back Function. VO_VOMM_CB_STREAMING *	) >
#define VO_VOMMP_PMID_DRM_API			(VO_INDEX_MFW_VOMMPLAY | 0x0223)	//!< Set, Stream Call Back Function. VO_VOMM_CB_STREAMING *	) >
#define VO_VOMMP_PMID_DRM_FileOP		(VO_INDEX_MFW_VOMMPLAY | 0x0224)

#define VO_VOMMP_PMID_ThreadNum			(VO_INDEX_MFW_VOMMPLAY | 0x0888)	//!< Get/Set, Thread Number. VO_U32 * >
#define VO_VOMMP_PMID_Playback			(VO_INDEX_MFW_VOMMPLAY | 0x0889)	//!< Set, Playback AV. VO_U32 *. 3, AV, 2, A, 1 V >
#define VO_VOMMP_PMID_SetWorkPath		(VO_INDEX_MFW_VOMMPLAY | 0x0890)	//!< Set, Working path. VO_TCHAR *  >

#define VO_VOMMP_CBID_EndOfFile			(VO_INDEX_MFW_VOMMPLAY | 0x0301)	//!< End of the source  >
#define VO_VOMMP_CBID_Error				(VO_INDEX_MFW_VOMMPLAY | 0x0302)	//!< Error when playvack* >
#define VO_VOMMP_CBID_DebugText			(VO_INDEX_MFW_VOMMPLAY | 0x0303)	//!< The pValue1 is debug text. char *  >
#define VO_VOMMP_CBID_Buffering			(VO_INDEX_MFW_VOMMPLAY | 0x0304)	//!< The pValue1 is buffering percent 0-100 VO_U32 *  >
#define VO_VOMMP_CBID_BuffStart			(VO_INDEX_MFW_VOMMPLAY | 0x0305)	//!< The pValue1 is buffering percent 0-100 VO_U32 *  >
#define VO_VOMMP_CBID_BuffStop			(VO_INDEX_MFW_VOMMPLAY | 0x0306)	//!< The pValue1 is buffering percent 0-100 VO_U32 *  >
#define VO_VOMMP_CBID_Download			(VO_INDEX_MFW_VOMMPLAY | 0x0307)	//!< The pValue1 is buffering percent 0-100 VO_U32 *  >

/**
 * Call back function
 */
typedef VO_U32 (VO_API * VOMMPlayCallBack) (VO_PTR pUserData, VO_U32 nID, VO_PTR pValue1, VO_PTR pValue2);


/**
 * the status defination
 */
typedef enum {
	VOMM_PLAY_StatusLoad	= 0X00,		/*!< The module was loaded */
	VOMM_PLAY_StatusStop	= 0X01,		/*!< Stopped */
	VOMM_PLAY_StatusPause	= 0X02,		/*!< Paused  */
	VOMM_PLAY_StatusRun		= 0X03,		/*!< Runing  */
	VOMM_PLAY_Status_MAX	= VO_MAX_ENUM_VALUE
} VOMMPlAYSTATUS;


#define VO_VOMM_CREATE_SOURCE_URL				0x0001
#define VO_VOMM_CREATE_SOURCE_HANDLE			0x0002
#define VO_VOMM_CREATE_SOURCE_ID				0x0003
#define VO_VOMM_CREATE_SOURCE_ONLY				0x0010

typedef struct
{
	VO_U32						nFlag;			/*!< Create flags. */
	VO_MEM_OPERATOR *			pMemOP;			/*!< source name or handle. */
	VO_FILE_OPERATOR *			pFileOP;		/*!< source operator functions. */
	VO_LIB_OPERATOR *			pLibOP;			/*!< library operator functions. */
	VO_SOURCEDRM_CALLBACK *		pDrmCB;			/*!< DRM operator call back functions. */
	VO_U32						nReserve;
}VO_VOMM_INITPARAM;

typedef struct
{
	VO_PTR					pUserData;
	VOVIDEOCALLBACKPROC		pCallBack;
}VO_VOMM_CB_VIDEORENDER;

typedef struct
{
	VO_PTR					pUserData;
	VOAUDIOCALLBACKPROC		pCallBack;
}VO_VOMM_CB_AUDIORENDER;

typedef struct
{
	VO_PTR						pUserData;
	STREAMING_CALLBACK_NOTIFY	pCallBack;
}VO_VOMM_CB_STREAMING;

typedef struct
{
	VO_S64					Time;					// <I> specified time stamp, can > end but must > start, otherwise return OMX_ErrorBadParameter
	VO_S64					PreviousKeyframeTime;	// <O> time stamp of previous key frame, <= Time, 0x7fffffffffffffffll mean no key frame(commonly no such case)
	VO_S64					NextKeyframeTime;		// <O> time stamp of next key frame, > Time, 0x7fffffffffffffffll mean no key frame(commonly reach end)
}VO_VOMM_NEARKEYFRAME;

/**
 * vommPlay engine function set
 */
typedef struct
{
	VO_U32 (VO_API * Init) (VO_HANDLE * phPlay, VO_VOMM_INITPARAM * pParam);
	VO_U32 (VO_API * Uninit) (VO_HANDLE hPlay);
	VO_U32 (VO_API * SetCallBack) (VO_HANDLE hPlay, VOMMPlayCallBack pCallBack, VO_PTR pUserData);
	VO_U32 (VO_API * SetViewInfo) (VO_HANDLE hPlay, VO_PTR hView, VO_RECT * pRect);
	VO_U32 (VO_API * Create) (VO_HANDLE hPlay, VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	VO_U32 (VO_API * Run) (VO_HANDLE hPlay);
	VO_U32 (VO_API * Pause) (VO_HANDLE hPlay);
	VO_U32 (VO_API * Stop) (VO_HANDLE hPlay);
	VO_U32 (VO_API * GetDuration) (VO_HANDLE hPlay, VO_U32 * pDuration);
	VO_U32 (VO_API * GetCurPos) (VO_HANDLE hPlay, VO_S32 * pPos);
	VO_U32 (VO_API * SetCurPos) (VO_HANDLE hPlay, VO_S32 nPos);
	VO_U32 (VO_API * SetParam) (VO_HANDLE hPlay, VO_U32 nID, VO_PTR pValue);
	VO_U32 (VO_API * GetParam) (VO_HANDLE hPlay, VO_U32 nID, VO_PTR pValue);
} VOMM_PLAYAPI;

/**
 * Get voMM Play API interface
 * \param pPlay [IN/OUT] Return the play API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_None Succeeded.
 */
VO_S32 VO_API vommGetPlayAPI (VOMM_PLAYAPI * pPlay, VO_U32 uFlag);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // __voMMPlay_H__


