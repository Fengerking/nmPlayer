//#include <voPDPort.h>
#include <tchar.h>
#include <stdio.h>
#include "CPDSessionWrapper2.h"
#include "CPDTrackWrapper2.h"

#include "CHttpDownloadFile.h"
#include "UFileReader.h"
#include "PDHeaderDataStruct.h"
#include "macro.h"

//#include "vostring.h"
//#include "voLog.h"

extern int g_nLog;
const char* version2="3.10.105.1625 :double check timeout";


static 	VO_PTR VO_API  OpenPDFileReader(VO_FILE_SOURCE * pSource)			/*!< File open operation, return the file IO handle. failed return NULL*/
{
	return UFileReader::voOpenHttpFile((const char*)pSource->pSource,FO_READ_ONLY);
}
static 	VO_S32 VO_API  ReadPDFileReader(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)	/*!< File read operation. return read number, failed return -1, retry return -2*/
{
	int resultSize=-1;
	resultSize=UFileReader::voReadFile(pFile,pBuffer,uSize,&resultSize);
	return resultSize;
}
static 	VO_S32 VO_API  WritePDFileReader(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)/*!< File write operation. return write number, failed return -1*/
{
	return -1;
}
static 	VO_S32 VO_API  FlushPDFileReader(VO_PTR pFile)									/*!< File flush operation. return 0*/
{
	return -1;
}
static 	VO_S64 VO_API  SeekPDFileReader(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag)	/*!< File seek operation. return current file position, failed return -1*/
{
	return UFileReader::voSeekFile(pFile,nPos,(VOFILESEEKPOS)(uFlag+1));
}
static 	VO_S64 VO_API  SizePDFileReader(VO_PTR pFile)								/*!< File get size operation. return file size, failed return -1*/
{
	int size=0;
	return UFileReader::voGetFileSize(pFile,&size);
}
static 	VO_S64 VO_API  SavePDFileReader(VO_PTR pFile)									/*!< File get saved operation. return file saved size, failed return -1*/
{
	return -1;
}
static 	VO_S32 VO_API  ClosePDFileReader(VO_PTR pFile)									/*!< File close operation, failed return -1*/
{
	UFileReader::voCloseFile(pFile);
	return 0;
}
#define VO_REMAIN_FILE_FRAGMENT_COUNT			30
static long VOFILEAPI  fileTime2Pos(HVOFILEREAD pPDSource, int nMediaTime, int* pnFilePos, bool bStart)
{
	CPDSessionWrapper2	* reader=(CPDSessionWrapper2*)pPDSource;
	return reader->Time2FilePos(nMediaTime, pnFilePos, bStart);
}
long CPDSessionWrapper2::Time2FilePos(int nMediaTime, int* pnFilePos, bool bStart)
{
	VO_FILE_MEDIATIMEFILEPOS data;
	data.nFlag				= bStart?1:0;
	data.llMediaTime	= nMediaTime;
	int ret= m_newReaderAPI.GetSourceParam(m_hSource,VO_PID_FILE_MEDIATIME2FILEPOS,&data);
	*pnFilePos = (int)data.llFilePos;
	return ret;
}
static int GetDefaultInitParam2(SPDInitParam* param)
{
#ifdef X86_PC
	char* logFlagName="c:/voLogFlag.tmp";
	char* logFlagName2="d:/voLogFlag.tmp";
#endif//_WIN32_WCE
#ifdef G1
	char* logFlagName="/data/local/voLogFlag.tmp";
	char* logFlagName2="/sdcard/voLogFlag.tmp";
#endif
#ifdef _WIN32_WCE
	char* logFlagName="/voLogFlag.tmp";
	char* logFlagName2="/my documents/voLogFlag.tmp";
#endif//G1

	FILE* pSDPFile=fopen(logFlagName,"r");
	if(pSDPFile==NULL)
		pSDPFile=fopen(logFlagName2,"r");
	if(pSDPFile)
	{
		fseek( pSDPFile, 0L, SEEK_END );
		int fileSize = ftell(pSDPFile);
		if(fileSize<0)
			fileSize = 1024*10;
		char* pSDPData = new char[fileSize+1];

		fseek(pSDPFile,0,SEEK_SET);
		int sdpDataSize = fread(pSDPData, 1, fileSize, pSDPFile);
		pSDPData[sdpDataSize] = '\0';
		char * pRTSPLink = strstr(pSDPData, "log=");
		int flag=0;
		if(pRTSPLink)
		{
			if(!(sscanf(pRTSPLink, "log=%d", &param->nSDKlogflag) == 1))//&&flag>0&&flag<3))
			{
				flag++;	
			}
		}

		delete[] pSDPData;
		fclose(pSDPFile);
		pSDPFile=NULL;
		return 1;
	}
	else
	{
		voLog_android(logFlagName,"fail to open");
	}
	return 0;
}
CPDSessionWrapper2::CPDSessionWrapper2(VO_SOURCE_OPENPARAM * pParam)
	: 
	m_hSource(NULL)	
	, m_dwDuration(0)	
	, m_fPDMediaTime2FilePos(NULL)
	, m_NetGet(this)
#ifdef _HTC
	, m_bNoWritePDHeader(false)
#else
	, m_bNoWritePDHeader(true)
#endif
	, m_dwClipBitrate(0)
	,m_downLoadThreadPriority(0)
	,m_downLoadType(VO_NETDOWN_TYPE_NORMAL)
	,m_url(NULL)
	,m_currentTrackType(0)
	,m_nMediaType(VOMediaTypeUnknown)
	,m_dllLoader(NULL)
	,m_newPos(-1)
	,m_previousSeekTime(0)
	,m_isLoading (false)
	,m_nAutoExtendBufTime(0)
{
	memset(&m_tracks,0,sizeof(m_tracks));
	m_sourceInfo.Tracks=0;
	m_opFile.Close		=	ClosePDFileReader;
	m_opFile.Flush		=	FlushPDFileReader;
	m_opFile.Open		=	OpenPDFileReader;
	m_opFile.Read		=	ReadPDFileReader;
	m_opFile.Save		=	SavePDFileReader;
	m_opFile.Seek		=	SeekPDFileReader;
	m_opFile.Size		=	SizePDFileReader;
	m_opFile.Write		=	WritePDFileReader;
	memcpy(&m_param,pParam,sizeof(VO_SOURCE_OPENPARAM ));
	memset(&m_newReaderAPI,0,sizeof(m_newReaderAPI));

	//TO_DO:init params
	memset(&m_initParam,0,sizeof(m_initParam));
	long version=1;
	PDSetParam(VOID_FILEREAD_CALLBACK_NOTIFY,m_param.pSourceOP);
	PDSetParam(VOID_STREAMING_SDK_VERSION,&version);
	if (pParam->nReserve)
	{
		PDSetParam(VOID_STREAMING_INIT_PARAM, (VO_PTR)pParam->nReserve);
	} 
	else//default params
	{
		m_initParam.nBufferTime = 5;			//<S>
		m_initParam.nPlayBufferTime = 90;		//<S>
		m_initParam.nHTTPDataTimeOut = 15000;	//<MS>
		m_initParam.nMaxBuffer = 5120;			//<KB>
		m_initParam.nTempFileLocation = 1;		//storage
		m_initParam.nPacketLength = 5;			//<KB>
		m_initParam.nHttpProtocol = 1;
		strcpy((char*)m_initParam.mUserAgent, "VisualOn-PD Player");
#ifdef _WIN32_WCE
		_tcscpy(m_initParam.mFilePath, _T("\\tmp.3gp"));
#else
		_tcscpy(m_initParam.mFilePath, _T("C:\\visualon\\VOPD\\PDSource.mp4"));
#endif
	}
#ifdef X86_PC
	//m_initParam.nSDKlogflag = 2;
	//m_initParam.nTempFileLocation = 1;
#endif
	//m_initParam.nTempFileLocation = 1;
	if (m_initParam.nSDKlogflag==0)
	{
		GetDefaultInitParam2(&m_initParam);
	}
	g_nLog = m_initParam.nSDKlogflag;
#if 0
	_tcscpy(m_initParam.mFilePath, _T("/sdcard/visualonLog_pd/PDSource.mp4"));
	m_initParam.nTempFileLocation = 1;
#endif
	m_initParam.nMaxBuffer = 128;
//	VOLOGE ("Proxy Name: %s", m_initParam.mProxyName);

	voLog(LL_DEBUG,"PDSource.txt","version=%s\nnBufferTime=%d\npath=%s\nProxyName=%s\nUserAgent=%s\nHTTPDataTimeOut=%d,nHttpProtocol=%d,nMaxBuffer=%d,PacketLength=%d,PlayBufferTime=%d,log=%d,nTempFileLocation=%d\n",
		version2,
		m_initParam.nBufferTime,
		m_initParam.mFilePath,
		m_initParam.mProxyName,
		m_initParam.mUserAgent,
		m_initParam.nHTTPDataTimeOut,
		m_initParam.nHttpProtocol,
		m_initParam.nMaxBuffer,
		m_initParam.nPacketLength,
		m_initParam.nPlayBufferTime,
		m_initParam.nSDKlogflag,
		m_initParam.nTempFileLocation,
		m_param.pSourceOP);
	//check if the time2Pos is supported
	IVOSocket::Init();
	//voLog_android("testLog","test");
	CREATE_MUTEX(m_csRead);
	UFileReader::g_bCancel = false;
}

CPDSessionWrapper2::~CPDSessionWrapper2()
{
	DumpLog("~CPDSessionWrapper2() start\r\n");
	Close();
	for (int i=0;i<m_sourceInfo.Tracks;i++)
	{
		SAFE_DELETE(m_tracks[i]);
	}
	VO_SAFE_DELETE(m_url);
	ReaderLoaderFactory::DestroyOneReaderLoader(m_dllLoader);
	IVOSocket::UnInit();
	DELETE_MUTEX(m_csRead);
	DumpLog("~CPDSessionWrapper2() end\r\n");
}
typedef struct
{
	TCHAR*					postFix;
	VOMediaType			type;
}TMediaTypeMap;
static  TMediaTypeMap sMediaTypeArray[]={
	//audio
	{_T(".mp3"),VOMediaTypeAudio},
	{_T(".mp31"),VOMediaTypeAudio},//for wmp mp3 PD
	{_T(".aac"),VOMediaTypeAudio},
	{_T(".amr"),VOMediaTypeAudio},
	{_T(".awb"),VOMediaTypeAudio},
	{_T(".wav"),VOMediaTypeAudio},
	{_T(".flac"),VOMediaTypeAudio},
	//mp4
	{_T(".mp4"),VOMediaTypeMP4},
	{_T(".3gp"),VOMediaTypeMP4},
	{_T(".3g2"),VOMediaTypeMP4},
	{_T(".mp4"),VOMediaTypeMP4},
	{_T(".m4a"),VOMediaTypeMP4},
	{_T(".m4v"),VOMediaTypeMP4},
	//wmv
	{_T(".wmv"),VOMediaTypeAsf},
	{_T(".wma"),VOMediaTypeAsf},
	{_T(".asf"),VOMediaTypeAsf},
	//RM
	{_T(".rm"),VOMediaTypeReal},
	{_T(".ra"),VOMediaTypeReal},
	{_T(".rv"),VOMediaTypeReal},
	//FLV
	{_T(".flv"),VOMediaTypeFLV},
};
typedef struct  
{
	TCHAR*	dllName;
	TCHAR*	apiName;
}TReaderAPI;
static TReaderAPI readerName[]={
	{	NULL,NULL},
	{	_T("voMP4FR"),	_T("voGetMP4ReadAPI")},
	{	_T("voAudioFR"),	_T("voGetAudioReadAPI")},
	{	_T("voRealFR"),		_T("voGetRealReadAPI")},
	{	_T("voASFFR"),		_T("voGetASFReadAPI")},
	{	_T("voFLVFR"),		_T("voGetFLVReadAPI")},
};
int	CPDSessionWrapper2::	CreateSourceByURL(TCHAR* szFile)
{

	CDownloader download;
	m_downLoadType=download.GetDownType(szFile);
	char* contentType=download.GetContentType();
	switch(m_downLoadType)
	{
	case VO_NETDOWN_TYPE_SHOUT_CAST:
		{

			m_nMediaType = VOMediaTypeAudio;
			char* str=NULL;
			if(contentType&&strstr(contentType,"audio/aac"))
			{
				//if(strstr(str,"aac"))
				UpdateTheTmpFileByURL(_T(".aac"));
			}
			else
				UpdateTheTmpFileByURL(_T(".mp3"));
		}


		return 1;
	case VO_NETDOWN_TYPE_WMS:
		m_nMediaType = VOMediaTypeAsf;
		UpdateTheTmpFileByURL(_T(".asf"));
		//m_initParam.nPacketLength=10;//force the packet length up to 10 KB
		return 1;
	case VO_NETDOWN_TYPE_FLV:
		m_nMediaType = VOMediaTypeFLV;
		UpdateTheTmpFileByURL(_T(".flv"));
		return 1;
	default:
		m_nMediaType=VOMediaTypeMP4;
	}

	return 0;
}

void CPDSessionWrapper2::SetMediaTypeByURL(TCHAR* url)
{

#define GOOGLE_TEMP 1
#if GOOGLE_TEMP
	if (_tcsstr(url,_T("googlevideo.com/")))
	{
		m_nMediaType = VOMediaTypeMP4;
		voLog(LL_DEBUG,"PDSource.txt","it is googleVideo\n");
		return;
	}
#endif
	if(m_nMediaType==VOMediaTypeUnknown)
	{
		TCHAR* postfix=_tcsrchr(url,_T('.')); 
		if(postfix)
		{
			int size=sizeof(sMediaTypeArray)/sizeof(TMediaTypeMap);
			for(int i=0;i<size;i++)
			{
				if(_tcsicmp(postfix,sMediaTypeArray[i].postFix)==0)
				{
					m_nMediaType = sMediaTypeArray[i].type;
					UpdateTheTmpFileByURL(url);
					return;
				}
			}
		}
	}
}
void CPDSessionWrapper2::	InitMediaTypeMap()
{
	
}
void	CPDSessionWrapper2::UpdateTheTmpFileByURL(const TCHAR* pExtName)
{
	if(pExtName)
	{
		TCHAR* postfix			=_tcsrchr(m_initParam.mFilePath,_T('.')); 
		TCHAR* pExtName2	=_tcsrchr((TCHAR*)pExtName,_T('.')); 
		if(postfix)
		{
			_tcscpy(postfix,pExtName2);
		}
	}

}
int		CPDSessionWrapper2::CreateReaderAPI(TCHAR* url)
{
	do 
	{
		SetMediaTypeByURL(url);
		if (m_nMediaType==VOMediaTypeUnknown )
		{
			CreateSourceByURL(url);
		}

		if (m_nMediaType!=VOMediaTypeUnknown )
		{
			TCHAR name[64]={0};
			_tcscat(name,readerName[m_nMediaType].dllName);
#ifdef LINUX
			_tcscat(name,_T(".so"));
#else//LINUX
			_tcscat(name,_T(".dll"));
#endif//LINUX
			voLog(LL_DEBUG,"PDSource.txt","reader is %s\n",name);
			if(m_dllLoader)
				ReaderLoaderFactory::DestroyOneReaderLoader(m_dllLoader);
			m_dllLoader = ReaderLoaderFactory::CreateOneReaderLoader(name,readerName[m_nMediaType].apiName,NULL);
			voReadAPI readerAPI=m_dllLoader->GetAPIEntry();
			if (readerAPI)
			{
				readerAPI((VO_PTR)&m_newReaderAPI,0);
			}
		}
		if (m_newReaderAPI.Open==NULL)
		{
			voLog(LL_ERROR,"error.txt","m_newReaderAPI.Open==NULL\n");
			break;
		}
		return 0;
	} while (0);
	return -1;
}
HRESULT CPDSessionWrapper2::OpenSource(TCHAR* pFile)
{
	voLog(LL_DEBUG,"PDSource.txt", "CPDSessionWrapper2::OpenSource\n");
	if(UFileReader::g_bCancel)
		return E_VO_USER_ABORT;
	if (CreateReaderAPI(pFile))
	{
		voLog(LL_ERROR,"error.txt","CreateReaderAPI fail\n");
		return -1;
	}
	
	int urlLen=_tcsclen(pFile);
	m_url = (TCHAR*)allocate(sizeof(TCHAR)*(urlLen+1),MEM_CHECK);// TCHAR[urlLen+1];
	_tcscpy(m_url,pFile);
	SPDInitParam* pInitParam = &m_initParam;

	VONETDOWNTYPE type=m_downLoadType;
	m_NetGet.SetDownLoadType(type);
	TCHAR* filetype=pInitParam->mFilePath;//m_szFileSource;


	
	m_NetGet.SetLoadState(true);
	m_NetGet.SetSinkType(pInitParam->nTempFileLocation);
	if(m_downLoadThreadPriority)
		m_NetGet.SetDownloadThreadPriority(m_downLoadThreadPriority);
	m_NetGet.SetUrl((LPCTSTR)pFile);
	
	CHttpDownloadFile::g_pNetGet = &m_NetGet;
	CHttpDownloadFile::g_dwOffset = 0;

	if(UFileReader::g_bCancel)
		return E_VO_USER_ABORT;

	DumpLog("CheckPDHeader\r\n");

	if(CheckPDHeader())
	{
		if(!m_NetGet.InitByPDHeader())
		{
			DumpLog("InitByPDHeader fail\r\n");
			return E_FAIL;
		}
	}
	else
	{
		DumpLog("m_NetGet.StartDownload start\r\n");

		 //Sleep( 2000 );
		//start http download file
		if(!m_NetGet.StartDownload())
		{
			DumpLog("m_NetGet.StartDownload fail end\r\n");
			if(m_NetGet.GetLastEventCode()==VO_PD_EVENT_GET_ASX)
				return VO_PD_EVENT_GET_ASX;
			else
				return E_NETWORK_ERROR;
		}

		DumpLog("m_NetGet.StartDownload ok end\r\n");
	}

	DWORD dwTimeRecord = voGetCurrentTime();

	if(UFileReader::g_bCancel)
		return E_VO_USER_ABORT;

	DumpLog("start parse file\r\n");
#if 0
	int seconds=20;
	do 
	{
		IVOThread::Sleep(1000);
	} while(seconds-->0);
#endif//
	VO_FILE_SOURCE tmpSrc={0};
	tmpSrc.nFlag			= VO_FILE_TYPE_NAME;
	tmpSrc.pSource	= m_initParam.mFilePath;
	tmpSrc.nMode		= VO_FILE_READ_ONLY;
	VO_SOURCE_OPENPARAM param={0};//TODO
	param.nFlag			= 0x102;//0x00020101;
	param.pSource		= &tmpSrc;//m_initParam.mFilePath;
	param.pSourceOP= &m_opFile;
	DWORD rc = m_newReaderAPI.Open(&m_hSource,&param);  //m_fPDOpen(&m_hSource, (char*)filetype, &m_opFile);
	if(UFileReader::g_bCancel)
	{
		DumpLog("after m_newReaderAPI.Open exit\r\n");
		return E_VO_USER_ABORT;
	}
	if(VORC_FILEREAD_OK != rc || !m_hSource)
	{
		Stop();
		voLog(LL_DEBUG,"error.txt","parse file fail. ret=%X\r\n",rc);
		return VFW_E_INVALID_FILE_FORMAT;
	}
	int testPos=1000;
	if(fileTime2Pos(this,2000,&testPos,false)==0)
		m_fPDMediaTime2FilePos=fileTime2Pos;
	else
		m_fPDMediaTime2FilePos=NULL;	
	DWORD dwFileHeaderSize = 0;
	
	PDGetParam(VO_PID_FILE_HEADSIZE, (LONG*)&dwFileHeaderSize);
	m_NetGet.SetFileHeaderSize(dwFileHeaderSize);
	m_newReaderAPI.GetSourceInfo(m_hSource,&m_sourceInfo);
	m_dwDuration = m_sourceInfo.Duration;
	if(m_sourceInfo.Tracks<0||m_sourceInfo.Tracks>=MAX_TRACK_COUNT)
	{
		voLog(LL_TRACE,"error.txt","m_sourceInfo.Tracks<0||m_sourceInfo.Tracks>=MAX_TRACK_COUNT\n");
		//break;
	}
	for (int i=0;i<m_sourceInfo.Tracks;i++)
	{
		m_tracks[i] = new(MEM_CHECK) CPDTrackWrapper2(this,i);
		m_tracks[i]->Init();
	}
	CDFInfo* pDfInfo = m_NetGet.GetDFInfo();
	CDFInfo2* pDfInfo2 = m_NetGet.GetDFInfo2();
	int fileSize=LIVE_SIZE;//default
	if(pDfInfo)
		fileSize=pDfInfo->GetFileSize();
	else if(pDfInfo2)
		fileSize=pDfInfo2->GetFileSize();

	if(fileSize!=LIVE_SIZE&&m_dwDuration>0)
	{
		if(pDfInfo)
			m_dwClipBitrate = LONGLONG(pDfInfo->GetFileSize() - dwFileHeaderSize) * 1000 / m_dwDuration;
		else if(pDfInfo2)
		{
			
			m_dwClipBitrate = LONGLONG(fileSize - dwFileHeaderSize) * 1000 / m_dwDuration;
			if(dwFileHeaderSize + LONGLONG(fileSize - dwFileHeaderSize) * MIN(m_dwDuration, pInitParam->nBufferTime * 1000 * 4) / m_dwDuration > pInitParam->nMaxBuffer * 1024)
			{
	#ifdef _HTC
				m_NetGet.NotifyEvent(HP_EVENT_INSUFFICIENT_SPACE, NULL);
	#else	//_HTC
				m_NetGet.NotifyEvent(VO_PD_EVENT_INSUFFICIENT_SPACE, NULL);
	#endif	//_HTC
				DumpLog("nMaxBuffer is too small!\r\n");
				return E_SMALL_BUFFER;
			}
		}
	}
	else
	{
		m_dwDuration	= MAXLONG;
		m_dwClipBitrate = 64*1024;
		voLog(LL_DEBUG,"PDSource.txt","this is a live stream\n");
	}
	//decide if select this clip, if not select this clip, it will return E_NOT_ENOUGH_BANDWIDTH
	//TO_DO:remove it
	AUTOSELECTCALLBACK pfAutoSelectCallback = NULL;//m_pSourceFilter->GetAutoSelectCallbackFunc();
	if(pfAutoSelectCallback && !m_NetGet.IsFinish())
	{
		//wait at least 1 second to compute throughput!!
		while(voGetCurrentTime() - dwTimeRecord < 1000 && !UFileReader::g_bCancel && !m_NetGet.IsFinish())
			IVOThread::Sleep(10);

		DWORD dwBytesPerSec = 0;
		while(!m_NetGet.GetParam(VOID_NETDOWN_BYTES_PER_SEC, (long*)&dwBytesPerSec) && !m_NetGet.IsFinish())
		{
			if(UFileReader::g_bCancel)
				return E_VO_USER_ABORT;

			DumpLog("download still not start, please wait!\r\n");
			IVOThread::Sleep(10);
		}

		if(!pfAutoSelectCallback(m_dwClipBitrate, dwBytesPerSec))
		{
			m_bNoWritePDHeader = true;
			return E_NOT_ENOUGH_BANDWIDTH;
		}
	}
	m_NetGet.SetLoadState(false);

	
	DumpLog("CPDSessionWrapper2::OpenSource ok end\r\n");
	if(UFileReader::g_bCancel)
	{
		DumpLog("after OpenSource ok exit\r\n");
		return E_VO_USER_ABORT;
	}
	Start();
	return S_OK;
}


bool CPDSessionWrapper2::Close()
{
	DumpLog("CPDSessionWrapper2::Close()\r\n");
	Stop();
	do 
	{
		IVOThread::Sleep(10);
		DumpLog("close: wait for loading url done");
	} while (m_isLoading);
	
	m_NetGet.DestroyBGThread();
	DumpLog("m_NetGet.DestroyBGThread()\r\n");

	m_NetGet.StopDownload();
	DumpLog("m_NetGet.StopDownload()\r\n");

	m_NetGet.CloseSink();
	DumpLog("m_NetGet.CloseSink()\r\n");

	if(!m_hSource )
		return true;
	DumpLog("m_fTrackClose2\r\n");

	//m_fPDClose(m_hSource);
	m_newReaderAPI.Close(m_hSource);
	m_hSource = NULL;
	DumpLog("m_fPDClose(m_hSource)\r\n");

	WritePDHeader();
	DumpLog("WritePDHeader()\r\n");

	return true;
}

bool CPDSessionWrapper2::Stop()
{

	UFileReader::g_bCancel = true;
	return true;
}

bool CPDSessionWrapper2::Pause()
{
	return true;
}

bool CPDSessionWrapper2::Run()
{
	
	return true;
}

bool CPDSessionWrapper2::Start()
{
	if(m_fPDMediaTime2FilePos)
	{
		voLog(LL_DEBUG,"PDSource.txt","CPDSessionWrapper2::Start()\n");
		DWORD dwCurrBufferingTime = GetCurrBufferingTime();
		int nStart = 0, nEnd = 0;
		if(VORC_FILEREAD_OK == m_fPDMediaTime2FilePos(this, 0, &nStart, true) && 
			VORC_FILEREAD_OK == m_fPDMediaTime2FilePos(this, dwCurrBufferingTime, &nEnd, false))
			m_NetGet.StartBuffering(nStart, nEnd, true, BT_SEEK, m_NetGet.GetCurrDownloadPos());
	}
	return true;
}
VO_U32 CPDSessionWrapper2::GetSample (VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	CAutoLock lock(m_csRead);
	//if(m_newPos!=-1)
	//	return VO_ERR_SOURCE_NEEDRETRY;
	m_currentTrackType=m_tracks[nTrack]->IsVideo()?TT_VIDEO:TT_AUDIO;
	int ret=m_newReaderAPI.GetSample(m_hSource,nTrack,pSample);
	
	
	if(ret==0)
	{
		m_tracks[nTrack]->SetCurTime(pSample->Time);
	}
	else if (ret==VO_ERR_SOURCE_NEEDRETRY)
	{
		DumpLog("CPDSessionWrapper2::GetSample DataLack\r\n");
		OnDataLack();
	}
	if (m_initParam.nSDKlogflag)
	{
		if (m_currentTrackType==TT_VIDEO)
		{
			voLog(LL_TRACE,"videoTS.txt","ts=%d,ret=%x\n",(long)pSample->Time,ret);
			if (0)//pSample->Time>5000)
			{
				//m_NetGet.NotifyEvent(VO_EVENT_BUFFERING_BEGIN,0);
			}
		}
		else
		{
			voLog(LL_TRACE,"audioTS.txt","ts=%d,ret=%x\n",(long)pSample->Time,ret);
		}
	}
	return ret;
}
bool CPDSessionWrapper2::SetPos(long& start)
{
	voLog(LL_DEBUG,"PDSource.txt","seek %d\n",start);
	CAutoLock lock(m_csRead);
	if(UFileReader::g_bCancel)
	{
		DumpLog("[CPDSessionWrapper2::SetPos g_bCancel,return]\r\n");
		return false;
	}
	DumpLog("[CPDSessionWrapper2::SetPos]\r\n");

	if(!m_NetGet.IsSupportSeek() || !m_fPDMediaTime2FilePos||(UFileReader::g_nFlag&VOFR_DISABLE_SEEK)==VOFR_DISABLE_SEEK)
	{
		voLog(LL_DEBUG,"PDSource.txt","DISABLE_SEEKs\n");
		m_NetGet.NotifyEvent(VO_EVENT_DISABLE_SCAN,start);
		return false;
	}
	//if download is finished
	//get file position according media time
	int nStart = 0;
	//start+=500;
	VO_S64  earlyPos=start;
	for (int i=0;i<m_sourceInfo.Tracks;i++)
	{
		VO_S64 start64= start; 
		m_newReaderAPI.SetPos(m_hSource,i,&start64);
		if(earlyPos>start64)
			earlyPos=start64;
		voLog(LL_TRACE,"seek.txt","seek ori=%d,actual pos=%d\n",start,start64);
	}
	start = earlyPos;
	if(start<0)
		start=0;
	if(VORC_FILEREAD_OK != m_fPDMediaTime2FilePos(this, start, &nStart , true))
		return false;
	DWORD dwCurrBufferingTime = GetCurrBufferingTime();
	int nEnd = 0;
	if(VORC_FILEREAD_OK != m_fPDMediaTime2FilePos(this, start+ dwCurrBufferingTime, &nEnd, false))
		return false;
	//int curPos=m_NetGet.GetCurrDownloadPos();
    //nStart=max(curPos,nStart);
#define DEFAULT_MIN_BUFSIZE 1024*128
	if (nEnd==nStart)//it is possible in FLV
	{
		voLog(LL_TRACE,"seek.txt","nEnd==nStart,add the default size\n");
		nEnd+=DEFAULT_MIN_BUFSIZE;
	}
	voLog(LL_TRACE,"seek.txt","start=%d,pos(%d-%d)\n",start,nStart,nEnd);
	
	if(!m_NetGet.SetDownloadPos(nStart))
	{
		if(m_NetGet.IsFinish())
		{
			if(PDGlobalData::data.version==1)
			{
				m_NetGet.NotifyEvent(VO_EVENT_BUFFERING_BEGIN,0);
				m_NetGet.NotifyEvent(VO_EVENT_BUFFERING_END,0);
			}
			return true;
		}
	}
	m_newPos = start;
	m_NetGet.StartBuffering(nStart, nEnd, false, BT_SEEK, -1);
	
	return true;
}
void CPDSessionWrapper2::NotifyEvent(int id,void* param)
{
	if (id==VO_EVENT_BUFFERING_END&&m_newPos!=-1)
	{
		voLog(LL_TRACE,"seek.txt","seek_buffering_end,pos=%d\n",m_newPos);
		//SetReaderPos();
		m_newPos = -1;
	}
}
DWORD CPDSessionWrapper2::PDSetParam(LONG nID, void* lValue)
{
	if(VOID_FILEREAD_CALLBACK_NOTIFY == nID)
	{
		m_NetGet.SetParam(nID, (long)lValue);
		return VORC_OK;
	}
	switch(nID)
	{
	case VOID_STREAMING_SDK_VERSION:
		{
			PDGlobalData::data.version = *(long*)lValue;
			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_STOP:
		{

			if(!Stop())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_START:
		{

			if(!Start())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_PAUSE:
		{

			if(!Pause())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_RUN:
		{
			if(!Run())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_SEEK:
		{
			long pos=*(long*)lValue;
			if(!SetPos(pos))
				return VORC_FILEREAD_NEED_RETRY;
  		    return VORC_FILEREAD_OK;
		}
	case VOID_STREAMING_INIT_PARAM:
		{
			PVOPDInitParam param=(PVOPDInitParam)lValue;
			memcpy(&m_initParam,param,sizeof(VOPDInitParam));
#ifdef WIN32
			m_initParam.mProxyName[0]='\0';
#endif
			return VORC_FILEREAD_OK;
		}
	case VOID_STREAMING_AUTO_EXTEND_BUFTIME:
		{
			m_nAutoExtendBufTime=*(long*)lValue;
			return VORC_FILEREAD_OK;
		}
	}
#ifdef _HTC
	if(ID_HTC_PD_PAUSE_DOWNLOAD == nID)
#else	//_HTC
	if(VOID_STREAMING_PAUSE_DOWNLOAD == nID)
#endif	//_HTC
	{
		bool bPauseDownload = *(long*)lValue;
		if(bPauseDownload)
			DumpLog("ID_HTC_PD_PAUSE_DOWNLOAD, pause.\r\n");
		else
			DumpLog("ID_HTC_PD_PAUSE_DOWNLOAD, continue.\r\n");

		m_NetGet.Pause(bPauseDownload);
		return VORC_OK;
	}
	return m_newReaderAPI.SetSourceParam(m_hSource,nID,lValue);
		//m_fPDSetParam(m_hSource, nID, lValue);
}

DWORD CPDSessionWrapper2::PDGetParam(LONG nID, void* plValue)
{
	if(!m_hSource)
		return (DWORD)VORC_FAIL;
	
	if(VOID_FILEREAD_BUFFERING_PROGRESS == nID)
	{
		*(long*)plValue = m_NetGet.GetBufferingPercent();
		return VORC_OK;
	}

#ifdef _HTC
	if(ID_HTC_STREAMING_INFO == nID)
	{
		HS_PDStreamingInfo* pInfo = (HS_PDStreamingInfo*)plValue;
		pInfo->clip_type = PROGRESSIVE;

		SPDInitParam* pInitParam = GetInitParam();
		WideCharToMultiByte(CP_ACP, 0, pInitParam->mFilePath, -1, pInfo->clip_title, MAX_PATH, NULL, NULL);

		pInfo->clip_duration = m_dwDuration / 1000;
		pInfo->clip_bitrate = m_dwClipBitrate;
		pInfo->clip_SupportPAUSE = 1;
		if(m_NetGet.IsFinish())
			pInfo->clip_SupportDirectSeek = 1;
		else
			pInfo->clip_SupportDirectSeek = m_NetGet.IsSupportSeek() ? 1 : 0;
		for (int i=0;i<m_sourceInfo.Tracks;i++)
		{
			CPDTrackWrapper2* track=m_tracks[i];
			VO_SOURCE_TRACKINFO trackInfo;
			track->GetTrackInfo(&trackInfo);
			if (trackInfo.Type==VO_SOURCE_TT_VIDEO)
			{
				switch(trackInfo.Codec)
				{
				case VO_VIDEO_CodingH264:
					pInfo->codecType[0] = kCodecTypeH264BL;
					break;
				case VO_VIDEO_CodingH263:
					pInfo->codecType[0] = kCodecTypeH263BL;
					break;
				case VO_VIDEO_CodingMPEG4:
					pInfo->codecType[0] = kCodecTypeMPEG4SP;
					break;
				case VO_VIDEO_CodingWMV:
					break;
				case VO_VIDEO_CodingRV:
					break;
				default:
					pInfo->codecType[1] = kCodecTypeUnknown;
					break;
				}
				//TODO
				//pInfo->clip_width = trackInfo.
			} 
			else if(trackInfo.Type==VO_SOURCE_TT_AUDIO)
			{
				switch(trackInfo.Codec)
				{
				case VO_AUDIO_CodingAAC:
					pInfo->codecType[1] = kCodecTypeMPEG4AAC;
					break;
				case VO_AUDIO_CodingAMRNB:
					pInfo->codecType[1] = kCodecTypeGSMAMRNB;
					break;
				case VO_AUDIO_CodingAMRWB:
					pInfo->codecType[1] = kCodecTypeGSMAMRWB;
					break;
				case VO_AUDIO_CodingAMRWBP:
					break;
				case VO_AUDIO_CodingQCELP13:
					pInfo->codecType[1] = kCodecTypeQCELP;
					break;
				case VO_AUDIO_CodingMP3:
					break;
				case VO_AUDIO_CodingEVRC:
					pInfo->codecType[1] = kCodecTypeEVRC;
					break;
				case VO_AUDIO_CodingRA:
					break;
				default:
					pInfo->codecType[1] = kCodecTypeUnknown;
					break;
				}
			}
		}
	
		CDFInfo* pDfInfo = m_NetGet.GetDFInfo();
		CDFInfo2* pDfInfo2 = m_NetGet.GetDFInfo2();
		if(pDfInfo)
			pInfo->clip_file_length = pDfInfo->GetFileSize();
		else if(pDfInfo2)
			pInfo->clip_file_length = pDfInfo2->GetFileSize();

		return VORC_OK;
	}
	else if(ID_HTC_PD_THROUGHPUT == nID)
	{
		if(m_NetGet.GetParam(VOID_NETDOWN_BYTES_PER_SEC, (long*)plValue))
			return VORC_OK;
		else
			return VORC_NETDOWN_UNKNOWN_ERROR;
	}
#else	//_HTC
	if(VOID_STREAMING_INFO == nID)
	{
		VOStreamingInfo* pInfo = (VOStreamingInfo*)plValue;
		pInfo->clipType = ST_PROGRESSIVE;

		SPDInitParam* pInitParam = GetInitParam();
		//WideCharToMultiByte(CP_ACP, 0, pInitParam->mFilePath, -1, pInfo->clip_title, MAX_PATH, NULL, NULL);
		#ifdef UNICODE
			wcstombs(pInfo->clip_title,pInitParam->mFilePath,MAX_PATH);
		#else//UNICODE
			strcpy(pInfo->clip_title,pInitParam->mFilePath);
		#endif//UNICDOE
		pInfo->clip_bitrate = 0;
		pInfo->clip_SupportPAUSE = 1;
		for (int i=0;i<m_sourceInfo.Tracks;i++)
		{
			CPDTrackWrapper2* track=m_tracks[i];
			VO_SOURCE_TRACKINFO trackInfo;
			track->GetTrackInfo(&trackInfo);
			if (trackInfo.Type==VO_SOURCE_TT_VIDEO)
			{
				switch(trackInfo.Codec)
				{
				case VO_VIDEO_CodingH264:
					pInfo->codecType[0] = VOCodecTypeH264BL;
					break;
				case VO_VIDEO_CodingH263:
					pInfo->codecType[0] = VOCodecTypeH263BL;
					break;
				case VO_VIDEO_CodingMPEG4:
					pInfo->codecType[0] = VOCodecTypeMPEG4SP;
				    break;
				case VO_VIDEO_CodingWMV:
				    break;
				case VO_VIDEO_CodingRV:
					break;
				default:
					pInfo->codecType[1] = VOCodecTypeUnknown;
					break;
				}
				//TODO
				//pInfo->clip_width = trackInfo.
			} 
			else if(trackInfo.Type==VO_SOURCE_TT_AUDIO)
			{
					switch(trackInfo.Codec)
					{
					case VO_AUDIO_CodingAAC:
						pInfo->codecType[1] = VOCodecTypeMPEG4AAC;
						break;
					case VO_AUDIO_CodingAMRNB:
						pInfo->codecType[1] = VOCodecTypeGSMAMRNB;
						break;
					case VO_AUDIO_CodingAMRWB:
						pInfo->codecType[1] = VOCodecTypeGSMAMRWB;
					    break;
					case VO_AUDIO_CodingAMRWBP:
					    break;
					case VO_AUDIO_CodingQCELP13:
						pInfo->codecType[1] = VOCodecTypeQCELP;
						break;
					case VO_AUDIO_CodingMP3:
						break;
					case VO_AUDIO_CodingEVRC:
						pInfo->codecType[1] = VOCodecTypeEVRC;
					    break;
					case VO_AUDIO_CodingRA:
					    break;
					default:
						pInfo->codecType[1] = VOCodecTypeUnknown;
						break;
					}
			}
		}
		return VORC_OK;
	}
#endif	//_HTC
#ifdef _HTC
	else if(ID_HTC_STREAMING_BUFFERING_PROGRESS == nID)
#else	//_HTC
	else if(VOID_STREAMING_BUFFERING_PROGRESS == nID)
#endif	//_HTC
	{
		*(long*)plValue = m_NetGet.GetBufferingPercent();
		return VORC_OK;
	}
#ifdef _HTC
	else if(ID_HTC_PD_DOWNLOADING_TIME == nID)
#else	//_HTC
	else if(VOID_STREAMING_DOWNLOADING_TIME == nID)
#endif	//_HTC
	{

		*(long*)plValue = m_NetGet.GetCurrDownloadPercent();	
		return VORC_OK;
	}
	return m_newReaderAPI.GetSourceParam(m_hSource,nID,plValue);
	//return m_fPDGetParam(m_hSource, nID, plValue);
}
void CPDSessionWrapper2::OnDataLack()
{
	if(m_NetGet.IsBuffering() || !m_fPDMediaTime2FilePos)
		return;
#if 1
	//get current media time

	int		 llMediaTime = 0;
	int		 llTmp = 0;
	for (int i=0;i<m_sourceInfo.Tracks;i++)
	{
		llTmp=m_tracks[i]->GetCurTime();
		if(llTmp > llMediaTime)
			llMediaTime = llTmp;
	}
	
#define EXTEND_BUFFERING_TIME 3000
#define MAX_BUFFERING_TIME 10000
	DWORD dwCurrBufferingTime = GetCurrBufferingTime();
	if (m_nAutoExtendBufTime)
	{
		dwCurrBufferingTime+=EXTEND_BUFFERING_TIME;
		if (dwCurrBufferingTime>MAX_BUFFERING_TIME)
		{
			dwCurrBufferingTime=MAX_BUFFERING_TIME;
		}
	}

	voLog(LL_DEBUG,"PDSource.txt","data lack: auto=%d,extend buffering time=%d\n",m_nAutoExtendBufTime,dwCurrBufferingTime);
	int nMediaTime = int(llMediaTime);
	int nEnd = 0;
	if(VORC_FILEREAD_OK != m_fPDMediaTime2FilePos(this, nMediaTime + dwCurrBufferingTime, &nEnd, false))
		return;

	int nStart = m_NetGet.GetCurrDownloadPos();
	//old: when data lack reach RESERVE_BUFFER_SIZE, we do buffering!!
	//now: once data lack, we do buffering!!
	if(nEnd > nStart/* + RESERVE_BUFFER_SIZE*/)
	{
		voLog(LL_TRACE,"buffering.txt", "[OnDataLack]nMediaTime: %d, nStart: %d, nEnd: %d\r\n", nMediaTime, nStart, nEnd);
		m_NetGet.StartBuffering(nStart, nEnd, false, BT_PLAY, -1);	
	}
	else
	{
		voLog(LL_TRACE,"error.txt","start=%d,nEnd (%d)< nStart(%d)\n",nMediaTime,nEnd,nStart);
	}
#endif//
}

bool CPDSessionWrapper2::CheckPDHeader()
{
	SPDInitParam* pInitParam = GetInitParam();
	if(0 == pInitParam->nTempFileLocation)	//storage to memory
		return false;
#ifdef UNICODE
	wcstombs(m_fileName,pInitParam->mFilePath,MAX_FNAME_LEN);
#else
	tcscpy(m_fileName,pInitParam->mFilePath);
#endif
	//FILE* hFile = CreateFile(pInitParam->mFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
	//	OPEN_EXISTING, 0, NULL);
	FILE* hFile = fopen(m_fileName,"a");
	if(NULL== hFile)
		return false;

	PDHChunk pc;
	DWORD dwReaded = 0;
	//if(!ReadFile(hFile, &pc, PD_Header_Chunk_Size, &dwReaded, NULL) || dwReaded < PD_Header_Chunk_Size || FOURCC_PD_Header != pc.fcc)
	dwReaded=fread(&pc,PD_Header_Chunk_Size,1,hFile);
	if(dwReaded < PD_Header_Chunk_Size || FOURCC_PD_Header != pc.fcc)
	{
		//CloseHandle(hFile);
		fclose(hFile);
		return false;
	}

	PBYTE pPDHeader = (PBYTE)allocate(pc.size,MEM_CHECK);//new BYTE[pc.size];
	if(!pPDHeader)
		return false;
	memcpy(pPDHeader, &pc, PD_Header_Chunk_Size);
	dwReaded=fread(pPDHeader + PD_Header_Chunk_Size,pc.size - PD_Header_Chunk_Size,1,hFile);
	//if(!ReadFile(hFile, pPDHeader + PD_Header_Chunk_Size, pc.size - PD_Header_Chunk_Size, &dwReaded, NULL) || dwReaded < pc.size - PD_Header_Chunk_Size)
	if(dwReaded < pc.size - PD_Header_Chunk_Size)
	{
		VO_SAFE_DELETE(pPDHeader);//delete [] pPDHeader;
		fclose(hFile);
		return false;
	}

	bool bRet = CheckBuffer(pPDHeader, pc.size);
	if(!bRet)
	{
		memset(pPDHeader, 0, pc.size);
		//SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		fseek(hFile,0,SEEK_SET);
		//WriteFile(hFile, pPDHeader, pc.size, &dwReaded, NULL);
		fwrite(pPDHeader,pc.size,1,hFile);
	}

	VO_SAFE_DELETE(pPDHeader);//delete [] pPDHeader;
	fclose(hFile);
	return bRet;
}

DWORD CPDSessionWrapper2::GetPDHeaderRemainSize(bool bDF)
{
	if(m_bNoWritePDHeader)
		return 0;
	DWORD dwHeaderSize = PD_Header_Chunk_Size;
	//source file url
	dwHeaderSize += (PD_Header_Chunk_Size + _tcsclen(m_url) );

	//download time
	dwHeaderSize += (PD_Header_Chunk_Size + sizeof(SYSTEMTIME));

	//source file length
	dwHeaderSize += (PD_Header_Chunk_Size + sizeof(DWORD));

	//file mapping
	if(bDF)
		dwHeaderSize += (PD_Header_Chunk_Size + sizeof(DWORD) + VO_REMAIN_FILE_FRAGMENT_COUNT * sizeof(PDHFileFragment));
	else
		dwHeaderSize += (PD_Header_Chunk_Size + sizeof(PDHFileMapping2));

	return dwHeaderSize;
}

bool CPDSessionWrapper2::WritePDHeader()
{
	if(m_bNoWritePDHeader)
		return true;
	DumpLog("CPDSessionWrapper2::WritePDHeader()\r\n");

	SPDInitParam* pInitParam = GetInitParam();
	if(0 == pInitParam->nTempFileLocation)
		return true;

	//compute size
	DWORD dwHeaderSize = PD_Header_Chunk_Size;
	//source file url
	dwHeaderSize += (PD_Header_Chunk_Size + _tcsclen(m_url) );

	//download time
	dwHeaderSize += (PD_Header_Chunk_Size + sizeof(SYSTEMTIME));

	//source file length
	dwHeaderSize += (PD_Header_Chunk_Size + sizeof(DWORD));

	//file mapping
	DWORD dwFragNum = 0;
	if(m_NetGet.GetDFInfo())
	{
		CDFInfo* pDFInfo = m_NetGet.GetDFInfo();
		dwFragNum = pDFInfo->GetFragmentNum();
		dwHeaderSize += (PD_Header_Chunk_Size + sizeof(DWORD) + dwFragNum * sizeof(PDHFileFragment));
		DWORD dwPDHeaderRemainSize = GetPDHeaderRemainSize(true);
		if(dwHeaderSize < dwPDHeaderRemainSize)
			dwHeaderSize = dwPDHeaderRemainSize;
	}
	else if(m_NetGet.GetDFInfo2())
		dwHeaderSize += (PD_Header_Chunk_Size + sizeof(PDHFileMapping2));

	PBYTE pPDHeader = (PBYTE)allocate(dwHeaderSize,MEM_CHECK);//new BYTE[dwHeaderSize];
	if(!pPDHeader)
		return false;
	memset(pPDHeader, 0, dwHeaderSize);

	PDHChunk pc;
	//generate PD header information
	//PD Header
	PBYTE pTmp = pPDHeader;
	pc.fcc = FOURCC_PD_Header;
	pc.size = dwHeaderSize;
	memcpy(pTmp, &pc, PD_Header_Chunk_Size);
	pTmp += PD_Header_Chunk_Size;

	//source file url
	pc.fcc = FOURCC_PD_Source_File_Url;
	pc.size = PD_Header_Chunk_Size + _tcsclen(m_url);
	memcpy(pTmp, &pc, PD_Header_Chunk_Size);
	pTmp += PD_Header_Chunk_Size;
	memcpy(pTmp, m_url, _tcsclen(m_url));
	pTmp +=_tcsclen(m_url);

#if 1//
	//download time
	int downLoadTime=voGetCurrentTime();
	int  timeSize=sizeof(long);//strlen(timeBuf);
	pc.fcc = FOURCC_PD_Download_Time;
	pc.size = PD_Header_Chunk_Size +timeSize ;
	memcpy(pTmp, &pc, PD_Header_Chunk_Size);
	pTmp += PD_Header_Chunk_Size;
	//SYSTEMTIME st;
	//GetSystemTime(&st);
	//memcpy(pTmp, &st, sizeof(SYSTEMTIME));
	//pTmp += sizeof(SYSTEMTIME);
	memcpy(pTmp,&downLoadTime,timeSize);
	pTmp+=timeSize;
#endif
	if(m_NetGet.GetDFInfo())
	{
		CDFInfo* pDFInfo = m_NetGet.GetDFInfo();
		//source file length
		pc.fcc = FOURCC_PD_Source_File_Length;
		pc.size = PD_Header_Chunk_Size + sizeof(DWORD);
		memcpy(pTmp, &pc, PD_Header_Chunk_Size);
		pTmp += PD_Header_Chunk_Size;
		DWORD dwFileSize = pDFInfo->GetFileSize();
		memcpy(pTmp, &dwFileSize, sizeof(DWORD));
		pTmp += sizeof(DWORD);

		//file mapping
		pc.fcc = FOURCC_PD_DFInfo;
		pc.size = PD_Header_Chunk_Size + sizeof(DWORD) + dwFragNum * sizeof(PDHFileFragment);
		memcpy(pTmp, &pc, PD_Header_Chunk_Size);
		pTmp += PD_Header_Chunk_Size;
		memcpy(pTmp, &dwFragNum, sizeof(DWORD));
		pTmp += sizeof(DWORD);

		pDFInfo->WriteOut(pTmp);
	}
	else if(m_NetGet.GetDFInfo2())
	{
		CDFInfo2* pDFInfo2 = m_NetGet.GetDFInfo2();
		//source file length
		pc.fcc = FOURCC_PD_Source_File_Length;
		pc.size = PD_Header_Chunk_Size + sizeof(DWORD);
		memcpy(pTmp, &pc, PD_Header_Chunk_Size);
		pTmp += PD_Header_Chunk_Size;
		DWORD dwFileSize = pDFInfo2->GetFileSize();
		memcpy(pTmp, &dwFileSize, sizeof(DWORD));
		pTmp += sizeof(DWORD);

		//file mapping
		pc.fcc = FOURCC_PD_DFInfo2;
		pc.size = PD_Header_Chunk_Size + sizeof(PDHFileMapping2);
		memcpy(pTmp, &pc, PD_Header_Chunk_Size);
		pTmp += PD_Header_Chunk_Size;

		pDFInfo2->WriteOut(pTmp);
	}

	//write to file header
	bool bRet = WriteBuffer(pPDHeader, dwHeaderSize, m_NetGet.GetPDHeaderSize());
	VO_SAFE_DELETE(pPDHeader);
	return bRet;
}

bool CPDSessionWrapper2::CheckBuffer(PBYTE pBuffer, DWORD dwSize)
{
	PDHChunk pc;
	//PD Header
	PBYTE pTmp = pBuffer;
	memcpy(&pc, pTmp, PD_Header_Chunk_Size);
	pTmp += PD_Header_Chunk_Size;
	if(FOURCC_PD_Header != pc.fcc || dwSize != pc.size)
		return false;

	DWORD dwPDHeaderSize = pc.size;
	DWORD dwFileSize = 0;
	DWORD dwFragNum = 0;
	bool bExit = false;
	while(pTmp - pBuffer < dwSize && !bExit)
	{
		memcpy(&pc, pTmp, PD_Header_Chunk_Size);
		pTmp += PD_Header_Chunk_Size;

		switch(pc.fcc)
		{
		case FOURCC_PD_Source_File_Url:			//source file url
			{
				TCHAR* pszFileSource = (TCHAR*)allocate((pc.size - PD_Header_Chunk_Size) / sizeof(TCHAR) + 1,MEM_CHECK);//new TCHAR[(pc.size - PD_Header_Chunk_Size) / sizeof(TCHAR) + 1];
				if(!pszFileSource)
					return false;
				memcpy(pszFileSource, pTmp, pc.size - PD_Header_Chunk_Size);
				pszFileSource[(pc.size - PD_Header_Chunk_Size) / sizeof(TCHAR)] = _T('\0');
				//TO_DO
				if(_tcscmp(m_url, pszFileSource))	//not this url
				{
					VO_SAFE_DELETE(pszFileSource);
					return false;
				}
				VO_SAFE_DELETE(pszFileSource);
		
			}
			break;

		case FOURCC_PD_Download_Time:			//download time
			{
				
				long stFile;
				memcpy(&stFile, pTmp, sizeof(long));
				long st=voGetCurrentTime();
				const long dayms=24*60*60*1000;
				int nDay = (st - stFile+dayms/2) / dayms;
				if(nDay > 30)
					return false;
				return true;

			}
			break;

		case FOURCC_PD_Source_File_Length:		//source file length
			{
				memcpy(&dwFileSize, pTmp, sizeof(DWORD));
				SPDInitParam* pInitParam = GetInitParam();
				if(dwFileSize + dwSize > pInitParam->nMaxBuffer * 1024)
					return false;
			}
		    break;

		case FOURCC_PD_DFInfo:					//file mapping 1
			{
				memcpy(&dwFragNum, pTmp, sizeof(DWORD));

				m_NetGet.ReadInDFInfo(pTmp + sizeof(DWORD), dwFragNum);
				CDFInfo* pDFInfo = m_NetGet.GetDFInfo();
				pDFInfo->SetFileSize(dwFileSize);
			}
		    break;

		case FOURCC_PD_DFInfo2:					//file mapping 2
			{
				return false;
			}
			break;

		default:
			bExit = true;
		    break;
		}

		if(!bExit)
			pTmp += (pc.size - PD_Header_Chunk_Size);
	}

	//connect server will waste much time, so we do not check it;
	//we can check it after start download in the future.
//	if(dwFileSize != m_NetGet.GetSourceFileLenWithoutDownload())
//		return false;
	if(m_bNoWritePDHeader==false)
		m_NetGet.SetPDHeaderSize(dwPDHeaderSize);
	if(dwFragNum == 1)
	{
		CDFInfo* pDFInfo = m_NetGet.GetDFInfo();
		if(pDFInfo)
		{
			DWORD dwStart = 0, dwLen = 0;
			if(!pDFInfo->GetBlankFragment(dwStart, dwLen))
				m_bNoWritePDHeader = true;
		}
	}
	return true;
}

bool CPDSessionWrapper2::WriteBuffer(PBYTE pBuffer, DWORD dwSize, DWORD dwFileStartCopy /* = 0 */)
{
	SPDInitParam* pInitParam = GetInitParam();
	///HANDLE hFileRead = CreateFile(pInitParam->mFilePath, GENERIC_READ, FILE_SHARE_WRITE, NULL, 
	//	OPEN_ALWAYS, 0, NULL);
#ifdef UNICODE
	wcstombs(m_fileName,pInitParam->mFilePath,MAX_FNAME_LEN);
#else
	tcscpy(m_fileName,pInitParam->mFilePath);
#endif
	FILE* hFileRead = fopen(m_fileName,"rb");
	if(NULL == hFileRead)
		return false;

	//HANDLE hFileWrite = CreateFile(pInitParam->mFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
	//	OPEN_ALWAYS, 0, NULL);
	FILE* hFileWrite = fopen(m_fileName,"wb");
	if(NULL == hFileWrite)
	{
		fclose(hFileRead);
		return false;
	}

	DWORD dwTmpFileBuffer = MAX(dwSize, 65536);
	PBYTE pTmpFileBuffer = (PBYTE)allocate(dwTmpFileBuffer,MEM_CHECK);// new BYTE[dwTmpFileBuffer];
	if(!pTmpFileBuffer)
	{
		fclose(hFileRead);
		fclose(hFileWrite);
		return false;
	}

	DWORD dwReaded = 0, dwWrited = 0;
	if(dwFileStartCopy >= dwSize)
	{
		//WriteFile(hFileWrite, pBuffer, dwSize, &dwWrited, NULL);
		dwWrited=fwrite(pBuffer,dwSize,1,hFileWrite);

/*		if(dwFileStartCopy > dwSize)
		{
			//Ë³Ðòcopy
			SetFilePointer(hFileRead, dwFileStartCopy, NULL, FILE_BEGIN);
			while(true)
			{
				if(!ReadFile(hFileRead, pTmpFileBuffer, dwTmpFileBuffer, &dwReaded, NULL))
					break;

				WriteFile(hFileWrite, pTmpFileBuffer, dwReaded, &dwWrited, NULL);

				if(dwReaded < dwTmpFileBuffer)
					break;
			}
			SetEndOfFile(hFileWrite);
		}*/
	}
	else
	{
		DWORD dwOffset = dwSize - dwFileStartCopy;

		DWORD dwFileLen = ftell(hFileRead);//GetFileSize(hFileRead, NULL);
		//SetFilePointer(hFileWrite, dwFileLen + dwOffset, NULL, FILE_BEGIN);
		fseek(hFileRead,dwFileLen,SEEK_SET);
		//SetEndOfFile(hFileWrite);
		fseek(hFileWrite,0,SEEK_END);
		//ÄæÐòcopy
		DWORD dwLeft = dwFileLen - dwFileStartCopy;
		DWORD dwToProcess = 0;
		DWORD dwLastFilePos = dwFileLen;
		while(dwLeft > 0)
		{
			dwToProcess = MIN(dwLeft, dwTmpFileBuffer);

			//SetFilePointer(hFileRead, dwLastFilePos - dwToProcess, NULL, FILE_BEGIN);
			fseek(hFileRead,dwLastFilePos - dwToProcess,SEEK_SET);
			dwReaded=fread(pTmpFileBuffer, dwToProcess,1,hFileRead);
			//if(!ReadFile(hFileRead, pTmpFileBuffer, dwToProcess, &dwReaded, NULL))
			if(ferror(hFileRead))
				break;

			//SetFilePointer(hFileWrite, dwOffset + dwLastFilePos - dwToProcess, NULL, FILE_BEGIN);
			fseek(hFileWrite,dwOffset + dwLastFilePos - dwToProcess,SEEK_SET);
			//WriteFile(hFileWrite, pTmpFileBuffer, dwReaded, &dwWrited, NULL);
			dwWrited=fwrite( pTmpFileBuffer, dwReaded,1,hFileWrite);
			dwLeft -= dwToProcess;
			dwLastFilePos -= dwToProcess;
		}

		//SetFilePointer(hFileWrite, 0, NULL, FILE_BEGIN);
		fseek(hFileWrite,0,SEEK_SET);
		//WriteFile(hFileWrite, pBuffer, dwSize, &dwWrited, NULL);
		dwWrited=fwrite( pBuffer, dwSize,1,hFileWrite);
	}

	VO_SAFE_DELETE(pTmpFileBuffer);
	fclose(hFileRead);
	fclose(hFileWrite);
	return true;
}

DWORD CPDSessionWrapper2::GetCurrBufferingTime()
{
	SPDInitParam* pInitParam = GetInitParam();
	DWORD dwBytesPerSec = 0;
	if(m_NetGet.GetParam(VOID_NETDOWN_BYTES_PER_SEC, (long*)&dwBytesPerSec))
	{
		if(g_nLog)
		{
			char sz[256];
			sprintf(sz, "[CPDSessionWrapper2::GetCurrBufferingTime]throughput: %d, clip bitrate: %d\r\n", dwBytesPerSec, m_dwClipBitrate);
			DumpLog(sz);
		}
		if(m_dwClipBitrate >= dwBytesPerSec)
		{
			DWORD dwBufferingTime = pInitParam->nPlayBufferTime;
			dwBufferingTime *= 1000;
			float time1=(float)dwBufferingTime / (1 + (float(dwBytesPerSec) / m_dwClipBitrate));
			dwBufferingTime = (long)time1;
			if(g_nLog)
			{
				char sz[128];
				sprintf(sz, "[CPDSessionWrapper2::GetCurrBufferingTime]buffering time: %d\r\n", dwBufferingTime);
				DumpLog(sz);
			}
			return dwBufferingTime;
		}
		else
			return (pInitParam->nBufferTime * 1000);
	}
	else
		return (pInitParam->nBufferTime * 1000);
}
static int instanceNum=0;
/**
* Close the opened source.
* \param pHandle [IN] The handle which was create by open function.
* \retval VO_ERR_NONE Succeeded.
*/
#define  USE_OPENLOCK 0
#if USE_OPENLOCK
static IVOMutex*			m_csOpenLock=NULL;
#endif//USE_OPENLOCK
VO_U32 VO_API voPDClose2(VO_PTR pHandle)
{
#if USE_OPENLOCK
	if (m_csOpenLock==NULL)
		return 123;
	CAutoLock lock(m_csOpenLock);//disable another open/close, and wait
#endif//USE_OPENLOCK
	while (instanceNum==3)//disable another close
	{
		voLog_android("voPDClose2","instanceNum!=1");
		//return 0;
		IVOThread::Sleep(10);
	}
	if (instanceNum==0)
	{
		voLog_android("voPDClose2","instanceNum==0,the close has been finished by another thread,exit");
		return 0;
	}
	instanceNum=3;//begin close, set the signal as 3
	voLog_android("voPDClose2","before destroy source");
	CPDSessionWrapper2* source = (CPDSessionWrapper2*)pHandle;
	SAFE_DELETE(source);
	CVOMemoryManager::reportMemStatus(VOMR_LEAK);
	CVOLog::DestroyVOLog();
#if USE_OPENLOCK
	if (m_csOpenLock)
	{
		DELETE_MUTEX(m_csOpenLock);
	}
#endif
	instanceNum=0;
	voLog_android("voPDClose2","END");
	return 0;
}

/**
* Open the source and return source handle
* \param ppHandle [OUT] Return the source operator handle
* \param pName	[IN] The source name
* \param pParam [IN] The source open param
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDOpen2(VO_PTR * ppHandle, VO_SOURCE_OPENPARAM * pParam)
{
	*ppHandle = NULL;

#if USE_OPENLOCK
	if (m_csOpenLock==NULL)
	{
		CREATE_MUTEX(m_csOpenLock);
	}
#endif
	CPDSessionWrapper2* source=NULL;
	{//the braces are necessary for non delay
#if USE_OPENLOCK
		CAutoLock lock(m_csOpenLock);////disable another open/close, and wait
#endif
		if (instanceNum!=0)
		{
			return 0;
		}
		instanceNum = 2;
		source=new(MEM_CHECK) CPDSessionWrapper2(pParam);
	}
	instanceNum=1;//good to close
#if !DELAY_OPEN_URL
	//m_NetGet.StopBuffering();
	int ret=source->OpenURL();
	if(ret==0)
	{
		*ppHandle=source;
		voLog(LL_DEBUG,"PDSource.txt","voPDOpen2 ok");
	}
	else
	{
		voLog(LL_DEBUG,"PDSource.txt","voPDOpen2 fail");
		voPDClose2(source);
	}
#else
	*ppHandle=source;
#endif
	return 0;
}



/**
* Get the source information
* \param pHandle [IN] The handle which was create by open function.
* \param pSourceInfo [OUT] The structure of source info to filled.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetSourceInfo2(VO_PTR pHandle, VO_SOURCE_INFO * pSourceInfo)
{
	CPDSessionWrapper2* source = (CPDSessionWrapper2*)pHandle;
	return source->GetSourceInfo(pSourceInfo);
}

/**
* Get the track information
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pTrackInfo [OUT] The track info to filled..
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetTrackInfo2(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
{
	CPDSessionWrapper2* source = (CPDSessionWrapper2*)pHandle;
	return source->GetTrackInfo(nTrack,pTrackInfo);
}

/**
* Get the track buffer
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pSample [OUT] The sample info was filled.
*		  Audio. It will fill the next frame audio buffer automatically.
*		  Video  It will fill the frame data depend on the sample time. if the next key frmae time
*				 was less than the time, it will fill the next key frame data, other, it will fill
*				 the next frame data.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetSample2(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	CPDSessionWrapper2* source = (CPDSessionWrapper2*)pHandle;
	return source->GetSample(nTrack,pSample);
}

/**
* Set the track read position.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param pPos [IN/OUT] The new pos will be set, and it will reset with previous key frame time if it is video track.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDSetPos2(VO_PTR pHandle, VO_U32 nTrack, VO_S64 * pPos)
{
	VO_U32 ret=0;
	if(1)//nTrack==0)
	{
		//voLog(LL_DEBUG,"PDSource.txt","seek %d\n",start);
		CPDSessionWrapper2* source = (CPDSessionWrapper2*)pHandle;
		VO_S32 pos=(VO_S32)*pPos;
		ret = source->SetPos(nTrack,&pos);
		*pPos=pos;
	}		
	return ret;
}

/**
* Set source param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDSetSourceParam2(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CPDSessionWrapper2* source = (CPDSessionWrapper2*)pHandle;
	return source->SetSourceParam(uID,pParam);
}

/**
* Get source param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param uID [IN] The param ID.
* \param pParam [Out] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetSourceParam2(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	CPDSessionWrapper2* source = (CPDSessionWrapper2*)pHandle;
	return source->GetSourceParam(uID,pParam);
}

/**
* Set track param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDSetTrackParam2(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	CPDSessionWrapper2* source = (CPDSessionWrapper2*)pHandle;
	return source->SetTrackParam(nTrack,uID,pParam);
}

/**
* Get track param for special target.
* \param pHandle [IN] The handle which was create by open function.
* \param nTrack [IN] The index of the track in source
* \param uID [IN] The param ID.
* \param pParam [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voPDGetTrackParam2(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	CPDSessionWrapper2* source = (CPDSessionWrapper2*)pHandle;
	return source->GetTrackParam(nTrack,uID,pParam);
}

VO_S32 VO_API voGetPDReadAPI (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag)
{
	if(pReadHandle)
	{
		pReadHandle->Close				 = voPDClose2;
		pReadHandle->GetSample		 =	voPDGetSample2;
		pReadHandle->GetSourceInfo =	voPDGetSourceInfo2;
		pReadHandle->GetSourceParam	=	voPDGetSourceParam2;
		pReadHandle->GetTrackInfo		=	voPDGetTrackInfo2;
		pReadHandle->GetTrackParam	=	voPDGetTrackParam2;
		pReadHandle->Open				 =		voPDOpen2;
		pReadHandle->SetPos				=		voPDSetPos2;
		pReadHandle->SetSourceParam	=	voPDSetSourceParam2;
		pReadHandle->SetTrackParam	=	voPDSetTrackParam2;

	}
	return 0;
}
