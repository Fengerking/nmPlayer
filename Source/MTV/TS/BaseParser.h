#ifndef _CBASE_PARSER_H
#define _CBASE_PARSER_H

#include "voYYDef_TS.h"
#include "TsParserBaseDef.h"
#include "voAudio.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CBaseTSParser
{
public:
	CBaseTSParser(void) 
	: m_pListener(0)
	{ }
	~CBaseTSParser(void) {}

public:
	void SetListener(VOSTREAMPARSELISTENER* listener) { m_pListener = listener; }

protected:
	VOSTREAMPARSELISTENER* m_pListener;

protected:
	inline void NewStreamMP3(int streamid, int channels, int samplerate, int maxframelen);
};



void CBaseTSParser::NewStreamMP3(int streamid, int channels, int samplerate, int maxframelen)
{
	
	VOSTREAMPARSESTREAMINFO info;
	info.is_video = false;
	info.id = streamid;
	info.codec = VO_AUDIO_CodingMP3;
	info.audio.channels = channels;
	info.audio.sample_rate = samplerate;
	info.audio.sample_bits = 16;
	info.max_frame_len = maxframelen;
	m_pListener->OnNewStream(&info);
	
}
#ifdef _VONAMESPACE
}
#endif

#endif // _CBASE_PARSER_H
