#include "BaseUrl.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

BaseUrl::BaseUrl(void)
{
	Reset();
	
}

BaseUrl::~BaseUrl(void)
{
}

VO_VOID BaseUrl::Reset()
{

	memset(m_byte_range, 0x00, sizeof(m_byte_range));
	memset(m_serviceLocation, 0x00, sizeof(m_serviceLocation));
	memset(p_value,0x00, sizeof(p_value));

}

VO_U32 BaseUrl::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	int hour = 0;
	int minute = 0;
	int second  = 0;
	char* attriValue = NULL;
	int nSize = 0;
	if(m_pXmlLoad->GetTagValue(pNode,&attriValue,nSize)== VO_ERR_NONE)
	{
		if(attriValue)
		{
		//	memcpy(p_value,attriValue,nSize);
			strcpy(p_value,attriValue);
		}
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(BYTE_RANGE),&attriValue,nSize) == VO_ERR_NONE)
	{
	if(attriValue)
	{
		  memcpy(m_byte_range, attriValue, strlen(attriValue));
		  m_byte_range[strlen(attriValue)] = '\0';
	}
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(BASE_LOCATION),&attriValue,nSize) == VO_ERR_NONE)
	{
	if(attriValue)
	{
		 memcpy(m_serviceLocation, attriValue, strlen(attriValue));
		 m_serviceLocation[strlen(attriValue)] = '\0';
	}
	}	
	return VO_RET_SOURCE2_OK;
	
}