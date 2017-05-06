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


#ifndef __voEAC3_H__
#define __voEAC3_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voAudio.h>

/* eAC3 decode error ID */
#define VO_ERR_eAC3_Mdoule				0x82250000
#define VO_ERR_eAC3_INVFRAME			VO_ERR_RA_Mdoule | 0x0001

#define   VO_PID_EAC3_Module	        0x42331000 
#define   VO_PID_EAC3_DownMix_Mode		(VO_PID_EAC3_Module | 0x0001)  /*!< the header data that the decoder supports, the parameter is a point */	

/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the eAC3 Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetEAC3DecAPI (VO_AUDIO_CODECAPI * pDecHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voEAC3_H__
