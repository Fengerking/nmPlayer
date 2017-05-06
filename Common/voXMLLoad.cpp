
#include "voXMLLoad.h"

#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

typedef VO_S32 ( VO_API *pvoXMLGetParserAPI)(VO_XML_PARSER_API * pParser);

CXMLLoad::CXMLLoad(void)
{
	m_parser_handle = NULL;
	memset(&m_parser_api,0x00,sizeof(VO_XML_PARSER_API));
}

CXMLLoad::~CXMLLoad(void)
{
	CloseParser();
}

VO_U32 CXMLLoad::CloseParser()
{
 	if( m_parser_api.Close )
	{
		m_parser_api.Close( m_parser_handle );
		m_parser_handle = NULL;
	}
	memset(&m_parser_api,0x00,sizeof(VO_XML_PARSER_API));
	FreeLib();
	return VO_ERR_NONE;
}
VO_U32 CXMLLoad::OpenParser()
{
	CloseParser();
	
	vostrcpy(m_szDllFile, _T("voXMLParser"));
	vostrcpy(m_szAPIName, _T("voXMLGetParserAPI"));

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, _T(".so"));
#elif defined _MAC_OS
	vostrcat(m_szDllFile, _T(".dylib"));
#elif defined _IOS
	{
		voXMLGetParserAPI (&m_parser_api);

		VO_XML_PARSER_INIT_INFO info;
		info.pProc = NULL;
		info.pUserData = this;
		info.pMemOP = NULL;

		return m_parser_api.Open( &m_parser_handle , &info );
	}
#endif

	if(CDllLoad::LoadLib(NULL) == 0)
	{
		VOLOGE ("LoadLib voXMLParser fail");
		return VO_ERR_FAILED;
	}

	pvoXMLGetParserAPI pXMLAPI = (pvoXMLGetParserAPI) m_pAPIEntry;
	if (pXMLAPI == NULL)
	{
		return VO_ERR_FAILED;
	}

	pXMLAPI (&m_parser_api);

	VO_XML_PARSER_INIT_INFO info;
	info.pProc = NULL;
	info.pUserData = this;
	info.pMemOP = NULL;

	return m_parser_api.Open( &m_parser_handle , &info );
}

VO_U32 CXMLLoad::LoadXML (char *buf, int nSize, int nFlag)
{
	if(!m_parser_handle)
		return VO_ERR_FAILED;
	return m_parser_api.LoadXML(m_parser_handle,(char*)buf, nSize,nFlag);
}
/* \param pNode[in] ,node pointer *
*  \param pChildNode[out] , if pNode is NULL then pChildNode return Root node 
*/ 
VO_U32 CXMLLoad::GetFirstChild (void *pNode,void **pChildNode)
{
	if(!m_parser_handle)
		return VO_ERR_FAILED;
	return m_parser_api.GetFirstChild(m_parser_handle,pNode,pChildNode);
}
VO_U32 CXMLLoad::GetNextSibling (void *pNode,void **pChildNode)
{
	if(!m_parser_handle)
		return VO_ERR_FAILED;
	return m_parser_api.GetNextSibling(m_parser_handle,pNode,pChildNode);
}
VO_U32 CXMLLoad::GetChildByName (void *pNode, char *pTagName,void **pChildNode)
{
	if(!m_parser_handle)
		return VO_ERR_FAILED;
	return m_parser_api.GetChildByName(m_parser_handle,pNode,pTagName,pChildNode);
}
VO_U32 CXMLLoad::GetAttributeValue (void *pNode, char *pAttriName,char **pAttriValue,int &nSize)
{
	if(!m_parser_handle)
		return VO_ERR_FAILED;
	return m_parser_api.GetAttributeValue(m_parser_handle,pNode,pAttriName,pAttriValue,nSize);
}
VO_U32 CXMLLoad::GetNextAttribute( void *pNode, void **ppNext, void* pCurrent, char **ppAttriName, int &nAttriNameSize, char **ppAttriValue, int &nAttriValuesize)
{
	if(!m_parser_handle)
		return VO_ERR_FAILED;
	return m_parser_api.GetNextAttribute(m_parser_handle,pNode,ppNext, pCurrent, ppAttriName, nAttriNameSize, ppAttriValue, nAttriValuesize);

}
VO_U32 CXMLLoad::GetTagValue(void *pNode,char **pTagValue,int &nSize)
{
	if(!m_parser_handle)
		return VO_ERR_FAILED;
	return m_parser_api.GetTagValue(m_parser_handle,pNode,pTagValue,nSize);

}
VO_U32 CXMLLoad::GetTagName(void *pNode,char **pTagName,int &nSize)
{
	if(!m_parser_handle)
		return VO_ERR_FAILED;
	return m_parser_api.GetTagName(m_parser_handle,pNode,pTagName,nSize);
}

VO_U32 CXMLLoad::SetParam(VO_U32 id, VO_PTR pParam)
{
	if(!m_parser_handle)
		return VO_ERR_FAILED;
	return m_parser_api.SetParam(m_parser_handle, id , pParam);
}

VO_U32 CXMLLoad::GetParam(VO_U32 id, VO_PTR pParam)
{
	if(!m_parser_handle)
		return VO_ERR_FAILED;
	return m_parser_api.GetParam(m_parser_handle, id , pParam);
}