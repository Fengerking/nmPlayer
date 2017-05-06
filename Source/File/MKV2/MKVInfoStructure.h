#pragma once

#include "list_T.h"

enum Section
{
	SegmentInfo,
	SeekHead,
	Tracks,
	Cluster,
	Cues
};

struct SeekHeadElementStruct
{
	Section			SeekID;
	VO_S64			SeekPos;
};

struct SampleElementStruct
{
	VO_U8 TrackNumber;
	VO_S64 RelativeTimeCode;
	VO_S64 TimeCode;
	VO_BOOL bIsKeyFrame;
	VO_U64 duration;
	VO_U64 framesize;
	
	VO_U64 filepos;
};

struct BlockGroupElementStruct
{
	VO_S64			BlockDuration;
	VO_S64			ReferenceTimecodec ;
	list_T<SampleElementStruct *>		SampleElementList;
};


struct TracksTrackEntryStruct
{
	VO_U32			TrackNumber;
	VO_U32			TrackType;				//if it is 1 use ( PixelWidth and PixelHeight ), if it is 2 use ( SampleFreq , Channels , BitDepth )
	VO_BOOL			IsEnabled;
	VO_BOOL			IsDefault;
	VO_BOOL			IsForced;
	VO_S64			DefaultDuration;
	double			TrackTimecodeScale;
	VO_S8			str_Language[10];
	VO_S8			str_CodecID[50];
	VO_S8*			pCodecPrivate;
	VO_U32			CodecPrivateSize;

	union
	{
		struct  
		{
			VO_U32	PixelWidth;
			VO_U32	PixelHeight;
		};

		struct
		{
			double	SampleFreq;
			VO_U32	Channels;
			VO_U32	BitDepth;
		};
	};

	VO_S32			ContentCompAlgo;
	VO_PBYTE		ptr_ContentCompSettings;
	VO_S32			ContentCompSettingsSize;
};

struct CuesCuePointTrackPositionsStruct
{
	VO_U32			CueTrack;
	VO_S64			CueClusterPosition;
	VO_U32			CueBlockNumber;
};

struct CuesCuePointStruct
{
	VO_S64			CueTime;
	list_T<CuesCuePointTrackPositionsStruct *>	TrackPositionsHeader;
};

struct MKVSegmentStruct
{
	VO_S64			TimecodeScale;
	double			Duration;

	VO_S64			ClusterPos;
	VO_S64			SegmentEndPos;
	VO_S64			SegmentBeginPos;

	list_T<SeekHeadElementStruct *>	SeekHeadElementEntryListHeader;
	list_T<TracksTrackEntryStruct *>	TracksTrackEntryListHeader;
	list_T<CuesCuePointStruct *>		CuesCuePointEntryListHeader;
	

};

struct MKVInfo
{
	list_T<MKVSegmentStruct *>	MKVSegmentListHeader;
};
