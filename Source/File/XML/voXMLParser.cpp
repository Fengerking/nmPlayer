// voXMLParser.cpp : Defines the exported functions for the DLL application.
//



#include "voType.h"
#include "voIndex.h"
#include "XMLOP.h"
#include "voXMLParser.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#if defined __cplusplus
extern "C" {
#endif
#endif
	VO_U32 VO_API voXMLParserOpen(VO_PTR * ppHandle,VO_XML_PARSER_INIT_INFO* pParam)
	{
		//CDumper::WriteLog("  + voXMLParserOpen");

		CXmlOP* pParser = NULL;

		pParser = new CXmlOP;

		//pParser = new CXML_Parser(pParam->pMemOP);

		if (!pParser)
		{
			return VO_ERR_FAILED;
		}
		pParser->Open(pParam);
		*ppHandle = pParser;

		return VO_ERR_NONE;
	}

	/**
	* Close the opened source.
	* \param pHandle [IN] The handle which was create by open function.
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
	VO_U32 VO_API voXMLParserClose(VO_PTR pHandle)
	{
		if(!pHandle)
			return VO_ERR_FAILED;

		CXmlOP* pParser = (CXmlOP*)pHandle;
		pParser->Close();
		delete pParser;

		return VO_ERR_NONE;
	}
	VO_U32 VO_API voLoadXML(VO_PTR pHandle,char *buf, int size,int nFlag)
	{
		if(!pHandle)
			return VO_ERR_FAILED;
		CXmlOP* pParser = (CXmlOP*)pHandle;
		return pParser->LoadXML(buf,size,nFlag);
	}
	VO_U32 VO_API voGetFirstChild(VO_PTR pHandle,void *pNode,void **pChildNode)
	{
		if(!pHandle)
			return VO_ERR_FAILED;
		CXmlOP* pParser = (CXmlOP*)pHandle;
		//*pChildNode = NULL;
		*pChildNode = pParser->GetFirstChild(pNode);
		if(*pChildNode == NULL) return VO_ERR_FAILED;
		return VO_ERR_NONE;
	}
	VO_U32 VO_API voGetNextSibling(VO_PTR pHandle,void *pNode,void **pChildNode)
	{
		if(!pHandle)
			return VO_ERR_FAILED;
		CXmlOP* pParser = (CXmlOP*)pHandle;
	
		*pChildNode = pParser->GetNextSibling(pNode);
		if(*pChildNode == NULL) return VO_ERR_FAILED;
		return VO_ERR_NONE;
	}

	VO_U32 VO_API voGetChildByName(VO_PTR pHandle,void *pNode, char *pTagName,void **pChildNode)
	{
		if(!pHandle)
			return VO_ERR_FAILED;
		CXmlOP* pParser = (CXmlOP*)pHandle;
		//*pChildNode = NULL;
		*pChildNode = pParser->GetChildByName(pNode,pTagName);
		if(*pChildNode == NULL) return VO_ERR_FAILED;
		return VO_ERR_NONE;
	}
	VO_U32 VO_API voGetAttributeValue(VO_PTR pHandle,void *pNode, char *pAttriName,char **pAttriValue,int &nSize)
	{
		if(!pHandle)
			return VO_ERR_FAILED;
		CXmlOP* pParser = (CXmlOP*)pHandle;
		return pParser->GetAttributeValue(pNode,pAttriName,pAttriValue,nSize);

	}
	VO_U32 VO_API voGetNextAttribute(VO_PTR pHandle, void *pNode, void **ppNext, void* pCurrent, char **ppAttriName, int &nAttriNameSize, char **ppAttriValue, int &nAttriValuesize)
	{
		if(!pHandle)
			return VO_ERR_FAILED;
		CXmlOP* pParser = (CXmlOP*)pHandle;

		*ppNext = pParser->GetNextAttribute(pNode,pCurrent,ppAttriName,nAttriNameSize, ppAttriValue, nAttriValuesize);
		if(*ppNext == NULL)
			return VO_ERR_FAILED;
		return VO_ERR_NONE;
	}

	VO_U32 VO_API voGetTagValue(VO_PTR pHandle,void *pNode,char **pTagValue,int &nSize)
	{
		if(!pHandle)
			return VO_ERR_FAILED;
		CXmlOP* pParser = (CXmlOP*)pHandle;
		return pParser->GetTagValue(pNode,pTagValue,nSize);
	}
	VO_U32 VO_API voGetTagName(VO_PTR pHandle,void *pNode,char **pTagName,int &nSize)
	{
		if(!pHandle)
			return VO_ERR_FAILED;
		CXmlOP* pParser = (CXmlOP*)pHandle;
		return pParser->GetTagName(pNode,pTagName,nSize);
		//if(pTagName == NULL) return VO_ERR_FAILED;
		//return VO_ERR_NONE;
	}
	/**
	* Parse the buffer.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pBuffer [IN] The buffer to be parsed
	* \retval VO_ERR_PARSER_OK Succeeded.
	*/
// 	VO_U32 VO_API voXMLParserProcess(VO_PTR pHandle, VO_PARSER_INPUT_BUFFER* pBuffer)
// 	{
// 		if(!pHandle)
// 			return VO_ERR_PARSER_INVLAID_HANDLE;
// 
// 		CXML_Parser* pParser = (CXML_Parser*)pHandle;
// 		return pParser->Process(pBuffer);
// 	}


	/**
	* Set param for special target.
	* \param pHandle [IN] The handle which was create by open function.
	* \param uID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID>
	* \retval VO_ERR_PARSER_OK Succeeded. 
	*/
	VO_U32 VO_API voSetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if (uID == VO_PID_COMMON_LOGFUNC)
		{
			//VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
			//vologInit (pVologCB->pUserData, pVologCB->fCallBack);
		}

		if(!pHandle)
			return VO_ERR_FAILED;

		
		CXmlOP* pParser = (CXmlOP*)pHandle;

		return pParser->SetParam(uID,pParam);
	}
	VO_U32 VO_API voGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(!pHandle)
			return VO_ERR_FAILED;
		CXmlOP* pParser = (CXmlOP*)pHandle;
		return pParser->GetParam(uID,pParam);
	}
// 
// 	/**
// 	* Get param for special target.
// 	* \param pHandle [IN] The handle which was create by open function.
// 	* \param uID [IN] The param ID.
// 	* \param pParam [IN] The param value depend on the ID>
// 	* \retval VO_ERR_PARSER_OK Succeeded.
// 	*/
// 	VO_U32 VO_API voXMLParserGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
// 	{
// 		/*CXML_Parser* pParser = (CXML_Parser*)pHandle;
// 		return pParser->GetParam(uID, pParam);
// 		*/
// 		return VO_ERR_PARSER_OK;
// 	}
#ifdef _VONAMESPACE
	}
#else
#if defined __cplusplus
	}
#endif
#endif

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#if defined __cplusplus
	extern "C" {
#endif
	VO_S32 VO_API voXMLGetParserAPI(VO_XML_PARSER_API* pHandle)
	{
		pHandle->Open		= voXMLParserOpen;
		pHandle->Close		= voXMLParserClose;
		pHandle->LoadXML	= voLoadXML;
		pHandle->GetFirstChild	= voGetFirstChild;
		pHandle->GetNextSibling	= voGetNextSibling;
		pHandle->GetChildByName	= voGetChildByName;
		pHandle->GetAttributeValue	= voGetAttributeValue;
		pHandle->GetNextAttribute   = voGetNextAttribute;
		pHandle->GetTagValue	= voGetTagValue;
		pHandle->GetTagName	= voGetTagName;
		pHandle->SetParam        = voSetParam;
		pHandle->GetParam   = voGetParam;
		return VO_ERR_NONE;
	}
#if defined __cplusplus
	}
#endif



