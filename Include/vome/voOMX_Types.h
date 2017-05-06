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

/**
* the track type
*/
typedef enum
{
	VOME_SOURCE_VIDEO	= 0,
	VOME_SOURCE_AUDIO	= 1,
	VOME_SOURCE_TEXT		= 2
} VOME_TRACKTYPE;

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


typedef struct OMX_VO_AUDIO_PARAM_DTSTYPE {
    OMX_U32 nSize;                 /**< size of the structure in bytes */
    OMX_VERSIONTYPE nVersion;      /**< OMX specification version information */
    OMX_U32 nPortIndex;            /**< port that this structure applies to */
    OMX_U32 nChannels;             /**< Number of channels */
    OMX_U32 nBitRate;              /**< Bit rate of the input data.  Use 0 for variable
                                        rate or unknown bit rates */
    OMX_U32 nSampleRate;           /**< Sampling rate of the source data.  Use 0 for
                                        variable or unknown sampling rate. */
} OMX_VO_AUDIO_PARAM_DTSTYPE;

typedef struct OMX_VO_AUDIO_PARAM_AC3TYPE {
    OMX_U32 nSize;                 /**< size of the structure in bytes */
    OMX_VERSIONTYPE nVersion;      /**< OMX specification version information */
    OMX_U32 nPortIndex;            /**< port that this structure applies to */
    OMX_U32 nChannels;             /**< Number of channels */
    OMX_U32 nBitRate;              /**< Bit rate of the input data.  Use 0 for variable
                                        rate or unknown bit rates */
    OMX_U32 nSampleRate;           /**< Sampling rate of the source data.  Use 0 for
                                        variable or unknown sampling rate. */
} OMX_VO_AUDIO_PARAM_AC3TYPE;

typedef struct OMX_VO_OUTPUTCONFIGDATATYPE {
	OMX_U32 nSize;                 /**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;      /**< OMX specification version information */
	OMX_U32 nPortIndex;            /**< port that this structure applies to */
	OMX_BOOL bOutputConfigData;	   /**< if output configure data or not */
} OMX_VO_OUTPUTCONFIGDATATYPE;

typedef struct VOME_TRACKINFO {
	OMX_U32				nIndex;
	VOME_TRACKTYPE		nType;
	//We can add other track info such as codec/bitrate/language heres
} VOME_TRACKINFO;

typedef struct VOME_TRACKSET{
	OMX_U32				nIndex;
	OMX_S32				nTimeStamp;
}VOME_TRACKSET;

typedef struct
{
	OMX_PTR	pUserData;
	OMX_PTR	(OMX_APIENTRY * LoadLib) (OMX_PTR pUserData, OMX_STRING pLibName, OMX_S32 nFlag);
	OMX_PTR	(OMX_APIENTRY * GetAddress) (OMX_PTR pUserData, OMX_PTR hLib, OMX_STRING pFuncName, OMX_S32 nFlag);
	OMX_S32	(OMX_APIENTRY * FreeLib) (OMX_PTR pUserData, OMX_PTR hLib, OMX_S32 nFlag);
} OMX_VO_LIB_OPERATOR;

OMX_API OMX_ERRORTYPE OMX_SetWorkingPath (OMX_IN  OMX_STRING cWorkingPath);

typedef enum
{
	OMX_VO_FILESINK_CB_START,		// start, create resource here
	OMX_VO_FILESINK_CB_WRITING,		// write file
	OMX_VO_FILESINK_CB_END,			// end, destroy resource here
	OMX_VO_FILESINK_CB_MAX = 0x7FFFFFFF
} OMX_VO_FILESINK_CB_FLAG;

/**
* File sink will callback the data out with some information
* \param pUserData [in] The user data which was set when initialized.
* \param nFlag [in] The callback flag, OMX_VO_FILESINK_CB_XXX
* \param nPostion [in] Valid only OMX_VO_FILESINK_CB_WRITING, The data position, absolute position in file.
* \param pBuffer [in] Valid only OMX_VO_FILESINK_CB_WRITING, The data content pointer.
* \param nSize [in] Valid only OMX_VO_FILESINK_CB_WRITING, The data content size.
* \param nStartTimeStamp [in] Valid only OMX_VO_FILESINK_CB_WRITING, The data start time(ms), -1 mean file sink also don't know time stamp information.
* \param nEndTimeStamp [in] Valid only OMX_VO_FILESINK_CB_WRITING, The data end time(ms), -1 mean file sink also don't know time stamp information.
* \retval	OMX_ErrorNone, Succeeded.
			OMX_ErrorInsufficientResources, Can't allocate resource when OMX_VO_FILESINK_CB_START
			OMX_ErrorNotReady, Can't finish writing when OMX_VO_FILESINK_CB_WRITING, need file sink retry
			OMX_ErrorUndefined, Unknown error
*/
typedef OMX_ERRORTYPE (OMX_APIENTRY * VOFILESINKCALLBACK) (OMX_PTR pUserData, OMX_VO_FILESINK_CB_FLAG nFlag, OMX_U32 nPosition, 
														   OMX_U8* pBuffer, OMX_U32 nSize, OMX_TICKS nStartTimeStamp, OMX_TICKS nEndTimeStamp);

typedef struct
{
	VOFILESINKCALLBACK	fCallback;		//callback function pointer
	OMX_PTR				pUserData;		//user data
} OMX_VO_FILESINK_CALLBACK;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __voOMX_Types_H__
