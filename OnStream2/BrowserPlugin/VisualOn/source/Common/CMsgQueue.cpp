
#include <stdio.h>
#include <string.h>
#include "CMsgQueue.h"

CMsgQueue::CMsgQueue(const int ncap)
					: m_nhead(-1)
					, m_ntail(-1)
					, m_ncount(0)
					, m_ncapability(ncap)
					, m_pdata(NULL)
{

	if (ncap > 0) {
		m_pdata = new VOCBMSG[ncap];
		if (m_pdata == NULL) 
			printf("no enough memory....");
	}

	clearAll();
}

CMsgQueue::~CMsgQueue() {

	if (m_pdata) {
		delete []m_pdata;
		m_pdata = NULL;
	}

	clearAll();
}

void CMsgQueue::setCapability(const int ncap) {

	if (ncap == m_ncapability || ncap == 0)
		return;

	m_ncapability = ncap;

	if (m_pdata != NULL) 
		delete []m_pdata;

	m_pdata = new VOCBMSG[ncap];
	if (m_pdata == NULL) {
		printf("no enough memory....");
		return;
	}

	m_nhead = m_ntail = 0;
}

void CMsgQueue::clearAll() {

	m_nhead = m_ntail = m_ncount = 0;
}

int CMsgQueue::insert2Head(const VOCBMSG& msg) {

	if (isFull())
		return VOQU_ERR;

	m_ncount++;
	m_nhead--;
	if (m_nhead < 0)
		m_nhead = m_ncapability - 1;

	*(m_pdata + m_nhead) = msg;
	return m_ncount;
}

int CMsgQueue::enqueue(const VOCBMSG& msg) {

	if (isFull())
		return VOQU_ERR;

	m_ncount++;
	*(m_pdata + m_ntail) = msg;
	m_ntail = (m_ntail + 1) % m_ncapability;

	return m_ncount;
}

int CMsgQueue::dequeue(VOCBMSG *pval) {

	if (isEmpty())
		return VOQU_ERR;

	m_ncount--;

	if (pval)
		*pval = *(m_pdata + m_nhead);

	m_nhead = (m_nhead + 1) % m_ncapability;

	return m_ncount;
}

int CMsgQueue::get(int &npos, VOCBMSG &nval) {

	if ((m_ntail > m_nhead) && (npos < m_nhead || npos >= m_ntail) || (m_ntail < m_nhead) && (npos >= m_ntail || npos > m_nhead))
		return VOQU_ERR;

	nval = *(m_pdata + npos);
	npos = (npos + 1) % m_ncapability;
	if (npos == m_ntail)
		npos = -1;

	return 0;
}

int CMsgQueue::getHeadPosition() {
	
	if (m_ncapability == 0)
		return VOQU_ERR;

	return m_nhead;
}

int CMsgQueue::getTailPosition() {
	
	if (m_ncapability == 0)
		return VOQU_ERR;

	return m_ntail;
}

bool CMsgQueue::isFull() {

	return (m_ncount == m_ncapability ? true : false);
}

bool CMsgQueue::isEmpty() {

	return (m_ncount == 0 ? true : false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
CVoQueue<T>::CVoQueue(const int ncap)
: m_nhead(-1)
, m_ntail(-1)
, m_ncount(0)
, m_ncapability(ncap)
, m_pdata(NULL)
{

	if (ncap > 0) {
		m_pdata = new T[ncap];
		if (m_pdata == NULL) 
			printf("no enough memory....");
	}

	clearAll();
}

template <typename T>
CVoQueue<T>::~CVoQueue() {

	clearAll();

	if (m_pdata) {
		delete []m_pdata;
		m_pdata = NULL;
	}
}

template <typename T>
void CVoQueue<T>::setCapability(const int ncap) {

	if (ncap == m_ncapability || ncap == 0)
		return;

	m_ncapability = ncap;

	if (m_pdata != NULL) 
		delete []m_pdata;

	m_pdata = new T[ncap];
	if (m_pdata == NULL) {
		printf("no enough memory....");
		return;
	}

	m_nhead = m_ntail = 0;
}

template <typename T>
void CVoQueue<T>::clearAll() {

	m_nhead = m_ntail = m_ncount = 0;
}

template <typename T>
int CVoQueue<T>::insert2Head(const T& item) {

	if (isFull())
		return VOQU_ERR;

	m_ncount++;
	m_nhead--;
	if (m_nhead < 0)
		m_nhead = m_ncapability - 1;

	*(m_pdata + m_nhead) = item;
	return m_ncount;
}

template <typename T>
int CVoQueue<T>::enqueue(const T& item) {

	if (isFull())
		return VOQU_ERR;

	m_ncount++;
	*(m_pdata + m_ntail) = item;
	m_ntail = (m_ntail + 1) % m_ncapability;

	return m_ncount;
}

template <typename T>
int CVoQueue<T>::dequeue(T *pval) {

	if (isEmpty())
		return VOQU_ERR;

	m_ncount--;

	if (pval)
		*pval = *(m_pdata + m_nhead);

	m_nhead = (m_nhead + 1) % m_ncapability;

	return m_ncount;
}

template <typename T>
int CVoQueue<T>::get(int &npos, T &nval) {

	if ((m_ntail > m_nhead) && (npos < m_nhead || npos >= m_ntail) || (m_ntail < m_nhead) && (npos >= m_ntail || npos > m_nhead))
		return VOQU_ERR;

	nval = *(m_pdata + npos);
	npos = (npos + 1) % m_ncapability;
	if (npos == m_ntail)
		npos = -1;

	return 0;
}

template<typename T>
int CVoQueue<T>::remove(T item) {

	if (isEmpty())
		return VOQU_ERR;

	bool bfound = false;
	T ti;
	int npos = m_nhead;
	while (npos > 0) {

		int ret = get(npos, ti);
		if (item == ti) {
			bfound = true;
			break;
		}
	}

	if (bfound) {
		//TO DO
	}

	return npos;
}

template <typename T>
int CVoQueue<T>::getHeadPosition() {

	if (m_ncapability == 0)
		return VOQU_ERR;

	return m_nhead;
}

template <typename T>
int CVoQueue<T>::getTailPosition() {

	if (m_ncapability == 0)
		return VOQU_ERR;

	return m_ntail;
}

template <typename T>
bool CVoQueue<T>::isFull() {

	return (m_ncount == m_ncapability ? true : false);
}

template <typename T>
bool CVoQueue<T>::isEmpty() {

	return (m_ncount == 0 ? true : false);
}

