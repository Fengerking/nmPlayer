#ifndef __PARSEENCMPEG4GENERICAUDIOMODE_H__
#define __PARSEENCMPEG4GENERICAUDIOMODE_H__

#include "ParseMpeg4GenericAudioMode.h"

class CParseEncMpeg4GenericRTP;

namespace Mpeg4GenericMode
{


// mode=CELP-cbr
class CParseEncCELPcbrMode : public CParseEncMode
{
public:
	CParseEncCELPcbrMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP);
	virtual ~CParseEncCELPcbrMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


// mode=CELP-vbr
class CParseEncCELPvbrMode : public CParseEncMode
{
public:
	CParseEncCELPvbrMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP);
	virtual ~CParseEncCELPvbrMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


// mode=AAC-lbr
class CParseEncAAClbrMode : public CParseEncMode
{
public:
	CParseEncAAClbrMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP);
	virtual ~CParseEncAAClbrMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


// mode=AAC-hbr
class CParseEncAAChbrMode : public CParseEncMode
{
public:
	CParseEncAAChbrMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP);
	virtual ~CParseEncAAChbrMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


};

#endif //__PARSEENCMPEG4GENERICAUDIOMODE_H__