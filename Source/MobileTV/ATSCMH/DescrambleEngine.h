#ifndef __DESCRAMBLEENGINE_H__
#define __DESCRAMBLEENGINE_H__

#ifdef _S3
#include "S3_Descrambler.h"
#endif //_S3

class CRTPPacket;
class CSDPMedia;

enum SCRAMBLETYPE
{
	NON_SCRAMBLE = 0,
	ISMA_SCRAMBLE = 1,
	SRTP_SCRAMBLE = 2
};

class CDescrambleEngine
{
public:
	CDescrambleEngine();
	virtual ~CDescrambleEngine();

public:
	virtual bool Init(CSDPMedia * pSDPMedia);
	virtual void UnInit();
public:
	virtual bool DescrambleRTPPacket(CRTPPacket * pRTPPacket);

private:
	enum SCRAMBLETYPE	m_scrambleType;

#ifdef _S3
private:
	bool Init_S3_Descrambler(CSDPMedia * pSDPMedia);
	void UnInit_S3_Descrambler();
private:
	bool S3_Descramble(CRTPPacket * pRTPPacket);
private:
	bool ParseSocketAddress(CSDPMedia * pSDPMedia);
private:
	S3_Descrambler * m_pS3Dsc;
	OHT_DSC_PLAYER_SocketAddress_t    m_socketAddress;
#endif //S3


};

#endif //__DESCRAMBLEENGINE_H__