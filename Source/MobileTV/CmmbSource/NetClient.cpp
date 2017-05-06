#include "NetClient.h"

CNetClient::CNetClient(CBufList* pBufList)
{
  m_pBufList = pBufList;
  m_hThread = 0;
  m_nTID = 0;
}

CNetClient::~CNetClient(void)
{
}

VO_U32 CNetClient::TcpThread(VO_PTR pParam)
{
  CNetClient* pThis = (CNetClient*)pParam;
  pThis->TcpProc();
  return 0;
}

VO_U32 CNetClient::UdpThread(VO_PTR pParam)
{
  CNetClient* pThis = (CNetClient*)pParam;
  pThis->UdpProc();
  return 0;
}

void CNetClient::TcpProc()
{
#define MAX_PACKAGE_SIZE (1*1024*1024)

  SOCKET sock;
  char buf[4096];
  int nLen;
  DWORD dwRecved;
  char* pBuf = 0;
  DWORD dwSize;
  int nRecvRet;
  int nSendRet;

  while ((sock = accept(m_socket, 0, 0)) != INVALID_SOCKET)
  {
    dwSize = 0;
    while ((nRecvRet = recv(sock, (char*)&dwSize, 4, 0)) == 4)// && dwSize <= MAX_PACKAGE_SIZE && (nSendRet = send(sock, "\x01", 1, 0)) == 1)
    {
      if (dwSize > MAX_PACKAGE_SIZE)
      {
        int a=3;
        break;
      }

      if ((nSendRet = send(sock, "\x01", 1, 0)) != 1)
      {
        int a=3;
        break;
      }

      {
        DWORD dw;
        HANDLE fp = CreateFile(L"c:\\b.dat", GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
        SetFilePointer(fp, 0, 0, FILE_END);
        WriteFile(fp, &dwSize, 4, &dw, 0);
        CloseHandle(fp);
      }

      pBuf = new char[dwSize];

      dwRecved = 0;
      nLen = dwSize - dwRecved > sizeof(buf) ? sizeof(buf) : dwSize - dwRecved;
      while (nLen > 0 &&
             (nRecvRet = recv(sock, buf, nLen, 0)) < dwSize && 
             nRecvRet > 0 && 
             nRecvRet <= nLen)
      {
        TCHAR dbg[128];
        wsprintf(dbg, L"dwSize = %d, nLen = %d dwRecved = %d Total = %d\r\n", dwSize, nLen, dwRecved, nLen + dwRecved);
        OutputDebugString(dbg);

        {
          DWORD dw;
          HANDLE fp = CreateFile(L"c:\\b.dat", GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
          SetFilePointer(fp, 0, 0, FILE_END);
          WriteFile(fp, buf, nLen, &dw, 0);
          CloseHandle(fp);
        }

        if (dwRecved + nLen > dwSize)
        {
          closesocket(sock);
          break;
        }
        cmnMemCopy(0, &pBuf[dwRecved], buf, nLen);
        dwRecved += nLen;
        nLen = dwSize - dwRecved > sizeof(buf) ? sizeof(buf) : dwSize - dwRecved;
        if (dwRecved == dwSize)
        {
          if (!m_pBufList->PushBack(pBuf, dwSize))
          {
            closesocket(sock);
            break;
          }
          pBuf = 0;
        }
        if((nSendRet = send(sock, "\x01", 1, 0)) != 1)
        {
          closesocket(sock);
          break;
        }
      }
      dwSize = 0;
    }
  }
}

void CNetClient::UdpProc()
{
  char buf[4096];
  int nBufLen;
  while ((nBufLen = recvfrom(m_socket, buf, sizeof(buf), 0, 0, 0)) != SOCKET_ERROR && nBufLen != 0)
  {
    if (!nBufLen || nBufLen == SOCKET_ERROR)
      return;

    char* pBuf = new char[nBufLen];
    memcpy(pBuf, buf, nBufLen);
    if (!m_pBufList->PushBack(pBuf, nBufLen))
      return;
  }
}

bool CNetClient::Start(bool bTcp, unsigned short nPort)
{
#ifdef _WIN32
  WSADATA wsa;
  if (WSAStartup(0x202, &wsa) != 0)
    return false;
#endif

  if (bTcp)
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
  else
    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (!m_socket || m_socket == SOCKET_ERROR)
    return false;

  SOCKADDR_IN addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_port = htons(nPort);
  addr.sin_family = AF_INET;
  if (bind(m_socket, (struct sockaddr*)&addr, sizeof(addr)) != 0)
  {
    closesocket(m_socket);
    return false;
  }

  if (bTcp)
  {
    if (listen(m_socket, 10) != 0)
    {
      closesocket(m_socket);
      return false;
    }
    if (voThreadCreate(&m_hThread, &m_nTID, TcpThread, this, 0) != VO_ERR_NONE)
    {
      closesocket(m_socket);
      return false;
    }
  }
  else
  {
    if (voThreadCreate(&m_hThread, &m_nTID, UdpThread, this, 0) != VO_ERR_NONE)
    {
      closesocket(m_socket);
      return false;
    }
  }

  return true;
}

bool CNetClient::Stop()
{
  if (m_socket)
    closesocket(m_socket);

  return true;
}
