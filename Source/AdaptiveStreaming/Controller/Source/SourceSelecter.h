/********************************************************************
* Copyright 2003 ~ 2012 by VisualOn Software, Inc.
* All modifications are confidential and proprietary information
* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
*********************************************************************
* File Name: 
*            SourceSelecter.h
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

#ifndef _SOURCESELECTER_H_

#define _SOURCESELECTER_H_
#include "list_T.h"
#include "voType.h"
#include "voSource2.h"
#include "voDSType.h"
#include "vo_singlelink_list.hpp"


#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class SourceSelecter
{
public:
	SourceSelecter();
	~SourceSelecter();
	
	VO_U32 SelectStream( _STREAM_INFO *pOldStreamInfo, _STREAM_INFO* pStreamInfo, vo_singlelink_list<VO_U32>* pTrackIDList );
	VO_U32 SelectTrack(_STREAM_INFO *pStreamInfo, VO_U32 TrackID, vo_singlelink_list<VO_U32>* pTrackIDList);
	VO_U32 SelectTrack(_STREAM_INFO *pStreamInfo, VO_DATASOURCE_TRACK_TYPE Type, VO_CHAR* pLanguage, vo_singlelink_list<VO_U32>* pTrackIDList);
protected:
	VO_U32 SelectFavoriteTrack(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO **pOldStreamInfo, _STREAM_INFO *pStreamInfo);
	VO_U32 CheckSelectedTrack(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pOldStreamInfo, _STREAM_INFO *pStreamInfo, _TRACK_INFO** ppTrack);
	VO_U32 IsNameSame(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pOldStreamInfo, _STREAM_INFO *pStreamInfo, _TRACK_INFO** ppTrack);
	VO_U32 IsNameSame(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pStreamInfo, VO_CHAR* pLanguage, _TRACK_INFO** ppTrack);		
	VO_U32 FindSelectedTrackByType(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pStreamInfo, _TRACK_INFO** ppTrack);
	VO_U32 EstimateSelInfo(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pStreamInfo, _TRACK_INFO** ppTrack);
	VO_U32 MinASIDWithSameType(VO_DATASOURCE_TRACK_TYPE type,_STREAM_INFO *pStreamInfo, VO_U32* pASID);
	VO_U32 MinTPIDWithSameASID(VO_DATASOURCE_TRACK_TYPE type, VO_U32 ASID, _STREAM_INFO *pStreamInfo,  _TRACK_INFO** ppTrack);
	VO_U32 MinTPIDInMuxGroup(VO_DATASOURCE_TRACK_TYPE type, _STREAM_INFO *pStreamInfo, _TRACK_INFO** ppTrack);
	VO_U32 UpdateTrackList(_TRACK_INFO* pTrack);
	VO_U32 MergeTrackID(vo_singlelink_list<VO_U32>* pTrackIDList);
	VO_U32 MergeStreamInfo(_STREAM_INFO *pStreamInfo);
	VO_U32 IsTrackSelect(VO_U32 TrackID);
	VO_U32 IsTypeSame(VO_DATASOURCE_TRACK_TYPE type1, VO_DATASOURCE_TRACK_TYPE type2);
	VO_VOID PrintTrack(const char* pFunction,_TRACK_INFO* pTrack);	
private:
	_STREAM_INFO*	m_pOldStreamInfo[10];
	_TRACK_INFO	m_nTrackList[VO_DATASOURCE_TT_MUXGROUP+1];
};

#ifdef _VONAMESPACE
}
#endif

#endif

