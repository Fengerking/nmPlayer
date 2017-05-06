#pragma once

#include "dmxbasicdef.h"
#include "CEsgXmlBaseParser.h"


class CEsgServiceInfoParser : public CEsgXmlBaseParser
{
public:
	CEsgServiceInfoParser(CDmxResultReciever* pRcv, EsgServiceInfo* pInfo)
	{
		m_pRecv			= pRcv;
		m_pServiceInfo	= pInfo;
	};
	virtual ~CEsgServiceInfoParser(void){};

protected:
	virtual bool doTagOpend(const std::string& tag_name, StringMap& attributes);
	virtual bool doCdataParsed(const std::string& cdata);
	virtual bool doTagClosed(const std::string& tag_name);

private:
	EsgServiceInfo* m_pServiceInfo;
};
