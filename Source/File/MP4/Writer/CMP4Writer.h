#ifndef _CMP4Writer_H_
#define _CMP4Writer_H_

#include "voSink.h"
#include "mp4cfg.h"
#include "mp4desc.h"
#include "isomwr.h"
#include "voCMutex.h"

class CMp4WriterStream;
class CBaseAnalyseData;

//#define DUMP_VIDEO 

class CMP4Writer
{
public:
	CMP4Writer();
	~CMP4Writer();
	
	VO_U32	Open(VO_FILE_SOURCE* pFileSource , VO_SINK_OPENPARAM * pParam);
	VO_U32	Close();
	VO_U32	AddSample( VO_SINK_SAMPLE * pSample);
	VO_U32	SetParam(VO_U32 uID, VO_PTR pParam);
	VO_U32	GetParam(VO_U32 uID, VO_PTR pParam);

protected:
	VO_BOOL	HandleFVSample(VO_BYTE *pData , VO_U32 nDataLen);
	VO_BOOL HandleFASample(VO_BYTE *pData , VO_U32 nDataLen);

	VO_BOOL	AddVideoTrack();
	VO_BOOL AddAudioTrack();

	VO_BOOL	SetFormatMPEG4();
	VO_BOOL SetFormatH265();
	VO_BOOL SetFormatH264();
	VO_BOOL SetFormatH263();
	VO_BOOL SetFormatAMR();
	VO_BOOL SetFormatAMRWB();
	VO_BOOL SetFormatQCELF();
	VO_BOOL SetFormatAAC();

	VO_BOOL SetTrackFormat(Track* track, VO_U32 sampletype, VO_U32 desctype, VO_U32 descsize=0, VO_BYTE* descdata=NULL);

	VO_U32  CheckH264HeaderType(VO_U32 nLen , VO_BYTE *pData);
	VO_U32  BufToWord(VO_BYTE *pBuf){VO_U32 len = pBuf[0]; return (len << 8) + pBuf[1] ;}
	//add for write index to head of file;
	VO_BOOL CopyOpenParam(VO_FILE_SOURCE* pFileSource , VO_SINK_OPENPARAM * pParam);
	VO_BOOL ResetWriter();

	VO_BOOL	Flush(); 
protected:
	VO_BOOL     mbDetectSH;  //detect video sequence header
	VO_BOOL		mbIsFirstVideo;
	VO_BOOL		mbIsFirstAudio;
	VO_S64		mnTimeOffset;
	VO_U32		mnVideoSampleCount;
	VO_U32		mnAudioSampleCount;

	VO_U32		mVideoCodec;
	VO_U32		mAudioCodec;

	voCMutex	mmxWriter;

	VO_VIDEO_FORMAT		mVideoFormat;
	VO_AUDIO_FORMAT		mAudioFormat;

	CBaseAnalyseData *	mpVAnalyse;
	CBaseAnalyseData *	mpAAnalyse;

	CMp4WriterStream *	mpStream;
	MovieWriter	*		mpMoveWriter;
	TrackWriter*	mpVTrackWriter;
	TrackWriter*	mpATrackWriter;
	
	VO_U16		mESID;

	VO_BOOL		mbClosed;
	VO_BOOL		mbMoveWirterOpen;

	VO_S64		mllALastTime;
	VO_S64		mllVLastTime;

	//for video/audio extension data 
	VO_BYTE *	mpVExtData;
	VO_U32		mnVExtDataLen;

	VO_BYTE *	mpAExtData;
	VO_U32		mnAExtDataLen;

	//add for write index to head of file;
	VO_BOOL		mbIndexFirst;

	VO_FILE_SOURCE		mFileSource;
	VO_SINK_OPENPARAM	mOpenParam;

	VO_U32		mnMoovSize;

	VO_BOOL		mbIntraDetectIF;

	VO_U32		mnVideoChunkSize;
	VO_U32		mnAudioChunkSize;

	VO_U32		mnVideoChunkDuration;
	VO_U32		mnAudioChunkDuration;

#ifdef DUMP_VIDEO
	FILE	*mhFile;
#endif
};

#endif//_CMP4Writer_H_
