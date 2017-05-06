
#ifndef __VOCHANNELTIEMTHREAD_H__

#define __VOCHANNELTIEMTHREAD_H__

#include "voItemThread.h"
#include "voAdaptiveStreamingFileParser.h"
#include "voDRM2.h"
#include "voDSType.h"
#include "voFormatChecker.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

enum ParseItemStatus
{
	PARSEITEMSTATUS_BASTART,
	PARSEITEMSTATUS_BAEND,
	PARSEITEMSTATUS_NORMAL,

	PARSEITEMSTATUS_MAX = 0x7fffffff,
};

class voChannelItemThread :
	public voItemThread
{
public:
	voChannelItemThread(void);
	virtual ~voChannelItemThread(void);

	virtual VO_VOID Start( THREADINFO * ptr_info );
	virtual VO_VOID Stop();
	virtual VO_VOID Resume();

protected:
	virtual void thread_function();
	/* .........GetItem_II instead of
	virtual VO_U32 GetItem( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item );
	...........*/

	static VO_S32 OnSample(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);

	virtual VO_U32 ParseData( VO_PBYTE pBuffer , VO_U32 size , VO_BOOL isEnd , VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item );

	virtual VO_U32 BeginChunkDRM();
	virtual VO_U32 EndChunkDRM( VO_PBYTE ptr_buffer , VO_U32 * ptr_size );

	virtual VO_U32 ProcessChunkDRM_II( VO_PBYTE ptr_buffer , VO_U32 uOffset,VO_U32 * ptr_size, VO_BOOL bChunkEnd );
	
	virtual VO_U32 EndChunkDRM_II();
/* .........CreateFileParser_II instead of
	virtual VO_U32 CreateFileParser( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item , VO_PBYTE pBuffer , VO_U32 size );
 .........CreateFileParser_II instead of*/
	virtual VO_U32 CreateFileParser_II( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item , VO_PBYTE *ppBuffer , VO_U32 *pSize, VO_BOOL bStreaming );

	virtual VO_U32 GetHeadData( VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE  Type );

	virtual VO_VOID TrackUnavailable( VO_U32 uTimes = 1 );
	virtual VO_VOID TrackUnavailable_II(VO_SOURCE2_TRACK_TYPE tracktype, VO_U32 nFlag, VO_U64 uTimeStamp );
	virtual VO_U32 ToProcessChunk(VO_HANDLE hIO, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U32 *readsize);
	
	virtual VO_U32 SendIOWarning(VO_HANDLE hHandle, VO_U32 errCode, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk);
	
	virtual VO_U32 SendIOInfo(VO_U32 nEventParam2, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk);
	virtual VO_U32 SendDownloadInfo(VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk);
	virtual VO_U32 SendChunkSample(VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, _SAMPLE * pSample);
	virtual VO_U32 SendDrmWarning(VO_U32 nEventParam2, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U32 nErrCode);
	virtual VO_U32 SendDrmError(VO_U32 nEventParam2, VO_U32 nErrCode);
	virtual VO_U32 SendCustomEvent(VO_U16 nOutputType, VO_PTR pData) { return VO_RET_SOURCE2_NOIMPLEMENT; }
	virtual VO_U32 ResetCustomerTagMap(){return VO_RET_SOURCE2_NOIMPLEMENT;}
	virtual VO_U32 SendLicenseCheckError(VO_U32 nEventParam2,VO_U32 nErrCode);
	virtual VO_U32 ToGetItem( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item,VO_U32 &nErrCode );
	virtual VO_U32  UpdateProcess(VO_BOOL isChunkBegin);
	virtual VO_U32 DownloadingForUpdateUrl(VO_HANDLE &h, VO_ADAPTIVESTREAMPARSER_CHUNK* pChunk, VO_U32 &uReadSize,VO_U64 &nDownloadTime);
	/* .........ToDealItem_III instead of
	virtual VO_U32 GetItem_II( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item, VO_BOOL bFlag );
	...........*/
#ifndef _USE_BUFFERCOMMANDER
	virtual VO_U32 GetItem_III( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item, VO_BOOL bFlag,VO_U32 &nErrCode );
#endif
	virtual VO_U32 GetItem_IV( VO_ADAPTIVESTREAMPARSER_CHUNK * ptr_item, VO_BOOL bFlag, VO_U32 &nErrCode );

	virtual VO_U32 ThreadEnding();

	/* .........ToDealItem_II instead of
	virtual VO_U32 ToDealItem(VO_ADAPTIVESTREAMPARSER_CHUNK *pChunk, VO_BOOL *pReset);
		...........*/
	virtual VO_U32 ToDealItem_II(VO_ADAPTIVESTREAMPARSER_CHUNK *pChunk, VO_BOOL *pReset);
	virtual VO_U32 SendIOWarning_II( VO_U32 nEventParam2, VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U32 nErrCode);
#ifndef _USE_BUFFERCOMMANDER
	virtual VO_U32 InfoChunk( VO_U64 uTimestamp, VO_BOOL isChunkBegin );
#endif
	virtual VO_U32 MarkOPFilter(VO_U16 nOutputType, VO_PTR pData);
	virtual VO_VOID DoDrop(VO_U32 &nFlag){ ;}
	
	virtual VO_U32  SendEventDownloadStartColletor(VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk);
	virtual VO_U32  SendEventDownloadEndColletor(VO_ADAPTIVESTREAMPARSER_CHUNK * pChunk, VO_U64 &uDownloadSize);
	static VO_U32 ProcessSampleDRM(VO_PTR pUseData, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_DRM2_DATATYPE eDataType, VO_DRM2_DATAINFO_TYPE eDataInfoType,VO_PTR pAdditionalInfo);
#ifdef _USE_BUFFERCOMMANDER
protected:
	virtual VO_U32 Downloading( VO_U64 &uDownloadTime, VO_U32 &uReadSize,VO_U32 &nErrCode);
	virtual VO_U32 SendChunkInfoEvent(VO_U32 uEvent, VO_U64 uDownloadTime, VO_U32 uDownloadSize, VO_U32 uRet);
#endif
protected:

	VO_BOOL m_is_stop;
	VO_SOURCE2_SAMPLECALLBACK m_sample_callback;

	voAdaptiveStreamingFileParser * m_pFileParser;

	VO_U32 m_audio_count;
	VO_U32 m_video_count;
	VO_U32 m_subtitle_count;

	ParseItemStatus m_audio_status;
	ParseItemStatus m_video_status;

	VO_ADAPTIVESTREAMPARSER_CHUNK * m_ptr_curitem;

	VO_U64 m_ref_timestamp;

	VO_SOURCE2_ADAPTIVESTREAMING_CHUNKTYPE m_chunkType;
	VO_SOURCE2_TRACK_TYPE m_trackType;

	VO_SOURCE2_TRACK_TYPE m_lastTrackType;

	VO_U64 m_LastStartTime;

	voFormatChecker m_FormatChecker;

	VO_BOOL m_bVideoChunkDropped;
	VO_BOOL m_bAudioChunkDropped;
	VO_BOOL m_bSubtitleChunkDropped;

	VO_BOOL	m_bFormatChange;

	VO_BOOL m_bChunkBegin;
	VO_BOOL m_bChunkEnd;
	VO_BOOL m_bChunkDiscontinue;

	VO_U64	m_uChunkID;	// currently, just HLS use it for DVR seek

	VO_BOOL	m_bChunkForceOutput;

	VO_BOOL m_bStreamingParser;
	VO_U32 m_nThreadID;
	VO_DATASOURCE_FILEPARSER_DRMCB m_dsdrmcb;

	VO_BOOL m_bCanSendFirstSample;
	VO_BOOL m_bLastAudioBAFlag;
	VO_BOOL	m_bLastVideoBAFlag;
	VO_BOOL	m_bSwitchAudioThread;

	VO_ADAPTIVESTREAMPARSER_CHUNK_INFO	m_chunk_info;
	VO_U32 m_nReadedSize;
#ifdef _USE_BUFFERCOMMANDER
protected:
	VO_VOID *m_pDownloadAgent;
#endif

};


#ifdef _VONAMESPACE
}
#endif
#endif
