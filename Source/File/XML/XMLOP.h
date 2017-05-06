

#ifndef __CXmlOP_H__
#define __CXmlOP_H__

#include "voXMLParser.h"
#include "CXMLParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
class CXmlOP
{
public:
	CXmlOP(void);
	~CXmlOP(void);
public:
	int LoadXML(char *buf, int size,int flag); 
	void *GetFirstChild(void *pNode);/* pNode[in] ,node pointer *, if NULL return Root node */ 

	void *GetNextSibling(void *pNode); 

	void *GetChildByName( void *pNode, char *pTagName); 

	int GetAttributeValue(void* pNode,char *pAttriName,char **pValue,int &nSize);  
	void *GetNextAttribute(void *pNode, void* pCurrent, char **ppAttriName, int &nAttriNameSize, char **ppAttriValue, int &nAttriValuesize);

	//int GetAttributeValue(void* pNode,char *pAttriName,int &attriValue);/* attriValue[out] , & */ 

	int GetTagValue(void *pNode,char **pValue,int &nSize);  

	int GetTagName(void *pNode,char **pValue,int &nSize); 

	int SetParam(VO_U32 uID, VO_PTR pParam);
	int GetParam(VO_U32 uID, VO_PTR pParam);

	int Close();
	int Open(VO_XML_PARSER_INIT_INFO *pParam);
private:	
	int Flush();

#ifdef __UNICODE
	CXMLParser<short> m_xmlParser;
#else
	CXMLParser<char> m_xmlParser;
#endif
	//void *m_xmlParser;
};

#ifdef _VONAMESPACE
}
#endif

#endif