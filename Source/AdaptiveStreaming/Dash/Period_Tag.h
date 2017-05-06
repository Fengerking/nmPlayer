
#ifndef _Period_Tag_H
#define _Period_Tag_H

#include "voType.h"
#include "voSource2.h"
#include "tinyxml.h"
#include  "Common_Tag.h"
#include "voXMLLoad.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
#ifdef _IOS
	using namespace _VONAMESPACE;
#endif



#define PERIOD_ID		"id"
#define PERIOD_START	"start"
#define PERIOD_DURATION	  "duration"
#define PERIOD_BITSWITCH	  "bitstreamSwitching"
#define TAG_DASH_STREAMING_Period   "Period"
#define TAG_DASH_STREAMING_Period_G1  "g1:Period"


class Period_Tag:public Common_Tag
	{
	public:
		Period_Tag(void);
		~Period_Tag(void);
	public:
		virtual VO_U32 Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode);
	public:
	    VO_U32 GetID(){ return m_uID;}
		VO_S64 GetDuration(){ return m_duration;}
		VO_S64 GetStart(){ return m_start;}
		VO_BOOL GetBitSwitch(){ return m_bitSwitch;}
		VO_VOID Delete();
	private:
		VO_U32	m_uID;
		VO_S64  m_start;
		VO_S64  m_duration;
		VO_BOOL  m_bitSwitch;
	//	Common_Tag * m_comtag;
};
#ifdef _VONAMESPACE
}
#endif
#endif