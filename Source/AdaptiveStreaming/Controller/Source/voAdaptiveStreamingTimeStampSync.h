
#ifndef __VOADAPTIVESTREAMINGTIMESTAMPSYNC_H__

#define __VOADAPTIVESTREAMINGTIMESTAMPSYNC_H__

#include "voSource2.h"
#include "voLockedObject.hpp"
#include "voDSType.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define RESET_IMMEDIATELY	0X01
#define RESET_FIRSTVIDEOFRAME	0X02
#define RESET_FIRSTAUDIOFRAME      0X03
#define RESET_FIRSTAVFRAME		0X04

class voAdaptiveStreamingTimeStampSync
{
public:
	voAdaptiveStreamingTimeStampSync();
	~voAdaptiveStreamingTimeStampSync();

	VO_VOID SetChunkDuration( VO_U64 duration );
	VO_U32 CheckAndWait( VO_U32 type , _SAMPLE * pSample , VO_BOOL * pIsStop );
	VO_U32 Reset( VO_U64 ullTime, VO_U32 nResetType);

	VO_VOID Disable( VO_BOOL b = VO_TRUE );

protected:
	VO_U32 Reset2( VO_U64 ullTime);

	voLockedObject< VO_U64 > m_sync_timestamp;
	voLockedObject< VO_U64 > m_last_audiotime;
	voLockedObject< VO_U64 > m_last_videotime;
	voLockedObject< VO_U64 > m_max_deltavalue;

	VO_BOOL m_isaudioavalible;
	VO_BOOL m_isvideoavalible;

	VO_BOOL m_disable;

	VO_U32 m_nResetType;
	VO_U64 m_nResetTime;
	VO_BOOL m_bReset;
	VO_U32 m_nVideoThreadID;
	VO_U32 m_nAudioThreadID;
};

#ifdef _VONAMESPACE
}
#endif


#endif