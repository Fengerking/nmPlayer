
#include "XMLOP.h"
#include "stdlib.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
CXmlOP::CXmlOP(void)
{
	//m_xmlParser =(void*) new XMLChar;
}

CXmlOP::~CXmlOP(void)
{
	Close();
}
int CXmlOP::SetParam(VO_U32 uID, VO_PTR pParam)
{
	switch(uID)
	{
	case VOXML_PID_PARSER_FLUSH:
		return Flush(); 
	case VOXML_PID_SOURCE:
		break;
	case VOXML_PID_PARSERMODE_SAX:
		m_xmlParser.SetSaxHandler(pParam);
		break;
	}
	return 0;
}
int CXmlOP::GetParam(VO_U32 uID, VO_PTR pParam)
{
	return 0;
}
int CXmlOP::Open(VO_XML_PARSER_INIT_INFO *pParam)
{

	return 0;
}
int CXmlOP::Flush()
{
	m_xmlParser.FlushXml();
	return 0;
}
int CXmlOP::Close()
{
	return Flush();
}
int CXmlOP::LoadXML(char *buf, int size,int flag)
{
	Flush();
	char *source = buf;
	if(flag == voXML_FLAG_SOURCE_BUFFER)
		source = buf;
	else if(flag == voXML_FLAG_SOURCE_URL)
		source = buf;
	VO_U32 ret =  m_xmlParser.ParseXml(source,size);
///////////////////////////	//Parser_node(&root,dd);
	//m_xmlParser.PrintXml();
	return ret;
}
void *CXmlOP::GetChildByName( void *pNode, char *pTagName)
{
	return m_xmlParser.GetChildByName(pNode,pTagName);
}
void *CXmlOP::GetFirstChild(void *pNode)
{
	return m_xmlParser.GetFirstChild(pNode);
}
int CXmlOP::GetAttributeValue(void* pNode,char *pAttriName, char **pValue, int &nSize)
{
	*pValue = NULL;
	nSize = 0;
	if(pNode)
	{
		*pValue = m_xmlParser.GetAttributeValue(pNode,pAttriName,nSize);
		if(nSize == 0) return -1;
		return 0;
	}
	
	return -1;
}
void *CXmlOP::GetNextAttribute(void *pNode, void* pCurrent, char **ppAttriName, int &nAttriNameSize, char **ppAttriValue, int &nAttriValuesize)
{
	return m_xmlParser.GetNextAttribute(pNode, pCurrent, ppAttriName, nAttriNameSize, ppAttriValue, nAttriValuesize);
}

// char *CXmlOP::GetAttributeValue(void* pNode,char *pAttriName,int &attriValue)
// {
// 	char *str = m_xmlParser.GetAttributeValue(pNode,pAttriName);
// 	attriValue = atoi(str);
// 	if(pNode)
// 			return str;
// 	return NULL;
// }
void *CXmlOP::GetNextSibling(void *pNode)
{
	if(pNode)
		return m_xmlParser.GetNextSibling(pNode);
	return NULL;
}
int CXmlOP::GetTagValue(void *pNode,char **pValue,int &nSize)
{
	*pValue = NULL;
	nSize = 0;
	if(pNode)
	{
		*pValue = m_xmlParser.GetTagValue(pNode,nSize);
		if(nSize == 0) return -1;
		return 0;
	}

	return -1;
}

int CXmlOP::GetTagName(void *pNode,char **pValue,int &nSize)
{
	*pValue = NULL;
	nSize = 0;
	if(pNode)
	{
		*pValue =  m_xmlParser.GetTagName(pNode,nSize);
		if(nSize == 0) return -1;
		return 0;
	}

	return -1;
}