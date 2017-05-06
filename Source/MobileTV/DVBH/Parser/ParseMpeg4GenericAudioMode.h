#ifndef __PARSEMPEG4GENERICAUDIOMODE_H__
#define __PARSEMPEG4GENERICAUDIOMODE_H__

#include "ParseMpeg4GenericMode.h"

namespace Mpeg4GenericMode
{


// mode=CELP-cbr
class CParseCELPcbrMode : public CParseMode
{
public:
	CParseCELPcbrMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP);
	virtual ~CParseCELPcbrMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


// mode=CELP-vbr
class CParseCELPvbrMode : public CParseMode
{
public:
	CParseCELPvbrMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP);
	virtual ~CParseCELPvbrMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


// mode=AAC-lbr
class CParseAAClbrMode : public CParseMode
{
public:
	CParseAAClbrMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP);
	virtual ~CParseAAClbrMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


// mode=AAC-hbr
class CParseAAChbrMode : public CParseMode
{
public:
	CParseAAChbrMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP);
	virtual ~CParseAAChbrMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


};



#endif //__PARSEMPEG4GENERICAUDIOMODE_H__
