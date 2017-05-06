#ifndef __PARSEH264VIDEORTP_H__
#define __PARSEH264VIDEORTP_H__

// rfc3984
#include "ParseRTP.h"

class CParseH264VideoRTP;

namespace H264VideoRTPPacketizationMode
{


class CParsePacketizationMode
{
public:
	CParsePacketizationMode(CParseH264VideoRTP * pParseH264VideoRTP);
	virtual ~CParsePacketizationMode();

public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss) = 0;

protected:
	CParseH264VideoRTP * m_pParseH264VideoRTP;
protected:
	bool   m_startOfFragmentedNALUnit;
	bool   m_lossOfFragmentedNALUnit;
	bool   m_endOfFragmentedNALUnit;
};


class CParseSingleNALUnitMode : public CParsePacketizationMode
{
public:
	CParseSingleNALUnitMode(CParseH264VideoRTP * pParseH264VideoRTP);
	virtual ~CParseSingleNALUnitMode();

public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};

class CParseNonInterleavedMode : public CParsePacketizationMode 
{
public:
	CParseNonInterleavedMode(CParseH264VideoRTP * pParseH264VideoRTP);
	virtual ~CParseNonInterleavedMode();

public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};

class CParseInterleavedMode : public CParsePacketizationMode
{
public:
	CParseInterleavedMode(CParseH264VideoRTP * pParseH264VideoRTP);
	virtual ~CParseInterleavedMode();

public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


};


class CParseH264VideoRTP : public CParseRTP
{
public:
	CParseH264VideoRTP(CMediaStream * pMediaStream);
	virtual ~CParseH264VideoRTP();

public:
	virtual VO_U32 Init();
protected:
	virtual VO_U32 ParseRTPPayload();
public:
	virtual void OnFrameStart(unsigned int rtpTimestamp);

protected:
	H264VideoRTPPacketizationMode::CParsePacketizationMode * m_pParsePacketizationMode;
};


#endif //__PARSEH264VIDEORTP_H__