#ifndef _TS_PARSE_H_
#define _TS_PARSE_H_

#include "voYYDef_TS.h"
#include "tsbspas.h"
#include "TsParserBaseDef.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

namespace TS {

class IParserListener
{
public:
	virtual void OnProgramIndexReady(TransportStreamInfo* tsi) {}
	virtual void OnProgramInfoReady(TransportStreamInfo* tsi) {}
};

typedef enum
{
	DETECTED_PAT = 1,
	DETECTED_PMT = 2,
	DETECTED_SDT = 4,
	DETECTED_NIT = 8,
	DETECTED_EIT = 0x10,
	DETECTED_PES = 0x20,
	DETECTED_NONE = 0
}DETECTED_TYPE;

class GeneralParser
	: public IPATListener
	, public IPMTListener
	, public ISDTListener
	, public INITListener
	, public IEITListener
	, public IRawListener
	, public ITDTListener
	, public IMGTListener
{
public:
	GeneralParser(IParserListener* listener, int type=PARSE_PLAYBACK2);
	~GeneralParser();
	void Reset();
	void Reset2();
	inline bool Process(uint8* pData, int32 cbData, int32* pcbProcessed);
	inline bool SetPIDFilter(int count, bit13* PIDs);
	inline TransportStreamInfo*  GetTransportStreamInfo();
	bool SelectProgram(bit16 programId);
	void SetEITCallbackFun(void*  pFunc);

public:
	void AddListener(IRawListener* obj) { rawListeners.Add(obj); }
	void AddListener(IPATListener* obj) { patListeners.Add(obj); }
	void AddListener(IPMTListener* obj) { pmtListeners.Add(obj); }
	void AddListener(INITListener* obj) { nitListeners.Add(obj); }
	void AddListener(IEITListener* obj) { eitListeners.Add(obj); }
	void AddListener(ISDTListener* obj) { sdtListeners.Add(obj); }
	void AddListener(IMGTListener* obj) { mgtListeners.Add(obj); }
	//void AddListener(IPESListener* obj) { pesListeners.Add(obj); }
	//void SetListener(IPESListener* obj) { pesListeners.Set(obj); }
	void SetListener(IPESListener* obj) { pesParser.SetListener(obj); }
	void SetErrorListener(IErrorListener* listener) { rawParser.SetErrorListener(listener); }

	virtual void OnRawPacket(RawPacket* packet);
	virtual void OnPAT(PAT* pat);
	virtual void OnPMT(PMT* pmt); 
	virtual void OnSDT(SDT* sdt);
	virtual void OnNIT(NIT* nit); 
	virtual void OnEIT(EIT* eit);
	virtual void OnTDT(TDT* eit);
    virtual void OnMGT(MGT* mgt);

protected:
	TransportStreamInfo ts;

	RawParser rawParser;
	PATParser patParser;
	PMTParser pmtParser;
	SDTParser sdtParser;
	NITParser nitParser;
	EITParser eitParser;
	PESParser pesParser;
	TDTParser tdtParser;
	MGTParser mgtParser;

	IRawListeners rawListeners;
	IPATListeners patListeners;
	IPMTListeners pmtListeners;
	ISDTListeners sdtListeners;
	INITListeners nitListeners;
	IEITListeners eitListeners;
	ITDTListeners tdtListeners;
    IMGTListeners mgtListeners;
	//IPESListeners pesListeners;

	IParserListener* parserListener;

	int status;

protected:
	void ResetStatus() { status = 0; }
	void SetStatusPATReady() { status |= 1; }
	void SetStatusPMTReady() { status |= 2; }
	void SetStatusSDTReady() { status |= 4; }
	void SetStatusEITReady() { status |= 8; }
	bool IsStatusPATReady() { return (status & 1) == 1; }
	bool IsStatusPMTReady() { return (status & 2) == 2; }
	bool IsStatusSDTReady() { return (status & 4) == 4; }
	bool IsStatusEITReady() { return (status & 8) == 8; }
	void ResetListeners();

public:
	void SetParserType(int type);
private:
	int parserType;
	int detectedType;

#ifdef _SUPPORT_CACHE
public:
	void EnableCache(bool b)
	{
		rawParser.EnableCache(false);
		rawParser.SetCacheSize(0);
	}

	void FlushCache()
	{
		return rawParser.FlushCache();
	}

	void ProcessCache()
	{
		rawParser.ProcessCache();
	}

#endif //_SUPPORT_CACHE

public:
	void BreakParse() { rawParser.BreakParse(); }
};

inline bool GeneralParser::Process(uint8* pData, int32 cbData, int32* pcbProcessed)
{
	return rawParser.Process(pData, cbData, pcbProcessed);
}

inline bool GeneralParser::SetPIDFilter(int count, bit13* PIDs)
{
	return rawParser.SetPIDFilter(count, PIDs);
}

inline TransportStreamInfo* GeneralParser::GetTransportStreamInfo()
{
	return &ts;
}


} //namespace TS

#ifdef _VONAMESPACE
}
#endif

#endif //_TS_PARSE_H_