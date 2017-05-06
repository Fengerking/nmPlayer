#ifndef __RTSPSESSION_H__
#define __RTSPSESSION_H__

class CMediaStream;
class CRTSPClientEngine;

class CRTSPSession
{
public:
	CRTSPSession(CRTSPClientEngine * engine);
	~CRTSPSession();

public:
	virtual bool ParseSDP(char * sessionDescription);

	virtual bool Init(char * sessionDescription, struct voSocketAddress * sessionAddr = NULL);


private:
	char * GetMediaTypeSupported(char * sdpline_m);
	char * GetNextMediaType(char * sdpline);

protected:
	bool ParseSDPParam_c(char * sdpParam);
	
public:
	struct voSocketAddress * SessionAddr() { return m_sessionAddr; }

	CMediaStream * MediaStream(int mediaStreamIndex);
	int MediaStreamCount() { return m_mediaStreamCount; }

protected:
	struct voSocketAddress * m_sessionAddr;
	
protected:
	CMediaStream      * m_mediaStreams[32];
	int                 m_mediaStreamCount;

	CRTSPClientEngine * m_engine;
	typedef struct{
		int timeBegin;
		int errPackNum;
	}TErrPackHandler;
	TErrPackHandler		m_errHandler;
	void ResetErrHandler(){memset(&m_errHandler,0,sizeof(m_errHandler));}
public:
	void ProcessErrPack();
};


char * GetNextLine(char * text);
char * StringDup(const char * srcString);

#endif //__RTSPSESSION_H__