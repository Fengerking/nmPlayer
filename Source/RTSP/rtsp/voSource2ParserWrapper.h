
#ifndef __VOSOURCE2PARSERWRAPPER_H__
#define __VOSOURCE2PARSERWRAPPER_H__

#include "voSource.h"
#include "voSource2.h"
#include "voString.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CVORTSPTrack2
{
public:
	CVORTSPTrack2(HVOFILEREAD session,int index);
	~CVORTSPTrack2();

	void	Init();
	VO_U32 	SetTrackParam(VO_U32 uID, VO_PTR pParam);
	VO_U32 	GetTrackParam (VO_U32 uID, VO_PTR pParam);
	VO_U32	GetTrackInfo (VO_SOURCE_TRACKINFO * pTrackInfo);
	VO_U32 	GetSample (VO_SOURCE_SAMPLE * pSample);
	VO_U32 	SetPos (VO_S32 * pPos);

public:
	long						m_startTime;
	HVOFILETRACK				m_track;
	VO_PBYTE					m_headData;
	HVOFILEREAD					m_session;
	VOSAMPLEINFO				m_sampleInfo;
	VO_SOURCE_TRACKINFO			m_trackInfo;
	VOSAMPLEINFO				m_nextKeyFrame;
};

class CVORTSPSource2
{
public:
	CVORTSPSource2(VO_SOURCE_OPENPARAM * pParam);
	~CVORTSPSource2();

	VO_U32 OpenURL();
	VO_U32 GetSourceInfo(VO_SOURCE_INFO * pSourceInfo);
	VO_U32 GetTrackInfo (VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo);
	VO_U32 GetSample (VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample);
	VO_U32 SetPos (VO_U32 nTrack, VO_S32 * pPos);
	VO_U32 SetSourceParam ( VO_U32 uID, VO_PTR pParam);
	VO_U32 GetSourceParam (VO_U32 uID, VO_PTR pParam);

	VO_U32 SetTrackParam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);
	VO_U32 GetTrackParam (VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);
private:
	void   Reset();
private:
	IVOMutex* 				m_sourceLock;
	VO_S32	 				m_newPos;
	VO_SOURCE_OPENPARAM		m_param;
	HVOFILEREAD				m_session;
	CVORTSPTrack2*			m_tracks[4];
	VO_SOURCE_INFO			m_sourceInfo;
	VO_S64					m_startTime;
	bool					m_isLoading;
};

class voStream2ParserWrapper
{
public:
	voStream2ParserWrapper();
	~voStream2ParserWrapper();

	VO_U32 Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam );
	VO_VOID UnInit();

	VO_U32 Open();
	VO_U32 Close();

	VO_U32 Start();
	VO_U32 Pause();
	VO_U32 Stop();
	VO_U32 Seek(VO_U64* pTimeStamp);

	VO_U32 GetDuration(VO_U64 * pDuration);
	VO_U32 GetSample(VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample );
	
	VO_U32 GetProgrammCount(VO_U32 *pStreamCount);
	VO_U32 GetProgrammInfo(VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo);
	VO_U32 GetCurTrackInfo(VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo );

	VO_U32 SelectProgram(VO_U32 nProgram);
	VO_U32 SelectStream( VO_U32 nStream);
	VO_U32 SelectTrack(VO_U32 nTrack);

	VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam);
	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);

	VO_VOID PrintTrackInfo(VO_SOURCE2_TRACK_INFO *pTrackInfo);
	VO_VOID PrintStreamInfo(VO_SOURCE2_STREAM_INFO *pStreamInfo);
	VO_VOID PrintProgramInfo(VO_SOURCE2_PROGRAM_INFO *pProgramInfo);

	VO_U32 SendEvent(long event,long param);
	VO_BOOL    GetLicState();
	VO_BOOL     IsAddLicCheck();
private:
	void   Reset();
	VO_U32 ReOpen(VO_U64* pTimeStamp);
	
protected:
	char	 					m_url[2048];
	VO_SOURCE2_EVENTCALLBACK	*m_pCallback;

	HVOFILEREAD					m_session;
	NotifyEventFunc				m_eventCallback;
	IVOMutex*	 				m_sourceLock;

	VO_SOURCE2_PROGRAM_INFO		m_programmInfo;
	VO_SOURCE2_STREAM_INFO		*m_streamInfo;

	VOStreamingInfo				m_info;
	HS_StreamingInfo				m_outInfo;
	VO_U32						m_trackCnt;
	CVORTSPTrack2*				m_tracks[4];
	VO_SOURCE_SAMPLE			m_audioSample;
	VO_SOURCE_SAMPLE			m_videoSample;

	bool						m_isFirstFrame[4];
	VO_U32						m_trackInfoSize[4];

	VO_U32 						m_nInitFlag;
	VO_SOURCE2_INITPARAM 		m_pInitParam;
	VO_U32					m_nFlag;
	bool						m_openFlag;
	VO_CHAR				m_workingPath[1024];
	VO_U32					m_uDuration;
	
public:
	bool						m_isSeeking;
	bool						m_isEOSReached;

private:
	VO_PTR						m_hLicCheck;
	VO_BOOL					m_isAddLicCheck;	

};

#ifdef _VONAMESPACE
}
#endif

#endif //__VOSOURCE2PARSERWRAPPER_H__
