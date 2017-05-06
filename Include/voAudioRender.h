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
#ifndef __voAudioRender_H__
#define __voAudioRender_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voAudio.h"
#include "voMem.h"

#define	VO_PID_AUDIORENDER_BASE		 0x46000000							/*!< The base param ID for audio render */
#define	VO_PID_AUDIORENDER_MINBUFF	(VO_PID_AUDIORENDER_BASE | 0X0001)	/*!< Get min buffer time (ms) */
#define	VO_PID_AUDIORENDER_MAXBUFF	(VO_PID_AUDIORENDER_BASE | 0X0002)	/*!< Get max buffer time (ms) */

/**
 * audio render function set
 */
typedef struct
{
	VO_U32 (VO_API * Init) (VO_HANDLE * phRender, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag);
	VO_U32 (VO_API * Uninit) (VO_HANDLE hRender);
	VO_U32 (VO_API * SetFormat) (VO_HANDLE hRender, VO_AUDIO_FORMAT * pFormat);
	VO_U32 (VO_API * Start) (VO_HANDLE hRender);
	VO_U32 (VO_API * Pause) (VO_HANDLE hRender);
	VO_U32 (VO_API * Stop) (VO_HANDLE hRender);
	VO_U32 (VO_API * Render) (VO_HANDLE hRender, VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait);
	VO_U32 (VO_API * Flush) (VO_HANDLE hRender);
	VO_U32 (VO_API * GetPlayingTime) (VO_HANDLE hRender, VO_S64	* pPlayingTime);
	VO_U32 (VO_API * GetBufferTime) (VO_HANDLE hRender, VO_S32	* pBufferTime);
	VO_U32 (VO_API * SetCallBack) (VO_HANDLE hRender, VOAUDIOCALLBACKPROC pCallBack, VO_PTR pUserData);
	VO_U32 (VO_API * SetParam) (VO_HANDLE hRender, VO_U32 nID, VO_PTR pValue);
	VO_U32 (VO_API * GetParam) (VO_HANDLE hRender, VO_U32 nID, VO_PTR pValue);
} VO_AUDIO_RENDERAPI;

/**
 * Get audio render API interface
 * \param pRender [IN/OUT] Return the audio render API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_None Succeeded.
 */
VO_S32 VO_API voGetAudioRenderAPI (VO_AUDIO_RENDERAPI * pRender, VO_U32 uFlag);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // __voAudioRender_H__


