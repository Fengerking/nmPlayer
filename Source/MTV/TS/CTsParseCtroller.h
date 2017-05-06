#pragma once
#include "voYYDef_TS.h"
#include "TSParser.h"
#include "fMacros.h"
#include "ITsParseCtroller.h"
#include "CvoBaseMemOpr.h"
#include "voCMutex.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#define MAX_STREAM_COUNT 12
class CStream;
class FrameChecker;

#define _USE_RELATIVE_TIME
//add by qichaoshen @ 2011-10-28 add playback ready check

#define _CHECK_TS_PLAYBACK_READY  0x11
#define _FORCE_TO_PLAY            0x12
//add by qichaoshen @ 2011-10-28 add playback ready check
//#define CHANGE_PARSE_TYPE

class CTsParseCtroller : public ITsParseController, public VOSTREAMPARSELISTENER, public CvoBaseMemOpr
{
public:
	CTsParseCtroller(void);
	virtual ~CTsParseCtroller(void);

	// interface from ITsParseController
public:
	virtual VO_U32 Open(PARSERPROC pProc, VO_VOID* pUserData);
	virtual VO_U32 Close();
	virtual VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer);
	virtual VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 GetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 SelectProgram(VO_U32 uProgramID);
	virtual VO_U32 SetParseType(VO_U32 nType);
	virtual VO_U32 ResetState();

	//for reader
	virtual VO_BOOL IsProgramInfoParsed();
	virtual VO_U8	GetStreamCount();
	virtual CStream*GetStreamByIdx(VO_U32 nIdx);

	virtual VO_BYTE* GetFrameBuf(int nStreamId);
	virtual VO_U32 GetBufferSize(int nStreamId);
	virtual VO_U32 SetNewBufSize(int nStreamId, VO_U32  ulNewBufferSize);
	virtual VO_U32 SetNewTimeStamp(int nStreamId, VO_S64  illTimeStamp);

public:
	void           SetEITCallbackFun(void*  pFunc);
    void           SetOpenFlag(VO_U32 ulOpenFlag);
    VO_BOOL        IsVideoTrackReady();


private:
	CStream*	GetStream(VO_U32 nStreamID);
	VO_VOID		IssueFrame(CStream* pStream);
	VO_U64		CheckTimestamp(VO_U64 time, VO_BOOL bVideo);
	VO_VOID		OnStreamChanged();

	// interface from VOSTREAMPARSELISTENER
public:
	virtual void OnProgramIndex(VOSTREAMPARSEPROGRAMINDEX* pProgramIndex);
	virtual void OnNewProgram(VOSTREAMPARSEPROGRAMINFO* pProgramInfo);
	virtual void OnNewProgramEnd();
	virtual void OnNewStream(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	virtual void OnStreamChanged(VOSTREAMPARSESTREAMINFO* pStreamInfo);
	virtual void OnNewStreamEnd();
	virtual void OnNewFrame(int nStreamId, VOSTREAMPARSEFRAMEINFO* pFrameInfo);
	virtual void OnFrameData(int nStreamId, void* pData, int nSize);
	virtual void OnFrameEnd(int nStreamId);
	virtual void OnFrameError(int nStreamId, int nErrorCode);
private:
	CTSParser*		m_pParser;
	VO_BOOL			m_bSelectProgram;
	CStream*		m_pStream[MAX_STREAM_COUNT];
	VO_U16			m_nStreamCount;
	VO_BOOL			m_bParsedProgramInfo;
	VO_U32			m_nParseType;

	VO_BYTE*        m_pBufferForPCM;
	VO_U32          m_ulPcmBufferMaxLen;
	VO_U32          m_ulPcmBufferLen;
	VO_U64          m_ullTimeForPCM;

	VO_U32          m_ulMPEGFrameCount;
	VO_U64          m_ullLastTimeStamp;
    VO_U32          m_ulOpenFlag;
    VO_BOOL         m_bVideoTrackReady;

	//voCMutex		m_Mutex;
};
#ifdef _VONAMESPACE
}
#endif

