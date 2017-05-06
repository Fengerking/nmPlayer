
#ifndef __VOADAPTIVESTREAMINGCONTROLLERINFO_H__

#define __VOADAPTIVESTREAMINGCONTROLLERINFO_H__

#include "voAdaptiveStreamParser.h"
#include "voStreamingDRM.h"
#include "voAdaptiveStreamParserWrapper.h"
#include "voAdaptiveStreamingTimeStampSync.h"
#include "voSource2_IO.h"
#include "voProgramInfoOp.h"
#include "voCMutex.h"
#include "CSourceIOUtility.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define _DRMCOUNTS 10

class voAdaptiveStreamingControllerInfo
{
public:
	voAdaptiveStreamingControllerInfo();
	~voAdaptiveStreamingControllerInfo();

	//For Program Type
	VO_VOID SetProgramType( VO_SOURCE2_PROGRAM_TYPE type );
	inline VO_SOURCE2_PROGRAM_TYPE GetProgramType(){ return m_ProgramType; }
	//

	//For Streaming Type
	VO_VOID SetStreamingType( VO_ADAPTIVESTREAMPARSER_STREAMTYPE type );
	inline VO_ADAPTIVESTREAMPARSER_STREAMTYPE GetStreamingType(){ return m_StreamingType; }
	//

	//For Streaming Parser
	VO_VOID SetAdaptiveStreamingParserPtr( voAdaptiveStreamParserWrapper * pParser );
	inline voAdaptiveStreamParserWrapper * GetAdaptiveStreamingParserPtr(){ return m_pStreamingParser; }
	//

	//For IO
	VO_VOID SetIOPtr( VO_SOURCE2_IO_API * pAPI );
	inline VO_SOURCE2_IO_API * GetIOPtr(){ return m_pIO; }
	//

	//For LibOp
	VO_VOID SetLibOp( VO_SOURCE2_LIB_FUNC * pLibOp );
	inline VO_SOURCE2_LIB_FUNC * GetLibOp(){ return m_pLibOP; }
	//
	//For LogFunc
	VO_VOID SetLogFunc( VO_LOG_PRINT_CB * pVologCB ) { m_pVologCB = pVologCB ;}
	inline VO_LOG_PRINT_CB * GetLogFunc(){ return m_pVologCB; }
	//

	//for WorkPath
	VO_VOID SetWorkPath( VO_TCHAR * pWorkPath );
	inline VO_TCHAR * GetWorkPath(){ return m_pWorkPath; }

	//For Sample Callback
	VO_VOID SetSampleCallback( VO_SOURCE2_SAMPLECALLBACK * pSampleCB );
	inline VO_SOURCE2_SAMPLECALLBACK * GetSampleCallback(){ return m_pSampleCB; }
	//

	//For Event Callback
	VO_VOID SetEventCallback( VO_SOURCE2_EVENTCALLBACK * pEventCB );
	inline VO_SOURCE2_EVENTCALLBACK * GetEventCallback(){ return m_pEventCB; }
	//For IOEvent Callback
	VO_VOID SetIOEventCallback( VO_SOURCEIO_EVENTCALLBACK * pEventCB );
	inline VO_SOURCEIO_EVENTCALLBACK * GetIOEventCallback(){ return m_pIOEventCB; }

	//For DRM
	VO_VOID SetDRMPtr(VO_U32 nThreadType, VO_StreamingDRM_API * pDRM );
	VO_StreamingDRM_API * GetDRMPtr(VO_U32 nThreadType);
	//

	//For UTC
	VO_VOID SetStartUTC( VO_U64 utc );
	inline VO_U64 GetStartUTC(){ return m_StartUTC; }

	VO_VOID SetStartSysTime( VO_U64 systime );
	inline VO_U64 GetStartSysTime(){ return m_StartSysTime; }
	//

	//For Chunk Sync
	inline voAdaptiveStreamingTimeStampSync & GetTimeStampSyncObject(){ return m_Sync; }
	//

	//For Max Download Bitrate
	VO_VOID SetMaxDownloadBitrate( VO_U32 max_bitrate );
	inline VO_U32 GetMaxDownloadBitrate(){ return m_maxbitrate; }
	//

	//For Authentication
	VO_VOID SetVerificationInfo( VO_SOURCE2_VERIFICATIONINFO * pInfo );
	inline VO_SOURCE2_VERIFICATIONINFO * GetVerificationInfo(){ return m_IsVerifyInfoValid ? &m_VerifyInfo : 0 ; }
	//

	//for StartTime
	VO_VOID SetStartTime( VO_U64  uStartTime ) { m_uStartTime = uStartTime; }
	inline VO_U64 GetStartTime(){ return m_uStartTime; }


	//for ProgramInfoOP
	VO_VOID SetProgramInfoOP( voProgramInfoOp *pvoProgramInof ) { m_pProgramInfoOP = pvoProgramInof; }
	inline voProgramInfoOp *GetProgramInfoOP() { return m_pProgramInfoOP; }

	//for Cookie
	VO_VOID SetIOHttpCallback( VO_SOURCE2_IO_HTTPCALLBACK *pIOHttpCallback ) { m_pIOHttpCallback = pIOHttpCallback; }
	inline VO_SOURCE2_IO_HTTPCALLBACK *GetIOHttpCallback() { return m_pIOHttpCallback; }

	//for buffer state
	VO_VOID			SetBuffering(VO_BOOL bBuffering);
	inline VO_BOOL	IsBuffering() {return m_bBuffering;}

	//for buffer duration
	VO_VOID			SetMinBufferDuration(VO_U32 uDuration){m_uMinDuration = uDuration;}
	inline VO_U32		GetMinBufferDuration(){return m_uMinDuration;}

	//for check whether it is playing at lowest bitrate
	VO_VOID			SetMinBitratePlay(VO_BOOL bMinPlaying){m_bMinBitratePlaying = bMinPlaying;}
	inline VO_BOOL	IsMinBitratePlaying(){return m_bMinBitratePlaying;}

	//for check whether BA happened
	VO_VOID			SetBitrateChanging(VO_BOOL	bBitrateChanging){m_bBitrateChanging = bBitrateChanging;}
	inline VO_BOOL	IsBitrateChanging(){return m_bBitrateChanging;}

	VO_VOID			SetBAEnable(VO_BOOL bBAEnable){m_bBAEnable = bBAEnable;}
	inline VO_BOOL	IsBAEnable(){return m_bBAEnable;}

	VO_VOID			SetUpdateUrlOn(THREADTYPE type, VO_BOOL bUpdateOn);
	VO_BOOL			IsUpdateUrlOn(THREADTYPE type);

	DownloadDataCollector			*GetDownloadDataColletor(){return &m_DownloadDataColletor;}
protected:
	VO_VOID DestroyVerificationInfo();

protected:
	VO_SOURCE2_PROGRAM_TYPE m_ProgramType;

	VO_ADAPTIVESTREAMPARSER_STREAMTYPE m_StreamingType;

	voAdaptiveStreamParserWrapper * m_pStreamingParser;

	VO_SOURCE2_IO_API * m_pIO;
	VO_SOURCE2_LIB_FUNC * m_pLibOP;
	VO_TCHAR * m_pWorkPath;
	VO_LOG_PRINT_CB * m_pVologCB;

	VO_SOURCE2_SAMPLECALLBACK * m_pSampleCB;
	VO_SOURCE2_EVENTCALLBACK * m_pEventCB;
	VO_SOURCEIO_EVENTCALLBACK * m_pIOEventCB;

	VO_StreamingDRM_API * m_pDRM[_DRMCOUNTS];

	VO_U64 m_StartUTC;
	VO_U64 m_StartSysTime;

	voAdaptiveStreamingTimeStampSync m_Sync;

	VO_U32 m_maxbitrate;

	VO_SOURCE2_VERIFICATIONINFO m_VerifyInfo;
	VO_BOOL m_IsVerifyInfoValid;

	VO_U64 m_uStartTime;
	VO_U32 m_uMinDuration;

	voProgramInfoOp *m_pProgramInfoOP;

	VO_SOURCE2_IO_HTTPCALLBACK *m_pIOHttpCallback;

	VO_BOOL m_bBuffering;
	VO_BOOL m_bMinBitratePlaying;
	VO_BOOL m_bBitrateChanging;
	VO_BOOL	 m_bBAEnable;
	VO_BOOL	 m_bVideoUpdateOn;
	VO_BOOL	 m_bAudioUpdateOn;
	VO_BOOL	 m_bSubtitleUpdateOn;
	
	DownloadDataCollector m_DownloadDataColletor;
};

#ifdef _VONAMESPACE
}
#endif


#endif