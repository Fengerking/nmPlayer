#ifndef __voTimestampRollbackInfo_H__

#define __voTimestampRollbackInfo_H__

#include "voDSType.h"
#include "vo_singlelink_list.hpp"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

struct _ROLLBACK_INFO
{
	VO_U64		uLastTimestamp;
	VO_U32		uFPTrackID;
};

class voTimestampRollbackInfo
{
public:
	voTimestampRollbackInfo();
	virtual ~voTimestampRollbackInfo();

	virtual VO_VOID AddRollbackInfo( VO_U32 uFPTrackID );
	virtual VO_BOOL CheckRollback( VO_U32 uFPTrackID,VO_U64 uTimestamp);
	virtual VO_VOID Reset();
	virtual VO_U32 GetCount(){return m_rollbackInfoList.count(); }
private:
	vo_singlelink_list< _ROLLBACK_INFO > m_rollbackInfoList;
};

#ifdef _VONAMESPACE
}
#endif

#endif
