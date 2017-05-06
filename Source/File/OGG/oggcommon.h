
#ifndef __VO_OGGCOMMON_H_

#define __VO_OGGCOMMON_H_

#include "OggFileDataStruct.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum OGG_MEDIA_TYPE
{
	MT_AUDIO,
	MT_VIDEO,
	MT_UNKNOWN
};

enum OGG_MEDIA_DETAIL
{
	MD_VORBIS,
	MD_UNKNOWN
};

struct track_index
{
	VO_U64 page_pos;
	VO_U64 granule_pos;

	track_index * ptr_next_index;
};

struct track_info
{
	VO_U32 serial_number;
	OGG_MEDIA_TYPE media_type;
	OGG_MEDIA_DETAIL media_detail;

	VO_U32 readpackets;

	union
	{
		//this is used when audio-vorbis
		struct  
		{
			OggIdentificationHeader info;
		};
		//
	};

	VO_PBYTE ptr_header_data;
	VO_U32   headersize;

	track_info * ptr_next_track;

	track_index * ptr_index_head;
	track_index * ptr_index_tail;
};

#ifdef _VONAMESPACE
}
#endif


#endif