	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CCMMCRTPSrc.h

	Contains:	CCMMCRTPSrc header file

	Written by: 	Thomas Liang

	Change History (most recent first):
	2011-03-09		Thomas Liang			Create file

*******************************************************************************/
#ifndef __CCMMCRTPSRC_H__
#define __CCMMCRTPSRC_H__

#include "voType.h"
#include "voLiveSource.h"
#include "voParser.h"
#include "voThread.h"
#include "voLog.h"
#include "Utility.h"
#include "CBaseConfig.h"

#ifdef LINUX
#define CONFIGFILE	"/etc/cmcc.cfg"
#elif defined WIN32
#define CONFIGFILE	_T("d:/cmcc.cfg")
#endif

#define ENTER()	VOLOGI("%s Enter",__FUNCTION__)
#define LEAVE()	VOLOGI("%s Leave",__FUNCTION__)

typedef VO_S32 (VO_API* GETPARSERAPI)(VO_PARSER_API * pParser);

class CCMCCRTPSrc
{
public:
	CCMCCRTPSrc();
	~CCMCCRTPSrc();

public:
	int			 		Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData);
	int					Close();
	void				Reset();

	int					OnReceiveRTPData(VO_PBYTE buf, unsigned long size);
	void				OnReceiveTSData(VO_PARSER_OUTPUT_BUFFER* pData);
	
	int					LoadTSParser();
	int					UnLoadTSParser();

	int					CreateThread();
	int					StopThread();

	int					CreateDumpFile();
	int					WriteDumpFile(VO_PBYTE buf, VO_U32 len);
	int					CloseDumpFile();

	virtual int			Start();
	virtual int			Stop();

	virtual void		MainLoop();
	virtual void 		ReceiveLoop();
	
private:
	VOSRCSTATUS				m_fSendStatus;
	VOSRCSENDDATA 			m_fSendData;
	VO_PTR 					m_pUserData;

	void *					m_pTSParser;
	GETPARSERAPI			m_fGetParserAPI;

	voThreadHandle			m_hReceiveThread;
	VO_U32					m_uReceivePID;

	voThreadHandle			m_hMainThread;
	VO_U32					m_uMainPID;
	int						m_bDumpEnable;

#ifdef WIN32
	HANDLE 					m_handle;
#else
	FILE *					m_handle;
#endif //WIN32

public:	
	VO_PTR					m_pTSParserData;
	VO_PARSER_API			m_pTSParserAPISet;

	VO_PARSER_INPUT_BUFFER	m_pParserInputBuf;
	int						m_bExit;
	
	VO_LIVESRC_SAMPLE		m_pLiveSample;
	VO_SOURCE_SAMPLE		m_pSample;

	CCMCCRTPSink *			m_hMemSink;
};

#endif //#define __CCMMCRTPSRC_H__
