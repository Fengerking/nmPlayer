	
#ifndef __CRTSPSource2_H__
#define __CRTSPSource2_H__

//#include "rtspstreamingsource.h"
#include "filebase.h"
#define VORTSPAPI __cdecl
typedef	void* HVORTSPSOURCE;
typedef	void* HVORTSPTRACK;
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionCreate(HVOFILEREAD * ppRTSPSession);
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionOpenURL(HVOFILEREAD pRTSPSession, const char * pRTSPLink, VOFILEOP * pFileOp);
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionOpen(HVOFILEREAD * ppRTSPSession, const char * pRTSPLink, VOFILEOP * pFileOp);
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionClose(HVOFILEREAD pRTSPSession);
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionSetParameter(HVOFILEREAD pRTSPSession, LONG lID, LONG lValue) ;
VOFILEREADRETURNCODE VOFILEAPI voRTSPSessionGetParameter(HVOFILEREAD pRTSPSession, LONG lID, LONG * plValue) ;
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackOpen(HVOFILETRACK * ppRTSPTrack, HVOFILEREAD pRTSPSession, int nIndex) ;
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackClose(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack) ;
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackSetParameter(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, LONG lID, LONG lValue); 
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackGetParameter(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, LONG lID, LONG * plValue);
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackInfo(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, VOTRACKINFO * pTrackInfo);
VOFILEREADRETURNCODE VOFILEAPI voRTSPTrackGetSampleByIndex(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, VOSAMPLEINFO * pSampleInfo);
int VOFILEAPI voRTSPTrackGetNextKeyFrame(HVOFILEREAD pRTSPSession, HVOFILETRACK pRTSPTrack, int nIndex, int nDirectionFlag);

// RTSP Source Function
typedef long (VORTSPAPI * VORTSPCREATE)			(HVORTSPSOURCE *ppRTSPSource);
typedef long (VORTSPAPI * VORTSPOPENURL)			(HVORTSPSOURCE pRTSPSource,const char *pSourceFile, VOFILEOP *pFileOp);
typedef long (VORTSPAPI * VORTSPOPEN)				(HVORTSPSOURCE *ppRTSPSource, const char *pSourceFile, VOFILEOP *pFileOp);
typedef long (VORTSPAPI * VORTSPCLOSE)				(HVORTSPSOURCE pRTSPSource);
typedef long (VORTSPAPI * VORTSPSETPARAMETER)		(HVORTSPSOURCE pRTSPSource, LONG nID, LONG lValue);
typedef long (VORTSPAPI * VORTSPGETPARAMETER)		(HVORTSPSOURCE pRTSPSource, LONG nID, LONG *plValue);

// RTSP Track Function
typedef long (VORTSPAPI * VOTRACKOPEN)				(HVORTSPTRACK * ppRTSPTrack, HVORTSPSOURCE pRTSPSource, int nIndex);
typedef long (VORTSPAPI * VOTRACKCLOSE)			(HVORTSPSOURCE pRTSPSource, HVORTSPTRACK pRTSPTrack);
typedef long (VORTSPAPI * VOTRACKSETPARAMETER)		(HVORTSPSOURCE pRTSPSource, HVORTSPTRACK pRTSPTrack, LONG nID, LONG lValue);
typedef long (VORTSPAPI * VOTRACKGETPARAMETER)		(HVORTSPSOURCE pRTSPSource, HVORTSPTRACK pRTSPTrack, LONG nID, LONG *plValue);
typedef long (VORTSPAPI * VOTRACKGETINFO)			(HVORTSPSOURCE pRTSPSource, HVORTSPTRACK pRTSPTrack, VOTRACKINFO *pTrackInfo);
typedef long (VORTSPAPI * VOTRACKGETSAMPLEBYINDEX) (HVORTSPSOURCE pRTSPSource, HVORTSPTRACK pRTSPTrack, VOSAMPLEINFO *pSampleInfo);
typedef long (VORTSPAPI * VOTRACKGETSAMPLEBYTIME)	(HVORTSPSOURCE pRTSPSource, HVORTSPTRACK pRTSPTrack, VOSAMPLEINFO *pSampleInfo);
typedef int   (VORTSPAPI * VOTRACKGETNEXTKEYFRAME)	(HVORTSPSOURCE pRTSPSource, HVORTSPTRACK pRTSPTrack, int nIndex, int nDirectionFlag);
#include "../include/MKPlayer/Player.h"

class CRTSPSource2
{
public:
	CRTSPSource2();
	virtual ~CRTSPSource2(void);
public:
	virtual bool	OpenSource (const char * pFile);
	virtual bool	Close (void);

	virtual bool	Stop (void);
	virtual bool	Pause (void);
	virtual bool	Run (void);
	virtual bool	Start (void);
	virtual bool SetPos (long llStart, long llEnd);

	virtual long	RTSPSetParam (LONG nID, LONG lValue);
	virtual long	RTSPGetParam (LONG nID, LONG * plValue);

	virtual long	TrackSetParam (HVORTSPTRACK pRTSPTrack, LONG nID, LONG lValue);
	virtual long	TrackGetParam (HVORTSPTRACK pRTSPTrack, LONG nID, LONG * plValue);

	virtual long	TrackGetInfo (HVORTSPTRACK pRTSPTrack, VOTRACKINFO *pTrackInfo);
	virtual long	TrackGetSampleByIndex (HVORTSPTRACK pRTSPTrack, VOSAMPLEINFO *pSampleInfo);
	virtual int		TrackGetNextKeySample (HVORTSPTRACK pRTSPTrack, int nIndex, int nDirectionFlag);

	
protected:
	virtual bool	        InitDll (void);


protected:
	HVORTSPSOURCE		m_hSource;
	VOFILEOP			m_opFile;

	int					m_nTracks;
	HVORTSPTRACK *		m_ppTrack;
	MK_TrackInfo			 m_TrackInfo[2];
	VOSAMPLEINFO		 m_samples[2];
	MK_Sample				 m_mkSample[2];
	int			width, height;
	int			sampleRate, channels;
	unsigned char		h264Head[512];
	VOCODECDATABUFFER sps;
	VOCODECDATABUFFER pps;
	HVORTSPTRACK		GetTrackByTrackInfo(MK_TrackInfo* trackInfo)
	{
		for (int i=0;i<2;i++)
		{
			if (trackInfo->Format==m_TrackInfo[i].Format)
			{
				return m_ppTrack[i];
			}
		}
		return NULL;
	}
public:
	int GetChannelNum(){return channels;}
	int GetSampleRate(){return sampleRate;};
	int GetWidth(){return width;};
	int GetHeight(){return height;}
	MK_U32 GetSPS(MK_U8* aBuf, MK_U32 aSz)
	{
		memcpy(aBuf,sps.buffer,sps.length);
		return sps.length;
	}
	MK_U32 GetPPS(MK_U8* aBuf, MK_U32 aSz)
	{
		memcpy(aBuf,pps.buffer,pps.length);
		return pps.length;
	}
public:
	int GetTrackCount(){return m_nTracks;}
	MK_TrackInfo* GetTrackInfo(int idx){return &m_TrackInfo[idx];}
	MK_Sample* GetTrackSample(int idx)
	{
		VOSAMPLEINFO* pSample=&m_samples[idx];
		if(TrackGetSampleByIndex (m_ppTrack[idx], pSample)==0)
		{
			m_mkSample[idx].mData		=	pSample->pBuffer;
			m_mkSample[idx].mDataLen =	pSample->uSize&0x7fffffff;
			m_mkSample[idx].mTime		=	(long long)pSample->uTime;
			pSample->uIndex++;
			return &m_mkSample[idx];
		}
		else
			return NULL;
	}
protected:
	VORTSPCREATE					m_fRTSPCreate;
	VORTSPOPENURL					m_fRTSPOpenURL;
	VORTSPOPEN						m_fRTSPOpen;
	VORTSPCLOSE						m_fRTSPClose;
	VORTSPSETPARAMETER				m_fRTSPSetParam;
	VORTSPGETPARAMETER				m_fRTSPGetParam;
	VOTRACKOPEN						m_fTrackOpen;
	VOTRACKCLOSE					m_fTrackClose;
	VOTRACKSETPARAMETER				m_fTrackSetParam;
	VOTRACKGETPARAMETER				m_fTrackGetParam;
	VOTRACKGETINFO					m_fTrackGetInfo;
	VOTRACKGETSAMPLEBYINDEX			m_fGetSampleByIndex;
	VOTRACKGETSAMPLEBYTIME			m_fGetSampleByTime; 
	VOTRACKGETNEXTKEYFRAME			m_fGetNextKeyFrame;

};

#endif // __CRTSPSource2_H__