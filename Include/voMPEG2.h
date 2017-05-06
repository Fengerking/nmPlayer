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


#ifndef __VO_MPEG2_DEC_H_
#define __VO_MPEG2_DEC_H_

#include <voVideo.h>
#include <viMem.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)


/**
 * MPEG2 Decoder specific return code 
 * \see VOCOMMONRETURNCODE
 */
#define VO_ERR_DEC_MPEG2_BASE                 VO_ERR_BASE | VO_INDEX_DEC_MPEG2
typedef enum
{
	VO_ERR_DEC_MPEG2_HEADER_ERR		        = VO_ERR_DEC_MPEG2_BASE | 0x0001,  /*!< MPEG2 Decoder specific return code 1 */
	VO_ERR_DEC_MPEG2_I_FRAME_ERR		    = VO_ERR_DEC_MPEG2_BASE | 0x0002,  /*!< MPEG2 Decoder specific return code 2 */
	VO_ERR_DEC_MPEG2_P_FRAME_ERR		    = VO_ERR_DEC_MPEG2_BASE | 0x0003,  /*!< MPEG2 Decoder specific return code 3 */
	VO_ERR_DEC_MPEG2_B_FRAME_ERR		    = VO_ERR_DEC_MPEG2_BASE | 0x0004,  /*!< MPEG2 Decoder specific return code 4 */
	VO_ERR_DEC_MPEG2_S_FRAME_ERR		    = VO_ERR_DEC_MPEG2_BASE | 0x0005,  /*!< MPEG2 Decoder specific return code 5 */
	VO_ERR_DEC_MPEG2_INTRA_MB_ERR		    = VO_ERR_DEC_MPEG2_BASE | 0x0006,  /*!< MPEG2 Decoder specific return code 6 */
	VO_ERR_DEC_MPEG2_INTER_MB_ERR		    = VO_ERR_DEC_MPEG2_BASE | 0x0007,  /*!< MPEG2 Decoder specific return code 7 */
	VO_ERR_DEC_MPEG2_NOTSUPPORT_FIELD_ERR	= VO_ERR_DEC_MPEG2_BASE | 0x0008,  /*!< MPEG2 Decoder specific return code 8 */
	VO_ERR_DEC_MPEG2_MAX                    = VO_MAX_ENUM_VALUE

}
VOMPEG2DECRETURNCODE;

#define VO_PID_DEC_MPEG2_BASE              VO_PID_COMMON_BASE | VO_INDEX_DEC_MPEG2
typedef enum
{
	/* for decoder */
	VO_PID_DEC_MPEG2_GETFIRSTFRAME		    = VO_PID_DEC_MPEG2_BASE | 0x0001,  /*!<it will force the decoder to output the first frame immediately,the parameter is a Boolean(LONG integer)*/
	VO_PID_DEC_MPEG2_MB_SKIP				= VO_PID_DEC_MPEG2_BASE | 0x0002,  /*!<Get mb skip infomation, the parameer is point of unsighed char(size = mb_width * mb_height)*/
	VO_PID_DEC_MPEG2_GETLASTOUTVIDEOBUFFER  = VO_PID_DEC_MPEG2_BASE | 0x0003,  /*!<it will force the decoder to output the last frame video buffer immediately,the parameter is a Boolean(LONG integer)*/
	VO_PID_DEC_MPEG2_GETLASTOUTVIDEOFORMAT  = VO_PID_DEC_MPEG2_BASE | 0x0004,  /*!<it will force the decoder to output the last frame video formant immediately,the parameter is a Boolean(LONG integer)*/
	VO_PID_DEC_MPEG2_DOWMSAMPLE			    = VO_PID_DEC_MPEG2_BASE | 0x000c,  /*!<it will decoder to downsample image*/
	VO_PID_DEC_MPEG2_ASPECT_RATIO			= VO_PID_DEC_MPEG2_BASE | 0x000d,  /*!<it will give aspect ratio information*/
	VO_PID_DEC_MPEG2_GET_VIDEOFORMAT        = VO_PID_DEC_MPEG2_BASE | 0x000e,  /*!<it will give video format information*/
	VO_PID_DEC_MPEG2_GET_VIDEO_WIDTH        = VO_PID_DEC_MPEG2_BASE | 0x000f,  /*!<Get video with*/    
	VO_PID_DEC_MPEG2_GET_VIDEO_HEIGHT       = VO_PID_DEC_MPEG2_BASE | 0x0010,  /*!<Get video height*/
	VO_PID_DEC_MPEG2_FLUSH                  = VO_PID_DEC_MPEG2_BASE | 0x0011,  /*!< Notify the decoder to flush all the reserved frames in buffer,the params is a int */
	VO_PID_DEC_MPEG2_GET_FRAME_RATE         = VO_PID_DEC_MPEG2_BASE | 0x0012,  /*!<Get video frame rate code */
	VO_PID_DEC_MPEG2_GET_PROGRESSIVE        = VO_PID_DEC_MPEG2_BASE | 0x0013,  /*!<Get video is interlaced or progressive.0 is interlaced and 1 is progressive */

	/* for encoder */
	VO_PID_ENC_MPEG2_BITRATE       		    = VO_PID_DEC_MPEG2_BASE | 0x0004,  /*!<Set bit rate, the parameter is LONG integer*/
	VO_PID_ENC_MPEG2_FRAMERATE       		= VO_PID_DEC_MPEG2_BASE | 0x0005,  /*!<Set frame rate, the parameter is float*/
	VO_PID_ENC_MPEG2_KEY_FRAME_INTERVAL  	= VO_PID_DEC_MPEG2_BASE | 0x0006,  /*!<Set key frame interval, the parameter is LONG integer*/
	VO_PID_ENC_MPEG2_VIDEO_QUALITY       	= VO_PID_DEC_MPEG2_BASE | 0x0007,  /*!<Set encoder video quality, the parameter is VOVIDEOENCQUALITY*/	
	VO_PID_ENC_MPEG2_DO_POSTPROCESS     	= VO_PID_DEC_MPEG2_BASE | 0x0008,  /*!<Set to do postprocessthe, parameter is Boolean(LONG integer)*/
	VO_PID_ENC_MPEG2_FORCE_KEY_FRAME     	= VO_PID_DEC_MPEG2_BASE | 0x0009,  /*!<Force currenct frame coded as key frame, parameter is Boolean(LONG integer)*/
	VO_PID_ENC_MPEG2_OUTBUF_SIZE     		= VO_PID_DEC_MPEG2_BASE | 0x000a,  /*!<Return the max out buffer size needed by encoder, the parameter is LONG integer */
	VO_PID_ENC_MPEG2_INPUT_ROTATION		    = VO_PID_DEC_MPEG2_BASE | 0x000b,  /*!<Set input video rotation type*/
	VO_PID_ENC_MPEG2_MAX                    = VO_MAX_ENUM_VALUE
}
VOMPEG2PARAMETERID;

#define IRDETO_DRM (VO_PID_COMMON_BASE | 0X0a01)

#define IDCT_DRM   (VO_PID_COMMON_BASE | 0X0b01)

typedef struct DRMclientData {
	/* fixed part */
	VO_S32 vendorID;  /* Irdeto, Authentec, NDS, etc. */
	VO_S32 vendorDRMtype; /* IDCT, MC, etc. */

	/* variable part, depending on vendorDRMtype */
	VO_HANDLE callback1;
	VO_HANDLE callback2;
	VO_HANDLE callback3;
	VO_HANDLE callback4;
	VO_HANDLE callback5;
	VO_HANDLE callback6;
	VO_HANDLE callback7;
	VO_HANDLE callback8;

	VO_HANDLE metadata1;
	VO_HANDLE metadata2;
	VO_HANDLE metadata3;
	VO_HANDLE metadata4;
	VO_HANDLE metadata5;
	VO_HANDLE metadata6;
	VO_HANDLE metadata7;
	VO_HANDLE metadata8;
} DRMclientData;


/**
* Get video decoder API interface
* \param pDecHandle [IN/OUT] Return the MPEG4 Decoder API handle.
* \param uFlag,reserved
* \retval VO_ERR_OK Succeeded.
*/
VO_S32 VO_API voGetMPEG2DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);



#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_MPEG2_DEC_H_ */



