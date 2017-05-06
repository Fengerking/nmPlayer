#pragma once
#ifndef __CXMLSaxDefaultHandler_H__
#define __CXMLSaxDefaultHandler_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CXMLSaxDefaultHandler
{
public:
	CXMLSaxDefaultHandler(void);
	virtual ~CXMLSaxDefaultHandler(void);
public:
	virtual void startDocument();
	virtual void endDocument();
	virtual void startElement(char *tagName, char **attriName, char **attriValue, int attriLength);
	virtual void endElement(char *tagName);
	virtual void characters( char* chars);
};
	
#ifdef _VONAMESPACE
}
#endif

#endif