#ifndef _CTS_PARSER_H
#define _CTS_PARSER_H

#include "voYYDef_TS.h"
#include "BaseParser.h"
#include "tsparse.h"
#include "FormatInit.h"

#include "CDumper.h"
#include "voCMutex.h"
#include "voCSemaphore.h"

using namespace TS;

#define _STREAM_CHECK_NORMAL_


class CTSParser 
	: public CBaseTSParser
	, public IPESListener
	, public IErrorListener
	, public IParserListener
{
public:
	CTSParser(int parseType);
	~CTSParser(void);

	VOSTREAMPARSERETURNCODE Process(int channel, void* pData, int nSize, int* pProcessed);
	VOSTREAMPARSERETURNCODE Reset();
	VOSTREAMPARSERETURNCODE Reset2();
	VOSTREAMPARSERETURNCODE Query();
	VOSTREAMPARSERETURNCODE SelectProgram(int programId);
	VOSTREAMPARSERETURNCODE SetParserType(int type);

	//add by qichaoshen @ 2011-10-28
	int   IsPlaybackReady();
	void  DoForcePlayAction();
	//add by qichaoshen @ 2011-10-28

//Implement IParserListener
//----------------------------------------
public:
	virtual void OnProgramIndexReady(TransportStreamInfo* tsi);
	virtual void OnProgramInfoReady(TransportStreamInfo* tsi);

	void WaitParseFinish();
	void BreakParse(){m_parser.BreakParse();};
	void SetEITCallbackFun(void*  pFunc);
    void SetOpenFlag(VO_U32 ulOpenFlag);
protected:
	void OutputProgramInfo(TransportStreamInfo* tsi);

//Implement IErrorListener
//----------------------------------------
	virtual void OnLosePacket(RawPacket* packet,int nErrorCode);

private:
	GeneralParser m_parser;
	CFormatInitializer* m_initializer;
	voCMutex m_csInitializer;
	PESDataDispatcher m_dispatcher;

	//add by qichaoshen @ 2011-10-28
	int   m_iCanPlayback;
	//add by qichaoshen @ 2011-10-28

    uint32     m_ulOpenFlag;

#if defined( _WRITE_LOG_ ) || defined( _PRINT_LOG_)
	CDumper		m_Dump;
#endif

	bool				m_bOutputed;
	voCSemaphore		m_hEnventParsing;
	bool				m_bParsing;


//Implement IPESListener
//----------------------------------------
public:
	virtual void OnElementInfo(int total, int playbackable);
	virtual void OnElementStream(ESConfig* escfg);
	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet);
	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData);

protected:
	void CheckStreamReady(ESConfig* escfg);
	void CheckStreamReadyNormal();

#ifdef _STREAM_CHECK_NORMAL_
	int	m_nStreamReadyCount;
#endif

};
#endif // _CTS_PARSER_H
