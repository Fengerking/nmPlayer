#ifndef _RTSPTVPARAM_H_
#define _RTSPTVPARAM_H_

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif

/*
Internal use, set param and callback functions to voRTSPEngine
*/


#define RTSPTV_ParamBase    (0x12344321)
typedef enum
{
	RTSPTV_ParamCallbacks = RTSPTV_ParamBase + 1,
	RTSPTV_ParamActionChangeChannel,

} RTSPTV_PARAMETER;


/**
 * Event type
*/
typedef enum
{
	RTSPTVEvent_InternalError,

	/* Access Status Events */
	RTSPTVEvent_AccessGranted,
	RTSPTVEvent_AccessDenied,
    RTSPTVEvent_AccessDeniedBlackout,
    RTSPTVEvent_AccessDeniedNoPermission,

	/* Network Status Events */
    RTSPTVEvent_NetworkStatusOK,
    RTSPTVEvent_NetworkAcquiring,
    RTSPTVEvent_NetworkOutOfCoverage,
    RTSPTVEvent_NetworkInterfaceDown,
    RTSPTVEvent_NetworkIncompatible,

	/* Stream Status Events */
    RTSPTVEvent_StreamStatusOK,
    RTSPTVEvent_StreamProgramActivationFailed,
    RTSPTVEvent_StreamProgramDeactivated

} RTSPTV_EVENTTYPE;


typedef enum
{
	RTSPTV_FrameAACAudio,
	RTSPTV_FrameH264Video
} RTSPTV_FrameType;

typedef struct
{
	/* RTSP engine send back events*/
	int (* EventHandler)(RTSPTV_EVENTTYPE eEventType, unsigned int nParam, void * pEventData);

	int (* PayloadReceived)(unsigned char * pPayloadData, unsigned int nDataLength); 
	int (* FrameReceived)(RTSPTV_FrameType nFrameType, unsigned char * pFrameData, unsigned int nDataLength, unsigned int nStartTime);

} RTSPTV_CALLBACK;

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif

#endif //_RTSPTVPARAM_H_