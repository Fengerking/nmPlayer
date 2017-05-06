/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		ISS_SmoothStreaming.cpp

	Contains:	ISS_SmoothStreaming class file

	Written by:	Aiven

	Change History (most recent first):
	2012-08-14		Aiven			Create file

*******************************************************************************/
#include "ISS_SmoothStreaming.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "CDataBox.h"
#include "voCheck.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define TIME_INTERVAL	100

ISS_SmoothStreaming::ISS_SmoothStreaming()
:m_nManager(NULL)
,m_pEventCallbackFunc(NULL)
,m_bUpdateEnalbe(VO_FALSE)
,m_pHLicCheck(NULL)
,m_bStartUpdate(VO_FALSE)
{
	VOLOGI("ISS_SmoothStreaming");
	m_nManager = new ISS_ManifestManager();
	memset(&m_nPlaylistData, 0x0, sizeof(m_nPlaylistData));
}

ISS_SmoothStreaming::~ISS_SmoothStreaming()
{
	VOLOGI("~ISS_SmoothStreaming");

	if(m_nManager){
		delete m_nManager;
		m_nManager = NULL;
	}

}

VO_U32 ISS_SmoothStreaming::Init_IIS(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_SOURCE2_EVENTCALLBACK*  pEventCallback, VO_ADAPTIVESTREAMPARSER_INITPARAM * pInitParam)
{
	VO_U32	nResult = VO_RET_SOURCE2_OK;

	VOLOGI("Init_IIS");
	
     
	nResult = voCheckLibInit (&m_pHLicCheck, VO_INDEX_SRC_ISS, VO_LCS_WORKPATH_FLAG,0,pInitParam->strWorkPath); 

	if(nResult != VO_ERR_NONE )
	{
		if(m_pHLicCheck != NULL)
		{
			voCheckLibUninit(m_pHLicCheck);
		}
		VOLOGE("CheckLib Fail!!!  pWorkingPath=%s",pInitParam->strWorkPath);
		
		m_pHLicCheck = NULL;
		return nResult;
	}
    
	if(NULL == pEventCallback || NULL == pData){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	//save the callback event
	m_pEventCallbackFunc = pEventCallback;

	//copy and save the playlist data
	memcpy(&m_nPlaylistData, pData, sizeof(m_nPlaylistData));
	if(NULL == m_nPlaylistData.pData){
		VOLOGE("Leak of memory!");
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	memcpy(m_nPlaylistData.szRootUrl, pData->szNewUrl, sizeof(pData->szNewUrl));
	memcpy(pData->szRootUrl, m_nPlaylistData.szRootUrl, sizeof(m_nPlaylistData.szRootUrl));

	return nResult;
	
}
VO_U32 ISS_SmoothStreaming::Uninit_IIS()
{
	VO_U32	nResult = VO_RET_SOURCE2_OK;
	VOLOGI("Uninit_IIS");
	
	if(m_pHLicCheck != NULL)
	{
		voCheckLibUninit(m_pHLicCheck);
		m_pHLicCheck = NULL;
	}
	return nResult;
}

VO_U32 ISS_SmoothStreaming::Open_IIS()
{
	VOLOGI("+Begin");
	voCAutoLock autolock(&m_Lock);
	
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	m_nManager->Init();

	nResult = m_nManager->ParseManifest(m_nPlaylistData.pData, m_nPlaylistData.uDataSize);
	
	if(VO_RET_SOURCE2_OK == nResult)
	{
		if(m_pEventCallbackFunc != NULL && m_pEventCallbackFunc->SendEvent != NULL){
			nResult = NotifyProgramInfo(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMRESET);
			m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_OPENCOMPLETE, 0, 0);
		}
	}

	//swith to the live point if the stream is live.
	if(m_nManager->Is_Live_Streaming()){
		VO_U64 max = 0xffffffffffffffffll;
		m_nManager->SetPos(&max, VO_ADAPTIVESTREAMPARSER_SEEKMODE_OBSOLUTE);
	}


	VOLOGI("-End---nResult=0x%08x", nResult);

	return nResult;
}

VO_U32 ISS_SmoothStreaming::Close_IIS()
{	
	VOLOGI("+Begin");
	VO_U32	nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);
	
	nResult = m_nManager->Uninit();
	VOLOGI("+End");

	return nResult;
}

VO_U32 ISS_SmoothStreaming::Start_IIS()
{
	VO_U32	nResult = VO_RET_SOURCE2_OK;
	
	VOLOGI("Start_IIS");

	if(m_nManager->Is_Live_Streaming()){
		m_bStartUpdate = VO_TRUE;
		
		StopPlaylistUpdate();

		StartPlaylistUpdate();
	}
	
	return nResult;
}

VO_U32 ISS_SmoothStreaming::Stop_IIS()
{	
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	VOLOGI("Stop_IIS");

	StopPlaylistUpdate();
	m_bStartUpdate = VO_FALSE;
	
	return nResult;

}

VO_U32 ISS_SmoothStreaming::Update_IIS(VO_ADAPTIVESTREAM_PLAYLISTDATA * pData)
{
	if(NULL == pData){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+Begin");

	VO_U32 nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);


	nResult = m_nManager->ParseManifest(pData->pData, pData->uDataSize);

	VOLOGI("-End---nResult=0x%08x", nResult);
	
	return nResult;
}

VO_U32 ISS_SmoothStreaming::GetChunk_IIS(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk )
{
	if(NULL == ppChunk){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+Begin---uID=%lu",uID);

	VO_U32 nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);

	nResult = m_nManager->GetChunk(uID, ppChunk);

	if(VO_RET_SOURCE2_OK == nResult){	
		memset((*ppChunk)->szRootUrl, 0x0, sizeof((*ppChunk)->szRootUrl));
		memcpy((*ppChunk)->szRootUrl, m_nPlaylistData.szRootUrl, sizeof(m_nPlaylistData.szRootUrl));
		VOLOGI("ok---uID=%d, starttime=%llu, url=%s",uID,(*ppChunk)->ullStartTime, (*ppChunk)->szUrl);
	}

	VOLOGI("-End---nResult=0x%08x", nResult);
	
	return nResult;

}

VO_U32 ISS_SmoothStreaming::Seek_IIS(VO_U64*  pTimeStamp, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode)
{
	if(NULL == pTimeStamp){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+Begin---pTimeStamp=%llu, sSeekMode=%lu", *pTimeStamp, sSeekMode);

	VO_U32 nResult = VO_RET_SOURCE2_OK;

	//Stop the update thread
	if(m_nManager->Is_Live_Streaming()){
		StopPlaylistUpdate();
	}
	voCAutoLock autolock(&m_Lock);

	nResult = m_nManager->SetPos(pTimeStamp, sSeekMode);
	

	if(m_nManager->Is_Live_Streaming()){
		StartPlaylistUpdate();
	}
	
	VOLOGI("-End---pTimeStamp=%llu, nResult=0x%08x", *pTimeStamp, nResult);
	return nResult;

}

VO_U32 ISS_SmoothStreaming::GetDuration_IIS(VO_U64 * pDuration)
{
	if(NULL == pDuration){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+Begin");

	VO_U32 nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);
	
	nResult = m_nManager->GetDuration(pDuration);
	
	VOLOGI("-End---pDuration=%llu", *pDuration);
	return nResult;

}

VO_U32 ISS_SmoothStreaming::GetProgramCounts_IIS(VO_U32*  pProgramCounts)
{
	if(NULL == pProgramCounts){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+Begin");

	VO_U32 nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);

	nResult = m_nManager->GetProgramCounts(pProgramCounts);
	
	VOLOGI("-End---pProgramCounts=%lu", *pProgramCounts);
	return nResult;

}

VO_U32 ISS_SmoothStreaming::GetProgramInfo_IIS(VO_U32 nProgramId, _PROGRAM_INFO **ppProgramInfo)
{
	if(NULL == ppProgramInfo){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+Begin---nProgramId=%lu", nProgramId);

	VO_U32 nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);

	nResult = m_nManager->GetProgramInfoByIndex(nProgramId, ppProgramInfo);

//	PrintProgramInfo((*ppProgramInfo));
	VOLOGI("-End---nResult=0x%08x", nResult);
	return nResult;

}

VO_U32 ISS_SmoothStreaming::GetCurTrackInfo_IIS(VO_SOURCE2_TRACK_TYPE sTrackType , _TRACK_INFO ** ppTrackInfo)
{
	if(NULL == ppTrackInfo){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+Begin---sTrackType=%lu", sTrackType);

	VO_U32 nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);
	
	nResult = m_nManager->GetCurTrackInfo(sTrackType, ppTrackInfo);
	
	VOLOGI("-End---nResult=0x%08x", nResult);

	return nResult;

}

VO_U32 ISS_SmoothStreaming::SelectProgram_IIS (VO_U32 uProgramId)
{
	VOLOGI("+Begin---uProgramId=%d", uProgramId);

	VO_U32 nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);
	
	VOLOGI("-End---nResult=0x%08x", nResult);
	return nResult;

}

VO_U32 ISS_SmoothStreaming::SelectStream_IIS(VO_U32 uStreamId, VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos)
{
	VOLOGI("+Begin---uStreamId=%lu", uStreamId);

	VO_U32 nResult = VO_RET_SOURCE2_OK;
		
	//Stop the update thread
	if(m_nManager->Is_Live_Streaming()){
		StopPlaylistUpdate();
	}
	
	voCAutoLock autolock(&m_Lock);
	nResult = m_nManager->SelectStream_IIS(uStreamId, sPrepareChunkPos);

	if(m_nManager->Is_Live_Streaming()){
		StartPlaylistUpdate();
	}

	if(VO_RET_SOURCE2_OK == nResult){
		nResult = NotifyProgramInfo(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMCHANGED);
	}
	
	VOLOGI("-End---nResult=0x%08x", nResult);

	return nResult;
}

VO_U32 ISS_SmoothStreaming::SelectTrack_IIS(VO_U32 nTrackID)
{
	VOLOGI("+Begin---nTrackID=%lu", nTrackID);

	VO_U32 nResult = VO_RET_SOURCE2_OK;
		
	//Stop the update thread
	if(m_nManager->Is_Live_Streaming()){
		StopPlaylistUpdate();
	}
	
	voCAutoLock autolock(&m_Lock);
	nResult = m_nManager->SelectTrack_IIS(nTrackID);

	if(m_nManager->Is_Live_Streaming()){
		StartPlaylistUpdate();
	}

	if(VO_RET_SOURCE2_OK == nResult){
		nResult = NotifyProgramInfo(VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMCHANGED);
	}
	
	VOLOGI("-End---nResult=0x%08x", nResult);	
	return nResult;
}

VO_U32 ISS_SmoothStreaming::GetDRMInfo_IIS(VO_SOURCE2_DRM_INFO** ppDRMInfo )
{
	if(NULL == ppDRMInfo){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+Begin");

	VO_U32 nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);

	nResult = m_nManager->GetDRMInfo(ppDRMInfo);

	VOLOGI("-End---nResult=0x%08x", nResult);	
	return nResult;
}

VO_U32 ISS_SmoothStreaming::GetParam_IIS(VO_U32 nParamID, VO_PTR pParam )
{
	VOLOGI("+Begin---nParamID=%lu",nParamID);
	
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);
	
	nResult = m_nManager->GetParam_IIS(nParamID, pParam);

	VOLOGI("-End---nResult=0x%08x", nResult);	

	return nResult;
}

VO_U32 ISS_SmoothStreaming::SetParam_IIS(VO_U32 nParamID, VO_PTR pParam )
{
	VOLOGI("+Begin---nParamID=%lu",nParamID);

	VO_U32 nResult = VO_RET_SOURCE2_OK;
	voCAutoLock autolock(&m_Lock);
	
	nResult = m_nManager->SetParam_IIS(nParamID, pParam);

	VOLOGI("-End---nResult=0x%08x", nResult);	

	return nResult;
}

VO_U32 ISS_SmoothStreaming::PlayListUpdateForLive()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	VOLOGI("PlayListUpdateForLive----start");
	VO_U32	ulTimeInterval = 0;
	VO_U32 	uinterval = 0;
	VO_U32 ullStartTime = 0;	
	VO_U32 TestStartTime = 0;
	while(m_bUpdateEnalbe)
	{
/*
		while(m_bUpdateEnalbe == VO_TRUE && !m_nManager->IsNeedToUpdate())
		{
			voOS_Sleep( 100 );
		}
*/		
		ullStartTime = voOS_GetSysTime();
			
		if(m_bUpdateEnalbe == VO_TRUE && m_pEventCallbackFunc != NULL && m_pEventCallbackFunc->SendEvent != NULL)
		{
			VO_DATABOX_CALLBACK DataBox_CallBack;
			CDataBox	databox;
			
			DataBox_CallBack.MallocData = databox.MallocData;
			DataBox_CallBack.pUserData = (VO_PTR)&databox;
			m_nPlaylistData.pReserve = (VO_PTR)&DataBox_CallBack;
			m_nPlaylistData.pData = NULL;
			m_nPlaylistData.uDataSize = 0;
			nResult = m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_NEEDPARSEITEM, (VO_U32)(&m_nPlaylistData), 0);
			if(VO_RET_SOURCE2_OK == nResult)
			{
				if(m_bUpdateEnalbe == VO_TRUE){
					VOLOGI("+Begin---theparse manifest");
					
					voCAutoLock autolock(&m_Lock);
					nResult = m_nManager->ParseManifest(m_nPlaylistData.pData, m_nPlaylistData.uDataSize);

					VO_U32 interval = 0;
					VO_U32 tmp = voOS_GetSysTime();
					if(TestStartTime){
						interval = tmp - TestStartTime;
					}
					TestStartTime = tmp;
					VOLOGI("-End--- theparse manifest--- cost time=%lu, interval = %lu", voOS_GetSysTime()- ullStartTime, interval);
				}
			}
		}
		
		ulTimeInterval = 0;
		m_nManager->GetUpdateIntervel(&ulTimeInterval);
		while(m_bUpdateEnalbe == VO_TRUE && ( (voOS_GetSysTime() - ullStartTime) <= ulTimeInterval ) )
		{
			voOS_Sleep(TIME_INTERVAL);
		}

	}
	VOLOGI("PlayListUpdateForLive----end---nResult=%lu",nResult);


	return nResult;

}

void ISS_SmoothStreaming::thread_function()
{
	set_threadname((char*) "Playlist Update" );
	PlayListUpdateForLive();
	VOLOGR( "Update Thread Exit!" );
}

VO_VOID ISS_SmoothStreaming::StopPlaylistUpdate()
{   
	VOLOGI( "+stop_updatethread" );
	m_bUpdateEnalbe = VO_FALSE;
	vo_thread::stop();
	VOLOGI( "-stop_updatethread" );
}

VO_VOID ISS_SmoothStreaming::StartPlaylistUpdate()
{   
	VOLOGI( "+start_updatethread" );

	if(m_bStartUpdate){
		m_bUpdateEnalbe = VO_TRUE;;
		vo_thread::begin();
	}else{
		VOLOGE( "can not strat the update thread until call Start." );
	}
	
	VOLOGI( "-start_updatethread" );
}

VO_U32 ISS_SmoothStreaming::NotifyProgramInfo(VO_U32 EventID)
{
	_PROGRAM_INFO *pProgramInfo = NULL;
	VO_U32	nResult = VO_RET_SOURCE2_OK;
	
	nResult = GetProgramInfo_IIS(0, &pProgramInfo);
	if(VO_RET_SOURCE2_OK == nResult){
		nResult = m_pEventCallbackFunc->SendEvent(m_pEventCallbackFunc->pUserData, EventID, (VO_U32)pProgramInfo, 0);
	}
	
	VOLOGI( "NotifyProgramInfo---nResult=%d", nResult);

	return nResult;
}


VO_BOOL ISS_SmoothStreaming::GetLicState()
{
	if(m_pHLicCheck != NULL)
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}


