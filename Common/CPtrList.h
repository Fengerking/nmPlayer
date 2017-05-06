	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseList.h

	Contains:	CBaseList header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#ifndef __CBaseList_H__
#define __CBaseList_H__

#include "voYYDef_Common.h"
#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#ifndef POSITION
#define POSITION VO_PTR
#endif // POSITION

class CBaseList 
{
public:
	class CNode
	{
		CNode *m_pPrev;
		CNode *m_pNext;
		VO_PTR m_pObject;

	public:
		CNode()
		{
			m_pPrev = NULL;
			m_pNext = NULL;
			m_pObject = NULL;
		};

		CNode *Prev() const { return m_pPrev; };
		CNode *Next() const { return m_pNext; };

		void SetPrev(CNode *p) { m_pPrev = p; };
		void SetNext(CNode *p) { m_pNext = p; };

		VO_PTR GetData() const { return m_pObject; };
		void SetData(VO_PTR p) { m_pObject = p; };
	};

	class CNodeCache
	{
	public:
		CNodeCache(VO_S32 iCacheSize)
			: m_iCacheSize(iCacheSize)
			, m_iUsed(0)
			, m_pHead(NULL)
		{
		};

		~CNodeCache()
		{
			CNode *pNode = m_pHead;
			while (pNode)
			{
				CNode *pCurrent = pNode;
				pNode = pNode->Next();
				delete pCurrent;
				m_iUsed--;
			}
		};

		void AddToCache(CNode *pNode)
		{
			if (m_iUsed < m_iCacheSize)
			{
				pNode->SetNext(m_pHead);
				m_pHead = pNode;
				m_iUsed++;
			}
			else
			{
				delete pNode;
			}
		};

		CNode * RemoveFromCache()
		{
			CNode *pNode = m_pHead;
			if (pNode != NULL)
			{
				m_pHead = pNode->Next();
				m_iUsed--;
			}
			return pNode;
		};

		private:
			int		m_iCacheSize;
			int		m_iUsed;
			CNode *	m_pHead;
	};

protected:
	CNode*	m_pFirst;
	CNode*	m_pLast;
	int		m_Count;

private:
	CNodeCache m_Cache;

private:
	CBaseList(const CBaseList &refList);
	CBaseList &operator=(const CBaseList &refList);

public:
	CBaseList(VO_S32 nItems = 0);
	virtual ~CBaseList();

	void		RemoveAll();
	POSITION	GetHeadPositionI() const;
	POSITION	GetTailPositionI() const;
	VO_S32		GetCountI() const;

	POSITION	GetPosition (VO_S32 nIndex);

protected:
	VO_PTR		GetNextI(POSITION& rp) const;
	VO_PTR		GetI(POSITION p) const;

public:
	POSITION Next(POSITION pos) const
	{
		if (pos == NULL)
		{
			return (POSITION) m_pFirst;
		}
		CNode *pn = (CNode *) pos;
		return (POSITION) pn->Next();
	} //Next

	// See Next
	POSITION Prev(POSITION pos) const
	{
		if (pos == NULL)
		{
			return (POSITION) m_pLast;
		}
		CNode *pn = (CNode *) pos;
		return (POSITION) pn->Prev();
	} //Prev

protected:
	POSITION	FindI( VO_PTR pObj) const;
	VO_PTR		RemoveHeadI();
	VO_PTR		RemoveTailI();
	VO_PTR		RemoveI(POSITION p);
	bool		RemoveObject(VO_PTR pObj);

	POSITION	AddTailI(VO_PTR pObj);

public:
	bool		AddTail(CBaseList *pList);

protected:
	POSITION	AddHeadI(VO_PTR pObj);
public:
	bool		AddHead(CBaseList *pList);

protected:
	POSITION	AddAfterI(POSITION p, VO_PTR pObj);
public:
	bool		AddAfter(POSITION p, CBaseList *pList);

protected:
	POSITION	AddBeforeI(POSITION p, VO_PTR pObj);
public:
	bool		AddBefore(POSITION p, CBaseList *pList);


	bool		MoveToTail(POSITION pos, CBaseList *pList);
	bool		MoveToHead(POSITION pos, CBaseList *pList);

	void		Reverse();


	// set cursor to the position of each element of list in turn  
	#define TRAVERSELIST(list, cursor)               \
	for ( cursor = (list).GetHeadPosition()           \
		; cursor!=NULL                               \
		; cursor = (list).Next(cursor)                \
		)


	// set cursor to the position of each element of list in turn
	// in reverse order
	#define REVERSETRAVERSELIST(list, cursor)        \
	for ( cursor = (list).GetTailPosition()           \
		; cursor!=NULL                               \
		; cursor = (list).Prev(cursor)                \
		)
}; // end of class declaration


template<class OBJECT> 
class CObjectList : public CBaseList
{
public:
	CObjectList(void) : CBaseList(10)
	{};
	virtual ~CObjectList(){};

	POSITION	GetHeadPosition() const { return (POSITION)m_pFirst; }
	POSITION	GetTailPosition() const { return (POSITION)m_pLast; }
	int			GetCount() const { return m_Count; }

	OBJECT *	GetNext(POSITION& rp) const { return (OBJECT *) GetNextI(rp); }

	OBJECT *	Get(POSITION p) const { return (OBJECT *) GetI(p); }
	OBJECT *	GetHead() const  { return Get(GetHeadPosition()); }
	OBJECT *	GetTail() const  { return Get(GetTailPosition()); }

	OBJECT *	RemoveHead() { return (OBJECT *) RemoveHeadI(); }

	OBJECT *	RemoveTail() { return (OBJECT *) RemoveTailI(); }

	OBJECT *	Remove(POSITION p) { return (OBJECT *) RemoveI(p); }
	bool		Remove(OBJECT * p) { return RemoveObject(p); }

	POSITION	AddBefore(POSITION p, OBJECT * pObj) { return AddBeforeI(p, pObj); }
	POSITION	AddAfter(POSITION p, OBJECT * pObj)  { return AddAfterI(p, pObj); }
	POSITION	AddHead(OBJECT * pObj) { return AddHeadI(pObj); }
	POSITION	AddTail(OBJECT * pObj)  { return AddTailI(pObj); }
	bool		AddTail(CObjectList<OBJECT> *pList)
					{ return CBaseList::AddTail((CBaseList *) pList); }
	bool		AddHead(CObjectList<OBJECT> *pList)
					{ return CBaseList::AddHead((CBaseList *) pList); }
	bool		AddAfter(POSITION p, CObjectList<OBJECT> *pList)
					{ return CBaseList::AddAfter(p, (CBaseList *) pList); };
	bool		AddBefore(POSITION p, CObjectList<OBJECT> *pList)
					{ return CBaseList::AddBefore(p, (CBaseList *) pList); };
	POSITION	Find( OBJECT * pObj) const { return FindI(pObj); }

}; // end of class declaration
    
#ifdef _VONAMESPACE
}
#endif

#endif //__CBaseList_H__

