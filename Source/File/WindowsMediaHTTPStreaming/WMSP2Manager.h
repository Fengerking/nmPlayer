#ifndef __CWMSP2Manager_H__
#define __CWMSP2Manager_H__

#include "WMHTTP.h"
#include "StreamCache.h"
#include "CDllLoad.h"
#include "voStreaming.h"
#include "voSourceBufferManager.h"
#include "voCSemaphore.h"
#include "voSourceBaseWrapper.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CWMSP2Manager
	: public voSourceBaseWrapper
	, public CWMHTTP::Delegate
	, public CThread
	, public CDllLoad
{
public:
	CWMSP2Manager(void);
	~CWMSP2Manager(void);

	virtual VO_U32 Init(VO_PTR pSource, VO_U32 nFlag, VO_SOURCE2_INITPARAM* pParam);
	virtual VO_U32 Uninit();

	virtual VO_U32 Open();

	virtual VO_U32 Start();
	virtual VO_U32 Pause();
	virtual VO_U32 Stop();
	virtual VO_U32 Seek(VO_U64* pTimeStamp);

	virtual VO_U32 GetSample(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_PTR pSample) { return m_SourceBuf.GetBuffer(nOutPutType, pSample); }

	virtual VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam) { return m_apiASFFR.GetSourceParam(m_hASFFR, nParamID, pParam); }
	virtual VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);

	CStreamCache * getStream() { return &m_Stream; }

	virtual VO_VOID some(VO_CHAR *buf, VO_S32 iLen) { m_Stream.write((VO_BYTE *)buf, iLen); }
	virtual VO_VOID NotifyEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

	virtual VO_VOID ThreadMain();

	VO_BOOL			HaveLicense() { return m_pCheck ? VO_TRUE: VO_FALSE; }

protected:
	VO_U32 doPlay(VO_S64 llStartTime = 0);
	VO_U32 doSeek();

	VO_U32 AddEOS();


	virtual VO_U32		OnOpen();
	virtual VO_U32		OnClose();
	virtual VO_U32		OnGetDuration(VO_U64 * pDuration);
	virtual VO_U32		OnGetTrackCnt(VO_U32 * pTraCnt);
	virtual VO_U32		OnGetSample(VO_SOURCE2_TRACK_TYPE nOutPutType , VO_SOURCE_SAMPLE * pSample);
	virtual VO_U32		OnGetTrackAvailable(VO_U32 uTrackIndex , VO_BOOL * beAlai);
	virtual VO_U32		OnGetTrackInfo(VO_U32 uTrackIndex , VO_SOURCE_TRACKINFO * pTrackInfo) { return m_apiASFFR.GetTrackInfo(m_hASFFR, uTrackIndex, pTrackInfo); }
	virtual VO_U32		OnGetTrackFourCC(VO_U32 uTrackIndex , VO_U32 * pFourCC) { return m_apiASFFR.GetTrackParam(m_hASFFR, uTrackIndex, VO_PID_SOURCE_CODECCC, pFourCC); }
	virtual VO_U32		OnGetTrackMaxSampleSize(VO_U32 uTrackIndex , VO_U32 * pMaxSampleSize) { return m_apiASFFR.GetTrackParam(m_hASFFR, uTrackIndex, VO_PID_SOURCE_MAXSAMPLESIZE, pMaxSampleSize); }
	virtual VO_U32		OnGetAudioFormat(VO_U32 uTrackIndex , VO_AUDIO_FORMAT * pAudioFormat) { return m_apiASFFR.GetTrackParam(m_hASFFR, uTrackIndex, VO_PID_AUDIO_FORMAT, pAudioFormat); }
	virtual VO_U32		OnGetVideoFormat(VO_U32 uTrackIndex , VO_VIDEO_FORMAT * pVideoFormat) { return m_apiASFFR.GetTrackParam(m_hASFFR, uTrackIndex, VO_PID_VIDEO_FORMAT, pVideoFormat); }
	virtual VO_U32		OnGetWaveFormatEx(VO_U32 uTrackIndex , VO_PBYTE * pExData) { return m_apiASFFR.GetTrackParam(m_hASFFR, uTrackIndex, VO_PID_SOURCE_WAVEFORMATEX, pExData); }
	virtual VO_U32		OnGetVideoBitMapInfoHead(VO_U32 uTrackIndex , VO_PBYTE * pBitMap) { return m_apiASFFR.GetTrackParam(m_hASFFR, uTrackIndex, VO_PID_SOURCE_BITMAPINFOHEADER, pBitMap); }
	virtual VO_U32		OnGetVideoUpSideDown(VO_U32 uTrackIndex , VO_BOOL * pUpSideDown) { return m_apiASFFR.GetTrackParam(m_hASFFR, uTrackIndex, VO_PID_VIDEO_UPSIDEDOWN, pUpSideDown); }
	virtual VO_U32		OnGetLanguage(VO_U32 uTrackIndex , VO_CHAR ** pLanguage) { return m_apiASFFR.GetTrackParam(m_hASFFR, uTrackIndex, VO_PID_SOURCE_TRACK_LANGUAGE, pLanguage); }

private:
	enum STATE_Manager {
		Manager_Stop,
		Manager_Running,

		Manager_Pausing,	//be able to stop WMSP
		Manager_Paused,		//the WMSP is stoped
	};

	STATE_Manager	m_eStateManager;

	VO_S64	m_llSeekTime;
	VO_S64	m_llStartTime;
	VO_BOOL	m_bSeeked;

	CWMHTTP			m_WMHTTP;
	CStreamCache	m_Stream;

	voCSemaphore	m_Event;
	voCMutex		m_lock;

	VOPDInitParam		m_PD_Param;

	VO_FILE_OPERATOR	m_FileIO;
	VO_SOURCE_READAPI	m_apiASFFR;
	VO_PTR				m_hASFFR;

	VO_PTR 			m_pCheck;

	voSourceBufferManager m_SourceBuf;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CWMSP2Manager_H__