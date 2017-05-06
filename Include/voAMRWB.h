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


#ifndef  __VOAMRWB_H__
#define  __VOAMRWB_H__

#include  "voAudio.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)

/*!* the bitrate that the decoder supports*/
typedef enum { 
	VOAMRWB_MDNONE		= -1,	/*!< Invalid mode */
	VOAMRWB_MD66		= 0,	/*!< 6.60kbps   */
	VOAMRWB_MD885		= 1,    /*!< 8.85kbps   */       
	VOAMRWB_MD1265		= 2,	/*!< 12.65kbps  */
	VOAMRWB_MD1425		= 3,	/*!< 14.25kbps  */
	VOAMRWB_MD1585		= 4,	/*!< 15.85bps   */
	VOAMRWB_MD1825		= 5,	/*!< 18.25bps   */
	VOAMRWB_MD1985		= 6,	/*!< 19.85kbps  */
	VOAMRWB_MD2305		= 7,    /*!< 23.05kbps  */
	VOAMRWB_MD2385          = 8,    /*!< 23.85kbps> */	
	VOAMRWB_MDDTX		= 9,	/*!< DTX mode   */
	VOAMRWB_N_MODES 	= 10,	/*!< Invalid mode */
	VOAMRWB_MODE_MAX        = VO_MAX_ENUM_VALUE
	
}VOAMRWBMODE;

/*!* the frame type that the decoder supports*/
typedef enum {
	VOAMRWB_DEFAULT  	= 0,	/*!< the frame type is the head (defined in RFC3267) + rawdata*/
	/*One word (2-byte) for sync word (0x6b21)*/
	/*One word (2-byte) for frame length N.*/
	/*N words (2-byte) containing N bits (bit 0 = 0x007f, bit 1 = 0x0081).*/
	VOAMRWB_ITU             = 1, 
	/*One word (2-byte) to indicate type of frame type.*/
	/*One word (2-byte) to indicate frame type.*/	
	/*One word (2-byte) to indicate mode.*/
	/*N words (2-byte) containing N bits (bit 0 = 0xff81, bit 1 = 0x007f).*/
	VOAMRWB_RFC3267		= 2,  
        VOAMRWB_TMAX            = VO_MAX_ENUM_VALUE	
}VOAMRWBFRAMETYPE;


#define    VO_PID_AMRWB_Module				0x42261000 
#define    VO_PID_AMRWB_FORMAT                          (VO_PID_AMRWB_Module | 0x0002)
#define    VO_PID_AMRWB_CHANNELS                        (VO_PID_AMRWB_Module | 0x0003)
#define    VO_PID_AMRWB_SAMPLERATE                      (VO_PID_AMRWB_Module | 0x0004)
#define    VO_PID_AMRWB_FRAMETYPE                       (VO_PID_AMRWB_Module | 0x0005)
#define    VO_PID_AMRWB_MODE                            (VO_PID_AMRWB_Module | 0x0006)
#define    VO_PID_AMRWB_DTX                             (VO_PID_AMRWB_Module | 0x0007)

/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the AMRWB Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAMRWBDecAPI(VO_AUDIO_CODECAPI * pDecHandle);


/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the AMRWB Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAMRWBEncAPI(VO_AUDIO_CODECAPI * pEncHandle);


#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VOAMRWB_H__

