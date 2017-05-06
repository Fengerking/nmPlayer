#pragma once
#ifndef __CXMLLoad_H__
#define __CXMLLoad_H__

#include "voYYDef_Common.h"
#include "CDllLoad.h"
#include "voXMLParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CXMLLoad: public CDllLoad
{
public:
	CXMLLoad(void);
	~CXMLLoad(void);
	VO_U32 OpenParser();
	VO_U32 CloseParser();

public:
	VO_U32 LoadXML (char *buf, int nSize,int nFlag);
	/* \param pNode[in] ,node pointer *
	*  \param pChildNode[out] , if pNode is NULL then pChildNode return Root node 
	*/ 
	VO_U32 GetFirstChild (void *pNode,void **pChildNode);

	VO_U32 GetNextSibling (void *pNode,void **pChildNode);
	VO_U32 GetChildByName (void *pNode, char *pTagName,void **pChildNode);
	VO_U32 GetAttributeValue (void *pNode, char *pAttriName,char **pAttriValue,int &nSize);
	VO_U32 GetNextAttribute( void *pNode, void **ppNext, void* pCurrent, char **ppAttriName, int &nAttriNameSize, char **ppAttriValue, int &nAttriValuesize);

	VO_U32 GetTagValue(void *pNode,char **pTagValue,int &nSize);
	VO_U32 GetTagName(void *pNode,char **pTagName,int &nSize);

	VO_U32 SetParam(VO_U32 id, VO_PTR pParam);
	VO_U32 GetParam(VO_U32 id, VO_PTR pParam);
private:
	VO_PTR				m_parser_handle;
	VO_XML_PARSER_API		m_parser_api;
};
	
#ifdef _VONAMESPACE
}
#endif

#endif
