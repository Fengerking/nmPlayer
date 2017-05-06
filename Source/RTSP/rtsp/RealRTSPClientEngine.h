#pragma once
#include "RTSPClientEngine.h"
enum
{
	SPT_REAL_STREAM_RULE = 1,
	SPT_REAL_STREAM_BANDWIDTH = 2,

};

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/**
\brief the engine core of the real streaming rtsp client

real rtsp streaming is different from the normal 3GPP rtsp streaming, 
It inherits from CRTSPClientEngine,and handle the special request/response fields.
*/
class CRealRTSPClientEngine :
	public CRTSPClientEngine
{
public:
	CRealRTSPClientEngine(void);
	virtual ~CRealRTSPClientEngine(void);

protected:
	virtual bool	DescribeCmd(char * rtspURL);
	virtual bool	SetupCmd(bool streamingOverTCP = false);
	virtual bool    SetupCmd_MediaStream(int streamIndex); 
	virtual bool	OptionsCmd(char * rtspURL);
	virtual bool	Set_Parameter_Cmd(int paramType,void* value);
	virtual bool	PlayCmd(float startTime,float endTime=0);
	virtual bool    ParseSetupCMDResponse_Transport(char * sdpParam, int streamIndex);
	virtual bool	PauseCmd(int waitResponseBySec=0);
	virtual int        DoReadStreamsOverTCP(int flag=0);
	virtual CMediaStream* CreateOneMediaStream(CRTSPSession* session);

private:
	char m_challenge2[64];
	char m_challenge1[64];
	char  m_checksum[48];
	bool m_bchallengeVerified;
	void real_calc_response_and_checksum();
	bool ParseChallenge1(char* responseline);
	bool ParseChallenge3(char* responseText);

};

#ifdef _VONAMESPACE
}
#endif