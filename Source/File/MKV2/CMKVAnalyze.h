#pragma once

#include "CGFileChunk.h"
//#include "CvoBaseMemOpr.h"
#include "MKVInfoStructure.h"

#define PROCESS_ANALYZE_MODE_FLAG		0x01
#define PROCESS_PARSER_MODE_FLAG		0x02
#define PROCESS_THUMBNAIL_MODE_FLAG	0x04

#define PROCESS_ALL_INFO_FLAG			(0xffff<<2)
#define PROCESS_CLUSTER_FLAG			(0x0001<<2)
#define PROCESS_BLOCK_FLAG				(0x0002<<2)
#define PROCESS_BLOCKGROUP_FLAG		(0x0004<<2)
#define PROCESS_CUE_FLAG				(0x0008<<2)
#define PROCESS_KEYFRAME_FLAG			(0x0010<<2)

#define PROCESS_ALL_TRACK_FLAG			(0xffff<<6)
#define PROCESS_VIDEOTRACK_FLAG		(0x0001<<6)
#define PROCESS_AUDIOTRACK_FLAG		(0x0002<<6)
#define PROCESS_COMPLEXTRACK_FLAG		(0x0004<<6)
#define PROCESS_LOGOTRACK_FLAG			(0x0008<<6)
#define PROCESS_SUBTITLETRACK_FLAG		(0x0010<<6)
#define PROCESS_BUTTONSTACK_FLAG		(0x0020<<6)
#define PROCESS_CONTROLTRACK_FLAG		(0x0040<<6)

#define PROCESS_CLUSTER_TIMES			0x00000001

#define PROCESS_ANALYZE_MODE 	PROCESS_ANALYZE_MODE_FLAG|PROCESS_ALL_INFO_FLAG|PROCESS_ALL_TRACK_FLAG
#define PROCESS_PARSE_MODE 		PROCESS_PARSER_MODE_FLAG|PROCESS_ALL_INFO_FLAG|PROCESS_ALL_TRACK_FLAG

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

struct IDAndSizeInfo 
{
	VO_PBYTE	pID;
	VO_S32		s_ID;
	VO_S64		size;
};

struct BlockGroupInfo 
{
	VO_S64 BlockDuration;
	VO_S64 ReferenceTimecodec;
};


class SeekHeadElementStructComp
{
public:
	VO_S64 operator()( SeekHeadElementStruct * p1 , SeekHeadElementStruct * p2 )
	{
		return p1->SeekPos - p2->SeekPos;
	}
};

class CuesCuePointStructComp
{
public:
	VO_S64 operator()( CuesCuePointStruct * p1 , CuesCuePointStruct * p2 )
	{
		return p1->CueTime - p2->CueTime;
	}
};


typedef VO_BOOL (*pOnNotify)(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam );

struct NotifyStruct
{
	pOnNotify		OnNotify;
	VO_PTR 		pOnNotifyUser;
};

enum NotifyElement
{
	TRACK		= 0,
	SEGMENT,
	ELEMENT_MAX
};

class NotifyCenter
{
public:
	NotifyCenter();
	~NotifyCenter();
	VO_VOID Reset();
	VO_VOID EventNotify(NotifyElement element, VO_PTR wParam , VO_PTR lParam);
	VO_BOOL	RegistNotify(NotifyElement ElementType,VO_PTR pOnNotifyUser, pOnNotify OnNotify);

	VO_BOOL IsNotifyRegist(NotifyElement ElementType);
	
	VO_BOOL CancelNotify(NotifyElement ElementType);
	
private:
	NotifyStruct	m_pNotify[ELEMENT_MAX];

};

#define MAX_TRACK_NUMBER	5
struct SampleNotify
{
	VO_U32		nTrackNumber;
	pOnNotify 	OnNotify;
	pOnNotify		CanGenerateMoreSample;		
	VO_PTR 		pOnNotifyUser;
};

class SampleNotifyCenter
{
public:
	SampleNotifyCenter();
	~SampleNotifyCenter();
	VO_VOID Reset();
	VO_BOOL Notify(VO_U32 nTrackNumber, VO_PTR wParam , VO_PTR lParam);
	VO_BOOL	RegistSampleNotify(VO_U32 nTrackNumber,VO_PTR pOnNotifyUser, pOnNotify OnNotify);

	VO_BOOL IsTrackRegist(VO_U32 nTrackNumber);
	
	VO_BOOL CancelSampleNotify(VO_U32 nTrackNumber);
	VO_BOOL CancelSampleNotify(pOnNotify notify);
	VO_BOOL CancelSampleNotify();
private:
	SampleNotify	m_pSampleNotify[MAX_TRACK_NUMBER];
};

enum PARSER_STATUS{
	IDLE_STATE	= 0,
	READY_STATE,	
	RUNNING_STATE,
	PAUSE_STATE,
	MAX_STATE
};

class Parser
{
public:
	Parser();	
	Parser(Parser& parser);
	~Parser();

	virtual VO_BOOL CopyParser(Parser* pParser);	
	virtual VO_VOID SetMode(VO_U64 mode);
	virtual VO_BOOL SetFileIO( VO_PTR pFileIO);	
	virtual VO_PTR GetFileIO();
	virtual VO_VOID Flush();
	
	virtual VO_VOID Start();
	virtual VO_VOID Stop();
	virtual VO_U32 DataParse();
	virtual VO_U32 MoveTo( VO_S64 llTimeStamp);	
	virtual VO_U32 GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime);


	
	VO_VOID AddSampleNotifyCallback(VO_U32 nTrackType,VO_PTR pOnNotifyUser, pOnNotify OnNotify);
	VO_VOID RemoveSampleNotifyCallback(VO_U32 nTrackType);

protected:
	virtual PARSER_STATUS GetStatus();
	virtual VO_BOOL SetStatus(PARSER_STATUS nStatus);

protected:
	SampleNotifyCenter* m_pSampleNotifyCenter;
	
	VO_U64        	m_nAnalyzeMode;	
	VO_BOOL		m_bParseEnd;
	VO_S64		m_nCurrectClusterTime;

private:	
	PARSER_STATUS m_nStatus;
};


class HeadParser
{
public:
	HeadParser();
	~HeadParser();
	
	//analyze the headdata
	virtual VO_VOID AnalyzeFile( VO_S64 filesize);
	
	VO_BOOL RegistNotify(NotifyElement ElementType,VO_PTR pOnNotifyUser, pOnNotify OnNotify);
	VO_BOOL CancelNotify(NotifyElement ElementType);

protected:
	NotifyCenter* m_pNotifyCenter;
};

enum ERROR_TYPE
{
	ERROR_NONE		= 0,
	ERROR_SKIP,
	ERROR_STOP,
	ERROR_STATUS_WRONG,	
	ERROR_EOS,
	ERROR_MAX
};

class CMKVAnalyze
	: public HeadParser
	, public Parser
{
typedef VO_U32 ( CMKVAnalyze:: * pProcessSection )( IDAndSizeInfo * pInfo , VO_VOID * pParam );

public:

	CMKVAnalyze( CGFileChunk* pFileChunk);
	CMKVAnalyze( CMKVAnalyze& pCMKVAnalyze );
	
	~CMKVAnalyze();

	
	//analyze the headdata
	virtual VO_VOID AnalyzeFile( VO_S64 filesize);
	VO_VOID EventNotify(IDAndSizeInfo * pInfo);
	
	//parse the file to generate the sample list.
	virtual VO_BOOL GetSampleData(VO_PTR pFrame, VO_PBYTE pFrameData, VO_U32* pFrameLength);	
	virtual VO_BOOL SetFileIO( VO_PTR pFileIO);	
	virtual VO_PTR GetFileIO();
	virtual VO_U32 DataParse();	
	virtual VO_VOID Start();
	virtual VO_U32 MoveTo( VO_S64 llTimeStamp);
	virtual VO_VOID Flush();
	virtual VO_U32 GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime);
protected:
	VO_U32 ParseCluster();
	MKVSegmentStruct * CMKVAnalyze::GetSegmentInfo( VO_U32 segmentindex );
	double	GetLastClusterTime();
	VO_S64 GuessPositionByTime( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo );
	VO_U64 GetPositionByTravelCluster( VO_S64 llTimeStamp , const MKVSegmentStruct * pSegInfo );
	VO_S64 FindCluster( VO_S64 llTimeStamp , VO_U64 startpos , VO_U64 endpos, VO_BOOL direction = VO_FALSE);
	VO_BOOL CheckIsCluster( VO_U64 filepos , VO_U64 endpos );
	VO_U64 GetClusterFirstKeyFrameTimeCodeDirectly( VO_U64 filepos );
	VO_S64 GetFirstKeyFrameRelativeTime( VO_U64 filepos , VO_U64 blockgroupsize );
	VO_U64 GetPositionWithCuesInfoByCount();
	VO_U64 GetPositionWithCuesInfoByTime( VO_S64 llTimeStamp , MKVSegmentStruct * pSegInfo );
	VO_U64 GetPositionPrecisely( VO_S64 llTimeStamp , VO_U64 posnear );
	

protected:
	VO_BOOL ReadIDAndSize( IDAndSizeInfo * pInfo );
	VO_U32 ReadSection( pProcessSection ProcessSection , VO_S64 SectionSize , VO_VOID * pParam );

	//if processed this section please return VO_TRUE, if not please return VO_FALSE
	VO_U32 ProcessMainSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_U32 ProcessSegmentSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_U32 ProcessSeekHeadSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ProcessSeekHeadSeekSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_U32 ProcessSegmentInfoSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_U32 ProcessTracksSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ProcessTrackEntrySection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ProcessTrackEntryVideoSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ProcessTrackEntryAudioSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_U32 ProcessCuesSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ProcessCuePointSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ProcessCuePointTrackPositionsSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );

	VO_U32 ProcessClusterSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 SkipClusterSection(IDAndSizeInfo * pInfo , VO_VOID * pParam);
	VO_U32 ParseClusterSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ProcessBlockSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 SkipBlockSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ParseBlockSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ProcessBlockGroupSection( IDAndSizeInfo * pInfo , VO_VOID * pParam );


	VO_U32 ProcessContentEncodings( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ProcessContentEncoding( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	VO_U32 ProcessContentCompression( IDAndSizeInfo * pInfo , VO_VOID * pParam );
	//
	TracksTrackEntryStruct* GetBestTrackInfo( VO_U32 tracktype );
	TracksTrackEntryStruct* GetTrackInfoByNumber( VO_U32 TrackNumber );
/*
	VO_U32 InsertTheSampleToList(list_T<SampleElementStruct *>* pSampleElement,VO_PTR pParam,SampleListStruct** pSampleList);
	VO_U32 InsertTheSampleToList(BlockGroupElementStruct* pBlockGroupElement,VO_PTR pParam, SampleListStruct** pSampleList);
	VO_U32 PostTheSampleInList(SampleListStruct*  pSampleList);
	VO_U32 PostAllSampleInList(MKVSegmentStruct * ptr_seg);
*/

	VO_U32 ReconstructSampleElement(list_T<SampleElementStruct *>* pSampleElement);
	VO_U32 ReconstructBlockGroupElement(BlockGroupElementStruct* pBlockGroupElement);

	//Addedb by Aiven, dump the MKV HeadData for debug.
	VO_VOID PrintMKVSegmentStruct(MKVSegmentStruct* info);

	VO_BOOL Read_vint( VO_S64 * pResult );
	VO_BOOL Read_svint( VO_S64 * pResult );	
	VO_BOOL Read_uint( VO_S32 readsize , VO_S64 * pResult );
	VO_BOOL Read_sint( VO_S32 readsize , VO_S64 * pResult );
	
	inline static void InitializeTracksTrackEntryStruct( TracksTrackEntryStruct * pObj )
	{
		pObj->IsDefault = VO_TRUE;
		pObj->IsEnabled = VO_TRUE;
		pObj->IsForced = VO_FALSE;
		pObj->TrackTimecodeScale = 1.0;
		strcpy( (char*)(pObj->str_Language) , "eng");

		pObj->SampleFreq = 8000.0;
		pObj->Channels = 1;

		pObj->ContentCompAlgo = -1;
	}
/*
	inline void InitializeCuesCuePointStruct( CuesCuePointStruct * pObj )
	{
		InitializeListHead( &( pObj->TrackPositionsHeader ) );
	}
*/
	inline void UnInitializeCuesCuePointStruct( list_T<CuesCuePointStruct *>* pObj )
	{
		list_T<CuesCuePointStruct *>::iterator iter;
		for(iter=pObj->begin(); iter!=pObj->end(); ++iter)
		{
			CuesCuePointStruct * CuesCue = (CuesCuePointStruct *)(*iter);
			CuesCue->TrackPositionsHeader.clear();
		}
		delete pObj;
	}
/*	
	inline void InitializeSampleListPointStruct( SampleListStruct * pObj )
	{
		InitializeListHead( &( pObj->SampleListHeader ) );
	}
*/	

	inline void InitializeMKVSegmentStruct( MKVSegmentStruct * pObj )
	{		
		pObj->Duration = (double)0;
		pObj->TimecodeScale = (VO_S64)1000000;
		pObj->ClusterPos = (VO_S64)0;
		pObj->SegmentEndPos = (VO_S64)0;
		pObj->SegmentBeginPos = (VO_S64)0;
	}

	inline void UnInitializeMKVSegmentStruct( MKVSegmentStruct * pObj )
	{
		pObj->SeekHeadElementEntryListHeader.clear();
		pObj->TracksTrackEntryListHeader.clear();
		UnInitializeCuesCuePointStruct(&pObj->CuesCuePointEntryListHeader);
	}

	inline void UnInitializeMKVInfo( list_T<MKVSegmentStruct *>* pObj )
	{
		list_T<MKVSegmentStruct *>::iterator iter;
		for(iter=pObj->begin(); iter!=pObj->end(); ++iter)
		{
			MKVSegmentStruct * pMKVSegment = (MKVSegmentStruct *)(*iter);
			UnInitializeMKVSegmentStruct(pMKVSegment);
		}
		pObj->clear();
	}
private:

	inline VO_VOID BigEndianLittleEndianExchange( VO_VOID * pData , VO_S32 DataSize )
	{
		VO_PBYTE pBuffer = (VO_PBYTE)pData;

		for( VO_S32 i = 0 ; i < DataSize / 2 ; i++ )
		{
			VO_BYTE temp = pBuffer[i];
			pBuffer[i] = pBuffer[DataSize-i-1];
			pBuffer[DataSize-i-1] = temp;
		}
	}


	CGFileChunk * m_pFileChunk;
	list_T<MKVSegmentStruct *> 	m_Info;
};
