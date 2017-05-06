#ifndef __MEDIASTREAM_H__
#define __MEDIASTREAM_H__

#include "voParser.h"
#include "netbase.h"

class CSDPMedia;
class CParseRTP;
class CParseRTCP;
class CSynchronizeStreams;

struct STREAMINFO
{
	VO_U32 nStreamId;
	VO_U32 nMediaType;
	VO_U32 nCodecType;
	VO_U32 nMaxFrameSize;
	VO_VOID* pExtraData;
	VO_U16 nExtraSize;
};

class CMediaStream
{
	friend class CSynchronizeStreams;

public:
	CMediaStream(VO_PARSER_INIT_INFO * pParserInitInfo);
	virtual ~CMediaStream();

public:
	virtual VO_U32 Init(CSDPMedia * pSDPMedia);
	virtual VO_U32 Process(VO_PARSER_INPUT_BUFFER * pBuffer);

public:
	VO_PARSER_INIT_INFO * GetParserInitInfo();
	STREAMINFO * GetStreamInfo();
	CSDPMedia * GetSDPMediaDescription();
public:
	int GetStreamId();
protected:
	int ParseStreamId();

protected:
	VO_PARSER_INIT_INFO   * m_pParserInitInfo;
	STREAMINFO   m_streamInfo;
protected:
	CSDPMedia * m_pSDPMedia;
protected:
	CParseRTP  * m_pParseRTP;
	CParseRTCP * m_pParseRTCP;

/////////////// Synchronize Media Stream ////////////////////
public:
	unsigned int          m_clockRate;
protected:
	unsigned int		  m_syncCount;
protected:
	struct timeval        m_syncWallClockTime;
	unsigned int          m_syncTimestamp;
protected:
	struct timeval        m_wallClockTime;
	unsigned int          m_timestamp;
};


#endif //__MEDIASTREAM_H__