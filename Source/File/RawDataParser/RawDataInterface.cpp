	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2011			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		RawDataInterface.cpp

	Contains:	raw data file parser interface.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2011-11-21		Rodney		Create file

*******************************************************************************/


#ifdef  _RAW_DATA_PARSER
	#include "voType.h"
	#include "CBaseReader.h"
	#include "H264RawDataInterface.h"
	#include "PCMRawDataInterface.h"
	#include "H265RawDataInterface.h"
#ifndef _SOURCE2
	VO_U32 g_dwFRModuleID = VO_INDEX_SRC_RAWDATA;
#endif
#endif //_RAW_DATA_PARSER
#define LOG_TAG "RawDataInterface"
#include "voLog.h"
#ifndef _SOURCE2
VO_HANDLE g_hInst = VO_NULL;
#endif

#if defined __cplusplus
extern "C" {
#endif

    #ifdef _WIN32
		#include <Windows.h>
		#ifndef _LIB
			//BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
			//{   
			//	g_hInst = hModule;
			//	return TRUE;
			//}
		#endif
    #endif	//_WIN32

#ifndef _SOURCE2
    VO_U32 VO_API voFRClose(VO_PTR pHandle)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CBaseReader* pReader = (CBaseReader*)pHandle;
	    VO_U32 rc = pReader->Close();
	    delete pReader;

	    return rc;
    }

    VO_U32 VO_API voFRGetFileInfo(VO_PTR pHandle, VO_SOURCE_INFO* pSourceInfo)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CBaseReader* pReader = (CBaseReader*)pHandle;

	    return pReader->GetInfo(pSourceInfo);
    }

    VO_U32 VO_API voFRGetFileParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
    {
	    if(!pHandle)
		    return VO_ERR_INVALID_ARG;

	    CBaseReader* pReader = (CBaseReader*)pHandle;

	    VOLOGI("+GetParameter. PID : 0x%08x", uID);
	    VO_U32  nRc = pReader->GetParameter(uID, pParam);
	    VOLOGI("-GetParameter. Res: 0x%08x", nRc);

	     return nRc;
    }
#endif

    VO_EXPORT_FUNC VO_S32 VO_API voGetH264RawDataParserAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    {
#ifndef _SOURCE2
	    pReadHandle->Open = voFRH264Open;
	    pReadHandle->Close = voFRClose;
	    pReadHandle->GetSourceInfo = voFRGetFileInfo;
	    pReadHandle->GetTrackInfo = voFRH264GetTrackInfo;
	    pReadHandle->GetSample = voFRH264GetSample;
	    pReadHandle->SetPos = voFRH264SetPos;
	    pReadHandle->SetSourceParam = voFRH264SetFileParam;
	    pReadHandle->GetSourceParam = voFRGetFileParam;
	    pReadHandle->SetTrackParam = voFRH264SetTrackParam;
	    pReadHandle->GetTrackParam = voFRH264GetTrackParam;
#endif

	    return VO_ERR_SOURCE_OK;
    }

    VO_EXPORT_FUNC VO_S32 VO_API voGetH265RawDataParserAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    {
#ifndef _SOURCE2
	    pReadHandle->Open = voFRH265Open;
	    pReadHandle->Close = voFRClose;
	    pReadHandle->GetSourceInfo = voFRGetFileInfo;
	    pReadHandle->GetTrackInfo = voFRH265GetTrackInfo;
	    pReadHandle->GetSample = voFRH265GetSample;
	    pReadHandle->SetPos = voFRH265SetPos;
	    pReadHandle->SetSourceParam = voFRH265SetFileParam;
	    pReadHandle->GetSourceParam = voFRGetFileParam;
	    pReadHandle->SetTrackParam = voFRH265SetTrackParam;
	    pReadHandle->GetTrackParam = voFRH265GetTrackParam;
#endif
	    return VO_ERR_SOURCE_OK;
    }


    VO_EXPORT_FUNC VO_S32 VO_API voGetPCMRawDataParserAPI(VO_SOURCE_READAPI* pReadHandle, VO_U32 uFlag)
    {
#ifndef _SOURCE2
	    pReadHandle->Open = voFRPCMOpen;
	    pReadHandle->Close = voFRClose;
	    pReadHandle->GetSourceInfo = voFRGetFileInfo;
	    pReadHandle->GetTrackInfo = voFRPCMGetTrackInfo;
	    pReadHandle->GetSample = voFRPCMGetSample;
	    pReadHandle->SetPos = voFRPCMSetPos;
	    pReadHandle->SetSourceParam = voFRPCMSetFileParam;
	    pReadHandle->GetSourceParam = voFRGetFileParam;
	    pReadHandle->SetTrackParam = voFRPCMSetTrackParam;
	    pReadHandle->GetTrackParam = voFRPCMGetTrackParam;
#endif
	    return VO_ERR_SOURCE_OK;
    }


#if defined __cplusplus
}
#endif


