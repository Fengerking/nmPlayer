#ifndef _VOSERVERANSYCSOCKET_H_
#define _VOSERVERANSYCSOCKET_H_
#include "voDataStream.h"
class voClientContext;
class voServerAnsycSocket
{
public:
	voServerAnsycSocket():m_gpClientContextHead(NULL){
		FD_ZERO(&m_gReadSet);
		FD_ZERO(&m_gWriteSet);
		FD_ZERO(&m_gExceptSet);
		m_pDataStreamAPI = new VO_DATA_STREAM_API;
		memset(m_pDataStreamAPI,0,sizeof(VO_DATA_STREAM_API));
	};
	~voServerAnsycSocket(){ 
		if(m_pDataStreamAPI)
		{
			m_pDataStreamAPI->Stop(m_pDataStreamAPI->hHandle);
			m_pDataStreamAPI->Uninit(m_pDataStreamAPI->hHandle);
			m_pDataStreamAPI->Close(m_pDataStreamAPI->hHandle);
			delete m_pDataStreamAPI;
			m_pDataStreamAPI = NULL;
		}

	}
#if defined(WIN32)
	void AcceptConnections(SOCKET ListenSocket);
	void InitSets(SOCKET ListenSocket);
	int GetSocketSpecificError(SOCKET Socket);
#elif defined(_POSIX)||defined(_LINUX_ANDROID)
	void AcceptConnections(int ListenSocket);
	void InitSets(int ListenSocket);
	int GetSocketSpecificError(int Socket);
#endif
	
	VO_DATA_STREAM_API * GetDtataStreamAPIObject();
	voClientContext* GetClientContextHead();
	void AddClientContextToList(voClientContext *pClientContext);
	voClientContext* DeleteClientContext(voClientContext *pClientContext);

private:
  fd_set m_gReadSet;  //g_ReadSet
  fd_set m_gWriteSet; //g_WriteSet
  fd_set m_gExceptSet;//g_ExceptSet
  VO_DATA_STREAM_API *m_pDataStreamAPI; //Contain the data stream related functions and pass it to client;
  voClientContext    *m_gpClientContextHead; //g_pClientContextHead
};
#endif