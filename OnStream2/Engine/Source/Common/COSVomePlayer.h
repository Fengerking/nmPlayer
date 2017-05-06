#ifndef _COSVomePlayer_h
#define _COSVomePlayer_h

#include "vompType.h"
#include "vompAPI.h"
#include "COSVomeEngine.h"
#include "voOnStreamType.h"
#include "voOnStreamEngine.h"

class COSVomePlayer : public COSVomeEngine 
{
public:
	COSVomePlayer();
	virtual ~COSVomePlayer();
	
	virtual int		Init();
	virtual int		Uninit();

	virtual int		SetDataSource (void * pSource, int nFlag);
	virtual int		SendBuffer(int nSSType, VOMP_BUFFERTYPE * pBuffer);

	virtual int		GetVideoBuffer(VOMP_BUFFERTYPE ** ppBuffer);
	virtual int		GetAudioBuffer(VOMP_BUFFERTYPE ** ppBuffer);

	virtual int		Run (void);
	virtual int		Pause (void); 
	virtual int		Stop (void);
	virtual int		Flush (void);
	virtual int		GetStatus (int * pStatus);

	virtual int		GetPos (int * pCurPos);
	virtual int		SetPos (int nCurPos);
 
	virtual int		GetParam (int nID, void * pValue);
	virtual int		SetParam (int nID, void * pValue);

	virtual int		HandleEvent (int nID, void * pParam1, void * pParam2);
	virtual int		ChangEventIDEvent (unsigned int nID, void * pParam1, void * pParam2);

	static int		voReadAudio(void * pUserData, VOMP_BUFFERTYPE * pBuffer);
	static int		voReadVideo(void * pUserData, VOMP_BUFFERTYPE * pBuffer);
	static int		voSendBuffer (void * pUserData, int nSSType, VOOSMP_BUFFERTYPE * pBuffer);

	int		ReturnCode (unsigned int nRC);
	int		ReturnMPCode (unsigned int nRC);
	int		ConvertCodecOS2MP(int sCodec, int nSSType);
	void	ConvertBufOS2MP(VOOSMP_BUFFERTYPE *sBuf, VOMP_BUFFERTYPE *dBuf, int nSSType);
	int		ConvertCodecMP2OS(int sCodec, int nSSType);
	void	ConvertBufMP2OS(VOMP_BUFFERTYPE *sBuf, VOOSMP_BUFFERTYPE *dBuf, int nSSType);

	int		ConvertColorOS2MP(VOOSMP_COLORTYPE *sColor);

protected:		
	VOOSMP_BUFFERTYPE			m_nAudioBuffer;
	VOOSMP_BUFFERTYPE			m_nVideoBuffer;
	VOMP_READBUFFER_FUNC		m_fReadBufFunc;

	VOMP_LIB_FUNC				m_fLibFunc;

	VOMP_BUFFERTYPE				m_nSendBuffer;
	VOOSMP_SENDBUFFER_FUNC		m_fSendBufFunc;

	VOOSMP_LISTENERINFO			m_fListener;

	VOMP_COLORTYPE				m_nColor;

	VOMP_AUDIO_FORMAT			m_sAudioFormat;
	VOMP_VIDEO_FORMAT			m_sVideoFormat;

	VOMP_TRACK_INFO				m_sAudioTrackInfo;
	VOMP_TRACK_INFO				m_sVideoTrackInfo;

	VOMP_CPU_INFO				m_sCPUInfo;

	VOMP_PERFORMANCEDATA		m_sPerformanceData;

	VOMP_RECT					m_rcDraw;
};

#endif
