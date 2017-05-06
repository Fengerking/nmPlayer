#ifndef __REALTIMESTREAMINGENGINE_H__
#define __REALTIMESTREAMINGENGINE_H__

#include "network.h"
#include "utility.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef int StreamReaderFunc(void * streamReaderParam);

class CStreamReader
{
public:
	CStreamReader(PIVOSocket streamSock, StreamReaderFunc * streamReader, void * streamReaderParam,int type);
	~CStreamReader();

public:
	PIVOSocket         m_streamSock;
	StreamReaderFunc * m_streamReader;
	void             * m_streamReaderParam;
	int m_type;
};

/**
\brief the engine of socket

It polls the socket,and notify the RTP or RTCP reader to get data when the data is ready on socket.
*/
class CRealTimeStreamingEngine
{
public:
	static CRealTimeStreamingEngine * CreateRealTimeStreamingEngine();
	static void DestroyRealTimeStreamingEngine();

protected:
	static CRealTimeStreamingEngine * m_pRealTimeStreamingEngine;

protected:
	CRealTimeStreamingEngine();
	virtual ~CRealTimeStreamingEngine();

public:
	void AddStreamReader(PIVOSocket streamSock, StreamReaderFunc * streamReader, void * streamReaderParam,int type=VOS_STREAM_ALL);
	CStreamReader * FindStreamReader(PIVOSocket streamSock);
	void RemoveStreamReader(PIVOSocket streamSock);

	bool StreamsReachable(unsigned int millisecond = 0);

public:
	virtual int ReadRealTimeStreams(int flag);
	int SetLocalStream(char* fileName);
	void SetLocalVideoPort(int rtpPort,int rtcpPort){portMap[LOCAL_VIDEO_RTP]=rtpPort;portMap[LOCAL_VIDEO_RTCP]=rtcpPort;};
	void SetLocalAudioPort(int rtpPort,int rtcpPort){portMap[LOCAL_AUDIO_RTP]=rtpPort;portMap[LOCAL_AUDIO_RTCP]=rtcpPort;};
	void Reset();
#ifdef DIMS_DATA
	void SetLocalDIMSPort(int rtpPort,int rtcpPort){portMap[LOCAL_DIMS_RTP]=rtpPort;portMap[LOCAL_DIMS_RTCP]=rtcpPort;};
#endif//
private://for local sdp2
	int	ReadRealTimeStreamsLocal();
	FILE* sdp2File;
	unsigned char* streamData;
	int portMap[8];
protected:
	CStreamReader * m_streamReaderSet[64];
	int             m_streamReaderCount;

protected:
	fd_set          m_streamSockSet;

};


#ifdef _VONAMESPACE
}
#endif




#endif //__REALTIMESTREAMINGENGINE_H__