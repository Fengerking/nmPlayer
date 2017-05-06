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


#ifndef __VO_PNG_H__
#define __VO_PNG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voImage.h>
#include <voVideo.h>
#include <viMem.h>
	
/**
 * PNG specific parameter id 
 * \see VOCOMMONPARAMETERID
 */
#define VO_PID_DEC_PNG_BASE            VO_PID_COMMON_BASE | VO_INDEX_DEC_PNG
typedef enum
{
	VO_PID_PNG_WIDTH			= VO_PID_DEC_PNG_BASE | 0x0001,
	VO_PID_PNG_HEIGHT			= VO_PID_DEC_PNG_BASE | 0x0002,
	VO_PID_PNG_SAMPLESPERPIXEL  = VO_PID_DEC_PNG_BASE | 0x0003,
	VO_PID_PNG_BITSPERSAMPLE	= VO_PID_DEC_PNG_BASE | 0x0004,
	VO_PID_PNG_ROWSPERSTRIP		= VO_PID_DEC_PNG_BASE | 0x0005,
	VO_PID_PNG_PHOTOMETRIC		= VO_PID_DEC_PNG_BASE | 0x0006,
	VO_PID_PNG_ORIENTATION		= VO_PID_DEC_PNG_BASE | 0x0007,
	VO_PID_PNG_XRESOLUTION		= VO_PID_DEC_PNG_BASE | 0x0008,
	VO_PID_PNG_YRESOLUTION		= VO_PID_DEC_PNG_BASE | 0x0009,
	VO_PID_PNG_XPOSITION		= VO_PID_DEC_PNG_BASE | 0x000a,
	VO_PID_PNG_YPOSITION		= VO_PID_DEC_PNG_BASE | 0x000b,
	VO_PID_PNG_COMPRESSION		= VO_PID_DEC_PNG_BASE | 0x000c,
	VO_PID_PNG_INPUTBUFFER		= VO_PID_DEC_PNG_BASE | 0x000d,
	VO_PID_PNG_INPUTFILE		= VO_PID_DEC_PNG_BASE | 0x000e,
	VO_PID_PNG_INFO		= VO_PID_DEC_PNG_BASE | 0x000f,
	VO_PID_PNG_FILEPATH         = VO_PID_DEC_PNG_BASE | 0x0010,
	VO_PID_PNG_FRAMESNUM        = VO_PID_DEC_PNG_BASE | 0x0011,
	VO_PID_PNG_OUTPUTDATA       = VO_PID_DEC_PNG_BASE | 0x0012,
	VO_PID_PNG_SCANLINEWIDTH    = VO_PID_DEC_PNG_BASE | 0x0013,
	VO_PID_PNG_COLORTYPE        = VO_PID_DEC_PNG_BASE | 0x0014,
	VO_PID_PNG_STRIPSIZE        = VO_PID_DEC_PNG_BASE | 0x0015,
	VO_PID_PNG_OUTPUTROWS       = VO_PID_DEC_PNG_BASE | 0x0016,
	VO_PID_PNG_RESET            = VO_PID_DEC_PNG_BASE | 0x0017
}
VOPNGPARAMETERID;


/**
* TIFF Decoder specific return code 
* \see VOCOMMONRETURNCODE
*/
#define VO_ERR_DEC_PNG_BASE              VO_ERR_BASE | VO_INDEX_DEC_PNG
typedef enum
{
	VO_ERR_PNGDEC_INBUFFERPOINT_ERR		= VO_ERR_DEC_PNG_BASE | 0x0001,  /*!< PNG Decoder invalid input buffer address return code 1 */
	VO_ERR_PNGDEC_UNSUPPORT_FEATURE		= VO_ERR_DEC_PNG_BASE | 0x0002,  /*!< PNG Decoder unspport PNG feature, return code 2 */
	VO_ERR_PNGDEC_DECODE_HEADER_ERR		= VO_ERR_DEC_PNG_BASE | 0x0003,  /*!< PNG Decoder decode header error, return code 3 */
	VO_ERR_PNGDEC_DECODE_TAIL_ERR		= VO_ERR_DEC_PNG_BASE | 0x0004,  /*!< PNG Decoder decode frame error, return code 4 */
	VO_ERR_PNGDEC_DECODE_UNFINISHED		= VO_ERR_DEC_PNG_BASE | 0x0005,  /*!< PNG Decoder decode frame unfinshed, return code 5 */
	VO_ERR_PNGDEC_NO_EXIFMARKER			= VO_ERR_DEC_PNG_BASE | 0x0006,  /*!< PNG file no exif marker, return code 6 */
	VO_ERR_PNGDEC_NO_IDITEM				= VO_ERR_DEC_PNG_BASE | 0x0007,  /*!< PNG file no data for given ID  */
	VO_ERR_PNGDEC_OPENFILE_ERROR		= VO_ERR_DEC_PNG_BASE | 0x0008,  /*!< open PNG file error */
	VO_ERR_PNGDEC_NOTHUMPIC				= VO_ERR_DEC_PNG_BASE | 0x0009,  /*!< PNG file no thumb picture */
	VO_ERR_PNGDEC_UNKNOWN_ERR			= VO_ERR_DEC_PNG_BASE | 0x000a,  /*!< MPNG Decoder un known error, return code FF */
	VO_ERR_PNGDEC_INVALID_ERR			= VO_ERR_DEC_PNG_BASE | 0x000b,
	VO_ERR_PNGDEC_MAX                          = VO_MAX_ENUM_VALUE
}
VOPNGDECRETURNCODE;

/**
 * Get image decorder API interface
 * \param pDecHandle [out] Return the PNG Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetPNGDecAPI (VO_IMAGE_DECAPI * pDecHandle, VO_U32 uFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VO_PNG_H__
