
#ifndef _BaseUrl_H
#define _BaseUrl_H

#include "voType.h"
#include "voSource2.h"
#include "Common_Tag.h"
#include "voXMLLoad.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#define BASE_LOCATION			"serviceLocation"
#define BYTE_RANGE		        "byteRange"
#define TAG_DASH_BASEURL_2       "BaseURL"






class BaseUrl:public Common_Tag
	{
	public:
		BaseUrl(void);
		~BaseUrl(void);
	public:
		virtual VO_U32 Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode);
	public:
	    VO_CHAR *GetRange(){ return m_byte_range;}
	    VO_CHAR *GetUrl(){ return p_value;}
		VO_CHAR *GetServiceLocation(){ return m_serviceLocation;}
		VO_VOID Reset();
	private:
		VO_CHAR m_byte_range[256];
		VO_CHAR p_value[256];
		VO_CHAR m_serviceLocation[256];
};
#ifdef _VONAMESPACE
}
#endif
#endif