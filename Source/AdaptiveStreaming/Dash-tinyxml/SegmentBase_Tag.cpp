#include "SegmentBase_Tag.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

SegmentBase_Tag::SegmentBase_Tag(void)
{
	
	Delete();
}

SegmentBase_Tag::~SegmentBase_Tag(void)
{
	
}
VO_VOID SegmentBase_Tag::Delete(void)
{
	m_scale = 0;
	memset(m_ini_Range, 0x00, sizeof(m_ini_Range));
	memset(m_ini_Url, 0x00, sizeof(m_ini_Url));
	
}

VO_U32 SegmentBase_Tag::Init(TiXmlNode* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	  const char* attriValue = pNode->ToElement()->Attribute(SEG_DASH_TIME_SCALE);
	   if(attriValue)
		m_scale = _ATO64(attriValue);
	   attriValue =  pNode->ToElement()->Attribute(Seg_DASH_STREAMING_SOURCEURL);
	   if(attriValue)
		   memcpy(m_ini_Url,attriValue,strlen(attriValue));
	   attriValue =  pNode->ToElement()->Attribute(Seg_DASH_RANGE);
	   if(attriValue)
	   { 
		   memcpy(m_ini_Range,attriValue,strlen(attriValue));
	   }
	

	return VO_RET_SOURCE2_OK;
}
