	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voList.h

	Contains:	list header file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2011-10-31		Rodney		Create file

*******************************************************************************/
/* Single List
	head            last        current          first          
	/---------\	   /---------\				/---------\
	|    |    |--->|         |---> ... ---> |    |NULL|
	\---------/	   \---------/				\---------/
*/

#pragma once
#include "voType.h"


class voList
{
	typedef struct Single_List_Node {
		VO_PTR				data;			// pointer to data
		Single_List_Node	*next;			// next List Node
	} SingleListNode, *PSingleListNode;
public:
	voList(void);
	virtual ~voList(void);

	VO_BOOL Add(VO_PTR data);				// add an element into a list.
	VO_PTR Get();							// Get the current node data from a list.
	VO_U32 GetCount() { return m_count; }	// Get the total number of nodes in a list.
	VO_BOOL Locate(VO_U64 posInList);		// locate a position in a list.

protected:
	VO_VOID Release ();			// release a list.

private:
	VO_VOID Delete();			// delete current element in a list.

private:
	PSingleListNode head;		// head pointer.
	PSingleListNode current;	// current node pointer.

	VO_U32			m_count;	// count a number of node in a list.
};


#ifndef SAFE_FREE
#define SAFE_FREE(x)\
if(x)\
{\
	free(x);\
	x = 0;\
}
#endif	//SAFE_FREE
