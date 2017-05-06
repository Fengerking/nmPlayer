#ifndef __STREAMCHANNEL_H__
#define __STREAMCHANNEL_H__

#include "vortpsrc.h"
#include "commonheader.h"

class CIPSocket;
class CSDPMedia;

class CStreamChannel
{
public:
	CStreamChannel(VO_RTPSRC_INIT_INFO * pInitInfo);
	virtual ~CStreamChannel();

public:
	virtual VORC_RTPSRC Init(CSDPMedia * pSDPMedia);
public:
	virtual VORC_RTPSRC Start();
	virtual VORC_RTPSRC Stop();
public:
	virtual VORC_RTPSRC Read() = 0;

public:
	virtual VORC_RTPSRC OnPacketArrived(void * _data, int datasize);

public:
	int GetChannelSocket();
	unsigned short GetTransportPort() { return m_transPort; }
protected:
	int ParseStreamId();

protected:
	VO_RTPSRC_INIT_INFO * m_pInitInfo;
protected:
	int m_channelID;
protected:
	CIPSocket * m_pIPSocket;
	unsigned short  m_transPort;
	unsigned char * m_buffer;
protected:
	CSDPMedia * m_pSDPMedia;
};


#ifdef _RTPSRC_LOG
void DumpStreamPacket(int channelID, void * _packet, int packetLength);
void DumpStreamPacketInfo(int channelID, int packetLength);
#endif //_RTPSRC_LOG

#endif //__STREAMCHANNEL_H__