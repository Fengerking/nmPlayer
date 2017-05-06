#include "VList.h"


// Sample data chain
VList::VList(void)
	: m_pHead(0)
	, m_pTail(0)
	, m_nCount(0)
{
}

VList::~VList(void)
{
}

bool VList::AddTail(void* pElement)
{
	if(pElement == 0)
		return false;

	VElement* pNew = new VElement;
	pNew->SetData(pElement);

	if (m_pTail == 0)
	{
		m_pHead = pNew;
		m_pTail = pNew;

		m_pHead->SetNext(0);
		m_pTail->SetNext(0);
	}
	else if (m_pTail == m_pHead)
	{
		m_pHead->SetNext(pNew);
		m_pTail = pNew;
	}
	else
	{
		m_pTail->SetNext(pNew);
		m_pTail = pNew;
	}

	pNew->SetNext(0);

	m_nCount++;

	return true;
}


void* VList::RemoveHead(void)
{
	VElement* pElement = 0;

	if(m_pHead == 0)
	{
		return 0;
	}
	else if (m_pHead->GetNext () == 0)
	{
		pElement = m_pHead;

		m_pHead = 0;
		m_pTail = 0;
	}
	else
	{
		pElement = m_pHead;
		m_pHead = m_pHead->GetNext ();
	}

	m_nCount--;

	void* data = pElement->GetData();
	delete pElement;

	return data;
}


int VList::GetCount (void)
{
	int nCount = 0;

	VElement* pElement = m_pHead;
	while (pElement != 0)
	{
		nCount++;
		pElement = pElement->GetNext ();
	}

	return nCount;
}

bool VList::Release (void)
{
	void* pData = 0;
	VElement* pElement = m_pHead;
	VElement* pDelElement = 0;

	while(pElement != 0)
	{
		pDelElement = pElement;
		pElement = pElement->GetNext();

		delete pDelElement;
	}

	m_pHead = 0;
	m_pTail = 0;

	return false;
}

void* VList::GetNext(void* pCurr)
{
	void* pData = 0;
	VElement* pElement = m_pHead;
	VElement* pDelElement = 0;

	while(pElement != 0)
	{
		pData = pElement->GetData();
		
		if(pData == pCurr)
		{
			pElement = pElement->GetNext();

			if(pElement)
			{
				if(pElement->GetData())
					return pElement->GetData();
				else
					continue;
			}
			else
				return 0;
				
		}

		pElement = pElement->GetNext();
	}

	return 0;
}