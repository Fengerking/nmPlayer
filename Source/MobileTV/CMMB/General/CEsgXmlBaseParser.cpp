#include "CEsgServiceInfoParser.h"

CEsgXmlBaseParser::CEsgXmlBaseParser(void)
: m_nParseDepth(0)
, m_nErrorCode(E_OK)
, m_strCurrTagName("")
, m_pRecv(NULL)
{
}

CEsgXmlBaseParser::~CEsgXmlBaseParser(void)
{
}

int CEsgXmlBaseParser::Parse(char* xml)
{
	bool bRet = parse(xml);
	return m_nErrorCode;

/*
	if(!bRet)
	{
		// get error code
		return m_nErrorCode;
	}

	return XML_OK;
	*/
}

/*
int CEsgXmlBaseParser::Parse(TCHAR* pszXmlFile)
{
	return XML_UNKNOWN_ERROR;
}
*/

void CEsgXmlBaseParser::MultiByte2WideChar(VO_TCHAR* pWideChar, const char* pMultiByte, int nLen)
{
#ifdef _WIN32
	MultiByteToWideChar(CP_UTF8, 0, pMultiByte, -1, pWideChar, nLen);
#else
	memcpy(pWideChar, pMultiByte, nLen);
#endif
}

bool CEsgXmlBaseParser::Str2Bool(const char* pStr)
{
	return !strcmp(ESG_VALUE_TRUE, pStr);
}

bool CEsgXmlBaseParser::on_tag_open(const std::string& tag_name, StringMap& attributes)
{
	m_nParseDepth++;
	m_strCurrTagName = tag_name;

	if(1 == m_nParseDepth)
		return m_strCurrTagName == ESG_BASE_TAG;

	return doTagOpend(m_strCurrTagName, attributes);
}

bool CEsgXmlBaseParser::on_cdata(const std::string& cdata)
{
	bool bRet = doCdataParsed(cdata);

	// NOTE: don't get value again until next tag opened.
	m_strCurrTagName = "";

	return bRet;
}

bool CEsgXmlBaseParser::on_tag_close(const std::string& tag_name)
{
	bool bRet = doTagClosed(tag_name);
	
	m_nParseDepth--;
	
	return bRet;
}

bool CEsgXmlBaseParser::on_document_begin()
{
	m_nParseDepth = 0;

	return true;
}

bool CEsgXmlBaseParser::on_document_end()
{
	m_nParseDepth = 0;

	return true;
}

void CEsgXmlBaseParser::on_error(int errnr, int line, int col, const std::string& message)
{
	m_nErrorCode = errnr;
}
