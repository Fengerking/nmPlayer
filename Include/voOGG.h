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


#ifndef _voOGG_H_
#define _voOGG_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voAudio.h>

/* OGG Param ID */
#define VO_PID_OGG_Mdoule				0x422f1000 
#define VO_PID_OGG_FORMATFILE			(VO_PID_OGG_Mdoule | 0x0001)	/* OGG Parameter ID for setting Vorbis file stream */
#define VO_PID_OGG_OUTBUFFERSIZE		(VO_PID_OGG_Mdoule | 0x0002)	/* OGG Parameter ID for getting out buffer size */
#define VO_PID_OGG_FRAMELENGTH			(VO_PID_OGG_Mdoule | 0x0003)	/* OGG Parameter ID for getting max fame length */

/* OGG decoder error ID */
#define VO_ERR_OGG_Mdoule				0x822f0000
	

typedef struct VORBIS_AUDIOFORMAT
{
	int Channels;
	int SamplesPerSec;
	int BitsPerSample;
	int HeadPacketSize[3];
}VORBIS_AUDIOFORMAT;

	/**
 * Get OGG deoder API interface
 * \param pDecHandle [out] Return the OGG Decoder handle.
 * \retval VO_ERR_NONE Succeeded.
 */
VO_S32 VO_API voGetOGGDecAPI (VO_AUDIO_CODECAPI * pDecHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	//_voOGG_H_
