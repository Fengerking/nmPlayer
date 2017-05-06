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


#ifndef __voRESAMPLE_H__
#define __voRESAMPLE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voAudio.h>

typedef enum {
	VO_RESAMPLE_LOW_LEVEL		= 0,	
	VO_RESAMPLE_NORMAL_LEVEL	= 1,	
	VO_RESAMPLE_HIGH_LEVEL		= 2,	
	VO_RESAMPLE_LEVEL_MAX		= VO_MAX_ENUM_VALUE
} VO_RESAMPLE_LEVEL;


#define VO_INDEX_RESAMPLE		0x0AAA0000

#define VO_PID_RESAMPLE_Mdoule	0x4AAA1000

#define VO_PID_RESAMPLE_INRATE 			VO_PID_RESAMPLE_Mdoule | 0x0001  
#define VO_PID_RESAMPLE_OUTRATE 		VO_PID_RESAMPLE_Mdoule | 0x0002  
#define VO_PID_RESAMPLE_LEVEL 			VO_PID_RESAMPLE_Mdoule | 0x0003 
#define VO_PID_RESAMPLE_SELECTCHS		VO_PID_RESAMPLE_Mdoule | 0x0004
#define VO_PID_RESAMPLE_INCHAN			VO_PID_RESAMPLE_Mdoule | 0x0005
#define VO_PID_RESAMPLE_OUTCHAN			VO_PID_RESAMPLE_Mdoule | 0x0006
#define VO_PID_RESAMPLE_CHMAPPING		VO_PID_RESAMPLE_Mdoule | 0x0007

/**
 * Get Resample API interface
 * \param pResHandle [out] Return the Resample handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetResampleAPI (VO_AUDIO_CODECAPI * pResHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voAAC_H__
