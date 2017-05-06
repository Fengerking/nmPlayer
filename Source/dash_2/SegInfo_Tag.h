#pragma once
#ifndef _SegInfo_Tag_H
#define _SegInfo_Tag_H

#include "voType.h"
#include "voSource2.h"
#include "tinyxml.h"
#include  "Common_Tag.h"

#define TAG_SEG_TIME_SCALE               "timescale"
#define TAG_SEG_TIME_Duration               "duration"


class SegInfo_Tag:public Common_Tag
{
public:
	SegInfo_Tag(void);
	~SegInfo_Tag(void);
public:
	virtual VO_U32 Init(TiXmlNode* pNode);
public:
		VO_S64 GetTimeScale(){return m_scale;}
		VO_S64 GetDuration(){ return m_duration;}


private:
	Common_Tag * m_comtag;
	VO_S64  m_scale;
	VO_S64  m_duration;



	

};

#endif