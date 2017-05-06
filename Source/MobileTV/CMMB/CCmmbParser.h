#pragma once

#include "CBaseParser.h"
#include "CDemultiplexer.h"
#include "voCMMBParser.h"

class CCmmbParser	:	public CBaseParser, public CDmxResultReciever
{
public:
	CCmmbParser(VO_MEM_OPERATOR* pMemOp);
	virtual ~CCmmbParser(void);

public:
	virtual VO_U32 Open(VO_PARSER_INIT_INFO* pParam);
	virtual	VO_U32 Close();
	virtual	VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer);

protected:
	virtual VO_U32 doSetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 doGetParam(VO_U32 uID, VO_PTR pParam);

	virtual VO_BOOL CheckHeadData(VO_U32 nCodecType, VO_BYTE* pData, VO_U32 nLen);


	//
public:
	// MF & MSF header
	virtual void WhenParsedMultiFrameHeader(MultiplexFrameHeader* pHeader);
	virtual void WhenParsedMultiSubFrameHeader(MultiSubFrameHeader* pHeader);

	// TS0 - Control table info
	virtual void WhenParsedCMCT(ContinueServiceMultiControlTable* pCMCT){};
	virtual void WhenParsedSMCT(ShortServiceMultiControlTable* pSMCT){};
	virtual void WhenParsedCSCT(ContinueServiceControlTable* pCSCT){};
	virtual void WhenParsedSSCT(ShortServiceControlTable* pSSCT){};
	virtual void WhenParsedEB(EmegencyBroadcast* pEB){};

	virtual void WhenParsedNIT(NetworkInfoTable* pNIT){};
	virtual void WhenParsedEsgBasicDescription(ESgBasicDescription* pEsgDesc){};
	virtual void WhenParsedEADesc(EncryptionAuthorizationDescTable* pDesc){};
	virtual void WhenParsedTS0(TS0Info* pTS0);

	// AV
	virtual void WhenParsedVideoFrame(VideoUnitBuf* pVideo);
	virtual void WhenParsedAudioFrame(AudioUnitBuf* pVideo);

	// ESG
	virtual void WhenParsedProgramGuide(ProgramGuideInfo* pInfo);
	virtual void WhenParsedEsgServiceInfo(EsgServiceInfo* pInfo);
	virtual void WhenParsedEsgServiceAuxInfo(EsgServiceAuxInfo* pInfo);
	virtual void WhenParsedEsgScheduleInfo(EsgScheduleInfo* pInfo);
	virtual void WhenParsedEsgServiceParamInfo(EsgServiceParamInfo* pInfo);
	virtual void WhenParsedEsgContentInfo(EsgContentInfo* pInfo);
	virtual void WhenParsedEsgFinished();

	// CA
	virtual void WhenParsedEncryptData(CaPacket* pPacket);

	// Error
	virtual void WhenParsedFail(int nErrorType, void* pParam);

	// Parse whole MF Finished
	virtual void WhenParseWholeMFFinished(MultiplexFrame* pMF);

	// Data Service
	virtual void WhenParsedXpe(XpeBuf* buf);

private:
	CDemultiplexer		m_Dmx;

	VO_U16				m_nFrameCurrLen;
};
