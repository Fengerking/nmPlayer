
/*
Mobile TV reader & parser
*/


#include "voType.h"
#include "CSMTHParser.h"

//#include "fMacros.h"
//#include "voLog.h"
// #include "CDumper.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#if defined __cplusplus
extern "C" {
#endif

#endif

	VO_U32 VO_API voSMTHParserOpen(VO_PTR * ppHandle, VO_PARSER_INIT_INFO* pParam)
	{
		//CDumper::WriteLog("  + voSMTHParserOpen");
		VO_U32 nResult = VO_ERR_PARSER_OK;
		CSMTH_Parser* pParser = NULL;
		
		 pParser = new CSMTH_Parser;

		//pParser = new CSMTH_Parser(pParam->pMemOP);

		if (!pParser)
		{
			return VO_ERR_PARSER_OPEN_FAIL;
		}

		*ppHandle = pParser;

		nResult = pParser->Open(pParam);

//		CDumper::WriteLog("  - voSMTHParserOpen");

		return nResult;
	}

	/**
	* Close the opened source.
	* \param pHandle [IN] The handle which was create by open function.
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voSMTHParserClose(VO_PTR pHandle)
	{
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		CSMTH_Parser* pParser = (CSMTH_Parser*)pHandle;
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
	VO_U32 VO_API voSMTHParserProcess(VO_PTR pHandle, VO_PARSER_INPUT_BUFFER* pBuffer)
	{
		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		CSMTH_Parser* pParser = (CSMTH_Parser*)pHandle;

		
		if(pParser->GetLicState() == VO_FALSE)
		{
			return VO_RET_SOURCE2_OPENFAIL;
		}
		
		
		return pParser->Process(pBuffer);
	}


	/**
	* Set param for special target.
	* \param pHandle [IN] The handle which was create by open function.
	* \param uID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID>
	* \retval VO_ERR_PARSER_OK Succeeded. 
	*/
	VO_U32 VO_API voSMTHParserSetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if (uID == VO_PID_COMMON_LOGFUNC)
		{
			VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
//			vologInit (pVologCB->pUserData, pVologCB->fCallBack);
		}

		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		CSMTH_Parser* pParser = (CSMTH_Parser*)pHandle;

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
	VO_U32 VO_API voSMTHParserGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		/*CSMTH_Parser* pParser = (CSMTH_Parser*)pHandle;
		return pParser->GetParam(uID, pParam);
*/
		return VO_ERR_PARSER_OK;
	}
#ifdef _VONAMESPACE
	}
#else
#if defined __cplusplus
}
#endif
#endif
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
#if defined __cplusplus
extern "C" {
#endif
	VO_S32 VO_API voGetSMTHParserAPI(VO_PARSER_API* pHandle)
	{
		pHandle->Open		= voSMTHParserOpen;
		pHandle->Close		= voSMTHParserClose;
		pHandle->Process	= voSMTHParserProcess;
		pHandle->SetParam	= voSMTHParserSetParam;
		pHandle->GetParam	= voSMTHParserGetParam;

		return VO_ERR_PARSER_OK;
	}
#if defined __cplusplus
}
#endif
