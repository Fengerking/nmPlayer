#pragma once

#include "cmnFile.h"
#include "voString.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#if 0

//#define		_DUMP_STREAM_
//#define			_DUMP_VIDEO_
//#define			_DUMP_VIDEO_HEAD_DATA_
//#define			_DUMP_AUDIO_
//#define			_ADD_ADTS_HEADER_
//#define			_WRITE_LOG_
#define			_PRINT_LOG_

// output to file
// #define			_LOG_VIDEO_OUTPUT_
// #define			_LOG_AUDIO_OUTPUT_


// output to screen
//#define			_PRINT_ORIGINAL_VIDEO_TIME_STAMP_
//#define			_PRINT_ORIGINAL_AUDIO_TIME_STAMP_

#endif

class CDumper
{
public:
	CDumper(void);
	~CDumper(void);

public:
	//
	static void DumpRawData(unsigned char* pData, int nLen);
	static void DumpVideoData(unsigned char* pData, int nLen);
	static void DumpVideoHeadData(unsigned char* pData, int nLen);
	static void DumpAudioData(unsigned char* pData, int nLen);
	static void CloseAllDumpFile();

	//
	static void WriteLog(char *format, ...);

	// 
	static void PrintMemory(unsigned char* pData, int nLen);

	// 
	static void IncreaseFilesize(VO_PTR pszSrcFileName, VO_PTR pszDstFileName, int multiple);
	static void MergeFile(VO_PTR pszSrcFileName1, VO_PTR pszSrcFileName2, VO_PTR pszDstFileName);

	//
	static void StartLogOutput();
	static void RecordVideoLog(VO_U64 llTs, int nLen, int Sync);
	static void RecordAudioLog(VO_U64 llTs, int nLen,int realsize=0);

private:
	static void OpenLogFile(VO_PTR pszFileName);
	static void CloseLogFile();

private:

#if defined( _LOG_VIDEO_OUTPUT_ ) || defined( _LOG_AUDIO_OUTPUT_)
	static VO_U64					m_llStartTime;
#endif

#ifdef _LOG_VIDEO_OUTPUT_
	static VO_PTR					m_hVideoLogFile;
	static VO_U32					m_dwVideoIdx;
#endif

#ifdef _LOG_AUDIO_OUTPUT_
	static VO_PTR					m_hAudioLogFile;
	static VO_U32					m_dwAudioIdx;
#endif

#ifdef _DUMP_STREAM_
	static VO_PTR			m_hRawDataFile;
#endif

#ifdef _DUMP_VIDEO_
	static VO_PTR			m_hVideoFile;
#endif

#ifdef _DUMP_AUDIO_
	static VO_PTR			m_hAudioFile;
#endif

#if defined( _WRITE_LOG_ ) || defined( _PRINT_LOG_)
	static VO_PTR			m_hLogFile;
	static unsigned long	m_dwLogCount;
	static bool				m_bClosLogFile;
#endif


};

#ifdef _VONAMESPACE
}
#endif
