#ifndef __VOADAPTIVESTREAMINGFILEPARSER_H__

#define __VOADAPTIVESTREAMINGFILEPARSER_H__

#include "voAdaptiveStreamParser.h"
#include "voFormatChecker.h"
#include "voSource2.h"
#include "CDllLoad.h"
#include "voParser.h"
#include "voProgramInfoOp.h"
#include "voTrackInfoGroup.h"
#include "voTimestampRollbackInfo.h"
#include "voStreamingDRM.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

struct VO_ADAPTIVESTREAMPARSER_BUFFER
{
	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE	Type;

	VO_U64								ullStartTime;		/*!< The start offset time of this chunk */
	VO_ADAPTIVESTREAMPARSER_STARTEXT *	pStartExtInfo;		/*!< The extension info of the start time */

	VO_U64								ullTimeScale;

	VO_U32								uSize;
	VO_PBYTE							pBuffer;

	VO_BOOL								bIsEnd;

	VO_PTR								pPrivateData;
};

class voAdaptiveStreamingFileParser
	:public CDllLoad
{
public:
	voAdaptiveStreamingFileParser( VO_SOURCE2_SAMPLECALLBACK * pCallback );
	

	virtual ~voAdaptiveStreamingFileParser();


	virtual VO_U32 LoadParser() = 0;
	virtual VO_U32 FreeParser() = 0;

	virtual VO_U32 Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer ) = 0;

	virtual VO_VOID SetFormatChange();

	virtual VO_VOID SetFileFormat( FileFormat ff ){ m_ff = ff; }
	virtual VO_VOID SetDrmHeadData(VO_SOURCE2_TRACK_TYPE type, VO_BOOL IsDrmHeadData);
	virtual FileFormat GetFileFormat(){ return m_ff; }

	virtual VO_SOURCE2_MEDIATYPE GetMeidaType() = 0;

	virtual VO_VOID SetTimeStampOffset( VO_U64 offset ){ m_offset = offset; }
	virtual VO_U32 CreateTrackInfo2(VO_SOURCE2_TRACK_TYPE type , _TRACK_INFO * pStreamInfo){ return VO_RET_SOURCE2_NOIMPLEMENT;}
	virtual VO_U32 SetParam(VO_U32 nID, VO_VOID * pParam) = 0;
	virtual VO_VOID ResetTrackInfo();

	virtual VO_U32 SendMediaInfo( VO_SOURCE2_TRACK_TYPE type , VO_U64 timestamp, VO_U32 uStreamID, VO_U32 uFlag, VO_U32 uMarkOP );
	virtual VO_U32 StoreTrackInfo(VO_SOURCE2_TRACK_TYPE type, _TRACK_INFO *pTrackInfo);

	virtual VO_VOID SetASTrackID( VO_U32 uASTrackID );
	virtual VO_VOID SetFPTrackID( VO_U32 uFPTrackID );

	virtual VO_VOID SetTrackReset(){ m_bTrackReset = VO_TRUE ;}

	virtual VO_VOID TryToSendTrackInfo(VO_SOURCE2_TRACK_TYPE uType, VO_U64 uTimeStamp, VO_U32 uTrackID );
	virtual VO_BOOL CheckRollback(VO_U32 uFPTrackID, VO_U64 uTimestamp );
	virtual VO_VOID	OnChunkEnd() {}	// called when whole chunk data is processed
	virtual VO_VOID 	SetDrmApi(VO_StreamingDRM_API* pDrm){m_pDRM = pDrm;}

	VO_VOID MarkSubtitleFlag(_SAMPLE *pSample, VO_U32 uReserve, VO_BOOL bMark, VO_U32 uValue);

protected:
	static void VO_API ParserProcEntry(VO_PARSER_OUTPUT_BUFFER* pData) ;
	virtual void ParserProc(VO_PARSER_OUTPUT_BUFFER* pData) { ;}
	virtual VO_VOID SendTrackInfoList(VO_U64 uTimestamp);
	
	VO_SOURCE2_SAMPLECALLBACK * m_pSampleCallback;

	VO_PTR				m_hModule;

	FileFormat			m_ff;

	VO_U64				m_offset;

	_TRACK_INFO m_audio_trackinfo;
	_TRACK_INFO m_video_trackinfo;
	_TRACK_INFO m_subtitle_trackinfo;

	VO_U32 m_audio_headdata_size;
	VO_U32 m_video_headdata_size;
	VO_U32 m_subtitle_headdata_size;


	VO_BOOL m_new_audio;
	VO_BOOL m_new_video;
	VO_BOOL m_new_subtitle;


	voTrackInfoGroup m_audioTrackG;
	voTrackInfoGroup m_videoTrackG;
	voTrackInfoGroup m_subtitleTrackG;
	VO_BOOL m_bShallSendTrackInfoList;

	VO_U32 m_uASTrackID;
	VO_U32 m_uFPTrackID;
	VO_BOOL m_bTrackReset;
	VO_U32  m_uSampleMarkFlag;

	VO_U64 m_nLastAudioTimestamp;
	VO_U64 m_nLastSubtitleTimestamp;
	VO_U64 m_nLastVideoTimestamp;

	VO_BOOL	m_bIsVideoDrmHeadData;
	VO_BOOL m_bIsAudioDrmHeadData;

	voTimestampRollbackInfo m_timestampRollbackInfo;
	VO_StreamingDRM_API* m_pDRM;
};

#ifdef _VONAMESPACE
}
#endif

#endif
