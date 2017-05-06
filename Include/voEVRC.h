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


#ifndef  __VOEVRC_H__
#define  __VOEVRC_H__

#include  "voAudio.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)

/* EVRC noise suppression code (on/off) */
typedef enum EVRC_Noise_Supp_tag{
    EVRC_NOISE_SUPP_OFF             = 0,                                    /*!< no noise suppression */
    EVRC_NOISE_SUPP_ON              = 1,                                    /*!< noise suppression is used */
}EVRC_NOISE_SUPP_T;

/* EVRC post filer code (on/off) */
typedef enum EVRC_Post_Filter_tag{
    EVRC_POST_FILER_OFF             = 0,                                    /*!< no post filter */
    EVRC_POST_FILER_ON              = 1,                                    /*!< post filer is used */
}EVRC_POST_FILTER_T;

/* EVRC rate. here excluding erasure and blank rates */
typedef enum EVRC_Rate_tag{
    EVRC_RATE_EIGHT                 = 1,                                    /*!< rate 1/8 */
    EVRC_RATE_HALFRATE              = 3,                                    /*!< rate 1/2 */
    EVRC_RATE_FULLRATE              = 4,                                    /*!< rate 1 */
}EVRC_RATE_T;

/* EVRC specific parameter id */

#define    VO_PID_EVRC_Module				0x42291000 
#define    VO_PID_EVRC_POSTFILTER                       (VO_PID_EVRC_Module | 0x0001)
#define    VO_PID_EVRC_FORMAT                           (VO_PID_EVRC_Module | 0x0002)
#define    VO_PID_EVRC_CHANNELS                         (VO_PID_EVRC_Module | 0x0003)
#define    VO_PID_EVRC_SAMPLERATE                       (VO_PID_EVRC_Module | 0x0004)
#define    VO_PID_EVRC_NOISESUPPT                       (VO_PID_EVRC_Module | 0x0005)
#define    VO_PID_EVRC_MAX_RATE                         (VO_PID_EVRC_Module | 0x0006)
#define    VO_PID_EVRC_MIN_RATE                         (VO_PID_EVRC_Module | 0x0007)
#define    VO_PID_EVRC_SET_RATE                         (VO_PID_EVRC_Module | 0x0008)

/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the EVRC Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetEVRCDecAPI(VO_AUDIO_CODECAPI * pDecHandle);


/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the EVRC Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetEVRCEncAPI(VO_AUDIO_CODECAPI * pEncHandle);


#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VOEVRC_H__

