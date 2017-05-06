
#ifndef __CXMLParser_H__
#define __CXMLParser_H__

#include <stdio.h>
#include <string.h>
#include "CPtrList.h"
#include "XMLSaxDefaultHandler.h"

#include "voXMLBase.h"
//#define __UNICODE

#define ISALPHA(c) ((((c >='a' && c <= 'z') || (c >= 'A' && c<= 'Z'))) ? true :false )
#define ISDIGIT(c)   ((c >= 0 && c<= 9 ) ? 1 : 0)
#define ISSPACE(c)  (c == ' ' ? 1 : 0)
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
template<typename T>
struct VO_XML_STR
{
	T *pBuf;
	int nSize;
	int nLen;
};

template<typename T>
struct VO_XML_ATTRIBUTE
{
	VO_XML_STR<T> *pName;
	VO_XML_STR<T> *pValue;
};

template<typename T>
struct  VO_XML_NODE
{
	VO_XML_STR<T> *pName;
	VO_XML_STR<T> *pValue;
	CObjectList<VO_S32> pAttributeList; /*VO_XML_ATTRIBUTE**/ 
	CObjectList<VO_S32>	pChildNodeList;/* VO_XML_NODE**/
	VO_XML_NODE<T> *pFatherNode;/*VO_XML_NODE* */
	POSITION pos;
};


template <class T>
class CXMLParser
{
public:
	CXMLParser();
	~CXMLParser();
	int ParseXml(char *xml, int nSize);	
	VO_XML_NODE<T> m_Root;

	void SetSaxHandler(void *pSaxHandler);

	void PrintXml();
	void FlushXml();
	void *GetFirstChild(void *pParam);
	void *GetChildByName( void *pParam, char *pTagName);
	void *GetNextSibling(void *pParam);
	char *GetAttributeValue(void* pParam, char *pAttriName, int &nSize);
	void *GetNextAttribute( void *pParam, void* pCurrent, char **ppAttriName, int &nAttriNameSize, char **ppAttriValue, int &nAttriValuesize);

	char *GetTagValue(void *pNode, int &size);
	char *GetTagName( void *pParam,int &size);
private:
	void	IssueParseResult(VO_U32 nType, VO_XML_NODE<T> *node, VO_VOID* pReserved=NULL);

	int Parser_node(VO_XML_NODE<T> *fNode,int &size);
	int Parser_node(int &size);

	void ReleaseXml(VO_XML_NODE<T>* fNode);
	void Printall(VO_XML_NODE<T>* fNode,int levels);
	void AddStr(VO_XML_STR<T> *str, T c);
	int m_nSize;
	T *m_pXmlBuf;
	T *m_pSourceXmlBuf;

	CXMLSaxDefaultHandler *m_pXMLSaxHandler;
	//VOXML_PID_PARSERMODE_SAX or VOXML_PID_PARSERMODE_DOM
};
template <class T>
void CXMLParser<T>::SetSaxHandler(void *pSaxHandler)
{
	m_pXMLSaxHandler = (CXMLSaxDefaultHandler*)pSaxHandler;
}
template <class T>
char *CXMLParser<T>::GetTagValue( void *pParam, int &size)
{
	if(!pParam) return NULL;
	VO_XML_NODE<T> *pNode = (VO_XML_NODE<T>*)pParam;
	size = pNode->pValue->nSize;
	return (char*)pNode->pValue->pBuf;
}
template <class T>
char *CXMLParser<T>::GetTagName( void *pParam,int &size)
{
	if(!pParam) return NULL;
	VO_XML_NODE<T> *pNode = (VO_XML_NODE<T>*)pParam;
	size = pNode->pName->nSize;
	return (char*)pNode->pName->pBuf;	
}


template <class T>
char *CXMLParser<T>::GetAttributeValue( void *pParam,char *pAttriName,int &size)
{
	VO_XML_NODE<T> *pNode = (VO_XML_NODE<T>*)pParam;
	POSITION pos = pNode->pAttributeList.GetHeadPosition();
	while (pos)
	{
		VO_XML_ATTRIBUTE<T> *attri = (VO_XML_ATTRIBUTE<T>*)pNode->pAttributeList.GetNext(pos);
		if(strcmp((char*)pAttriName, (char*)attri->pName->pBuf) == 0)
		{
			size = attri->pValue->nSize;
			return  (char*)attri->pValue->pBuf;
		}
	}
	return NULL;
}
template <class T>
void *CXMLParser<T>::GetNextAttribute(void *pParam, void* pCurrent, char **ppAttriName, int &nAttriNameSize, char **ppAttriValue, int &nAttriValuesize)
{
	VO_XML_NODE<T> *pNode = (VO_XML_NODE<T>*)pParam;

	VO_XML_ATTRIBUTE<T> *pNextAttri = NULL;
	
	POSITION pos = ( POSITION )pCurrent;
	pos = pNode->pAttributeList.GetHeadPosition();
	while (pos)
	{
		VO_XML_ATTRIBUTE<T> *pAttri = (VO_XML_ATTRIBUTE<T>*)pNode->pAttributeList.GetNext(pos);
		if( NULL == pCurrent )
			pNextAttri = pAttri;
		else if(pCurrent == pAttri)
			pNextAttri = (VO_XML_ATTRIBUTE<T>*)pNode->pAttributeList.GetNext(pos);

		if(pNextAttri)
			break;
	}

	if( pNextAttri )
	{	
		*ppAttriName = (char*)pNextAttri->pName->pBuf;
		nAttriNameSize = pNextAttri->pName->nSize;
		*ppAttriValue = (char*)pNextAttri->pValue->pBuf;
		nAttriValuesize = pNextAttri->pValue->nSize;
	}

	return (void*)pNextAttri;
}

template <class T>
void *CXMLParser<T>::GetNextSibling( void *pParam)
{
	if(!pParam) return NULL;
	VO_XML_NODE<T> *pNode = (VO_XML_NODE<T>*)pParam;
	VO_XML_NODE<T> *pFather = pNode->pFatherNode;
	POSITION pos = pNode->pos;
	VO_XML_NODE<T> *node = (VO_XML_NODE<T>*)pFather->pChildNodeList.GetNext(pos);
	if(pParam == node)
		return (void*)pFather->pChildNodeList.GetNext(pos);

	
	/*POSITION pos = pFather->pChildNodeList.GetHeadPosition();
	while (pos)
	{
		VO_XML_NODE<T> *node = (VO_XML_NODE<T>*)pFather->pChildNodeList.GetNext(pos);
		if(pParam == node)
			return (void*)pFather->pChildNodeList.GetNext(pos);
	}*/
	return NULL;
}
template <class T>
void *CXMLParser<T>::GetChildByName( void *pParam, char *pTagName)
{
	VO_XML_NODE<T> *pNode;
	if(pParam == NULL)
		pNode = &m_Root;
	else 
		pNode = (VO_XML_NODE<T>*)pParam;
	POSITION pos = pNode->pChildNodeList.GetHeadPosition();
	while (pos)
	{
		VO_XML_NODE<T> *node = (VO_XML_NODE<T>*)pNode->pChildNodeList.GetNext(pos);
		if(strcmp((char*)pTagName, (char*)node->pName->pBuf) == 0)
			return (void*)node;
	}
	return NULL;
}
template <class T>
void *CXMLParser<T>::GetFirstChild( void *pParam)
{
	VO_XML_NODE<T> *pNode;
	if(pParam == NULL)
		pNode = &m_Root;
	else 
		pNode = (VO_XML_NODE<T>*)pParam;

	if(pNode->pChildNodeList.GetCount() >0)
		return (void*)pNode->pChildNodeList.GetHead();
	return NULL;
}
template <class T>
void CXMLParser<T>::AddStr(VO_XML_STR<T> *str, T c)
{
	if(!str )
		return ;
	if(str->nSize+1 >=str->nLen)
	{
		int len = 20+ str->nLen;
		T *tmpBuf = new T[len];
		memset(tmpBuf ,0x00 ,len );
		if(str->pBuf)
		{
			memcpy(tmpBuf,str->pBuf, str->nLen );
			delete []str->pBuf;
			str->pBuf = NULL;
		}
		str->pBuf = tmpBuf;
		str->nLen = len;
	}
	str->pBuf[str->nSize] = c;
	str->nSize ++;
}

template <class T>
CXMLParser<T>::CXMLParser()
{
	memset(&m_Root,0x00,sizeof(VO_XML_NODE<T>));

	m_pXMLSaxHandler = NULL;
	m_pSourceXmlBuf = NULL;
}
template <class T>
CXMLParser<T>::~CXMLParser()
{
	FlushXml();
	if( m_pSourceXmlBuf )
		delete []m_pSourceXmlBuf;
	m_pSourceXmlBuf = NULL;
}
template <class T>
int CXMLParser<T>::ParseXml(char *xml, int nSize)
{
#if 0
	T *pXmlBuf = (T*)xml;
	m_pSourceXmlBuf = m_pXmlBuf = new T[nSize];
	T * pTmpXmlBuf = m_pXmlBuf;
	memset(m_pXmlBuf, 0x00, nSize * sizeof(T));
	m_nSize  = 0 ;

	while( nSize != 0 )
	{
		T c = *pXmlBuf;
		if(c != 0x0d && c != 0x0a)
		{
			*pTmpXmlBuf = c;
			m_nSize ++;
			++ pTmpXmlBuf ;
		}

		++ pXmlBuf;
		nSize --;
	}
#else
	m_pXmlBuf = (T*)xml;
	m_nSize = nSize;
#endif
	//m_nSize = nSize;
	if(m_pXMLSaxHandler == NULL)
		Parser_node(&m_Root,m_nSize);
	else
	{
		IssueParseResult(VO_XML_ACTION_STARTDOCUMENT,NULL);
		Parser_node(m_nSize);
		IssueParseResult(VO_XML_ACTION_ENDDOCUMENT,NULL);
	}
	return 0;
}

template <class T>
int CXMLParser<T>::Parser_node(int &size)
{
	T c;
	//T *pBuf = NULL;

	int result = 0;
	VO_XML_STR<T> *pBuf = NULL;
	VO_XML_NODE<T> *node = NULL;
	VO_XML_ATTRIBUTE<T> *attri = NULL;
	c =*m_pXmlBuf;

	bool bNodeEnd = false;
	int nAttriValue = 0;
	bool bAttriName = false;
	bool bNodeName = true;
	bool bNodeValue = false;
	int nNodeEnd = 0;
	while( size -- >0)
	{
		c= *m_pXmlBuf++;
		//printf("%c", c);
		if(c == '<')
		{	
			if(*m_pXmlBuf == '?' || *m_pXmlBuf == '!' )
			{
				while(c!= '>'){ c= *m_pXmlBuf++; size--;}
			}
			//if( *m_pXmlBuf == '/')return 2;
			bNodeValue = false;
			bAttriName = false;
			nAttriValue = 0;
			nNodeEnd = 0;

			IssueParseResult(VO_XML_ACTION_STARTELEMENT,node);
			int ret = Parser_node(size) ;
			if(size <= 0) { result = -1; break;}
			if(ret == -1) 
			{
				node = NULL;
				continue ;
			}
			if(ret == 1)
			{
				//node = NULL;
				continue;
			}
			if(ret == 2)continue;
			else if(ret == 3)
			{
				result = 1; break;
			}
			else
			{
				c= *m_pXmlBuf++;
				continue;
			}
		}
		else if( (ISALPHA(c) || ISDIGIT(c) || bNodeValue || nAttriValue %2 == 1 )&&(nNodeEnd != 2  && nNodeEnd != 3))
		{
			if(bNodeName)//!node)
			{
				node = new VO_XML_NODE<T>;
				memset(node,0x00,sizeof(VO_XML_NODE<T>));
				node->pName = new VO_XML_STR<T>;
				node->pValue = new VO_XML_STR<T>;
				memset(node->pName,0x00,sizeof(VO_XML_STR<T>));
				memset(node->pValue,0x00,sizeof(VO_XML_STR<T>));
				pBuf = node->pName;
				bNodeName = false;//true
				nNodeEnd = 1;

			}
			else if(bAttriName)
			{
				attri = new VO_XML_ATTRIBUTE<T>;
				memset(attri,0x00,sizeof(VO_XML_ATTRIBUTE<T>));
				attri->pName = new VO_XML_STR<T>;
				attri->pValue = new VO_XML_STR<T>;
				memset(attri->pName,0x00,sizeof(VO_XML_STR<T>));
				memset(attri->pValue,0x00,sizeof(VO_XML_STR<T>));
				node->pAttributeList.AddTail((VO_S32*)attri);
				bAttriName = false;
				pBuf = attri->pName;

			}
			else if(nAttriValue %2 == 1)
			{
				pBuf = attri->pValue;
				nAttriValue -= 2;
			}
			else if(bNodeValue)
			{
				pBuf = node->pValue;
				bNodeValue = false;
			}
			AddStr(pBuf,c);
			//*pBuf ++ = c;
		}
		else if(c == 0xD || c== 0xa || ISSPACE(c))
		{
			if(nAttriValue == -1)
			{
				AddStr(pBuf,c);
			}
			else
			{
				bNodeName = false;
				bAttriName = true;
			}

			//	if(bNodeName){ pBuf = NULL;	bNodeName = false;}
			//	if(pBuf == NULL){ bAttriName = true;}
		}	
		else if(c == '"')
		{
			if(*m_pXmlBuf == '"')
			{	c= *m_pXmlBuf++; size--;}				
			else
			{
				pBuf = NULL;
				nAttriValue ++;
			}
		}
		else if(c == '/')
		{
			if(nAttriValue == -1)
			{
				AddStr(pBuf,c);
				continue;
				//*pBuf++ = c;
			}
			else if (nNodeEnd == 0)
			{
				bNodeEnd = false;
				nNodeEnd = 3;
			}
			else
			{
				bNodeEnd = true;		
				nNodeEnd = 2;
			}
			bNodeName = false;
			bAttriName = false;
		}
		else if(c == '>')
		{
			if(nAttriValue == -1)
			{
				AddStr(pBuf,c);
				//*pBuf++ = c;
			}
			else if (nNodeEnd == 2)
			{
				result = 1;
				break;
			}
			else if(nNodeEnd == 3)
			{
				result = 3;
				break;
			}
			else
			{
				bAttriName = false;
				nNodeEnd = 0;
				if(!bNodeEnd)	bNodeValue = true;
			}
		}
		else if(c== '=')
		{
			if(nAttriValue == -1)
			{
				AddStr(pBuf,c);
				//*pBuf++ = c;
			}
		}
		else
			if(pBuf) AddStr(pBuf,c);//*pBuf++ = c;

	}
	IssueParseResult(VO_XML_ACTION_CHARACTERS,node);
	IssueParseResult(VO_XML_ACTION_ENDELEMENT,node);
	ReleaseXml(node);
	return result;
}


template <class T>
int CXMLParser<T>::Parser_node(VO_XML_NODE<T> *fNode,int &size)
{
	T c;

	int result = 0;
	VO_XML_STR<T> *pBuf = NULL;
	VO_XML_NODE<T> *node = NULL;
	c =*m_pXmlBuf;

	bool bNodeEnd = false;
	int nAttriValue = 0;
	bool bAttriName = false;
	bool isAttriName = false;
	bool bNodeName = true;
	bool bNodeValue = false;
	bool isNodeValue =false;
	bool bInAngleBrackets = true;

	int nNodeEnd = 0;
	VO_XML_ATTRIBUTE<T> *attri = NULL;
	while( size -- >0)
	{
		c= *m_pXmlBuf++;
//		printf("%c", c);
		if(size <25199)
			int x = 0;
		if(c == 0xD || c== 0xa || c== 0x09 || ISSPACE(c) || c == '/')
		{
			if(ISSPACE(c) && isAttriName )
			{
				continue;
			}
			else 
				if(c == '/')
			{

				if(nAttriValue == -1 || isNodeValue)
				{
					AddStr(pBuf,c);
					continue;
				}
				else if (nNodeEnd == 0)
				{
					bNodeEnd = false;
					nNodeEnd = 3;
				}
				if(*m_pXmlBuf == '>')
				{
					*m_pXmlBuf ++;
					size -- ;
					result = 1;
					break;
				}
				else
				{
					bNodeEnd = true;		
					nNodeEnd = 2;
				}
				bNodeName = false;
				bAttriName = false;
			}
			//else if(c == 0xD || c== 0xa)
			//	continue;
			else
			{
				if(nAttriValue == -1 || true == isNodeValue)
				{
					AddStr(pBuf,c);
				}
				/*in <> 0x0d & 0x0a is right, it like space should not be skip. before node value, these should be skip
				http://usa.visualon.com/streaming/VerizonDash/sourceA/Dash/h264baseline-1000/Manifest.mpd
				&& CBSi_VMAP.xml*/
				else if(bInAngleBrackets)
				{
					bNodeName = false;
					bAttriName = true;
				}
				else 
					continue;
			}
		}
		else if(c == '<')
		{	
			bInAngleBrackets = true;

			char p = *m_pXmlBuf;
			if(p  == '?' || p  == '!' || p  == '/')
			{
				while(size > 0 && *(m_pXmlBuf + 1) != '<')
				{ 
					c = *m_pXmlBuf++; size--;
				}
				if( p  == '/' )
				{
					result = 1;
					break;
				}
				else if( (&m_Root)->pChildNodeList.GetCount() == 0)
				{
					int i = 0;;
				}
				else
					continue;
			}

			bNodeValue = false;
			bAttriName = false;
			isNodeValue = false;
			nAttriValue = 0;
			isAttriName = false;
			nNodeEnd = 0;

			if(!node)	
				node = fNode;
			int ret = Parser_node(node,size) ;
			if(size <= 0) 
				return -1;
			if(ret == -1) 
			{
				node = NULL;
				continue ;
			}
			if(ret == 1)
			{
			//	node = NULL;
				continue;
			}
			if(ret == 2)continue;
			else if(ret == 3)
			{
				result = 1; break;
			}
			else
			{
				c= *m_pXmlBuf++;
				continue;
			}
		}
		else if( (ISALPHA(c) || ISDIGIT(c) || bNodeValue || nAttriValue %2 == 1 )&&(nNodeEnd != 2  && nNodeEnd != 3) )
		{
			if(bNodeName)//!node)
			{
				node = new VO_XML_NODE<T>;
				memset(node,0x00,sizeof(VO_XML_NODE<T>));
				node->pName = new VO_XML_STR<T>;
				node->pValue = new VO_XML_STR<T>;
				memset(node->pName,0x00,sizeof(VO_XML_STR<T>));
				memset(node->pValue,0x00,sizeof(VO_XML_STR<T>));
				node->pFatherNode = fNode;
				fNode->pChildNodeList.AddTail((VO_S32*)node);
				node->pos = fNode->pChildNodeList.GetTailPosition();
				pBuf = node->pName;
				bNodeName = false;//true
				nNodeEnd = 1;

			}
			else if(bAttriName)
			{
				attri = new VO_XML_ATTRIBUTE<T>;
				memset(attri,0x00,sizeof(VO_XML_ATTRIBUTE<T>));
				attri->pName = new VO_XML_STR<T>;
				attri->pValue = new VO_XML_STR<T>;
				memset(attri->pName,0x00,sizeof(VO_XML_STR<T>));
				memset(attri->pValue,0x00,sizeof(VO_XML_STR<T>));
				node->pAttributeList.AddTail((VO_S32*)attri);

				bAttriName = false;
				pBuf = attri->pName;
				isAttriName = true;

			}
			else if(nAttriValue %2 == 1)
			{
				pBuf = attri->pValue;
				nAttriValue -= 2;
			}
			else if(bNodeValue)
			{
				pBuf = node->pValue;
				bNodeValue = false;
				isNodeValue = true;
			}
			AddStr(pBuf,c);
		}
		else if(c == '"' && VO_TRUE == bInAngleBrackets)
		{
			if(*m_pXmlBuf == '"')
			{	
				c= *m_pXmlBuf++; size--;
			}				
			else
			{
				pBuf = NULL;
				nAttriValue ++;
			}
		}
		else if(c == '>')
		{
			bInAngleBrackets = false;

			if(nAttriValue == -1)
			{
				AddStr(pBuf,c);
			}
			else if (nNodeEnd == 2)
			{
				result = 1;
				break;
			}
			else if(nNodeEnd == 3)
			{
				result = 3;
				break;
			}
			else
			{
				bAttriName = false;
				nNodeEnd = 0;
				if(!bNodeEnd)	bNodeValue = true;
			}
		}
		else if(c== '=')
		{
			if(nAttriValue == -1 || isNodeValue)
			{
				AddStr(pBuf,c);
			}
			else
				isAttriName = false;
		}
		else
		{
			AddStr(pBuf,c);
		}

	}
	
	return result;
}

template<class T>
void CXMLParser<T>::ReleaseXml(VO_XML_NODE<T>* fNode)
{
	int count = fNode->pAttributeList.GetCount();
	while(count--)
	{
		VO_XML_ATTRIBUTE<T> *attri = (VO_XML_ATTRIBUTE<T>*)fNode->pAttributeList.GetHead();
		fNode->pAttributeList.RemoveHead();
		if(attri->pName){delete []attri->pName->pBuf;delete attri->pName;}
		if(attri->pValue){delete []attri->pValue->pBuf; delete attri->pValue;}
		delete attri;
		attri = NULL;
	}
	count =fNode->pChildNodeList.GetCount();
	while(count--)
	{
		VO_XML_NODE<T> *node = (VO_XML_NODE<T>*)fNode->pChildNodeList.GetHead();
		fNode->pChildNodeList.RemoveHead();
		ReleaseXml(node);
		
		if(node->pName){delete []node->pName->pBuf;delete node->pName;}
		if(node->pValue){delete []node->pValue->pBuf; delete node->pValue;}
		delete node;
		node = NULL;
	}
}

template <class T>
void CXMLParser<T>::Printall(VO_XML_NODE<T>* fNode,int levels)
{

	T  cccc[1000];
	memset(cccc,0x00,sizeof(cccc));
	printf("\n");
	for(int i = 0 ;i<levels;i++)
		cccc[i]='-';
#ifdef __UNICODE
typedef wchar_t xchar;
	xchar *ttt1;
	xchar *ttt2;
#define NODE_NAME L"%snode %s=%s\n"
#define ATTRI_NAME L"%sattri %s=%s"
#define PRINTZ wprintf 
#else
typedef  char xchar;
	xchar *ttt1;
	xchar *ttt2;
#define NODE_NAME "%snode %s=%s\n"
#define ATTRI_NAME "%sattri %s=%s\n"
#define PRINTZ printf 
#endif
	

	if(fNode->pName && fNode->pValue)
	{
		ttt1 =(xchar*) fNode->pName->pBuf;
		ttt2 =(xchar*) fNode->pValue->pBuf;
		PRINTZ(NODE_NAME,cccc,ttt1,ttt2);
	}
	POSITION pos = fNode->pAttributeList.GetHeadPosition();
	while(pos != NULL)
	{
		//VO_S32 *ptr = 
		VO_XML_ATTRIBUTE<T> *attri = (VO_XML_ATTRIBUTE<T>*)fNode->pAttributeList.GetNext(pos);
		//VO_XML_ATTRIBUTE *attri =(VO_XML_ATTRIBUTE<T>*) fnode->pAttributeList.GetNext(pos);
		ttt1 =(xchar*) attri->pName->pBuf;
		ttt2 =(xchar*) attri->pValue->pBuf;
		PRINTZ(ATTRI_NAME,cccc,ttt1,ttt2);
	}
	pos =fNode->pChildNodeList.GetHeadPosition();
	if(pos != NULL)	levels+=2;
	VO_XML_NODE<T> *node = (VO_XML_NODE<T>*)fNode->pChildNodeList.GetNext(pos);

	while(node != NULL)
	{
// 		VO_XML_NODE<T> *node = GetNextSibling()
// 		VO_XML_NODE<T> *node = (VO_XML_NODE<T>*)fNode->pChildNodeList.GetNext(pos);
		Printall( node,levels);
		node = (VO_XML_NODE<T> *)GetNextSibling((void*) node );
	}
}

template <class T>
void CXMLParser<T>::FlushXml()
{
	if(!m_pXMLSaxHandler)
	{
		ReleaseXml(&m_Root);
		memset(&m_Root,0x00,sizeof(VO_XML_NODE<T>));
	}
}
template <class T>
void CXMLParser<T>::PrintXml()
{
	Printall(&m_Root,0);
}
template <class T>
VO_VOID	CXMLParser<T>::IssueParseResult(VO_U32 nType, VO_XML_NODE<T> *node, VO_VOID* pReserved)
{
	if (m_pXMLSaxHandler)
	{
		switch(nType)
		{
		case VO_XML_ACTION_STARTELEMENT:
			{
				if(node == NULL) break;
				POSITION pos = node->pAttributeList.GetHeadPosition();
				int i =0;
				char *(attriName[100]);
				char *(attriValue[100]);
				/*attriName[i] attriValue[i] */
				while(pos != NULL)
				{
					VO_XML_ATTRIBUTE<T> *attri = (VO_XML_ATTRIBUTE<T>*)node->pAttributeList.GetNext(pos);
					attriName[i]=(char*) attri->pName->pBuf;
					attriValue[i] =(char*) attri->pValue->pBuf;
					i++;
				}
				m_pXMLSaxHandler->startElement((char*)node->pName,attriName,attriValue,i);
		
				break;
			}
		case VO_XML_ACTION_ENDELEMENT:
			if(node == NULL) break;
			m_pXMLSaxHandler->endElement((char*)node->pName);
			break;
		case VO_XML_ACTION_CHARACTERS:
			if(node == NULL) break;
			m_pXMLSaxHandler->characters((char*)node->pValue);
			break;
		case VO_XML_ACTION_ENDDOCUMENT:
			m_pXMLSaxHandler->endDocument();
		case VO_XML_ACTION_STARTDOCUMENT:
			m_pXMLSaxHandler->startDocument();
			break;
		}
	}
}
#ifdef _VONAMESPACE
}
#endif
#endif
