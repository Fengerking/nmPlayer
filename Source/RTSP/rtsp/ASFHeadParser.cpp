
#include "ASFHeadParser.h"
#include "memory.h"
#include "stdlib.h"
#include "stdio.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

int __inline IsEqualVOGUID2(VOGUID rVOGUID1, VOGUID rVOGUID2)
{
	return !memcmp(&rVOGUID1, &rVOGUID2, sizeof(VOGUID));
}
CASFHeadParser::CASFHeadParser(void)
{
	m_head = NULL;
	m_headSize = 0;
	m_videoInfo.streamNum = -1;
	m_audioInfo.streamNum = -1;
	memset(&m_videoInfo.head,0,sizeof(VOWMV9DECHEADER) );
	memset(&m_audioInfo.head,0,sizeof(WMAHeaderInfo));
}

CASFHeadParser::~CASFHeadParser(void)
{
	SAFE_DELETE_ARRAY(m_videoInfo.head.pHdrExt);
	SAFE_DELETE_ARRAY(m_audioInfo.head.pHdrExt);
	SAFE_DELETE_ARRAY(m_videoInfo.head.pRawBitmapHead);
	SAFE_DELETE_ARRAY(m_audioInfo.head.pRawWAVFormat);
}
bool	CASFHeadParser::	ReadFileHeader()
{
	AsfObject ao;
	read_VOGUID(ao.id);
	read_qword(ao.size);
	if(!IsEqualVOGUID2(ao.id, VO_ASF_GUID_Header_Object)) {
		return false;
	}
	sprintf(CLog::formatString,"id=%ld,%d,%d,size=%lld\n",ao.id.Data1,ao.id.Data2,ao.id.Data3,ao.size);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt",CLog::formatString);
	AsfHeaderObject aho; 
	read_dword(aho.header_objects_number);
	read_byte(aho.reserved1);
	read_byte(aho.reserved2);
	for(DWORD i = 0; i < aho.header_objects_number; i++) {
		sprintf(CLog::formatString,"headObj=%ld,num=%ld\n",i,aho.header_objects_number);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt",CLog::formatString);
		if(!ReadHeaderInfo()) {
			return false;
		}
	}
	return true;
}
bool	CASFHeadParser::	ReadHeaderInfo()
{
	AsfObject ao;
	read_VOGUID(ao.id);
	read_qword(ao.size);
	
	sprintf(CLog::formatString,"id=%ld,%d,%d,size=%lld\n",ao.id.Data1,ao.id.Data2,ao.id.Data3,ao.size);
	CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt",CLog::formatString);
	if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_File_Properties_Object)) {
		skip_asf_object();
		/*
		AsfFilePropObject afpo;
		Read_Value_VOGUID(afpo.file_id);
		Read_Value_QWORD(afpo.file_size);
		Read_Value_QWORD(afpo.creation_date);
		Read_Value_QWORD(afpo.data_packets_count);
		Read_Value_QWORD(afpo.play_duration);
		Read_Value_QWORD(afpo.send_duration);
		Read_Value_QWORD(afpo.preroll);
		Read_Value_DWORD(afpo.flags);
		Read_Value_DWORD(afpo.min_data_packet_size);
		Read_Value_DWORD(afpo.max_data_packet_size);
		Read_Value_DWORD(afpo.max_bitrate);
		//m_dwTimeOffset = (unsigned long)afpo.preroll;
		//m_llDuration = afpo.play_duration;
		if(afpo.min_data_packet_size == afpo.max_data_packet_size) {
			m_dwPacketSize = afpo.min_data_packet_size;
		}*/
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","ASF_File_Properties_Object\n");
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Stream_Properties_Object)) {
		ReadStreamPropObject();
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","ASF_Stream_Properties_Object\n");
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Header_Extension_Object)) {
		AsfHeaderExtObject aheo;
		read_VOGUID(aheo.reserved_filed_1);
		read_word(aheo.reserved_filed_2);
		read_dword(aheo.data_size);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Header_Extension_Object\n");
		//note: aheo.data_size inefficacy, the correct data size is (ao.size - Len_Object - Len_Header_Ext_Object);
		QWORD qwLeave = ao.size - Len_Object - Len_Header_Ext_Object;
		while(qwLeave > 0) {
			int nSize = ReadExtHeaderInfo();
			if(nSize <= 0) {
				skip(qwLeave - Len_Object);
				qwLeave = 0;
			}
			else {
				qwLeave -= nSize;
			}

			if(qwLeave > 0 && qwLeave < Len_Object) {
				skip(qwLeave);
				qwLeave = 0;
			}
		}
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Codec_List_Object)) {
		//get the code list info, but this is optional, so skip for safe
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Codec_List_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Script_Command_Object)) {
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Script_Command_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Marker_Object)) {
		//only skip
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt"," VO_ASF_GUID_Marker_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Bitrate_Mutual_Exclusion_Object)) {
		//only skip
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Bitrate_Mutual_Exclusion_Object\n");
		AsfBitrateMutexObject abmo;
		read_VOGUID(abmo.mutex_type);
		read_word(abmo.stream_numbers_count);
		WORD wStreamNum;
		for(WORD i = 0; i < abmo.stream_numbers_count; i++) {
			read_word(wStreamNum);
		}
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Error_Correction_Object)) {
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Error_Correction_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Content_Description_Object)) {
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Content_Description_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Extended_Content_Description_Object)) {
		//get extended description info, it is unimportant for our filter, so skip
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Extended_Content_Description_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Content_Branding_Object)) {
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Content_Branding_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Stream_Bitrate_Properties_Object)) {
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Stream_Bitrate_Properties_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Content_Encryption_Object)) {
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Content_Encryption_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Extended_Content_Encryption_Object)) {
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Extended_Content_Encryption_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Digital_Signature_Object)) {
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Digital_Signature_Object\n");
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Padding_Object)) {
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","VO_ASF_GUID_Padding_Object\n");
		skip_asf_object();
	}
	else {
		//reading occur exception, please check and fix. currently, we only process mostly ASF files.
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt","error:Invalid_Object\n");
		return false;
	}
	return true;
}
DWORD	CASFHeadParser::	ReadExtHeaderInfo()
{
	AsfObject ao;
	read_VOGUID(ao.id);
	read_qword(ao.size);
	if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Extended_Stream_Properties_Object)) {
		DWORD dwLeave = DWORD(ao.size - Len_Object);

		AsfExtStreamPropObject aespo;
		read_qword(aespo.start_time);
		read_qword(aespo.end_time);
		read_dword(aespo.data_bitrate);
		read_dword(aespo.buffer_size);
		read_dword(aespo.init_buffer_fullness);
		read_dword(aespo.alternate_data_bitrate);
		read_dword(aespo.alternate_buffer_size);
		read_dword(aespo.alternate_init_buffer_fullness);
		read_dword(aespo.max_object_size);
		read_dword(aespo.flags);
		read_word(aespo.stream_number);
		read_word(aespo.stream_lang_id_index);
		read_qword(aespo.average_time_per_frame);
		read_word(aespo.stream_name_count);
		read_word(aespo.payload_ext_sys_count);
		dwLeave -= Len_Ext_Stream_Prop_Object;
		sprintf(CLog::formatString,"start_%lld,end_%lld,num_%d,maxsize=%ld\n",aespo.start_time,aespo.end_time,aespo.stream_number,aespo.max_object_size);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt",CLog::formatString);
		AsfStreamName asn;
		WORD i = 0;
		for(i = 0; i < aespo.stream_name_count; i++) {
			read_word(asn.lang_id_index);
			read_word(asn.name_len);
			skip(asn.name_len);
			dwLeave -= (Len_Stream_Name + asn.name_len);
		}

		AsfPayloadExtSys apes;
		for(i = 0; i < aespo.payload_ext_sys_count; i++) {
			read_VOGUID(apes.id);
			read_word(apes.size);
			read_dword(apes.info_len);
			skip(apes.info_len);
			dwLeave -= (Len_Payload_Ext_Sys + apes.info_len);
		}

		if(dwLeave > 0) {
			AsfObject ao;
			read_VOGUID(ao.id);
			read_qword(ao.size);

			ReadStreamPropObject();
		}
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Advanced_Mutual_Exclusion_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Group_Mutual_Exclusion_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Stream_Prioritization_Object)) {		//0 or 1 in file
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Bandwidth_Sharing_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Language_List_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Metadata_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Metadata_Library_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Index_Parameters_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Media_Object_Index_Parameters_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Timecode_Index_Parameters_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Compatibility_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Advanced_Content_Encryption_Object)) {
		skip_asf_object();
	}
	else if(IsEqualVOGUID2(ao.id, VO_ASF_GUID_Padding_Object)) {
		skip_asf_object();
	}
	else {
		skip_asf_object();
	}
	return (DWORD)ao.size;
}
VOWMV9DECHEADER* CASFHeadParser::GetAsfVideoMediaType()
{
	return m_videoInfo.streamNum==0?&m_videoInfo.head:NULL;
}
WMAHeaderInfo*	CASFHeadParser::GetAsfAudioMediaType()
{
	return m_audioInfo.streamNum==0?&m_audioInfo.head:NULL;
}
bool	CASFHeadParser::	ReadStreamPropObject()
{
	//get streams' properties.
	AsfStreamPropObject aspo;
	read_VOGUID(aspo.stream_type);
	read_VOGUID(aspo.error_correction_type);
	read_qword(aspo.time_offset);
	read_dword(aspo.type_specific_data_len);
	read_dword(aspo.error_correction_data_len);
	read_word(aspo.flags);
	read_dword(aspo.reserved);

	//get stream's media type
	if(IsEqualVOGUID2(aspo.stream_type, VO_ASF_GUID_Audio_Media)) {
		AsfAudioMediaType aamt;
		read_word(aamt.codec_id);
		read_word(aamt.channels);
		read_dword(aamt.samples_per_second);
		read_dword(aamt.average_bytes_per_second);
		read_word(aamt.block_alignment);
		read_word(aamt.bits_per_sample);
		read_word(aamt.codec_data_len);
#define WAV_LEN 18
		if(m_audioInfo.streamNum==-1) {
			WMAHeaderInfo *head = &m_audioInfo.head;
			head->pRawWAVFormat = new BYTE[WAV_LEN+aamt.codec_data_len];;TRACE_NEW("head->pRawWAVFormat",head->pRawWAVFormat);
		
			AsfAudioMediaType* wavHead=(AsfAudioMediaType*)head->pRawWAVFormat;
			if (aamt.codec_id<352)
			{
				//aamt.codec_id=354;//error resilience
			}
			wavHead->codec_id									= head->wFormatTag = aamt.codec_id;
			wavHead->channels									= head->nChannels	 = aamt.channels;
			wavHead->average_bytes_per_second		=head->nAvgBytesPerSec = aamt.average_bytes_per_second;
			wavHead->block_alignment						=head->nBlockAlign = aamt.block_alignment;
			wavHead->samples_per_second				=head->nSamplesPerSec = aamt.samples_per_second;
			//if(aamt.bits_per_sample==0)
			//	aamt.bits_per_sample=16;
			wavHead->bits_per_sample						=head->nValidBitsPerSample = aamt.bits_per_sample;
			wavHead->codec_data_len						=head->iExtSize	= aamt.codec_data_len;
			m_audioInfo.streamNum=0;
			sprintf(CLog::formatString,"Audio:codec_%d,channel_%d,BPS_%ld,SPS=%ld,aligement=%d,bPS=%d\n",
				aamt.codec_id,
				aamt.channels,
				aamt.average_bytes_per_second,
				aamt.samples_per_second,
				aamt.block_alignment,
				aamt.bits_per_sample);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt",CLog::formatString);
			if(head->iExtSize>0)
			{
				head->pHdrExt = new BYTE[head->iExtSize];TRACE_NEW("head->pHdrExt",head->pHdrExt);
		
				read_pointer(head->pHdrExt, head->iExtSize);
				memcpy(head->pRawWAVFormat+WAV_LEN,head->pHdrExt,head->iExtSize);
			}
			else
			{
				head->pHdrExt = NULL;
			}
		}
		else
		{
			skip(aamt.codec_data_len);

		}
		
	}
	else if(IsEqualVOGUID2(aspo.stream_type, VO_ASF_GUID_Video_Media)) {
		AsfVideoMediaType avmt;
		read_dword(avmt.image_width);
		read_dword(avmt.image_height);
		read_byte(avmt.reserved);
		read_word(avmt.format_data_len);
		//get subtype according avmt.format_data.compression_id(VOFOURCC)
		WORD wBufSize = avmt.format_data_len - Len_Video_Format;
		if(m_videoInfo.streamNum==-1) {
			int temp;
			
			VOWMV9DECHEADER *head=&m_videoInfo.head;
			head->pRawBitmapHead = new BYTE[avmt.format_data_len];TRACE_NEW("head->pRawBitmapHead",head->pRawBitmapHead);
			

			memcpy(head->pRawBitmapHead,m_head,avmt.format_data_len);
			head->iExtSize = wBufSize;
			read_dword(temp);
			read_dword(head->iPicHorizSize);
			read_dword(head->iPicVertSize);
			read_word(temp);
			read_word(temp);
			read_dword(head->iCodecVer);
			read_dword(temp);
			read_dword(temp);
			read_dword(temp);
			read_dword(temp);
			read_dword(temp);
			if(wBufSize>0)
			{
				head->pHdrExt = new BYTE[wBufSize];TRACE_NEW("rtss_head->pHdrExt2",head->pHdrExt);
				
				read_pointer(head->pHdrExt, wBufSize);
			}
			else
			{
				head->pHdrExt = NULL;

			}
			sprintf(CLog::formatString,"Video:codec_%d,w_%d,h_%d\n",
				head->iCodecVer,
				head->iPicHorizSize,
				head->iPicVertSize
				);
			CLog::Log.MakeLog(LL_SOCKET_ERR,"wmv.txt",CLog::formatString);

			m_videoInfo.streamNum = 0;
		}
		else {
			skip(avmt.format_data_len);
		}
	}
	else {	//skip
		skip(aspo.type_specific_data_len);
	}
	skip(aspo.error_correction_data_len);

	return true;
}
bool CASFHeadParser::Parse(unsigned char* head,int size)
{
	m_head = head;
	m_headSize = size;
	return ReadFileHeader();
}
