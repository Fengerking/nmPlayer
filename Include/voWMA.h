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



#ifndef __voWMA_H__
#define __voWMA_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//#include <voLog.h>
#include <voAudio.h>

/* WMA Param ID */
#define VO_PID_WMA_Mdoule				0x42231000 
#define VO_PID_WMA_SUPPTHISMPLRT		(VO_PID_WMA_Mdoule | 0x0001)	/*!< WMA Parameter ID for setting to support high sample rate */
#define VO_PID_WMA_SUPPORT24BIT			(VO_PID_WMA_Mdoule | 0x0002)	/*!< WMA Parameter ID for setting to support 24 bits */
#define VO_PID_WMA_SUPPTMTCHANL			(VO_PID_WMA_Mdoule | 0x0003)	/*!< WMA Parameter ID for setting to support multi-channel */
#define VO_PID_WMA_OUTBUFFERSIZE		(VO_PID_WMA_Mdoule | 0x0004)	/*!< WMA Parameter ID for getting out buffer size */
#define VO_PID_WMA_FRAMELENGTH			(VO_PID_WMA_Mdoule | 0x0005)	/*!< WMA Parameter ID for getting max fame length */


/* WMA decoder error ID */
#define VO_ERR_WMA_Mdoule				0x82230000
#define VO_ERR_WMA_NOTSUPPORT			(VO_ERR_WMA_Mdoule | 0x0001)	/*!< WMA Decoder initialization parameter is not supported */
#define VO_ERR_WMA_INSIZENOTnBLOCKALIGN	(VO_ERR_WMA_Mdoule | 0x0002)	/*!< WMA input size need equale to nBlockAlign or n * nBlockAlign*/

/**
 * Get WMA deoder API interface
 * \param pDecHandle [out] Return the WMA Decoder handle.
 * \retval VO_ERR_NONE Succeeded.
 */
VO_S32 VO_API voGetWMADecAPI (VO_AUDIO_CODECAPI * pDecHandle);

/**
 * Get WMA encoder API interface
 * \param pEncHandle [out] Return the WMA Encoder handle.
 * \retval VO_ERR_NONE Succeeded.
 */
//VO_S32 VO_API voGetWMAEncAPI (VO_AUDIO_CODECAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voWMA_H__



