

#include "contentComponent_Tag.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

contentComponent_Tag::contentComponent_Tag()
{
Delete();
}


VO_U32 contentComponent_Tag::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

    char* attriValue = NULL;
	int nSize = 0;
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_ID),&attriValue,nSize) == VO_ERR_NONE)
	{
	if(attriValue)
		m_uID = atoi(attriValue);
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_LANG),&attriValue,nSize) == VO_ERR_NONE)
	{
	if(attriValue)
		memcpy(m_chLang, attriValue, strlen(attriValue));
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_CONTENTTYPE),&attriValue,nSize) == VO_ERR_NONE)
	{
	if(attriValue)
		memcpy(m_chContentType, attriValue, strlen(attriValue));
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(MPD_PAR),&attriValue,nSize) == VO_ERR_NONE)
	{
	if(attriValue)
		memcpy(m_chPar, attriValue, strlen(attriValue));
	}

	return VO_RET_SOURCE2_OK;
	
}
VO_VOID  contentComponent_Tag::Delete()
{
	m_uID = 0;
	memset(m_chLang, 0x00, sizeof(m_chLang));
	memset(m_chContentType, 0x00, sizeof(m_chContentType));
	memset(m_chPar, 0x00, sizeof(m_chPar));
}