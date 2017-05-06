/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		ISS_ManifestManager.cpp

	Contains:	ISS_ManifestManager class file

	Written by:	Aiven

	Change History (most recent first):
	2012-08-14		Aiven			Create file

*******************************************************************************/

#include "ISS_ManifestManager.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voToolUtility.h"
#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#define DEFAULT_UPDATE_INTERVEL	2000
#define PERCENT_TRUNK				0.8
#define LIVE_AHEAD_BUFFER_LENGTH	2
#define DefaultMaxDuration			100000000
VO_BOOL replace(char *source,const char *sub,const char *rep )
{
	
	int in,out;
	in = out = 0;
	int lsub = strlen(sub);
	int lsour = strlen(source);
	char tmp[155];
	char *p = source;
	char sourceclone[1024];
	memset(sourceclone,0x00,1024);
	memcpy(sourceclone,source,lsour);
	while(lsour >= lsub)
	{
		memset(tmp,0x00,155);
		memcpy(tmp,p,lsub);
		if(strcmp(tmp,sub)==0)
		{
			out = in +lsub;
			break;
		}
		in ++;
		p++;
		lsour -- ;
	}
	if(out >0 && in != out)
	{
		memset(source, 0 ,strlen(source));
		memcpy(source, sourceclone,in);
		strcat(source,rep);
		strcat(source, &sourceclone[out]);
		return VO_TRUE;
	}
	return VO_FALSE;
}

ISS_ManifestManager::ISS_ManifestManager()
:m_pManifestInfo(NULL)
,m_pManifestParser(NULL)
,m_pDRMInfo(NULL)
,m_pProgramInfo(NULL)
,m_pPlaySession(NULL)
,m_nTrackMode(0)
{
	VOLOGI("ISS_ManifestManager");

	m_pManifestInfo = new SmoothStreamingMedia;
	memset(&m_pManifestInfo->piff, 0x0, sizeof(m_pManifestInfo->piff));
	m_pManifestParser = new ISS_ManifestParser;
//	m_pPlaySession = new PlaySession;

	memset(&m_sAudioChumk, 0x0, sizeof(m_sAudioChumk));
	memset(&m_sVideoChumk, 0x0, sizeof(m_sVideoChumk));
	memset(&m_sTextChumk, 0x0, sizeof(m_sTextChumk));
	
	InitPlaySession();

}

ISS_ManifestManager::~ISS_ManifestManager()
{
	VOLOGI("~ISS_ManifestManager");


	ManifestReset();

	if(m_pPlaySession){
		delete m_pPlaySession;
		m_pPlaySession = NULL;
	}

	if(m_pManifestParser)
	{
		if(m_pManifestInfo)
		{
			m_pManifestParser->ReleaseManifestInfo(m_pManifestInfo);			
			delete m_pManifestInfo;
			m_pManifestInfo = NULL;
		}
		
		delete m_pManifestParser;
		m_pManifestParser = NULL;
	}
	
}

VO_U32 ISS_ManifestManager::Init()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	nResult = m_pManifestParser->Init();

	return nResult;

}

VO_U32 ISS_ManifestManager::Uninit()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	nResult = m_pManifestParser->Uninit();

	return nResult;

}


VO_U32 ISS_ManifestManager::ManifestReset()
{
	VOLOGI("ManifestReset---start");

	if(m_pManifestInfo)
	{
		m_pManifestParser->ReleaseManifestInfo(m_pManifestInfo);			
	}

	if(m_pDRMInfo)
	{
		delete m_pDRMInfo;
		m_pDRMInfo = NULL;
	}
	
	ReleaseProgramInfo();

	VOLOGI("ManifestReset---end");

	return VO_RET_SOURCE2_OK;
}

VO_U32 ISS_ManifestManager::ParseManifest(VO_PBYTE pBuffer,  VO_U32 uSize)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	VO_U32 nBegin = voOS_GetSysTime();

	VOLOGI("ParseManifest---start---uSize=%lu", uSize);

	if(NULL == pBuffer ||0 == uSize ){
		VOLOGE("ParseManifest---error");
		return VO_RET_SOURCE2_FAIL;
	}

	ManifestReset();

	nResult = m_pManifestParser->Parse( pBuffer,  uSize, m_pManifestInfo);

	CreateProgramInfo();
	CreateDrmInfo();

	SyncPlaySession();

#ifdef _VOLOG_INFO
	if(VO_RET_SOURCE2_OK != nResult){
		DoumpTheRawData(pBuffer, uSize);
	}
#endif

	VOLOGR("ParseManifest---end---cost = %lu", voOS_GetSysTime() - nBegin);

	return nResult;

}

VO_U32 ISS_ManifestManager::GetChunk(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk )
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	FragmentItem* pItem = NULL;
	VO_U32	size = 0;
	VO_BOOL IsTrunkDrop = VO_FALSE;
	VO_U64 duration = 0;

	if(!m_pPlaySession){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	nResult = CheckTrackStatus(uID);

	if(VO_RET_SOURCE2_OK == nResult){
		nResult = CheckLookAheadCount(uID);
	}


	if(VO_RET_SOURCE2_OK == nResult)
	{
		switch(uID)
		{
		case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
			{
				memset(&m_sAudioChumk, 0x0, sizeof(m_sAudioChumk));
				*ppChunk = &m_sAudioChumk;	
				pItem = (*m_pPlaySession->audioiter);
				//don't check the trunk drop after seeking, opening,and the duration is 0.
				if(!(m_pPlaySession->curaudiofragment.starttime&COMPARE_FLAG) && m_pPlaySession->curaudiofragment.duration){
					IsTrunkDrop=(pItem->starttime > (m_pPlaySession->curaudiofragment.starttime+ 4./3 * m_pPlaySession->curaudiofragment.duration) ||
						pItem->starttime < (m_pPlaySession->curaudiofragment.starttime+2./3 * m_pPlaySession->curaudiofragment.duration) ) ? VO_TRUE: VO_FALSE;
				}
				memcpy(&m_pPlaySession->curaudiofragment, pItem, sizeof(FragmentItem));
				size = m_pPlaySession->pAudioChunkList->size();
				VOLOGR("GetChunk---AUDIO---curaudiotime = %llu", (m_pPlaySession->curaudiofragment.starttime&(~COMPARE_FLAG))); 
				m_pPlaySession->audioiter++;
				//generate the duration if the default duration is null
				if(m_pPlaySession->audioiter != m_pPlaySession->pAudioChunkList->end()){
					FragmentItem* pNextItem = (*m_pPlaySession->audioiter);					
					duration = (pItem->duration)?pItem->duration:(pNextItem->starttime - pItem->starttime);
				}else{
					duration = DefaultMaxDuration;
				}
			}
			break;
		case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
			{
				memset(&m_sVideoChumk, 0x0, sizeof(m_sVideoChumk)); 	
				*ppChunk = &m_sVideoChumk;
				pItem = (*m_pPlaySession->videoiter);				
				if(!(m_pPlaySession->curvideofragment.starttime&COMPARE_FLAG) && m_pPlaySession->curvideofragment.duration){
					IsTrunkDrop=(pItem->starttime > (m_pPlaySession->curvideofragment.starttime+ 4./3 * m_pPlaySession->curvideofragment.duration) ||
						pItem->starttime < (m_pPlaySession->curvideofragment.starttime+2./3 * m_pPlaySession->curvideofragment.duration) ) ? VO_TRUE: VO_FALSE;
				}
				memcpy(&m_pPlaySession->curvideofragment, pItem, sizeof(FragmentItem));
				size = m_pPlaySession->pVideoChunkList->size();
				VOLOGR("GetChunk---VIDEO---curvideotime = %llu", (m_pPlaySession->curvideofragment.starttime&(~COMPARE_FLAG)));
				m_pPlaySession->videoiter++;
				if(m_pPlaySession->videoiter != m_pPlaySession->pVideoChunkList->end()){
					FragmentItem* pNextItem = (*m_pPlaySession->videoiter);					
					duration = (pItem->duration)?pItem->duration:(pNextItem->starttime - pItem->starttime);
				}else{
					duration = DefaultMaxDuration;
				}
				
			}
			break;
		case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
			{
				memset(&m_sTextChumk, 0x0, sizeof(m_sTextChumk));		
				*ppChunk = &m_sTextChumk;
				pItem = (*m_pPlaySession->Textiter);
				if(!(m_pPlaySession->curtextfragment.starttime&COMPARE_FLAG) && m_pPlaySession->curtextfragment.duration){
					IsTrunkDrop=(pItem->starttime > (m_pPlaySession->curtextfragment.starttime+ 4./3 * m_pPlaySession->curtextfragment.duration) ||
						pItem->starttime < (m_pPlaySession->curtextfragment.starttime+2./3 * m_pPlaySession->curtextfragment.duration) ) ? VO_TRUE: VO_FALSE;
				}
				memcpy(&m_pPlaySession->curtextfragment, pItem, sizeof(FragmentItem));
				size = m_pPlaySession->pTextChunkList->size();
				VOLOGR("GetChunk---SUBTITLE---curtexttime = %llu", (m_pPlaySession->curtextfragment.starttime&(~COMPARE_FLAG)));
				m_pPlaySession->Textiter++;
				if(m_pPlaySession->Textiter != m_pPlaySession->pTextChunkList->end()){
					FragmentItem* pNextItem = (*m_pPlaySession->Textiter);					
					duration = (pItem->duration)?pItem->duration:(pNextItem->starttime - pItem->starttime);
				}else{
					duration = DefaultMaxDuration;
				}
				
			}
			break;
		case VO_SOURCE2_ADAPTIVESTREAMING_AUDIOVIDEO:
		case VO_SOURCE2_ADAPTIVESTREAMING_HEADDATA:
			nResult = VO_RET_SOURCE2_NOIMPLEMENT;
			break;
		default:
			break;
		}
	}



	if (VO_RET_SOURCE2_OK == nResult)
	{
		VOLOGI("GetChunk---streamindex=%ld, index=%ld, size=%lu, nIsVideo=%ld, duration=%llu", pItem->streamindex, pItem->index + 1, size, pItem->nIsVideo, duration);

		memset((*ppChunk)->szUrl, 0x0, sizeof((*ppChunk)->szUrl));
		nResult = GenerateChunkUrlByFragment(pItem,(*ppChunk)->szUrl);
		if(VO_RET_SOURCE2_OK != nResult){
			return nResult;
		}

		(*ppChunk)->Type = uID;

		//	*ppChunk->szRootUrl = ;
		//	*ppChunk->szUrl = ;

		(*ppChunk)->ullChunkOffset = INAVALIBLEU64;
		(*ppChunk)->ullChunkSize = INAVALIBLEU64;

		if(Is_Live_Streaming()){
			(*ppChunk)->ullChunkLiveTime = m_utc_time+ m_cursystem_time - m_basesystem_time;
			(*ppChunk)->ullChunkDeadTime = (*ppChunk)->ullChunkLiveTime + (pItem->index+1)*(duration/GetTimeScaleMs());
			VOLOGI("GetChunk---livetime=%llu,deadtime=%llu",(*ppChunk)->ullChunkLiveTime, (*ppChunk)->ullChunkDeadTime);
		}
		else{
			(*ppChunk)->ullChunkLiveTime = INAVALIBLEU64;
			(*ppChunk)->ullChunkDeadTime = INAVALIBLEU64;
		}

		(*ppChunk)->ullStartTime = pItem->starttime;
		(*ppChunk)->pStartExtInfo = NULL;

		(*ppChunk)->ullDuration = duration;
		(*ppChunk)->ullTimeScale = m_pManifestInfo->timeScale;

		(*ppChunk)->uFlag = 0;
		if(IsTrunkDrop){
			(*ppChunk)->uFlag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_CHUNKDROPBEFORE;			
			VOLOGI("The trunk has been dropped before!");
		}
		
		(*ppChunk)->pFlagData = NULL;

#ifdef _new_programinfo
		(*ppChunk)->sKeyID.uProgramID = 0;

		_STREAM_INFO* pStreamInfo = NULL;
		nResult = GetCurrentSreamInfo(m_pProgramInfo, &pStreamInfo);
		(*ppChunk)->sKeyID.uStreamID = pStreamInfo->uStreamID;

		_TRACK_INFO* pTrackInfo = NULL;
		GetCurTrackInfo(ConvertTrackType(pItem->nIsVideo), &pTrackInfo);
		(*ppChunk)->sKeyID.uTrackID = pTrackInfo->uASTrackID;
		
//		(*ppChunk)->uChunkID = pItem->index;//_VODS_INT32_MAX;
		(*ppChunk)->uChunkID = pItem->starttime;

		VOLOGI("GetChunk---ProgramID=%lu, StreamID=%lu, TrackID=%lu", (*ppChunk)->sKeyID.uProgramID, (*ppChunk)->sKeyID.uStreamID, (*ppChunk)->sKeyID.uTrackID);		
#endif

		switch(uID)
		{
		case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
			if(m_pPlaySession->audioflag){
				(*ppChunk)->uFlag |= m_pPlaySession->audioflag;	
				VOLOGI("GetChunk---AUDIO---NEWFORMAT---flag=%lu", m_pPlaySession->audioflag);
				m_pPlaySession->audioflag = 0;
				m_nTrackMode &= (~AUDIO_TRACK);
			}
			break;
		case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
			if(m_pPlaySession->videoflag){
				(*ppChunk)->uFlag |= m_pPlaySession->videoflag;
				VOLOGI("GetChunk---VIDEO---NEWFORMAT---flag=%lu", m_pPlaySession->videoflag);				
				m_pPlaySession->videoflag = 0;				
				m_nTrackMode &= (~VIDEO_TRACK);				
			}			
			break;
		case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
			if(m_pPlaySession->textflag){
				(*ppChunk)->uFlag |= m_pPlaySession->textflag;
				VOLOGI("GetChunk---SUBTITLE---NEWFORMAT---flag=%lu", m_pPlaySession->textflag);				
				m_pPlaySession->textflag = 0;				
				m_nTrackMode &= (~SUBTILE_TRACK);				
			}
			break;
			
		}

		(*ppChunk)->pPrivateData = NULL;
		(*ppChunk)->pChunkDRMInfo = NULL;
		(*ppChunk)->pReserved = NULL;


	}

//	CheckTheUpdateFlag();


	return nResult;
	
}

VO_U32 ISS_ManifestManager::SetPos( VO_U64* pos, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	StreamIndex*	pStreamIndex = NULL;
	VO_U64	timescale = m_pManifestInfo->timeScale/1000;
	VO_U64 	tmpPos = INAVALIBLEU64;
	VO_U64 	tmpTextPos = 0;
	VO_U64 	tmpResultPos = 0;
	
	FragmentItem* pItem = NULL;

	if(!m_pManifestInfo){
		return VO_RET_SOURCE2_FAIL;
	}

	switch(sSeekMode)
	{
	case VO_ADAPTIVESTREAMPARSER_SEEKMODE_BACKWARD:
		{
			VO_U64 curpos=0;
			if(VO_RET_SOURCE2_OK == GetCurrentDownloadTime(&curpos)){
				tmpPos = curpos-((*pos)* timescale);
			}
		}
		break;
	case VO_ADAPTIVESTREAMPARSER_SEEKMODE_FORWARD:
		{
			VO_U64 curpos=0;
			if(VO_RET_SOURCE2_OK == GetCurrentDownloadTime(&curpos)){
				tmpPos = curpos+((*pos) * timescale);
			}
		}
		break;
	case VO_ADAPTIVESTREAMPARSER_SEEKMODE_OBSOLUTE:
	case VO_ADAPTIVESTREAMPARSER_SEEKMODE_DVRWINDOWPOS:
		{
			VO_U64 curpos=0;
			if(VO_RET_SOURCE2_OK == GetFirstChunkTimeStamp(curpos)){
				tmpPos = curpos+((*pos) * timescale);
			}
		}
		break;
	}

	if(INAVALIBLEU64 == tmpPos){
		return VO_RET_SOURCE2_FAIL;
	}

	VOLOGI("SetPos---tmpPos=%llu,pos=%llu,sSeekMode=%ld",tmpPos, *pos, sSeekMode);

	if(Is_Live_Streaming())
	{
		VO_U64	chunk_duration = 0;
	
		if( VO_RET_SOURCE2_OK != CalcThelivePoint(&tmpTextPos,&chunk_duration)){
			return VO_RET_SOURCE2_FAIL;
		}
		
		//this change only for test
		//tmpPos = 0xf000000000000000;
		
		if(tmpPos >= tmpTextPos){
			tmpPos = tmpTextPos+chunk_duration/2;
		}
	}else{
	
		//out of duration
		if(tmpPos > (VO_U64)m_pManifestInfo->duration*1000){
			return VO_RET_SOURCE2_FAIL;
		}

	}


	memset(&m_pPlaySession->curaudiofragment, 0x0, sizeof(m_pPlaySession->curaudiofragment));
	memset(&m_pPlaySession->curvideofragment, 0x0, sizeof(m_pPlaySession->curvideofragment));
	memset(&m_pPlaySession->curtextfragment, 0x0, sizeof(m_pPlaySession->curtextfragment));

	if(VO_RET_SOURCE2_OK == GetStreamIndexByType(VO_SOURCE2_TT_VIDEO, &pStreamIndex)){
		VO_U64	tempposition = tmpPos;
		
		nResult = SetStreamIndexPos(VO_SOURCE2_TT_VIDEO, &tempposition);
		tmpResultPos = tempposition;
//		tmpTextPos = tmpPos;
		m_pPlaySession->videoflag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;	
		pItem = (FragmentItem*)(*m_pPlaySession->videoiter);
		m_pPlaySession->curvideofragment.starttime = pItem->starttime |COMPARE_FLAG;
	}
	
	if(VO_RET_SOURCE2_OK == GetStreamIndexByType(VO_SOURCE2_TT_AUDIO, &pStreamIndex)){
		VO_U64	tempposition = tmpPos;
		
		nResult = SetStreamIndexPos(VO_SOURCE2_TT_AUDIO, &tempposition);
//		tmpTextPos = (tmpTextPos != 0) ? tmpTextPos : tmpPos;
		tmpResultPos = (tmpResultPos != 0) ? tmpResultPos : tempposition;
		
		m_pPlaySession->audioflag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;	
		pItem = (FragmentItem*)(*m_pPlaySession->audioiter);
		m_pPlaySession->curaudiofragment.starttime = pItem->starttime |COMPARE_FLAG;
	}

	if(VO_RET_SOURCE2_OK == GetStreamIndexByType(VO_SOURCE2_TT_SUBTITLE, &pStreamIndex)){
		VO_U64	tempposition = tmpPos;
		
		nResult = SetStreamIndexPos(VO_SOURCE2_TT_SUBTITLE, &tmpTextPos);
		tmpResultPos = (tmpResultPos != 0) ? tmpResultPos : tempposition;
		m_pPlaySession->textflag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;	
		pItem = (FragmentItem*)(*m_pPlaySession->Textiter);
		m_pPlaySession->curtextfragment.starttime = pItem->starttime |COMPARE_FLAG;
	}

	if(VO_ADAPTIVESTREAMPARSER_SEEKMODE_DVRWINDOWPOS == sSeekMode || VO_ADAPTIVESTREAMPARSER_SEEKMODE_OBSOLUTE == sSeekMode){
		VO_U64 curpos=0;
		GetFirstChunkTimeStamp(curpos);
		
		*pos = (tmpResultPos - curpos)/timescale;
	}else{
		*pos = tmpResultPos/timescale;
	}

	VOLOGI("SetPos---pos=%llu,sSeekMode=%ld",*pos, sSeekMode);

	m_nTrackMode = 0;

	SyncPlaySession();

	return nResult;
}

VO_U32 ISS_ManifestManager::GetDuration(VO_U64 * pDuration)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	if(m_pManifestInfo->islive){
		*pDuration = 0;
	}else{
		*pDuration = m_pManifestInfo->duration/m_pManifestInfo->timeScale*1000;
	}
	
	return nResult;
}

VO_U32 ISS_ManifestManager::GetProgramCounts(VO_U32*  pProgramCounts)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	*pProgramCounts = 1;
	
	return nResult;

}

VO_U32 ISS_ManifestManager::GetProgramInfoByIndex(VO_U32 index, _PROGRAM_INFO **ppProgramInfo)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	
	*ppProgramInfo = m_pProgramInfo;
	
	return nResult;
}

VO_U32 ISS_ManifestManager::GetCurTrackInfo(VO_SOURCE2_TRACK_TYPE TrackType, _TRACK_INFO ** ppTrackInfo)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	_STREAM_INFO* pStreamInfo = NULL;

	if(!m_pProgramInfo){
		return VO_RET_SOURCE2_FAIL;
	}
	
	nResult = GetCurrentSreamInfo(m_pProgramInfo, &pStreamInfo);

	if(VO_RET_SOURCE2_OK == nResult){
		nResult = GetCurrentTrackInfo(pStreamInfo, TrackType, ppTrackInfo);
	}

	return nResult;
}

VO_U32 ISS_ManifestManager::GetDRMInfo(VO_SOURCE2_DRM_INFO** ppDRMInfo )
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	
	*ppDRMInfo = m_pDRMInfo;
	
	return nResult;

}

VO_U32 ISS_ManifestManager::SelectStream_IIS(VO_U32 uStreamId, VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VO_U32 ulIndex = 0;

	for(ulIndex=0; ulIndex<m_pProgramInfo->uStreamCount; ulIndex++)
	{
		if(m_pProgramInfo->ppStreamInfo[ulIndex]->uStreamID == uStreamId)
		{
			if(VO_SOURCE2_SELECT_SELECTED == (VO_SOURCE2_SELECT_SELECTED&m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo)){
				return VO_RET_SOURCE2_OK;
			}

			m_pPlaySession->videoflag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;//(VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE|VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_SMOOTH_ADAPTIONEX);	
			nResult = VO_RET_SOURCE2_OK;
			break;
		}
	}

	if(VO_RET_SOURCE2_OK == nResult)
	{
		for(ulIndex=0; ulIndex<m_pProgramInfo->uStreamCount; ulIndex++)
		{
			if(m_pProgramInfo->ppStreamInfo[ulIndex]->uStreamID == uStreamId)
			{
				m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo |= VO_SOURCE2_SELECT_SELECTED;
				m_pPlaySession->curstream = ulIndex;
				VOLOGI("the current stream bitrate:%d", m_pProgramInfo->ppStreamInfo[ulIndex]->uBitrate);
			}
			else
			{
				m_pProgramInfo->ppStreamInfo[ulIndex]->uSelInfo &= (~VO_SOURCE2_SELECT_SELECTED);
			}
		}
		SyncPlaySession(sPrepareChunkPos);
		
	}


	return nResult;
}

VO_U32 ISS_ManifestManager::SelectTrack_IIS(VO_U32 nTrackID)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_STREAM_INFO* pStreamInfo = NULL;
	_TRACK_INFO* pTrackInfo = NULL;
	VO_U32 ulIndex = 0;

	if(VO_RET_SOURCE2_OK != GetCurrentSreamInfo(m_pProgramInfo, &pStreamInfo)){
		return VO_RET_SOURCE2_FAIL;
	}

	//check the trackid and find the track type;
	_TRACK_INFO* pTargetTrack = NULL;
	for(ulIndex=0; ulIndex < pStreamInfo->uTrackCount; ulIndex++)
	{
		pTargetTrack = pStreamInfo->ppTrackInfo[ulIndex];

#ifdef _new_programinfo
		if(pTargetTrack->uASTrackID == nTrackID)
#else
		if(pTargetTrack->uTrackID == nTrackID)
#endif
		{		
			if(VO_SOURCE2_SELECT_SELECTED == (VO_SOURCE2_SELECT_SELECTED&pTargetTrack->uSelInfo)){
				return VO_RET_SOURCE2_OK;
			}			

			if(VO_SOURCE2_TT_AUDIO == pTargetTrack->uTrackType){
				m_pPlaySession->audioflag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
			}else if(VO_SOURCE2_TT_SUBTITLE == pTargetTrack->uTrackType){
				m_pPlaySession->textflag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
			}
			nResult = VO_RET_SOURCE2_OK;
			break;
		}
	}


	if(VO_RET_SOURCE2_OK == nResult)
	{
		VO_U32 track_index= 0;
		for(ulIndex=0; ulIndex < pStreamInfo->uTrackCount; ulIndex++)
		{
			//only change the  track which type is same.
			if(pStreamInfo->ppTrackInfo[ulIndex]->uTrackType != pTargetTrack->uTrackType){
				continue;
			}
#ifdef _new_programinfo
					if(pStreamInfo->ppTrackInfo[ulIndex]->uASTrackID == pTargetTrack->uASTrackID)
#else
					if(pStreamInfo->ppTrackInfo[ulIndex]->uTrackID == pTargetTrack->uTrackID)
#endif
		
			{
				if(VO_SOURCE2_TT_AUDIO == pTargetTrack->uTrackType){
					m_pPlaySession->curaudiotrack = track_index;
				}else if(VO_SOURCE2_TT_SUBTITLE == pTargetTrack->uTrackType){
					m_pPlaySession->curtexttrack = track_index;
				}
			
				pStreamInfo->ppTrackInfo[ulIndex]->uSelInfo |= VO_SOURCE2_SELECT_SELECTED;
			}
			else
			{
				pStreamInfo->ppTrackInfo[ulIndex]->uSelInfo &= (~VO_SOURCE2_SELECT_SELECTED);
			}
			track_index++;
		}
		SyncPlaySession();
		
	}


	return nResult;

}

VO_U32 ISS_ManifestManager::SetParam_IIS(VO_U32 nParamID, VO_PTR pParam )
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;

	VOLOGR("SetParam_IIS---nParamID = %lu",nParamID);

	switch(nParamID)
	{
	case VO_PID_SOURCE2_LIBOP:
		if(pParam){
			nResult = m_pManifestParser->SetLibOp((VO_LIB_OPERATOR*)pParam);
		}
		break;
	case VO_PID_SOURCE2_WORKPATH:
		if(pParam){
			nResult = m_pManifestParser->SetWorkPath((VO_TCHAR *)pParam);
		}
		break;
	case VO_PID_ADAPTIVESTREAMING_UTC:
		{
			m_utc_time =*((VO_U64*) pParam);
			m_basesystem_time = voOS_GetSysTime();
			VOLOGR("SetParam_IIS---m_utc_time = %llu, m_basesystem_time=%llu",m_utc_time, m_basesystem_time);	
			break;
		}
		break;
	case VO_PID_COMMON_LOGFUNC:
		{
			//VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
			//vologInit (pVologCB->pUserData, pVologCB->fCallBack);
		}
		break;		
	default:
		break;
	}

	return nResult;

}

VO_U32 ISS_ManifestManager::GetParam_IIS(VO_U32 nParamID, VO_PTR pParam )
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;


	VOLOGR("GetParam_IIS---nParamID = %lu",nParamID);
	switch(nParamID)
	{
	case VO_PID_ADAPTIVESTREAMING_DVR_ENDTIME:
		{
			nResult = GetEndTime((VO_U64*)pParam);
			VO_U64* ptmp = (VO_U64*)pParam;
			*ptmp = (*ptmp)/GetTimeScaleMs();
		}
		break;
	case VO_PID_ADAPTIVESTREAMING_DVR_WINDOWLENGTH:
		{
			nResult = GetDvrWindowLength((VO_U64*)pParam);
			VO_U64* ptmp = (VO_U64*)pParam;
			*ptmp = (*ptmp)/GetTimeScaleMs();	
		}
		break;
	case VO_PID_ADAPTIVESTREAMING_DVR_LIVETIME:
		{
			nResult = CalcThelivePoint((VO_U64*)pParam);
			VO_U64* ptmp = (VO_U64*)pParam;
			*ptmp = (*ptmp)/GetTimeScaleMs();	
		}
		break;
	case VO_PID_ADAPTIVESTREAMING_DVR_ENDLENGTH:
		{
			VO_U64 endtime = 0;
			VO_U64 curtime = 0;			
			GetEndTime((VO_U64*)&endtime);
			GetCurrentDownloadTime((VO_U64*)&curtime);

			VO_U64* ptmp = (VO_U64*)pParam;
			*ptmp = (endtime-curtime)/GetTimeScaleMs();
			
			VOLOGI("GetParam_IIS---ENDLENGTH = %llu",(*ptmp));
		}
		break;
	case VO_PID_ADAPTIVESTREAMING_DVR_LIVELENGTH:
		{
			VO_U64 curtime = 0;
			VO_U64 liveime = 0;			
			VO_U64 duration = 0;
			CalcThelivePoint((VO_U64*)&liveime,&duration);
			GetCurrentDownloadTime((VO_U64*)&curtime);

			VO_U64* ptmp = (VO_U64*)pParam;
			if(curtime >= liveime + duration){
				*ptmp = 0;
			}
			else{
				*ptmp = (liveime-curtime + duration)/GetTimeScaleMs();
			}
			
			VOLOGI("GetParam_IIS---LIVELENGTH = %llu",(*ptmp));
		}
		break;

	case VO_PID_ADAPTIVESTREAMING_DVR_CHUNKWINDOWPOS:
		{
			VO_U32	 ulTimeOffset = 0;
			VO_ADAPTIVESTREAMPARSER_CHUNKWINDOWPOS*   pChunkPosInfo = (VO_ADAPTIVESTREAMPARSER_CHUNKWINDOWPOS*)pParam;
			nResult = GetChunkOffsetValueBySequenceStartTime(pChunkPosInfo->uChunkID, &ulTimeOffset);
			if(VO_RET_SOURCE2_OK == nResult){
				pChunkPosInfo->ullWindowPosition = (VO_U64)ulTimeOffset;
			}
			break;
		}
		
	default:
		break;
	}

	return nResult;

}

VO_U32 ISS_ManifestManager::CheckTrackStatus(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	if(!Is_Live_Streaming()){
		if((m_nTrackMode&AUDIO_TRACK) && (m_nTrackMode&VIDEO_TRACK)){
			VOLOGI("CheckTrackStatus---END---uID=%lu,m_nTrackMode =%lu", uID, m_nTrackMode);		
			return VO_RET_SOURCE2_END;
		}
	}
	

	switch(uID)
	{
	case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
		if(NULL == m_pPlaySession->pAudioChunkList)
		{
			nResult = VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;	
			m_nTrackMode |= AUDIO_TRACK;
//			m_pPlaySession->curaudiofragment.starttime = 0;
		}
		else if(m_pPlaySession->audioiter == m_pPlaySession->pAudioChunkList->end())
		{
			nResult = Is_Live_Streaming() ? VO_RET_SOURCE2_NEEDRETRY : VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;	
			m_nTrackMode |= AUDIO_TRACK;
//			m_pPlaySession->curaudiofragment.starttime = Is_Live_Streaming() ? m_pPlaySession->curaudiofragment.starttime : 0;	
		}
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
		if(NULL == m_pPlaySession->pVideoChunkList)
		{
			nResult = VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;			
			m_nTrackMode |= VIDEO_TRACK;
//			m_pPlaySession->curvideofragment.starttime = 0;			
		}
		else if(m_pPlaySession->videoiter == m_pPlaySession->pVideoChunkList->end())
		{
			nResult = Is_Live_Streaming() ? VO_RET_SOURCE2_NEEDRETRY : VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;	
			m_nTrackMode |= VIDEO_TRACK;
//			m_pPlaySession->curvideofragment.starttime = Is_Live_Streaming() ? m_pPlaySession->curvideofragment.starttime : 0;	
		}
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
		if(NULL == m_pPlaySession->pTextChunkList)
		{
			nResult = VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;			
			m_nTrackMode |= SUBTILE_TRACK;
//			m_pPlaySession->curtextfragment.starttime = 0;
		}
		else if(m_pPlaySession->Textiter == m_pPlaySession->pTextChunkList->end())
		{
			nResult = Is_Live_Streaming() ? VO_RET_SOURCE2_NEEDRETRY : VO_RET_SOURCE2_OUTPUTNOTAVALIBLE;	
			m_nTrackMode |= SUBTILE_TRACK;
//			m_pPlaySession->curtextfragment.starttime = Is_Live_Streaming() ? m_pPlaySession->curtextfragment.starttime : 0;	
		}
		break;
	default:
		break;
	}

	if(VO_RET_SOURCE2_OK != nResult){
		VOLOGR("CheckTrackStatus---uID=%lu,m_nTrackMode =%lu, nResult = %x", uID, m_nTrackMode, nResult);
	}

	return nResult;
}


VO_VOID ISS_ManifestManager::CheckTheUpdateFlag()
{
	FragmentItem* tmp = NULL;
	float size = 0;
	float index = 0;

	if(!m_pPlaySession){
		return;
	}

	if(!Is_Live_Streaming()){
		return;
	}


	if(NULL != m_pPlaySession->pVideoChunkList){
		if(m_pPlaySession->videoiter == m_pPlaySession->pVideoChunkList->end()){
			m_pPlaySession->isneedtoupdate = VO_TRUE;
			VOLOGI("CheckTheUpdateFlag-video--end---size=%f, index=%f",size, index);	
			return;
		}

		//check the video list
		tmp = (FragmentItem*)(*m_pPlaySession->videoiter);
		size = (float)m_pPlaySession->pVideoChunkList->size();
		index = (float)(tmp->index+1);
//		VOLOGR("CheckTheUpdateFlag-video--size=%f, index=%f",size, index);
		if(index /size > PERCENT_TRUNK){
			m_pPlaySession->isneedtoupdate = VO_TRUE;
			VOLOGI("CheckTheUpdateFlag-video--size=%f, index=%f",size, index);	
			return;
		}

	}

	if(NULL != m_pPlaySession->pAudioChunkList){
		if(m_pPlaySession->audioiter == m_pPlaySession->pAudioChunkList->end()){
			m_pPlaySession->isneedtoupdate = VO_TRUE;
			VOLOGI("CheckTheUpdateFlag-audio--end---size=%f, index=%f",size, index);
			return;
		}
		
		//check the audio list
		tmp = (FragmentItem*)(*m_pPlaySession->audioiter);
		size = (float)m_pPlaySession->pAudioChunkList->size();
		index = (float)(tmp->index+1);

//		VOLOGR("CheckTheUpdateFlag-audio--size=%f, index=%f",size, index);
		if(index /size > PERCENT_TRUNK){
			m_pPlaySession->isneedtoupdate = VO_TRUE;
			VOLOGI("CheckTheUpdateFlag-audio--size=%f, index=%f",size, index);			
			return;
		}
	}

	if(NULL != m_pPlaySession->pTextChunkList){
		if(m_pPlaySession->Textiter == m_pPlaySession->pTextChunkList->end()){
			m_pPlaySession->isneedtoupdate = VO_TRUE;
			VOLOGI("CheckTheUpdateFlag-text--end---size=%f, index=%f",size, index);
			return;
		}
		
		//check the text list
		tmp = (FragmentItem*)(*m_pPlaySession->Textiter);
		size = (float)m_pPlaySession->pTextChunkList->size();
		index = (float)(tmp->index+1);
		
//		VOLOGR("CheckTheUpdateFlag-text--size=%f, index=%f",size, index);			
		if(index /size > PERCENT_TRUNK){
			m_pPlaySession->isneedtoupdate = VO_TRUE;
			VOLOGI("CheckTheUpdateFlag-text--size=%f, index=%f",size, index);			
			return;
		}
	}

}

VO_BOOL ISS_ManifestManager::IsNeedToUpdate()
{
	VO_BOOL bResult = VO_FALSE;

	if(VO_TRUE == m_pPlaySession->isneedtoupdate){
		VOLOGI("IsNeedToUpdate---update the manifest.");
		
		m_pPlaySession->isneedtoupdate = VO_FALSE;
		bResult = VO_TRUE;
	}

	return bResult;
}


VO_BOOL ISS_ManifestManager::Is_Live_Streaming()
{
	if(!m_pManifestInfo){
		return VO_FALSE;
	}
	
	return m_pManifestInfo->islive;
}

VO_U32 ISS_ManifestManager::CreateProgramInfo()
{
	VOLOGI("+CreateProgramInfo");


	if(!m_pManifestInfo){
		return VO_RET_SOURCE2_FAIL;
	}

	VO_U32 streamcount = m_pManifestInfo->streamIndexlist.size() ;

	//delete programinfo
	ReleaseProgramInfo();

	if(streamcount <= 0)
		return VO_RET_SOURCE2_FAIL;

	m_cursystem_time = voOS_GetSysTime();
	VOLOGR("CreateProgramInfo---m_cursystem_time = %llu",m_cursystem_time);	
	
	m_pProgramInfo = new _PROGRAM_INFO;
	memset(m_pProgramInfo, 0x0, sizeof(_PROGRAM_INFO));
	m_pProgramInfo->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
	m_pProgramInfo->sProgramType = m_pManifestInfo->islive? VO_SOURCE2_STREAM_TYPE_LIVE :VO_SOURCE2_STREAM_TYPE_VOD;
	m_pProgramInfo->uProgramID = 1;
	sprintf(m_pProgramInfo->strProgramName,"%s", "ISS");

	int videoCount = 0;
	int audioCount = 0;
	int textCount = 0;

	//calc the stream(video+1) count and track(audio) count.
	list_T<StreamIndex *>::iterator streamIndexiter;
	for(streamIndexiter = m_pManifestInfo->streamIndexlist.begin(); streamIndexiter != m_pManifestInfo->streamIndexlist.end(); ++streamIndexiter)
	{
		StreamIndex* tmp = (StreamIndex*)(*streamIndexiter);

		if( strcmp( tmp->type, "video") ==0)
		{
			videoCount += tmp->QL_List.size();
		}
		else if(strcmp(tmp->type, "audio") == 0)
		{
			audioCount += tmp->QL_List.size();//m_pManifestInfo->streamIndex[i].audio_QL[0] ? 1 :0);
		}
		else if(strcmp(tmp->type, "text") == 0)
		{
			textCount += tmp->QL_List.size();//m_pManifestInfo->streamIndex[i].audio_QL[0] ? 1 :0);
		}
	}

#ifdef SUPPORT_PURE_AUIDO	
	m_pProgramInfo->uStreamCount = videoCount + 1;//add pure audio stream;
#else
	m_pProgramInfo->uStreamCount = (videoCount == 0)? ((audioCount > 0)? 1: 0) : videoCount;
#endif

	//Create the stream and tack
	m_pProgramInfo->ppStreamInfo = new _STREAM_INFO* [m_pProgramInfo->uStreamCount];
	memset(m_pProgramInfo->ppStreamInfo, 0x00, m_pProgramInfo->uStreamCount);
	for(VO_U32 n = 0; n < m_pProgramInfo->uStreamCount ;n++)
	{
		_STREAM_INFO *pStreamInfo = m_pProgramInfo->ppStreamInfo[n] = new _STREAM_INFO;
		memset(pStreamInfo, 0x00, sizeof(_STREAM_INFO));
	
		pStreamInfo->uTrackCount = audioCount + textCount;

#ifdef SUPPORT_PURE_AUIDO
		//last stream in pure audio stream
		//other is 1 video + mulitAudio
		if(n != m_pProgramInfo->uStreamCount -1)
			pStreamInfo->uTrackCount += 1;
#else
		pStreamInfo->uTrackCount += 1;
#endif

		if(n != m_pProgramInfo->uStreamCount -1)
			pStreamInfo->uTrackCount += 1;

		pStreamInfo->ppTrackInfo = new _TRACK_INFO* [pStreamInfo->uTrackCount];
		memset(pStreamInfo->ppTrackInfo, 0x00 , pStreamInfo->uTrackCount);
/*
		for(VO_U32 l =0; l < pStreamInfo->uTrackCount; l ++)
		{
			pStreamInfo->ppTrackInfo[l] = new _TRACK_INFO;
			memset(pStreamInfo->ppTrackInfo[l], 0x00, sizeof(_TRACK_INFO));
		}
*/

		pStreamInfo->uTrackCount = 0;

	}

	//init the StreamInfo and TrackInfo
//	VO_U32 videocount = 0;
	VO_U32 audioindex = 0;
	VO_U32 textindex = 0;

	for(streamIndexiter = m_pManifestInfo->streamIndexlist.begin(); streamIndexiter != m_pManifestInfo->streamIndexlist.end(); ++streamIndexiter)
	{
		StreamIndex* tmp = (StreamIndex*)(*streamIndexiter);
		
		list_T<QualityLevel *>::iterator qliter;

		if(strcmp( tmp->type, "video") == 0)
		{

			for(qliter = tmp->QL_List.begin(); qliter != tmp->QL_List.end(); ++qliter)
			{
				QualityLevel * ql = (QualityLevel*)(*qliter);
//				_STREAM_INFO *pStreamInfo = m_pProgramInfo->ppStreamInfo[videocount];
//				pStreamInfo->uSelInfo	= (videocount== m_pPlaySession->curstream ? VO_SOURCE2_SELECT_SELECTED: VO_SOURCE2_SELECT_SELECTABLE);
//				pStreamInfo->uStreamID	= ql->index_QL;
//				pStreamInfo->uBitrate	+= (VO_U32)ql->bitrate;
//				videocount++;
				
				_TRACK_INFO *pTrackInfo = new _TRACK_INFO ;
				memset(pTrackInfo, 0x0, sizeof(_TRACK_INFO));
#ifdef _new_programinfo
				pTrackInfo->uASTrackID	= ql->index_QL;
#else
				pTrackInfo->uTrackID	= ql->index_QL;
#endif				
				pTrackInfo->uSelInfo	=	VO_SOURCE2_SELECT_SELECTED;//VO_SOURCE2_SELECT_DEFAULT;
				memcpy(pTrackInfo->strFourCC , ql->fourCC, sizeof(pTrackInfo->strFourCC));
				pTrackInfo->uTrackType	=	VO_SOURCE2_TT_VIDEO;
				pTrackInfo->uCodec		=	tmp->nCodecType;
				pTrackInfo->uDuration	=	m_pManifestInfo->duration *1000/ m_pManifestInfo->timeScale;
				pTrackInfo->uChunkCounts=	tmp->Chunk_List.size();
				pTrackInfo->uBitrate	=	(VO_U32)ql->bitrate;
				pTrackInfo->sVideoInfo.sFormat.Height = ql->video_info.Height;
				pTrackInfo->sVideoInfo.sFormat.Width = ql->video_info.Width;
				pTrackInfo->uHeadSize = ql->length_CPD;
				pTrackInfo->pHeadData = new VO_BYTE[pTrackInfo->uHeadSize+1];
				memset(pTrackInfo->pHeadData, 0x0, pTrackInfo->uHeadSize);
				memcpy(pTrackInfo->pHeadData, ql->codecPrivateData, pTrackInfo->uHeadSize);

				m_videotracklist.push_back(pTrackInfo);
			}


		}
		else if(strcmp( tmp->type, "audio") == 0)
		{
			//create the audio track

			for(qliter = tmp->QL_List.begin(); qliter != tmp->QL_List.end(); ++qliter)
			{
				QualityLevel * ql = (QualityLevel*)(*qliter);
   				_TRACK_INFO *pTrackInfo = new _TRACK_INFO;
				memset(pTrackInfo, 0x0, sizeof(_TRACK_INFO));
  				pTrackInfo->uSelInfo	= (audioindex== m_pPlaySession->curaudiotrack? VO_SOURCE2_SELECT_SELECTED: VO_SOURCE2_SELECT_SELECTABLE);				
#ifdef _new_programinfo
				pTrackInfo->uASTrackID	= ql->index_QL;
#else
   				pTrackInfo->uTrackID	= ql->index_QL;
#endif
				audioindex++;
//				pStreamInfo->uTrackCount ++;

   				memcpy(pTrackInfo->strFourCC , ql->fourCC, sizeof(pTrackInfo->strFourCC));
   				pTrackInfo->uTrackType	= VO_SOURCE2_TT_AUDIO;
   				pTrackInfo->uCodec		= tmp->nCodecType;
   				pTrackInfo->uDuration	= m_pManifestInfo->duration *1000/m_pManifestInfo->timeScale;
   				pTrackInfo->uChunkCounts= tmp->Chunk_List.size();
   				pTrackInfo->uBitrate	= (VO_U32) ql->bitrate;
   				pTrackInfo->sAudioInfo.sFormat.Channels = ql->audio_info.audio_format.Channels;
   				pTrackInfo->sAudioInfo.sFormat.SampleBits = ql->audio_info.audio_format.SampleBits;
   				pTrackInfo->sAudioInfo.sFormat.SampleRate = ql->audio_info.audio_format.SampleRate;
//				memcpy(pTrackInfo->sAudioInfo.chLanguage, tmp->language, sizeof(tmp->language));
				VO_CHAR language_subtype[256] = {0};
				if(strlen(tmp->subtype)){
					sprintf(language_subtype,"%s-%s",tmp->language,tmp->subtype);
				}else{
					memcpy(language_subtype, tmp->language, sizeof(tmp->language));
				}
				sprintf(pTrackInfo->sAudioInfo.chLanguage,"%s-%lu",language_subtype,pTrackInfo->uASTrackID);
				VOLOGI("pTrackInfo->sAudioInfo.chLanguage %s", pTrackInfo->sAudioInfo.chLanguage);
   				pTrackInfo->uHeadSize = ql->length_CPD;
				VOLOGR("pTrackInfo->uHeadSize %d", pTrackInfo->uHeadSize);
   				pTrackInfo->pHeadData = new VO_BYTE[pTrackInfo->uHeadSize+1];
				memset(pTrackInfo->pHeadData, 0x0, pTrackInfo->uHeadSize);
   				memcpy(pTrackInfo->pHeadData, ql->codecPrivateData, pTrackInfo->uHeadSize);

				m_audiotracklist.push_back(pTrackInfo);
			}

 		}
		else if(strcmp( tmp->type, "text") == 0)
		{

			for(qliter = tmp->QL_List.begin(); qliter != tmp->QL_List.end(); ++qliter)
			{
				QualityLevel * ql = (QualityLevel*)(*qliter);
				_TRACK_INFO *pTrackInfo = new _TRACK_INFO;
				memset(pTrackInfo, 0x0, sizeof(_TRACK_INFO));				
				pTrackInfo->uSelInfo	= (textindex== m_pPlaySession->curtexttrack? VO_SOURCE2_SELECT_SELECTED: VO_SOURCE2_SELECT_SELECTABLE);				
#ifdef _new_programinfo
				pTrackInfo->uASTrackID	= ql->index_QL;
#else
				pTrackInfo->uTrackID	= ql->index_QL;
#endif
				textindex++;
//				pStreamInfo->uTrackCount ++;

				memcpy(pTrackInfo->strFourCC , ql->fourCC, sizeof(pTrackInfo->strFourCC));
				pTrackInfo->uTrackType	= VO_SOURCE2_TT_SUBTITLE;
				pTrackInfo->uCodec		= tmp->nCodecType;
				pTrackInfo->uDuration	= m_pManifestInfo->duration *1000/m_pManifestInfo->timeScale;
				pTrackInfo->uChunkCounts= tmp->Chunk_List.size();
				pTrackInfo->uBitrate	= (VO_U32) ql->bitrate;
//				memcpy(pTrackInfo->sSubtitleInfo.chLanguage, tmp->language, sizeof(tmp->language));
				VO_CHAR language_subtype[256] = {0};
				if(strlen(tmp->subtype)){
					sprintf(language_subtype,"%s-%s",tmp->language,tmp->subtype);
				}else{
					memcpy(language_subtype, tmp->language, sizeof(tmp->language));
				}
				sprintf(pTrackInfo->sSubtitleInfo.chLanguage,"%s-%lu",language_subtype,pTrackInfo->uASTrackID);
				VOLOGI("pTrackInfo->sSubtitleInfo.chLanguage %s", pTrackInfo->sSubtitleInfo.chLanguage);
				pTrackInfo->sSubtitleInfo.uCodingType = 0;
				memset(pTrackInfo->sSubtitleInfo.Align, 0x0, sizeof(pTrackInfo->sSubtitleInfo.Align));
				pTrackInfo->uHeadSize = ql->length_CPD;
				VOLOGR("pTrackInfo->uHeadSize %d", pTrackInfo->uHeadSize);
				pTrackInfo->pHeadData = new VO_BYTE[pTrackInfo->uHeadSize+1];
				memset(pTrackInfo->pHeadData, 0x0, pTrackInfo->uHeadSize);
				memcpy(pTrackInfo->pHeadData, ql->codecPrivateData, pTrackInfo->uHeadSize);

				m_texttracklist.push_back(pTrackInfo);
			}

		}

	}

	//calc the video track number
	VO_U32 index = 0;
	_STREAM_INFO *pStreamInfo = m_pProgramInfo->ppStreamInfo[index];
	_TRACK_INFO *pTrackInfo = NULL;
	
	list_T<_TRACK_INFO *>::iterator trackiter;
	for(trackiter = m_videotracklist.begin(); trackiter != m_videotracklist.end(); ++trackiter)
	{
		pTrackInfo = (_TRACK_INFO*)(*trackiter);
	
		pStreamInfo = m_pProgramInfo->ppStreamInfo[index];
		pStreamInfo->uBitrate	= (VO_U32)pTrackInfo->uBitrate;
		pStreamInfo->ppTrackInfo[pStreamInfo->uTrackCount] = pTrackInfo;
		pStreamInfo->uTrackCount++;
		index++;
	}

	//calc the audio/text track number and stream bitrate
	for(VO_U32 index = 0; index < m_pProgramInfo->uStreamCount; ++index)
	{
		VO_U32	max_bitrate = 0;
		_STREAM_INFO *pStreamInfo = m_pProgramInfo->ppStreamInfo[index];
		pStreamInfo->uSelInfo	= (index== m_pPlaySession->curstream ? VO_SOURCE2_SELECT_RECOMMEND|VO_SOURCE2_SELECT_SELECTED: VO_SOURCE2_SELECT_SELECTABLE);
		pStreamInfo->uStreamID = index;

		list_T<_TRACK_INFO *>::iterator trackiter;
		for(trackiter = m_audiotracklist.begin(); trackiter != m_audiotracklist.end(); ++trackiter)
		{
			_TRACK_INFO *pTrackInfo = (_TRACK_INFO*)(*trackiter);

			if(pTrackInfo->uBitrate > max_bitrate){
				max_bitrate = pTrackInfo->uBitrate;
			}
			pStreamInfo->ppTrackInfo[pStreamInfo->uTrackCount] = pTrackInfo;
			pStreamInfo->uTrackCount++;
		}
		pStreamInfo->uBitrate += max_bitrate;

		max_bitrate = 0;
		for(trackiter = m_texttracklist.begin(); trackiter != m_texttracklist.end(); ++trackiter)
		{
			_TRACK_INFO *pTrackInfo = (_TRACK_INFO*)(*trackiter);

			if(pTrackInfo->uBitrate > max_bitrate){
				max_bitrate = pTrackInfo->uBitrate;
			}				
			pStreamInfo->ppTrackInfo[pStreamInfo->uTrackCount] = pTrackInfo;
			pStreamInfo->uTrackCount++;
		}
		pStreamInfo->uBitrate += max_bitrate;

	}

	VOLOGI("-CreateProgramInfo");

	PrintDSProgramInfo(m_pProgramInfo);
	return VO_RET_SOURCE2_OK;
}


VO_U32 ISS_ManifestManager::ReleaseProgramInfo()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	if(!m_pProgramInfo)
		return VO_RET_SOURCE2_FAIL;

	VOLOGI("ReleaseProgramInfo---start");

	list_T<_TRACK_INFO *>::iterator trackiter;
	for(trackiter = m_audiotracklist.begin(); trackiter != m_audiotracklist.end(); ++trackiter)
	{
		_TRACK_INFO *pTrackInfo = (_TRACK_INFO*)(*trackiter);
	
		if(pTrackInfo->pHeadData)
			delete []pTrackInfo->pHeadData;
		delete pTrackInfo;
	}
	m_audiotracklist.clear();

	for(trackiter = m_videotracklist.begin(); trackiter != m_videotracklist.end(); ++trackiter)
	{
		_TRACK_INFO *pTrackInfo = (_TRACK_INFO*)(*trackiter);
	
		if(pTrackInfo->pHeadData)
			delete []pTrackInfo->pHeadData;		
		delete pTrackInfo;
	}
	m_videotracklist.clear();

	for(trackiter = m_texttracklist.begin(); trackiter != m_texttracklist.end(); ++trackiter)
	{
		_TRACK_INFO *pTrackInfo = (_TRACK_INFO*)(*trackiter);
	
		if(pTrackInfo->pHeadData)
			delete []pTrackInfo->pHeadData;
		delete pTrackInfo;
	}
	m_texttracklist.clear();

	
	for (VO_U32 i= 0;i<m_pProgramInfo->uStreamCount; i++ )
	{
		_STREAM_INFO *ppSInfo = m_pProgramInfo->ppStreamInfo[i] ;
/*
		for (VO_U32 n= 0; n < ppSInfo->uTrackCount; n++ )
		{
			_TRACK_INFO *ppTInfo = ppSInfo->ppTrackInfo[n];
			if(ppTInfo->pHeadData)
				delete []ppTInfo->pHeadData;
			delete ppTInfo;
			
		}
*/
		
		if(ppSInfo->uTrackCount)
			delete []ppSInfo->ppTrackInfo;
		delete ppSInfo;
			
	}
	if(m_pProgramInfo->uStreamCount)
		delete []m_pProgramInfo->ppStreamInfo;
	
	memset(m_pProgramInfo, 0x00, sizeof(_PROGRAM_INFO));
	delete m_pProgramInfo;
	
	m_pProgramInfo = NULL;
	VOLOGI("ReleaseProgramInfo---end");

	return nResult;
}

VO_U32 ISS_ManifestManager::CreateDrmInfo()
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	if(!m_pManifestInfo){
		return VO_RET_SOURCE2_FAIL;
	}

	if(m_pManifestInfo->piff.isUsePiff)
	{
		m_pDRMInfo = new VO_SOURCE2_DRM_INFO ;
		m_pDRMInfo->nType = VO_SOURCE2_DRM_PLAYREADY;
		m_pDRMInfo->pDRM_Info = (VO_PTR*)(&m_pManifestInfo->piff);
	}
	else
		m_pDRMInfo = NULL;
	
	return nResult;
}

VO_U32 ISS_ManifestManager::GetCurrentSreamInfo(_PROGRAM_INFO* pProgramInfo, _STREAM_INFO** ppStreamInfo)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_STREAM_INFO *pSInfo = 0;

	if(!pProgramInfo){
		return VO_RET_SOURCE2_FAIL;
	}

	for (VO_U32 i= 0;i<pProgramInfo->uStreamCount; i++ )
	{
		pSInfo = pProgramInfo->ppStreamInfo[i] ;

		if(VO_SOURCE2_SELECT_SELECTED == (VO_SOURCE2_SELECT_SELECTED & pSInfo->uSelInfo)){
			*ppStreamInfo = pSInfo;
			nResult = VO_RET_SOURCE2_OK;
			break;
		}		
	}
	
	return nResult;
}

VO_U32 ISS_ManifestManager::GetCurrentTrackInfo(_STREAM_INFO* pStreamInfo, VO_SOURCE2_TRACK_TYPE TrackType, _TRACK_INFO** ppTrackInfo)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO *pTInfo = 0;

	if(!pStreamInfo){
		return VO_RET_SOURCE2_FAIL;
	}

	for (VO_U32 i= 0;i<pStreamInfo->uTrackCount; i++ )
	{
		pTInfo = pStreamInfo->ppTrackInfo[i] ;

		if(VO_SOURCE2_SELECT_SELECTED == (VO_SOURCE2_SELECT_SELECTED & pTInfo->uSelInfo) && TrackType == pTInfo->uTrackType){
			*ppTrackInfo = pTInfo;
			nResult = VO_RET_SOURCE2_OK;
			break;
		}		
	}
	
	return nResult;
}


VO_U32 ISS_ManifestManager::GetStreamIndexByType(VO_SOURCE2_TRACK_TYPE TrackType, StreamIndex** ppStreamIndex)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* pTrackInfo = NULL;

	nResult = GetCurTrackInfo(TrackType, &pTrackInfo);

	if(VO_RET_SOURCE2_OK != nResult || NULL == pTrackInfo){
		return VO_RET_SOURCE2_FAIL;
	}

	list_T<StreamIndex *>::iterator streamIndexiter;
	for(streamIndexiter = m_pManifestInfo->streamIndexlist.begin(); streamIndexiter != m_pManifestInfo->streamIndexlist.end(); ++streamIndexiter)
	{
		StreamIndex* tmp = (StreamIndex*)(*streamIndexiter);

		if(tmp->nTrackType != TrackType){
			continue;
		}

		list_T<QualityLevel *>::iterator qliter;
		for(qliter = tmp->QL_List.begin(); qliter != tmp->QL_List.end(); ++qliter)
		{
			QualityLevel * ql = (QualityLevel*)(*qliter);
			
#ifdef _new_programinfo
			if(ql->index_QL == pTrackInfo->uASTrackID){
#else
			if(ql->index_QL == pTrackInfo->uTrackID){
#endif
				*ppStreamIndex = tmp;
				return VO_RET_SOURCE2_OK;
			}
		}
	}

	return nResult;
}

VO_U32 ISS_ManifestManager::GenerateChunkUrlByFragment(FragmentItem* pItem,VO_CHAR* ptr_path)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	StreamIndex* pStreamIndex = NULL;
	_TRACK_INFO* pTrackInfo = NULL;
	VO_SOURCE2_TRACK_TYPE	TrackType;//pItem->nIsVideo ? VO_SOURCE2_TT_VIDEO : VO_SOURCE2_TT_AUDIO;
	VO_S64	url_bitrate = 0;
	VO_S64   chunkid = 0;

	if(!pItem){
		return VO_RET_SOURCE2_FAIL;
	}

	TrackType = ConvertTrackType(pItem->nIsVideo);

	nResult = GetStreamIndexByType(TrackType, &pStreamIndex);
	if(VO_RET_SOURCE2_OK != nResult){
		return nResult;
	}

	nResult = GetCurTrackInfo(TrackType, &pTrackInfo);
	if(VO_RET_SOURCE2_OK != nResult){
		return nResult;
	}

	memcpy(ptr_path, pStreamIndex->url, sizeof(pStreamIndex->url));
	url_bitrate = pTrackInfo->uBitrate;
	chunkid = pItem->index;
		
		
	VO_CHAR c[10];
	memset(c,0x00,sizeof(c));
#if defined _WIN32
	strcat(c,"%llu");
	//	strcat(base_url, "/QualityLevels(%I64d)/Fragments(");
#elif defined LINUX
	strcat(c,"%llu");
	//	strcat(base_url, "/QualityLevels(%lld)/Fragments(");
#elif defined _IOS
	strcat(c,"%llu");
#elif defined _MAC_OS
	strcat(c,"%llu");
#endif
	//VOLOGI("+url: %s",ptr_path);
	char str[255];
	memset(str,0x00,sizeof(str));
	sprintf(str,c,url_bitrate);
	replace(ptr_path,"{bitrate}",str);

	memset(str,0x00,sizeof(str));
	if(strlen(pStreamIndex->CustomAttribute.Name)){
		strcat(str,pStreamIndex->CustomAttribute.Name);
		strcat(str,"=");
		strcat(str,pStreamIndex->CustomAttribute.Value);
	}
	replace(ptr_path,",{CustomAttributes}",str);
	
//	if(ptr_item->starttime ==18446744071587064320)
//		int x =0;
	memset(str,0x00,sizeof(str));
	sprintf(str,c, pItem->starttime);
	if (!replace(ptr_path,"{start time}",str)){
		replace(ptr_path,"{start_time}",str);
	}

	memset(str,0x00,sizeof(str));
	sprintf(str, c, chunkid);
	replace(ptr_path,"{chunk id}",str);
	VOLOGR("+++++++++++++url: %s,%llu",ptr_path,pItem->starttime);
//	printf("+++++++++++++url: %s,%llu",ptr_path,pItem->starttime);

	return nResult;
}

VO_SOURCE2_TRACK_TYPE ISS_ManifestManager::ConvertTrackType(VO_U32 ItemType)
{
	VO_SOURCE2_TRACK_TYPE	TrackType = VO_SOURCE2_TT_MAX;
	
	switch(ItemType)
	{
	case 0:
		TrackType = VO_SOURCE2_TT_AUDIO;
		break;
	case 1:
		TrackType = VO_SOURCE2_TT_VIDEO;
		break;
	case 2:
		TrackType = VO_SOURCE2_TT_SUBTITLE;
		break;
	}

	return TrackType;
}

VO_U32 ISS_ManifestManager::SetStreamIndexPos(VO_SOURCE2_TRACK_TYPE   TrackType, VO_U64* pos)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	list_T<FragmentItem*>* pChunkList = NULL;
	list_T<FragmentItem *>::iterator*	pIter = NULL;

	VOLOGR("SetStreamIndexPos---begin---TrackType = %d, pos=%llu",TrackType, *pos);

	switch(TrackType)
	{
	case VO_SOURCE2_TT_VIDEO:
		pChunkList = m_pPlaySession->pVideoChunkList;
		pIter = &m_pPlaySession->videoiter;
		break;
	case VO_SOURCE2_TT_AUDIO:
		pChunkList = m_pPlaySession->pAudioChunkList;
		pIter = &m_pPlaySession->audioiter;		
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		pChunkList = m_pPlaySession->pTextChunkList;
		pIter = &m_pPlaySession->Textiter; 	
		break;		
	default:
		nResult = VO_RET_SOURCE2_NOIMPLEMENT;
		break;
	}

	if(VO_RET_SOURCE2_OK == nResult){
		
		FragmentItem * ptr_temp = NULL;
		FragmentItem *tmpPtr = NULL;
//		FragmentItem *tmpPtrPre = NULL;
//		VO_U64 tmptime = 0;

		list_T<FragmentItem *>::iterator	pTmpIter;
		for( pTmpIter = pChunkList->begin(); pTmpIter != pChunkList->end(); ++pTmpIter)
		{
			ptr_temp = (FragmentItem*)(*pTmpIter);
		
			if(*pos >= ptr_temp->starttime)
			{
//				tmpPtrPre = tmpPtr;
				tmpPtr = ptr_temp;
			}
			else if(*pos < ptr_temp->starttime )
			{
				break;
			}
		}

		*pIter = (pTmpIter == pChunkList->begin()) ? pTmpIter: --pTmpIter;
		ptr_temp = (FragmentItem*)(*(*pIter));
		
		*pos = ptr_temp->starttime;

		VOLOGI("SetStreamIndexPos---end---TrackType = %d, pos=%llu, index=%lu, size=%lu",TrackType, *pos, ptr_temp->index + 1, pChunkList->size());

	}


	return nResult;

}

VO_U32 ISS_ManifestManager::InitPlaySession()
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;

	m_pPlaySession = new PlaySession;
	
	m_pPlaySession->pAudioChunkList = NULL;	
	m_pPlaySession->audioflag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
	memset(&m_pPlaySession->curaudiofragment, 0x0, sizeof(m_pPlaySession->curaudiofragment));
	m_pPlaySession->curaudiofragment.starttime = 0|COMPARE_FLAG;
	
	m_pPlaySession->pVideoChunkList = NULL;
	m_pPlaySession->videoflag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
	memset(&m_pPlaySession->curvideofragment, 0x0, sizeof(m_pPlaySession->curvideofragment));
	m_pPlaySession->curvideofragment.starttime = 0|COMPARE_FLAG;

	m_pPlaySession->pTextChunkList = NULL;
	m_pPlaySession->textflag = VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
	memset(&m_pPlaySession->curtextfragment, 0x0, sizeof(m_pPlaySession->curtextfragment));
	m_pPlaySession->curtextfragment.starttime = 0|COMPARE_FLAG;
	
	m_pPlaySession->curstream = 0;
	m_pPlaySession->curaudiotrack = 0;
	m_pPlaySession->curtexttrack = 0;

	m_pPlaySession->isneedtoupdate = VO_FALSE;

	return nResult;
}

VO_U32 ISS_ManifestManager::SyncPlaySession(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	StreamIndex*	pStreamIndex = NULL;
	FragmentItem* tmp = NULL;

	//print the unsigned long long by 64 scale, fill 0 if not enough.
	VOLOGI("SyncPlaySession---start---curvideotime=%016I64x, curaudiotime=%016I64x, curtexttime=%016I64x", 
	m_pPlaySession->curvideofragment.starttime, m_pPlaySession->curaudiofragment.starttime, m_pPlaySession->curtextfragment.starttime);

/*
	//temp change
	if(0 == m_pPlaySession->curvideofragment.starttime){
		m_pPlaySession->curvideofragment.starttime = m_pPlaySession->curaudiofragment.starttime;
	}
	
	if(0 == m_pPlaySession->curaudiofragment.starttime){
		m_pPlaySession->curaudiofragment.starttime = m_pPlaySession->curvideofragment.starttime;
	}
	
	if(0 == m_pPlaySession->curtextfragment.starttime){
		m_pPlaySession->curtextfragment.starttime = m_pPlaySession->curaudiofragment.starttime;
	}
*/

	if(VO_RET_SOURCE2_OK == GetStreamIndexByType(VO_SOURCE2_TT_VIDEO, &pStreamIndex)){
		m_pPlaySession->pVideoChunkList = &pStreamIndex->Chunk_List;
		for(m_pPlaySession->videoiter = m_pPlaySession->pVideoChunkList->begin(); 
			m_pPlaySession->videoiter != m_pPlaySession->pVideoChunkList->end(); ++m_pPlaySession->videoiter)
		{
			tmp = (FragmentItem*)(*m_pPlaySession->videoiter);

			if((COMPARE_FLAG & m_pPlaySession->curvideofragment.starttime) || (VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS_PRESENT == sPrepareChunkPos)){
				if(tmp->starttime >= (m_pPlaySession->curvideofragment.starttime&(~COMPARE_FLAG))){
					break;
				}
			}
			else{
				if(tmp->starttime > m_pPlaySession->curvideofragment.starttime){
					break;
				}

			}
		
		}

		if(m_pPlaySession->videoiter != m_pPlaySession->pVideoChunkList->end()){
			tmp = (FragmentItem*)(*m_pPlaySession->videoiter);	
			VOLOGI("SyncPlaySession---index=%ld, size =%lu, video starttime=%llu, curvideotime =%llu",tmp->index+1, m_pPlaySession->pVideoChunkList->size(), tmp->starttime, m_pPlaySession->curvideofragment.starttime);			
		}

	}
	else{
		VOLOGI("SyncPlaySession---video---empity");
		m_pPlaySession->pVideoChunkList = NULL;
		m_pPlaySession->videoflag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
		m_pPlaySession->curvideofragment.starttime = 0;
	}	

		
	if(VO_RET_SOURCE2_OK == GetStreamIndexByType(VO_SOURCE2_TT_AUDIO, &pStreamIndex)){
		m_pPlaySession->pAudioChunkList = &pStreamIndex->Chunk_List;
		for(m_pPlaySession->audioiter = m_pPlaySession->pAudioChunkList->begin(); 
			m_pPlaySession->audioiter != m_pPlaySession->pAudioChunkList->end(); ++m_pPlaySession->audioiter)
		{
			tmp = (FragmentItem*)(*m_pPlaySession->audioiter);
		
			if((COMPARE_FLAG & m_pPlaySession->curaudiofragment.starttime)/* || (VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS_PRESENT == sPrepareChunkPos)*/){
				if(tmp->starttime >= (m_pPlaySession->curaudiofragment.starttime&(~COMPARE_FLAG))){
					break;
				}
			
			}
			else{
				if(tmp->starttime > m_pPlaySession->curaudiofragment.starttime){
					break;
				}
			
			}
		}

		if(m_pPlaySession->audioiter != m_pPlaySession->pAudioChunkList->end()){
			tmp = (FragmentItem*)(*m_pPlaySession->audioiter);		
			VOLOGI("SyncPlaySession---index=%ld, size =%lu, audio starttime=%llu, curaudiotime=%llu",tmp->index+1, m_pPlaySession->pAudioChunkList->size(), tmp->starttime, m_pPlaySession->curaudiofragment.starttime);
		}

	}
	else{
		VOLOGI("SyncPlaySession---audio---empity");
		m_pPlaySession->pAudioChunkList = NULL; 
		m_pPlaySession->audioflag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
		m_pPlaySession->curaudiofragment.starttime = 0;		
	}

	if(VO_RET_SOURCE2_OK == GetStreamIndexByType(VO_SOURCE2_TT_SUBTITLE, &pStreamIndex)){
		m_pPlaySession->pTextChunkList = &pStreamIndex->Chunk_List;
		for(m_pPlaySession->Textiter = m_pPlaySession->pTextChunkList->begin(); 
			m_pPlaySession->Textiter != m_pPlaySession->pTextChunkList->end(); ++m_pPlaySession->Textiter)
		{
			tmp = (FragmentItem*)(*m_pPlaySession->Textiter);
		
			if((COMPARE_FLAG & m_pPlaySession->curtextfragment.starttime)/*  || (VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS_PRESENT == sPrepareChunkPos)*/){
				if(tmp->starttime >= (m_pPlaySession->curtextfragment.starttime&(~COMPARE_FLAG))){
					break;
				}
			
			}
			else{
				if(tmp->starttime > m_pPlaySession->curtextfragment.starttime){
					break;
				}
			
			}
		}
		
		if(m_pPlaySession->Textiter != m_pPlaySession->pTextChunkList->end()){
			tmp = (FragmentItem*)(*m_pPlaySession->Textiter);		
			VOLOGI("SyncPlaySession---index=%ld, size =%lu, text starttime=%llu, curtexttime=%llu",tmp->index+1, m_pPlaySession->pTextChunkList->size(), tmp->starttime, m_pPlaySession->curtextfragment.starttime);
		}

	}
	else{
		VOLOGI("SyncPlaySession---text---empity");
		m_pPlaySession->pTextChunkList = NULL;
		m_pPlaySession->textflag |= VO_ADAPTIVESTREAMPARSER_CHUNKFLAG_FORMATCHANGE;
		m_pPlaySession->curtextfragment.starttime = 0;
	}

	
	VOLOGI("SyncPlaySession---stop");

	return nResult;
}

VO_U32 ISS_ManifestManager::CheckLookAheadCount(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	list_T<FragmentItem *>::iterator	pIter;
	list_T<FragmentItem*>* pChunkList = NULL;
	FragmentItem* tmp = NULL;
	
	if(!m_pManifestInfo->islive || m_pManifestInfo->lookaheadfragmentcount <= 0)
	{
		return nResult;
	}
	
	VOLOGR("CheckLookAheadCount---uID=%lu",uID);

	switch(uID)
	{
	case VO_SOURCE2_ADAPTIVESTREAMING_AUDIO:
			pIter = m_pPlaySession->audioiter;
			pChunkList = m_pPlaySession->pAudioChunkList;
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_VIDEO:
			pIter = m_pPlaySession->videoiter;
			pChunkList = m_pPlaySession->pVideoChunkList;
		break;
	case VO_SOURCE2_ADAPTIVESTREAMING_SUBTITLE:
			pIter = m_pPlaySession->Textiter;
			pChunkList = m_pPlaySession->pTextChunkList;			
		break;
	default:
		return nResult;
	}


	if(NULL != pChunkList && pChunkList->size() > m_pManifestInfo->lookaheadfragmentcount){
		for(int i = 0; i< m_pManifestInfo->lookaheadfragmentcount; i++){
			if(++pIter == pChunkList->end()){
				VOLOGE("CheckLookAheadCount---i=%d, lookaheadfragmentcount=%ld",i, m_pManifestInfo->lookaheadfragmentcount);
				nResult = VO_RET_SOURCE2_NEEDRETRY;
				break;
			}
		}
	}
	
	tmp = (FragmentItem*)(*pIter);
	VO_U32 index = (pIter == pChunkList->end())?0xffffffff : tmp->index+1;
	
	VOLOGR("CheckLookAheadCount---nResult=%x, index=%lu, size=%lu",nResult, index, pChunkList->size());

	return nResult;

}

VO_U32 ISS_ManifestManager::CalcThelivePoint(VO_U64 *pos, VO_U64 *duration)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	VO_S32 live_point_intervel = m_pManifestInfo->lookaheadfragmentcount + LIVE_AHEAD_BUFFER_LENGTH;
	list_T<FragmentItem*>* pChunkList = NULL;
	list_T<FragmentItem *>::iterator	Iter;
	FragmentItem* tmp = NULL;

	if(!pos){
		return VO_RET_SOURCE2_FAIL;
	}

	if(NULL != m_pPlaySession->pVideoChunkList && m_pPlaySession->pVideoChunkList->end() != m_pPlaySession->videoiter){
		pChunkList = m_pPlaySession->pVideoChunkList;
	}else if(NULL != m_pPlaySession->pAudioChunkList && m_pPlaySession->pAudioChunkList->end() != m_pPlaySession->audioiter){
		pChunkList = m_pPlaySession->pAudioChunkList;
	}else if(NULL != m_pPlaySession->pTextChunkList && m_pPlaySession->pTextChunkList->end() != m_pPlaySession->Textiter){
		pChunkList = m_pPlaySession->pTextChunkList;
	}

	if(NULL == pChunkList){
		return VO_RET_SOURCE2_FAIL;
	}

	Iter = pChunkList->end();
	
	if(pChunkList->size() > live_point_intervel){
		for(int i = 0; i< live_point_intervel + 1; i++){
			Iter--;
		}
	}else if(pChunkList->size() > m_pManifestInfo->lookaheadfragmentcount){
		for(int i = 0; i< m_pManifestInfo->lookaheadfragmentcount + 1; i++){
			Iter--;
		}
	}else if(pChunkList->size() > 0){
		Iter--;
	}else{
		VOLOGE("Thunk list is empity.");
		return VO_RET_SOURCE2_FAIL;
	}
	
	tmp = (FragmentItem*)(*Iter);

	*pos = tmp->starttime;

	if(duration){
		*duration = tmp->duration;
	}
	
	VOLOGI("CalcThelivePoint---starttime=%llu, index=%lu, size=%lu",*pos, tmp->index+1 ,pChunkList->size());
	
	return nResult;
}

VO_U32 ISS_ManifestManager::GetDvrWindowLength(VO_U64 *pos)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	*pos = m_pManifestInfo->dvrwindowlength;
	VOLOGI("GetDvrWindowLength---pos=%llu",*pos);

	return nResult;
}

VO_U32 ISS_ManifestManager::GetEndTime(VO_U64 *pos)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	list_T<FragmentItem*>* pChunkList = NULL;
	list_T<FragmentItem *>::iterator	Iter;
	FragmentItem* tmp = NULL;

	if(NULL != m_pPlaySession->pVideoChunkList){
		pChunkList = m_pPlaySession->pVideoChunkList;
	}else if(NULL != m_pPlaySession->pAudioChunkList){
		pChunkList = m_pPlaySession->pAudioChunkList;
	}

	if(NULL == pChunkList || 0 == pChunkList->size()){
		nResult = VO_RET_SOURCE2_FAIL;
	}
	else{
		Iter = --pChunkList->end();
		tmp = (FragmentItem*)(*Iter);
		*pos = (tmp->starttime + tmp->duration);
	}

	VOLOGI("GetEndTime---pos=%llu,nResult=%x",*pos, nResult);

	return nResult;
}


VO_U32 ISS_ManifestManager::GetCurrentDownloadTime(VO_U64 *pos)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	list_T<FragmentItem *>::iterator	*pIter=NULL;
	FragmentItem* tmp = NULL;
	list_T<FragmentItem*>* pChunkList = NULL;

	if(NULL != m_pPlaySession->pVideoChunkList){
		pChunkList = m_pPlaySession->pVideoChunkList;
		pIter = &m_pPlaySession->videoiter;
	}else if(NULL != m_pPlaySession->pAudioChunkList){
		pChunkList = m_pPlaySession->pAudioChunkList;	
		pIter = &m_pPlaySession->audioiter;
	}else if(NULL != m_pPlaySession->pTextChunkList){
		pChunkList = m_pPlaySession->pTextChunkList;		
		pIter = &m_pPlaySession->Textiter;
	}

	if(NULL == pChunkList || (*pIter) == pChunkList->end()){
		return VO_RET_SOURCE2_FAIL;
	}
	
	tmp = (FragmentItem*)(*(*pIter));

	*pos = (VO_U64)(tmp->starttime);
	VOLOGI("GetCurrentTime---starttime=%llu, index=%lu",*pos, tmp->index+1);

	return nResult;
}


VO_U32 ISS_ManifestManager::GetUpdateIntervel(VO_U32 *pos)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	list_T<FragmentItem*>* pChunkList = NULL;
	list_T<FragmentItem *>::iterator*	pIter = NULL;
	FragmentItem* tmp = NULL;
	VO_U64	curstarttime = 0;

	if(NULL != m_pPlaySession->pVideoChunkList && m_pPlaySession->pVideoChunkList->end() != m_pPlaySession->videoiter){
		pChunkList = m_pPlaySession->pVideoChunkList;
		pIter = &m_pPlaySession->videoiter;
		
		curstarttime = m_pPlaySession->curvideofragment.starttime&(~COMPARE_FLAG);
	}else if(NULL != m_pPlaySession->pAudioChunkList && m_pPlaySession->pAudioChunkList->end() != m_pPlaySession->audioiter){
		pChunkList = m_pPlaySession->pAudioChunkList;
		pIter = &m_pPlaySession->audioiter;
		
		curstarttime = m_pPlaySession->curaudiofragment.starttime&(~COMPARE_FLAG);
	}else{
		VOLOGE("GetUpdateIntervel---Error!");
	}

	if(NULL == pChunkList || (*pIter) == pChunkList->end()){
		*pos = DEFAULT_UPDATE_INTERVEL;
		nResult = VO_RET_SOURCE2_FAIL;
	}else{
		tmp = (FragmentItem*)(**pIter);
		VOLOGI("GetUpdateIntervel---duration=%llu, starttime=%llu, curstarttime=%llu",tmp->duration, tmp->starttime,curstarttime);	
		
		if(tmp->duration){
			*pos = (VO_U32)(tmp->duration/((VO_U64)m_pManifestInfo->timeScale/1000));			
		}else{
			VO_U64 duration = (curstarttime == 0 ||tmp->starttime == 0)?0 : (tmp->starttime - curstarttime);
			*pos = (0==duration)?DEFAULT_UPDATE_INTERVEL:((VO_U32)(duration/((VO_U64)m_pManifestInfo->timeScale/1000)));
		}
	}

	if(NULL != pos){
		*pos /=2;
	}

	VOLOGI("GetUpdateIntervel---pos=%lu,nResult=%x",*pos, nResult);

	return nResult;
}

VO_U32 ISS_ManifestManager::GetChunkOffsetValueBySequenceStartTime(VO_U64  uStartTime, VO_U32* pTimeOffset)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	
	list_T<FragmentItem *>::iterator Iter;
	FragmentItem* tmp = NULL;
	list_T<FragmentItem*>* pChunkList = NULL;
	VO_U64 begin = 0;

	if(NULL != m_pPlaySession->pVideoChunkList){
		pChunkList = m_pPlaySession->pVideoChunkList;
	}else if(NULL != m_pPlaySession->pAudioChunkList){
		pChunkList = m_pPlaySession->pAudioChunkList;	
	}else if(NULL != m_pPlaySession->pTextChunkList){
		pChunkList = m_pPlaySession->pTextChunkList;		
	}

	if(NULL == pChunkList){
		return VO_RET_SOURCE2_FAIL;
	}
	
	Iter = pChunkList->begin();
	tmp = (FragmentItem*)(*Iter);
	begin = tmp->starttime;
	for(Iter = pChunkList->begin(); Iter != pChunkList->end(); ++Iter)
	{
		tmp = (FragmentItem*)(*Iter);
		if(tmp->starttime == uStartTime){
			nResult = VO_RET_SOURCE2_OK;
			*pTimeOffset = (VO_U32)((uStartTime - begin)/GetTimeScaleMs());
			break;
		}
	}

	VOLOGI("GetChunkOffsetValueBySequenceStartTime---uStartTime=%llu, *pTimeOffset=%lu, nResult=%x",uStartTime, *pTimeOffset, nResult);

	return nResult;

}

VO_U32 ISS_ManifestManager::GetFirstChunkTimeStamp(VO_U64&  StartTime)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	
	list_T<FragmentItem *>::iterator Iter;
	FragmentItem* tmp = NULL;
	list_T<FragmentItem*>* pChunkList = NULL;

	if(NULL != m_pPlaySession->pVideoChunkList){
		pChunkList = m_pPlaySession->pVideoChunkList;
	}else if(NULL != m_pPlaySession->pAudioChunkList){
		pChunkList = m_pPlaySession->pAudioChunkList;	
	}else if(NULL != m_pPlaySession->pTextChunkList){
		pChunkList = m_pPlaySession->pTextChunkList;		
	}

	if((NULL == pChunkList) || (!pChunkList->size())){
		return VO_RET_SOURCE2_FAIL;
	}
	
	Iter = pChunkList->begin();
	tmp = (FragmentItem*)(*Iter);
	StartTime = tmp->starttime;

	VOLOGI("GetFirstChunkTimeStamp---StartTime=%llu, nResult=%x",StartTime, nResult);

	return nResult;

}


VO_U32 ISS_ManifestManager::DoumpTheRawData(VO_PBYTE pBuffer, VO_U32 nLen, VO_CHAR* pName)
{
	FILE * pFile;
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VO_U32 tmpsize = 0;
	char pFilename[36] = {0};
#ifdef _WIN32
	memcpy(pFilename, "d:\\", strlen("d:\\"));
#else
	memcpy(pFilename, "/sdcard/", strlen("/sdcard/"));
#endif
	strcat(pFilename, pName);

	VOLOGI("pBuffer=%lu, nLen=%lu", pBuffer, nLen);
	if (!nLen || !pBuffer)
	{
		return nResult;
	}

	pFile = fopen (pFilename,"wb");
	if (pFile!=NULL)
	{
		tmpsize = fwrite (pBuffer , 1 , nLen, pFile );
		fclose (pFile);
		nResult=VO_RET_SOURCE2_OK;
	}

	VOLOGI("tmpsize=%lu, result=%lu", tmpsize, nResult);
	return nResult;
}
