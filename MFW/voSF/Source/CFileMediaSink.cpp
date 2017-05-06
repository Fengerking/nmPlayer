/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CFileMediaSink.cpp

Contains:	CFileMediaSink class file

Written by:	

Change History (most recent first):


*******************************************************************************/
#include "CFileMediaSink.h"
#include "CBaseMediaSource.h"

#include "CDataSourceOP.h"
#include "voAudio.h"
#include "voVideo.h"
#include "voFile.h"
#include "voIndex.h"
#include "cmnMemory.h"
#include "voOSFunc.h"
#include "voCMutex.h"
#include "CFileSink.h"

#include "voLog.h"
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CFileMediaSink"

//////////////////////////////////////////////////////////////////////////
CFileMediaSink::CFileMediaSink(int fd, int format, VOSF_MEDIABUFFERTYPE * pMediaBufferOP, 
							   VOSF_METADATATYPE * pMetaDataOP, VOSF_MEDIASOURCETYPE * pMediaSourceOP)
	: CBaseMediaSink(fdopen(fd,"wb"), format, pMediaBufferOP, pMetaDataOP, pMediaSourceOP),
	  m_pSink(NULL),
	  m_bStarted(false)
{
	cmnMemFillPointer('SFPS');
}

CFileMediaSink::~CFileMediaSink()
{

}

int CFileMediaSink::AddSource(void *pSource)
{
	Track *track = new Track(this, pSource);
	m_Tracks.push_back(track);

	return VO_ERR_NONE;
}

int CFileMediaSink::Start(void *pMetaData)
{
	if(m_bStarted)
		return VO_ERR_NONE;

	if(m_pSink == NULL)
	{
		VO_U32 nAudioCodec = VO_AUDIO_CodingUnused;
		VO_U32 nVideoCodec = VO_VIDEO_CodingUnused;
		bool bHasAudio = false;
		bool bHasVideo = false;
		VO_AUDIO_FORMAT audioFormat;
		VO_VIDEO_FORMAT videoFormat;

		for (TrackList::NODE* pNode = m_Tracks.getHead(); pNode != NULL; pNode = m_Tracks.getNext(pNode))
		{
			Track* pTrack = m_Tracks.getData(pNode);
			void* meta = pTrack->getFormat();
			const char* mime;
			m_pMetaDataOP->findCString(meta, kKeyMIMEType, &mime);
			if (!bHasAudio&&!vostrncmp(mime, _T("audio/"), 6))// audio track
			{
				bHasAudio = true;
				memset(&audioFormat, 0, sizeof(VO_AUDIO_FORMAT));
				m_pMetaDataOP->findInt32(meta, kKeySampleRate,	 (int*)&audioFormat.SampleRate);
				m_pMetaDataOP->findInt32(meta, kKeyChannelCount, (int*)&audioFormat.Channels);
				m_pMetaDataOP->findInt32(meta, kKeyBitRate,		 (int*)&audioFormat.SampleBits);
				if(audioFormat.SampleBits == 0)
					audioFormat.SampleBits = 16;				
				nAudioCodec = getAudioCodec(mime);			
			}else if (!bHasVideo&&!vostrncmp(mime, _T("video/"), 6))// video track
			{
				bHasVideo = true;
				memset(&videoFormat, 0, sizeof(VO_VIDEO_FORMAT));
				m_pMetaDataOP->findInt32(meta, kKeyWidth, (int*)&videoFormat.Width);
				m_pMetaDataOP->findInt32(meta, kKeyHeight,(int*)&videoFormat.Height);
				nVideoCodec = getVideoCodec(mime);				
			}
		}
		VO_FILE_SOURCE FileSource; 
		voFileFillPointer ();
		m_pSink = new CFileSink(NULL, (VO_FILE_FORMAT)m_format, NULL, NULL);
		if(NULL == m_pSink)
		{
			VOLOGE("create File Sink failed");
			return VO_ERR_FAILED;
		}

		memset(&FileSource, 0, sizeof(VO_FILE_SOURCE));
		FileSource.pSource	= (VO_PTR)m_fd;
		FileSource.nFlag	= VO_FILE_TYPE_HANDLE;
		FileSource.nMode	= VO_FILE_WRITE_ONLY;

		int ret = m_pSink->Init(&FileSource, nAudioCodec, nVideoCodec);
		if(ret!= VO_ERR_NONE)
		{
			VOLOGE("Init  File sink failed, ret=%d", ret);
			return VO_ERR_FAILED;
		}
		
		if(bHasAudio)
			m_pSink->SetParam(VO_PID_AUDIO_FORMAT, &audioFormat);
		if(bHasVideo)
			m_pSink->SetParam(VO_PID_VIDEO_FORMAT, &videoFormat);
	
		m_pSink->Start();
	}	

	for (TrackList::NODE* pNode = m_Tracks.getHead(); pNode != NULL; pNode = m_Tracks.getNext(pNode))
	{
		Track* pTrack = m_Tracks.getData(pNode);
		VO_U32 err = pTrack->start();

		if (err != VO_ERR_NONE) {	 
			for (TrackList::NODE* pNode2 = m_Tracks.getHead(); pNode2 != pNode; pNode2 = m_Tracks.getNext(pNode2))
			{
				Track* pTrack2 = m_Tracks.getData(pNode2);
				pTrack2->stop();
			}
			return err;
		}
	}
	m_bStarted = true;
	return VO_ERR_NONE;
}

int CFileMediaSink::Stop()
{
	if(!m_bStarted)
		return VO_ERR_NONE;

	for (TrackList::NODE* pNode = m_Tracks.getHead(); pNode != NULL; pNode = m_Tracks.getNext(pNode))
	{
		Track* pTrack = m_Tracks.getData(pNode);
		pTrack->stop();
	}
	if(m_pSink)
	{	
		m_pSink->Stop();
		delete m_pSink;
	}
	m_pSink = NULL;	
	if(m_fd)
		fclose(m_fd);
	m_fd = NULL;
	m_bStarted = false;
	return VO_ERR_NONE;
}

int CFileMediaSink::Pause()
{
	if(!m_bStarted)
		return VO_ERR_WRONG_STATUS;

	for (TrackList::NODE* pNode = m_Tracks.getHead(); pNode != NULL; pNode = m_Tracks.getNext(pNode))
	{
		Track* pTrack = m_Tracks.getData(pNode);
		pTrack->pause();
	}

	return VO_ERR_NONE;
}

bool CFileMediaSink::ReachEOS()
{
	bool bReachEOS = true;

	for (TrackList::NODE* pNode = m_Tracks.getHead(); pNode != NULL; pNode = m_Tracks.getNext(pNode))
	{
		Track* pTrack = m_Tracks.getData(pNode);
		bReachEOS &= pTrack->reachedEOS();
	}
	return bReachEOS;
}

VO_U32 CFileMediaSink::getAudioCodec(const char* szMIME)
{
	if(szMIME == NULL)
		return VO_AUDIO_CodingUnused;
	if (strstr (szMIME, "audio/3gpp") != NULL)
		return VO_AUDIO_CodingAMRNB;
	else if (strstr (szMIME, "audio/amr-wb") != NULL)
		return VO_AUDIO_CodingAMRWB;
	else if (strstr (szMIME, "audio/amr-wbp") != NULL)
		return VO_AUDIO_CodingAMRWBP;
	else if (strstr (szMIME, "audio/mpeg") != NULL)
		return VO_AUDIO_CodingMP3;
	else if (strstr (szMIME, "audio/mp4a-latm") != NULL)
		return VO_AUDIO_CodingAAC;
	else if (strstr (szMIME, "audio/qcelp") != NULL)
		return VO_AUDIO_CodingQCELP13;
	else if (strstr (szMIME, "audio/evrc") != NULL)
		return VO_AUDIO_CodingEVRC;
	else if (strstr (szMIME, "audio/flac") != NULL)
		return VO_AUDIO_CodingFLAC;
	else if (strstr (szMIME, "audio/ac3") != NULL)
		return VO_AUDIO_CodingAC3;
	else if (strstr (szMIME, "audio/adpcm") != NULL)
		return VO_AUDIO_CodingADPCM;
	else if (strstr (szMIME, "audio/ogg") != NULL)
		return VO_AUDIO_CodingOGG;
	else if (strstr (szMIME, "audio/wma") != NULL)
		return VO_AUDIO_CodingWMA;
	else if (strstr (szMIME, "audio/real") != NULL)
		return VO_AUDIO_CodingRA;
	else if (strstr (szMIME, "audio/midi") != NULL)
		return VO_AUDIO_CodingMIDI;
	else if (strstr (szMIME, "audio/dra") != NULL)
		return VO_AUDIO_CodingDRA;
	else if (strstr (szMIME, "audio/g729") != NULL)
		return VO_AUDIO_CodingG729;
	else if (strstr (szMIME, "audio/eac3") != NULL)
		return VO_AUDIO_CodingEAC3;
	else if (strstr (szMIME, "audio/ape") != NULL)
		return VO_AUDIO_CodingAPE;
	else if (strstr (szMIME, "audio/flac") != NULL)
		return VO_AUDIO_CodingALAC;
	else if (strstr (szMIME, "audio/dts") != NULL)
		return VO_AUDIO_CodingDTS;
	else
		return VO_AUDIO_CodingUnused;
}

VO_U32 CFileMediaSink::getVideoCodec(const char* szMIME)
{
	if(szMIME == NULL)
		return VO_VIDEO_CodingUnused;

	if (strstr (szMIME, "video/avc") != NULL)
		return VO_VIDEO_CodingH264;
	else if (strstr (szMIME, "video/mp4v-es") != NULL)
		return VO_VIDEO_CodingMPEG4;
	else if (strstr (szMIME, "video/3gpp") != NULL)
		return VO_VIDEO_CodingH263;
	else if (strstr (szMIME, "video/wmv") != NULL)
		return VO_VIDEO_CodingWMV;
	else if (strstr (szMIME, "video/rmvb") != NULL)
		return VO_VIDEO_CodingRV;
	else if (strstr (szMIME, "video/mpeg2") != NULL)
		return VO_VIDEO_CodingMPEG2;
	else if (strstr (szMIME, "video/s263") != NULL)
		return VO_VIDEO_CodingS263;
	else if (strstr (szMIME, "video/mjpeg") != NULL)
		return VO_VIDEO_CodingMJPEG;
	else if (strstr (szMIME, "video/vp6") != NULL)
		return VO_VIDEO_CodingVP6;
	else
		return VO_VIDEO_CodingUnused;
}


//////////////////////////////////////////////////////////////////////////
//implementation of Track

CFileMediaSink::Track::Track(CFileMediaSink* pSelf, void* pSource)
:	m_pSource(pSource),
	m_pSelf(pSelf),
	m_bDone(false),
	m_bPaused(false),
	m_bResume(false),
	m_lPauseDuration(0),
	m_lPauseTime(0),
	m_Thread(NULL),
	m_bReachedEOS(false),
	m_bAudio(true)
{
	const char *mime;
	void* meta = m_pSelf->m_pMediaSourceOP->getFormat(m_pSource);
	m_pSelf->m_pMetaDataOP->findCString(meta, kKeyMIMEType, &mime);
	m_bAudio = !vostrncmp(mime, _T("audio/"), 6);
}

CFileMediaSink::Track::~Track()
{
	stop();
}

int CFileMediaSink::Track::start()
{
	if(!m_bDone && m_bPaused)
	{
		if(m_bPaused)
		{
			m_bPaused = false;
			m_bResume = true;
		}
		return VO_ERR_NONE;
	}
	int err = m_pSelf->m_pMediaSourceOP->start(m_pSource, NULL);
	if (err != 0) {
		m_bDone = m_bReachedEOS = true;
		return err;
	}
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	m_bDone = false;
	m_bReachedEOS = false;

	pthread_create(&m_Thread, &attr, ThreadWrapper, this);
	pthread_attr_destroy(&attr);

	return VO_ERR_NONE;
}

void CFileMediaSink::Track::stop()
{
	if (m_bDone) {
		return;
	}
	m_bDone = true;
	void *dummy;
	pthread_join(m_Thread, &dummy);

	m_pSelf->m_pMediaSourceOP->stop(m_pSource);
}

int CFileMediaSink::Track::pause()
{
	m_bPaused = true;
	m_lPauseTime = voOS_GetSysTime();
	return VO_ERR_NONE;
}

bool CFileMediaSink::Track::reachedEOS()
{
	return m_bReachedEOS;
}
void* CFileMediaSink::Track::getFormat()
{
	return m_pSelf->m_pMediaSourceOP->getFormat(m_pSource);
}

void* CFileMediaSink::Track::ThreadWrapper(void *me)
{
	Track *track = static_cast<Track *>(me);

	track->threadEntry();
	return NULL;
}

void CFileMediaSink::Track::threadEntry()
{
	void* buffer;
	while (!m_bDone && m_pSelf->m_pMediaSourceOP->read(m_pSource, &buffer, NULL) == 0)
	{
		if(m_pSelf->m_pMediaBufferOP->range_length(buffer) == 0)
		{
			m_pSelf->m_pMediaBufferOP->release(buffer);
			buffer = NULL;
			continue;
		}
		if(m_bPaused&&!m_bResume)
		{
			m_pSelf->m_pMediaBufferOP->release(buffer);
			buffer = NULL;
			continue;
		}
		if(m_bResume)
		{
			m_bResume = false;
			m_lPauseDuration += voOS_GetSysTime() - m_lPauseTime;
			m_lPauseTime = 0;
		}

		void* meta = m_pSelf->m_pMediaBufferOP->meta_data(buffer);
		VO_S64 timestamp;
		m_pSelf->m_pMetaDataOP->findInt64(meta, kKeyTime, &timestamp);
			
		VO_SINK_SAMPLE	sinkSample;
		memset(&sinkSample, 0, sizeof(VO_SINK_SAMPLE));
		sinkSample.Buffer	= (VO_PBYTE)m_pSelf->m_pMediaBufferOP->data(buffer);
		sinkSample.Size		= m_pSelf->m_pMediaBufferOP->range_length(buffer);
		sinkSample.Time		= timestamp/1000 - m_lPauseDuration;
		sinkSample.nAV		= m_bAudio ? 0 : 1;

		m_pSelf->m_pSink->AddSample(&sinkSample);	

		m_pSelf->m_pMediaBufferOP->release(buffer);
		buffer = NULL;
	}
	m_bReachedEOS = true;
}

//////////////////////////////////////////////////////////////////////////
CFileMediaSink::TrackList::TrackList():m_Head(NULL), m_Tail(NULL)
{

}
CFileMediaSink::TrackList::~TrackList()
{
	destroy();
}

void CFileMediaSink::TrackList::push_back(CFileMediaSink::Track *pTrack)
{
	NODE* pNode = (NODE*)malloc(sizeof(NODE));
	pNode->m_pNext = NULL;
	pNode->m_Data  = pTrack;

	if(m_Head == NULL)
	{
		m_Head = m_Tail = pNode;
	}
	else
	{
		m_Tail->m_pNext = pNode;
		m_Tail = pNode;
	}
}

CFileMediaSink::TrackList::NODE* CFileMediaSink::TrackList::getNext(CFileMediaSink::TrackList::NODE *pNode) const
{
	if(pNode == NULL)
		return NULL;
	return pNode->m_pNext;
}

CFileMediaSink::Track* CFileMediaSink::TrackList::getData(CFileMediaSink::TrackList::NODE *pNode) const
{
	if(pNode == NULL)
		return NULL;
	return pNode->m_Data;		
}

void CFileMediaSink::TrackList::destroy()
{
	NODE* pNode;
	while (m_Head)
	{
		pNode = m_Head->m_pNext;
		delete m_Head->m_Data;
		free(m_Head);
		m_Head = pNode;
	}
	m_Head = m_Tail = NULL;
}