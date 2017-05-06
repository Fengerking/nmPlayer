#pragma once
#include "dmxbasicdef.h"
#include "CEsgXmlBaseParser.h"

class CEsgContentInfoParser : public CEsgXmlBaseParser
{
public:
	CEsgContentInfoParser(CDmxResultReciever* pRcv, EsgContentInfo* pInfo)
	{
		m_pRecv			= pRcv;
		m_pContentInfo	= pInfo;
	};
	virtual ~CEsgContentInfoParser(void);

protected:
	virtual bool doTagOpend(const std::string& tag_name, StringMap& attributes);
	virtual bool doCdataParsed(const std::string& cdata);
	virtual bool doTagClosed(const std::string& tag_name);

private:
	EsgContentInfo* m_pContentInfo;

};
