/*
 *  CReadSource.h
 *
 *  Created by Lin Jun on 4/21/11.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _READ_SRC_H_
#define _READ_SRC_H_

#include "voType.h"
#include "vompType.h"
#include "volspAPI.h"

class CSourceInterface;

struct voPlayerInfo
{
	int audio_buffer_time;
	int video_buffer_time;
	int video_drop_frame_num;
	int video_delay_time;
	int video_buffering_count;
	int audio_buffering_count;
};

typedef VO_U32 (* PushBufferCallBack) (void * pUserData, VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer);

class CReadSource
{
public:
	CReadSource(void* pUserData, PushBufferCallBack pPushBufferCallBack);
	virtual ~CReadSource();
	
public:
	int Start();
	int Pause();
	int Stop();
	int SetStream(unsigned char* pUrl, int nPort, bool bFile);
	
public:
	int PushBuffer(unsigned char* pData, int nSize);
	
private:
	bool				m_bProgramChanged;
	CSourceInterface*	m_pStreamingSource;
	
	void*				m_pUserData;
	PushBufferCallBack	m_pPushBufferCallBack;
};

#endif // _READ_SRC_H_