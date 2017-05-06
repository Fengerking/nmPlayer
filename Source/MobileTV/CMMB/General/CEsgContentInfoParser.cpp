#include "CEsgContentInfoParser.h"

CEsgContentInfoParser::~CEsgContentInfoParser(void)
{
}


bool CEsgContentInfoParser::doTagOpend(const std::string& tag_name, StringMap& attributes)
{
	if(2 == m_nParseDepth)
	{
		StringMap::const_iterator i = attributes.begin();
		m_pContentInfo->content_id = atoi((i->second).c_str());
		return true;
	}

	if(tag_name == TAG_CONTENT_LANG_SPECIAL)
	{
		StringMap::const_iterator i = attributes.begin();
		MultiByte2WideChar(m_pContentInfo->content_lang_special, i->second.c_str(), i->second.length());
	}

	return true;
}

bool CEsgContentInfoParser::doCdataParsed(const std::string& cdata)
{
	if(m_strCurrTagName == TAG_CONTENT_CLASS)
	{
		MultiByte2WideChar(m_pContentInfo->content_class, cdata.c_str(), cdata.length());
	}
	else if(m_strCurrTagName == TAG_CONTENT_GENRE)
	{
		m_pContentInfo->content_genre = atoi(cdata.c_str());
	}
	else if(m_strCurrTagName == TAG_DURATION)
	{
		m_pContentInfo->duration = atoi(cdata.c_str());
	}
	else if(m_strCurrTagName == TAG_DIGEST_INFO)
	{
		MultiByte2WideChar(m_pContentInfo->digest_info, cdata.c_str(), cdata.length());
	}
	else if(m_strCurrTagName == TAG_SCHEDULE_TITLE)
	{
		MultiByte2WideChar(m_pContentInfo->title, cdata.c_str(),cdata.length());

// 		OutputDebugString(m_pContentInfo->title);
// 		OutputDebugString(_T("\r\n"));
	}
	else if(m_strCurrTagName == TAG_AUDIO_LANG)
	{
		MultiByte2WideChar(m_pContentInfo->audio_lang, cdata.c_str(),cdata.length());
	}
	else if(m_strCurrTagName == TAG_SUBTITLE_LANG)
	{
		MultiByte2WideChar(m_pContentInfo->subtitle_lang, cdata.c_str(),cdata.length());
	}

	return true;
}

bool CEsgContentInfoParser::doTagClosed(const std::string& tag_name)
{
	if(2==m_nParseDepth && m_pRecv && m_pContentInfo)
		m_pRecv->WhenParsedEsgContentInfo(m_pContentInfo);

	return true;
}

