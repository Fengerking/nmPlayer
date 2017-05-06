#ifndef _MTV_ENGINE_BASE_H_
#define _MTV_ENGINE_BASE_H_

#include "voString.h"
//#include "voMTVEngine.h"
#include "voLiveSource.h"
#include "voLog.h"
#include "cmnFile.h"
#include "voParser.h"
#include "CCMMBDemux.h"
#include "ChannelInfo.h"

const int MAX_CHANNEL = 128;

class CEngineBase
{
public:
	CEngineBase();
	int Open (void* pUserData, VOSRCSTATUS	fStatus, VOSRCSENDDATA fSendData);
	int Close ();

	virtual int Scan (VO_LIVESRC_SCAN_PARAMEPTERS* pParam) = 0;
	virtual int	Cancel (int nID);

	virtual int	Start (void);
	virtual int	Stop (void);

	virtual int SetDataSource(VO_PTR pSource, VO_U32 nFlag){return 0;};
	virtual int Seek(VO_U64 nTimeStamp){return 0;};

	virtual int	SetChannel (int nChannelID) = 0;
	virtual int	GetESG (int nChannelID, VO_LIVESRC_ESG_INFO* ppESGInfo) = 0;

	virtual int GetSignalStrength (void) { return 0; }

	int	GetChannel (int * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo)
	{
		*pCount = m_cChannel;
		*ppChannelInfo = m_aChannelInfo;
		return 0;
	}
	int	GetChannelNum (void) {return m_cChannel;}

	int StartRecord (VO_PTCHAR pPath)
	{
		VO_FILE_SOURCE	fileSource;
		memset(&fileSource, 0, sizeof(VO_FILE_SOURCE));
		fileSource.nFlag = VO_FILE_TYPE_NAME;
		fileSource.nOffset = 0;
		fileSource.nLength = 0xFFFFFFFF;
		fileSource.pSource = (VO_PTR)pPath;
		fileSource.nMode = VO_FILE_WRITE_ONLY;
		m_fRecording = cmnFileOpen(&fileSource);
		if (m_fRecording)
			return VO_ERR_LIVESRC_OK;
		else 
			return VO_ERR_LIVESRC_FAIL;
	}

	int EndRecord (void)
	{
		if (!m_fRecording)
			return VO_ERR_LIVESRC_FAIL;
		cmnFileClose(m_fRecording);
		return VO_ERR_LIVESRC_OK;
	}

public:
	virtual int OpenDevice() = 0;
	virtual int CloseDevice() = 0;
	virtual int LoadConfig() = 0;
	virtual int SaveConfig() = 0;
	virtual int StartReceive() = 0;
	virtual int StopReceive() = 0;

protected:
	void SendStatus(int nID, int nParam1, int nParam2)
	{
		m_fStatus(m_pOrigUserData, nID, nParam1, nParam2);
	}

	virtual void SendData(int bOutputType, unsigned char * pData, int nDataLen, unsigned long long llTimeStamp)
	{
#ifdef _DUMP_DATA
		if (m_fDump)
		{
			VO_MTV_FRAME_BUFFER * pFrame = (VO_MTV_FRAME_BUFFER *)pData;
// 			if (pFrame->nCodecType == VO_VIDEO_CodingH264)
			if (pFrame->nCodecType == VO_AUDIO_CodingAAC)
				cmnFileWrite(m_fDump, pFrame->pData, pFrame->nSize);
// 			cmnFileWrite(m_fDump, pData, nDataLen);
		}
#endif //_DUMP_DATA
		if (m_fRecording)
		{
			cmnFileWrite(m_fRecording, pData, nDataLen);
		}

		m_AVSample.Buffer	= pData;
		m_AVSample.Size		= nDataLen;
		m_AVSample.Duration	= 0;
		m_AVSample.Time		= llTimeStamp;
		//m_fSendData(m_pUserData, pData);
		m_fSendData(m_pUserData, bOutputType, &m_AVSample);
	}

	void SendScanProgress(int nParam1, int nParam2)
	{
		SendStatus(VO_LIVESRC_STATUS_SCAN_PROCESS, nParam1, nParam2);
	}

	void SendChannelStopped(VO_LIVESRC_FORMATTYPE oldType, VO_LIVESRC_FORMATTYPE newType)
	{
		SendStatus(VO_LIVESRC_STATUS_CHANNEL_STOP, oldType, newType);
	}

	void SendChannelStarted(VO_LIVESRC_FORMATTYPE oldType, VO_LIVESRC_FORMATTYPE newType)
	{
		SendStatus(VO_LIVESRC_STATUS_CHANNEL_START, oldType, newType);
	}

	void SendProgramChanged(int oldProgramID, int newProgramID)
	{
		SendStatus(VO_LIVESRC_STATUS_CODEC_CHANGED, oldProgramID, newProgramID);
	}

protected:
	void SetStatusClosed() { m_nStatus = 0; }
	void SetStatusOpened() { m_nStatus = 1; }
	void SetStatusStarted() { m_nStatus = 2; }
	void SetStatusStopped() { m_nStatus = 1; }
	bool IsStatusClosed() { return m_nStatus == 0; }
	bool IsStatusOpened() { return m_nStatus >= 1; }
	bool IsStatusStarted() { return m_nStatus == 2; }
	bool IsStatusStopped() { return m_nStatus == 1; }

private:
	int m_nStatus;

protected:
	void* m_pUserData;
	void* m_pOrigUserData;
	VOSRCSENDDATA m_fSendData;
	VOSRCSTATUS	m_fStatus;
	VO_SOURCE_SAMPLE m_AVSample;
	
protected:
	CCMMBDemux *		m_pParser;
	
protected:
	VO_U32 m_nDeviceParam; //parameter for OpenDevice;
	int m_cChannel;
	CChannelInfo m_aChannelInfo[MAX_CHANNEL];
	int m_nCurrentChannel;
	int m_nDefaultChannel;

	int FindChannel(VO_LIVESRC_FORMATTYPE type);

	VO_LIVESRC_ESG_INFO	m_TotalEsgInfo;
private:
	VO_BOOL m_bEnableScan;
protected:
	void EnableScan(VO_BOOL b) { m_bEnableScan = b; }
	VO_BOOL ScanEnabled() { return m_bEnableScan; }

protected:
	VO_PTR m_fRecording;

#ifdef _DUMP_DATA
protected:
	VO_PTR m_fDump;
#endif //_DUMP_DATA

};


#endif //_MTV_ENGINE_BASE_H_
