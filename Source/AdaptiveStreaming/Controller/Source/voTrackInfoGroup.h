#ifndef __VOTRACKINFOGROUP_H__

#define __VOTRACKINFOGROUP_H__

#include "voDSType.h"
#include "vo_singlelink_list.hpp"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

struct _TRACK_INFO_EX
{
	_TRACK_INFO *pTrackInfo;
	VO_U32		bNew;
};

class voTrackInfoGroup
{
public:
	voTrackInfoGroup();
	virtual ~voTrackInfoGroup();

	virtual _TRACK_INFO * GetTrackInfoByFPTrackID( VO_U32 uFPTrackID );
	virtual VO_VOID AddTrackInfo( _TRACK_INFO *pTrackInfo );
	virtual VO_U32 GetNewByFPTrackID( VO_U32 uFPTrackID );
	virtual VO_U32 SetNewByFPTrackID( VO_U32 uFPTrackID, VO_U32 nFlag );
	virtual _TRACK_INFO * GetNextTrackInfo( _TRACK_INFO *pTrackInfo,VO_BOOL bIsNew = VO_FALSE);
	virtual VO_VOID Reset();
	virtual VO_U32 GetCount(){return m_trackInfoList.count(); }
private:
	vo_singlelink_list< _TRACK_INFO_EX > m_trackInfoList;
};

#ifdef _VONAMESPACE
}
#endif

#endif
