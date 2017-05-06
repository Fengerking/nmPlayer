#pragma once
#include "RTSPClientEngine.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

/**
\brief the engine of microsoft RTSP/MMS
*/
class CASFRTSPClientEngine :
	public CRTSPClientEngine
{
public:
	CASFRTSPClientEngine(void);
	virtual ~CASFRTSPClientEngine(void);

protected:
	virtual bool	DescribeCmd(char * rtspURL);
	virtual bool	SetupCmd(bool streamingOverTCP = false);
	virtual bool    SetupCmd_MediaStream(int streamIndex); 
	virtual bool	OptionsCmd(char * rtspURL);
	virtual bool	Set_Parameter_Cmd(int paramType,void* value);
	virtual bool	PlayCmd(float startTime,float endTime=0);
	virtual bool    ParseSetupCMDResponse_Transport(char * sdpParam, int streamIndex);
	virtual bool	PauseCmd();
	virtual CMediaStream* CreateOneMediaStream(CRTSPSession* session);

private:
	

};

#ifdef _VONAMESPACE
}
#endif