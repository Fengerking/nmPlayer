#pragma once
#include "CDump.h"

class CAmrDump : public CDump
{
public:
	CAmrDump(void);
	virtual ~CAmrDump(void);

public:
	virtual long long CreateVideoTrack (int nWidth, int nHeight);
	virtual long long CreateAudioTrack (int nSampleRate, int nChannels, int nBits, int nCodec);
	virtual bool SetHeaderInfo(unsigned char* pInfo, int nInfoLen);
	virtual TCHAR* GetFileExtName();

protected:
	virtual bool doInit();
	virtual bool doStart();
	virtual bool doStop();

private:
	virtual int DumpVideoSample(CSampleData* pSample);
	virtual int DumpAudioSample(CSampleData* pSample);

private:
	VO_PTR m_pFile;

};
