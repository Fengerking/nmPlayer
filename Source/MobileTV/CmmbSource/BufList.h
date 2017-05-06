#pragma once
#include "voCMutex.h"

class CBufList
{
  typedef struct _BUFFER_INFO
  {
    unsigned int nBufferLen;
    void* pBuffer;
  }BUFFER_INFO;

  typedef struct _BUFFER_LIST
  {
    struct _BUFFER_LIST* pNext;
    BUFFER_INFO bufInfo;
  }BUFFER_LIST;

public:
  CBufList(void);
  ~CBufList(void);

  bool PushBack(void* pBuf, unsigned int nBufLen);
  bool PopFront(void** ppBuf, unsigned int& nBufLen);

protected:
  unsigned int m_nCount;
  BUFFER_LIST* m_pBufHead;

  voCMutex m_mutex;
};
