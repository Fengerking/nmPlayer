#ifndef _CQCPWriter_H_
#define _CQCPWriter_H_

#include "voSink.h"
//#include "voType.h"
#include "CvoBaseMemOpr.h"
#include "CvoBaseFileOpr.h"

typedef struct _GUID{
  VO_U32 Data1;
  VO_U16 Data2;
  VO_U16 Data3;
  VO_U8  Data4[8];
} GUID;


typedef struct tagQCP_FILE_TYPE {
	VO_U32 szRIFF;
	VO_S32 nFileSize;
	VO_U32 szType;
} QCP_FILE_TYPE, *PQCP_FILE_TYPE;

typedef struct tagQCP_FILE_FORMAT {
	VO_U32  szFormat;
	VO_S32  nFormatSize;
	VO_U8   cMajor;
	VO_U8   cMinor;
	GUID    gCodec;
	VO_U16  sVersion;
	VO_S8   szName[80];
	VO_U16  sAverageBPS;
	VO_U16  sPocketSize;
	VO_U16  sBlockSize;
	VO_U16  sSampleRate;
	VO_U16  sSampleSize;
	VO_U32  dwNumRate;
	VO_U8   cRateMapEntry[8][2];
	VO_U32  dwReserve[5];
} QCP_FILE_FORMAT, *PQCP_FILE_FORMAT;

typedef struct tagQCP_FILE_VRAT {
	VO_U32 szType;
	VO_S32 nChunkSize;
	VO_S32 nFlag;
	VO_S32 nPackets;
} QCP_FILE_VRAT, *PQCP_FILE_VRAT;

typedef struct tagQCP_FILE_CHUNK {
	VO_U32 szChunk;
	VO_S32 nChunkSize;
} QCP_FILE_CHUNK, *PQCP_FILE_CHUNK;


class CQCPWriter : public CvoBaseMemOpr, public CvoBaseFileOpr
{
  public:
	CQCPWriter(VO_MEM_OPERATOR* pMemOp, VO_FILE_OPERATOR* pFileOp);
	virtual ~CQCPWriter();
	
	VO_U32  Open(VO_FILE_SOURCE *pSource, VO_SINK_OPENPARAM * pParam);
	VO_U32  Close();
	VO_U32  SetParam(VO_U32 uID, VO_PTR pParam);
	VO_U32  GetParam(VO_U32 uID, VO_PTR pParam);
	VO_U32  WriteHeadData(void);
	VO_U32  CalculateFrames(VO_PBYTE pBuf, VO_U32 nBufLen);
	VO_U32  AddSample(VO_SINK_SAMPLE * pSample);
	VO_U32  GetFileSize(void);
	VO_S32  GetFrameSize(VO_U8 nValue);

  private:
	VO_PTR              m_hFile;
	VO_S64              mnTimeOffset;
	VO_U32              m_nDataSize;
	VO_U32              mnTotalFrames;
	VO_U32              mnFrameSize;
	VO_U32              mnHeadSize;
	VO_BOOL             mbIsCBR;
	VO_U32              mAudioCodec; 
	VO_BOOL             mbIsFirstAudio;
	VO_AUDIO_FORMAT		mAudioFormat;
	QCP_FILE_TYPE       mFileType;
	QCP_FILE_FORMAT     mFileFormat;
	QCP_FILE_VRAT       mFileVart;
	QCP_FILE_CHUNK      mFileChunk;
};

#if 0
	VO_BOOL	AddVideoTrack();
	VO_BOOL HandleFASample(VO_BYTE *pData , VO_U32 nDataLen);
	VO_BOOL	HandleFVSample(VO_BYTE *pData , VO_U32 nDataLen);
	VO_BOOL	SetFormatMPEG4();
	VO_BOOL SetFormatH264();
	VO_BOOL SetFormatH263();
	VO_BOOL SetFormatAMR();
	VO_BOOL SetFormatQCELF();
	VO_BOOL SetFormatAAC();
	VO_BOOL SetTrackFormat(ISOM::Track* track, VO_U32 sampletype, VO_U32 desctype, VO_U32 descsize=0, VO_BYTE* descdata=NULL);
	VO_BOOL		mbIsFirstVideo;
	CBaseAnalyseData *	mpVAnalyse;
	CBaseAnalyseData *	mpAAnalyse;
	CQcpWriterStream *	mpStream;
	ISOM::MovieWriter	mMoveWriter;
	ISOM::TrackWriter*	mpVTrackWriter;
	ISOM::TrackWriter*	mpATrackWriter;
	VO_U32		mESID;
	VO_BOOL		mbClosed;
#endif	/* if 0 */
	

#endif//_CQCPWriter_H_
