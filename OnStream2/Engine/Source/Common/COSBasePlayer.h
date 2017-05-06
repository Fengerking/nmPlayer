#ifndef __COSBasePlayer_H__
#define __COSBasePlayer_H__

#ifdef _LINUX
#include <jni.h>
#include "CJniEnvUtil.h"
#endif
#include "voType.h"
#include "voOnStreamType.h"
#include "voOnStreamEngine.h"
#include "CVideoSubtitle.h"
#include "voVideoParser.h"
#include "CBaseConfig.h"
#include "voCMutex.h"
#include "cmnFile.h"
#include "voString.h"
#include "CSEIInfo.h"
#include "CModuleVersion.h"

#include "IOSBasePlayer.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class COSBasePlayer : public IOSBasePlayer
{
public:
	COSBasePlayer();
	virtual ~COSBasePlayer();
	virtual int		Init();
	virtual int		Uninit();

	virtual int		SetDataSource (void * pSource, int nFlag);

	virtual int		Run (void);
	virtual int		Pause (void); 
	virtual int		Stop (void);
    virtual int		Close (void);
	virtual int		Flush (void);
	virtual int		GetStatus (int * pStatus);
	virtual int		GetDuration (int * pDuration);

	virtual int		doReadAudio(VOOSMP_BUFFERTYPE * pBuffer);
	virtual int		doReadVideo(VOOSMP_BUFFERTYPE * pBuffer);
	virtual int		doSendBuffer (int nSSType, VOOSMP_BUFFERTYPE * pBuffer);

	virtual int		SetView(void* pView);
	virtual	int		SetJavaVM(void *pJavaVM, void* obj);
	virtual void*	GetJavaObj();

	virtual int		GetPos (int * pCurPos);
	virtual int		SetPos (int nCurPos);
 

	virtual int		GetParam (int nID, void * pValue);
	virtual int		SetParam (int nID, void * pValue);
	virtual int		GetSubtileSample (voSubtitleInfo * pSample);
	virtual int		GetSEISample (VOOSMP_SEI_INFO * pSample);
	virtual int		GetSubLangNum(int *pNum);
	virtual int		GetSubLangItem(int Index, VOOSMP_SUBTITLE_LANGUAGE** pSubLangItem);
	virtual int		GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** pSubLangInfo);
	virtual int		SelectLanguage(int Index);

protected:
	virtual int		HandleSubtitle(VOOSMP_BUFFERTYPE* pBuffer, int Type, int RC);
	virtual int		HandleImageSub(VOOSMP_BUFFERTYPE* pBuffer, int Type, int RC);
	virtual int		HandleH264(VOOSMP_BUFFERTYPE* pBuffer);

	virtual int		HandleEvent (int nID, void * pParam1, void * pParam2);
	virtual int		ChangEventIDEvent (unsigned int nID, void * pParam1, void * pParam2);

	virtual int		CallBackImage();

	int				ClearLanguage();
	int				CheckingStatus(int nCount);



	int				CompareSPS(parser_out_sps_t *pSrc, parser_out_sps_t *pDes);
	int				CopySPS(parser_out_sps_t *pSrc, parser_out_sps_t *pDes);

	virtual int		OpenDebugConfig();
	virtual int		DumpDebugInfo(int ssType, int nRC, VOOSMP_BUFFERTYPE * pBuffer);

protected:
	VO_LOG_PRINT_CB 			m_cbVOLOG;
	VOOSMP_LIB_FUNC*			m_pOSLibFunc;
	VOOSMP_READBUFFER_FUNC*		m_pOSReadBufFunc;
	VOOSMP_BUFFER_FORMAT		m_sInVideoFormat;
	VOOSMP_BUFFER_FORMAT		m_sInAudioFormat;

	CVideoSubtitle*			m_pVideoSubtile;

	VO_BOOL					m_bSeeking;
	VO_BOOL					m_bCCParser;

	VO_VIDEO_PARSERAPI		m_sVideoParserAPI;
	VO_HANDLE				m_hVideoParser;

#if defined(_LINUX) && !defined(__ODM__)
	JavaVM *				m_JavaVM;
	jobject					m_jOSmpEng;
	CJniEnvUtil*			m_JniUtil;				
#endif

	int						m_nPrefixEnable;
	int						m_nSuffixEnable;
	char					m_cPrefix[32]; //almost using in Android
	char					m_cSuffix[32]; //almost using in Android

	//int						m_nLocalFile;
	//char					m_cLocalFilePath[2*1024]; //almost using in Android

	VO_TCHAR				m_szPathLib[1024];

	int						m_nChanging;
	int						m_nImageUpdated;

	int						m_nH264SEIInfo;
	int						m_nLastSendtime;
	int						m_nSEICallBackTime;

	CSEIInfo*				m_pCSEIInfo;
	int						m_nSEICount;

	int						m_nSubLangGot;
	VOOSMP_SUBTITLE_LANGUAGE_INFO m_sSubLangInfo;

	parser_out_sps_t		m_sH264SPS;
	int						m_nVideoChanged;

	int						m_nSaveVideoBuffer;
	VOOSMP_BUFFERTYPE		m_sBufferVideoSaved;

	int						m_nSaveAudioBuffer;
	VOOSMP_BUFFERTYPE		m_sBufferAudioSaved;

	voCMutex				m_Lock;
	voCMutex				m_EventLock;

	VO_FILE_OPERATOR *		m_pFileOP;

	// config for the debug
	CBaseConfig				m_cfgSource;
	int						m_nLogAudioLevel;
	int						m_nLogVideoLevel;

	char*					m_pAudioDumpFile;
	char*					m_pVideoDumpFile;
	int						m_nDumpAudioLevel;
	int						m_nDumpVideoLevel;
	int						m_nStartSystemTime;

	void*					m_hAudioDumpFile;
	void*					m_hVideoDumpFile;
	CModuleVersion*			m_pModuleVersion;
	int						m_nAudioVideoOnly;
};

#endif
