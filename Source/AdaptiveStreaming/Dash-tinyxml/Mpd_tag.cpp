#include "Mpd_tag.h"

#include "Common_Tag.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

Mpd_tag::Mpd_tag(void)
{
	Delete();
	
}

Mpd_tag::~Mpd_tag(void)
{
}

VO_VOID Mpd_tag::Delete()
{
	m_uID = 0;
	m_duration = 0;
	m_is_live = 0;;
	m_is_need_update = VO_FALSE;
	m_avail_start_time = 0;
	m_avail_end_time = 0;
	m_update_period = 0;
	m_buffer_time = 0;
	max_seg_duration = 0;
	min_seg_duration = 0;
}
VO_U32 Mpd_tag::Init(TiXmlNode* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	int hour = 0;
	int minute = 0;
	int second  = 0;
	const char* attriValue = pNode->ToElement()->Attribute(MPD_PFOFILE);
	if(attriValue)
	{
		if(strstr(attriValue,"mp2t-main"))
		{
			m_is_live = 5; //urn:mpeg:dash:profile:mp2t-main:2011
		}
		else if(strstr(attriValue,"on-demand"))
		{
			m_is_live = 1;// vod
		}
		else if(strstr(attriValue,"isoff-live"))
		{
			m_is_live = 2;//live
		}
		else if(strstr(attriValue,"full"))
		{
			m_is_live = 3;
		}
		else if(strstr(attriValue,"mp2t-simple")||strstr(attriValue,"mpegB"))
		{
			m_is_live = 4;//urn:mpeg:dash:profile:mp2t-simple:2011
		}
		else if(strstr(attriValue,"main"))
		{
			m_is_live = 0;// the mp4 profile
		}
	}
	else
	{
		m_is_live = 0;
	}

	attriValue = pNode->ToElement()->Attribute(MPD_TYPE);
	if(attriValue)
	{
		m_is_need_update = VO_FALSE;
	
	 if(0 ==m_comtag->StrCompare(attriValue, "IsLive"))
	{
			m_is_need_update = VO_TRUE; 
	}
	else if(0 == m_comtag->StrCompare(attriValue, "static"))
	{
			m_is_need_update = VO_FALSE;
	}
	else if(0 == m_comtag->StrCompare(attriValue, "dynamic"))
	{
	      	m_is_need_update = VO_TRUE;
			if(m_is_live==3)
			{
				m_is_live = 1;
			}
	}
	}
	else
	{
		m_is_need_update =  VO_FALSE;

	}
	attriValue = pNode->ToElement()->Attribute(MPD_DASH_UPDATE_PERIOD);
	if(attriValue)
	{
		VO_CHAR	duration[512];
		memcpy(duration,attriValue,strlen(attriValue));
		duration[strlen(attriValue)] = '\0';
		m_comtag->replace(duration,"PT","");
		m_comtag->replace(duration,"S","");
	    m_update_period = _ATO64(duration)*1000;
	}
	attriValue = pNode->ToElement()->Attribute(MPD_MEFIA_DURATION);
	if(attriValue)
	{
		VO_CHAR	duration[512];
		memcpy(duration,attriValue,strlen(attriValue));
		duration[strlen(attriValue)] = '\0';
		m_comtag->replace(duration,"S","");
		m_comtag->replace(duration,"PT","");
		m_comtag->replace(duration,"0H0M","");
		m_comtag->replace(duration,"0H","");
		const char * find_url_h = "H";
		int h_index = lastIndex(duration,find_url_h);
		VO_CHAR h_dd[512];
		hour = 0;
		int hh_index = 0;
		if(h_index!=-1)
		{
			while(h_index>0)
			{   
				int temp = duration[hh_index]-48;
				hour*=10;
				hour+=temp;
				hh_index++;
				h_index--;
			}
			if(hour<10)
				duration[h_index-1]= '0';
			m_comtag->replace(duration,"0H","");
	    }
		m_comtag->replace(duration,"H","");
	    const char * find_url_m = "M";
		int m_index = lastIndex(duration,find_url_m);
		int mm_index = 0;
		if(m_index>0)
		{
		while(m_index>0)
		{
		    int temp =(duration[mm_index])-48;
			minute*=10;
			minute+= temp;
			mm_index++;
			m_index--;	
		}
		}
		else if(m_index ==0)
		{
			minute = (duration[m_index+1])-48;
			m_comtag->replace(duration,"M","");
			duration[m_index] = '0';
			m_comtag->replace(duration,"0M","");
		}
		if(minute>0)
		{
		if(minute<10)
		{
			duration[0]= '0';
			m_comtag->replace(duration,"0M","");
		}
		else if(minute<100)
		{
			duration[0]= '0';
			duration[1]= '0';
			m_comtag->replace(duration,"00M","");
		}
		}
		replace(duration,"M","");
		VO_CHAR s_dd[3];
		const char * find_url_dot = ".";
		int s_index = lastIndex(duration,find_url_dot);
		if(s_index!=-1)
		{
			for(int j = 0;j<3;j++)
				s_dd[j] = duration[s_index+j+1];

		}

		if( _ATO64(duration)>0)
			second = _ATO64(s_dd);
		else
			second = _ATO64(s_dd)*1000;
		if(hour>0)
			m_duration = (hour)*3600*60+ (minute)*60*1000+second;
		else
			m_duration= _ATO64(duration)*1000+ (minute)*60*1000+second;
		}

		attriValue = pNode->ToElement()->Attribute(MPD_DASH_BufferDepth);
		if(attriValue)
	{
		VO_CHAR	duration[512];
		memcpy(duration,attriValue,strlen(attriValue));
		duration[strlen(attriValue)] = '\0';
		m_comtag->replace(duration,"S","");
		m_comtag->replace(duration,"PT","");
		m_comtag->replace(duration,"0H0M","");
		m_comtag->replace(duration,"0H","");
		const char * find_url_h = "H";
		int h_index = m_comtag->lastIndex(duration,find_url_h);
		VO_CHAR h_dd[512];
		hour = 0;
		int hh_index = 0;
		if(h_index!=-1)
		{
			while(h_index>0)
			{   
				int temp = duration[hh_index]-48;
				hour*=10;
				hour+=temp;
				hh_index++;
				h_index--;
			}
			if(hour<10)
				duration[h_index-1]= '0';
			m_comtag->replace(duration,"0H","");
	    }
		m_comtag->replace(duration,"H","");
	    const char * find_url_m = "M";
		int m_index = lastIndex(duration,find_url_m);
		int mm_index = 0;
		if(m_index>0)
		{
		while(m_index>0)
		{
		    int temp =(duration[mm_index])-48;
			minute*=10;
			minute+= temp;
			mm_index++;
			m_index--;	
		}
		}
		else if(m_index ==0)
		{
			minute = (duration[m_index+1])-48;
			m_comtag->replace(duration,"M","");
			duration[m_index] = '0';
			replace(duration,"0M","");
		}
		if(minute>0)
		{
		if(minute<10)
		{
			duration[0]= '0';
			m_comtag->replace(duration,"0M","");
		}
		else if(minute<100)
		{
			duration[0]= '0';
			duration[1]= '0';
			m_comtag->replace(duration,"00M","");
		}
		}
		replace(duration,"M","");
		VO_CHAR s_dd[512];
		const char * find_url_dot = ".";
		int s_index = lastIndex(duration,find_url_dot);
		if(s_index!=-1)
		{
			for(int j = 0;j<3;j++)
				s_dd[j] = duration[s_index+j+1];
		}

		if( _ATO64(duration)>0)
			second = _ATO64(s_dd);
		else
			second = _ATO64(s_dd)*1000;
		if(hour>0)
			m_duration = (hour)*3600*60+ (minute)*60*1000+second;
		else
			m_duration= _ATO64(duration)*1000+ (minute)*60*1000+second;

	}
	attriValue = pNode->ToElement()->Attribute(MPD_MIN_BUFFER_TIME);
	if(attriValue)
	{
		m_buffer_time = _ATO64(attriValue)*1000;
	}
   	attriValue = pNode->ToElement()->Attribute(MPD_AVAIL_START_TIME);
	if(attriValue)
	{
		m_avail_start_time = m_comtag->FormatTime2(attriValue);
		VOLOGW("m_avail_start_time ssss %s",attriValue);
	}
	//
	return VO_RET_SOURCE2_OK;
	
}