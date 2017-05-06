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


VO_U32 ProgramInformation::Init(TiXmlNode* pNode)
{
	if(!pNode)
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	int hour = 0;
	int minute = 0;
	int second  = 0;
	const char* attriValue = pNode->ToElement()->Attribute(PRO_LANG);
	if(attriValue)
	{
		  memcpy(m_chLang, attriValue, strlen(attriValue));
		  m_chLang[strlen(attriValue)] = '\0';
	}
	attriValue = pNode->ToElement()->Attribute(PRO_MORE_URL);
	if(attriValue)
	{
		 memcpy(m_more_Url, attriValue, strlen(attriValue));
		 m_more_Url[strlen(attriValue)] = '\0';
	
	}
	TiXmlNode* pChildNode = pNode->FirstChild();
	while(pChildNode)
	{
		const char* pTag_Name = pChildNode->Value();
		attriValue = pChildNode->ToElement()->Value();
		if(m_comtag->StrCompare(pTag_Name,TITLE_TAG)==0)
		{
			if(attriValue)
			{
				memcpy(m_title, attriValue, strlen(attriValue));
				m_title[strlen(attriValue)] = '\0';
			}
		}
		if(m_comtag->StrCompare(pTag_Name,SOURCE_TAG)==0)
		{
			if(attriValue)
			{
				memcpy(m_source, attriValue, strlen(attriValue));
				m_source[strlen(attriValue)] = '\0';
			}
		}
		if(m_comtag->StrCompare(pTag_Name,COPY_TAG)==0)
		{
			if(attriValue)
			{
				memcpy(m_source, attriValue, strlen(attriValue));
				m_copy_right[strlen(attriValue)] = '\0';
			}
		}
		pChildNode = pChildNode->NextSibling();

	}

	return VO_RET_SOURCE2_OK;
	
}