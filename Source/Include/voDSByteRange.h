
//value indicate for DATA SOURCE TEAM 
#ifndef __VO_DS_BYTE_RANGE_H__
#define __VO_DS_BYTE_RANGE_H__

#include "voSource2.h"


//VO_PARSER_FLAG_STREAM_DASHINDEX					/*!<identify this is DASH index data */
//VO_PARSER_OT_SEGMENTINDEX_INFO					/*!<'sidx' information.Segment index box */
//VO_PID_ADAPTIVESTREAMING_SEGMENTINDEX_INFO		/*!< identify the data is segment index information, VODS_SEG_INDEX_INFO* */
//VO_SOURCE2_ADAPTIVESTREAMING_SEGMENTINDEX			/*!< identify the chunk type is segment index, for DASH 'sidx' */
//VO_SOURCE2_TT_SEGMENTINDEX						/*!< segment index information*/


enum VODS_SUBSEGMENT_TYPE
{
	VODS_SUBSEGMENT_TYPE_INDEX		= 1, /*!< identify this byterange directs to another index*/
	VODS_SUBSEGMENT_TYPE_SEGMENT	= 2, /*!< identify this byterange directs to a segment*/
		
	VODS_BYTERANGE_MAX = 255
};

typedef struct 
{	
	VODS_SUBSEGMENT_TYPE	uType;					/*!< identify the subsegment's type*/
	VO_U32					uSegIndex;				
	VO_U64					uTimeScale;
	VO_U64					uStartTime;				/*!< identify the subsegment's start time, no divide timescale */
	VO_U64 					uDuration;				/*!< identify the subsegment's duration, no divide timescale */
	VO_U64					uOffset;				/*!< identify the subsegment start positon, it is a relative value.*/
	VO_U64					uSize;					/*!< identify the subsegment's size*/
	VO_BOOL					bFirstKeyFrame;			/*!< identify the first sample is a key frame in subsegment */
}VODS_SUBSEGMENT_INFO;

typedef struct
{
	VO_U32	uStreamID;	
	VO_U32	uTrackID;
	VO_U32	uCount;			
	VODS_SUBSEGMENT_INFO	*pSegInfo;
}VODS_SEG_INDEX_INFO;

typedef struct
{
	VO_U64 uOffset;
	VO_U64 uSize;
}VODS_OFFSET;
/*!< one information about header and index offset*/
typedef struct
{
	VO_U32 uStreamID;
	VO_U32 uTrackID;
	VODS_OFFSET sHeader; /*!< identify "moov" box*/
}VODS_INITDATA_INFO;

#endif