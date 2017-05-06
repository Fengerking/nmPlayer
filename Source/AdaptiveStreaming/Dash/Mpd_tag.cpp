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
VO_U32 Mpd_tag::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	int hour = 0;
	int minute = 0;
	int second  = 0;
	int nSize = 0;
	char* attriValue = NULL;
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_PFOFILE),&attriValue,nSize) == VO_ERR_NONE)
	{

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
			else if(strstr(attriValue,"mp2t-simple"))
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
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_TYPE),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
			m_is_need_update = VO_FALSE;

			if(0 ==StrCompare(attriValue, "IsLive"))
			{
				m_is_need_update = VO_TRUE; 
			}
			else if(0 ==StrCompare(attriValue, "static"))
			{
				m_is_need_update = VO_FALSE;
			}
			else if(0 ==StrCompare(attriValue, "dynamic"))
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
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_DASH_UPDATE_PERIOD),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
			VO_CHAR	duration[512];
			memcpy(duration,attriValue,strlen(attriValue));
			duration[strlen(attriValue)] = '\0';
			replace(duration,"PT","");
			replace(duration,"S","");
			m_update_period = _ATO64(duration)*1000;
		}
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_MEFIA_DURATION),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
			VO_CHAR	duration[512];
			memcpy(duration,attriValue,strlen(attriValue));
			duration[strlen(attriValue)] = '\0';
			m_duration = Compute_Time_Duration(duration);
			VOLOGI("MPD_Tag m_duration: %lld ", m_duration);
		}
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_DASH_BufferDepth),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
			VO_CHAR	duration[512];
			memcpy(duration,attriValue,strlen(attriValue));
			duration[strlen(attriValue)] = '\0';
			m_duration = Compute_Time_Duration(duration);
		}
	}
	//	attriValue = pNode->ToElement()->Attribute(MPD_MIN_BUFFER_TIME);
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_MIN_BUFFER_TIME),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
			m_buffer_time = _ATO64(attriValue)*1000;
		}
	}
	// 	attriValue = pNode->ToElement()->Attribute(MPD_AVAIL_START_TIME);
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_AVAIL_START_TIME),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
			m_avail_start_time = FormatTime2(attriValue);
			VOLOGW("m_avail_start_time ssss %s",attriValue);
		}
	}
	//
	return VO_RET_SOURCE2_OK;

}