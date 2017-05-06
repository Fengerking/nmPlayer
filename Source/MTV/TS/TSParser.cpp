#include "TSParser.h"
#include "mp4cfg.h"
#include "CDumper.h"
#include "voOSFunc.h"
#include "voLog.h"

/*#include "diagtool.h"*/

#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

using namespace MP4;

const int MAX_INIT_DATA_SIZE = 512 << 10; //512 K
const int DEFAULT_AAC_FRAME_SIZE = 16 << 10;
const int MAX_ADTS_HEADER_SIZE = 9;

void ExportEPGItem(EventItem* src, VOSTREAMPARSEEPGITEM* dst)
{
	src->ExportEventName(dst->name, 128);
	src->ExportEventDescription(dst->description, 256);
	dst->id = src->event_id;
	dst->start_time = src->start_time;
	dst->duration = src->duration;

/*
	TCHAR debug[512];
	memset(debug, 0, 512);
	swprintf(debug, _T("EPG Time:%u %lld\r\n"), dst->start_time, src->start_time);
	OutputDebugString(debug);
*/
}

void ExportEPG(ProgramInfo* program, VOSTREAMPARSEEPGINFO* epg)
{
	int total = program->GetEventTotal();
	epg->count = total;
	if (total == 0)
	{
		epg->items = NULL;
		return;
	}
	VOSTREAMPARSEEPGITEM* item = new VOSTREAMPARSEEPGITEM[total];
	epg->items = item;
	for (int i = 0; i < program->event_count; i++)
	{
		for (int j = 0; j < program->events[i].count; j++)
		{
			ExportEPGItem(program->events[i].items + j, item);
			item++;
		}
	}
}

//==============================================================================

CTSParser::CTSParser(int parseType)
: m_parser(this, parseType) // 20100317 modify to PARSE_ALL
, m_initializer(NULL)
, m_bParsing(false)
{
	//m_parser.AddListener((IPESListener*) this);
	//m_parser.SetListener((IPESListener*) this);
    //InitializeCriticalSection(&m_csInitializer);

	m_bOutputed = false;
	//m_hEnventParsing = ::CreateEvent(NULL, TRUE, TRUE, _T("ParseEvent"));

	m_parser.SetErrorListener(this);

    m_iCanPlayback = 0;
    m_ulOpenFlag = 0;

#ifdef _STREAM_CHECK_NORMAL_ 
	m_nStreamReadyCount = 0;
#endif
}

CTSParser::~CTSParser(void)
{
	//EnterCriticalSection(&m_csInitializer);

	// tag: 20100423
	//voCAutoLock lock(&m_csInitializer);

	CDumper::CloseAllDumpFile();

	if (m_initializer)
	{
#ifdef _DEBUG
		CDumper::WriteLog((char *)"+ delete initializer");
#endif //_DEBUG
		delete m_initializer;
		m_initializer = NULL;
#ifdef _DEBUG
		CDumper::WriteLog((char *)"- delete initializer");
#endif //_DEBUG
	}

// 	CloseHandle(m_hEnventParsing);
// 	m_hEnventParsing = NULL;

    //LeaveCriticalSection(&m_csInitializer);
    //DeleteCriticalSection(&m_csInitializer);
}


VOSTREAMPARSERETURNCODE CTSParser::Reset()
{
	CDumper::WriteLog((char *)"CTSParser::Reset()");

	m_parser.Reset();
	return VORC_STREAMPARSE_OK;
}

VOSTREAMPARSERETURNCODE CTSParser::Reset2()
{
	CDumper::WriteLog((char *)"CTSParser::Reset2()");

	m_parser.Reset2();
	

	return VORC_STREAMPARSE_OK;
}


VOSTREAMPARSERETURNCODE CTSParser::Process(int channel, void* pData, int nSize, int* pProcessed)
{
	VOSTREAMPARSERETURNCODE rc = VORC_STREAMPARSE_OK;

#if 1
	
	//CDumper::WriteLog("CTSParser::Process+++");

	// tag: 20100423
	//m_hEnventParsing.Reset();
	m_bParsing = true;
	bool b = m_parser.Process((uint8*)pData, nSize, (int32*)pProcessed);
	m_bParsing = false;

	//m_hEnventParsing.Signal();

	if (!b)
		return VORC_STREAMPARSE_ERROR;
#else //test only
	*pProcessed = nSize;
#endif

	return rc;
}


//add by qichaoshen @ 2011-10-28
int  CTSParser::IsPlaybackReady()
{
	return m_iCanPlayback;
}
//add by qichaoshen @ 2011-10-28


//add by qichaoshen @ 2011-10-28
void  CTSParser::DoForcePlayAction()
{
	if((m_initializer != NULL) && ( m_initializer->IsVideoReady() || m_initializer->IsAudioReady()))
	{

		CDumper::WriteLog((char *)"CTSParser::CheckStreamReadyNormal OK");
		m_pListener->OnNewStreamEnd();
		m_parser.SetListener((IPESListener*) &m_dispatcher);
		delete m_initializer;
		m_initializer = NULL;

#ifdef _SUPPORT_CACHE
		m_parser.FlushCache();
        m_parser.ProcessCache();
            
#endif //_SUPPORT_CACHE
	}
}

//add by qichaoshen @ 2011-10-28
void  CTSParser::OnLosePacket(RawPacket* packet,int nErrorCode)
{
	m_pListener->OnFrameError(packet->PID, nErrorCode); //TODO
}

void CTSParser::OnProgramIndexReady(TransportStreamInfo* tsi)
{
	CDumper::WriteLog((char *)"CTSParser::OnProgramIndexReady");
	VOSTREAMPARSEPROGRAMINDEX pi;
	pi.pid_count = tsi->ExportProgramMapPIDs(pi.pids, sizeof(pi.pids) / sizeof(int));
	m_pListener->OnProgramIndex(&pi);
}
 
void CTSParser::OnProgramInfoReady(TransportStreamInfo* tsi)
{
	CDumper::WriteLog((char *)"CTSParser::OnProgramInfoReady");
	OutputProgramInfo(tsi);
}

void GetEventInfo(unsigned long long inStartTime, unsigned long inDuration, 
				  unsigned short& outYear, unsigned short& outMonth, unsigned short& outDay,
				  unsigned short& outHour, unsigned short& outMinute, unsigned short& outSecond,
				  unsigned short& outDurationHour, unsigned short& outDurationMinute, unsigned short& outDurationSecond)
{
	bit8* tmp = (bit8*)&inStartTime;

	bit8 hour	= *(tmp+2);
	hour		= (hour>>4) * 10 + (hour & 0xf);
	bit8 minute	= *(tmp+1);
	minute		= (minute>>4) * 10 + (minute & 0xf);
	bit8 second	= *(tmp);
	second		= (second>>4) * 10 + (second & 0xf);

	bit16 start_date;
	memcpy(&start_date, tmp+3, 2);
	int year	= (int)((start_date-15078.2)/365.25);
	int month	= (int)((start_date-14956.1-(int)(year*365.25))/30.6001);
	int day		= start_date-14956-(int)(year*365.25)-(int)(month*30.6001);

	int k = 0;
	if(month==14 || month==15)
		k = 1;
	year	= year + k + 1900;
	month	= month - 1 - k*12;

	tmp				= (bit8*)&inDuration;
	bit8 hour1		= *(tmp+2);
	hour1			= (hour1>>4) * 10 + (hour1 & 0xf);
	bit8 minute1	= *(tmp+1);
	minute1			= (minute1>>4) * 10 + (minute1 & 0xf);
	bit8 second1	= *(tmp);
	second1			= (second1>>4) * 10 + (second1 & 0xf);

	// out
	outYear		= year;
	outMonth	= month;
	outDay		= day;

	outHour		= hour;
	outMinute	= minute;
	outSecond	= second;

	outDurationHour		= hour1;
	outDurationMinute	= minute1;
	outDurationSecond	= second1;
}

void CTSParser::SetEITCallbackFun(void*  pFunc)
{
    m_parser.SetEITCallbackFun(pFunc);
}



void CTSParser::OutputProgramInfo(TransportStreamInfo* tsi)
{
	if(!tsi->programs)
		return;

	if (m_bOutputed)
	{
		CDumper::WriteLog((char *)"don't output program info again.");
		return;
	}

	// tag: 20100423
	//voCAutoLock lock(&m_csInitializer);

	m_bOutputed = true;

	VOSTREAMPARSEPROGRAMINFO info;
	memset(&info, 0, sizeof(VOSTREAMPARSEPROGRAMINFO));
	ProgramInfo* program = tsi->programs;
	int count = tsi->program_count;
	for (int i = 0; i < count; i++, program++)
	{
		// 20100105
		if(!program)
		{
// 			LeaveCriticalSection(&m_csInitializer);
			return;
		}

#if 1 //ignore the programs we don't support
		if (program->GetPlaybackableElementCount() == 0) 
			continue;
#endif
		info.id = program->program_number;
		if((m_ulOpenFlag & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL) == VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)
		{
            info.pid_count = program->ExportVideoElementPIDs(info.pids, sizeof(info.pids) / sizeof(VO_U32));
		}
		else
		{
            info.pid_count = program->ExportElementPIDs(info.pids, sizeof(info.pids) / sizeof(VO_U32));
		}
		if (info.pid_count <= 0) // no info, 2/12/2009
			continue; 
#if 1 //include PMT & PAT
		info.pids[info.pid_count++] = program->program_map_PID;
		info.pids[info.pid_count++] = PID_PAT;
#endif
		tsi->programs[i].ExportServiceName(info.name, 64);
		ExportEPG(program, &info.EPG);
		m_pListener->OnNewProgram(&info);
		if (info.EPG.items)
			delete[] info.EPG.items;


		// debug info
		/*
		int total_event_count = 0;
		CDumper::WriteLog("+++++++++++++++++++++++++OnNewProgram : channel name=%s, program No.=%d+++++++++++++++++++++++++", tsi->programs[i].service_name, info.id);
		//CDumper::WriteLog("++++++++++++++++++++++print EPG information++++++++++++++++++++++");
		for (int n=0; n<program->event_count; n++)
		{
			for (int m=0; m<(program->events[n]).count; m++)
			{
				total_event_count++;
				unsigned short year,month,day,hour,minute,second, hour1,minute1,second1;
				GetEventInfo(program->events[n].items[m].start_time, program->events[n].items[m].duration,
					year, month, day, hour, minute, second, hour1, minute1, second1);

				CDumper::WriteLog("(%03d)EPG Time:%04d-%02d-%02d %02d:%02d:%02d  Duration:%02d:%02d:%02d Name: %s", total_event_count, year, month, day, hour, minute, second, hour1, minute1, second1, program->events[n].items[m].event_name?program->events[n].items[m].event_name:"");
				//CDumper::WriteLog("(%03d)EPG Desc:%s\r\n", total_event_count, program->events[n].items[m].text?program->events[n].items[m].text:"");
			}
		}

		if (total_event_count != 6)
		{
			int n = 0;
		}

		CDumper::WriteLog("EPG total count = %d", total_event_count);
		*/
		// debug info end
	}

	m_pListener->OnNewProgramEnd();
// 	LeaveCriticalSection(&m_csInitializer);
}


/*
void CTSParser::OutputProgramInfo(TransportStreamInfo* tsi)
{
	VOSTREAMPARSEPROGRAMINFO info;
	ProgramInfo* program = tsi->programs;
	int count = tsi->program_count;
	for (int i = 0; i < count; i++, program++)
	{
		// 20100105
		if(!program)
			return;

#if 1 //ignore the programs we don't support
		if (program->GetPlaybackableElementCount() == 0) 
			continue;
#endif
		info.id = program->program_number;
		info.pid_count = program->ExportElementPIDs(info.pids, sizeof(info.pids) / sizeof(int));
		if (info.pid_count <= 0) // no info, 2/12/2009
			continue; 
#if 1 //include PMT & PAT
		info.pids[info.pid_count++] = program->program_map_PID;
		info.pids[info.pid_count++] = PID_PAT;
#endif
		tsi->programs[i].ExportServiceName(info.name, 64);
		ExportEPG(program, &info.EPG);
		m_pListener->OnNewProgram(&info);
		if (info.EPG.items)
			delete[] info.EPG.items;

#if 1 // test
	TCHAR service_info[256];
	swprintf(service_info, _T("+++++++++++++++++++++++++OnNewProgram : channel name=%s, program No.=%d+++++++++++++++++++++++++\r\n"), info.name, info.id);
	OutputDebugString(service_info);
#endif

#if 1 // test

		OutputDebugString(_T("\r\n\r\n++++++++++++++++++++++print EGP information++++++++++++++++++++++\r\n"));
		for (int n=0; n<program->event_count; n++)
		{
			for (int m=0; m<(program->events[n]).count; m++)
			{
				unsigned short year,month,day,hour,minute,second, hour1,minute1,second1;
				GetEventInfo(program->events[n].items[m].start_time, program->events[n].items[m].duration,
					year, month, day, hour, minute, second, hour1, minute1, second1);

				TCHAR name[128];
				program->events[n].items[m].ExportEventName(name, 128);
				TCHAR debug[512];
				memset(debug, 0, 512);
				//swprintf(debug, _T("Event Name: %s, start time:%llu, duration:%d \r"), name, program->events[n].items[m].start_time, program->events[n].items[m].duration);
				swprintf(debug, _T("EPG Time:%04d-%02d-%02d %02d:%02d:%02d  Duration:%02d:%02d:%02d Name: %s\r\n"), year, month, day, hour, minute, second, hour1, minute1, second1, name);
				OutputDebugString(debug);
			}
		}
#endif // end test

	}
	m_pListener->OnNewProgramEnd();
}
*/


VOSTREAMPARSERETURNCODE CTSParser::Query()
{
	CDumper::WriteLog((char *)"CTSParser::Query");
	//if (IsStatusProgramInfoReady())
	//	return VORC_OK;

	CDumper::WriteLog((char *)"+++++++++++++++++Query Program Info++++++++++++++++\r\n");

	m_bOutputed = false;

	OutputProgramInfo(m_parser.GetTransportStreamInfo());
	return VORC_STREAMPARSE_OK;
}

VOSTREAMPARSERETURNCODE CTSParser::SelectProgram(int programId)
{
	//programId = 59224;

	CDumper::WriteLog((char *)"CTSParser::SelectProgram");
	if (m_initializer == NULL)
	{
		m_initializer = new CFormatInitializer();
		m_initializer->SetOpenFlag(m_ulOpenFlag);
		m_initializer->SetListener(m_pListener);
	}
	m_parser.SetListener((IPESListener*) this); //switch data handler
	return m_parser.SelectProgram(programId) ? VORC_STREAMPARSE_OK : VORC_STREAMPARSE_ERROR;
}

VOSTREAMPARSERETURNCODE CTSParser::SetParserType(int type)
{
	CDumper::WriteLog((char *)"CTSParser::SetParserType");
	CDumper::WriteLog((char *)"Set parse type = %x", type);
	m_parser.SetParserType(type);
	return VORC_STREAMPARSE_OK;
}

void CTSParser::OnElementInfo(int total, int playbackable)
{
// 	EnterCriticalSection(&m_csInitializer);

	// tag: 20100423
	//voCAutoLock lock(&m_csInitializer);
	if (m_initializer)
		m_initializer->OnElementInfo(total, playbackable);
//     LeaveCriticalSection(&m_csInitializer);
}

void CTSParser::OnElementStream(ESConfig* escfg)
{
	if (escfg->extension)
		return;
	CDumper::WriteLog((char *)"CTSParser::OnElementStream");
// 	EnterCriticalSection(&m_csInitializer);
	// tag: 20100423
	//voCAutoLock lock(&m_csInitializer);

	if (m_initializer)
		m_initializer->OnElementStream(escfg);

	// tag: 20100429 remove
#ifndef _STREAM_CHECK_NORMAL_
	CheckStreamReady(escfg);
#endif
	// end

//     LeaveCriticalSection(&m_csInitializer);
}

void CTSParser::OnPESHead(ESConfig* escfg, PESPacket* packet)
{
	//escfg->extension有值，意味着已经找到stream的head data
	if (escfg->extension)
	{
		CheckStreamReadyNormal();
		return;
	}
	//DBG_MSG(_T("CTSParser::OnPESHead"));
// 	EnterCriticalSection(&m_csInitializer);

	// tag: 20100423
	//voCAutoLock lock(&m_csInitializer);

	if (m_initializer)
	{
		m_initializer->OnPESHead(escfg, packet);
		CheckStreamReady(escfg);
	}
//     LeaveCriticalSection(&m_csInitializer);
}

void CTSParser::OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData)
{
	if (escfg->extension)
	{
		CheckStreamReadyNormal();
		return;
	}
		
	//DBG_MSG(_T("CTSParser::OnPESData"));
// 	EnterCriticalSection(&m_csInitializer);

	// tag: 20100423
	//voCAutoLock lock(&m_csInitializer);

	if (m_initializer)
	{
		m_initializer->OnPESData(escfg, pData, cbData);
		CheckStreamReady(escfg);
	}
//     LeaveCriticalSection(&m_csInitializer);
}

void CTSParser::CheckStreamReadyNormal()
{
#ifdef _STREAM_CHECK_NORMAL_
	m_nStreamReadyCount++;

	// 256这个值不安全
	// tag: 20100524
	//if (m_nStreamReadyCount > 2560)
    if (m_nStreamReadyCount > 25600)  //modify by qichaoshen@2011-10-28  add the length for header data check
	//if (m_nStreamReadyCount > 256)
	{
		if(m_initializer->IsVideoReady() || m_initializer->IsAudioReady())
		{
			CDumper::WriteLog((char *)"CTSParser::CheckStreamReadyNormal OK");

			m_pListener->OnNewStreamEnd();
			m_parser.SetListener((IPESListener*) &m_dispatcher);
			m_parser.Reset2();
			delete m_initializer;
			m_initializer = NULL;

		    //add by qichaoshen @ 2011-10-28
		    m_iCanPlayback = 1;
		    //add by qichaoshen @ 2011-10-28

#ifdef _SUPPORT_CACHE
			m_parser.BreakParse();
			m_parser.FlushCache();
#endif 
		}
	}
	else
	{
        if(m_initializer->IsStreamReady())
		{		
			CDumper::WriteLog((char *)"CTSParser::CheckStreamReady OK");
			m_pListener->OnNewStreamEnd();
			m_parser.SetListener((IPESListener*) &m_dispatcher);
            m_parser.Reset2();            
			delete m_initializer;
			m_initializer = NULL;

		    //add by qichaoshen @ 2011-10-28
		    m_iCanPlayback = 1;
		    //add by qichaoshen @ 2011-10-28
		
#ifdef _SUPPORT_CACHE
		    m_parser.BreakParse();
		    m_parser.FlushCache();
#endif
		}
	}
#endif
}

void CTSParser::CheckStreamReady(ESConfig* escfg)
{
	//CDumper::WriteLog("CTSParser::CheckStreamReady... stream type = %d, element_pid = %d", escfg->stream_type, escfg->elementary_PID);

	if (m_initializer->IsStreamReady())
	{
		CDumper::WriteLog((char *)"CTSParser::CheckStreamReady OK");

		m_pListener->OnNewStreamEnd();
		m_parser.SetListener((IPESListener*) &m_dispatcher);
        m_parser.Reset2();
		if(m_initializer != NULL)
		{
			delete m_initializer;
			m_initializer = NULL;
		}

		//add by qichaoshen @ 2011-10-28
		m_iCanPlayback = 1;
		//add by qichaoshen @ 2011-10-28
		
#ifdef _SUPPORT_CACHE
		m_parser.BreakParse();
		m_parser.FlushCache();
#endif //_SUPPORT_CACHE
	}
}


void CTSParser::WaitParseFinish()
{
	uint32 dwTime = voOS_GetSysTime();

	//WaitForSingleObject(m_hEnventParsing, 5000);

	// tag: 20100426
	//m_hEnventParsing.Wait(5000);

	int total = 0;
	while (m_bParsing)
	{
		voOS_Sleep(10);
		total += 10;

		if (total > 5000)
		{
			break;
		}
	}
	// end

	CDumper::WriteLog((char *)"wait time = %d", voOS_GetSysTime() - dwTime);
}

void CTSParser::SetOpenFlag(VO_U32 ulOpenFlag)
{
    m_ulOpenFlag = ulOpenFlag;    
}



#if 0

void CTSParser::InitStreamFromDescriptor(ESConfig* escfg)
{
	MP4::DecoderConfigDescriptor* DCD = escfg->GetDCD();
	MP4::DecoderSpecificInfo* DSI = escfg->GetDSI();

	// for T-DMB only
	switch (DCD->objectTypeIndication)
	{
	case 0x21: //AVC
		if (DSI == NULL)
			return;
		NewStreamAVC(escfg->elementary_PID, DSI->GetBodyData(), DSI->GetBodySize());
		if (m_repacker == NULL)
			m_repacker = new CRepacker(m_pListener);
		SetVideoReady();
		break;

	case 0x40: //AAC
		if (DSI == NULL)
			return;
		NewStreamAAC(escfg->elementary_PID, DSI->GetBodyData(), DSI->GetBodySize());
		if (m_repacker == NULL)
			m_repacker = new CRepacker(m_pListener);
		SetAudioReady();
		break;

	case 0x6c: // Visual ISO/IEC 10918-1, JPEG!
		break;

	default:
		break;
	}
}


void CTSParser::InitStreamFromConfig(ESConfig* escfg)
{
#if 0
	// fix settings for GoldSpotMedia
	BYTE profile = 3;
	BYTE sri = 5;
	BYTE channel = 1;
	BYTE dsi[2];

	switch (escfg->stream_type)
	{
	case ST_144963AudioWithLATM:
		//NewStreamAAC(escfg->elementary_PID, escfg->ES_info, escfg->ES_info_length);
		dsi[0] = (profile << 3) | (sri >> 1);
		dsi[1] = (sri << 7) | (channel << 4);
		NewStreamAAC(escfg->elementary_PID, dsi, 2);
		if (m_repacker == NULL)
			m_repacker = new CRepacker(m_pListener);
		SetAudioReady();
		break;

	case ST_138187AudioWithADTS:
	case ST_H264:
		break;

	default:
		break;
	}
#endif
}


void CTSParser::NewStreamAVC(int streamid, void* dsidata, int dsisize)
{
	//TODO: get width & height
	int width = 320;
	int height = 240;

#if 0
	AVCDecoderConfigurationRecord cfg;
	if (!cfg.Load((uint8*)dsidata, dsisize))
		return;

	int num1 = cfg.numOfSequenceParameterSets;
	int num2 = cfg.numOfPictureParameterSets;
	int seqhsize = (num1 + num2) * 2;
	int i;
	for (i = 0; i < num1; i++)
		seqhsize += cfg.sequenceParameterSet[i].len;
	for (i = 0; i < num2; i++)
		seqhsize += cfg.pictureParameterSet[i].len;

	int fmtsize = (sizeof(MPEG2VIDEOINFO) + seqhsize + 3 ) / 4 * 4; //DWORD align

	BYTE* p = (BYTE*) vi->dwSequenceHeader;
	for (i = 0; i < num1; i++)
	{
		int len = cfg.sequenceParameterSet[i].len;
		*p++ = (BYTE)((len >> 8) & 0xff);
		*p++ = (BYTE) len & 0xff;
		memcpy(p, cfg.sequenceParameterSet[i].data, len);
		p += len;
	}
	for (i = 0; i < num2; i++)
	{
		int len = cfg.pictureParameterSet[i].len;
		*p++ = (BYTE)((len >> 8) & 0xff);
		*p++ = (BYTE) len & 0xff;
		memcpy(p, cfg.pictureParameterSet[i].data, len);
		p += len;
	}

	VIDEOINFOHEADER2* vih = &vi->hdr;
	vih->dwInterlaceFlags = 0;
	vih->dwCopyProtectFlags = 0;
	vih->dwPictAspectRatioX = 4;
	vih->dwPictAspectRatioY = 3;

	BITMAPINFOHEADER* bih = &vih->bmiHeader;
	memset(bih, 0, sizeof(BITMAPINFOHEADER));
	bih->biSize = sizeof(BITMAPINFOHEADER);
	bih->biWidth = width;
	bih->biHeight = height;
	bih->biBitCount = 24; 
	bih->biPlanes = 1; 

#endif

	VOSTREAMPARSESTREAMINFO info;
	info.id = streamid;
	info.is_video = true;
	info.codec = VC_AVC;
	info.video.width = width;
	info.video.height = height;

	info.extra_data = dsidata;
	info.extra_size = dsisize;

	info.max_frame_len = 20480; //??
	m_pListener->OnNewStream(&info);

}

void CTSParser::NewStreamAAC(int streamid, void* dsidata, int dsisize)
{
	VOSTREAMPARSESTREAMINFO info;
	info.id = streamid;
	info.is_video = false;
	info.codec = AC_AAC;

#if 0
	AudioSpecificConfig cfg;
	if (!cfg.Load((uint8*)dsidata, dsisize))
		return;

	info.audio.channels = cfg.channelConfiguration;
	info.audio.sample_rate = cfg.samplingFrequency;
	info.audio.sample_bits = 16;
#endif

	info.max_frame_len = DEFAULT_AAC_FRAME_SIZE; //?? increase the size for there are many frames in a packet in Korean T-DMB files.
	info.extra_data = dsidata;
	info.extra_size = dsisize;

	m_pListener->OnNewStream(&info);
}



void CTSParser::InitStreamFromPES(ESConfig* escfg, PESPacket* packet)
{
	VOSTREAMPARSESTREAMINFO info;
	info.id = escfg->elementary_PID;
	info.extra_data = packet->data;

	switch (escfg->stream_type)
	{
	case ST_H262Video: //13818-2, 11172-2
		if (IsVideoReady())
			return;
		{
			info.is_video = true;
		PatternFinder finder(0x000001B3);
		uint8* pEnd = packet->data + packet->datasize;
		uint8* p = finder.Find(packet->data, pEnd);
		if (p)
		{
			uint8* p1 = p;
			while (p < pEnd)
			{
				finder.SetPattern(0x00000100, 0xffffff80);
				p = finder.Find(p, pEnd); 
				if (p == NULL)
					return;
				if ((*p & 0xf0) != 0xB0)
				{
					uint8* p2 = p - 4;
					break;
				}
			}
			return;
		}
		}
		break;

	case ST_111723Audio:
		if (IsAudioReady())
			return;
		break;

	case ST_138183Audio:
		if (IsAudioReady())
			return;
		break;

	case ST_138187AudioWithADTS:
		if (IsAudioReady())
			return;
		info.codec = AC_AAC;
		info.max_frame_len = 8192;  
		info.extra_size = MAX_ADTS_HEADER_SIZE;
#if 0
		if (packet->PTS_DTS_flags & 0x02) //??? 6/24/2008
			escfg->context = 0; //use PTS
		else 
			if (packet->PTS_DTS_flags & 0x01)
			escfg->context = 1; //use DTS
#endif
		m_pListener->OnNewStream(&info);
		if (m_repacker == NULL)
			m_repacker = new CRepacker(m_pListener);
		SetAudioReady();
		break;

	case ST_H264: 
		if (IsVideoReady())
			return;
		if ((packet->data[4] & 0x0f) != 0x07) //SPS flag
			return;
		info.codec = VC_H264;
		info.max_frame_len = 0; // do not know
		info.extra_size = packet->datasize;
#if 0
		if (packet->PTS_DTS_flags & 0x02) //??? 6/24/2008
			escfg->context = 0; //use PTS
		else 
			if (packet->PTS_DTS_flags & 0x01)
			escfg->context = 1; //
#endif

		m_pListener->OnNewStream(&info);
		SetVideoReady();
		if (m_repacker == NULL)
			m_repacker = new CRepacker(m_pListener);
		break;

	default:
		return;
	}

}

#endif
