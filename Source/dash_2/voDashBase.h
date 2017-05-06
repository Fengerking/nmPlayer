#ifndef _DASH_BASE_H_
#define _DASH_BASE_H_

#include "voType.h"



//Set param
#define VODASH_PID_DRM_CALLBACK														0x000001
#define VODASHPID_DRM_INFO																0x000002


#define VO_DASH_PID_AUDIO_TRACKINFO														0x000001
#define VO_DASH_PID_VIDEO_TRACKINFO														0x000002
/*
typedef struct
{
	VO_BOOL		isUsePiff;
	VO_BYTE		systemID[16];
	VO_U32		dataSize;
	VO_BYTE		*data;
}ProtectionHeader;*/

/**
* Frame position type. 
* identify the the data pos in frame.
*/
typedef enum
{
	VO_DASH_FRAME_POS_BEGIN		= 0x00000001,  /*!< the begin of frame data */
	VO_DASH_FRAME_POS_MID			= 0x00000002,  /*!< the mid of frame data */
	VO_DASH_FRAME_POS_END			= 0x00000003,  /*!< the end of frame data */
	VO_DASH_FRAME_POS_WHOLE		= 0x00000004   /*!< the whole of frame data */
}VO_DASH_FRAME_POS_TYPE;


/**
* Frame buffer structure for video or audio data
*/
typedef struct
{
	VO_PBYTE					pData;			/*!< the frame data pointer */
	VO_U32						nSize;			/*!< the frame data size */
	VO_U64						nStartTime;		/*!< the frame start time */
	VO_U64						nEndTime;		/*!< the frame end time */
	VO_U8							nFrameType;		/*!< the frame type, 0 key frame, others normal frame , refer to VO_VIDEO_FRAMETYPE*/
	VO_DASH_FRAME_POS_TYPE		nPos;			/*!< the frame position */
	VO_U32						nCodecType;		/*!< the frame codec type, refer to VO_VIDEO_CODINGTYPE, VO_AUDIO_CODINGTYPE */
	
	//PIFF
	VO_PBYTE pInitializationVector;
	VO_U32    nIVsize;
	VO_U32    nBlockOffset;
	VO_U8		btByteOffset;
}VO_DASH_FRAME_BUFFER;

#endif