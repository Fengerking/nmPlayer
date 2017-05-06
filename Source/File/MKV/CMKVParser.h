#pragma once

#include "MKVInfoStructure.h"
#include "CvoFileParser.h"
#include "CMKVAnalyze.h"
#include "LIST_ENTRY.h"
#include "CBaseStreamFileReader.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum DataParser_NotifyID
{
	NewClusterTimeCode = 1999,	//it means a new cluster time code has arrived. and the wParam is filled the pointer of the time in Int64 type
	NewCluser,					//it means a new cluster has arrived. Parameter should be null.
	NewBlock,					//it means a new block has arrived. and the wParam is filled the pointer of the tracknumber in vo_s64 type
	End,						//it means no data anymore
	NewBlockGroup,				//it means a new block group has arrived. Parameter should be null.
	NewReferenceBlock,			//it means a new reference block has arrived. Parameter should be null.
	SimpleBlock
};

enum TrackType
{
	Video_Type		= 0x01,
	Audio_Type		= 0x02,
	Complex_Type	=0x03,
	Logo_Type		=0x10,
	Subtitle_Type		=0x11,
	Buttons_Type		=0x12,
	Control_Type		=0x20,
};

class CMKVHeadParser : public CvoFileHeaderParser
{
public:

	CMKVHeadParser( CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp );
	virtual ~CMKVHeadParser();

	VO_BOOL			ReadFromFile();

	VO_VOID			SetFileSize( VO_U64 filesize );

	VO_U32			GetSegmentCount();
	PvoLIST_ENTRY	GetSeekHeadList( VO_U32 segmentindex );
	TracksTrackEntryStruct * GetBestAudioTrackInfo( VO_U32 segmentindex );
	TracksTrackEntryStruct * GetBestVideoTrackInfo( VO_U32 segmentindex );
	PvoLIST_ENTRY	GetCuesList( VO_U32 segmentindex );
	MKVSegmentStruct * GetSegmentInfo( VO_U32 segmentindex );

protected:
	VO_VOID			GetDuration();
	double			GetLastClusterTime( MKVSegmentStruct * ptr_seg );
	VO_BOOL			Read_uint( VO_S32 readsize , VO_S64 * pResult );
	VO_BOOL			ReadIDAndSize( IDAndSizeInfo * pInfo );
	VO_U64			GetRelativeTime( VO_S64 timescale );
	VO_BOOL			Read_vint( VO_S64 * pResult );
	VO_BOOL			Read_sint( VO_S32 readsize , VO_S64 * pResult );

private:

	MKVInfo m_Info;
	VO_U64  m_filesize;
	CMKVAnalyze m_analyze;
};

struct MKVFrame
{
	VO_U8 TrackNumber;
	VO_S64 RelativeTimeCode;
	VO_S64 TimeCode;
	VO_BOOL bIsKeyFrame;
	VO_U64 duration;
	VO_U64 framesize;

	VO_U64 filepos; //only used for onblock function:(

	CGFileChunk * pFileChunk;
};

typedef VO_BOOL (*pOnNotify)(VO_PTR pUser, VO_U8 StreamNumber , DataParser_NotifyID NotifyID , VO_PTR wParam , VO_PTR lParam );

class CMKVDataParser : 
	public CvoFileDataParser
{
public:
	CMKVDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CMKVDataParser();

	void SetOnNotifyCallback( CBaseStreamFileReader *pReader , pOnNotify callback )
	{
		m_pOnNotifyUser = (VO_PTR)pReader;
		m_OnNotify = callback;
	}

	VO_U32 MoveTo( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo );

	VO_VOID SetVideoID( VO_U32 VideoID ){ m_VideoID = VideoID; }
	VO_VOID SetSegmentInfo( MKVSegmentStruct * pSegInfo ){ m_pSegInfo = pSegInfo; };
	VO_VOID SetFlagForThumbnail(VO_BOOL beThumbnail){m_IsGetThumbNail = beThumbnail;}
	VO_U64  GetPositionWithCuesInfoByCount(const MKVSegmentStruct * pSegInfo );
	//Added by Aiven, to get the thumbnail buffer.It is dangerous.Please be careful.
	VO_BOOL GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos);	
public:
	virtual VO_VOID			Init(VO_PTR pParam);

	VO_VOID Reset();


	VO_VOID CopyParametersFromOtherParser( CMKVDataParser& other );
protected:
	virtual VO_BOOL			StepB();

	VO_BOOL ReadIDAndSize( IDAndSizeInfo * pInfo );
	VO_BOOL Read_uint( VO_S32 readsize , VO_S64 * pResult );
	VO_BOOL Read_sint( VO_S32 readsize , VO_S64 * pResult );

	VO_BOOL Read_vint( VO_S64 * pResult );
	VO_BOOL Read_svint( VO_S64 * pResult );

	VO_BOOL ReadBlock();

	VO_S64 GuessPositionByTime( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo );
	VO_U64 GetPositionByTravelCluster( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo );
	VO_U64 GetPositionWithCuesInfoByTime( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo );
	VO_U64 GetPositionPrecisely( VO_S64 llTimeStamp , VO_U64 posnear );
	VO_S64 FindCluster( VO_S64 llTimeStamp , VO_U64 startpos , VO_U64 endpos ); //the return value shows the position of a cluster,-1 means can not find the cluster,-2 means the timecode of the cluster is too large
	VO_BOOL CheckIsCluster( VO_U64 filepos , VO_U64 endpos ); //this function will not modify file pointer
	VO_U64 GetClusterFirstKeyFrameTimeCodeDirectly( VO_U64 filepos ); //this function will not modify file pointer
	VO_S64 GetFirstKeyFrameRelativeTime( VO_U64 filepos , VO_U64 blockgroupsize );

	VO_U64 GetClusterByTimeCodeDirectly( VO_U64 filepos );
private:
	VO_S64 m_CurrentTimeCode;

	VO_BOOL m_bIsInBlock;
	VO_U8 m_framecount;
	VO_U32 m_framesizearray[256];
	VO_U8 m_currentframeindex;
	VO_U64 m_currentblocksize;
	VO_S64 m_block_timecode;
	VO_S64 m_block_tracknumber;
	VO_BOOL m_bIsKeyFrame;

	VO_BOOL m_simpleblock_kf_way;
	VO_BOOL m_groupblock_kf_way;

	VO_U32 m_VideoID;

	pOnNotify m_OnNotify;
	VO_PTR m_pOnNotifyUser;

	MKVSegmentStruct * m_pSegInfo;
	VO_U64 m_nCurCluserEndPos;///<when data corruption,discard current cluser and parser from next
	VO_BOOL m_IsGetThumbNail;
};

#ifdef _VONAMESPACE
}
#endif
