#include "voTimestampRollbackInfo.h"
#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define TIMESTAMP_ROLLBACK_DELTA  10*1000

voTimestampRollbackInfo::voTimestampRollbackInfo(void)
{
}

voTimestampRollbackInfo::~voTimestampRollbackInfo(void)
{
	Reset();
}


VO_VOID voTimestampRollbackInfo::AddRollbackInfo( VO_U32 uFPTrackID )
{

	_ROLLBACK_INFO info;
	info.uFPTrackID = uFPTrackID;
	info.uLastTimestamp = 0;

	m_rollbackInfoList.push_back( info );
}



VO_VOID voTimestampRollbackInfo::Reset()
{
	m_rollbackInfoList.reset();
}

VO_BOOL voTimestampRollbackInfo::CheckRollback(VO_U32 uFPTrackID, VO_U64 uTimeStamp)
{
	VO_BOOL bRollback = VO_FALSE;

	vo_singlelink_list< _ROLLBACK_INFO >::iterator iter = m_rollbackInfoList.begin();
	vo_singlelink_list< _ROLLBACK_INFO >::iterator itere = m_rollbackInfoList.end();

	while( iter != itere )
	{
		if( (*iter).uFPTrackID == uFPTrackID )
		{
			VO_U64 delta = 0;
			(*iter).uLastTimestamp > TIMESTAMP_ROLLBACK_DELTA ? delta = (*iter).uLastTimestamp - TIMESTAMP_ROLLBACK_DELTA : delta = 0;
			if( uTimeStamp < delta )
			{
				bRollback = VO_TRUE;
				VOLOGW("Timestamp rollback. FPTrackID:%d, LastTimeStamp:%lld, CurTimeStamp:%lld.",(*iter).uFPTrackID, (*iter).uLastTimestamp, uTimeStamp);
			}
			else
			{
				bRollback = VO_FALSE;
			}
			(*iter).uLastTimestamp = uTimeStamp;
		}
		iter++;
	}

	return bRollback;
}