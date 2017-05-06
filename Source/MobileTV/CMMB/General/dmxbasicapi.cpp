#include "dmxbasicapi.h"

/***********************************************
************************************************
*				CRC check                  *
************************************************
************************************************/
const int CRC8_SEED		= 0x31;
const int CRC16_SEED	= 0x8005;
const int CRC32_SEED	= 0x04C11DB7;
static unsigned long crc32_table[256];
static unsigned char crc8_table[256];

void BuildCrcTable()
{
	for (int i = 0; i < 256; i++ )
	{
		unsigned char nData = i;
		unsigned char nAccum = 0;
		for (size_t j = 0; j < 8; j++ )
		{
			if ( ( nData ^ nAccum ) & 0x80 )
				nAccum = ( nAccum << 1 ) ^ CRC8_SEED;
			else
				nAccum <<= 1;
			nData <<= 1;
		}
		crc8_table[i] = nAccum;
	}
	/*
	for (size_t i = 0; i < 256; i++ )
	{
	unsigned short nData = ( unsigned short )( i << 8 );
	unsigned short nAccum = 0;
	for (size_t j = 0; j < 8; j++ )
	{
	if ( ( nData ^ nAccum ) & 0x8000 )
	nAccum = ( nAccum << 1 ) ^ CRC16_SEED;
	else
	nAccum <<= 1;
	nData <<= 1;
	}
	crc16_table[i] = nAccum;
	}
	*/
	for (size_t i = 0; i < 256; i++ )
	{
		unsigned long nData = ( unsigned long )( i << 24 );
		unsigned long nAccum = 0;
		for (size_t j = 0; j < 8; j++ )
		{
			if ( ( nData ^ nAccum ) & 0x80000000 )
				nAccum = ( nAccum << 1 ) ^ CRC32_SEED;
			else
				nAccum <<= 1;
			nData <<= 1;
		}
		crc32_table[i] = nAccum;
	}
}

unsigned char CalcCRC8(const LPBYTE pbuf, size_t len) 
{ 
	unsigned char crc = 0; 
	for(unsigned i=0; i < len; i++) 
	{ 
		crc = crc8_table[crc ^ pbuf[i]]; 
	} 
	return crc; 
}

unsigned long CalcCRC32(const LPBYTE inData, size_t inLen)
{
	unsigned long dwRegister = 0xFFFFFFFF;
	for(unsigned long i=0; i<inLen; i++ )
	{
		dwRegister = ((dwRegister) << 8) ^ crc32_table[(inData[i]) ^ (dwRegister>>24)];
	}
	return dwRegister;
}

bool CheckCRC32(const LPBYTE lpIn, size_t len)
{
	unsigned long dwCalCRC		= CalcCRC32(lpIn, len);
	unsigned long u32CRCValue	= GET32(lpIn + len);
	return dwCalCRC == u32CRCValue;
}


int ParseNIT(BYTE* pData, int nLen, NetworkInfoTable* pInfo)
{
	if(false == CheckCRC32(pData, nLen-NIT_CRC32_LEN))
		return DEMUX_NIT_CRC_ERROR;

	BYTE  nTemp;
	WORD  wTemp;

	pInfo->table_id			= STEP8(pData);

	nTemp					= STEP8(pData);
	pInfo->nit_update_sn	= nTemp >> 4;

	pInfo->mjd_date			= STEP16(pData);
	pInfo->hour				= STEP8(pData);
	pInfo->minute			= STEP8(pData);
	pInfo->second			= STEP8(pData);

	pInfo->country_code[0]	= STEP8(pData);
	pInfo->country_code[1]	= STEP8(pData);
	pInfo->country_code[2]	= STEP8(pData);

	wTemp					= STEP16(pData);
	pInfo->net_level		= (wTemp>>12) & 0x0f;
	pInfo->net_id			= wTemp & 0x0fff;

	pInfo->net_name_len		= STEP8(pData);
	for(BYTE n=0; n<pInfo->net_name_len; n++)
	{
		pInfo->net_name[n]	= STEP8(pData);
	}
	//pInfo->net_name[pInfo->net_name_len] = '\0';

	pInfo->freq_sn			= STEP8(pData);
	pInfo->center_freq		= STEP32(pData);

	nTemp					= STEP8(pData);
	pInfo->band_width		= (nTemp>>4) & 0x0f;
	pInfo->net_other_freq_num = nTemp & 0x0f;
	for(BYTE n=0; n<pInfo->net_other_freq_num; n++)
	{
		pInfo->net_other_freq[n].freq_sn		= STEP8(pData);
		pInfo->net_other_freq[n].center_freq	= STEP32(pData);

		nTemp									= STEP8(pData);
		pInfo->net_other_freq[n].band_width		= (nTemp>>4) & 0x0f;
		//reserved
	}

	nTemp = STEP8(pData);
	pInfo->near_net_num	= (nTemp>>4) & 0x0f;
	for(BYTE n=0; n<pInfo->near_net_num; n++)
	{
		wTemp										= STEP16(pData);
		pInfo->neighbor_net[n].neighbor_net_level	= (wTemp>>12);
		pInfo->neighbor_net[n].neighbor_net_id		= wTemp & 0x0fff;
		pInfo->neighbor_net[n].neighbor_freq_sn		= STEP8(pData);
		pInfo->neighbor_net[n].neighbor_center_freq = STEP32(pData);
		nTemp										= STEP8(pData);
		pInfo->neighbor_net[n].neighbor_band_width	= nTemp>>4;
	}

	//crc 32

	return DEMUX_OK;
}


int ParseCMCT(BYTE* pData, int nLen, ContinueServiceMultiControlTable* pCMCT)
{
	if(false == CheckCRC32(pData, nLen-CMCT_CRC32_LEN))
		return DEMUX_CMCT_CRC_ERROR;

	pCMCT->table_id			= STEP8(pData);		
	pCMCT->freq				= STEP8(pData);

	WORD wTemp							= STEP16(pData);
	pCMCT->multi_table_update_sn		= (wTemp >> 12);
	pCMCT->mf_num						= (wTemp & 0x3F);

	for(BYTE i=0; i<pCMCT->mf_num; i++)
	{
		BYTE nTemp					= STEP8(pData);
		pCMCT->mf_info[i].mf_id		= nTemp >> 2;
		pCMCT->mf_info[i].rs		= nTemp & 3;

		wTemp										= STEP16(pData);
		pCMCT->mf_info[i].byte_interlace			= wTemp >> 14;
		pCMCT->mf_info[i].ldpc						= (wTemp >> 12) & 3;
		pCMCT->mf_info[i].mode						= (wTemp >> 10) & 3;
		pCMCT->mf_info[i].scramble_code				= (wTemp >> 6) & 7;		//
		pCMCT->mf_info[i].ts_num					= wTemp & 0x3f;			//

		for(BYTE n=0; n<pCMCT->mf_info[i].ts_num; n++)
		{
			pCMCT->mf_info[i].ts_sn[n] = STEP8(pData) >> 2;;
		}

		nTemp						= STEP8(pData);
		pCMCT->mf_info[i].msf_num	= nTemp & 0xF;

		for(BYTE j=0; j<pCMCT->mf_info[i].msf_num; j++)
		{
			nTemp = STEP8(pData);
			pCMCT->mf_info[i].msf_info[j].msf_sn		= nTemp >> 4;
			pCMCT->mf_info[i].msf_info[j].service_id	= STEP16(pData);
		}
	}

	return DEMUX_OK;
}

int ParseCSCT(BYTE* pData, int nLen, ContinueServiceControlTable* pCSCT)
{
	if(false == CheckCRC32(pData, nLen-CSCT_CRC32_LEN))
		return DEMUX_CSCT_CRC_ERROR;

	pCSCT->table_id			= STEP8(pData);
	pCSCT->segment_len		= STEP16(pData);
	pCSCT->segment_sn		= STEP8(pData);
	pCSCT->segment_count	= STEP8(pData);

	BYTE nTmp = STEP8(pData);
	pCSCT->service_update_sn = (nTmp>>4) & 0x0f;

	pCSCT->service_count	= STEP16(pData);

	for(WORD n=0; n<pCSCT->service_count; n++)
	{
		pCSCT->service_info[n].service_id	= STEP16(pData);
		pCSCT->service_info[n].freq			= STEP8(pData);
	}


	return DEMUX_OK;
}

int ParseEBDT(BYTE* pData, int nLen, ESgBasicDescription* pEsg)
{
	if(false == CheckCRC32(pData, nLen-EBDT_CRC32_LEN))
		return DEMUX_EBDT_CRC_ERROR;

	pEsg->table_id	= STEP8(pData);

	WORD wTemp			= STEP16(pData);
	pEsg->esg_update_sn	= wTemp >> 12;
	pEsg->segment_len	= wTemp & 0xfff;

	BYTE nTemp			= STEP8(pData);
	pEsg->segment_sn	= nTemp >> 4;
	pEsg->segment_count	= nTemp & 0xf;

	wTemp				= STEP16(pData);
	pEsg->network_level	= wTemp>>12;
	pEsg->network_id	= wTemp & 0xfff;

	nTemp					= STEP8(pData);
	pEsg->local_time_offset	= nTemp & 0x3f;

	nTemp					= STEP8(pData);
	pEsg->char_type			= nTemp >> 4;
	pEsg->esg_service_count = nTemp & 0xf;

	for(BYTE i=0; i<pEsg->esg_service_count; i++)
	{
		nTemp = STEP8(pData);
		pEsg->esg_service_desc[i].esg_index_id		= nTemp & 0xf;
		pEsg->esg_service_desc[i].esg_service_id	= STEP16(pData);
	}

	nTemp					= STEP8(pData);
	pEsg->data_type_count	= nTemp & 0xf;	//

	for(BYTE i=0; i<pEsg->data_type_count; i++)
	{
		nTemp									= STEP8(pData);
		pEsg->data_type_desc[i].data_type_id	= (nTemp >> 4) & 0x0F;
		pEsg->data_type_desc[i].fragment_count		= STEP8(pData);

		for(BYTE j=0; j<pEsg->data_type_desc[i].fragment_count; j++)
		{
			pEsg->data_type_desc[i].data_fragment[j].fragment_id			= STEP8(pData);
			nTemp															= STEP8(pData);
			pEsg->data_type_desc[i].data_fragment[j].fragment_version		= nTemp >> 4;
			pEsg->data_type_desc[i].data_fragment[j].esg_service_index_id	= nTemp & 0xf;
		}
	}

	return DEMUX_OK;
}

int ParseEADesc(BYTE* pData, int nLen, EncryptionAuthorizationDescTable* pDesc)
{
	if(false == CheckCRC32(pData, nLen-EA_DESC_CRC32_LEN))
		return DEMUX_EA_DESC_CRC_ERROR;

	pDesc->table_data_ptr = pData;

	pDesc->table_id		= STEP8(pData);
	pDesc->segment_len	= STEP16(pData);

	DWORD dwTmp			= STEP32(pData);
	pDesc->ea_update_sn	= (dwTmp>>8) % 0x0f;
	pDesc->segment_sn	= dwTmp & 0xff;

	pDesc->segment_count= STEP8(pData);

	for(BYTE n=0; n<pDesc->segment_count; n++)
	{
		pDesc->ea_data_desc[n].ca_id				= STEP16(pData);
		pDesc->ea_data_desc[n].service_id			= STEP16(pData);
		pDesc->ea_data_desc[n].emm_data_type		= STEP8(pData);
		pDesc->ea_data_desc[n].ecm_data_type		= STEP8(pData);
		pDesc->ea_data_desc[n].ecm_transfer_type	= (GET8(pData)>>6) & 0x03;

		// reserved : 32 bits
		pData += 4;
	}

	pDesc->table_data_len = pDesc->segment_len + EA_DESC_CRC32_LEN;

	return DEMUX_OK;
}

int ParseEB(BYTE* pData, int nLen, EmegencyBroadcast* pEB)
{
	if(false == CheckCRC32(pData, nLen-EMEGENCY_BROAD_CRC32_LEN))
		return DEMUX_EMEGENCY_BROADCAST_CRC_ERROR;

	pEB->table_id		= STEP8(pData);

	BYTE nTmp			= STEP8(pData);
	pEB->msg_count		= (nTmp>>4) & 0xf;
	pEB->reserved		= (nTmp>>2) & 0x3;
	pEB->broadcast_sn	= nTmp & 0x3;

	pEB->broadcast_data_len	= STEP16(pData);
	pEB->broadcast_data		= pData;

	nTmp = STEP8(pData);
	pEB->eb_data_segment.version		= (nTmp>>4) & 0xF;
	pEB->eb_data_segment.mini_version	= (nTmp) & 0xF0;
	
	WORD wTmp = STEP16(pData);
	pEB->eb_data_segment.net_level	= (wTmp>>12) & 0xf;
	pEB->eb_data_segment.net_id		= wTmp & 0xFFF;

	pEB->eb_data_segment.msg_id		= STEP16(pData);

	wTmp = STEP16(pData);
	pEB->eb_data_segment.curr_segment_sn	= (wTmp>>8) & 0xff;
	pEB->eb_data_segment.last_segment_sn	= (wTmp) & 0xff;

	wTmp = STEP16(pData);
	pEB->eb_data_segment.reseved		= (wTmp>>13) & 0x7;
	pEB->eb_data_segment.eb_data_len	= (wTmp) & 0x1fff;

	pEB->eb_data_segment.eb_data = pData;

	nTmp = STEP8(pData);
	pEB->eb_data_segment.trigger_flag	= (nTmp>>7) & 0x1;

	if(pEB->eb_data_segment.trigger_flag == 0)
	{
		pEB->eb_data_segment.eb_msg0.trigger_type	= nTmp & 0x7f;

		nTmp = STEP8(pData);
		pEB->eb_data_segment.eb_msg0.eb_level	= (nTmp>>5) & 0x7;
		pEB->eb_data_segment.eb_msg0.eb_charset	= (nTmp>>2) & 0x7;
		pEB->eb_data_segment.eb_msg0.reserved1	= nTmp & 0x3;

		//LONG64 lTmp = STEP64(pData);
		STEP32(pData);
		wTmp = STEP32(pData);
		pEB->eb_data_segment.eb_msg0.text_data_count = wTmp & 0xf;

		for(BYTE n=0; n<pEB->eb_data_segment.eb_msg0.text_data_count; n++)
		{
			pEB->eb_data_segment.eb_msg0.text_data[n].language_type		= STEP24(pData);
			
			wTmp = STEP16(pData);
			pEB->eb_data_segment.eb_msg0.text_data[n].organization_flag	= (wTmp>>15) & 0x1;
			pEB->eb_data_segment.eb_msg0.text_data[n].ref_service_flag	= (wTmp>>14) & 0x1;
			pEB->eb_data_segment.eb_msg0.text_data[n].reserved1			= (wTmp>>12) & 0x3;
			pEB->eb_data_segment.eb_msg0.text_data[n].text_data_len		= wTmp & 0xfff;
			pEB->eb_data_segment.eb_msg0.text_data[n].text_data_ptr		= pData;

			// skip text data len
			pData += pEB->eb_data_segment.eb_msg0.text_data[n].text_data_len;

			if (pEB->eb_data_segment.eb_msg0.text_data[n].organization_flag == 1)
			{
				pEB->eb_data_segment.eb_msg0.text_data[n].organization_name_len	= STEP8(pData);
				pEB->eb_data_segment.eb_msg0.text_data[n].organization_name_ptr	= pData;

				// skip name len
				pData += pEB->eb_data_segment.eb_msg0.text_data[n].organization_name_len;
			}
			else
			{
				pEB->eb_data_segment.eb_msg0.text_data[n].organization_name_len	= 0;
				pEB->eb_data_segment.eb_msg0.text_data[n].organization_name_ptr	= NULL;
			}

			if (pEB->eb_data_segment.eb_msg0.text_data[n].ref_service_flag == 1)
			{
				pEB->eb_data_segment.eb_msg0.text_data[n].ref_service_id = STEP16(pData);
			}
			else
				pEB->eb_data_segment.eb_msg0.text_data[n].ref_service_id = 0;

			BYTE nTmp = STEP8(pData);
			pEB->eb_data_segment.eb_msg0.text_data[n].reserved2		= (nTmp>>4) & 0xf;
			pEB->eb_data_segment.eb_msg0.text_data[n].aux_data_idx	= nTmp & 0xf;

		}

		// aux data
		nTmp = STEP8(pData);
		pEB->eb_data_segment.eb_msg0.reserved2		= (nTmp>>4) & 0xf;
		pEB->eb_data_segment.eb_msg0.aux_data_count	= nTmp & 0xf;

		for (BYTE n=0; n<pEB->eb_data_segment.eb_msg0.aux_data_count; n++)
		{
			pEB->eb_data_segment.eb_msg0.aux_data[n].aux_data_type	= STEP8(pData);
			pEB->eb_data_segment.eb_msg0.aux_data[n].aux_data_len	= STEP16(pData);
			pEB->eb_data_segment.eb_msg0.aux_data[n].aux_data_ptr	= pData;
			pData += pEB->eb_data_segment.eb_msg0.aux_data[n].aux_data_len;
		}
	}
	else
	{
		pEB->eb_data_segment.eb_msg1.trigger_msg_level	= nTmp & 0x7f;

		nTmp = STEP8(pData);
		pEB->eb_data_segment.eb_msg1.trigger_msg_level	= (nTmp>>4) & 0xf;
		pEB->eb_data_segment.eb_msg1.reserved1			= nTmp & 0xf;

		wTmp = STEP16(pData);
		pEB->eb_data_segment.eb_msg1.trigger_net_level	= (wTmp>>12) & 0xf;
		pEB->eb_data_segment.eb_msg1.trigger_network_sn	= wTmp & 0xfff;

		pEB->eb_data_segment.eb_msg1.trigger_freq_point	= STEP8(pData);

		pEB->eb_data_segment.eb_msg1.trigger_center_freq = STEP32(pData);

		nTmp = STEP8(pData);
		pEB->eb_data_segment.eb_msg1.trigger_band_width = nTmp & 0xf0;
		pEB->eb_data_segment.eb_msg1.reserved2			= nTmp & 0x0f;
	}

	return DEMUX_OK;
}

int ParseMFHeader(BYTE* pData, MultiplexFrameHeader* pHeader)
{
	if(memcmp(pData, MF_START_CODE, MF_START_CODE_LEN))
		return DEMUX_MF_START_CODE_ERROR;

	pHeader->header_len	= GET8(pData + MF_START_CODE_LEN);

	if(false == CheckCRC32(pData, pHeader->header_len))
		return DEMUX_MF_HEADER_CRC_ERROR;

	// skip start code and header length
	pData += (MF_START_CODE_LEN + 1);

	WORD wTemp				= STEP16(pData);
	pHeader->version		= wTemp >> 11;			// 5 bits
	pHeader->min_version    = (wTemp >> 6) & 0x1f;	// 5
	pHeader->mf_id          = wTemp & 0x3f;			// 6

	BYTE  nTemp							= STEP8(pData);
	pHeader->urgency_broadcast_flag		= nTemp >> 6;				// 2 bits
	pHeader->next_frame_para_flag		= (nTemp >> 5) & 1;			// 1
	pHeader->control_table_update_flag	= nTemp & 3;				// 2
	//pHeader->encrypt_flag				= (nTemp>>3) & 3;
	//pHeader->ecm_flag					= (nTemp>>2) & 1;

	DWORD u32Tmp					= STEP32(pData);
	pHeader->nit_update_sn			= BYTE(u32Tmp >> 28);			// 4 bits
	pHeader->cmct_update_sn	        = BYTE((u32Tmp >> 24) & 0xf);	// 4
	pHeader->csct_update_sn	        = BYTE((u32Tmp >> 20) & 0xf);	// 4
	pHeader->smct_update_sn	        = BYTE((u32Tmp >> 16) & 0xf);	// 4
	pHeader->ssct_update_sn	        = BYTE((u32Tmp >> 12) & 0xf);	// 4
	pHeader->esg_update_sn	        = BYTE((u32Tmp >>  8) & 0xf);	// 4
	pHeader->multi_sub_frame_num	= BYTE(u32Tmp & 0xf);			// 4


	for(BYTE i=0; i<pHeader->multi_sub_frame_num; i++)
	{
		pHeader->multi_sub_frame_len[i]  = STEP24(pData);
	}

	pHeader->next_frame_parameter.multi_frame_header_len		= pHeader->next_frame_para_flag ?STEP8(pData):0;
	pHeader->next_frame_parameter.first_sub_frame_len			= pHeader->next_frame_para_flag ?STEP24(pData):0;
	pHeader->next_frame_parameter.first_sub_frame_header_len	= pHeader->next_frame_para_flag ?STEP8(pData):0;

	return DEMUX_OK;
}


int ParseMSFHeader(BYTE* pData, MultiSubFrameHeader* pHeader)
{
	pHeader->header_len = GET8(pData);

	if(false == CheckCRC32(pData, pHeader->header_len))
		return DEMUX_MSF_HEADER_CRC_ERROR;

	//skip header len. 8 bits
	pData++;

	//
	BYTE nTemp						= STEP8(pData);
	pHeader->start_time_flag		= nTemp >> 7;
	pHeader->video_seg_flag			= (nTemp >> 6) & 1;
	pHeader->audio_seg_flag			= (nTemp >> 5) & 1;
	pHeader->data_seg_flag			= (nTemp >> 4) & 1;
	pHeader->extend_area_flag		= (nTemp >> 3) & 1;
	pHeader->encrypt_flag			= (nTemp>>1) & 3;
	pHeader->packet_mode			= nTemp & 1;

	pHeader->start_time_stamp		= (pHeader->start_time_flag==1)?STEP32(pData):0;

	if(pHeader->video_seg_flag == 1)
	{
		DWORD u32Tmp				= STEP24(pData);
		pHeader->video_seg_len		= u32Tmp >> 3;
		pHeader->video_stream_num	= BYTE(u32Tmp & 0x7);
	}
	else
	{
		pHeader->video_seg_len		= 0;
		pHeader->video_stream_num	= 0;
	}

	if(pHeader->audio_seg_flag == 1)
	{
		DWORD u32Tmp				= STEP24(pData);
		pHeader->audio_seg_len		= u32Tmp>>3;
		pHeader->audio_stream_num	= BYTE(u32Tmp & 0x7);
	}
	else
	{
		pHeader->audio_seg_len		= 0;
		pHeader->audio_stream_num	= 0;
	}

	if(pHeader->data_seg_flag == 1)
	{
		DWORD u32Tmp				= STEP24(pData);
		pHeader->data_seg_len		= u32Tmp>>3;
		pHeader->ecm_flag			= (BYTE)((u32Tmp>>2)&1);//????
	}
	else
	{
		pHeader->data_seg_len		= 0;
		pHeader->ecm_flag			= 0;
	}

	if(pHeader->extend_area_flag == 1)
	{
		for(int i=0; i<pHeader->video_stream_num; i++)
		{
			nTemp = STEP8(pData);
			pHeader->video_stream_param[i].algorithm_type		= nTemp >> 5;
			pHeader->video_stream_param[i].bit_rate_flag		= (nTemp >> 4) & 1;
			pHeader->video_stream_param[i].video_display_flag	= (nTemp >> 3) & 1;
			pHeader->video_stream_param[i].resolution_flag		= (nTemp >> 2) & 1;
			pHeader->video_stream_param[i].frame_freq_flag		= (nTemp >> 1) & 1;
			pHeader->video_stream_param[i].reserved				= nTemp & 1;

			pHeader->video_stream_param[i].vsp_extent.bitrate	= (pHeader->video_stream_param[i].bit_rate_flag==1)?STEP16(pData):0;

			if(pHeader->video_stream_param[i].video_display_flag == 1)
			{                    
				WORD wTemp = STEP16(pData);
				pHeader->video_stream_param[i].vsp_extent.x					= wTemp >> 10;
				pHeader->video_stream_param[i].vsp_extent.y					= (wTemp >> 4) & 0x3f;
				pHeader->video_stream_param[i].vsp_extent.display_priority  = (wTemp >> 1) & 7;
			}
			else
			{
				pHeader->video_stream_param[i].vsp_extent.x					= 0;
				pHeader->video_stream_param[i].vsp_extent.y					= 0;
				pHeader->video_stream_param[i].vsp_extent.display_priority  = 0;
			}

			if(pHeader->video_stream_param[i].resolution_flag == 1)
			{                    
				DWORD u32Tmp = STEP24(pData);
				pHeader->video_stream_param[i].vsp_extent.x_resolution = WORD((u32Tmp>>10) & 0x3ff);
				pHeader->video_stream_param[i].vsp_extent.y_resolution = WORD(u32Tmp & 0x3ff);
			}
			else
			{
				pHeader->video_stream_param[i].vsp_extent.x_resolution = 0;
				pHeader->video_stream_param[i].vsp_extent.y_resolution = 0;
			}

			pHeader->video_stream_param[i].vsp_extent.frame_freq = (pHeader->video_stream_param[i].frame_freq_flag==1)?STEP8(pData):0;
		}

		for(int i=0; i<pHeader->audio_stream_num; i++)
		{
			nTemp = STEP8(pData);
			pHeader->audio_stream_param[i].algorithm_type	= nTemp >> 4;
			pHeader->audio_stream_param[i].bit_rate_flag	= (nTemp >> 3) & 1;
			pHeader->audio_stream_param[i].sample_rate_flag	= (nTemp >> 2) & 1;
			pHeader->audio_stream_param[i].stream_desc_flag	= (nTemp >> 1) & 1;

			if(pHeader->audio_stream_param[i].bit_rate_flag == 1)
			{
				WORD wTmp = STEP16(pData);
				pHeader->audio_stream_param[i].asp_extent.bitrate = wTmp >> 2;
			}
			else
				pHeader->audio_stream_param[i].asp_extent.bitrate = 0;

			if(pHeader->audio_stream_param[i].sample_rate_flag == 1)
			{                                        
				nTemp = STEP8(pData);
				pHeader->audio_stream_param[i].asp_extent.sample_rate = nTemp & 0xF;
			}
			else
				pHeader->audio_stream_param[i].asp_extent.sample_rate = 0;

			pHeader->audio_stream_param[i].asp_extent.stream_desc = (pHeader->audio_stream_param[i].stream_desc_flag==1)?STEP24(pData):0;
		}
	}

	return DEMUX_OK;
}

int ParseVideoSegmentHeader(LPBYTE pData, VideoSegmentHeader* pHeader)
{
	pHeader->video_unit_count = 0;
	pHeader->header_len = GET16(pData) >> 4;

	if(false == CheckCRC32(pData, pHeader->header_len))
		return DEMUX_VIDEO_SEG_HEADER_CRC_ERROR;

	LPBYTE segment_end = pData + pHeader->header_len;

	//skip header len. 16 bits
	pData += 2;

	// MAX_VIDEO_UNIT_PARAM_LEN is 40 bits, if has relative time.
	while((pData+MIN_VIDEO_UNIT_PARAM_LEN) <= segment_end)
	{
		pHeader->video_unit_param[pHeader->video_unit_count].unit_len  = STEP16(pData);

		BYTE  u8TP_SN_EF_RPTF = STEP8(pData);
		pHeader->video_unit_param[pHeader->video_unit_count].frame_type		= u8TP_SN_EF_RPTF >> 5;			// 3 I/B/P
		pHeader->video_unit_param[pHeader->video_unit_count].stream_sn		= (u8TP_SN_EF_RPTF >> 2) & 7;	// 3
		pHeader->video_unit_param[pHeader->video_unit_count].frame_end_flag = (u8TP_SN_EF_RPTF >> 1) & 1;	// 1
		pHeader->video_unit_param[pHeader->video_unit_count].play_time_flag = u8TP_SN_EF_RPTF & 1;			// 1

		if(pHeader->video_unit_param[pHeader->video_unit_count].play_time_flag == 1)
		{
			pHeader->video_unit_param[pHeader->video_unit_count].relative_play_time = STEP16(pData);
		}
		else
		{
			pHeader->video_unit_param[pHeader->video_unit_count].relative_play_time = 0;// in 1/22500 ms
		}

		pHeader->video_unit_count++;
	}

	return DEMUX_OK;
}

FRAMEPOS CalFramePos(BYTE nStartFlag, BYTE nEndFlag)
{
	if(nStartFlag==0 && nEndFlag==1)
		return FRAME_END;
	else if(nStartFlag==1 && nEndFlag==1)
		return FRAME_WHOLE;
	else if(nStartFlag==0 && nEndFlag==0)
		return FRAME_MID;
	else if(nStartFlag==1 && nEndFlag==0)
		return FRAME_BEGIN;

	return FRAME_MID;
}

int ParseFragmentMode2Header(BYTE* pData, Mode2FragmentHeader* pHeader)
{
	if(pData[0] != MODE2_FRAGMENT_START_CODE)
		return DEMUX_FRAGMENT_STRAT_CODE_ERROR;

	pHeader->start_code = STEP8(pData);

	WORD temp				= STEP16(pData);
	pHeader->start_flag	    = (temp>>15) & 1;
	pHeader->end_flag		= (temp>>14) & 1;
	pHeader->fragment_type	= (temp>>12) & 3;
	pHeader->payload_len	= temp&0xfff;

	if(pHeader->fragment_type == DATA_UNIT)
	{
		pHeader->header_len = MIN_MODE2_FRAGMENT_HEADER_LEN + 1 - MODE2_FRAGMENT_CRC8_LEN; // not include crc8 len
		pHeader->data_unit_type = STEP8(pData);
	}
	else
	{	
		pHeader->header_len = MIN_MODE2_FRAGMENT_HEADER_LEN - MODE2_FRAGMENT_CRC8_LEN;	// not include crc8 len
		pHeader->data_unit_type = 254;	// reserved type, maybe has issue here
	}

	BYTE crc8 = STEP8(pData);

	if(crc8 != CalcCRC8(pData-(pHeader->header_len+MODE2_FRAGMENT_CRC8_LEN), pHeader->header_len))
		return DEMUX_FRAGMENT_HEADER_CRC_ERROR;

	return DEMUX_OK;
}

//DWORD ts, BYTE * buf, int len,NAL_FRAGMENT_POS_IND StartEndInd
int _ParseMode2VideoFragment(BYTE* pData, Mode2FragmentHeader* pHeader, FRAMEPOS nPos, VideoUnitBuf* pFrame)
{
	WORD len = pHeader->payload_len;
	BYTE nal_type = pData[0] & 0x1f;

	/*
	Neither an aggregation packet nor a fragmentation unit can be used within a
	single NAL unit packet.  A NAL unit stream composed by decapsulating
	single NAL unit packets in RTP sequence number order MUST conform to
	the NAL unit decoding order.  The structure of the single NAL unit
	packet is shown in Figure.

	Informative note: The first byte of a NAL unit co-serves as the
	RTP payload header.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|F|NRI|  type   |                                               |
	+-+-+-+-+-+-+-+-+                                               |
	|                                                               |
	|               Bytes 2..n of a Single NAL unit                 |
	|                                                               |
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	*/

	if(nal_type == 12)
	{
		pFrame->frame[pFrame->buf_count].pData = NULL;
		pFrame->frame[pFrame->buf_count].dwLen = 0;
		pFrame->frame[pFrame->buf_count].pos = FRAME_END;
		pFrame->buf_count++;

		return DEMUX_OK;
	}

	if (nal_type >= 1 && nal_type <= SINGLE_NAL)// 23
	{
		pData[-3] = 0;
		pData[-2] = 0;
		pData[-1] = 1;

		pFrame->frame[pFrame->buf_count].pData = pData-3;
		pFrame->frame[pFrame->buf_count].dwLen = pHeader->payload_len+3;
		pFrame->frame[pFrame->buf_count].pos = nPos;
		pFrame->buf_count++;

		return DEMUX_OK;
	}

	/*
	STAP		- Single-time Aggregation Packet
	STAP-A		- Single-time Aggregation Packet without DON(Decoder Order Number)
	STAP-B		- Single-time Aggregation Packet including DON(Decoder Order Number)
	
	Single-time aggregation packet (STAP): aggregates NAL units with
	identical NALU-time.  Two types of STAPs are defined, one without
	DON (STAP-A) and another including DON (STAP-B).

	Single-time aggregation packet (STAP) SHOULD be used whenever NAL
	units are aggregated that all share the same NALU-time.  The payload
	of an STAP-A does not include DON and consists of at least one
	single-time aggregation unit, as presented in Figure 4.  The payload
	of an STAP-B consists of a 16-bit unsigned decoding order number
	(DON) (in network byte order) followed by at least one single-time
	aggregation unit, as presented in Figure 5.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	:                                               |
	+-+-+-+-+-+-+-+-+                                               |
	|                                                               |
	|                single-time aggregation units                  |
	|                                                               |
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 4.  Payload format for STAP-A

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	:  decoding order number (DON)  |               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               |
	|                                                               |
	|                single-time aggregation units                  |
	|                                                               |
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 5.  Payload format for STAP-B

	//
	Figure 7 presents an example of an RTP packet that contains an STAP-
	A.  The STAP contains two single-time aggregation units, labeled as 1
	and 2 in the figure.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                          RTP Header                           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|STAP-A NAL HDR |         NALU 1 Size           | NALU 1 HDR    |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                         NALU 1 Data                           |
	:                                                               :
	+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|               | NALU 2 Size                   | NALU 2 HDR    |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                         NALU 2 Data                           |
	:                                                               :
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 7.  An example of an RTP packet including an STAP-A and two
	single-time aggregation units

	
	Figure 8 presents an example of an RTP packet that contains an STAP-
	B.  The STAP contains two single-time aggregation units, labeled as 1
	and 2 in the figure.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                          RTP Header                           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|STAP-B NAL HDR | DON                           | NALU 1 Size   |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	| NALU 1 Size   | NALU 1 HDR    | NALU 1 Data                   |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
	:                                                               :
	+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|               | NALU 2 Size                   | NALU 2 HDR    |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                       NALU 2 Data                             |
	:                                                               :
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 8.  An example of an RTP packet including an STAP-B and two
	single-time aggregation units

*/

	if(nal_type==STAP_A || nal_type==STAP_B)                   // STAP (one packet, multiple nals)
	{
		// skip type
		pData++;
		len--;

		if (nal_type == STAP_B)
		{
			// skip DON, 16 bits
			pData	+= 2;
			len		-= 2;
			//OutputDebugString(_T("[DMX]STAP_B \n"));
		}
		else
		{
			//OutputDebugString(_T("[DMX]STAP_A \n"));
		}
			
		BYTE *src= pData;

		BYTE	start	= FRAME_BEGIN;
		BYTE	end		= FRAME_END;
		BYTE	lastnal = FRAME_MID;

		BYTE count = 0;
		FRAMEPOS pos = nPos;

		while(src < pData + len - 2)
		{
			WORD nal_size = GET16(src);

			if(src + nal_size + 2 > pData + len)
			{
				break;
			}

			src[-1] = 0;
			src[0] = 0;
			src[1] = 1;

			count++;
			lastnal =( ((src+nal_size + 2)>=(pData + len - 2))?FRAME_END:FRAME_MID);

			if(nPos == FRAME_WHOLE)
			{
				if(lastnal == FRAME_END)
					pos = count==1?FRAME_WHOLE:FRAME_END;
				else
					pos = count==1?FRAME_BEGIN:FRAME_MID;
			}
			else if(nPos == FRAME_BEGIN)
			{
				if(lastnal == FRAME_END)
					pos = count==1?FRAME_BEGIN:FRAME_MID;
				else if(lastnal == FRAME_MID)
					pos = count==1?FRAME_BEGIN:FRAME_MID;
			}
			else if(nPos == FRAME_END)
			{
				if(lastnal == FRAME_END)
					pos = FRAME_END;
				else if(lastnal == FRAME_MID)
					pos = FRAME_MID;
			}

			pFrame->frame[pFrame->buf_count].pData = src-1;
			pFrame->frame[pFrame->buf_count].dwLen = nal_size+3;
			pFrame->frame[pFrame->buf_count].pos = pos;
			pFrame->buf_count++;

			start = FRAME_MID;
			src += nal_size + 2;
		}
		return DEMUX_OK;
	}

	/*
	Multi-time aggregation packet (MTAP): aggregates NAL units with
	potentially differing NALU-time.  Two different MTAPs are defined,
	differing in the length of the NAL unit timestamp offset.
	*/


	/*
	Figure 12 presents an example of an RTP packet that contains a
	multi-time aggregation packet of type MTAP16 that contains two
	multi-time aggregation units, labeled as 1 and 2 in the figure.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                          RTP Header                           |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|MTAP16 NAL HDR |  decoding order number base   | NALU 1 Size   |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|  NALU 1 Size  |  NALU 1 DOND  |       NALU 1 TS offset        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|  NALU 1 HDR   |  NALU 1 DATA                                  |
	+-+-+-+-+-+-+-+-+                                               +
	:                                                               :
	+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|               | NALU 2 SIZE                   |  NALU 2 DOND  |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|       NALU 2 TS offset        |  NALU 2 HDR   |  NALU 2 DATA  |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+               |
	:                                                               :
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 12.  An RTP packet including a multi-time aggregation
	packet of type MTAP16 and two multi-time aggregation
	units
*/

	if(nal_type == MTAP_16|| nal_type == MTAP_24)
	{
		//OutputDebugString(_T("nal type = 26,27\n"));
		return DEMUX_UNSUPORT_H264_NAL_TYPE;
	}

	/*
	Figure 14 presents the RTP payload format for FU-As.  An FU-A
	consists of a fragmentation unit indicator of one octet, a
	fragmentation unit header of one octet, and a fragmentation unit
	payload.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	| FU indicator  |   FU header   |                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               |
	|                                                               |
	|                         FU payload                            |
	|                                                               |
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 14.  RTP payload format for FU-A


	Figure 15 presents the RTP payload format for FU-Bs.  An FU-B
	consists of a fragmentation unit indicator of one octet, a
	fragmentation unit header of one octet, a decoding order number (DON)
	(in network byte order), and a fragmentation unit payload.  In other
	words, the structure of FU-B is the same as the structure of FU-A,
	except for the additional DON field.

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	| FU indicator  |   FU header   |               DON             |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-|
	|                                                               |
	|                         FU payload                            |
	|                                                               |
	|                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               :...OPTIONAL RTP padding        |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	Figure 15.  RTP payload format for FU-B


	/////////////////////

	The FU indicator octet has the following format:

	+---------------+
	|0|1|2|3|4|5|6|7|
	+-+-+-+-+-+-+-+-+
	|F|NRI|  Type   |
	+---------------+


	The FU header has the following format:

	+---------------+
	|0|1|2|3|4|5|6|7|
	+-+-+-+-+-+-+-+-+
	|S|E|R|  Type   |
	+---------------+

	S: 1 bit
	When set to one, the Start bit indicates the start of a fragmented
	NAL unit.  When the following FU payload is not the start of a
	fragmented NAL unit payload, the Start bit is set to zero.

	E: 1 bit
	When set to one, the End bit indicates the end of a fragmented NAL
	unit, i.e., the last byte of the payload is also the last byte of
	the fragmented NAL unit.  When the following FU payload is not the
	last fragment of a fragmented NAL unit, the End bit is set to
	zero.

	R: 1 bit
	The Reserved bit MUST be equal to 0 and MUST be ignored by the
	receiver.

	Type: 5 bits
	The NAL unit payload type as defined in table 7-1 of [1].
*/
	if(nal_type==FU_A || nal_type==FU_B)                   // FU-A (fragmented nal)
	{
		BYTE fu_indicator = pData[0];

		// skip type
		pData++;
		len--;

		BYTE fu_header	= *pData;   // read the fu_header.
		BYTE start_bit	= fu_header >> 7;
		BYTE end_bit	= (fu_header >> 6) & 1;
		BYTE reconstructed_nal;

		reconstructed_nal = fu_indicator & 0xe0;	// 11100000 the original nal forbidden bit and NRI are stored in this packet's nal;
		reconstructed_nal |= fu_header&0x1f;		// 00011111

		// skip the fu_header...
		pData++;
		len--;

		if (nal_type == FU_B)
		{
			//skip DON, 16 bits
			pData	+= 2;
			len		-= 2;
			//OutputDebugString(_T("[DMX]FU_B \n"));
		}

		if(start_bit)
		{
			pData[-4] = 0;
			pData[-3] = 0;
			pData[-2] = 1;
			pData[-1] = reconstructed_nal;

			pFrame->frame[pFrame->buf_count].pData = pData - 4;
			pFrame->frame[pFrame->buf_count].dwLen = len   + 4;
			pFrame->frame[pFrame->buf_count].pos = nPos;
			pFrame->buf_count++;
		}
		else
		{
			pFrame->frame[pFrame->buf_count].pData = pData;
			pFrame->frame[pFrame->buf_count].dwLen = len;
			pFrame->frame[pFrame->buf_count].pos = nPos;
			pFrame->buf_count++;
		}

		return DEMUX_OK;
	}

	//OutputDebugString(_T("nal type = DEMUX_UNSUPORT_H264_NAL_TYPE\n"));
	return DEMUX_UNSUPORT_H264_NAL_TYPE;
}

int ParseMode2AudioFragment(BYTE* pData, Mode2FragmentHeader* pHeader, AudioUnitBuf* pUnit)
{
	LPBYTE lpEnd = pData + pHeader->payload_len;

	while(pData < lpEnd)
	{
		WORD length = pData[0];

		if(pData + length > lpEnd)
			break;

		switch(length)
		{
		case 0xff:
			{
				length = 0x100 + pData[1] - 1;
				pData += 2;
			}
			break;
		case 0xfe:
			{
				length = 0x200 + pData[1] - 1;
				pData += 2;
			}
			break;
		default:
			pData++;
			break;
		}

		pUnit->frame[pUnit->buf_count].pData	= pData;
		pUnit->frame[pUnit->buf_count].dwLen	= length;
		pUnit->buf_count++;

		pData += length;
	}

	return DEMUX_OK;
}


int _ParseFragmentMode2(BYTE* pData, Mode2FragmentHeader* pHeader, void* pOut)
{
	int nRet = DEMUX_OK;

	switch(pHeader->fragment_type)
	{
	case VIDEO_UNIT:
		{
			nRet = ::_ParseMode2VideoFragment(pData, pHeader, CalFramePos(pHeader->start_flag, pHeader->end_flag), (VideoUnitBuf*)pOut);
		}
		break;
	case AUDIO_UNIT:
		{
			nRet = ::ParseMode2AudioFragment(pData, pHeader, (AudioUnitBuf*)pOut);
		}
		break;
	case DATA_UNIT:
		{

		}
		break;
	}

	return nRet;
}

int ParseDraFragmentMode2(BYTE* pData, Mode2FragmentHeader* pHeader, void* pOut)
{
	AudioUnitBuf* pBuf = (AudioUnitBuf*)pOut;

	pData += 2; // skip rtp header '00 01'?
	pBuf->frame[pBuf->buf_count].pData	= pData; 

	BYTE flag = (pData[2]>>7) & 0x1;
	WORD length = ((pData[2]<<8) | pData[3]);

	if(flag == 1)
	{
		// 13 bits
		length = ((length & 0x7FFF) >> 2) << 2;
	}
	else
	{
		// 10 bits
		length = ((length & 0x7FFF) >> 5) << 2;
	}

#if	0
	TCHAR tmp[128];
	_stprintf(tmp, _T("DRA len=%d\n"), length);
	//OutputDebugString(tmp);
#endif
	if(length > (pHeader->payload_len - 2))
		length = pHeader->payload_len - 2;

	pBuf->frame[pBuf->buf_count].dwLen	= length;

	pBuf->buf_count++;

	return DEMUX_OK;
}


int ParseAudioSegmentHeader(LPBYTE pData, AudioSegmentHeader* pHeader)
{
	BYTE* pTmp = pData;

	pHeader->header_len = 0;
	pHeader->unit_count = STEP8(pData);

	for(BYTE n=0; n<pHeader->unit_count; n++)
	{
		pHeader->audio_unit_param[n].unit_len = STEP16(pData);

		BYTE nTmp = STEP8(pData);
		pHeader->audio_unit_param[n].stream_sn = nTmp>>5;

		pHeader->audio_unit_param[n].relative_play_time = STEP16(pData);
	}

	pHeader->header_len = 1 + 5*pHeader->unit_count;	// not include crc32

	if(false == CheckCRC32(pData-pHeader->header_len, pHeader->header_len))
		return DEMUX_AUDIO_SEG_HEADER_CRC_ERROR;

	return DEMUX_OK;
}

int ParseDataSegmentHeader(LPBYTE pData, DataSegmentHeader* pHeader)
{
	pHeader->unit_count = GET8(pData);
	pHeader->header_len	= 1 + DATA_UNIT_PARAM_LEN*pHeader->unit_count;

	if(!CheckCRC32(pData, pHeader->header_len))
		return DEMUX_DATA_SEG_HEADER_CRC_ERROR;

	// skip unit count, 8bits
	pData++;

	for(BYTE n=0; n<pHeader->unit_count; n++)
	{
		pHeader->data_unit_param[n].unit_type	= STEP8(pData);
		pHeader->data_unit_param[n].unit_len	= STEP16(pData); 
	}

	return DEMUX_OK;
}


int ParseProgramGuideInfo(BYTE* pData, ProgramGuideInfo* pInfo)
{
	pInfo->record_count = GET8(pData) >> 6;

	BYTE* pStart = pData;

	//skip record count, 8bits
	pData++;
	pInfo->info_len = 1;

	for(BYTE n=0; n<pInfo->record_count; n++)
	{
		pInfo->record_info[n].date			= STEP16(pData);
		
		*(pData-2) = 0;
		*(pData-1) = 0;


		// 20090729
		//pInfo->record_info[n].time			= STEP24(pData);
		BYTE nTemp	 = STEP8(pData);
		pInfo->record_info[n].hour = (nTemp>>4) * 10 + (nTemp & 0xf);
		nTemp		= STEP8(pData);
		pInfo->record_info[n].minute = (nTemp>>4) * 10 + (nTemp & 0xf);
		nTemp		= STEP8(pData);
		pInfo->record_info[n].second = (nTemp>>4) * 10 + (nTemp & 0xf);

		pInfo->record_info[n].duration		= STEP16(pData);
		pInfo->record_info[n].language_type	= STEP24(pData);
		pInfo->record_info[n].theme_len		= STEP8(pData);
		pInfo->record_info[n].theme_ptr		= pData;
		pData += pInfo->record_info[n].theme_len;

		pInfo->info_len += 11 + pInfo->record_info[n].theme_len;
	}

	// 20090729
// 	if(!CheckCRC32(pStart, pInfo->info_len))
// 		return DEMUX_PROGRAME_GUIDE_CRC_ERROR;

	return DEMUX_OK;
}

int ParseEsgDataSegmentHeader(BYTE* pData, EsgDataSegmentHeader* pHeader)
{
	pHeader->id	= STEP8(pData);

	BYTE nTmp		= STEP8(pData);
	pHeader->type	= nTmp >> 4;
	pHeader->encode	= nTmp & 0x07;

	pHeader->seg_sn			= STEP8(pData);
	pHeader->total			= STEP8(pData);
	pHeader->payload_len	= STEP16(pData);

	return DEMUX_OK;
}

int ParseEsgDataSegment(BYTE* pData, EsgDataSegment* pSeg)
{
	int nRet = ::ParseEsgDataSegmentHeader(pData, &pSeg->header);

	if(!CheckCRC32(pData, DATA_SEGMENT_HEADER_LEN+pSeg->header.payload_len))
		return DEMUX_ESG_SEG_HEADERCRC_ERROR;

	pSeg->payload = pData + DATA_SEGMENT_HEADER_LEN;

	return nRet;
}

/////////////////////////////////

#ifdef _OLD_ESG_PARSE

EsgDataContainer::EsgDataContainer()
{
	seg = NULL;
	seg_count = 0;
	curr_seg_count = 0;
	total_payload_len = 0;

	next_seg_sn = 0;
}

EsgDataContainer::~EsgDataContainer(void)
{
}

void EsgDataContainer::init(EsgDataSegment* pSeg)
{
	total_payload_len	= 0;
	curr_seg_count		= 0;
	seg_count			= pSeg->header.total;
	seg					= new EsgDataSegment * [pSeg->header.total];

	for (int n=0; n<pSeg->header.total; n++)
	{
		seg[n] = new EsgDataSegment;
		seg[n]->init();
	}

	next_seg_sn = 0;
}

void EsgDataContainer::insert(EsgDataSegment* pSeg)
{
	curr_seg_count++;
	total_payload_len += pSeg->header.payload_len;
	seg[pSeg->header.seg_sn]->payload = new BYTE[pSeg->header.payload_len];
	memcpy(seg[pSeg->header.seg_sn]->payload, pSeg->payload, pSeg->header.payload_len);
	memcpy(&seg[pSeg->header.seg_sn]->header, &pSeg->header, sizeof(EsgDataSegment::EsgDataSegmentHeader));

	next_seg_sn++;
}

void EsgDataContainer::reset()
{	
	if (seg)
	{
		for (BYTE n=0; n<seg_count; n++)
		{
			EsgDataSegment* pSeg = seg[n];
			if (pSeg->payload)
			{
				delete pSeg->payload;
				pSeg->payload = NULL;
				pSeg->header.payload_len = 0;
			}

			delete pSeg;
			pSeg = NULL;
		}

		delete []seg;
		seg				= NULL;
	}

	curr_seg_count		= 0;
	seg_count			= 0;
	total_payload_len	= 0;

	next_seg_sn			= 0;
}
#else
// 20090827

EsgDataContainer::EsgDataContainer()
{
	//seg = NULL;
	seg_count = 0;
	curr_seg_count = 0;
	total_payload_len = 0;

	next_seg_sn = 0;
}

EsgDataContainer::~EsgDataContainer(void)
{
}

void EsgDataContainer::init(EsgDataSegment* pSeg)
{
	total_payload_len	= 0;
	curr_seg_count		= 0;
	seg_count			= pSeg->header.total;
	//seg					= new EsgDataSegment * [pSeg->header.total];

	for (int n=0; n<pSeg->header.total; n++)
	{
		//seg[n] = new EsgDataSegment;
		seg[n].init();
	}

	next_seg_sn = 0;
}

void EsgDataContainer::insert(EsgDataSegment* pSeg)
{
	curr_seg_count++;
	total_payload_len += pSeg->header.payload_len;
	seg[pSeg->header.seg_sn].payload = new BYTE[pSeg->header.payload_len];
	memcpy(seg[pSeg->header.seg_sn].payload, pSeg->payload, pSeg->header.payload_len);
	memcpy(&seg[pSeg->header.seg_sn].header, &pSeg->header, sizeof(EsgDataSegmentHeader));

	next_seg_sn++;
}

void EsgDataContainer::reset()
{	
	//if (seg)
	{
		for (BYTE n=0; n<seg_count; n++)
		{
			EsgDataSegment* pSeg = &seg[n];
			if (pSeg->payload && pSeg->header.payload_len!=0)
			{
				delete pSeg->payload;
				pSeg->payload = NULL;
				pSeg->header.payload_len = 0;
				//OutputDebugString(_T("release seg payload ok\n"));
			}

			//delete pSeg;
			//pSeg = NULL;
		}

		//delete []seg;
		//seg				= NULL;
	}

	curr_seg_count		= 0;
	seg_count			= 0;
	total_payload_len	= 0;

	next_seg_sn			= 0;
}

#endif
