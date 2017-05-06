
#include "vo_tracksample_filter.h"
#include "voDSType.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_tracksample_filter::vo_tracksample_filter( voProgramInfoOp * ptr_programinfo_op )
:m_ptr_programinfo_op(ptr_programinfo_op)
{
	for( VO_S32 i = 0 ; i < 255 ; i++ )
	{
		m_trackselectinfo[i].status = TRACKSTATUS_INIT;
		m_trackselectinfo[i].cur_selected_trackid = 0xffffffff;
		m_trackselectinfo[i].pre_selected_trackid = 0xffffffff;
	}
}

vo_tracksample_filter::~vo_tracksample_filter(void)
{
}

VO_U32 vo_tracksample_filter::check_sample( VO_U32 type , VO_DATASOURCE_SAMPLE * ptr_sample )
{
	if( m_ptr_programinfo_op->TrackIDIsDirty() )
	{
		fill_selectinfo();
	}

	if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE )
		return VO_RET_SOURCE2_OK;

	if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_EOS )
		return VO_RET_SOURCE2_OK;

	if( m_trackselectinfo[type].status == TRACKSTATUS_INIT )
		return VO_RET_SOURCE2_TRACKNOTFOUND;

	if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_BASTART )
		m_trackselectinfo[type].status = TRACKSTATUS_BA;

	if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_BAEND )
		m_trackselectinfo[type].status = TRACKSTATUS_NORMAL;

	if( m_trackselectinfo[type].status == TRACKSTATUS_BA && 
		( ( m_trackselectinfo[type].cur_selected_trackid == ptr_sample->uFPTrackID && m_trackselectinfo[type].cur_selected_astrackid == ptr_sample->uASTrackID)
		|| (type != VO_SOURCE2_TT_AUDIO &&m_trackselectinfo[type].pre_selected_trackid == ptr_sample->uFPTrackID && m_trackselectinfo[type].pre_selected_astrackid == ptr_sample->uASTrackID ) ) )
		return VO_RET_SOURCE2_OK;

	if( m_trackselectinfo[type].status == TRACKSTATUS_NORMAL && 
		m_trackselectinfo[type].cur_selected_trackid == ptr_sample->uFPTrackID && m_trackselectinfo[type].cur_selected_astrackid == ptr_sample->uASTrackID)
		return VO_RET_SOURCE2_OK;
	else if( m_trackselectinfo[type].status == TRACKSTATUS_NORMAL &&
		m_trackselectinfo[type].pre_selected_trackid != 0xffffffff &&
		m_trackselectinfo[type].cur_selected_astrackid != m_trackselectinfo[type].pre_selected_astrackid && 
		( ( m_trackselectinfo[type].cur_selected_trackid == ptr_sample->uFPTrackID && m_trackselectinfo[type].cur_selected_astrackid == ptr_sample->uASTrackID)
		|| (m_trackselectinfo[type].pre_selected_trackid == ptr_sample->uFPTrackID && m_trackselectinfo[type].pre_selected_astrackid == ptr_sample->uASTrackID ) ) )

		//( m_trackselectinfo[type].cur_selected_trackid == ptr_sample->uTrackID || m_trackselectinfo[type].pre_selected_trackid == ptr_sample->uTrackID ) )
		return VO_RET_SOURCE2_OK;

	return VO_RET_SOURCE2_TRACKNOTFOUND;
}

VO_VOID vo_tracksample_filter::fill_selectinfo()
{
	vo_singlelink_list< VODS_SELECTED_INFO > list;

	m_ptr_programinfo_op->GetSelectedTrackID( &list );

	vo_singlelink_list< VODS_SELECTED_INFO >::iterator iter = list.begin();
	vo_singlelink_list< VODS_SELECTED_INFO >::iterator itere = list.end();

	while( iter != itere )
	{
		if( m_trackselectinfo[iter->nType].status == TRACKSTATUS_INIT )
		{
			m_trackselectinfo[iter->nType].status = TRACKSTATUS_NORMAL;
			m_trackselectinfo[iter->nType].cur_selected_trackid = iter->uFPTrackID;
			m_trackselectinfo[iter->nType].cur_selected_astrackid = iter->uASTrackID;
		}
		else
		{
			if( m_trackselectinfo[iter->nType].cur_selected_trackid != iter->uFPTrackID || m_trackselectinfo[iter->nType].cur_selected_astrackid != iter->uASTrackID )
			{
				m_trackselectinfo[iter->nType].pre_selected_trackid = m_trackselectinfo[iter->nType].cur_selected_trackid;
				m_trackselectinfo[iter->nType].pre_selected_astrackid = m_trackselectinfo[iter->nType].cur_selected_astrackid;
				m_trackselectinfo[iter->nType].cur_selected_trackid = iter->uFPTrackID;
				m_trackselectinfo[iter->nType].cur_selected_astrackid = iter->uASTrackID;
			}
		}

		iter++;
	}

	VOLOGI( "Audio Status: %d Pre_ASTID: %d PreTID: %d Cur_ASTID: %d Cur_TID: %d" , (VO_U32)m_trackselectinfo[VO_SOURCE2_TT_AUDIO].status , m_trackselectinfo[VO_SOURCE2_TT_AUDIO].pre_selected_astrackid , m_trackselectinfo[VO_SOURCE2_TT_AUDIO].pre_selected_trackid , m_trackselectinfo[VO_SOURCE2_TT_AUDIO].cur_selected_astrackid , m_trackselectinfo[VO_SOURCE2_TT_AUDIO].cur_selected_trackid );
	VOLOGI( "Video Status: %d Pre_ASTID: %d PreTID: %d Cur_ASTID: %d Cur_TID: %d" , (VO_U32)m_trackselectinfo[VO_SOURCE2_TT_VIDEO].status , m_trackselectinfo[VO_SOURCE2_TT_VIDEO].pre_selected_astrackid , m_trackselectinfo[VO_SOURCE2_TT_VIDEO].pre_selected_trackid , m_trackselectinfo[VO_SOURCE2_TT_VIDEO].cur_selected_astrackid , m_trackselectinfo[VO_SOURCE2_TT_VIDEO].cur_selected_trackid );
	VOLOGI( "Subtitle Status: %d Pre_ASTID: %d PreTID: %d Cur_ASTID: %d Cur_TID: %d" , (VO_U32)m_trackselectinfo[VO_SOURCE2_TT_SUBTITLE].status , m_trackselectinfo[VO_SOURCE2_TT_SUBTITLE].pre_selected_astrackid , m_trackselectinfo[VO_SOURCE2_TT_SUBTITLE].pre_selected_trackid , m_trackselectinfo[VO_SOURCE2_TT_SUBTITLE].cur_selected_astrackid , m_trackselectinfo[VO_SOURCE2_TT_SUBTITLE].cur_selected_trackid );
}