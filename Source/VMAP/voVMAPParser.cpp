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
#include "voVMAPParser.h"
#include "CVMAPParser.h"
#include "voLog.h"
#include "voSource2.h"
#include "fMacros.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif

	VO_U32 VO_API voVMAPParserOpen(VO_PTR * ppHandle, VO_TCHAR *pWorkPath)
	{
		CVMAPParser* pParser = NULL;
		pParser = new CVMAPParser();

		if (!pParser)
			return VO_RET_SOURCE2_OPENFAIL;

		VO_U32 ret = pParser->Open(pWorkPath);
		if(ret != VO_RET_SOURCE2_OK)
		{
			SAFE_DELETE(pParser);
			return ret;
		}
		*ppHandle = pParser;
		return VO_RET_SOURCE2_OK;
	}

	
	VO_U32 VO_API voVMAPParserClose(VO_PTR pHandle)
	{
		if(!pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CVMAPParser* pParser = (CVMAPParser*)pHandle;
		pParser->Close();
		delete pParser;

		return VO_RET_SOURCE2_OK;
	}

	VO_U32 VO_API voVMAPParserProcess(VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize)
	{
		if(!pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CVMAPParser* pParser = (CVMAPParser*)pHandle;
		VO_U32 ret = pParser->Process(pBuffer,uSize);
		return ret;
	}

	VO_U32 VO_API voVMAPParserGetData(VO_PTR pHandle,  VOAdInfo** ppData)
	{
		if(!pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CVMAPParser* pParser = (CVMAPParser*)pHandle;
		VO_U32 ret = pParser->GetData(ppData);
		return ret;
	}

	VO_U32 VO_API voVMAPParserSetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CVMAPParser* pParser = (CVMAPParser*)pHandle;
		VO_U32 ret = pParser->SetParam(uID,pParam);
		return ret;
	}

	VO_U32 VO_API voVMAPParserGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CVMAPParser* pParser = (CVMAPParser*)pHandle;
		VO_U32 ret = pParser->GetParam(uID,pParam);
		return ret;
	}
#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	VO_S32 VO_API voGetVMAPParserAPI(VO_VMAP_PARSER_API* pHandle)
	{
		pHandle->Open		= voVMAPParserOpen;
		pHandle->Close		= voVMAPParserClose;
		pHandle->Process	= voVMAPParserProcess;
		pHandle->GetData	= voVMAPParserGetData;
		pHandle->SetParam	= voVMAPParserSetParam;
		pHandle->GetParam	= voVMAPParserGetParam;

		return VO_RET_SOURCE2_OK;
	}

#if defined __cplusplus
}
#endif ///<__cplusplus

