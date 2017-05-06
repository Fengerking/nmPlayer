#include "Period_Tag.h"



Period_Tag::Period_Tag(void)
{
m_duration = 0;
m_start = 0;
m_bitSwitch = VO_FALSE;
m_uID = 0;
	
	
}

Period_Tag::~Period_Tag(void)
{
}


VO_U32 Period_Tag::Init(TiXmlNode* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	int hour = 0;
	int minute = 0;
	int second  = 0;
	const char* attriValue = pNode->ToElement()->Attribute(PERIOD_DURATION);
	if(attriValue)
	{
		VO_CHAR	duration[512];
		memcpy(duration,attriValue,strlen(attriValue));
		replace(duration,"S","");
		replace(duration,"PT","");
		replace(duration,"0H0M","");
		replace(duration,"0H","");
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
			replace(duration,"0H","");
	    }
		replace(duration,"H","");
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
			replace(duration,"M","");
			duration[m_index] = '0';
			replace(duration,"0M","");
		}
		if(minute>0)
		{
		if(minute<10)
		{
			duration[0]= '0';
			replace(duration,"0M","");
		}
		else if(minute<100)
		{
			duration[0]= '0';
			duration[1]= '0';
			replace(duration,"00M","");
		}
		}
		VO_CHAR s_dd[512];
		const char * find_url_dot = ".";
		int s_index = lastIndex(duration,find_url_dot);
		if(s_index!=-1)
		{
			for(int j = 0;j<strlen(attriValue);j++)
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

	 attriValue = pNode->ToElement()->Attribute(PERIOD_START);
	 if(attriValue)
	{
		VO_CHAR	duration[512];
		memcpy(duration,attriValue,strlen(attriValue));
		replace(duration,"PT","");
		replace(duration,"PTM","");
		replace(duration,"S","");
		replace(duration,"0H0M","");
		replace(duration,"0H","");
		const char * find_url_h = "H";
		int h_index = m_comtag->lastIndex(duration,find_url_h);
		if(h_index!=-1)
		{
			 hour = duration[h_index-1]-48;
			 duration[h_index-1]= '0';
			 replace(duration,"0H","");
	    }
	    const char * find_url_m = "M";
	    int m_index = m_comtag->lastIndex(duration,find_url_m);
		if(m_index!=-1)
		{
		   minute = (duration[m_index-1])-48;
		   duration[m_index-1]= '0';
		   replace(duration,"0M","");
		}
		VO_CHAR s_dd[512];
		const char * find_url_dot = ".";
		int s_index = m_comtag->lastIndex(duration,find_url_dot);
		if(s_index!=-1)
		{
			for(int j = 0;j<strlen(attriValue);j++)
				s_dd[j] = duration[s_index+j+1];
		}

		second = _ATO64(s_dd);
		m_start = _ATO64(duration)*1000+ (minute)*60*1000+second;

	}
	
	return VO_RET_SOURCE2_OK;
	
}