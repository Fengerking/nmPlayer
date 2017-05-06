#include "CDump.h"
#include "vcamerabase.h"

#ifdef _TEST_PERFORMANCE_
unsigned long dwVideoDumpCount = 0;
unsigned long dwAudioDumpCount = 0;
unsigned long dwAudioLastTime = 0;
unsigned long dwVideoLastTime = 0;
unsigned long dwRecvVideoSampleCount = 0;
unsigned long dwRecvAudioSampleCount = 0;
#endif

#define _NO_USE_THREAD


CDump::CDump(void)
	: m_bStop(true)
	, m_dwVideoLastTime(0)
	, m_nErrorCode(0)
	, m_llCurrFileSize(0)
	, m_nFrameRate(0.0)
	, m_hDump(NULL)
{
	memset(m_szName, 0, sizeof (m_szName));
}

void CDump::SetRealFrameRate(float fRate)
{
	m_nFrameRate = fRate;
}

CDump::~CDump(void)
{
}

bool CDump::SetName(const TCHAR * pName)
{
	if (pName == NULL)
		return false;

	_tcscpy (m_szName, pName);
	return true;
}

bool CDump::Start(void)
{
	if(!m_bStop)
		return false;

	m_llCurrFileSize = 0;

	m_nErrorCode = I_VORC_REC_NO_ERRORS;
	doStart();
	m_bStop = false;

	return true;
}

long long CDump::GetCurrFileSize()
{
	return m_llCurrFileSize;
}

// Must call SetName() befroe call this func
bool CDump::Init(void)
{

	bool bRet = doInit();

	return bRet;
}

bool CDump::Stop(void)
{
	m_bStop = true;

	if(!(m_nErrorCode&I_VORC_REC_DISK_FULL))
		DumpLeft();

	m_lstVideoFull.Release();
	m_lstVideoFree.Release();
	m_lstAudioFull.Release();
	m_lstAudioFree.Release();

#ifdef _TEST_PERFORMANCE_
// 	TCHAR debug[512];
// 	memset(&debug, 0, sizeof(TCHAR)*512);
// 	swprintf(debug, _T("Recved video sample=%u, audio=%u(%u), \nLast audio dump time is %u, video is %u. \nTotal audio dumped count %u(%u), video = %u.\n\n"), 
// 						dwRecvVideoSampleCount, dwRecvAudioSampleCount, dwRecvAudioSampleCount/50, dwAudioLastTime, dwVideoLastTime, dwAudioDumpCount, dwAudioDumpCount/50, dwVideoDumpCount);
// 	OutputDebugString(debug);

	dwVideoDumpCount = 0;
	dwAudioDumpCount = 0;
	dwRecvVideoSampleCount = 0;
	dwRecvAudioSampleCount = 0;
#endif

	return doStop();
}

void CDump::DumpLeft()
{
	int nAudioCount = 0;
	int nVideoCount = 0;
	
	unsigned long dwStart = 0, dwEnd = 0, dwLastTime = 0;

	//audio last dump time
	CSampleData* pFirstSample = m_lstAudioFull.GetTail();
	if(!pFirstSample)
		pFirstSample = m_lstAudioFree.GetTail();

	if(pFirstSample)
		pFirstSample->GetTime(dwStart, dwEnd);

	//video last dump time
/*	pFirstSample = m_lstVideoFull.GetTail();
	if(!pFirstSample)
		pFirstSample = m_lstVideoFree.GetTail();

	if(pFirstSample)
		pFirstSample->GetTime(dwStart, dwLastTime);

	//video > audio
	if(dwLastTime > dwEnd)
		dwLastTime = dwEnd;
*/
	dwLastTime = dwEnd;

	//begin dump left
	pFirstSample = m_lstAudioFull.GetHead();
	while(pFirstSample != NULL)
	{
		pFirstSample->GetTime(dwStart,dwEnd);

		//if(dwLastTime >= dwStart)
		{
			nAudioCount++;
			m_llCurrFileSize += pFirstSample->GetDataLen();
			DumpAudioSample(pFirstSample);

#ifdef _TEST_PERFORMANCE_
			dwAudioLastTime = dwEnd;
#endif
		}

		pFirstSample = pFirstSample->GetNext();
	}

	pFirstSample = m_lstVideoFull.GetHead();
	while(pFirstSample != 0)
	{
		pFirstSample->GetTime(dwStart, dwEnd);

		if(dwLastTime >= dwStart)
		{
			nVideoCount++;
			m_llCurrFileSize += pFirstSample->GetDataLen();
			DumpVideoSample(pFirstSample);

#ifdef _TEST_PERFORMANCE_
			dwVideoLastTime = dwEnd;
#endif

		}
		else
		{
			break;
		}

		pFirstSample = pFirstSample->GetNext();
	}

#ifdef _TEST_PERFORMANCE_
	dwVideoDumpCount += nVideoCount;
#endif


#ifdef _TEST_PERFORMANCE_
	dwAudioDumpCount += nAudioCount;

// 	TCHAR debug[100];
// 	memset(&debug, 0, 100);
// 	swprintf(debug, _T("++++++++++++++dump left video count = %d, audio = %d.++++++++++++++\n\n"), nVideoCount, nAudioCount);
// 	OutputDebugString(debug);
#endif
}


bool CDump::DumpAudioSample(VO_U32 dwTime)
{
	if(m_lstAudioFull.GetDuration () < dwTime)
		return false;

	VO_U32 dwStart, dwEnd;
	CSampleData* pFirstSample = m_lstAudioFull.GetHead ();
	pFirstSample->GetTime (dwStart, dwEnd);
	VO_U32 dwFirstStart = dwStart;

	int nSampleCount = 0;

	do{
		m_llCurrFileSize += pFirstSample->GetDataLen();
		DumpAudioSample(pFirstSample);

#ifdef _TEST_PERFORMANCE_
		pFirstSample->GetTime(dwStart, dwAudioLastTime);
#endif

		nSampleCount++;

		pFirstSample = pFirstSample->GetNext();

		if (pFirstSample == NULL)
			break;

		pFirstSample->GetTime (dwStart, dwEnd);

	}while(dwEnd - dwFirstStart <= dwTime);

#ifdef _TEST_PERFORMANCE_
	dwAudioDumpCount += nSampleCount;
#endif

	voCAutoLock lock (&m_csAudioSample);
	
	while(nSampleCount > 0)
	{
		m_lstAudioFree.AddTail(m_lstAudioFull.RemoveHead());
		nSampleCount--;
	}

	return true;
}


long long CDump::ReceiveAudioSample(CSampleData * pSample)
{
	if(m_bStop || m_nErrorCode&I_VORC_REC_DISK_FULL)
	{
		m_lstAudioFree.AddTail(pSample);
		return I_VORC_REC_NO_ERRORS;
	}

	voCAutoLock lock (&m_csAudioSample);

#ifdef _TEST_PERFORMANCE_
	dwRecvAudioSampleCount++;
#endif


#ifdef _TEST_PERFORMANCE_
	dwAudioDumpCount++;
	VO_U32 dwStart;
	pSample->GetTime(dwStart, dwAudioLastTime);
#endif

	m_llCurrFileSize += pSample->GetDataLen();
	int ret = DumpAudioSample(pSample);
	m_lstAudioFree.AddTail(pSample);

	if(ret != I_VORC_REC_NO_ERRORS)
	{
		m_nErrorCode |= ret;

		return ret;

// 		if(I_VORC_REC_MEMORY_FULL == ret)
// 		{
// 			m_nErrorCode |= I_VORC_REC_MEMORY_FULL;
// 			return I_VORC_REC_MEMORY_FULL;
// 		}
// 		else if(I_VORC_REC_DISK_FULL == ret)
// 		{
// 			m_nErrorCode |= I_VORC_REC_DISK_FULL;
// 			return I_VORC_REC_DISK_FULL;
// 		}
// 		else if(I_VORC_REC_WRITE_FAILED == ret)
// 		{
// 			m_nErrorCode |= I_VORC_REC_WRITE_FAILED;
// 			return I_VORC_REC_WRITE_FAILED;
// 		}
			
	}

	return I_VORC_REC_NO_ERRORS;
}

long long CDump::ReceiveVideoSample(CSampleData * pSample)
{
	if(m_bStop || m_nErrorCode&I_VORC_REC_DISK_FULL)
	{
		m_lstAudioFree.AddTail(pSample);
		return I_VORC_REC_NO_ERRORS;
	}

	//CvoiAutoLock lock (&m_csVideoSample);

#ifdef _TEST_PERFORMANCE_
	dwRecvVideoSampleCount++;
	dwVideoDumpCount++;
	VO_U32 dwStart;
	pSample->GetTime(dwStart, dwVideoLastTime);
#endif

	m_llCurrFileSize += pSample->GetDataLen();
	int ret = DumpVideoSample(pSample);
	m_lstVideoFree.AddTail(pSample);

	if(ret != I_VORC_REC_NO_ERRORS)
	{
		m_nErrorCode |= I_VORC_REC_MEMORY_FULL;
		return ret;

		/*if(VORC_FILEWRITE_OUT_OF_MEMORY == ret)
		{
			m_nErrorCode |= VORC_REC_MEMORY_FULL;
			return VORC_REC_MEMORY_FULL;
		}
		else if(VORC_FILEWRITE_DISK_FULL == ret)
		{
			m_nErrorCode |= VORC_REC_DISK_FULL;
			return VORC_REC_DISK_FULL;
		}
		else if(VORC_FILEWRITE_WRITE_FAILED == ret)
		{
			m_nErrorCode |= VORC_REC_WRITE_FAILED;
			return VORC_REC_WRITE_FAILED;
		}
		*/

	}

	return I_VORC_REC_NO_ERRORS;
}

CSampleData* CDump::GetVideoSampleData()
{
	if (m_bStop)
		return NULL;

	//voCAutoLock lockVideo (&m_csVideoSample);

	CSampleData * pFreeSample = m_lstVideoFree.RemoveHead();

	if(pFreeSample == NULL)
	{
		//note the total count
		if(m_lstVideoFull.GetSampleCount() < 400)
		{
			pFreeSample = new CSampleData();
			
			if(0 == pFreeSample)
			{						
				return false;
			}
		}
		else
			pFreeSample = m_lstVideoFull.RemoveHead();
	}
	
	//20070608
/*	if(m_nVideoSampleSize != 0)
	{
		if (pFreeSample != NULL)
			pFreeSample->AllocBuf (m_nVideoSampleSize);
	}
*/
	return pFreeSample;
}

CSampleData* CDump::GetAudioSampleData()
{
	if(m_bStop)
		return NULL;

	voCAutoLock lockAudio(&m_csAudioSample);

	CSampleData* pFreeSample = m_lstAudioFree.RemoveHead();

	if(pFreeSample == NULL)
	{
		//note the total count
		if(m_lstAudioFull.GetSampleCount() < 2000)
		{
			pFreeSample = new CSampleData();
			
			if(0 == pFreeSample)
			{						
				return false;
			}
		}
		else
			pFreeSample = m_lstAudioFull.RemoveHead();
	}

	return pFreeSample;
}

TCHAR* CDump::GetFileExtName()
{
	return _T(".mp4");
}




