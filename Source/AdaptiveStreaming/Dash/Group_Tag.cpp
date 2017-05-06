#include "Group_Tag.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
Group_Tag::Group_Tag(void)
{
	Delete();

}
VO_VOID Group_Tag::Delete()
{

	memset(m_chLang, 0x00, sizeof(m_chLang));
	m_codectype  = VO_VIDEO_CodingH264;
	m_track_type = VO_SOURCE_TT_MAX;
	m_track_count = 1;
	period_duration = 0;
	m_is_ttml = VO_TRUE;

}

Group_Tag::~Group_Tag(void)
{

}

VO_U32 Group_Tag::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	int hour = 0;
	int minute = 0;
	int second  = 0;
	char* attriValue = NULL;
	int nSize = 0;
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(GROUP_STREAMING_MIMETYPE),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
			if(0==StrCompare(attriValue,"video/mp4") || 0==StrCompare(attriValue,"video/3gpp"))
			{
				m_track_type = VO_SOURCE_TT_VIDEO;
			}
			else if(0==StrCompare(attriValue,"video/mp2t"))
			{
				m_track_type = VO_SOURCE_TT_VIDEO;
			}
			else if(0==StrCompare(attriValue,"audio/mp4"))
			{
				m_track_type = VO_SOURCE_TT_AUDIO;
			}
			else if(0==StrCompare(attriValue,"application/ttml+xml"))
			{
				m_track_type = VO_SOURCE_TT_SUBTITLE;
			//	m_pXmlLoad->GetNextSibling(pNode,&pNode);
				//continue;
			}
			else if(0==StrCompare(attriValue,"application/mp4"))
			{
				m_track_type = VO_SOURCE_TT_SUBTITLE;
				m_is_ttml= VO_FALSE;
			//	m_pXmlLoad->GetNextSibling(pNode,&pNode);
				//continue;
			}
		}
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(GROUP_LANG),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
			memcpy(m_chLang, attriValue, strlen(attriValue));
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(TAG_CODECS),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
			if (StrCompare(attriValue, "H264")== 0)
				m_codectype = VO_VIDEO_CodingH264;
			else if(StrCompare(attriValue,"avc1"))
				m_codectype = VO_VIDEO_CodingH264;
			else if (StrCompare(attriValue, "WVC1")== 0)
				m_codectype = VO_VIDEO_CodingVC1;//VOMP_VIDEO_CodingVC1;
			else if (StrCompare(attriValue, "WmaPro")== 0 || StrCompare(attriValue, "WMAP") == 0)
				m_codectype = VO_AUDIO_CodingWMA;
			else if (StrCompare(attriValue, "AACL")== 0)
				m_codectype = VO_AUDIO_CodingAAC;
			  if(strstr(attriValue,"avc")&&strstr(attriValue,"mp4")||strstr(attriValue,","))
			  {
			  m_track_count =2;
			  }
			else
				m_track_count =1;
		}
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(Group_Duration),&attriValue,nSize) == VO_ERR_NONE)
	{
		//  attriValue = pNode->ToElement()->Attribute(Group_Duration);
		if(attriValue)
		{
			VO_CHAR	duration[512];
			memcpy(duration,attriValue,strlen(attriValue));
			duration[strlen(attriValue)] = '\0';
			replace(duration,"S","");
			replace(duration,"PT","");
			replace(duration,"0H0M","");
			replace(duration,"0H","");
			const char * find_url_h = "H";
			int h_index = lastIndex(duration,find_url_h);
		//	VO_CHAR h_dd[512];
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
			replace(duration,"M","");
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
				period_duration = (hour)*3600*60+ (minute)*60*1000+second;
			else
				period_duration= _ATO64(duration)*1000+ (minute)*60*1000+second;

		}
	}


	return VO_RET_SOURCE2_OK;
}
