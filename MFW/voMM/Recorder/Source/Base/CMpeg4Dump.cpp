#include "CMpeg4Dump.h"
#include "vcamerabase.h"
#include "cmnFile.h"
#include "voOSFunc.h"



//#define _Write_Amr_File

#ifdef _Write_Amr_File
FILE* amr = NULL;
#endif


typedef VO_S32 (VO_API * MP4WriterAPIEntry) (VO_SINK_WRITEAPI* pReadHandle, VO_U32 uFlag);


CMpeg4Dump::CMpeg4Dump(void)
{
#ifdef _WIN32
	vostrcpy(m_szDllFile, _T("voMP4FW.dll"));
#elif defined LINUX
	vostrcpy(m_szDllFile, _T("voMP4FW.so"));
#endif

	_tcscpy(m_szAPIName, _T("voGetMP4WriterAPI"));

	m_FileOp.Close	= cmnFileClose;
	m_FileOp.Flush	= cmnFileFlush;
	m_FileOp.Open	= cmnFileOpen;
	m_FileOp.Read	= cmnFileRead;
	m_FileOp.Save	= cmnFileSave;
	m_FileOp.Seek	= cmnFileSeek;
	m_FileOp.Size	= cmnFileSize;
	m_FileOp.Write	= cmnFileWrite;



#ifdef _Write_Amr_File
#ifndef _WIN32_WCE
	amr = fopen("e:\\dump\\test.amr", "wb");
#else
	amr = fopen("\\Storage Card\\test.amr", "wb");
#endif
	int n = fwrite("#!AMR\n", 1, strlen("#!AMR\n"), amr);
#endif
}

CMpeg4Dump::~CMpeg4Dump(void)
{

}

bool CMpeg4Dump::doInit(void)
{
	if (!LoadLib(NULL))
		return false;

	CPerfTest::DumpLog("[REC] load mepeg4 file writer module ok\r\n");


	((MP4WriterAPIEntry)m_pAPIEntry)(&m_Mpeg4FWFunc, 0);

	m_OpenParam.nFlag			= 1;
	m_OpenParam.pSinkOP			= (VO_PTR)&m_FileOp;
	m_OpenParam.pMemOP			= NULL;
	m_OpenParam.nAudioCoding	= VO_AUDIO_CodingAMRNB;
	m_OpenParam.nVideoCoding	= VO_VIDEO_CodingMPEG4;

	m_Mpeg4FWFunc.Open(&m_hDump, m_szName, &m_OpenParam);

	if(!m_hDump)
		return false;

	return true;
}

bool CMpeg4Dump::doStart(void)
{
	return true;
}

bool CMpeg4Dump::doStop (void)
{
#ifdef _TEST_PERFORMANCE_
	TCHAR debug[100];
	VO_U32 dwStart = voOS_GetSysTime();
#endif

	CloseDumpFile();
	
#ifdef _TEST_PERFORMANCE_
// 	_stprintf(debug, _T("Close Dump File use time = %d.\n"), GetTickCount()-dwStart);
// 	OutputDebugString(debug);
#endif

	return true;
}

long long CMpeg4Dump::CreateVideoTrack (int nWidth, int nHeight)
{
	VO_VIDEO_FORMAT vf ;
	vf.Height = nWidth;
	vf.Width = nHeight;

	m_Mpeg4FWFunc.SetParam(m_hDump , VO_PID_VIDEO_FORMAT , &vf);

	return I_VORC_REC_NO_ERRORS;
}

bool CMpeg4Dump::SetHeaderInfo(unsigned char* pInfo, int nInfoLen)
{

	return true;
}

long long CMpeg4Dump::CreateAudioTrack (int nSampleRate, int nChannels, int nBits, int nCodec)
{
	VO_AUDIO_FORMAT af;
	af.Channels = nChannels;
	af.SampleBits = nBits;
	af.SampleRate = nSampleRate;

	m_Mpeg4FWFunc.SetParam(m_hDump , VO_PID_AUDIO_FORMAT , &af);

	return I_VORC_REC_NO_ERRORS;
}

bool CMpeg4Dump::CloseDumpFile()
{
	m_Mpeg4FWFunc.Close(m_hDump);

	return true;
}

int CMpeg4Dump::DumpVideoSample(CSampleData* pSample)
{
	VO_U32 dwStart, dwEnd;
	pSample->GetTime(dwStart, dwEnd);

	VO_SINK_SAMPLE sinkSample;
	sinkSample.Buffer	= pSample->GetDataPoint();
	sinkSample.Time		= dwStart;
	sinkSample.nAV		= 1;
	sinkSample.Size		= pSample->GetDataLen();

	//printf("send video data: start = %08d\n", sinkSample.Time);


	m_Mpeg4FWFunc.AddSample(m_hDump, &sinkSample);


	return I_VORC_REC_NO_ERRORS;
}

int CMpeg4Dump::DumpAudioSample(CSampleData* pSample)
{
	VO_U32 dwStart, dwEnd;
	pSample->GetTime(dwStart, dwEnd);

	VO_SINK_SAMPLE sinkSample;
	sinkSample.Buffer	= pSample->GetDataPoint();
	sinkSample.Time		= dwStart;
	sinkSample.nAV		= 0;
	sinkSample.Size		= pSample->GetDataLen();

	m_Mpeg4FWFunc.AddSample(m_hDump, &sinkSample);


	return I_VORC_REC_NO_ERRORS;
}

TCHAR* CMpeg4Dump::GetFileExtName()
{
	return _T(".mp4");
}



