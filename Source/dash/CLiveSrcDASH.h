#ifndef __CLiveSrcDASH_H__
#define __CLiveSrcDASH_H__

#include "voString.h"
#include "voCSemaphore.h"
#include "voLog.h"
#include "voString.h"

#include "vo_mpd_streaming.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
//#define __USE_VOSDK
#ifdef __USE_VOSDK
typedef int _vosdk_int;
typedef long long int _vosdk_int64;
#include "CvoSDK_PushSrcBase.h"
class CLiveSrcDASH: public CvoSDK_PushSrcBase
#else
typedef VO_U32 _vosdk_int;
typedef VO_U64 _vosdk_int64;
#include "CLiveSrcBase.h"
class CLiveSrcDASH : public CLiveSrcBase
#endif
{
public:
	CLiveSrcDASH(void);
	virtual ~CLiveSrcDASH(void);
#ifdef __USE_VOSDK
	virtual _vosdk_int OpenSource();
	virtual _vosdk_int SetPos(_vosdk_int64 nTimeStamp);
#else
	virtual _vosdk_int	Open(VO_PTR pUserData, VOSRCSTATUS fStatus, VOSRCSENDDATA fSendData);
	virtual _vosdk_int Seek(_vosdk_int64 nTimeStamp);

	static VO_VOID	StatusOutData (VO_VOID * pUserData, _vosdk_int id, VO_VOID *pBuffer);
	static	VO_VOID	SendOutData (VO_VOID * pUserData, VO_LIVESRC_SAMPLE * ptr_sample);
	static	VO_VOID	NotifyEvent (VO_VOID * pUserData, Event_Dash * pEvent);
#endif

	virtual	_vosdk_int	Close();
	virtual	_vosdk_int 	Start();
	virtual	_vosdk_int 	Pause();
	virtual	_vosdk_int 	Stop();
	virtual	_vosdk_int 	SetDataSource(VO_PTR pSource, _vosdk_int nFlag);
	virtual	_vosdk_int 	GetParam(_vosdk_int nParamID, VO_PTR pValue);
	virtual	_vosdk_int 	SetParam(_vosdk_int nParamID, VO_PTR pValue);

protected:
	vo_mpd_streaming *	m_pDash;
	VO_LIVESRC_SAMPLE			m_sVideoSample;
	VO_SOURCE_SAMPLE			m_videosample;
	VO_LIVESRC_SAMPLE			m_sAudioSample;
	VO_SOURCE_SAMPLE			m_audiosample;

};
#ifdef _VONAMESPACE
}
#endif

#endif //__CLiveSrcDASH_H__
