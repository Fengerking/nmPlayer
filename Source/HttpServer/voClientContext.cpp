#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
	#include <winsock2.h>
#elif defined(_LINUX_ANDROID)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/time.h>
	#include <netdb.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
	#include <sys/utsname.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <errno.h>
	#include <semaphore.h>
	#include <malloc.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <strings.h>
#endif

#include "voClientContext.h" 

#ifdef WIN32
#define strncasecmp(x,y,z) _strnicmp(x,y,z)
#endif

void voClientContext::SetTotalBytes(int n)
 {
      m_nTotalBytes = n;
 }

 int voClientContext::GetTotalBytes()
 {
      return m_nTotalBytes;
 }

 void voClientContext::SetSentBytes(int nNum)
 {
      m_nSentBytes = nNum;
 }

 void voClientContext::IncrSentBytes(int nNum)
 {
      m_nSentBytes += nNum;
 }

 int voClientContext::GetSentBytes()
 {
      return m_nSentBytes;
 }

 #if defined(WIN32)
void voClientContext::SetSocket(SOCKET s)
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
void voClientContext::SetSocket(int s)
#endif
 {
      m_Socket = s;
 }
#if defined(WIN32)
 SOCKET voClientContext::GetSocket()
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
int voClientContext::GetSocket()
#endif
 {
      return m_Socket;
 }

 void voClientContext::SetBuffer(char *szBuffer)
 {
      strcpy(m_szBuffer, szBuffer);
 }

 void voClientContext::GetBuffer(char *szBuffer)
 {
      strcpy(szBuffer, m_szBuffer);
 }

 char* voClientContext::GetBuffer()
 {
      return m_szBuffer;
 }

 void voClientContext::ZeroBuffer()
 {
      memset(m_szBuffer,0, MAX_BUFFER_LEN);
 }

 voClientContext* voClientContext::GetNext()
 {
      return m_pNext;
 }

 void voClientContext::SetResponseFlag(bool bFlag)
 {
	m_bIsNeedResponse = bFlag;
 }

 bool voClientContext::GetResponseFlag()
 {
	return m_bIsNeedResponse;
 }
FILE * voClientContext::GetFileStreamHandle()
{
   return m_pFileStream;
}
void voClientContext::SetFileStreamHandle(FILE *f)
{
	m_pFileStream = f;
}
 void voClientContext::SetNext(voClientContext *pNext)
 {
      m_pNext = pNext;
 }
/*! \fn String_StartsWithEx(const char *inString, int inStringLength, const char *startsWithString, int startsWithStringLength, int caseSensitive)
	\brief Determines if a string starts with a given substring
	\param inString Pointer to char* to process
	\param inStringLength The length of \a inString
	\param startsWithString The substring to match
	\param startsWithStringLength The length of \a startsWithString
	\param caseSensitive Non-zero if match is to be case sensitive
	\returns Non-zero if the string starts with the substring
*/
 bool voClientContext::SetStreamStartPosition(int position)
 {
	 m_nStartPosition = position;
	 return true;
 }
 bool voClientContext::SetStreamEndPosition(int position)
 {
	 m_nEndPosition = position;
	 return true;
 }
int voClientContext::String_StartsWithEx(const char *inString, int inStringLength, const char *startsWithString, int startsWithStringLength, int caseSensitive)
{
	int RetVal = 0;

	if(inStringLength>=startsWithStringLength)
	{
		if(caseSensitive!=0 && memcmp(inString,startsWithString,startsWithStringLength)==0)
		{
			RetVal = 1;
		}
		else if(caseSensitive==0 && strncasecmp(inString,startsWithString,startsWithStringLength)==0)
		{
			RetVal = 1;
		}
	}
	return(RetVal);
}
void voClientContext::SetDataStreamAPI(VO_DATA_STREAM_API *pDataStream)
{
	m_pDataStreamAPI = pDataStream;
}
VO_DATA_STREAM_API *voClientContext::GetDataStreamAPI()
{
	return m_pDataStreamAPI;
}
 int  voClientContext::ParseClientRequestion(char *sInfo)
 {
	char *seps = "\r\n\r\n";
	
	//char sCp[6024];
	char *sCp = new char[strlen(sInfo) + 1];
	char *token;
	strcpy(sCp,sInfo);
	sCp[strlen(sCp)] = 0;
	token = strtok(sCp,seps);
	char Range[100];
	memset(Range,0,sizeof(Range));
	while(token != NULL )
    {
	  // While there are tokens in "string"
	  printf( " %s\n", token );
	  // Get next token: 
	  token = strtok( NULL, seps ); // C4996
	  char *cpToken = (char *)token;
	  if(cpToken)
	  {
		  if(String_StartsWithEx(cpToken,(int)strlen(cpToken),"Range:",6,1))
		  {
			strcpy(Range,token);
			Range[strlen(Range)] = 0;
			break;
		  }
	  }
    }
	if(0 == strlen(Range))
	{
		delete []sCp;
		m_bIsRangeRequestion = false;//Range param not in the requestion
		//m_bIsRangeRequestion = true;
		return -1;
	}
	m_bIsRangeRequestion = true;	
	
	seps = "=-";
	token = strtok(Range,seps);
	
	int iPos = 0; 
	while(token != NULL )
    {
	  // While there are tokens in "string"
	  printf( " %s\n", token );
	  // Get next token: 
	  token = strtok( NULL, seps ); 
	  if(0 == iPos)
          m_nStartPosition = atoi(token);
	  else
	  {
		  if(NULL == token)
			 m_nEndPosition = 0;
		  else
		    m_nEndPosition = atoi(token);
	  }
	  iPos++;
    }
	delete []sCp;
    return 1;
 }

int voClientContext::Send_KeepAlive()
{
	 int nBytes = 0;
	 int nBufferLen = 0;
#ifdef _READLOCAL
     if(!m_pFileStream)//open file failed
		return S_OPENFAILED_ERROR;
	 if((m_nStartPosition > m_nEndPosition)||m_nStartPosition > m_nTotalFileSize)
		return S_REQUEST_ERROR;
	 if(m_nTotalFileSize > m_nEndPosition)
		nBufferLen = m_nEndPosition - m_nStartPosition;
	 else
		nBufferLen = m_nTotalFileSize - m_nStartPosition;
	 int nHeadlen = sprintf(m_szBuffer,"HTTP/1.1 206 Partial Content\r\nContent-Range: bytes 0-%d/%d\r\ntransferMode.dlna.org: Streaming\r\nContent-Type: video/mpeg\r\nTransfer-Encoding: chunked\r\n\r\n",(m_nTotalFileSize -1),(m_nTotalFileSize));
	 fseek(m_pFileStream, m_nStartPosition, SEEK_SET);
     int nNum = fread(m_szBuffer + nHeadlen,  1, nBufferLen, m_pFileStream);
     while(nBytes < nBufferLen)
	 {
		int nSendBytes = send(m_Socket, m_szBuffer, nBufferLen, 0);
		if(SOCKET_ERROR == nBytes)
		{
			return SOCKET_ERROR;
			 m_bIsNeedResponse = false;
		}
		nBytes += nSendBytes;
	 }
	 m_bIsNeedResponse = false;
#else

#endif
	 return nBytes;
}

int voClientContext::Send_Raw()
{
	 int nBytes = 0;
	 int nBufferLen = 0;
#ifdef _READLOCAL
     if(!m_pFileStream)//open file failed
		return S_OPENFAILED_ERROR;
	 if(m_nStartPosition > m_nTotalFileSize)
		return S_REQUEST_ERROR;
	 
	
     int nHeadlen = sprintf(m_szBuffer,"HTTP/1.1 206 Partial Content\r\nContent-Range: bytes 0-%d/%d\r\ntransferMode.dlna.org: Streaming\r\nContent-Type: video/mp4\r\nContent-Length: %d\r\nTransfer-Encoding: chunked\r\n\r\n",(m_nTotalFileSize -1),(m_nTotalFileSize),m_nTotalFileSize);
	 send(m_Socket, (m_szBuffer), nHeadlen, 0);//Send head first
	 fseek(m_pFileStream, m_nStartPosition, SEEK_SET);

	int nNumread = 0;
	int nSendBytes = 0;
	int nHeadSendBytes = 0;
	int nHeadLen = 0;
	int nEndLen = 0;
	int nEndBufferLen = 2;

	char HeadBuffer[200];
	char EndBuffer[20];
	char sBuffer[SENDSIZE + 1];
	memset(EndBuffer,0,sizeof(EndBuffer));
	sprintf(EndBuffer,"\r\n");
	memset(sBuffer,0,sizeof(sBuffer));
	while(nNumread = (int)fread(sBuffer,  sizeof(char), SENDSIZE, m_pFileStream))
	{
	    nHeadLen = sprintf(HeadBuffer,"%X\r\n",nNumread);
	    nHeadSendBytes = send(m_Socket, HeadBuffer, nHeadLen, 0);
		int nHeadBeenSentBytes = 0;
		while(nHeadSendBytes < nHeadLen)
		{
#ifdef WIN32
			Sleep(100);
#else
			sleep(100);
#endif
			
			if(SOCKET_ERROR != nHeadSendBytes)
				nHeadBeenSentBytes = nHeadSendBytes;
			nHeadSendBytes = send(m_Socket, (HeadBuffer + nHeadBeenSentBytes), (nHeadLen - nHeadBeenSentBytes), 0);
			if(nHeadSendBytes >= 0)
			{
				nHeadSendBytes += nHeadBeenSentBytes;
				nHeadBeenSentBytes = nHeadSendBytes;
				continue;
			}
			continue;
			//if(SOCKET_ERROR == nSendBytes)
			{
#ifdef WIN32
			//	int error = GetLastError();
			//	if( (WSAENETDOWN  != error)    &&
			//		(WSAEINTR     != error)    &&
			//		(WSAENETRESET != error)    &&
			//		(WSAESHUTDOWN != error)    &&
			//		(WSAECONNABORTED  != error)&&
			//		(WSAETIMEDOUT  != error)
			//	  )
				{
					continue;
				}
#endif
			}

#ifdef WIN32
			printf("send end string error: %p\n",GetLastError);
#endif
			break;

		}
	   if(SOCKET_ERROR == nHeadSendBytes)
	   {
#if defined(WIN32)
		   printf("send head length : %d socket error: %p return \n",nHeadLen,GetLastError);
#endif
			m_bIsNeedResponse = false;
			return nHeadSendBytes;
		}
		nSendBytes = send(m_Socket, sBuffer, nNumread, 0);
		int nBeenSentBytes = 0;
		while(nSendBytes < nNumread)
		{

			if(SOCKET_ERROR != nSendBytes)
			  nBeenSentBytes = nSendBytes;
			nSendBytes = send(m_Socket, sBuffer + nBeenSentBytes, nNumread, 0);
			if(SOCKET_ERROR == nSendBytes)
			{
#ifdef WIN32
				int error = GetLastError();
				if( (WSAENETDOWN  != error)    &&
					(WSAEINTR != error)        &&
					(WSAENETRESET != error)    &&
					(WSAESHUTDOWN != error)    &&
					(WSAECONNABORTED  != error)&&
					(WSAETIMEDOUT  != error)
				  )
				{
					continue;
				}
#endif
			}
			else if(SOCKET_ERROR != nSendBytes)
			{
				nSendBytes += nBeenSentBytes;
				nBeenSentBytes = nSendBytes;
				continue;
			}
			break;

		}
		printf("socket :%p Read num: %d  Sent bytes: %d\n",m_Socket,nNumread,nSendBytes);
		if(SOCKET_ERROR == nSendBytes)
		{
#if defined(WIN32)
			printf("socket error:%p return \n",GetLastError());
#endif
			m_bIsNeedResponse = false;
			return nSendBytes;
		}
	    nEndLen = send(m_Socket, EndBuffer, nEndBufferLen, 0);
		
		while(nEndLen < nEndBufferLen)
		{
#ifdef WIN32
			Sleep(100);
#else
			sleep(100);
#endif
			nEndLen = send(m_Socket, EndBuffer, nEndBufferLen, 0);
			if(nEndLen = nEndBufferLen)
				break;
			continue;
			//if(SOCKET_ERROR == nEndLen)
			{
#ifdef WIN32
				//int error = GetLastError();
				//if( (WSAENETDOWN  != error)    &&
				//	(WSAEINTR != error)        &&
				//	(WSAENETRESET != error)    &&
				//	(WSAESHUTDOWN != error)    &&
				//	(WSAECONNABORTED  != error)&&
				//	(WSAETIMEDOUT  != error)
				//  )
				{
					continue;
				}
#endif
			}

#ifdef WIN32
			printf("send end string error: %p\n",GetLastError);
#endif
			break;

		}
		nBytes += nSendBytes;
#if defined(WIN32)
		Sleep(25);
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
		sleep(25);
#endif
		 
	}
	 
#endif//_READLOCAL
    nEndLen = sprintf(sBuffer,"HTTP/1.1 %d %s\r\nContent-Length: 0\r\n\r\n",200,"OK");
	nSendBytes = send(m_Socket, sBuffer, nEndLen, 0);
	if(nSendBytes == SOCKET_ERROR)
	{
		 printf("send the 200 ok failed\n");
	}
	m_bIsNeedResponse = false;
	return (nBytes + nEndLen);	
}
 int  voClientContext::SendTo()
 {
	 int nBytes = 0;
	 if(false  == m_bIsRangeRequestion) //Range param not in the requestion
		 nBytes = Send_Raw();
		 //return S_REQUEST_NO_RANGE_ERROR;
     
     else if(m_nStartPosition == m_nEndPosition)//simple to d0 a judge
		nBytes = Send_Raw();
	 else if((m_nStartPosition > m_nEndPosition)&&(m_nEndPosition <= 0))
			nBytes = Send_Raw();
	 else if(m_nStartPosition < m_nEndPosition)
		nBytes = Send_KeepAlive();
	 else
		nBytes = Send_Raw();
    return nBytes;
 }
