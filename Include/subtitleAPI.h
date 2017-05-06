/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifndef __subtitleAPI_H__
#define __subtitleAPI_H__

#include "voType.h"
#include "voString.h"
#include "voIVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#ifndef SUBTITLE_APIENTRY
#define SUBTITLE_APIENTRY 
#endif 
#ifndef	SUBTITLE_API
#define SUBTITLE_API
#endif


typedef enum {
	SUBTITLE_TRACK_TYPE_UNKNOWN  = 0,   /*!< Unknown track */
	SUBTITLE_TRACK_TYPE_TEXT     = 1,   /*!< Text Format   */
	SUBTITLE_TRACK_TYPE_BITMAP   = 2	/*!< Bitmap format */
} Subtitle_Track_Type;

typedef enum {
	SUBTITLE_LANGUAGE_UNKNOWN  = 0,   /*!< Unknown Language */
	SUBTITLE_LANGUAGE_ENGLISH  = 1,   /*!< English  */
	SUBTITLE_LANGUAGE_SCHINESE = 2,   /*!< Simplified Chinese   */
	SUBTITLE_LANGUAGE_TCHINESE = 3,   /*!< Traditional Chinese   */
	SUBTITLE_LANGUAGE_KOREAN   = 4,   /*!< Korean   */
	SUBTITLE_LANGUAGE_JAPANESE = 5,   /*!< Japanese */	
	SUBTITLE_LANGUAGE_SPANISH = 6,    /*!<Spanish> */
	SUBTITLE_LANGUAGE_GERMAN = 7,     /*!<German>*/
	SUBTITLE_LANGUAGE_FRENCH = 8,		/*!<French>*/
	SUBTITLE_LANGUAGE_UNICODE  = 10   /*!< Unicode  */
} Subtitle_Language;

typedef enum {
	SUBTITLE_FILEFORMAT_NORMAL   = 0,   /*!< Normal Language */
	SUBTITLE_FILEFORMAT_UTF8     = 1,   /*!< UTF8  */	
	SUBTITLE_FILEFORMAT_UTF16    = 2    /*!< UTF16  */
} Subtitle_File_Format;


typedef enum {
	SUBTITLE_PARAMETER_UNKNOWN			   = 0,  
	SUBTITLE_PARAMETER_SET_FONT_COLOR_RGB  = 1,  
	SUBTITLE_PARAMETER_SET_FONT_COLOR_UYVY = 2,
	SUBTITLE_PARAMETER_SET_FONT_SIZE	   = 3,  
	SUBTITLE_PARAMETER_GET_LANGUAGE		   = 4,  
	SUBTITLE_PARAMETER_SET_LANGUAGE		   = 5,  
	SUBTITLE_PARAMETER_GET_LANGUAGE_COUNT  = 6,   
	SUBTITLE_PARAMETER_SET_DISPLAY	       = 7,
	SUBTITLE_PARAMETER_ONLY_SHOW_BY_LANGUAGE  = 8,
	SUBTITLE_PARAMETER_ONLY_SHOW_BY_INDEX     = 9
} Subtitle_Parameter;



typedef enum SUBTITLE_ERRORTYPE
{
	Subtitle_ErrorNone = 0,

	/** Unknown error */
	Subtitle_ErrorUnknown = 0x8000001,

	/** There were insufficient resources to perform the requested operation */
	Subtitle_ErrorInsufficientResources =  0x80001000,

	/** There was an error, but the handle could not be determined */
	Subtitle_ErrorInvalidState =  0x80001001,

	/** No component with the specified name string was found */
	Subtitle_ErrorFileNotFound =  0x80001002,

	/** No component with the specified name string was found */
	Subtitle_ErrorParserFailed =  0x80001003,

	/** No component with the specified name string was found */
	Subtitle_ErrorBadParameter =  0x80001004,
	
	Subtitle_ErrorMax = 0x7FFFFFFF

} SUBTITLE_ERRORTYPE;


/**
 * VisualOn Subtitle function set
 */
typedef struct
{
	int (SUBTITLE_APIENTRY* vostOpen) (VO_PTR * phSubtitle);
	int (SUBTITLE_APIENTRY* vostClose) (VO_PTR hSubtitle);

	// =============================== Common  =================================

	int (SUBTITLE_APIENTRY* vostSetMediaFile) (VO_PTR hSubtitle, VO_TCHAR * pPath);
	int (SUBTITLE_APIENTRY* vostGetTrackCount) (VO_PTR hSubtitle);
	Subtitle_Language (SUBTITLE_APIENTRY* vostGetTrackLanguage) (VO_PTR hSubtitle, int nIndex);
	Subtitle_Track_Type (SUBTITLE_APIENTRY* vostGetTrackType) (VO_PTR hSubtitle, int nIndex);

	// =============================== Overlay =================================

	int (SUBTITLE_APIENTRY* vostSetVideoInfo) (VO_PTR hSubtitle, int nWidth, int nHeight, int nAngle);
	int (SUBTITLE_APIENTRY* vostOverlay) (VO_PTR hSubtitle, VO_VIDEO_BUFFER * pVideoBuffer);
	int (SUBTITLE_APIENTRY* vostSetParameter) (VO_PTR hSubtitle, long nID, long lValue);
	int (SUBTITLE_APIENTRY* vostGetParameter) (VO_PTR hSubtitle, long nID, long *plValue);

}VO_SUBTITLE_API;



/**
 * Get Subtitle API interface
 * \param pSubtitle [IN/OUT] Return the Engine API handle.
 * \param uFlag,reserved
 * \retval Subtitle_ErrorNone Succeeded.
 */
SUBTITLE_ERRORTYPE SUBTITLE_APIENTRY voGetSubtitleAPI (VO_SUBTITLE_API * pSubtitleAPI, int uFlag);

typedef SUBTITLE_ERRORTYPE (SUBTITLE_APIENTRY * VOGETSUBTITLEAPI) (VO_SUBTITLE_API * pFunc, int uFlag);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __subtitleAPI_H__
