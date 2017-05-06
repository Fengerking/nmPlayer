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
#endif
#include "voDataStream.h"
#include "voClientContext.h"
#include "voServerAnsycSocket.h"

//This function will loop on while it will manage
//multiple clients using select()
#if defined(WIN32)
void voServerAnsycSocket::AcceptConnections(SOCKET ListenSocket)
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
void voServerAnsycSocket::AcceptConnections(int ListenSocket)
#endif
{
    while (true)
     {
          InitSets(ListenSocket);

          if (select(0, &m_gReadSet, &m_gWriteSet, &m_gExceptSet, 0) > 0) 
          {
               //One of the socket changed state, let's process it.

               //ListenSocket?  Accept the new connection
               if (FD_ISSET(ListenSocket, &m_gReadSet)) 
               {
                    sockaddr_in ClientAddress;
                    int nClientLength = sizeof(ClientAddress);
#if defined(WIN32)
                    //Accept remote connection attempt from the client
                    SOCKET Socket = accept(ListenSocket, (sockaddr*)&ClientAddress, &nClientLength);
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
					int Socket = accept(ListenSocket, (sockaddr*)&ClientAddress, &nClientLength);
#endif

                    if (INVALID_SOCKET == Socket)
                    {
                         printf("\nError occurred while accepting socket: %ld.\n", GetSocketSpecificError(ListenSocket));
                    }

                    //Display Client's IP
                    printf("\nClient connected from: %s\n", inet_ntoa(ClientAddress.sin_addr)); 

                    //Making it a non blocking socket
#if defined(WIN32)
                    u_long nNoBlock = 1;
                    ioctlsocket(Socket, FIONBIO, &nNoBlock);
#elif defined(_LINUX_ANDROID)
					int oldflag = fcntl(Socket, F_GETFL, 0);
					fcntl(Socket, F_SETFL, oldflag | O_NONBLOCK );
#endif
                    voClientContext   *pClientContext  = new voClientContext;
					pClientContext->SetDataStreamAPI(m_pDataStreamAPI);//Pass the DataStreamAPI to related client
                    pClientContext->SetSocket(Socket);

                    //Add the client context to the list
                    AddClientContextToList(pClientContext);
               }

               //Error occured for ListenSocket?
               if (FD_ISSET(ListenSocket, &m_gExceptSet)) 
               {
                    printf("\nError occurred while accepting socket: %ld\n.", GetSocketSpecificError(ListenSocket));
                    continue;
               }

               //Iterate the client context list to see if any of the socket there has changed its state
               voClientContext   *pClientContext  = GetClientContextHead();

               while (pClientContext)
               {
                    //Check in Read Set
                    if (FD_ISSET(pClientContext->GetSocket(), &m_gReadSet))
                    {
						char sBuffer[102400];
						memset(sBuffer,0,sizeof(sBuffer));

                        //int nBytes = recv(pClientContext->GetSocket(), pClientContext->GetBuffer(), MAX_BUFFER_LEN, 0);
						int nBytes = recv(pClientContext->GetSocket(), sBuffer, MAX_BUFFER_LEN, 0);
						if(nBytes > 0)
						{
							printf("socket: %p recieved bytes num: %d recieved string: %s  \n",pClientContext->GetSocket(),nBytes,sBuffer);
						}
						pClientContext->ParseClientRequestion(sBuffer);

                         if ((0 == nBytes) || (SOCKET_ERROR == nBytes))
                         {
                              if (0 != nBytes) //Some error occured, client didn't close the connection
                              {
                                   printf("\nError occurred while recieving on the socket: %d.\n", GetSocketSpecificError(pClientContext->GetSocket()));
                              }

                              //In either case remove the client from list
                              pClientContext = DeleteClientContext(pClientContext);
                              continue;
                         }

                         //Set response flag
                         pClientContext->SetResponseFlag(true);
                         printf("\nThe following message was received: %s\n", sBuffer);
                    }

                    //Check in Write Set
                    if (FD_ISSET(pClientContext->GetSocket(), &m_gWriteSet))
                    {
                         int Result = 0;
						 Result = pClientContext->SendTo();
						 if (SOCKET_ERROR == Result)
						 {
                             printf("\nError occurred while sending on the socket: %p. than close it\n", GetSocketSpecificError(pClientContext->GetSocket()));
                             pClientContext = DeleteClientContext(pClientContext);
                             continue;
						 }
						 else if(S_REQUEST_NO_RANGE_ERROR == Result)
						 {
							 printf("\n Error: Range param not include in the requestion\n");
							 pClientContext->SetResponseFlag(false);   //Remove the Socket from the write socket sets
						 }

                    }

                    //Check in Exception Set
                    if (FD_ISSET(pClientContext->GetSocket(), &m_gExceptSet))
                    {
                         printf("\nError occurred on the socket: %p.\n", GetSocketSpecificError(pClientContext->GetSocket()));

                         pClientContext = DeleteClientContext(pClientContext);
                         continue;
                    }

                    //Move to next node on the list
                    pClientContext = pClientContext->GetNext();
               }//while
          }
          else //select
          {
#ifdef WIN32
               printf("\nError occurred while executing select(): %ld.\n", WSAGetLastError());
#endif
               return; //Get out of this function
          }
     }

}
#if defined(WIN32)
void voServerAnsycSocket::InitSets(SOCKET ListenSocket)
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
void voServerAnsycSocket::InitSets(int ListenSocket)
#endif
{
     //Initialize
     FD_ZERO(&m_gReadSet);
     FD_ZERO(&m_gWriteSet);
     FD_ZERO(&m_gExceptSet);

     //Assign the ListenSocket to Sets
     FD_SET(ListenSocket, &m_gReadSet);
     FD_SET(ListenSocket, &m_gExceptSet);

     //Iterate the client context list and assign the sockets to Sets
     voClientContext   *pClientContext  = GetClientContextHead();

     while(pClientContext)
     {
          if(pClientContext->GetResponseFlag())
          {
               //We have data to send
               FD_SET(pClientContext->GetSocket(), &m_gWriteSet);
          }
          else
          {
               //We can read on this socket
               FD_SET(pClientContext->GetSocket(), &m_gReadSet);
          }

          //Add it to Exception Set
          FD_SET(pClientContext->GetSocket(), &m_gExceptSet); 

          //Move to next node on the list
          pClientContext = pClientContext->GetNext();
     }
}
#if defined(WIN32)
int voServerAnsycSocket::GetSocketSpecificError(SOCKET Socket)
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
int voServerAnsycSocket::GetSocketSpecificError(int Socket)
#endif
{
     int nOptionValue;
     int nOptionValueLength = sizeof(nOptionValue);

     //Get error code specific to this socket
     getsockopt(Socket, SOL_SOCKET, SO_ERROR, (char*)&nOptionValue, &nOptionValueLength);

     return nOptionValue; 
}

voClientContext* voServerAnsycSocket::GetClientContextHead()
{
	return m_gpClientContextHead;
}

void voServerAnsycSocket::AddClientContextToList(voClientContext *pClientContext)
{
     //Add the new client context right at the head
     pClientContext->SetNext(m_gpClientContextHead);
     m_gpClientContextHead = pClientContext;
}

voClientContext* voServerAnsycSocket::DeleteClientContext(voClientContext *pClientContext)
{   
	//See if we have to delete the head node
     if (pClientContext == m_gpClientContextHead) 
     {
          voClientContext *pTemp = m_gpClientContextHead;
          m_gpClientContextHead = m_gpClientContextHead->GetNext();
          delete pTemp;
          return m_gpClientContextHead;
     }

     //Iterate the list and delete the appropriate node
     voClientContext *pPrev = m_gpClientContextHead;
     voClientContext *pCurr = m_gpClientContextHead->GetNext();

     while (pCurr)
     {
          if (pCurr == pClientContext)
          {
               voClientContext *pTemp = pCurr->GetNext();
               pPrev->SetNext(pTemp);
               delete pCurr;
               return pTemp;
          }

          pPrev = pCurr;
          pCurr = pCurr->GetNext();
     }

     return NULL;  
}