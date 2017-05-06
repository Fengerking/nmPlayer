#pragma once
#include "CDemultiplexer.h"

typedef enum
{
	FRAME_TYPE_CONTROL_INFO=0,
	FRAME_TYPE_MULTIPLEX_SUBFRAME,
	FRAME_TYPE_ESG_DATA_FRAME,
	FRAME_TYPE_COMMAND_RESPONSE = 3,
	FRAME_TYPE_REGISTER_READ = 4,
	FRAME_TYPE_RESYNC_INFO  = 5
}FRAME_TYPE_E;

typedef enum
{
	FRAME_CTRL_INFO_SUB_TYPE_NIT=1,
	FRAME_CTRL_INFO_SUB_TYPE_CMCT,
	FRAME_CTRL_INFO_SUB_TYPE_CSCT,
	FRAME_CTRL_INFO_SUB_TYPE_SMCT,
	FRAME_CTRL_INFO_SUB_TYPE_SSCT,
	FRAME_CTRL_INFO_SUB_TYPE_ESGDT,
	FRAME_CTRL_INFO_SUB_TYPE_EADT,
	FRAME_CTRL_INFO_SUB_TYPE_EB = 0x10
}FRAME_CTRL_INFO_SUB_TYPE_E;


// #ifdef WIN32
// class __declspec(dllexport) CDemultiplexerTp : public CDemultiplexer
// #else
class CDemultiplexerTp : public CDemultiplexer
//#endif
{
public:
	CDemultiplexerTp(void);
	virtual ~CDemultiplexerTp(void);

public:
	virtual int Demux(BYTE* pData, int nLen, WORD wServiceId=0xffff);

protected:
	virtual bool FindStartCode(LPBYTE& pData, int& nLen);
	virtual bool GetMfLen(LPBYTE pData, int nDataLen, int& nMfLen);
};


// #ifdef WIN32
// class __declspec(dllexport) CDemultiplexerKTouch : public CDemultiplexer
// #else
class CDemultiplexerKTouch : public CDemultiplexer
// #endif
{
public:
	CDemultiplexerKTouch(void){};
	virtual ~CDemultiplexerKTouch(void){};

protected:
	virtual int ParseUnitMode2(BYTE* pData, BYTE nUnitType, WORD wUnitLen, void* pUnitParam, DWORD dwPrevUnitTimeStamp=0);
	virtual int ParseEcm(BYTE* pData, int nLen, BYTE nEcmType);
	virtual int ParseEmm(BYTE* pData, int nLen, BYTE nEmmType);

	virtual int ParseVideo(BYTE* pData, MultiSubFrameHeader* pHeader);
	virtual int ParseAudio(BYTE* pData, MultiSubFrameHeader* pHeader);
	// /virtual int ParseData(BYTE* pData, MultiSubFrameHeader* pHeader);


};
