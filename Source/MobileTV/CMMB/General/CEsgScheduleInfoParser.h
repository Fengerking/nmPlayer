#pragma once
#include "dmxbasicdef.h"
#include "CEsgXmlBaseParser.h"



class CEsgScheduleInfoParser : public CEsgXmlBaseParser
{
public:
	CEsgScheduleInfoParser(CDmxResultReciever* pRecv, EsgScheduleInfo* pInfo)
	{
		m_pRecv = pRecv;
		m_pInfo = pInfo;
	};
	virtual ~CEsgScheduleInfoParser(void);

protected:
	virtual bool doTagOpend(const std::string& tag_name, StringMap& attributes);
	virtual bool doCdataParsed(const std::string& cdata);
	virtual bool doTagClosed(const std::string& tag_name);

private:
	EsgScheduleInfo* m_pInfo;

};
