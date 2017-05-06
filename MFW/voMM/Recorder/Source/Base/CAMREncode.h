#ifndef _CAMR_ENCODER_H
#define _CAMR_ENCODER_H

#include "CEncode.h"
#include "voAMRNB.h"


class CAMREncode : public CEncode
{
public:
	CAMREncode(void);
public:
	virtual ~CAMREncode(void);

public:
	virtual bool	Init (void);
	virtual bool	Close (void);

	virtual long long	ReceiveSample(CSampleData * pSample, VO_IV_COLORTYPE& videoType);
	virtual long long	ReceiveAudioSample(CSampleData * pSample);
	virtual bool	GetHeaderInfo(unsigned char** pInfo, int& nInfoLen){return true;};
protected:

private:

	int				m_nOutBufSize;
	int				m_nInBufSize;
	VO_U32			m_dwFrameTime;

	VO_MEM_OPERATOR		m_MemOp;
	VO_AUDIO_CODECAPI	m_AmrNbEncodeFunc;
};

class CPcmRawEncode : public CEncode
{
public:
	CPcmRawEncode(void){};
	virtual ~CPcmRawEncode(void){};

public:
	virtual bool	Init(void){return true;};
	virtual bool	Close(void){return true;};

	virtual long long	ReceiveSample(CSampleData * pSample, VO_IV_COLORTYPE& videoType){return 0;};
	virtual long long	ReceiveAudioSample(CSampleData * pSample);
	virtual bool GetHeaderInfo(unsigned char** pInfo, int& nInfoLen){return false;};

};

#endif //_CAMR_ENCODER_H
