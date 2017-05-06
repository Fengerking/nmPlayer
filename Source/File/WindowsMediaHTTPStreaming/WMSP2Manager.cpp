#include "WMSP2Manager.h"
#include "fMacros.h"
#include "voOSFunc.h"
#include "TCPClientSocket.h"
#include "voASF.h"
#include "voIndex.h"
#include "voCheck.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


static VO_PTR VO_API stream_open(VO_FILE_SOURCE * pSource)
{
	CWMSP2Manager * pManager = (CWMSP2Manager *)pSource->nReserve;
	return pManager->getStream();
}

static VO_S32 VO_API stream_read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	CStreamCache * pCachebuffer = (CStreamCache*)pFile;
	return pCachebuffer->read((VO_PBYTE)pBuffer, uSize);
}

static VO_S32 VO_API stream_write(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	return -1;
}

static VO_S32 VO_API stream_flush(VO_PTR pFile)
{
	return -1;
}

static VO_S64 VO_API stream_seek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag)
{
	CStreamCache * pCachebuffer = (CStreamCache*)pFile;
	return pCachebuffer->seek(nPos, uFlag);
}

static VO_S64 VO_API stream_size(VO_PTR pFile)
{
	CStreamCache * pCachebuffer = (CStreamCache*)pFile;
	return pCachebuffer->FileSize();
}

static VO_S64 VO_API stream_save(VO_PTR pFile)
{
	return -1;
}

static VO_S32 VO_API stream_close(VO_PTR pFile)
{
	CStreamCache * pCachebuffer = (CStreamCache*)pFile;
	return pCachebuffer->seek(0, VO_FILE_BEGIN);
}


CWMSP2Manager::CWMSP2Manager(void)
: CThread("CWMSP2Manager")
, m_eStateManager(Manager_Stop)
, m_llSeekTime(0)
, m_llStartTime(0)
, m_bSeeked(VO_FALSE)
, m_WMHTTP(this)
, m_hASFFR(NULL)
, m_pCheck(NULL)
{
	m_FileIO.Close = stream_close;
	m_FileIO.Flush = stream_flush;
	m_FileIO.Open = stream_open;
	m_FileIO.Read = stream_read;
	m_FileIO.Save = stream_save;
	m_FileIO.Seek = stream_seek;
	m_FileIO.Size = stream_size;
	m_FileIO.Write = stream_write;

	memset(&m_apiASFFR, 0, sizeof(m_apiASFFR));
}

CWMSP2Manager::~CWMSP2Manager(void)
{
}

VO_U32 CWMSP2Manager::Init(VO_PTR pSource, VO_U32 nFlag, VO_SOURCE2_INITPARAM* pParam)
{
	VOLOGINIT(pParam->strWorkPath);

	VO_U32 uRet = voSourceBaseWrapper::Init(pSource, nFlag, pParam);
	if (uRet)
		return uRet;

	if (!(VO_SOURCE2_FLAG_OPEN_URL & nFlag))
		return VO_RET_SOURCE2_OPENFAIL;

	return VO_RET_SOURCE2_OK;
}

VO_U32 CWMSP2Manager::Uninit()
{
	VOLOGUNINIT();

	voSourceBaseWrapper::Uninit();

	if (m_hASFFR)
	{
		m_apiASFFR.Close(m_hASFFR);
		m_hASFFR = NULL;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 CWMSP2Manager::Open()
{
	VO_U32 uRet = voCheckLibInit(&m_pCheck, VO_INDEX_SRC_MSHTTP, VO_LCS_WORKPATH_FLAG, 0, m_pstrWorkPath);
	if (uRet)
	{
		if (m_pCheck)
			voCheckLibUninit(m_pCheck);

		m_pCheck = NULL;
	}
	else
	{
		VO_CHAR szURL[2048] = { 0 };
		if (sizeof(VO_TCHAR) != 1)
			wcstombs(szURL, (wchar_t*)m_Url, 2048);
		else
			strcpy(szURL, (char*)m_Url);

		uRet = m_WMHTTP.open(szURL);
		if (uRet)
		{
			VOLOGE("!Open 0x%08x", uRet);
		}
		else
		{
			uRet = voSourceBaseWrapper::Open();
		}
	}

	if (m_nFlag & VO_SOURCE2_FLAG_OPEN_ASYNC)
		NotifyEvent(VO_EVENTID_SOURCE2_OPENCOMPLETE, (VO_U32)&uRet, 0);

	if (uRet)
	{
		VOLOGE("!Open 0x%08x", uRet);
		return uRet;
	}

	return CThread::ThreadStart();
}

VO_U32 CWMSP2Manager::Start()
{
	if (Manager_Paused == m_eStateManager)
	{
		if (m_WMHTTP.play(0, m_Stream.WrittenSize()) != 0) {
			VOLOGE("WMHTTP play");
			return VO_ERR_BASE;
		}

		VOLOGR("WMHTTP play 0, %lld", m_Stream.WrittenSize());
	}

	m_eStateManager = Manager_Running;

	return VO_RET_SOURCE2_OK;
}

VO_U32 CWMSP2Manager::Pause()
{
	m_eStateManager = Manager_Pausing;

	return VO_RET_SOURCE2_OK;
}

VO_U32 CWMSP2Manager::Stop()
{
	m_eStateManager = Manager_Stop;

	m_Stream.close();
	m_WMHTTP.stop(8000);

	return VO_RET_SOURCE2_OK;
}
VO_U32 CWMSP2Manager::Seek(VO_U64* pTimeStamp)
{
	VOLOGR("+Seek %lld", *pTimeStamp);
	m_llSeekTime = *pTimeStamp;

	m_bSeeked = VO_TRUE;
	m_Event.Signal();

	voCAutoLock lock(&m_lock);

	VOLOGR("Flush");
	m_SourceBuf.Flush();

	if (Manager_Paused == m_eStateManager)
		m_eStateManager = Manager_Pausing;

	m_SourceBuf.set_pos(*pTimeStamp);
	VOLOGR("-Seek %lld", *pTimeStamp);

	return VO_RET_SOURCE2_OK;
}


VO_U32 CWMSP2Manager::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	voSourceBaseWrapper::SetParam(nParamID, pParam);

	switch (nParamID)
	{
	case VO_PID_SOURCE2_LIBOP:
		{
			SetLibOperator((VO_LIB_OPERATOR *)pParam);
			return VO_ERR_NONE;
		}
		break;

	case VO_PID_SOURCE2_WORKPATH:
		{
			SetWorkPath((TCHAR*)pParam);
			return VO_ERR_NONE;
		}
		break;

	case VO_PID_SOURCE2_EVENTCALLBACK:
	case VO_PID_SOURCE2_DRMCALLBACK:
	case VO_PID_COMMON_LOGFUNC:
	case VO_PID_SOURCE2_IO:
	case VO_PID_SOURCE2_SAMPLECALLBACK:
	case VO_PID_SOURCE2_BITRATEADAPTATIONCALLBACK:
	case VO_PID_SOURCE2_CC_AUTO_SWITCH_DURATION:
	case VO_PID_SOURCE2_ENABLECC:
		return VO_ERR_NONE;
	}

	return m_hASFFR ? m_apiASFFR.SetSourceParam(m_hASFFR, nParamID, pParam) : VO_ERR_NONE;
}


VO_U32 CWMSP2Manager::doPlay(VO_S64 llStartTime/* = 0*/)
{
	m_Stream.close();
	m_Stream.start();

	m_llStartTime = llStartTime;
	if (m_WMHTTP.play(llStartTime) != 0) {
		VOLOGE("!WMHTTP play");

		return VO_ERR_BASE;
	}

	VO_FILE_SOURCE source;
	source.nFlag = 0x102;//0x00020101;
	source.pSource = m_PD_Param.mFilePath;
	source.nReserve = (VO_U32)this;

	VO_SOURCE_OPENPARAM param = { 0 };//TODO
	param.nFlag = source.nFlag;
	param.pSource = &source;
	param.pSourceOP = &m_FileIO;
	if (m_ptr_drmcallback->fCallback)
		param.pDrmCB = (VO_SOURCEDRM_CALLBACK*)m_ptr_drmcallback;

	if (m_apiASFFR.Open(&m_hASFFR, &param) != 0)
	{
		VOLOGE("!FR Open");
		//NotifyEvent(VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL, 0, 0);
		m_hASFFR = NULL;
		m_eStateManager = Manager_Stop;
		return VO_ERR_BASE;
	}

	return VO_RET_SOURCE2_OK;
}


VO_U32 CWMSP2Manager::AddEOS()
{
	VOLOGR("AddEOS");

	VO_SOURCE2_SAMPLE s_Sample2;

	VO_CHAR   data[20];

	s_Sample2.uSize = 20;
	s_Sample2.uFlag = VO_SOURCE2_FLAG_SAMPLE_EOS;
	s_Sample2.pBuffer = (VO_PBYTE)data;

	while (m_SourceBuf.AddBuffer(VO_SOURCE2_TT_VIDEO, &s_Sample2) == VO_RET_SOURCE2_NEEDRETRY && Manager_Stop != m_eStateManager)
		voOS_Sleep(500);

	while (m_SourceBuf.AddBuffer(VO_SOURCE2_TT_AUDIO, &s_Sample2) == VO_RET_SOURCE2_NEEDRETRY && Manager_Stop != m_eStateManager)
		voOS_Sleep(500);

	return VO_RET_SOURCE2_OK;
}


VO_U32 CWMSP2Manager::doSeek()
{
	if (m_hASFFR)
	{
		m_apiASFFR.Close(m_hASFFR);
		m_hASFFR = NULL;
	}

	m_Stream.close();
	VOLOGR("WMHTTP stop");
	m_WMHTTP.stop();

	VOLOGR("Seek Loop %lld", m_llSeekTime);
	VO_U32 iRet = doPlay(m_llSeekTime);
	if (iRet) {
		VOLOGE("!doPlay");

		m_eStateManager = Manager_Stop;
	}

	return iRet;
}


VO_VOID CWMSP2Manager::ThreadMain()
{
	VO_U64 ullTimeA = 0;
	VO_U64 ullTimeV = 0;
	VO_SOURCE2_SAMPLE s_Sample2;

	while (Manager_Stop != m_eStateManager)
	{
		{
			voCAutoLock lock(&m_lock);

			memset(&s_Sample2, 0, sizeof(VO_SOURCE2_SAMPLE));

			if (ullTimeV <= ullTimeA)
			{
				s_Sample2.uTime = ullTimeV;

				VO_U32 iRet = voSourceBaseWrapper::GetSample(VO_SOURCE2_TT_VIDEO, &s_Sample2);
				if (iRet)
				{
					ullTimeV = VO_MAXU64;
				}
				else
				{
					if (s_Sample2.uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
					{
					}
					else
					{
						ullTimeV = s_Sample2.uTime;
					}

					while (!m_bSeeked  && m_SourceBuf.AddBuffer(VO_SOURCE2_TT_VIDEO, &s_Sample2) == VO_RET_SOURCE2_NEEDRETRY && Manager_Stop != m_eStateManager)
					{
						if (Manager_Pausing == m_eStateManager)
						{
							VOLOGR("WMHTTP stop");
							m_WMHTTP.stop();
							m_eStateManager = Manager_Paused;
						}

						voOS_Sleep(500);
					};

				}
			}
			else
			{
				s_Sample2.uTime = ullTimeA;

				VO_U32 iRet = voSourceBaseWrapper::GetSample(VO_SOURCE2_TT_AUDIO, &s_Sample2);
				if (iRet)
				{
					ullTimeA = VO_MAXU64;
				}
				else
				{
					if (s_Sample2.uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
					{
					}
					else
					{
						ullTimeA = s_Sample2.uTime;
					}

					while (!m_bSeeked && m_SourceBuf.AddBuffer(VO_SOURCE2_TT_AUDIO, &s_Sample2) == VO_RET_SOURCE2_NEEDRETRY && Manager_Stop != m_eStateManager)
					{
						if (Manager_Pausing == m_eStateManager)
						{
							VOLOGR("WMHTTP stop");
							m_WMHTTP.stop();
							m_eStateManager = Manager_Paused;
						}

						voOS_Sleep(500);
					};
				}
			}

			if (VO_MAXU64 == ullTimeA && VO_MAXU64 == ullTimeV)
			{
				AddEOS();

				m_Event.Wait();
			}
		}

		if (m_bSeeked || m_llStartTime != m_llSeekTime)
		{
			VOLOGR("+seeking");

			VO_U32 uRet = 0;
			do
			{
				uRet = doSeek();
				if (uRet) {
					VOLOGE("!doSeek");
					break;
				}
			} while (m_llStartTime != m_llSeekTime);
			VOLOGR("-seeking");

			ullTimeA = 0;
			ullTimeV = 0;

			m_bSeeked = VO_FALSE;

			m_bAudioHeadDataSend = VO_FALSE;
			m_bVideoHeadDataSend = VO_FALSE;

			if (m_nFlag & VO_SOURCE2_FLAG_OPEN_ASYNC)
				m_ptr_EventCallback->SendEvent(m_ptr_EventCallback->pUserData, VO_EVENTID_SOURCE2_SEEKCOMPLETE, (VO_U32)&m_llSeekTime, (VO_U32)&uRet);
		}
	}

	AddEOS();
}

VO_VOID CWMSP2Manager::NotifyEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	if (m_ptr_EventCallback && m_ptr_EventCallback->SendEvent)
	{
		VOLOGI("CBEvent %x", nID);
		m_ptr_EventCallback->SendEvent(m_ptr_EventCallback->pUserData, nID, nParam1, nParam2);
	}
}


VO_U32 CWMSP2Manager::OnOpen()
{
	VO_S32(*pvoGetFRAPI)(VO_SOURCE_READAPI*, VO_U32);

#ifdef _IOS
	voGetASFReadAPI(&m_apiASFFR, 0);
#else
	vostrcpy(m_szDllFile, _T("voASFFR"));
	vostrcpy(m_szAPIName, _T("voGetASFReadAPI"));

#ifdef _WINDOWS
	vostrcat(m_szDllFile, _T(".Dll"));
#else
	vostrcat(m_szDllFile, _T(".so"));
#endif

	if (LoadLib(NULL) == 0)
	{
		VOLOGE("!LoadLib");
		return VO_RET_SOURCE2_FAIL;
	}

	if (m_pAPIEntry)
	{
		pvoGetFRAPI = (VO_S32(*)(VO_SOURCE_READAPI*, VO_U32))m_pAPIEntry;
		pvoGetFRAPI(&m_apiASFFR, 0);
	}
	else
	{
		VOLOGE("!getapi");
		return VO_RET_SOURCE2_FAIL;
	}
#endif //_IOS

	VO_S32 iRet = doPlay();
	if (iRet) {
		VOLOGE("!doPlay");
		return VO_RET_SOURCE2_FAIL;
	}

	m_eStateManager = Manager_Running;

	return VO_RET_SOURCE2_OK;
}

VO_U32 CWMSP2Manager::OnClose()
{
	if (m_pCheck)
	{
		voCheckLibUninit(m_pCheck);
		m_pCheck = NULL;
	}

	m_Event.Signal();

	m_WMHTTP.close();

	m_SourceBuf.Flush();

	return WaitingThreadExit();
}

VO_U32 CWMSP2Manager::OnGetDuration(VO_U64 * pDuration)
{
	if (0 == m_ullDuration)
	{
		VO_SOURCE_INFO s_SourceInfo = { 0 };

		VO_U32 uRet = m_apiASFFR.GetSourceInfo(m_hASFFR, &s_SourceInfo);
		if (uRet)
		{
			VOLOGE("!GetSourceInfo %x", uRet);

			m_eStateManager = Manager_Stop;
			return uRet;
		}

		m_ullDuration = s_SourceInfo.Duration;
	}

	*pDuration = m_ullDuration;

	return VO_RET_SOURCE2_OK;
}

VO_U32 CWMSP2Manager::OnGetTrackCnt(VO_U32 * pTraCnt)
{
	VO_SOURCE_INFO s_SourceInfo = { 0 };

	VO_U32 uRet = m_apiASFFR.GetSourceInfo(m_hASFFR, &s_SourceInfo);
	if (uRet)
	{
		VOLOGE("!GetSourceInfo %x", uRet);

		m_eStateManager = Manager_Stop;
		return uRet;
	}

	*pTraCnt = s_SourceInfo.Tracks;

	return VO_RET_SOURCE2_OK;
}

VO_U32 CWMSP2Manager::OnGetSample(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_SOURCE_SAMPLE * pSample)
{
	if (nOutPutType == VO_SOURCE2_TT_VIDEO)
		return m_apiASFFR.GetSample(m_hASFFR, m_uVideoTrack, pSample);
	else if (nOutPutType == VO_SOURCE2_TT_AUDIO)
		return m_apiASFFR.GetSample(m_hASFFR, m_uAudioTrack, pSample);
	else
		return VO_RET_SOURCE2_OUTPUTNOTFOUND;
}

VO_U32 CWMSP2Manager::OnGetTrackAvailable(VO_U32 uTrackIndex, VO_BOOL * beAlai)
{
	VO_SOURCE_TRACKINFO s_TrackInfo;
	memset(&s_TrackInfo, 0, sizeof(VO_SOURCE_TRACKINFO));

	VO_U32 uRet = m_apiASFFR.GetTrackInfo(m_hASFFR, uTrackIndex, &s_TrackInfo);
	if (VO_ERR_INVALID_ARG == uRet)
		*beAlai = VO_FALSE;
	else
		*beAlai = VO_TRUE;

	return VO_RET_SOURCE2_OK;
}
