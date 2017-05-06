#pragma once

#include "CBasePSI.h"
#include "CBaseAssembler.h"
#include "voVideoParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define MAX_PID_COUNT			256
#define program_map_PID_base	0x0100
#define elementary_PID_base		0x0101
#define PCR_PID_BASE			0x1001

#define DEFAULT_PES_HEADER_FREQUENCE 16
#define DEFAULT_PES_PAYLOAD_SIZE ((DEFAULT_PES_HEADER_FREQUENCE - 1) * 184 + 170)
#define DEFAULT_PAT_PMT_PERIOD 40

enum Psi_Type
{
	Psi_Type_PAT,
	Psi_Type_PMT
};
class CTsPacket;
struct  ContinuityCounter
{
	VO_U16		pid;
	VO_BYTE		continuity_counter;
};

VO_S64 AVRescaleRound(VO_S64 arg1, VO_S64 arg2, VO_S64 arg3, VO_U32 uRound);

class CTsAssembler : public CBaseAssembler
{
public:
	CTsAssembler(void);
	virtual ~CTsAssembler(void);

public:
	virtual VO_VOID	Open(ASSEMBLERINITINFO* pInit);
	virtual VO_VOID	Close();
	virtual VO_VOID	AddExtData(VO_U16 nStreamID, VO_SINK_EXTENSION* pExtData);
	VO_VOID	AddSample(VO_BOOL bSync, VO_SINK_SAMPLE * pSample);
	VO_U32	AssembleBasicTable(VO_BOOL bForce = VO_FALSE);
	VO_U32	GetDefaultPESSize(){ return m_uDefultPESPayloadSize; }
	VO_U32	FormatChange();
protected:
	virtual VO_VOID	doAddSample(VO_BOOL bSync, VO_SINK_SAMPLE * pSample);
//	virtual VO_VOID	doAddNonPCRSample(VO_BOOL bSync, VO_SINK_SAMPLE * pSample);
	VO_VOID	Flush(){}
protected:
	
	VO_U32	AssembleTsPacketHeader(VO_U16 pid, VO_BYTE payload_unit_start_indicator, VO_BOOL bDataByte, VO_S16 adaptation_field_length,VO_BOOL bPCRPacket,VO_S64 DTS = 0);
	VO_VOID	SetPAT();
	VO_VOID	SetPMT();
	VO_U32	AssemblePAT();
	VO_U32	AssemblePMT();
	VO_U32	AssembleCommonPSI(CTableHeader* pSection,Psi_Type eType);
	VO_U32	AssemblePesHeader(CPESPacket* pPes);
	VO_U32	AssembleH264Prefix(VO_PBYTE pSampleData ,VO_U32 uLen);
	VO_BOOL	HasH264Prefix(VO_PBYTE pSampleData ,VO_U32 uLen);
	VO_BOOL	HasH264HeadData(VO_PBYTE pSampleData ,VO_U32 uLen);

//	VO_U32	AssemblePCRPacket(VO_U64 nTimestamp);

	VO_U8	GetStreamType(VO_U32 nStreamID,VO_U16 nCodec);
	VO_U8	GetStreamID(VO_U32 nStreamID,VO_U16 nCodec);

	VO_VOID	InitContinuityCounter();
	VO_VOID	RegisterContinuityCounter(VO_U16 pid);
	VO_BYTE	GetContinuityCount(VO_U16 pid);

	VO_U16	CalcPesPacketLength(VO_BOOL bSync, VO_BOOL bVideo, VO_U16 nCodec, VO_U32 nFrameLength, VO_BYTE nTotalTsPacketSize, VO_BYTE nPesHeaderLen);
	VO_U16	PreparePesHeader(CPESPacket* pPes, VO_U8 nStreamID, VO_U64 nPTS, VO_U64 nDTS,VO_U32 uPayLoadSize);
	VO_U16	PrepareAdtsHeader(VO_BYTE* pHeader, VO_BYTE* pExtraData, VO_U16 nFrameSize);

	VO_VOID SetPesPacketLength(VO_U16 nLen);

private:
	VO_VOID	WriteTsPacket();
	VO_BOOL IsADTSSyncWord(VO_BYTE* p);

	VO_VOID CheckHeadData(VO_BOOL bAudio, VO_U16 nStreamID, VO_U16 nCodec, VO_U32 nFrameSize, VO_BYTE** outHead, VO_U16* outHeadLen);
	VO_U32 EnCodePCMHead(VO_PBYTE pData,VO_U32 nSize);
	VO_PBYTE FindH264SyncPos(VO_PBYTE pData , VO_U32 uLen,VO_U32* uSyncWord);
protected:
	CTsPacket*	m_pTsPacekt;

	CPat		pat;
	CPmt		pmt;
	CPESPacket	pes;
	ContinuityCounter	m_continuity_counter[MAX_PID_COUNT];
	VO_BYTE				m_continuity_counter_count;
	VO_BYTE				m_AdtsHeader[7];
	VO_BOOL				m_bFirstSample;
	VO_U32				m_uDefultPESPayloadSize;
	VO_U32				m_uBasicPeriod;
	VO_VIDEO_PARSERAPI	m_funH264VideoParser;
	VO_HANDLE			m_pH264VideoParser;
	VO_PBYTE			m_pBufData;
	VO_U32				m_uBufLen;
};

#ifdef _VONAMESPACE
}
#endif

