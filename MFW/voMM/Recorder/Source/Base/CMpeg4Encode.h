#pragma once

#include "CEncode.h"
#include "voMPEG4.h"

class CMpeg4Encode : public CEncode
{
public:
	CMpeg4Encode();
public:
	virtual ~CMpeg4Encode(void);

public:
	virtual bool Init(void);
	virtual bool Close(void);

	virtual long long ReceiveSample (CSampleData * pSample, VO_IV_COLORTYPE& videoType);

	virtual bool SetEncodeQuality (int nQuality);
	virtual bool SetVideoSize (VO_U32 dwWidth, VO_U32 dwHeight);

	virtual bool GetHeaderInfo(unsigned char** pInfo, int& nInfoLen);
	virtual bool SetVideoInfo(int nFrmRate, int nKeyFrmInterval, long long nBitrate);

private:
	VO_VIDEO_ENCAPI m_Mpeg4EncFunc;
};
