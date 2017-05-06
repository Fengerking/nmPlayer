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
	//m_ini_Url =  NULL;
	
}

VO_U32 SegmentBase_Tag::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	  char* attriValue = NULL;
	  int  nSize = 0;
	  if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(SEG_DASH_TIME_SCALE),&attriValue,nSize) == VO_ERR_NONE)
	  {
	   if(attriValue)
		m_scale = _ATO64(attriValue);
	  }
	   if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(Seg_DASH_STREAMING_SOURCEURL),&attriValue,nSize) == VO_ERR_NONE)
	   {
	   if(attriValue)
		 /*  strcpy(m_ini_Url,attriValue);*/
	   {
		   memcpy(m_ini_Url,attriValue,strlen(attriValue));
		   m_ini_Url[strlen(attriValue)] = '\0';
	   }
	   }
	   if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(Seg_DASH_RANGE),&attriValue,nSize) == VO_ERR_NONE)
	   {
	   if(attriValue)
	   { 
		   memcpy(m_ini_Range,attriValue,strlen(attriValue));
		   m_ini_Range[strlen(attriValue)] = '\0';
	   }
	   }
	

	return VO_RET_SOURCE2_OK;
}
