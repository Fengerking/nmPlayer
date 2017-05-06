#ifndef _VOCLIENTCONTEXT_H_
#define _VOCLIENTCONTEXT_H_

#include "voType.h"
#include "voDataStream.h"
//Disable deprecation warnings
#pragma warning(disable: 4996)

#if defined(WIN32)
//#define LOCAL_FILE_PATH  "C:\\Users\\Public\\Videos\\Sample Videos\\Clip_480p_5sec_6mbps_new.mpg"
#define LOCAL_FILE_PATH  "D:\\Test\\HttpServer\\vome_av.mp4"
#elif defined(_LINUX_ANDROID)
#define LOCAL_FILE_PATH ""
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR            (-1)
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET    -1
#endif
const int MAX_BUFFER_LEN = 1024*1000; 

const int SENDSIZE = 16384;
const int H_LEN = 1024;
const int S_OPENFAILED_ERROR = -1;
const int S_REQUEST_ERROR = -2;
const int S_REQUEST_NO_RANGE_ERROR = -3;

class voClientContext
{
private:
#if defined(WIN32)
   SOCKET    m_Socket;  //accepted socket
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
	int m_Socket;
#endif
   VO_PCHAR  m_pFilePath; //local file path that will be as the source content
   FILE * m_pFileStream; //The opened file handle
   VO_CHAR m_szBuffer[MAX_BUFFER_LEN];
   int  m_nTotalBytes;
   int  m_nSentBytes;
   int  m_nCurrentPosition;
   int  m_nStartPosition;
   int  m_nEndPosition;
   int  m_nSendLenth;
   int  m_nTotalFileSize;
   bool m_bIsRangeRequestion;
   bool m_bIsNeedResponse;
   
   voClientContext   *m_pNext; //this will be a singly linked list

public:
	voClientContext():m_nTotalBytes(0),m_nSentBytes(0),m_pNext(NULL),m_pFileStream(NULL),
					  m_nCurrentPosition(0),m_nSendLenth(0),m_nTotalFileSize(0),m_nStartPosition(0),
					  m_nEndPosition(0),m_bIsRangeRequestion(false),m_bIsNeedResponse(false),m_pDataStreamAPI(NULL)
     {
          m_Socket =  SOCKET_ERROR;
          memset(m_szBuffer,0, MAX_BUFFER_LEN);
#ifdef _READLOCAL
		  m_pFileStream = fopen(LOCAL_FILE_PATH,"rb");
		  if(!m_pFileStream)
		  {
			  printf("Fail to open the media file\n");
		  }
		  else
		  {
			fseek(m_pFileStream,0,SEEK_END);
			m_nTotalFileSize =  ftell(m_pFileStream);
			fseek(m_pFileStream,0,SEEK_SET);
		  }
#else
#endif

      }

	voClientContext(VO_DATA_STREAM_API *pDataStreamObject):m_nTotalBytes(0),m_nSentBytes(0),m_pNext(NULL),m_pFileStream(NULL),
					  m_nCurrentPosition(0),m_nSendLenth(0),m_nTotalFileSize(0),m_nStartPosition(0),
					  m_nEndPosition(0),m_bIsRangeRequestion(false),m_bIsNeedResponse(false),m_pDataStreamAPI(NULL)
     {
          m_Socket =  SOCKET_ERROR;
          memset(m_szBuffer,0, MAX_BUFFER_LEN);
#ifdef _READLOCAL
		  //m_pFileStream = fopen(LOCAL_FILE_PATH,"rb");
		  if(!pDataStreamObject->hHandle)
		  {
			  printf("Fail to open the media file\n");
		  }
		  else
		  {
			fseek((FILE*)pDataStreamObject->hHandle,0,SEEK_END);
			m_nTotalFileSize =  ftell((FILE*)pDataStreamObject->hHandle);
			fseek((FILE*)pDataStreamObject->hHandle,0,SEEK_SET);
		  }
#else
#endif

      }	


     //destructor
     ~voClientContext()
     {
		 if(m_pFileStream)
		 {
			 fclose(m_pFileStream);
			 m_pFileStream = NULL;
		 }
#if defined(WIN32)
          closesocket(m_Socket);
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
		 close(m_Socket);
#endif
     }  

     //Get/Set calls
	 void SetTotalBytes(int nNum);
	 int GetTotalBytes();
	 void SetSentBytes(int nNum);
	 void IncrSentBytes(int nNum);
	 int GetSentBytes();
	 void SetResponseFlag(bool bFlag);
	 bool GetResponseFlag();

	 bool SetStreamStartPosition(int position);//Seek begin position
	 bool SetStreamEndPosition(int position);  //Seek end position
	 int GetStreamStartPosition();
	 int GetStreamEndPosition();
#if defined(WIN32)
	 void SetSocket(SOCKET s);
	 SOCKET GetSocket();
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
	void SetSocket(int s);
	int GetSocket();
#endif
     void SetBuffer(char *szBuffer);
     void GetBuffer(char *szBuffer);
     char* GetBuffer();
     void ZeroBuffer();
	 FILE * GetFileStreamHandle();
	 void SetFileStreamHandle(FILE *f);

	 void SetDataStreamAPI(VO_DATA_STREAM_API *pDataStream);
	 VO_DATA_STREAM_API *GetDataStreamAPI();

	 int  SendTo();
	 int Send_KeepAlive();
	 int Send_Raw();
	 int String_StartsWithEx(const char *inString, int inStringLength, const char *startsWithString, int startsWithStringLength, int caseSensitive);
	 int  ParseClientRequestion(char *sInfo);

	 VO_DATA_STREAM_API *m_pDataStreamAPI;//Data stream API structure,it will be init and pass by voServerAnsycSocket object.

     voClientContext* GetNext();
     void SetNext(voClientContext *pNext);
    //Constructor

};

#endif