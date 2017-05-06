#ifndef __VIDEOSTREAM_H__
#define __VIDEOSTREAM_H__

#include "MediaStream.h"

class CVideoStream : public CMediaStream
{
public:
	CVideoStream(VO_PARSER_INIT_INFO * pParserInitInfo);
	virtual ~CVideoStream();

public:
	virtual VO_U32 Init(CSDPMedia * pSDPMedia);

protected:
	VO_U32 NewStreamH264();
	VO_U32 NewStreamEncMpeg4Generic();
	VO_U32 NewStreamRichMediaXml();
};


#endif //__VIDEOSTREAM_H__