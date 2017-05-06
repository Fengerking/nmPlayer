	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMX_Types.h

	Contains:	memory operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voOMX_Types_H__
#define __voOMX_Types_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "OMX_Core.h"
#include "OMX_Video.h"

/** This enum defines the transition states of the Component*/
typedef enum OMX_TRANS_STATE {
	COMP_TRANSSTATE_None			= 0x00,
	COMP_TRANSSTATE_IdleToLoaded,
	COMP_TRANSSTATE_WaitResToLoaded,
	COMP_TRANSSTATE_LoadedToWaitRes,
	COMP_TRANSSTATE_WaitResToIdle,
	COMP_TRANSSTATE_LoadedToIdle,
	COMP_TRANSSTATE_PauseToIdle,
	COMP_TRANSSTATE_ExecuteToIdle,
	COMP_TRANSSTATE_IdleToExecute,
	COMP_TRANSSTATE_PauseToExecute,
	COMP_TRANSSTATE_IdleToPause,
	COMP_TRANSSTATE_ExecuteToPause,
	COMP_TRANSSTATE_AnyToInvalid,
	COMP_TRANSSTATE_MAX				= 0x7FFFFFFF
} OMX_TRANS_STATE;

typedef enum PORT_TRANS_STATUS {
	PORT_TRANS_DONE				= 0x0000,
	PORT_TRANS_DIS2ENA			= 0x0001,
	PORT_TRANS_ENA2DIS			= 0x0002,
	PORT_TRANS_MAX				= 0x7FFFFFFF
}	PORT_TRANS_STATUS;

/** This enum defines the channel config for audio*/
typedef enum AUDIO_CHANNEL_CONFIG {
	AUDIO_CHAN_NULL				= 0x0000,		/**< no channel  */
	AUDIO_CHAN_MONO				= 0x0001,		/**< Mono channel  */
	AUDIO_CHAN_DUALONE			= 0x0002,		/**< double mono channel */
	AUDIO_CHAN_DUALMONO			= 0x0010,		/**< dual mono(bilingual)channel */
	AUDIO_CHAN_DUALLEFT			= 0x0011,		/**< dual left channel  */
	AUDIO_CHAN_DUALRIGHT		= 0x0012,		/**< dual right channel */
	AUDIO_CHAN_STEREO			= 0x0020,		/**< stereo channel */
	AUDIO_CHAN_STE2MONO			= 0x0021,		/**< stereo channel to mono channel */
	AUDIO_CHAN_MULTI			= 0x0030,		/**< multichannel , channel number gt 2 */
	AUDIO_CHAN_MULDOWNMIX2		= 0x0031,		/**< multichannel,downmix to 2 channel */
	AUDIO_CHAN_MODE_MAX			= 0x7FFFFFFF
}	AUDIO_CHANNEL_CONFIG;

typedef OMX_ERRORTYPE (OMX_APIENTRY * OMXCHECKRENDERBUFFER)(OMX_IN OMX_HANDLETYPE hComponent,
														    OMX_IN OMX_PTR pAppData,
														    OMX_IN OMX_BUFFERHEADERTYPE* pBuffer,
															OMX_IN OMX_INDEXTYPE nType,
															OMX_IN OMX_PTR pFormat);
typedef struct OMX_VO_CHECKRENDERBUFFERTYPE {
    OMXCHECKRENDERBUFFER	pCallBack;
    OMX_PTR					pUserData;
} OMX_VO_CHECKRENDERBUFFERTYPE;

typedef struct OMX_VO_VIDEOBUFFERTYPE {
    OMX_U8 *				Buffer[3];
	OMX_U32					Stride[3];
	OMX_U32					Width;
	OMX_U32					Height;
	OMX_COLOR_FORMATTYPE	Color;
	OMX_S64					Time;
} OMX_VO_VIDEOBUFFERTYPE;

typedef struct OMX_VO_DISPLAYAREATYPE  {
	OMX_PTR					hView;
	OMX_U32					nX;
	OMX_U32					nY;
	OMX_U32					nWidth;
	OMX_U32					nHeight;
} OMX_VO_DISPLAYAREATYPE;

typedef struct OMX_VO_SOURCEDRMCALLBACK {
	OMX_PTR					pCallBack;	//VOSOURCEDRMCALLBACKFUNC
	OMX_PTR					pUserData;
} OMX_VO_SOURCEDRMCALLBACK;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __voOMX_Types_H__
