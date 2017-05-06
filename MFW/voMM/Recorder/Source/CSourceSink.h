#ifndef _CSOURCE_SINK_H
#define _CSOURCE_SINK_H

#include "CSrc.h"
#include "voMMRecord.h"
#include "CSampleData.h"
#include "voCSemaphore.h"
#include "voThread.h"
#include "vcamerabase.h"

#define DROP_FRM_UPDATE_COUNT 500

class CAVEncoder;
class CColorConvert;


class CSourceSink
{
public:
	CSourceSink(void);
public:
	virtual ~CSourceSink(void);
public:
	
	static VO_U32  EncodeAudioFunc(VO_PTR lParam);
	static VO_U32  EncodeVideoFunc(VO_PTR lParam);
	static VO_U32  EncodeAVFunc(VO_PTR lParam);

public:
	bool RecvAudioSource(VO_MMR_ASBUFFER* pBuffer);
	bool RecvRecordVideoSource(VO_MMR_VSBUFFER* pBuffer);

	void Close();

	bool StartRecord();
	void PauseRecord();
	void RestartRecord();
	void StopRecord();

public:
	void SetAVEncoder(CAVEncoder* pEncoder);

	void SetEncoderCC(CColorConvert* pCC);
	void SetNotifyProc(VOMMRecordCallBack pProc);

	void SetRecordVideoSrcFormat(VO_MMR_VSFORMAT* pFmt);
	void SetAudioFmt(VO_MMR_ASFORMAT* pFmt);
	
	bool GetVideoSrcFrameRate(float* pFrameRate);

	void SetContentType(int nType);
	int	 GetContentType();
	bool IsRecordAudio();
	bool IsRecordVideo();

	unsigned long GetEncodedDataSize();
	void SetImageQuality(int nQuality);
	void SetVideoQuality(int nQuality);

	void SetRecTimeLimit(VO_U32 dwTimeLimit);
	unsigned long GetRecTimeLimit();
	unsigned long GetCurrRecTime();

	void SetRecSizeLimit(VO_U32 dwSizeLimit);
	VO_U32 GetRecSizeLimit();

	// test code
#ifdef _TEST_THREAD_PERFORMANCE_
	CPerfTest*	m_pPerfTest;
	HANDLE		m_hRenderThread;
#endif

private:
	//use for thread
	void EncodeAudioSamples();
	void EncodeVideoSamples();
	void EncodeAudioVideoSamples();

	//
	void EncodeLeft();

private:
	bool EncodeAudioSamples(unsigned long dwTime);
	bool EncodeVideoSamples(CSampleData* pSample);

	void ReleaseAVSamples();

	bool IsNeedEncode(unsigned long& dwStart, unsigned long& dwEnd);
	void ProcessVideoSyncDrop(CSampleData* pSample);
	void ProcessVideoFullDrop(CSampleData* pSample);

	bool IsDropFrmNeighbor(unsigned long& dwStart, unsigned long& dwEnd);
	void AddDropFrm(unsigned long& dwStart, unsigned long& dwEnd);

	bool CheckErrorCode();
	bool ProcessErrorCode();

	bool CheckTimeLimit();
	bool CheckSizeLimit();

	bool AdjustAudioTimestamp(VO_MMR_ASBUFFER* pBuffer);
	bool AdjustVideoTimestamp(VO_MMR_VSBUFFER* pBuffer);
private:
	CAVEncoder* m_pAVEncoder;
	CColorConvert* m_pCCForEncoder;

	CSampleChain m_lstFullRecVS;
	CSampleChain m_lstFreeRecVS;
	CSampleChain m_lstFullRecAS;
	CSampleChain m_lstFreeRecAS;

	unsigned long m_dwStartTime;
	unsigned long m_dwLastTime;
	unsigned long m_dwCurrAudioStartTime;
	unsigned long m_dwCurrAudioLastTime;

	voCMutex m_csVideoSample;
	voCMutex m_csAudioSample;
	voCMutex m_csEncodeVideoSample;
	voCMutex m_csEncodeAudioSample;
	voCMutex m_csFindVideoDropSample;

	voThreadHandle m_hEncodeAudioThread;
	voThreadHandle m_hEncodeVideoThread;
	
	void*				m_pUserData;
	VOMMRecordCallBack m_pNotifyCB;

	bool m_bResetting;
	bool m_bRecording;
	bool m_bHasPreviewVS;

	VO_MMR_ASFORMAT m_AudioFmt;
	VO_MMR_VSFORMAT m_RenderVideoFormat;
	VO_MMR_VSFORMAT m_RecordVideoFormat;

	int m_nVSBufCount;
	unsigned long m_dwSyncVideoDropTime;
	unsigned long m_dwBufVideoDropTime;

	CSampleData* m_pSampleDataForVideoEncode;

	 
	voCSemaphore m_EventEncodeAudio;

	unsigned long m_dwTotalPreviewTime;
	unsigned long m_dwTotalVideoFrameCount;

	int m_nDropFrmUpdateInx;
	unsigned long m_arDropTimes[DROP_FRM_UPDATE_COUNT];//0:start time, 1:end time

	long long m_dwErrorCode;
	unsigned long m_dwRecordTimeLimit;
	unsigned long m_dwRecordSizeLimit;

	unsigned long m_dwCurrRecTime;


	int m_nTotalPauseTime;
	VO_U32 m_dwPauseTime;
	VO_U32 m_dwLastTimeForRecord;
	VO_U32 m_dwStartTimeForRecord;

	VO_U32 m_dwAudioLastTime;
	VO_U32 m_dwTotalAudioRawDataLen;

	bool m_bAdjustTimestamp;

	// 20090713
	VO_U32	m_dwFirstAVFrameStartTime;

	VO_U32	m_dwAudioLastFrameStartTime;
	VO_U32	m_dwVideoLastFrameStartTime;

	VO_U32	m_dwAudioPauseTime;
	VO_U32	m_dwVideoPauseTime;

	int		m_nAudioSampleDuration;
	int		m_nAudioTimeOffset;
};

#endif // _CSOURCE_SINK_H
