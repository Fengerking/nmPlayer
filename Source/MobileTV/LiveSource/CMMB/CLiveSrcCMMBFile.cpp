#include <stdlib.h>
#include <string.h>

#include "CLiveSrcCMMBFile.h"
#include "cmnFile.h"
#include "voOSFunc.h"

#pragma warning (disable : 4996)

const int TS_PACKET_SIZE = 188;
const VO_BYTE TS_SYNC_BYTE = 0x47;

const int MAX_PACKET_SIZE = 512 << 10;

VO_TCHAR * g_szESGName[] = 
		{_T("早闻天下"), _T("新闻联播"), _T("抗震救灾，众志成城"),
		 _T("神探狄仁杰三"), _T("NAB季后赛直播"), _T("风云足球：亚冠小组赛"), 
		 _T("五星体育"), _T("奥运中国：圣火上海行"), _T("中国新闻:实时报道救灾最新进展"),
		 _T("银河剧场：狭路相逢"),  _T("音乐人生：享受音乐感悟人生"),  _T("科学世界：科技冲击波5"),};

#define TIME_COPY(time1, time2) {\
	time1.wYear = time2->wYear; \
	time1.wMonth = time2->wMonth; \
	time1.wDayOfWeek = time2->wDayOfWeek; \
	time1.wDay = time2->wDay; \
	time1.wHour = time2->wHour; \
	time1.wMinute = time2->wMinute; \
	time1.wSecond = time2->wSecond; \
	time1.wMilliseconds = time2->wMilliseconds; }

#ifdef _LINUX_ANDROID
#define _SINGLE_FILE_PLAY
#endif

#ifdef _VMT2MFS
HANDLE g_hMFS = NULL;
#endif

VO_PBYTE FindTSPacketHeader(VO_PBYTE pData, int cbData, int packetSize)
{
	VO_PBYTE p = pData;
	VO_PBYTE p2 = pData + cbData - packetSize;
	while (p < p2)
	{
		if ( (*p == TS_SYNC_BYTE) && (*(p + packetSize) == TS_SYNC_BYTE) )
			return p;
		++p;
	}
	return 0;
}

VO_PBYTE FindTSPacketHeader2(VO_PBYTE pData, int cbData, int packetSize)
{
	VO_PBYTE p = FindTSPacketHeader(pData, cbData, packetSize);
	if (p)
	{
		VO_PBYTE p2 = p + packetSize * 2;
		if (p2 < pData + cbData)
			if (*p2 == TS_SYNC_BYTE)
				return p;
	}
	return 0;
}

CLiveSrcCMMBFile::CLiveSrcCMMBFile()
	: m_nPacketSize (0)
	, m_nBitrate (0)
	, m_ppFilesName (NULL)
	, m_hPushThread (NULL)
	, m_nStartTime (0)
	, m_hFile (NULL)
	, m_nSyncOffset (0)
	, m_bMTPDumpFile (false)
	, m_pBuffer (NULL)
	, m_nSent (0)
	, m_bReading (false)
	, m_nFileType (0)
	, m_nBufSize (0)
	, m_nBufRest (0)
	, m_dwCMMBID (0)
	, m_nCMMBCount (0)
{
	vostrcpy (m_szChannelFilePath, m_szAppPath);
	vostrcat (m_szChannelFilePath, _T("voChannelInfoFile.Dat"));
	vostrcpy (m_szFileFolder, _T(""));


#ifdef _VMT2MFS
	g_hMFS = CreateFile(_T("E:\\1.mfs"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
#endif
}

CLiveSrcCMMBFile::~CLiveSrcCMMBFile(void)
{
	Stop();

	if (m_hFile != NULL)
		cmnFileClose (m_hFile);
	if (m_pBuffer != NULL)
		delete []m_pBuffer;

#ifdef _VMT2MFS
	if (g_hMFS != NULL)
		CloseHandle(g_hMFS);
#endif
}

int CLiveSrcCMMBFile::Scan (VO_LIVESRC_SCAN_PARAMEPTERS * pParam, bool nQuickScan)
{
// 	VO_LIVESRC_FILE_SCANPARAM * pScanInfo = (VO_LIVESRC_FILE_SCANPARAM *)pParam;
// 	vostrcpy (m_szFileFolder, pScanInfo->pFolder);
// 	m_nPacketSize = pScanInfo->nPackSize;
// 	m_nBitrate = pScanInfo->nBitrate;

	// tag: 20100813
	//vostrcpy (m_szFileFolder, _T("E:\\Media\\MFS\\Clips"));
	vostrcpy (m_szFileFolder, (TCHAR*)pParam->pParam);

	m_nPacketSize = 64512;
	m_nBitrate = 65000;

	ReleaseData ();

	m_bCancelScan = false;
	m_nCurChannel = 0;

#ifdef _SINGLE_FILE_PLAY

	vostrcpy (m_szFileFolder, _T("/sdcard/cmmb.mfs"));
	m_nPacketSize = 64512;
	m_nBitrate = 65000;

	m_nChannelCount = 1;

	m_pChannelInfo = new VO_LIVESRC_CHANNELINFO[m_nChannelCount];
	memset (m_pChannelInfo, 0, sizeof (VO_LIVESRC_CHANNELINFO) * m_nChannelCount);

	m_ppFilesName = new VO_TCHAR * [m_nChannelCount];
	memset (m_ppFilesName, 0, sizeof (VO_TCHAR *) * m_nChannelCount);

	int nIndex = 0;
	m_pChannelInfo[nIndex].nChannelID = nIndex;
	m_pChannelInfo[nIndex].hIcon = NULL;
	vostrcpy (m_pChannelInfo[nIndex].szName, "CMMB Test File");
	m_pChannelInfo[nIndex].nCurrent = false;

	m_ppFilesName[nIndex] = new VO_TCHAR [MAX_PATH];
	vostrcpy (m_ppFilesName[nIndex], m_szFileFolder);
	m_pChannelInfo[nIndex].pData = (void *)m_ppFilesName[nIndex];

	VOLOGI ("[ENG] single file [%s]\n", m_szFileFolder);
	VOLOGI (" ++++++++++-----------++++++++++ \n");

#else
	//Find the files under the folder
	VO_TCHAR szFilter[MAX_PATH];
	vostrcpy (szFilter, m_szFileFolder);
	vostrcat (szFilter, _T("\\*."));
	vostrcat (szFilter, _T("MFS"));

	WIN32_FIND_DATA ffData;
	HANDLE hFind = FindFirstFile (szFilter, &ffData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		//NotifyStatus(VO_LIVESRC_STATUS_SCAN_PROCESS, 100, 0);
		return VO_ERR_LIVESRC_NO_CHANNEL;
	}
	m_nChannelCount++;
	while(true) 
	{
		if(!FindNextFile(hFind, &ffData))
			break;
		m_nChannelCount++;
	}
	FindClose (hFind);

	m_pChannelInfo = new VO_LIVESRC_CHANNELINFO[m_nChannelCount];
	memset (m_pChannelInfo, 0, sizeof (VO_LIVESRC_CHANNELINFO) * m_nChannelCount);

	m_ppFilesName = new TCHAR * [m_nChannelCount];
	memset (m_ppFilesName, 0, sizeof (TCHAR *) * m_nChannelCount);

	int nIndex = 0;
	hFind = FindFirstFile (szFilter, &ffData);

	m_pChannelInfo[nIndex].nChannelID = nIndex;
	m_pChannelInfo[nIndex].hIcon = NULL;
	vostrcpy (m_pChannelInfo[nIndex].szName, ffData.cFileName);
	m_pChannelInfo[nIndex].nCurrent = false;

	m_ppFilesName[nIndex] = new TCHAR [MAX_PATH];
	vostrcpy (m_ppFilesName[nIndex], ffData.cFileName);
	m_pChannelInfo[nIndex].pData = (void *)m_ppFilesName[nIndex];
	nIndex++;

	while(true) 
	{
		if(!FindNextFile(hFind, &ffData))
			break;

		m_pChannelInfo[nIndex].nChannelID = nIndex;
		m_pChannelInfo[nIndex].hIcon = NULL;
		vostrcpy (m_pChannelInfo[nIndex].szName, ffData.cFileName);
		m_pChannelInfo[nIndex].nCurrent = false;

		m_ppFilesName[nIndex] = new TCHAR [MAX_PATH];
		vostrcpy (m_ppFilesName[nIndex], ffData.cFileName);
		m_pChannelInfo[nIndex].pData = (void *)m_ppFilesName[nIndex];

		if (m_fStatus != NULL)
		{
			voOS_Sleep (200);
			NotifyStatus(VO_LIVESRC_STATUS_SCAN_PROCESS, nIndex * 100 / m_nChannelCount, 0);
		}

		if (m_bCancelScan)
			break;

		nIndex++;
	}
	FindClose (hFind);

	NotifyStatus(VO_LIVESRC_STATUS_SCAN_PROCESS, 100, 0);

#endif // _SINGLE_FILE_PLAY

	if (m_bCancelScan)
		return -1;

	m_pChannelESGNum = new int[m_nChannelCount];
	memset (m_pChannelESGNum, 0, sizeof (int) * m_nChannelCount);

	m_ppESGInfo = new VO_CMMB_ESG_ITEM * [m_nChannelCount];
	memset (m_ppESGInfo, 0, sizeof (VO_CMMB_ESG_ITEM *) * m_nChannelCount);

#ifdef _WIN32
	VODATETIME tm;
	GetLocalTime((LPSYSTEMTIME)&tm);
#endif

	for (int i = 0; i < m_nChannelCount; i++)
	{
		m_pChannelESGNum[i] = rand () % 6 + 6;
		m_ppESGInfo[i] = new VO_CMMB_ESG_ITEM[m_pChannelESGNum[i]];

		int nStart = 0;
		int nStep = 24 * 60 * 60 / m_pChannelESGNum[i];

		for (int j = 0; j < m_pChannelESGNum[i]; j++)
		{
			m_ppESGInfo[i][j].nChannelID = i;
			m_ppESGInfo[i][j].nIndex = i;
			vostrcpy (m_ppESGInfo[i][j].szName, g_szESGName[rand () % 12]);

#ifdef _WIN32
			tm.wHour = nStart / (60 * 60);
			tm.wMinute = (int)(nStart / 60) - tm.wHour * 60;
			tm.wSecond = nStart - tm.wMinute * 60;
			tm.wMilliseconds = 0;
			TIME_COPY(m_ppESGInfo[i][j].nStartTime, (&tm));
#endif
			nStart += nStep;
		}
	}

	WriteData ();

	return m_nChannelCount;
}

VO_U32 CLiveSrcCMMBFile::Start ()
{
	if (m_hPushThread != NULL)
		return false;

	m_bStop = false;
	m_bPause = false;
	m_nStartTime = voOS_GetSysTime ();

	VO_U32 dwThreadID;
	voThreadCreate(&m_hPushThread, &dwThreadID, PushDataEntry, this, 0);
	if (m_hPushThread == NULL)
		return 0;

	return 0;
}

VO_U32 CLiveSrcCMMBFile::Stop ()
{
	if(m_bStop)
		return 0;

	m_bStop = true;

	VO_U32 wait = 0;
	while (m_hPushThread != NULL)
	{
		wait += 10;
		voOS_Sleep (10);

		if (wait >= 5000)
		{
			break;
		}
	}
		

	//WaitForSingleObject(m_hPushThread, 5000);

	m_bReading = false;

	return 0;
}

int CLiveSrcCMMBFile::SetChannel (int nNewChannelID)
{
	if (m_nChannelCount < 0)
		return -1;

	if (nNewChannelID < 0)
		nNewChannelID = m_nCurChannel;

	if (nNewChannelID < 0 || nNewChannelID >= m_nChannelCount)
		nNewChannelID = 0;
	m_nCurChannel = nNewChannelID;

	int i = 0;
	for (i = 0; i < m_nChannelCount; i++)
	{
		m_pChannelInfo[i].nCurrent = false;
	}

	if (nNewChannelID >= 0)
	{
		for (i = 0; i < m_nChannelCount; i++)
		{
			if (m_pChannelInfo[i].nChannelID == nNewChannelID)
			{
				m_pChannelInfo[i].nCurrent = true;
				break;
			}
		}
	}

	VOLOGI("[ENG] Select ch No.%d out of %d\n", nNewChannelID, m_nChannelCount);

	VO_TCHAR szFile[MAX_PATH];
#ifdef _SINGLE_FILE_PLAY
	vostrcpy (szFile, (TCHAR *)m_pChannelInfo[i].pData);
#else
	vostrcpy (szFile, m_szFileFolder);
	vostrcat (szFile, _T("\\"));
	vostrcat (szFile, (TCHAR *)m_pChannelInfo[i].pData);
#endif
	VOLOGI("[ENG] File path [%s]\n", szFile);

	m_bPause = true;
	while (m_bReading)
		voOS_Sleep (10);

	int nRC = 0;
	if (m_hFile != NULL)
	{
		VOLOGI("[ENG] File was open. Close it now.\n");
		nRC = cmnFileClose (m_hFile);

		if (m_fStatus != NULL)
			m_fStatus (m_pUserData, VO_LIVESRC_STATUS_CHANNEL_STOP, VO_LIVESRC_FT_FILE, VO_LIVESRC_FT_FILE);
//		if (m_fSendData != NULL)
//			m_fSendData (m_pUserData, m_nCurChannel, NULL, -1);
	}

	m_nBufRest = 0;
	m_dwCMMBID = 0;

// 	TCHAR * pExt = vostrrchr (szFile, _T('.')) + 1;
// 	TCHAR	szExt[8];
// 	vostrcpy(szExt, pExt);
//  	vostrupr (szExt);
// 	if (!vostrcmp (szExt, _T("MFS")) || !vostrcmp(szExt, _T("VMT")))
	{
		m_nFileType = 10;
		m_nBufRest = 0;
		m_dwCMMBID = 0;
		if (m_pBuffer == NULL)
		{
			m_nBufSize = m_nPacketSize * 2;
			m_pBuffer = new VO_BYTE[m_nBufSize];
		}
	}

	VO_FILE_SOURCE	fileSource;
	memset(&fileSource, 0, sizeof(VO_FILE_SOURCE));
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)szFile;
	fileSource.nMode = VO_FILE_READ_ONLY;

	m_hFile = cmnFileOpen(&fileSource);
	if (m_hFile == NULL)
	{
		VOLOGE("[ENG] Failed to open file [%s]\n", fileSource.pSource);
		m_hFile = NULL;
		return -1;
	}

	VO_U32 dwRead = 0;
	VO_TCHAR szMTPDump[32];
	dwRead = cmnFileRead (m_hFile, szMTPDump, 32);
	if (dwRead > 11 && !vostrncmp (szMTPDump, _T("vovoMTPDump"), 11))
	{
		m_nSyncOffset = 11;
		m_bMTPDumpFile = true;
	}
	else
	{
		m_bMTPDumpFile = false;
		m_nSyncOffset = 0;
	}
	cmnFileSeek (m_hFile, m_nSyncOffset, VO_FILE_BEGIN);

	m_bPause = false;

	if (m_fStatus != NULL)
		m_fStatus (m_pUserData, VO_LIVESRC_STATUS_CHANNEL_START, VO_LIVESRC_FT_FILE, VO_LIVESRC_FT_FILE);

	return 0;
}

/*
int CLiveSrcCMMBFile::GetESG (int nChannelID, int * pCount, VO_LIVESRC_ESGINFO ** ppESGInfo)
{
	*pCount = 0;
	*ppESGInfo = NULL;

	if (m_nChannelCount <= 0)
		return -1;

	int i = 0;
	for (i = 0; i < m_nChannelCount; i++)
	{
		if (m_pChannelInfo[i].nChannelID == nChannelID)
		{
			break;
		}
	}

	*pCount = m_pChannelESG[i];
	*ppESGInfo = m_ppESGInfo[i];

	return 0;
}
*/

VO_U32 CLiveSrcCMMBFile::PushDataEntry(VO_PTR pParam)
{
	CLiveSrcCMMBFile * pFileSource = (CLiveSrcCMMBFile*) pParam;
	VO_U32 nRC = pFileSource->PushData();
	return nRC;
}

VO_U32 CLiveSrcCMMBFile::PushData()
{
	VO_U32	dwRead = 0;
	VO_PBYTE	pData = m_pBuffer;
	int		nSize = 0;

	bool	bReset = false;

	while (!m_bStop)
	{
		m_bReading = false;
		if (m_bPause)
		{
			voOS_Sleep (10);
			continue;
		}
		m_bReading = true;

		if (m_hFile == NULL)
		{
			break;
		}

		if (m_bMTPDumpFile)
		{
			/*
			VO_U32 dwPacketSize = 0;
			ReadFile(m_hFile, &dwPacketSize, 4, &dwRead, NULL);
			if (dwRead != 4)
			{
				bReset = true;
			}
			else
			{
				ReadFile(m_hFile, m_pBuffer, dwPacketSize, &dwRead, NULL);
				nSize = dwRead;
				if (dwRead != dwPacketSize)
					bReset = true;
			}
			*/
			int nPacketType;
			VO_MTV_FRAME_BUFFER nFrameBuffer;
			dwRead = cmnFileRead(m_hFile, &nPacketType, 4);
			if (dwRead != 4)	
			{
				bReset = true;
			}
			else
			{
				dwRead = cmnFileRead(m_hFile, &nFrameBuffer, sizeof(VO_MTV_FRAME_BUFFER));
				if (dwRead != sizeof(VO_MTV_FRAME_BUFFER))	
				{
					bReset = true;
				}
				else
				{
					dwRead = cmnFileRead(m_hFile, m_pBuffer, nFrameBuffer.nSize);
					if (dwRead != nFrameBuffer.nSize)	
						bReset = true;
					else
						nFrameBuffer.pData = m_pBuffer;
				}
			}

			if (bReset)
			{
				m_nBufRest = 0;
				pData = NULL;
				nSize = 0;

				cmnFileSeek (m_hFile, m_nSyncOffset, VO_FILE_BEGIN);

				bReset = false;
			}
			else
			{
				int n = 0;
				//SendData(/*PACKET_AUDIO |*/ VO_AUDIO_CodingDRA/*nPacketType*/, (VO_BYTE *)&nFrameBuffer, sizeof(VO_MTV_FRAME_BUFFER));
			}
			continue;
		}
		else
		{
			if (m_nFileType == 10) // CMMB
			{
				if (m_nBufRest > 0 && m_nBufRest < m_nBufSize)
					memcpy (m_pBuffer, m_pBuffer + m_nBufSize - m_nBufRest, m_nBufRest);
				else if (m_nBufRest == m_nBufSize)
					m_nBufRest = 0;

				dwRead = 0;

				if (m_nBufRest < m_nBufSize)
				{
					dwRead = cmnFileRead(m_hFile, m_pBuffer + m_nBufRest, m_nBufSize - m_nBufRest);
					//VOLOGI("[DBG] Read data 1: size[%d] realsize[%d]", m_nBufSize - m_nBufRest, dwRead);

					if (dwRead != m_nBufSize - m_nBufRest || dwRead == 0)
					{
						m_nBufRest = 0;
						bReset = true;
					}
				}

				nSize = m_nBufRest + dwRead;
				pData = ParseCMMBBuffer (m_pBuffer, nSize);

				if (pData == NULL)
				{
					m_nBufRest = 0;
				}
				else
				{
// 					if (nSize == 0)
// 					{
// 						m_nBufRest = m_nBufSize - (pData - m_pBuffer);
// 						pData = NULL;
// 					}
// 					else
// 						m_nBufRest = m_nBufSize - nSize;

					if (nSize == 0)
						nSize = 55296;//m_nPacketSize;
					m_nBufRest = m_nBufSize - nSize;
				}

				if (pData != NULL)
				{
					if (m_dwCMMBID == 0)
					{
// 						VO_U32 dwWord = *(VO_U32 *)(pData + 4);
// 						m_dwCMMBID = (dwWord >> 16) & 0X3F;
						VO_BYTE bByte = *(VO_BYTE *)(pData + 4 + 2);
						m_dwCMMBID = bByte & 0x3F;
					}
					else
					{
// 						VO_U32 dwWord = *(VO_U32 *)(pData + 4);
// 						dwWord = (dwWord >> 16) & 0X3F;
						VO_U32 dwWord = 0;
						VO_BYTE bByte = *(VO_BYTE *)(pData + 4 + 2);
						dwWord = bByte & 0x3F;

						//VOLOGI("[ENG] Current MFS service ID : %d", dwWord);

// 						if (dwWord != m_dwCMMBID)
// 						{
// 							pData = NULL;
// 							OutputDebugString(_T("[ENG] Discard MFS with different service ID\n"));
// 						}
					}
				}
			}
			else
			{
				dwRead = cmnFileRead(m_hFile, m_pBuffer, m_nPacketSize);
				VOLOGI("[DBG] Read data 2: size[%d] realsize[%d]", m_nPacketSize, dwRead);

				nSize = dwRead;
				if (dwRead != m_nPacketSize)
					bReset = true;
			}
		}

		if (bReset)
		{
			if (m_fStatus != NULL)
				m_fStatus (m_pUserData, VO_LIVESRC_STATUS_CHANNEL_STOP, VO_LIVESRC_FT_FILE, VO_LIVESRC_FT_FILE);

			m_nBufRest = 0;
			pData = NULL;
			nSize = 0;

			cmnFileSeek (m_hFile, m_nSyncOffset, VO_FILE_BEGIN);


			if (m_fStatus != NULL)
				m_fStatus (m_pUserData, VO_LIVESRC_STATUS_CHANNEL_START, VO_LIVESRC_FT_FILE, VO_LIVESRC_FT_FILE);

			bReset = false;
		}
		else
		{
			if (m_fSendData && pData != NULL)
			{
#ifdef _VMT2MFS
				VO_U32 nWriteSize;
				WriteFile (g_hMFS, pData, nSize, &nWriteSize, NULL);
#endif
				//VOLOGI("[ENG] Send Data size=[%d] Total size=[%d]", nSize, m_nSent);
// 				m_fSendData (m_pUserData, m_nCurChannel, pData, nSize);

				static VO_U32 nLastTime = voOS_GetSysTime();
				int nBeforeParse = voOS_GetSysTime();
				//VOLOGI("[ENG] Time step since last data pack is sent and parsed : [%d]", nBeforeParse - nLastTime);

				VO_PARSER_INPUT_BUFFER inBuffer;
				inBuffer.pBuf = pData;
				inBuffer.nBufLen = nSize;
				inBuffer.nStreamID = (VO_U32)m_dwCMMBID;
				int nRC = m_pParser->Process(&inBuffer);

				int nNewTime = voOS_GetSysTime();
				//VOLOGI("[ENG] Current parse time : [%d]", nNewTime - nBeforeParse);
				nLastTime = nNewTime;
			}
			m_nSent += nSize;

			//Bitrate control for CMMB
// 			int passTime = GetTickCount() - m_nStartTime;
// 			m_nCMMBCount ++;
// 			int nDelay = 1000 * m_nCMMBCount - passTime;
// 			if (nDelay > 2)
// 				voOS_Sleep(nDelay);
		}

		BitrateControl();
	}

	m_hPushThread = NULL;

	return 0;
}

VO_PBYTE CLiveSrcCMMBFile::ParseCMMBBuffer (VO_PBYTE pBuffer, int & nBufSize)
{
	VO_PBYTE pHead = NULL;
	VO_U32 dwSync = 0X01000000;

	int i = 0;
	for (i = 0; i < nBufSize - 3; i++)
	{
		if (!memcmp (pBuffer + i, &dwSync, 4))
		{
			pHead = pBuffer + i;
			if (m_dwCMMBID == 0)
			{
				VO_BYTE bByte = *(VO_BYTE *)(pHead + 4 + 2);
				m_dwCMMBID = bByte & 0x3F;
				//VOLOGI("[ENG] Current MFS service ID : %d", m_dwCMMBID);
			}
			break;
		}
	}

	if (pHead == NULL)
	{
		nBufSize = 0;
		return NULL;
	}

	VO_PBYTE pTail = NULL;
	int nNext = pHead - pBuffer + 4;
	for (i = nNext; i < nBufSize - 3; i++)
	{
		if (!memcmp (pBuffer + i, &dwSync, 4))
		{
			VO_PBYTE pTailCandid = pBuffer + i;
			VO_BYTE bByte = *(VO_BYTE *)(pTailCandid + 4 + 2);
			VO_U32 dwWord = bByte & 0x3F;
			if (dwWord == m_dwCMMBID)
			{
				pTail = pTailCandid;
				break;
			}
		}
	}
	if (pTail == NULL)
	{
		nBufSize = 0;
		return pHead;
	}

	nBufSize = pTail - pHead;

	return pHead;
}

void CLiveSrcCMMBFile::BitrateControl(void)
{
// 	if (m_nFileType == 10) // CMMB
// 		return;

	VO_S64 totalBytes = m_nSent;
	int totalTime = (int)(totalBytes * 1000 / m_nBitrate);
	int usedTime = voOS_GetSysTime() - m_nStartTime;
	int nDelay = totalTime - usedTime;
	if (nDelay < 0)
		nDelay = 0;

	if (nDelay > 2)
		voOS_Sleep(nDelay);

	//VOLOGI("[SRC] File sleep: %d ms", nDelay);
}

void CLiveSrcCMMBFile::ReleaseData (void)
{
	if (m_nChannelCount == 0)
		return;

	int i = 0;
	int j = 0;

	if (m_ppESGInfo != NULL)
	{
		for (i = 0; i < m_nChannelCount; i++)
		{
			if (m_ppESGInfo[i] != NULL)
				delete []m_ppESGInfo[i];
		}
		delete []m_ppESGInfo;
		m_ppESGInfo = NULL;
	}

	if (m_pChannelESGNum != NULL)
	{
		delete []m_pChannelESGNum;
		m_pChannelESGNum = NULL;
	}

	for (i = 0; i < m_nChannelCount; i++)
	{
		if (m_ppFilesName[i] != NULL)
			delete []m_ppFilesName[i];
	}
	delete []m_ppFilesName;

	delete []m_pChannelInfo;
	m_pChannelInfo = NULL;

	m_nChannelCount = 0;
}

void CLiveSrcCMMBFile::ReadData (void)
{
	VO_FILE_SOURCE	fileSource;
	memset(&fileSource, 0, sizeof(VO_FILE_SOURCE));
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)m_szChannelFilePath;
	fileSource.nMode = VO_FILE_READ_ONLY;

	VO_PTR hFile = cmnFileOpen(&fileSource);
	if (hFile == NULL)
		return;

	bool	bRead = true;
	VO_U32	dwRead = 0;
	
	dwRead = cmnFileRead (hFile, &m_nChannelCount, sizeof (int));
	if (dwRead != 4 || m_nChannelCount <= 0)
		bRead = false;
	dwRead = cmnFileRead (hFile, &m_nCurChannel, sizeof (int));
	if (dwRead != 4 || m_nCurChannel < 0)
		bRead = false;

	int i = 0;
	int j = 0;

	if (bRead)
	{
		m_pChannelInfo = new VO_LIVESRC_CHANNELINFO[m_nChannelCount];
		memset (m_pChannelInfo, 0, sizeof (VO_LIVESRC_CHANNELINFO) * m_nChannelCount);

		m_ppFilesName = new TCHAR * [m_nChannelCount];
		memset (m_ppFilesName, 0, m_nChannelCount * sizeof (TCHAR *));

		for (i = 0; i < m_nChannelCount; i++)
		{
			dwRead = cmnFileRead (hFile, &m_pChannelInfo[i], sizeof (VO_LIVESRC_CHANNELINFO));
			if (dwRead != sizeof (VO_LIVESRC_CHANNELINFO))
			{
				bRead = false;
				break;
			}
			
			m_ppFilesName[i] = new TCHAR[MAX_PATH];
			dwRead = cmnFileRead (hFile,m_ppFilesName[i], sizeof (TCHAR) * MAX_PATH);
			if (dwRead != sizeof (TCHAR) * MAX_PATH)
			{
				bRead = false;
				break;
			}

			m_pChannelInfo[i].pData		= m_ppFilesName[i];
			m_pChannelInfo[i].nCurrent	= (i == m_nCurChannel)?1:0;
		}
	}

	if (bRead)
	{
		m_pChannelESGNum = new int[m_nChannelCount];
		for (i = 0; i < m_nChannelCount; i++)
		{
			dwRead = cmnFileRead (hFile, &m_pChannelESGNum[i], sizeof (int));
			if (dwRead != sizeof (int))
			{
				bRead = false;
				break;
			}
		}
	}

	if (bRead)
	{
		m_ppESGInfo = new VO_CMMB_ESG_ITEM * [m_nChannelCount];
		for (i = 0; i < m_nChannelCount; i++)
		{
			m_ppESGInfo[i] = new VO_CMMB_ESG_ITEM[m_pChannelESGNum[i]];
			for (j = 0; j < m_pChannelESGNum[i]; j++)
			{
				dwRead = cmnFileRead (hFile, &m_ppESGInfo[i][j], sizeof (VO_CMMB_ESG_ITEM));
				if (dwRead != sizeof (VO_CMMB_ESG_ITEM))
				{
					bRead = false;
					break;
				}
			}
		}
	}

	if (bRead)
	{
		dwRead = cmnFileRead (hFile, m_szFileFolder, MAX_PATH * sizeof (TCHAR));
		dwRead = cmnFileRead (hFile, &m_nPacketSize, sizeof (int));
		dwRead = cmnFileRead (hFile, &m_nBitrate, sizeof (int));
	}

	if (!bRead)
		ReleaseData ();

	cmnFileClose(hFile);
}

void CLiveSrcCMMBFile::WriteData (void)
{
	VO_FILE_SOURCE	fileSource;
	memset(&fileSource, 0, sizeof(VO_FILE_SOURCE));
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)m_szChannelFilePath;
	fileSource.nMode = VO_FILE_WRITE_ONLY;

	VO_PTR hFile = cmnFileOpen(&fileSource);
	if (hFile == NULL)
		return;

	VO_U32 dwWrite = 0;
	dwWrite = cmnFileWrite (hFile, &m_nChannelCount, sizeof (int));
	dwWrite = cmnFileWrite (hFile, &m_nCurChannel, sizeof (int));

	int i = 0;
	int j = 0;
	for (i = 0; i < m_nChannelCount; i++)
	{
		dwWrite = cmnFileWrite (hFile, &m_pChannelInfo[i], sizeof (VO_LIVESRC_CHANNELINFO));
		dwWrite = cmnFileWrite (hFile, m_ppFilesName[i], sizeof (VO_TCHAR) * MAX_PATH);
	}

	for (i = 0; i < m_nChannelCount; i++)
		dwWrite = cmnFileWrite (hFile, &m_pChannelESGNum[i], sizeof (int));

	if (m_pChannelESGNum != NULL)
	{
		for (i = 0; i < m_nChannelCount; i++)
		{
			for (j = 0; j < m_pChannelESGNum[i]; j++)
				dwWrite = cmnFileWrite (hFile, &m_ppESGInfo[i][j], sizeof (VO_CMMB_ESG_ITEM));
		}
	}

	dwWrite = cmnFileWrite (hFile, m_szFileFolder, MAX_PATH * sizeof (VO_TCHAR));
	dwWrite = cmnFileWrite (hFile, &m_nPacketSize, sizeof (int));
	dwWrite = cmnFileWrite (hFile, &m_nBitrate, sizeof (int));

	cmnFileClose (hFile);
}

void CLiveSrcCMMBFile::WriteCurChannel()
{
	VO_FILE_SOURCE	fileSource;
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)m_szChannelFilePath;
	fileSource.nMode = VO_FILE_READ_WRITE;

	VO_PTR hFile = cmnFileOpen(&fileSource);
	if (hFile == NULL)
	{
		WriteData();	// Re-create the channel file
		hFile = cmnFileOpen(&fileSource);
		if (hFile == NULL)
			return;
	}

	VO_S64 dwPtr = cmnFileSeek(hFile, sizeof(int), VO_FILE_BEGIN);
	if (dwPtr < 0)
	{
		cmnFileClose(hFile);
		return;
	}

	cmnFileWrite (hFile, &m_nCurChannel, sizeof (int));
	cmnFileClose(hFile);
}

