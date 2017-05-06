#ifndef __PARSEMPEG4GENERICRTP_H__
#define __PARSEMPEG4GENERICRTP_H__

// rfc3640
#include "ParseRTP.h"
#include "ParseMpeg4GenericMode.h"

using namespace Mpeg4GenericMode;

class CParseMpeg4GenericRTP : public CParseRTP
{
public:
	CParseMpeg4GenericRTP(CMediaStream * pMediaStream);
	virtual ~CParseMpeg4GenericRTP();

public:
	virtual VO_U32 Init();
protected:
	virtual VO_U32 ParseRTPPayload();

protected:
	CParseMode * m_pParseMode;
};


#endif //__PARSEMPEG4GENERICRTP_H__