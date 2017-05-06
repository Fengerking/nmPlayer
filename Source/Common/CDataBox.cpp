	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDataBox.cpp

	Contains:	CDataBox head file

	Written by:	Aiven

	Change History (most recent first):
	2013-04-2		Aiven			Create file

*******************************************************************************/

#include "CDataBox.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

/*
VO_U32 MallocBuffer(VO_PTR pUserData, VO_PBYTE* ppData, VO_U32 nSize)
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	 
	if(!pUserData){
		return ret;
	}

	CDataBox* pUser = (CDataBox*)pUserData;
	
	ret = pUser->MallocData(nSize);

	return ret;
}
*/


CDataBox::CDataBox()
:m_pData(NULL)
,m_uDataSize(0)
,m_uUsedDataSize(0)
{
}

CDataBox::~CDataBox()
{
	if(m_pData){
		delete []m_pData;
		m_pData = NULL;
		m_uDataSize = 0;
	}
}

VO_U32 CDataBox::MallocData(VO_PTR pUserData, VO_PBYTE* ppData, VO_U32 nSize)
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;

	if(NULL == pUserData){
		return ret;
	}

	CDataBox* pUser = (CDataBox*)pUserData;


	if(pUser->m_pData){
		delete []pUser->m_pData;
		pUser->m_pData = NULL;
	}

	
	pUser->m_pData = new unsigned char[nSize];
	
	if(pUser->m_pData){
		pUser->m_uDataSize = nSize;
		pUser->m_uUsedDataSize = 0;

		memset(pUser->m_pData, 0x0, pUser->m_uDataSize);
		*ppData = pUser->m_pData;
		ret = VO_RET_SOURCE2_OK;
	}


	return ret;
}

VO_U32 CDataBox::GetDataAndSize(VO_PBYTE* ppData, VO_U32** ppUsedDataSize, VO_U32* pDataSize)
{
	if(NULL == ppData || NULL == ppUsedDataSize){
		return VO_RET_SOURCE2_FAIL;
	}
	
	*ppData = m_pData;
	*ppUsedDataSize = &m_uUsedDataSize;
	
	if(pDataSize){
		*pDataSize = m_uDataSize;
	}
	
	return VO_RET_SOURCE2_OK;
}

