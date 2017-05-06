/*
 *  CTSLiveSource.h
 *
 *  Created by Lin Jun on 4/28/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */
#ifndef CTSLiveSource_H_
#define CTSLiveSource_H_

#include "CSourceInterface.h"
#include "CUdpClientRecv.h"



// Cisco implement this class for live streaming output


typedef int (* READSOCKET) (void* pParam);

class CTSLiveSource : public CSourceInterface
{
public:
	CTSLiveSource(void * pUserData, SENDDATACB fSendData);
	virtual ~CTSLiveSource(void);
	
public:
	virtual bool Open(unsigned char* url, int nPort);
	virtual bool Close();
	virtual bool Start();
	virtual bool Pause();
	virtual bool Stop();
	
	virtual bool IsStatusClose(){return false;};
	virtual bool IsStatusOpen(){return false;};
	virtual bool IsStatusRunning(){return false;};
	virtual bool IsStatusStopped(){return false;};
	virtual bool IsStatusPaused(){return false;};
	
	static int PushDataEntry(void* pParam);
	int PushData();
	
private:
	void CreateThread(void** pHandle, int* pID, READSOCKET fProc, void* pParam, int uFlag);
	
private:
	CUdpClientRecv*		m_pUdpClient;
	void*			m_hReadThread;
	
	unsigned char*	m_pReadBuffer;
	int				m_nReadBufferSize;
	bool			m_bStop;
};

#endif //CTSLiveSource_H_