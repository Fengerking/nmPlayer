#pragma once
#include "IParserPush.h"
#include "CDllLoad.h"
#include "CDllLoad.h"
#include "CCheckTsPacketSize.h"


class CParserOriginalPush : public
	IParserPush
{
public:
	CParserOriginalPush(void);
	~CParserOriginalPush(void);

	VO_U32 Open(VO_PARSER_INIT_INFO *pParma);
	VO_U32 Close();
	VO_U32 Reset();

	VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer);
	VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	VO_U32 GetParam(VO_U32 uID, VO_PTR pParam);

	VO_U32 setDLL(VO_TCHAR *szDllFile, VO_TCHAR *szAPIName);

	VO_S64 getLastTimeStamp();
	VO_VOID setAddEndTimeStamp(VO_S64);

	void doParserProc(VO_PARSER_OUTPUT_BUFFER* pData);
protected:
	VO_U32 doProcessTS(VO_PBYTE, VO_U32);

private:
	VO_PARSER_INIT_INFO m_parser_init_info;
	VO_PTR m_parser_handle;
	CDllLoad m_dlEngine;
	VO_PARSER_API m_parser_api;

	CCheckTsPacketSize m_CheckTSPacketSize;
	VO_U16 m_nTSPacketSize;

	VO_BYTE m_Buffer[256];
	VO_S32 m_iBufLen;

	VO_S64 m_nLastTimeStamp;
	VO_S64 m_nAddendTimeStamp;
};

