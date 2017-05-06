

#include "CBaseReader.h"
//#include "CMKVParser.h"
#include "CMKVAnalyze.h"
#include "CMKVAudioTrack.h"
#include "CMKVVideoTrack.h"

#define MAX_CACHE_FRAME_COUNT 20

class CTrackCenter{
public:
	CTrackCenter();
	~CTrackCenter();
	
	VO_BOOL RegistNewTrack(CBaseTrack* pTrack);
	VO_VOID UnRegistTrack(CBaseTrack* pTrack);

	VO_VOID	Flush();
	VO_VOID	InitTracks();
	VO_VOID	UnInitTracks();

	VO_U32 GetTrackNumbers();
	CBaseTrack* GetTrackbyTrackNumber(VO_U32 TrackNumber);
	CBaseTrack* GetTrackbyIndex(VO_U32 Index);
protected:
private:
	list_T<CBaseTrack*>	m_nTrackList;
};


class CMKVFileReader:CBaseReader
{
public:
	CMKVFileReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB );
	virtual ~CMKVFileReader(void);

	VO_U32 Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	VO_U32 Close();

	VO_U32 InitTracks(TracksTrackEntryStruct* pAudioTrackEntry, TracksTrackEntryStruct* pVideoTrackEntry);
/*
	VO_U8 OnPacket( MKVFrame* pSample );
	VO_BOOL Notify( VO_U8 StreamNumber , DataParser_NotifyID NotifyID , VO_PTR wParam , VO_PTR lParam );
*/
//	VO_VOID OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser);

	VO_U32 MoveTo(VO_S64 llTimeStamp);

	VO_VOID Flush();
	virtual CBaseTrack*	GetTrackByIndex(VO_U32 nIndex);
//	static	VO_U32  GetMaxFrameCnt(){return m_MaxFrameCnt;}
	TracksTrackEntryStruct * GetBestTrackInfo(TrackType tracktype,list_T<TracksTrackEntryStruct *>* pEntry );
	VO_U32 GetInfo(VO_SOURCE_INFO* pSourceInfo);
	static VO_BOOL NeedCreateNewDataParser(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam );

protected:
	VO_VOID InitalizeTracks();

	VO_VOID ParseMPEG4SequenceHeader( MKVFrame * pSample , TracksTrackEntryStruct * pVideoTrackEntry );
	VO_BYTE * memsub( VO_BYTE * ptr_buffer , VO_S32 buffer_size , VO_BYTE * ptr_sub , VO_S32 sub_size );
	static VO_BOOL SegmentComplete(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam );
	static VO_BOOL TracksComplete(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam );
private:
	VO_U32 m_nSourceOpenFlags;
	MKVSegmentStruct*		m_pSegmentInfo;
	CMKVAnalyze*			m_pAnalyze;
	CTrackCenter*			m_pCTrackCenter;
//	CMKVHeadParser m_HeadAnaylze;
//	CMKVDataParser m_DataParser;

//	TracksTrackEntryStruct * m_pVideoTrackEntry;
//	TracksTrackEntryStruct * m_pAudioTrackEntry;

//	CMKVVideoTrack * m_pVideoTrack;
//	CMKVAudioTrack * m_pAudioTrack;
//	VO_U32 m_Duration;
//	VO_BOOL m_is_init;
//	static VO_U32 m_MaxFrameCnt;
};
