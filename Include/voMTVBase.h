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
#ifndef _MTV_BASE_H_
#define _MTV_BASE_H_

#include "voType.h"
#include "voVideo.h"
#include "voAudio.h"


/**
* Frame position type. 
* identify the the data pos in frame.
*/
typedef enum
{
	VO_MTV_FRAME_POS_BEGIN		= 0x00000001,  /*!< the begin of frame data */
	VO_MTV_FRAME_POS_MID		= 0x00000002,  /*!< the mid of frame data */
	VO_MTV_FRAME_POS_END		= 0x00000003,  /*!< the end of frame data */
	VO_MTV_FRAME_POS_WHOLE		= 0x00000004   /*!< the whole of frame data */
}VO_MTV_FRAME_POS_TYPE;


/**
* Frame buffer structure for video or audio data
*/
typedef struct
{
	VO_PBYTE					pData;			/*!< the frame data pointer */
	VO_U32						nSize;			/*!< the frame data size */
	VO_U64						nStartTime;		/*!< the frame start time */
	VO_U64						nEndTime;		/*!< the frame end time */
	VO_U8						nFrameType;		/*!< the frame type, 0 key frame, others normal frame , refer to VO_VIDEO_FRAMETYPE*/
	VO_MTV_FRAME_POS_TYPE		nPos;			/*!< the frame position */
	VO_U32						nCodecType;		/*!< the frame codec type, refer to VO_VIDEO_CODINGTYPE, VO_AUDIO_CODINGTYPE */
}VO_MTV_FRAME_BUFFER;


/**
* Date and Time
*/
typedef struct
{
	VO_U16 wYear;
	VO_U16 wMonth;
	VO_U16 wDayOfWeek;
	VO_U16 wDay;
	VO_U16 wHour;
	VO_U16 wMinute;
	VO_U16 wSecond;
	VO_U16 wMilliseconds;
} VODATETIME;

typedef enum
{
	MTV_PARSER_ERROR_CODE_NO_CONTINUITY = 0,
	MTV_PARSER_ERROR_CODE_PACKET_SIZE_ERROR

}MTV_PARSER_ERROR_CODE;

#endif