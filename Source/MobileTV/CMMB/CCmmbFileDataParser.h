#pragma once
#include "CvoStreamFileDataParser.h"
#include "CDemultiplexer.h"
#include "voVideo.h"
#include "voAudio.h"
#include "CAnalyseH264.h"

typedef struct
{
	VO_S32				nStreamID;
	VO_S32				nDuration;
	VO_U64				nFirstTimeStamp;
	VO_VIDEO_CODINGTYPE	nVideoCodec;
	VO_AUDIO_CODINGTYPE	nAudioCodec;
}CMMB_FILE_INFO;

typedef struct
{
	VO_BOOL				bKeyFrame;
	VO_BOOL				bVideo;
	VO_PBYTE			pData;
	VO_U32				nDataLen;
	VO_U32				nTimeStamp;
}CMMB_FRAME_BUFFER;

class CCmmbFileDataParser : public CvoStreamFileDataParser, public CDmxResultReciever
{
public:
	CCmmbFileDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CCmmbFileDataParser(void);

public:
	virtual VO_VOID			Init(VO_PTR pParam);
	VO_VOID					Uninit();
	CMMB_FILE_INFO*			GetFileInfo();

	//
	virtual VO_BOOL	GetFileInfo(VO_SOURCE_INFO* pSourceInfo);
	virtual	VO_S64	GetFirstTimeStamp();
	virtual	VO_S64	Reset();

protected:
	virtual VO_U64	CheckTimestamp(VO_U64 time);

private:

public:
	// MF & MSF header
	virtual void WhenParsedMultiFrameHeader(MultiplexFrameHeader* pHeader){};
	virtual void WhenParsedMultiSubFrameHeader(MultiSubFrameHeader* pHeader){};

	// TS0 - Control table info
	virtual void WhenParsedCMCT(ContinueServiceMultiControlTable* pCMCT){};
	virtual void WhenParsedSMCT(ShortServiceMultiControlTable* pSMCT){};
	virtual void WhenParsedCSCT(ContinueServiceControlTable* pCSCT){};
	virtual void WhenParsedSSCT(ShortServiceControlTable* pSSCT){};
	virtual void WhenParsedEB(EmegencyBroadcast* pEB){};

	virtual void WhenParsedNIT(NetworkInfoTable* pNIT){};
	virtual void WhenParsedEsgBasicDescription(ESgBasicDescription* pEsgDesc){};
	virtual void WhenParsedEADesc(EncryptionAuthorizationDescTable* pDesc){};
	virtual void WhenParsedTS0(TS0Info* pTS0){};

	// AV
	virtual void WhenParsedVideoFrame(VideoUnitBuf* pVideo);
	virtual void WhenParsedAudioFrame(AudioUnitBuf* pVideo);

	// ESG
	virtual void WhenParsedProgramGuide(ProgramGuideInfo* pInfo){};
	virtual void WhenParsedEsgServiceInfo(EsgServiceInfo* pInfo){};
	virtual void WhenParsedEsgServiceAuxInfo(EsgServiceAuxInfo* pInfo){};
	virtual void WhenParsedEsgScheduleInfo(EsgScheduleInfo* pInfo){};
	virtual void WhenParsedEsgServiceParamInfo(EsgServiceParamInfo* pInfo){};
	virtual void WhenParsedEsgContentInfo(EsgContentInfo* pInfo){};
	virtual void WhenParsedEsgFinished(){};

	// CA
	virtual void WhenParsedEncryptData(CaPacket* pPacket){};

	// Error
	virtual void WhenParsedFail(int nErrorType, void* pParam){};

	// Parse whole MF Finished
	virtual void WhenParseWholeMFFinished(MultiplexFrame* pMF){};

	// Data Service
	virtual void WhenParsedXpe(XpeBuf* buf){};

	VO_VOID DumpAAC(VO_PBYTE pData, VO_U32 dwLen);

protected:
	virtual VO_BOOL			StepB();

private:
	CMMB_FILE_INFO			m_FileInfo;
	CDemultiplexer			m_Dmx;


	VO_U16				m_nFrameCurrLen;
	VO_PBYTE			m_pVideoFrameBuf;

	VO_PBYTE			m_pMfsFrame;

	CAnalyseH264		m_FrameChecker;
};

