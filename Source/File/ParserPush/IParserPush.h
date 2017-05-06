#pragma once
#include "voParser.h"
#include "voString.h"

#define LOG_TAG "vo_parser_push"

class IParserPush
{
public:
	virtual ~IParserPush(void) {}

	virtual VO_U32 Open(VO_PARSER_INIT_INFO *pParma = NULL) = 0;
	virtual VO_U32 Close() = 0;
	virtual VO_U32 Reset() = 0;

	virtual VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer) = 0;
	virtual VO_U32 SetParam(VO_U32 uID, VO_PTR pParam) = 0;
	virtual VO_U32 GetParam(VO_U32 uID, VO_PTR pParam) = 0;
	
	virtual VO_U32 setDLL(VO_TCHAR* szDllFile, VO_TCHAR* szAPIName) = 0;

	virtual VO_S64 getLastTimeStamp() = 0;
	virtual VO_VOID setAddEndTimeStamp(VO_S64) = 0;
};