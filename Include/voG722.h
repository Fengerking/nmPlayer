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


#ifndef __VO_G722_H__
#define __VO_G722_H__

#include "voAudio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifndef __LINUX
#pragma pack(push, 4)
#endif


/* the bitrate that the decoder supports */
typedef enum {
    VOG722_MDNONE		= -1,	/*!< Invalid mode */
    VOG722_MD24			= 0,	/*!< 24kbps   */
	VOG722_MD32			= 1,    /*!< 32kbps   */ 
	VOG722_MD48         = 2,    /*!< 48kbps     */	
}VOG722MODE;

/* the frame type that the decoder supports */
typedef enum {
	VOG722_DEFAULT  	= 0,	/*!< packed the bitstream   */
	VOG722_ITU          = 1,    /*!< unpacked the bitstream */
}VOG722FRAMETYPE;

typedef enum {
        VOG722_7K       = 0,   /*!< Samples rate 7KHz, support 24kbit/s and 32kbit/s*/
        VOG722_14K      = 1,   /*!< Samples rate 14KHz, support 24kbit/s¡¢32kbit/s and 48kbit/s*/
}VOG722SAMPLERATE;


/* G722 specific parameter id see VOCOMMONPARAMETERID */
#define  VO_PID_G722_Module          0x42351000
/*!< the type of frame,the default value is VOI_RFC3267, the parameter is a LONG integer */
#define  VO_PID_G722_FRAMETYPE       (VO_PID_G722_Module | 0x0001) 

#define  VO_PID_G722_MODE            (VO_PID_G722_Module | 0x0002)
/*DTX Parameter ID*/
#define  VO_PID_G722_DTX             (VO_PID_G722_Module | 0x0003)
/* Flush ID */
#define  VO_PID_G722_FLUSH           (VO_PID_G722_Module | 0x0004)
/*alter 7KHz to 14KHz*/
#define  VO_PID_G722_MRATE           (VO_PID_G722_Module | 0x0005)


/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the G722 Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetG722DecAPI(VO_AUDIO_CODECAPI * pDecHandle);


/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the G722 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetG722EncAPI(VO_AUDIO_CODECAPI * pEncHandle);

#ifndef _LINUX
#pragma pack(pop)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __VO_G722__ */


