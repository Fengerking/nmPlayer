/*
 *  COMXALWrapper.h
 *
 *  Created by Lin Jun on 19/03/12.
 *	Refer example code of NDK (/android-ndk-r8b/samples/native-media/jni/native-media-jni.c)
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _COMXALWrapper_H__
#define _COMXALWrapper_H__

#include "voNPWrap.h"
#include "voCSemaphore.h"
#include "voCMutex.h"
#include "voThread.h"

#ifdef _VODEBUG
#include <stdio.h>
#endif	// _VODEBUG

#include <OMXAL/OpenMAXAL.h>
#include <OMXAL/OpenMAXAL_Android.h>
#include <android/native_window_jni.h>

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

// number of required interfaces for the MediaPlayer creation
#define NB_MAXAL_INTERFACES 3 // XAAndroidBufferQueueItf, XAStreamInformationItf and XAPlayItf

// number of buffers in our buffer queue, an arbitrary number
#define NB_BUFFERS 2

// we're streaming MPEG-2 transport stream data, operate on transport stream block size
#define MPEG2_TS_PACKET_SIZE 188

// number of MPEG-2 transport stream blocks per buffer, an arbitrary number
#define PACKETS_PER_BUFFER 10

// determines how much memory we're dedicating to memory caching
#define BUFFER_SIZE (PACKETS_PER_BUFFER*MPEG2_TS_PACKET_SIZE)


// constant to identify a buffer context which is the end of the stream to decode
static const int kEosBufferCntxt = 1980; // a magic value we can compare against

static const int kDiscontinuityBufferCntxt = 1981;

typedef enum
{
	READ_TYPE_INIT,
	READ_TYPE_NORMAL
}OMXAL_READ_TYPE;

//read buffer callback
typedef int	(* OMXALReadBuf) (void * pUserData, char* pBuffer, int* pWantReadSize, int nReadType);

typedef struct
{
	void*			pUserData;
	OMXALReadBuf	pRead;
}OMXAL_READBUFFER_FUNC;

//event callback
typedef int	(* OMXALEventCallback) (void * pUserData, int nEventID, void* pPram);

typedef enum
{
	OMXAL_EVENT_Seek_Complete,				//NULL
	OMXAL_EVENT_Current_Position, 			//int *, ms
	OMXAL_EVENT_VideoRender_Start, 			//NULL
}OMXAL_EVENT_TYPE;

typedef struct
{
	void*				pUserData;
	OMXALEventCallback	pEvenetCallback;
}OMXAL_EVENT_LISTENER;


class COMXALWrapper
{
public:
	COMXALWrapper(OMXAL_EVENT_LISTENER * pListener);
	virtual ~COMXALWrapper(void);
	
public:
	virtual int Init();
	virtual int Uninit();
	virtual int Open(OMXAL_READBUFFER_FUNC * pReadBufFunc, ANativeWindow * pNativeWindow);
	virtual int Close();
	virtual int Run();
	virtual int Pause();
	virtual int Stop();
	virtual int SetPos(int nPos);
	virtual int Flush();

	virtual	int	getPositionUpdatePeriod() {return (int)m_nPositionUpdatePeriod;}
    
private:
	int SendEvent(int nEventID, void* pParam);

    int CreateRunThread();
	static VO_U32	RunThreadProc(VO_PTR pParam);

	XAresult EnqueueEOS();
	XAresult EnqueueDiscontinuity(XAmillisecond * pnTimeMS);
	bool EnqueueInitialBuffers(bool bDiscontinuity);
	
private:
	static XAresult		AndroidBufferQueueCallback(XAAndroidBufferQueueItf caller, void * pCallbackContext, 
												void * pBufferContext, void * pBufferData, XAuint32 dataSize, XAuint32 dataUsed, 
												const XAAndroidBufferItem * pItems, XAuint32 itemsLength);
	virtual XAresult	OnAndroidBufferQueue(XAAndroidBufferQueueItf caller, 
											void * pBufferContext, void * pBufferData, XAuint32 dataSize, XAuint32 dataUsed, 
											const XAAndroidBufferItem * pItems, XAuint32 itemsLength);
	
	static void			StreamChangeCallback(XAStreamInformationItf caller, XAuint32 eventId, XAuint32 streamIndex, void * pEventData, void * pContext);
	virtual void		OnStreamChange(XAStreamInformationItf caller, XAuint32 eventId, XAuint32 streamIndex, void * pEventData);
	
	static void			PlayEventCallback(XAPlayItf caller, void * pContext, XAuint32 playEvent);
	virtual void		OnPlayEvent(XAPlayItf caller, XAuint32 playEvent);
	
private:
	bool					m_bStop;
	OMXAL_READBUFFER_FUNC*	m_pReadBufFunc;
	OMXAL_EVENT_LISTENER*	m_pEventListener;
	int						m_nCurrPos;
	
private:
	XAObjectItf				m_piEngineObject;		// engine object, entry
	XAEngineItf				m_piEngine;				// engine, get from m_piEngineObject and used for create output mix and media player
	XAObjectItf				m_piOutputMixObject;	// output mix
	XAObjectItf             m_piPlayerObject;		// player object
	XAPlayItf               m_piPlay;
	XAAndroidBufferQueueItf m_piAndroidBufferQueue;
	XAStreamInformationItf  m_piStreamInformation;
	XAVolumeItf             m_piVolume;
	
	// where we cache in memory the data to play
	// note this memory is re-used by the buffer queue callback
	char m_dataCache[BUFFER_SIZE * NB_BUFFERS];

	bool					m_bEOSReached;
	
	voCSemaphore	m_semSeek;
	voCMutex		m_mtxStatus;
	bool			m_bPause;

    voThreadHandle  m_hRunThread;
	XAmillisecond	m_nPositionUpdatePeriod;

#ifdef _VODEBUG
private:
	VO_BOOL			dumpTsFile(VO_PBYTE pBuffer, VO_U32 nSize);	// if pBuffer == NULL, dumping ends so close this file

private:
	bool			m_bDumpTsFile;
	int				m_nFileSuffix;
	FILE *			m_hDumpTsFile;
#endif	// _VODEBUG
	bool			m_bSendVideoRenderStartEvent;
};


#endif //_COMXALWrapper_H_