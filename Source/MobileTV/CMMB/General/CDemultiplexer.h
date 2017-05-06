#pragma once
#include "dmxbasicdef.h"
#include "CAnalyseH264.h"

#define MAX_MF_LEN	200*1024


typedef struct
{
	//BYTE*	pRawData;
	BYTE pRawData[MAX_MF_LEN];
	int	nCurrLen;
	int	nLeftLen;
}MfRawData;

class CDemultiplexer
{
public:
	CDemultiplexer(void);
	virtual ~CDemultiplexer(void);

public:
	virtual int Demux(BYTE* pData, int nLen, WORD wServiceId=0xffff);
	void SetReciever(CDmxResultReciever* pRecv);
	void Open();
	void Close();
	void StopForceParse();

	int  CalMfsLen(BYTE* pData);
	MultiplexFrame* GetMF(){return &m_MF;};

protected:
	int ParseMFHeader(BYTE* pData, MultiplexFrameHeader* pHeader);
	int ParseMSF(BYTE* pData, int nLen);
	int ParseMSFHeader(BYTE* pData, MultiSubFrameHeader* pHeader);

	virtual int ParseVideo(BYTE* pData, MultiSubFrameHeader* pHeader);
	int ParseVideoSegmentHeader(LPBYTE pData, VideoSegmentHeader* pHeader);
	
	virtual int ParseAudio(BYTE* pData, MultiSubFrameHeader* pHeader);
	int ParseAudioSegmentHeader(LPBYTE pData, AudioSegmentHeader* pHeader);
	
	// Mode 1
	int ParseDra(LPBYTE pData, int nLen);
	int ParseAac(LPBYTE pData, int nLen);
	int ParseAacEx(LPBYTE pData, int nLen);

	virtual int ParseData(BYTE* pData, MultiSubFrameHeader* pHeader);
	int ParseDataSegmentHeader(LPBYTE pData, DataSegmentHeader* pHeader);
	int ParseProgramGuide(BYTE* pData, WORD wUnitLen, BYTE nPacketMode);
	int ParseProgramGuide(BYTE* pData, ProgramGuideInfo* pGuide);
	int ParseEsg(BYTE* pData, WORD wUnitLen, BYTE nPacketMode);
	int ParseEsgSegment(BYTE* pData, EsgDataSegment* pSeg);

	//parse unit on mode2(include audio,video,data)
	virtual int ParseUnitMode2(BYTE* pData, BYTE nUnitType, WORD wUnitLen, void* pUnitParam, DWORD dwPrevUnitTimeStamp=0);
	int ParseFragmentMode2(BYTE* pData, Mode2FragmentHeader* pHeader, void* pOut);
	int ParseMode2VideoFragment(BYTE* pData, Mode2FragmentHeader* pHeader, FRAMEPOS nPos, VideoUnitBuf* pFrame);

	//control information
	int ParseTS0(BYTE* pData, MultiplexFrameHeader* pHeader);
	int ParseNIT(BYTE* pData, int nLen);
	int ParseCMCT(BYTE* pData, int nLen);
	int ParseSMCT(BYTE* pData, int nLen);
	int ParseEsgDesc(BYTE* pData, int nLen);
	int ParseEADesc(BYTE* pData, int nLen);
	int ParseEB(BYTE* pData, int nLen);
	int ParseCSCT(BYTE* pData, int nLen);
	int ParseSSCT(BYTE* pData, int nLen);

	// CA
	virtual int ParseEcm(BYTE* pData, int nLen, BYTE nEcmType);
	virtual int ParseEmm(BYTE* pData, int nLen, BYTE nEmmType);
	int			Decrypt(CaPacket* pPacket, BYTE* pData, int nLen, BYTE nDescramblingType, BYTE nEcmEmmType=-1);
	bool		IsEmmFrame();

	// force parse MF when CRC error or ohters
	int ForceParse(BYTE* pData, int nLen, BYTE nType, int nErrorType=DEMUX_MF_HEADER_CRC_ERROR, DWORD dwTimeStamp=0);
	int DemuxEx(BYTE* pData, int nLen, WORD wServiceId);

	int ParseXpe(BYTE* pData, int nLen);
	int ParseXpeFileMode(BYTE* pData, int nLen);
	int ParseXpe_FEC(BYTE* pData, int nLen);

protected:
	virtual bool FindStartCode(LPBYTE& pData, int& nLen);
	virtual bool GetMfLen(LPBYTE pData, int nDataLen, int& nMfLen);
	void CopyRawData(MfRawData* pRawDataBuf, BYTE* pData, int nLen);
	bool FindDraFrmFlag(LPBYTE& pData, int& nLen);

	void AssembleAudioTimeStamp();
	void AssembleVideoTimeStamp();

private:
	void ResetEsgSegment(EsgDataSegment* pSeg);
	void ResetEsgDataContainer(EsgDataContainer* pContainer);
	int CheckFrameLength(MultiplexFrameHeader* pHeader, int nLen);
	int TotalEsgParsed(EsgDataSegment* pSeg);

protected:
	CDmxResultReciever*			m_pReciever;

	DWORD						m_dwCurrVideoTimeStamp;
	DWORD						m_dwCurrAudioTimeStamp;
	DWORD						m_dwLastAudioTimeStamp;

	BYTE						m_nVideoAlgorithm;
	BYTE						m_nAudioAlgorithm;
	BYTE						m_nVideoFrameType;
	
	VideoUnitBuf				m_VideoBuf;
	AudioUnitBuf				m_AudioBuf;
	
//  EsgDataSegment				m_EsgSeg[TOTAL_ESG_DATA_TYPE+1];
	EsgDataContainer			m_EsgDataContainer[TOTAL_ESG_DATA_TYPE+1];
	EsgParseState				m_EsgParseState[TOTAL_ESG_DATA_TYPE+1];

	bool						m_bStopForceParse;
	MfRawData					m_RawData;

protected:
	MultiplexFrame				m_MF;
	int							m_nEmmMfId;

	CAnalyseH264				m_KeyFrameChecker;
};
