

#include "contentComponent_Tag.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

contentComponent_Tag::contentComponent_Tag()
{
Delete();
}


VO_U32 contentComponent_Tag::Init(TiXmlNode* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	const char* attriValue = pNode->ToElement()->Attribute(MPD_ID);
	if(attriValue)
		m_uID = atoi(attriValue);
	attriValue = pNode->ToElement()->Attribute(MPD_LANG);
	if(attriValue)
		memcpy(m_chLang, attriValue, strlen(attriValue));
	attriValue = pNode->ToElement()->Attribute(MPD_CONTENTTYPE);
	if(attriValue)
		memcpy(m_chContentType, attriValue, strlen(attriValue));
	attriValue = pNode->ToElement()->Attribute(MPD_PAR);
	if(attriValue)
		memcpy(m_chPar, attriValue, strlen(attriValue));
	
	return VO_RET_SOURCE2_OK;
	
}
VO_VOID  contentComponent_Tag::Delete()
{
	m_uID = 0;
	memset(m_chLang, 0x00, sizeof(m_chLang));
	memset(m_chContentType, 0x00, sizeof(m_chContentType));
	memset(m_chPar, 0x00, sizeof(m_chPar));
}