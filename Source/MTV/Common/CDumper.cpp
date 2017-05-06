#include "CDumper.h"
#include "voOSFunc.h"
#include "fMacros.h"
#include "stdarg.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef _DUMP_STREAM_
VO_PTR			CDumper::m_hRawDataFile = VO_NULL;
#endif

#ifdef _DUMP_VIDEO_
VO_PTR			CDumper::m_hVideoFile = VO_NULL;
#endif

#ifdef _DUMP_AUDIO_
VO_PTR			CDumper::m_hAudioFile = VO_NULL;
#endif

#if defined( _WRITE_LOG_ ) || defined( _PRINT_LOG_)
VO_PTR			CDumper::m_hLogFile = VO_NULL;
unsigned long	CDumper::m_dwLogCount = 0;
bool			CDumper::m_bClosLogFile = false;
unsigned long	g_dwDumpID = 0;
#endif

#if defined( _LOG_VIDEO_OUTPUT_ ) || defined( _LOG_AUDIO_OUTPUT_)
VO_U64			CDumper::m_llStartTime = 0;
#endif

#ifdef _LOG_VIDEO_OUTPUT_
VO_PTR			CDumper::m_hVideoLogFile	= VO_NULL;
VO_U32			CDumper::m_dwVideoIdx		= 0;
#endif

#ifdef _LOG_AUDIO_OUTPUT_
VO_PTR			CDumper::m_hAudioLogFile	= VO_NULL;
VO_U32			CDumper::m_dwAudioIdx		= 0;
#endif

CDumper::CDumper(void)
{
#if defined( _WRITE_LOG_ ) || defined( _PRINT_LOG_)
	g_dwDumpID = voOS_GetSysTime();
#endif

	//CDumper::IncreaseFilesize(_T("E:\\Media\\Apple.HTTP.Live.Streaming\\media_162.ts"), _T("E:\\Media\\Apple.HTTP.Live.Streaming\\media_162.1.ts"), 30);
	// 	CDumper::MergeFile(_T("E:\\Media\\Apple.HTTP.Live.Streaming\\fileSequence00.ts"),
	// 						_T("E:\\Media\\Apple.HTTP.Live.Streaming\\fileSequence0.ts"),
	// 						_T("E:\\Media\\Apple.HTTP.Live.Streaming\\00_0_merge.ts"));

}

CDumper::~CDumper(void)
{
}

void CDumper::DumpRawData(unsigned char* pData, int nLen)
{
#ifdef _DUMP_STREAM_

	if (!m_hRawDataFile)
	{
		VO_FILE_SOURCE		fileSource;
		fileSource.nFlag	= VO_FILE_TYPE_NAME;
		fileSource.nOffset	= 0;
		fileSource.nLength	= 0xFFFFFFFF;
		fileSource.nMode	= VO_FILE_WRITE_ONLY;

#ifdef _WIN32_WCE
		fileSource.pSource = (VO_PTR)_T("\\ResidentFlash\\dumper_stream_file.ts");
#elif defined _WIN32
		fileSource.pSource = (VO_PTR)_T("d:\\dumper_stream_file.ts");
#elif defined _IOS
		TCHAR szTmp[1024];
		memset(szTmp, 0, 1024*sizeof(TCHAR));
		voOS_GetAppFolder(szTmp, 1024);
		vostrcat(szTmp, _T("/"));
		vostrcat(szTmp, _T("dumper_stream_file.ts"));
		fileSource.pSource = szTmp;
#else
		fileSource.pSource = (VO_PTR)_T("/sdcard/dumper_stream_file.ts");
#endif	
		m_hRawDataFile = cmnFileOpen(&fileSource);	
	}


	if(!m_hRawDataFile)
		return;

	cmnFileWrite(m_hRawDataFile, pData, nLen);
	//cmnFileFlush(m_hRawDataFile);

#endif
}

void CDumper::DumpAudioData(unsigned char* pData, int nLen)
{
#ifdef _DUMP_AUDIO_

	if (!m_hAudioFile)
	{
		VO_FILE_SOURCE		fileSource;
		fileSource.nFlag	= VO_FILE_TYPE_NAME;
		fileSource.nOffset	= 0;
		fileSource.nLength	= 0xFFFFFFFF;
		fileSource.nMode	= VO_FILE_WRITE_ONLY;

#ifdef _WIN32_WCE
		fileSource.pSource = (VO_PTR)_T("\\ResidentFlash\\dumper_audio_file.aac");
#elif defined _WIN32
		fileSource.pSource = (VO_PTR)_T("d:\\dumper_audio_file.aac");
#elif defined _IOS
		TCHAR szTmp[1024];
		memset(szTmp, 0, 1024*sizeof(TCHAR));
		voOS_GetAppFolder(szTmp, 1024);
		vostrcat(szTmp, _T("/"));
		vostrcat(szTmp, _T("dumper_audio_file.aac"));
		fileSource.pSource = szTmp;
#else
		fileSource.pSource = (VO_PTR)_T("/sdcard/dumper_audio_file.aac");
#endif	


		m_hAudioFile = cmnFileOpen(&fileSource);	
	}

	if(!m_hAudioFile)
		return;

#ifdef _ADD_ADTS_HEADER_

	static char head1[7];
	int temp = nLen + 7;

	head1[0] = (char)0xff;
	head1[1] = (char)0xf9;

	/*
	const int sample_rates[] =
	{
	96000, 88200, 64000, 48000, 
	44100, 32000,24000, 22050, 
	16000, 12000, 11025, 8000,
	0, 0, 0, 0
	};

	*/
	// 	int sr_index = rtp_aac_get_sr_index(aac_param_ptr->sample_rate);
	// 	head1[2] = (0x01<<6)|(sr_index<<2)|0x00;

	head1[2] = 0x58;

	head1[3] = (char)0x40;// 0x80 profile
	head1[4] = (temp>>3)&0xff;
	head1[5] = ((temp&0x07)<<5|0x1f);
	head1[6] = (char)0xfc;

	cmnFileWrite(m_hAudioFile, head1, sizeof(head1));
#endif

	cmnFileWrite(m_hAudioFile, pData, nLen);
	//cmnFileFlush(m_hAudioFile);

#endif
}

void CDumper::IncreaseFilesize(VO_PTR pszSrcFileName, VO_PTR pszDstFileName, int multiple)
{
#ifdef _WIN32

	VO_FILE_SOURCE src;
	src.nFlag	= VO_FILE_TYPE_NAME;
	src.nOffset	= 0;
	src.nLength	= 0xFFFFFFFF;
	src.nMode	= VO_FILE_READ_ONLY;
	src.pSource = pszSrcFileName;
	VO_PTR hSrc = cmnFileOpen(&src);
	int nLen	= cmnFileSize(hSrc);
	VO_PBYTE pBuf = new VO_BYTE[nLen];
	cmnFileRead(hSrc, pBuf, nLen);
	cmnFileClose(hSrc);

	VO_FILE_SOURCE		fileSource;
	fileSource.nFlag	= VO_FILE_TYPE_NAME;
	fileSource.nOffset	= 0;
	fileSource.nLength	= 0xFFFFFFFF;
	fileSource.nMode	= VO_FILE_WRITE_ONLY;

	fileSource.pSource = pszDstFileName;
	VO_PTR hDst = cmnFileOpen(&fileSource);

	for (int n = 0; n<multiple; n++)
	{
		cmnFileWrite(hDst, pBuf, nLen);
	}

	cmnFileClose(hDst);
	cmnFileClose(hSrc);

	delete pBuf;

#endif
}

void CDumper::MergeFile(VO_PTR pszSrcFileName1, VO_PTR pszSrcFileName2, VO_PTR pszDstFileName)
{
#ifdef _WIN32

	VO_FILE_SOURCE		fileSource;
	fileSource.nFlag	= VO_FILE_TYPE_NAME;
	fileSource.nOffset	= 0;
	fileSource.nLength	= 0xFFFFFFFF;
	fileSource.nMode	= VO_FILE_WRITE_ONLY;

	fileSource.pSource = pszDstFileName;
	VO_PTR hDst = cmnFileOpen(&fileSource);


	// copy 1
	VO_FILE_SOURCE src;
	src.nFlag	= VO_FILE_TYPE_NAME;
	src.nOffset	= 0;
	src.nLength	= 0xFFFFFFFF;
	src.nMode	= VO_FILE_READ_ONLY;
	src.pSource = pszSrcFileName1;
	VO_PTR hSrc = cmnFileOpen(&src);
	int nLen	= cmnFileSize(hSrc);
	VO_PBYTE pBuf = new VO_BYTE[nLen];
	cmnFileRead(hSrc, pBuf, nLen);
	cmnFileClose(hSrc);

	cmnFileWrite(hDst, pBuf, nLen);
	delete pBuf;

	// copy 2
	src.nFlag	= VO_FILE_TYPE_NAME;
	src.nOffset	= 0;
	src.nLength	= 0xFFFFFFFF;
	src.nMode	= VO_FILE_READ_ONLY;
	src.pSource = pszSrcFileName2;
	hSrc = cmnFileOpen(&src);
	nLen	= cmnFileSize(hSrc);
	pBuf = new VO_BYTE[nLen];
	cmnFileRead(hSrc, pBuf, nLen);
	cmnFileClose(hSrc);

	cmnFileWrite(hDst, pBuf, nLen);
	delete pBuf;

	cmnFileClose(hDst);

#endif

}

void CDumper::DumpVideoHeadData(unsigned char* pData, int nLen)
{
#ifdef _DUMP_VIDEO_HEAD_DATA_
	DumpVideoData(pData, nLen);
#endif
}

void CDumper::DumpVideoData(unsigned char* pData, int nLen)
{
#ifdef _DUMP_VIDEO_

	if (!m_hVideoFile)
	{
		VO_FILE_SOURCE		fileSource;
		fileSource.nFlag	= VO_FILE_TYPE_NAME;
		fileSource.nOffset	= 0;
		fileSource.nLength	= 0xFFFFFFFF;
		fileSource.nMode	= VO_FILE_WRITE_ONLY;

// 		VO_TCHAR szFile[256];
// #ifdef _WIN32
// 		vostrcpy (szFile, _T("d:\\dumper_video_file.h264"));
// #else
// 		vostrcpy (szFile, _T("/data/local/voOMXPlayer/dumper_video_file.h264"));
// #endif // _WIN32
// 		fileSource.pSource = (VO_PTR)szFile;


#ifdef _WIN32_WCE
		fileSource.pSource = (VO_PTR)_T("\\ResidentFlash\\dumper_video_file.h264");
#elif defined _WIN32
		fileSource.pSource = (VO_PTR)_T("d:\\dumper_video_file.h264");
#elif defined _IOS
		TCHAR szTmp[1024];
		memset(szTmp, 0, 1024*sizeof(TCHAR));
		voOS_GetAppFolder(szTmp, 1024);
		vostrcat(szTmp, _T("/"));
		vostrcat(szTmp, _T("dumper_video_file.h264"));
		fileSource.pSource = szTmp;
#else
		fileSource.pSource = (VO_PTR)_T("/sdcard/dumper_video_file.h264");
#endif	
		m_hVideoFile = cmnFileOpen(&fileSource);	
	}



	if(!m_hVideoFile)
	{
		VOLOGI("+++++++++++++++++++++Create File failed.....++++++++++++++++++++++");
		return;
	}
		

	cmnFileWrite(m_hVideoFile, pData, nLen);
	//cmnFileFlush(m_hVideoFile);

#endif
}

void CDumper::CloseAllDumpFile()
{
#ifdef _DUMP_STREAM_
	if (m_hRawDataFile)
	{
		cmnFileClose(m_hRawDataFile);
		m_hRawDataFile = VO_NULL;
	}
#endif


#ifdef _DUMP_VIDEO_
	if (m_hVideoFile)
	{
		cmnFileClose(m_hVideoFile);
		m_hVideoFile = VO_NULL;
	}
#endif

#ifdef _DUMP_AUDIO_
	if (m_hAudioFile)
	{
		cmnFileClose(m_hAudioFile);
		m_hAudioFile = VO_NULL;
	}
#endif


	CloseLogFile();

#ifdef _LOG_VIDEO_OUTPUT_
	if(m_hVideoLogFile)
		cmnFileClose(m_hVideoLogFile);
	m_hVideoLogFile = VO_NULL;
#endif

#ifdef _LOG_AUDIO_OUTPUT_
	if(m_hAudioLogFile)
		cmnFileClose(m_hAudioLogFile);
	m_hAudioLogFile = VO_NULL;
#endif

	//DeleteCriticalSection(&m_CS);
}

void CDumper::PrintMemory(unsigned char* pData, int nLen)
{
#if defined( _WRITE_LOG_ ) || defined( _PRINT_LOG_)
	
	char	szInfo[256],tmp[2];
	memset(szInfo, 0, 256);

	int pos = 0;
	for (int n=0; n<nLen; n++)
	{
#ifdef _WIN32
		sprintf(tmp, "%02X", *(pData+n));
#else
		_stprintf(tmp, "%02X", *(pData+n));
#endif
		if (pos == 0)
		{
			szInfo[pos++] = '0';
			szInfo[pos++] = 'X';
			szInfo[pos++] = ' ';
		}
	
		strcpy(szInfo+pos,  tmp);
		pos += 2;
		szInfo[pos++] = ' ';
	}

	WriteLog(szInfo);

#endif
}

void CDumper::WriteLog(char *format, ...)
{
#if defined( _WRITE_LOG_ ) || defined( _PRINT_LOG_)
	if(m_bClosLogFile)
		return;

	char	szInfo[256];
	memset(szInfo, 0, 256);
	char	common_info[256];
	memset(common_info, 0, 256);

	va_list arg_ptr;
	va_start(arg_ptr, format);
	int nWrittenBytes = vsprintf(common_info, format, arg_ptr);
	va_end(arg_ptr);

	// current time
#ifdef _WIN32
	SYSTEMTIME t;
	GetLocalTime(&t);
	//sprintf(szInfo, "[MTV][%03d]%02d:%02d:%02d.%05d  %u +++", m_dwLogCount, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, g_dwDumpID);
	sprintf(szInfo, "[MTV][%03d]%02d:%02d:%02d.%05d  +++", m_dwLogCount, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
#else
	time_t now;
	struct tm *timenow;
	char strtemp[255];

	time(&now);
	timenow = localtime(&now);
	_stprintf(szInfo, "[MTV][%03d]%02d:%02d:%02d  %u +++", m_dwLogCount, timenow->tm_hour, timenow->tm_min, timenow->tm_sec, g_dwDumpID);
#endif // _WIN32
	strcat(szInfo, common_info);
#endif // _WRITE_LOG_


#ifdef _PRINT_LOG_
	VOLOGI("%s", szInfo);
#endif


#ifdef _WRITE_LOG_
	if(!m_hLogFile)
	{

#ifdef _WIN32
		CDumper::OpenLogFile((VO_PTR)_T("d:\\dumper_parse_log.txt"));
#else
		CDumper::OpenLogFile((VO_PTR)_T("/data/local/voOMXPlayer/dumper_parse_log.txt"));
#endif

		if(m_hLogFile)
			CDumper::WriteLog("Begin Log...");
	}

	if(m_hLogFile)
	{
		strcat(szInfo, "\r\n");
		cmnFileWrite(m_hLogFile, szInfo, strlen (szInfo));
	}
#endif


#if defined( _WRITE_LOG_ ) || defined( _PRINT_LOG_)
	m_dwLogCount++;
#endif

	//LeaveCriticalSection(&m_CS);
}

void CDumper::OpenLogFile(VO_PTR pszFileName)
{
#ifdef _WRITE_LOG_
	m_bClosLogFile = false;

	if(m_hLogFile)
		return;	

	VO_FILE_SOURCE		fileSource;
	fileSource.nFlag	= VO_FILE_TYPE_NAME;
	fileSource.nOffset	= 0;
	fileSource.nLength	= 0xFFFFFFFF;
	fileSource.nMode	= VO_FILE_READ_WRITE;
	fileSource.pSource = (VO_PTR)pszFileName;

	m_hLogFile = cmnFileOpen(&fileSource);	

	if(m_hLogFile == VO_NULL)
	{
		return;
	}	
	else
	{
		cmnFileSeek(m_hLogFile, 0, VO_FILE_END);
	}


	char	szInfo[256];
	sprintf(szInfo, "\r\n\r\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
	cmnFileWrite(m_hLogFile, szInfo, strlen (szInfo));

#ifdef _WIN32
	SYSTEMTIME t;
	GetLocalTime(&t);
	sprintf(szInfo, "This log was created on %d/%02d/%02d %02d:%02d:%02d\r\n", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
#else
	time_t now;
	struct tm *timenow;
	char strtemp[255];

	time(&now);
	timenow = localtime(&now);
	_stprintf(szInfo, "This log was created on %d/%02d/%02d %02d:%02d:%02d\r\n", timenow->tm_year, timenow->tm_mon, timenow->tm_mday, timenow->tm_hour, timenow->tm_min, timenow->tm_sec);
#endif

	cmnFileWrite (m_hLogFile, szInfo, strlen (szInfo));
	sprintf(szInfo, "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
	cmnFileWrite (m_hLogFile, szInfo, strlen (szInfo));
#endif
}

void CDumper::CloseLogFile()
{
#ifdef _WRITE_LOG_

	if(m_hLogFile)
	{
		CDumper::WriteLog("End Log...");

		m_bClosLogFile = true;
		cmnFileClose(m_hLogFile);
		m_hLogFile = VO_NULL;
	}
#endif
}

void CDumper::StartLogOutput()
{
#if defined( _LOG_VIDEO_OUTPUT_ ) || defined( _LOG_AUDIO_OUTPUT_)
	m_llStartTime = voOS_GetSysTime();
#endif

#ifdef _LOG_VIDEO_OUTPUT_
	m_dwVideoIdx = 0;
#endif

#ifdef _LOG_AUDIO_OUTPUT_
	m_dwAudioIdx = 0;
#endif
}

void CDumper::RecordVideoLog(VO_U64 llTs, int nLen, int Sync)
{
#ifdef _LOG_VIDEO_OUTPUT_
	m_dwVideoIdx++;

	char	common_info[256];
	memset(common_info, 0, 256);

	if (!m_hVideoLogFile)
	{
		VO_FILE_SOURCE		fileSource;
		fileSource.nFlag	= VO_FILE_TYPE_NAME;
		fileSource.nOffset	= 0;
		fileSource.nLength	= 0xFFFFFFFF;
		fileSource.nMode	= VO_FILE_READ_WRITE;

#ifdef _WIN32_WCE
		fileSource.pSource = (VO_PTR)_T("\\ResidentFlash\\dumper_video_log.txt");
#elif defined _WIN32
		fileSource.pSource = (VO_PTR)_T("d:\\dumper_video_log.txt");
#elif defined _IOS
		TCHAR szTmp[1024];
		memset(szTmp, 0, 1024*sizeof(TCHAR));
		voOS_GetAppFolder(szTmp, 1024);
		vostrcat(szTmp, _T("/"));
		vostrcat(szTmp, _T("dumper_video_log.txt"));
		fileSource.pSource = szTmp;
#else
		fileSource.pSource = (VO_PTR)_T("/sdcard/dumper_video_log.txt");
#endif	
		m_hVideoLogFile = cmnFileOpen(&fileSource);	

		if(m_hVideoLogFile == VO_NULL)
		{
			return;
		}

		strcpy(common_info, "Idx		Real		TS			Size		Sync(0:I frame)\r\n");
		cmnFileWrite(m_hVideoLogFile, common_info, strlen(common_info));
		
	}

	memset(common_info, 0, 256);
	VO_U32 t = voOS_GetSysTime()-m_llStartTime;
	sprintf(common_info, "%d		%u		%llu		%d		%d\r\n", m_dwVideoIdx, t, llTs, nLen, Sync);
	cmnFileWrite(m_hVideoLogFile, common_info, strlen(common_info));
#endif

}

void CDumper::RecordAudioLog(VO_U64 llTs, int nLen,int realsize)
{
#ifdef _LOG_AUDIO_OUTPUT_
	m_dwAudioIdx++;

	char	common_info[256];
	memset(common_info, 0, 256);

	if (!m_hAudioLogFile)
	{
		VO_FILE_SOURCE		fileSource;
		fileSource.nFlag	= VO_FILE_TYPE_NAME;
		fileSource.nOffset	= 0;
		fileSource.nLength	= 0xFFFFFFFF;
		fileSource.nMode	= VO_FILE_READ_WRITE;

#ifdef _WIN32_WCE
		fileSource.pSource = (VO_PTR)_T("\\ResidentFlash\\dumper_audio_log.txt");
#elif defined _WIN32
		fileSource.pSource = (VO_PTR)_T("d:\\dumper_audio_log.txt");
#elif defined _IOS
		TCHAR szTmp[1024];
		memset(szTmp, 0, 1024*sizeof(TCHAR));
		voOS_GetAppFolder(szTmp, 1024);
		vostrcat(szTmp, _T("/"));
		vostrcat(szTmp, _T("dumper_audio_log.txt"));
		fileSource.pSource = szTmp;
#else
		fileSource.pSource = (VO_PTR)_T("/sdcard/dumper_audio_log.txt");
#endif	
		m_hAudioLogFile = cmnFileOpen(&fileSource);	

		if(m_hAudioLogFile == VO_NULL)
		{
			return;
		}

		strcpy(common_info, "Idx		Real		TS			Size		real size\r\n");
		cmnFileWrite(m_hAudioLogFile, common_info, strlen(common_info));

	}

	memset(common_info, 0, 256);
	VO_U32 t = voOS_GetSysTime()-m_llStartTime;
	sprintf(common_info, "%d		%u		%llu		%d		%d\r\n", m_dwAudioIdx, t, llTs, nLen,realsize);
	cmnFileWrite(m_hAudioLogFile, common_info, strlen(common_info));
#endif

}

