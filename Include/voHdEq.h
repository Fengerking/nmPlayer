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


#ifndef __VOHDEQ_H__
#define __VOHDEQ_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voAudio.h>
	
/* The sample rate of the PCM */

typedef enum
{
	sr_48000,     /* sample rate is 48k */
	sr_44100,     /* sample rate is 44.1k */
	sr_32000,     /* sample rate is 32k */
	sr_24000,     /* sample rate is 24k */
	sr_22050,     /* sample rate is 22.05k */
	sr_16000,     /* sample rate is 16k */
	sr_12000,     /* sample rate is 12k */
	sr_11025,     /* sample rate is 11k */
	sr_8000,      /* sample rate is  8k */
	EQ_SAMPLE_MAX = VO_MAX_ENUM_VALUE,
}EQ_SAMPLE_RATE;

/* The normal equalizer mode */

typedef enum
{
	NONE	= -1,   /* no mode */
	POP     = 0,    /* Pop Music */
	LIVE,		    /* Live Music */
	JAZZ,		    /* Jazz Music */
	ROCK,		    /* Rock */
	BASS,		    /* Bass */
	CLASS,              /* Classical */
	TREBLE,		    /* Treble */
	EQ_MODE_MAX = VO_MAX_ENUM_VALUE,
}EQ_EQUAL_MODE;

/* HDEQ Param ID */
#define VO_PID_HDEQ_MODULE				0x422A1200
#define VO_PID_HDEQ_SAMPLERATE			        (VO_PID_HDEQ_MODULE | 0x0001)
#define VO_PID_HDEQ_CHANNELNUM     		        (VO_PID_HDEQ_MODULE | 0x0002)		
#define VO_PID_HDEQ_MODE      		                (VO_PID_HDEQ_MODULE | 0x0003)


/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the ADPCM Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetHDEQAPI (VO_AUDIO_CODECAPI * pDecHandle);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__VOHDEQ_H__

