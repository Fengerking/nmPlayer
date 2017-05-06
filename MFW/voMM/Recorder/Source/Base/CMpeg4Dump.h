#ifndef _CMPEG4_DUMP_H
#define _CMPEG4_DUMP_H

#include "CDump.h"
#include "voSink.h"
#include "voFile.h"


class CMpeg4Dump : public CDump 
{
public:
	CMpeg4Dump(void);
	virtual ~CMpeg4Dump(void);

public:
	virtual long long CreateVideoTrack(int nWidth, int nHeight);
	virtual long long CreateAudioTrack(int nSampleRate, int nChannels, int nBits, int nCodec);
	virtual bool SetHeaderInfo(unsigned char* pInfo, int nInfoLen);
	virtual TCHAR* GetFileExtName();

protected:
	virtual bool doInit();
	virtual bool doStart();
	virtual bool doStop();

private:
	bool CloseDumpFile();

	virtual int DumpVideoSample(CSampleData* pSample);
	virtual int DumpAudioSample(CSampleData* pSample);

private:
	VO_SINK_OPENPARAM	m_OpenParam;
	VO_FILE_OPERATOR	m_FileOp;
	VO_SINK_WRITEAPI	m_Mpeg4FWFunc;
};

#endif //_CMPEG4_DUMP_H
