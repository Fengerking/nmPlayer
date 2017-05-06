#include "vo_MPD.h"
#include "voString.h"

vo_MPD::vo_MPD(void)
{
}

vo_MPD::~vo_MPD(void)
{

}

VO_S64 vo_MPD::get_mpd_duration()
{


}
VO_BOOL vo_MPD::is_live()
{
}
VO_BOOL vo_MPD::is_update()
{

}
VO_VOID vo_MPD::set_mpd_duration(VO_CHAR * attr)
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
VO_VOID vo_MPD::set_is_live(VO_CHAR * attr)
{

}
VO_VOID vo_MPD::set_is_update(VO_CHAR * attr)
{

}



