#pragma once
#include "dmxbasicdef.h"
#include "CEsgXmlBaseParser.h"



class CEsgServiceAuxInfoParser : public CEsgXmlBaseParser
{
public:
	CEsgServiceAuxInfoParser(CDmxResultReciever* pRecv, EsgServiceAuxInfo* pInfo)
	{
		m_pRecv = pRecv;
		m_pInfo = pInfo;
	};
	virtual ~CEsgServiceAuxInfoParser(void);

protected:
	virtual bool doTagOpend(const std::string& tag_name, StringMap& attributes);
	virtual bool doCdataParsed(const std::string& cdata);
	virtual bool doTagClosed(const std::string& tag_name);

private:
	EsgServiceAuxInfo* m_pInfo;
};
