#pragma once
#include "voType.h"
#include "voSource2.h"
#include "voOnStreamType.h"
//#include "NetClient.h"
#include "BufList.h"
#include "voThread.h"
#include "CLiveParserBase.h"
#include "voCSemaphore.h"
#include "cmnMemory.h"
#include "voLog.h"
#include "voCMMBParser.h"

#ifdef CAS_BASE
#include "CCASbase.h"
#define CAS_CLASS	CCASBase
#define CAS_TYPE	0
#endif

class CCMMBSource
{
public:
  CCMMBSource(void);
  ~CCMMBSource(void);

  VO_U32 Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam);
  VO_U32 Uninit();
  VO_U32 Open();
  VO_U32 Close();
  VO_U32 Start();
  VO_U32 Pause();
  VO_U32 Stop();
  VO_U32 Seek(VO_U64* pTimeStamp);
  VO_U32 GetDuration(VO_U64 * pDuration);
  VO_U32 GetSample(VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample);
  VO_U32 GetProgramCount(VO_U32 *pProgramCount);
  VO_U32 GetProgramInfo(VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo);
  VO_U32 GetCurTrackInfo(VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo);
  VO_U32 SelectProgram(VO_U32 nProgram);
  VO_U32 SelectStream(VO_U32 nStream);
  VO_U32 SelectTrack(VO_U32 nTrack);
  VO_U32 GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo);
  VO_U32 SendBuffer(const VO_SOURCE2_SAMPLE& buffer);
  VO_U32 GetParam(VO_U32 nParamID, VO_PTR pParam);
  VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);

  void ProcessProc();
  void GetMFSProc();
  void OnParsedDataProc(VO_PARSER_OUTPUT_BUFFER* pData);

  bool SendData(unsigned char* pData, unsigned int nDataLen);


protected:
  static VO_U32 ProcessThread(VO_PTR pParam);
  static VO_U32 GetMFSThread(VO_PTR pParam);
  static void OnParsedData(VO_PARSER_OUTPUT_BUFFER* pData);
  void FakeProgram();

  void OnAudioAacFrame(VO_MTV_FRAME_BUFFER * pFrame);
  void OnAudioDraFrame(VO_MTV_FRAME_BUFFER * pFrame);

	VO_BOOL SplitUrl(const char* pUrl);
	VO_BOOL GetUrlParam(const char* pUrl, const char* pName, char* pVal, int nValSize);
  char* StringToLower(char* szString);

  CBufList m_bufMFS;
  CBufList m_bufAudio;
  CBufList m_bufVideo;
  //CNetClient m_netClient;
  voThreadHandle m_hThread;
  VO_U32 m_nTID;
  bool m_bRunning;
  CLiveParserBase*	m_pParser;
  voCSemaphore		m_hESGParsed;
  VO_BOOL m_bStreamInfoReady;
  VO_BOOL m_bFirstKeyFSend;

  VO_SOURCE2_PROGRAM_INFO* m_pProgramInfo;
  VO_U32 m_nProgramCount;

	char m_szMethod[10];
	char m_szPath[300];


private:
  VO_BYTE*	m_pADTSHeadData;
  int		m_nADTSHeadSize;
};
