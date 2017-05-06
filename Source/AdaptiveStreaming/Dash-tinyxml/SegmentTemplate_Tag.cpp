#include "SegmentTemplate_Tag.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

SegmentTemplate_Tag::SegmentTemplate_Tag(void)
{
  Delete();
}

SegmentTemplate_Tag::~SegmentTemplate_Tag(void)
{
	

}
VO_VOID SegmentTemplate_Tag::Delete(void)
{
   	m_scale = 0;
	m_duration = 0;
	m_start_number = 1;
	m_offset = 0;
	memset(m_media_url, 0x00, sizeof(m_media_url));
	memset(m_inia_url, 0x00, sizeof(m_inia_url));
	memset(m_index_url, 0x00, sizeof(m_index_url));
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
			   int len = strlen(attriValue);
			   duration[len]= '\0';
			   memcpy(duration,attriValue,strlen(attriValue));
			   replace(duration,"PT","");
			   replace(duration,"S","");
			  
			   VO_CHAR s_dd[512];
			   VO_CHAR s_s[512];
			   const char * find_url_dot = ".";
			   VO_S64 s_index = m_comtag->lastIndex(duration,find_url_dot);
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
				   s_s[lenth] = '\0';
			   }
             //  VO_U32 b = 0;
			 //  sscanf(duration, "%d,%d", &m_duration,&b);
			//   VOLOGW("mm_duration1 %d",m_duration);
			  // VOLOGW("index %lld",s_index);
			 
			   if(_ATO64(duration)<10)
			     m_duration = _ATO64(duration);
			   else
				 m_duration = _ATO64(duration)+_ATO64(s_s);
			   
			  // VOLOGW("mm_duration %lld",m_duration);
			 //  VOLOGW("MM_duration %lld ,ss %lld ,change_dutration,%lld",m_duration,_ATO64(s_s),_ATO64(duration));

	   }
	    attriValue = pNode->ToElement()->Attribute(TAG_START_NUMBER);
	   if(attriValue)
	   {
		   m_start_number = _ATO64(attriValue);
		


	   }
	   attriValue = pNode->ToElement()->Attribute(TAG_PRESENTATIONOFFSET);
	   if(attriValue)
	   {
		   m_offset = _ATO64(attriValue);

	   }



	return VO_RET_SOURCE2_OK;
}
