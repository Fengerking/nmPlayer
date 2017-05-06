#ifndef _CDUMP_H
#define _CDUMP_H

#include "voString.h"
#include "CSampleData.h"
#include "CDllLoad.h"

class CDump : public CDllLoad
{
public:
	CDump(void);
public:
	virtual ~CDump(void);

public:
	bool Init(void);
	bool Start(void);
	bool Stop(void);

	long long ReceiveVideoSample(CSampleData* pSample);
	long long ReceiveAudioSample(CSampleData* pSample);

	CSampleData* GetVideoSampleData();
	CSampleData* GetAudioSampleData();
	bool SetName (const TCHAR * pName);
	long long GetCurrFileSize();

	virtual long long CreateVideoTrack (int nWidth, int nHeight)=0;
	virtual long long CreateAudioTrack (int nSampleRate, int nChannels, int nBits, int nCodec)=0;
	virtual bool SetHeaderInfo(unsigned char* pInfo, int nInfoLen)=0;
	virtual TCHAR* GetFileExtName();
	virtual void SetRealFrameRate(float fRate);

protected:
	virtual bool doInit()=0;
	virtual bool doStart()=0;
	virtual bool doStop()=0;

private:
	virtual int DumpVideoSample(CSampleData* pSample)=0;
	virtual int DumpAudioSample(CSampleData* pSample)=0;

	bool DumpAudioSample(VO_U32 dwTime);
	void DumpLeft();

protected:
	bool		m_bStop;
	VO_TCHAR	m_szName[256];

	CSampleChain	m_lstVideoFull;
	CSampleChain	m_lstVideoFree;
	CSampleChain	m_lstAudioFull;
	CSampleChain	m_lstAudioFree;
	voCMutex		m_csVideoSample;
	voCMutex		m_csAudioSample;

	VO_U32	m_dwVideoLastTime;
	int		m_nErrorCode;

	unsigned long m_lVideoSampleTime[2];
	unsigned long m_lAudioSampleTime[2];
	long long m_llCurrFileSize;
	float m_nFrameRate;

	VO_PTR m_hDump;
};

#endif // _CDUMP_H
