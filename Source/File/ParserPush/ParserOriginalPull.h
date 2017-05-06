#pragma once
#include "IParserPush.h"
#include "StreamCache.h"
#include "CDllLoad.h"
#include "voThread.h"
#include "voCSemaphore.h"
#include "Thread.h"

struct TrackThreadInfo{
	VO_PTR					pSelf;
	VO_S32					nTrack;
	VO_PARSER_OUTPUT_TYPE	Type;
	voThreadHandle			Handle;
};

class CParserOriginalPull :
	public IParserPush, CThread
{
public:
	CParserOriginalPull(void);
	~CParserOriginalPull(void);

	VO_U32 Open(VO_PARSER_INIT_INFO *pParma);
	VO_U32 Close();
	VO_U32 Reset();

	VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer);
	VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	VO_U32 GetParam(VO_U32 uID, VO_PTR pParam);

	VO_U32 setDLL(VO_TCHAR *szDllFile, VO_TCHAR *szAPIName);

	VO_S64 getLastTimeStamp();
	VO_VOID setAddEndTimeStamp(VO_S64);

	VO_PTR getStream() { return &m_Stream; }

	VO_VOID ThreadMain();

	VO_VOID thread_GetSample(TrackThreadInfo *);
private:
	VO_BOOL m_bRunning;
	VO_BOOL m_bInited;
	TrackThreadInfo m_track_thread_info[2];

	VO_PARSER_INIT_INFO m_parser_init_info;
	VO_FILE_OPERATOR m_opFile;
	CStreamCache m_Stream;
	voCSemaphore m_event;

	CDllLoad m_dlEngine;
	VO_PTR m_parser_handle;
	VO_SOURCE_READAPI m_parser_api;

	VO_S64 m_nLastTimeStamp;
	VO_S64 m_nAddendTimeStamp;
};

