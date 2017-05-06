#ifndef __CMp4PushReader2_H__
#define __CMp4PushReader2_H__

#define MULITI_TRACK
#ifdef MULITI_TRACK

#include "CBaseReader.h"
#include "CMp4PushFileStream.h"
#include "isomrd.h"
#include "CMp4Track2.h"
#include "voParser.h"
#include "voLiveSource.h"
#include "voSource.h"

#include "CMp4PushReader.h"
#include "SegmentTrack.h"
#include "voDSByteRange.h"
#include "voDSDRM.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
//setparam
#define VOSTREAMREADER_PID_DRM_CALLBACK		0x000001
#define VOSTREAMREADER_PID_DRM_INFO				0x000002

// typedef struct
// {
// 	VO_BOOL		isUsePiff;
// 	VO_BYTE		systemID[16];
// 	VO_U32		dataSize;
// 	VO_BYTE		*data;
// }ProtectionHeader;
typedef struct
{
	VO_U32 uBuffersize;
	VO_U32 uUsedBuffersize;
	VO_BYTE *pDataBuffer;
}INITDATASTRUCTURE;
typedef struct
{
	VO_U32 uBuffersize;
	VO_BYTE *pDataBuffer;
}CENCDRMSAMPLEINFO;

class CMp4PushReader2 :
	public CBaseReader
{
public:
	CMp4PushReader2();
	virtual ~CMp4PushReader2();

public:
	virtual VO_U32  Open(PARSERPROC pProc, VO_VOID* pUserData,VO_TCHAR* pWorkingPath);
	virtual VO_U32  Process(VO_PARSER_INPUT_BUFFER* pBuffer);
	virtual VO_U32	ParserHead(VO_PBYTE pBuf,VO_U64 bufsize);
	virtual VO_U32	ParserData(VO_PBYTE pBuf,VO_U64 bufsize);
	virtual VO_U32  ParserSidx(VO_PBYTE pBuf,VO_U64 bufsize);
	virtual VO_BOOL ProcessDashIfInitdata(VO_PARSER_INPUT_BUFFER* pBuffer);
	virtual VO_U32  ParserDashIfInitalHead(VO_PBYTE pBuf,VO_U64 bufsize);
	virtual VO_U32  ParserDashIfSidx(VO_PBYTE pBuf,VO_U64 bufsize);
	virtual VO_U32	ParserDashIfHead(VO_PBYTE pBuf,VO_U64 bufsize);
	virtual VO_BOOL IsInitdataEnough(Reader* r,VO_U32 uBuflen);
	virtual VO_BOOL Initialdatastream(VO_PBYTE pBuf,VO_U32 uBuflen);
	virtual VO_BOOL InitialDashHeard(INITDATASTRUCTURE &initdatastructure,VO_PBYTE pBuf,VO_U32 uBuflen);
	virtual VO_BOOL UnInitialdatastream();
	        VO_BOOL ReAllocInitialDataBuffer();
#ifndef LIEYUN_DEBUG
	VO_BOOL TestDashIf();
#endif
	virtual VO_U32  FreeSegmentIndexInfo();
	virtual VO_U32  SetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32  GetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32	CloseHead();
	virtual VO_U32	CloseData();
	virtual VO_U32  Close();

protected:
	virtual VO_U32		InitTracks(VO_U32 nSourceOpenFlags);


	//2011/11/21,by leon ,fix 9158
	virtual CBaseTrack* GetTrackByIndex(VO_U32 nIndex);
	
protected:
	ReaderMSB*			m_pReader;
	CMp4PushFileStream*		m_pStream;
	Movie*				m_pMovie;

public:
	CvoDrmCallback *m_pPlayready_Drmcallback;
	VO_DATASOURCE_FILEPARSER_DRMCB *m_pCencDrmCallbackProc;//from controler for cenc drm
protected:
	PARSERPROC	m_pProc;
	VO_VOID*	m_pUserData;
	VO_U64		m_u64Starttime;
	VO_U64		m_u64PreChunkEndtime;//for as next chunk start time if m_u64Starttime < m_u64PreChunkEndtime
	VO_U32		m_u32StreamID;
	VO_U32		m_u32TimeScale;
	VO_LIVESRC_TRACK_INFOEX *m_trackInfo;
	VODS_SEG_INDEX_INFO *m_pSidxinfo;
	INITDATASTRUCTURE   m_InitDataStructure;
	VODS_INITDATA_INFO  m_InitHeadDataInfo;  //in dash-if mode,pass the head information to control
private:
	uint32 m_nDefaultSize_Video;
	uint32 m_nDefaultDuration_Video;
	uint32 m_nDefaultSize_Audio;
	uint32 m_nDefaultDuration_Audio;
	VO_U32 CreateTrackInfo(int id);
	VO_U32 CreateAudioTrackInfo();
	VO_VOID	IssueParseResult(VO_U32 nType, VO_U64 ullTrackId, VO_VOID* pValue, VO_VOID* pReserved = NULL);
	VO_U32 CreateStream(VO_U32 nFlag,VO_PBYTE buf, VO_U32 nSize);
	VO_PBYTE m_pBuf_x;
	VO_BOOL m_bBufNew;

	VO_BOOL m_bExit;
		
};


class TrackOp
{
public:
	TrackOp();
	virtual ~TrackOp();
	VO_U32 Init(ReaderMSB *pReader, CMp4PushFileStream* pStream,SegmentsBox *pSb,
				VO_U64 nStartTime, VO_U64 nDefaultDuration, VO_U64 nDefaultSize,VO_U64 nOffset);
	VO_U32 GetSample(VO_U32 nType, VO_SOURCE_SAMPLE *pSample);
	VO_BOOL IsEOS();
	void SetStreamDrmCallback(VO_DATASOURCE_FILEPARSER_DRMCB *pCB){m_pCencDrmCallbackProc = pCB;}
	MP4RRC GetNextMoofInfo();
	CENCDRMSAMPLEINFO *GetCencDrmInfo(VO_U32 index);
	VO_DATASOURCE_FILEPARSER_DRMCB *m_pCencDrmCallbackProc;//from controler for cenc drm
private:
	SegmentsBox *m_pSegmentsBox;
	VO_U64 m_nDefaultDuration;
	VO_U64 m_nDefaultSize;
	VO_U32 m_nCount;
	VO_U32 m_nCurSampleIndex;
	VO_U64 m_nStartTime;
	VO_U64 m_nDataOffset;
	unsigned char *m_pBuffer;
	VO_U32 m_nCurSampleSize;
private:
	ReaderMSB*			m_pReader;
	CMp4PushFileStream*		m_pStream;
	CENCDRMSAMPLEINFO    m_CencDrmSampleInfo;
	
};
#ifdef _VONAMESPACE
}
#endif

#endif
#endif	//__CMp4Reader_2_H__