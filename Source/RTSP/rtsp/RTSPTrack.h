#ifndef _RTSPTRACT_H1
#define _RTSPTRACT_H1
 
#include "filebase.h"
#include "network.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef int VOFOURCC; 
class CMediaStream;
class CRTSPClientEngine;
class CRTSPSourceEX;
/**
\brief the C++ wrapper of the C style SDK of track

The CRTSPTrack wraps the track relative SDK,such as voRTSPTrackSetParameter,voRTSPTrackGetParameter
voRTSPTrackGetSampleByIndex,etc.
*/
class CRTSPTrack
{
public:
	CRTSPTrack(CRTSPSourceEX * pRTSPSource, CRTSPClientEngine * pRTSPClientEngine);
	virtual ~CRTSPTrack(void);

public:

	/**wrapper of voRTSPTrackSetParameter*/
	VOFILEREADRETURNCODE SetParameter(LONG lID, LONG lValue);
	/**wrapper of voRTSPTrackGetParameter*/
	VOFILEREADRETURNCODE GetParameter(LONG lID, LONG * plValue);
	/**wrapper of voRTSPTrackInfo*/
	VOFILEREADRETURNCODE GetInfo(VOTRACKINFO * pTrackInfo);
	/**wrapper of voRTSPTrackGetSampleByIndex*/
	VOFILEREADRETURNCODE GetSampleByIndex(VOSAMPLEINFO * pSampleInfo);
	/**wrapper of voRTSPTrackGetNextKeyFrame*/
	int GetNextKeyFrame(int nIndex, int nDirectionFlag);
private:
	bool m_bEndOfStream;
public:
	bool  IsNearEnd(unsigned int nearValue);

	int   SetDiffTimeInPlayResp(float dwStart);

	bool  SetPosition(const DWORD dwStart,int flushFlag=-1);
	void  SetEndOfStream(bool isEnd);
	bool  GetEndofStream(){return m_bEndOfStream;};
	bool  IsBufEmpty(){return m_pMediaStream->HaveBufferedMediaFrameCount()==0;}
	DWORD GetStartTime(){return m_dwTrackStartTime;};
	void  SetStartTime(int start){m_dwTrackStartTime=start;};

public:
	
	unsigned long GetMaxFrameSize();
	/*Get the codec ID*/
	VOAVCODEC     GetCodec();
	/*Get the codec ID of VOVOFOURCC format*/
	VOFOURCC        GetCodecName();
	/*Get the duration of the track*/
	DWORD         GetTrackDuration();
	/**Get the current position of the track*/
	DWORD         GetTrackPlayPos();
	DWORD		  GetLastTimeInBuffer();
	/**check if the track is played to the end*/
	bool          IsTrackPlayedComplete();

public:
	/**associate the track to a media stream*/
	void           SetMediaStream(CMediaStream * pMediaStream);
	void		   ResetTrack();
	/**get the associated  media stream*/
	CMediaStream * GetMediaStream() { return m_pMediaStream; }
	void           SetTrackName(const char * trackName);
	char         * GetTrackName() { return m_pTrackName; } 

protected:
	char              * m_pTrackName;
	DWORD               m_dwTrackStartTime;
	DWORD               m_dwTrackCurTime;
	bool			  m_IsNeedIFrame;
	CRTSPSourceEX       * m_pRTSPSource;
	CRTSPClientEngine * m_pRTSPClientEngine;
	CMediaStream      * m_pMediaStream; 
private:
	bool			 m_nearEnd;
	int				 m_tryCount;
	int				 dump;
	int				 m_livePausePos;
	int				 m_livePauseRefTime;
	int				 m_delayTime;
	int				 m_frameNum;
	int				m_innerPosTime;
	int				 m_frames;
public:
	int			  GetOutPutFrames(){return m_frames;}
	void		  SetLivePauseRefTime(int time);
	int			  GetTrackLivePausePos(){return m_livePausePos;};
	void		  SetTrackLivePausePos(int pos);
	void		  SetDelayTime(int time);
	void		  RetrieveAllPacketsFromRTPBuffer(){m_pMediaStream->retrieveAllPacketsFromRTPBuffer();}
};


#ifdef _VONAMESPACE
}
#endif

#endif//_RTSPTRACT_H1
