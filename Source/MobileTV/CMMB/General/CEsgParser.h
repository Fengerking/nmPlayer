#pragma once
#include "dmxbasicdef.h"
#include "tinyxml.h"
#include "voString.h"

class CEsgParser
{
public:
	CEsgParser(void);
	virtual ~CEsgParser(void);

public:
	bool ParseContentInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver);
	bool ParseScheduleInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver);
	bool ParseServiceAuxInfo(unsigned char* pXML, int nLen,  CDmxResultReciever* pRecver);
	bool ParseServiceInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver);
	bool ParseServiceParam(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver);

private:
	bool IsTheSameTag(TiXmlNode* pNode, char* pszTag);
	int  StrCompare(const char* p1, const char* p2);
	bool Str2Bool(const char* pStr);
	void MultiByte2WideChar(VO_TCHAR* pWideChar, const char* pMultiByte, int nLen);
};
