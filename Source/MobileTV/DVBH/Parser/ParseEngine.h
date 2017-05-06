#ifndef __PARSEENGINE_H__
#define __PARSEENGINE_H__

#include "voParser.h"
#include "list_T.h"

class CSDPParse;
class CMediaStream;

class CParseEngine
{
public:
	CParseEngine();
	~CParseEngine();

public:
	VO_U32 Open(VO_PARSER_INIT_INFO * pInitInfo);
	VO_U32 Close();
	VO_U32 Reset();
	VO_U32 Process(VO_PARSER_INPUT_BUFFER * pBuffer);
	VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	VO_U32 GetParam(VO_U32 uID, VO_PTR pParam);

protected:
	VO_U32 InitializeWithSDP(const char * _sdpText);

private:
	VO_PARSER_INIT_INFO    m_InitInfo;
private:
	CSDPParse * m_pSDPParse;
private:
	list_T<CMediaStream *> m_listMediaStream;
};


#ifdef _RTPPSR_LOG
void DumpStreamData(int channelID, void * _data, int len);
#endif //_RTPPSR_LOG

#endif //__PARSEENGINE_H__