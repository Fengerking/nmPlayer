#include "Metrics.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

Metrics::Metrics(void)
{
 Delete();
	
	
}

Metrics::~Metrics(void)
{
}
VO_VOID Metrics::Delete(void)
{
    memset(m_byte_range, 0x00, sizeof(m_byte_range));
	memset(m_serviceLocation, 0x00, sizeof(m_serviceLocation));
}

VO_U32 Metrics::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	int hour = 0;
	int minute = 0;
	int second  = 0;
	char* attriValue = NULL;
	int nSize = 0;
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