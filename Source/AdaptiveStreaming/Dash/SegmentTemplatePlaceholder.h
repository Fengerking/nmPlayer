
#ifndef _SegmentTemplatePlaceholder_H
#define _SegmentTemplatePlaceholder_H

#include "voType.h"
#include "voSource2.h"
#include "Common_Tag.h"
#include "voXMLLoad.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#define MEDIA			 "media"
#define INITURL		     "initialization"
#define DURATION          "duration"
#define TIMESCALE         "timescale"





class SegmentTemplatePlaceholder:public Common_Tag
	{
	public:
		SegmentTemplatePlaceholder(void);
		~SegmentTemplatePlaceholder(void);
	public:
		virtual VO_U32 Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode);
	public:
	    VO_CHAR *GetMediaUrl(){ return m_meida_url;}
	    VO_CHAR *GetInitUrl(){ return m_init_url;}
		VO_S64 GetDuration(){ return m_duration;}
		VO_VOID Reset();
	private:
		VO_CHAR m_meida_url[256];
		VO_CHAR m_init_url[256];
		VO_S64  m_duration;
};
#ifdef _VONAMESPACE
}
#endif
#endif