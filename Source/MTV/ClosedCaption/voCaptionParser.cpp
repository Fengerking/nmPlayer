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
/************************************************************************
* @file voCaptionParser.cpp
*    entrance of closed caption lib
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#include "voCaptionParser.h"
#include "CCaptionParser.h"
#include "voLog.h"
#include "fMacros.h"

#ifdef _IOS
#define g_dwFRModuleID	VO_INDEX_SRC_CLOSEDCAPTION
#else
VO_U32 g_dwFRModuleID = VO_INDEX_SRC_CLOSEDCAPTION;
#endif

#if defined __cplusplus
extern "C" {
#endif ///<__cplusplus
	
#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

	VO_U32 VO_API voCaptionParserOpen(VO_PTR * ppHandle, VO_CAPTION_PARSER_INIT_INFO* pParam)
	{
		VOLOGINIT(pParam->strWorkPath)
		VOLOGE("voCaptionParserOpen :%d",pParam->nType);
	//	pParam->nType = VO_CAPTION_TYPE_DEFAULT_708;
		VOLOGE("pParamType:%d",pParam->nType);
		CCaptionParser* pParser = NULL;
		pParser = new CCaptionParser();

		if (!pParser)
			return VO_ERR_PARSER_OPEN_FAIL;

		VO_U32 rc = pParser->Open(pParam);
		if(rc != VO_ERR_PARSER_OK)
		{
			SAFE_DELETE(pParser);
			return rc;
		}
		*ppHandle = pParser;
		return VO_ERR_PARSER_OK;
	}

	/**
	* Close the opened source.
	* \param pHandle [IN] The handle which was create by open function.
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voCaptionParserClose(VO_PTR pHandle)
	{
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;
		CCaptionParser* pParser = (CCaptionParser*)pHandle;
		pParser->Close();
		delete pParser;
		pParser = NULL;
		VOLOGUNINIT()
		return VO_ERR_PARSER_OK;
	}

	/**
	* Parse the buffer.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pBuffer [IN] The buffer to be parsed
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voCaptionParserProcess(VO_PTR pHandle, VO_CAPTION_PARSER_INPUT_BUFFER* pBuffer)
	{
	//	VOLOGI("voCaptionParserProcess");
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		CCaptionParser* pParser = (CCaptionParser*)pHandle;
		return pParser->Process(pBuffer);
	}

	/**
	* Get Output Data.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pBuffer [IN] The buffer address will be filled by parser
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voCaptionParserGetData(VO_PTR pHandle, VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer)
	{
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		CCaptionParser* pParser = (CCaptionParser*)pHandle;
		return pParser->GetData(pBuffer);
	}

	/**
	* Get Language Infomation.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pLangInfo [IN] buffer will be filled by parser
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voCaptionParserGetLanguage(VO_PTR pHandle, VO_CAPTION_LANGUAGE_INFO** pLangInfo)
	{
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;
	//	VOLOGE("voCaptionParserGetLanguage");
		CCaptionParser* pParser = (CCaptionParser*)pHandle;
		return pParser->GetLanguage(pLangInfo);
	}

	/**
	* Select Language Information.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pLangInfo [IN] will be the language of selected
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voCaptionParserSelectLanguage(VO_PTR pHandle, VO_U32 nLangNum)
	{
		VOLOGE("voCaptionParserSelectLanguage:%d",nLangNum);
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		CCaptionParser* pParser = (CCaptionParser*)pHandle;
		return pParser->SelectLanguage(nLangNum);
	}

	/**
	* Set param for special target.
	* \param pHandle [IN] The handle which was create by open function.
	* \param uID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID>
	* \retval VO_ERR_PARSER_OK Succeeded. 
	*/
	VO_U32 VO_API voCaptionParserSetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		CCaptionParser* pParser = (CCaptionParser*)pHandle;
		return pParser->SetParam(uID,pParam);
	}

	/**
	* Get param for special target.
	* \param pHandle [IN] The handle which was create by open function.
	* \param uID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID>
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voCaptionParserGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;
		return VO_ERR_PARSER_OK;
	}

	VO_S32 VO_API voGetCaptionParserAPI(VO_CAPTION_PARSER_API* pHandle)
	{
		pHandle->Open		= voCaptionParserOpen;
		pHandle->Close		= voCaptionParserClose;
		pHandle->Process	= voCaptionParserProcess;
		pHandle->GetData	= voCaptionParserGetData;
		pHandle->GetLanguage = voCaptionParserGetLanguage;
		pHandle->SelectLanguage = voCaptionParserSelectLanguage;
		pHandle->SetParam	= voCaptionParserSetParam;
		pHandle->GetParam	= voCaptionParserGetParam;

		return VO_ERR_PARSER_OK;
	}

#if defined __cplusplus
}
#endif ///<__cplusplus

