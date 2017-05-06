#include "CEngineBase.h"

CEngineBase::CEngineBase()
: m_cChannel(0)
, m_nCurrentChannel(-1)
, m_nDefaultChannel(-1)
, m_nDeviceParam(0)
, m_pParser(NULL)
, m_fRecording(NULL)
#ifdef _DUMP_DATA
, m_fDump(NULL)
#endif //_DUMP_DATA
{
	m_TotalEsgInfo.pEsgInfo = NULL;
	m_TotalEsgInfo.tEngine	= VO_LIVESRC_FT_FILE;

	SetStatusClosed();
}

int CEngineBase::Open (void * pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData)
{
	m_pUserData = m_pOrigUserData = pUserData;
	m_fStatus = fStatus;
	m_fSendData = fSendData;

	int rc = LoadConfig();

	rc = OpenDevice();
	if (rc == 0)
		SetStatusOpened();

	return rc;
}

int CEngineBase::Close ()
{
	if (IsStatusStarted())
		Stop();
	int rc = CloseDevice();
	if (rc == 0)
		SetStatusClosed();
	SaveConfig();
	return rc;
}

int	CEngineBase::Cancel (int nID)
{
	if (nID == VO_LIVESRC_TASK_SCAN)
	{
		EnableScan(VO_FALSE);
		return 0;
	}
	return -1;
}
/*
VO_U32 CEngineBase::TestPushDataEntry(VO_PTR pParam)
{
	CEngineBase * pInst = (CEngineBase *)pParam;
	int dwRet = pInst->TestPushData();
	return dwRet;
}

int CEngineBase::TestPushData()
{
	return 0;

	int BufferSize = 3760;
	VO_BYTE * pFileBuffer = new BYTE[BufferSize + 2];
	HANDLE tsFile = CreateFile (_T("\\ResidentFlash\\491.ts"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	if (tsFile != INVALID_HANDLE_VALUE)
	{
		while (!m_bStopData)
		{
			VO_U32 dwRead;
			VO_U32 dwPtr = ReadFile(tsFile, pFileBuffer, BufferSize, &dwRead, NULL);
			if (dwRead == BufferSize )
			{
				SendData(VO_LIVESRC_DATABLOCK, pFileBuffer, BufferSize);
				printf("[][] %d byte data sent.\n", BufferSize);
			}
		}
	}
	CloseHandle(tsFile);
	delete[] pFileBuffer;
	return 0;
}
*/
int	CEngineBase::Start (void)
{
#ifdef _DUMP_DATA

	if (m_fDump)
		cmnFileClose(m_fDump);

	VO_TCHAR	szPath[MAX_PATH];
	_stprintf(szPath, _T("D:\\CH%d.ts"), m_nCurrentChannel);
	VO_FILE_SOURCE	fileSource;
	memset(&fileSource, 0, sizeof(VO_FILE_SOURCE));
	fileSource.nFlag = VO_FILE_TYPE_NAME;
	fileSource.nOffset = 0;
	fileSource.nLength = 0xFFFFFFFF;
	fileSource.pSource = (VO_PTR)szPath;
	fileSource.nMode = VO_FILE_WRITE_ONLY;
	m_fDump = cmnFileOpen(&fileSource);
#endif //_DUMP_DATA

/*
	m_bStopData = VO_FALSE;
	VO_U32 dwThreadID;
	m_TestPushDataThread = CreateThread (NULL, 0, TestPushDataEntry, this, 0, &dwThreadID);
	if (m_TestPushDataThread == NULL)
		return VORC_FAIL;
*/

	int rc = StartReceive();
	if (rc == 0)
		SetStatusStarted();
	return rc;
}

int	CEngineBase::Stop (void)
{
/*
	m_bStopData = VO_TRUE;
	WaitForSingleObject(m_TestPushDataThread, 5000);
*/

	int rc = StopReceive();
	if (rc == 0)
		SetStatusStopped();

#ifdef _DUMP_DATA
	if (m_fDump)
	{
		cmnFileClose(m_fDump);
		m_fDump = NULL;
	}
#endif //_DUMP_DATA

	if (m_fRecording)
	{
		cmnFileClose(m_fRecording);
		m_fRecording = NULL;
	}

	return rc;
}

int CEngineBase::FindChannel(VO_LIVESRC_FORMATTYPE type)
{
	CChannelInfo* p = m_aChannelInfo;
	for (int i = 0; i < m_cChannel; i++)
	{
		if (p->nType == type)
			return i;
		p++;
	}
	return -1;
}

