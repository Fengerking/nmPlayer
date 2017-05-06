

#include "CBaseStreamFileReader.h"
#include "CMKVParser.h"
#include "CMKVAudioTrack.h"
#include "CMKVVideoTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define MAX_CACHE_FRAME_COUNT 20

class CMKVFileReader :
	public CBaseStreamFileReader
{
public:
	CMKVFileReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB );
	virtual ~CMKVFileReader(void);

	VO_U32 Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	VO_U32 Close();

	VO_U32 InitTracks(VO_U32 nSourceOpenFlags);

	VO_U8 OnPacket( MKVFrame* pSample );
	VO_BOOL Notify( VO_U8 StreamNumber , DataParser_NotifyID NotifyID , VO_PTR wParam , VO_PTR lParam );

	VO_VOID OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser);

	VO_U32 MoveTo(VO_S64 llTimeStamp);

	VO_VOID Flush();
//	static	VO_U32  GetMaxFrameCnt(){return m_MaxFrameCnt;}
	
	VO_BOOL GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos);
	virtual VO_U32 SetSelectTrack(VO_U32 nIndex, VO_BOOL bInUsed, VO_U64 llTs);
	
protected:
	VO_VOID ParseMPEG4SequenceHeader( MKVFrame * pSample , TracksTrackEntryStruct * pVideoTrackEntry );
	VO_BYTE * memsub( VO_BYTE * ptr_buffer , VO_S32 buffer_size , VO_BYTE * ptr_sub , VO_S32 sub_size );

	VO_U32 SelectStream(VO_U32 StreamNumber, VO_U64 llTs);
	VO_U32 InitAudioTrack(CMKVTrack** pTrack, TracksTrackEntryStruct * pTrackEntry);
	VO_U32 InitVideoTrack(CMKVTrack** pTrack, TracksTrackEntryStruct * pTrackEntry, VO_U32 nSourceOpenFlags);
	VO_U32 MultiTrackMoveTo(CMKVTrack* pTrack, VO_S64 llTimeStamp);
	VO_U32 InitHeadData();
private:
	CMKVHeadParser m_HeadAnaylze;
	CMKVDataParser m_DataParser;

	TracksTrackEntryStruct * m_pVideoTrackEntry;
	TracksTrackEntryStruct * m_pAudioTrackEntry;
	MKVSegmentStruct       * m_pSegmentInfo;

	CMKVVideoTrack * m_pVideoTrack;
	CMKVAudioTrack * m_pAudioTrack;
	VO_U32 m_Duration;
	VO_BOOL m_is_init;
	VO_U32 m_MaxFrameCnt;
};

#ifdef _VONAMESPACE
}
#endif
