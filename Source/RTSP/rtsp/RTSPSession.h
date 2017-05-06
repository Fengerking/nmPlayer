#ifndef __RTSPSESSION_H__
#define __RTSPSESSION_H__
#include "network.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMediaStream;
class CASFHeadParser;
class CRTSPClientEngine;
#define MAX_SESSION_TITLE_SIZE 1024
/**
\brief the alive rtsp session

1,parses the SDP of a session for rtspClientEngine,and maintain the session attributes.
2,creates the media stream
*/
class CRTSPSession
{
public:
	CRTSPSession(CRTSPClientEngine* engine);
	~CRTSPSession();

public:
	bool	ParseASFSDP(char * sessionDescription);
	/**parse the sdp from a sdp file or the response of Describe*/
	virtual bool ParseSDP(char * sessionDescription);
	/**creates the media streams*/
	virtual bool Init(char * sessionDescription, struct sockaddr_storage * sessionAddr = NULL);
	bool PauseLiveStream();
	bool ResumeLiveStream();
	void ResetPauseTime();
private:
	int m_dropFrames;
public:
	void SetDropFrames(int count){m_dropFrames=count;};
	int GetDropFrames(){return m_dropFrames;};
private:
	bool   IsMediaTypeSupported(char * sdpline_m);
	char * GetNextMediaType(char * sdpline);
	static unsigned long	   m_pausedTime[2];
protected:
	char m_sessionTitle[MAX_SESSION_TITLE_SIZE];

	bool ParseSDPParam_s(char * sdpParam);
	bool ParseSDPParam_c(char * sdpParam);
	bool ParseSDPParam_b_AS(char * sdpParam);
	bool ParseSDPParam_bitrate(char * sdpParam);
	bool ParseSDPAttribute_etag(char * sdpAttr);
	bool ParseSDPAttribute_range(char * sdpAttr);
	bool ParserSDPAttribute_ASFHead(char* sdpAttr);
public:
	struct sockaddr_storage * SessionAddr() { return m_sessionAddr; }
	unsigned int SessionBandwidth() { return m_sessionBandwidth; }
	char * SessionEntityTag() { return m_sessionEntityTag; }
	float SessionDuration();
	void SetSessionDuration(float duration) {m_sessionDuration=duration;}

	CMediaStream * MediaStream(int mediaStreamIndex);
	int MediaStreamCount() { return m_mediaStreamCount; }

protected:
	struct sockaddr_storage * m_sessionAddr;

	unsigned int     m_sessionBandwidth;
	char		   * m_sessionEntityTag;
	float            m_sessionDuration;
	CASFHeadParser	*m_asfheadParser;
public:
	CASFHeadParser	*GetASFHeadParser(){return m_asfheadParser;}
protected:
	CMediaStream      * m_mediaStreams[32];
	int                 m_mediaStreamCount;
	CMediaStream*	  m_applicationStream;
	CRTSPClientEngine* m_engine;
public:
	bool IsNeedRTCPSync();
	CMediaStream* GetApplicationStream(){return m_applicationStream;}
};


char * GetNextLine(char * text);
char * StringDup(const char * srcString);


#ifdef _VONAMESPACE
}
#endif

#endif //__RTSPSESSION_H__