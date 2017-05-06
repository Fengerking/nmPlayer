#include "SegmentTemplate_Tag.h"

SegmentTemplate_Tag::SegmentTemplate_Tag(void)
{
	m_scale = 0;
	m_duration = 0;
	m_start_number = 0;
	memset(m_media_url, 0x00, sizeof(m_media_url));
	memset(m_inia_url, 0x00, sizeof(m_inia_url));
	memset(m_index_url, 0x00, sizeof(m_index_url));

}

SegmentTemplate_Tag::~SegmentTemplate_Tag(void)
{
	

}
VO_U32 SegmentTemplate_Tag::Init(TiXmlNode* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	  const char* attriValue = pNode->ToElement()->Attribute(TAG_DASH_MEDIABASEURL);
	  if(attriValue)
	  {
		  memcpy(m_media_url, attriValue, strlen(attriValue));
		  m_media_url[strlen(attriValue)] = '\0';

	  }
	  attriValue = pNode->ToElement()->Attribute(TAG_DASH_BASEURL_4);
	  if(attriValue)
	  {
		  memcpy(m_inia_url, attriValue, strlen(attriValue));
		  m_inia_url[strlen(attriValue)] = '\0';

	  }
	  attriValue = pNode->ToElement()->Attribute(TAG_DASH_BASEURL_3);
	  if(attriValue)
	  {
		  memcpy(m_inia_url, attriValue, strlen(attriValue));
		   m_inia_url[strlen(attriValue)] = '\0';
	  }
	    attriValue = pNode->ToElement()->Attribute(TAG_DASH_INDEX);
	  if(attriValue)
		  memcpy(m_index_url, attriValue, strlen(attriValue));
	  attriValue = pNode->ToElement()->Attribute(TAG_DASH_TIME_SCALE);
	  if(attriValue)
		m_scale = _ATO64(attriValue);
	   attriValue = pNode->ToElement()->Attribute(TAG_DASH_TIME_Duration);
	   if(attriValue)
		   {
			   VO_CHAR	duration[512];
			   memcpy(duration,attriValue,strlen(attriValue));
			   replace(duration,"PT","");
			   replace(duration,"S","");
			   VO_CHAR s_dd[512];
			   VO_CHAR s_s[512];
			   const char * find_url_dot = ".";
			   int s_index = m_comtag->lastIndex(duration,find_url_dot);
			   if(s_index!=-1)
			   {
				   int lenth = strlen(attriValue)- s_index-1;
				   int i =0;
				   for(int j = 0;j<strlen(attriValue);j++)
				   {
					   s_dd[i++] = duration[s_index+j+1];
				   }
				   for(int k = 0;k<lenth;k++)
				   {
					   s_s[lenth-k-1] = s_dd[k];
				   }
			   }
			   if(_ATO64(duration)<10)
			    m_duration = _ATO64(duration)*1000+_ATO64(s_s);
			   else
				 m_duration = _ATO64(duration)+_ATO64(s_s);

	   }
	    attriValue = pNode->ToElement()->Attribute(TAG_START_NUMBER);
	   if(attriValue)
	   {
		   m_start_number = _ATO64(attriValue);
		   if(m_start_number ==0)
			   m_start_number =1;


	   }


	return VO_RET_SOURCE2_OK;
}
