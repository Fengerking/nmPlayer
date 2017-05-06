#ifndef _LIVE_SRC_BASE_H_
#define _LIVE_SRC_BASE_H_

	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2010			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CLiveSrcBase.h

	Contains:	Live Source Base Class

	Written by:	Lin Jun

	Change History (most recent first):
	2010-10-10		Jun			Create file

*******************************************************************************/


#include "voLiveSource.h"
#include "CLiveParserBase.h"
#include "ChannelInfo.h"
#include "vompType.h"

const VO_U16 MAX_CHANNEL = 128;

// used by parser output callback,each source subclass can set it in doInitParser
void OnParsedDataProc(VO_PARSER_OUTPUT_BUFFER* pData);

class CLiveSrcBase
{
public:
	CLiveSrcBase();
	virtual ~CLiveSrcBase();

public:
	virtual VO_U32 Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData);
	virtual	VO_U32 Close();
	virtual	VO_U32 Scan(VO_LIVESRC_SCAN_PARAMEPTERS* pParam);
	virtual	VO_U32 Cancel(VO_U32 nID);
	virtual	VO_U32 Start();
	virtual	VO_U32 Pause();
	virtual	VO_U32 Stop();
	virtual VO_U32 SendBuffer(VOMP_BUFFERTYPE * pBuffer);
	virtual	VO_U32 SetDataSource(VO_PTR pSource, VO_U32 nFlag);
	virtual	VO_U32 Seek(VO_U64 nTimeStamp);
	virtual	VO_U32 GetChannel(VO_U32 * pCount, VO_LIVESRC_CHANNELINFO ** ppChannelInfo);
	virtual	VO_U32 SetChannel(VO_S32 nChannelID);
	virtual	VO_U32 GetESG(VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** pESGInfo);
	virtual	VO_U32 GetParam(VO_U32 nParamID, VO_PTR pValue);
	virtual	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pValue);

protected:
	virtual VO_U32 doOpenDevice();
	virtual VO_U32 doCloseDevice();
	virtual VO_U32 doStartRecieve();
	virtual VO_U32 doStopRecieve();

	virtual VO_U32 doInitParser();
	virtual VO_U32 doUninitParser();
	virtual	VO_U32 doGetParam(VO_U32 nParamID, VO_PTR pValue);
	virtual VO_U32 doSetParam(VO_U32 nParamID, VO_PTR pValue);

	virtual VO_U32 GetSignalStrength();

	// subclass MUST call this func to parse data when data output from tunner, refer to VO_PARSER_INPUT_BUFFER
	virtual VO_U32 OnRecvData(VO_PARSER_INPUT_BUFFER* pRecvedData);

protected:
	VO_U16	GetChannelNum();
	VO_U32	RecordStart(VO_PTCHAR pPath);
	VO_U32  RecordWrite(VO_PBYTE pData, VO_U32 nDataLen);
	VO_U32	RecordEnd();

	VO_VOID NotifyStatus(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);


public:
	// parser callback func
	virtual void OnParsedData(VO_PARSER_OUTPUT_BUFFER * pData);
protected:
	virtual VO_BOOL doOnParsedData(VO_PARSER_OUTPUT_BUFFER * pData);

protected:
	VO_VOID SendStatus(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	VO_VOID SendData(VO_U32 nOutputType, VO_PBYTE pData, VO_U32 nDataLen, VO_U64 llTimeStamp, VO_U32 nCodecType, VO_U32 nStreamId, VO_BOOL bKeyFrame);
	VO_VOID SendScanProgress(VO_U32 nParam1, VO_U32 nParam2);
	VO_VOID SendChannelStopped(VO_LIVESRC_FORMATTYPE oldType, VO_LIVESRC_FORMATTYPE newType);
	VO_VOID SendChannelStarted(VO_LIVESRC_FORMATTYPE oldType, VO_LIVESRC_FORMATTYPE newType);
	VO_VOID SendProgramChanged(VO_U32 oldProgramID, VO_U32 newProgramID);

private:
	VO_U32 LoadConfig(){return VO_ERR_LIVESRC_NOIMPLEMENT;};

protected:
	VO_PTR				m_pUserData;
	VO_PTR				m_pOrigUserData;
	VOSRCSENDDATA		m_fSendData;
	VOSRCSTATUS			m_fStatus;

	CLiveParserBase*	m_pParser;
	
protected:
	VO_U32				m_cChannel;
	CChannelInfo		m_aChannelInfo[MAX_CHANNEL];
	VO_U32				m_nCurrentChannel;
	VO_U32				m_nDefaultChannel;


	VO_LIVESRC_ESG_INFO	m_TotalEsgInfo;

protected:
	VO_VOID EnableScan(VO_BOOL b) { m_bEnableScan = b; }
	VO_BOOL ScanEnabled() { return m_bEnableScan; }
	VO_U32 FindChannel(VO_LIVESRC_FORMATTYPE type);

	VO_VOID SetStatusClosed() { m_nStatus = 0; }
	VO_VOID SetStatusOpened() { m_nStatus = 1; }
	VO_VOID SetStatusStarted() { m_nStatus = 2; }
	VO_VOID SetStatusStopped() { m_nStatus = 1; }
	VO_BOOL IsStatusClosed() { return m_nStatus == 0 ? VO_TRUE:VO_FALSE; }
	VO_BOOL IsStatusOpened() { return m_nStatus >= 1 ? VO_TRUE:VO_FALSE; }
	VO_BOOL IsStatusStarted() { return m_nStatus == 2 ? VO_TRUE:VO_FALSE; }
	VO_BOOL IsStatusStopped() { return m_nStatus == 1 ? VO_TRUE:VO_FALSE; }

protected:
	VO_BOOL	m_bStreamInfoReady;
private:
	VO_BOOL	m_bEnableScan;
	VO_U32	m_nStatus;
	VO_PTR	m_fRecording;
	VO_BOOL m_bEnableRecvData;
	
	VO_U32 m_nVideoType;
	VO_U32 m_nAudioType;
//	VO_BOOL	m_bRecvFirstVideoFrame;
	
	VO_BOOL m_bFirstKeyFSend;
};


#endif //_LIVE_SRC_BASE_H_
