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

#ifndef __voImage_H__
#define __voImage_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIVCommon.h"
#include "voMem.h"

#include <voVideo.h>

/** 
 * Enumeration used to define the possible image compression coding. 
 */
typedef enum VO_IMAGE_CODINGTYPE {
    VO_IMAGE_CodingUnused,      /**< Value when format is N/A */
    VO_IMAGE_CodingJPEG,        /**< JPEG/JFIF image format */
    VO_IMAGE_CodingGIF,         /**< Graphics image format */
    VO_IMAGE_CodingPNG,         /**< PNG image format */
    VO_IMAGE_CodingTIFF,        /**< TIFF image format */
    VO_IMAGE_CodingBMP,         /**< Windows Bitmap format */
    VO_IMAGE_CodingJPEG2K,      /**< JPEG 2000 image format */
    VO_IMAGE_CodingEXIF,        /**< EXIF image format */
    VO_IMAGE_CodingLZW,         /**< LZW image format */
	VO_IMAGE_Coding_MAX	= VO_MAX_ENUM_VALUE
} VO_IMAGE_CODINGTYPE;

typedef struct
{
	/**
	 * Init the image decoder module and return decorder handle
	 * \param phDec [OUT] Return the video decoder handle
	 * \param vType	[IN] The image coding type if the module support multi codec.
	 * \param pUserData	[IN] The init param. It is memory operator or alloced memory
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Init) (VO_HANDLE * phDec,VO_IMAGE_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);

	/**
	 * Decoder the comprossed image data to raw image data
	 * \param hDec [IN]] The Dec Handle which was created by Init function.
	 * \param pInput [IN] The input buffer param.
	 * \param pOutput [OUT] The output buffer param.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Process) (VO_HANDLE hDec, VO_CODECBUFFER * pInput, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO *pOutFormat);

	/**
	 * Set the param for special target.
	 * \param hDec [IN]] The Dec Handle which was created by Init function.
	 * \param uParamID [IN] The param ID.
	 * \param pData [IN] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * SetParam) (VO_HANDLE hEnc, VO_S32 uParamID, VO_PTR pData);

	/**
	 * Get the param for special target.
	 * \param hDec [IN]] The Dec Handle which was created by Init function.
	 * \param uParamID [IN] The param ID.
	 * \param pData [OUT] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetParam) (VO_HANDLE hEnc, VO_S32 uParamID, VO_PTR pData);

	/**
	 * Uninit the decoder.
	 * \param hDec [IN]] The Dec Handle which was created by Init function.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Uninit) (VO_HANDLE hEnc);
} VO_IMAGE_DECAPI;

typedef struct
{
	/**
	 * Init the image encoder module and return encoder handle
	 * \param phEnc [OUT] Return the image encoder handle
	 * \param vType	[IN] The encoder type if the module support multi codec.
	 * \param pUserData	[IN] The init param. It is memory operator or alloced memory
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Init) (VO_HANDLE * phEnc,VO_IMAGE_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);

	/**
	 * Encode the raw image data.
	 * \param hEnc [IN]] The Dec Handle which was created by Init function.
	 * \param pInput [IN] The input buffer param.
	 * \param pOutput [Out] The compressed image data.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Process) (VO_HANDLE hDec, VO_VIDEO_BUFFER * pInput, VO_CODECBUFFER * pOutput,VO_VIDEO_OUTPUTINFO *pOutFormat);

	/**
	 * Set the param for special target.
	 * \param hEnc [IN]] The Dec Handle which was created by Init function.
	 * \param uParamID [IN] The param ID.
	 * \param pData [IN] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * SetParam) (VO_HANDLE hEnc, VO_S32 uParamID, VO_PTR pData);

	/**
	 * Get the param for special target.
	 * \param hEnc [IN]] The Dec Handle which was created by Init function.
	 * \param uParamID [IN] The param ID.
	 * \param pData [OUT] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetParam) (VO_HANDLE hEnc, VO_S32 uParamID, VO_PTR pData);

	/**
	 * Uninit the encoder.
	 * \param hEnc [IN]] The Dec Handle which was created by Init function.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Uninit) (VO_HANDLE hEnc);
} VO_IMAGE_ENCAPI;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voImage_H__
