#pragma once
#include "dmxbasicdef.h"
#include "CEsgXmlBaseParser.h"


class CEsgServiceParamParser : public CEsgXmlBaseParser
{
public:
	CEsgServiceParamParser(CDmxResultReciever* pRecv, EsgServiceParamInfo* pInfo)
	{
		m_pRecv = pRecv;
		m_pInfo = pInfo;
	};
	virtual ~CEsgServiceParamParser(void);

protected:
	virtual bool doTagOpend(const std::string& tag_name, StringMap& attributes);
	virtual bool doCdataParsed(const std::string& cdata);
	virtual bool doTagClosed(const std::string& tag_name);

private:
	EsgServiceParamInfo* m_pInfo;
};
