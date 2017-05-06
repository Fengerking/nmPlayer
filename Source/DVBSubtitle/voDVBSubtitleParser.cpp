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
* @file voDVBSubtitleParser.cpp
*    
*
*
* @author  Ferry Zhang
* Change History
* 2012-12-19  Create File
************************************************************************/

#include "voDVBSubtitleParser.h"
#include "voLog.h"
#include "DVBParser.h"

#ifdef __cplusplus
extern "C"{
#endif	/* __cplusplus */

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif


VO_U32 VO_API voDVBParserOpen(VO_PTR * ppHandle, VO_DVB_PARSER_INIT_INFO* pParam)
{
	VOLOGINIT(pParam->strWorkPath)
	DVBParser* pParser = NULL;
	pParser = new DVBParser();
	if(!pParser)
		return VO_ERR_DVB_PARSER_OPEN_FAIL;
	*ppHandle = pParser;
	return VO_ERR_DVB_PARSER_OK;	
}


VO_U32 VO_API voDVBParserClose(VO_PTR pHandle)
{
	if(!pHandle)
		return VO_ERR_DVB_PARSER_INVLAID_HANDLE;

	DVBParser* pParser = (DVBParser*)pHandle;
	delete pParser;
	pParser = NULL;
	VOLOGUNINIT()
	return VO_ERR_DVB_PARSER_OK;
}

VO_U32 VO_API voDVBParserProcess(VO_PTR pHandle, VO_DVB_PARSER_INPUT_BUFFER* pBuffer)
{
	if(!pHandle)
		return VO_ERR_DVB_PARSER_INVLAID_HANDLE;
	DVBParser *pParser = (DVBParser*)pHandle;
	
	return pParser->Process(pBuffer);
}

VO_U32 VO_API voDVBParserGetData(VO_PTR pHandle, VO_DVB_PARSER_OUTPUT_BUFFER* pBuffer)
{
	if(!pHandle)
		return VO_ERR_DVB_PARSER_INVLAID_HANDLE;
	DVBParser *pParser = (DVBParser*)pHandle;
	
	return pParser->GetData(pBuffer);
}

VO_U32 VO_API voDVBParserGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_DVB_PARSER_OK;
}

VO_U32 VO_API voDVBParserSetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_DVB_PARSER_OK;
}

VO_S32 VO_API voGetDVBParserAPI(VO_DVB_PARSER_API * pHandle)
{
	pHandle->Open = voDVBParserOpen;
	pHandle->Close = voDVBParserClose;
	pHandle->Process = voDVBParserProcess;
	pHandle->GetData = voDVBParserGetData;
	pHandle->GetParam = voDVBParserGetParam;
	pHandle->SetParam = voDVBParserSetParam;

	return VO_ERR_DVB_PARSER_OK;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */