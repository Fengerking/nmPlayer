#pragma once
#include "voYYDef_TS.h"
#include "CMTVReader.h"
#include "CBaseStreamFileTrack.h"
#include "voMTVBase.h"
#include "voCSemaphore.h"

//#define _STEP_EVENT_

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif



#define   TS_TRACK_MODE_NORMAL      0
#define   TS_TRACK_MODE_THUMBNAIL   1
#define   TS_MAX_I_FRAME_COUNT      1024
#define   TS_INVALID_I_FRAME_INDEX  0xffffffff

typedef enum
{
	TS_STREAM_NUMBERAUDIO = 0x00,
	TS_STREAM_NUMBERVIDEO = 0x01,
	TS_STREAM_NUMBERSUBTITLE = 0x02,
	TS_STREAM_NUMBERMAX
}TS_STREAM_NUMBER;
class CStream;

class CTsTrack : public CBaseStreamFileTrack
{
public:
	CTsTrack(VO_TRACKTYPE nType, VO_U32 btStreamNum, VO_U32 dwDuration, CMTVReader* pReader, VO_MEM_OPERATOR* pMemOp, CStream* pStream);
	virtual ~CTsTrack(void);

public:
	virtual VO_U32		Init(VO_U32 nSourceOpenFlags);

	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32		GetCodec(VO_U32* pCodec);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);

	virtual VO_U32		AddFrame(VO_MTV_FRAME_BUFFER* pBuf);
	virtual VO_BOOL		CalculateTimeStamp(PBaseStreamMediaSampleIndexEntry pNewEntry);
	virtual VO_BOOL		PushTmpSampleToStreamFile(PBaseStreamMediaSampleIndexEntry pNewEntry);
	virtual VO_BOOL     InitGlobalBuffer();
    VO_U32              RemoveSampleUnUsed();

	VO_VOID				Wait();
	VO_U8				GetStreamNumByType(VO_TRACKTYPE nStreamType);
	VO_U32				GetStreamPID(){return m_StramPID;}
    VO_VOID             SetBufferState(VO_U32  ulBufferState);
	virtual     VO_VOID		Flush();
    VO_U32		        GetSampleUnUsed(VO_SOURCE_SAMPLE* pSample);
    VO_VOID             SetTrackLanguage(VO_CHAR*   planguage);
    VO_VOID             CacheFrameForThumbnail(VO_MTV_FRAME_BUFFER* pBuf);
	VO_VOID             TrackGenerateThumbnail();
	virtual VO_U32 	    GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbnail);
	VO_VOID             DeleteAllThumbnail();



protected:
	//GetSample and SetPos implement of key frame mode!!
	virtual VO_U32		GetSampleK(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPosK(VO_S64* pPos);

	//GetSample and SetPos implement of normal mode!!
	virtual VO_U32		GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPosN(VO_S64* pPos);

	virtual VO_BOOL		TrackGenerateIndex();

	inline VO_BOOL TrackGenerateIndexC(CvoFileDataParser* pDataParser);
	inline VO_U8 IsCannotGenerateIndex(CvoFileDataParser* pDataParser);

	VO_VOID   InitAllFileDump();
	VO_VOID   CreateDumpByStreamId(VO_U32 ulPID);
	VO_VOID   DumpFrame(VO_SOURCE_SAMPLE* pSample);
	VO_VOID   CloseAllFileDump();
private:
	CMTVReader*	m_pReader;
	CStream*	m_pStream;
	VO_U16		m_nNoTimeStampCount;
//	PBaseStreamMediaSampleIndexEntry m_pLastValidEntry;
	VO_U64		m_nLastTimeStamp;
	VO_U32		m_StramPID;
	PBaseStreamMediaSampleIndexEntry	m_TmpStampEntryListHead;
	PBaseStreamMediaSampleIndexEntry	m_TmpStampEntryListTail;
	VO_S64								m_ValidTimeStamp;
	VO_U32								m_wUnvalidTimeStampEntry;
    VO_U32                              m_ulBufferState;
    VO_U32                              m_ulTrackMode;
    VO_U32                              m_ulCurIFrameCount;
    VO_U32                              m_ulIFrameCountPerTime;
    VO_U32                              m_ulIFrameCurIndex;

    VO_SOURCE_THUMBNAILINFO*           m_pFrameArrayForThumbnail[TS_MAX_I_FRAME_COUNT];
    

#ifdef _DUMP_FRAME
	FILE*          m_pFileData;
	FILE*          m_pFileTimeStamp_Size;
#endif 

private:

#ifdef _STEP_EVENT_
	voCSemaphore	m_StepEvent;
#endif
};

#ifdef _VONAMESPACE
}
#endif