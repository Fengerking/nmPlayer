#ifndef __CWMSPManager_H__
#define __CWMSPManager_H__

#pragma once
#include "WMHTTP.h"
#include "StreamCache.h"
#include "CDllLoad.h"
#include "voSource.h"
#include "voStreaming.h"

class CWMSPManager
	: public CWMHTTP::Delegate
	, public CThread
{
public:
	CWMSPManager(void);
	~CWMSPManager(void);

	VO_BOOL	Open(VO_SOURCE_OPENPARAM * pParam);
	VO_U32	Close();
	VO_U32	GetSourceInfo(VO_SOURCE_INFO * pSourceInfo);
	VO_U32	GetTrackInfo(VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo);
	VO_U32	GetSample(VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample);
	VO_U32	SetPos(VO_U32 nTrack, VO_S64 * pPos);
	VO_VOID	MoveTo( VO_S64 pos );
	VO_U32	SetSourceParam(VO_U32 uID, VO_PTR pParam);
	VO_U32	GetSourceParam(VO_U32 uID, VO_PTR pParam);
	VO_U32	SetTrackParam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);
	VO_U32	GetTrackParam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);

	CStreamCache * getStream() { return &m_Stream; }

	virtual VO_VOID some(VO_CHAR *buf, VO_S32 iLen);

	virtual VO_VOID ThreadMain();

protected:
	VO_S32 Play();

	VO_VOID process_STREAMING_QUICKRESPONSE_COMMAND( VO_QUICKRESPONSE_COMMAND command );

	VO_VOID notify( long EventCode, long * EventParam );

private:
	enum STATE_Manager {
		Manager_Stop,
		Manager_Running,	//be able to start
		Manager_Pausing,	//be able to stop
		Manager_Paused,

		//Manager_Starting,
		Manager_Seeking//=stop+running
	};

	STATE_Manager	m_StateManager;

	VO_SOURCE_OPENPARAM m_source_param;
	VO_SOURCEDRM_CALLBACK m_sourcedrm_callbcak;
	VOPDInitParam m_PD_param;
	VO_CHAR m_url[1024];

	CWMHTTP m_WMHTTP;
	CStreamCache m_Stream;

	VO_SOURCE_READAPI m_parser_api;
	CDllLoad m_dllloader;

	VO_FILE_OPERATOR m_opFile;
	VO_PTR m_fileparser;

	VO_S32 m_duration;

	VO_S64 m_llSeekTime;

	VO_S32 m_iOverall_Kbitrate;

	StreamingNotifyEventFunc m_notifier;

	VO_BOOL m_bBuffering;
	VO_BOOL m_bNeedCBSeekPos;
	
	VO_BOOL m_bStopedLastWhileSeeking;

	VO_SOURCE_TRACKTYPE	m_TrackType[2];
};
#endif //__CWMSPManager_H__