#ifndef __PARSEENCMPEG4GENERICVIDEOMODE_H__
#define __PARSEENCMPEG4GENERICVIDEOMODE_H__

#include "ParseMpeg4GenericMode.h"

class CParseEncMpeg4GenericRTP;

namespace Mpeg4GenericMode
{


// mode=mpeg4-video
class CParseMpeg4VideoMode : public CParseEncMode
{
public:
	CParseMpeg4VideoMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP);
	virtual ~CParseMpeg4VideoMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


// mode=avc-video
class CParseAVCVideoMode : public CParseEncMode
{
public:
	CParseAVCVideoMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP);
	virtual ~CParseAVCVideoMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


};



#endif //__PARSEENCMPEG4GENERICVIDEOMODE_H__
