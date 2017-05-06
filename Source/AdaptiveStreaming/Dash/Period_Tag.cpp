#include "Period_Tag.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

Period_Tag::Period_Tag(void)
{

	Delete();

}

Period_Tag::~Period_Tag(void)
{
}
VO_VOID Period_Tag::Delete()
{
	m_duration = 0;
	m_start = 0;
	m_bitSwitch = VO_FALSE;
	m_uID = 0;
	m_start = -1; 
}

VO_U32 Period_Tag::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	int hour = 0;
	int minute = 0;
	int second  = 0;
	int nSize = 0;
	char* attriValue = NULL;
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(PERIOD_DURATION),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
			VO_CHAR	duration[512];
			memcpy(duration,attriValue,strlen(attriValue));
			duration[strlen(attriValue)] = '\0';
			m_duration = Compute_Time_Duration(duration);
			VOLOGI("Period_Tag m_duration: %lld ", m_duration);

		}
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(PERIOD_START),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
			VO_CHAR	m_duration[512];
			memcpy(m_duration,attriValue,strlen(attriValue));
			m_duration[strlen(attriValue)] = '\0';
			m_start = Compute_Time_Duration(m_duration);
			VOLOGI("Period_Tag  m_start_time: %lld ", m_duration);

		}
	}

	return VO_RET_SOURCE2_OK;

}