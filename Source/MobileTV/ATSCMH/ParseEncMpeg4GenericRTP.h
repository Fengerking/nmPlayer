#ifndef __PARSEENCMPEG4GENERICRTP_H__
#define __PARSEENCMPEG4GENERICRTP_H__

//ISMACryp1.1

#include "ParseMpeg4GenericRTP.h"

class CParseEncMpeg4GenericRTP : public CParseMpeg4GenericRTP
{
public:
	CParseEncMpeg4GenericRTP(CMediaStream * pMediaStream);
	virtual ~CParseEncMpeg4GenericRTP();

public:
	virtual VO_U32 Init();
};


#endif //__PARSEENCMPEG4GENERICRTP_H__