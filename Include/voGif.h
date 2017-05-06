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

#ifndef __VO_GIF_H__
#define __VO_GIF_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voImage.h>
#include <voVideo.h>
#include <viMem.h>
	
/**
 * GIFF specific parameter id 
 * \see VOCOMMONPARAMETERID
 */
#define VO_PID_DEC_GIF_BASE            VO_PID_COMMON_BASE | VO_INDEX_DEC_GIF
typedef enum
{
	VO_PID_GIF_WIDTH			= VO_PID_DEC_GIF_BASE | 0x0001,
	VO_PID_GIF_HEIGHT			= VO_PID_DEC_GIF_BASE | 0x0002,
	VO_PID_GIF_SAMPLESPERPIXEL  = VO_PID_DEC_GIF_BASE | 0x0003,
	VO_PID_GIF_BITSPERSAMPLE	= VO_PID_DEC_GIF_BASE | 0x0004,
	VO_PID_GIF_FRAMESNUM        = VO_PID_DEC_GIF_BASE | 0x0005,
	VO_PID_GIF_OUTPUTDATA       = VO_PID_DEC_GIF_BASE | 0x0006,
	VO_PID_GIF_SCANLINEWIDTH    = VO_PID_DEC_GIF_BASE | 0x0007,
	VO_PID_GIF_COLORTYPE        = VO_PID_DEC_GIF_BASE | 0x0008,
	VO_PID_GIF_FILEPATH         = VO_PID_DEC_GIF_BASE | 0x0009,
	VO_PID_GIF_DEVCLOR          = VO_PID_DEC_GIF_BASE | 0x000a
}
VOGIFFPARAMETERID;


/**
* GIF Decoder specific return code 
* \see VOCOMMONRETURNCODE
*/
#define VO_ERR_DEC_GIF_BASE              VO_ERR_BASE | VO_INDEX_DEC_GIF
typedef enum
{
	VO_ERR_GIFDEC_INBUFFERPOINT_ERR		= VO_ERR_DEC_GIF_BASE | 0x0001,  /*!< JPEG Decoder invalid input buffer address return code 1 */
	VO_ERR_GIFDEC_UNSUPPORT_FEATURE		= VO_ERR_DEC_GIF_BASE | 0x0002,  /*!< JPEG Decoder unspport JPEG feature, return code 2 */
	VO_ERR_GIFDEC_DECODE_HEADER_ERR		= VO_ERR_DEC_GIF_BASE | 0x0003,  /*!< JPEG Decoder decode header error, return code 3 */
	VO_ERR_GIFDEC_DECODE_TAIL_ERR		= VO_ERR_DEC_GIF_BASE | 0x0004,  /*!< JPEG Decoder decode frame error, return code 4 */
	VO_ERR_GIFDEC_DECODE_UNFINISHED		= VO_ERR_DEC_GIF_BASE | 0x0005,  /*!< JPEG Decoder decode frame unfinshed, return code 5 */
	VO_ERR_GIFDEC_NO_EXIFMARKER			= VO_ERR_DEC_GIF_BASE | 0x0006,  /*!< JPEG file no exif marker, return code 6 */
	VO_ERR_GIFDEC_NO_IDITEM				= VO_ERR_DEC_GIF_BASE | 0x0007,  /*!< JPEG file no data for given ID  */
	VO_ERR_GIFDEC_OPENFILE_ERROR		= VO_ERR_DEC_GIF_BASE | 0x0008,  /*!< open JPEG file error */
	VO_ERR_GIFDEC_NOTHUMPIC				= VO_ERR_DEC_GIF_BASE | 0x0009,  /*!< JPEG file no thumb picture */
	VO_ERR_GIFDEC_UNKNOWN_ERR			= VO_ERR_DEC_GIF_BASE | 0x000a,  /*!< MJPEG Decoder un known error, return code FF */
	VO_ERR_GIFDEC_INVALID_GIFF			= VO_ERR_DEC_GIF_BASE | 0x000b,
	VO_ERR_MAX                          = VO_MAX_ENUM_VALUE
}
VOGIFDECRETURNCODE;

/**
 * Get image decorder API interface
 * \param pDecHandle [out] Return the H264 Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetGIFDecAPI (VO_IMAGE_DECAPI * pDecHandle, VO_U32 uFlag);

/**
 * Get image encoder API interface
 * \param pEncHandle [out] Return the H264 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
//VO_S32 VO_API voGetGIFEncAPI (VO_IMAGE_ENCAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voJpeg_H__
