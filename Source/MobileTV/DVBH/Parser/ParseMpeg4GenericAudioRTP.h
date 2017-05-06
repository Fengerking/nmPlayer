#ifndef __PARSEMPEG4GENERICAUDIORTP_H__
#define __PARSEMPEG4GENERICAUDIORTP_H__

// rfc3640
#include "ParseRTP.h"
#include "ParseMpeg4GenericMode.h"

using namespace Mpeg4GenericMode;

class CParseMpeg4GenericAudioRTP : public CParseRTP
{
public:
	CParseMpeg4GenericAudioRTP(CMediaStream * pMediaStream);
	virtual ~CParseMpeg4GenericAudioRTP();

public:
	virtual VO_U32 Init();
protected:
	virtual VO_U32 ParseRTPPayload();

protected:
	CBaseParseMode * m_pBaseParseMode;
};


#endif //__PARSEMPEG4GENERICAUDIORTP_H__
