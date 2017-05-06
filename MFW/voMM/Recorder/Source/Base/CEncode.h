#ifndef _CENCODE_H
#define _CENCODE_H

#include "voIVCommon.h"
#include "CSampleData.h"
#include "CDllLoad.h"

class CDump;

class CEncode : public CDllLoad
{
public:
	CEncode(void);
public:
	virtual ~CEncode(void);

public:
	virtual bool	Init(void)=0;
	virtual bool	Close(void)=0;

	virtual bool	Start(void);
	virtual bool	Stop(void);

	virtual long long	ReceiveSample(CSampleData * pSample, VO_IV_COLORTYPE& videoType)=0;
	virtual long long	ReceiveAudioSample(CSampleData * pSample);
	virtual bool    GetHeaderInfo(unsigned char** pInfo, int& nInfoLen)=0;

	virtual bool	SetDump(CDump * pDump);
	virtual bool	SetEncodeQuality(int nQuality) {/*m_nEncodeQuality = nQuality; */return true;}
	virtual bool	SetVideoSize(VO_U32 dwWidth, VO_U32 dwHeight) {return false;}
	virtual bool	SetAudioInfo(int nSampleRate, int nChannels, int nBits) {return false;}
	virtual bool	SetVideoInfo(int nFrmRate, int nKeyFrmInterval, long long nBitrate){return false;};

protected:
	CDump*		m_pDump;
	bool		m_bStop;

	VO_U32		m_dwVideoWidth;
	VO_U32		m_dwVideoHeight;
	VO_PBYTE		m_pEncodeData;

	VO_HANDLE	m_hCodec;
};

#endif // _CENCODE_H
