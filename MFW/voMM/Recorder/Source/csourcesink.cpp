#include "CSourceSink.h"
#include "CEncode.h"
#include "CAVEncoder.h"
#include "voOSFunc.h"


#if defined(DMEMLEAK)
#  include "dmemleak.h"
#endif 

//#define VS_BUF_COUNT 45


#ifdef _TEST_PERFORMANCE_

int nVideoDropCount = 0;
int nAudioDropCount = 0;
unsigned long dwRenderUseTime = 0;
unsigned long dwRenderedCount = 0;
unsigned long dwAudioEncodedCount = 0;
unsigned long dwVideoEncodedCount = 0;
unsigned long dwRecordTime = 0;
unsigned long dwVSRecCount = 0;
unsigned long dwASRecCount = 0;
unsigned long dwLastEncodedAudioTime = 0;
unsigned long dwLastRecvAudioTime = 0;

unsigned long aRecvFrameTimes[5000][2];//0:start time, 1:end time
unsigned long aEncodedFrameTimes[5000][3];//0:start time, 1:encode use time, 2:full buf current count
unsigned long aDropFrameTimes[5000][3];//0:start time, 1:drop type, 2:full buf current count
unsigned long aDrawFrameTimes[5000];//0:start time
unsigned long aRecvAudioTimes[5000][2];//0:start time, 1:end time


unsigned long dwAudioEncodeUseTime;
unsigned long dwAudioEncodeUseTimeAdd;
unsigned long dwAudioEncodeMaxUseTime;
unsigned long dwAudioEncodeMinUseTime;

unsigned long dwVideoEncodeUseTime;
unsigned long dwVideoEncodeUseTimeAdd;
unsigned long dwVideoEncodeMaxUseTime;
unsigned long dwVideoEncodeMinUseTime;

unsigned long dwEncodeCCUseTime;
unsigned long dwRenderCCUseTime;
unsigned long dwTmp;
bool bRecorded = false;


int IsVideoDrop(unsigned long& dwStart)
{
	for(int n=0; n<nVideoDropCount; n++)
	{
		if(dwStart == aDropFrameTimes[n][0])
			return n;
	}

	return -1;
}

bool IsFrameDraw(unsigned long& dwStart)
{
	for(int n=0; n<dwRenderedCount; n++)
	{
		if(dwStart == aDrawFrameTimes[n])
			return true;
	}

	return false;
}

unsigned long GetVideoIndex(unsigned long& dwStart)
{
	for(unsigned long n=0; n<dwVSRecCount; n++)
	{
		if(dwStart == aRecvFrameTimes[n][0])
			return n;
	}

	return 0;
}

void DumpPerformance(void)
{
	if(dwVSRecCount == 0)
		return;

	VO_U32	dwWrite = 0;
	int		i = 0;
	char	szInfo[256];

#ifdef WIN32
	VO_PTR hFile = cmnFileOpen(0, _T("c:\\recperf.txt"), VO_FILE_WRITE_ONLY);
#else
	VO_PTR hFile = cmnFileOpen(0, _T("/data/recperf.txt"), VO_FILE_WRITE_ONLY);
#endif

	sprintf(szInfo, "++++++++++++AS&VS Info+++++++++++++\r\n\r\n");
	cmnFileWrite(0, hFile, szInfo, strlen (szInfo));

	sprintf(szInfo, "VS total = %d, AS = %d, F/S = %f \r\nLast video frame time=%08d \r\nLast audio frame time=%08d\r\n", dwVSRecCount, dwASRecCount,
				dwVSRecCount*1000.0 / (float)(aRecvFrameTimes[dwVSRecCount-1][0]-aRecvFrameTimes[0][0]), aRecvFrameTimes[dwVSRecCount-1][0], dwLastRecvAudioTime);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
	sprintf(szInfo, "Record time is %d.Time error is %d.\r\n", dwRecordTime, dwRecordTime-dwLastEncodedAudioTime);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
	
	//encode
	int nAVTimeError = 0;
	if(dwVideoEncodedCount > 0)
		nAVTimeError = aEncodedFrameTimes[dwVideoEncodedCount-1][0]>dwLastEncodedAudioTime?(aEncodedFrameTimes[dwVideoEncodedCount-1][0]-dwLastEncodedAudioTime):(dwLastEncodedAudioTime-aEncodedFrameTimes[dwVideoEncodedCount-1][0]);

	sprintf(szInfo, "\r\nLast audio encode sample time is %d \r\nLast video encode sample time is %d.\r\nAV time error is %d\r\n", dwLastEncodedAudioTime, dwVideoEncodedCount==0?0:aEncodedFrameTimes[dwVideoEncodedCount-1][0], nAVTimeError);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
	int render_perf = ((float)dwRenderedCount/(float)dwVSRecCount)*100;
	int enc_perf = ((float)dwVideoEncodedCount/(float)dwVSRecCount)*100;
	float frmrate = 0;
	if(dwVideoEncodedCount > 0)
		frmrate = aEncodedFrameTimes[dwVideoEncodedCount-1][0]==0?0:(float)dwVideoEncodedCount*1000.0/((float)(aRecvFrameTimes[dwVSRecCount-1][0]-aRecvFrameTimes[0][0]));

	int enc_cc_time = dwVideoEncodedCount==0?0:dwEncodeCCUseTime / dwVideoEncodedCount;
	int render_cc_time = dwRenderedCount==0?0:dwRenderCCUseTime / dwRenderedCount;
	int render_average_time = dwRenderedCount==0?0:dwRenderUseTime / dwRenderedCount;
	sprintf(szInfo, "Rendered count %u, render performance %d(%d) \r\nVideo encode performance %d, F/S = %f.\r\nCC for encode time %d, CC for render time %d\r\n", dwRenderedCount, render_perf, render_average_time, enc_perf, frmrate, enc_cc_time, render_cc_time);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
	sprintf(szInfo, "Video encoded %d, video drop %d, audio encoded %d, audio drop %d\r\n", dwVideoEncodedCount, nVideoDropCount, dwAudioEncodedCount, nAudioDropCount);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
	sprintf(szInfo, "\r\nVideo encode average time is %04d(%04d)\r\n", dwVideoEncodedCount==0?0:dwVideoEncodeUseTime/dwVideoEncodedCount, dwVideoEncodedCount==0?0:dwVideoEncodeUseTimeAdd/dwVideoEncodedCount);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
	sprintf(szInfo, "Audio encode average time is %04d(%04d)\r\n", dwAudioEncodedCount==0?0:dwAudioEncodeUseTime/dwAudioEncodedCount, dwAudioEncodedCount==0?0:dwAudioEncodeUseTimeAdd/dwAudioEncodedCount);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
	sprintf(szInfo, "Video encode max time is %04d, min is %d\r\n", dwVideoEncodeMaxUseTime, dwVideoEncodeMinUseTime);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
	sprintf(szInfo, "Audio encode max time is %04d, min is %d\r\n", dwAudioEncodeMaxUseTime, dwAudioEncodeMinUseTime);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
	sprintf(szInfo, "tmp = %d\r\n\r\n", dwTmp);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));

	int nDropByBufFull = 0;
	int nDropByAVSync = 0;
	int nDropByMemory = 0;
	int nDropByUnknown = 0;
	int nDropIndex = 0;
	int nEncodedIndex = 0;
	bool bDraw = false;

	//if(0 > dwVSRecCount)
	{
		for (i = 0; i < dwVSRecCount - 1; i++)
		{
			bDraw = IsFrameDraw(aRecvFrameTimes[i][0]);
			nDropIndex = IsVideoDrop(aRecvFrameTimes[i][0]);

			if(-1 != nDropIndex)
			{
				if(aDropFrameTimes[nDropIndex][1] == 0)
					nDropByBufFull++;
				else if(aDropFrameTimes[nDropIndex][1] == 1)
					nDropByAVSync++;
				else if(aDropFrameTimes[nDropIndex][1] == 2)
					nDropByMemory++;
				else if(aDropFrameTimes[nDropIndex][1] == 3)
					nDropByUnknown++;


				sprintf (szInfo, "%04u  %06u  %06u %s I:%04u Drop(%d, %d)\r\n", 
					i, aRecvFrameTimes[i][0], aRecvFrameTimes[i][1], bDraw?"D":" ", aRecvFrameTimes[i+1][0] - aRecvFrameTimes[i][0], aDropFrameTimes[nDropIndex][1], aDropFrameTimes[nDropIndex][2]);
			}
			else
			{
				if(dwVideoEncodedCount>0 && aEncodedFrameTimes[nEncodedIndex][1]-15 > dwVideoEncodeUseTime/dwVideoEncodedCount)
				{
					int nOver = aEncodedFrameTimes[nEncodedIndex][1] - dwVideoEncodeUseTime/dwVideoEncodedCount;

					if(nOver > dwVideoEncodeUseTime/dwVideoEncodedCount)
						sprintf (szInfo, "%04u  %06u  %06u %s I:%04d ET:%03d[%03d]**\r\n", i, aRecvFrameTimes[i][0], aRecvFrameTimes[i][1], bDraw?"D":" ", aRecvFrameTimes[i+1][0] - aRecvFrameTimes[i][0], aEncodedFrameTimes[nEncodedIndex][1], aEncodedFrameTimes[nEncodedIndex][2]);
					else
						sprintf (szInfo, "%04u  %06u  %06u %s I:%04d ET:%03d[%03d]*\r\n", i, aRecvFrameTimes[i][0], aRecvFrameTimes[i][1], bDraw?"D":" ", aRecvFrameTimes[i+1][0] - aRecvFrameTimes[i][0], aEncodedFrameTimes[nEncodedIndex][1], aEncodedFrameTimes[nEncodedIndex][2]);
				}
				else
					sprintf (szInfo, "%04u  %06u  %06u %s I:%04d ET:%03d[%03d]\r\n", i, aRecvFrameTimes[i][0], aRecvFrameTimes[i][1], bDraw?"D":" ", aRecvFrameTimes[i+1][0] - aRecvFrameTimes[i][0], aEncodedFrameTimes[nEncodedIndex][1], aEncodedFrameTimes[nEncodedIndex][2]);

				nEncodedIndex++;
			}

			cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
		}
	}

	sprintf(szInfo, "\r\n\r\nDrop by buffer full(0) %d, drop by AV sync(1) %d\r\nDrop by unknown(2) %d, drop by memory(3) %d\r\n", 
						nDropByBufFull, nDropByAVSync, nDropByMemory, nDropByUnknown);
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));


	// dump audio frame
	sprintf(szInfo, "\r\n\r\nAll audio frame\r\n");
	cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
	if (dwASRecCount > 0)
	{
		for (i=0; i<dwASRecCount; i++)
		{
			sprintf (szInfo, "%04u  %06u  %06u I:%04u\r\n", 
				i, aRecvAudioTimes[i][0], aRecvAudioTimes[i][1], aRecvAudioTimes[i+1][0] - aRecvAudioTimes[i][0]);

			cmnFileWrite (0, hFile, szInfo, strlen (szInfo));
		}
	}

	cmnFileClose(0, hFile);
}

#endif

#define BUF_COUNT_NORMAL 20

CSourceSink::CSourceSink(void)
: m_pCCForEncoder(0)
, m_pAVEncoder(0)
, m_dwLastTime(0)
, m_dwStartTime(0)
, m_dwCurrAudioStartTime(0)
, m_dwCurrAudioLastTime(0)
, m_hEncodeAudioThread(NULL)
, m_hEncodeVideoThread(NULL)
, m_pNotifyCB(0)
, m_bResetting(false)
, m_bRecording(false)
, m_bHasPreviewVS(false)
, m_nVSBufCount(BUF_COUNT_NORMAL)
, m_dwSyncVideoDropTime(0)
, m_dwBufVideoDropTime(0)
, m_pSampleDataForVideoEncode(0)
, m_dwTotalPreviewTime(0)
, m_dwTotalVideoFrameCount(0)
, m_dwErrorCode(I_VORC_REC_NO_ERRORS)
, m_dwRecordTimeLimit(0)
, m_dwRecordSizeLimit(0)
, m_dwCurrRecTime(0)
, m_dwTotalAudioRawDataLen(0)
, m_bAdjustTimestamp(true)
, m_dwAudioLastTime(0)
, m_pUserData(NULL)
{
	memset(&m_RecordVideoFormat, 0, sizeof(m_RecordVideoFormat));
	m_pSampleDataForVideoEncode = new CSampleData;

// #if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE) 
// 	m_hEventEncodeAudio = CreateEvent(NULL, FALSE, FALSE, _T("EncodeAudioEvent"));
// 	m_hEventEncodeVideo = CreateEvent(NULL, FALSE, FALSE, _T("EncodeVideoEvent"));
// #elif defined(LINUX)
// 	m_hEventEncodeAudio = CreateEvent(NULL, FALSE, FALSE, NULL);
// 	m_hEventEncodeVideo = CreateEvent(NULL, FALSE, FALSE, NULL);
// #endif 

	memset(m_arDropTimes, 0, DROP_FRM_UPDATE_COUNT*sizeof(unsigned long));
	m_nDropFrmUpdateInx = 0;


	m_AudioFmt.fmt.Channels = 1;
	m_AudioFmt.fmt.SampleBits = 16;
	m_AudioFmt.fmt.SampleRate = 8000;

	//
	m_dwFirstAVFrameStartTime = 0;
	
	m_dwAudioLastFrameStartTime = 0;
	m_dwVideoLastFrameStartTime = 0;

	m_dwAudioPauseTime = 0;
	m_dwVideoPauseTime = 0;

	m_nAudioTimeOffset = 0;
	m_nAudioSampleDuration = 0;

#ifdef _TEST_THREAD_PERFORMANCE_
	m_pPerfTest = NULL;
	m_hRenderThread = NULL;
#endif
}

CSourceSink::~CSourceSink(void)
{
// 	if(m_hEventEncodeAudio)
// 		CloseHandle(m_hEventEncodeAudio);
}

void CSourceSink::Close()
{

#ifdef _TEST_PERFORMANCE_
// 	TCHAR debug[100];
// 	int tmp = dwRenderedCount==0?0:(((float)dwRenderedCount*100)/(float)m_dwTotalVideoFrameCount);
// 	int tmp2 = dwRenderedCount==0?0:(((float)dwRenderUseTime)/(float)dwRenderedCount);
// 	swprintf(debug, _T("\nRendered count %u, performance %d, render time %d, CC time %d\n\n"), dwRenderedCount, tmp, tmp2,dwRenderedCount==0?0:dwRenderCCUseTime / dwRenderedCount);
// 	OutputDebugString(debug);

#endif

#ifdef _TEST_THREAD_PERFORMANCE_
	if(m_pPerfTest)
	{
		//m_pPerfTest->EndRender(m_hRenderThread);
		delete m_pPerfTest;
		m_pPerfTest = NULL;
	}
#endif

	if(m_pCCForEncoder)
	{
		delete m_pCCForEncoder;
		m_pCCForEncoder = NULL;
	}

	if(m_hEncodeAudioThread)
	{
		voThreadClose(m_hEncodeAudioThread, 0);
		m_hEncodeAudioThread = NULL;
	}

	if(m_hEncodeVideoThread)
	{
		voThreadClose(m_hEncodeVideoThread, 0);
		m_hEncodeVideoThread = NULL;
	}

	ReleaseAVSamples();

	if(m_pSampleDataForVideoEncode)
	{
		delete m_pSampleDataForVideoEncode;
		m_pSampleDataForVideoEncode = 0;
	}

#ifdef _TEST_PERFORMANCE_
	if(!bRecorded)
		DumpPerformance();
#endif
}


void CSourceSink::PauseRecord()
{
	m_dwPauseTime = voOS_GetSysTime();
}


void CSourceSink::RestartRecord()
{
	m_dwPauseTime = voOS_GetSysTime() - m_dwPauseTime;
	m_nTotalPauseTime += m_dwPauseTime;
}


void CSourceSink::ReleaseAVSamples()
{
	m_lstFullRecVS.Release();
	m_lstFreeRecVS.Release();
	
	m_lstFullRecAS.Release();
	m_lstFreeRecAS.Release();
}


bool CSourceSink::StartRecord()
{
	CPerfTest::DumpLog("[REC] source sink start rec\r\n");

	if(m_bRecording)
		return true;

	m_dwErrorCode = I_VORC_REC_NO_ERRORS;

	memset(m_arDropTimes, 0, DROP_FRM_UPDATE_COUNT*sizeof(unsigned long));
	m_nDropFrmUpdateInx = 0;

	m_nTotalPauseTime = 0;
	m_dwCurrRecTime = 0;
	m_dwBufVideoDropTime = 0;
	m_dwSyncVideoDropTime = 0;
	m_dwCurrAudioStartTime = 0;
	m_dwCurrAudioLastTime = 0;

	m_dwAudioLastTime = 0;
	m_dwTotalAudioRawDataLen = 0;

	//
	m_dwFirstAVFrameStartTime = 0;
	m_dwAudioLastFrameStartTime = 0;
	m_dwVideoLastFrameStartTime = 0;
	m_dwAudioPauseTime = 0;
	m_dwVideoPauseTime = 0;
	m_nAudioTimeOffset = 0;
	m_nAudioSampleDuration = 0;


	if(m_RecordVideoFormat.width*m_RecordVideoFormat.height <= 176*144)
	{
		m_nVSBufCount = 80;
	}
	else
	{
		m_nVSBufCount = 50;
	}

	if(m_pAVEncoder)
	{
		m_dwErrorCode |= m_pAVEncoder->StartEncode();

		if(CheckErrorCode())
			return false;
	}
		

#ifdef _TEST_PERFORMANCE_
	nVideoDropCount = 0;
	nAudioDropCount = 0;
	dwRenderedCount = 0;
	dwLastEncodedAudioTime = 0;
	dwAudioEncodedCount = 0;
	dwVideoEncodedCount = 0;
	dwRecordTime = voOS_GetSysTime();
	dwASRecCount = 0;
	dwLastRecvAudioTime = 0;
	dwAudioEncodeUseTime = 0;
	dwVideoEncodeUseTime = 0;
	dwAudioEncodeUseTimeAdd = 0;
	dwVideoEncodeUseTimeAdd = 0;
	dwVideoEncodeMaxUseTime = 0;
	dwVideoEncodeMinUseTime = 10000;
	dwAudioEncodeMaxUseTime = 0;
	dwAudioEncodeMinUseTime = 10000;
	dwEncodeCCUseTime = 0;
	dwRenderCCUseTime = 0;
	dwRenderUseTime = 0;
	dwTmp = 0;
	bRecorded = true;

#endif
	
#ifdef _TEST_THREAD_PERFORMANCE_
	if(m_pPerfTest)
	{
		m_pPerfTest->Reset();
	}
	else
		m_pPerfTest = new CPerfTest;
#endif		


	m_bRecording = true;
	VO_U32 dwID;

	//if(m_RecordVideoFormat.width*m_RecordVideoFormat.height <= 176*144)
	if (true)
	{
		//m_hEncodeVideoThread = CreateThread(0, 0, EncodeAVFunc, this, 0, &dwID);
		voThreadCreate(&m_hEncodeVideoThread, &dwID, EncodeAVFunc, this, 0);
	}
	else
	{
#ifdef _TEST_THREAD_PERFORMANCE_
		if(m_pPerfTest && m_hRenderThread)
			m_pPerfTest->StartRender(m_hRenderThread);
#endif		

		if(m_pAVEncoder->IsRecordAudio())
		{
			//m_hEncodeAudioThread = CreateThread(0, 0, EncodeAudioFunc, this, 0, &dwID);		
			voThreadCreate(&m_hEncodeAudioThread, &dwID, EncodeAudioFunc, this, 0);		

#ifdef _TEST_THREAD_PERFORMANCE_
			if(m_pPerfTest)
				m_pPerfTest->StartAE(m_hEncodeAudioThread);
#endif		
		}

		if(m_pAVEncoder->IsRecordVideo())
		{
			//m_hEncodeVideoThread = CreateThread(0, 0, EncodeVideoFunc, this, 0, &dwID);
			voThreadCreate(&m_hEncodeVideoThread, &dwID, EncodeVideoFunc, this, 70);		

#ifdef _TEST_THREAD_PERFORMANCE_
			if(m_pPerfTest)
				m_pPerfTest->StartVE(m_hEncodeVideoThread);
#endif
		}
	}

	m_dwLastTimeForRecord = 0;
	m_dwStartTimeForRecord = voOS_GetSysTime();

	CPerfTest::DumpLog("[REC] source sink starts record ok\r\n");

	return true;
}


void CSourceSink::StopRecord()
{
	if(!m_bRecording)
		return;

#ifdef _TEST_PERFORMANCE_
	TCHAR debug[100];
	VO_U32 dwStart = voOS_GetSysTime();
	dwRecordTime = voOS_GetSysTime() - dwRecordTime;
#endif

#ifdef _TEST_THREAD_PERFORMANCE_
	m_pPerfTest->EndVE(m_hEncodeVideoThread);
	m_pPerfTest->EndAE(m_hEncodeAudioThread);
	if (m_hRenderThread)
	{
		m_pPerfTest->EndRender(m_hRenderThread);
	}
	//m_pPerfTest->EndRender(m_hRenderThread);
	m_pPerfTest->DumpResult();
	//delete m_pPerfTest;
	//m_pPerfTest = 0;
#endif

	m_bRecording = false;
	
	// 20090630
// 	WaitForSingleObject(m_hEncodeAudioThread, INFINITE);
// 	WaitForSingleObject(m_hEncodeVideoThread, INFINITE);
	while (m_hEncodeAudioThread != NULL)
		voOS_Sleep(2);
	while (m_hEncodeVideoThread != NULL)
		voOS_Sleep(2);
	
	if(!(m_dwErrorCode & I_VORC_REC_DISK_FULL))
	{
		EncodeLeft();
	}

#ifdef _TEST_PERFORMANCE_
	DumpPerformance();
#endif

	if(m_pAVEncoder)
		m_pAVEncoder->StopEncode();

	if(m_hEncodeAudioThread)
	{
		voThreadClose(m_hEncodeAudioThread, 0);
		m_hEncodeAudioThread = NULL;
	}

	if(m_hEncodeVideoThread)
	{
		voThreadClose(m_hEncodeVideoThread, 0);
		m_hEncodeVideoThread = NULL;
	}

	ReleaseAVSamples();

#ifdef _TEST_PERFORMANCE_
	_stprintf(debug, _T("CSourceSink::StopRecord() use time = %d.\n"), voOS_GetSysTime()-dwStart);
	//OutputDebugString(debug);
#endif

	m_nVSBufCount = BUF_COUNT_NORMAL;
}


VO_U32 CSourceSink::EncodeAudioFunc(VO_PTR lParam)
{
	CSourceSink* This = (CSourceSink*)lParam;
	This->EncodeAudioSamples();

	return 0;
}


VO_U32 CSourceSink::EncodeVideoFunc(VO_PTR lParam)
{
	CSourceSink* This = (CSourceSink*)lParam;
	This->EncodeVideoSamples();

	return 0;
}

VO_U32 CSourceSink::EncodeAVFunc(VO_PTR lParam)
{
	CSourceSink* This = (CSourceSink*)lParam;
	This->EncodeAudioVideoSamples();

	return 0;
}

bool CSourceSink::IsNeedEncode(unsigned long& dwStart, unsigned long& dwEnd)
{
	unsigned long dwCurrAudioStartTime = m_dwCurrAudioStartTime;
	unsigned long dwCurrAudioLastTime = m_dwCurrAudioLastTime;

	if(dwEnd > dwCurrAudioStartTime)
		return true;
				
	if(m_lstFullRecVS.GetSampleCount() < m_nVSBufCount-5)
		return true;

	if(IsDropFrmNeighbor(dwStart, dwEnd))
		return true;

	return false;
}


bool CSourceSink::EncodeVideoSamples(CSampleData* pSample)
{
	bool bColorConvertOk = false;
	unsigned long dwStart, dwEnd;
	pSample->GetTime(dwStart, dwEnd);

	if(!IsRecordAudio())
		m_dwCurrRecTime = dwEnd;

	InputBuffer in;
	OutputBuffer* out = 0;
	in.pInBuf = pSample->GetDataPoint();

	if(m_pCCForEncoder)
	{
#ifdef _TEST_PERFORMANCE_
		unsigned long cc_time = voOS_GetSysTime();
#endif
// 		if(!m_pCCForEncoder->Convert(&out, &in))
// 		{
// 			m_dwErrorCode |= VORC_REC_CC_ERROR;
// 			return false;
// 		}

		if(!m_pSampleDataForVideoEncode->GetDataPoint())
		{
			m_pSampleDataForVideoEncode->AllocBuf(out->OutBufSize[0]+out->OutBufSize[1]+out->OutBufSize[2]);
			m_pSampleDataForVideoEncode->SetDataLen(out->OutBufSize[0]+out->OutBufSize[1]+out->OutBufSize[2]);
		}

#ifdef _TEST_PERFORMANCE_
		cc_time = voOS_GetSysTime() - cc_time;
		dwEncodeCCUseTime += cc_time;
#endif
			
		memcpy(m_pSampleDataForVideoEncode->GetDataPoint(), out->pOutBuf[0], out->OutBufSize[0]+out->OutBufSize[1]+out->OutBufSize[2]), 
		m_pSampleDataForVideoEncode->SetTime(dwStart, dwEnd);

		VO_IV_COLORTYPE v = VO_COLOR_YUV_PLANAR420;
		m_dwErrorCode |= m_pAVEncoder->RecvVideoSample(m_pSampleDataForVideoEncode, v);
	}
	else
	{
		//no need cc,yuv420planner,vyuv422packed,rgb565
		//CPerfTest::DumpLog("begin to encode video\r\n");
		m_dwErrorCode |= m_pAVEncoder->RecvVideoSample(pSample, m_RecordVideoFormat.video_type);
	}

	ProcessErrorCode();
	return true;
}

bool CSourceSink::ProcessErrorCode()
{
	if(m_dwErrorCode & I_VORC_REC_DISK_FULL)
	{
		if(m_pNotifyCB)
			m_pNotifyCB(m_pUserData, VO_MMR_MSG_DISK_FULL, 0);

		return true;
	}
	else if(m_dwErrorCode & I_VORC_REC_MEMORY_FULL)
	{
		if(m_pNotifyCB)
			m_pNotifyCB(m_pUserData, VO_MMR_MSG_MEMORY_FULL, 0);

		return true;
	}

	return false;
}


bool CSourceSink::CheckErrorCode()
{
	if(m_dwErrorCode & I_VORC_REC_DISK_FULL)
		return true;
	else if(m_dwErrorCode & I_VORC_REC_MEMORY_FULL)
		return true;
	else if(m_dwErrorCode & I_VORC_REC_FW_INIT_ERROR)
		return true;
	else if(m_dwErrorCode & I_VORC_REC_TIME_EXPIRED)
		return true;
	else if(m_dwErrorCode & I_VORC_REC_SIZE_EXPIRED)
		return true;

	return false;
}

void CSourceSink::EncodeAudioVideoSamples()
{
	//single thread
#ifdef _DEBUG
	OutputDebugString(_T("Begin Encode Video&Audio Thread.\n"));
#endif
	//::SetThreadPriority(m_hEncodeVideoThread, THREAD_PRIORITY_ABOVE_NORMAL);

	bool bEncode = true;
	unsigned long dwStart=0, dwEnd=0, dwCurrAudioStartTime=0, dwCurrAudioLastTime=0;

	while(m_bRecording)
	{
#ifdef _TEST_PERFORMANCE_
		unsigned long a_enc_time_add = voOS_GetSysTime();
#endif

		voCAutoLock lock(&m_csEncodeVideoSample);

		if(CheckErrorCode())
		{
			voOS_Sleep(20);
			continue;
		}

		CSampleData* pSample = 0;

		if(m_pAVEncoder->IsRecordAudio())
		{
			pSample = m_lstFullRecAS.GetHead();

			if(pSample)
			{
				if(m_pAVEncoder)
				{

	#ifdef _TEST_PERFORMANCE_
					unsigned long a_enc_time = voOS_GetSysTime();
	#endif
					m_dwErrorCode |= m_pAVEncoder->RecvAudioSample(pSample);

					ProcessErrorCode();

	#ifdef _TEST_PERFORMANCE_
					a_enc_time = voOS_GetSysTime() - a_enc_time;
					dwAudioEncodeUseTime += a_enc_time;
					dwAudioEncodeUseTimeAdd += voOS_GetSysTime() - a_enc_time_add;

					dwAudioEncodeMaxUseTime = dwAudioEncodeMaxUseTime>a_enc_time?dwAudioEncodeMaxUseTime:a_enc_time;
					dwAudioEncodeMinUseTime = dwAudioEncodeMinUseTime>a_enc_time?a_enc_time:dwAudioEncodeMinUseTime;
	#endif
				}

				pSample->GetTime(m_dwCurrAudioStartTime, m_dwCurrAudioLastTime);
				m_dwCurrRecTime = m_dwCurrAudioLastTime;

	#ifdef _TEST_PERFORMANCE_
				dwAudioEncodedCount++;
				dwLastEncodedAudioTime = m_dwCurrAudioLastTime;
	#endif
				pSample = m_lstFullRecAS.RemoveHead();
				m_lstFreeRecAS.AddTail(pSample);

				CheckTimeLimit();
				CheckSizeLimit();
			}
			else
			{
				//voOS_Sleep(10);
			}
		}


		if(m_pAVEncoder->IsRecordVideo())
		{
			//video
	#ifdef _TEST_PERFORMANCE_
			unsigned long enc_time_add = voOS_GetSysTime();
	#endif

			bEncode = false;
			pSample = m_lstFullRecVS.GetHead();

			if(pSample)
			{
				pSample->GetTime(dwStart, dwEnd);

				if(!IsRecordAudio())
					m_dwCurrRecTime = dwEnd;

				bEncode = IsNeedEncode(dwStart, dwEnd);

				//remove 
				if(!bEncode)
				{
					ProcessVideoSyncDrop(pSample);
				}
				else
				{

	#ifdef _TEST_PERFORMANCE_
			unsigned long enc_time = voOS_GetSysTime();
	#endif
					EncodeVideoSamples(pSample);

	#ifdef _TEST_PERFORMANCE_
					enc_time = voOS_GetSysTime() - enc_time;
					dwVideoEncodeUseTime += enc_time;
					dwVideoEncodeUseTimeAdd += voOS_GetSysTime()-enc_time_add;

					dwVideoEncodeMaxUseTime = dwVideoEncodeMaxUseTime>enc_time?dwVideoEncodeMaxUseTime:enc_time;
					dwVideoEncodeMinUseTime = dwVideoEncodeMinUseTime>enc_time?enc_time:dwVideoEncodeMinUseTime;

					aEncodedFrameTimes[dwVideoEncodedCount][0] = dwStart;
					aEncodedFrameTimes[dwVideoEncodedCount][1] = enc_time;
					aEncodedFrameTimes[dwVideoEncodedCount][2] = m_lstFullRecVS.GetSampleCount();
					dwVideoEncodedCount++;
	#endif			
					CheckTimeLimit();
					CheckSizeLimit();

					pSample = m_lstFullRecVS.RemoveHead();
					m_lstFreeRecVS.AddTail(pSample);
				}	
			}
			else
			{
				voOS_Sleep(10);
			}
		}
	}

#ifdef _TEST_PERFORMANCE_
// 	TCHAR debug2[120];
// 	swprintf(debug2, _T("CSourceSink::EncodeAudioVideoSamples() -> Full buf left %d.\n"), m_lstFullRecVS.GetSampleCount());
// 	OutputDebugString(debug2);
#endif

	m_hEncodeVideoThread = NULL;

}

bool CSourceSink::CheckTimeLimit()
{
	if(m_dwErrorCode&I_VORC_REC_TIME_EXPIRED)
		return false;

	if(m_dwRecordTimeLimit!=0 && m_dwCurrRecTime>=m_dwRecordTimeLimit)
	{
		m_dwErrorCode |= I_VORC_REC_TIME_EXPIRED;
		
		if(m_pNotifyCB)
			m_pNotifyCB(m_pUserData, VO_MMR_MSG_REC_TIME_EXPIRED, 0);

		return false;
	}

	return true;
}


bool CSourceSink::CheckSizeLimit()
{
	if(m_dwErrorCode&I_VORC_REC_SIZE_EXPIRED)
		return false;

	if(0 != m_dwRecordSizeLimit)
	{
		long long total = m_pAVEncoder->GetDumpSize();

		if(total >= m_dwRecordSizeLimit)
		{
			m_dwErrorCode |= I_VORC_REC_SIZE_EXPIRED;

			if(m_pNotifyCB)
				m_pNotifyCB(m_pUserData, VO_MMR_MSG_REC_SIZE_EXPIRED, 0);

			return false;
		}
	}

	return true;
}


unsigned long CSourceSink::GetCurrRecTime()
{
	return m_dwCurrRecTime;
}


void CSourceSink::EncodeVideoSamples()
{
	//two threads
#ifdef _TEST_PERFORMANCE_
	//CPerfTest::DumpLog(_T("Begin Encode Video Thread.\n"));
#endif
	//::SetThreadPriority(m_hEncodeVideoThread, THREAD_PRIORITY_ABOVE_NORMAL);

	bool bFirst = true;
	bool bEncode = true;
	bool bColorConvertOk = false;
	unsigned long dwStart=0, dwEnd=0, dwCurrAudioStartTime=0, dwCurrAudioLastTime=0;

	while(m_bRecording)
	{
#ifdef _TEST_PERFORMANCE_
		unsigned long enc_time_add = voOS_GetSysTime();
#endif

		voCAutoLock lock(&m_csEncodeVideoSample);

		if(CheckErrorCode())
		{
			voOS_Sleep(20);
			//printf("EncodeVideoSamples continue.\n");
			continue;
		}

		bEncode = false;
		bColorConvertOk = false;
		CSampleData* pSample = m_lstFullRecVS.GetHead();

		if(pSample)
		{
// 			if(bFirst)
// 			{
// 				bFirst = false;
// 				pSample = m_lstFullRecVS.RemoveHead();
// 				delete pSample;
// 				continue;
// 			}

			pSample->GetTime(dwStart, dwEnd);
			
			bEncode = IsNeedEncode(dwStart, dwEnd);

			//remove 
			if(!bEncode)
			{
#ifdef _TEST_PERFORMANCE_
				/*TCHAR debug[120];
				swprintf(debug, _T("Drop in encode thread. %d - %d, Count:%d\n"), dwStart, dwEnd, m_lstFullRecVS.GetSampleCount());
				OutputDebugString(debug);
				*/
				aDropFrameTimes[nVideoDropCount][0] = dwStart;
				aDropFrameTimes[nVideoDropCount][1] = 1;
				aDropFrameTimes[nVideoDropCount][2] = m_lstFullRecVS.GetSampleCount();
				nVideoDropCount++;
	#endif

				//AddDropFrm(dwStart, dwEnd);
				m_dwSyncVideoDropTime = dwEnd;
				pSample = m_lstFullRecVS.RemoveHead();
				m_lstFreeRecVS.AddTail(pSample);
			}
			else
			{

#ifdef _TEST_PERFORMANCE_
			unsigned long enc_time = voOS_GetSysTime();
#endif
				EncodeVideoSamples(pSample);


#ifdef _TEST_PERFORMANCE_
				enc_time = voOS_GetSysTime() - enc_time;
				dwVideoEncodeUseTime += enc_time;
				dwVideoEncodeUseTimeAdd += voOS_GetSysTime()-enc_time_add;

				dwVideoEncodeMaxUseTime = dwVideoEncodeMaxUseTime>enc_time?dwVideoEncodeMaxUseTime:enc_time;
				dwVideoEncodeMinUseTime = dwVideoEncodeMinUseTime>enc_time?enc_time:dwVideoEncodeMinUseTime;

				aEncodedFrameTimes[dwVideoEncodedCount][0] = dwStart;
				aEncodedFrameTimes[dwVideoEncodedCount][1] = enc_time;
				aEncodedFrameTimes[dwVideoEncodedCount][2] = m_lstFullRecVS.GetSampleCount();

				dwVideoEncodedCount++;
#endif			
				CheckTimeLimit();
				CheckSizeLimit();

				pSample = m_lstFullRecVS.RemoveHead();
				m_lstFreeRecVS.AddTail(pSample);
			}
		}
		else
		{
			voOS_Sleep(100);
		}
	}

#ifdef _TEST_PERFORMANCE_
	unsigned long count = m_lstFullRecVS.GetSampleCount();
	char debug2[120];
	sprintf(debug2, "CSourceSink::EncodeVideoSamples() -> Full buf left %d.\r\n", count);
	CPerfTest::DumpLog(debug2);
#endif

	m_hEncodeVideoThread = NULL;
}


void CSourceSink::ProcessVideoSyncDrop(CSampleData* pSample)
{
	unsigned long dwStart, dwEnd;
	pSample->GetTime(dwStart, dwEnd);


#ifdef _TEST_PERFORMANCE_
	/*TCHAR debug[120];
	swprintf(debug, _T("ProcessVideoSyncDrop: Drop in encode thread. %d - %d, Count:%d\n"), dwStart, dwEnd, m_lstFullRecVS.GetSampleCount());
	OutputDebugString(debug);*/
	aDropFrameTimes[nVideoDropCount][0] = dwStart;
	aDropFrameTimes[nVideoDropCount][1] = 1;
	aDropFrameTimes[nVideoDropCount][2] = m_lstFullRecVS.GetSampleCount();
	nVideoDropCount++;
#endif

	//dwEnd is error
	m_dwSyncVideoDropTime = dwEnd;
	pSample = m_lstFullRecVS.RemoveHead();
	m_lstFreeRecVS.AddTail(pSample);
}

void CSourceSink::ProcessVideoFullDrop(CSampleData* pSample)
{

}

void CSourceSink::EncodeAudioSamples()
{
#ifdef _TEST_PERFORMANCE_
	//OutputDebugString(_T("Begin Encode Audio Thread.\n"));
#endif
	//::SetThreadPriority(m_hEncodeAudioThread, THREAD_PRIORITY_ABOVE_NORMAL);

	VO_U32 dwStart, dwEnd;

	while(m_bRecording)
	{
		if(CheckErrorCode())
		{
			voOS_Sleep(20);
			continue;
		}

#ifdef _TEST_PERFORMANCE_
		unsigned long enc_time_add = voOS_GetSysTime();
#endif
		// 20090630
		//voCAutoLock lock(&m_csEncodeAudioSample);

		CSampleData* pSample = m_lstFullRecAS.GetHead();

		if(pSample)
		{
			if(m_pAVEncoder)
			{

#ifdef _TEST_PERFORMANCE_
				unsigned long enc_time = voOS_GetSysTime();
#endif
				m_dwErrorCode |= m_pAVEncoder->RecvAudioSample(pSample);

#ifdef _TEST_PERFORMANCE_
				enc_time = voOS_GetSysTime() - enc_time;
				dwAudioEncodeUseTime += enc_time;
				dwAudioEncodeUseTimeAdd += voOS_GetSysTime() - enc_time_add;

				dwAudioEncodeMaxUseTime = dwAudioEncodeMaxUseTime>enc_time?dwAudioEncodeMaxUseTime:enc_time;
				dwAudioEncodeMinUseTime = dwAudioEncodeMinUseTime>enc_time?enc_time:dwAudioEncodeMinUseTime;
#endif
			}

			pSample->GetTime(m_dwCurrAudioStartTime, m_dwCurrAudioLastTime);
			m_dwCurrRecTime = m_dwCurrAudioLastTime;
			CheckTimeLimit();
			CheckSizeLimit();

#ifdef _TEST_PERFORMANCE_
			dwAudioEncodedCount++;
			dwLastEncodedAudioTime = m_dwCurrAudioLastTime;
#endif
			pSample = m_lstFullRecAS.RemoveHead();
			m_lstFreeRecAS.AddTail(pSample);
		}
		else
		{
#ifdef _TEST_PERFORMANCE_
			VO_U32 dwUseTime = voOS_GetSysTime();
#endif
			//20090630
			// voOS_Sleep(200);
			VO_U32 dwWaitResult = m_EventEncodeAudio.Wait(1000);
			if(dwWaitResult == VOVO_SEM_OK)
			{
				m_EventEncodeAudio.Reset();

#ifdef _TEST_PERFORMANCE_
				dwUseTime = voOS_GetSysTime() - dwUseTime;
				char log[128];
				sprintf(log, ("[Rec] wait audio data, use time = %d\r\n"), dwUseTime);
				CPerfTest::DumpLog(log);
#endif

			}
			else
			{
#ifdef _TEST_PERFORMANCE_
				dwUseTime = voOS_GetSysTime() - dwUseTime;
				char log[128];
				sprintf(log, ("[Rec] wait audio data time out, use time = %d\r\n"), dwUseTime);
				CPerfTest::DumpLog(log);
#endif
			}
		}
	}

	m_hEncodeAudioThread = NULL;
}


void CSourceSink::EncodeLeft()
{
#ifdef _TEST_PERFORMANCE_
	//OutputDebugString(_T("Enter Encode left.\n"));
#endif
	
	unsigned long dwStart=0,dwEnd=0, dwFirstStart=0;
	CSampleData* pSample = m_lstFullRecAS.GetHead();

	//first encode audio left
	while(pSample)
	{
		if(pSample)
		{
			pSample->GetTime(m_dwCurrAudioStartTime, m_dwCurrAudioLastTime);

			if(dwFirstStart == 0)
				dwFirstStart = m_dwCurrAudioLastTime;

			if(m_dwErrorCode & I_VORC_REC_TIME_EXPIRED)
				if(m_dwRecordTimeLimit <= m_dwCurrAudioStartTime)
					break;
			
			if(m_pAVEncoder)
			{
#ifdef _TEST_PERFORMANCE_
				int enc_time = voOS_GetSysTime();
#endif
				m_dwErrorCode |= m_pAVEncoder->RecvAudioSample(pSample);

#ifdef _TEST_PERFORMANCE_
				dwAudioEncodedCount++;
				dwLastEncodedAudioTime = m_dwCurrAudioLastTime;
#endif
			}

			m_dwCurrRecTime = m_dwCurrAudioLastTime;
			pSample = m_lstFullRecAS.RemoveHead();
			m_lstFreeRecAS.AddTail(pSample);
		}
		
		pSample = m_lstFullRecAS.GetHead();
	}


	//encode video left
	int nCount = 0;
	pSample = m_lstFullRecVS.GetHead();
	while(pSample)
	{
		pSample->GetTime(dwStart, dwEnd);

		if(m_dwErrorCode & I_VORC_REC_TIME_EXPIRED)
			if(m_dwRecordTimeLimit <= dwStart)
				break;

		if(m_pAVEncoder->IsRecordAudio())
		{
			if(m_dwCurrAudioLastTime < dwStart)
			{
#ifdef _DEBUG
				TCHAR debug2[56];
				swprintf(debug2, _T("Encode left video buf braak.Encoded %d frame.\n"), nCount);
				OutputDebugString(debug2);
#endif
				break;
			}
		}

		nCount++;
		EncodeVideoSamples(pSample);
		m_dwCurrRecTime = dwEnd;

		pSample = m_lstFullRecVS.RemoveHead();
		m_lstFreeRecVS.AddTail(pSample);

		pSample = m_lstFullRecVS.GetHead();
	}
}


bool CSourceSink::EncodeAudioSamples(unsigned long dwTime)
{
	if(m_lstFullRecAS.GetDuration () < dwTime)
		return false;

	voCAutoLock lock (&m_csAudioSample);

	VO_U32 dwStart, dwEnd;
	CSampleData * pFirstSample = m_lstFullRecAS.GetHead();
	pFirstSample->GetTime(dwStart, dwEnd);
	VO_U32 dwFirstStart = dwStart;

	int nSampleCount = 0;

	do{
		nSampleCount++;

		if(m_pAVEncoder)
			m_dwErrorCode != m_pAVEncoder->RecvAudioSample(pFirstSample);

		pFirstSample->GetTime(m_dwCurrAudioStartTime, m_dwCurrAudioLastTime);
		
		pFirstSample = pFirstSample->GetNext();

		if(pFirstSample == 0)
			break;

		pFirstSample->GetTime(dwStart, dwEnd);

	} while(dwEnd - dwFirstStart <= dwTime);


#ifdef _TEST_PERFORMANCE_
	dwAudioEncodedCount += nSampleCount;

/*	if(dwAudioEncodedCount > 10)
	{
		testTime = voOS_GetSysTime() - testTime;
		TCHAR debug[128];
		memset(&debug, 0, 128);
		swprintf(debug, _T("Encode audio samples count = %u, use time=%u\n"), dwAudioEncodedCount, testTime);
		OutputDebugString(debug);
	}
	*/
#endif

	CSampleData * pFreeSample = NULL;
	
	while(nSampleCount > 0)
	{
		pFreeSample = m_lstFullRecAS.RemoveHead();
		m_lstFreeRecAS.AddTail(pFreeSample);
		nSampleCount--;
	}

#ifdef _TEST_PERFORMANCE_
	if(pFreeSample)
		pFreeSample->GetTime(dwStart, dwLastEncodedAudioTime);
#endif

	return true;
}

bool CSourceSink::GetVideoSrcFrameRate(float* pFrameRate)
{	
	if(0 == m_dwTotalVideoFrameCount || m_dwTotalPreviewTime==0)
	{
		*pFrameRate = 0.0;
		return true;
	}

	*pFrameRate = (float)m_dwTotalVideoFrameCount*1000.0/(float)m_dwTotalPreviewTime;

	return true;
}

bool CSourceSink::AdjustAudioTimestamp(VO_MMR_ASBUFFER* pBuffer)
{
	if(pBuffer->disconnect == 1)
		m_dwAudioPauseTime += (pBuffer->start_time - m_dwAudioLastFrameStartTime);

	if(m_bAdjustTimestamp)
	{
		if(m_nAudioSampleDuration == 0)
		{
			int nOneMsDataLen = 16;
			m_nAudioSampleDuration = pBuffer->length / nOneMsDataLen;
		}

		m_nAudioTimeOffset += (m_nAudioSampleDuration - (pBuffer->end_time - pBuffer->start_time));

		if(m_dwFirstAVFrameStartTime == 0)
			m_dwFirstAVFrameStartTime	= pBuffer->start_time;

		if(m_dwAudioLastFrameStartTime == 0)
			m_dwAudioLastFrameStartTime = pBuffer->start_time;

		pBuffer->start_time = m_dwAudioLastFrameStartTime;
		pBuffer->end_time	= pBuffer->start_time + m_nAudioSampleDuration;
		m_dwTotalAudioRawDataLen += pBuffer->length;
		m_dwAudioLastFrameStartTime	= pBuffer->end_time; 
		return true;

		// first frame
		if(m_dwFirstAVFrameStartTime == 0)
			m_dwFirstAVFrameStartTime	= pBuffer->start_time;

		if(m_dwAudioLastFrameStartTime == 0)
			m_dwAudioLastFrameStartTime = m_dwFirstAVFrameStartTime;

		int nOneMsDataLen = 16;
		int nDuration = pBuffer->length / nOneMsDataLen;

		pBuffer->start_time = m_dwAudioLastFrameStartTime + nDuration - m_dwAudioPauseTime;
		pBuffer->end_time	= pBuffer->start_time + nDuration;
		m_dwTotalAudioRawDataLen += pBuffer->length;
		m_dwAudioLastFrameStartTime	= pBuffer->end_time; 
	}
	else
	{
		pBuffer->start_time -= m_nTotalPauseTime;
		pBuffer->end_time	-= m_nTotalPauseTime;
	}


// 	if(pBuffer->disconnect == 1)
// 		m_dwAudioPauseTime += (pBuffer->start_time - m_dwAudioLastFrameStartTime);
// 
// 	if(m_bAdjustTimestamp)
// 	{
// 		pBuffer->start_time -= m_dwAudioPauseTime;
// 		pBuffer->end_time	-= m_dwAudioPauseTime;
// 		m_dwAudioLastFrameStartTime += pBuffer->end_time;
// 		return;
// 
// 		// first frame
// 		if(m_dwFirstAVFrameStartTime == 0)
// 			m_dwFirstAVFrameStartTime	= pBuffer->start_time;
// 
// 		if(m_dwAudioLastFrameStartTime == 0)
// 			m_dwAudioLastFrameStartTime = m_dwFirstAVFrameStartTime;
// 
// 		int nOneMsDataLen = 16;
// 		int nDuration = pBuffer->length / nOneMsDataLen;
// 		
// 		pBuffer->start_time = m_dwAudioLastFrameStartTime + nDuration - m_dwAudioPauseTime;
// 		pBuffer->end_time	= pBuffer->start_time + nDuration;
//  		m_dwTotalAudioRawDataLen += pBuffer->length;
// 		m_dwAudioLastFrameStartTime	= pBuffer->end_time; 
// 	}
// 	else
// 	{
// 		pBuffer->start_time -= m_nTotalPauseTime;
// 		pBuffer->end_time	-= m_nTotalPauseTime;
// 	}





// 	if(m_bAdjustTimestamp)
// 	{
// 		pBuffer->start_time = m_dwAudioLastTime - m_nTotalPauseTime;
// 
// 		m_dwTotalAudioRawDataLen += pBuffer->length;
// 		m_dwAudioLastTime += ((pBuffer->length*8*1000)/(m_AudioFmt.fmt.SampleRate*m_AudioFmt.fmt.SampleBits*m_AudioFmt.fmt.Channels));
// 		pBuffer->end_time = m_dwAudioLastTime - m_nTotalPauseTime;
// 	}
// 	else
// 	{
// 		pBuffer->start_time -= m_nTotalPauseTime;
// 		pBuffer->end_time	-= m_nTotalPauseTime;
// 	}

	return true;
}


bool CSourceSink::AdjustVideoTimestamp(VO_MMR_VSBUFFER* pBuffer)
{
	if(pBuffer->disconnect == 1)
		m_dwVideoPauseTime += (pBuffer->start_time - m_dwVideoLastFrameStartTime);

	if(m_bAdjustTimestamp)
	{
		int start	= pBuffer->start_time + m_nAudioTimeOffset - m_dwVideoPauseTime;
		int end		= pBuffer->end_time + m_nAudioTimeOffset - m_dwVideoPauseTime;

		if(start <= m_dwVideoLastFrameStartTime)
		{
			pBuffer->start_time = m_dwVideoLastFrameStartTime + 20;
			pBuffer->end_time	= pBuffer->start_time + 20;
			return true;
		}
		else
		{
			pBuffer->start_time = start;
			pBuffer->end_time	= end;
		}

		m_dwVideoLastFrameStartTime = pBuffer->start_time;
		return true;

		if(m_dwFirstAVFrameStartTime == 0)
		{
			m_dwFirstAVFrameStartTime	= pBuffer->start_time;
			m_dwAudioLastFrameStartTime	= pBuffer->start_time;
		}

		VO_U32 dwEndTime = voOS_GetSysTime () - m_dwStartTimeForRecord - m_dwVideoPauseTime;
		pBuffer->start_time = m_dwLastTimeForRecord;
		pBuffer->end_time = dwEndTime;
		m_dwLastTimeForRecord = dwEndTime;
	}
	else
	{
		// only adjust pause time
		pBuffer->start_time -= m_nTotalPauseTime;
		pBuffer->end_time	-= m_nTotalPauseTime;
	}



// 	if(pBuffer->disconnect == 1)
// 		m_dwVideoPauseTime += (pBuffer->start_time - m_dwVideoLastFrameStartTime);
// 
// 	if(m_bAdjustTimestamp)
// 	{
// 		pBuffer->start_time -= m_dwVideoPauseTime;
// 		pBuffer->end_time	-= m_dwVideoPauseTime;
// 		m_dwVideoLastFrameStartTime = pBuffer->start_time;
// 		return;
// 
// 		if(m_dwFirstAVFrameStartTime == 0)
// 		{
// 			m_dwFirstAVFrameStartTime	= pBuffer->start_time;
// 			m_dwAudioLastFrameStartTime	= pBuffer->start_time;
// 		}
// 
// 		VO_U32 dwEndTime = voOS_GetSysTime () - m_dwStartTimeForRecord - m_dwVideoPauseTime;
// 		pBuffer->start_time = m_dwLastTimeForRecord;
// 		pBuffer->end_time = dwEndTime;
// 		m_dwLastTimeForRecord = dwEndTime;
// 	}
// 	else
// 	{
// 		// only adjust pause time
// 		pBuffer->start_time -= m_nTotalPauseTime;
// 		pBuffer->end_time	-= m_nTotalPauseTime;
// 	}




// 	if(m_bAdjustTimestamp)
// 	{
// 		VO_U32 dwEndTime = voOS_GetSysTime () - m_dwStartTimeForRecord - m_nTotalPauseTime;
// 		pBuffer->start_time = m_dwLastTimeForRecord;
// 		pBuffer->end_time = dwEndTime;
// 		m_dwLastTimeForRecord = dwEndTime;
// 	}
// 	else
// 	{
// 		// only adjust pause time
// 		pBuffer->start_time -= m_nTotalPauseTime;
// 		pBuffer->end_time	-= m_nTotalPauseTime;
// 	}
}

bool CSourceSink::RecvAudioSource(VO_MMR_ASBUFFER* pBuffer)
{
	if(m_bResetting)
		return false;

#ifdef _TEST_PERFORMANCE_
	aRecvAudioTimes[dwASRecCount][0] = pBuffer->start_time;
	aRecvAudioTimes[dwASRecCount][1] = pBuffer->end_time;
#endif

	AdjustAudioTimestamp(pBuffer);

#ifdef _TEST_PERFORMANCE_
	dwASRecCount++;
	dwLastRecvAudioTime = pBuffer->end_time;
#endif

	CSampleData* pSample = m_lstFreeRecAS.RemoveHead();

	if (pSample == NULL)
	{
		if(m_lstFullRecAS.GetSampleCount () < 50)
		{
			pSample = new CSampleData();

			if(NULL == pSample)
				return false;
		}
		else
		{		
			pSample = m_lstFullRecAS.RemoveHead();
#ifdef _TEST_PERFORMANCE_
			// added by gtxia 2008-1-10
			printf("delete a SampleData from m_lstFullRecAS\n");	
			nAudioDropCount++;
			//OutputDebugString(_T("+++++++++Audio drop frame.+++++++++\n"));
#endif
		}
	}

	if(pSample == NULL)
	{
		return false;
	}
		
	pSample->SetTime(pBuffer->start_time, pBuffer->end_time);
	
	int nBufLen = pBuffer->length;
	if(!pSample->AllocBuf(nBufLen))
	{
		delete pSample;
		pSample = NULL;		
		return false;
	}

	VO_PBYTE pOutBuf = pSample->GetDataPoint();
	memcpy(pOutBuf, pBuffer->buffer, nBufLen);
	pSample->SetDataLen(nBufLen);

	m_lstFullRecAS.AddTail(pSample);
	
	//SetEvent(m_hEventEncodeAudio);
	m_EventEncodeAudio.Signal();

	return true;
}


bool CSourceSink::RecvRecordVideoSource(VO_MMR_VSBUFFER* pBuffer)
{
	AdjustVideoTimestamp(pBuffer);

	m_dwTotalPreviewTime = pBuffer->end_time;

	if(!m_bHasPreviewVS)
		m_dwTotalVideoFrameCount++;

#ifdef _TEST_PERFORMANCE_
		aRecvFrameTimes[dwVSRecCount][0] = pBuffer->start_time;
		aRecvFrameTimes[dwVSRecCount][1] = pBuffer->end_time;
		dwVSRecCount++;
#endif

	if(m_bResetting)
	{
		voOS_Sleep(50);
		return false;
	}

	if(!m_bRecording)
		return true;
	
	voCAutoLock lock(&m_csVideoSample);
	CSampleData* pSample = m_lstFreeRecVS.RemoveHead();

	if (pSample == 0)
	{
		if(m_lstFullRecVS.GetSampleCount() < m_nVSBufCount)
		{
			pSample = new CSampleData();
		}
		else
		{		
			if(m_bRecording && m_pAVEncoder->IsRecordVideo())
			{
				unsigned long dwStart, dwEnd;
				pSample = m_lstFullRecVS.GetHead();

				while(pSample)
				{
					pSample->GetTime(dwStart, dwEnd);

					if(!IsDropFrmNeighbor(dwStart, dwEnd))
					{
						break;
					}

					pSample = pSample->GetNext();
				}

				if(pSample)
				{
					m_lstFullRecVS.Remove(pSample);
					m_dwBufVideoDropTime = dwEnd;
	
#ifdef _TEST_PERFORMANCE_
					aDropFrameTimes[nVideoDropCount][2] = m_lstFullRecVS.GetSampleCount();
					aDropFrameTimes[nVideoDropCount][0] = dwStart;
					aDropFrameTimes[nVideoDropCount][1] = 0;
					nVideoDropCount++;
#endif

				}
					

			}
			else//not recording
			{
				pSample = m_lstFullRecVS.RemoveHead();
			}
		}
	}

	if(pSample == 0)
	{
		// note : here drop anyway!!!!

#ifdef _TEST_PERFORMANCE_
		dwTmp++;
		aDropFrameTimes[nVideoDropCount][2] = m_lstFullRecVS.GetSampleCount();
		aDropFrameTimes[nVideoDropCount][0] = pBuffer->start_time;
		aDropFrameTimes[nVideoDropCount][1] = 2;
		nVideoDropCount++;
#endif

		voOS_Sleep(50);
		return false;
	}
	
	pSample->SetTime(pBuffer->start_time, pBuffer->end_time);

// 	int nBufLen = pBuffer->length[0]+pBuffer->length[1]+pBuffer->length[2];
	int nBufLen = pBuffer->buffer.Stride[0]*m_RecordVideoFormat.height + pBuffer->buffer.Stride[1]*m_RecordVideoFormat.height + pBuffer->buffer.Stride[2]*m_RecordVideoFormat.height;
	
	if(!pSample->AllocBuf(nBufLen))
	{
		delete pSample;
		pSample = 0;

		unsigned long dwStart, dwEnd;
		pSample = m_lstFullRecVS.GetHead();

		while(pSample)
		{
			pSample->GetTime(dwStart, dwEnd);

			if(!IsDropFrmNeighbor(dwStart, dwEnd))
			{
				break;
			}

			pSample = pSample->GetNext();
		}


		m_lstFullRecVS.Remove(pSample);
		if(!pSample)
			return false;
		m_dwBufVideoDropTime = dwEnd;
		pSample->SetTime(pBuffer->start_time, pBuffer->end_time);
#ifdef _TEST_PERFORMANCE_
		aDropFrameTimes[nVideoDropCount][2] = m_lstFullRecVS.GetSampleCount();
		aDropFrameTimes[nVideoDropCount][0] = dwStart;
		aDropFrameTimes[nVideoDropCount][1] = 3;
		nVideoDropCount++;
#endif

		voOS_Sleep(100);
	}

	// tag: has issue 20090624
	VO_PBYTE pOutBuf = pSample->GetDataPoint();
	memcpy(pOutBuf, pBuffer->buffer.Buffer[0], nBufLen);
	pSample->SetDataLen(nBufLen);

	return m_lstFullRecVS.AddTail(pSample);
}

void CSourceSink::SetRecordVideoSrcFormat(VO_MMR_VSFORMAT* pFmt)
{
	m_RecordVideoFormat.height = pFmt->height;
	m_RecordVideoFormat.width = pFmt->width;
	m_RecordVideoFormat.video_type = pFmt->video_type;
	m_RecordVideoFormat.rotation_type = pFmt->rotation_type;
}

void CSourceSink::SetAudioFmt(VO_MMR_ASFORMAT* pFmt)
{
	m_AudioFmt.fmt.Channels		= pFmt->fmt.Channels;
	m_AudioFmt.fmt.SampleBits	= pFmt->fmt.SampleBits;
	m_AudioFmt.fmt.SampleRate	= pFmt->fmt.SampleRate;
}

void CSourceSink::SetRecTimeLimit(VO_U32 dwTimeLimit)
{
	m_dwRecordTimeLimit = dwTimeLimit;
}

unsigned long CSourceSink::GetRecTimeLimit()
{
	return m_dwRecordTimeLimit;
}

void CSourceSink::SetRecSizeLimit(VO_U32 dwSizeLimit)
{
	m_dwRecordSizeLimit = dwSizeLimit;
}

VO_U32 CSourceSink::GetRecSizeLimit()
{
	return m_dwRecordSizeLimit;
}

void CSourceSink::SetAVEncoder(CAVEncoder* pEncoder)
{
	if(m_pAVEncoder)
		delete m_pAVEncoder;

	m_pAVEncoder = pEncoder;
}

void CSourceSink::SetEncoderCC(CColorConvert* pCC)
{
	if(m_pCCForEncoder)
		delete m_pCCForEncoder;

	m_pCCForEncoder = pCC;
}


void CSourceSink::SetNotifyProc(VOMMRecordCallBack pProc)
{
	m_pNotifyCB = pProc;
}

void CSourceSink::SetContentType(int nType)
{
	m_pAVEncoder->SetContentType(nType);
}

int	CSourceSink::GetContentType()
{
	return m_pAVEncoder->GetContentType();
}

bool CSourceSink::IsRecordAudio()
{
	return m_pAVEncoder->IsRecordAudio();
}

bool CSourceSink::IsRecordVideo()
{
	return m_pAVEncoder->IsRecordVideo();
}

void CSourceSink::SetImageQuality(int nQuality)
{
	m_pAVEncoder->SetImageQuality(nQuality);
}

void CSourceSink::SetVideoQuality(int nQuality)
{
	m_pAVEncoder->SetEncodeQuality(nQuality);
}

bool CSourceSink::IsDropFrmNeighbor(unsigned long& dwStart, unsigned long& dwEnd)
{
	voCAutoLock lock(&m_csFindVideoDropSample);

#ifdef _TEST_PERFORMANCE_
	VO_U32 dwUseTime = voOS_GetSysTime();
#endif

	bool bRet = false;
	int n = 0;
	for(; n<DROP_FRM_UPDATE_COUNT; n++)
	{
		if(m_arDropTimes[n] == dwStart)
		{
			bRet = true;
			break;
		}

		if(m_arDropTimes[n] == dwEnd)
		{
			bRet = true;
			break;
		}
	}

#ifdef _TEST_PERFORMANCE_
	dwUseTime = voOS_GetSysTime() - dwUseTime;
	char log[256];
	sprintf(log, ("check drop use time = %03d, check count = %03d, full buffer size=%03d, drop frame index=%03d\r\n"), dwUseTime, n, m_lstFullRecVS.GetSampleCount(), m_nDropFrmUpdateInx);
	CPerfTest::DumpLog(log);
#endif

	if(bRet)
		return true;

	m_arDropTimes[m_nDropFrmUpdateInx] = dwStart;

	m_nDropFrmUpdateInx++;
	if(m_nDropFrmUpdateInx >= DROP_FRM_UPDATE_COUNT)
		m_nDropFrmUpdateInx = 0;

	m_arDropTimes[m_nDropFrmUpdateInx] = dwEnd;

	m_nDropFrmUpdateInx++;
	if(m_nDropFrmUpdateInx >= DROP_FRM_UPDATE_COUNT)
		m_nDropFrmUpdateInx = 0;

#ifdef _DUMP_LOG_
// 	char debug[256];
// 	sprintf(debug, "[Rec] drop frame:start time = %08d, end time = %08d\r\n", dwStart, dwEnd);
// 	CPerfTest::DumpLog(debug);
#endif

	return false;
}

void CSourceSink::AddDropFrm(unsigned long& dwStart, unsigned long& dwEnd)
{
	m_arDropTimes[m_nDropFrmUpdateInx] = dwStart;

	m_nDropFrmUpdateInx++;
	if(m_nDropFrmUpdateInx >= DROP_FRM_UPDATE_COUNT)
		m_nDropFrmUpdateInx = 0;

	m_arDropTimes[m_nDropFrmUpdateInx] = dwEnd;

	m_nDropFrmUpdateInx++;
	if(m_nDropFrmUpdateInx >= DROP_FRM_UPDATE_COUNT)
		m_nDropFrmUpdateInx = 0;

}

unsigned long CSourceSink::GetEncodedDataSize()
{
	return m_pAVEncoder->GetDumpSize();
}

