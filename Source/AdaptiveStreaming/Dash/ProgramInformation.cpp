#include "ProgramInformation.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

ProgramInformation::ProgramInformation()
{
	Delete();
}

ProgramInformation::~ProgramInformation()
{
}
VO_VOID ProgramInformation::Delete()
{
	memset(m_chLang, 0x00, sizeof(m_chLang));
	memset(m_more_Url, 0x00, sizeof(m_more_Url));
	memset(m_title, 0x00, sizeof(m_title));
	memset(m_source, 0x00, sizeof(m_source));
	memset(m_copy_right, 0x00, sizeof(m_copy_right));
}


VO_U32 ProgramInformation::Init(CXMLLoad *m_pXmlLoad,VO_VOID* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	int hour = 0;
	int minute = 0;
	int second  = 0;
	int nSize = 0;
	char * attriValue = NULL;
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(PRO_LANG),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
		  memcpy(m_chLang, attriValue, strlen(attriValue));
		  m_chLang[strlen(attriValue)] = '\0';
		}
	}
	if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(PRO_MORE_URL),&attriValue,nSize) == VO_ERR_NONE)
	{
		if(attriValue)
		{
		 memcpy(m_more_Url, attriValue, strlen(attriValue));
		 m_more_Url[strlen(attriValue)] = '\0';
		}
	}

	VO_VOID* pChildNode = NULL;
	m_pXmlLoad->GetFirstChild(pChildNode,&pChildNode);
	while(pChildNode)
	{
		 char* pTag_Name = NULL;
		 m_pXmlLoad->GetTagName(pChildNode,&pTag_Name,nSize);
		if(m_pXmlLoad->GetAttributeValue(pNode,(char*)(PRO_MORE_URL),&attriValue,nSize) == VO_ERR_NONE)
		{
			if(StrCompare(pTag_Name,TITLE_TAG)==0)
		{
			if(attriValue)
			{
				memcpy(m_title, attriValue, strlen(attriValue));
				m_title[strlen(attriValue)] = '\0';
			}
		}
		if(StrCompare(pTag_Name,SOURCE_TAG)==0)
		{
			if(attriValue)
			{
				memcpy(m_source, attriValue, strlen(attriValue));
				m_source[strlen(attriValue)] = '\0';
			}
		}
		if(StrCompare(pTag_Name,COPY_TAG)==0)
		{
			if(attriValue)
			{
				memcpy(m_source, attriValue, strlen(attriValue));
				m_copy_right[strlen(attriValue)] = '\0';
			}
		}
		}
		m_pXmlLoad->GetNextSibling(pChildNode,&pChildNode);

	}


	return VO_RET_SOURCE2_OK;
	
}