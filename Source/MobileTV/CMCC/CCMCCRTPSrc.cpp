	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CCMMCRTPSrc.cpp

	Contains:	CCMMCRTPSrc source file

	Written by: 	Thomas Liang

	Change History (most recent first):
	2011-03-09		Thomas Liang			Create file

*******************************************************************************/
#include "CCMCCRTPSrc.h"
#include "voOSFunc.h"

#ifdef WIN32
#include "Windows.h"
#elif defined LINUX
#include <dlfcn.h>
#include <arpa/inet.h>
#endif //WIN32

#include "stdio.h"

#ifdef WIN32
#define DUMPRTPFILENAME	"d:/cmcc.ts"
#define TSPARSERLIBNAME	TEXT("voTsParser.dll")
#elif defined LINUX
#define DUMPRTPFILENAME	"/data/local/visualon/cmcc.ts"
#define TSPARSERLIBNAME	("libvoTsParser.so")
#endif //WIN32

#define DEFAULTMEMNODENUM	6000

#define LOG_TAG "CCMCCRTPSrc"

void ReceiveDataFromTS(VO_PARSER_OUTPUT_BUFFER* pData)
{
	CCMCCRTPSrc *rtpSrc = (CCMCCRTPSrc *)pData->pUserData;

	rtpSrc->OnReceiveTSData(pData);
}

VO_U32 ReceiveThreadProc(VO_PTR pParam)
{
	CCMCCRTPSrc *localSrc = (CCMCCRTPSrc *)pParam;

	localSrc->ReceiveLoop();

	return -1;
}

VO_U32 MainThreadProc(VO_PTR pParam)
{
	CCMCCRTPSrc *localSrc = (CCMCCRTPSrc *)pParam;

	localSrc->MainLoop();

	return -1;
}


CCMCCRTPSrc::CCMCCRTPSrc()
{
	Reset();
}

CCMCCRTPSrc::~CCMCCRTPSrc()
{
	
}

void CCMCCRTPSrc::Reset()
{
	m_fSendStatus = NULL;
	m_fSendData = NULL;
	m_pUserData = NULL;
	m_pTSParser = NULL;
	m_fGetParserAPI = NULL;

	m_pTSParserData = NULL;

	m_hReceiveThread = NULL;
	m_uReceivePID = 0;

	m_uMainPID=0;
	m_hMainThread = NULL;

	m_hMemSink = NULL;
	m_bExit = 0;

	m_bDumpEnable = 0;
	m_handle = 0;
	
	memset(&m_pLiveSample, 0, sizeof(VO_LIVESRC_SAMPLE));
	memset(&m_pSample, 0, sizeof(VO_SOURCE_SAMPLE));
	
	memset(&m_pTSParserAPISet,0,sizeof(VO_PARSER_API));
	memset(&m_pParserInputBuf, 0, sizeof(VO_PARSER_INPUT_BUFFER));
}

int	CCMCCRTPSrc::Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	ENTER();

	VOLOGI("%s: data %08x, status %08x, sendData %08x", __FUNCTION__,pUserData,fStatus,fSendData);
	
	m_pUserData = pUserData;
	m_fSendStatus = fStatus;
	m_fSendData = fSendData;

	if(LoadTSParser())
	{
		VOLOGE("TS Parser load failed");
		return -1;
	}
	
	VO_PARSER_INIT_INFO info;
	info.pMemOP = NULL;
	info.pProc = ReceiveDataFromTS;
	info.pUserData = this;

	if(!m_pTSParserAPISet.Open)
	{
		VOLOGE("TS Parser Open is NULL");
		return -1;
	}
	
	if(m_pTSParserAPISet.Open(&m_pTSParserData,&info)!=VO_ERR_PARSER_OK)
	{
		VOLOGE("TS Parser Open failed");
		return -1;
	}

	LEAVE();
	return 0;
}

int CCMCCRTPSrc::Close()
{
	ENTER();

	if(m_pTSParserAPISet.Close)
		m_pTSParserAPISet.Close(m_pTSParserData);

	UnLoadTSParser();
	Reset();

	LEAVE();
	return 0;
}

int CCMCCRTPSrc::OnReceiveRTPData(VO_PBYTE buf, unsigned long size)
{
	m_pParserInputBuf.pBuf = buf;
	m_pParserInputBuf.nBufLen = size;
	
	ENTER();

	if(m_bDumpEnable)
		WriteDumpFile(buf,size);

	if(m_pTSParserAPISet.Process != NULL)
	{
		VOLOGW("$$$$Time: %lu, before process ",voTimeGetTime());
		//call TS parser process to convert buf
		if(m_pTSParserAPISet.Process(m_pTSParserData,&m_pParserInputBuf) != VO_ERR_PARSER_OK)
		{
			VOLOGE("TS Parser Process failed");
			return -1;
		}
		VOLOGW("$$$$Time: %lu, after process",voTimeGetTime());
	}

	LEAVE();
	return 0;	
}

void CCMCCRTPSrc::OnReceiveTSData(VO_PARSER_OUTPUT_BUFFER* pData)
{
	int type = 0;
	VO_MTV_FRAME_BUFFER *mtv_buf = NULL;
	ENTER();

	switch(pData->nType)
	{
		case VO_PARSER_OT_AUDIO:
		case VO_PARSER_OT_VIDEO:

			type = (pData->nType==VO_PARSER_OT_VIDEO)?VO_LIVESRC_OUTPUT_VIDEO:VO_LIVESRC_OUTPUT_AUDIO;
			
			mtv_buf = (VO_MTV_FRAME_BUFFER *)pData->pOutputData;

			m_pLiveSample.Sample.Buffer 	= mtv_buf->pData;
			m_pLiveSample.Sample.Duration 	= 0; //???
			m_pLiveSample.Sample.Size 		= mtv_buf->nSize;

			m_pLiveSample.Sample.Time 		= mtv_buf->nStartTime;

			m_pLiveSample.nCodecType 		= mtv_buf->nCodecType;
			m_pLiveSample.nTrackID 			= (pData->nType==VO_PARSER_OT_VIDEO)?2:1;

			if((!mtv_buf->nFrameType) && (type== VO_LIVESRC_OUTPUT_VIDEO))
			{
				VOLOGI("####This frame is video key frame, track type %d, nFrameType %d", type,mtv_buf->nFrameType);
				m_pLiveSample.Sample.Flag = 0x20;
			}
			else
			{
				m_pLiveSample.Sample.Flag = 0;
				VOLOGI("####This frame is not video key frame, track type %d, nFrameType %d", type,mtv_buf->nFrameType);

			}
			
			//send the frame to FS
			if(m_fSendData)
			{
				VOLOGW("####before sent to VOME, Time:%lu: type %d, size 0x%08x, time %lld, startTime %llu, codec 0x%08x",
				voTimeGetTime(),type,m_pLiveSample.Sample.Size, m_pLiveSample.Sample.Time,mtv_buf->nStartTime,m_pLiveSample.nCodecType);

				m_fSendData(m_pUserData, type, &m_pLiveSample);
				
				VOLOGW("####after sent to VOME, time %lu", voTimeGetTime());
			}
			break;
			
		default:
			VOLOGE("Unknown data received from TS parser, type is %d", pData->nType);
			break;
	}
	
	LEAVE();
	return;
}

int	CCMCCRTPSrc::LoadTSParser()
{
#ifdef _WIN32
	if(m_pTSParser != NULL)
		FreeLibrary ((HMODULE)m_pTSParser);
		
	m_pTSParser = LoadLibrary(TSPARSERLIBNAME);
	if(!m_pTSParser)
		return -1;
	
	m_fGetParserAPI = (GETPARSERAPI) GetProcAddress ((HMODULE)m_pTSParser, ("voGetParserAPI"));
#else
	if(m_pTSParser != NULL)
		dlclose (m_pTSParser);
	
	m_pTSParser = dlopen(TSPARSERLIBNAME, RTLD_NOW);
	
	if(!m_pTSParser)
		return -1;
	
	m_fGetParserAPI = (GETPARSERAPI) dlsym (m_pTSParser, ("voGetParserAPI"));
#endif

	if(m_fGetParserAPI == NULL)
		return -1;

	if(m_fGetParserAPI(&m_pTSParserAPISet))
		return -1;
	
	return 0;
}

int CCMCCRTPSrc::UnLoadTSParser()
{
	if(!m_pTSParser)
		return -1;
	
#ifdef _WIN32
	FreeLibrary ((HMODULE)m_pTSParser);
#else
	dlclose (m_pTSParser);
#endif
	
	m_pTSParser = NULL;

	return 0;
}

int	CCMCCRTPSrc::Start()
{
	ENTER();

	int numOfNode = 0;
	int numOfIncrease = 0; 
	
	CBaseConfig *config = new CBaseConfig();

	if(config->Open((VO_PTCHAR)CONFIGFILE)==VO_FALSE)
	{
		VOLOGE("config file could not be opened\n");
		delete config;
		return -1;
	}

	numOfNode = config->GetItemValue ("CONFIG", "NodeNum", DEFAULTMEMNODENUM);	
	m_bDumpEnable = config->GetItemValue ("CONFIG", "DumpEnable", 0);
	
	delete config;
	
	m_hMemSink = new CCMCCRTPSink(numOfNode);

	if(m_bDumpEnable)
		CreateDumpFile();

	CreateThread();

	LEAVE();
	return 0;
}

int	CCMCCRTPSrc::Stop()
{
	ENTER();
	StopThread();

	if(m_bDumpEnable)
		CloseDumpFile();
	
	if(m_hMemSink)
	{
		delete m_hMemSink;
		m_hMemSink = NULL;
	}
	LEAVE();
	return 0;
}

int	CCMCCRTPSrc::CreateThread()
{
	voThreadCreate(&m_hReceiveThread,&m_uReceivePID,ReceiveThreadProc,(VO_PTR)this,0);
	voThreadCreate(&m_hMainThread,&m_uMainPID,MainThreadProc,(VO_PTR)this,0);
	
	return 0;
}

int	CCMCCRTPSrc::StopThread()
{
	m_bExit = 1;

	int nCount = 0;
	while(m_hReceiveThread)
	{
		nCount++;
		voOS_Sleep(10);
		//if(nCount == 200)
			//break;
		continue;
	}

	nCount = 0;
	while(m_hMainThread)
	{
		nCount++;
		voOS_Sleep(10);
		//if(nCount == 200)
			//break;
		continue;
	}
	return 0;
}

void CCMCCRTPSrc::MainLoop()
{
	ENTER();

	while(1)
	{
		if(m_bExit)
			break;
	}

	voThreadClose(m_hMainThread,0);
	m_hMainThread = NULL;
	LEAVE();
	return;
}

void CCMCCRTPSrc::ReceiveLoop()
{
	ENTER();
	while(1)
	{
		if(m_bExit)
			break;
	}

	voThreadClose(m_hReceiveThread,0);
	m_hReceiveThread = NULL;
	LEAVE();
	return;
}

int	CCMCCRTPSrc::CreateDumpFile()
{
#ifdef WIN32
	m_handle = CreateFile(_T(DUMPRTPFILENAME),                // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       OPEN_ALWAYS,             // 
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template) == NULL)

	if (m_handle == INVALID_HANDLE_VALUE)            
	{
		VOLOGE("%s can not be opened", DUMPRTPFILENAME);
		return -1;
	}
#else
	m_handle = fopen(DUMPRTPFILENAME,"w");
	if(!m_handle)
	{
		VOLOGE("%s can not be opened", DUMPRTPFILENAME);
		return -1;
	}
#endif
	return 0;
}

int	CCMCCRTPSrc::WriteDumpFile(VO_PBYTE pBuf, VO_U32 len)
{
	if (m_handle != NULL)
	{
#ifdef WIN32
		BOOL bErrorFlag = FALSE;
		DWORD dwBytesWritten = 0;
		
		bErrorFlag = WriteFile(m_handle,           // open file handle
								pBuf,      // start of data to write
								len,  // number of bytes to write
								&dwBytesWritten, // number of bytes that were written
								NULL);            // no overlapped structure

		if (FALSE == bErrorFlag)
		{
			VOLOGE("Unable to write packet to file.");
		}
		else
		{
			VOLOGI("Write %d bytes data to file",dwBytesWritten);
		}
#else
		fwrite(pBuf,1,len,m_handle);
#endif
	}
	return 0;
}

int	CCMCCRTPSrc::CloseDumpFile()
{
	if(m_handle!=NULL)
#ifdef WIN32
		CloseHandle(m_handle);
#else
		fclose(m_handle);
#endif

	m_handle = NULL;
	return 0;
}

