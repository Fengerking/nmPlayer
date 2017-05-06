
/*
Mobile TV reader & parser
*/

// #ifdef MTV_CMMB
// #include "CCmmbParser.h"
// #elif defined MTV_TS
// #include "CTsParser.h"
// #endif


#include "voType.h"
#include "fMacros.h"
#include "voLog.h"
#include "voParser.h"
#include "voIndex.h"

#include "voSource2.h"
#include "CMp4PushReader2.h"
#include "CMp4PushReader.h"


#ifdef MULITI_TRACK
#define PUSH_READER CMp4PushReader2
#else
#define PUSH_READER CMp4PushReader
#endif



#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
	#if defined __cplusplus
		extern "C" {
	#endif
#endif

	VO_U32 VO_API voStreamReaderOpen(VO_PTR * ppHandle, VO_PARSER_INIT_INFO* pParam)
	{
		//VOLOGI("  + voStreamReaderOpen");

		PUSH_READER* pReader= NULL;

		pReader = new PUSH_READER;

		if (!pReader)
		{
			return VO_ERR_PARSER_OPEN_FAIL;
		}

		*ppHandle = pReader;

		pReader->Open(pParam->pProc, pParam->pUserData,pParam->strWorkPath);

		return VO_ERR_PARSER_OK;

		//VOLOGI("  - voStreamReaderOpen");
	}

	/**
	* Close the opened source.
	* \param pHandle [IN] The handle which was create by open function.
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voStreamReaderClose(VO_PTR pHandle)
	{
		if(!pHandle)
			return VO_RET_SOURCE2_FAIL;

		PUSH_READER* pReader = (PUSH_READER*)pHandle;
		pReader->Close();
		delete pReader;

		return VO_ERR_PARSER_OK;
	}

	/**
	* Parse the buffer.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pBuffer [IN] The buffer to be parsed
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voStreamReaderProcess(VO_PTR pHandle, VO_PARSER_INPUT_BUFFER* pBuffer)
	{
		if(!pHandle)
			return VO_RET_SOURCE2_FAIL;

		PUSH_READER* pReader = (PUSH_READER*)pHandle;
		return pReader->Process(pBuffer);
	}


	/**
	* Set param for special target.
	* \param pHandle [IN] The handle which was create by open function.
	* \param uID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID>
	* \retval VO_ERR_PARSER_OK Succeeded. 
	*/
	VO_U32 VO_API voStreamReaderSetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		VOLOGR("+voStreamReaderSetParam");
		if (uID == VO_PID_COMMON_LOGFUNC)
		{
		//	VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
		//	vologInit (pVologCB->pUserData, pVologCB->fCallBack);
		}
		VOLOGR("1");

		if(!pHandle)
			return VO_ERR_PARSER_INVLAID_HANDLE;

		PUSH_READER* pReader = (PUSH_READER*)pHandle;
		VOLOGR("-voStreamReaderSetParam");
		return pReader->SetParam(uID, pParam);

	}

	/**
	* Get param for special target.
	* \param pHandle [IN] The handle which was create by open function.
	* \param uID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID>
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voStreamReaderGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_RET_SOURCE2_FAIL;
		PUSH_READER* pReader = (PUSH_READER*)pHandle;
		return pReader->GetParam(uID, pParam);

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
#ifdef _MP4_READER
	VO_S32 VO_API voGetMp4StreamReaderAPI(VO_PARSER_API* pHandle)
#else
	VO_S32 VO_API voGetStreamReaderAPI(VO_PARSER_API* pHandle)
#endif	
	{
		pHandle->Open		= voStreamReaderOpen;
		pHandle->Close		= voStreamReaderClose;
		pHandle->Process	= voStreamReaderProcess;
		pHandle->SetParam	= voStreamReaderSetParam;
		pHandle->GetParam	= voStreamReaderGetParam;

		return VO_ERR_PARSER_OK;
	}

#if defined __cplusplus
}
#endif

