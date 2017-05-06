#include "BufList.h"

CBufList::CBufList(void)
{
  m_nCount = 0;
  m_pBufHead = 0;
}

CBufList::~CBufList(void)
{
}

bool CBufList::PushBack(void* pBuf, unsigned int nBufLen)
{
  if (!pBuf)
    return false;

  BUFFER_LIST* pNewBufList = new BUFFER_LIST;
  pNewBufList->bufInfo.nBufferLen = nBufLen;
  pNewBufList->bufInfo.pBuffer = pBuf;
  pNewBufList->pNext = 0;

  m_mutex.Lock();
  if (!m_pBufHead)
  {
    m_pBufHead = pNewBufList;
  }
  else
  {
    BUFFER_LIST* pBufList = m_pBufHead;
    while (pBufList->pNext)
      pBufList = pBufList->pNext;
    pBufList->pNext = pNewBufList;
  }
  m_nCount++;
  m_mutex.Unlock();

  return true;
}

bool CBufList::PopFront(void** ppBuf, unsigned int& nBufLen)
{
  if (!m_pBufHead)
    return false;

  m_mutex.Lock();
  *ppBuf = m_pBufHead->bufInfo.pBuffer;
  nBufLen = m_pBufHead->bufInfo.nBufferLen;

  BUFFER_LIST* pBufList = m_pBufHead;
  m_pBufHead = m_pBufHead->pNext;
  delete [] pBufList;
  m_mutex.Unlock();

  return true;
}
