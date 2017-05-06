#include "tsbspas.h"

#include "readutil.h"
#include <string.h>
#ifdef DMEMLEAK
#include "dmemleak.h"
#endif //DMEMLEAK
#include "CDumper.h"
#include "voLog.h"
#include "voMTVBase.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

using namespace TS;

// RawParser
//==============================================================================

uint8* RawParser::FindPacketHeader(uint8* pData, int cbData, int packetSize)
{
	uint8* p = pData;
	uint8* p2 = pData + cbData - packetSize;
	while (p < p2)
	{
		if ( (*p == TransportPacketSyncByte) && (*(p + packetSize) == TransportPacketSyncByte) )
			return p;
		++p;
	}
	return 0;
}

uint8* RawParser::FindPacketHeader2(uint8* pData, int cbData, int packetSize)
{
	uint8* p = FindPacketHeader(pData, cbData, packetSize);
	if (p)
	{
		uint8* p2 = p + packetSize * 2;
		if (p2 < pData + cbData)
			if (*p2 == TransportPacketSyncByte)
				return p;
	}
	return 0;
}

int RawParser::CheckPacketSize(uint8* pData, int cbData)
{
	if (cbData <= 204) //only one packet
		return cbData;

	if (cbData <= 408) //less than 2 packets
	{
		if (FindPacketHeader(pData, cbData, 188))
			return 188;
		if (FindPacketHeader(pData, cbData, 204))
			return 204;
		if (FindPacketHeader(pData, cbData, 192))
			return 192;
		return 0;
	}

	if (FindPacketHeader2(pData, cbData, 188))
		return 188;
	if (FindPacketHeader2(pData, cbData, 204))
		return 204;
	if (FindPacketHeader2(pData, cbData, 192))
		return 192;
	return 0;
}


//==============================================================================


RawParser::RawParser()
{
    _packetSize = 0;
	_rawListener = NULL;
    _errListener = NULL;
	_iPacket = 0;
    _filterPIDs = NULL;
    _filterCount = 0;
    _breakParse = false;
	m_pPacketBak = new uint8[1024];///<we new possible max packet size
	m_nLenPacketBak = 0;
}


RawParser::~RawParser()
{
	if (m_pPacketBak)
	{
		delete []m_pPacketBak;
		m_pPacketBak = NULL;
	}
	SetPIDFilter(0);
}

bool RawParser::SetPIDFilter(int count, bit13* PIDs)
{
	if (_filterPIDs)
	{
		delete[] _filterPIDs;
		_filterPIDs = NULL;
		_filterCount = 0;
	}
	if (count)
	{
		_filterPIDs = new bit13[count];
		if (_filterPIDs == NULL)
			return false;
		for (int i = 0; i < count; i++)
			_filterPIDs[i] = PIDs[i];
		_filterCount = count;
	}
	return true;
}

void RawParser::Reset()
{
	_iPacket = 0;
	_packetSize = 0;
	_countSyncErr = 0;
	if (m_pPacketBak)
	{
		delete []m_pPacketBak;
		m_pPacketBak = NULL;
	}
	m_nLenPacketBak = 0;
	_PIDsInfo.Reset();
	BreakParse(false);
}

#ifdef _SUPPORT_CACHE
void  RawParser::ProcessCache()
{
	int32 processed;
	EnableCache(false);
	bool b = true;
	if (_cache.GetSize() < CACHE_SIZE) //make sure not overflow
		b = Process(_cache.GetBuffer(), _cache.GetSize(), &processed);
}
#endif



bool RawParser::Process(uint8* pData, int32 cbData, int32* pcbProcessed)
{
	// 20100106
	int  iPostFixLen = 0; 
    int  iSyncOffset = 0;

	if (ShouldBreak())
	{
		*pcbProcessed = 0;
		return false;
	}

	uint8* bak = pData;
	int32  baklen = cbData;

	if (_packetSize == 0)
	{
		_packetSize = CheckPacketSize(pData, cbData);
		if (_packetSize <= 0)
		{
			*pcbProcessed = 0;
			return false;
		}
	}
	uint8* pCur = NULL;
	int count = 0;
	
	if(_packetSize == 192)
	{
		iSyncOffset = 4;
	}

	if(_packetSize == 204)
	{
		iPostFixLen = 16;
	}

	if (m_nLenPacketBak)
	{
        if((m_nLenPacketBak+cbData)< _packetSize)
		{
			memcpy(m_pPacketBak+m_nLenPacketBak,pData,cbData);
			m_nLenPacketBak += cbData;
			count = 0;
		}
		else
		{
			memcpy(m_pPacketBak+m_nLenPacketBak,pData,_packetSize - m_nLenPacketBak);
			if (TransportPacketSyncByte == (*(m_pPacketBak+iSyncOffset)))
			{
				ParseOnePacket(m_pPacketBak+iSyncOffset, TransportPacketSize);
				count = cbData - (_packetSize - m_nLenPacketBak);
				*pcbProcessed = cbData + m_nLenPacketBak;
				pCur = pData + (_packetSize - m_nLenPacketBak);

				if (ShouldBreak())
				{
					BreakParse(false); //reset the status
				}
			}
			else
			{
				*pcbProcessed = cbData;
				pCur = pData;
				count = cbData;
			}

			m_nLenPacketBak = 0;
		}
		
	}
	else
	{
		*pcbProcessed = cbData;
		pCur = pData;
		count = cbData;
	}
	


	while (count > 0)
	{
		if (ShouldBreak())
		{
			BreakParse(false); //reset the status
			break;
		}
		//Check Sync Byte
		if (*(pCur+iSyncOffset) != TransportPacketSyncByte)
		{
			uint8* p = FindPacketHeader(pCur, count, _packetSize);
			_countSyncErr++;
			if (p == 0) // cannot find
			{
				pCur += count;
				count = 0;
				break;
			}

			if(_packetSize == 188)
			{
				count -= (int32)(p - pCur);
				pCur = p;
			}

			if(_packetSize == 192)
			{
				count -= (int32)(p - pCur);
				count += 4;
				pCur = p;
				pCur -= 4;
			}
		}
		if ((uint32)count < _packetSize)
		{
			memcpy(m_pPacketBak,pCur,count);
			m_nLenPacketBak = count;
			break;
		}

		bool b = false;
		b =	ParseOnePacket(pCur+iSyncOffset, TransportPacketSize);
		//if (!b) return b;
		pCur += _packetSize;
		count -= _packetSize;
		if (ShouldBreak()) //11/23/2007
		{
			CDumper::WriteLog((char *)"Cancel parse... 2");
			BreakParse(false); //reset the status
			break;
		}
	}
	if (!m_nLenPacketBak)
	{
		*pcbProcessed -= count;
	}
#ifdef _SUPPORT_CACHE
	if (ShouldDoCache())
	{
		if (ShouldFlushCache())
		{
			int32 processed;
			EnableCache(false);
			bool b = true;
			if (_cache.GetSize() < CACHE_SIZE) //make sure not overflow
				b = Process(_cache.GetBuffer(), _cache.GetSize(), &processed);

			SetCacheSize(0); //don't need cache any more

			// tag: 20100426 process the data left
			Process(bak, baklen, pcbProcessed);
			return b;
		}

		_cache.AddData(pData, cbData);
		if (ShouldSendCache())
		{
			// 20100426
			return true;
		}
	}
#endif //_SUPPORT_CACHE

	return true;
}

#if 0

bool RawParser::ProcessCache()
{
	int32 processed;
	EnableCache(false);
	bool b = Process(_cache.GetBuffer(), _cache.GetSize(), &processed);
	SetCacheSize(0); //don't need cache any more
	return b;
}

#endif //_SUPPORT_CACHE


bool RawParser::ParseOnePacket(uint8* pData, uint32 cbData)
{
	//CDumper::WriteLog("RawParser::ParseOnePacket+++");

	RawPacket packet; //current packet

	packet.head = pData;

	uint8* p = pData;
	++p; //already checked sync byte

	++_iPacket;
	packet.index = _iPacket;

	packet.transport_error_indicator = R1B7(*p);
	packet.payload_unit_start_indicator = R1B6(*p);
	packet.transport_priority = R1B5(*p);
	R13B(p, packet.PID);
    if(packet.PID == PID_ATSC_PSIP)
    {
        packet.index = _iPacket;
	}

	//CDumper::WriteLog("packet no.%08d, pid = %04x", packet.index, packet.PID);

	packet.transport_scrambling_control = R2B7(*p);
	packet.adaptation_field_control = R2B5(*p);
	packet.continuity_counter = R4B3(*p);
	++p;

	//check continuity counter
	_PIDInfo = _PIDsInfo.GetAddPIDInfo(packet.PID,packet.continuity_counter);
	if (_PIDInfo == 0)
		return false;
	++_PIDInfo->count;

	if (packet.adaptation_field_control & 0x2)
		p = ParseAdaptationField(p);

	packet.data = p;
	uint32 cbHead = (p - pData);
	if (cbHead >= cbData)
	{
		// tag: lin
		if (_errListener && (cbHead > cbData))
		{
			_errListener->OnLosePacket(&packet,MTV_PARSER_ERROR_CODE_PACKET_SIZE_ERROR);
		}
		return false;
	}
	packet.datasize = cbData - cbHead;

#if 1
	if (!CheckContinuityCounter(&packet))
	{
		//return false;
		if (_errListener)
			_errListener->OnLosePacket(&packet,MTV_PARSER_ERROR_CODE_NO_CONTINUITY);
	}
#endif
	
// 	if (packet.PID != 17)
// 		CDumper::DumpRawData(pData, cbData);

	if (packet.PID == 0x1fff) //ignore padding packets
		return true;

	if (_filterPIDs)
	{
		for (int i = 0; i < _filterCount; i++)
		{
			if (packet.PID == _filterPIDs[i]  || packet.PID == PID_ATSC_PSIP)
			{
				if (ShouldBreak())
				{
					CDumper::WriteLog((char *)"Cancel parse... 3");
					return true;
				}

				// tag: 20100428
				if (packet.PID!=258)
				{
					//return true;
				}
				// end

				_rawListener->OnRawPacket(&packet);
				return true;
			}
		}

		// 20100120 parse current UTC time
		if (packet.PID == PID_TDT)
			_rawListener->OnRawPacket(&packet);
		// end

		return true;
	}

	if (ShouldBreak())
	{
		CDumper::WriteLog((char *)"Cancel parse... 4");
		return true;
	}

	_rawListener->OnRawPacket(&packet);
	return true;
}

bool RawParser::CheckContinuityCounter(RawPacket* packet)
{
	uint8 lastCounter = _PIDInfo->continuity_counter;
	_PIDInfo->continuity_counter = packet->continuity_counter;
	if ((_PIDInfo->count > 1)//not check for first packet
		&& (packet->adaptation_field_control & 0x01) )
	{
		if (packet->continuity_counter == lastCounter)
		{
			++_PIDInfo->countDuplicate;
			return true; //ignore this packet
		}
		if (packet->continuity_counter != ((lastCounter + 1) % 16)) //error
			return false;
	}
	return true;
}

uint8* RawParser::ParseAdaptationField(uint8* pData)
{
	uint8 adaptation_field_length = *pData++;

	// tag: 20100413
// 	uint8* data		= pData;
// 	uint8 pcr_flag	= R1B4(*data);
// 	
// 	data++;
// 	if (pcr_flag)
// 	{
// 		uint64 pcr_base;
// 		uint16 pcr_ext;
// 		R33B(data, pcr_base);
// 
// 		data += 4;
// 
// 		uint64 pcr = pcr_base*300 + pcr_ext;
// 		
// 		pcr = pcr_base / 27000000;
// 
// 		//CDumper::WriteLog("pcr = %d", pcr);
// 
// 		int n = 0;
// 	}
	
	// end

	pData += adaptation_field_length;

	//CDumper::WriteLog("Ts packet header adaptation field len = %d", adaptation_field_length);
	return pData;
}


// PSIParser
//==============================================================================

int PSIParser::PeekSectionLength(uint8* data)
{
	// data's len don't less than 3 bytes
	bit12 len = 0;
	BitStream bs(data);
	bs.SkipBits(12);
	bs.ReadBits(12, len);

	return len;
}



//#define _OLD_PARSE_SECTION

#ifndef _OLD_PARSE_SECTION
// new parse section
void PSIParser::ParseSection(uint8* data, int data_len)
{
	if(data_len < 3)
		return;

	uint8* p = data;
	BitStream bs(p);

	bit1 section_syntax_indicator; 
	bs.ReadBits(8, _PSI.table_id);
	bs.ReadBits(1, section_syntax_indicator);
// 	if (section_syntax_indicator == 0) //error
// 		return;
	bs.SkipBits(3);
	bs.ReadBits(12, _PSI.section_length);

	int leftsize = data_len - (bs.Position() - data);
	if (leftsize <= 0)
		return;

	if (_PSI.section_length <= leftsize)
	{
		_parsingSection = true;

		_PSI.section_data = bs.Position();
		_listener->OnPSI(&_PSI);

		leftsize -= _PSI.section_length;
		if (leftsize <= 0)
			return;

		if (_PSI.table_id == 78)
		{
			_PSI.section_data += _PSI.section_length;

			// 20100119
			int peek_len = PeekSectionLength(_PSI.section_data);

			if(peek_len == 0 || peek_len > leftsize)
			{
				//_PSI.section_length = leftsize;
				_PSI.section_length = peek_len;

				//ParseSection(_PSI.section_data, leftsize);
				_bufferPSI.SetMaxSize(_PSI.section_length + TransportPacketSize);
				_bufferPSI.AddData(_PSI.section_data, leftsize);
			}
			else
			{
				ParseSection(_PSI.section_data, leftsize);
			}
		}
	}
	else
	{
		if (_parsingSection)
		{
			if(_PSI.section_length > leftsize)
				_PSI.section_length = leftsize;

			_PSI.section_data = bs.Position();
			_listener->OnPSI(&_PSI);

			leftsize -= _PSI.section_length;
			if (leftsize <= 0)
				return;

			if (_PSI.table_id == 78)
			{
				_PSI.section_data += _PSI.section_length;
				_PSI.section_length = leftsize;

				//ParseSection(_PSI.section_data, leftsize);
				_bufferPSI.SetMaxSize(_PSI.section_length + TransportPacketSize);
				_bufferPSI.AddData(_PSI.section_data, leftsize);
			}
		}
		else
		{
			_bufferPSI.SetMaxSize(_PSI.section_length + TransportPacketSize);
			_bufferPSI.AddData(bs.Position(), leftsize);
		}
	}
}

#else

void PSIParser::ParseSection(uint8* data, int data_len)
{
	uint8* p = data;
	BitStream bs(p);

	bit1 section_syntax_indicator; 
	bs.ReadBits(8, _PSI.table_id);
	bs.ReadBits(1, section_syntax_indicator);
	if (section_syntax_indicator == 0) //error
		return;
	bs.SkipBits(3);
	bs.ReadBits(12, _PSI.section_length);

	uint8 leftsize = data_len - (bs.Position() - data);
	if (leftsize <= 0)
		return;

	if (_PSI.section_length <= leftsize)
	{
		_parsingSection = true;

		_PSI.section_data = bs.Position();
		_listener->OnPSI(&_PSI);

		leftsize -= _PSI.section_length;
		if (leftsize <= 0)
			return;

		if (_PSI.table_id == 78)
		{
			_PSI.section_data += _PSI.section_length;
			_PSI.section_length = leftsize;

 			ParseSection(_PSI.section_data, leftsize);
		}
	}
	else
	{
		if (_parsingSection)
		{
			if(_PSI.section_length > leftsize)
				_PSI.section_length = leftsize;

			_PSI.section_data = bs.Position();
			_listener->OnPSI(&_PSI);

			leftsize -= _PSI.section_length;
			if (leftsize <= 0)
				return;

			if (_PSI.table_id == 78)
			{
				_PSI.section_data += _PSI.section_length;
				_PSI.section_length = leftsize;
				
 				ParseSection(_PSI.section_data, leftsize);
			}
		}
		else
		{
			_bufferPSI.SetMaxSize(_PSI.section_length + TransportPacketSize);
			_bufferPSI.AddData(bs.Position(), leftsize);
		}
	}
}

#endif


void PSIParser::OnRawPacket(RawPacket* packet)
{
	if (packet->payload_unit_start_indicator)
	{
		_parsingSection = false;
		uint8* pp = packet->data;
		ParseSection( (uint8*)(pp + *pp + 1), packet->datasize-(*pp + 1));
		return;
	}
	else
	{
		if (!_bufferPSI.AddData(packet->data, packet->datasize))
			return;

		int size = _bufferPSI.GetSize();

		if (_bufferPSI.GetSize() >= _PSI.section_length)  // 3/2/2009, modify "==" to ">="
		{
			// 20100112
			if(_parsingSection)
			{
				ParseSection(_bufferPSI.GetBuffer(), size);
				_bufferPSI.Reset();
				_parsingSection = false;
				return;
			}
			// end

			_PSI.section_data = _bufferPSI.GetBuffer();
			_listener->OnPSI(&_PSI);
			_bufferPSI.Reset();
		}
	}
}


/*
void PSIParser::OnRawPacket(RawPacket* packet)
{
	if (packet->payload_unit_start_indicator)
	{
		uint8* p = packet->data;
		BitStream bs(p + *p + 1); //pointer_field

		bit1 section_syntax_indicator; 
		bs.ReadBits(8, _PSI.table_id);
		bs.ReadBits(1, section_syntax_indicator);
		if (section_syntax_indicator == 0) //error
			return;
		bs.SkipBits(3);
		bs.ReadBits(12, _PSI.section_length);

		int leftsize = packet->datasize - (bs.Position() - packet->data);
		if (leftsize <= 0)
			return; //error. 2/23/2009

		if (_PSI.section_length <= leftsize) // don't need buffer
		{
			_PSI.section_data = bs.Position();
			_listener->OnPSI(&_PSI);
		}
		else //buffer it
		{
			_bufferPSI.SetMaxSize(_PSI.section_length + TransportPacketSize); // 3/2/2009 the real data may be exceed section_length
			_bufferPSI.AddData(bs.Position(), leftsize);
		}
	}
	else
	{
		if (!_bufferPSI.AddData(packet->data, packet->datasize))
			return;
		if (_bufferPSI.GetSize() >= _PSI.section_length)  // 3/2/2009, modify "==" to ">="
		{
			_PSI.section_data = _bufferPSI.GetBuffer();
			_listener->OnPSI(&_PSI);
			_bufferPSI.Reset();
		}
	}
}
*/

// PATParser
//==============================================================================

PATParser::PATParser()
{
	_PSIParser.SetListener(this);
}

void PATParser::OnPSI(PSI* psi)
{
	if (psi->table_id != 0x00) //should be 0x00
		return; 
	if (psi->section_length > MAX_PAT_SECTION_LENGTH)
		return;

	BitStream bs(psi->section_data);
	uint8* p2 = psi->section_data + psi->section_length - 4; //CRC 4 bytes
	_PAT.Load(bs, p2);
	_listener->OnPAT(&_PAT);
}


void PATParser::OnRawPacket(RawPacket* packet)
{
	if (packet->PID == PID_PAT)
		_PSIParser.OnRawPacket(packet);
	else if ( (packet->PID >= 0x1FC8) && (packet->PID <= 0x1FCF) ) //ISDB-T
		InitForISDBT(packet);
}

void PATParser::InitForISDBT(RawPacket* packet)
{
	uint8* p = packet->data;
	BitStream bs(p + *p + 1); //pointer_field

	uint8 table_id;
	bs.ReadBits(8, table_id);
	if (table_id != 0x02) 
		return; 
	bit1 section_syntax_indicator; 
	bs.ReadBits(1, section_syntax_indicator);
	if (section_syntax_indicator == 0) //error
		return;
	bs.SkipBits(3);
	bit12 section_length;
	bs.ReadBits(12, section_length);
	bit16 program_number;
	bs.ReadBits(16, program_number);

	if (_PAT.AddProgram(program_number, packet->PID))	
		return;
	//since we met same program PID, we think all programs shown already
	
	// tag : Why set it to 1, refer to TransportStreamInfo::Import
	_PAT.transport_stream_id = 1;
	//_PAT.transport_stream_id = 1850;

	_PAT.version_number = 0;
	_PAT.current_next_indicator = 0;
	_PAT.section_number = 0;
	_PAT.last_section_number = 1;
	_listener->OnPAT(&_PAT);
}


// MGTParser
//==============================================================================

MGTParser::MGTParser()
{
	_PSIParser.SetListener(this);
}

void MGTParser::OnPSI(PSI* psi)
{
	if (psi->table_id != Master_Guide_Table_ID) //should be 0x00
		return; 
	if (psi->section_length > MAX_MGT_SECTION_LENGTH)
		return;

	BitStream bs(psi->section_data);
	uint8* p2 = psi->section_data + psi->section_length - 4; //CRC 4 bytes
	_MGT.Load(bs, p2);
	_listener->OnMGT(&_MGT);
}

void MGTParser::OnRawPacket(RawPacket* packet)
{
	if (packet->PID == PID_ATSC_PSIP)
	{
		_PSIParser.OnRawPacket(packet);
	}
}



// PMTParser
//==============================================================================

PMTParser::PMTParser()
: _PAT(NULL)
{
	_PSIParser.SetListener(this);
}

void PMTParser::Reset()
{
}

void PMTParser::OnRawPacket(RawPacket* packet)
{
	// 是否解析到PAT
	if (_PAT == NULL)
		return;

	// 只解析PAT里面存在的PMT
	int pn = _PAT->GetProgramNumber(packet->PID);
	if (pn <= 0)
		return;

	//_PMT.program_map_PID = packet->PID;
	_PSIParser.OnRawPacket(packet);
}


void PMTParser::OnPSI(PSI* psi)
{
	if (psi->table_id != 0x02) 
		return; 
	if (psi->section_length > MAX_PMT_SECTION_LENGTH)
		return;

	BitStream bs(psi->section_data);
	uint8* p2 = psi->section_data + psi->section_length - 4; //CRC 4 bytes
	_PMT.Load(bs, p2);
	if (_PMT.TS_program_map_items) //make sure... 2/23/2009
		_listener->OnPMT(&_PMT);
}




// SDTParser
//==============================================================================

SDTParser::SDTParser()
{
	_PSIParser.SetListener(this);
}

void SDTParser::OnRawPacket(RawPacket* packet)
{
	if (packet->PID != PID_SDT)
		return;

	_PSIParser.OnRawPacket(packet);
}

void SDTParser::OnPSI(PSI* psi)
{
	if (psi->table_id != 0x42) 
		return; 
	if (psi->section_length > MAX_SDT_SECTION_LENGTH)
		return;

	BitStream bs(psi->section_data);
	uint8* p2 = psi->section_data + psi->section_length - 4; //CRC 4 bytes
	SDT object;
	object.Load(bs, p2);

#if 0 // test
	service_description_item* item = object.service_description_items;

	while(item)
	{
		service_descriptor* desc = (service_descriptor*)item->descriptors;

		if (desc)
		{
			TCHAR tmp[512], debug[512];
			
			MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)desc->service_name, -1, tmp, desc->service_name_length);
			swprintf(debug, _T("+++++ service name: %s \r\n"), tmp);
			OutputDebugString(debug);

		}
		item = (service_description_item*)item->more;
	}
#endif

	if(_listener)
		_listener->OnSDT(&object);
}

// NITParser
//==============================================================================

NITParser::NITParser()
: _network_PID(-1)
{
	_PSIParser.SetListener(this);
}

void NITParser::OnPAT(PAT* pat)
{
	_network_PID = pat->GetNetworkPID();
}

void NITParser::OnRawPacket(RawPacket* packet)
{
	if (packet->PID != _network_PID)
		return;

	_PSIParser.OnRawPacket(packet);
}

void NITParser::OnPSI(PSI* psi)
{
	if ( (psi->table_id != 0x40) && (psi->table_id != 0x41) )
		return; 
	if (psi->section_length > MAX_NIT_SECTION_LENGTH)
		return;

	BitStream bs(psi->section_data);
	uint8* p2 = psi->section_data + psi->section_length - 4; //CRC 4 bytes
	NIT object;
	object.Load(bs, p2);
	_listener->OnNIT(&object);
}


// EITParser
//==============================================================================

EITParser::EITParser()
{
	_PSIParser.SetListener(this);
	m_pMGT = NULL;
	m_pEITCallbackFunc = NULL;
}


void EITParser::OnMGT(MGT* pMGT)
{
    m_pMGT = pMGT;
}

void EITParser::OnRawPacket(RawPacket* packet)
{
	// 20091210 ISDB-T: 0x27:portable 0x26:mobile 0x12:fixed
	if(m_pMGT == NULL)
    {
		if ( (packet->PID != 0x0027) && (packet->PID != 0x0026) && (packet->PID != PID_EIT) )
			return;
    }
	else
    {
		int pn = m_pMGT->GetEITNumber(packet->PID);
		if (pn <= 0)
			return;
    }

// 	if (packet->PID != PID_EIT)
// 		return;

	_PSIParser.OnRawPacket(packet);
}

void GetEventInfo2(unsigned long long inStartTime, unsigned long inDuration, 
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

void EITParser::OnPSI(PSI* psi)
{
	//if (psi->table_id != 0x42) 
	//	return; 
	if (psi->section_length > MAX_EIT_SECTION_LENGTH)
		return;

	BitStream bs(psi->section_data);
	uint8* p2 = psi->section_data + psi->section_length - 4; //CRC 4 bytes

	EIT object;
    EIT_ATSC objectATSC;
    if(m_pMGT == NULL)
	{
        //0x4E event_information_section - actual_transport_stream, present/following
        //0x4F event_information_section - other_transport_stream, present/following
        //0x50 to 0x5F event_information_section - actual_transport_stream, schedule
        //0x60 to 0x6F event_information_section - other_transport_stream, schedule

		if((psi->table_id == 0x4E) ||
			(psi->table_id == 0x4F) ||
			(psi->table_id >= 0x50 && psi->table_id<=0x5F) ||
			(psi->table_id >= 0x60 && psi->table_id<=0x6F))
		{
			object.Load(bs, p2);
		}	
	}
	else
	{
        event_information_item_ATSC*   pEITItem = NULL;
		EITDescDataCallback    pFunCallback = NULL;
		int   iLength = 0;
		descriptor*     pDesc = NULL;
		caption_service_descriptor*    pCaptionDesc = NULL;
        uint8*        pData = NULL;
		objectATSC.Load(bs, p2);

		pEITItem = objectATSC.event_information_items;
		while(pEITItem != NULL)
		{
			pDesc = pEITItem->descriptors;
			while(pDesc != NULL)
			{
				//caption_service
				if(pDesc->tag == 0x86)
				{
					pCaptionDesc = (caption_service_descriptor*)pDesc;
					iLength = pDesc->length;
					pData = pCaptionDesc->GetDescData();
					pFunCallback = (EITDescDataCallback)m_pEITCallbackFunc;
					if(pFunCallback != NULL)
					{
						pFunCallback(pData, iLength);
					}
				}
				pDesc = (descriptor*)(pDesc->more);
			}

			pEITItem = (event_information_item_ATSC*)(pEITItem->more);
		}
	}

	// 20100118
// 	if (!bRet)
// 	{
// 		return;
// 	}
	// end


#if 0 // test
	event_information_item* item = object.event_information_items;
	while(item)
	{
		unsigned short year,month,day,hour,minute,second, hour1,minute1,second1;
		GetEventInfo2(item->start_time, item->duration,
			year, month, day, hour, minute, second, hour1, minute1, second1);

		TCHAR debug[512];
		memset(debug, 0, 512);

		short_event_descriptor* desc = (short_event_descriptor*)item->descriptors;
		if (desc)
		{
			TCHAR tmp[512];
			memset(tmp, 0, 512);

			if (desc->event_name && desc->event_name_length>0)
			{
				MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)desc->event_name, -1, tmp, desc->event_name_length);
				swprintf(debug, _T("+++++ event name(%03d): %s\r\n"), desc->event_name_length, tmp);
				OutputDebugString(debug);
			}

			memset(debug, 0, 512);
			memset(tmp, 0, 512);

			if (desc->text && desc->text_length)
			{
				MultiByteToWideChar(CP_ACP, 0, (LPCSTR)desc->text, -1, tmp, desc->text_length);
				swprintf(debug, _T("+++++ event description(%03d): %s\r\n"), desc->text_length, tmp);
				OutputDebugString(debug);
			}
		}

		memset(debug, 0, 512);
		swprintf(debug, _T("+++++ event time:%04d-%02d-%02d %02d:%02d:%02d  Duration:%02d:%02d:%02d Running Status:%d\r\n\r\n"), year, month, day, hour, minute, second, hour1, minute1, second1, item->running_status);
		OutputDebugString(debug);

		item = (event_information_item*)item->more;
	}

#endif
		
	if(_listener)
		_listener->OnEIT(&object);
}

void EITParser::IsReady()
{

}

// TDTParser
//==============================================================================
TDTParser::TDTParser()
{
	_PSIParser.SetListener(this);
}

void TDTParser::OnRawPacket(RawPacket* packet)
{
	if ( (packet->PID != PID_TDT) )
		return;

	_PSIParser.OnRawPacket(packet);
}

void TDTParser::OnPSI(PSI* psi)
{
	//if (psi->table_id != 0x42) 
	//	return; 
	if (psi->section_length > MAX_EIT_SECTION_LENGTH)
		return;

	BitStream bs(psi->section_data);
	uint8* p2 = psi->section_data + psi->section_length - 4; //CRC 4 bytes

	TDT object;
	object.Load(bs, p2);

#if 0 // test

	unsigned short year,month,day,hour,minute,second, hour1,minute1,second1;
	GetEventInfo2(object.UTC_time, object.UTC_time,
		year, month, day, hour, minute, second, hour1, minute1, second1);
	CDumper::WriteLog("Current program time = %04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);

#endif


	if(_listener)
		_listener->OnTDT(&object);
}


// PESParser
//==============================================================================

PESParser::PESParser()
{
    _pi = NULL;
    _listener=NULL;
    _pes_header_parsed = false;
	m_llLastValidTimeStamp = 0;
	//m_lLastValidTimeStamp = 0;
}

void PESParser::OnRawPacket(RawPacket* packet)
{
	if (_pi == NULL)
		return;
	ESConfig* escfg = _pi->GetElementInfoByPID(packet->PID);
	if (escfg == NULL)
		return;

	if (escfg->stream_type == ST_14496Section)
		Parse14496Section(packet, escfg);
	else
		ParsePESPacket(packet, escfg);

#if 0 //3/10/2009
	switch(escfg->stream_type)
	{
	case ST_111722Video:
	case ST_H262Video:
	case ST_111723Audio:
	case ST_138183Audio:
	case ST_144963AudioWithLATM:
	case ST_144961PES:
	case ST_H264:
	case 0x81:
	case ST_138187AudioWithADTS: 
	case ST_H2220_138181_PESPackets: //??? 3/10/2009
		ParsePESPacket(packet, escfg);
		break;

	case ST_14496Section:
		Parse14496Section(packet, escfg);
		break;

	default:
		//_listener->OnRawData(escfg, packet->data, packet->datasize);
		break;
	}
#endif

}

void PESParser::ParsePESPacket(RawPacket* packet, ESConfig* escfg)
{
	//CDumper::WriteLog("PESParser::ParsePESPacket");

	if (packet->payload_unit_start_indicator)
	{
		//CDumper::WriteLog("PES.Load");
		PESPacket PES;
		if (PES.Load(packet->data, packet->datasize))
		{
			// check and parse SL packet
			MP4::SLConfigDescriptor* SLCD = escfg->GetSLCD();
			if (SLCD)
			{
				SLPacketHeader slhead;
				if(SLCD->predefined == 0)
				{
				    uint8* p = slhead.Parse(PES.data, PES.datasize, SLCD);
				    //!!! set SL/CTS into PES/PTS
				    PES.PTS = slhead.compositionTimeStamp * 90000 / SLCD->timeStampResolution;
				    uint16 slheadsize = (uint16)(p - PES.data);
				    PES.data = p;
				    if (PES.datasize < slheadsize)
					    return; //error occurs
				    PES.datasize -= slheadsize;
				    PES.payloadsize -= slheadsize;
				}
			}
			if (PES.PTS_DTS_flags == 0x02 || PES.PTS_DTS_flags == 0x03)
			{
				m_llLastValidTimeStamp = PES.PTS;
			}
			else
			{
				if(m_llLastValidTimeStamp != 0xFFFFFFFFFFFFFFFFLL)
				{
					PES.PTS = m_llLastValidTimeStamp;
				}
				else
				{
					PES.PTS =  0x1FFFFFFFFLL;    //
					PES.DTS =  0x1FFFFFFFFLL;    //
				}
			}
			_pes_header_parsed = true;
			_listener->OnPESHead(escfg, &PES);
		}
	}
	else
	{
		//CDumper::WriteLog("_listener->OnPESData");

		if(_pes_header_parsed)
			_listener->OnPESData(escfg, packet->data, packet->datasize);
		else
		{
			// tag: 20100609
			//CDumper::WriteLog("pes header not parsed.");
		}
	}
}

void PESParser::SetProgram(ProgramInfo* pi)
{
	_pi = pi;

	_listener->OnElementInfo(pi->element_count, pi->GetPlaybackableElementCount());



	// tag: 20100511, removed
	//MP4::InitialObjectDescriptor* IOD = pi->GetIOD();
	//if (IOD)
		//return;
	// end



	for (int i = 0; i < pi->element_count; i++)
	{
		_listener->OnElementStream(pi->elements + i);
	}
}


void PESParser::Parse14496Section(RawPacket* packet, ESConfig* escfg)
{
	if (packet->payload_unit_start_indicator != 1)
		return;

	uint8* p = packet->data;
	BitStream bs(p + *p + 1); //pointer_field

	bit1 section_syntax_indicator; 
	bit8 table_id;
	bs.ReadBits(8, table_id);
	bs.ReadBits(1, section_syntax_indicator);
	if (section_syntax_indicator == 0) //error
		return;
	bs.SkipBits(3);
	bit12 section_length;
	bs.ReadBits(12, section_length);

	p = bs.Position() + 5; //skip 5 bytes
	uint32 leftsize = packet->datasize - (p - packet->data);
	uint32 size = section_length - 9;
	if (size > leftsize) //need more than 1 packet
		return;

	MP4::SLConfigDescriptor* SLCD = escfg->GetSLCD();
	if (SLCD)
	{
		SLPacketHeader slhead;
		uint8* p2 = slhead.Parse(p, size, SLCD);
		size -= (uint32)(p2 - p);
		p = p2;
		MP4::DecoderConfigDescriptor* DCD = escfg->GetDCD();
		if ((DCD->objectTypeIndication == 1 || DCD->objectTypeIndication == 2) //Systems ISO/IEC 14496-1
#if 1
			&& (DCD->streamType == 1 || DCD->streamType == 3)// 11172 Video/Audio, 13818-1 Table 2-29
#endif
			) 
		{
			ParseODStream(p, size);
		}
	}
}

void PESParser::ParseODStream(uint8* data, uint32 len)
{
	//??? why skip 2 bytes
	uint8* p = data + 2;
	int size = len - 2;
#if 1
	if (*(p - 1) & 0x80)
	{
		p++;
		size--;
	}
#endif
	while (size > 0)
	{
		MP4::Descriptor* d = MP4::Descriptor::Load(p, size);
		if (d == 0)
			return;
		size -= d->GetSize();
		p += d->GetSize();
		MP4::ESDescriptor* ESD = (MP4::ESDescriptor*) d->GetChildByTag(MP4::ES_DescrTag);
		if (ESD == 0)
		{
			delete d;
			continue;
		}
        ESConfig* escfg = _pi->GetElementInfoByESID(ESD->ES_ID);
		if (escfg == 0)
		{
			delete d;
			continue;
		}
		if (escfg->GetESD() == NULL)
		{
			escfg->SetRootDescriptor(d);
			escfg->SetESD(ESD);
			_listener->OnElementStream(escfg);
		}
	}
}


