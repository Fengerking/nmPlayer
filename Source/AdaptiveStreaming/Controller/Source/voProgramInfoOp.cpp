
#include "voProgramInfoOp.h"
#include "voProgramInfo.h"
#include "voToolUtility.h"

//#define XMLDUMPER
#ifdef XMLDUMPER
#include "SourceInfoXMLDumper.h"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//#define __selecttrack_uselanguage



#define __CHECK_GROUP(A, B) { \
	if(	   A == VO_SOURCE2_TT_AUDIOGROUP \
		|| A == VO_SOURCE2_TT_VIDEOGROUP \
		|| A == VO_SOURCE2_TT_SUBTITLEGROUP \
		|| A == VO_SOURCE2_TT_MUXGROUP ) \
		B = 1; \
	else \
		B = 0; \
	}
voProgramInfoOp::voProgramInfoOp(VO_DATASOURCE_CALLBACK *pEVENTCB)
:m_nType(VO_SOURCE2_STREAM_TYPE_UNKNOWN)
,m_pProgramInfo(NULL)
,m_pProgramInfoPlus(NULL)
,m_pProgramInfoPlusBackUp(NULL)
,m_pProgramInfoPlusBackUp_DS(NULL)
,m_uHasSelInfo(0)
,m_pASParser(NULL)
,m_bTrackIDDirty(VO_FALSE)
,m_pEventCB( pEVENTCB )
,m_uAddCounts(0)
,m_pTrackInfoBK_Audio(NULL)
,m_pTrackInfoBK_Subtitle(NULL)
,m_pTrackInfoBK_Video(NULL)
{
	for( VO_U32 i = 0;i<__ENDING_FRUUID; i++ )
		m_bUUID[i] = VO_FALSE;

}

voProgramInfoOp::~voProgramInfoOp()
{
	DestroyProgramInfo();
	m_listSelectedInfo.reset();
}
VO_U32 voProgramInfoOp::GetProgramInfo( VO_U32 uID, _PROGRAM_INFO **ppPragramInfo )
{	
	if(!ppPragramInfo){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+ GetProgramInfo");
	voCAutoLock lock( &m_lock );

	VO_U32 ret = VO_RET_SOURCE2_OK;

	ReleaseProgramInfoOP_T( m_pProgramInfoPlusBackUp_DS );
	m_pProgramInfoPlusBackUp_DS = NULL;
	if( m_pProgramInfoPlus )
	{
		CopyProgramInfoOP_T( m_pProgramInfoPlus, &m_pProgramInfoPlusBackUp_DS );
		*ppPragramInfo = m_pProgramInfoPlusBackUp_DS;

		PrintDSProgramInfo( *ppPragramInfo );
		ret = VO_RET_SOURCE2_OK;
	}
	else
		ret = VO_RET_SOURCE2_FAIL;

	SetProgramType();

	VOLOGI("- GetProgramInfo.Ret:0x%08x ", ret );
	return ret ;

}
VO_U32 voProgramInfoOp::GetSource2ProgramInfo(VO_U32 uID, VO_SOURCE2_PROGRAM_INFO **ppPragramInfo )
{
	if(!ppPragramInfo){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+ GetSource2ProgramInfo");
	voCAutoLock lock( &m_lock );

	VO_U32 ret = VO_RET_SOURCE2_OK;

	ReleaseProgramInfoOP_T( m_pProgramInfoPlusBackUp );
	m_pProgramInfoPlusBackUp = NULL;
	if( m_pProgramInfoPlus )
	{
 		CopyProgramInfoOP_EX( m_pProgramInfoPlus, &m_pProgramInfoPlusBackUp );
 		*ppPragramInfo = m_pProgramInfoPlusBackUp;
#ifdef XMLDUMPER	
		SourceInfoXMLDumper xmlDumper1("/", "", "pProgramInfo.xml", NULL );	
		xmlDumper1.DumpProgram( m_pProgramInfoPlus );
#endif
		PrintProgramInfo( *ppPragramInfo );
		ret = VO_RET_SOURCE2_OK;
	}
	else
		ret = VO_RET_SOURCE2_FAIL;

	SetProgramType();

	VOLOGI("- GetSource2ProgramInfo.Ret:0x%08x ", ret );
	return ret ;

}

VO_U32 voProgramInfoOp::GetCurSource2TrackInfo( VO_SOURCE2_TRACK_TYPE uType, VO_SOURCE2_TRACK_INFO **ppTrackInfo)
{
	if(!ppTrackInfo){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+ GetCurSource2TrackInfo. Type:%d", TrackType2String(uType) );
	voCAutoLock look(&m_lock);
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	
	if( !m_pProgramInfoPlusBackUp )
		return ret;

	VO_SOURCE2_TRACK_INFO **ppTmpTrackInfo = NULL;
	if( uType == VO_SOURCE2_TT_AUDIO ) ppTmpTrackInfo = &m_pTrackInfoBK_Audio;
	if( uType == VO_SOURCE2_TT_VIDEO ) ppTmpTrackInfo = &m_pTrackInfoBK_Video;
	if( uType == VO_SOURCE2_TT_SUBTITLE ) ppTmpTrackInfo = &m_pTrackInfoBK_Subtitle;

	VO_SOURCE2_STREAM_INFO *pStreamInfo = NULL;
	GetSelectedStreamOP_T(m_pProgramInfoPlusBackUp, &pStreamInfo );
	
	if(pStreamInfo)
	{
		VO_SOURCE2_TRACK_INFO *pTrackInfo = NULL;
		if( VO_RET_SOURCE2_OK == GetSelectedTrackOP_T( pStreamInfo, uType, &pTrackInfo ) )
		{
			CopyTrackInfoOP_T( pTrackInfo, ppTmpTrackInfo );
			*ppTrackInfo = *ppTmpTrackInfo;

			ret = VO_RET_SOURCE2_OK;
		}
	}
	VOLOGI("- GetCurSource2TrackInfo. Ret:0x%08x", ret);

	return ret;
}

VO_U32 voProgramInfoOp::ResetProgramInfo( _PROGRAM_INFO *pProgInfo )
{
	if(!pProgInfo){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+ ResetProgramInfo");
	voCAutoLock lock( &m_lock );
	DestroyProgramInfo();

	m_uHasSelInfo = 0;
	for( VO_U32 i = 0;i<__ENDING_FRUUID; i++ )
		m_bUUID[i] = VO_FALSE;


	CopyProgramInfoOP_T( pProgInfo, &m_pProgramInfo );
	ResetUUID( m_pProgramInfo );
	CopyProgramInfoOP_T( m_pProgramInfo, &m_pProgramInfoPlus  );
	SetProgramType(pProgInfo->sProgramType);
#ifdef WIN32
#ifdef XMLDUMPER
	SourceInfoXMLDumper xmlDumper("d:/", "", "oProgramInfo.xml", NULL );
	xmlDumper.DumpProgram( pProgInfo );
	SourceInfoXMLDumper xmlDumper1("d:/", "", "pProgramInfo.xml", NULL );
	xmlDumper1.DumpProgram( m_pProgramInfoPlus );
#endif
	VO_SOURCE2_PROGRAM_INFO  *pp = NULL;
	GetSource2ProgramInfo(0, &pp);
#ifdef XMLDUMPER
	SourceInfoXMLDumper xmlDumper2("d:/", "", "xProgramInfo.xml", NULL );
	xmlDumper2.DumpProgram( pp );
#endif
#endif
	
	//SendEvent( VO_DATASOURCE_EVENTID_POP_NEWPROGRAM );
	SendEvent(VO_DATASOURCE_EVENTID_POP_PROGRAMINFORESET );

	VOLOGI("- ResetProgramInfo");
	return VO_RET_SOURCE2_OK;
}


VO_VOID voProgramInfoOp::SetProgramType( VO_SOURCE2_PROGRAM_TYPE nType)
{
	VOLOGI("+SetProgramType:%d", nType);
//	voCAutoLock lock(&m_lock);
	m_nType = nType;
/*
	if( m_pProgramInfoPlus )
		m_pProgramInfoPlus->sProgramType = nType;
	if( m_pProgramInfoPlusBackUp )
		m_pProgramInfoPlusBackUp->sProgramType = nType;
	if( m_pProgramInfoPlusBackUp_DS )
		m_pProgramInfoPlusBackUp_DS->sProgramType = nType;
	if( m_pProgramInfo )
		m_pProgramInfo->sProgramType = nType;
*/	
	VOLOGI("-SetProgramType:%d", nType);
	
}

VO_U32 voProgramInfoOp::SelectStream( VO_U32 uStreamID, VO_U32 uFlag )
{
	VOLOGI("+SelectStream. StreamID:%d, uFlag=%lu", uStreamID, uFlag);

	VO_U32 ret = VO_RET_SOURCE2_OK;
	if( !m_pASParser ){
		VOLOGI("-SelectStream---m_pASParser is null.");
		return VO_RET_SOURCE2_FAIL;		
	}
	if(uFlag == 1)
	{
		ret = m_pASParser->SelectStream( uStreamID );
	}
	if( ret == VO_RET_SOURCE2_OK )
	{
		//move the lock here because m_pASParser->SelectStream may trigger the player to GetProgramInfo, this option will led to dead lock.
		//So I move this lock from Top to here.
		voCAutoLock lock(&m_lock);

		ret =  SelfSelStream( uStreamID );

		if( ret == VO_RET_SOURCE2_OK)
		{
			vo_singlelink_list< VODS_SELECTED_INFO >::iterator iter = m_listSelectedInfo.begin();
			vo_singlelink_list< VODS_SELECTED_INFO >::iterator itere = m_listSelectedInfo.end();
			while( iter != itere )
			{
				VOLOGI("To Select FileParser Track. Type:%d,ASID:%d,FPID:%d,UUID:%d"
					,iter->nType, iter->uASTrackID, iter->uFPTrackID, iter->uUUID);
				VO_U32 ret1 = m_pASParser->SelectTrack( iter->uASTrackID, iter->nType );
				iter ++;
			}
		}



	}
	VOLOGI("-SelectStream. StreamID:%d.Ret:0x%08x ", uStreamID,ret );

	return ret;
}

VO_U32 voProgramInfoOp::SelectTrack( VO_U32 uOutSideTrackID )
{
	VOLOGI("+SelectTrack. TrackID:%d.%p ", uOutSideTrackID, m_pASParser );
	voCAutoLock lock( &m_lock );
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	if( m_pASParser )
	{
		VOLOGI("SelectTrack UUID:%d", uOutSideTrackID );


		_TRACK_INFO *pTrackInfo = NULL;
		GetTrackInfoByIDOP_T( m_pProgramInfoPlus, uOutSideTrackID, &pTrackInfo);
		if( pTrackInfo )
		{
			ret = m_pASParser->SelectTrack( pTrackInfo->uASTrackID, (VO_SOURCE2_TRACK_TYPE)pTrackInfo->uTrackType );
			if( ret ==  VO_RET_SOURCE2_OK )
			{
				if( (pTrackInfo->uTrackType == VO_SOURCE2_TT_AUDIO || pTrackInfo->uTrackType == VO_SOURCE2_TT_AUDIOGROUP) ){			
					ret = SelfSelTrack((VO_DATASOURCE_TRACK_TYPE)pTrackInfo->uTrackType, pTrackInfo->sAudioInfo.chLanguage);
				}
				if( (pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE || pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLEGROUP) ){
					ret = SelfSelTrack((VO_DATASOURCE_TRACK_TYPE)pTrackInfo->uTrackType, pTrackInfo->sSubtitleInfo.chLanguage);
				}
			}
			else
				VODS_VOLOGW("ASParser selectTrack error.0x%08x", ret);
		}
	}

	VOLOGI("-SelectTrack. TrackID:%d.Ret:0x%08x ", uOutSideTrackID, ret );

	return ret;
}


VO_U32 voProgramInfoOp::GetSelectedStreamID( VO_U32 *pStreamID )
{
	if(!pStreamID){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+GetSelectedStreamID. pStreamID:%lu", pStreamID );
	voCAutoLock lock( &m_lock );

	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	_STREAM_INFO *pSelectedStreamInfo = NULL;
	GetSelectedStreamOP_T( m_pProgramInfoPlus, &pSelectedStreamInfo );

	if( !pSelectedStreamInfo )
		return ret;

	*pStreamID = pSelectedStreamInfo->uStreamID;

	VOLOGI("-GetSelectedStreamID. pStreamID:%lu", pStreamID );
	
	return VO_RET_SOURCE2_OK;
}
VO_U32 voProgramInfoOp::ConstructProgramInfo(VO_U32 uOPFlag, VO_U32 uOPID, VO_U32 uProgramID, VO_U64 uMarkedID, _TRACK_INFO *pSourceTrackInfo, VO_U32 uFlag )
{
	if(!pSourceTrackInfo){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+ConstructProgramInfo---uOPFlag=%lu,uOPID=%lu,uProgramID=%lu,uMarkedID=%lu,uFlag=%lu",uOPFlag, uOPID, uProgramID, uMarkedID, uFlag);

	VO_U32 ret = VO_RET_SOURCE2_FAIL;

	_PROGRAM_INFO *pProgramInfo = m_pProgramInfoPlus;

	if( uOPFlag == VO_DATASOURCE_MARKFLAG_RESET )
	{
		VOLOGI("VO_DATASOURCE_MARKFLAG_RESET, uASTrackID:%d", pSourceTrackInfo->uASTrackID);
		voCAutoLock lock( &m_lock );

		//SetUUID( pTrackInfoPlus->uOutSideTrackID );
		RemoveTrackBySameASID( pProgramInfo, uMarkedID, pSourceTrackInfo->uASTrackID );
		//SendEvent( VO_DATASOURCE_EVENTID_POP_NEWPROGRAM);

	}
	else 
	if ( uOPFlag == VO_DATASOURCE_MARKFLAG_ADD )
	{
		VOLOGI("VO_DATASOURCE_MARKFLAG_ADD. OPID:%d", uOPID);

 		switch(uOPID)
 		{
 		case VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADDBEGIN:
 			{
				m_addlock.Lock();
				m_uAddCounts = 0;
 			}
 			break;
 		case VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADD:
 			{
				voCAutoLock lock( &m_lock );
				ret = AddTrack( uProgramID, pProgramInfo, pSourceTrackInfo );
 			}
 			break;
 		case VO_DATASOURCE_MARKOP_CONSTRUCTPROGRAMINFO_ADDEND:
 			{
				voCAutoLock lock( &m_lock );
				
				SortTrackInfo( m_pProgramInfoPlus );
				_STREAM_INFO *pSelectedStreamInfo = NULL;
				GetSelectedStreamOP_T( m_pProgramInfoPlus, &pSelectedStreamInfo);

				if( pSelectedStreamInfo )
				{
					VOLOGI("Add Track Complete. [%d]tracks have added in.", m_uAddCounts );
					//it means at least one track add succeed.
					if(m_uAddCounts > 0)
						ret = SelectStream( pSelectedStreamInfo->uStreamID, 0 );
					else
						ret = SelfSelStream(pSelectedStreamInfo->uStreamID);
				}
				
				m_addlock.Unlock();
 			}
 			break;
		}
	}
	VOLOGI("-ConstructProgramInfo---Ret:0x%08x",ret);
	return ret;
}

VO_U32 voProgramInfoOp::GetCurTrackInfo( VO_SOURCE2_TRACK_TYPE uType, _TRACK_INFO **pTrackInfo )
{
	if(!pTrackInfo){
		VOLOGE("GetCurTrackInfo return VO_RET_SOURCE2_EMPTYPOINTOR!");
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+GetCurTrackInfo---uType=%lu",uType);
	voCAutoLock lock( &m_lock );
	
	VO_U32 ret = VO_RET_SOURCE2_FAIL;

	_STREAM_INFO *pSelectedStreamInfo = NULL;
	GetSelectedStreamOP_T( m_pProgramInfoPlus, &pSelectedStreamInfo );
	if( !pSelectedStreamInfo ){
		VOLOGI("-GetCurTrackInfo---Ret:0x%08x",VO_RET_SOURCE2_NOIMPLEMENT);
		return VO_RET_SOURCE2_NOIMPLEMENT;		
	}

	VO_U32 uUUID = _VODS_INT32_MAX;

	{
//		voCAutoLock lock(&m_SelInfolock);

		vo_singlelink_list< VODS_SELECTED_INFO >::iterator iter = m_listSelectedInfo.begin();
		vo_singlelink_list< VODS_SELECTED_INFO >::iterator itere = m_listSelectedInfo.end();
		while( iter != itere )
		{
			if( uType == (*iter).nType )
			{
				uUUID = (*iter).uUUID;
				break;
			}
			iter ++;
		}
	}

	VO_DATASOURCE_TRACK_INFO *pInfo = GetTrackInfoByIDOP_T( pSelectedStreamInfo, uUUID);
	if( pInfo )
	{
		*pTrackInfo = pInfo;
		VOLOGR("GetCurTrackInfo. ASTrackID:%d, FPTrackID:%d, UUID:%d", pInfo->uASTrackID, pInfo->uFPTrackID, pInfo->uOutSideTrackID );
		ret = VO_RET_SOURCE2_OK;
	}

	VOLOGI("-GetCurTrackInfo---Ret:0x%08x",ret);
	
	return ret;
}

VO_U32 voProgramInfoOp::GetSelectedTrackID(vo_singlelink_list<VODS_SELECTED_INFO> *pListSelInfo )
{
	if(!pListSelInfo){
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	VOLOGI("+GetSelectedTrackID");
//	voCAutoLock lock( &m_SelInfolock );
	voCAutoLock lock( &m_lock );

	vo_singlelink_list< VODS_SELECTED_INFO >::iterator iter = m_listSelectedInfo.begin();
	vo_singlelink_list< VODS_SELECTED_INFO >::iterator itere = m_listSelectedInfo.end();
	while( iter != itere )
	{
		pListSelInfo->push_back( (*iter) );
		VOLOGI("SelectedTrack. ASID:%d, FP:%d", iter->uASTrackID,iter->uFPTrackID);
		iter ++;
	}
	m_bTrackIDDirty = VO_FALSE;
	
	VOLOGI("-GetSelectedTrackID");

	return VO_RET_SOURCE2_OK;
}

/****************************The followed function is used by internal*********************************************************/
VO_U32 voProgramInfoOp::DestroyProgramInfo( )
{
//	voCAutoLock lock(&m_lock);
	VOLOGI("DestroyProgramInfo");

	ReleaseProgramInfoOP_T( m_pProgramInfo );
	m_pProgramInfo = NULL;
	VOLOGR("1");
	ReleaseProgramInfoOP_T( m_pProgramInfoPlus );
	m_pProgramInfoPlus = NULL;
	VOLOGR("2");

	ReleaseProgramInfoOP_T( m_pProgramInfoPlusBackUp );
	m_pProgramInfoPlusBackUp = NULL;
	VOLOGR("3");

	ReleaseProgramInfoOP_T( m_pProgramInfoPlusBackUp_DS );
	m_pProgramInfoPlusBackUp_DS = NULL;
	VOLOGR("4");
	
	ReleaseTrackInfoOP_T(m_pTrackInfoBK_Video);
	m_pTrackInfoBK_Video = NULL;
	ReleaseTrackInfoOP_T(m_pTrackInfoBK_Audio);
	m_pTrackInfoBK_Audio = NULL;
	ReleaseTrackInfoOP_T(m_pTrackInfoBK_Subtitle);
	m_pTrackInfoBK_Subtitle = NULL;
	return VO_RET_SOURCE2_OK;
}


VO_U32 voProgramInfoOp::SelfSelStream( VO_U32 uStreamID )
{
//	voCAutoLock lock( &m_lock );

	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	if( !m_pProgramInfoPlus )
		return ret;
	_STREAM_INFO *pOldStreamInfo = NULL;
	GetSelectedStreamOP_T( m_pProgramInfoPlus, &pOldStreamInfo );
	_STREAM_INFO *pSelectedStreamInfo = NULL;

	pSelectedStreamInfo = GetStreamInfoByIDOP_T( m_pProgramInfoPlus, uStreamID);

	if( pSelectedStreamInfo )
	{	
		vo_singlelink_list<VO_U32> lSeledTUUID;
		ret = m_SourceSelOp.SelectStream( pOldStreamInfo, pSelectedStreamInfo, &lSeledTUUID );
		if( ret == VO_RET_SOURCE2_OK )
		{
			ResetSelInfo( &m_pProgramInfoPlus, uStreamID, lSeledTUUID );
		}
	}
	return ret;
}

VO_U32 voProgramInfoOp::SelfSelTrack( VO_U32 uOutSideTrackID )
{
	VOLOGI("+SelfSelTrack:%d", uOutSideTrackID);
//	voCAutoLock lock( &m_lock );
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	_STREAM_INFO *pSelectedStreamInfo = NULL;
	GetSelectedStreamOP_T( m_pProgramInfoPlus, &pSelectedStreamInfo );

	if( !pSelectedStreamInfo )
		return ret;
	vo_singlelink_list<VO_U32> lSeledTUUID;
#if 1
	_TRACK_INFO *pTrackInfo = NULL;
	GetTrackInfoByIDOP_T(m_pProgramInfoPlus, uOutSideTrackID, &pTrackInfo );
	if(pTrackInfo){
		if(pTrackInfo->uTrackType == VO_SOURCE2_TT_AUDIO || pTrackInfo->uTrackType == VO_SOURCE2_TT_AUDIOGROUP){
			ret = m_SourceSelOp.SelectTrack( pSelectedStreamInfo, (VO_DATASOURCE_TRACK_TYPE)pTrackInfo->uTrackType, pTrackInfo->sAudioInfo.chLanguage, &lSeledTUUID);

		}else if(pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE || pTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLEGROUP){
			ret = m_SourceSelOp.SelectTrack( pSelectedStreamInfo, (VO_DATASOURCE_TRACK_TYPE)pTrackInfo->uTrackType, pTrackInfo->sSubtitleInfo.chLanguage, &lSeledTUUID);
		}
	}
#else
	ret = m_SourceSelOp.SelectTrack( pSelectedStreamInfo, uOutSideTrackID, &lSeledTUUID);
#endif
	if( ret == VO_RET_SOURCE2_OK )
	{
		ResetSelInfo( &m_pProgramInfoPlus, pSelectedStreamInfo->uStreamID, lSeledTUUID );
	}
	VOLOGI("-SelfSelTrack:%d.Ret:0x%08x", uOutSideTrackID,ret);

	return ret;
}

VO_U32 voProgramInfoOp::SelfSelTrack(VO_DATASOURCE_TRACK_TYPE type, VO_CHAR* pLanguage)
{
//	voCAutoLock lock( &m_lock );
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	_STREAM_INFO *pSelectedStreamInfo = NULL;

	if(!pLanguage || !strlen(pLanguage)){
		return ret;
	}
	VOLOGI("+SelfSelTrack:%s", pLanguage);
	
	GetSelectedStreamOP_T( m_pProgramInfoPlus, &pSelectedStreamInfo );

	if( !pSelectedStreamInfo )
		return ret;
	
	vo_singlelink_list<VO_U32> lSeledTUUID;
	ret = m_SourceSelOp.SelectTrack( pSelectedStreamInfo, type, pLanguage, &lSeledTUUID);
	
	if( ret == VO_RET_SOURCE2_OK )
	{
		ResetSelInfo( &m_pProgramInfoPlus, pSelectedStreamInfo->uStreamID, lSeledTUUID );
	}
	VOLOGI("-SelfSelTrack:%s.Ret:0x%08x", pLanguage,ret);

	return ret;
}


VO_BOOL voProgramInfoOp::CheckIsGroup(VO_U32 uProgramID, _PROGRAM_INFO *pProgramInfoPlus, VO_U32 uStreamID, _TRACK_INFO *pSourceTrackInfo)
{
	VO_BOOL bGroup = VO_FALSE;

	_STREAM_INFO * pStreamInfo = GetStreamInfoByIDOP_T( m_pProgramInfo, uStreamID );

	//check audio-group, video-group, subtitle-group first
	VO_SOURCE2_TRACK_TYPE nType = VO_SOURCE2_TT_MAX;
	switch( pSourceTrackInfo->uTrackType)
	{
	case VO_SOURCE2_TT_AUDIO:
		nType = VO_SOURCE2_TT_AUDIOGROUP;
		break;
	case VO_SOURCE2_TT_VIDEO:
		nType = VO_SOURCE2_TT_VIDEOGROUP;
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		nType = VO_SOURCE2_TT_SUBTITLEGROUP;
		break;
	}
	_TRACK_INFO * pTrackInfo = GetTrackInfoByASIDandTypeOP_T( pStreamInfo, pSourceTrackInfo->uASTrackID, nType );
	//if it is not video/audio/subtitle group, then check mux-group
	if( !pTrackInfo )
	{
		pTrackInfo = GetTrackInfoByASIDandTypeOP_T( pStreamInfo, pSourceTrackInfo->uASTrackID, VO_SOURCE2_TT_MUXGROUP );
	}
	VO_U32 isGroup = 0;
	if( pTrackInfo )
	{
		__CHECK_GROUP( pTrackInfo->uTrackType, isGroup );
	}
	
	if( isGroup == 1 )
	{		
		bGroup = VO_TRUE;

		
 		if( !( m_uHasSelInfo & _AUDIO_SELECTED ) || !( m_uHasSelInfo & _VIDEO_SELECTED ) || !( m_uHasSelInfo & _SUBTITLE_SELECTED ) )
 			pSourceTrackInfo->uSelInfo = pTrackInfo->uSelInfo;
		

		switch( pSourceTrackInfo->uTrackType )
		{
		case VO_SOURCE2_TT_AUDIO: 
			{
 				if(pTrackInfo->uTrackType != VO_SOURCE2_TT_MUXGROUP)
 					memcpy( &pSourceTrackInfo->sAudioInfo, &pTrackInfo->sAudioInfo, sizeof( VO_SOURCE2_AUDIO_INFO ) );
 				m_uHasSelInfo |= _AUDIO_SELECTED;
			}
			break;
		case VO_SOURCE2_TT_VIDEO:
			{
				if(pTrackInfo->uTrackType != VO_SOURCE2_TT_MUXGROUP)
					memcpy( &pSourceTrackInfo->sVideoInfo, &pTrackInfo->sVideoInfo, sizeof( VO_SOURCE2_VIDEO_INFO ) );
				m_uHasSelInfo |= _VIDEO_SELECTED;
			}
			break;
		case VO_SOURCE2_TT_SUBTITLE:
			{
				if(pTrackInfo->uTrackType != VO_SOURCE2_TT_MUXGROUP)
					memcpy( &pSourceTrackInfo->sSubtitleInfo, &pTrackInfo->sSubtitleInfo, sizeof( VO_SOURCE2_SUBTITLE_INFO ) );
				m_uHasSelInfo |= _SUBTITLE_SELECTED;
			}
			break;
		}

		RemoveTrack(pProgramInfoPlus, uStreamID,pSourceTrackInfo->uASTrackID, VO_SOURCE2_TT_MUXGROUP );
		RemoveTrack(pProgramInfoPlus, uStreamID,pSourceTrackInfo->uASTrackID, nType );


	}

	return bGroup;
}
VO_VOID voProgramInfoOp::RemoveTrack(_PROGRAM_INFO *pProgramInfoPlus, VO_U32 uStreamID, VO_U32 uASID, VO_SOURCE2_TRACK_TYPE type)
{
	_STREAM_INFO * pStreamInfoPlus = GetStreamInfoByIDOP_T( pProgramInfoPlus, uStreamID );
	_TRACK_INFO * pTrackInfoPlus = GetTrackInfoByASIDandTypeOP_T( pStreamInfoPlus, uASID,type );

	VO_U32 isGroup1 = 0;
	if( pTrackInfoPlus )
	{
		__CHECK_GROUP( pTrackInfoPlus->uTrackType, isGroup1 );
	}
	if( isGroup1 )
	{
		//save SelInfo when it has been selected.
// 		if( pTrackInfoPlus->uSelInfo & VO_SOURCE2_SELECT_SELECTED)
// 			pSourceTrackInfo->uSelInfo |= VO_SOURCE2_SELECT_SELECTED;

		SetUUID( pTrackInfoPlus->uOutSideTrackID );
		RemoveTrackFromStreamOP_T( pStreamInfoPlus, pTrackInfoPlus );
		pTrackInfoPlus = NULL;
	}
}

VO_U32 voProgramInfoOp::AddTrack(VO_U32 uProgramID, _PROGRAM_INFO *pProgramInfoPlus, _TRACK_INFO *pSourceTrackInfo )
{

//	voCAutoLock lock( &m_lock );

	VO_U32 ret = VO_RET_SOURCE2_OK;
	
	VO_BOOL bAdd = VO_FALSE;

	if( !pProgramInfoPlus )
		return VO_RET_SOURCE2_FAIL;
	if( pSourceTrackInfo->uTrackType == VO_SOURCE2_TT_AUDIO && strlen( pSourceTrackInfo->sAudioInfo.chLanguage ) == 0 )
		sprintf( pSourceTrackInfo->sAudioInfo.chLanguage, "A%d", (int)pSourceTrackInfo->uFPTrackID);
	if( pSourceTrackInfo->uTrackType == VO_SOURCE2_TT_SUBTITLE && strlen( pSourceTrackInfo->sSubtitleInfo.chLanguage ) == 0 )
		sprintf( pSourceTrackInfo->sSubtitleInfo.chLanguage, "S%d", (int)pSourceTrackInfo->uFPTrackID);


	pSourceTrackInfo->uOutSideTrackID = GetUUID( __BEGIN_FPUUID);
	/*To check all tracks' ID in ProgramInfo.If ASTrackID is the same, replace it.*/
	VOLOGI("+ AddTrack. FPTrackID:%d, ASTrackID:%d, UUID:%d, TrackType:%d "
		, pSourceTrackInfo->uFPTrackID, pSourceTrackInfo->uASTrackID, pSourceTrackInfo->uOutSideTrackID, pSourceTrackInfo->uTrackType);
	for( VO_U32 i = 0; i < pProgramInfoPlus->uStreamCount; i++ )
	{
		ret = AddTrack( uProgramID, pProgramInfoPlus,  pProgramInfoPlus->ppStreamInfo[i]->uStreamID, pSourceTrackInfo );
		if( ret == VO_RET_SOURCE2_OK )
		{
			m_uAddCounts ++;
			bAdd = VO_TRUE;
		}
	}
	if( bAdd )
		ret = VO_RET_SOURCE2_OK;
	else
	{
		SetUUID(pSourceTrackInfo->uOutSideTrackID);
		ret = VO_RET_SOURCE2_FAIL;
	}

// 	_STREAM_INFO *pStreamInfo = NULL;
// 	GetSelectedStreamOP_T(pProgramInfoPlus, &pStreamInfo );
// 	vo_singlelink_list<VO_U32> list;
// 	vo_singlelink_list< VODS_SELECTED_INFO  >::iterator iter = m_listSelectedInfo.begin();
// 	vo_singlelink_list< VODS_SELECTED_INFO  >::iterator itere = m_listSelectedInfo.end();
// 	_STREAM_INFO pTrackInfo = NULL;
// 	while( iter != itere )
// 	{
// 		if( iter->nType == VO_SOURCE2_TT_AUDIOGROUP )
// 			SelectTrackBy2ID_T(pStreamInfo, iter->uASTrackID, iter->uFPTrackID, &pTrackInfo );
// 		iter ++;
// 	}
//	ResetSelInfo( &pProgramInfoPlus,  pStreamInfo->uStreamID, m_listSelectedInfo );

	VOLOGI("- AddTrack. Count:%d ", m_uAddCounts );

	return ret;

}

VO_U32 voProgramInfoOp::AddTrack(VO_U32 uProgramID, _PROGRAM_INFO *pProgramInfoPlus, VO_U32 uStreamID, _TRACK_INFO *pSourceTrackInfo )
{

	_STREAM_INFO * pStreamInfoPlus = GetStreamInfoByIDOP_T( pProgramInfoPlus, uStreamID );
	if( !pStreamInfoPlus )
		return VO_RET_SOURCE2_FAIL;

	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	VO_BOOL bGroup = CheckIsGroup( uProgramID, pProgramInfoPlus, uStreamID, pSourceTrackInfo );
	
	ret = VO_RET_SOURCE2_FAIL;
	if( bGroup )
	{

		ret = InsertTrackInfoOP2_T( pStreamInfoPlus, pSourceTrackInfo );
		VOLOGI("InsertTrack. ASID:%d, FPID:%d, UUID:%d, Type:%d"
			, pSourceTrackInfo->uASTrackID, pSourceTrackInfo->uFPTrackID, pSourceTrackInfo->uOutSideTrackID, pSourceTrackInfo->uTrackType);
	}
	else
	{
		_TRACK_INFO *pTrackInfo = GetTrackInfoByASIDandTypeOP_T(pStreamInfoPlus, pSourceTrackInfo->uASTrackID, (VO_SOURCE2_TRACK_TYPE)pSourceTrackInfo->uTrackType );
		if( pTrackInfo )
		{ 	
			//update FPTrackID
			pTrackInfo->uFPTrackID = pSourceTrackInfo->uFPTrackID;
		}
	}

	return ret;
}

VO_VOID voProgramInfoOp::ResetUUID( _PROGRAM_INFO *pProgInfo )
{	
	if( !pProgInfo )
		return ;

	for( VO_U32 i = 0; i < pProgInfo->uStreamCount; i++ )
	{
		for ( VO_U32 n = 0; n < pProgInfo->ppStreamInfo[i]->uTrackCount; n++ )
		{
			pProgInfo->ppStreamInfo[i]->ppTrackInfo[n]->uOutSideTrackID = GetUUID( __BEGIN_ASUUID );//VO_U32( m_uUUIDforAS ++ );
			//at first, set FPTrackID to a biggest value
			pProgInfo->ppStreamInfo[i]->ppTrackInfo[n]->uFPTrackID = _VODS_INT32_MAX;
		}
	}
}
VO_VOID voProgramInfoOp::MeregeSelInfoBy2ID( _PROGRAM_INFO *pProgInfo, VO_U32 uSelStreamID, vo_singlelink_list< VODS_SELECTED_INFO > &list)
{
//	voCAutoLock lock(&m_SelInfolock);

	if( !pProgInfo )
		return;

	ResetProgramSelInfo_T( pProgInfo );
	
	_STREAM_INFO *pStreamInfo = NULL;
	_TRACK_INFO *pTrackInfo = NULL;

	VO_U32 ret = SelectStreamOP_T( pProgInfo, uSelStreamID, &pStreamInfo );
	
	vo_singlelink_list< VODS_SELECTED_INFO >::iterator iter = list.begin();
	vo_singlelink_list< VODS_SELECTED_INFO >::iterator itere = list.end();
	while( iter != itere )
	{
		SelectTrackBy2ID_T(pStreamInfo, iter->uASTrackID, iter->uFPTrackID, &pTrackInfo );
		iter ++;
	}
}
VO_VOID voProgramInfoOp::ResetSelInfo( _PROGRAM_INFO **ppProgInfo, VO_U32 uSelStreamID, vo_singlelink_list<VO_U32>& list )
{
//	voCAutoLock lock(&m_SelInfolock);

	if( !(*ppProgInfo) )
		return;
// 	_PROGRAM_INFO *pProg = NULL;
// 	CopyProgramInfoOP_T( *ppProgInfo, &pProg );
// 	*ppProgInfo = pProg;
	
	ResetProgramSelInfo_T( *ppProgInfo );
	m_listSelectedInfo.reset();

	_STREAM_INFO *pStreamInfo = NULL;
	_TRACK_INFO *pTrackInfo = NULL;

	VO_U32 ret = SelectStreamOP_T( *ppProgInfo, uSelStreamID, &pStreamInfo );
	if( pStreamInfo )
	{
		vo_singlelink_list< VO_U32 >::iterator iter = list.begin();
		vo_singlelink_list< VO_U32 >::iterator itere = list.end();
		while( iter != itere )
		{
			SelectTrackOP_T(pStreamInfo, (*iter), &pTrackInfo );
			if( pTrackInfo )
			{
				/*merge selected track info to list*/
				VODS_SELECTED_INFO info;
				info.nType = ( VO_SOURCE2_TRACK_TYPE )pTrackInfo->uTrackType;
				info.uFPTrackID = pTrackInfo->uFPTrackID;
				info.uASTrackID = pTrackInfo->uASTrackID;
				info.uUUID = pTrackInfo->uOutSideTrackID;
				m_listSelectedInfo.push_back( info );
				if( info.nType == VO_SOURCE2_TT_AUDIO || info.nType == VO_SOURCE2_TT_AUDIOGROUP)
				{
					VOLOGI("Stream = %d Selected. (TrackType: %d, FPTrackID: %d, ASTrackID: %d, uUUID: %d, Lang:%s )",uSelStreamID, info.nType, info.uFPTrackID, info.uASTrackID, info.uUUID, pTrackInfo->sAudioInfo.chLanguage );
				}
				else if( info.nType == VO_SOURCE2_TT_SUBTITLE || info.nType == VO_SOURCE2_TT_SUBTITLEGROUP)
				{
					VOLOGI("Stream = %d Selected. (TrackType: %d, FPTrackID: %d, ASTrackID: %d, uUUID: %d, Lang:%s )",uSelStreamID, info.nType, info.uFPTrackID, info.uASTrackID, info.uUUID, pTrackInfo->sSubtitleInfo.chLanguage );
				}
				else
				{
					VOLOGI("Stream = %d Selected. (TrackType: %d, FPTrackID: %d, ASTrackID: %d, uUUID: %d )",uSelStreamID, info.nType, info.uFPTrackID, info.uASTrackID, info.uUUID );
				}
			}
			
			iter ++;
		}
		m_bTrackIDDirty = VO_TRUE;
		
		//SendEvent( VO_DATASOURCE_EVENTID_POP_NEWPROGRAM );
		SendEvent( VO_DATASOURCE_EVENTID_POP_PROGRAMINFOCHANGED );
	}

#ifdef WIN32
#ifdef XMLDUMPER
	SourceInfoXMLDumper xmlDumper("d:/", "", "newProgramInfo.xml", NULL );
	xmlDumper.DumpProgram( m_pProgramInfoPlus );
#endif
#endif
}

VO_BOOL voProgramInfoOp::TrackIDIsDirty()
{
	return m_bTrackIDDirty;
}

VO_U32 voProgramInfoOp::SendEvent(VO_U32 uID )
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;

	VOLOGI("+ SendEvent:0x%08x. Ret:0x%08x", uID, ret);

	if( m_pEventCB )
	{
		switch( uID )
		{
		case VO_DATASOURCE_EVENTID_POP_PROGRAMINFORESET:
		case VO_DATASOURCE_EVENTID_POP_PROGRAMINFOCHANGED:
			ret = m_pEventCB->SendEvent(m_pEventCB->pUserData, uID, (VO_U32)m_pProgramInfoPlus, 0 );
			break;
		case VO_DATASOURCE_EVENTID_POP_NEWPROGRAM:
			ret = m_pEventCB->SendEvent(m_pEventCB->pUserData, uID, (VO_U32)m_pProgramInfoPlus, 0 );
			break;
		default:
			break;
		}
	}
	VOLOGR(" ProgramInfoPlus_OT:%p", m_pProgramInfoPlus);
	VOLOGI("- SendEvent:0x%08x. Ret:0x%08x", uID, ret);

	return ret;
}


VO_VOID voProgramInfoOp::RemoveTrackBySameASID( _PROGRAM_INFO *pProgramInfo, VO_U64 uMarkedID, VO_U32 uASID )
{
//	voCAutoLock lock(&m_lock);
	for( VO_U32 i = 0; i< pProgramInfo->uStreamCount; i ++)
	{
		_STREAM_INFO *pSampleInfo = pProgramInfo->ppStreamInfo[i];
		_TRACK_INFO *pTrackInfo = NULL;	
		VO_BOOL bDelete = VO_FALSE;

		//First, remove the track whose uASID are same.
		do
		{
			if( pTrackInfo )
			{
				SetUUID( pTrackInfo->uOutSideTrackID );
				RemoveTrackFromStreamOP_T(pSampleInfo, pTrackInfo );
				pTrackInfo = NULL;
				bDelete = VO_TRUE;
			}
			pTrackInfo = GetTrackInfoByASIDOP_T(pProgramInfo->ppStreamInfo[i], uASID);
		}while(pTrackInfo);

		//Second, if remove the track successfully, add the original track.
		if(bDelete)	
		{
			_TRACK_INFO *ppTrackInfo[10] = {0};//is an array 10 pointers to _TRACK_INFO
			GetTrackInfoByASIDOP2_T(m_pProgramInfo->ppStreamInfo[i], uASID, ppTrackInfo);
			_TRACK_INFO **pTI = ppTrackInfo;
			while( *pTI )
			{
				VOLOGI("Rollback Track.StreamID:%d, ASID:%d, FPID:%d, TrackType:%d",pSampleInfo->uStreamID, (*pTI)->uASTrackID, (*pTI)->uFPTrackID, (*pTI)->uTrackType);
				InsertTrackInfoOP3_T( pSampleInfo, (*pTI));
				pTI ++;
			}
		}

	//	RemoveTrackFromStreamByASIDOP_T( pProgramInfo->ppStreamInfo[i], uASID );
	}
}

VO_U32 voProgramInfoOp::GetUUID( VO_U32 uStartNumber )
{
	for( VO_U32 i = uStartNumber;i < __ENDING_FRUUID; i++)
	{
		if( m_bUUID[i] == VO_FALSE)
		{
			m_bUUID[i] = VO_TRUE;
			return i;
		}
	}
	return 0;
}

VO_VOID voProgramInfoOp::SetUUID( VO_U32 uID)
{
	if( uID < __ENDING_FRUUID )
		m_bUUID[uID] = VO_FALSE;
}


VO_VOID voProgramInfoOp::SortTrackInfo( _PROGRAM_INFO *pProgramInfo )
{
//	voCAutoLock lock(&m_lock);
	//bubble sort algorithm. ASTrackID samll first, FPID samll first
	for(VO_U32 i = 0; i< pProgramInfo->uStreamCount; i++)
	{
		_STREAM_INFO *pStreamInfo = pProgramInfo->ppStreamInfo[i];
		VO_U32 len = pStreamInfo->uTrackCount;
		for (VO_U32 j = 0; j < len; j++) 
		{ 
			for (VO_U32 n = len - 1; n > j; n--) 
			{ 
				if (pStreamInfo->ppTrackInfo[j]->uASTrackID > pStreamInfo->ppTrackInfo[n]->uASTrackID) 
				{ 
					_TRACK_INFO *pTemp = pStreamInfo->ppTrackInfo[j]; 
					pStreamInfo->ppTrackInfo[j] = pStreamInfo->ppTrackInfo[n]; 
					pStreamInfo->ppTrackInfo[n] = pTemp; 
				} 
				else if( pStreamInfo->ppTrackInfo[j]->uASTrackID == pStreamInfo->ppTrackInfo[n]->uASTrackID)
				{
					if (pStreamInfo->ppTrackInfo[j]->uFPTrackID > pStreamInfo->ppTrackInfo[n]->uFPTrackID) 
					{ 
						_TRACK_INFO *pTemp = pStreamInfo->ppTrackInfo[j]; 
						pStreamInfo->ppTrackInfo[j] = pStreamInfo->ppTrackInfo[n]; 
						pStreamInfo->ppTrackInfo[n] = pTemp; 
					} 
				}
			} 
		}
	}
#ifdef WIN32
#ifdef XMLDUMPER
	SourceInfoXMLDumper xmlDumper("d:/", "", "StorProgramInfo.xml", NULL );
	xmlDumper.DumpProgram( m_pProgramInfoPlus );
#endif
#endif

}


VO_VOID voProgramInfoOp::SetLanguage(VO_SOURCE2_TRACK_TYPE uType, VO_CHAR *pLanguage)
{
}

VO_VOID voProgramInfoOp::SetProgramType()
{
	VOLOGI("SetProgramType:---%d---internal", m_nType);
	if( m_pProgramInfoPlus )
		m_pProgramInfoPlus->sProgramType = m_nType;
	if( m_pProgramInfoPlusBackUp )
		m_pProgramInfoPlusBackUp->sProgramType = m_nType;
	if( m_pProgramInfoPlusBackUp_DS )
		m_pProgramInfoPlusBackUp_DS->sProgramType = m_nType;
	if( m_pProgramInfo )
		m_pProgramInfo->sProgramType = m_nType;
	
}

