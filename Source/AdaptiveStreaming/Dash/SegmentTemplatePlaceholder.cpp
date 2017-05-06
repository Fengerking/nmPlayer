#include "SegmentTemplatePlaceholder.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

SegmentTemplatePlaceholder::SegmentTemplatePlaceholder(void)
{
	Reset();
	
}

SegmentTemplatePlaceholder::~SegmentTemplatePlaceholder(void)
{
}

VO_VOID SegmentTemplatePlaceholder::Reset()
{

	memset(m_meida_url, 0x00, sizeof(m_meida_url));
	memset(m_init_url, 0x00, sizeof(m_init_url));
	m_duration = 0;

}

VO_U32 SegmentTemplatePlaceholder::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	Reset();
	char* attriValue = NULL;
	int nSize = 0;
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MEDIA),&attriValue,nSize) == VO_ERR_NONE)
	{
	if(attriValue)
	{
		  memcpy(m_meida_url, attriValue, strlen(attriValue));
		  m_meida_url[strlen(attriValue)] = '\0';
	}
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(INITURL),&attriValue,nSize) == VO_ERR_NONE)
	{
	if(attriValue)
	{
		 memcpy(m_init_url, attriValue, strlen(attriValue));
		 m_init_url[strlen(attriValue)] = '\0';
	}
	}	
	return VO_RET_SOURCE2_OK;
	
}