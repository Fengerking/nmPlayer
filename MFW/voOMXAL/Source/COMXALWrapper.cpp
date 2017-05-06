/*
 *  COMXALWrapper.cpp
 *
 *  Created by Lin Jun on 19/03/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include <assert.h>
#include "voLog.h"
#include "voOSFunc.h"
#include "COMXALWrapper.h"

COMXALWrapper::COMXALWrapper(OMXAL_EVENT_LISTENER * pListener)
	: m_bStop(false)
	, m_pReadBufFunc(NULL)
	, m_pEventListener(pListener)
	, m_nCurrPos(0)
	, m_piEngineObject(NULL)
	, m_piEngine(NULL)
	, m_piOutputMixObject(NULL)
	, m_piPlayerObject(NULL)
	, m_piPlay(NULL)
	, m_piAndroidBufferQueue(NULL)
	, m_piStreamInformation(NULL)
	, m_piVolume(NULL)
	, m_bEOSReached(false)
	, m_bPause(false)
	, m_hRunThread(NULL)
	, m_nPositionUpdatePeriod(100)
#ifdef _VODEBUG
	, m_bDumpTsFile(false)
	, m_nFileSuffix(0)
	, m_hDumpTsFile(NULL)
#endif	// _VODEBUG
	, m_bSendVideoRenderStartEvent(false)
{
#ifdef _VODEBUG
	FILE * hCheck = fopen("/sdcard/voomxal/dumpomxalenqueue.txt", "rb");
	if(hCheck)
	{
		m_bDumpTsFile = true;
		fclose(hCheck);
	}
#endif	// _VODEBUG
}

COMXALWrapper::~COMXALWrapper(void)
{
	//Uninit();

#ifdef _VODEBUG
	if(m_bDumpTsFile)
		dumpTsFile(NULL, 0);
#endif	// _VODEBUG
}

int COMXALWrapper::Init()
{
	VOLOGI("[OMXAL]+create Engine %x %x %x", (unsigned int)m_piEngineObject, (unsigned int)m_piEngine, (unsigned int)m_piOutputMixObject);
	
    // create engine
    XAresult res = xaCreateEngine(&m_piEngineObject, 0, NULL, 0, NULL, NULL);
    assert(XA_RESULT_SUCCESS == res);
	
    // realize the engine
	res = (*m_piEngineObject)->Realize(m_piEngineObject, XA_BOOLEAN_FALSE);
    assert(XA_RESULT_SUCCESS == res);
	
    // get the engine interface, which is needed in order to create other objects
    res = (*m_piEngineObject)->GetInterface(m_piEngineObject, XA_IID_ENGINE, &m_piEngine);
    assert(XA_RESULT_SUCCESS == res);
	
    // create output mix
    res = (*m_piEngine)->CreateOutputMix(m_piEngine, &m_piOutputMixObject, 0, NULL, NULL);
    assert(XA_RESULT_SUCCESS == res);
	
    // realize the output mix
    res = (*m_piOutputMixObject)->Realize(m_piOutputMixObject, XA_BOOLEAN_FALSE);
    assert(XA_RESULT_SUCCESS == res);
	
	VOLOGI("[OMXAL]-create Engine %x %x %x", (unsigned int)m_piEngineObject, (unsigned int)m_piEngine, (unsigned int)m_piOutputMixObject);
	if(XA_RESULT_SUCCESS != res)// avoid warning
		return VONP_ERR_Unknown;
	
	return VONP_ERR_None;
}

int COMXALWrapper::Uninit()
{
	VOLOGI("[OMXAL]+Uninit");
	
	Stop();
	Close();
	
	VOLOGI("[OMXAL]-Uninit");

	return VONP_ERR_None;
}

int COMXALWrapper::Open(OMXAL_READBUFFER_FUNC * pReadBufFunc, ANativeWindow * pNativeWindow)
{
	VOLOGI("[OMXAL]%x,+Create streaming media player", this);
    
    if(m_piPlayerObject)
    {
        VOLOGI("[OMXAL]Init done.-Create streaming media player");
        return VONP_ERR_None;
    }

	if(!pNativeWindow)
	{
		VOLOGE("[OMXAL]native window is NULL!!");
		return VONP_ERR_Pointer;
	}
	
	VOLOGI("[OMXAL]native window is %x", (unsigned int)pNativeWindow);
	
	m_nCurrPos	= 0;
	m_bStop		= false;
	m_pReadBufFunc = pReadBufFunc;
	
    // configure data source
    XADataLocator_AndroidBufferQueue dlAndroidBufferQueue = {XA_DATALOCATOR_ANDROIDBUFFERQUEUE, NB_BUFFERS};
    XADataFormat_MIME dfMime = {XA_DATAFORMAT_MIME, XA_ANDROID_MIME_MP2TS, XA_CONTAINERTYPE_MPEG_TS};
    XADataSource dsSource = {&dlAndroidBufferQueue, &dfMime};

	// XADataSink::pFormat is ignored if XADataSink::pLocator is XADataLocator_IODevice, XADataLocator_OutputMix or XADataLocator_NativeDisplay
    // configure audio sink
    XADataLocator_OutputMix dlOutputMix = {XA_DATALOCATOR_OUTPUTMIX, m_piOutputMixObject};
    XADataSink dsAudioSink = {&dlOutputMix, NULL};
	
    // configure video sink
    XADataLocator_NativeDisplay dlNativeDisplay = 
	{
		XA_DATALOCATOR_NATIVEDISPLAY,		// locatorType
		(void *)pNativeWindow,				// hWindow, pNativeWindow must be valid, otherwise, create media player will fail
		NULL								// hDisplay, must be NULL
    };
    XADataSink dsVideoSink = {&dlNativeDisplay, NULL};

    // declare interfaces to use
    XAboolean     bRequiredArray[NB_MAXAL_INTERFACES] = {XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE};
    XAInterfaceID iidArray[NB_MAXAL_INTERFACES] = {XA_IID_PLAY, XA_IID_ANDROIDBUFFERQUEUESOURCE, XA_IID_STREAMINFORMATION};

	XAresult res = (*m_piEngine)->CreateMediaPlayer(m_piEngine,					// engine
													&m_piPlayerObject,			// newly-created media player object
													&dsSource,					// data source
													NULL,						// instrument bank in Mobile DLS format, only MIDI used
													&dsAudioSink,				// audio data sink
													&dsVideoSink,				// image/video data sink
													NULL,						// the Vibra I/O device, only MIDI used
													NULL,						// the LED array I/O device, only MIDI used
													NB_MAXAL_INTERFACES,		// number of interfaces that the object is requested to support
													iidArray,					// array of numInterfaces interface IDs, which the object should support
													bRequiredArray);			// whether the respective interface is required on the object or optional
    assert(XA_RESULT_SUCCESS == res);

    res = (*m_piPlayerObject)->Realize(m_piPlayerObject, XA_BOOLEAN_FALSE);
    assert(XA_RESULT_SUCCESS == res);

	// play interface
    res = (*m_piPlayerObject)->GetInterface(m_piPlayerObject, XA_IID_PLAY, &m_piPlay);
    assert(XA_RESULT_SUCCESS == res);

	res = (*m_piPlay)->RegisterCallback(m_piPlay, PlayEventCallback, this);
	assert(XA_RESULT_SUCCESS == res);

	// "markers are delivered accurately, but new position updates are limited to every 100 ms" from \android-4.0.1_r1\android-4.0.1_r1\system\media\wilhelm\src\android\android_GenericPlayer.cpp
	// so we use 100ms, but it is not accurate actually!!!
	res = (*m_piPlay)->SetPositionUpdatePeriod(m_piPlay, m_nPositionUpdatePeriod);	// default 1s

/*	
	XA_PLAYEVENT_HEADATEND:		Playback head is at the end of the current content and the player has paused.
	XA_PLAYEVENT_HEADATMARKER:	Playback head is at the specified marker position.
	XA_PLAYEVENT_HEADATNEWPOS:	Playback head is at a new position (period between notifications is specified in by application).
	XA_PLAYEVENT_HEADMOVING:	Playback head has begun to move.
	XA_PLAYEVENT_HEADSTALLED:	Playback head has temporarily stopped moving.	
	Currently Android OMXAL seems support XA_PLAYEVENT_HEADATEND, XA_PLAYEVENT_HEADATMARKER, XA_PLAYEVENT_HEADATNEWPOS, East 20121214
*/
	res = (*m_piPlay)->SetCallbackEventsMask(m_piPlay, (XA_PLAYEVENT_HEADATNEWPOS | XA_PLAYEVENT_HEADATEND));

	// android buffer queue interface
	res = (*m_piPlayerObject)->GetInterface(m_piPlayerObject, XA_IID_ANDROIDBUFFERQUEUESOURCE, &m_piAndroidBufferQueue);
	assert(XA_RESULT_SUCCESS == res);

	res = (*m_piAndroidBufferQueue)->SetCallbackEventsMask(m_piAndroidBufferQueue, XA_ANDROIDBUFFERQUEUEEVENT_PROCESSED);
	assert(XA_RESULT_SUCCESS == res);

	res = (*m_piAndroidBufferQueue)->RegisterCallback(m_piAndroidBufferQueue, AndroidBufferQueueCallback, this);
	assert(XA_RESULT_SUCCESS == res);

    // stream information interface (for video size)
    res = (*m_piPlayerObject)->GetInterface(m_piPlayerObject, XA_IID_STREAMINFORMATION, &m_piStreamInformation);
    assert(XA_RESULT_SUCCESS == res);

	res = (*m_piStreamInformation)->RegisterStreamChangeCallback(m_piStreamInformation, StreamChangeCallback, this);
	assert(XA_RESULT_SUCCESS == res);
	
    // volume interface
    res = (*m_piPlayerObject)->GetInterface(m_piPlayerObject, XA_IID_VOLUME, &m_piVolume);
    assert(XA_RESULT_SUCCESS == res);
/*	
    // prepare the player
    res = (*m_piPlay)->SetPlayState(m_piPlay, XA_PLAYSTATE_PAUSED);
    assert(XA_RESULT_SUCCESS == res);
 */

	// set the volume
	// we need use [SL_MILLIBEL_MIN(-0x7FFF - 1), PLATFORM_MILLIBEL_MAX_VOLUME(0)], otherwise fail
	res = (*m_piVolume)->SetVolumeLevel(m_piVolume, 0);	// it means same as device media volume, if you want to mute it, you can use SL_MILLIBEL_MIN
	if(XA_RESULT_SUCCESS != res)
	{
		VOLOGW("[OMXAL]set volume level fail %d", res);
	}
	
	VOLOGI("[OMXAL]-Create streaming media player");

	return VONP_ERR_None;
}

int COMXALWrapper::Close()
{
	VOLOGI("[OMXAL]%x,+shutdown", this);
	// destroy streaming media player object, and invalidate all associated interfaces
    if (m_piPlayerObject != NULL)
	{
        (*m_piPlayerObject)->Destroy(m_piPlayerObject);
        m_piPlayerObject = NULL;

        m_piPlay = NULL;
        m_piAndroidBufferQueue = NULL;
        m_piStreamInformation = NULL;
        m_piVolume = NULL;
    }

	// destroy output mix object, and invalidate all associated interfaces
    if(m_piOutputMixObject != NULL) 
	{
        (*m_piOutputMixObject)->Destroy(m_piOutputMixObject);
        m_piOutputMixObject = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if(m_piEngineObject != NULL) 
	{
        (*m_piEngineObject)->Destroy(m_piEngineObject);
        m_piEngineObject = NULL;

        m_piEngine = NULL;
    }

	VOLOGI("[OMXAL]-shutdown");
	return VONP_ERR_None;
}

int COMXALWrapper::Run()
{
	VOLOGI("[OMXAL]Run");
	
	if(m_bPause)
	{
        // start the playback
        m_bPause	= false;
        m_bStop		= false;
        (*m_piPlay)->SetPlayState(m_piPlay, XA_PLAYSTATE_PLAYING);
	}
	else
	{
        CreateRunThread();
    }
	
	return VONP_ERR_None;
}

int COMXALWrapper::Pause()
{
	VOLOGI("[OMXAL]Pause");
	
    XAresult res = (*m_piPlay)->SetPlayState(m_piPlay, XA_PLAYSTATE_PAUSED);
	assert(XA_RESULT_SUCCESS == res);
	m_bPause = true;
	
	return (XA_RESULT_SUCCESS == res)?VONP_ERR_None:VONP_ERR_Unknown;
}

int COMXALWrapper::Stop()
{
	VOLOGI("[OMXAL]+Stop");

	m_bStop		= true;
	m_nCurrPos	= 0;
	voOS_Sleep(100);

//	if(m_piPlay)
//	{
//		XAresult res = (*m_piPlay)->SetPlayState(m_piPlay, XA_PLAYSTATE_STOPPED);
//		assert(XA_RESULT_SUCCESS == res);
//		return (XA_RESULT_SUCCESS == res)?VONP_ERR_None:VONP_ERR_Unknown;
//	}
	
	VOLOGI("[OMXAL]-Stop");

	return VONP_ERR_None;
}

int COMXALWrapper::SetPos(int nPos)
{
	m_nCurrPos = nPos;

	return VONP_ERR_None;
}

bool COMXALWrapper::EnqueueInitialBuffers(bool bDiscontinuity)
{
	VOLOGI("[OMXAL]Init enqueue buffer, discontinuity %s, read buffer %x", bDiscontinuity?"true":"false", (unsigned int)m_pReadBufFunc);
    /* Fill our cache.
     * We want to read whole packets (integral multiples of MPEG2_TS_PACKET_SIZE).
     * fread returns units of "elements" not bytes, so we ask for 1-byte elements
     * and then check that the number of elements is a multiple of the packet size.
     */
    size_t bytesRead = 0;
	bool bResChanged = false;
	char* pTemp = m_dataCache;
    
	for(int n=0; n<NB_BUFFERS; n++)
	{
		int nSize = 0;
		//while(nSize <= 0)
		{
			int nWantSize	= BUFFER_SIZE;
			int nRet = m_pReadBufFunc->pRead(m_pReadBufFunc->pUserData, pTemp, &nWantSize, bDiscontinuity ? READ_TYPE_NORMAL : READ_TYPE_INIT);
			
			if((unsigned int)nRet == VONP_ERR_Format_NotSupport)
            {
                VOLOGW("[OMXAL]%02d Return init ts buf by format not support.", n);
                return false;
            }
			else if((unsigned int)nRet == VONP_ERR_EOS)
			{
				VOLOGE("[OMXAL]Enqueue EOS, should not be here!!!");
				EnqueueEOS();
				return false;
			}
            else if((unsigned int)nRet == VONP_ERR_Format_Changed)
            {
                bResChanged = true;
                nSize = nWantSize;
                bytesRead	+= nSize;
                pTemp		+= nSize;
                break;
            }
			else if((unsigned int)nRet == VONP_ERR_FLush_Buffer)
			{
				VOLOGI("[OMXAL]VONP_ERR_FLush_Buffer");
				Flush();
				EnqueueDiscontinuity(NULL);
				return true;
			}
				
			if(nWantSize <= 0)
			{
				//VOLOGI("[OMXAL]Wait TS buffer ready");
				voOS_Sleep(10);
			}
            else
            {
                VOLOGI("[OMXAL]%02d Read TS : count %d, size %d", n, nWantSize/MPEG2_TS_PACKET_SIZE, nWantSize);
            }
			
			nSize = nWantSize;
			
			if(m_bStop)
            {
                VOLOGW("[OMXAL]Return init ts buf by stop, index %d", n);
                return false;
            }
		}
		
		bytesRead	+= nSize;
		pTemp		+= nSize;
	}
	
    if (bytesRead <= 0) 
	{
        // could be premature EOF or I/O error
		VOLOGI("[OMXAL]read buffer failed");
        return false;
    }
    if ((bytesRead % MPEG2_TS_PACKET_SIZE) != 0) 
	{
        VOLOGI("[OMXAL]Dropping last packet because it is not whole");
    }
    size_t packetsRead = bytesRead / MPEG2_TS_PACKET_SIZE;
    VOLOGI("[OMXAL]Initially queueing %u packets", packetsRead);
	
    /* Enqueue the content of our cache before starting to play,
	 we don't want to starve the player */
    size_t i;
    for (i = 0; i < NB_BUFFERS && packetsRead > 0; i++) 
	{
        if(m_bStop)
            return false;

        // compute size of this buffer
        size_t packetsThisBuffer = packetsRead;
        if (packetsThisBuffer > PACKETS_PER_BUFFER) 
		{
            packetsThisBuffer = PACKETS_PER_BUFFER;
        }
        size_t bufferSize = packetsThisBuffer * MPEG2_TS_PACKET_SIZE;
		XAresult res;
        if(bResChanged)
        {
#ifdef _VODEBUG
			if(m_bDumpTsFile)
				dumpTsFile(NULL, 0);
#endif	// _VODEBUG

            // signal resolution change
			VOLOGI("[OMXAL]Notify OMX AL,this is a resolution change buffer queue");
            XAAndroidBufferItem items[1];
            items[0].itemKey = XA_ANDROID_ITEMKEY_FORMAT_CHANGE;
            items[0].itemSize = 0;
            res = (*m_piAndroidBufferQueue)->Enqueue(m_piAndroidBufferQueue, NULL, m_dataCache + i * BUFFER_SIZE, bufferSize, items, sizeof(XAuint32) * 2);
#ifdef _VOLOG_INFO
			VOLOGI("Enqueue XA_ANDROID_ITEMKEY_FORMAT_CHANGE 0x%08X, %d successed %s", m_dataCache + i * BUFFER_SIZE, bufferSize, XA_RESULT_SUCCESS == res ? "YES" : "NO");
#else
			res = res;
#endif
            bResChanged = false;
        }
		else 
		{			
            res = (*m_piAndroidBufferQueue)->Enqueue(m_piAndroidBufferQueue, NULL, m_dataCache + i * BUFFER_SIZE, bufferSize, NULL, 0);
			VOLOGI("Enqueue 0x%08X, %d", m_dataCache + i * BUFFER_SIZE, bufferSize);
        }

#ifdef _VODEBUG
		if(m_bDumpTsFile)
			dumpTsFile((VO_PBYTE)(m_dataCache + i * BUFFER_SIZE), bufferSize);
#endif	// _VODEBUG

        assert(XA_RESULT_SUCCESS == res);
        packetsRead -= packetsThisBuffer;
    }
	
    return true;
}

// AndroidBufferQueueItf callback to supply MPEG-2 TS packets to the media player
XAresult COMXALWrapper::AndroidBufferQueueCallback(XAAndroidBufferQueueItf caller, void * pCallbackContext, 
												   void * pBufferContext, void * pBufferData, XAuint32 dataSize, XAuint32 dataUsed, 
												   const XAAndroidBufferItem * pItems, XAuint32 itemsLength)
{
	if(!pCallbackContext)
		return XA_RESULT_SUCCESS;
	
	COMXALWrapper* pWrap = (COMXALWrapper*)pCallbackContext;
	return pWrap->OnAndroidBufferQueue(caller, pBufferContext, pBufferData, dataSize, dataUsed, pItems, itemsLength);
}

XAresult COMXALWrapper::OnAndroidBufferQueue(XAAndroidBufferQueueItf caller, 
											 void * pBufferContext, void * pBufferData, XAuint32 dataSize, XAuint32 dataUsed, 
											 const XAAndroidBufferItem * pItems, XAuint32 itemsLength)
{
//	VOLOGI("OnAndroidBufferQueue 0x%08X, %d", pBufferData, dataSize);

	XAresult res	= XA_RESULT_SUCCESS;
	int nRet		= VONP_ERR_None;
	int nSize		= 0;
	int nWantSize	= BUFFER_SIZE;
	
	if(m_bStop)
	{
		VOLOGW("[OMXAL]Return buffer queue by stopped");
		return XA_RESULT_SUCCESS;
	}
	
	//while(nRet == VONP_ERR_WaitTime)
	{
		// note there is never any contention on this mutex unless a discontinuity request is active
		voCAutoLock lock(&m_mtxStatus);
		
		if ((pBufferData == NULL) && (pBufferContext != NULL)) 
		{
			const int processedCommand = *(int *)pBufferContext;
			if (kEosBufferCntxt == processedCommand) 
			{
				VOLOGI("[OMXAL]%x,EOS was processed\n", this);
				// our buffer with the EOS message has been consumed
				assert(0 == dataSize);
				nRet = VONP_ERR_None;
				return XA_RESULT_SUCCESS;
			}
			else if(kDiscontinuityBufferCntxt == processedCommand)
			{
				VOLOGI("[OMXAL]%x,Discontinuity was processed\n", this);

				EnqueueInitialBuffers(true);

				SendEvent(OMXAL_EVENT_Seek_Complete, NULL);
				m_semSeek.Signal();

				return XA_RESULT_SUCCESS;
			}
		}
		
		// pBufferData is a pointer to a buffer that we previously Enqueued
		assert((dataSize > 0) && ((dataSize % MPEG2_TS_PACKET_SIZE) == 0));
		assert(m_dataCache <= (char *) pBufferData && (char *) pBufferData <
			   &m_dataCache[BUFFER_SIZE * NB_BUFFERS]);
		assert(0 == (((char *) pBufferData - m_dataCache) % BUFFER_SIZE));
		
		// don't bother trying to read more data once we've hit EOF
		if(m_bEOSReached) 
			return XA_RESULT_SUCCESS;
		
		// note we do call fread from multiple threads, but never concurrently
		size_t bytesRead;
		
		nRet = VONP_ERR_Retry;
		nSize = 0;
		
		nWantSize = BUFFER_SIZE;
		if(m_pReadBufFunc)
			nRet = m_pReadBufFunc->pRead(m_pReadBufFunc->pUserData, (char*)pBufferData, &nWantSize, READ_TYPE_NORMAL);

		if(VONP_ERR_FLush_Buffer == nRet)
		{
			VOLOGI("[OMXAL]VONP_ERR_FLush_Buffer");
			Flush();
			EnqueueDiscontinuity(NULL);
			return XA_RESULT_SUCCESS;
		}

		if(nWantSize <= 0)
		{
			VOLOGI("[OMXAL]%x,Read buf failed in AndroidBufferQueueCallback", this);
		}

		nSize = nWantSize;
				
		if(VONP_ERR_EOS == nRet)
		{
			VOLOGI("[OMXAL]read returns EOS of TS stream...");
			bytesRead = 0;
		}
		else
			bytesRead	= nSize;

		if (bytesRead > 0) 
		{            
            size_t packetsRead = bytesRead / MPEG2_TS_PACKET_SIZE;
            size_t bufferSize = packetsRead * MPEG2_TS_PACKET_SIZE;

			if(VONP_ERR_Format_Changed == nRet)
			{
#ifdef _VODEBUG
				if(m_bDumpTsFile)
					dumpTsFile(NULL, 0);
#endif	// _VODEBUG

				VOLOGI("[OMXAL]read returns format changed...");
				XAAndroidBufferItem items[1];
				items[0].itemKey = XA_ANDROID_ITEMKEY_FORMAT_CHANGE;
				items[0].itemSize = 0;
				res = (*caller)->Enqueue(caller, NULL /*pBufferContext*/,
					pBufferData, 
					bufferSize, 
					items /*pMsg*/,
					sizeof(XAuint32)*2 /*msgLength*/);

				VOLOGI("Enqueue XA_ANDROID_ITEMKEY_FORMAT_CHANGE 0x%08X, %d", pBufferData, bufferSize);
			}
			else
			{
				res = (*caller)->Enqueue(caller, NULL /*pBufferContext*/,
					pBufferData /*pData*/,
					bufferSize /*dataLength*/,
					NULL /*pMsg*/,
					0 /*msgLength*/);
//				VOLOGI("Enqueue 0x%08X, %d", pBufferData, bufferSize);
			}

#ifdef _VODEBUG
			if(m_bDumpTsFile)
				dumpTsFile((VO_PBYTE)pBufferData, bufferSize);
#endif	// _VODEBUG

			assert(XA_RESULT_SUCCESS == res);

			if(false == m_bSendVideoRenderStartEvent)
			{
				m_bSendVideoRenderStartEvent = true;
				SendEvent(OMXAL_EVENT_VideoRender_Start, NULL);
			}
		} 
		else 
		{
			VOLOGI("[OMXAL]EOS of TS stream...");
			res = EnqueueEOS();
		}
	}

	if(XA_RESULT_SUCCESS != res) // avoid warning
		return XA_RESULT_UNKNOWN_ERROR;	

	return XA_RESULT_SUCCESS;	
}

// callback invoked whenever there is new or changed stream information
void COMXALWrapper::StreamChangeCallback(XAStreamInformationItf caller, XAuint32 eventId, XAuint32 streamIndex, void * pEventData, void * pContext)
{
	if(!pContext)
		return;
	
	COMXALWrapper* pWrap = (COMXALWrapper*)pContext;
	return pWrap->OnStreamChange(caller, eventId, streamIndex, pEventData);	
}

void COMXALWrapper::OnStreamChange(XAStreamInformationItf caller, XAuint32 eventId, XAuint32 streamIndex, void * pEventData)
{
	VOLOGI("[OMXAL]OnStreamChange event %u stream %u, sys time %d", eventId, streamIndex, voOS_GetSysTime());

    switch(eventId)
	{
	case XA_STREAMCBEVENT_PROPERTYCHANGE:
		{
			XAuint32 domain = XA_DOMAINTYPE_UNKNOWN;
			XAresult res = (*caller)->QueryStreamType(caller, streamIndex, &domain);
			if(XA_RESULT_SUCCESS != res)
			{
				VOLOGW("[OMXAL]QueryStreamType fail %d", res);
			}
			else
			{
				if(XA_DOMAINTYPE_VIDEO != domain)
				{
					VOLOGW("[OMXAL]Unexpected domain %u", domain);
				}
				else
				{
					XAVideoStreamInformation vsi;
					res = (*caller)->QueryStreamInformation(caller, streamIndex, &vsi);
					if(XA_RESULT_SUCCESS != res)
					{
						VOLOGW("[OMXAL]QueryStreamInformation fail %d", res);
					}
					else
					{
						// since it maybe be sent after playback and async, so we just show information
						// we will parse video sequence header ourselves
						VOLOGI("[OMXAL]Found video size %u x %u, codec ID %u, frameRate %u, bitRate %u, duration %u", 
							vsi.width, vsi.height, vsi.codecId, vsi.frameRate, vsi.bitRate, vsi.duration);
					}
				}
			}
		}
		break;

	default:
		{
			VOLOGW("[OMXAL]Unexpected stream event ID %u", eventId);
		}
		break;
    }
}

void COMXALWrapper::PlayEventCallback(XAPlayItf caller, void * pContext, XAuint32 playEvent)
{
	COMXALWrapper * pThis = (COMXALWrapper *)pContext;
	if(pThis)
		pThis->OnPlayEvent(caller, playEvent);
}

void COMXALWrapper::OnPlayEvent(XAPlayItf caller, XAuint32 playEvent)
{
//	VOLOGI("[OMXAL]PlayEvent caller 0x%08X, playEvent %d", caller, playEvent);

	if(XA_PLAYEVENT_HEADATNEWPOS & playEvent)
	{
		XAmillisecond nPosition = 0;
		XAresult res = (*caller)->GetPosition(caller, &nPosition);
		if(XA_RESULT_SUCCESS != res)
		{
			VOLOGW("[OMXAL]XA_PLAYEVENT_HEADATNEWPOS error 0x%08X", res);
		}
		else
		{
			VOLOGI("[OMXAL]XA_PLAYEVENT_HEADATNEWPOS position %d, sys time %d", (int)nPosition, voOS_GetSysTime());

			m_nCurrPos = nPosition;
			SendEvent(OMXAL_EVENT_Current_Position, &m_nCurrPos);
		}
	}

	if(XA_PLAYEVENT_HEADATEND & playEvent)
	{
		VOLOGI("[OMXAL]XA_PLAYEVENT_HEADATEND play end");
	}
}

int COMXALWrapper::Flush()
{
	XAresult res = XA_RESULT_SUCCESS;
	
	// clear the buffer queue
	VOLOGI("[OMXAL]+clear buffer queue");
	if(m_piAndroidBufferQueue)
		res = (*m_piAndroidBufferQueue)->Clear(m_piAndroidBufferQueue);
	VOLOGI("[OMXAL]-clear buffer queue");
	m_bSendVideoRenderStartEvent = false;
	assert(XA_RESULT_SUCCESS == res);
	
	if(XA_RESULT_SUCCESS != res) // avoid warning
		return XA_RESULT_UNKNOWN_ERROR;
	return XA_RESULT_SUCCESS;
}

int COMXALWrapper::SendEvent(int nEventID, void* pParam)
{
	if(m_pEventListener && m_pEventListener->pEvenetCallback && m_pEventListener->pUserData)
		return m_pEventListener->pEvenetCallback(m_pEventListener->pUserData, nEventID, pParam);

	return VONP_ERR_Pointer;
}

int COMXALWrapper::CreateRunThread()
{
	if(m_hRunThread)
		return VONP_ERR_None;

	VO_U32 nID = 0;
	voThreadCreate(&m_hRunThread, &nID, RunThreadProc, this, 0);
	return VONP_ERR_None;
}

VO_U32	COMXALWrapper::RunThreadProc(VO_PTR pParam)
{
#ifdef _VOLOG_INFO
    int nUseTime    = 0;
    nUseTime        = voOS_GetSysTime();
#endif
    
    COMXALWrapper* pWrap = (COMXALWrapper*)pParam;
    
    if(pWrap)
    {
        if (!pWrap->EnqueueInitialBuffers(false))
		{
			VOLOGE("[OMXAL]Init buffers fail or canceled!!!");
		}
        else
        {
            // start the playback
            pWrap->m_bPause	= false;
            pWrap->m_bStop	= false;
            (*(pWrap->m_piPlay))->SetPlayState(pWrap->m_piPlay, XA_PLAYSTATE_PLAYING);
        }
        
        pWrap->m_hRunThread = NULL;
    }
    
#ifdef _VOLOG_INFO
    VOLOGI("[OMXAL]Exit OMXAL run thread! system time %d, use time %d", voOS_GetSysTime(), voOS_GetSysTime()-nUseTime);
#endif
    
    return 0;
}

XAresult COMXALWrapper::EnqueueEOS()
{
    VOLOGI("[OMXAL]Enqueue EOS");

	if(!m_piAndroidBufferQueue)
		return XA_RESULT_UNKNOWN_ERROR;

    XAAndroidBufferItem msgEos[1];
    msgEos[0].itemKey = XA_ANDROID_ITEMKEY_EOS;
    msgEos[0].itemSize = 0;
	XAresult res = (*m_piAndroidBufferQueue)->Enqueue(m_piAndroidBufferQueue, (void *)&kEosBufferCntxt, NULL, 0, msgEos, sizeof(XAuint32) * 2);

	m_bEOSReached = true;

	return res;
}

XAresult COMXALWrapper::EnqueueDiscontinuity(XAmillisecond * pnTimeMS)
{
	VOLOGI("[OMXAL]Enqueue Discontinuity %d(ms)", pnTimeMS ? *pnTimeMS : -1);

	if(!m_piAndroidBufferQueue)
		return XA_RESULT_UNKNOWN_ERROR;

	XAuint8 bufItem[16];
	XAAndroidBufferItem * pItem = (XAAndroidBufferItem *)bufItem;
	pItem->itemKey = XA_ANDROID_ITEMKEY_DISCONTINUITY;
	pItem->itemSize = 0;
	XAuint32 nItemLength = sizeof(XAuint32) * 2;
	if(pnTimeMS)
	{
		// The new presentation time may be optionally specified as a parameter, expressed in itemData as a 64-bit unsigned integer in units of 90 kHz clock ticks.
		// In NuPlayer, it will skip rendering audio | video until this media time
		pItem->itemSize = sizeof(XAtime);
		nItemLength += sizeof(XAtime);

		XAtime * pnTime = (XAtime *)pItem->itemData;
		*pnTime = XAtime(*pnTimeMS) * 90;	// 90 kHz clock ticks
	}

	return (*m_piAndroidBufferQueue)->Enqueue(m_piAndroidBufferQueue, (void *)&kDiscontinuityBufferCntxt, NULL, 0, pItem, nItemLength);
}

#ifdef _VODEBUG
VO_BOOL COMXALWrapper::dumpTsFile(VO_PBYTE pBuffer, VO_U32 nSize)
{
	if(!pBuffer)
	{
		if(m_hDumpTsFile)
		{
			fclose(m_hDumpTsFile);
			m_hDumpTsFile = NULL;
		}

		return VO_TRUE;
	}

	if(!m_hDumpTsFile)
	{
		char szFile[256];
		sprintf(szFile, "/sdcard/voomxal/omxalenqueue_%d.ts", m_nFileSuffix);
		m_hDumpTsFile = fopen(szFile, "wb");

		if(NULL == m_hDumpTsFile)
		{
			VOLOGE("[OMXAL]create omxal enqueue dump ts file %d fail", m_nFileSuffix);
			return VO_FALSE;
		}

		m_nFileSuffix++;
	}

	if(nSize > 0)
		fwrite(pBuffer, 1, nSize, m_hDumpTsFile);

	return VO_TRUE;
}
#endif	// _VODEBUG
