#ifndef __VOITEMTHREAD_H__

#define __VOITEMTHREAD_H__

#include "vo_thread.h"
#include "voSource2_IO.h"
#include "voAdaptiveStreamParserWrapper.h"
#include "voStreamingDRM.h"
#include "voAdaptiveStreamingControllerInfo.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKBEGIN	(VO_EVENTID_SOURCE2_BASE | VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x1001)
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKCOMPLETE	(VO_EVENTID_SOURCE2_BASE | VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x1002)
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKEOS	(VO_EVENTID_SOURCE2_BASE | VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x1003)
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_CHUNKTIMEREVISE	(VO_EVENTID_SOURCE2_BASE | VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x1004)
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_DOWNLOADSTARTCOLLECTOR (VO_EVENTID_SOURCE2_BASE | VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x1005)
#define VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_DOWNLOADENDCOLLECTOR (VO_EVENTID_SOURCE2_BASE | VO_EVENTID_SOURCE2_ADAPTIVESTREAMING_BASE | 0x1006)

/*in voAdaptiveStreamingControllerInfo.h, the number of DRM is defined with this enum counts.
Now the default DRM counts is 10, if ThreadType is lager than 10, please modify the DRM counts.
*/


struct THREADINFO
{
	THREADTYPE type;
	voAdaptiveStreamingControllerInfo * pInfo;
};

struct CHUNKINFO
{
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE chunktype;
#ifdef _USE_BUFFERCOMMANDER
	VO_U32  uASTrackID;
	VO_BOOL	bDownloadComplete;
	VO_BOOL bNeedSmoothSwitch;
	VO_BOOL	bNeedSwitchAudioThread;
#endif
	VO_U64	start_timestamp;
	VO_U64	end_timestamp;

	VO_U64	size;
	VO_U32	duration;
#ifdef _USE_BUFFERCOMMANDER
	VO_U64	download_time;
	VO_VOID **ppChunkAgent;
#else
	VO_U32 download_time;
#endif
};

class voItemThread :
	public vo_thread
{
public:
	voItemThread(void);
	virtual ~voItemThread(void);

	virtual VO_VOID Start( THREADINFO * ptr_info );
	virtual VO_VOID Stop();
	virtual VO_VOID Resume();

protected:

	THREADINFO * m_ptr_info;
};

#ifdef _VONAMESPACE
}
#endif

#endif
