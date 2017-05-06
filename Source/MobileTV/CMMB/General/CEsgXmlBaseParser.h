#pragma once

#include "xmlsp.h"
//#include <tchar.h>
//#include "dmemleak.h"


using namespace std;
using namespace XMLSP;

class CDmxResultReciever;

class CEsgXmlBaseParser : public Parser
{
public:
	CEsgXmlBaseParser(void);
	virtual ~CEsgXmlBaseParser(void);

public:
	int Parse(char* xml);
// 	int Parse(TCHAR* pszXmlFile);

public:
	// virtual functions from base class(Psrser)
	virtual bool on_tag_open(const std::string& tag_name, StringMap& attributes);
	virtual bool on_cdata(const std::string& cdata);
	virtual bool on_tag_close(const std::string& tag_name);
	//virtual bool on_comment(const std::string& comment);
	//virtual bool on_processing(const std::string& value);
	//virtual bool on_doctype(const std::string& value);
	virtual bool on_document_begin();
	virtual bool on_document_end();
	virtual void on_error(int errnr, int line, int col, const std::string& message);

protected:
	virtual bool doTagOpend(const std::string& tag_name, StringMap& attributes){return true;};
	virtual bool doCdataParsed(const std::string& cdata){return true;};

	// It must be implemented by derived class to issue the parsed result.
	virtual bool doTagClosed(const std::string& tag_name)=0;

	bool Str2Bool(const char* pStr);

	// 20100226
	void MultiByte2WideChar(VO_TCHAR* pWideChar, const char* pMultiByte, int nLen);

protected:
	int					m_nErrorCode;
	int					m_nParseDepth;
	string				m_strCurrTagName;
	CDmxResultReciever*	m_pRecv;
};

typedef enum
{
	XML_OK		= 0,
	XML_TAG_NOT_CLOSED,
	XML_MANY_ROOT_TAGS,
	XML_UNEXPECTED_CDATA,
	XML_UNEXPECTED_CHAR,
	XML_INVALID_ENTITY,
	XML_UNKNOWN_ELEMENT,
	XML_BLOCK_NOT_CLOSED,
	XML_TAG_NOT_FINISHED,
	XML_MULTIPL_ROOT,
	XML_TAG_MISMATCH,
	XML_INTERNAL,
	XML_UNKNOWN_ERROR
}XML_ERROR_CODE;
