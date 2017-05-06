/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CFileMediaSink.h

Contains:	CFileMediaSink class file

Written by:	

Change History (most recent first):


*******************************************************************************/

#ifndef _CFILEMEDIASINK_H_
#define _CFILEMEDIASINK_H_

#include "CBaseMediaSink.h"

#include <pthread.h>

#include "voType.h"

class CFileSink;

class CFileMediaSink : public CBaseMediaSink
{
public:
	CFileMediaSink(int fd, int format, VOSF_MEDIABUFFERTYPE * pMediaBufferOP, 
					VOSF_METADATATYPE * pMetaDataOP, VOSF_MEDIASOURCETYPE * pMediaSourceOP);
	~CFileMediaSink();

	int	AddSource(void* pSource);
	int Start(void *pMetaData = 0);
	int Stop();
	int Pause();
	bool ReachEOS();

protected:
	VO_U32	getAudioCodec(const char* szMIME);
	VO_U32	getVideoCodec(const char* szMIME);

private:
	class Track
	{
	public:
		Track(CFileMediaSink* self, void* pSource);
		~Track();

		int start();
		void stop();
		int pause();
		bool reachedEOS();
		void* getFormat();

	private:
		void*				m_pSource;
		CFileMediaSink*		m_pSelf;
		volatile bool		m_bDone;
		volatile bool		m_bPaused;
		volatile bool		m_bResume;
		VO_U32				m_lPauseDuration;
		VO_U32				m_lPauseTime;

		pthread_t			m_Thread;	

		bool				m_bReachedEOS;
		bool				m_bAudio;

		static void *ThreadWrapper(void *me);
		void threadEntry();

		Track(const Track &);
		Track &operator=(const Track &);
	};
	friend class  Track;

	class TrackList
	{
	public:
		typedef struct tagNode  
		{
			Track* m_Data;
			tagNode* m_pNext;
		}NODE;
	public:
		TrackList();
		~TrackList();

		void push_back(Track* pTrack);
		NODE* getHead() const {return m_Head;}
		NODE* getNext(NODE* pNode) const;
		Track* getData(NODE* pNode) const;

	private:
		void destroy();

		NODE* m_Head;
		NODE* m_Tail;
	};

	CFileSink*		m_pSink;
	TrackList		m_Tracks;
	bool			m_bStarted;
};
#endif