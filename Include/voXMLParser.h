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
 
#ifndef __voXMLParser_H__
#define __voXMLParser_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voXMLBase.h"

	typedef struct
	{
		VO_VOID			*pProc;/* CALL BACK PROC*/
		VO_MEM_OPERATOR *	pMemOP;				/*!< memory operator functions. */
		VO_VOID*			pUserData;
	}VO_XML_PARSER_INIT_INFO;

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
		VO_U32 (VO_API * Open) (VO_PTR * ppHandle,VO_XML_PARSER_INIT_INFO* pParam);

		/**
		* Close the opened source.
		* \param pHandle [IN] The handle which was create by open function.
		* \retval VO_ERR_PARSER_OK Succeeded.
		*/
		VO_U32 (VO_API * Close) (VO_PTR pHandle);
		/**
		*\param buf[in] , xml source pointer (file url or buffer), it is indicated by param nFlag
		* file url has not been achieved.
		*\param nSize[in] ,buf length
		*\param nFlag[in] ,source flag 
		*/
		VO_U32 (VO_API *LoadXML) (VO_PTR pHandle,char *buf, int nSize, int nFlag);
		/* \param pNode[in] ,node pointer *
		*  \param pChildNode[out] , if pNode is NULL then pChildNode return Root node 
		*/ 
		VO_U32 (VO_API *GetFirstChild) (VO_PTR pHandle,void *pNode,void **pChildNode);

		VO_U32 (VO_API *GetNextSibling) (VO_PTR pHandle,void *pNode,void **pChildNode);
		VO_U32 (VO_API *GetChildByName) (VO_PTR pHandle,void *pNode, char *pTagName,void **pChildNode);
		VO_U32 (VO_API *GetAttributeValue) (VO_PTR pHandle,void *pNode, char *pAttriName,char **pAttriValue,int &nSize);
		VO_U32 (VO_API *GetNextAttribute)	(VO_PTR pHandle, void *pNode, void **ppNext, void* pCurrent, char **ppAttriName, int &nAttriNameSize, char **ppAttriValue, int &nAttriValuesize);


		VO_U32 (VO_API *GetTagValue) (VO_PTR pHandle,void *pNode,char **pTagValue,int &nSize);
		VO_U32 (VO_API *GetTagName) (VO_PTR pHandle,void *pNode,char **pTagName,int &nSize);
		VO_U32 (VO_API *SetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
		VO_U32 (VO_API *GetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
	} VO_XML_PARSER_API;

	/**
	* Get parser API interface
	* \param pParser [out] Return the parser handle.
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_S32 VO_API voXMLGetParserAPI(VO_XML_PARSER_API * pXMLParser);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voParser_H__