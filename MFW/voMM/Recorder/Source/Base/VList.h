#pragma once

class VElement
{
public:
	VElement(void)
	{
		m_pNext = 0;
		m_pData = 0;
	};
	virtual ~VElement(void){};

public:
	void SetNext(VElement* pElement){m_pNext = pElement;};
	VElement* GetNext(){return m_pNext;};
	void* GetData(){return m_pData;};
	void SetData(void* pData){m_pData = pData;};

private:
	VElement* m_pNext;
	void* m_pData;
};

class VList
{
public:
	VList (void);
	virtual ~ VList (void);

public:
	bool AddTail(void* pElement);
	void* RemoveHead (void);

	void* GetHead (void)
	{
		if(!m_pHead)
			return 0;

		return m_pHead->GetData();
	}
	void* GetTail (void)
	{
		if(!m_pTail)
			return 0;

		return m_pTail->GetData();
	}

	void* GetNext(void* pCurr);

	int GetCount(void);

	bool Release (void);

private:
	VElement*	m_pHead;
	VElement*	m_pTail;
	int m_nCount;
};