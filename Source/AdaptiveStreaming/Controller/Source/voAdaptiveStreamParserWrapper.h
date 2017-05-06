#ifndef __VOADAPTIVESTREAMPARSERWRAPPER_H__

#define __VOADAPTIVESTREAMPARSERWRAPPER_H__

#include "CDllLoad.h"
#include "voAdaptiveStreamParser.h"
#include "voCMutex.h"
#include "voDSType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voAdaptiveStreamParserWrapper
	:public CDllLoad
{
public:
	voAdaptiveStreamParserWrapper( VO_U32 adaptivestream_type , VO_SOURCE2_LIB_FUNC * pLibOp, VO_TCHAR * pWorkPath, VO_LOG_PRINT_CB * pVologCB );
	virtual ~voAdaptiveStreamParserWrapper(void);

	virtual VO_U32 Init ( VO_ADAPTIVESTREAM_PLAYLISTDATA * pData , VO_SOURCE2_EVENTCALLBACK * pCallback );
	virtual VO_U32 UnInit ();
	virtual VO_U32 Open ();
	virtual VO_U32 Close ();
	virtual VO_U32 Start ();
	virtual VO_U32 Stop ();
	virtual VO_U32 Update ( VO_ADAPTIVESTREAM_PLAYLISTDATA * pData );
	virtual VO_U32 GetChunk ( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE uID ,  VO_ADAPTIVESTREAMPARSER_CHUNK **ppChunk );
	virtual VO_U32 Seek ( VO_U64 * pTimeStamp, VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode );
	virtual VO_U32 GetDuration ( VO_U64 * pDuration);
	virtual VO_U32 GetProgramCount ( VO_U32 *pProgramCount );
	virtual VO_U32 GetProgramInfo ( VO_U32 uProgram , _PROGRAM_INFO  **ppProgramInfo );
	virtual VO_U32 GetCurTrackInfo ( VO_SOURCE2_TRACK_TYPE sTrackType , _TRACK_INFO ** ppTrackInfo );
	virtual VO_U32 SelectProgram ( VO_U32 uProgram);
	virtual VO_U32 SelectStream ( VO_U32 uStream);
#ifdef _new_programinfo
	virtual VO_U32 SelectTrack ( VO_U32 uTrack, VO_SOURCE2_TRACK_TYPE sTrackType);
#else
	virtual VO_U32 SelectTrack ( VO_U32 uTrack );
#endif
	virtual VO_U32 GetDRMInfo ( VO_SOURCE2_DRM_INFO **ppDRMInfo );
	virtual VO_U32 GetParam ( VO_U32 nParamID, VO_PTR pParam );
	virtual VO_U32 SetParam ( VO_U32 nParamID, VO_PTR pParam );

protected:
	VO_ADAPTIVESTREAM_PARSER_API m_api;
	VO_HANDLE m_parserhandle;

	voCMutex m_lock;
};

#ifdef _VONAMESPACE
}
#endif

#endif
