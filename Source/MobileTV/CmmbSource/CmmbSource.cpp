#include "CmmbSource.h"
#if defined _LINUX
#include <unistd.h>
#endif


//#define WRITE_TO_FILE

#define SAMPLE_TIME_DIV 1000 

CCMMBSource::CCMMBSource(void)
//            :m_netClient(&m_bufMFS)
{
  VOLOGI("Start CMMBSource...");
  m_hThread = 0;
  m_nTID = 0;
  m_bRunning = false;
  m_bStreamInfoReady = VO_FALSE;
  m_bFirstKeyFSend = VO_FALSE;
  m_pProgramInfo = 0;
  m_nProgramCount = 0;

  FakeProgram();

  m_nADTSHeadSize = 7;
  m_pADTSHeadData = new VO_BYTE[9];
  m_pADTSHeadData[0] = 0xFF;	
  m_pADTSHeadData[1] = 0xF9;
  int nSampleRate = 6;
  m_pADTSHeadData[2] = (0x1 << 6) | (nSampleRate & 0xF) << 2 ;	
  m_pADTSHeadData[3] = 0x80;
  m_pADTSHeadData[6] = 0xFC;
}

CCMMBSource::~CCMMBSource(void)
{
  delete []m_pADTSHeadData;
}

VO_U32 CCMMBSource::GetMFSThread(VO_PTR pParam)
{
  CCMMBSource* pThis = (CCMMBSource*)pParam;
  pThis->GetMFSProc();
  return 0;
}

void CCMMBSource::GetMFSProc()
{
#define MAX_PACKAGE_SIZE (1*1024*1024)

  unsigned long dwSize;

#ifdef _WIN32
//  FILE* f = fopen("C://cmmb.mfs", "rb");
  FILE* f = fopen(m_szPath, "rb");
#else
  FILE* f = fopen(m_szPath, "rb");
#endif
  if (f)
  {
    fseek(f, 0, SEEK_SET);
    while (fread(&dwSize, 4, 1, f) == 1 && m_bRunning)
    {
      if (dwSize > MAX_PACKAGE_SIZE)
      {
        fclose(f);
        return;
      }
      unsigned char* pData = new unsigned char[dwSize];

      if (fread(pData, dwSize, 1, f) != 1 || !m_bRunning)
      {
        delete [] pData;
        fclose(f);
        return;
      }
      if (!m_bufMFS.PushBack(pData, dwSize) || !m_bRunning)
      {
        delete [] pData;
        fclose(f);
        return;
      }
    }
    fclose(f);
  }
  else
  {
    VOLOGI("Open file error!!-------------------------------------");
  }
}

bool CCMMBSource::SendData(unsigned char* pData, unsigned int nDataLen)
{
  if (!pData || nDataLen == 0)
    return false;
  
  unsigned char* pLocalData = new unsigned char[nDataLen];
  memcpy(pLocalData, pData, nDataLen);
  m_bufMFS.PushBack(pLocalData, nDataLen);

  return true;
}

VO_U32 CCMMBSource::ProcessThread(VO_PTR pParam)
{
  CCMMBSource* pThis = (CCMMBSource*)pParam;
  pThis->ProcessProc();
  return 0;
}

void CCMMBSource::ProcessProc()
{
  while (m_bRunning)
  {
    unsigned char* pBuf = 0;
    unsigned int nBufLen;
    while (!m_bufMFS.PopFront((void**)&pBuf, nBufLen))
    {
#ifdef WIN32
      Sleep(100);
#else
      usleep(100);
#endif
    }

#ifdef WRITE_TO_FILE
    HANDLE fp = CreateFile(_T("c:\\cmmb.bak"), GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
    SetFilePointer(fp, 0, 0, FILE_END);
    unsigned long dw;
    WriteFile(fp, pBuf, nBufLen, &dw, 0);
    CloseHandle(fp);
#endif

#if 1
    VO_PARSER_INPUT_BUFFER inBuffer;
    cmnMemSet(0, &inBuffer, 0, sizeof(inBuffer));
    inBuffer.pBuf = pBuf;
    inBuffer.nBufLen = nBufLen;
    int ret = m_pParser->Process(&inBuffer);
#endif
  }
}


void CCMMBSource::OnParsedData(VO_PARSER_OUTPUT_BUFFER* pData)
{
  ((CCMMBSource*)(pData->pUserData))->OnParsedDataProc(pData);
}

void CCMMBSource::OnParsedDataProc(VO_PARSER_OUTPUT_BUFFER* pData)
{
  switch(pData->nType)
  {
  case VO_PARSER_OT_CMMB_TS0_INFO:
    break;
  case VO_PARSER_OT_CMMB_SERVICE_INFO:
    break;
  case VO_PARSER_OT_CMMB_PROGRAM_GUIDE:
    break;
  case VO_PARSER_OT_CMMB_CONTENT_INFO:
    break;
  case VO_PARSER_OT_CMMB_SCHEDULE_INFO:
    break;
  case VO_PARSER_OT_CMMB_SERVICE_AUX_INFO:
    break;
  case VO_PARSER_OT_CMMB_SERVICE_PARAM_INFO:
    break;
  case VO_PARSER_OT_CMMB_ESG_PARSE_FINISHED:
    break;
  case VO_PARSER_OT_CMMB_DESCRAMBLING:
    break;
  case VO_PARSER_OT_CMMB_MF_HEADER_INFO:
    break;
  case VO_PARSER_OT_CMMB_MSF_HEADER_INFO:
    break;
  case VO_PARSER_OT_CMMB_FAILED:
    break;
  case VO_PARSER_OT_CMMB_PACKET_DATA:
    break;
  case VO_PARSER_OT_STREAMINFO :
    {
      VO_PARSER_STREAMINFO* pInfo = (VO_PARSER_STREAMINFO*)pData->pOutputData;

      m_pProgramInfo[0].ppStreamInfo[0]->uTrackCount = 0;
      if (pInfo->pVideoExtraData && pInfo->nVideoExtraSize>0)
      {
        m_pProgramInfo[0].ppStreamInfo[0]->uTrackCount++;
      }
      if (pInfo->pAudioExtraData && pInfo->nAudioExtraSize>0)
      {
        m_pProgramInfo[0].ppStreamInfo[0]->uTrackCount++;
      }
      if(m_pProgramInfo[0].ppStreamInfo[0]->uTrackCount == 0)
        return;

      m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo = new VO_SOURCE2_TRACK_INFO*[m_pProgramInfo[0].ppStreamInfo[0]->uTrackCount];
      for (VO_U32 i=0; i<m_pProgramInfo[0].ppStreamInfo[0]->uTrackCount; i++)
      {
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[i] = new VO_SOURCE2_TRACK_INFO;
        cmnMemSet(0, m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[i], 0, sizeof(VO_SOURCE2_TRACK_INFO));
      }
      
      VO_U32 track_id = 0;
      if (pInfo->pVideoExtraData && pInfo->nVideoExtraSize>0)
      {
        VOLOGI("Video codec %d", pInfo->nVideoCodecType);
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uTrackID = track_id;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;

        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uCodec		= pInfo->nVideoCodecType;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->pHeadData	= new unsigned char[pInfo->nVideoExtraSize];
        cmnMemCopy(0, m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->pHeadData, pInfo->pVideoExtraData, pInfo->nVideoExtraSize);
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uHeadSize	= pInfo->nVideoExtraSize;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uTrackType = VO_SOURCE2_TT_VIDEO;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uDuration	= 0;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->sVideoInfo.sFormat.Height = pInfo->VideoFormat.height;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->sVideoInfo.sFormat.Width = pInfo->VideoFormat.width;
        //m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->sVideoInfo.sFormat.Type = pInfo->

        track_id++;
      }

      if (pInfo->pAudioExtraData && pInfo->nAudioExtraSize>0)
      {
        VOLOGI("Audio codec %d", pInfo->nAudioCodecType);
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uTrackID = track_id;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;

        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uCodec		= pInfo->nAudioCodecType;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->pHeadData	= new unsigned char[pInfo->nAudioExtraSize];
        cmnMemCopy(0, m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->pHeadData, pInfo->pAudioExtraData, pInfo->nAudioExtraSize);
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uHeadSize	= pInfo->nAudioExtraSize;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uTrackType = VO_SOURCE2_TT_AUDIO;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->uDuration	= 0;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->sAudioInfo.sFormat.Channels = pInfo->AudioFormat.channels;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->sAudioInfo.sFormat.SampleBits = pInfo->AudioFormat.sample_bits;
        m_pProgramInfo[0].ppStreamInfo[0]->ppTrackInfo[track_id]->sAudioInfo.sFormat.SampleRate = pInfo->AudioFormat.sample_rate;

        track_id++;
      }

      m_bStreamInfoReady = VO_TRUE;
    }
    break;
  case VO_PARSER_OT_AUDIO :
    {
      if(m_bStreamInfoReady)
      {
        VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);

		if(pFrame->nCodecType==VO_AUDIO_CodingAAC)
			OnAudioAacFrame(pFrame);
		else if(pFrame->nCodecType==VO_AUDIO_CodingDRA)
			OnAudioDraFrame(pFrame);
		else
			VOLOGI(L"the audio coding type is not supported!\r\n");

#ifdef WIN32
		{
          TCHAR dbg[128];
          static VO_U64 old_tm = 0;
          wsprintf(dbg, _T("audio timestamp: %I64d, interval: %I64d\r\n"), pFrame->nStartTime, pFrame->nStartTime - (old_tm == 0 ? pFrame->nStartTime : old_tm));
          old_tm = pFrame->nStartTime;
          OutputDebugString(dbg);
        }
#endif
      }
/*
	  {
		VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);
		DWORD dw;

		char head1[7];
		int temp = pFrame->nSize + 7;

		head1[0] = (char)0xff;
		head1[1] = (char)0xf9;
		head1[2] = 0x58;
		head1[3] = (char)0x40;// 0x80 profile
		head1[4] = (temp>>3)&0xff;
		head1[5] = ((temp&0x07)<<5|0x1f);
		head1[6] = (char)0xfc;

		HANDLE fp = CreateFile(L"\\residentflash\\audio.aac", GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
		SetFilePointer(fp, 0, 0, FILE_END);
		WriteFile(fp, head1, 7, &dw, 0);
		WriteFile(fp, pFrame->pData, pFrame->nSize, &dw, 0);
		CloseHandle(fp);
	  }
*/
    }
    break;
  case VO_PARSER_OT_VIDEO :
    {
      if(m_bStreamInfoReady)
      {
        VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);

        if (VO_FALSE == m_bFirstKeyFSend) 
        {
          if ((NULL != pFrame) && (0 == pFrame->nFrameType)) 
          {
            m_bFirstKeyFSend = VO_TRUE;
          }
        }

        if (VO_TRUE == m_bFirstKeyFSend) 
        {
          m_bFirstKeyFSend = VO_TRUE;

          VO_SOURCE2_SAMPLE* pSample = new VO_SOURCE2_SAMPLE;
          cmnMemSet(0, pSample, 0, sizeof(VO_SOURCE2_SAMPLE));
          pSample->pBuffer = new unsigned char[pFrame->nSize];
          cmnMemCopy(0, pSample->pBuffer, pFrame->pData, pFrame->nSize);
//          pSample->uDuration = pFrame->nEndTime - pFrame->nStartTime;
          pSample->uSize = pFrame->nSize;

          pSample->uTime = pFrame->nStartTime * SAMPLE_TIME_DIV / 22500;

          m_bufVideo.PushBack(pSample, sizeof(VO_SOURCE2_SAMPLE));
        }
 
 /*
		{
			DWORD dw;
			HANDLE fp = CreateFile(L"E:\\video.h264", GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
			SetFilePointer(fp, 0, 0, FILE_END);
			WriteFile(fp, pFrame->pData, pFrame->nSize, &dw, 0);
			CloseHandle(fp);
		}
*/
      }
/*
#ifdef WIN32
	  {
		  VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)(pData->pOutputData);
		  TCHAR dbg[128];
		  static VO_U64 old_tm = 0;
		  wsprintf(dbg, _T("vedio timestamp: %I64d, interval: %I64d\r\n"), pFrame->nStartTime, pFrame->nStartTime - (old_tm == 0 ? pFrame->nStartTime : old_tm));
		  old_tm = pFrame->nStartTime;
		  OutputDebugString(dbg);
	  }
#endif
*/
    }
    break;
  default:
    {
      //VOLOGI("A parser callback not process!!!");
    }
    break;
  }
}

char* CCMMBSource::StringToLower(char* szString)
{
  for (int i=0; i<strlen(szString); i++)
  {
    if (szString[i] >= 'A' && szString[i] <= 'Z')
      szString[i] = szString[i] + 0x20;
  }
  return szString;
}

VO_BOOL CCMMBSource::GetUrlParam(const char* pUrl, const char* pName, char* pVal, int nValSize)
{
  const char* pStart = pUrl;
  const char* pEnd = 0;
  int nValLen = 0;

  if (!pUrl || !pName || !pVal || nValSize <= 0)
    return VO_FALSE;
	
  while (1)
  {
    pStart = strstr(pStart, pName);
    if (!pStart)
      return VO_FALSE;
    if (pStart == pUrl || (*(pStart - 1) == '?' && *(pStart - 1) == '&' && *(pStart + strlen(pName)) != '='))
    {
      pStart ++;
      continue;
    }

    pStart = pStart + strlen(pName) + 1;
    pEnd = strstr(pStart, "&");
    if (!pEnd)
      nValLen = strlen(pStart);
    else
      nValLen = pEnd - pStart;

    if (nValLen >= nValSize)
      return VO_FALSE;

    strncpy(pVal, pStart, nValLen);
    *(pVal + nValLen) = 0;

    return VO_TRUE;
  }
}

VO_BOOL CCMMBSource::SplitUrl(const char* pUrl)
{
	if (!pUrl)
		return VO_FALSE;
#ifdef _LINUX
	if (strncasecmp(pUrl, "mtv://cmmb?", 11))
#else
	if (strncmp(pUrl, "mtv://cmmb?", 11) && strncmp(pUrl, "MTV://CMMB?", 11))
#endif
		return VO_FALSE;

	VOLOGI("pass the url check");

/*	char* szUrl = new char[strlen(pUrl)+1];
	strcpy(szUrl, pUrl);
	StringToLower(szUrl);

	if (!GetUrlParam(szUrl, "method", m_szMethod, sizeof(m_szMethod)))
	{
		delete [] szUrl;
		return VO_FALSE;
	}

	if (strcmp(m_szMethod, "live") == 0)
	{
		delete [] szUrl;
		return VO_TRUE;
	}
	else if (strcmp(m_szMethod, "local") == 0)
	{
		if (!GetUrlParam(szUrl, "path", m_szPath, sizeof(m_szPath)))
    {
      delete [] szUrl;
			return VO_FALSE;
    }
		else
    {
      delete [] szUrl;
			return VO_TRUE;
    }
	}
	else
	{
		delete [] szUrl;
		return VO_FALSE;
	}
*/
	return VO_TRUE;
}


VO_U32 CCMMBSource::Init(VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam)
{
#ifdef _WIN32
	TCHAR szWorkPath[] = _T("/data/data/com.visualon.osmpDemoPlayer");
#else
  char szWorkPath[] = "/data/data/com.visualon.osmpDemoPlayer";
#endif

	VOLOGI("Source: %s", pSource);
	if (!SplitUrl((const char*)pSource))
		return VOOSMP_ERR_ParamID;

  m_pParser = new CLiveParserBase(VO_LIVESRC_FT_CMMB);
  m_pParser->SetWorkPath(szWorkPath);

  VO_PARSER_INIT_INFO	dmxInitInfo;
  memset (&dmxInitInfo, 0, sizeof(VO_PARSER_INIT_INFO));
  dmxInitInfo.pProc = OnParsedData;
  dmxInitInfo.pUserData = this;
  VO_U32 nRC = m_pParser->Open(&dmxInitInfo);

#ifdef CAS_BASE
  //m_pParser->SetCASType(CAS_TYPE);
  VO_U32 cas_type = CAS_TYPE;
  m_pParser->SetParam(VO_PID_PARSER_CMMB_CA_TYPE, &cas_type);
#endif

  m_hESGParsed.Reset();

  return VOOSMP_ERR_None;
}

VO_U32 CCMMBSource::Uninit()
{
  m_pParser->Close();
  delete m_pParser;
  return VOOSMP_ERR_None;
}

VO_U32 CCMMBSource::Open()
{
//  if (!strcmp(m_szMethod, "local"))
  {
    m_bRunning = true;
    //m_netClient.Start(true, 1234);
    if (voThreadCreate(&m_hThread, &m_nTID, ProcessThread, this, 0) != VO_ERR_NONE)// || 
      // voThreadCreate(&m_hThread, &m_nTID, GetMFSThread, this, 0) != VO_ERR_NONE)
    {
      m_bRunning = false;
      return VOOSMP_ERR_Unknown;
    }
    return VOOSMP_ERR_None;
  }
//  else
//  {
//    return VOOSMP_ERR_Implement;
//  }
}

VO_U32 CCMMBSource::Close()
{
  return VOOSMP_ERR_None;
}

VO_U32 CCMMBSource::Start()
{
  return VOOSMP_ERR_None;
}

VO_U32 CCMMBSource::Pause()
{
  return VOOSMP_ERR_Implement;
}

VO_U32 CCMMBSource::Stop()
{
  //m_netClient.Stop();
  m_bRunning = false;
  return VOOSMP_ERR_None;
}

VO_U32 CCMMBSource::GetSample(VO_SOURCE2_TRACK_TYPE nTrackType , VO_PTR pSample)
{
  unsigned int nBufLen;
  VO_SOURCE2_SAMPLE* sample = 0;
  if (nTrackType == VO_SOURCE2_TT_VIDEO)
  {
    if (m_bufVideo.PopFront((void**)&sample, nBufLen))
    {
      cmnMemCopy(0, pSample, sample, sizeof(VO_SOURCE2_SAMPLE));
      //{
      //  TCHAR dbg[128];
      //  static VO_U64 old_tm = 0;
      //  wsprintf(dbg, _T("timestamp: %I64d, interval: %I64d\r\n"), sample->uTime, sample->uTime - (old_tm == 0 ? sample->uTime : old_tm));
      //  old_tm = sample->uTime;
      //  OutputDebugString(dbg);
      //}
      return VOOSMP_ERR_None;
    }
    else
    {
      return VOOSMP_ERR_Retry;
    }
  }
  else if (nTrackType == VO_SOURCE2_TT_AUDIO)
  {
    if (m_bufAudio.PopFront((void**)&sample, nBufLen))
    {
      cmnMemCopy(0, pSample, sample, sizeof(VO_SOURCE2_SAMPLE));
      //{
      //  TCHAR dbg[128];
      //  static VO_U64 old_tm = 0;
      //  wsprintf(dbg, _T("timestamp: %I64d, interval: %I64d\r\n"), sample->uTime, sample->uTime - (old_tm == 0 ? sample->uTime : old_tm));
      //  old_tm = sample->uTime;
      //  OutputDebugString(dbg);
      //}
      return VOOSMP_ERR_None;
    }
    else
    {
      return VOOSMP_ERR_Retry;
    }
  }
  else
  {
    return VOOSMP_ERR_Implement;
  }
}

VO_U32 CCMMBSource::GetProgramCount(VO_U32 *pProgramCount)
{
  *pProgramCount = m_nProgramCount;
  return VOOSMP_ERR_None;
}

VO_U32 CCMMBSource::GetProgramInfo(VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
{
  *pProgramInfo = m_pProgramInfo;
  return VOOSMP_ERR_None;
}

VO_U32 CCMMBSource::GetCurTrackInfo(VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo)
{
  for (VO_U32 i=0; i<m_nProgramCount; i++)
  {
    for (VO_U32 j=0; j<m_pProgramInfo[i].uStreamCount; j++)
    {
      for (VO_U32 k=0; k<m_pProgramInfo[i].ppStreamInfo[j]->uTrackCount; k++)
      {
        if (nTrackType == m_pProgramInfo[i].ppStreamInfo[j]->ppTrackInfo[k]->uTrackType)
        {
          *ppTrackInfo = m_pProgramInfo[i].ppStreamInfo[j]->ppTrackInfo[k];
          return VOOSMP_ERR_None;
        }
      }
    }
  }

  return VOOSMP_ERR_EOS;
}

VO_U32 CCMMBSource::SelectProgram(VO_U32 nProgram)
{
  VO_BOOL bFound = VO_FALSE;
  for (VO_U32 i=0; i<m_nProgramCount; i++)
  {
    if (nProgram == m_pProgramInfo[i].uProgramID)
    {
      bFound = VO_TRUE;
      m_pProgramInfo[i].uSelInfo = VO_SOURCE2_SELECT_SELECTED;
    }
    else
    {
      m_pProgramInfo[i].uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;
    }
  }
  if (bFound)
    return VOOSMP_ERR_None;
  else
    return VOOSMP_ERR_EOS;
}

VO_U32 CCMMBSource::SelectStream(VO_U32 nStream)
{
  VO_BOOL bFound = VO_FALSE;
  for (VO_U32 i=0; i<m_nProgramCount; i++)
  {
    if (m_pProgramInfo[i].uSelInfo == VO_SOURCE2_SELECT_SELECTED)
    {
      for (VO_U32 j=0; j<m_pProgramInfo[i].uStreamCount; j++)
      {
        if (nStream == m_pProgramInfo[i].ppStreamInfo[j]->uStreamID)
        {
          bFound = VO_TRUE;
          m_pProgramInfo[i].ppStreamInfo[j]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
        }
        else
        {
          m_pProgramInfo[i].ppStreamInfo[j]->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;
        }
      }
      if (bFound)
        return VOOSMP_ERR_None;
      else
        return VOOSMP_ERR_EOS;
    }
  }

  return VOOSMP_ERR_EOS;
}

VO_U32 CCMMBSource::SelectTrack(VO_U32 nTrack)
{
  VO_BOOL bFound = VO_FALSE;
  for (VO_U32 i=0; i<m_nProgramCount; i++)
  {
    if (m_pProgramInfo[i].uSelInfo == VO_SOURCE2_SELECT_SELECTED)
    {
      for (VO_U32 j=0; j<m_pProgramInfo[i].uStreamCount; j++)
      {
        if (m_pProgramInfo[i].ppStreamInfo[j]->uSelInfo == VO_SOURCE2_SELECT_SELECTED)
        {
          for (VO_U32 k=0; k<m_pProgramInfo[i].ppStreamInfo[j]->uTrackCount; k++)
          {
            if (nTrack == m_pProgramInfo[i].ppStreamInfo[j]->ppTrackInfo[k]->uTrackID)
            {
              bFound = VO_TRUE;
              m_pProgramInfo[i].ppStreamInfo[j]->ppTrackInfo[k]->uSelInfo = VO_SOURCE2_SELECT_SELECTED;
            }
            else
            {
              m_pProgramInfo[i].ppStreamInfo[j]->ppTrackInfo[k]->uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;
            }
          }
          if (bFound)
            return VOOSMP_ERR_None;
          else
            return VOOSMP_ERR_EOS;
        }
      }
      return VOOSMP_ERR_EOS;
    }
  }

  return VOOSMP_ERR_EOS;
}

VO_U32 CCMMBSource::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
  if (nParamID == 0x123321AB)
  {
    int* handle = (int*)pParam;
    *handle = (int)this;
    return VOOSMP_ERR_None;
  }
  return VOOSMP_ERR_Implement;
}

VO_U32 CCMMBSource::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
  return VOOSMP_ERR_Implement;
}

VO_U32 CCMMBSource::Seek(VO_U64* pTimeStamp)
{
  return VOOSMP_ERR_Implement;
}

VO_U32 CCMMBSource::GetDuration(VO_U64 * pDuration)
{
  return VOOSMP_ERR_Implement;
}

VO_U32 CCMMBSource::GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
  return VOOSMP_ERR_Implement;
}

VO_U32 CCMMBSource::SendBuffer(const VO_SOURCE2_SAMPLE& buffer)
{
  return VOOSMP_ERR_Implement;
}

void CCMMBSource::FakeProgram()
{
  m_nProgramCount = 1;
  m_pProgramInfo = new VO_SOURCE2_PROGRAM_INFO[m_nProgramCount];
  cmnMemSet(0, m_pProgramInfo, 0, sizeof(VO_SOURCE2_PROGRAM_INFO));
  m_pProgramInfo[0].uProgramID = 0;
  m_pProgramInfo[0].uSelInfo = VO_SOURCE2_SELECT_SELECTABLE;
  m_pProgramInfo[0].sProgramType = VO_SOURCE2_STREAM_TYPE_LIVE;
  strcpy(m_pProgramInfo[0].strProgramName, "CCTV News");
  m_pProgramInfo[0].uStreamCount = 1;
  m_pProgramInfo[0].ppStreamInfo = new VO_SOURCE2_STREAM_INFO*[1];
  m_pProgramInfo[0].ppStreamInfo[0] = new VO_SOURCE2_STREAM_INFO;
  cmnMemSet(0, m_pProgramInfo[0].ppStreamInfo[0], 0, sizeof(VO_SOURCE2_STREAM_INFO));
  m_pProgramInfo[0].ppStreamInfo[0]->uStreamID = 0;
  m_pProgramInfo[0].ppStreamInfo[0]->uSelInfo = 1;
  m_pProgramInfo[0].ppStreamInfo[0]->uBitrate = 0;
}

void CCMMBSource::OnAudioAacFrame(VO_MTV_FRAME_BUFFER * pFrame)
{
  if (NULL==pFrame)
  {
    return;
  }

  VO_SOURCE2_SAMPLE* pSample = new VO_SOURCE2_SAMPLE;
  cmnMemSet(0, pSample, 0, sizeof(VO_SOURCE2_SAMPLE));

  int nSizeSample = m_nADTSHeadSize + pFrame->nSize;
//add adts head
  m_pADTSHeadData[4] = (nSizeSample >> 3) & 0xFF;
  m_pADTSHeadData[5] = (nSizeSample & 0x7) << 5 | 0x1F;

  pSample->pBuffer = new unsigned char[nSizeSample];
  cmnMemCopy(0, pSample->pBuffer, m_pADTSHeadData, m_nADTSHeadSize);
  cmnMemCopy(0, pSample->pBuffer+m_nADTSHeadSize, pFrame->pData, pFrame->nSize);

//pSample->uDuration = pFrame->nEndTime - pFrame->nStartTime;
  pSample->uSize = nSizeSample;
  pSample->uTime = pFrame->nStartTime * SAMPLE_TIME_DIV / 22500;
  m_bufAudio.PushBack(pSample, sizeof(VO_SOURCE2_SAMPLE));
}

void CCMMBSource::OnAudioDraFrame(VO_MTV_FRAME_BUFFER * pFrame)
{
  if (NULL==pFrame)
  {
    return;
  }

  VOLOGI("dra need to do!");
}
