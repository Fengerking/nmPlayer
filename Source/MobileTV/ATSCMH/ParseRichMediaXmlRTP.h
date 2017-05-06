#ifndef __ParseRichMediaXmlRTP_H__
#define __ParseRichMediaXmlRTP_H__

#include "ParseRTP.h"

class CParseRichMediaXmlRTP : public CParseRTP
{
public:
	CParseRichMediaXmlRTP(CMediaStream * pMediaStream);
	virtual ~CParseRichMediaXmlRTP();

public:
	virtual VO_U32 Init();
protected:
	virtual VO_U32 ParseRTPPayload();

protected:
	unsigned int m_VersionProfile;
	unsigned int m_Level;
};

#endif //__ParseRichMediaXmlRTP_H__