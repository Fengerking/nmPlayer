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

#ifndef __voParser_H__
#define __voParser_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"
#include "voMem.h"
#include "voMTVBase.h"
#include "voType.h"


/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_PARSER_BASE				0x53000000							/*!< the base param ID for source modules */
#define	VO_PID_PARSER_QUERY_ESG			(VO_PID_PARSER_BASE | 0x0001)		/*!< param ID for query ESG information */
#define	VO_PID_PARSER_SELECT_PROGRAM	(VO_PID_PARSER_BASE | 0x0002)		/*!< param ID for select a program*/
#define	VO_PID_PARSER_TS_BASE			0x54000000							/*!< the base param ID for TS */
#define	VO_PID_PARSER_CMMB_BASE			0x55000000							/*!< the base param ID for CMMB */


/**
* Error code
*/
#define VO_ERR_PARSER_OK				VO_ERR_NONE
#define VO_ERR_PARSER_BASE				0x96000000
#define VO_ERR_PARSER_OPEN_FAIL			(VO_ERR_PARSER_BASE | 0x0001)		/*!< open fail */
#define VO_ERR_PARSER_NOT_IMPLEMENT		(VO_ERR_PARSER_BASE | 0x0002)		/*!< not implemented */
#define VO_ERR_PARSER_INVLAID_PARAM_ID	(VO_ERR_PARSER_BASE | 0x0003)		/*!< param id not support*/
#define VO_ERR_PARSER_INVLAID_HANDLE	(VO_ERR_PARSER_BASE | 0x0004)		/*!< handle is invalid*/
#define VO_ERR_PARSER_FAIL				(VO_ERR_PARSER_BASE | 0x0005)		/*!< parse failed*/
#define VO_ERR_PARSER_OUT_OF_MEMORY		(VO_ERR_PARSER_BASE | 0x0006)		/*!< parse failed*/
#define VO_ERR_PARSER_ERROR				(VO_ERR_PARSER_BASE | 0x0007)		/*!< parse failed*/
#define VO_ERR_PARSER_INVALID_ARG		(VO_ERR_PARSER_BASE | 0x0008)		/*!< parse failed*/
#define VO_ERR_PARSER_RETRY				(VO_ERR_PARSER_BASE | 0x0009)		/*!< CallBack data overflow,Retry*/


#define VO_ERR_PARSER_BASE_CMMB			(VO_ERR_PARSER_BASE | 0x0100)
#define VO_ERR_PARSER_BASE_TS			(VO_ERR_PARSER_BASE | 0x0200)





/**
* Parser media type
*/
typedef enum
{
	VO_PARSER_MEDIA_TYPE_EX_BASE		        = 0x2000,
	VO_PARSER_MEDIA_TYPE_EX_AUDIO		        = (VO_PARSER_MEDIA_TYPE_EX_BASE | 0x0001),			// refer to AUDIO_TYPE
	VO_PARSER_MEDIA_TYPE_EX_VIDEO		        = (VO_PARSER_MEDIA_TYPE_EX_BASE | 0x0002),			// refer to VIDEO_TYPE
	VO_PARSER_MEDIA_TYPE_EX_TEXT		        = (VO_PARSER_MEDIA_TYPE_EX_BASE | 0x0003),          // refer to SUBTITLE
    VO_PARSER_MEDIA_TYPE_EX_PRIVATE_DATA        = (VO_PARSER_MEDIA_TYPE_EX_BASE | 0x0004),			// refer to ID3 Tag In TsParser    
    VO_PARSER_MEDIA_TYPE_EX_INVALID_DATA        = (VO_PARSER_MEDIA_TYPE_EX_BASE | 0x0005),			// refer to INVALID
}VO_PARSER_MEDIA_TYPE_EX;

/**
* Parser media type
*/


/**
* Parser output type
*/
typedef enum
{
	VO_PARSER_OT_BASE		= 0x1000,

	VO_PARSER_OT_AUDIO		= (VO_PARSER_OT_BASE | 0x0001),			// refer to VO_MTV_FRAME_BUFFER
	VO_PARSER_OT_VIDEO		= (VO_PARSER_OT_BASE | 0x0002),			// refer to VO_MTV_FRAME_BUFFER
	VO_PARSER_OT_EPG		= (VO_PARSER_OT_BASE | 0x0003),	
	VO_PARSER_OT_ERROR		= (VO_PARSER_OT_BASE | 0x0004),
	VO_PARSER_OT_STREAMINFO = (VO_PARSER_OT_BASE | 0x0005),         // refer to VO_PARSER_STREAMINFO
	VO_PARSER_OT_TEXT		= (VO_PARSER_OT_BASE | 0x0006),         // refer to VO_PARSER_STREAMINFO
	VO_PARSER_OT_MEDIATYPE  = (VO_PARSER_OT_BASE | 0x0007),			// refer to VO_PARSER_MEDIA_TYPE
	VO_PARSER_OT_TRACKINFO  = (VO_PARSER_OT_BASE | 0x0008),			// refer to VO_LIVESRC_TRACK_INFOEX

    VO_PARSER_OT_PRIVATE_DATA  = (VO_PARSER_OT_BASE | 0x0009),			// refer to ID3 Tag In TsParser
    VO_PARSER_OT_TS_TIMESTAMP_ROLLBACK  = (VO_PARSER_OT_BASE | 0x0010), // indicate that the timestamp rollback
    VO_PARSER_OT_TS_WITHOUT_TIMESTAMP_OFFSET  = (VO_PARSER_OT_BASE | 0x0011), // indicate that the timestamp without timestamp offset
	VO_PARSER_OT_SEGMENTINDEX_INFO = (VO_PARSER_OT_BASE | 0x0012),   /*!<'sidx' information.Segment index box */
	VO_PARSER_OT_INITDATA_INFO = (VO_PARSER_OT_BASE | 0x0013),   /*!<head information, to notify the control it's initdata */
	VO_PARSER_OT_INITDATA_ISENOUGH = (VO_PARSER_OT_BASE | 0x0014),   /*!<to notify the control whether the initial data is enough */

	// Custom  base
	VO_PARSER_OT_BASE_TS	= 0x2000,
	VO_PARSER_OT_BASE_CMMB	= 0x3000
}VO_PARSER_OUTPUT_TYPE;

/**
* Parser output type
*/
typedef enum
{
	VO_PARSER_FLAG_BASE					= 0x00001000,

	VO_PARSER_FLAG_STREAM_CHANGED		= (VO_PARSER_FLAG_BASE | 0x0001),
	VO_PARSER_FLAG_STREAM_END			= (VO_PARSER_FLAG_BASE | 0x0002),	//used for other push mode file parser except ts file parser
	VO_PARSER_FLAG_STREAM_RESET_ALL = (VO_PARSER_FLAG_BASE | 0x0003),
	VO_PARSER_FLAG_STREAM_DASHHEAD       =(VO_PARSER_FLAG_BASE | 0X0004),
	VO_PARSER_FLAG_STREAM_DASHDATA       =(VO_PARSER_FLAG_BASE | 0X0005),
	VO_PARSER_FLAG_STREAM_DASHINDEX      =(VO_PARSER_FLAG_BASE | 0X0006),	/*!<identify this is DASH index data */
	VO_PARSER_FLAG_STREAM_DASHINITDATA   = (VO_PARSER_FLAG_BASE | 0X0007),	/*!<identify this is for dash-if init data */
}VO_PARSER_FLAG;

typedef enum
{
	VO_MEDIA_PURE_AUDIO,
	VO_MEDIA_PURE_VIDEO,
	VO_MEDIA_AUDIO_VIDEO,
}VO_PARSER_MEDIA_TYPE;


typedef struct
{
	VO_U32		nStreamID;
	VO_PBYTE	pBuf;
	VO_U32		nBufLen;
	VO_U32		nFlag;			/*!< refer to VO_PARSER_FLAG*/
	VO_VOID*	pReserved;
}VO_PARSER_INPUT_BUFFER;

typedef struct
{
	VO_U64						nStreamID;
	VO_U32						nType;			/*!< refer to VO_PARSER_OUTPUT_TYPE. */ 
	VO_VOID*					pOutputData;
	VO_VOID*					pUserData;
	VO_VOID*					pReserved;
}VO_PARSER_OUTPUT_BUFFER;

typedef struct
{
//	VO_BOOL beVideo;
	VO_U32 nVideoCodecType; /*!< video codec type */
	VO_VOID* pVideoExtraData; /*!< extra data, format specific */
	VO_U16 nVideoExtraSize;  /*!< size of extra data */
	VO_U32 nAudioCodecType; /*!< audio Codec type */
	VO_VOID* pAudioExtraData; /*!< extra data, format specific */
	VO_U16 nAudioExtraSize;  /*!< size of extra data */
	VO_U32 nSubTitleCodecType; /*!< subtitle Codec type */
	VO_VOID* pSubTitleExtraData; /*!< extra data, format specific */
	VO_U16 nSubTitleExtraSize;  /*!< size of extra data */
    VO_U32 ulStreamId;          /*!< the stream id of element stream */
    VO_PARSER_MEDIA_TYPE_EX   eMediaType;   /*!< the stream media type */

	union
	{
		struct 
		{
			VO_U32 width;   /*!< Width */
			VO_U32 height;  /*!< Height */
		}VideoFormat;
		struct 
		{
			VO_U32 sample_rate;  /*!< Sample rate */
			VO_U16 channels;    /*!< Channel count */
			VO_U16 sample_bits;  /*!< Bits per sample */
			VO_CHAR   strLanguage[16];  /*!< Language */
		}AudioFormat;
		struct 
		{
			VO_CHAR   strLanguage[16];  /*!< Language */
		}SubTitleFormat;
	};
    VO_VOID* pVideoClosedCaptionDescData; /*!< extra data, CC Desc Data */
    VO_U16   nVideoClosedCaptionDescDataLen;  /*!< size of CC Desc Data */
}VO_PARSER_STREAMINFO;


/**
* Callback function. The Parser will call this function after parse buffer
* \param VO_PARSER_OUTPUT_BUFFER [out]
*/
typedef void (VO_API *PARSERPROC)(VO_PARSER_OUTPUT_BUFFER* pData);

typedef struct
{
	PARSERPROC			pProc;
	VO_MEM_OPERATOR *	pMemOP;				/*!< memory operator functions. */
	VO_VOID*			pUserData;
	VO_TCHAR *   		strWorkPath;     /*!< Load library work path */
}VO_PARSER_INIT_INFO;


/**
 * Parser function set
 */
typedef struct
{
	/**
	 * Open the parser and return handle
	 * \param ppHandle [OUT] Return the parser operator handle
	 * \param pParam [IN] The parser open param
	 * \retval VO_ERR_PARSER_OK Succeeded.
	 */
	VO_U32 (VO_API * Open) (VO_PTR * ppHandle, VO_PARSER_INIT_INFO* pParam);

	/**
	 * Close the opened source.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \retval VO_ERR_PARSER_OK Succeeded.
	 */
	VO_U32 (VO_API * Close) (VO_PTR pHandle);

	/**
	* Parse the buffer.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pBuffer [IN] The buffer to be parsed
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 (VO_API * Process) (VO_PTR pHandle, VO_PARSER_INPUT_BUFFER* pBuffer);


	/**
	 * Set param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param uID [IN] The param ID.
	 * \param pParam [IN] The param value depend on the ID>
	 * \retval VO_ERR_PARSER_OK Succeeded. 
	 */
	VO_U32 (VO_API * SetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	/**
	 * Get param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param uID [IN] The param ID.
	 * \param pParam [IN] The param value depend on the ID>
	 * \retval VO_ERR_PARSER_OK Succeeded.
	 */
	VO_U32 (VO_API * GetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
} VO_PARSER_API;

#ifdef _PUSH_READER
VO_S32 VO_API voGetParserPushAPI(VO_PARSER_API * pParser);
#else
/**
* Get parser API interface
* \param pParser [out] Return the parser handle.
* \retval VO_ERR_PARSER_OK Succeeded.
*/
VO_S32 VO_API voGetMp4StreamReaderAPI(VO_PARSER_API * pParser);
VO_S32 VO_API voGetSMTHParserAPI(VO_PARSER_API * pParser);
VO_S32 VO_API yyGetTsDemuxFunc(VO_PARSER_API * pParser);
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voParser_H__
