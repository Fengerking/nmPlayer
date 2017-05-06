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

#ifndef __voCaptionParser_H__
#define __voCaptionParser_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"
#include "voSubtitleType.h"
/**
* Error code
*/
#define VO_ERR_PARSER_OK						VO_ERR_NONE
#define VO_ERR_PARSER_BASE						0x96000000
#define VO_ERR_PARSER_OPEN_FAIL					(VO_ERR_PARSER_BASE | 0x0001)		/*!< open fail */
#define VO_ERR_PARSER_NOT_IMPLEMENT				(VO_ERR_PARSER_BASE | 0x0002)		/*!< not implemented */
#define VO_ERR_PARSER_INVLAID_PARAM_ID			(VO_ERR_PARSER_BASE | 0x0003)		/*!< param id not support*/
#define VO_ERR_PARSER_INVLAID_HANDLE			(VO_ERR_PARSER_BASE | 0x0004)		/*!< handle is invalid*/
#define VO_ERR_PARSER_FAIL						(VO_ERR_PARSER_BASE | 0x0005)		/*!< parse failed*/
#define VO_ERR_PARSER_OUT_OF_MEMORY				(VO_ERR_PARSER_BASE | 0x0006)		/*!< parse failed*/
#define VO_ERR_PARSER_ERROR						(VO_ERR_PARSER_BASE | 0x0007)		/*!< parse failed*/
#define VO_ERR_PARSER_INVALID_ARG				(VO_ERR_PARSER_BASE | 0x0008)		/*!< parse failed*/
#define VO_ERR_PARSER_INPUT_BUFFER_SMALL		(VO_ERR_PARSER_BASE | 0x0009)		/*!< parse failed*/
typedef struct
{
	VO_U64			nTimeStamp;
	VO_U32			nSize;
	VO_PBYTE		pData;
	VO_VOID*		pReserved;
}VO_CAPTION_PARSER_INPUT_BUFFER;

typedef struct
{
	voSubtitleInfo*				pSubtitleInfo;///<refer to voSubtitleInfo
	VO_PTR						pReserved;
}VO_CAPTION_PARSER_OUTPUT_BUFFER;

typedef struct
{
	VO_CHAR chLangName[4];
	VO_PTR	pReserved;
}VO_CAPTION_LANGUAGE;

typedef struct
{
	VO_U32					nLangCnt;
	VO_CAPTION_LANGUAGE		**pLanguage;
	VO_PTR					pReserved;
}VO_CAPTION_LANGUAGE_INFO;

typedef struct
{
	VO_U32					uDataLen;///<Data length
	VO_VOID					*pData;
	VO_PTR					pReserved;
}VO_CAPTION_LANGUAGE_DESCRIPTION;

typedef enum
{
	VO_CAPTION_TYPE_EIA608 = 0,
	VO_CAPTION_TYPE_EIA708,
	VO_CAPTION_TYPE_DEFAULT_608,
	VO_CAPTION_TYPE_DEFAULT_708,
	VO_CAPTION_TYPE_ARIB
}VO_CAPTION_TYPE;

typedef enum
{
	VO_CAPTION_CHANNEL_1 = 0,
	VO_CAPTION_CHANNEL_2,
	VO_CAPTION_CHANNEL_3,
	VO_CAPTION_CHANNEL_4,
	VO_CAPTION_CHANNEL_XDS,
	VO_CAPTION_CHANNEL_UNKOWN
}VO_CAPTION_CHANNEL;

typedef enum
{
	VO_PARAMETER_ID_CHANNEL = 0,	///<the Param value refer to  VO_CAPTION_CHANNEL
	VO_PARAMETER_ID_RESTART,		///<restart to parse CC data,first clear all cache
	VO_PARAMETER_ID_FLUSH,			///<Once EOS,Flush CC Cached Input data,then Call GetData to Get the output data ,the Param value is useless 
	VO_PARAMETER_ID_DATAINFO,		///<Set new head data to CC, cc just destory the cache and create a new one
	VO_PARAMETER_ID_708LANGUAGEDes ///<Set the language infromation data to CC,CC can get the language from those data.
}VO_PARAMETER_ID;

typedef struct
{
	VO_U32		nType;///<0 indicate the H264 frame,1 indicate the SEI_RBSP
	VO_U32		nSize;///<if nType equal 0, it indicate H264 head data size,else it is useless
	VO_PBYTE	pHeadData;///<if nType equal 0,H264 head data,else it is useless
}VO_CAPTION_DATA_INFO;

typedef struct
{
	VO_U32					 nType;///<refer to VO_CAPTION_TYPE
	VO_CAPTION_DATA_INFO	 stDataInfo;
	VO_TCHAR *   				strWorkPath; 
	VO_PTR					 pReserved;
}VO_CAPTION_PARSER_INIT_INFO;

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
	VO_U32 (VO_API * Open) (VO_PTR * ppHandle, VO_CAPTION_PARSER_INIT_INFO* pParam);

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
	VO_U32 (VO_API * Process) (VO_PTR pHandle, VO_CAPTION_PARSER_INPUT_BUFFER* pBuffer);


	/**
	* Get Output Data.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pBuffer [IN] pOutputData be alloced whthin Parser,it shall be free once need process new data by call Process or any other I/F
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 (VO_API * GetData) (VO_PTR pHandle, VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer);

	/**
	* Get Language Infomation.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pLangInfo [IN] buffer will be filled by parser
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 (VO_API *GetLanguage)(VO_PTR pHandle, VO_CAPTION_LANGUAGE_INFO** pLangInfo);

	/**
	* Select Language Information.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pLangInfo [IN] will be the language of selected
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 (VO_API *SelectLanguage)(VO_PTR pHandle, VO_U32 nLangNum);

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
} VO_CAPTION_PARSER_API;

/**
* Get parser API interface
* \param pParser [out] Return the parser handle.
* \retval VO_ERR_PARSER_OK Succeeded.
*/
VO_S32 VO_API voGetCaptionParserAPI(VO_CAPTION_PARSER_API * pParser);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voCaptionParser_H__
