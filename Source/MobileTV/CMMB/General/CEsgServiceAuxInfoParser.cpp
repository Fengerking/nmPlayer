#include "CEsgServiceAuxInfoParser.h"



CEsgServiceAuxInfoParser::~CEsgServiceAuxInfoParser(void)
{
}

bool CEsgServiceAuxInfoParser::doTagOpend(const std::string& tag_name, StringMap& attributes)
{
	if(2 == m_nParseDepth && tag_name==TAG_SERVICE_AUX)
	{
		StringMap::const_iterator i = attributes.begin();
		m_pInfo->service_id = atoi(i->second.c_str());
		return true;
	}

	if(3==m_nParseDepth && tag_name==TAG_SERVICE_LANG_SPC)
	{
		StringMap::const_iterator i = attributes.begin();
		MultiByte2WideChar(m_pInfo->service_lang_special, i->second.c_str(), i->second.length());
		return true;
	}

	return true;
}

bool CEsgServiceAuxInfoParser::doCdataParsed(const std::string& cdata)
{
	if(TAG_SERVICE_DESC == m_strCurrTagName)
	{
		memset(m_pInfo->service_desc, 0, 256);
		MultiByte2WideChar(m_pInfo->service_desc, cdata.c_str(), cdata.length());
	}
	else if(TAG_SERVICE_PROVIDER == m_strCurrTagName)
	{
		memset(m_pInfo->service_provider, 0, 256);
		MultiByte2WideChar(m_pInfo->service_provider, cdata.c_str(), cdata.length());
	}
	else if(TAG_SERVICE_LANGUAGE == m_strCurrTagName)
	{
		memset(m_pInfo->service_lang, 0, 32);
		MultiByte2WideChar(m_pInfo->service_lang, cdata.c_str(), cdata.length());
	}
	else if(TAG_MEDIA_USAGE == m_strCurrTagName)
		m_pInfo->media_type.usage = atoi(cdata.c_str());
	else if(TAG_MEDIA_CONTENT_CLASS == m_strCurrTagName)
	{
		memset(m_pInfo->media_type.content_class, 0, 32);
		MultiByte2WideChar(m_pInfo->media_type.content_class, cdata.c_str(), cdata.length());
	}
	else if(TAG_MEDIA_URI == m_strCurrTagName)
	{
		memset(m_pInfo->media_type.media_uri, 0, 512);
		MultiByte2WideChar(m_pInfo->media_type.media_uri, cdata.c_str(), cdata.length());
	}
	else if(TAG_MEDIA_DATA == m_strCurrTagName)
	{

	}

	
	return true;
}

bool CEsgServiceAuxInfoParser::doTagClosed(const std::string& tag_name)
{
	if(2 == m_nParseDepth && m_pRecv && m_pInfo)
		m_pRecv->WhenParsedEsgServiceAuxInfo(m_pInfo);

	return true;
}

