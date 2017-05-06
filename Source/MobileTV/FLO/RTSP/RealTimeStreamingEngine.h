#ifndef __REALTIMESTREAMINGENGINE_H__
#define __REALTIMESTREAMINGENGINE_H__

#include "network.h"

typedef int StreamReaderFunc(void * streamReaderParam);

class CStreamReader
{
public:
	CStreamReader(PIVOSocket streamSock, StreamReaderFunc * streamReader, void * streamReaderParam, int type);
	~CStreamReader();

public:
	PIVOSocket         m_streamSock;
	StreamReaderFunc * m_streamReader;
	void             * m_streamReaderParam;
	int                m_type;
};


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

	void Reset();

public:
	void AddStreamReader(PIVOSocket streamSock, StreamReaderFunc * streamReader, void * streamReaderParam,int type = VOS_STREAM_ALL);
	CStreamReader * FindStreamReader(PIVOSocket streamSock);
	void RemoveStreamReader(PIVOSocket streamSock);

public:
	virtual int ReadRealTimeStreams(int flag);
	
protected:
	CStreamReader * m_streamReaderSet[64];
	int             m_streamReaderCount;

protected:
	VOSocketGroup          m_streamSockSet;
};

#endif //__REALTIMESTREAMINGENGINE_H__