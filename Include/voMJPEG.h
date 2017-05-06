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

   
#ifndef __VO_MJPEG_DEC_H_
#define __VO_MJPEG_DEC_H_

#include <voVideo.h>
#include <viMem.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)


/**
* MJPEG specific parameter id 
* \see VOCOMMONPARAMETERID
*/
#define VO_PID_DEC_MJPEG_BASE            VO_PID_COMMON_BASE | VO_INDEX_DEC_MJPEG
#define VO_PID_ENC_MJPEG_BASE            VO_PID_COMMON_BASE | VO_INDEX_ENC_MJPEG
typedef enum
{
	/* for encoder */
	VO_PID_DEC_MJPEG_QUANLITY           = VO_PID_DEC_MJPEG_BASE | 0x0001,  /*!< MJPEG specific parameter id 1,  
																		  the parameter is a LONG integer, 
																		  from 1 to 100, define the quliaty of MJPEG */
	/* for decoder */
	VO_PID_DEC_MJPEG_GET_VIDEO_WIDTH    = VO_PID_DEC_MJPEG_BASE | 0x0010,  /*!<Get video with*/    
	VO_PID_DEC_MJPEG_GET_VIDEO_HEIGHT   = VO_PID_DEC_MJPEG_BASE | 0x0011,  /*!<Get video height*/
	VO_PID_DEC_MJPEG_SET_NORMAL_CHROMA  = VO_PID_DEC_MJPEG_BASE | 0x0012,  /*!<Set normal chroma flage*/
    VO_PID_DEC_MJPEG_SET_BUFFERSWITCH  = VO_PID_DEC_MJPEG_BASE | 0x0013,
	VO_PID_DEC_MJPEG_MAX                = VO_MAX_ENUM_VALUE,
	VO_PID_ENC_MJPEG_QUANLITY               = VO_PID_ENC_MJPEG_BASE | 0x0001,
	VO_PID_ENC_MJPEG_WIDTH               = VO_PID_ENC_MJPEG_BASE | 0x0010,
	VO_PID_ENC_MJPEG_HEIGHT               = VO_PID_ENC_MJPEG_BASE | 0x0011,
	VO_PID_ENC_MJPEG_MAX               = VO_MAX_ENUM_VALUE
}
VOMJPEGPARAMETERID;


/**
 * MJPEG Decoder specific return code 
 * \see VOCOMMONRETURNCODE
 */
#define VO_ERR_DEC_MJPEG_BASE              VO_ERR_BASE | VO_INDEX_DEC_MJPEG
#define VO_ERR_ENC_MJPEG_BASE              VO_ERR_BASE | VO_INDEX_ENC_MJPEG
typedef enum
{
	VO_ERR_DEC_MJPEG_INBUFFERADR			= VO_ERR_DEC_MJPEG_BASE | 0x0001,  /*!< MJPEG Decoder invalid input buffer address return code 1 */
	VO_ERR_DEC_MJPEG_UNSUPPORT_FEATURE	    = VO_ERR_DEC_MJPEG_BASE | 0x0002,  /*!< MJPEG Decoder unspport JPEG feature, return code 2 */
	VO_ERR_DEC_MJPEG_DECODE_HEADER		    = VO_ERR_DEC_MJPEG_BASE | 0x0003,  /*!< MJPEG Decoder decode header err, return code 3 */
	VO_ERR_DEC_MJPEG_DECODE_TAIL			= VO_ERR_DEC_MJPEG_BASE | 0x0004,  /*!< MJPEG Decoder decode frame error, return code 4 */
	VO_ERR_DEC_MJPEG_INDATAERROE           = VO_ERR_DEC_MJPEG_BASE | 0x0005,	
	VO_ERR_DEC_MJPEG_UNKNOWN				= VO_ERR_DEC_MJPEG_BASE | 0x00FF,  /*!< MJPEG Decoder unknown err, return code FF */
	VO_ERR_DEC_MJPEG_MAX                    = VO_MAX_ENUM_VALUE
}
VOMJPEGDECRETURNCODE;

typedef enum
{
	VO_ERR_ENC_MJPEG_INBUFFERADR			= VO_ERR_ENC_MJPEG_BASE | 0x0001,  /*!< MJPEG Decoder invalid input buffer address return code 1 */
	VO_ERR_ENC_MJPEG_WIDTH_INVALID			= VO_ERR_ENC_MJPEG_BASE | 0x0002,  /*!<  MJPEG Encoder encode width invalid, return code 2 */
	VO_ERR_ENC_MJPEG_HEIGHT_INVALID			= VO_ERR_ENC_MJPEG_BASE | 0x0003,  /*!<  MJPEG Encoder encode height invalid, return code 3 */
	VO_ERR_ENC_MJPEG_UNSUPPORT_COLOR	    = VO_ERR_ENC_MJPEG_BASE | 0x0004,  /*!< MJPEG Decoder unspport JPEG feature, return code 2 */
	VO_ERR_ENC_MJPEG_ENCODE_HEADER		    = VO_ERR_ENC_MJPEG_BASE | 0x0005,  /*!< MJPEG Decoder decode header err, return code 3 */
	VO_ERR_ENC_MJPEG_ENCODE_TAIL			= VO_ERR_ENC_MJPEG_BASE | 0x0006,  /*!< MJPEG Decoder decode frame error, return code 4 */
	VO_ERR_ENC_MJPEG_ENCUNFINISHED			= VO_ERR_ENC_MJPEG_BASE | 0x0007,  /*!< MJPEG Encoder encode unfinished, should continue to call function */
	VO_ERR_ENC_MJPEG_UNKNOWN				= VO_ERR_ENC_MJPEG_BASE | 0x00F8,  /*!< MJPEG Decoder unknown err, return code FF */
	VO_ERR_ENC_MJPEG_MAX                    = VO_MAX_ENUM_VALUE
}
VOMJPEGENCRETURNCODE;
/**
* Get video decoder API interface
* \param pDecHandle [IN/OUT] Return the MJPEG Decoder API handle.
* \param uFlag,reserved
* \retval VO_ERR_OK Succeeded.
*/
VO_S32 VO_API voGetMJPEGDecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

/**
* Get video encoder API interface
* \param pEncHandle [IN/OUT] Return the MJPEG Encoder API handle.
* \param uFlag,reserved
* \retval VO_ERR_OK Succeeded.
*/
VO_S32 VO_API voGetMJPEGEncAPI (VO_VIDEO_ENCAPI * pDecHandle);
#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_MJPEG_DEC_H_ */



