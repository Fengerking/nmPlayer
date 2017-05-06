
#include <stdio.h>
#include <string.h>
#include "voinfo.h"
#include "CIntQueue.h"

CIntQueue::CIntQueue(const int ncap)
					: m_nhead(-1)
					, m_ntail(-1)
					, m_ncount(0)
					, m_ncapability(ncap)
					, m_pdata(NULL)
{
	if (ncap == 0)  return;

	m_pdata = new int[ncap];
	if (m_pdata == NULL) {
		VOINFO("no enough memory....");
	}
}

CIntQueue::~CIntQueue() {

	if (m_pdata) {
		delete []m_pdata;
		m_pdata = NULL;
	}

	clearAll();
}

void CIntQueue::setCapability(const int ncap) {

	m_ncapability = ncap;
	if (ncap == 0) 
		return;

	if (m_pdata != NULL) 
		delete []m_pdata;

	m_pdata = new int[ncap];
	if (m_pdata == NULL) {
		VOINFO("no enough memory....");
		return;
	}

	for (int i = 0; i < m_ncapability; i++)
		*(m_pdata + i) = -1;

	m_nhead = m_ntail = 0;
}

void CIntQueue::clearAll() {

	m_nhead = m_ntail = m_ncount = 0;
}

int CIntQueue::enqueue(const int nval) {

	if (isFull())
		return -1;

	m_ncount++;
	*(m_pdata + m_ntail) = nval;
	m_ntail = (m_ntail + 1) % m_ncapability;

	return m_ncount;
}

int CIntQueue::dequeue(int *pval) {

	if (isEmpty())
		return -1;

	m_ncount--;

	if (pval)
		*pval = *(m_pdata + m_nhead);

	m_nhead = (m_nhead + 1) % m_ncapability;

	return m_ncount;
}

int CIntQueue::get(int &npos, int &nval) {

	if (npos < 0 || npos >= m_ncapability || npos == m_ntail)
		return -1;

	nval = *(m_pdata + npos);
	npos = (npos + 1) % m_ncapability;
	if (npos == m_ntail)
		npos = -1;

	return 0;
}

int CIntQueue::getHeadPosition() {
	
	if (isEmpty())
		return -1;

	return m_nhead;
}

int CIntQueue::getTailPosition() {
	
	if (isEmpty())
		return -1;

	return m_ntail;
}

bool CIntQueue::isFull() {

	return (m_ncount == m_ncapability ? true : false);
}

bool CIntQueue::isEmpty() {

	return (m_ncount == 0 ? true : false);
}

