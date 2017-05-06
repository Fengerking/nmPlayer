#include "commonheader.h"
#include "sdp_headers.h"
#include "MediaStream.h"
#include "SynchronizeStreams.h"

#define SYNCHRONIZE_CYCLE (120)

CSynchronizeStreams * CSynchronizeStreams::m_pStandaloneInstance = NULL;

CSynchronizeStreams * CSynchronizeStreams::CreateInstance()
{
	if(m_pStandaloneInstance == NULL)
	{
		m_pStandaloneInstance = new CSynchronizeStreams();
	}

	return m_pStandaloneInstance;
}

void CSynchronizeStreams::DestroyInstance()
{
	SAFE_DELETE(m_pStandaloneInstance);
}


CSynchronizeStreams::CSynchronizeStreams()
{
}

CSynchronizeStreams::~CSynchronizeStreams()
{
}

void CSynchronizeStreams::AddStream(CMediaStream * pStream)
{
	list_T<CMediaStream *>::iterator iter;
	for(iter=m_listStream.begin(); iter!=m_listStream.end(); ++iter)
	{
		if(pStream == *iter)
			return;
	}
	m_listStream.push_back(pStream);
}

bool CSynchronizeStreams::AllStreamsHaveBeenSynchronized()
{
	return false;


	list_T<CMediaStream *>::iterator iter;
	for(iter=m_listStream.begin(); iter!=m_listStream.end(); ++iter)
	{
		CMediaStream * pStream = (CMediaStream *)(*iter);
		if(pStream->m_syncCount <= 1)
			return false;
	}

	return true;
}

void CSynchronizeStreams::ResynchronizeAllStreams()
{
	list_T<CMediaStream *>::iterator iter;
	for(iter=m_listStream.begin(); iter!=m_listStream.end(); ++iter)
	{
		CMediaStream * pStream = (CMediaStream *)(*iter);
		ResynchronizeStream(pStream);
	}
}

void CSynchronizeStreams::ResynchronizeStream(CMediaStream * pStream)
{
	pStream->m_syncCount = 0;
	pStream->m_syncWallClockTime.tv_sec = pStream->m_syncWallClockTime.tv_usec = 0;
	pStream->m_syncTimestamp = 0;
	pStream->m_wallClockTime.tv_sec = pStream->m_wallClockTime.tv_usec = 0;
	pStream->m_timestamp = 0;
}

void CSynchronizeStreams::SynchronizeStream(CMediaStream * pStream, unsigned int ntpTimestampMSW, unsigned int ntpTimestampLSW, unsigned int rtpTimestamp)
{
#if 0
	if((pStream->m_syncCount++) > 0) // synchronize only one time
		return;
#endif

#if 1
	if((pStream->m_syncCount++ % SYNCHRONIZE_CYCLE) != 0) // synchronize cycle 
		return;
#endif

	pStream->m_syncTimestamp = rtpTimestamp;
	pStream->m_syncWallClockTime.tv_sec = ntpTimestampMSW - 0x83AA7E80;
	double microseconds = (ntpTimestampLSW * 15625.0) / 0x04000000;
	pStream->m_syncWallClockTime.tv_usec = (unsigned int)(microseconds + 0.5);
}

void CSynchronizeStreams::CalculateFramePresentationTime(CMediaStream * pStream, unsigned int rtpTimestamp, __int64 * presentationTime)
{
	if(AllStreamsHaveBeenSynchronized())
	{
		int timestampDiff = rtpTimestamp - pStream->m_syncTimestamp;
		double timeDiff = timestampDiff / (double)pStream->m_clockRate;
		const unsigned int million = 1000000;
		unsigned int sec, usec;
		if(timeDiff >= 0.0)
		{
			sec  = pStream->m_syncWallClockTime.tv_sec + (unsigned int)(timeDiff);
			usec = pStream->m_syncWallClockTime.tv_usec + (unsigned int)((timeDiff - (unsigned int)timeDiff) * million);
			if(usec >= million)
			{
				usec -= million;
				++sec;
			}
		}
		else
		{
			timeDiff = -timeDiff;
			sec  = pStream->m_syncWallClockTime.tv_sec - (unsigned int)(timeDiff);
			usec = pStream->m_syncWallClockTime.tv_usec - (unsigned int)((timeDiff - (unsigned int)timeDiff) * million);
			if((int)usec < 0)
			{
				usec += million;
				--sec;
			}
		}

		pStream->m_syncTimestamp = rtpTimestamp;
		pStream->m_syncWallClockTime.tv_sec = sec;
		pStream->m_syncWallClockTime.tv_usec = usec;

		*presentationTime = Timeval2Int64(&(pStream->m_syncWallClockTime));
	}
	else
	{
		if(pStream->m_timestamp == 0)
		{
			pStream->m_timestamp = rtpTimestamp;
		}

		int timestampDiff = rtpTimestamp - pStream->m_timestamp;
		double timeDiff = timestampDiff / (double)pStream->m_clockRate;
		const unsigned int million = 1000000;
		unsigned int sec, usec;
		if(timeDiff >= 0.0)
		{
			sec  = pStream->m_wallClockTime.tv_sec + (unsigned int)(timeDiff);
			usec = pStream->m_wallClockTime.tv_usec + (unsigned int)((timeDiff - (unsigned int)timeDiff) * million);
			if(usec >= million)
			{
				usec -= million;
				++sec;
			}
		}
		else
		{
			timeDiff = -timeDiff;
			sec  = pStream->m_wallClockTime.tv_sec - (unsigned int)(timeDiff);
			usec = pStream->m_wallClockTime.tv_usec - (unsigned int)((timeDiff - (unsigned int)timeDiff) * million);
			if((int)usec < 0)
			{
				usec += million;
				--sec;
			}
		}

		pStream->m_timestamp = rtpTimestamp;
		pStream->m_wallClockTime.tv_sec = sec;
		pStream->m_wallClockTime.tv_usec = usec;

		*presentationTime = Timeval2Int64(&(pStream->m_wallClockTime));
	}
}

__int64 Timeval2Int64(struct timeval * _t)
{
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
	return _t->tv_sec*1000i64 + _t->tv_usec/1000i64;
#elif defined(LINUX)
	return _t->tv_sec*1000ll + _t->tv_usec/1000ll;
#endif
}
