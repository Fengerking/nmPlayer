/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		StreamingDownloader.cpp

	Contains:	CStreamingDownloader class file

	Written by:	Aiven

	Change History (most recent first):
	2013-09-02		Aiven			Create file

*******************************************************************************/

#include "SDownloaderLog.h"
#include "CSourceIOUtility.h"
#include "StreamingDownloader.h"
#include "voToolUtility.h"
#include "CDataBox.h"
#include "voStreamingDownloader.h"
#include "voProgramInfo.h"
#include "voToolUtility.h"
#include "voCheck.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


typedef VO_VOID ( VO_API * pvoGetSourceIOAPI)( VO_SOURCE2_IO_API * ptr_api );


CStreamingDownloader::CStreamingDownloader()
:m_pDownloadlist(NULL)
,m_pTrunkdownloader(NULL)
,m_pIO(NULL)
,m_pIOHttpHeader(NULL)
,m_pIOHttpProxy(NULL)
,m_pVologCB(NULL)
,m_bStop(VO_FALSE)
,m_bManifestFlag(VO_TRUE)
,m_nDuration(0)
,m_eStatus(STATUS_IDLE)
,m_hCheck(NULL)
{
	m_sUpdateCallback.pUserData = this;
	m_sUpdateCallback.SendEvent = OnUpdateCallback;

	memset(m_WorkPath, 0x0, sizeof(m_WorkPath));


}

CStreamingDownloader::~CStreamingDownloader()
{

	Reset();
}

VO_U32 CStreamingDownloader::Init(VO_SOURCE2_EVENTCALLBACK* pCallback, VO_SOURCE2_INITPARAM * pParam)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	SD_LOGI("+Init");

	if(pParam){
		SD_LOGI("lisence check!");
		VO_U32 ret = voCheckLibInit(&m_hCheck,VO_INDEX_SRC_DOWNLOADER,VO_LCS_WORKPATH_FLAG,0,pParam->strWorkPath);
		if(ret != VO_ERR_NONE)
		{
			if(m_hCheck != NULL)
				voCheckLibUninit(m_hCheck);
			m_hCheck = NULL;
			return ret;
		}
	}
	
	memset(m_WorkPath, 0x0, sizeof(m_WorkPath));
	if(pParam&&pParam->strWorkPath){
		memcpy(m_WorkPath, (VO_CHAR*)pParam->strWorkPath, strlen((VO_CHAR*)pParam->strWorkPath));
	}

	if(VO_RET_SOURCE2_OK != CheckStatus(STATUS_IDLE)){
		return VO_RET_SOURCE2_FAIL;
	}

	m_sEventCallBack.pUserData=pCallback->pUserData;
	m_sEventCallBack.SendEvent=pCallback->SendEvent;

	SD_LOGI("m_WorkPath=%s", m_WorkPath);
	if(!m_pIO)
	{
//		SetLibOperator( (VO_LIB_OPERATOR*)m_info.GetLibOp() );
		SetWorkPath(m_WorkPath);
		vostrcpy( m_szDllFile , _T("voSourceIO") );
		vostrcpy( m_szAPIName , _T("voGetSourceIOAPI") );
		
		if(LoadLib(NULL) == 0)
		{
			SD_LOGE ("Load IO fail");
			return VO_RET_SOURCE2_FAIL;
		}
		
		pvoGetSourceIOAPI getapi = (pvoGetSourceIOAPI)m_pAPIEntry;
		
		if( !getapi )
		{
			SD_LOGE("Can not get IO API from IO Module!" );
			return VO_RET_SOURCE2_FAIL;
		}
		m_pIO = new VO_SOURCE2_IO_API;
		getapi( m_pIO );
		
		m_pIO->SetParam( m_pIO->hHandle, VO_PID_SOURCE2_WORKPATH, ( VO_PTR )m_WorkPath);
		if(m_pVologCB){
			m_pIO->SetParam( m_pIO->hHandle, VO_PID_COMMON_LOGFUNC,  m_pVologCB);
		}
		m_pIO->SetParam( m_pIO->hHandle, VO_SOURCE2_IO_PARAMID_HTTPHEADINFO, (VO_PTR) m_pIOHttpHeader );
		m_pIO->SetParam( m_pIO->hHandle, VO_SOURCE2_IO_PARAMID_HTTPPROXYINFO, (VO_PTR) m_pIOHttpProxy );
		
		SD_LOGI("-Load SourceIO");
	}

	if(VO_RET_SOURCE2_OK == ret){
		ret = SwitchToNextStatus(STATUS_INIT);
	}

	SD_LOGI("-Init---ret=%lu", ret);

	return ret;
}

VO_U32 CStreamingDownloader::Uninit()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	SD_LOGI("+Uninit");

	if(m_hCheck != NULL)
	{
		voCheckLibUninit(m_hCheck);
		m_hCheck = NULL;
	}
	
	if(VO_RET_SOURCE2_OK != CheckStatus(STATUS_INIT)){
		return VO_RET_SOURCE2_FAIL;
	}


	if(m_pIO){
		delete m_pIO;
		m_pIO = NULL;
	}
	
	if(VO_RET_SOURCE2_OK == ret){
		ret = SwitchToNextStatus(STATUS_IDLE);
	}

	SD_LOGI("-Uninit---ret=%lu", ret);

	return ret;
}

VO_U32 CStreamingDownloader::Open(VO_PTR pSource, VO_U32 uFlag, VO_PTCHAR pLocalDir)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	SD_LOGI( "+Open" );

	if(VO_RET_SOURCE2_OK != CheckStatus(STATUS_INIT)){
		return VO_RET_SOURCE2_FAIL;
	}

	Reset();

	SD_LOGI("URL:%s", pSource);
	memcpy( m_sPlaylistData.szUrl, (VO_CHAR*)pSource, strlen((VO_CHAR*)pSource));

	SD_LOGI("URL:%s", pLocalDir);
	strcpy((char*)m_LocalPath, (char*)pLocalDir);

	if(VO_FLAG_STREAMDOWNLOADER_ASYNCOPEN&uFlag){
		vo_thread::begin();
	}
	else{
		ret = AsyncOpen();
	}
	
	SD_LOGI( "-Open---ret=%x", ret);

	return ret;
}

void CStreamingDownloader::thread_function()
{
	set_threadname((char*) "AyncOpen" );
	AsyncOpen();
	SD_LOGI( "AyncOpen Exit!" );
}


VO_U32 CStreamingDownloader::AsyncOpen()
{
	VO_U32 ret = VO_RET_SOURCE2_OPENFAIL;
	VO_ADAPTIVESTREAMPARSER_STREAMTYPE adaptivestream_type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN;
	
	VO_DATABOX_CALLBACK DataBox_CallBack;
	CDataBox	databox;
	
	SD_LOGI("+AsyncOpen");
	DataBox_CallBack.MallocData = databox.MallocData;
	DataBox_CallBack.pUserData = (VO_PTR)&databox;
	m_sPlaylistData.pReserve = (VO_PTR)&DataBox_CallBack;
	m_sPlaylistData.pData = NULL;
	m_sPlaylistData.uDataSize = 0;

	VOOSMP_SRC_SOURCE_FORMAT type = VOOSMP_SRC_AUTO_DETECT;
	IsLinkSupport(m_sPlaylistData.szUrl, type);

	switch(type)
	{
	case VOOSMP_SRC_FFSTREAMING_HLS:
		ret = VO_RET_SOURCE2_OK;
		break;
	case VOOSMP_SRC_AUTO_DETECT:
		SD_LOGI("SDError::::Not support Local link!");		
		m_sEventCallBack.SendEvent(m_sEventCallBack.pUserData, VO_EVENTID_STREAM_ERR_NOTSUPPORT_LOCAL_FAIL, 0, 0);		
		break;
	default:
		break;
	}

	if(VO_RET_SOURCE2_OK != ret){
		SD_LOGI("this link doesn't support!");
		return VO_RET_SOURCE2_OPENFAIL;
	}

	if(VO_RET_SOURCE2_OK == ret){
		ret = DownloadItem_III(m_pIO, NULL, NULL,&m_sPlaylistData, &m_bStop);
	}

	if(VO_RET_SOURCE2_OK != ret ){
		SD_LOGI("SDError::::Download Manifest failed!");		
		m_sEventCallBack.SendEvent(m_sEventCallBack.pUserData, VO_EVENTID_STREAM_ERR_DOWNLOADMANIFEST_FAIL, 0, 0);
		return VO_RET_SOURCE2_OPENFAIL;
	}

	CheckStreamingType(&m_sPlaylistData, &adaptivestream_type);
	if(VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS != adaptivestream_type){
		SD_LOGI("Not HLS link!--adaptivestream_type=%x", adaptivestream_type);
		return VO_RET_SOURCE2_OPENFAIL;
	}

	if(VO_RET_SOURCE2_OK == ret){
		AddManifestInfo(&m_sPlaylistData);
		m_pDownloadlist = new CDownloadList(adaptivestream_type ,  NULL, m_WorkPath, m_pVologCB);
		m_pTrunkdownloader = new CChunkDownloader(m_pDownloadlist, this, &m_sUpdateCallback, m_pIO, m_LocalPath);
	}

	if(VO_RET_SOURCE2_OK == ret){
		ret = m_pDownloadlist->Open(&m_sPlaylistData, &m_sUpdateCallback);
	}

	if(VO_RET_SOURCE2_OK != ret){
		SD_LOGI("StreamParser Open failed!");
		
		m_pDownloadlist->Close();
		delete m_pDownloadlist;
		delete m_pTrunkdownloader;
		return VO_RET_SOURCE2_OPENFAIL;
	}

	if(VO_RET_SOURCE2_OK == ret){
		ret = IsTypeSupport();
	}

	if(VO_RET_SOURCE2_OK != ret){
		SD_LOGI("SDError::::Not support live!");
		m_sEventCallBack.SendEvent(m_sEventCallBack.pUserData, VO_EVENTID_STREAM_ERR_NOTSUPPORT_LIVE_FAIL, 0, 0);
		
		m_pDownloadlist->Close();
		delete m_pDownloadlist;
		delete m_pTrunkdownloader;
		return VO_RET_SOURCE2_OPENFAIL;
	}

	if(VO_RET_SOURCE2_OK == ret){
		ret = SwitchToNextStatus(STATUS_OPEN);
	}
	if(VO_RET_SOURCE2_OK == ret){
		ret = m_sEventCallBack.SendEvent(m_sEventCallBack.pUserData, VO_EVENTID_STREAMDOWNLOADER_OPENCOMPLETE, 0, 0);
	}

	ret = (VO_RET_SOURCE2_OK == ret) ? VO_RET_SOURCE2_OK: VO_RET_SOURCE2_OPENFAIL;

	SD_LOGI("-AsyncOpen---ret=%x", ret);
	return ret;
}

VO_U32 CStreamingDownloader::Close()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	SD_LOGI("+Close");

	if(VO_RET_SOURCE2_OK != CheckStatus(STATUS_OPEN)){
		return VO_RET_SOURCE2_FAIL;
	}

	m_pDownloadlist->Close();

	m_bStop = VO_TRUE;

	if(m_pDownloadlist){
		delete m_pDownloadlist;
		m_pDownloadlist = NULL;
	}

	if(m_pTrunkdownloader){
		delete m_pTrunkdownloader;
		m_pTrunkdownloader = NULL;
	}


	if(VO_RET_SOURCE2_OK == ret){
		ret = SwitchToNextStatus(STATUS_INIT);
	}

	SD_LOGI("-Close---ret=%x", ret);

	return ret;
}


VO_U32 CStreamingDownloader::StartDownload()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	SD_LOGI("+StartDownload");

	if(VO_RET_SOURCE2_OK != CheckStatus(STATUS_OPEN)){
		return VO_RET_SOURCE2_FAIL;
	}

	
	ret = m_pDownloadlist->GenerateDownloadList();

	if(VO_RET_SOURCE2_OK == ret){
		ret = m_pTrunkdownloader->Start();
	}

	if(VO_RET_SOURCE2_OK == ret){
		ret = SwitchToNextStatus(STATUS_START);
	}

	SD_LOGI("-StartDownload---ret=%x", ret);
	
	return ret;

}

VO_U32 CStreamingDownloader::StopDownload()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	
	SD_LOGI("+StopDownload");

	if(VO_RET_SOURCE2_OK != CheckStatus(STATUS_START)){
		return VO_RET_SOURCE2_FAIL;
	}
	
	ret = m_pTrunkdownloader->Stop();

	if(VO_RET_SOURCE2_OK == ret){
		ret = SwitchToNextStatus(STATUS_OPEN);
	}

	SD_LOGI("-StopDownload---ret=%x", ret);

	return ret;

}
/*
VO_U32 CStreamingDownloader::GetProgramInfo(VO_SOURCE2_PROGRAM_INFO** ppProgramInfo)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	VO_DATASOURCE_PROGRAM_INFO* pProgramInfo = NULL;
	
	*ppProgramInfo = m_pProgramInfo;
	return ret;
}
*/
VO_U32 CStreamingDownloader::SelectStream(VO_U32 nStreamID)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	SD_LOGI("+SelectStream");
	
	ret = m_pDownloadlist->SelectStream(nStreamID);
	
	m_bManifestFlag = VO_TRUE;

	SD_LOGI("-SelectStream---ret=%x", ret);
	
	return ret;


}

VO_U32 CStreamingDownloader::GetDuration(VO_U64* pDuration)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	SD_LOGI("+GetDuration");

	ret = m_pDownloadlist->GetDuration(pDuration);
	
	SD_LOGI("-GetDuration---ret=%x, pDuration=%llu", ret, *pDuration);
	return ret;
}

VO_U32 CStreamingDownloader::SelectTrack(VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE nType)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	SD_LOGI("+SelectTrack");
	
	ret = m_pDownloadlist->SelectTrack(nTrackID, nType);

	m_bManifestFlag = VO_TRUE;

	SD_LOGI("-SelectTrack---ret=%x", ret);
	
	return ret;
}


VO_U32 CStreamingDownloader::SetParam( VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	switch(nParamID)
	{
		case VO_PID_SOURCE2_HTTPHEADER:
			{
				m_pIOHttpHeader = (VO_SOURCE2_HTTPHEADER *)pParam;
				if(m_pIO)
					m_pIO->SetParam(0, VO_SOURCE2_IO_PARAMID_HTTPHEADINFO,pParam );
			}
			break;
		case VO_PID_SOURCE2_HTTPPROXYINFO:
			{
				m_pIOHttpProxy = (VO_SOURCE2_HTTPPROXY *)pParam;
				if(m_pIO)
					m_pIO->SetParam(0, VO_SOURCE2_IO_PARAMID_HTTPPROXYINFO,pParam );
			}
			break;
		case VO_PID_SOURCE2_WORKPATH:
			{				
				memset(m_WorkPath, 0x0, sizeof(m_WorkPath));
				memcpy(m_WorkPath, (VO_CHAR*)pParam, strlen((VO_CHAR*)pParam));
			}
			break;
		case VO_PID_COMMON_LOGFUNC:
			{
				m_pVologCB = (VO_LOG_PRINT_CB *)pParam;
//				if(m_pVologCB){
//					vologInit (m_pVologCB->pUserData, m_pVologCB->fCallBack);
//				}
			}
			break;

	}

	return ret;
}

VO_U32 CStreamingDownloader::GetParam( VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	return ret;
}

VO_S32 CStreamingDownloader::OnUpdateCallback(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	CStreamingDownloader* ptr = (CStreamingDownloader*)pUserData;

	switch (nID)
	{
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMRESET:
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMCHANGED:
		{
			SD_LOGI("VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_PROGRAMRESET");
			_PROGRAM_INFO *pInfo = (_PROGRAM_INFO*)nParam1;
			ptr->SetProgramInfo(pInfo);
		}
		break;
	case VO_EVENTID_SOURCE2_OPENCOMPLETE:
		SD_LOGI("VO_EVENTID_SOURCE2_OPENCOMPLETE");
		break;
	case VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_NEEDPARSEITEM:
		SD_LOGI("VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_NEEDPARSEITEM");
		{
			VO_ADAPTIVESTREAM_PLAYLISTDATA * pData = ( VO_ADAPTIVESTREAM_PLAYLISTDATA * )nParam1;
			if(pData )
			{
				ret = DownloadItem_III(ptr->m_pIO, NULL, NULL,pData, &ptr->m_bStop);
				if( ret == VO_RET_SOURCE2_OK)
				{	
					ptr->AddManifestInfo(pData);
				}else{
					if(ptr->m_sEventCallBack.SendEvent && ptr->m_sEventCallBack.pUserData){
						ptr->m_sEventCallBack.SendEvent(ptr->m_sEventCallBack.pUserData, VO_EVENTID_STREAM_ERR_DOWNLOADMANIFEST_FAIL, 0, 0);
					}		
				}
			}
			break;
		}
	case DOWNLOADER_EVENT_END:
		SD_LOGI("DOWNLOADER_EVENT_END");
		if(ptr->m_sEventCallBack.SendEvent && ptr->m_sEventCallBack.pUserData){
			ptr->m_sEventCallBack.SendEvent(ptr->m_sEventCallBack.pUserData, VO_EVENTID_STREAMDOWNLOADER_END, 0, 0);
		}
		break;
	case DOWNLOADER_EVENT_MANIFESTUPDATE:
		if(ptr->m_bManifestFlag){
			SD_LOGI("VO_EVENTID_STREAMDOWNLOADER_MANIFEST_OK---strMaster=%s",(VO_CHAR*)nParam1);
			ptr->m_sEventCallBack.SendEvent(ptr->m_sEventCallBack.pUserData, VO_EVENTID_STREAMDOWNLOADER_MANIFEST_OK, nParam1, 0);
			ptr->m_bManifestFlag = VO_FALSE;
			ptr->GetDuration(&ptr->m_nDuration);
		}else{
			PROGRESS_INFO info;
			memset(&info, 0x0, sizeof(PROGRESS_INFO));
			info.TotalDuration = (VO_U32)(ptr->m_nDuration);
			info.CurrDuration = (0 == nParam2) ? info.TotalDuration : ((VO_U32)(*((VO_U64*)nParam2)));
			SD_LOGI("DOWNLOADER_EVENT_MANIFESTUPDATE---CurrDuration=%lu, TotalDuration=%lu",info.CurrDuration, info.TotalDuration);
			ptr->m_sEventCallBack.SendEvent(ptr->m_sEventCallBack.pUserData, VO_EVENTID_STREAMDOWNLOADER_MANIFEST_UPDATE, (VO_U32)&info, 0);
		}
		break;
	case DOWNLOADER_EVENT_DOWNLOADCHUNK_FAIL:
		SD_LOGI("SDError::DOWNLOADER_EVENT_DOWNLOADCHUNK_FAIL");
		if(ptr->m_sEventCallBack.SendEvent && ptr->m_sEventCallBack.pUserData){
			ptr->m_sEventCallBack.SendEvent(ptr->m_sEventCallBack.pUserData, VO_EVENTID_STREAM_ERR_DOWNCHUNK_FAIL, 0, 0);
		}				
		break;
	case DOWNLOADER_EVENT_WRITECHUNK_FAIL:
		SD_LOGI("SDError::DOWNLOADER_EVENT_WRITECHUNK_FAIL");
		if(ptr->m_sEventCallBack.SendEvent && ptr->m_sEventCallBack.pUserData){
			ptr->m_sEventCallBack.SendEvent(ptr->m_sEventCallBack.pUserData, VO_EVENTID_STREAM_ERR_WRITECHUNK_FAIL, 0, 0);
		}				
		break;
	case DOWNLOADER_EVENT_GENERATEMANIFEST_FAIL:
		SD_LOGI("SDError::DOWNLOADER_EVENT_GENERATEMANIFEST_FAIL");
		if(ptr->m_sEventCallBack.SendEvent && ptr->m_sEventCallBack.pUserData){
			ptr->m_sEventCallBack.SendEvent(ptr->m_sEventCallBack.pUserData, VO_EVENTID_STREAM_ERR_GENERATE_MANIFEST_FAIL, 0, 0);
		}
		break;
	case DOWNLOADER_EVENT_WRITEMANIFEST_FAIL:
		SD_LOGI("SDError::DOWNLOADER_EVENT_WRITEMANIFEST_FAIL");		
		if(ptr->m_sEventCallBack.SendEvent && ptr->m_sEventCallBack.pUserData){
			ptr->m_sEventCallBack.SendEvent(ptr->m_sEventCallBack.pUserData, VO_EVENTID_STREAM_ERR_WRITECHUNK_FAIL, 0, 0);
		}
		break;
	}



	return ret;
}


VO_U32 CStreamingDownloader::CheckStreamingType( VO_ADAPTIVESTREAM_PLAYLISTDATA * pData ,VO_ADAPTIVESTREAMPARSER_STREAMTYPE* pType)
{
	VO_ADAPTIVESTREAMPARSER_STREAMTYPE	Type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN;
	
	VO_CHAR * ptr_buffer = (VO_CHAR*)pData->pData;
	VO_U32 size = pData->uDataSize;

	// for UTF-8 text, 0xEF 0xBB 0xBF will at the head, East 20130409
	char * pFind = strstr(ptr_buffer, "#EXTM3U");
	if(pFind && ((pFind == ptr_buffer) || (pFind == ptr_buffer + 3 && (VO_BYTE)ptr_buffer[0] == 0xEF && (VO_BYTE)ptr_buffer[1] == 0xBB && (VO_BYTE)ptr_buffer[2] == 0xBF)))
		Type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS;
	else if( CheckString( ptr_buffer , size, "<SmoothStreamingMedia" ) )
		Type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS;
	else if( CheckString( ptr_buffer , size, "<MPD" ) )
		Type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH;
	else 
		Type = VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN;

	SD_LOGI(" StreamType :%x", Type );
	
	if( Type == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_UNKOWN )
		return VO_RET_SOURCE2_NOIMPLEMENT;
	
	*pType = Type;
	return VO_RET_SOURCE2_OK;
}


VO_U32 CStreamingDownloader::AddManifestInfo( VO_ADAPTIVESTREAM_PLAYLISTDATA* pPlaylistdata)
{
	VO_U32 nResult= VO_RET_SOURCE2_OK;

	MANIFEST_INFO* ptemp = new MANIFEST_INFO;
	memset(ptemp, 0x0, sizeof(MANIFEST_INFO));

	memcpy(ptemp->szRootUrl, pPlaylistdata->szRootUrl, strlen(pPlaylistdata->szRootUrl));
	memcpy(ptemp->szUrl, pPlaylistdata->szUrl, strlen(pPlaylistdata->szUrl));

	ptemp->pManifestData = (VO_PBYTE)new VO_CHAR[pPlaylistdata->uDataSize];
	memcpy(ptemp->pManifestData, pPlaylistdata->pData, pPlaylistdata->uDataSize);
	ptemp->nLength = pPlaylistdata->uDataSize;

	list_T<MANIFEST_INFO *>::iterator iter;
	for(iter = m_nManifestList.begin(); iter != m_nManifestList.end(); ++iter)
	{
		MANIFEST_INFO *pInfo = (MANIFEST_INFO*)(*iter);

		if(!strcmp(pPlaylistdata->szRootUrl, pInfo->szRootUrl) && 
			!strcmp(pPlaylistdata->szUrl, pInfo->szUrl)){

			if (pInfo->pManifestData){
				delete pInfo->pManifestData;
				pInfo->pManifestData=NULL;
			}
			delete pInfo;
			m_nManifestList.remove(iter);
			iter = m_nManifestList.begin();
		}
	}

	m_nManifestList.push_back(ptemp);

	for(iter = m_nManifestList.begin(); iter != m_nManifestList.end(); ++iter)
	{
		MANIFEST_INFO *pInfo = (MANIFEST_INFO*)(*iter);
		SD_LOGI("**************start******************");
		SD_LOGI("RootUrl=%s", pInfo->szRootUrl);
		SD_LOGI("Url=%s", pInfo->szUrl);
		SD_LOGI("**************end******************");
	}

	return nResult;
}

VO_U32 CStreamingDownloader::GetManifestInfo( list_T<MANIFEST_INFO*>** ppManifestList)
{
	*ppManifestList = &m_nManifestList;

	return VO_RET_SOURCE2_OK;
}

VO_U32 CStreamingDownloader::ReleaseManifestInfo(list_T<MANIFEST_INFO*>*	pInfolist)
{
	VO_U32 ret =  VO_RET_SOURCE2_OK;

	list_T<MANIFEST_INFO *>::iterator trackiter;
	for(trackiter = pInfolist->begin(); trackiter != pInfolist->end(); ++trackiter)
	{
		MANIFEST_INFO *pInfo = (MANIFEST_INFO*)(*trackiter);

		if(pInfo->pManifestData){
			delete pInfo->pManifestData;
			pInfo->pManifestData = NULL;
		}
		
		delete pInfo;
	}
	pInfolist->clear();

	return ret;
}

VO_U32 CStreamingDownloader::CommetSelection(VO_U32 bSelect)
{
	SD_LOGI("@@@### CommetSelection---bSelect=%d", bSelect);

	if(VO_RET_SOURCE2_OK != CheckStatus(STATUS_OPEN)){
		return VO_RET_SOURCE2_FAIL;
	}

	
	if(bSelect)
	{
		VOOSMP_SRC_CURR_TRACK_INDEX mCurIndex;		
		VO_U32 nRC = GetCurMediaTrack(&mCurIndex);
		if(nRC) return nRC;
		
		VO_S32 nAudioSelect = -1;
		VO_S32 nVideoSelect = -1;
		VO_S32 nSubTitleSelect = -1;
		VO_U32 nStream = 0;

		if(m_nVideoSelected == -2)
		{
			nVideoSelect = mCurIndex.nCurrVideoIdx;
		}
		else
		{
			nVideoSelect = m_nVideoSelected;
		}
        
		if(m_nAudioSelected == -2)
		{
			nAudioSelect = mCurIndex.nCurrAudioIdx;
		}
		else
		{
			nAudioSelect = m_nAudioSelected;
		}

		if(m_nSubtitleSelected == -2)
		{
			nSubTitleSelect = mCurIndex.nCurrSubtitleIdx;
		}
		else
		{
			nSubTitleSelect = m_nSubtitleSelected;
		}

		SD_LOGI ("nVideoSelect %d nAudioSelect %d, nSubTitleSelect %d", nVideoSelect, nAudioSelect, nSubTitleSelect);
		
		nStream = IsAvaialbeIndex(&nAudioSelect, &nVideoSelect, &nSubTitleSelect);

		if(nStream == 0 && m_nVideoSelected != -2)
		{
			nAudioSelect = -1;
			nSubTitleSelect = -1;

			if(m_nAudioSelected >= 0)
			{
				nAudioSelect = m_nAudioSelected;
			}

			if(m_nSubtitleSelected >= 0)
			{
				nSubTitleSelect = m_nSubtitleSelected;
			}
			
			nStream = IsAvaialbeIndex(&nAudioSelect, &nVideoSelect, &nSubTitleSelect);
		}

		if(nStream)
		{
			//VO_U32 nProgramIndex = (nStream&0xFF0000) >> 16;

			//nRC = m_pBaseSource->SelectProgram(nProgramIndex);
			//if(nRC) return nRC;

			SD_LOGI ("nVideoSelect %d nAudioSelect %d, nSubTitleSelect %d", nVideoSelect, nAudioSelect, nSubTitleSelect);

			nVideoSelect = (nVideoSelect < 0)? 0: nVideoSelect;
			nRC = SelectStream(nVideoSelect);

			if(nRC != VO_RET_SOURCE2_OK && nRC != VO_RET_SOURCE2_FAIL) return nRC;

			if(m_nAudioSelected >= 0)
			{
				//if(m_nVideoSelected == -2)
				//{
					SD_LOGI ("nAudioSelect %d, trackID %d", m_nAudioSelected, nAudioSelect);
					nRC = SelectTrack(nAudioSelect, VO_SOURCE2_TT_AUDIO);
					if(nRC != VO_RET_SOURCE2_OK && nRC != VO_RET_SOURCE2_FAIL) return nRC;
				//}
				//else
				//{
				//	SD_LOGI ("nAudioSelect %d, language %s", m_nAudioSelected, m_pAudioTrackLan[m_nAudioSelected]);
				//	nRC = m_pBaseSource->SetParam(VO_PID_SOURCE2_AUDIOLANGUAGE, m_pAudioTrackLan[m_nAudioSelected]);
				//	if(nRC != VO_RET_SOURCE2_OK && (unsigned VO_U32)nRC != VO_RET_SOURCE2_FAIL) return nRC;
				//}
			}

			if(m_nSubtitleSelected >= 0 && m_nSubtitleCount)
			{
				//if(m_nVideoSelected == -2)
				//{
					SD_LOGI ("nSubTitleSelect %d, trackID %d", m_nSubtitleSelected, nSubTitleSelect);
					nRC = SelectTrack(nSubTitleSelect, VO_SOURCE2_TT_SUBTITLE);
					if(nRC != VO_RET_SOURCE2_OK && nRC != VO_RET_SOURCE2_FAIL) return nRC;					
				//}
				//else
				//{
				//	SD_LOGI ("nSubTitleSelect %d, language %s", m_nSubtitleSelected, m_pSubtitleTrackLan[m_nSubtitleSelected]);
				//	nRC = m_pBaseSource->SetParam(VO_PID_SOURCE2_SUBTITLELANGUAGE, m_pSubtitleTrackLan[m_nSubtitleSelected]);
				//	if(nRC != VO_RET_SOURCE2_OK && (unsigned VO_U32)nRC != VO_RET_SOURCE2_FAIL) return nRC;
				//}
			}
#if 0
			if(m_nVideoSelected >= 0 || (m_nVideoSelected == -2 && mCurIndex.nCurrVideoIdx >= 0))
			{
				m_nBAStreamEnable = 0;
			}
			else
			{
				nRC = SelectStream(nVideoSelect);
				if(nRC != VO_RET_SOURCE2_OK && nRC != VO_RET_SOURCE2_FAIL) return nRC;
				m_nBAStreamEnable = 1;
			}
#endif			
		}
		else
		{
			return VOOSMP_ERR_ParamID;
		}
	}

	m_nAudioSelected = -2;
	m_nVideoSelected = -2;
	m_nSubtitleSelected = -2;

	return VO_RET_SOURCE2_OK;
}


VO_VOID CStreamingDownloader::Reset()
{
	SD_LOGI ("Reset");

	AdaptDataSource::Reset();

	memset( &m_sPlaylistData , 0 , sizeof( VO_ADAPTIVESTREAM_PLAYLISTDATA ) );
	memset(m_LocalPath, 0x0, sizeof(m_LocalPath));	
	ReleaseManifestInfo(&m_nManifestList);

	m_bStop = VO_FALSE;
	m_bManifestFlag = VO_TRUE;
	m_nDuration = 0;	
}


VO_U32 CStreamingDownloader::IsTypeSupport()
{
	VO_U32 nResult = VO_RET_SOURCE2_FORMATUNSUPPORT;
	
	if(m_pProgramInfo)
	{
		nResult = (m_pProgramInfo->sProgramType == VO_SOURCE2_STREAM_TYPE_VOD) ? VO_RET_SOURCE2_OK : VO_RET_SOURCE2_FORMATUNSUPPORT;
	}

	SD_LOGI ("IsTypeSupport nResult=%x", nResult);
	return nResult;
}

VO_U32 CStreamingDownloader::IsLinkSupport(VO_CHAR* pUrl, VOOSMP_SRC_SOURCE_FORMAT& type)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;

	if (!strncmp (pUrl,"RTSP://", 6) || !strncmp (pUrl, "rtsp://", 6) || 
		!strncmp (pUrl, "MMS://", 5) || !strncmp (pUrl, "mms://", 5))
	{
		type = VOOSMP_SRC_FFSTREAMING_RTSP;
	}
	else if (!strncmp (pUrl, "HTTP://", 6) || !strncmp (pUrl, "http://", 6) 
		|| !strncmp (pUrl, "HTTPS://", 7) || !strncmp (pUrl, "https://", 7))
	{
		VO_CHAR* p = strstr(pUrl, ".sdp");
		if(!p)
			p = strstr(pUrl, ".SDP");

		if( strstr(pUrl , ".m3u") || strstr(pUrl , ".M3U") )
		{
			type = VOOSMP_SRC_FFSTREAMING_HLS;
			nResult = VO_RET_SOURCE2_OK;
		}
		else if( strstr(pUrl , ".mpd") || strstr(pUrl , ".MPD"))
		{
			type = VOOSMP_SRC_FFSTREAMING_DASH;
		}
		else if(p && ((p + 4 - pUrl) == (int)strlen(pUrl) || p[4] == _T('?')))
		{
			type = VOOSMP_SRC_FFSTREAMING_RTSP;
		}
		else
		{				
			if( strstr(pUrl , "/manifest") || strstr (pUrl , "/MANIFEST") || strstr (pUrl , "/Manifest"))
			{
				type = VOOSMP_SRC_FFSTREAMING_SSS;
			}
			else
			{
				type = VOOSMP_SRC_FFSTREAMING_HTTPPD;
			}
		}
	}else{
		type = VOOSMP_SRC_AUTO_DETECT;
	}


	SD_LOGI ("IsLinkSupport nResult=%lu, type=%x", nResult, type);
	return nResult;
}


VO_U32 CStreamingDownloader::SwitchToNextStatus(SDOWNLOADER_STATUS eStatus)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;

	switch(eStatus)
	{
		case STATUS_IDLE:
			if(STATUS_INIT == m_eStatus){
				m_eStatus = STATUS_IDLE;
				nResult = VO_RET_SOURCE2_OK;
			}
			break;
		case STATUS_INIT:
			if(STATUS_OPEN == m_eStatus || STATUS_IDLE == m_eStatus){
				m_eStatus = STATUS_INIT;
				nResult = VO_RET_SOURCE2_OK;
			}
			break;
		case STATUS_OPEN:
			if(STATUS_INIT == m_eStatus || STATUS_START == m_eStatus){
				m_eStatus = STATUS_OPEN;
				nResult = VO_RET_SOURCE2_OK;
			}
			break;
		case STATUS_START:
			if(STATUS_OPEN == m_eStatus){
				m_eStatus = STATUS_START;
				nResult = VO_RET_SOURCE2_OK;
			}
			break;
		default:
			break;
	}

	SD_LOGI ("CheckStatus---m_eStatus=%lu, eStatus=%lu, nResult=%x", m_eStatus, eStatus, nResult);
	return nResult;

}

VO_U32 CStreamingDownloader::CheckStatus(SDOWNLOADER_STATUS eStatus)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	
	if(m_eStatus == eStatus){
		nResult = VO_RET_SOURCE2_OK;
	}
	
	SD_LOGI ("CheckStatus---m_eStatus=%lu, eStatus=%lu, nResult=%x", m_eStatus, eStatus, nResult);
	return nResult;

}

