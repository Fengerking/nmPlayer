#pragma once
#include "BufList.h"
#include "voThread.h"
#include "cmnMemory.h"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

class CNetClient
{
public:
  CNetClient(CBufList* pBufList);
  ~CNetClient(void);

  bool Start(bool bTcp, unsigned short nPort);
  bool Stop();

  void TcpProc();
  void UdpProc();

protected:
  static VO_U32 TcpThread(VO_PTR pParam);
  static VO_U32 UdpThread(VO_PTR pParam);

  SOCKET m_socket;
  CBufList* m_pBufList;
  voThreadHandle m_hThread;
  VO_U32 m_nTID;
};
