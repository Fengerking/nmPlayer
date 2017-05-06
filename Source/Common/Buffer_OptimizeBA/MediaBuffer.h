#ifndef __MEDIABUFFER_H__
#define __MEDIABUFFER_H__

#include "VideoTrackBuffer.h"
#include "SubtitleTrackBuffer.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum VO_BUFFERING_STYLE_TYPE
{
	VO_BUFFERING_AV,
	VO_BUFFERING_V,
	VO_BUFFERING_A
};

enum VO_BUFFER_PID_TYPE
{
	VO_BUFFER_PID_DURATION_A,	/*!< <G> get the duration of audio in buffer*/
	VO_BUFFER_PID_DURATION_V,	/*!< <G> get the duration of video in buffer*/
	VO_BUFFER_PID_DURATION_T,	/*!< <G> get the duration of subtitle in buffer*/
	VO_BUFFER_PID_MAX_SIZE,		/*!< <S/G> set/get the maximum length of buffer*/
	VO_BUFFER_PID_IS_RUN,		/*!< <G> check whether the buffer will output data*/
	VO_BUFFER_PID_STYLE,
	VO_BUFFER_PID_Buffering_Duration,
	VO_BUFFER_PID_Quik_Start_Duration,
	VO_BUFFER_PID_EFFECTIVE_DURATION	/*!< <G> get the time of continous playback*/
};

VO_U32 _SetPos(std::list< shared_ptr<_SAMPLE> >* plist, std::list< shared_ptr<_SAMPLE> >::iterator iterP);

class MediaBuffer
{
public:
	explicit MediaBuffer(VO_U32 uMaxDuration = 20000, VO_U32 m_uBufferingDuration = 5000, VO_S32 m_uQuikStartDuration = 2000);
	~MediaBuffer(void);

	VO_U32 PutSample(const shared_ptr<_SAMPLE> pSample, VO_SOURCE2_TRACK_TYPE eTrackType);
	VO_U32 PutSamples(std::list< shared_ptr<_SAMPLE> >* plist, VO_SOURCE2_TRACK_TYPE eTrackType);
	VO_U32 GetSample(_SAMPLE *pSample, VO_SOURCE2_TRACK_TYPE eTrackType);

	VO_U32 SetPos(VO_U64* pullTimePoint);

	VO_U32 Flush();

	VO_U32 SetParameter(VO_U32 uID, VO_PTR pParam);
	VO_U32 GetParameter(VO_U32 uID, VO_PTR pParam) /*const*/;

private:
	//VO_U32 Filter(VO_SOURCE2_TRACK_TYPE eTrackType);

	VO_U32 On(VO_SOURCE2_TRACK_TYPE eTrackType);
	VO_U32 Off(VO_SOURCE2_TRACK_TYPE eTrackType);

	enum BUFFER_STATE {
		Buffer_Starting,
		Buffer_Seeking,		//Video prior to Audio
		Buffer_Running,
		Buffer_Buffering
	};

	BUFFER_STATE m_eBufferState;

	TrackBuffer			m_AudioStreamBuffer;//StreamBuffers for select 'Track'
	VideoTrackBuffer	m_VideoStreamBuffer;
	SubtitleTrackBuffer m_SubtitleStreamBuffer;

	// uMaxDuration >= m_uBufferingDuration >= m_uQuikStartDuration !!
	VO_U32 m_uMaxDuration;
	VO_U32 m_uBufferingDuration;
	VO_S32 m_uQuikStartDuration;

	VO_U64 m_ullWantedSeekPoint;
	VO_U64 m_ullActualSeekPoint;
	VO_BOOL m_bSoughtVideo;

	VO_BUFFERING_STYLE_TYPE m_eBufferingStyle;

	voCMutex m_lock;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__TRACKBUFFER_H__
