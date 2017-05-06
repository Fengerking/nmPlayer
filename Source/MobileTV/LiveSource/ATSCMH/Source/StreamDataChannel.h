#ifndef __STREAMDATACHANNEL_H__
#define __STREAMDATACHANNEL_H__

#include "StreamChannel.h"

class CStreamDataChannel : public CStreamChannel
{
public:
	CStreamDataChannel(VO_RTPSRC_INIT_INFO * pInitInfo);
	virtual ~CStreamDataChannel();

public:
	virtual VORC_RTPSRC Init(CSDPMedia * pSDPMedia);
public:
	virtual VORC_RTPSRC Start();
	virtual VORC_RTPSRC Stop();
public:
	virtual VORC_RTPSRC Read();

protected:
	static const int MTUSize = 1500;
};


#endif //__STREAMDATACHANNEL_H__
