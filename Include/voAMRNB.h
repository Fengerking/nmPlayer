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


#ifndef  __VOAMRNB_H__
#define  __VOAMRNB_H__

#include  "voAudio.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifndef _LINUX
#pragma pack(push, 4)
#endif
/* the bitrate that the decoder supports*/
typedef enum { 
	VOAMRNB_MDNONE		= -1,	/*!< Invalid mode */
	VOAMRNB_MD475		= 0,	/*!< 475kbps */
	VOAMRNB_MD515		= 1,    /*!< 515kbps */       
	VOAMRNB_MD59		= 2,	/*!< 590kbps */
	VOAMRNB_MD67		= 3,	/*!< 670kbps */
	VOAMRNB_MD74		= 4,	/*!< 740kbps */
	VOAMRNB_MD795		= 5,	/*!< 795kbps */
	VOAMRNB_MD102		= 6,	/*!< 10.2kbps */
	VOAMRNB_MD122		= 7,    /*!< 12.2kbps */	        
	VOAMRNB_MDDTX		= 8,	/*!< DTX mode */
	VOAMRNB_N_MODES 	= 9,	/*!< Invalid mode */
    VOAMRNB_MAX         = VO_MAX_ENUM_VALUE
}VOAMRNBMODE;

/* the frame type that the decoder supports*/
typedef enum {
	VOAMRNB_RFC3267 	= 0,	/*!< the frame type is the head (defined in RFC3267) + rawdata*/
	VOAMRNB_IF1		    = 2,    /*!< the frame type is the IF1 head + rawdata*/        
	VOAMRNB_IF2		    = 3,	/*!< the frame type is the IF2 head + rawdata*/
    VOAMRNB_TMAX        = VO_MAX_ENUM_VALUE
}VOAMRNBFRAMETYPE;


/* AMRNB specific parameter id */
#define VO_PID_AMRNB_Module	   0x42251000

#define VO_PID_AMRNB_FRAMETYPE	   VO_PID_AMRNB_Module | 0x0001 /*!< the type of frame,the default value is VOI_RFC3267, the parameter is a LONG integer */
#define VO_PID_AMRNB_MODE	   VO_PID_AMRNB_Module | 0x0002 /*!< the mode of frame,the default value is VOAMRNB_MD122, the parameter is a LONG integer */
#define VO_PID_AMRNB_FORMAT        VO_PID_AMRNB_Module | 0x0004
#define VO_PID_AMRNB_CHANNELS      VO_PID_AMRNB_Module | 0x0005
#define VO_PID_AMRNB_SAMPLERATE    VO_PID_AMRNB_Module | 0x0006
#define VO_PID_AMRNB_DTX           VO_PID_AMRNB_Module | 0x0007



/* AMRNB decoder error ID */

/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the ARMNB Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAMRNBDecAPI(VO_AUDIO_CODECAPI * pDecHandle);


/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the AMRNB Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAMRNBEncAPI(VO_AUDIO_CODECAPI * pEncHandle);

#ifndef _LINUX
#pragma pack(pop)
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VOAMRNB_H__

