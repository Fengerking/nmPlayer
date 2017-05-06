#pragma once

#include "LIST_ENTRY.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

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

	voLIST_ENTRY	List;
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

	voLIST_ENTRY	List;

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

	voLIST_ENTRY	List;
};

struct CuesCuePointStruct
{
	VO_S64			CueTime;
	voLIST_ENTRY	TrackPositionsHeader;

	voLIST_ENTRY	List;
};

struct MKVSegmentStruct
{
	double			Duration;
	VO_S64			TimecodeScale;

	voLIST_ENTRY	SeekHeadElementEntryListHeader;
	voLIST_ENTRY	TracksTrackEntryListHeader;
	voLIST_ENTRY	CuesCuePointEntryListHeader;

	VO_S64			ClusterPos;
	VO_S64			SegmentEndPos;	
	VO_S64			SupposeSegmentEndPos;	//added by Aiven
	VO_S64			SegmentBeginPos;

	voLIST_ENTRY	List;
};

struct MKVInfo
{
	voLIST_ENTRY	MKVSegmentListHeader;
};

#ifdef _VONAMESPACE
}
#endif
