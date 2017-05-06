#pragma once

#include "CvoFileParser.h"
#include "MpegFileDataStruct.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef enum
{
	VO_MPEG_PARSER_MODE_AUDIO = 0x1,
	VO_MPEG_PARSER_MODE_VIDEO = 0x2,
	VO_MPEG_PARSER_MODE_PLAYBACK = VO_MPEG_PARSER_MODE_AUDIO|VO_MPEG_PARSER_MODE_VIDEO,
	VO_MPEG_PARSER_MODE_THUMBNAIL = VO_MPEG_PARSER_MODE_VIDEO
}VO_MPEG_PARSER_MODE;

typedef struct tagMpegDataParserFrame
{
	VO_U8			btStreamID;
	VO_U16			wPacketLen;
	VO_S64			dwTimeStamp;
	VO_BOOL			beEncrpt;
	CGFileChunk*	pFileChunk;
	VO_U64			dwPESPos;
} MpegDataParserFrame, *PMpegDataParserFrame;

typedef struct tagMpegDataDrmInfo
{
	VO_PBYTE	pDrmData;
	VO_U32		btDrmDataSize;
} MpegDataDrmInfo, *PMpegDataDrmInfo;

typedef enum
{
	ParsedDataType_Normal,
	ParsedDataType_DrmInfo,
	ParsedDataType_TimeStamp,
	ParsedDataType_LastTimeStamp
}ParsedDataType;

class CMpegDataParser : 
	public CvoFileDataParser
{
public:
	CMpegDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CMpegDataParser();

public:
	virtual VO_VOID			Init(VO_PTR pParam);
	virtual VO_VOID			Reset();
	virtual VO_U32			MoveTo( VO_S64 llTimeStamp,VO_U32 dwDuration,VO_U64 ullFileSize);
	VO_PBYTE				GetStreamInfo(){return (VO_PBYTE)m_StreamIfo;}
	VO_VOID					InitParser(CMpegDataParser &Parser);
	VO_BOOL					IsDrmReady(){return m_DrmInfoReady;}
	VO_VOID					SetParserMode(VO_MPEG_PARSER_MODE eMode){m_ParserMode = eMode;}
	VO_U32					GetParserMode(){return m_ParserMode;}
	VO_MPEG_STREAM_TYPE		CheckMediaTypeByStreamID(VO_U32 nStreamID);
protected:
	virtual VO_BOOL			StepB();
	inline	VO_BOOL			ParsePES(VO_U8 btStreamID);
	inline	VO_BOOL			ParseTimeStamp(VO_U8 btValue, VO_S64* pdwTimeStamp);
	inline	VO_BOOL			ParseFrame(VO_U8 btStreamID, VO_U16 wPacketLen, VO_S64 dwTimeStamp, VO_U64 dwPESPos ,VO_BOOL beEncrpt = VO_FALSE);
	inline  VO_BOOL			ParserPSMTable();
private:
	VO_VOID					InitStreamItem(StreamInfo* pInfo);
	VO_VOID					DelStreamList();
protected:
	VO_U8					m_btPackLen;
	StreamInfo*				m_StreamIfo;
	VO_PBYTE				m_pEcmData;
	VO_BOOL					m_DrmInfoReady;
	VO_MPEG_PARSER_MODE		m_ParserMode;
};
#ifdef _VONAMESPACE
}
#endif