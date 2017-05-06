#include "BaseUrl.h"



BaseUrl::BaseUrl(void)
{
	memset(m_byte_range, 0x00, sizeof(m_byte_range));
	memset(m_serviceLocation, 0x00, sizeof(m_serviceLocation));
	
	
}

BaseUrl::~BaseUrl(void)
{
}


VO_U32 BaseUrl::Init(TiXmlNode* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	int hour = 0;
	int minute = 0;
	int second  = 0;
	const char* attriValue = pNode->ToElement()->Attribute(BYTE_RANGE);
	if(attriValue)
	{
		  memcpy(m_byte_range, attriValue, strlen(attriValue));
		  m_byte_range[strlen(attriValue)] = '\0';
	}
	attriValue = pNode->ToElement()->Attribute(BASE_LOCATION);
	if(attriValue)
	{
		 memcpy(m_serviceLocation, attriValue, strlen(attriValue));
		 m_serviceLocation[strlen(attriValue)] = '\0';
		}

	return VO_RET_SOURCE2_OK;
	
}