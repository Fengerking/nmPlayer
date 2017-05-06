/********************************************************************
* Copyright 2003 ~ 2012 by VisualOn Software, Inc.
* All modifications are confidential and proprietary information
* of VisualOn Software, Inc. ALL RIGHTS RESERVED.
*********************************************************************
* File Name: 
*            voProgramInfoOp.h
*
* Project:
* contents/description: a serious of operation func for ProgramInfo
*            
***************************** Change History**************************
* 
*    DD/MMM/YYYY     Code Ver     Description             Author
*    -----------     --------     -----------             ------
*    01-06-2013        1.0							     Leon Huang
*                                             
**********************************************************************/

#ifndef __VOPROGRAMINFOOP_H__
#define __VOPROGRAMINFOOP_H__


#include "voToolUtility.h"
#include "voLog.h"
 
#include "voDSType.h"
#include "voCMutex.h"
#include "voAdaptiveStreamParserWrapper.h"

#include "vo_singlelink_list.hpp"
#include "SourceSelecter.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

//#define __TOSETSAMEID
#ifdef __TOSETSAMEID
#define __Delta             0x80
#else
#define __Delta             0x00
#endif

#define __ENDING_FRUUID		0x100 + __Delta
#define __BEGIN_FPUUID		0x80 + __Delta
#define __ENDING_ASUUID		0x80 - 1 + __Delta
#define __BEGIN_ASUUID		0X00 + __Delta


typedef struct
{
	VO_SOURCE2_TRACK_TYPE nType;
	VO_U32				  uFPTrackID;/*this trackID equals to FPTrackID*/
	VO_U32				  uASTrackID;
	VO_U32				  uUUID;/*outside id*/
}VODS_SELECTED_INFO;


#define _AUDIO_SELECTED		0x00000001
#define _VIDEO_SELECTED		0X00000002
#define _SUBTITLE_SELECTED	0X00000004


class voProgramInfoOp
{
public:
	voProgramInfoOp(VO_DATASOURCE_CALLBACK *pEVENTCB);
	virtual ~voProgramInfoOp();
public:
	virtual VO_VOID SetASParser(voAdaptiveStreamParserWrapper  * pASParser) { m_pASParser = pASParser; }
	virtual VO_BOOL TrackIDIsDirty();	
	virtual VO_VOID SetLanguage(VO_SOURCE2_TRACK_TYPE uType, VO_CHAR *pLanguage);

	/*those functions followed will be used by serveral threads, so add the lcok to make sure the data only can be reached one by one.*/
	virtual VO_U32 GetProgramInfo( VO_U32 uID, _PROGRAM_INFO **ppPragramInfo );
	virtual VO_U32 GetSource2ProgramInfo(VO_U32 uID, VO_SOURCE2_PROGRAM_INFO **ppPragramInfo );
	virtual VO_U32 GetCurSource2TrackInfo( VO_SOURCE2_TRACK_TYPE uType, VO_SOURCE2_TRACK_INFO **ppTrackInfo);
	virtual VO_U32 GetSelectedStreamID( VO_U32 *pStreamID );
	virtual VO_U32 GetSelectedTrackID( vo_singlelink_list<VODS_SELECTED_INFO> *pListSelInfo );
	virtual VO_U32 GetCurTrackInfo( VO_SOURCE2_TRACK_TYPE uType, _TRACK_INFO **pTrackInfo );

	virtual VO_U32 SelectStream( VO_U32 uStreamID, VO_U32 uFlag = 1);
	virtual VO_U32 SelectTrack( VO_U32 uOutSideTrackID );

	virtual VO_U32 ResetProgramInfo( _PROGRAM_INFO *pProgInfo );
	virtual VO_U32 ConstructProgramInfo(VO_U32 uOPFlag,VO_U32 uOPID, VO_U32 uProgramID, VO_U64 uMarkedID,_TRACK_INFO *pSourceTrackInfo, VO_U32 uFlag );
	virtual VO_VOID SetProgramType( VO_SOURCE2_PROGRAM_TYPE nType);
	
private:
	
	virtual VO_U32 AddTrack(VO_U32 uProgramID, _PROGRAM_INFO *pProgramInfoPlus, _TRACK_INFO *pSourceTrackInfo );
	virtual VO_U32 AddTrack(VO_U32 uProgramID, _PROGRAM_INFO *pProgramInfoPlus, VO_U32 uStreamID, _TRACK_INFO *pTrackInfo );
	virtual VO_BOOL CheckIsGroup(VO_U32 uProgramID, _PROGRAM_INFO *pProgramInfoPlus, VO_U32 uStreamID, _TRACK_INFO *pSourceTrackInfo);
	virtual VO_VOID ResetUUID( _PROGRAM_INFO *pProgInfo );
	virtual VO_U32 SelfSelTrack( VO_U32 uOutSideTrackID );
	virtual VO_U32 SelfSelTrack(VO_DATASOURCE_TRACK_TYPE type, VO_CHAR* pLanguage);	
	virtual VO_U32 SelfSelStream( VO_U32 uStreamID );
	virtual VO_VOID ResetSelInfo( _PROGRAM_INFO **ppProgInfo, VO_U32 uSelStreamID, vo_singlelink_list<VO_U32>& list );
	virtual VO_U32 SendEvent(VO_U32 uID );
	
	virtual VO_U32 DestroyProgramInfo( );
	virtual VO_VOID RemoveTrack(_PROGRAM_INFO *pProgramInfoPlus, VO_U32 uStreamID, VO_U32 uASID, VO_SOURCE2_TRACK_TYPE type);
	virtual VO_VOID RemoveTrackBySameASID( _PROGRAM_INFO *pPragramInfo,VO_U64 uMarkedID, VO_U32 uASID );
	virtual VO_VOID MeregeSelInfoBy2ID( _PROGRAM_INFO *pProgInfo, VO_U32 uSelStreamID, vo_singlelink_list< VODS_SELECTED_INFO > &list);
	
	virtual VO_U32 GetUUID( VO_U32 uStartNumber );
	virtual VO_VOID SetUUID( VO_U32 uID );

	virtual VO_VOID SortTrackInfo( _PROGRAM_INFO *pProgramInfo );
	virtual VO_VOID SetProgramType();
	
private:
	voCMutex m_lock;
	VO_SOURCE2_PROGRAM_TYPE m_nType;

	_PROGRAM_INFO	*m_pProgramInfo;
	_PROGRAM_INFO	*m_pProgramInfoPlus;
	_PROGRAM_INFO	*m_pProgramInfoPlusBackUp_DS;

//	voCMutex m_SelInfolock;
	vo_singlelink_list< VODS_SELECTED_INFO > m_listSelectedInfo;

	VO_SOURCE2_PROGRAM_INFO *m_pProgramInfoPlusBackUp;
	
	VO_U32 m_uHasSelInfo; /*_AUDIO_SELECTED; _VIDEO_SELECTED; _SUBTITLE_SELECTED; */

	/*0000,0000,XXXX,XXXX*/
	/*small than 0x80, it means that this track is assigned by Adaptive Streaming
	  Otherwise it means that this track is assigned by File Parser*/
	VO_BOOL m_bUUID[__ENDING_FRUUID];/*0x00<= ~ < 0x80 ( 0 ~ 127 )  0000,0000,01111,1111 */

	voAdaptiveStreamParserWrapper *m_pASParser;
	SourceSelecter m_SourceSelOp;

	VO_BOOL m_bTrackIDDirty;

	//send event for programinfo changed or other event;
	VO_DATASOURCE_CALLBACK *m_pEventCB;

	//for add track
	voCMutex m_addlock;
	VO_U32 m_uAddCounts;

	//Source2 trackinfo backup
	VO_SOURCE2_TRACK_INFO *m_pTrackInfoBK_Video;
	VO_SOURCE2_TRACK_INFO *m_pTrackInfoBK_Audio;
	VO_SOURCE2_TRACK_INFO *m_pTrackInfoBK_Subtitle;
};

#ifdef _VONAMESPACE
}
#endif
#endif  //__FAUDIOHEADDATAINFO_H__








