	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompList.cpp

	Contains:	voCOMXCompList class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "voCOMXCompList.h"

#include "voLog.h"
#undef LOG_TAG
#define LOG_TAG "voCOMXCompList"

voCOMXCompList::voCOMXCompList()
	: m_pConfig (NULL)
	, m_pWorkPath (NULL)
	, m_nLoaderNum (0)
	, m_ppCompLoader (NULL)
	, m_nItemCount (0)
	, m_pFirstItem (NULL)
	, m_nHandleCount (0)
	, m_pFirstHandle (NULL)
	, m_pLibOP (NULL)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXCompList::~voCOMXCompList()
{
	VOLOGS ();

	ReleaseHandles ();
	ReleaseItems ();
	ReleaseLoader ();
}

void voCOMXCompList::SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;
}

OMX_ERRORTYPE voCOMXCompList::Init (voCOMXCfgCore * pConfig, OMX_STRING pWorkPath)
{


	m_pConfig = pConfig;
	m_pWorkPath = pWorkPath;

	return OMX_ErrorNone;
}

OMX_COMPONENTTYPE *	voCOMXCompList::CreateComponent (OMX_STRING pName, OMX_PTR pAppData,
													  OMX_CALLBACKTYPE* pCallBacks)
{
	VOLOGS ();

	if (m_pFirstItem == NULL)
	{
		if (LoadCore () != OMX_ErrorNone)
			return NULL;
	}

	OMX_COMPONENTTYPE * pHandle = NULL;

	voCOMXCoreLoader * pLoader = FindLoader (pName);
	if (pLoader == NULL)
	{
		VOLOGI ("The component %s could bot be found", pName);
		return NULL;
	}

	OMX_ERRORTYPE errType = pLoader->OMX_GetHandle ((OMX_HANDLETYPE *)&pHandle, pName, pAppData, pCallBacks);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("The component %s could bot be created", pName);
		return NULL;
	}

	SCompHandle * pNewHandle = (SCompHandle *) voOMXMemAlloc (sizeof (SCompHandle));
	if (pNewHandle == NULL)
		return NULL;

	voOMXMemSet (pNewHandle, 0, sizeof (SCompHandle));
	pNewHandle->Handle = pHandle;
	pNewHandle->Loader = pLoader;

	SCompHandle * pCurrHandle = m_pFirstHandle;
	while (pCurrHandle  != NULL)
	{
		if (pCurrHandle->Next == NULL)
			break;
		pCurrHandle = pCurrHandle->Next;
	}

	if (pCurrHandle == NULL)
		m_pFirstHandle = pNewHandle;
	else
		pCurrHandle->Next = pNewHandle;

	return pHandle;
}

void voCOMXCompList::FreeComponent (OMX_COMPONENTTYPE * pComp)
{
	VOLOGS ();

	SCompHandle * pPrevHandle = m_pFirstHandle;
	SCompHandle * pCurrHandle = m_pFirstHandle;
	while (pCurrHandle  != NULL)
	{
		if (pCurrHandle->Handle == pComp)
		{
			pCurrHandle->Loader->OMX_FreeHandle (pCurrHandle->Handle);
			break;
		}
		pPrevHandle = pCurrHandle;
		pCurrHandle = pCurrHandle->Next;
	}

	if (m_pFirstHandle == pCurrHandle)
		m_pFirstHandle = pCurrHandle->Next;
	else
		pPrevHandle->Next = pCurrHandle->Next;

	voOMXMemFree (pCurrHandle);
}

OMX_ERRORTYPE voCOMXCompList::GetComponentName (OMX_STRING pName, OMX_U32 nIndex)
{
	VOLOGS ();

	if (m_pFirstItem == NULL)
	{
		if (LoadCore () != OMX_ErrorNone)
			return OMX_ErrorNoMore;
	}

	if (nIndex >= m_nItemCount)
		return OMX_ErrorNoMore;

	SCompItem * pCurrItem = m_pFirstItem;
	OMX_U32		nCount = 0;
	while (pCurrItem != NULL && nCount < nIndex)
	{
		pCurrItem = pCurrItem->Next;
		nCount++;
	}

	voOMXMemCopy (pName, pCurrItem->Name, 128);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompList::GetRolesOfComponent (OMX_STRING compName, OMX_U32 *pNumRoles, OMX_U8 **roles)
{
	VOLOGS ();

	if (m_pFirstItem == NULL)
	{
		if (LoadCore () != OMX_ErrorNoMore)
			return OMX_ErrorInvalidComponentName;
	}

	voCOMXCoreLoader * pLoader = FindLoader (compName);
	if (pLoader == NULL)
	{
		VOLOGE ("It could not find the loader by component %s", compName);
		return OMX_ErrorInvalidComponentName;
	}

	return pLoader->OMX_GetRolesOfComponent (compName, pNumRoles, roles);
}

OMX_ERRORTYPE voCOMXCompList::LoadCore (void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pFirstItem != NULL)
		return OMX_ErrorNone;

	if (m_pConfig == NULL)
		return OMX_ErrorComponentNotFound;

	COMXCfgSect * pCoreSect = m_pConfig->GetCoreSect ();
	if (pCoreSect == NULL)
	{
		VOLOGE ("It could not get OMX Core item from vomeCore.cfg file.");
		return OMX_ErrorComponentNotFound;
	}

	OMX_U32	nCoreCount = 0;
	COMXCfgItem * pItem = m_pConfig->GetFirstItem ();
	while (pItem != NULL)
	{
		if (pItem->m_pSection == pCoreSect)
			nCoreCount++;
		pItem = pItem->m_pNext;
	}

	ReleaseLoader ();

	m_ppCompLoader = (voCOMXCoreLoader **)voOMXMemAlloc (nCoreCount * sizeof (voCOMXCoreLoader*));
	if (m_ppCompLoader == NULL)
		return OMX_ErrorInsufficientResources;
	voOMXMemSet (m_ppCompLoader, 0, nCoreCount * sizeof (voCOMXCoreLoader*));

	pItem = m_pConfig->GetFirstItem ();
	while (pItem != NULL)
	{
		if (pItem->m_pSection->m_pName == pCoreSect->m_pName)
		{
			m_ppCompLoader[m_nLoaderNum] = new voCOMXCoreLoader ();
			if (m_ppCompLoader[m_nLoaderNum] == NULL)
			{
				VOLOGE ("It could not create new voCOMXCoreLoader.");
				return OMX_ErrorInsufficientResources;
			}
			m_ppCompLoader[m_nLoaderNum]->SetCallBack (m_pCallBack, m_pUserData);
			if (!strcmp (pItem->m_pName, "TI") || !strcmp (pItem->m_pName, "SEC_") || !strcmp (pItem->m_pName, "FSL_"))
				errType = m_ppCompLoader[m_nLoaderNum]->SetCoreFile (pItem->m_pValue, m_pLibOP, pItem->m_pName, m_pWorkPath);
			else
				errType = m_ppCompLoader[m_nLoaderNum]->SetCoreFile (pItem->m_pValue, m_pLibOP, NULL, m_pWorkPath);

			if (errType != OMX_ErrorNone)
			{
				VOLOGW ("The result is 0X%08X from SetCoreFile function.", errType);
				delete m_ppCompLoader[m_nLoaderNum];
				m_ppCompLoader[m_nLoaderNum] = NULL;
				// return errType;
			}

			m_nLoaderNum++;
		}
		pItem = pItem->m_pNext;
	}

	SCompItem * pCurrItem = NULL;

	for (OMX_U32 i = 0; i < m_nLoaderNum; i++)
	{
		if (m_ppCompLoader[i] != NULL)
		{
			if ((errType = m_ppCompLoader[i]->OMX_Init ()) != OMX_ErrorNone)
			{
				VOLOGE ("The result is 0X%08X from OMX_Init function.", errType);
				return OMX_ErrorComponentNotFound;
			}

			OMX_U32		nIndex = 0;
			//OMX_U32		nLength = 0;
			OMX_STRING	pName = (OMX_STRING)voOMXMemAlloc (128);
			if (pName == NULL)
				return OMX_ErrorInsufficientResources;

			while (m_ppCompLoader[i]->OMX_ComponentNameEnum (pName, 127, nIndex) == OMX_ErrorNone)
			{
				SCompItem * pItem = (SCompItem*)voOMXMemAlloc (sizeof (SCompItem));
				if (pItem == NULL)
					return OMX_ErrorInsufficientResources;
				voOMXMemSet (pItem, 0, sizeof (SCompItem));

				pItem->Name = pName;
				pItem->Loader = m_ppCompLoader[i];

				if (m_pFirstItem == NULL)
					m_pFirstItem = pItem;
				if (pCurrItem != NULL)
					pCurrItem->Next = pItem;
				m_nItemCount++;

				pCurrItem = pItem;
				nIndex++;
				pName = (OMX_STRING)voOMXMemAlloc (128);
				if (pName == NULL)
					return OMX_ErrorInsufficientResources;
			}

			voOMXMemFree (pName);

	//		m_ppCompLoader[i]->OMX_Deinit ();
		}
	}


	if (m_pFirstItem == NULL)
		return OMX_ErrorComponentNotFound;

	return OMX_ErrorNone;
}

voCOMXCoreLoader * voCOMXCompList::FindLoader (OMX_STRING pName)
{
	VOLOGS ();

	SCompItem * pCurrItem = m_pFirstItem;
	while (pCurrItem != NULL)
	{
		if (pCurrItem->Name != NULL)
		{
			if (!strcmp (pCurrItem->Name, pName))
				return pCurrItem->Loader;
		}

		pCurrItem = pCurrItem->Next;
	}

	return NULL;
}

void voCOMXCompList::ReleaseItems (void)
{
	VOLOGS ();

	SCompItem * pCurrItem = m_pFirstItem;
	SCompItem * pNextItem = m_pFirstItem;
	while (pCurrItem != NULL)
	{
		pNextItem = pCurrItem->Next;
		if (pCurrItem->Name != NULL)
			voOMXMemFree (pCurrItem->Name);
		voOMXMemFree (pCurrItem);
		pCurrItem = pNextItem;
		m_nItemCount--;
	}

	m_pFirstItem = NULL;
}

void voCOMXCompList::ReleaseHandles (void)
{
	VOLOGS ();

	SCompHandle * pCurrItem = m_pFirstHandle;
	SCompHandle * pNextItem = m_pFirstHandle;
	while (pCurrItem != NULL)
	{
		pNextItem = pCurrItem->Next;
		if (pCurrItem->Handle != NULL)
		{
			pCurrItem->Loader->OMX_FreeHandle (pCurrItem->Handle);
		}
		voOMXMemFree (pCurrItem);
		pCurrItem = pNextItem;
		m_nItemCount--;
	}

	m_pFirstHandle = NULL;
}

void voCOMXCompList::ReleaseLoader (void)
{
	VOLOGS ();

	if (m_ppCompLoader != NULL)
	{
		for (OMX_U32 i = 0; i < m_nLoaderNum; i++)
		{
			if (m_ppCompLoader[i] != NULL)
				delete m_ppCompLoader[i];
		}

		voOMXMemFree (m_ppCompLoader);
		m_ppCompLoader = NULL;
		m_nLoaderNum = 0;
	}
}
