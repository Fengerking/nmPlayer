#include "SegInfo_Tag.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

SegInfo_Tag::SegInfo_Tag(void)
{
	


}

SegInfo_Tag::~SegInfo_Tag(void)
{
	
}
VO_VOID SegInfo_Tag::Delete(void)
{
		m_scale = 0;
	m_duration = 0;
}
VO_U32 SegInfo_Tag::Init(TiXmlNode* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	  const char* attriValue = pNode->ToElement()->Attribute(TAG_SEG_TIME_SCALE);
	  if(attriValue)
	  {
		  m_scale = _ATO64(attriValue);
	  }
	   attriValue = pNode->ToElement()->Attribute(TAG_SEG_TIME_Duration);
	   if(attriValue)
		   {
			   VO_CHAR	duration[512]; 
			   int len = strlen(attriValue);
			   memcpy(duration,attriValue,strlen(attriValue));
               duration[len]='\0';
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
				   s_s[lenth] = '\0';
			   }
			   if(_ATO64(duration)<10)
				   m_duration = _ATO64(duration);
			   else
				   m_duration = _ATO64(duration)+_ATO64(s_s);

	   }
		  
	return VO_RET_SOURCE2_OK;
}
