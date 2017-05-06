#ifndef __PARSERTCP_H__
#define __PARSERTCP_H__

#include "voParser.h"


// RTCP Packet Type:
const unsigned char RTCP_PACKET_SR      = 200;
const unsigned char RTCP_PACKET_RR      = 201;
const unsigned char RTCP_PACKET_SDES    = 202;
const unsigned char RTCP_PACKET_BYE     = 203;
const unsigned char RTCP_PACKET_APP     = 204;

// SDES:
const unsigned char RTCP_SDES_END		= 0;
const unsigned char RTCP_SDES_CNAME		= 1;
const unsigned char RTCP_SDES_NAME		= 2;
const unsigned char RTCP_SDES_EMAIL		= 3;
const unsigned char RTCP_SDES_PHONE		= 4;
const unsigned char RTCP_SDES_LOC		= 5;
const unsigned char RTCP_SDES_TOOL		= 6;
const unsigned char RTCP_SDES_NOTE		= 7;
const unsigned char RTCP_SDES_PRIV		= 8;


class CMediaStream;

class CParseRTCP
{
public:
	CParseRTCP(CMediaStream * pMediaStream);
	virtual ~CParseRTCP();

public:
	virtual VO_U32 Init();
public:
	VO_U32 Process(const unsigned char * _rtcpData, int rtcpDataSize);

protected:
	CMediaStream    * m_pMediaStream;
};

// class CSDESItem

class CSDESItem
{
public:
	CSDESItem(unsigned char tag, unsigned char * value);
	CSDESItem(const unsigned char * SDESItemData);
	~CSDESItem();

public:
	unsigned char   SDESTag();
	unsigned char * SDESData();
	unsigned int    SDESDataSize();
	unsigned int    TotalSize();

private:
	unsigned char m_SDESData[2 + 0xFF];
};

#endif //__PARSERTCP_H__