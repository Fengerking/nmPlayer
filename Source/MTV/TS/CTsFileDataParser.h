#pragma once
#include "voYYDef_TS.h"
#include "CvoStreamFileDataParser.h"
#include "CTsParseCtroller.h"



#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


#define TS_STREAM_PACKET_COUNT	200
// #define TS_STREAM_BUF_LEN	200*188
#define MAX_TIMESTAMP		0xFFFFFFFF
#define MAX_TIMESTAMP_64		0xFFFFFFFFFFFFFFFFLL

enum TS_PARSE_STATE
{
	PARSE_NONE				= 0x0001,
	PARSE_FIRST_TIMESTAMP	= 0x0002,
	PARSE_LAST_TIMESTAMP	= 0x0004,
	PARSE_PROGRAM_INFO		= 0x0008,
};

class CTsFileDataParser : public CvoStreamFileDataParser
{
public:
	CTsFileDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CTsFileDataParser(void);
	
public:
	virtual VO_VOID	Init(VO_PTR pParam);
	virtual VO_VOID	Uninit();

	//
	virtual VO_BOOL	GetFileInfo(VO_SOURCE_INFO* pSourceInfo);
	virtual	VO_S64	GetFirstTimeStamp();
	virtual	VO_S64	Reset();
	virtual VO_U16 GetPacketLen(){return m_nTsPacketLen;}

public:
	CStream*	GetStreamByIdx(VO_U32 nIdx);
    void        SetEITCallbackFun(void*  pFunc);
    void        SetOpenFlag(VO_U32 ulOpenFlag);

protected:
	virtual VO_BOOL	StepB();
	virtual VO_U64	CheckTimestamp(VO_U64 time);

private:
	static VO_VOID OnParsed(VO_PARSER_OUTPUT_BUFFER* pBuf);
	VO_VOID HandleParseResult(VO_PARSER_OUTPUT_BUFFER* pBuf);

	VO_BOOL	UpdateTimeStamp(VO_U64& nTimeStamp, VO_BOOL bVideo);
	VO_U32	Process(VO_BYTE* pData, VO_U32 nLen);
	VO_U8* FindPacketHeader(VO_U8* pData, int cbData, int packetSize);

    VO_VOID   InitAllFileDump();
    VO_VOID   CreateDumpByStreamId(VO_U32 ulPID);
	VO_VOID   DumpFrame(VO_PARSER_OUTPUT_BUFFER* pBuf);
	VO_VOID   CloseAllFileDump();
private:
	ITsParseController*	m_pTsCtrl;
	VO_SOURCE_INFO		m_FileInfo;
	VO_PBYTE			m_pTsStreamBuf;
	VO_U64				m_nFirstTimeStamp;
	VO_U64				m_nLastTimeStamp;
	VO_U64				m_nFirstVideoTimeStamp;
	VO_U64				m_nVideoCurrTimeStamp;
	VO_U64				m_nVideoPrevTimeStamp;
	VO_U64				m_nAudioCurrTimeStamp;
	VO_U64				m_nAudioPrevTimeStamp;
	VO_U32				m_nParseState;
	VO_U32				m_nReadBufLen;
	VO_U16				m_nTsPacketLen;
	VO_U32              m_ulOpenFlag;
	//
	VO_U16				m_nTsPacketSyncOffset;

#ifdef _DUMP_FRAME
    FILE*          m_pFileData[0x2000];
	FILE*          m_pFileTimeStamp_Size[0x2000];
#endif
	//
};

#ifdef _VONAMESPACE
}
#endif

