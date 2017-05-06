
/*
Mobile TV reader & parser
*/

#ifdef MTV_CMMB
#include "CCmmbParser.h"
#elif defined MTV_TS
#include "CTsParser.h"
#endif


#include "voType.h"
#include "fMacros.h"
#include "voLog.h"
#include "CDumper.h"

#if defined __cplusplus
extern "C" {
#endif

	VO_U32 VO_API voMTVParserOpen(VO_PTR * ppHandle, VO_PARSER_INIT_INFO* pParam)
	{

		CBaseParser* pParser = VO_NULL;

#ifdef MTV_CMMB
		pParser = new CCmmbParser(pParam->pMemOP);
#elif defined MTV_TS
		pParser = new CTsParser(pParam->pMemOP);
#endif
		if (!pParser)
		{
			return VO_ERR_PARSER_OPEN_FAIL;
		}
		
		*ppHandle = pParser;

		pParser->Open(pParam);

		return VO_ERR_PARSER_OK;
	}

	/**
	* Close the opened source.
	* \param pHandle [IN] The handle which was create by open function.
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voMTVParserClose(VO_PTR pHandle)
	{
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		CBaseParser* pParser = (CBaseParser*)pHandle;
		pParser->Close();
		delete pParser;

		return VO_ERR_PARSER_OK;
	}

	/**
	* Parse the buffer.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pBuffer [IN] The buffer to be parsed
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voMTVParserProcess(VO_PTR pHandle, VO_PARSER_INPUT_BUFFER* pBuffer)
	{
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		CBaseParser* pParser = (CBaseParser*)pHandle;
		return pParser->Process(pBuffer);
	}


	/**
	* Set param for special target.
	* \param pHandle [IN] The handle which was create by open function.
	* \param uID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID>
	* \retval VO_ERR_PARSER_OK Succeeded. 
	*/
	VO_U32 VO_API voMTVParserSetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		CBaseParser* pParser = (CBaseParser*)pHandle;
		return pParser->SetParam(uID, pParam);

		return VO_ERR_PARSER_OK;
	}

	/**
	* Get param for special target.
	* \param pHandle [IN] The handle which was create by open function.
	* \param uID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID>
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voMTVParserGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		CBaseParser* pParser = (CBaseParser*)pHandle;
		return pParser->GetParam(uID, pParam);

		return VO_ERR_PARSER_OK;
	}

	VO_S32 VO_API voGetParserAPI(VO_PARSER_API* pHandle)
	{
		pHandle->Open		= voMTVParserOpen;
		pHandle->Close		= voMTVParserClose;
		pHandle->Process	= voMTVParserProcess;
		pHandle->SetParam	= voMTVParserSetParam;
		pHandle->GetParam	= voMTVParserGetParam;

		return VO_ERR_PARSER_OK;
	}

#if defined __cplusplus
}
#endif

