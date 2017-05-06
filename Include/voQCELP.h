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



#ifndef  __VOQCELP_H__
#define  __VOQCELP_H__

#include  "voAudio.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifndef _LINUX
#pragma pack(push, 4)
#endif
/* the bitrate that the decoder supports*/
typedef enum {
	VO_BLANK	         = 0,
	VO_EIGHT                 = 1,
	VO_QUARTERATE_UNVOICED   = 2,
	VO_HALFRATE_VOICED       = 3,
	VO_FULLRATE_VOICED       = 4,
} VOQCPRATEMODE;

#define    VO_PID_QCELP_Module				0x42281000 
#define    VO_PID_QCELP_FORMAT                          (VO_PID_QCELP_Module | 0x0001)
#define    VO_PID_QCELP_CHANNELS                        (VO_PID_QCELP_Module | 0x0002)
#define    VO_PID_QCELP_SAMPLERATE                      (VO_PID_QCELP_Module | 0x0003)
#define    VO_PID_QCELP_RDA                             (VO_PID_QCELP_Module | 0x0004)
/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the QCELP Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetQCELPDecAPI(VO_AUDIO_CODECAPI * pDecHandle);


/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the QCELP Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetQCELPEncAPI(VO_AUDIO_CODECAPI * pEncHandle);

#ifndef _LINUX
#pragma pack(pop)
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VOQCELP_H__

