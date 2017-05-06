#pragma once
#ifndef _Group_Tag_H
#define _Group_Tag_H

#include "voType.h"
#include "voSource2.h"
#include "tinyxml.h"
#include  "Common_Tag.h"

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


class Group_Tag:public Common_Tag
{
public:
	Group_Tag(void);
	~Group_Tag(void);
public:
	virtual VO_U32 Init(TiXmlNode* pNode);
public:
		VO_U32 GetID(){return m_uID;}
		VO_SOURCE_TRACKTYPE GetTrackType(){return m_track_type;}
		VO_CHAR *GetLang(){ return m_chLang;}
		VO_U32 GetCodecs(){ return m_codectype;}


private:
	Common_Tag * m_comtag;
	VO_U32	m_uID;
	VO_SOURCE_TRACKTYPE m_track_type;
	VO_CHAR m_chLang[20];
	VO_U32 m_codectype;


	

};

#endif