/*
 *  CSourceInterface.h
 *
 *  Created by Lin Jun on 4/28/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _SOURCE_INTERFACE_H_
#define _SOURCE_INTERFACE_H_

typedef int (* SENDDATACB) (void * pUserData, unsigned char * pData, int nDataLen);

class CSourceInterface
{
public:
	CSourceInterface(void * pUserData, SENDDATACB fSendData);
	virtual ~CSourceInterface(void);
	virtual bool Open(unsigned char* url, int nPort) = 0;
	virtual bool Close()=0;
	virtual bool Start()=0;
	virtual bool Pause()=0;
	virtual bool Stop()=0;
	
	virtual bool IsStatusClose()=0;
	virtual bool IsStatusOpen()=0;
	virtual bool IsStatusRunning()=0;
	virtual bool IsStatusStopped()=0;
	virtual bool IsStatusPaused()=0;
	
protected:
	void *		m_pUserData;
	SENDDATACB	m_fSendData;
};

#endif //_SOURCE_INTERFACE_H_