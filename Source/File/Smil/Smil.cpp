	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		Smil.h

	Contains:	Smil class file

	Written by:	Aiven

	Change History (most recent first):
	2013-07-08		Aiven			Create file

*******************************************************************************/
#include "Smil.h"
#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CSmil::CSmil()
:m_pXmlLoad(NULL)
,m_pBuffer(NULL)
,m_uSize(0)
{
	m_pXmlLoad = new CXMLLoad;
	memset(&m_nSMIL, 0x0, sizeof(m_nSMIL));
}

CSmil::~CSmil()
{
	if(m_pBuffer){
		delete m_pBuffer;
		m_pBuffer = NULL;
		m_uSize = 0;
	}
	
	if(m_pXmlLoad){
		delete m_pXmlLoad;
		m_pXmlLoad = NULL;
	}

	ReleaseSmil(&m_nSMIL);
}

VO_U32 CSmil::Init(VO_U32 nFlag, VO_PTR pParam)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	VOLOGINIT((VO_TCHAR*)pParam);
	if(pParam){
		m_pXmlLoad->SetWorkPath((VO_TCHAR*)pParam);
	}
	
	m_pXmlLoad->OpenParser();

	return nResult;
}

VO_U32 CSmil::Uninit()
{

	VO_U32 nResult = VO_RET_SOURCE2_OK;
	
	m_pXmlLoad->CloseParser();
	VOLOGUNINIT();
	return nResult;

}

VO_U32 CSmil::Parse(VO_PBYTE pBuffer, VO_U32 nSize, VO_PTR* ppSmilStruct)
{
	_VODS_CHECK_NULL_POINTER_THREE_(pBuffer, nSize, ppSmilStruct, VO_RET_SOURCE2_FAIL)

	VO_U32 nResult = VO_RET_SOURCE2_FAIL;

	if(m_pBuffer){
		delete m_pBuffer;
		m_pBuffer = NULL;
		m_uSize = 0;
	}

	m_pBuffer = new unsigned char[nSize];
	memcpy(m_pBuffer, pBuffer, nSize);
	m_uSize = nSize;

	if(VO_RET_SOURCE2_OK == ParseSmil(m_pBuffer, m_uSize, &m_nSMIL)){
		*ppSmilStruct = (VO_PTR)&m_nSMIL;
		nResult = VO_RET_SOURCE2_OK;
	}

	return nResult;
}

VO_BOOL CSmil::GetLicState()
{
	return VO_TRUE;
}

VO_U32 CSmil::FindAttribute(VO_ELEMENT_STRUCT* pElement, VO_CHAR* pName, VO_ATTRIBUTE_STRUCT** ppAttribute)
{
	_VODS_CHECK_NULL_POINTER_THREE_(pElement, pName, ppAttribute, VO_RET_SOURCE2_FAIL)
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;

	list_T<VO_ATTRIBUTE_STRUCT *>::iterator attributeiter;
	for(attributeiter = pElement->AttributeList.begin(); attributeiter != pElement->AttributeList.end(); ++attributeiter)
	{
		VO_ATTRIBUTE_STRUCT* tmpattribute = (VO_ATTRIBUTE_STRUCT*)(*attributeiter);

		if(0 == strncmp(pName, tmpattribute->pName, tmpattribute->nNamesize)){
			*ppAttribute = tmpattribute;
			nResult = VO_RET_SOURCE2_OK;
			break;
		}
	}

	return nResult;
}

VO_U32 CSmil::ParseSmil(VO_PBYTE pBuffer, VO_U32 nSize, VO_SMIL_STRUCT* pSMIL)
{
	_VODS_CHECK_NULL_POINTER_TWO_(pBuffer, nSize, VO_RET_SOURCE2_FAIL)
	VO_U32 nResult = VO_RET_SOURCE2_OK;

	ReleaseSmil(pSMIL);

	if(VO_ERR_NONE != m_pXmlLoad->LoadXML((char*)pBuffer, (int)nSize, voXML_FLAG_SOURCE_BUFFER)){
		VOLOGE("parse xml failed!");
		return VO_RET_SOURCE2_FAIL;
	}

	void *pRoot = NULL;
	m_pXmlLoad->GetFirstChild(NULL,&pRoot);
	if (!pRoot) 
		return VO_RET_SOURCE2_FAIL;
	
	char *pTagName;
	int size;
	m_pXmlLoad->GetTagName(pRoot,&pTagName,size);

	void *pvoChildNode = NULL;
	m_pXmlLoad->GetFirstChild(pRoot,&pvoChildNode);
	while (pvoChildNode)
	{
		m_pXmlLoad->GetTagName(pvoChildNode,&pTagName,size);
		if(0 == strncmp(pTagName,TAG_LEVELTWO_HEAD, size)){

		}else if(0 == strncmp(pTagName,TAG_LEVELTWO_BODY, size)){

			pSMIL->body = new VO_ELEMENT_STRUCT;
			ParseElement(pvoChildNode, pSMIL->body);
		}
		m_pXmlLoad->GetNextSibling(pvoChildNode, &pvoChildNode);
	}
	return nResult;
}
VO_U32 CSmil::ParseElement(VO_PTR pRoot, VO_ELEMENT_STRUCT* pElement)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	_VODS_CHECK_NULL_POINTER_TWO_(pRoot, pElement, VO_RET_SOURCE2_FAIL)

	GenerateElemenType(pRoot, pElement);
	
	switch(pElement->Type)
	{
	case TYPE_ELEMENT_PARAM:
		ParseParamElement(pRoot, pElement);
		break;
	default:					
		ParseBaseElement(pRoot, pElement);
		break;
	}
	
	return nResult;
}
VO_U32 CSmil::GenerateElemenType(VO_PTR pRoot, VO_ELEMENT_STRUCT* pElement)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	_VODS_CHECK_NULL_POINTER_ONE_(pElement, VO_RET_SOURCE2_FAIL)

	char *pTagName;
	int size;
	m_pXmlLoad->GetTagName(pRoot,&pTagName,size);

	if(0 == strncmp(pTagName, ELEMENT_A, size)){
		pElement->Type = TYPE_ELEMENT_A;
	}else if(0 == strncmp(pTagName, ELEMENT_ANIMATION, size)){
		pElement->Type = TYPE_ELEMENT_ANIMATION;
	}else if(0 == strncmp(pTagName, ELEMENT_AUDIO, size)){
		pElement->Type = TYPE_ELEMENT_AUDIO;
	}else if(0 == strncmp(pTagName, ELEMENT_IMG, size)){
		pElement->Type = TYPE_ELEMENT_IMG;
	}else if(0 == strncmp(pTagName, ELEMENT_PAR, size)){
		pElement->Type = TYPE_ELEMENT_PAR;
	}else if(0 == strncmp(pTagName, ELEMENT_REF, size)){
		pElement->Type = TYPE_ELEMENT_REF;
	}else if(0 == strncmp(pTagName, ELEMENT_SEQ, size)){
		pElement->Type = TYPE_ELEMENT_SEQ;
	}else if(0 == strncmp(pTagName, ELEMENT_SWITCH, size)){
		pElement->Type = TYPE_ELEMENT_SWITCH;
	}else if(0 == strncmp(pTagName, ELEMENT_TEXT, size)){
		pElement->Type = TYPE_ELEMENT_TEXT;
	}else if(0 == strncmp(pTagName, ELEMENT_TEXTSTREAM, size)){
		pElement->Type = TYPE_ELEMENT_TEXTSTREAM;
	}else if(0 == strncmp(pTagName, ELEMENT_VIDEO, size)){
		pElement->Type = TYPE_ELEMENT_VIDEO;
	}else if(0 == strncmp(pTagName, ELEMENT_PARAM, size)){
		pElement->Type = TYPE_ELEMENT_PARAM;
	}else if(0 == strncmp(pTagName, TAG_LEVELTWO_HEAD, size)){
		pElement->Type = TYPE_ELEMENT_HEAD;
	}else if(0 == strncmp(pTagName, TAG_LEVELTWO_BODY, size)){
		pElement->Type = TYPE_ELEMENT_BODY;
	}

	return nResult;
}

VO_U32 CSmil::ParseBaseElement(VO_PTR pRoot, VO_ELEMENT_STRUCT* pElement)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	_VODS_CHECK_NULL_POINTER_ONE_(pElement, VO_RET_SOURCE2_FAIL)

	char* pAttriValue = NULL;
	VO_U32 nSize = 0;

	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(ATTRIBUTE_GUID),&pAttriValue,(int&)nSize) == VO_ERR_NONE){
		VO_ATTRIBUTE_STRUCT* attribute = new VO_ATTRIBUTE_STRUCT;
		attribute->pValue = new VO_CHAR[nSize];
		memcpy(attribute->pValue, pAttriValue, nSize);
		attribute->nValuesize = nSize;
		
		nSize = sizeof(ATTRIBUTE_GUID);
		attribute->pName = new VO_CHAR[nSize];
		memcpy(attribute->pName, ATTRIBUTE_GUID, nSize);
		attribute->nNamesize = nSize;

		pElement->AttributeList.push_back(attribute);
	}

	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(ATTRIBUTE_SRC),&pAttriValue,(int&)nSize) == VO_ERR_NONE){
		VO_ATTRIBUTE_STRUCT* attribute = new VO_ATTRIBUTE_STRUCT;
		attribute->pValue = new VO_CHAR[nSize];
		memcpy(attribute->pValue, pAttriValue, nSize);
		attribute->nValuesize = nSize;
		
		nSize = sizeof(ATTRIBUTE_SRC);
		attribute->pName = new VO_CHAR[nSize];
		memcpy(attribute->pName, ATTRIBUTE_SRC, nSize);		
		attribute->nNamesize = nSize;

		pElement->AttributeList.push_back(attribute);
	}

	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(ATTRIBUTE_TITLE),&pAttriValue,(int&)nSize) == VO_ERR_NONE){
		VO_ATTRIBUTE_STRUCT* attribute = new VO_ATTRIBUTE_STRUCT;
		attribute->pValue = new VO_CHAR[nSize];
		memcpy(attribute->pValue, pAttriValue, nSize);
		attribute->nValuesize = nSize;
		
		nSize = sizeof(ATTRIBUTE_TITLE);
		attribute->pName = new VO_CHAR[nSize];
		memcpy(attribute->pName, ATTRIBUTE_TITLE, nSize);		
		attribute->nNamesize = nSize;

		pElement->AttributeList.push_back(attribute);
	}
	
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(ATTRIBUTE_CLIP_BEGIN),&pAttriValue,(int&)nSize) == VO_ERR_NONE){
		VO_ATTRIBUTE_STRUCT* attribute = new VO_ATTRIBUTE_STRUCT;
		attribute->pValue = new VO_CHAR[nSize];
		memcpy(attribute->pValue, pAttriValue, nSize);
		attribute->nValuesize = nSize;
		
		nSize = sizeof(ATTRIBUTE_CLIP_BEGIN);
		attribute->pName = new VO_CHAR[nSize];
		memcpy(attribute->pName, ATTRIBUTE_CLIP_BEGIN, nSize);		
		attribute->nNamesize = nSize;

		pElement->AttributeList.push_back(attribute);
	}
	
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(ATTRIBUTE_CLIP_END),&pAttriValue,(int&)nSize) == VO_ERR_NONE){
		VO_ATTRIBUTE_STRUCT* attribute = new VO_ATTRIBUTE_STRUCT;
		attribute->pValue = new VO_CHAR[nSize];
		memcpy(attribute->pValue, pAttriValue, nSize);
		attribute->nValuesize = nSize;
		
		nSize = sizeof(ATTRIBUTE_CLIP_END);
		attribute->pName = new VO_CHAR[nSize];
		memcpy(attribute->pName, ATTRIBUTE_CLIP_END, nSize);
		attribute->nNamesize = nSize;

		pElement->AttributeList.push_back(attribute);
	}


	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(ATTRIBUTE_DUR),&pAttriValue,(int&)nSize) == VO_ERR_NONE){
		VO_ATTRIBUTE_STRUCT* attribute = new VO_ATTRIBUTE_STRUCT;
		attribute->pValue = new VO_CHAR[nSize];
		memcpy(attribute->pValue, pAttriValue, nSize);
		attribute->nValuesize = nSize;

		nSize = sizeof(ATTRIBUTE_DUR);
		attribute->pName = new VO_CHAR[nSize];
		memcpy(attribute->pName, ATTRIBUTE_DUR, nSize);
		attribute->nNamesize = nSize;

		pElement->AttributeList.push_back(attribute);
	}
	void *pvoChildNode = NULL;
	m_pXmlLoad->GetFirstChild(pRoot,&pvoChildNode);

	while(pvoChildNode){
		VO_ELEMENT_STRUCT* element = new VO_ELEMENT_STRUCT;
		ParseElement(pvoChildNode, element);
		pElement->ChildList.push_back(element);

		m_pXmlLoad->GetNextSibling(pvoChildNode,&pvoChildNode);		
	}

	return nResult;
}

VO_U32 CSmil::ParseParamElement(VO_PTR pRoot, VO_ELEMENT_STRUCT* pElement)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	_VODS_CHECK_NULL_POINTER_ONE_(pElement, VO_RET_SOURCE2_FAIL)

	char* pAttriValue = NULL;
	VO_U32 nSize = 0;
	
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(PARAM_ATTRIBUTE_NAME),&pAttriValue,(int&)nSize) == VO_ERR_NONE){
		VO_ATTRIBUTE_STRUCT* attribute = new VO_ATTRIBUTE_STRUCT;
		attribute->pValue = new VO_CHAR[nSize];
		memcpy(attribute->pValue, pAttriValue, nSize);
		attribute->nValuesize = nSize;
		
		nSize = sizeof(PARAM_ATTRIBUTE_NAME);
		attribute->pName = new VO_CHAR[nSize];
		memcpy(attribute->pName, PARAM_ATTRIBUTE_NAME, nSize);		
		attribute->nNamesize = nSize;

		pElement->AttributeList.push_back(attribute);
	}
	if(m_pXmlLoad->GetAttributeValue(pRoot,(char*)(PARAM_ATTRIBUTE_VALUE),&pAttriValue,(int&)nSize) == VO_ERR_NONE){
		VO_ATTRIBUTE_STRUCT* attribute = new VO_ATTRIBUTE_STRUCT;
		attribute->pValue = new VO_CHAR[nSize];
		memcpy(attribute->pValue, pAttriValue, nSize);
		attribute->nValuesize = nSize;
		
		nSize = sizeof(PARAM_ATTRIBUTE_VALUE);
		attribute->pName = new VO_CHAR[nSize];
		memcpy(attribute->pName, PARAM_ATTRIBUTE_VALUE, nSize);
		attribute->nNamesize = nSize;

		pElement->AttributeList.push_back(attribute);
		
	}

	return nResult;
}


VO_U32 CSmil::ReleaseSmil(VO_SMIL_STRUCT* pSMIL)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	_VODS_CHECK_NULL_POINTER_ONE_(pSMIL, VO_RET_SOURCE2_FAIL)

	ReleaseElement(&pSMIL->head);
	ReleaseElement(&pSMIL->body);

	return nResult;
}

VO_U32 CSmil::ReleaseElement(VO_ELEMENT_STRUCT** ppElement)
{
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	_VODS_CHECK_NULL_POINTER_TWO_(ppElement, *ppElement, VO_RET_SOURCE2_FAIL)

	VO_ELEMENT_STRUCT* pElement = *ppElement;
	list_T<VO_ATTRIBUTE_STRUCT *>::iterator attributeiter;
	for(attributeiter = pElement->AttributeList.begin(); attributeiter != pElement->AttributeList.end(); ++attributeiter)
	{
		VO_ATTRIBUTE_STRUCT* tmpattribute = (VO_ATTRIBUTE_STRUCT*)(*attributeiter);
		delete tmpattribute->pName;
		delete tmpattribute->pValue;

		delete tmpattribute;
	}
	pElement->AttributeList.clear();
	
	list_T<VO_ELEMENT_STRUCT *>::iterator elementiter;
	for(elementiter = pElement->ChildList.begin(); elementiter != pElement->ChildList.end(); ++elementiter)
	{
		VO_ELEMENT_STRUCT* tmpelement = (VO_ELEMENT_STRUCT*)(*elementiter);
		ReleaseElement(&tmpelement);
	}
	pElement->ChildList.clear();

	delete pElement;
	
	pElement = NULL;

	return nResult;
}


