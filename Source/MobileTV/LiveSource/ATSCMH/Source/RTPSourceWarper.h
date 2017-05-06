#ifndef __RTPSOURCEWARPER_H__
#define __RTPSOURCEWARPER_H__


#include "SourceEngine.h"
#include "voLiveSource.h"
#include "RTPSource.h"
#include "voParser.h"
#include "CDllLoad.h"

class CRTPSource;
class CRTPSourceWarper :public CDllLoad
{

public:
	CRTPSourceWarper();
	virtual ~CRTPSourceWarper();

public:
	VO_U32 voRtpSrcOpen(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData);
	VO_U32 voRtpSrcClose();
	VO_U32 voRtpSrcScan(VO_LIVESRC_SCAN_PARAMEPTERS* pParam);
	VO_U32 voRtpSrcCancel(VO_U32 nID);
	VO_U32 voRtpSrcStart();
	VO_U32 voRtpSrcPause();
	VO_U32 voRtpSrcStop();
	VO_U32 voRtpSrcSetDataSource(VO_PTR pSource, VO_U32 nFlag);
	VO_U32 voRtpSrcSeek(VO_U64 nTimeStamp);
	VO_U32 voRtpSrcGetChannel(VO_U32* pCount, VO_LIVESRC_CHANNELINFO** ppChannelInfo);
	VO_U32 voRtpSrcSetChannel(VO_S32 nChannelID);
	VO_U32 voRtpSrcGetESG(VO_S32 nChannelID, VO_LIVESRC_ESG_INFO** ppESGInfo);
	VO_U32 voRtpSrcGetParam(VO_U32 nParamID, VO_PTR pParam);
	VO_U32 voRtpSrcSetParam(VO_U32 nParamID, VO_PTR pParam);
protected:
	VO_U32 ReadSDPData(VO_PTCHAR pPath, VO_PBYTE* ppData);
	VO_U32 LoadRtpParser (void);
	void OnParsedData( VO_PARSER_OUTPUT_BUFFER * pData );
	static	void  VO_API OnParsedDataProc(VO_PARSER_OUTPUT_BUFFER* pData)
	{
		((CRTPSourceWarper*)(pData->pUserData))->OnParsedData(pData);
	}
	void OnSourceData(VO_RTPSRC_DATA * pData);
	static  void VO_API OnSourceDataProc(VO_RTPSRC_DATA * pData)
	{
		((CRTPSourceWarper*)(pData->pUserData))->OnSourceData(pData);
	}
private:
	VO_U32 ParserOpen(VO_PTR * ppHandle);
	VO_U32 ParserClose(VO_PTR pHandle);
	VO_U32 ParserProcess(VO_PTR pHandle, VO_PARSER_INPUT_BUFFER* pBuffer);
	VO_U32 ParserSetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
	VO_U32 ParserGetParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
#ifdef _WIN32
	void  DumpData(PBYTE pdata,DWORD dwSize);
#endif

private:
	CRTPSource*				m_RtpSource;
	VO_PARSER_API			m_ParserFunc;
	VO_PARSER_INIT_INFO		m_ParserInfo;
	VO_HANDLE				m_pParser;
	VO_BYTE					m_AudioExData[2];
	VO_RTPSRC_INIT_INFO		m_SrcInitInfo;
	VO_RTPSRCWRAP_INIT_INFO m_SrcWraperInfo;
#if defined _WIN32
	HANDLE					m_hDumpFile;
#endif
};

#endif //__RTPSOURCEWARPER_H__