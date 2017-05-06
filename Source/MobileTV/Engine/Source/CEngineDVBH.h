#ifndef _MTV_ENGINE_DVBH_H_
#define _MTV_ENGINE_DVBH_H_

#include "voType.h"

#include "CEngineBase.h"
#include "CCMMBDemux.h"
#include "CSourceWrapper.h"
// #include "ConfigFile.h"

class CEngineDVBH : public CEngineBase
{
public:
	CEngineDVBH(void);
	~CEngineDVBH(void);

	virtual int Scan(VO_LIVESRC_SCAN_PARAMEPTERS* pParam);

	virtual int	SetChannel (int nChannelID);
	virtual int	GetESG (int nChannelID, VO_LIVESRC_ESG_INFO * ppESGInfo);

public:
	virtual int LoadConfig();
	virtual int SaveConfig();

	virtual int OpenDevice();
	virtual int CloseDevice();
	virtual int StartReceive();
	virtual int StopReceive();

protected:
	int				InitParser(void);
	int				ReleaseParser(void);

	void			OnSourceData(VO_RTPSRC_DATA * pData);
	static  void VO_API OnSourceDataProc(VO_RTPSRC_DATA * pData)
	{
		((CEngineDVBH*)(pData->pUserData))->OnSourceData(pData);
	}

protected:
	void	OnParsedData( VO_PARSER_OUTPUT_BUFFER * pData );
	static	void  VO_API OnParsedDataProc(VO_PARSER_OUTPUT_BUFFER* pData)
	{
		((CEngineDVBH*)(pData->pUserData))->OnParsedData(pData);
	}


private:
// 	CSourceWrapper m_source;
// 	CParseWrapper m_parser;

	CMTVSourceWrapper	m_Source;

// 	HANDLE m_hFinishEvent;
	VOSRCSENDDATA m_fOrigSendData;

	VO_PBYTE	m_pSDPData;
	
	VO_U64		m_nAudioTimeStamp;
	VO_BYTE		m_AudioExData[2];

	VO_MTV_FRAME_POS_TYPE	m_lastVideoFrameType;
	VO_MTV_FRAME_BUFFER		m_BufferedVideoFrame;

	VO_TCHAR		m_szDebugString[256];

protected:
	int BeginScan(VO_PTCHAR szParseModule);
	int EndScan();
	void ResetParser();
	void OnSendData(int nChannelID, unsigned char * pData, int nDataLen);
	static void VO_API OnSendDataEntry(void* pUserData, int nChannelID, unsigned char * pData, int nDataLen)
	{
		((CEngineDVBH*)pUserData)->OnSendData(nChannelID, pData, nDataLen);
	}

protected:
	int		VideoFrameBuffering(VO_MTV_FRAME_BUFFER * pFrame);
	int		ReadSDPData(VO_PTCHAR pPath, VO_PBYTE* pData);

	void ReleaseMemory();

};

#endif //_MTV_ENGINE_DVBH_H_
