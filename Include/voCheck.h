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


#ifndef __voCheck_H__
#define __voCheck_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voType.h"
#include "voString.h"

#include "voSource.h"
#include "voAudio.h"
#include "voVideo.h"

#define		VOLICENSE_SET_PATH	20050422
#define		VOLICENSE_SET_TEXT	20050309
#define		VOLICENSE_SET_DATA	20050505
#define		VOLICENSE_SET_VOLOG	20010805
	

#define VO_ERR_LCS_LACK_DATA            (VO_ERR_LICENSE_ERROR|0x01)  // lack the path that locates the configure file or configure file data
#define VO_ERR_LCS_LACK_KEY             (VO_ERR_LICENSE_ERROR|0x02)  // lack the key that 
#define VO_ERR_LCS_UNAUTHORIZED         (VO_ERR_LICENSE_ERROR|0x03)  // the module does not authorized
#define VO_ERR_LCS_AUTHORIZED_EXPIRED   (VO_ERR_LICENSE_ERROR|0x04)  // the authorized has been expired 
#define VO_ERR_LCS_OS_UNMATCH           (VO_ERR_LICENSE_ERROR|0x05)  // the license data does not match the platform

#define VO_LCS_LIBOP_FLAG                0x0010
#define VO_LCS_WORKPATH_FLAG             0x1000
	
// re define the api name
#ifndef voCheckLibCheckAudio
#define voCheckLibCheckAudio DcAFNgUjsmcyhBWsrzESmeA /*-P-*/
#endif
#ifndef voCheckLibCheckImage
#define voCheckLibCheckImage CfiYpmPlVSfjfGmITmouoXA /*-P-*/
#endif
#ifndef voCheckLibCheckVideo
#define voCheckLibCheckVideo HogAZKvyHjoHGdcNJBFlhG /*-P-*/
#endif
#ifndef voCheckLibInit
#define voCheckLibInit CWpboQKCZfVYfvlkYOpBbwa /*-P-*/
#endif
#ifndef voCheckLibReadAudio
#define voCheckLibReadAudio OQeZWXmERrvfafUuehzNwp /*-P-*/
#endif
#ifndef voCheckLibReadVideo
#define voCheckLibReadVideo GACDWYGJvDfvrwxGUqrRSBD /*-P-*/
#endif
#ifndef voCheckLibRenderVideo
#define voCheckLibRenderVideo EzqyEamHvGwHgsCczqZsnNR /*-P-*/
#endif
#ifndef voCheckLibResetVideo
#define voCheckLibResetVideo ELWZflsMZUCfkIojVuMHfYt /*-P-*/
#endif
#ifndef voCheckLibUninit
#define voCheckLibUninit FmlOnDeZkAVXabwPtmKdcwB /*-P-*/
#endif

/**
 * Init the check static library
 * \param phCheck [out] Return the check handle.
 * \param nID [in] The codec id. It was defined in voIndex.h
 * \param nFlag [in] 0 is video and 1 is audio, 2 Image  
 * \0x0010 for pValue refers VO_LIB_OPERATOR*
 * \0x0100 for Thumbnail or Video, WARNING if you set the flag, our licence check NOT application the calling moudle. It just for get the thumbnail
 * \0x1000 for the pValue refers to VO_PTCHAR, It refers to work path
 * \param hInst [in] The instance handle.
 * \retval VO_ERR_NONE Succeeded.
 */
//VO_U32 VO_API voCheckLibInit (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst, VO_LIB_OPERATOR * pLibOP);
	
VO_U32 VO_API voCheckLibInit (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst, VO_PTR  pValue);

/**
 * Check the video buffer before output.
 * \param hCheck [in] Return by .voCheckLibInit
 * \param pSample [in] The output video buffer
 * \retval VO_ERR_NONE Succeeded.
 */
VO_U32 VO_API voCheckLibReadVideo (VO_PTR hCheck, VO_SOURCE_SAMPLE * pSample);


/**
 * Check the audio buffer before output.
 * \param hCheck [in] Return by .voCheckLibInit
 * \param pSample [in] The output video buffer
 * \retval VO_ERR_NONE Succeeded.
 */
VO_U32 VO_API voCheckLibReadAudio (VO_PTR hCheck, VO_SOURCE_SAMPLE * pSample);


/**
 * Check the video buffer before output. Add the logo or not. Call this in GetOutputData function.
 * \param hCheck [in] Return by .voCheckLibInit
 * \param pOutBuffer [in] The output video buffer
 * \param pOutInfo [in] The output video info
 * \retval VO_ERR_NONE Succeeded.
 */
VO_U32 VO_API voCheckLibCheckVideo (VO_PTR hCheck, VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_FORMAT * pOutInfo);

/**
 * Reset the video buffer. Call this when reference the video buffer..
 * \param hCheck [in] Return by .voCheckLibInit
 * \param pOutBuffer [in] The output video buffer. If NULL, reset all video buffer.
 * \retval VO_ERR_NONE Succeeded.
 */
VO_U32 VO_API voCheckLibResetVideo (VO_PTR hCheck, VO_VIDEO_BUFFER * pOutBuffer);


/**
 * Check the audio buffer. Add the noise or not. Call this in GetOutputData function.
 * \param hCheck [in] Return by .voCheckLibInit
 * \param pOutBuffer [in] The output audio buffer
 * \param pOutInfo [in] The output audio format
 * \retval VO_ERR_NONE Succeeded.
 */
VO_U32 VO_API voCheckLibCheckAudio (VO_PTR hCheck, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_FORMAT * pFormat);


/**
 * Check the video buffer before render(CCRR).
 * \param hCheck [in] Return by .voCheckLibInit
 * \param pOutBuffer [in] The output video buffer
 * \retval VO_ERR_NONE Succeeded.
 */
VO_U32 VO_API voCheckLibRenderVideo (VO_PTR hCheck, VO_VIDEO_BUFFER * pOutBuffer);


/**
 * Check the image buffer before codec.
 * \param hCheck [in] Return by .voCheckLibInit
 * \param pOutBuffer [in] The image buffer
 * \param pFormat [in] The image format
 * \retval VO_ERR_NONE Succeeded.
 */
VO_U32 VO_API voCheckLibCheckImage (VO_PTR hCheck, VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_FORMAT * pFormat);


/**
 * Uninit the check library
 * \param hCheck [in] Return by .voCheckLibInit
 * \retval VO_ERR_NONE Succeeded.
 */
VO_U32 VO_API voCheckLibUninit (VO_PTR hCheck);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voCheck_H__
