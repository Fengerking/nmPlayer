/********************************************************************
* Copyright 2003 ~ 2012 by VisualOn Software, Inc.
* All modifications are confidential and proprietary information
* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
*********************************************************************
* File Name: 
*            SourceSelecter.cpp
*
* Project:
* contents/description: a serious of operation func for ProgramInfo
*            
***************************** Change History**************************
* 
*    DD/MMM/YYYY     Code Ver     Description             Author
*    -----------     --------     -----------             ------
*    01-28-2013        1.0							     Aiven Yang
*
**********************************************************************/


#include "SourceSelecter.h"
#include "voLog.h"
#include "voProgramInfo.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

SourceSelecter::SourceSelecter()
{
	memset(m_pOldStreamInfo,0x0,sizeof(m_pOldStreamInfo));
	memset(m_nTrackList, 0x0, sizeof(m_nTrackList));
}

SourceSelecter::~SourceSelecter()
{
	for( int i = 0;i<10;i++)
	{
		ReleaseStreamInfoOP_T(m_pOldStreamInfo[i]);
		m_pOldStreamInfo[i]=NULL;
	}

}


VO_U32 SourceSelecter::SelectStream( _STREAM_INFO *pOldStreamInfo, _STREAM_INFO* pStreamInfo, vo_singlelink_list<VO_U32>* pTrackIDList )
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VO_U32 index = 0;

	if(!pStreamInfo || !pTrackIDList){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	VOLOGI("uStreamID=%lu,track count=%lu",pStreamInfo->uStreamID, pStreamInfo->uTrackCount);
	
	for(index = 0; index < pStreamInfo->uTrackCount; index++){
		PrintTrack("SelectStream---new", pStreamInfo->ppTrackInfo[index]);
	}
	//if only copy the oldstreaminfo when the stream id is different.
	if(m_pOldStreamInfo[0]){
		VOLOGI("old---uStreamID=%lu,track count=%lu",m_pOldStreamInfo[0]->uStreamID, m_pOldStreamInfo[0]->uTrackCount);
		
		for( index = 0; index < m_pOldStreamInfo[0]->uTrackCount; index++){
			PrintTrack("SelectStream---old", m_pOldStreamInfo[0]->ppTrackInfo[index]);
		}
	}
	//start to select the track
	memset(m_nTrackList, 0x0, sizeof(m_nTrackList));
/*
	//first check whether the track has been seleceted 
	for(VO_U32 type = VO_DATASOURCE_TT_AUDIO; type <= VO_DATASOURCE_TT_MUXGROUP; type++)
	{
		CheckSelectedTrack((VO_DATASOURCE_TRACK_TYPE)type, m_pOldStreamInfo, pStreamInfo);
	}
*/
	//second select the track by order.
	for(VO_U32 type = VO_DATASOURCE_TT_AUDIO; type <= VO_DATASOURCE_TT_MUXGROUP; type++)
	{
		if(VO_SOURCE2_SELECT_SELECTED == (m_nTrackList[VO_DATASOURCE_TT_AUDIOGROUP].uSelInfo&VO_SOURCE2_SELECT_SELECTED) && 
			type == VO_DATASOURCE_TT_AUDIO){
			continue;
		}else if(VO_SOURCE2_SELECT_SELECTED == (m_nTrackList[VO_DATASOURCE_TT_VIDEOGROUP].uSelInfo&VO_SOURCE2_SELECT_SELECTED) && 
			type == VO_DATASOURCE_TT_VIDEO){
			continue;
		}else if(VO_SOURCE2_SELECT_SELECTED == (m_nTrackList[VO_DATASOURCE_TT_SUBTITLEGROUP].uSelInfo&VO_SOURCE2_SELECT_SELECTED) && 
			type == VO_DATASOURCE_TT_SUBTITLE){
			continue;
		}else if(VO_SOURCE2_SELECT_SELECTED == (m_nTrackList[VO_DATASOURCE_TT_AUDIO].uSelInfo&VO_SOURCE2_SELECT_SELECTED) && 
			type == VO_DATASOURCE_TT_AUDIOGROUP){
			continue;
		}else if(VO_SOURCE2_SELECT_SELECTED == (m_nTrackList[VO_DATASOURCE_TT_VIDEO].uSelInfo&VO_SOURCE2_SELECT_SELECTED) && 
			type == VO_DATASOURCE_TT_VIDEOGROUP){
			continue;
		}else if(VO_SOURCE2_SELECT_SELECTED == (m_nTrackList[VO_DATASOURCE_TT_SUBTITLE].uSelInfo&VO_SOURCE2_SELECT_SELECTED) && 
			type == VO_DATASOURCE_TT_SUBTITLEGROUP){
			continue;
		}
		SelectFavoriteTrack((VO_DATASOURCE_TRACK_TYPE)type, m_pOldStreamInfo, pStreamInfo);
	}


	nResult = MergeTrackID(pTrackIDList);
	MergeStreamInfo(pStreamInfo);

	return nResult;
}


VO_U32 SourceSelecter::SelectTrack(_STREAM_INFO *pStreamInfo, VO_U32 TrackID, vo_singlelink_list<VO_U32>* pTrackIDList)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* pTrack = NULL;
	
	if(!pStreamInfo ||!pTrackIDList){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}
	
	memset(m_nTrackList, 0x0, sizeof(m_nTrackList));


	VOLOGI("uStreamID=%lu,track count=%lu, TrackID=%lu",pStreamInfo->uStreamID, pStreamInfo->uTrackCount, TrackID);
	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++){
		PrintTrack("SelectTrack---new", pStreamInfo->ppTrackInfo[index]);
	}
	
	if(m_pOldStreamInfo[0]){
		VOLOGI("old---uStreamID=%lu,track count=%lu",m_pOldStreamInfo[0]->uStreamID, m_pOldStreamInfo[0]->uTrackCount);		
		for(VO_U32 index = 0; index < m_pOldStreamInfo[0]->uTrackCount; index++){
			PrintTrack("SelectTrack---old", m_pOldStreamInfo[0]->ppTrackInfo[index]);
		}
	}

	for(VO_U32 type = VO_DATASOURCE_TT_AUDIO; type <= VO_DATASOURCE_TT_MUXGROUP; type++)
	{
		if(VO_RET_SOURCE2_OK == FindSelectedTrackByType((VO_DATASOURCE_TRACK_TYPE)type, pStreamInfo, &pTrack)){
			UpdateTrackList(pTrack);
		}
	}

	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		pTrack = pStreamInfo->ppTrackInfo[index];

		if(TrackID == pTrack->uOutSideTrackID && VO_SOURCE2_SELECT_DISABLE != (pTrack->uSelInfo&VO_SOURCE2_SELECT_DISABLE)){
			PrintTrack("SelectTrack---ok", pTrack);
			UpdateTrackList(pTrack);
			nResult = VO_RET_SOURCE2_OK;
			break;
		}
	}
	
	nResult = MergeTrackID(pTrackIDList);
	MergeStreamInfo(pStreamInfo);

	return nResult;
}


VO_U32 SourceSelecter::SelectTrack(_STREAM_INFO *pStreamInfo, VO_DATASOURCE_TRACK_TYPE Type, VO_CHAR* pLanguage, vo_singlelink_list<VO_U32>* pTrackIDList)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* pTrack = NULL;
	

	if(!pStreamInfo || !pLanguage || !strlen(pLanguage) ||!pTrackIDList){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	memset(m_nTrackList, 0x0, sizeof(m_nTrackList));

	VOLOGI("uStreamID=%lu,track count=%lu, type=%lu, Language=%s",pStreamInfo->uStreamID, pStreamInfo->uTrackCount, Type, pLanguage);


	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++){
		PrintTrack("SelectTrack---new", pStreamInfo->ppTrackInfo[index]);
	}
	
	if(m_pOldStreamInfo[0]){
		VOLOGI("old---uStreamID=%lu,track count=%lu",m_pOldStreamInfo[0]->uStreamID, m_pOldStreamInfo[0]->uTrackCount);				
		for(VO_U32 index = 0; index < m_pOldStreamInfo[0]->uTrackCount; index++){
			PrintTrack("SelectTrack---old", m_pOldStreamInfo[0]->ppTrackInfo[index]);
		}
	}

	for(VO_U32 type = VO_DATASOURCE_TT_AUDIO; type <= VO_DATASOURCE_TT_MUXGROUP; type++)
	{
		if(VO_RET_SOURCE2_OK == FindSelectedTrackByType((VO_DATASOURCE_TRACK_TYPE)type, pStreamInfo, &pTrack)){
			UpdateTrackList(pTrack);
		}
	}

	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		pTrack = pStreamInfo->ppTrackInfo[index];

		if(VO_RET_SOURCE2_OK == IsNameSame(Type, pStreamInfo, pLanguage, &pTrack)){
			PrintTrack("SelectTrack---ok", pTrack);
			UpdateTrackList(pTrack);
			nResult = VO_RET_SOURCE2_OK;
			break;
		}
	}
	
	nResult = MergeTrackID(pTrackIDList);
	MergeStreamInfo(pStreamInfo);

	return nResult;
}


VO_U32 SourceSelecter::MergeStreamInfo(_STREAM_INFO *pStreamInfo)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	_TRACK_INFO* pTrack = NULL;

	if (!pStreamInfo)
	{
		return VO_RET_SOURCE2_FAIL;
	}

	VOLOGR("copy the stream!");
	//disable those process
/*
	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		pTrack = pStreamInfo->ppTrackInfo[index];

		if(VO_RET_SOURCE2_OK == IsTrackSelect(pTrack->uOutSideTrackID) &&
			pTrack->uTrackType == VO_DATASOURCE_TT_MUXGROUP){
			VOLOGI("MergeStreamInfo---muxgroup selected!");
			return VO_RET_SOURCE2_FAIL;
		}
	}
*/
	if( m_pOldStreamInfo[1])
		CopyStreamInfoOP_T(m_pOldStreamInfo[1],&m_pOldStreamInfo[2]);

	if(m_pOldStreamInfo[0])
		CopyStreamInfoOP_T(m_pOldStreamInfo[0],&m_pOldStreamInfo[1]);

	CopyStreamInfoOP_T(pStreamInfo, &m_pOldStreamInfo[0]);
	

	for(VO_U32 index = 0; index < m_pOldStreamInfo[0]->uTrackCount; index++)
	{
		pTrack = m_pOldStreamInfo[0]->ppTrackInfo[index];

		if(VO_RET_SOURCE2_OK == IsTrackSelect(pTrack->uOutSideTrackID)){
			pTrack->uSelInfo = pTrack->uSelInfo|VO_SOURCE2_SELECT_SELECTED;
		}else{
			pTrack->uSelInfo = pTrack->uSelInfo&~VO_SOURCE2_SELECT_SELECTED;
		}
	}


	return nResult;
}

VO_U32 SourceSelecter::IsTrackSelect(VO_U32 TrackID)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;

	for(VO_U32 i = VO_DATASOURCE_TT_AUDIO; i <= VO_DATASOURCE_TT_MUXGROUP; i++)
	{
		_TRACK_INFO* pTrack = &m_nTrackList[i];
		
		if(VO_SOURCE2_SELECT_SELECTED == (pTrack->uSelInfo&VO_SOURCE2_SELECT_SELECTED) && 
			pTrack->uOutSideTrackID == TrackID){
			nResult = VO_RET_SOURCE2_OK;
			break;
		}
	}

	return nResult;
}

VO_U32 SourceSelecter::SelectFavoriteTrack(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO **pOldStreamInfo, _STREAM_INFO *pStreamInfo)
{
	_TRACK_INFO* pTrack = NULL;
	VO_U32 ASID = 0;
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VOLOGI("type=%lu",type);
	if(VO_RET_SOURCE2_OK == CheckSelectedTrack((VO_DATASOURCE_TRACK_TYPE)type, pOldStreamInfo[0], pStreamInfo, &pTrack)){
		PrintTrack("SelectFavoriteTrack---selected track", pTrack);
	}
	else if(VO_RET_SOURCE2_OK == IsNameSame((VO_DATASOURCE_TRACK_TYPE)type, pOldStreamInfo[0], pStreamInfo, &pTrack)){
		PrintTrack("SelectFavoriteTrack---same name check 1", pTrack);
	}
	else if(VO_RET_SOURCE2_OK == IsNameSame((VO_DATASOURCE_TRACK_TYPE)type, pOldStreamInfo[1], pStreamInfo, &pTrack)){
		PrintTrack("SelectFavoriteTrack---same name check 2", pTrack);
	}
	else if(VO_RET_SOURCE2_OK == IsNameSame((VO_DATASOURCE_TRACK_TYPE)type, pOldStreamInfo[2], pStreamInfo, &pTrack)){
		PrintTrack("SelectFavoriteTrack---same name check 3", pTrack);
	}
	else if(VO_RET_SOURCE2_OK == EstimateSelInfo((VO_DATASOURCE_TRACK_TYPE)type, pStreamInfo, &pTrack)){
		PrintTrack("SelectFavoriteTrack---eatimate the selinfo", pTrack);
	}
	else if(VO_RET_SOURCE2_OK == MinASIDWithSameType((VO_DATASOURCE_TRACK_TYPE)type, pStreamInfo, &ASID) &&
		VO_RET_SOURCE2_OK == MinTPIDWithSameASID((VO_DATASOURCE_TRACK_TYPE)type, ASID, pStreamInfo, &pTrack)){
		PrintTrack("SelectFavoriteTrack---min asid and min tpid", pTrack);
	}
	else if(VO_RET_SOURCE2_OK == MinTPIDInMuxGroup((VO_DATASOURCE_TRACK_TYPE)type, pStreamInfo, &pTrack)){
		PrintTrack("SelectFavoriteTrack---min tpid in mux group", pTrack);
	}


	if(pTrack){
		UpdateTrackList(pTrack);
		nResult = VO_RET_SOURCE2_OK;
	}

	return nResult;
}

VO_U32 SourceSelecter::CheckSelectedTrack(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pOldStreamInfo, _STREAM_INFO *pStreamInfo, _TRACK_INFO** ppTrack)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* pTrack = NULL;
	VO_U32 ASID = 0xffffffff;
	VO_U32 TPID = 0xffffffff;

	if(!pStreamInfo || !pOldStreamInfo){
		return nResult;
	}

/*
	//find the track from new stream.
	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		pTrack = pStreamInfo->ppTrackInfo[index];
	
		if(type == pTrack->uTrackType && VO_SOURCE2_SELECT_SELECTED==(pTrack->uSelInfo&VO_SOURCE2_SELECT_SELECTED)){
			ASID = pTrack->uASTrackID;
			TPID = pTrack->uFPTrackID;
			
			nResult = VO_RET_SOURCE2_OK;			
			break;
		}
	}
*/
	//find the selected track from old stream.
	//check the selected track;
	for(VO_U32 index = 0; index < pOldStreamInfo->uTrackCount; index++)
	{
		pTrack = pOldStreamInfo->ppTrackInfo[index];
	
		if(type == pTrack->uTrackType && VO_SOURCE2_SELECT_SELECTED==(pTrack->uSelInfo&VO_SOURCE2_SELECT_SELECTED)){
			ASID = pTrack->uASTrackID;
			TPID = pTrack->uFPTrackID;
			
			nResult = VO_RET_SOURCE2_OK;			
			break;
		}
	}

	//check the selected track in current stream;
	if(VO_RET_SOURCE2_OK == nResult){
		nResult = VO_RET_SOURCE2_FAIL;
		for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
		{
			pTrack = pStreamInfo->ppTrackInfo[index];
		
			if(type == pTrack->uTrackType && pTrack->uASTrackID == ASID && pTrack->uFPTrackID == TPID){
				*ppTrack = pTrack;
				nResult = VO_RET_SOURCE2_OK;			
				break;
			}
		}
	}
/*
	//if can't find the selected track from old stream, find the selected track in new stream
	if(NULL == pTargetTrack){
		nResult = FindSelectedTrackByType(type, pStreamInfo, &pTargetTrack);
	}
*/
	return nResult;
}


VO_U32 SourceSelecter::IsTypeSame(VO_DATASOURCE_TRACK_TYPE type1, VO_DATASOURCE_TRACK_TYPE type2)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;

	if(type1 == type2){
		nResult = VO_RET_SOURCE2_OK;
	}
	else{
		switch(type1)
		{
		case VO_DATASOURCE_TT_AUDIO:
		case VO_DATASOURCE_TT_AUDIOGROUP:
			if(VO_DATASOURCE_TT_AUDIO == type2 ||VO_DATASOURCE_TT_AUDIOGROUP == type2){
				nResult = VO_RET_SOURCE2_OK;
			}
			break;
		case VO_DATASOURCE_TT_VIDEO:
		case VO_DATASOURCE_TT_VIDEOGROUP:
			if(VO_DATASOURCE_TT_VIDEO == type2 ||VO_DATASOURCE_TT_VIDEOGROUP == type2){
				nResult = VO_RET_SOURCE2_OK;
			}
			break;
		case VO_DATASOURCE_TT_SUBTITLE:
		case VO_DATASOURCE_TT_SUBTITLEGROUP:
			if(VO_DATASOURCE_TT_SUBTITLE == type2 ||VO_DATASOURCE_TT_SUBTITLEGROUP == type2){
				nResult = VO_RET_SOURCE2_OK;
			}
			break;
		}
	}


	return nResult;
}

VO_U32 SourceSelecter:: IsNameSame(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pOldStreamInfo, _STREAM_INFO *pStreamInfo, _TRACK_INFO** ppTrack)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* tmpTrack = NULL;
	_TRACK_INFO* pTrack = NULL;

	if(!pOldStreamInfo || !pStreamInfo){
		return nResult;
	}

	if(VO_RET_SOURCE2_OK != FindSelectedTrackByType(type, pOldStreamInfo, &tmpTrack)){
		return nResult;
	}


	//check the asid and name are all same	
	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		pTrack = pStreamInfo->ppTrackInfo[index];

		if(VO_RET_SOURCE2_OK != IsTypeSame(type, (VO_DATASOURCE_TRACK_TYPE)pTrack->uTrackType)){
			continue;
		}


		switch(pTrack->uTrackType)
		{
		case VO_DATASOURCE_TT_AUDIO:
		case VO_DATASOURCE_TT_AUDIOGROUP:
			if(!strlen(pTrack->sAudioInfo.chLanguage)){
				break;
			}

			if(!memcmp(tmpTrack->sAudioInfo.chLanguage, pTrack->sAudioInfo.chLanguage, sizeof(tmpTrack->sAudioInfo.chLanguage)) &&
				tmpTrack->uASTrackID == pTrack->uASTrackID){
				*ppTrack = pTrack;
				nResult = VO_RET_SOURCE2_OK;
			}
			break;
		case VO_DATASOURCE_TT_SUBTITLE:
		case VO_DATASOURCE_TT_SUBTITLEGROUP:
			if(!strlen(pTrack->sSubtitleInfo.chLanguage)){
				break;
			}

			if(!memcmp(tmpTrack->sSubtitleInfo.chLanguage, pTrack->sSubtitleInfo.chLanguage, sizeof(tmpTrack->sSubtitleInfo.chLanguage)) &&
				tmpTrack->uASTrackID == pTrack->uASTrackID){
				*ppTrack = pTrack;				
				nResult = VO_RET_SOURCE2_OK;
			}
			break;
		default:
			break;
		}

		if(VO_RET_SOURCE2_OK == nResult){
			break;
		}
		
	}


	//check the name is  same	
	if(VO_RET_SOURCE2_OK != nResult){
		for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
		{
			pTrack = pStreamInfo->ppTrackInfo[index];
		
			if(VO_RET_SOURCE2_OK != IsTypeSame(type, (VO_DATASOURCE_TRACK_TYPE)pTrack->uTrackType)){
				continue;
			}
		
		
			switch(pTrack->uTrackType)
			{
			case VO_DATASOURCE_TT_AUDIO:
			case VO_DATASOURCE_TT_AUDIOGROUP:
				if(!strlen(pTrack->sAudioInfo.chLanguage)){
					break;
				}
		
				if(!memcmp(tmpTrack->sAudioInfo.chLanguage, pTrack->sAudioInfo.chLanguage, sizeof(tmpTrack->sAudioInfo.chLanguage))){
					*ppTrack = pTrack;
					nResult = VO_RET_SOURCE2_OK;
				}
				break;
			case VO_DATASOURCE_TT_SUBTITLE:
			case VO_DATASOURCE_TT_SUBTITLEGROUP:
				if(!strlen(pTrack->sSubtitleInfo.chLanguage)){
					break;
				}
		
				if(!memcmp(tmpTrack->sSubtitleInfo.chLanguage, pTrack->sSubtitleInfo.chLanguage, sizeof(tmpTrack->sSubtitleInfo.chLanguage))){
					*ppTrack = pTrack;				
					nResult = VO_RET_SOURCE2_OK;
				}
				break;
			default:
				break;
			}

			if(VO_RET_SOURCE2_OK == nResult){
				break;
			}
			
		}

	}


	return nResult;
}

VO_U32 SourceSelecter::IsNameSame(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pStreamInfo, VO_CHAR* pLanguage, _TRACK_INFO** ppTrack)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* pTrack = NULL;

	if(!pStreamInfo || !pLanguage || !strlen(pLanguage)){
		return nResult;
	}

	//check the name is  same	
	if(VO_RET_SOURCE2_OK != nResult){
		for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
		{
			pTrack = pStreamInfo->ppTrackInfo[index];
		
			if(VO_RET_SOURCE2_OK != IsTypeSame(type, (VO_DATASOURCE_TRACK_TYPE)pTrack->uTrackType)){
				continue;
			}
		
			switch(pTrack->uTrackType)
			{
			case VO_DATASOURCE_TT_AUDIO:
			case VO_DATASOURCE_TT_AUDIOGROUP:
				if(strlen(pTrack->sAudioInfo.chLanguage) && !strcmp(pLanguage, pTrack->sAudioInfo.chLanguage)){
					*ppTrack = pTrack;
					nResult = VO_RET_SOURCE2_OK;
				}
				break;
			case VO_DATASOURCE_TT_SUBTITLE:
			case VO_DATASOURCE_TT_SUBTITLEGROUP:		
				if(strlen(pTrack->sSubtitleInfo.chLanguage) && !strcmp(pLanguage, pTrack->sSubtitleInfo.chLanguage)){
					*ppTrack = pTrack;				
					nResult = VO_RET_SOURCE2_OK;
				}
				break;
			default:
				break;
			}

			if(VO_RET_SOURCE2_OK == nResult){
				break;
			}
		}

	}


	return nResult;
}


VO_U32 SourceSelecter::FindSelectedTrackByType(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pStreamInfo, _TRACK_INFO** ppTrack)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* pTrack = NULL;

	if(!pStreamInfo){
		return nResult;
	}

	//check the default first;
	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		pTrack = pStreamInfo->ppTrackInfo[index];

		if(VO_RET_SOURCE2_OK == IsTypeSame(type, (VO_DATASOURCE_TRACK_TYPE)pTrack->uTrackType) && VO_SOURCE2_SELECT_SELECTED==(pTrack->uSelInfo&VO_SOURCE2_SELECT_SELECTED)){
			*ppTrack = pTrack;
			PrintTrack("FindSelectedTrackByType", pTrack);
			nResult = VO_RET_SOURCE2_OK;			
			break;
		}
	}

	return nResult;
}

VO_U32 SourceSelecter::EstimateSelInfo(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pStreamInfo, _TRACK_INFO** ppTrack)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* pTrack = NULL;

	if(!pStreamInfo){
		return nResult;
	}

	//check the default first;
	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		pTrack = pStreamInfo->ppTrackInfo[index];

		if(type == pTrack->uTrackType && VO_SOURCE2_SELECT_DEFAULT==(pTrack->uSelInfo&VO_SOURCE2_SELECT_DEFAULT)){
			*ppTrack = pTrack;							
			nResult = VO_RET_SOURCE2_OK;			
			break;
		}
	}

	//check the recommend second;
	if(VO_RET_SOURCE2_OK != nResult){
		for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
		{
			pTrack = pStreamInfo->ppTrackInfo[index];
		
			if(type == pTrack->uTrackType && VO_SOURCE2_SELECT_RECOMMEND ==(pTrack->uSelInfo&VO_SOURCE2_SELECT_RECOMMEND)){
				*ppTrack = pTrack;				
				nResult = VO_RET_SOURCE2_OK;
				break;
			}
		}

	}


	//check the force third;
	if(VO_RET_SOURCE2_OK != nResult){
		for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
		{
			pTrack = pStreamInfo->ppTrackInfo[index];
		
			if(type == pTrack->uTrackType && VO_SOURCE2_SELECT_FORCE ==(pTrack->uSelInfo&VO_SOURCE2_SELECT_FORCE)){
				*ppTrack = pTrack;				
				nResult = VO_RET_SOURCE2_OK;
				break;
			}
		}

	}

	return nResult;

}

VO_U32 SourceSelecter::MinASIDWithSameType(VO_DATASOURCE_TRACK_TYPE type,_STREAM_INFO *pStreamInfo, VO_U32* pASID)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* pTrack = NULL;

	if(!pASID){
		return nResult;
	}

	if(!pStreamInfo){
		return nResult;
	}

	*pASID = 0xFFFFFFFF;
	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		pTrack = pStreamInfo->ppTrackInfo[index];
		
		if(pTrack->uTrackType == type){
			*pASID = VODS_MIN(*pASID, pTrack->uASTrackID);
			nResult = VO_RET_SOURCE2_OK;
		}
	}

	return nResult;

}

VO_U32 SourceSelecter::MinTPIDWithSameASID(VO_DATASOURCE_TRACK_TYPE type, VO_U32 ASID, _STREAM_INFO *pStreamInfo,  _TRACK_INFO** ppTrack)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* pTrack = NULL;
	VO_U32 FPID = 0xFFFFFFFF;


	if(!pStreamInfo){
		return nResult;
	}

	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		pTrack = pStreamInfo->ppTrackInfo[index];
		
		if(pTrack->uTrackType == type && pTrack->uASTrackID == ASID){
			FPID = VODS_MIN(FPID, pTrack->uFPTrackID);
			nResult = VO_RET_SOURCE2_OK;	
		}
	}

	if(VO_RET_SOURCE2_OK == nResult){
		for(VO_U32 i = 0; i < pStreamInfo->uTrackCount; i++)
		{
			pTrack = pStreamInfo->ppTrackInfo[i];
			
			if(pTrack->uTrackType == type && pTrack->uFPTrackID == FPID){
				*ppTrack = pTrack;
				nResult = VO_RET_SOURCE2_OK;
				break;
			}
		}

	}

	return nResult;

}

VO_U32 SourceSelecter::MinTPIDInMuxGroup(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pStreamInfo, _TRACK_INFO** ppTrack)
{
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	_TRACK_INFO* pTrack = NULL;
	VO_U32 	VASID = 0;

	if(!pStreamInfo){
		return nResult;
	}


	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		pTrack = pStreamInfo->ppTrackInfo[index];

		if(pTrack->uTrackType == VO_DATASOURCE_TT_VIDEO){
			VASID = pTrack->uASTrackID;
			nResult = VO_RET_SOURCE2_OK;
			break;
		}
	}

	if(VO_RET_SOURCE2_OK == nResult){
		nResult = MinTPIDWithSameASID(type, VASID, pStreamInfo, ppTrack);
	}

	return nResult;

}

VO_U32 SourceSelecter::UpdateTrackList(_TRACK_INFO* pTrack)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	if(!pTrack){
		return nResult;
	}

	memcpy(&m_nTrackList[pTrack->uTrackType], pTrack, sizeof(_TRACK_INFO));
	m_nTrackList[pTrack->uTrackType].uSelInfo = m_nTrackList[pTrack->uTrackType].uSelInfo|VO_SOURCE2_SELECT_SELECTED;

	if( pTrack->uTrackType == VO_DATASOURCE_TT_AUDIOGROUP )
		m_nTrackList[ VO_DATASOURCE_TT_AUDIO ].uSelInfo = m_nTrackList[VO_DATASOURCE_TT_AUDIO].uSelInfo & ~VO_SOURCE2_SELECT_SELECTED;
	else if( pTrack->uTrackType == VO_DATASOURCE_TT_AUDIO )
		m_nTrackList[ VO_DATASOURCE_TT_AUDIOGROUP ].uSelInfo = m_nTrackList[VO_DATASOURCE_TT_AUDIOGROUP].uSelInfo & ~VO_SOURCE2_SELECT_SELECTED;
	else if( pTrack->uTrackType == VO_DATASOURCE_TT_VIDEOGROUP )
		m_nTrackList[ VO_DATASOURCE_TT_VIDEO ].uSelInfo = m_nTrackList[VO_DATASOURCE_TT_VIDEO].uSelInfo & ~VO_SOURCE2_SELECT_SELECTED;
	else if( pTrack->uTrackType == VO_DATASOURCE_TT_VIDEO )
		m_nTrackList[ VO_DATASOURCE_TT_VIDEOGROUP ].uSelInfo = m_nTrackList[VO_DATASOURCE_TT_VIDEOGROUP].uSelInfo & ~VO_SOURCE2_SELECT_SELECTED;
	else if( pTrack->uTrackType == VO_DATASOURCE_TT_SUBTITLE )
		m_nTrackList[ VO_DATASOURCE_TT_SUBTITLEGROUP ].uSelInfo = m_nTrackList[VO_DATASOURCE_TT_SUBTITLEGROUP].uSelInfo & ~VO_SOURCE2_SELECT_SELECTED;
	else if( pTrack->uTrackType == VO_DATASOURCE_TT_SUBTITLEGROUP )
		m_nTrackList[ VO_DATASOURCE_TT_SUBTITLE ].uSelInfo = m_nTrackList[VO_DATASOURCE_TT_SUBTITLE].uSelInfo & ~VO_SOURCE2_SELECT_SELECTED;

	PrintTrack("UpdateTrackList", pTrack);
	return nResult;
}

VO_U32 SourceSelecter::MergeTrackID(vo_singlelink_list<VO_U32>* pTrackIDList)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	if(!pTrackIDList){
		return nResult;
	}
	
	for(VO_U32 i = VO_DATASOURCE_TT_AUDIO; i <= VO_DATASOURCE_TT_MUXGROUP; i++)
	{
		_TRACK_INFO* pTrack = &m_nTrackList[i];

		if(VO_SOURCE2_SELECT_SELECTED == (pTrack->uSelInfo&VO_SOURCE2_SELECT_SELECTED)){
			pTrackIDList->push_back( pTrack->uOutSideTrackID );
			PrintTrack("MergeTrackID", pTrack);
			nResult = VO_RET_SOURCE2_OK;
		}
	}

	return nResult;
}


VO_VOID SourceSelecter::PrintTrack(const char* pFunction,_TRACK_INFO* pTrack)
{
	if(!pTrack){
		return;
	}

	if(!pFunction){
		return;
	}

//	VOLOGI("pFunction is %s",pFunction);

	if(VO_DATASOURCE_TT_AUDIO == pTrack->uTrackType || VO_DATASOURCE_TT_AUDIOGROUP == pTrack->uTrackType){
		VOLOGI("%s---uOutSideTrackID=%lu, uASTrackID=%lu, uFPTrackID=%lu, uSelInfo=%lu, uTrackType=%lu, sAudioInfo.chLanguage=%s", 
			pFunction, pTrack->uOutSideTrackID, pTrack->uASTrackID, pTrack->uFPTrackID, pTrack->uSelInfo, pTrack->uTrackType, pTrack->sAudioInfo.chLanguage);	
	}else if(VO_DATASOURCE_TT_SUBTITLE == pTrack->uTrackType || VO_DATASOURCE_TT_SUBTITLEGROUP == pTrack->uTrackType){
		VOLOGI("%s---uOutSideTrackID=%lu, uASTrackID=%lu, uFPTrackID=%lu, uSelInfo=%lu, uTrackType=%lu, sSubtitleInfo.chLanguage=%s", 
			pFunction, pTrack->uOutSideTrackID, pTrack->uASTrackID, pTrack->uFPTrackID, pTrack->uSelInfo, pTrack->uTrackType, pTrack->sSubtitleInfo.chLanguage);	
	}else if(VO_DATASOURCE_TT_VIDEO == pTrack->uTrackType || VO_DATASOURCE_TT_VIDEOGROUP == pTrack->uTrackType || 
			VO_DATASOURCE_TT_MUXGROUP == pTrack->uTrackType){
		VOLOGI("%s---uOutSideTrackID=%lu, uASTrackID=%lu, uFPTrackID=%lu, uSelInfo=%lu, uTrackType=%lu, uBitrate=%lu", 
			pFunction, pTrack->uOutSideTrackID, pTrack->uASTrackID, pTrack->uFPTrackID, pTrack->uSelInfo, pTrack->uTrackType, pTrack->uBitrate); 	
	}else{
		VOLOGE("wrong track type!");
	}

}

#ifdef _VONAMESPACE
}
#endif

