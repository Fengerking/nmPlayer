#include "Common_Tag.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
Common_Tag::Common_Tag(void)
{
}

Common_Tag::~Common_Tag(void)
{
}

time_t  Common_Tag::FormatTime2(const char * szTime)
{
        struct tm tm1;
        time_t time1;
		//2012-05-31T14:21:00
       sscanf(szTime, "%4d-%2d-%2dT%2d:%2d:%2d",    
                     &tm1.tm_year,
                     &tm1.tm_mon,
                     &tm1.tm_mday,
                     &tm1.tm_hour,
                     &tm1.tm_min,
                     &tm1.tm_sec);
              
        tm1.tm_year -= 1900;
        tm1.tm_mon --;
        tm1.tm_isdst=-1;
   			  			
#ifdef WIN32
  		tzset();
        time1 = mktime(&tm1)- timezone;
#else
		struct tm * tempForTZ;
		time_t localUTC;   
		localUTC = time(&localUTC);
		tempForTZ = localtime((const time_t *)&localUTC);
		time1 = mktime(&tm1) + tempForTZ->tm_gmtoff;
#endif
        return time1;
}
int Common_Tag::StrCompare(const char* p1, const char* p2)
{
#if defined WINCE
  return strcmp(p1, p2);
#elif defined WIN32
	return stricmp(p1, p2);
#elif defined LINUX
	return strcasecmp(p1, p2);
#elif defined _IOS
	return strcasecmp(p1, p2);
#elif defined _MAC_OS
	return strcasecmp(p1, p2);
#endif
}

VO_VOID Common_Tag::replace(char *source,const char *sub,const char *rep )
{

	int in,out;
	in = out = 0;
	int lsub = strlen(sub);
	int lsour = strlen(source);
	char tmp[1024];
	char *p = source;
	char sourceclone[1024];
	memset(sourceclone,0x00,1024);
	memcpy(sourceclone,source,lsour);
	while(lsour >= lsub)
	{
		memset(tmp,0x00,155);
		memcpy(tmp,p,lsub);
		if(strcmp(tmp,sub)==0)
		{
			out = in +lsub;
			break;
		}
		in ++;
		p++;
		lsour -- ;
	}
	if(out >0 && in != out)
	{
		memset(source, 0 ,strlen(source));
		memcpy(source, sourceclone,in);
		strcat(source,rep);
		strcat(source, &sourceclone[out]);
	}
}
VO_S32 Common_Tag::lastIndex(const char *s,const char *t){
	const char *s1=s;
	const char *t1=t;
	int n1=strlen(s1);
	int n2=strlen(t1);
	int i=0;
	int j=0;
	bool find=false;
	int loc=0;
	while(i<n1&&j<n2)
	{

		if(*(s1+i)==*(t1+j))
		{
			const char *s2=s1+i+1;
			const char *t2=t1+j+1;
			while(*s2!='\0'&&*t2!='\0')
			{
				if(*s2==*t2)
				{
					s2++;
					t2++;
				}
				else
				{
					break;
				}
			}
	
			if(*t2=='\0')
			{
				find=true;
				loc=i;
			}
			i++;
		}
		
		else
		{
			i++;
		}
	}
	if(find)return loc;
	else return -1;

}
