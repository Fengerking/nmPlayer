#include "CTsAssembler.h"
#include "CTsPacket.h"
#include "CDumper.h"
#include "cmnMemory.h"
#include "voLog.h"
#include "CDumper.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

// add to support KMP
#define ADD_VIDEO_HEAD_DATA
#define VO_INT32_MAX 2147483647

//#define SET_PES_LENGTH

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
VO_S64 AVRescaleRound(VO_S64 arg1, VO_S64 arg2, VO_S64 arg3, VO_U32 uRound)
{

	VO_S64 r=0;
	if(uRound==5) 
		r= arg3 / 2;
	else if(uRound&1)             
		r= arg3-1;

	if(arg2 <= VO_INT32_MAX && arg3 <= VO_INT32_MAX)
	{
		if(arg1<=VO_INT32_MAX)
			return (arg1 * arg2 + r)/arg3;
		else
			return arg1/arg3*arg2 + (arg1%arg3*arg2 + r)/arg3;
	}
	else
	{
		VO_U64 a0= arg1&0xFFFFFFFF;
		VO_U64 a1= arg1>>32;
		VO_U64 b0= arg2&0xFFFFFFFF;
		VO_U64 b1= arg2>>32;
		VO_U64 t1= a0*b1 + a1*b0;
		VO_U64 t1a= t1<<32;
		VO_S32 i;

		a0 = a0*b0 + t1a;
		a1 = a1*b1 + (t1>>32) + (a0<t1a);
		a0 += r;
		a1 += a0<r;

		for(i=63; i>=0; i--){
			//            VO_S32 o= a1 & 0x8000000000000000ULL;
			a1+= a1 + ((a0>>i)&1);
			t1+=t1;
			if(/*o || */arg3 <= a1){
				a1 -= arg3;
				t1++;
			}
		}
		return t1;
	}
}
#ifdef _VONAMESPACE
}
#endif

CTsAssembler::CTsAssembler(void)
: m_pTsPacekt(VO_NULL)
, m_bFirstSample(VO_TRUE)
, m_uBasicPeriod(DEFAULT_PAT_PMT_PERIOD - 1)
, m_pBufData(NULL)
, m_uBufLen(0)
{
	BuildCrcTable();

	InitContinuityCounter();
	m_pTsPacekt = new CTsPacket;

	m_pTsPacekt->Init();

	cmnMemSet(0, &pat, 0, sizeof(CPat));
	cmnMemSet(0, &pmt, 0, sizeof(CPmt));
	cmnMemSet(0, &pes, 0, sizeof(CPESPacket));
	m_uDefultPESPayloadSize = (DEFAULT_PES_PAYLOAD_SIZE + 14 + 183) / 184 * 184 - 14;
	voGetVideoParserAPI(&m_funH264VideoParser,VO_VIDEO_CodingH264);
	m_funH264VideoParser.Init(&m_pH264VideoParser);

}

CTsAssembler::~CTsAssembler(void)
{
	Close();
}

VO_VOID	CTsAssembler::Open(ASSEMBLERINITINFO* pInit)
{
	CBaseAssembler::Open(pInit);

	for (VO_BYTE n=0; n<1; n++)
	{
		SetPAT();
		SetPMT();
	}
}

VO_VOID	CTsAssembler::Close()
{
	if (m_pBufData)
	{
		delete []m_pBufData;
		m_pBufData = NULL;
		m_uBufLen = 0;
	}

	if (m_pTsPacekt)
	{
		delete m_pTsPacekt;
		m_pTsPacekt = VO_NULL;
	}
	pmt.Uninit();
	CBaseAssembler::Close();
	if (m_pH264VideoParser)
	{
		m_funH264VideoParser.Uninit(m_pH264VideoParser);
		m_pH264VideoParser = NULL;
	}
}

VO_U32	CTsAssembler::AssembleTsPacketHeader(VO_U16 pid, VO_BYTE payload_unit_start_indicator, VO_BOOL bDataByte, VO_S16 adaptation_field_length,VO_BOOL bPCRPacket,VO_S64 DTS)
{
	VO_U32 total_bits = 0;

	// sync byte
	total_bits += m_pTsPacekt->WriteBits(8, 0x47);

	// Set_transport_error_indicator
	total_bits += m_pTsPacekt->WriteBits(1, 0);

	// Set_payload_unit_start_indicator
	total_bits += m_pTsPacekt->WriteBits(1, payload_unit_start_indicator);

	// Set_transport_priority
	total_bits += m_pTsPacekt->WriteBits(1, 0);

	// Set_PID
//	total_bits += m_pTsPacekt->WriteBits(5, 0);
	total_bits += m_pTsPacekt->WriteBits(13, pid);

	// Set_transport_scarambling_control
	total_bits += m_pTsPacekt->WriteBits(2, 0);// 0 : unencrypt

	// Set_adaptation_field_control
	if (adaptation_field_length > 0)
	{
		total_bits += m_pTsPacekt->WriteBits(2, 3);// '11'
	}
	else
		total_bits += m_pTsPacekt->WriteBits(2, 1);// '01'

	// Set_continuity_counter
	total_bits += m_pTsPacekt->WriteBits(4, GetContinuityCount(pid));


	// total 32 bits

	// pointer field ????? refer to <数字电视>p33

	if (adaptation_field_length > 0)
	{
		total_bits += m_pTsPacekt->WriteBits(8, adaptation_field_length-1);
		adaptation_field_length -= 1;
		if (bPCRPacket && adaptation_field_length >= 7)
		{
			///<now we just write the PCR flah
			total_bits += m_pTsPacekt->WriteBits(8, 0x10);

			const VO_S64 sDelay = AVRescaleRound(700000LL, 90000LL, 1000000LL,5);
			VO_U64 PCR = (DTS - sDelay)*300;

			VO_S64 PCRLow = PCR % 300, PCRHigh = PCR / 300;

			total_bits += m_pTsPacekt->WriteBits(8, PCRHigh>>25);
			total_bits += m_pTsPacekt->WriteBits(8, PCRHigh>>17);
			total_bits += m_pTsPacekt->WriteBits(8, PCRHigh>>9);
			total_bits += m_pTsPacekt->WriteBits(8, PCRHigh>>1);
			total_bits += m_pTsPacekt->WriteBits(8, PCRHigh << 7 | PCRLow >> 8 | 0x7e);
			total_bits += m_pTsPacekt->WriteBits(8, PCRLow);
			adaptation_field_length -= 7;
		}
		else
		{
			if (adaptation_field_length > 0)
			{
				total_bits += m_pTsPacekt->WriteBits(8, 0x00);
				adaptation_field_length -= 1;
			}
		}
	

		VO_BYTE stuffing_byte = 0xFF;
		for (VO_BYTE n=0; n<adaptation_field_length; n++)
		{
			total_bits += m_pTsPacekt->WriteByte(1, &stuffing_byte);
		}
	}
	if(bDataByte && total_bits < 188)
		total_bits += m_pTsPacekt->WriteBits(8, 0);

	m_pTsPacekt->Flush();

	return total_bits;
}
VO_VOID CTsAssembler::SetPAT()
{
	VO_BYTE program_count = 1;// now only support mux one program 

	cmnMemSet(0, &pat, 0, sizeof(CPat));

	// header
	pat.table_id					= 0x0;	// must 0
	pat.section_synctax_indicator	= 0x1;	// must 1
	pat.unknown						= 0;
	pat.reserved1					= 3;
	pat.section_length				= pat.GetSectionSize()*program_count + pat.GetCrcSize() + 5; // 5 is size of transport_stream_id ~ last_section_number
	pat.transport_stream_id			= 0;	// user define
	pat.reserved2					= 3;
	pat.version_number				= 0;	// NOTE: it will increased by 1 when PAT changed
	pat.current_next_indicator		= 1;
	pat.section_number				= 0;	// here has issue
	pat.last_section_number			= 0;

	// section
	pat.program_count = program_count;
	for (VO_BYTE n=0; n<program_count; n++)
	{
		pat.program_number[n]	= 1;		// user define. 0 is for network id
		pat.program_map_PID[n]	= program_map_PID_base + n;	// user define.
	}
}
VO_U32 CTsAssembler::AssemblePAT()
{
	VO_U32 total_bits = 0;
	total_bits += AssembleTsPacketHeader(0, 1, VO_TRUE, 0,VO_FALSE);
	VO_BYTE* pStart = m_pTsPacekt->Position();
	int ts_header_len = total_bits/8;

	total_bits += AssembleCommonPSI(&pat,Psi_Type_PAT);

	for (VO_BYTE n=0; n<pat.program_count; n++)
	{
		total_bits += m_pTsPacekt->WriteBits(16, pat.program_number[n]);
		total_bits += m_pTsPacekt->WriteBits(3, 7);
		total_bits += m_pTsPacekt->WriteBits(13, pat.program_map_PID[n]);
	}

	// crc32
	int m = ((total_bits/8) - ts_header_len);
	VO_U32 crc_32	= CalcCRC32(pStart, m);
	m_pTsPacekt->Position();
	total_bits	+= m_pTsPacekt->WriteBits(16, crc_32>>16);
	total_bits	+= m_pTsPacekt->WriteBits(16, crc_32);
	
	return total_bits;
}

VO_VOID CTsAssembler::SetPMT()
{
	// here has iisue if has more 1 program count, must Write
	for (VO_BYTE n=0; n<pat.program_count; n++)
	{
		pmt.Uninit();
		cmnMemSet(0, &pmt, 0, sizeof(CPmt));

		pmt.table_id					= 0x02;	// must 2
		pmt.section_synctax_indicator	= 0x01;	// must 1
		pmt.unknown						= 0;
		pmt.reserved1					= 3;
		pmt.section_length				= 0x9;///<now we set the fixed section length
		pmt.program_number				= pat.program_number[n];// or pat.transport_stream_id???? 
		pmt.reserved2					= 3;
		pmt.version_number				= 0;	// NOTE: it will increased by 1 when PAT changed
		pmt.current_next_indicator		= 1;
		pmt.section_number				= 0;	// here has issue
		pmt.last_section_number			= 0;

	//	pmt.reserved1					= 0;
		pmt.PCR_PID						= elementary_PID_base + 1;
	//	pmt.reserved2					= 0;
		pmt.program_info_length			= 0;
		if (GetStreamType(0,m_pAssembleInitInfo->codec_list[0]) == 0x80)
		{
			CRegistration_Descriptor* descriptor = new CRegistration_Descriptor;
			descriptor->descriptor_tag = 0x05;
			descriptor->descriptor_length = 4;
			descriptor->format_identifier = 0x48444d56;
			descriptor->additional_info_len = 0;
			descriptor->additional_identification_info = NULL;
			pmt.AddDescriptor(0,Pmt_Descriptor_Type_Program,descriptor);
			pmt.program_info_length += descriptor->GetLength();
		}
		pmt.section_length += pmt.program_info_length;

		///<the Program info fixed length is 5 byte
		VO_U32 nProgramStreamInfoLen = 0;
		VO_U32 uStreamCount = 0;
		for (VO_U32 m=0; (m < MAX_AV_STREAM_COUNT) && (uStreamCount < m_pAssembleInitInfo->codec_count); m++)
		{ 
			if(m_pAssembleInitInfo->codec_list[m] == 0)
				continue;
			uStreamCount++;
			nProgramStreamInfoLen += 5;
			pmt.stream_type[m]	= GetStreamType(m,m_pAssembleInitInfo->codec_list[m]);
			pmt.reserved3[m]	= 0;
			pmt.elementary_PID[m]	= elementary_PID_base + m;
			pmt.reserved4[m]	= 0;
			pmt.ES_info_length[m] = 0;
			if (m == 0 && pmt.stream_type[m] == 0x80)
			{
				CRegistration_Descriptor* descriptor = new CRegistration_Descriptor;
				descriptor->descriptor_tag = 0x05;
				descriptor->descriptor_length = 4;
				descriptor->format_identifier = 0x48444d56;
				descriptor->additional_info_len = 0;
				descriptor->additional_identification_info = NULL;

				pmt.AddDescriptor(0,Pmt_Descriptor_Type_Stream,descriptor);
				pmt.ES_info_length[m]	+= descriptor->GetLength();
				nProgramStreamInfoLen += pmt.ES_info_length[m];
			}
		}

		pmt.section_length += nProgramStreamInfoLen;
		pmt.section_length += 4;///<CRC size
	}
}


VO_U32 CTsAssembler::AssemblePMT()
{
	if(!m_pAssembleInitInfo)
		return 0;

	VO_U32 total_bits		= 0;

	// here has iisue if has more 1 program count, must Write
	for (VO_BYTE n=0; n<pat.program_count; n++)
	{
		total_bits += AssembleTsPacketHeader(pat.program_map_PID[n], 1, VO_TRUE, 0,VO_FALSE);
		
		VO_BYTE* pStart	= m_pTsPacekt->Position();
		int ts_header_len = total_bits/8;
		total_bits += AssembleCommonPSI(&pmt,Psi_Type_PMT);

		total_bits += m_pTsPacekt->WriteBits(3, 7);
		total_bits += m_pTsPacekt->WriteBits(13, pmt.PCR_PID);
		total_bits += m_pTsPacekt->WriteBits(4, 0xF);
		total_bits += m_pTsPacekt->WriteBits(12, pmt.program_info_length);
		CDescriptor * pTmp = pmt.pProgramdescriptorHead;
		VO_U32 nProDesSize = 0;
		while(pTmp)
		{
			nProDesSize += pTmp->AssembleDescriptor(m_pTsPacekt);
			pTmp = pTmp->pNext;
		}
		if (nProDesSize != pmt.program_info_length)
		{
			VOLOGE("TS Muxer Error:program_info error");
		}
		total_bits += nProDesSize*8;
		
		VO_U32 uStreamCount = 0;
		for (VO_U32 m=0; (m < MAX_AV_STREAM_COUNT) && (uStreamCount < m_pAssembleInitInfo->codec_count); m++)
		{ 
			if(m_pAssembleInitInfo->codec_list[m] == 0)
				continue;
			uStreamCount++;
			total_bits += m_pTsPacekt->WriteBits(8, pmt.stream_type[m]);
			total_bits += m_pTsPacekt->WriteBits(3, 7);
			total_bits += m_pTsPacekt->WriteBits(13, pmt.elementary_PID[m]);
			total_bits += m_pTsPacekt->WriteBits(4, 0xF);
			total_bits += m_pTsPacekt->WriteBits(12, pmt.ES_info_length[m]);

			CDescriptor * pTmp = pmt.pStreamdescriptorHead[m];
			VO_U32 nStreamDesSize = 0;
			while (pTmp)
			{
				nStreamDesSize += pTmp->AssembleDescriptor(m_pTsPacekt);
				pTmp = pTmp->pNext;
			}
			if (nStreamDesSize != pmt.ES_info_length[m])
			{
				VOLOGE("TS Muxer Error:stream info descriptro error");
			}
			total_bits += nStreamDesSize*8;
		}

		int len = total_bits/8-ts_header_len;
		pmt.crc_32	= CalcCRC32(pStart, len);
	//	VO_U32* p = (VO_U32*)m_pTsPacekt->Position();
		total_bits	+= m_pTsPacekt->WriteBits(16, pmt.crc_32>>16);
		total_bits	+= m_pTsPacekt->WriteBits(16, pmt.crc_32);

		CheckCRC32(pStart, len);
		//int n = 0;
	}

	return total_bits;
}

VO_U8 CTsAssembler::GetStreamType(VO_U32 nStreamID,VO_U16 nCodec)
{
	VO_U8 type = 0xFF;
	if (nStreamID == 0)///<Audio
	{
		switch (nCodec)
		{
		case VO_AUDIO_CodingAAC:
			type = 0x0F;
			break;
		case VO_AUDIO_CodingMP1:
			type = 0x03;			// ISO/IEC 11172 Audio(layer 2)
			break;
		case VO_AUDIO_CodingMP3:
			type = 0x04;			// ISO/IEC 13818-3 Audio(layer 3)
			break;
		case VO_AUDIO_CodingPCM:
			type = 0x80;
			break;
		}
	}
	else
	{
		switch (nCodec)
		{
		case VO_VIDEO_CodingMPEG4:
			type = 0x10;
			break;
		case VO_VIDEO_CodingH264:
			type = 0x1B;
			break;
		case VO_VIDEO_CodingMPEG2:
			type = 0x02;			// ITU-T Rec. H.262 | ISO/IEC 13818-2 Video or ISO/IEC 11172-2 constrained parameter video stream
			break;
		}
	}
	

	return type;
}

VO_U8 CTsAssembler::GetStreamID(VO_U32 nStreamID,VO_U16 nCodec)
{
	// audio start with '110x xxxx' 0xC0
	// video start with '1110 xxxx' 0xE0
	VO_U8 id = 0xE0;

	if(nStreamID == 0)
	{

		switch (nCodec)
		{
		case VO_AUDIO_CodingPCM:
			{
				id = 0xBD;///<PCM muxed PES as private_stream_1 
			}
			break;
		default:
			{
				id = 0xC0;
			}
			break;
		}
	}
	else
	{
		id = 0xE0;
	}
	return id;
}

VO_U32	CTsAssembler::AssembleCommonPSI(CTableHeader* pHeader,Psi_Type eType)
{
	VO_U32 total_bits = 0;

	total_bits += m_pTsPacekt->WriteBits(8, pHeader->table_id);
	total_bits += m_pTsPacekt->WriteBits(1, pHeader->section_synctax_indicator);
	total_bits += m_pTsPacekt->WriteBits(1, pHeader->unknown);
	total_bits += m_pTsPacekt->WriteBits(2, 3);
	total_bits += m_pTsPacekt->WriteBits(12, pHeader->section_length);
	if (eType == Psi_Type_PAT)
	{
		total_bits += m_pTsPacekt->WriteBits(16, pHeader->transport_stream_id);
	}
	else if (eType == Psi_Type_PMT)
	{
		total_bits += m_pTsPacekt->WriteBits(16, pHeader->program_number);
	}

	
	total_bits += m_pTsPacekt->WriteBits(2, 3);
	total_bits += m_pTsPacekt->WriteBits(5, pHeader->version_number);
	total_bits += m_pTsPacekt->WriteBits(1, pHeader->current_next_indicator);
	total_bits += m_pTsPacekt->WriteBits(8, pHeader->section_number);
	total_bits += m_pTsPacekt->WriteBits(8, pHeader->last_section_number);

	return total_bits;
}

VO_U32	CTsAssembler::AssembleBasicTable(VO_BOOL bForce)
{
	if(!m_pAssembleInitInfo)
		return VO_ERR_SINK_OPENFAIL;
	
	VO_U32 total_bits = 0;

	if (bForce || ++m_uBasicPeriod == DEFAULT_PAT_PMT_PERIOD)
	{
		m_uBasicPeriod = 0;

		// Assemble PAT
		total_bits += AssemblePAT();
		Write(m_pTsPacekt->GetPacket(), m_pTsPacekt->GetPacketSize());
		m_pTsPacekt->Reset();

		// Assemble PMT
		total_bits += AssemblePMT();
		Write(m_pTsPacekt->GetPacket(), m_pTsPacekt->GetPacketSize());
		m_pTsPacekt->Reset();
	}
	

	// end of Assemble PAT
	return total_bits;
}

VO_VOID CTsAssembler::CheckHeadData(VO_BOOL bAudio, VO_U16 nStreamID, VO_U16 nCodec, VO_U32 nFrameSize, VO_BYTE** outHead, VO_U16* outHeadLen)
{
	VO_BYTE* extra_data		= VO_NULL;
	VO_U16 extra_data_size	= 0;

	CItem* item = m_HeadDataList.GetItemByID(nStreamID);

	if (item)
	{
		extra_data		= (VO_BYTE*)item->Data();
		extra_data_size	= item->DataLen();
	}

	*outHead	= extra_data;
	*outHeadLen = extra_data_size;
}

VO_U16 CTsAssembler::PrepareAdtsHeader(VO_BYTE* adts, VO_BYTE* pExt, VO_U16 nFrameSize)
{
	if (!pExt)
	{
		return 0;
	}

	int size			= nFrameSize + 7;
	int aac_index		= ( (pExt[0] << 1) | (pExt[1] >> 7) ) & 0x0f;
	int aac_profile		= (pExt[0] >> 3) - 1; // i_profile < 4 
	int aac_channels	= (pExt[aac_index == 0x0f ? 4 : 1] >> 3) & 0x0f;

	adts[0] = 0xff;
	adts[1] = 0xf1;
	adts[2] = (aac_profile << 6) | ((aac_index & 0x0f) << 2) | ((aac_channels >> 2) & 0x01) ;
	adts[3] = (aac_channels << 6) | ((size >> 11) & 0x03);

	int i_fullness = 0x7ff; // 0x7ff means VBR 
	// XXX: We should check if it's CBR or VBR, but no known implementation
	//do that, and it's a pain to calculate this field 

	adts[4] = size >> 3;
	adts[5] = ((size & 0x07) << 5) | ((i_fullness >> 6) & 0x1f);
	adts[6] = ((i_fullness & 0x3f) << 2); // | 0xfc ;

	return 7*8;
}

VO_VOID CTsAssembler::SetPesPacketLength(VO_U16 nLen)
{
	if (!m_pTsPacekt)
	{
		return;
	}

	VO_BYTE* pes_packet_length_addr		= m_pTsPacekt->GetPacket() + 4 + 4; // 4 is packet_start_code_prefix ~ stream_id
	*(VO_U16*)pes_packet_length_addr	= nLen;
}

VO_BOOL CTsAssembler::IsADTSSyncWord(VO_BYTE* p)
{
	VO_BOOL bRet = ( (*p == 0xff) && ((*(p+1) & 0xf0) == 0xf0) )?VO_TRUE:VO_FALSE;

	if (!bRet)
	{
		p += 2;
		bRet = ( (*p == 0xff) && ((*(p+1) & 0xf0) == 0xf0) )?VO_TRUE:VO_FALSE;
	}

	return bRet;
}

VO_VOID CTsAssembler::doAddSample(VO_BOOL bSync, VO_SINK_SAMPLE * pSample)
{
	if (!pSample || !pSample->Buffer)
	{
		return;
	}

	VO_SINK_SAMPLE TempSample = *pSample;

	VO_U32 uMuxedBit		= 0;
	VO_U32	uCurPayloadSize	= 0;
	VO_BOOL	bStart			= VO_TRUE;
	VO_U32 nMinAdaptationLength = 0;
	VO_S64 uLeftSampleData = TempSample.Size;
	VO_BOOL bMuxPCR = VO_FALSE;

	while (uLeftSampleData > 0)
	{
		AssembleBasicTable();
		nMinAdaptationLength = 0;
		if (bStart)
		{
			VO_U16 uHeadLen		= 0;
			VO_PBYTE pHeadData		= VO_NULL;
			VO_U32	uPreFixLen		= 0;

			if(bSync)
			{
				CheckHeadData(TempSample.nAV==0?VO_TRUE:VO_FALSE, TempSample.nAV, m_pAssembleInitInfo->codec_list[TempSample.nAV], TempSample.Size, &pHeadData, &uHeadLen);
			}
			if (TempSample.nAV == 1 && m_pAssembleInitInfo->codec_list[TempSample.nAV] == VO_VIDEO_CodingH264)
			{
				if (VO_FALSE == HasH264Prefix(TempSample.Buffer,TempSample.Size))
				{
					uPreFixLen = 6;///<0x0000000109F0
				}	
				if (bSync)
				{
					if(HasH264HeadData(TempSample.Buffer,TempSample.Size))
					{
						uHeadLen = 0;///<if there is head data,we will not assemble the head data for H264
					}
					else
					{
						VO_U32 uSkip = 0;
						VO_PBYTE pTemp = TempSample.Buffer;
						if ((TempSample.Buffer[3] & 0x1F )== 9)
						{
							uSkip = 4;
						}
						else if ((TempSample.Buffer[4] & 0x1F )== 9)
						{
							uSkip = 5;
						}
						if (uSkip)
						{
							pTemp += uSkip;
							pTemp = FindH264SyncPos(pTemp,TempSample.Size - uSkip,NULL);
							if (pTemp)
							{
								TempSample.Size -= (pTemp - TempSample.Buffer);
								TempSample.Buffer = pTemp;
								uLeftSampleData = TempSample.Size;

								uPreFixLen = 6;///<0x0000000109F0
							}
							else
							{
								uHeadLen = 0;
							}
							
						}
						
					}
				}
				if(uPreFixLen + uHeadLen)
				{
					if (m_uBufLen < TempSample.Size + uHeadLen + uPreFixLen)
					{
						if (m_pBufData)
						{
							delete []m_pBufData;
						}
						m_uBufLen = TempSample.Size + uHeadLen + uPreFixLen;
						m_pBufData = new VO_BYTE[m_uBufLen];
					}
					if (uPreFixLen)
					{
						VO_U64 uPrefix = 0xF00901000000LL;
						memcpy(m_pBufData,&uPrefix,6);
					}
					if (pHeadData && uHeadLen)
					{
						memcpy(m_pBufData+uPreFixLen,pHeadData,uHeadLen);
					}
					memcpy(m_pBufData+uPreFixLen+uHeadLen,TempSample.Buffer,TempSample.Size);
					TempSample.Buffer = m_pBufData;
					TempSample.Size = TempSample.Size + uPreFixLen + uHeadLen;
					uLeftSampleData = TempSample.Size;
				}
			}
		}

		uCurPayloadSize	   = uLeftSampleData;

		VO_U32 uPesHeaderLen = 0;
		if (bStart)
		{
			uPesHeaderLen = PreparePesHeader(&pes, GetStreamID(TempSample.nAV,m_pAssembleInitInfo->codec_list[TempSample.nAV]), TempSample.Time, TempSample.DTS,uCurPayloadSize);
			if (TempSample.nAV == 1)///<for video,we need mux the PCR
			{
				bMuxPCR = VO_TRUE;
			}
		}	
		if (bMuxPCR)
		{
			nMinAdaptationLength = 8;
		}
		VO_S32 Tmp = m_pTsPacekt->GetPacketSize() - 4 - uPesHeaderLen - uCurPayloadSize - nMinAdaptationLength;
		if (Tmp >= 2)///<need stuff byte
		{
			nMinAdaptationLength += Tmp;
		}
		else if (Tmp == 1)///<the adaptation_field_length must >=2
		{
			if (bMuxPCR)
			{
				nMinAdaptationLength += 1;
			}
			else
				nMinAdaptationLength += 2;///<if no pcr,it indicate no Adaptation,set the min Adaptation length is 2
		}

		VO_U32 uBitTSPacketHeaderLen = 0;
		if (TempSample.nAV == 1)
		{
			uBitTSPacketHeaderLen = AssembleTsPacketHeader(pmt.elementary_PID[TempSample.nAV], bStart== VO_TRUE?1:0, VO_FALSE, nMinAdaptationLength,bMuxPCR,TempSample.DTS);
		}
		else
			uBitTSPacketHeaderLen = AssembleTsPacketHeader(pmt.elementary_PID[TempSample.nAV], bStart== VO_TRUE?1:0, VO_FALSE, nMinAdaptationLength,bMuxPCR);

		uMuxedBit += uBitTSPacketHeaderLen;

		if (bStart)
		{
			uMuxedBit += AssemblePesHeader(&pes);

			if (TempSample.nAV == 1 && m_pAssembleInitInfo->codec_list[TempSample.nAV] == VO_VIDEO_CodingH264)
			{
				uMuxedBit += AssembleH264Prefix(TempSample.Buffer,TempSample.Size);
			}
		}

		{
			bStart = VO_FALSE;
			bMuxPCR = VO_FALSE;
		}
		
		VO_U32 uPayloadSize	= m_pTsPacekt->GetPacketSize()-(uMuxedBit/8);

		if (uPayloadSize > uLeftSampleData)
		{
			VOLOGE("TS Muxer Error:uPayloadSize < uLeftSampleData:%lld",TempSample.Time);
			m_pTsPacekt->Reset();
			return;
		}
		
		uMuxedBit	+= m_pTsPacekt->WriteByte(uPayloadSize, TempSample.Buffer+(TempSample.Size - uLeftSampleData));
		if (uMuxedBit != 1504)
		{
			VOLOGE("TS Muxer Error:uMuxedBit != 1504:%lld",TempSample.Time);
		}
		uLeftSampleData -= uPayloadSize;
		WriteTsPacket();
		uMuxedBit = 0;
	}
}

VO_U16 CTsAssembler::PreparePesHeader(CPESPacket* pPes, VO_U8 nStreamID, VO_U64 nPTS, VO_U64 nDTS,VO_U32 uPayLoadSize)
{
	VO_U16 total_bits = 0;
	cmnMemSet(0, pPes, 0, sizeof(CPESPacket));

	total_bits += 24;
	pPes->packet_start_code_prefix	= 0x000001;
	
	total_bits += 8;
	pPes->stream_id					= nStreamID;

	total_bits += 16;
	if (nStreamID != 0xE0)
	{
		pPes->PES_packet_length = uPayLoadSize + 8;///<for non-video,we must calcaulate the PES length
	}
	else
		pPes->PES_packet_length			= 0;

	// add adjust field, refer spec p31
	total_bits += 2;

	total_bits += 2;
	pPes->PES_scrambling_control	= 0;

	total_bits += 1;
	pPes->PES_priority				= 0;

	total_bits += 1;
	pPes->data_alignment_indicator	= 1;

	total_bits += 1;
	pPes->copyright					= 0;

	total_bits += 1;
	pPes->original_or_copy			= 0;
	
	total_bits += 2;
	if (nStreamID == 0xe0)
	{
		pPes->PTS_DTS_flags				= 3;	//	use PTS,DTS '11'
	}
	else
		pPes->PTS_DTS_flags				= 2;	//	use PTS,DTS '11'
	
	
	total_bits += 1;
	pPes->ESCR_flag					= 0;
	
	total_bits += 1;
	pPes->ES_rate_flag				= 0;
	
	total_bits += 1;
	pPes->DSM_trick_mode_flag		= 0;

	total_bits += 1;
	pPes->additional_copy_info_flag	= 0;

	total_bits += 1;
	pPes->PES_CRC_flag				= 0;

	total_bits += 1;
	pPes->PES_extension_flag		= 0;

	total_bits += 8;
	if (pPes->PTS_DTS_flags == 3)
		pPes->PES_header_data_length	= 10;	//	PES_header_data_length指的是pes头里面的可选字段(带flag的对应字段)和填充字段的长度.
	else
		pPes->PES_header_data_length	= 5;

	total_bits += 40;
	pPes->PTS						= nPTS;//*90;//nPTS*90000/1000;

	if (pPes->PTS_DTS_flags == 3)
	{
		total_bits += 40;
		pPes->DTS						= nDTS;//*90;//90000/1000;
	}

	return total_bits/8;
}

VO_U16	CTsAssembler::CalcPesPacketLength(VO_BOOL bSync, VO_BOOL bVideo, VO_U16 nCodec, VO_U32 nFrameLength, VO_BYTE nTotalTsPacketSize, VO_BYTE nPesHeaderLen)
{
#ifndef SET_PES_LENGTH
	return 0;
#endif

	VO_U16	pes_packet_length	= 0;
	VO_BYTE extra_data_len		= 0;
	VO_SINK_EXTENSION* pExt		= GetExtDataByID(bVideo?1:0);

	if (bSync)
	{
		if (nCodec == VO_VIDEO_CodingMPEG4)
		{
#ifdef ADD_VIDEO_HEAD_DATA
			extra_data_len = pExt?pExt->Size:0;
#endif
		}
		else if (nCodec == VO_VIDEO_CodingH264)
		{
#ifdef ADD_VIDEO_HEAD_DATA
			extra_data_len = pExt?pExt->Size:0;
#endif
		}
		else if (nCodec == VO_AUDIO_CodingAAC)
		{
			extra_data_len = 7;
		}
	}
	else
	{
		if (nCodec == VO_AUDIO_CodingAAC)
		{
			// adts head len
			extra_data_len = 7;
		}
	}

	// 4 is ts header len
	int tmp = nTotalTsPacketSize - 4 - nPesHeaderLen - extra_data_len - nFrameLength;
	if (tmp >= 0)	// ==0 mean one ts packet
	{
		// 6 is length of packet_start_code_prefix ~ PES_packet_length
		pes_packet_length = (nPesHeaderLen - 6) + extra_data_len + nFrameLength;

		if(nCodec != VO_AUDIO_CodingAAC)
			VOLOGI("pes_packet_length = %d", pes_packet_length);
			//CDumper::WriteLog("pes_packet_length = %d", pes_packet_length);

		return pes_packet_length;
	}

	// more than 1 ts packet
	// 4 is ts header len
	VO_U16 frame_length_in_first_ts_packet	= nTotalTsPacketSize - 4 - nPesHeaderLen - extra_data_len;
	VO_U16 left_frame_length				= nFrameLength - frame_length_in_first_ts_packet;

	VO_BYTE ts_count			= left_frame_length / (nTotalTsPacketSize-4);
	VO_BYTE	last_frame_length	= left_frame_length % (nTotalTsPacketSize-4);

#if 0
	pes_packet_length = (nTotalTsPacketSize - 4 - 6) + (nTotalTsPacketSize-4)*ts_count;

	if (last_frame_length > 0)
	{
		pes_packet_length += last_frame_length;
	}
#else	

// 	pes_packet_length = nFrameLength + extra_data_len + nPesHeaderLen - 6;
// 	return pes_packet_length;

	pes_packet_length = (nTotalTsPacketSize - 4 - 6) + (nTotalTsPacketSize)*ts_count;

	if (last_frame_length > 0)
	{
		//pes_packet_length += nTotalTsPacketSize;
		pes_packet_length += last_frame_length;
	}

#endif

	if(nCodec != VO_AUDIO_CodingAAC)
		VOLOGI("pes_packet_length = %d", pes_packet_length);
		//CDumper::WriteLog("pes_packet_length = %d", pes_packet_length);

	return pes_packet_length;
}


VO_U32	CTsAssembler::AssemblePesHeader(CPESPacket* pPes)
{
	VO_U32 total_bits = 0;

	total_bits += m_pTsPacekt->WriteBits(24, pPes->packet_start_code_prefix);
	total_bits += m_pTsPacekt->WriteBits(8, pPes->stream_id);
	total_bits += m_pTsPacekt->WriteBits(16, pPes->PES_packet_length);

	// add adjust field, refer spec p31
	total_bits += m_pTsPacekt->WriteBits(2, 2); // '10'

	//pes.PES_scrambling_control		= 0;
	total_bits += m_pTsPacekt->WriteBits(2, pPes->PES_scrambling_control);

	//pes.PES_priority				= 1;
	total_bits += m_pTsPacekt->WriteBits(1, pPes->PES_priority);

	//pes.data_alignment_indicator	= 0;
	total_bits += m_pTsPacekt->WriteBits(1, pPes->data_alignment_indicator);

	//pes.copyright					= 0;
	total_bits += m_pTsPacekt->WriteBits(1, pPes->copyright);
														
	//pes.original_or_copy			= 1;
	total_bits += m_pTsPacekt->WriteBits(1, pPes->original_or_copy);
	
	//pes.PTS_DTS_flags				= 3;	//	use PTS,DTS
	total_bits += m_pTsPacekt->WriteBits(2, pPes->PTS_DTS_flags);

	//pes.ESCR_flag					= 0;
	total_bits += m_pTsPacekt->WriteBits(1, pPes->ESCR_flag);
	
	pes.ES_rate_flag				= 0;
	total_bits += m_pTsPacekt->WriteBits(1, pPes->ES_rate_flag);

	//pes.DSM_trick_mode_flag			= 0;
	total_bits += m_pTsPacekt->WriteBits(1, pPes->DSM_trick_mode_flag);

	//pes.additional_copy_info_flag	= 0;
	total_bits += m_pTsPacekt->WriteBits(1, pPes->additional_copy_info_flag);
	
	//pes.PES_CRC_flag				= 0;
	total_bits += m_pTsPacekt->WriteBits(1, pPes->PES_CRC_flag);


	//pes.PES_extension_flag			= 0;
	total_bits += m_pTsPacekt->WriteBits(1, pPes->PES_extension_flag);


	//pes.PES_header_data_length		= 0;	//	????
	total_bits += m_pTsPacekt->WriteBits(8, pPes->PES_header_data_length);

	// 填充字段和可选字段
	VO_BYTE length = total_bits;

	// PTS
	total_bits += m_pTsPacekt->WriteBits(4, pPes->PTS_DTS_flags); // '0010'
	total_bits += m_pTsPacekt->WriteBits(3, pPes->PTS >> 30 );
	total_bits += m_pTsPacekt->WriteBits(1, 0x01 ); // marker
	total_bits += m_pTsPacekt->WriteBits(15, pPes->PTS >> 15 );
	total_bits += m_pTsPacekt->WriteBits(1, 0x01 ); // marker
	total_bits += m_pTsPacekt->WriteBits(15, pPes->PTS );
	total_bits += m_pTsPacekt->WriteBits(1, 0x01 ); // marker

	// DTS
	if (pPes->PTS_DTS_flags == 3)
	{
		total_bits += m_pTsPacekt->WriteBits(4, 1); // '0001'
		total_bits += m_pTsPacekt->WriteBits(3, pPes->DTS >> 30 );
		total_bits += m_pTsPacekt->WriteBits(1, 0x01 ); // marker
		total_bits += m_pTsPacekt->WriteBits(15, pPes->DTS >> 15 );
		total_bits += m_pTsPacekt->WriteBits(1, 0x01 ); // marker
		total_bits += m_pTsPacekt->WriteBits(15, pPes->DTS );
		total_bits += m_pTsPacekt->WriteBits(1, 0x01 ); // marker
	}
	length = (total_bits - length)/8;
	length = pPes->PES_header_data_length - length;
	while ( length )
	{
		total_bits += m_pTsPacekt->WriteBits(8, 0xFF);
		length--;
	}

	return total_bits;
}


VO_VOID	CTsAssembler::WriteTsPacket()
{
	if (m_pTsPacekt)
	{
		Write(m_pTsPacekt->GetPacket(), m_pTsPacekt->GetPacketSize());
		m_pTsPacekt->Reset();
	}
}

VO_VOID	CTsAssembler::InitContinuityCounter()
{
	m_continuity_counter_count = 0;

	for (VO_U16 n=0; n<MAX_PID_COUNT; n++)
	{
		m_continuity_counter[n].continuity_counter = 0;
		m_continuity_counter[n].pid				   = -1;
	}

	// PAT
	RegisterContinuityCounter(0x00);
	// PMT
	RegisterContinuityCounter(program_map_PID_base);

	// ES pid
	for (VO_BYTE n=0; n<12; n++)
	{
		RegisterContinuityCounter(elementary_PID_base+n);
	}
}

VO_VOID	CTsAssembler::RegisterContinuityCounter(VO_U16 pid)
{
	m_continuity_counter[m_continuity_counter_count].continuity_counter = 0;
	m_continuity_counter[m_continuity_counter_count].pid				= pid;

	m_continuity_counter_count++;
}

VO_BYTE	CTsAssembler::GetContinuityCount(VO_U16 pid)
{
	for (VO_U16 n=0; n<m_continuity_counter_count; n++)
	{
		if (m_continuity_counter[n].pid == pid)
		{
			VO_BYTE counter = m_continuity_counter[n].continuity_counter;
			m_continuity_counter[n].continuity_counter = (counter+1)%16;
			return counter;
		}
	}

	return 0;
}

VO_VOID	CTsAssembler::AddExtData(VO_U16 nStreamID, VO_SINK_EXTENSION* pExtData)
{
	if (!pExtData || !pExtData->Buffer)
	{
		VOLOGE("TsWriter Log:NULL pExtData Data");
		return;
	}
	switch (pExtData->nAV)
	{
	case 0:
		{
			switch (m_pAssembleInitInfo->codec_list[nStreamID])
			{
			case VO_AUDIO_CodingPCM:
				{
					pExtData->Size = EnCodePCMHead(pExtData->Buffer,pExtData->Size);
				}
				break;
			case VO_AUDIO_CodingAAC:///<for ADTS aac ,we don't need the head data
				{
					VOLOGE("TsWriter Log:don't add AAc head data");
				}
				return;
			default:
				break;
			}
		}
		break;
	default:
		
		break;
	}
	
	if (nStreamID == 0)
	{
		CDumper::DumpAudioData((VO_PBYTE)&pExtData->Size,4);
		CDumper::DumpAudioData(pExtData->Buffer,pExtData->Size);
	}
	else
	{
		CDumper::DumpVideoData((VO_PBYTE)&pExtData->Size,4);
		CDumper::DumpVideoData((VO_PBYTE)pExtData->Buffer,pExtData->Size);
	}
	VOLOGE("TsWriter Log:Release last Video head data:streamid=%d",nStreamID);
	ReleaseExtData();
	m_HeadDataList.Release();

	m_pExtData[nStreamID] = CloneExtData(pExtData);

	AddHeadData(nStreamID, m_pExtData[nStreamID]->Buffer, m_pExtData[nStreamID]->Size);
	VOLOGE("TsWriter Log:add new Video head data:streamid=%d:headsize=%d",nStreamID,m_pExtData[nStreamID]->Size);
}

VO_U32 CTsAssembler::EnCodePCMHead(VO_PBYTE pData,VO_U32 nSize)
{
	if (!pData || nSize < sizeof(VO_SINK_MEDIA_INFO))
	{
		VOLOGE("TS Muxer Error:Wrong pExtData Data");
		return 0;
	}
	VO_SINK_MEDIA_INFO pPCMInfo;
	memcpy(&pPCMInfo,pData,nSize);
	memset(pData,0,nSize);

	VO_U16 nSizeInByte = pPCMInfo.AudioFormat.nSample_rate/100 * pPCMInfo.AudioFormat.nSample_bits /8;
	pData[0] = nSizeInByte >> 8;
	pData[1] = (VO_BYTE)nSizeInByte;
	VO_U8 Index = 0;
	switch (pPCMInfo.AudioFormat.nChannels)
	{
	case 0:
		{
			Index = 0;
		}
		break;
	case 1:
		{
			Index = 1;
		}
		break;
	case 2:
		{
			Index = 3;
		}
		break;
	case 3:
		{
			Index = 4;
		}
		break;
	case 4:
		{
			Index = 6;
		}
		break;
	case 5:
		{
			Index = 8;
		}
		break;
	case 6:
		{
			Index = 9;
		}
		break;
	case 7:
		{
			Index = 10;
		}
		break;
	case 8:
		{
			Index = 11;
		}
		break;
	}
	pData[2] = (Index << 4)&0xF0;

	Index = 0;
	switch (pPCMInfo.AudioFormat.nSample_rate)
	{
	case 48000:
		{
			Index = 1;
		}
		break;
	case 96000:
		{
			Index = 4;
		}
		break;
	case 192000:
		{
			Index = 5;
		}
		break;
	default:
		{

		}
		break;
	}
	pData[2] |= (Index & 0x0F);

	Index = 0;
	switch (pPCMInfo.AudioFormat.nSample_bits)
	{
	case 16:
		{
			Index = 1;
		}
		break;
	case 20:
		{
			Index = 2;
		}
		break;
	case 24:
		{
			Index = 3;
		}
		break;
	default:
		break;
	}
	pData[3] = ((Index << 6) & 0xC0);
	pData[3] |= ((1 << 5) & 0x20);
	return 4;
}

VO_VOID	CTsAssembler::AddSample(VO_BOOL bSync, VO_SINK_SAMPLE * pSample)
{
	if (pSample->nAV == 0 && m_pAssembleInitInfo->codec_list[pSample->nAV] == VO_AUDIO_CodingPCM)
	{
		VO_U16* pTempBuf = (VO_U16*)pSample->Buffer;///<PCM in TS must be Big Endian
		VO_U32 nSize = (pSample->Size)/2;
		for (VO_U32 nCnt = 0;nCnt < nSize;nCnt++)
		{
			pTempBuf[nCnt] = (pTempBuf[nCnt] >> 8)|(pTempBuf[nCnt]<<8);
		}
		bSync = VO_TRUE;///<we need set it true,so we will add the ext data to every sample for PCM
		doAddSample(bSync,pSample);			
	}
	else
		doAddSample(bSync, pSample);
}

VO_U32 CTsAssembler::AssembleH264Prefix(VO_PBYTE pSampleData ,VO_U32 uLen)
{
	VO_U32 uBitCnt = 0;
	if (VO_FALSE == HasH264Prefix(pSampleData,uLen))
	{
		VO_U64 uPrefix = 0xF00901000000LL;
		uBitCnt += m_pTsPacekt->WriteByte(6, (uint8*)&uPrefix);
	}
	return uBitCnt;
}

VO_BOOL CTsAssembler::HasH264Prefix(VO_PBYTE pSampleData ,VO_U32 uLen)
{
	if (uLen < 5 || !pSampleData)
	{
		return VO_FALSE;
	}
	VO_PBYTE pTmp = pSampleData;
	
	if((pTmp[2] == 0x1 && (pTmp[3] & 0x1F )!= 9) || (pTmp[3] == 0x1 && (pTmp[4] & 0x1F )!= 9))
	{
		return VO_FALSE;
	}
	return VO_TRUE;
}

VO_BOOL CTsAssembler::HasH264HeadData(VO_PBYTE pSampleData ,VO_U32 uLen)
{
	VO_CODECBUFFER stBuffer = {0};
	stBuffer.Buffer = pSampleData;
	stBuffer.Length = uLen;

	VO_S32	ret = 0;
	ret = m_funH264VideoParser.Process(m_pH264VideoParser,&stBuffer);
//	ret = m_funH264VideoParser.GetParam(m_pH264VideoParser,VO_PID_VIDEOPARSER_WIDTH,&sWidth);///<if we can get the width,so there must be head data in it
	if (ret == VO_RETURN_SQHEADER || ret == VO_RETURN_SQFMHEADER)
	{
		return VO_TRUE;
	}
	return VO_FALSE;
}

VO_PBYTE CTsAssembler::FindH264SyncPos(VO_PBYTE pData , VO_U32 uLen , VO_U32* uSyncWord)
{
	VO_PBYTE pTemp = pData;
	VO_BYTE *pEnd = pData + uLen - 4;

	while (pTemp < pEnd)
	{
		if(pTemp[0] == 0 && pTemp[1] == 0 )
		{	
			if(pTemp[2] == 0 && pTemp[3] == 1)
			{
				if (uSyncWord)
				{
					*uSyncWord = 4;
				}
				return pTemp;
			}
			else if(pTemp[2] == 1)
			{
				if (uSyncWord)
				{
					*uSyncWord = 3;
				}
				return pTemp;
			}
		}
		pTemp += 1;
	}
	return NULL;
}

VO_U32 CTsAssembler::FormatChange()
{
	ReleaseExtData();
	m_HeadDataList.Release();
	VOLOGE("VO_PID_SINK_FORMAT_CHANGE End");
	return VO_ERR_NONE;
}