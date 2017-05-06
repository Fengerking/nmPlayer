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


#ifndef __VO_G711_H__
#define __VO_G711_H__

#include "voAudio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifndef __LINUX
#pragma pack(push, 4)
#endif


/* the bitrate that the decoder supports */
typedef enum {
        VOG711_MDNONE		= -1,	/*! Invalid mode */
        VOG711_MDMU_LAW		= 0,	/*! u-law mode  */
	VOG711_MDA_LAW		= 1,    /*! a-law mode   */
	VOG711_ENUM_MAX         = VO_MAX_ENUM_VALUE
}VOG711MODE;



/* G711 specific parameter id see VOCOMMONPARAMETERID */
#define  VO_PID_G711_Module          0x42361000
#define  VO_PID_G711_MODE            (VO_PID_G711_Module | 0x0001)
/* Flush ID */
#define  VO_PID_G711_FLUSH           (VO_PID_G711_Module | 0x0002)

/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the G711 Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetG711DecAPI(VO_AUDIO_CODECAPI * pDecHandle);


/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the G711 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetG711EncAPI(VO_AUDIO_CODECAPI * pEncHandle);

#ifndef _LINUX
#pragma pack(pop)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __VO_G711__ */



