	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFileMediaExtractor.h

	Contains:	CFileMediaExtractor header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifndef __CFileMediaExtractor_H__
#define __CFileMediaExtractor_H__

#include "voCMutex.h"
#include "CBaseConfig.h"

#include "CFileSource.h"
#include "CVideoDecoder.h"
#include "CAudioDecoder.h"

#include "CBaseMediaExtractor.h"
#include "CBaseMediaSource.h"

#include "voThread.h"

#define		VOSF_MEDIABUFFER_NUM	64

class CFileMediaExtractor : public CMediaBufferObserver
						  , public CBaseMediaExtractor
{
public:
	CFileMediaExtractor(void * pDataSource, int nFlag, VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP);
	virtual ~CFileMediaExtractor(void);

	virtual void		signalBufferReturned (void * pBuffer);

	virtual int			GetTrackNum (void);
	virtual void *		GetTrackSource (int nTrack);
	virtual void *		GetTrackMetaData (int nTrack, int nFlag);
	virtual void *		GetMetaData (void);
	virtual int			GetFlag (void);

	virtual	int			GetParam (int nParamID, void * pValue1, void * pValue2);
	virtual	int			SetParam (int nParamID, void * pValue1, void * pValue2);

	virtual int			Start (int nTrack, void * pParam);
	virtual int			Stop (int nTrack);
	virtual int			ReadSample (void ** ppBuffer, void * pOptions, int nTrack);

	static  int			vosfFileSourceStreamCallBack (long EventCode, long * EventParam1, long * userData);

protected:
	virtual bool		LoadSource (void);
	virtual bool		GetMetaData (void * pMetaData, int nMediaID, int nKey);
	//added by gtxia 2011-2-22 	
	bool            	getAlbumArt(void* pMedaData);
	virtual bool		CreateAudioDec (void);
	virtual bool		CreateVideoDec (void);

	virtual int			ReadAudioSample (void);
	virtual int			DecAudioSample (void);	
	
	virtual int			ReadVideoSample (void);
	virtual int			DecVideoSample (void);

	virtual void *		FillMediaBuffer (void * pData, int nSize, bool bAudio);
	virtual int			GetPlayingTime (void);

	static VO_S32		voDecodeProc (VO_PTR pParam);
	VO_S32				voDecodeLoop();

	void *				FindMediaBuffer(void *pBuf);
	bool				AddMediaBuffer(void *pBuf);
	bool				ReleaseMediaBuf();
	void				ReleaseBufList();

	// add head data for the third-party audio codec
	void addESDSFromAudioSpecificInfo(void* pMeta, const void *pHeadData, VO_S32 nHeadSize);
	void AddOggCodecInfo(void* pMeta, VO_BYTE *pHeadData, VO_S32 nHeadSize);
	void copyYUVFrame(VO_PBYTE* ppBuffer);
protected:
	VOSF_DATASOURCETYPE *	m_pDataSource;
	void **					m_ppAudioBuffer;
	void **					m_ppVideoBuffer;
	CBaseMediaSource *		m_pAudioSource;
	CBaseMediaSource *		m_pVideoSource;
	VOSFNOFITY				m_pNotifyFunc;
	void *					m_pUserData;

	char					m_szURL[1024];

	voCMutex				m_mtSource;
	voCMutex				m_mtRead;
	CBaseConfig				m_cfgPlay;

	// added by gtxia 2011-1-19
	CBaseConfig             m_cfgCodec;
	VO_FILE_OPERATOR *		m_pFileOP;

	CBaseSource *			m_pSource;
	VO_SOURCE_INFO			m_sFilInfo;
	int						m_nTrackCount;
	VO_FILE_FORMAT			m_nFF;
	long long				m_llLastSeekPos;

	int						m_nAudioTrack;
	int						m_nAudioTrackIndex;
	char					m_szAudioMIME[32];
	char					m_szAudioSect[64];
	VO_SOURCE_TRACKINFO		m_sAudioTrackInfo;
	VO_AUDIO_FORMAT			m_sAudioFormat;
	VO_SOURCE_SAMPLE		m_sAudioSample;
	VO_U32					m_nAudioMaxSize;
	VO_U32					m_nAudioReadSamples;
	VO_PBYTE				mpAudioHead;
	VO_U32					mnAudioHeadSize;

	int						m_nVideoTrack;
	int						m_nVideoTrackIndex;
	char					m_szVideoMIME[32];
	char					m_szVideoSect[64];
	VO_SOURCE_TRACKINFO		m_sVideoTrackInfo;
	VO_VIDEO_FORMAT			m_sVideoFormat;
	VO_SOURCE_SAMPLE		m_sVideoSample;
	VO_U32					m_nVideoMaxSize;
	VO_U32					m_nVideoReadSamples;

	CAudioDecoder *			m_pAudioDec;
	VO_CODECBUFFER			m_sAudioDecInBuf;
	VO_CODECBUFFER			m_sAudioDecOutBuf;
	VO_CODECBUFFER *		m_pLastAudioBuff;
	VO_U32					m_nAudioFrameSize;
	VO_U32					m_nAudioBuffTime;


	CVideoDecoder *			m_pVideoDec;
	VO_CODECBUFFER			m_sVideoDecInBuf;
	VO_VIDEO_BUFFER			m_sVideoDecOutBuf;
	VO_VIDEO_BUFFER *		m_pLastVideoBuff;
	VO_BOOL					m_bVideoYUVBuffer;
	VO_BOOL					m_bVideoYUVCopy;
	VO_PBYTE				m_pVideoYUVBuffer;
	VO_BOOL					m_bMoreVideoBuff;
	VO_BOOL					m_bFileDropFrame;
	VO_U32					m_nNotFrameBnum;
	VO_BOOL					m_bDropFrame;
	VO_BOOL					m_bFrameDropped;

	VO_U32					m_nStartSysTime;

	// check the audio playback speed
	VO_U32					m_nAudioBytesPerSec;
	VO_S32					m_nAudioPcmDataTime;
	VO_S32					m_nAudioSysStartTime;
	// record last video buffer timestamp
	VO_U32					m_nLastVideoBuffTime;

	// for streaming
	VO_U32					m_nBufferPercent;
	VO_BOOL					m_bStop;

	//use stagefright reference clock.
	VO_S64					mllClockTime;
	VO_BOOL					mbInnerClock;

	//Use a independent thread to decode
	VO_BOOL					mbUseDecThread;
	voThreadHandle			mhDecThread;
	VO_U32					mnDecThreadID;

	VO_U32					mnBufferCount;
	VO_U32					mnReadIndex;
	VO_U32					mnWriteIndex;
	VO_VIDEO_BUFFER*		mpBufferList;  //store decoder date 
	void **					mppMediaBufList;    //store StageFright MediaBuffer struct

	voCMutex				m_mtDecoder;
	VO_BOOL					mbStop;
	VO_BOOL					mbSeeking;	
	VO_BOOL					mbEOS;

	//dump media data
	VO_BOOL					mbDumpVideo;
	VO_BOOL					mbDumpAudio;
	FILE *					mhVideoFile;
	FILE *					mhAudioFile;

	//for ESDS , AVCDecoderConfigurationRecord etc.
	VO_BYTE *				mpVideoSpecData;
	
	//temp for H264 head data is 00 00 01 start
	VO_BYTE*				mpTempHD; 
};

#endif // __CFileMediaExtractor_H__
