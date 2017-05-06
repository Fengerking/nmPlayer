
#ifndef _Metrics_H
#define _Metrics_H

#include "voType.h"
#include "voSource2.h"
#include "tinyxml.h"
#include  "Common_Tag.h"




#define BASE_LOCATION			"serviceLocation"
#define BYTE_RANGE		"byteRange"


class Metrics:public Common_Tag
	{
	public:
		Metrics(void);
		~Metrics(void);
	public:
		virtual VO_U32 Init(TiXmlNode* pNode);
	public:
		
	    VO_CHAR *GetRange(){ return m_byte_range;}
		VO_CHAR *GetServiceLocation(){ return m_serviceLocation;}
	private:
		VO_CHAR m_byte_range[256];
		VO_CHAR m_serviceLocation[256];
		Common_Tag * m_comtag;
};
#endif