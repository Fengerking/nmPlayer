#pragma once
#include "dmxbasicdef.h"

//#define _USE_XMLSP_

class CEsgParseMng
{
public:
	CEsgParseMng(void);
	~CEsgParseMng(void);

public:
	bool ParseContentInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver);
	bool ParseScheduleInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver);
	bool ParseServiceAuxInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver);
	bool ParseServiceInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver);
	bool ParseServiceParam(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver);
};
