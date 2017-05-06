#include "voEVENTHandler.h"
#include "EventHandler.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#if defined __cplusplus
extern "C" {
#endif

	VO_U32 VO_API voStremingEVENTHandlerInit(VO_PTR* ppHandle, VO_SOURCE2_EVENTCALLBACK * pEventCB,VO_ADAPTIVESTREAMPARSER_STREAMTYPE nType, VO_PTR pReserved)
	{
		CEventHandler *pEventHandler = NULL;
		
		//if (pEventHandlerCB->szDRMTYPE)
			pEventHandler = new CEventHandler(pEventCB, nType, pReserved);

		if (NULL == pEventHandler)
			return VO_ERR_OUTOF_MEMORY;

		*ppHandle = pEventHandler;

		return VO_ERR_NONE;
	}

	VO_U32 VO_API voStremingEVENTHandlerUninit(VO_PTR pHandle)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CEventHandler *pEventHandler = (CEventHandler*)pHandle;
		
		delete pEventHandler;
		pEventHandler = NULL;
		return VO_ERR_NONE;
	}
	VO_U32 VO_API voStremingEVENTHandlerCheckEvent(VO_PTR pHandle, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2, VO_PTR pReserved)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;
		CEventHandler *pEventHandler = (CEventHandler*)pHandle;

		return pEventHandler->CheckEvent( nID, nParam1, nParam2, pReserved );

	}
	VO_U32 VO_API voStremingEVENTHandlerSetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CEventHandler * pEventHandler = (CEventHandler*)pHandle;

		return pEventHandler->SetParameter(uID, pParam);
	}

	VO_U32 VO_API voStremingEVENTHandlerGetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CEventHandler * pEventHandler = (CEventHandler*)pHandle;

		return pEventHandler->GetParameter(uID, pParam);
	}

	VO_U32 VO_API voStremingEVENTHandlerSetStop(VO_PTR pHandle, VO_BOOL bStop )
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CEventHandler * pEventHandler = (CEventHandler*)pHandle;

		pEventHandler->SetStop( bStop );
		return VO_RET_SOURCE2_OK;
	}
#if defined __cplusplus
}
#endif

#if defined __cplusplus
extern "C" {
#endif

	VO_S32 VO_API voGetStreamingEVENTHandlerAPI (VO_StreamingEVENTHandler_API * pEventHandlerHandle, VO_U32 uFlag)
	{
		if(pEventHandlerHandle)
		{
			pEventHandlerHandle->Init				= voStremingEVENTHandlerInit;
			pEventHandlerHandle->Uninit				= voStremingEVENTHandlerUninit;
			pEventHandlerHandle->CheckEvent			= voStremingEVENTHandlerCheckEvent;
			pEventHandlerHandle->SetStop			= voStremingEVENTHandlerSetStop;
			pEventHandlerHandle->SetParameter		= voStremingEVENTHandlerSetParameter;
			pEventHandlerHandle->GetParameter		= voStremingEVENTHandlerGetParameter;
		}	

		return VO_ERR_NONE;
	}
#if defined __cplusplus
}
#endif