
//#include <sys/time.h>
#include <string.h>

#include "network.h"
#include "mswitch.h"


#if _TEST_OCC_TIME

COCCTimeInfo * COCCTimeInfo::m_pOCCTimeInfo = NULL;

COCCTimeInfo * COCCTimeInfo::CreateInstance()
{
	if(m_pOCCTimeInfo == NULL)
	{
		m_pOCCTimeInfo = new COCCTimeInfo();
	}
	
	return m_pOCCTimeInfo;
}

void COCCTimeInfo::DestroyInstance()
{
	if(m_pOCCTimeInfo != NULL)
	{
		delete m_pOCCTimeInfo;
		m_pOCCTimeInfo = NULL;
	}
}

COCCTimeInfo::COCCTimeInfo()
{
	Reset();
}

COCCTimeInfo::~COCCTimeInfo()
{
}

void COCCTimeInfo::Reset()
{
	nOCCStartTime = 0;
	nOCCStopTime = 0;
	nOCCFirstVideoRTPReceivedTime = 0;
	nOCCFirstAudioRTPReceivedTime = 0;
	nOCCImageReadyTime = 0;
	nOCCImageRenderedTime = 0;
	
	nOCCStartTime_sec = 0;
	nOCCStopTime_sec = 0;
	nOCCFirstVideoRTPReceivedTime_sec = 0;
	nOCCFirstAudioRTPReceivedTime_sec = 0;
	nOCCImageReadyTime_sec = 0;
	nOCCImageRenderedTime_sec = 0;
	
	nOCCStartTime_usec = 0;
	nOCCStopTime_usec = 0;
	nOCCFirstVideoRTPReceivedTime_usec = 0;
	nOCCFirstAudioRTPReceivedTime_usec = 0;
	nOCCImageReadyTime_usec = 0;
	nOCCImageRenderedTime_usec = 0;
}

int COCCTimeInfo::GetTimeofDay(unsigned int & sec, unsigned int & usec)
{
	struct timeval tv;
	//gettimeofday(&tv, NULL);
	gettimeofday(&tv);
	sec = tv.tv_sec;
	usec = tv.tv_usec;
	return 0;
}

#endif //_TEST_OCC_TIME


char _LogDir[256] = "\0";

const char * GetLogDir()
{
#ifdef _WIN32
	strcpy(_LogDir, "C://voLog//");
#elif defined _LINUX
	strcpy(_LogDir, "/sdcard/");
#endif // _WIN32

	return _LogDir;
}
