#pragma once
#include "vomtpAPI.h"
#include "voLiveSource.h"
#include "CMFWBase.h"

class CLiveSourceBase;
class CRecMng;

class CMTPSDK
{
public:
	CMTPSDK(void);
	virtual ~CMTPSDK(void);

public:
	virtual VO_U32 Open (VO_MTP_OPEN_PARAM* pOpenParam);
	virtual VO_U32 Close ();
	virtual VO_U32 SetDataSource (VO_PTR pSource, VO_U32 nSourceFormat);
	virtual VO_U32 SetDrawArea (VO_U16 nLeft, VO_U16 nTop, VO_U16 nRight, VO_U16 nBottom);
	virtual VO_U32 Scan (VO_MTP_SCAN_PARAMEPTERS* pParam);
	virtual VO_U32 Cancel (VO_U32 nID);
	virtual VO_U32 Seek (VO_U64 nPosition);
	virtual VO_U32 GetPosition (VO_U64* pPosition);
	virtual VO_U32 Start ();
	virtual VO_U32 Stop ();
	virtual VO_U32 GetChannel (VO_U32* pCount, VO_MTP_CHANNELINFO** ppChannelInfo);
	virtual VO_U32 SetChannel (VO_S32 nChannelID);
	virtual VO_U32 GetESG (VO_S32 nChannelID, VO_MTP_ESG_INFO* pESGInfo);
	virtual VO_U32 AudioControl (VO_BOOL bSuspend);
	virtual VO_U32 GetVolume (VO_U16* pVolume, VO_U16* pMaxVolume);
	virtual VO_U32 SetVolume (VO_U16 nVolume);
	virtual VO_U32 RecStart (TCHAR* pRecFile);
	virtual VO_U32 RecStop ();
	virtual VO_U32 GetParam (VO_U32 nParamID, VO_PTR pParam);
	virtual VO_U32 SetParam (VO_U32 nParamID, VO_PTR pParam);

public:
	// process data from media framework
	VO_VOID ProcessMFWStatusCallback(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	VO_VOID ProcessMFWOutputCallback(MFW_OUTPUT_BUFFER* pSample);

	// process data from live source
	VO_VOID ProcessSrcStatusCallback(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	VO_VOID ProcessSrcSendDataCallback(VO_U16 nOutputType, VO_PTR pData);

private:
	VO_U32	OpenSource();
	VO_U32	CloseSource();

	VO_U32	OpenMFW();
	VO_U32	CloseMFW();

	VO_U32  ReleaseRecMng();
	VO_U32	ReleaseChannelInfo();

	VO_LIVESRC_FORMATTYPE MtpType2SrcType(VO_MTP_SOURCE_FORMAT type);
	VO_MTP_SOURCE_FORMAT SrcType2MtpType(VO_LIVESRC_FORMATTYPE type);
	VO_U32 LiveSrcErr2MtpErr(VO_U32 nLiveSrcErr);

	VO_VOID NotifyStatus(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

protected:
	CRecMng*				m_pRec;
	CMFWBase*				m_pMFW;
	CLiveSourceBase*		m_pSrc;

	VO_MTP_OPEN_PARAM		m_OpenParam;

	VO_U32					m_nChannelCount;
	VO_MTP_CHANNELINFO*		m_pChannelInfo;
};
