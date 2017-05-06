#ifndef __SYNCHRONIZESTREAMS_H__
#define __SYNCHRONIZESTREAMS_H__

#include "netbase.h"
#include "list_T.h"

class CMediaStream;

class CSynchronizeStreams
{
public:
	static CSynchronizeStreams * CreateInstance();
	static void DestroyInstance();
protected:
	static CSynchronizeStreams * m_pStandaloneInstance;

protected:
	CSynchronizeStreams();
	virtual ~CSynchronizeStreams();

public:
	void AddStream(CMediaStream * pStream);
public:
	bool AllStreamsHaveBeenSynchronized();
public:
	void ResynchronizeAllStreams();
	void ResynchronizeStream(CMediaStream * pStream);
public:
	virtual void SynchronizeStream(CMediaStream * pStream, unsigned int ntpTimestampMSW, unsigned int ntpTimestampLSW, unsigned int rtpTimestamp);
	virtual void CalculateFramePresentationTime(CMediaStream * pStream, unsigned int rtpTimestamp, __int64 * presentationTime);

protected:
	list_T<CMediaStream *>    m_listStream;
};

__int64 Timeval2Int64(struct timeval * _t);

#endif //__SYNCHRONIZESTREAMS_H__