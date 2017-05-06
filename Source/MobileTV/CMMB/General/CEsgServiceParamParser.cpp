#include "CEsgServiceParamParser.h"

CEsgServiceParamParser::~CEsgServiceParamParser(void)
{
}

bool CEsgServiceParamParser::doTagOpend(const std::string& tag_name, StringMap& attributes)
{
	if(2==m_nParseDepth && TAG_SERVICE_PARA_ID==tag_name)
	{
		StringMap::const_iterator i = attributes.begin();
		m_pInfo->service_param_id = atoi(i->second.c_str());
	}

	return true;
}

bool CEsgServiceParamParser::doCdataParsed(const std::string& cdata)
{
	if(TAG_SERVICE_PARA_DATA == m_strCurrTagName)
	{
		
	}
	return true;
}

bool CEsgServiceParamParser::doTagClosed(const std::string& tag_name)
{
	if(2 == m_nParseDepth && m_pInfo && m_pRecv)
		m_pRecv->WhenParsedEsgServiceParamInfo(m_pInfo);

	return true;
}

