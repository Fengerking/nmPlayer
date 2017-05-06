// voParserPush.cpp : Defines the exported functions for the DLL application.
//
#include "voParser.h"
#include "ParserPush.h"

VO_U32 Open(VO_PTR * ppHandle, VO_PARSER_INIT_INFO* pParam)
{
	CParserPush *pParserPush = new CParserPush();
	if (NULL == pParserPush)
		return VO_ERR_OUTOF_MEMORY;

	if (pParserPush->Open(pParam))
		return VO_ERR_FAILED;

	*ppHandle = pParserPush;

	return VO_ERR_NONE;
}

VO_U32 Close(VO_PTR pHandle)
{
	if (NULL == pHandle)
		return VO_ERR_INVALID_ARG;

	CParserPush *pParserPush = (CParserPush *)(pHandle);
	pParserPush->Close();
	delete pHandle;

	return VO_ERR_NONE;
}

VO_U32 Process(VO_PTR pHandle, VO_PARSER_INPUT_BUFFER* pBuffer)
{
	if (NULL == pHandle)
		return VO_ERR_INVALID_ARG;

	CParserPush *pParserPush = (CParserPush *)(pHandle);

	return pParserPush->Process(pBuffer);
}

VO_U32 SetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if (NULL == pHandle)
		return VO_ERR_INVALID_ARG;

	CParserPush *pParserPush = (CParserPush *)(pHandle);

	return pParserPush->SetParam(uID, pParam);
}

VO_U32 GetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if (NULL == pHandle)
		return VO_ERR_INVALID_ARG;

	CParserPush *pParserPush = (CParserPush *)(pHandle);

	return pParserPush->GetParam(uID, pParam);
}

VO_S32 VO_API voGetParserPushAPI(VO_PARSER_API * pParser)
{
	if (pParser == NULL)
		return VO_ERR_INVALID_ARG;

	pParser->Open		= Open;
	pParser->Close		= Close;
	pParser->Process	= Process;
	pParser->SetParam	= SetParam;
	pParser->GetParam	= GetParam;

	return VO_ERR_NONE;
}

