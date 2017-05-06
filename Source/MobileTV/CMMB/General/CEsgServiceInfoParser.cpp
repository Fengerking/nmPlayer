#include "CEsgServiceInfoParser.h"


bool CEsgServiceInfoParser::doTagOpend(const std::string& tag_name, StringMap& attributes)
{
	if(2==m_nParseDepth/* && tag_name=="Service"*/)
	{
		StringMap::const_iterator i = attributes.begin();

		m_pServiceInfo->service_id = atoi(i->second.c_str());
		return true;
	}

	if(tag_name == TAG_SERVICE_NAME)
	{
		StringMap::const_iterator i = attributes.begin();
		MultiByte2WideChar(m_pServiceInfo->service_name_lang, i->second.c_str(), i->second.length());
	}

	return true;
}

bool CEsgServiceInfoParser::doCdataParsed(const std::string& cdata)
{
	if(m_strCurrTagName == TAG_SERVICE_CLASS)
	{
		m_pServiceInfo->service_class = atoi(cdata.c_str());
	}
	else if(m_strCurrTagName == TAG_SERVICE_GENRE)
	{
		m_pServiceInfo->service_genre = atoi(cdata.c_str());
	}
	else if(m_strCurrTagName == TAG_SERVICE_PARA_ID)
	{
		m_pServiceInfo->service_param_id = atoi(cdata.c_str());
	}
	else if(m_strCurrTagName == TAG_SERVICE_NAME_STR)
	{
		memset(m_pServiceInfo->service_name_str, 0, 256);
		MultiByte2WideChar(m_pServiceInfo->service_name_str, cdata.c_str(), cdata.length());
	}
	else if(m_strCurrTagName == TAG_SERVICE_FOR_FREE)
	{
		m_pServiceInfo->for_free = Str2Bool(cdata.c_str());
	}

	return true;
}

bool CEsgServiceInfoParser::doTagClosed(const std::string& tag_name)
{
	if(m_nParseDepth == 2 && m_pRecv && m_pServiceInfo)
		m_pRecv->WhenParsedEsgServiceInfo(m_pServiceInfo);

	return true;
}



