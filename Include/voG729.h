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


#ifndef  __VOG729_H__
#define  __VOG729_H__

#include  "voAudio.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifndef _LINUX
#pragma pack(push, 4)
#endif

/* the bitrate that the codec supports*/
typedef enum { 
        VOG729_Full        = 0,                               //G729A has only bit rate type
        VOG729_MAX         = VO_MAX_ENUM_VALUE
}VOG729MODE;

/* the frame type that the Codec supports*/
typedef enum {
        VOG729RAW_DATA     = 0,                                //G729 Encode Raw Data, one frame 10Bytes
        VOG729PACK_DATA    = 1,                                //G729 Encode Pack Data to G729AB Decode, add sync_word and bitrate	
        VOG729_TMAX        = VO_MAX_ENUM_VALUE
}VOG729FRAMETYPE;


/* G729 specific parameter id */
#define VO_PID_G729_Module	   0x42301000

#define VO_PID_G729_FRAMETYPE	   VO_PID_G729_Module | 0x0001 
#define VO_PID_G729_MODE	   VO_PID_G729_Module | 0x0002 
#define VO_PID_G729_FORMAT         VO_PID_G729_Module | 0x0004
#define VO_PID_G729_CHANNELS       VO_PID_G729_Module | 0x0005
#define VO_PID_G729_SAMPLERATE     VO_PID_G729_Module | 0x0006


/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the G729 Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetG729DecAPI(VO_AUDIO_CODECAPI * pDecHandle);


/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the G729 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetG729EncAPI(VO_AUDIO_CODECAPI * pEncHandle);

#ifndef _LINUX
#pragma pack(pop)
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VOG729_H__


