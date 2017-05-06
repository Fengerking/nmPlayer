#include "CEsgScheduleInfoParser.h"


CEsgScheduleInfoParser::~CEsgScheduleInfoParser(void)
{
}

bool CEsgScheduleInfoParser::doTagOpend(const std::string& tag_name, StringMap& attributes)
{
	if(1 == m_nParseDepth)
	{
		return tag_name == ESG_BASE_TAG;
	}

	if(2 == m_nParseDepth)
	{
		StringMap::const_iterator i = attributes.begin();
		m_pInfo->schedule_id = atoi((i->second).c_str());
		return true;
	}

	if(tag_name == TAG_SCHEDULE_TITLE)
	{
		StringMap::const_iterator i = attributes.begin();
		MultiByte2WideChar(m_pInfo->lang, i->second.c_str(), i->second.length());
	}

	return true;
}

bool CEsgScheduleInfoParser::doCdataParsed(const std::string& cdata)
{
	if(TAG_SCHEDULE_SERVICE_ID == m_strCurrTagName)
		m_pInfo->service_id = atoi(cdata.c_str());
	else if(TAG_CONTENT_ID == m_strCurrTagName)
		m_pInfo->content_id = atoi(cdata.c_str());
	else if(TAG_TITLE_STR == m_strCurrTagName)
		MultiByte2WideChar(m_pInfo->title, cdata.c_str(), cdata.length());
	else if(TAG_DATE == m_strCurrTagName)
		MultiByte2WideChar(m_pInfo->date, cdata.c_str(), cdata.length());
	else if(TAG_TIME == m_strCurrTagName)
		MultiByte2WideChar(m_pInfo->time, cdata.c_str(), cdata.length());
	else if(TAG_FOR_FREE == m_strCurrTagName)
		m_pInfo->for_free = Str2Bool(cdata.c_str());
	//else if(TAG_CA_DESC == m_strCurrTagName)
		//m_pInfo->schedule_id = atoi(cdata.c_str());
	else if(TAG_LIVE == m_strCurrTagName)
		m_pInfo->live = Str2Bool(cdata.c_str());
	else if(TAG_REPEAT == m_strCurrTagName)
		m_pInfo->repeat = Str2Bool(cdata.c_str());


	return true;
}

bool CEsgScheduleInfoParser::doTagClosed(const std::string& tag_name)
{
	if(m_nParseDepth==2 && m_pRecv && m_pInfo)
		m_pRecv->WhenParsedEsgScheduleInfo(m_pInfo);

	return true;
}

