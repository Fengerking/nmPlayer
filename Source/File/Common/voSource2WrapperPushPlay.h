#ifndef __VOSOURCE2WRAPPERPUSHPLAY_H__
#define __VOSOURCE2WRAPPERPUSHPLAY_H__

#include "voYYDef_filcmn.h"
#include "voSource2ParserWrapper.h"
#include "voSourceBufferManager.h"
#include "vo_thread.h"
#include "voCSemaphore.h"
#include "CBaseReader.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define PLAYPUSH_BUFFER_THREAD

enum STATE_PushPlay {
	PushPlay_Stop,
	PushPlay_Running,
	PushPlay_Seeking,
};

enum ThreadStatus
{
	ThreadStatus_Running,
	ThreadStatus_Pause
};
class voSource2WrapperPushPlay;

class CPushPlayBuff :public vo_thread
{
public:
	CPushPlayBuff(voSource2WrapperPushPlay *pWrapper);
	virtual ~CPushPlayBuff();
public:
	void	thread_function();
	VO_U32	SignalEvent(){return m_Event.Signal();}
	VO_U32  ResetEvent(){return m_Event.Reset();}
	VO_VOID	BeginThread(){return begin();}
	VO_VOID	StopThread(){return stop();}
	VO_BOOL PauseThead(){return m_Status == ThreadStatus_Pause ? VO_TRUE : VO_FALSE;}
private:
	voSource2WrapperPushPlay*	m_pWrapper;
	voCSemaphore				m_Event;
	ThreadStatus				m_Status;				
};

class voSource2WrapperPushPlay
	: public voSource2ParserWrapper
	, public vo_thread
{
public:
	voSource2WrapperPushPlay(void);
	~voSource2WrapperPushPlay(void);

	virtual VO_U32 Init(VO_PTR pSource, VO_U32 nFlag, VO_SOURCE2_INITPARAM* pParam);

	virtual VO_U32	Open();

	virtual VO_U32	Start();
	virtual VO_U32	Stop();
	virtual VO_U32	Seek(VO_U64* pTimeStamp);

	virtual VO_U32	GetSample(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_PTR pSample);

	virtual void	thread_function();

	STATE_PushPlay	GetStatus();
	VO_VOID			SetStatus(STATE_PushPlay eStatus);

	VO_U32			AddSample(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_U64 &pTime);
	VO_U32			AddEOS(VO_SOURCE2_TRACK_TYPE nOutPutType);

	VO_U64			GetCurTime(VO_SOURCE2_TRACK_TYPE nOutPutType);
	VO_VOID			SetCurTime(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_U64 llTs);
protected:
	virtual VO_U32	OnClose();
	virtual VO_U32	OnGetFileLength(VO_U64 * pLength);

	virtual VO_U32	SetTrackPos(VO_U32 nTrack, VO_U64 * pPos);
	virtual VO_U32	GenerateIndex();
	virtual VO_U32  OpenPushIO();
#ifndef PLAYPUSH_BUFFER_THREAD
	VO_U32			AddSample();
	virtual VO_U32	CheckBufferStatus();
	virtual VO_U32	GetTrackBufferDuration(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_PUSH_TRACK_INFO* pTrackInfo);
	virtual VO_BOOL	IsTrackBufferFull(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_U32 uDutation, VO_BOOL bBuffering, VO_U32 uBufferTime);
#endif
	virtual VO_U32  Flush();

private:

	STATE_PushPlay				m_eStatePushPlay;
	voCSemaphore				m_Event;
	voCMutex					m_lock;
	voSourceBufferManager		m_SourceBuf;
	VO_U64						m_ullTimeA;
	VO_U64						m_ullTimeV;
	VO_U64						m_ullCurTimeA;
	VO_U64						m_ullCurTimeV;
	voSourceSwitch				m_SrcIOPush;///<for pushplay generate index
	VO_U32						m_uBufferTime;
	VO_BOOL						m_bNeedBufing;
	CPushPlayBuff				m_PushPlayBuffer;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__VOSOURCE2WRAPPERPUSHPLAY_H__
