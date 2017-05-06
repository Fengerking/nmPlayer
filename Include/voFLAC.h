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


#ifndef  __VOFLAC_H__
#define  __VOFLAC_H__

#include  "voAudio.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)


#define   VO_PID_FLAC_Module	        0x422D1000 
#define   VO_PID_FLAC_HEADER_PARAMETER  (VO_PID_FLAC_Module | 0x0001)  /*!< the header data that the decoder supports, the parameter is a point */	
#define   VO_PID_FLAC_GETCHANNEL	    (VO_PID_FLAC_Module | 0x0002)      /*!< get the channel, the parameter is a LONG point */
#define   VO_PID_FLAC_GETBLOCKSIZE      (VO_PID_FLAC_Module | 0x0004)      /*!< get the max blocksize, the parameter is a LONG point */
#define   VO_PID_FLAC_GET24BITS         (VO_PID_FLAC_Module | 0x0008)      /*!< get sample of 24bits length */
#define   VO_PID_FLAC_DVDDATA           (VO_PID_FLAC_Module | 0x0010)      /*!< get sample of 24bits length, dvd data format */

/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the FLAC Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetFLACDecAPI(VO_AUDIO_CODECAPI * pDecHandle);


/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the FLAC Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetFLACEncAPI(VO_AUDIO_CODECAPI * pEncHandle);


#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VOFLAC_H__

