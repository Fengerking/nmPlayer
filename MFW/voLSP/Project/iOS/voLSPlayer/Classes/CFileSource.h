#pragma once


#ifdef _LINUX_ANDROID
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>
#include <ctype.h>
#elif defined _IOS
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>
#include <ctype.h>
#endif

#include "voSource.h"
#include "voCMutex.h"

typedef void (* FILESOURCE_SENDBUFFER_CALLBACK) (void* pUserData, bool bVideo, VO_SOURCE_SAMPLE * pSample);

#define FILESOURCE_PUSHTHREAD_COUNT 2
enum {FILESOURCE_PUSHTHREAD_AUDIO = 0, FILESOURCE_PUSHTHREAD_VIDEO = 1};

#ifdef _LINUX_ANDROID
#define MAX_PATH 300
#elif defined _IOS
#define MAX_PATH 300
#elif defined _MAC_OS
#define MAX_PATH 300
#endif

class CFileSourceEx
{
public:
	CFileSourceEx(void);
	~CFileSourceEx(void);
	
	virtual bool Open(TCHAR* pFileName, FILESOURCE_SENDBUFFER_CALLBACK fSendBuffer, void* pUserData, VO_U32* pVCodec , VO_U32* pACodec);
	virtual bool Close();
	
	virtual bool Play();
	virtual bool Pause();
	
protected:
#ifdef WIN32
	static DWORD WINAPI PushAudioEntry(void* pParam);
	static DWORD WINAPI PushVideoEntry(void* pParam);
#else
	static void* PushAudioEntry(void* pParam);
	static void* PushVideoEntry(void* pParam);	
#endif
	
  	void PushAudioData();
	void PushVideoData();	
	bool SelectFileReader(const VO_TCHAR *pFileName);
	bool LoadDll();
	void FreeDll();
	
	bool CloseFile();
	
	virtual bool	GetVideoSample(VO_SOURCE_SAMPLE *pSample);
	virtual bool	GetAudioSample(VO_SOURCE_SAMPLE *pSample);
	
	virtual bool	GetVideoExtData(VO_PTR *ppBuf , int &nBufLen);
	virtual bool	GetAudioExtData(VO_PTR *ppBuf , int &nBufLen);
	
	virtual bool	GetAudioProperty(VO_AUDIO_FORMAT *pAudioFormat);
	virtual bool	GetVideoProperty(VO_VIDEO_FORMAT *pVideoFormat);
	
	virtual bool	SetFilePos(VO_S64 nPos);
	
	void			SendVideoHeadData();
	
	FILESOURCE_SENDBUFFER_CALLBACK  m_fSendBuffer;
	void*                           m_pUserData;
	
	bool                            m_bPause;
	bool                            m_bExitThread;
	
	VO_TCHAR                        m_szDllName[MAX_PATH];
	VO_CHAR                         m_szFuncName[MAX_PATH];
	bool                            m_bLoaded;
	VO_SOURCE_READAPI               m_ReadAPI;
	
	VO_PTR                          m_hFileHandle;
	int                             m_nVideoID;
	int                             m_nAudioID;
	VO_AUDIO_FORMAT                 m_AudioForamt;
	VO_VIDEO_FORMAT                 m_VideoFormat;
	VO_U8                           m_pVHD[1024];
	VO_U8                           m_pAHD[1024];
	int                             m_nVHDLen;
	int                             m_nAHDLen;
	
	VO_S64                          m_nLastAudioTime;
	VO_S64                          m_nLastVideoTime;
	bool							m_bSentVideoHeadData;
	voCMutex						m_mtxSendBuf;
	
#ifdef WIN32
	HINSTANCE m_hDll;
	HANDLE    m_hPushThread[FILESOURCE_PUSHTHREAD_COUNT];
#else
	void*     m_hDll;
	pthread_t m_hPushThread[FILESOURCE_PUSHTHREAD_COUNT];	
#endif
		
private:
	void InitVars();
};
