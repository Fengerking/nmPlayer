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

time_t  Common_Tag::FormatTime2(const VO_CHAR * szTime)
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
#if defined WINCE
	_tzset();
	time1 = mktime(&tm1)- _timezone;
#elif defined WIN32
	tzset();
	time1 = mktime(&tm1)- timezone;
#else
	struct tm * tempForTZ;
	time_t localUTC;   
	localUTC = time(&localUTC);
	tempForTZ = localtime((const time_t *)&localUTC);
	tm1.tm_isdst = 0;
	//time_t ret = mktime(&tm1);
	time1 = mktime(&tm1) +  tempForTZ->tm_gmtoff - (tempForTZ->tm_isdst == 1 ? 3600 :0);
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

VO_VOID Common_Tag::replace(VO_CHAR *source,const VO_CHAR *sub,const VO_CHAR *rep )
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
VO_S32 Common_Tag::lastIndex(const VO_CHAR *s,const VO_CHAR *t){
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
VO_CHAR * Common_Tag::Linux_Strrev(VO_CHAR * attr)
{
	if(!attr)
		return NULL;
	int i = strlen(attr);
	int t = !(i%2)?1:0;
	for(int j = i-1,k= 0;j>(i/2-t);j--)
	{
		VO_CHAR ch  = attr[j];
		attr[j]   = attr[k];
		attr[k++] = ch;
	}
	return attr;
}
VO_S64 Common_Tag::Compute_Time_Duration(VO_CHAR * attr) // PT**H**M***.****S utc_time fuc
{
	VO_S64 hour = 0;
	VO_S64 minute = 0;
	VO_S64 second  = 0;
	VO_S64 mil_sec = 0;
	VO_S64 m_duration = -1;
	VO_CHAR	duration[256];
	VO_CHAR	temp_hour[256];
	VO_CHAR	temp_min[256];
	VO_CHAR temp_sec[256];
	memset(temp_hour, 0x00, sizeof(temp_hour));
	memset(temp_min, 0x00, sizeof(temp_min));
	memset(temp_sec, 0x00, sizeof(temp_sec));
	memset(duration, 0x00, sizeof(duration));
	const char * find_url_h = "H";
	const char * find_url_m = "M";
	const char * find_url_s = ".";
	VO_U32  len = strlen(attr);
	memcpy(duration,attr,len);
	replace(duration,"S","");
	replace(duration,"PT","");
#if defined WINCE
	char * rev = Linux_Strrev(duration);
#elif defined WIN32
	char * rev = strrev(duration);
#else
	char * rev = Linux_Strrev(duration);
#endif
	char * hour_str = strstr(duration,find_url_h);
	if(hour_str)
	{
		len = strlen(hour_str);
#if defined WINCE
		memcpy(temp_hour,Linux_Strrev(hour_str),len);
		replace(duration,Linux_Strrev(hour_str),"");	

#elif defined WIN32
		memcpy(temp_hour,strrev(hour_str),len);
		replace(duration,strrev(hour_str),"");
#else
		memcpy(temp_hour,Linux_Strrev(hour_str),len);
		replace(duration,Linux_Strrev(hour_str),"");	
#endif
		replace(temp_hour,"H","");
		hour = _ATO64(temp_hour);
	}
	char * min_str = strstr(duration,find_url_m);
	if(min_str)
	{
		len = strlen(min_str);
#if defined WINCE
		memcpy(temp_min,Linux_Strrev(min_str),len);
		replace(duration,Linux_Strrev(min_str),"");
#elif defined WIN32
		memcpy(temp_min,strrev(min_str),len);
		replace(duration,strrev(min_str),"");

#else
		memcpy(temp_min,Linux_Strrev(min_str),len);
		replace(duration,Linux_Strrev(min_str),"");
#endif
		replace(temp_min,"M","");
		minute = _ATO64(temp_min);
	}
	char * ss_str = strstr(duration,find_url_s);
	if(ss_str)
	{
		len = strlen(ss_str);
#if defined WINCE
		memcpy(temp_sec,Linux_Strrev(ss_str),len);
		replace(duration,Linux_Strrev(ss_str),"");
#elif defined WIN32
		memcpy(temp_sec,strrev(ss_str),len);
		replace(duration,strrev(ss_str),"");
#else
		memcpy(temp_sec,Linux_Strrev(ss_str),len);
		replace(duration,Linux_Strrev(ss_str),"");
#endif
		replace(temp_sec,".","");
		second = _ATO64(temp_sec);
	}
	if(duration)
	{
		if(ss_str==NULL)
		{
			mil_sec = 0;
#if defined WINCE
			char * ss = Linux_Strrev(duration);

#elif defined WIN32
			char * ss = strrev(duration);
#else
			char * ss = Linux_Strrev(duration);
#endif
			second =  _ATO64(duration);
		}
		else
		{
#if defined WINCE
			char * temp = Linux_Strrev(duration);
#elif defined WIN32
			char * temp = strrev(duration);

#else
			char * temp = Linux_Strrev(duration);
#endif
			duration[3]='\0';
			mil_sec =  _ATO64(duration);
		}
	}
	m_duration = (hour)*3600*1000+ (minute)*60*1000+second*1000+mil_sec;
	return m_duration;
}
