#pragma once
#ifndef _SegInfo_Tag_H
#define _SegInfo_Tag_H

#include "voType.h"
#include "voSource2.h"
#include "tinyxml.h"
#include  "Common_Tag.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
#ifdef _IOS
	using namespace _VONAMESPACE;
#endif


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
		VO_VOID Delete();


private:
	Common_Tag * m_comtag;
	VO_S64  m_scale;
	VO_S64  m_duration;

};
#ifdef _VONAMESPACE
}
#endif

#endif