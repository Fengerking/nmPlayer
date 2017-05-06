/************************************************************************
VisualOn Proprietary
Copyright (c) 2003, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifndef _voDVBSubtitleParser_H__
#define _voDVBSubtitleParser_H__

#ifdef __cplusplus
extern "C"{
#endif	/* __cplusplus */

#include "voIndex.h"
#include "voSubtitleType.h"

/* Error Code */
#define VO_ERR_DVB_PARSER_OK				VO_ERR_NONE
#define VO_ERR_DVB_PARSER_BASE				0x95000000
#define VO_ERR_DVB_PARSER_OPEN_FAIL			(VO_ERR_DVB_PARSER_BASE | 0x0001)		/*!< open fail */
#define VO_ERR_DVB_PARSER_NOT_IMPLEMENT		(VO_ERR_DVB_PARSER_BASE | 0x0002)		/*!< not implemented */
#define VO_ERR_DVB_PARSER_INVLAID_PARAM_ID	(VO_ERR_DVB_PARSER_BASE | 0x0003)		/*!< param id not support*/
#define VO_ERR_DVB_PARSER_INVLAID_HANDLE	(VO_ERR_DVB_PARSER_BASE | 0x0004)		/*!< handle is invalid*/
#define VO_ERR_DVB_PARSER_FAIL				(VO_ERR_DVB_PARSER_BASE | 0x0005)		/*!< parse failed*/
#define VO_ERR_DVB_PARSER_OUT_OF_MEMORY		(VO_ERR_DVB_PARSER_BASE | 0x0006)		/*!< parse failed*/
#define VO_ERR_DVB_PARSER_ERROR				(VO_ERR_DVB_PARSER_BASE | 0x0007)		/*!< parse failed*/
#define VO_ERR_DVB_PARSER_INVALID_ARG		(VO_ERR_DVB_PARSER_BASE | 0x0008)		/*!< parse failed*/
#define VO_ERR_DVB_PARSER_RETRY				(VO_ERR_DVB_PARSER_BASE | 0x0009)		/*!< CallBack data overflow,Retry*/



/* DVB type enum */
typedef enum
{
	VO_DVB_SUBTITLE_TYPE_TELETEXT = 0,
	VO_DVB_SUBTITLE_TYPE_SYSTEM,
}VO_DVB_PARSER_TYPE;

typedef struct
{
	VO_U32			nType;
	VO_TCHAR *   		strWorkPath; 
	VO_PTR			pReserved;
}VO_DVB_PARSER_INIT_INFO;

typedef struct
{
	VO_U64			nTimeStamp;
	VO_U32			nSize;
	VO_PBYTE		pData;
	VO_VOID*		pReserved;
}VO_DVB_PARSER_INPUT_BUFFER;

typedef struct
{
	voSubtitleInfo*				pSubtitleInfo;///<refer to voSubtitleInfo
	VO_PTR						pReserved;
}VO_DVB_PARSER_OUTPUT_BUFFER;

/* DVB Subtitle function set */
typedef struct
{
	/**
	 * Open the parser and return handle
	 * \param ppHandle [OUT] Return the parser operator handle
	 * \param pParam [IN] The parser open param
	 * \retval VO_ERR_PARSER_OK Succeeded.
	 */
	VO_U32(VO_API *Open)(VO_PTR* ppHandle, VO_DVB_PARSER_INIT_INFO* pParam);

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
	VO_U32 (VO_API * Process) (VO_PTR pHandle, VO_DVB_PARSER_INPUT_BUFFER* pBuffer);

	/**
	* Get Output Data.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pBuffer [IN] pOutputData be alloced whthin Parser,it shall be free once need process new data by call Process or any other I/F
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 (VO_API * GetData) (VO_PTR pHandle, VO_DVB_PARSER_OUTPUT_BUFFER* pBuffer);

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
} VO_DVB_PARSER_API;

/**
* Get parser API interface
* \param pParser [out] Return the parser handle.
* \retval VO_ERR_PARSER_OK Succeeded.
*/
VO_S32 VO_API voGetDVBParserAPI(VO_DVB_PARSER_API * pHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif