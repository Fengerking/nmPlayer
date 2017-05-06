#include "tsparse.h"
#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK
#include "CDumper.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

using namespace TS;


// GeneralParser
//==============================================================================

GeneralParser::GeneralParser(IParserListener* listener, int type)
: parserListener(listener)
, parserType(0)
, detectedType(DETECTED_NONE)
{
	// tag: 20100423
	SetParserType(type);
	// end

#ifdef _SUPPORT_CACHE
	rawParser.EnableCache(true);
	rawParser.SetCacheSize(CACHE_SIZE);
#endif //_SUPPORT_CACHE

}

GeneralParser::~GeneralParser()
{
}

void GeneralParser::Reset()
{
	rawParser.Reset();
	ResetStatus();

	// 20100113
	//ts.Reset();
	//SetParserType(parserType);


#ifdef _SUPPORT_CACHE
	rawParser.EnableCache(true);
	rawParser.SetCacheSize(CACHE_SIZE);
#endif //_SUPPORT_CACHE
}

void GeneralParser::Reset2()
{
// 	rawParser.Reset();
// 	ResetStatus();
	ts.Reset2();
	pesParser.Reset();
	pesParser.ResetValidTimeStamp();
}

void GeneralParser::OnRawPacket(RawPacket* packet)
{
}

void GeneralParser::OnPAT(PAT* pat) 
{ 
	CDumper::WriteLog((char *)"GeneralParser::OnPAT");

	detectedType |= DETECTED_PAT;

	if (!pat)
	{
		CDumper::WriteLog((char *)"Invalid PAT");
		return;
	}

	ts.Import(pat);

#if 0
	program_association_item* item = pat->program_association_items;
	while (item)
	{
		//if (item)
		{
			TCHAR debug[256];
			swprintf(debug, _T("program no. = %d, program map id = %d \r\n"), item->program_number, item->program_map_PID);
			OutputDebugString(debug);
		}

		item = (program_association_item*)item->more;
	}
#endif

	rawListeners.Remove(&patParser);

	//pat = (PAT* )-1;
	//pat->last_section_number = 1234; //test
	parserListener->OnProgramIndexReady(&ts);
#ifdef _SUPPORT_CACHE
	rawParser.SendCache();
#endif //_SUPPORT_CACHE
}


// tag: 20100427
//#define _HTTP_LIVE_STREAMING_
// end



void  GeneralParser::OnMGT(MGT* mgt)
{
}




void GeneralParser::OnPMT(PMT* pmt) 
{ 
	detectedType |= DETECTED_PMT;

	//const int PMT_TIMEOUT = 2;

#ifdef _HTTP_LIVE_STREAMING_
	const int SDT_TIMEOUT = 0;
	const int EIT_TIMEOUT = 0;
#else
	const int SDT_TIMEOUT = 30;
	const int EIT_TIMEOUT = 30;
#endif

	CDumper::WriteLog((char *)"GeneralParser::OnPMT");
	//CDumper::WriteLog("PCR_PID = %d", pmt->PCR_PID);

	int rc = ts.Import(pmt);

#if 0 // test
	TCHAR val[128];
	swprintf(val, _T("import PMT times = %d\r\n"), rc);
	OutputDebugString(val);
#endif

	// 20100317 removed
// 	if (rc < PMT_TIMEOUT)
// 		return;
	// end

	// 20100118
// 	if ( (parserType & PARSE_SDT) && (rc < SDT_TIMEOUT) )
// 		return;
// 	if ( (parserType & PARSE_EIT) && (rc < EIT_TIMEOUT) )
// 		return;

	/*
	if ( (parserType & PARSE_EIT) && (!ts.ImportedEIT2()) )
	{
		// 有些码流不包含EIT
		if(rc < EIT_TIMEOUT)
		{
			CDumper::WriteLog((char *)"EIT is not ready!");
			return;
		}
	}
	*/

	/*

	if ( (parserType & PARSE_SDT) && (!ts.ImportedSDT2()) )
	{
		if(rc < SDT_TIMEOUT)
		{
			CDumper::WriteLog((char *)"SDT is not ready!");
			return;
		}
	}
	*/

	// end

	// test code
	for (int n=0; n<ts.program_count; n++)
	{
		CDumper::WriteLog((char *)"Program %d : program_map_PID = %d", ts.transport_stream_id, ts.programs[n].program_map_PID);
		for (int i = 0; i < ts.programs[n].element_count; i++)
			CDumper::WriteLog((char *)"Program %d : stream_PID = %d", ts.transport_stream_id, ts.programs[n].elements[i].elementary_PID);
	}


	// end

	// 20091210 removed
	//rawListeners.Remove(&eitParser);
	rawListeners.Remove(&sdtParser);
	rawListeners.Remove(&pmtParser);

	CDumper::WriteLog((char *)"++++++++++ Notify program info by PMT detected %d times ++++++++++", rc);
	parserListener->OnProgramInfoReady(&ts);
#ifdef _SUPPORT_CACHE
	rawParser.SendCache();
#endif //_SUPPORT_CACHE
}

void GeneralParser::OnSDT(SDT* sdt) 
{ 
	detectedType |= DETECTED_SDT;

	const int SDT_TIMEOUT = 2;
	//const int EIT_TIMEOUT = 2;

	CDumper::WriteLog((char *)"GeneralParser::OnSDT");
	int rc = ts.Import(sdt);

	if (rc < SDT_TIMEOUT)
		return;

	// 20100118
// 	if ( (parserType & PARSE_PMT) && !ts.ImportedPMT() ) //make sure already get PMT
// 		return;
	if ( (parserType & PARSE_PMT) && !ts.ImportedPMT2() ) //make sure already get PMT
		return;

// 	if ( (parserType & PARSE_EIT) && (rc < EIT_TIMEOUT) )
// 		return;
	if ( parserType & PARSE_EIT )
	{
		// 如果有EIT的包，则需要等到解析完EIT
		if(detectedType & DETECTED_EIT)
			if(!ts.ImportedEIT())
			{
				// 可能有些流不包含EIT, 
				if (rc < 5)
					return;
			}
				
	}
	// end

	// 20091210
	//rawListeners.Remove(&eitParser);
	//rawListeners.Remove(&sdtParser);

	rawListeners.Remove(&pmtParser);

	CDumper::WriteLog((char *)"++++++++++ Notify program info by SDT detected %d times ++++++++++", rc);
	parserListener->OnProgramInfoReady(&ts);

#ifdef _SUPPORT_CACHE
	rawParser.SendCache();
#endif //_SUPPORT_CACHE
}

void GeneralParser::OnEIT(EIT* eit) 
{ 
	detectedType |= DETECTED_EIT;

	//const int EIT_TIMEOUT = 2;

	CDumper::WriteLog((char *)"GeneralParser::OnEIT");

	CDumper::WriteLog((char *)"Program import EIT");
	int rc = ts.Import(eit);

	// 20100118
// 	if (rc < EIT_TIMEOUT)
// 		return;
	if( (parserType&PARSE_EIT) && !ts.ImportedEIT() )
		return;
	// end

	// 20100119
	if( (parserType&PARSE_SDT) && !ts.ImportedSDT() )
		return;
	if( (parserType&PARSE_PMT) && !ts.ImportedPMT() )
		return;
	// end


	// 20091210
// 	rawListeners.Remove(&eitParser);
// 	rawListeners.Remove(&sdtParser);
// 	rawListeners.Remove(&pmtParser);

	CDumper::WriteLog((char *)"++++++++++ Notify program info by EIT detected %d times ++++++++++", rc);
	parserListener->OnProgramInfoReady(&ts);
#ifdef _SUPPORT_CACHE
	rawParser.SendCache();
#endif //_SUPPORT_CACHE
}

void GeneralParser::OnTDT(TDT* eit)
{
	
}


void GeneralParser::OnNIT(NIT* nit) 
{ 
	detectedType |= DETECTED_NIT;

	CDumper::WriteLog((char *)"GeneralParser::OnNIT");

	CDumper::WriteLog((char *)"Remove NIT parser");
	rawListeners.Remove(&nitParser);
	CDumper::WriteLog((char *)"End remove NIT parser");
}

bool GeneralParser::SelectProgram(bit16 programId)
{
	ProgramInfo* pi = ts.FindProgram(programId);

	if (pi == NULL)
	{
		CDumper::WriteLog((char *)"No program found.");

		// 20100113
		Reset();
		rawParser.SetPIDFilter(0, NULL);
		// end

		return false;
	}
		
	const int PID_SIZE = 32;
	bit13 pids[PID_SIZE];

	int count = pi->ExportElementPIDs(pids, PID_SIZE);
	rawParser.SetPIDFilter(count, pids);
	pesParser.SetProgram(pi);

	return true;
}


void GeneralParser::SetEITCallbackFun(void*  pFunc)
{
	eitParser.SetEITCallbackFun(pFunc);
}


// 20100406
void GeneralParser::ResetListeners()
{
	rawListeners.Clear();

	patListeners.Clear();
	pmtListeners.Clear();
	sdtListeners.Clear();
	nitListeners.Clear();
	eitListeners.Clear();
	tdtListeners.Clear();
	mgtListeners.Clear();
}
// end


void GeneralParser::SetParserType(int type)
{
	rawParser.SetRawListener(&rawListeners);
	patParser.SetListener(&patListeners);
	pmtParser.SetListener(&pmtListeners);
	sdtParser.SetListener(&sdtListeners);
	nitParser.SetListener(&nitListeners);
	eitParser.SetListener(&eitListeners);
	tdtParser.SetListener(&tdtListeners);
	mgtParser.SetListener(&mgtListeners);
	//pesParser.SetListener(&pesListeners);

	// 20100406
	ResetListeners();
	// end
	
	rawListeners.Add(&tdtParser);
    rawListeners.Add(&mgtParser);

	if (type & PARSE_PAT)
		rawListeners.Add(&patParser);
	if (type & PARSE_PMT)
	{
		rawListeners.Add(&pmtParser);
		patListeners.Add(&pmtParser);
	}
	if (type & PARSE_PES)
	{
		rawListeners.Add(&pesParser);
		//pmtListeners.Add(&pesParser);
	}
	if (type & PARSE_EIT)
	{
		rawListeners.Add(&eitParser);
        mgtListeners.Add(&eitParser);
	}
	if (type & PARSE_SDT)
		rawListeners.Add(&sdtParser);
	if (type & PARSE_NIT)
	{
		rawListeners.Add(&nitParser);
		patListeners.Add(&nitParser);
	}

	patListeners.Add(this);
	pmtListeners.Add(this);
	sdtListeners.Add(this);
	nitListeners.Add(this);
	eitListeners.Add(this);
	tdtListeners.Add(this);
    mgtListeners.Add(this);

	parserType = type;

	ResetStatus();
}
