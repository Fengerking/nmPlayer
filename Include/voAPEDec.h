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


#ifndef  __VOAPE_H__
#define  __VOAPE_H__

#include  "voAudio.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)


#define    VO_PID_APE_Module			      0x42311000
 	
#define    VO_PID_APE_FORMAT                          (VO_PID_APE_Module | 0x0001)
#define    VO_PID_APE_CHANNELS                        (VO_PID_APE_Module | 0x0002)

/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the APE Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAPEDecAPI(VO_AUDIO_CODECAPI * pDecHandle);


/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the APE Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAPEEncAPI(VO_AUDIO_CODECAPI * pEncHandle);


#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VOAPE_H__


