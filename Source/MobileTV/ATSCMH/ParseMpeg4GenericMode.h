#ifndef __PARSEMPEG4GENERICMODE_H__
#define __PARSEMPEG4GENERICMODE_H__

#include "voParser.h"

class CRTPPacket;
class CSDPMedia;
class CParseMpeg4GenericRTP;
class CParseEncMpeg4GenericRTP;

namespace Mpeg4GenericMode
{


class CParseMode
{
public:
	CParseMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP);
	virtual ~CParseMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss) = 0;

protected:
	CParseMpeg4GenericRTP * m_pParseMpeg4GenericRTP;
protected:
	bool m_startOfFragmentedAccessUnit;
	bool m_lossOfFragmentedAccessUnit;
	bool m_endOfFragmentedAccessUnit;
protected:
	unsigned short m_maxAUHeadersLength;
	unsigned char * m_AUHeaders;
	unsigned short m_AUHeadersLength;
protected:
	unsigned int m_sizeLength;
	unsigned int m_indexLength;
	unsigned int m_indexDeltaLength;
	unsigned int m_CTSDeltaLength;
	unsigned int m_DTSDeltaLength;
	unsigned int m_randomAccessIndication;
	unsigned int m_streamStateIndication;
};


class CParseEncMode : public CParseMode
{
public:
	CParseEncMode(CParseEncMpeg4GenericRTP * pParseEncMpeg4GenericRTP);
	virtual ~CParseEncMode();
public:
	virtual VO_U32 Init();

protected:
	char * m_ISMACrypCryptoSuite;
	unsigned int m_ISMACrypIVLength;
	unsigned int m_ISMACrypDeltaIVLength;
	unsigned int m_ISMACrypSelectiveEncryption;
	unsigned int m_ISMACrypKeyIndicatorLength;
	unsigned int m_ISMACrypKeyIndicatorPerAU;
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)	
	unsigned __int64 m_ISMACrypSalt;
#elif defined(LINUX)
	__int64 m_ISMACrypSalt;
#endif 	

	char * m_ISMACrypKey;
	unsigned int m_ISMACrypKMSID;
	unsigned int m_ISMACrypKMSVersion;
	char * m_ISMACrypKMSSpecificData;
};


// mode=generic
class CParseGenericMode : public CParseMode
{
public:
	CParseGenericMode(CParseMpeg4GenericRTP * pParseMpeg4GenericRTP);
	virtual ~CParseGenericMode();
public:
	virtual VO_U32 Init();
public:
	virtual VO_U32 ParseRTPPayload(CRTPPacket * pRTPPacket, bool packetLoss);
};


};


#endif //__PARSEMPEG4GENERICMODE_H__
