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

#ifndef __voSUBTITLEParser_H__
#define __voSUBTITLEParser_H__

#include "voType.h"
#include "voString.h"
#include "voIVCommon.h"
#include "voSubtitleType.h"
#include "voOnStreamType.h"

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

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
	SUBTITLE_PARSER_TYPE_UNKNOWN  = 0,    /*!< Unknown track */
	SUBTITLE_PARSER_TYPE_SRT      = 1,    /*!< SRT Format   */
	SUBTITLE_PARSER_TYPE_SMI	  = 2,    /*!< SMI format */
	SUBTITLE_PARSER_TYPE_TTML		= 3,     /*!< timed text mark language format */
	SUBTITLE_PARSER_TYPE_SSM_TTML   = 4,     /*!<ssm timed text mark language format */
	SUBTITLE_PARSER_TYPE_WEBVTT = 5,		/*!< Web Video Text Tracks Format */
	SUBTITLE_PARSER_TYPE_LRC = 6		/*!< Lyric Format */
} Subtitle_Parser_Type;

typedef struct
{
	VO_U32			nType;
	VO_TCHAR *   		strWorkPath; 
	VO_PTR			pReserved;
}VO_SUBTITLE_PARSER_INIT_INFO;

typedef enum {
	SUBTITLE_PARAMETER_UNKNOWN				= 0, 
	//To clear inneral buffers
	SUBTITLE_PARAMETER_FLUSH_BUFFER			= 1,  
	//To set current JNIEnv* to lib for android version
	ID_SET_JAVA_ENV						   = 100,
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
	//open the library
	int (SUBTITLE_APIENTRY* voInit) (VO_PTR * phSubtitle,VO_SUBTITLE_PARSER_INIT_INFO* pParam);

	//close the library
	int (SUBTITLE_APIENTRY* voUninit) (VO_PTR hSubtitle);
	
		/**
	 * set file path or url
	 * \param hSubtitle [in] object from voInit.
	 * \param pPath [in] media file path.
	 * \param nType [in] Subtitle type
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* voSetTimedTextFilePath) (VO_PTR hSubtitle, VO_TCHAR * pPath, int nType);

	/**
	 * set media file path
	 * \param hSubtitle [in] object from voInit.
	 * \param pPath [in] subtitle data buffer.
	 * \param len   [in] buffer length.
	 * \param nType [in] subtitle type
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* voSetInputData) (VO_PTR hSubtitle, VO_CHAR * pData, int len,int nType, VO_S64 llBaseTime);

	/**
	 * Get Subtitle Item
	 * \param hSubtitle [in] object from voInit.
	 * \param nTimeCurrent [in] input current time in ms.
	 * \param subtitleInfo [out] the returned subtitle object.
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* voGetSubtitleInfo) (VO_PTR hSubtitle, long nTimeCurrent, voSubtitleInfo ** subtitleInfo);



	/**
	 * get track count
	 * \param hSubtitle [in] object from voInit.
	 * \retval track count.
	 */
	int (SUBTITLE_APIENTRY* voGetTrackCount) (VO_PTR hSubtitle);

	/**
	 * get all language
	 * \param hSubtitle [in] object from voInit.
	 * \param pTrackInfo [out] track information VOOSMP_SUBTITLE_LANGUAGE*.
	 * \param nIndex [in] track index.
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* voGetTrackInfo) (VO_PTR hSubtitle, VOOSMP_SUBTITLE_LANGUAGE * pTrackInfo, int nIndex);

	/**
	 * get select track by track index
	 * \param hSubtitle [in] object from voInit.
	 * \param nIndex [in] track index.
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* voSelectTrackByIndex) (VO_PTR hSubtitle, int nIndex);


	/**
	 * set param
	 * \param hSubtitle [in] object from voInit.
	 * \param nID [in] the ID of parameter.
	 * \param pValue [in] the parameter object.
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* voSetParam) (VO_PTR hSubtitle, long nID, void* pValue);

	/**
	 * get param
	 * \param hSubtitle [in] object from voInit.
	 * \param nID [in] the ID of parameter.
	 * \param pValue [out] the output value of parameter.
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* voGetParam) (VO_PTR hSubtitle, long nID, void *pValue);
	

}VO_SUBTITLE_PARSER;



/**
 * Get Subtitle Parser API interface
 * \param pSubtitle [IN/OUT] Return the Engine API handle.
 * \param uFlag,reserved
 * \retval Subtitle_ErrorNone Succeeded.
 */
SUBTITLE_ERRORTYPE SUBTITLE_APIENTRY voGetSubTitleParserAPI (VO_SUBTITLE_PARSER * pSubtitleAPI, int uFlag);

typedef SUBTITLE_ERRORTYPE (SUBTITLE_APIENTRY * VOGETSUBTITLEPARSERAPI) (VO_SUBTITLE_PARSER * pFunc, int uFlag);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voSUBTITLEParser_H__