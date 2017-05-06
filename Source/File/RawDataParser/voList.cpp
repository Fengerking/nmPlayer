	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voList.cpp

	Contains:	list implementation file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2011-10-31		Rodney		Create file

*******************************************************************************/
#include <stdlib.h>
#include "voList.h"


voList::voList(void) : head(NULL), current(NULL), m_count(0)
{
	head = current = (PSingleListNode)malloc(sizeof(SingleListNode));
	head->next = NULL;
	head->data = NULL;
}

voList::~voList(void)
{
	Release();
	SAFE_FREE(head);
}


// release a list
VO_VOID voList::Release()
{
	current = head->next;
	do {
		Delete();
	} while (current);
}


// add an element into a list
VO_BOOL voList::Add(VO_PTR data)
{
	PSingleListNode pSLNode = (PSingleListNode)malloc(sizeof(SingleListNode));
	if (!pSLNode) return VO_FALSE;
	pSLNode->next = head->next;
	pSLNode->data = data;
	head->next = pSLNode;
	m_count++;

	return VO_TRUE;
}


// delete current element in a list.
VO_VOID voList::Delete()
{
	PSingleListNode pCurrentNode = current;
	current = current->next;
	SAFE_FREE(pCurrentNode);
	m_count--;
}


// Get the current node data from a list.
VO_PTR voList::Get()
{
	return current->data;
}


// locate a position in a list.
VO_BOOL voList::Locate(VO_U64 posInList)
{
	if (posInList >= m_count) return VO_FALSE;
	current = head->next;
	if (!current) return VO_FALSE;
	VO_U64 index = m_count - posInList - 1;
	while (index--) current = current->next;
	
	return VO_TRUE;
}

