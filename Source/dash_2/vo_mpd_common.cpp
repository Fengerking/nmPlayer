#include "vo_MPD.h"
#include "voString.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "math.h"

vo_mpd_common::vo_mpd_common(void)
{
}

vo_mpd_common::~vo_mpd_common(void)
{

}

int vo_mpd_common::StrCompare(const char* p1, const char* p2)
{
#if defined _WIN32
	return stricmp(p1, p2);
#elif defined LINUX
	return strcasecmp(p1, p2);
#elif defined _IOS
	return strcasecmp(p1, p2);
#elif defined _MAC_OS
	return strcasecmp(p1, p2);
#endif
}

VO_S32 vo_mpd_common::lastIndex(const char *s,const char *t){
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
VO_VOID vo_mpd_common::replace(char *source,const char *sub,const char *rep );
{
	VO_CHAR	duration[512];
	memcpy(duration,attr,strlen(attr));
	replace(duration,"S","");
	replace(duration,"PT","");
	replace(duration,"0H0M","");
	replace(duration,"0H","");
	const char * find_url_h = "H";
	int h_index = lastIndex(duration,find_url_h);
	if(h_index!=-1)
	{
		hour = duration[h_index-1]-48;
		duration[h_index-1]= '0';
		replace(duration,"0H","");
	}
	replace(duration,"H","");
	const char * find_url_m = "M";
	int m_index = lastIndex(duration,find_url_m);
	if(m_index>0)
	{
		minute = (duration[m_index-1])-48;
		duration[m_index-1]= '0';
		replace(duration,"0M","");
	}
	else if(m_index ==0)
	{
		minute = (duration[m_index+1])-48;
		replace(duration,"M","");
		duration[m_index] = '0';
	}
	VO_CHAR s_dd[512];
	const char * find_url_dot = ".";
	int s_index = lastIndex(duration,find_url_dot);
	if(s_index!=-1)
	{
		for(int j = 0;j<strlen(attriValue);j++)
			s_dd[j] = duration[s_index+j+1];
	}
#if defined _WIN32
		if( _atoi64(duration)>0)
		second = _atoi64(s_dd);
		else
		second = _atoi64(s_dd)*1000;
#elif defined LINUX
		if( atoll(duration)>0)
		second = atoll(s_dd);
		else
		second = atoll(s_dd)*1000;
#endif
#if defined _WIN32
		mediaPresentationDuration = _atoi64(duration)*1000+ (minute)*60*1000+second;
#elif defined LINUX
		mediaPresentationDuration = atoll(duration)*1000+minute*60*1000+second;
#endif

}
VO_VOID vo_mpd_common::set_is_live(VO_CHAR * attr)
{

}
VO_VOID vo_mpd_common::set_is_update(VO_CHAR * attr)
{

}



