#ifndef __PARSEMP4ALATMRTP_H__
#define __PARSEMP4ALATMRTP_H__

#include "ParseRTP.h"

class CParseMp4aLatmRTP : public CParseRTP
{
public:
	CParseMp4aLatmRTP(CMediaStream * pMediaStream);
	virtual ~CParseMp4aLatmRTP();

public:
	virtual VO_U32 Init();
protected:
	virtual VO_U32 ParseRTPPayload();
};

#endif //__PARSEMP4ALATMRTP_H__