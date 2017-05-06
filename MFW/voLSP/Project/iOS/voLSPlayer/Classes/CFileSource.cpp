#include "CFileSource.h"
#include "voFile.h"
#include "voLog.h"
#include "voTS.h"
#include "vompType.h"

#ifdef WIN32
#define DELAY(x) Sleep(x)
#else
#define DELAY(x) usleep(x*1000)
#endif

typedef VO_S32 (VO_API * VOGETFILEREADAPI) (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag);

CFileSourceEx::CFileSourceEx(void)
{
	InitVars();
}

CFileSourceEx::~CFileSourceEx(void)
{
	Close();
}

void CFileSourceEx::InitVars()
{
	m_fSendBuffer     = 0;
	m_pUserData       = 0;
	m_bPause          = true;
	m_bExitThread     = false;
	m_bLoaded         = false;
	m_hDll            = 0;
	m_hFileHandle     = 0;
	m_nVideoID        = 0;
	m_nAudioID        = 0;
	m_nVHDLen         = 0;
	m_nAHDLen         = 0;
	m_nLastAudioTime  = 0;
	m_nLastVideoTime  = 0;
	m_bSentVideoHeadData = false;
	
	memset(m_szDllName, 0, sizeof(m_szDllName));
	memset(m_szFuncName, 0, sizeof(m_szFuncName));
	memset(&m_ReadAPI, 0, sizeof(m_ReadAPI));
	memset(&m_AudioForamt, 0, sizeof(m_AudioForamt));
	memset(&m_VideoFormat, 0, sizeof(m_VideoFormat));
	memset(&m_pVHD, 0, sizeof(m_pVHD));
	memset(&m_pAHD, 0, sizeof(m_pAHD));
	
	m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO] = 0;
	m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO] = 0;
}

#ifdef WIN32
DWORD WINAPI 
#else
void*
#endif
CFileSourceEx::PushAudioEntry(void* pParam)
{
	CFileSourceEx* pThis = (CFileSourceEx*)pParam;
	//pThis->PushData(false);
	pThis->PushAudioData();
	return 0;
}

#ifdef WIN32
DWORD WINAPI 
#else
void*
#endif
CFileSourceEx::PushVideoEntry(void* pParam)
{
	CFileSourceEx* pThis = (CFileSourceEx*)pParam;
	//pThis->PushData(true);
	pThis->PushVideoData();
	return 0;
}

void CFileSourceEx::PushAudioData()
{
	/*
	bool bRet = true;
	
	VO_SOURCE_SAMPLE sample;

	while (!m_bExitThread)
	{
		if (m_bPause)
		{
			DELAY(10);
			continue;
		}
		
		if(!m_bSentVideoHeadData)
		{
			m_bSentVideoHeadData = true;
			SendVideoHeadData();
		}
		
		memset(&sample, 0, sizeof(sample));
		
		if(m_nLastAudioTime <= m_nLastVideoTime)
		{
			sample.Time = m_nLastAudioTime;
			bRet = GetAudioSample(&sample);
			
			if (!bRet)
				break;
			
			m_nLastAudioTime = sample.Time;
			m_fSendBuffer(m_pUserData, false, &sample);
		}
		else
		{
			sample.Time = m_nLastVideoTime;
			bRet = GetVideoSample(&sample);
			
			if (!bRet)
				break;
			
			m_nLastVideoTime = sample.Time;	
			m_fSendBuffer(m_pUserData, false, &sample);
		}
		
		//DELAY(10);
	}
	
	if (!bRet)
	{
		printf("Read file failed!\n");
		m_nLastAudioTime = 0xFFFFFFFF;
		m_nLastVideoTime = 0xFFFFFFFF;
		m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO] = 0;
	}
	
	
	printf("Exit read file thread!!!\n");
	*/
	

	printf("Push audio data...\n");
	VO_SOURCE_SAMPLE sample;
	bool bRet = true;
	
	while (!m_bExitThread)
	{
		if (m_bPause)
		{
			DELAY(10);
			continue;
		}
		
		memset(&sample, 0, sizeof(sample));
		
		{
			if (m_nLastAudioTime > m_nLastVideoTime)
				DELAY(5);
			
			while (m_nLastAudioTime <= m_nLastVideoTime && !m_bExitThread)
			{
				voCAutoLock sendLock(&m_mtxSendBuf);
				sample.Time = m_nLastAudioTime;
				bRet = GetAudioSample(&sample);
				if (!bRet)
					break;
				m_nLastAudioTime = sample.Time;
				
				m_fSendBuffer(m_pUserData, false, &sample);
				//DELAY(1);
				DELAY(10);
			}
		}
		
		if (!bRet)
		{
			printf("Read audio failed!\n");
			m_nLastAudioTime = 0xFFFFFFFF;
			m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO] = 0;
			break;
		}
	}
	
	printf("Exit audio thread!!!\n");
}

void CFileSourceEx::PushVideoData()
{
	VO_SOURCE_SAMPLE sample;
	bool bRet = true;
	
	while (!m_bExitThread)
	{
		if (m_bPause)
		{
			DELAY(10);
			continue;
		}
		
		if(!m_bSentVideoHeadData)
		{
			m_bSentVideoHeadData = true;
			SendVideoHeadData();
		}

		
		memset(&sample, 0, sizeof(sample));
		{
			if (m_nLastVideoTime >= m_nLastAudioTime)
				DELAY(5);
			
			while (m_nLastVideoTime < m_nLastAudioTime && !m_bExitThread)
			{
				voCAutoLock sendLock(&m_mtxSendBuf);
				sample.Time = m_nLastVideoTime;
				bRet = GetVideoSample(&sample);
				if (!bRet)
					break;
				m_nLastVideoTime = sample.Time;
				
				m_fSendBuffer(m_pUserData, true, &sample);
				//DELAY(1);
				DELAY(15);
			}
		}
		
		if (!bRet)
		{
			printf("Read video failed!\n");
			m_nLastVideoTime = 0xFFFFFFFF;
			m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO] = 0;
			break;
		}
	}
	
	printf("Exit video thread!!!\n");
}


bool CFileSourceEx::Open(TCHAR* pFileName, FILESOURCE_SENDBUFFER_CALLBACK fSendBuffer, void* pUserData, VO_U32* pVCodec , VO_U32* pACodec)
{
	VO_SOURCE_OPENPARAM sourceOpen;
	VO_FILE_SOURCE fileSource;
	VO_SOURCE_INFO srcInfo;
	VO_SOURCE_TRACKINFO trackInfo;
	
	if (m_hFileHandle)
		return false;
	
	InitVars();
	
	if (pVCodec)
		*pVCodec = VO_VIDEO_CodingUnused;
	
	if (pACodec)
		*pACodec = VO_AUDIO_CodingUnused;
	
	SelectFileReader(pFileName);
	
	if (!LoadDll())
		return false;
	
	memset(&sourceOpen, 0, sizeof(sourceOpen));
	memset(&fileSource, 0, sizeof(fileSource));
	memset(&srcInfo, 0, sizeof(srcInfo));
	
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nMode = VO_FILE_READ_ONLY;
	fileSource.pSource = pFileName;
	fileSource.nLength = 0;
	fileSource.nOffset = 0;
	
	sourceOpen.pSource = &fileSource;
	sourceOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR | VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE;
	
	VO_U32 nRC = m_ReadAPI.Open(&m_hFileHandle , &sourceOpen);
	if(nRC != VO_ERR_NONE)
		return false;
	
	nRC = m_ReadAPI.GetSourceInfo(m_hFileHandle , &srcInfo);
	if(nRC != VO_ERR_SOURCE_OK)
		return false;
	
	for (VO_U32 i=0; i<srcInfo.Tracks; i++)
	{
		nRC = m_ReadAPI.GetTrackInfo(m_hFileHandle, i, &trackInfo);
		if(nRC != VO_ERR_SOURCE_OK)
			return false;
		
		if(trackInfo.Type == VO_SOURCE_TT_VIDEO)
		{
			if (pVCodec)
				*pVCodec = trackInfo.Codec;
			m_nVideoID = i ; 
			m_nVHDLen = trackInfo.HeadSize;
			if(m_nVHDLen > 0)
				memcpy(m_pVHD , trackInfo.HeadData , m_nVHDLen);
		}
		else if(trackInfo.Type == VO_SOURCE_TT_AUDIO)
		{
			if (pACodec)
				*pACodec = trackInfo.Codec;
			m_nAudioID = i ; 
			m_nAHDLen = trackInfo.HeadSize;
			if(m_nAHDLen > 0)
				memcpy(m_pAHD , trackInfo.HeadData , m_nAHDLen);
		}
	}
	
	m_ReadAPI.GetTrackParam(m_hFileHandle , m_nAudioID , VO_PID_AUDIO_FORMAT , &m_AudioForamt);
	m_ReadAPI.GetTrackParam(m_hFileHandle , m_nVideoID , VO_PID_VIDEO_FORMAT , &m_VideoFormat);
	
	m_pUserData = pUserData;
	m_fSendBuffer = fSendBuffer;
	
	m_bExitThread = false;
	m_bPause = true;
	
#ifdef WIN32
	DWORD tid;
	m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO] = CreateThread(0, 0, PushAudioEntry, this, 0, &tid);
	m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO] = CreateThread(0, 0, PushVideoEntry, this, 0, &tid);
#else
	pthread_create(&m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO], 0, PushAudioEntry, this);
	pthread_create(&m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO], 0, PushVideoEntry, this);
#endif
	
	return true;
}

bool CFileSourceEx::Close()
{
	m_bExitThread = true;
	
#ifdef WIN32
	if (m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO] ||
		m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO])
	{
		if (WaitForMultipleObjects(FILESOURCE_PUSHTHREAD_COUNT, 
								   m_hPushThread, 
								   TRUE, 
								   -1) != WAIT_OBJECT_0)
		{
			DWORD ec;
			
			GetExitCodeThread(m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO], &ec);
			TerminateThread(m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO], ec);
			
			GetExitCodeThread(m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO], &ec);
			TerminateThread(m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO], ec);
		}
	}
#elif defined _LINUX_ANDROID
	void* pThreadResult;
	if (m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO])
		pthread_join(m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO], &pThreadResult);
	if (m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO])
		pthread_join(m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO], &pThreadResult);
#else // _IOS
	
	int nTryTime = 0;
	while (m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO]) 
	{
		DELAY(10);
		nTryTime++;
		if(nTryTime >=50)
			break;
	}
	nTryTime = 0;
	while (m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO]) 
	{
		DELAY(10);
		nTryTime++;
		if(nTryTime >=50)
			break;
	}
	
#endif
	
	bool bRet = CloseFile();
	InitVars();
	
	return bRet;
}

bool CFileSourceEx::Play()
{
	if (!m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO] || 
		!m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO])
		return false;
	
	m_bPause = false;
	return true;
}

bool CFileSourceEx::Pause()
{
	if (!m_hPushThread[FILESOURCE_PUSHTHREAD_AUDIO] || 
		!m_hPushThread[FILESOURCE_PUSHTHREAD_VIDEO])
		return false;
	
	m_bPause = true;
	return true;
}

bool CFileSourceEx::SelectFileReader(const VO_TCHAR *pFileName)
{
	VO_TCHAR cName[MAX_PATH];
	
	vostrcpy(cName , pFileName);
	
	VO_TCHAR *pStr = vostrrchr(cName , '.');
	for (VO_U32 i=0; i<vostrlen(pStr); i++)
		pStr[i] = toupper(pStr[i]);
	
	pStr += 1;
	
	if(!vostrcmp(pStr , _T("AVI")))
	{
		vostrcpy(m_szDllName , _T("voAviFR.dll"));
		strcpy(m_szFuncName , "voGetAVIReadAPI");
	}
	else if(!vostrcmp(pStr , _T("MP4")) || !vostrcmp(pStr , _T("3GP")) || !vostrcmp(pStr , _T("3GPP")))
	{
		vostrcpy(m_szDllName , _T("voMP4FR.dll"));
		strcpy(m_szFuncName , "voGetMP4ReadAPI");
	}
	else if(!vostrcmp(pStr , _T("WMV")) || !vostrcmp(pStr , _T("ASF")))
	{
		vostrcpy(m_szDllName , _T("voASFFR.dll"));
		strcpy(m_szFuncName , "voGetASFReadAPI");
	}
	else if(!vostrcmp(pStr , _T("MKV")))
	{
		vostrcpy(m_szDllName , _T("voMKVFR.dll"));
		strcpy(m_szFuncName , "voGetMKVReadAPI");
	}
	else if(!vostrcmp(pStr , _T("TS")))
	{
		vostrcpy(m_szDllName , _T("voTSParser.dll"));
		strcpy(m_szFuncName , "voGetMTVReadAPI");
	}
	else 
	{
		return false;
	}
	
	return true;
}

bool CFileSourceEx::LoadDll()
{
#ifdef WIN32
	VO_TCHAR szDll[MAX_PATH];
	VO_TCHAR szPath[MAX_PATH];
	GetModuleFileName (0, szPath, sizeof (szPath));
	
	VO_TCHAR * pPos = vostrrchr(szPath, _T('\\'));
	*(pPos + 1) = 0;
	
	vostrcpy(szDll, szPath);
	vostrcat(szDll, m_szDllName);
	m_hDll = LoadLibrary (szDll);
	
	if (m_hDll == NULL)
	{
		::GetModuleFileName (NULL, szPath, sizeof (szPath));
		VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
		*(pPos + 1) = 0;
		
		vostrcpy(szDll, szPath);
		vostrcat(szDll, m_szDllName);
		m_hDll = LoadLibrary (szDll);
	}
#endif
	
	if (m_hDll == NULL)
#ifdef WIN32
		m_hDll = LoadLibrary (m_szDllName);
#elif defined _LINUX_ANDROID
    m_hDll = dlopen(m_szDllName, RTLD_LAZY);
#elif defined _MAC_OS
    m_hDll = dlopen(m_szDllName, RTLD_LAZY);
#elif defined _IOS
    m_hDll = &m_hDll;
#endif
	
	if(m_hDll)
	{
#ifdef WIN32
		VOGETFILEREADAPI pAPI = (VOGETFILEREADAPI)GetProcAddress(m_hDll, m_szFuncName);
#elif defined _LINUX_ANDROID
		VOGETFILEREADAPI pAPI = (VOGETFILEREADAPI)dlsym(m_hDll, m_szFuncName);
#elif defined _MAC_OS
		VOGETFILEREADAPI pAPI = (VOGETFILEREADAPI)dlsym(m_hDll, m_szFuncName);
#elif defined _IOS
		VOGETFILEREADAPI pAPI = voGetMTVReadAPI;
#endif
		
		if(pAPI != NULL)
		{
			pAPI(&m_ReadAPI, 0);
			m_bLoaded = true;
			
			return true;
		}
	}
	
	return false;
}

void CFileSourceEx::FreeDll()
{
	if (m_hDll)
	{
#ifdef WIN32
		FreeLibrary(m_hDll);
#elif defined _LINUX_ANDROID
		dlclose(m_hDll);
#elif defined _IOS
		dlclose(m_hDll);
#endif
		m_hDll = 0;
		memset(&m_ReadAPI, 0, sizeof(m_ReadAPI));
		m_bLoaded = false;
	}
}

bool CFileSourceEx::CloseFile()
{
	if(m_hFileHandle)
	{
		VO_U32 nRC = m_ReadAPI.Close(m_hFileHandle);
		m_hFileHandle = NULL;
		return VO_ERR_NONE == nRC;
	}
	
	return true;
}

bool CFileSourceEx::GetVideoExtData(VO_PTR *ppBuf , int &nBufLen)
{
	if(m_hFileHandle == NULL)
		return false;
	
	nBufLen = m_nVHDLen;
	if(m_nVHDLen > 0)
		*ppBuf = m_pVHD;
	
	return true;
}

bool CFileSourceEx::GetAudioExtData(VO_PTR *ppBuf , int &nBufLen)
{
	if(m_hFileHandle == NULL)
		return false;
	
	nBufLen = m_nAHDLen;
	if(m_nAHDLen > 0)
		*ppBuf = m_pAHD;
	
	return true;
}

bool CFileSourceEx::GetAudioProperty(VO_AUDIO_FORMAT *pAudioFormat)
{
	if(m_hFileHandle == NULL)
		return false;
	
	memcpy(pAudioFormat , &m_AudioForamt , sizeof(VO_AUDIO_FORMAT));
	
	return true;
}

bool CFileSourceEx::GetVideoProperty(VO_VIDEO_FORMAT *pVideoFormat)
{
	if(m_hFileHandle == NULL)
		return false;
	
	memcpy(pVideoFormat , &m_VideoFormat , sizeof(VO_VIDEO_FORMAT));
	
	return true;
}

bool CFileSourceEx::SetFilePos(VO_S64 nPos)
{
	if(!m_hFileHandle)
		return false;
	
	VO_S64  llPos = nPos;
	m_ReadAPI.SetPos(m_hFileHandle , m_nAudioID , &llPos);
	m_ReadAPI.SetPos(m_hFileHandle , m_nVideoID , &llPos);
	
	return true;
}

bool CFileSourceEx::GetVideoSample(VO_SOURCE_SAMPLE *pSample)
{
	if(!m_hFileHandle)
		return false;
	
	VO_U32 nRC = m_ReadAPI.GetSample(m_hFileHandle , m_nVideoID , pSample);
	return nRC == 0 ? true : false;
}

bool CFileSourceEx::GetAudioSample(VO_SOURCE_SAMPLE *pSample)
{
	if(!m_hFileHandle)
		return false;
	
	VO_U32 nRC = m_ReadAPI.GetSample(m_hFileHandle , m_nAudioID , pSample);
	return nRC == 0 ? true : false;
}


void CFileSourceEx::SendVideoHeadData()
{
	VO_SOURCE_SAMPLE sample;
	memset(&sample, 0, sizeof(sample));
	
	int size = 0;
	VO_PTR head = NULL;
	GetVideoExtData(&head, size);
	
	sample.Buffer	= (unsigned char*)head;
	sample.Size		= size;
	
	if(m_fSendBuffer)
		m_fSendBuffer(m_pUserData, true, &sample);
	
	printf("Send head data...\n");
	
}