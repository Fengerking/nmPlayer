#pragma once
#ifndef _SegmentBase_Tag_H
#define _SegmentBase_Tag_H

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
#define SEG_DASH_TIME_SCALE               "timescale"
#define SEG_DASH_INDEX_RANGE              "indexRange"
#define SEG_DASH_INDEX_RANGE_EXACT             "indexRangeExact"
#define SEG_DASH_OFFSET  "presentationTimeOffset"
#define SEG_DASH_INDEX_RANGE_EX  "indexRangeExact"
#define Seg_DASH_STREAMING_SOURCEURL  "sourceURL"
#define Seg_DASH_RANGE                   "range"




class SegmentBase_Tag:public Common_Tag
{
public:
	SegmentBase_Tag(void);
	~SegmentBase_Tag(void);
public:
	virtual VO_U32 Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode);
public:
		VO_S64 GetTimeScale(){return m_scale;}
		VO_CHAR *GetIniUrl(){ return m_ini_Url;}
		VO_CHAR *GetIni_range(){ return m_ini_Range;}
		VO_VOID Delete(void);
private:
	//Common_Tag * m_comtag;
	VO_S64  m_scale;
	/*VO_CHAR * m_ini_Url; */
	VO_CHAR m_ini_Url[1024];
	VO_CHAR m_ini_Range[256];
};
#ifdef _VONAMESPACE
}
#endif
#endif