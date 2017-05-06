#pragma once
#ifndef _Group_Tag_H
#define _Group_Tag_H

#include "voType.h"
#include "voSource2.h"
#include  "Common_Tag.h"
#include "voXMLLoad.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
#ifdef _IOS
	using namespace _VONAMESPACE;
#endif

#define GROUP_STREAMING_MIMETYPE  "mimeType"
#define GROUP_WIDTH "width"
#define GROUP_HEIGHT  "height"
#define GROUP_BIRATE "bandwidth"
#define GROUP_DURATION   "duration"
#define GROUP_LANG				"lang"
#define GROUP_ID  "id"
#define GROUP_MIN_Height "minHeight"
#define GROUP_MAX_Height "maxHeight"
#define GROUP_MAX_Width "maxWidth"
#define GROUP_MIN_Width "minWidth"
#define GROUP_MIN_FrameRate "minFrameRate"
#define GROUP_MAXFrameRate "maxFrameRate"
#define GROUP_BIT_SWITCH  "bitstreamSwitching"
#define GROUP_ALIGN "subsegmentAlignment"
#define GROUP_SAP "subsegmentStartsWtihSAP"
#define TAG_CODECS            "codecs"
#define Group_Duration   "duration"

	//add group_tag
#define TAG_SIMPLE_ADAPTIONSET  "AdaptationSet"
#define TAG_DASH_STREAMING_Group    "Group"
#define TAG_DASH_STREAMING_Group_G1   "Group"
#define TAG_DASH_STREAMING_MEDIA_G1	"g1:MPD"
#define TAG_SIMPLE_ADAPTIONSET_G1  "g1:AdaptationSet"


	class Group_Tag:public Common_Tag
	{
	public:
		Group_Tag(void);
		~Group_Tag(void);
	public:
		virtual VO_U32 Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode);
	public:
		VO_U32 GetID(){return m_uID;}
		VO_SOURCE_TRACKTYPE GetTrackType(){return m_track_type;}
		VO_CHAR *GetLang(){ return m_chLang;}
		VO_U32 GetCodecs(){ return m_codectype;}
		VO_U32 Get_Period_Duration(){return period_duration;}
		VO_S32 GetTrackCount(){return m_track_count;}
		VO_VOID Delete();
		VO_BOOL IS_TTML(){return m_is_ttml;}
	private:
		//Common_Tag * m_comtag;
		VO_U32	m_uID;
		VO_SOURCE_TRACKTYPE m_track_type;
		VO_CHAR m_chLang[20];
		VO_U32 m_codectype;
		VO_U32 period_duration;
		VO_S32 m_track_count;
		VO_BOOL m_is_ttml;
	};
#ifdef _VONAMESPACE
}
#endif

#endif