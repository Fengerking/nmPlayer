#ifndef __STREAMCONTROLCHANNEL_H__
#define __STREAMCONTROLCHANNEL_H__

#include "StreamDataChannel.h"

class CStreamControlChannel : public CStreamDataChannel
{
public:
	CStreamControlChannel(VO_RTPSRC_INIT_INFO * pInitInfo);
	virtual ~CStreamControlChannel();

public:
	virtual VORC_RTPSRC Init(CSDPMedia * pSDPMedia);
};


#endif //__STREAMCONTROLCHANNEL_H__