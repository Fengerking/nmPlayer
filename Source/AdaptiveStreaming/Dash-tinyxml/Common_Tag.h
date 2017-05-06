#pragma once
#ifndef _Common_Tag_H
#define _Common_Tag_H

#include "voString.h"
#include "voType.h"
#include "tinyxml.h"
#ifdef WIN32 
#include "time.h"
#elif defined LINUX|| defined _IOS
#include <time.h>
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
#ifdef _IOS
	using namespace _VONAMESPACE;
#endif
#ifdef WIN32 
#define _ATO64(a) _atoi64(a) 
#elif defined LINUX || defined _IOS
#define _ATO64(a) atoll(a) 
#endif 
class Common_Tag
{
public:
	Common_Tag(void);
	~Common_Tag(void);
public:
	int StrCompare(const char* p1, const char* p2);
	VO_VOID replace(char *source,const char *sub,const char *rep );
	public:
	virtual VO_U32 Init(TiXmlNode* pNode)= 0;
    VO_S32  lastIndex(const char *s,const char *t);
	time_t  FormatTime2(const char * szTime);

};
#ifdef _VONAMESPACE
}
#endif

#endif
