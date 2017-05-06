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


#ifndef  __VOAMRWBP_H__
#define  __VOAMRWBP_H__

#include  "voAudio.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define    VO_PID_AMRWBP_Module				0x42271000 
#define    VO_PID_AMRWBP_MONO_OUT           (VO_PID_AMRWBP_Module | 0x0001)
#define    VO_PID_AMRWBP_HEADER_METE        (VO_PID_AMRWBP_Module | 0x0002)
#define    VO_PID_AMRWBP_FRAME_LENS         (VO_PID_AMRWBP_Module | 0x0003)

/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the AMRWB++ Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAMRWBPDecAPI(VO_AUDIO_CODECAPI * pDecHandle);


/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the AMRWB++ Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAMRWBPEncAPI(VO_AUDIO_CODECAPI * pEncHandle);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VOAMRWBP_H__

