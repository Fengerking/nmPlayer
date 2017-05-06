#ifndef __PARSERTP_H__
#define __PARSERTP_H__

#include "voParser.h"

class CMediaStream;
class CReorderRTPPacket;
class CRTPPacket;
class CDescrambleEngine;

class CParseRTP
{
public:
	CParseRTP(CMediaStream * pMediaStream);
	virtual ~CParseRTP();

public:
	virtual VO_U32 Init();
public:
	VO_U32 Process(unsigned char * _rtpData, int rtpDataSize);
protected:
	virtual VO_U32 ParseRTPPayload();
protected:
	virtual bool DetectStreamLoop(CRTPPacket * pRTPPacket);

public:
	virtual void OnFrameStart(unsigned int rtpTimestamp);
	virtual void OnFrameData(void * _frameData, int frameSize);
	virtual void OnFrameEnd();
	virtual void OnFrameError(int errorCode);

public:
	CMediaStream * GetMediaStream() { return m_pMediaStream; }
protected:
	CMediaStream * m_pMediaStream;
	VO_PARSER_INIT_INFO * m_pParserInitInfo;
protected:
	CReorderRTPPacket * m_pReorderRTPPacket;
protected:
	unsigned int    m_lastRTPTimestamp;
protected:
	CDescrambleEngine * m_pDescrambleEngine;
protected:
	unsigned int    m_cFrame;
	unsigned long   m_sysTime;
protected:
	VO_MTV_FRAME_BUFFER        m_FrameBuffer;
	VO_PARSER_OUTPUT_BUFFER    m_OutputBuffer;


#ifdef _RTPPSR_LOG
protected:
	void DumpRTPInfo(CRTPPacket * pRTPPacket);
	void DumpFrameInfo();
#endif //_RTPPSR_LOG

};

#endif //__PARSERTP_H__