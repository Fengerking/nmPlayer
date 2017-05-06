	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CMediaDecoderSource.h

	Contains:	CMediaDecoderSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifndef __CMediaDecoderSource_H__
#define __CMediaDecoderSource_H__

#include "voCMutex.h"
#include "CBaseConfig.h"

#include "CFileSource.h"
#include "CVideoDecoder.h"
#include "CAudioDecoder.h"
#include "voThread.h"

#include "CBaseMediaSource.h"
#include "CMediaBufferList.h"

typedef enum
{
	SF_STATUS_UNKNOWN = 0,
	SF_STATUS_PAUSE,
	SF_STATUS_RUN ,
	SF_STATUS_SEEKING,
	SF_STATUS_STOP,
	SF_STATUS_MAX = 0xFFFFFFFF,
}PlayStatus;

class CMediaBufferList;

class CMediaDecoderSource : public CMediaBufferObserver
						  , public CBaseMediaSource
{
public:
	CMediaDecoderSource(void * pMediaSource, int nFlag, VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP);
	virtual ~CMediaDecoderSource(void);

	virtual void		signalBufferReturned (void * pBuffer);

	virtual int			Start (void * pParam);
	virtual int			Stop (void);
	virtual void *		GetFormat (void);
	virtual int			Read (void ** ppBuffer, void * pOptions);

protected:
	virtual bool		CreateAudioDec (void);
	virtual bool		CreateVideoDec (void);

	virtual int			ReadAudioSample (void * pOptions);
	virtual int			DecAudioSample (void);	
	
	virtual int			ReadVideoSample (void * pOptions);
	virtual int			DecVideoSample (void);

	virtual int			UpdateParamete();

	static VO_S32		voDecodeProc (VO_PTR pParam);
	VO_S32				voDecodeLoop();

	void *				FindMediaBuffer(void *pBuf);
	bool				AddMediaBuffer(void *pBuf);
	bool				ReleaseMediaBuf();
	void				ReleaseBufList();

	VO_U32				getFourCC();

	long long			GetClockTime();
	void				UpdateClockTime(long long llClockTime);

protected:
	CBaseConfig				m_cfgPlay;
	bool					m_bIsVideo;
	char					m_szMIME[128];

	VOSF_MEDIASOURCETYPE *	m_pMediaSource;
	void *					m_pMeidaBuffer;
	void *					m_pInputBuffer;
	long long				m_llDuration;
	long long				m_llMediaTime;
	VO_U64					m_nMediaFrameCount;

	CAudioDecoder *			m_pAudioDec;
	VO_AUDIO_FORMAT			m_sAudioFormat;
	VO_CODECBUFFER			m_sAudioDecInBuf;
	VO_CODECBUFFER			m_sAudioDecOutBuf;
	VO_CODECBUFFER *		m_pLastAudioBuff;
	VO_U32					m_nAudioFrameSize;

	CVideoDecoder *			m_pVideoDec;
	VO_VIDEO_FORMAT			m_sVideoFormat;
	VO_CODECBUFFER			m_sVideoDecInBuf;
	VO_VIDEO_BUFFER			m_sVideoDecOutBuf;
	VO_VIDEO_BUFFER *		m_pLastVideoBuff;
	VO_BOOL					m_bVideoYUVBuffer;
	VO_BOOL					m_bVideoYUVCopy;
	VO_PBYTE				m_pVideoYUVBuffer;
	VO_BOOL					m_bMoreVideoBuff;
	VO_U32					m_nVideoFrameCount;
	VO_U32					m_nCoreNumber;

	//get sequence header form source 
	int						mnSeqDataLen;
	VO_PBYTE				mpSeqData;

	VO_VIDEO_CODINGTYPE		mnDecType;

	//buffering samples
	CMediaBufferList*		m_pBufferList;
	long long				m_llReferenceTime;
	long long				m_llSystemTime;
	VO_BOOL					mbUseBuffer;
	VO_BOOL					mbDropFrames;
	VO_U32					mnDrawFrames;
	VO_U32					mnDrawBFrames;
	VO_U32					mnWaterMark;
	VO_U32					mnDropAllBFrame;

	//dump data
	VO_BOOL					mbDumpData;
	FILE *					mhDumpDataFile;
	VO_BOOL					mbDumpOutData;
	FILE *					mhDumpOutData;

	VO_U32					mnLogLevel;
	voCMutex				m_mtSource;

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

	//adjust time stamp
	VO_S64					mllLastTime;
	VO_S64					mllLastOrgTime;

	//this is reference clock
	VO_U32					mnSystemTime;

	//show whether use video memory operator allocate buffer
	VO_BOOL					mbUseVideoMemOP;

	//enable or disable deblock deinterlace
	VO_U32					mnDeblockBufCount;
	VO_U32					mnDeblockEdge;
	VO_U32					mnDeInterlace;
	VO_S32					mBufferCount[10];
};

#endif // __CMediaDecoderSource_H__