#pragma once
#ifndef _SegmentTemplate_Tag_H
#define _SegmentTemplate_Tag_H

#include "voType.h"
#include "voSource2.h"
#include "tinyxml.h"
#include  "Common_Tag.h"

#define TAG_DASH_MEDIABASEURL             "media"
#define TAG_DASH_BASEURL_3                 "initialization"
#define TAG_DASH_BASEURL_4                 "initialisation"
#define TAG_DASH_INDEX                "index"
#define TAG_DASH_TIME_SCALE               "timescale"
#define TAG_DASH_TIME_Duration               "duration"
#define TAG_START_NUMBER              "startNumber"
#define TAG_BIT_SWITCH             "bitstreamSwitching"


class SegmentTemplate_Tag:public Common_Tag
{
public:
	SegmentTemplate_Tag(void);
	~SegmentTemplate_Tag(void);
public:
	virtual VO_U32 Init(TiXmlNode* pNode);
public:
		VO_CHAR *GetMediaUrl(){ return m_media_url;}
		VO_CHAR * GetIniaUrl(){ return m_inia_url;}
		VO_CHAR * GetIndex(){ return m_index_url;}
		VO_S64 GetTimeScale(){return m_scale;}
		VO_S64 GetDuration(){return m_duration;}
		int GetStart_number(){return m_start_number;}

private:
		Common_Tag * m_comtag;
		VO_CHAR m_media_url[256];
		VO_CHAR m_inia_url[256];
		VO_CHAR m_index_url[256];
		VO_S64  m_scale;
		VO_S64  m_duration;
		VO_S64  m_start_number;


};

#endif