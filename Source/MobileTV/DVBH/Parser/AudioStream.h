#ifndef __AUDIOSTREAM_H__
#define __AUDIOSTREAM_H__

#include "MediaStream.h"

class CAudioStream : public CMediaStream
{
public:
	CAudioStream(VO_PARSER_INIT_INFO * pParserInitInfo);
	virtual ~CAudioStream();

public:
	virtual VO_U32 Init(CSDPMedia * pSDPMedia);

protected:
	VO_U32 NewStreamMpeg4Generic();
	VO_U32 NewStreamEncMpeg4Generic();
	VO_U32 NewStreamMp4aLatm();
};


#endif //__AUDIOSTREAM_H__