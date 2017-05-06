#include "voParser.h"
#include "vortpbase.h"
#include "commonheader.h"
#include "ParseEngine.h"

namespace psr {

VO_U32 Open(VO_PTR * ppHandle, VO_PARSER_INIT_INFO* pParam)
{
	CParseEngine * pParseEngine = new CParseEngine();
	if(pParseEngine == NULL)
		return VO_ERR_PARSER_OUT_OF_MEMORY;

	VO_U32 rc = pParseEngine->Open(pParam);
	if(rc != VO_ERR_PARSER_OK)
	{
		SAFE_DELETE(pParseEngine);
		return rc;
	}

	*ppHandle = pParseEngine;

	return VO_ERR_PARSER_OK;
}

VO_U32 Close(VO_PTR pHandle)
{
	CParseEngine * pParseEngine = (CParseEngine *)pHandle;
	if(pParseEngine == NULL)
		return VO_ERR_PARSER_INVALID_ARG;

	VO_U32 rc = pParseEngine->Close();
	SAFE_DELETE(pParseEngine);
	return rc;
}

VO_U32 Reset(VO_PTR pHandle)
{
	CParseEngine * pParseEngine = (CParseEngine *)pHandle;
	if(pParseEngine == NULL)
		return VO_ERR_PARSER_INVALID_ARG;

	return pParseEngine->Reset();
}

VO_U32 Process(VO_PTR pHandle, VO_PARSER_INPUT_BUFFER* pBuffer)
{
	CParseEngine * pParseEngine = (CParseEngine *)pHandle;
	if(pParseEngine == NULL)
		return VO_ERR_PARSER_INVALID_ARG;

	return pParseEngine->Process(pBuffer);
}

VO_U32 SetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CParseEngine * pParseEngine = (CParseEngine *)pHandle;
	if(pParseEngine == NULL)
		return VO_ERR_PARSER_INVALID_ARG;

	return pParseEngine->SetParam(uID, pParam);
}

VO_U32 GetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CParseEngine * pParseEngine = (CParseEngine *)pHandle;
	if(pParseEngine == NULL)
		return VO_ERR_PARSER_INVALID_ARG;

	return pParseEngine->GetParam(uID, pParam);
}

} //namespace psr


VO_S32 VO_API voGetParserAPI(VO_PARSER_API * pParser)
{
	pParser->Open = psr::Open;
	pParser->Close = psr::Close;
	pParser->Process = psr::Process;
	pParser->SetParam = psr::SetParam;
	pParser->GetParam = psr::GetParam;

	return VO_ERR_PARSER_OK;
}
