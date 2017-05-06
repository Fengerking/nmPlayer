#include "voERRORHandler.h"
#include "ErrorHandler.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#if defined __cplusplus
extern "C" {
#endif

	VO_U32 VO_API voStremingERRORHandlerInit(VO_PTR* ppHandle, VO_SOURCE2_EVENTCALLBACK * pEventCB,VO_ADAPTIVESTREAMPARSER_STREAMTYPE nType, VO_PTR pReserved)
	{
		CErrorHandler *pErrorHandler = NULL;
		
		//if (pErrorHandlerCB->szDRMTYPE)
			pErrorHandler = new CErrorHandler(pEventCB, nType, pReserved);

		if (NULL == pErrorHandler)
			return VO_ERR_OUTOF_MEMORY;

		*ppHandle = pErrorHandler;

		return VO_ERR_NONE;
	}

	VO_U32 VO_API voStremingERRORHandlerUninit(VO_PTR pHandle)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CErrorHandler *pErrorHandler = (CErrorHandler*)pHandle;
		
		delete pErrorHandler;

		return VO_ERR_NONE;
	}
	VO_U32 VO_API voStremingERRORHandlerCheckEvent(VO_PTR pHandle, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2, VO_PTR pReserved)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;
		CErrorHandler *pErrorHandler = (CErrorHandler*)pHandle;

		return pErrorHandler->CheckEvent( nID, nParam1, nParam2, pReserved );

	}
	VO_U32 VO_API voStremingERRORHandlerSetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CErrorHandler * pErrorHandler = (CErrorHandler*)pHandle;

		return pErrorHandler->SetParameter(uID, pParam);
	}

	VO_U32 VO_API voStremingERRORHandlerGetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CErrorHandler * pErrorHandler = (CErrorHandler*)pHandle;

		return pErrorHandler->GetParameter(uID, pParam);
	}

	VO_U32 VO_API voStremingERRORHandlerSetStop(VO_PTR pHandle, VO_BOOL bStop )
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CErrorHandler * pErrorHandler = (CErrorHandler*)pHandle;

		pErrorHandler->SetStop( bStop );
		return VO_RET_SOURCE2_OK;
	}
#if defined __cplusplus
}
#endif

#if defined __cplusplus
extern "C" {
#endif

	VO_S32 VO_API voGetStreamingErrorHandlerAPI (VO_StreamingERRORHandler_API * pErrorHandlerHandle, VO_U32 uFlag)
	{
		if(pErrorHandlerHandle)
		{
			pErrorHandlerHandle->Init				= voStremingERRORHandlerInit;
			pErrorHandlerHandle->Uninit				= voStremingERRORHandlerUninit;
			pErrorHandlerHandle->CheckEvent			= voStremingERRORHandlerCheckEvent;
			pErrorHandlerHandle->SetStop			= voStremingERRORHandlerSetStop;
			pErrorHandlerHandle->SetParameter		= voStremingERRORHandlerSetParameter;
			pErrorHandlerHandle->GetParameter		= voStremingERRORHandlerGetParameter;
		}	

		return VO_ERR_NONE;
	}
#if defined __cplusplus
}
#endif