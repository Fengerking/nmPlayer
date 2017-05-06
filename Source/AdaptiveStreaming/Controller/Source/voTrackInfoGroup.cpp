#include "voTrackInfoGroup.h"
#include "voProgramInfo.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voTrackInfoGroup::voTrackInfoGroup(void)
{
}

voTrackInfoGroup::~voTrackInfoGroup(void)
{
	Reset();
}


_TRACK_INFO * voTrackInfoGroup::GetTrackInfoByFPTrackID( VO_U32 uFPTrackID )
{
	vo_singlelink_list< _TRACK_INFO_EX >::iterator iter = m_trackInfoList.begin();
	vo_singlelink_list< _TRACK_INFO_EX >::iterator itere = m_trackInfoList.end();

	while( iter != itere )
	{
		if( (*iter).pTrackInfo->uFPTrackID == uFPTrackID )
			return (*iter).pTrackInfo;
		iter++;
	}

	return NULL;
}

VO_U32 voTrackInfoGroup::GetNewByFPTrackID( VO_U32 uFPTrackID )
{
	vo_singlelink_list< _TRACK_INFO_EX >::iterator iter = m_trackInfoList.begin();
	vo_singlelink_list< _TRACK_INFO_EX >::iterator itere = m_trackInfoList.end();

	while( iter != itere )
	{
		if( (*iter).pTrackInfo->uFPTrackID == uFPTrackID )
			return (*iter).bNew;
		iter++;
	}

	return 0;
}

VO_U32 voTrackInfoGroup::SetNewByFPTrackID( VO_U32 uFPTrackID, VO_U32 nFlag )
{
	vo_singlelink_list< _TRACK_INFO_EX >::iterator iter = m_trackInfoList.begin();
	vo_singlelink_list< _TRACK_INFO_EX >::iterator itere = m_trackInfoList.end();

	while( iter != itere )
	{
		if( (*iter).pTrackInfo->uFPTrackID == uFPTrackID )
			(*iter).bNew = nFlag;
		iter++;
	}

	return 0;
}


VO_VOID voTrackInfoGroup::AddTrackInfo( _TRACK_INFO *pTrackInfo )
{
	_TRACK_INFO * pInfo = NULL;
	CopyTrackInfoOP_T( pTrackInfo, &pInfo );

	_TRACK_INFO_EX infoEx;
	infoEx.pTrackInfo = pInfo;
	infoEx.bNew = VO_TRUE;

	m_trackInfoList.push_back( infoEx );
}

_TRACK_INFO * voTrackInfoGroup::GetNextTrackInfo( _TRACK_INFO *pTrackInfo, VO_BOOL bIsNew)
{
	vo_singlelink_list< _TRACK_INFO_EX >::iterator iter = m_trackInfoList.begin();
	vo_singlelink_list< _TRACK_INFO_EX >::iterator itere = m_trackInfoList.end();
#if 0
	while( iter != itere )
	{
		if(pTrackInfo == NULL)
			return (*iter).pTrackInfo;

		if( pTrackInfo == (*iter).pTrackInfo )
		{
			iter ++;
			if( iter != itere )
				return (*iter).pTrackInfo;
			else
				return NULL;
		}
		iter++;
	}
#else
	while( iter != itere )
	{
		if(pTrackInfo == NULL )
		{
			if( !bIsNew || (*iter).bNew == VO_TRUE )
				return (*iter).pTrackInfo;
		}
		else if( pTrackInfo == (*iter).pTrackInfo )
		{
			iter ++;
			if( iter == itere )
				return NULL;
			while( iter != itere )
			{
				if( !bIsNew || (*iter).bNew == VO_TRUE )
					return (*iter).pTrackInfo;
				iter ++;
			}
			return NULL;
		}
		iter++;
	}
#endif

	return NULL;
}

VO_VOID voTrackInfoGroup::Reset()
{
	vo_singlelink_list< _TRACK_INFO_EX >::iterator iter = m_trackInfoList.begin();
	vo_singlelink_list< _TRACK_INFO_EX >::iterator itere = m_trackInfoList.end();

	while( iter != itere )
	{
		ReleaseTrackInfoOP_T( (*iter).pTrackInfo );
		iter++;
	}

	m_trackInfoList.reset();
}