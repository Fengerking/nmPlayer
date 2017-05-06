#include "CEsgParseMng.h"

#ifdef _USE_XMLSP_

#include "CEsgServiceInfoParser.h"
#include "CEsgContentInfoParser.h"
#include "CEsgScheduleInfoParser.h"
#include "CEsgServiceAuxInfoParser.h"
#include "CEsgServiceParamParser.h"

#else

#include "CEsgParser.h"

#endif



CEsgParseMng::CEsgParseMng(void)
{
}

CEsgParseMng::~CEsgParseMng(void)
{
}

bool CEsgParseMng::ParseContentInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver)
{
#ifdef _USE_XMLSP_
	EsgContentInfo info;
	CEsgContentInfoParser parser(pRecver, &info);
	parser.Parse((char*)pXML);
#else
	CEsgParser parser;
	parser.ParseContentInfo(pXML, nLen, pRecver);
#endif

	return true;
}

bool CEsgParseMng::ParseScheduleInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver)
{
#ifdef _USE_XMLSP_
	EsgScheduleInfo info;
	CEsgScheduleInfoParser parser(pRecver, &info);
	parser.Parse((char*)pXML);
#else
	CEsgParser parser;
	parser.ParseScheduleInfo(pXML, nLen, pRecver);
#endif

	return true;
}

bool CEsgParseMng::ParseServiceAuxInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver)
{
#ifdef _USE_XMLSP_
	EsgServiceAuxInfo info;
	CEsgServiceAuxInfoParser parser(pRecver, &info);
	parser.Parse((char*)pXML);
#else
	CEsgParser parser;
	parser.ParseServiceAuxInfo(pXML, nLen, pRecver);
#endif
	return true;
}

bool CEsgParseMng::ParseServiceInfo(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver)
{
#ifdef _USE_XMLSP_
	EsgServiceInfo info;
	CEsgServiceInfoParser parser(pRecver, &info);
	parser.Parse((char*)pXML);
#else
	CEsgParser parser;
	parser.ParseServiceInfo(pXML, nLen, pRecver);
#endif

	return true;
}

bool CEsgParseMng::ParseServiceParam(unsigned char* pXML, int nLen, CDmxResultReciever* pRecver)
{
#ifdef _USE_XMLSP_
	EsgServiceParamInfo info;
	CEsgServiceParamParser parser(pRecver, &info);
	parser.Parse((char*)pXML);
#else
	CEsgParser parser;
	parser.ParseServiceParam(pXML, nLen, pRecver);
#endif

	return true;
}
