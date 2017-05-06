#include "CAsfParser.h"
#include "AsfFileGuid.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#define skip_asf_object()	ptr_skip(ao.size - Len_Object)

#define read_by_type(d, type) {\
	switch(type)\
	{\
	case 1:\
		{\
			VO_U8 b;\
			ptr_read_byte(b);\
			d = b;\
		}\
		break;\
	case 2:\
		{\
			VO_U16 w;\
			ptr_read_word(w);\
			d = w;\
		}\
		break;\
	case 3:\
		ptr_read_dword(d);\
		break;\
	default:\
		break;\
	}\
}

#define skip_by_type(type) {\
	switch(type)\
	{\
	case 1:\
		ptr_skip(1);\
		break;\
	case 2:\
		ptr_skip(2);\
		break;\
	case 3:\
		ptr_skip(4);\
		break;\
	default:\
		break;\
	}\
}

CAsfHeaderParser::CAsfHeaderParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileHeaderParser(pFileChunk, pMemOp)
	, m_nTrackInfoCount(0)
	, m_paTrackInfo(VO_NULL)
	, m_paStreamExtInfo(VO_NULL)
	, m_wBitrateInfoCount(0)
	, m_paBitrateInfo(VO_NULL)
	, m_wPriorInfoCount(0)
	, m_paPriorInfo(VO_NULL)
	, m_pContentEncryptionObject(VO_NULL)
	, m_dwContentEncryptionObject(0)
	, m_pExtendedContentEncryptionObject(VO_NULL)
	, m_dwExtendedContentEncryptionObject(0)
	, m_pProtectionSystemIdentifierObject(VO_NULL)
	, m_dwProtectionSystemIdentifierObject(0)
	, m_pIVInfos(VO_NULL)
	, m_iStreamNum(0)
	, m_ullTimeOffset(0)
	, m_dwDuration(0)
	, m_dwPacketSize(0)
	, m_ullIndexObjectsFilePos(0)
	, m_ullMediaDataFilePos(0)
	, m_ullContentDescriptionObjectFilePos(VO_MAXU64)
	, m_ullExtendedContentDescriptionObjectFilePos(VO_MAXU64)
	, m_ullMetadataObjectFilePos(VO_MAXU64)
	, m_ullMetadataLibraryObjectFilePos(VO_MAXU64)
	, m_ullActualFileSize(0)
{
}

CAsfHeaderParser::~CAsfHeaderParser()
{
	ReleaseTmpInfo();
	
	if (m_pIVInfos)
		SAFE_MEM_FREE(m_pIVInfos);
}

VO_BOOL CAsfHeaderParser::ReadFromFile()
{
	//process header information
	AsfObject ao;
	ptr_read_guid(ao.id);
	ptr_read_qword(ao.size);
	if(!voIsEqualGUID(ao.id, ASF_Header_Object))
		return VO_FALSE;

	AsfHeaderObject aho;
	ptr_read_dword(aho.header_objects_number);
	ptr_read_byte(aho.reserved1);
	ptr_read_byte(aho.reserved2);
	for(VO_U32 i = 0; i <= aho.header_objects_number; i++)
	{
		VO_S32 nRet = ReadHeaderInfo();
		if(!nRet)
			return VO_FALSE;
		else if(2 == nRet)
			break;
	}

	//process media data
	ptr_read_guid(ao.id);
	ptr_read_qword(ao.size);
	if(!voIsEqualGUID(ao.id, ASF_Data_Object))
		return VO_FALSE;

//	AsfDataObject ado;
//	read_guid(ado.file_id);
//	read_qword(ado.total_data_packets);
//	read_word(ado.reserved);
	//maybe some files not walk the chalks, we should support them!
//	if(0x101 != ado.reserved)
//		return false;

	m_ullMediaDataFilePos = m_pFileChunk->FGetFilePos() + Len_Data_Object;
	m_ullIndexObjectsFilePos = m_pFileChunk->FGetFilePos() + ao.size - Len_Object;

	return VO_TRUE;
}

VO_S32 CAsfHeaderParser::ReadHeaderInfo()
{
	AsfObject ao;
	ptr_read_guid(ao.id);
	ptr_read_qword(ao.size);
	if(voIsEqualGUID(ao.id, ASF_File_Properties_Object))
	{
		AsfFilePropObject afpo;
		ptr_read_guid(afpo.file_id);
		ptr_read_qword(afpo.file_size);
		ptr_read_qword(afpo.creation_date);
		ptr_read_qword(afpo.data_packets_count);
		ptr_read_qword(afpo.play_duration);
		ptr_read_qword(afpo.send_duration);
		ptr_read_qword(afpo.preroll);
		ptr_read_dword(afpo.flags);
		ptr_read_dword(afpo.min_data_packet_size);
		ptr_read_dword(afpo.max_data_packet_size);
		ptr_read_dword(afpo.max_bitrate);
		m_ullTimeOffset = afpo.preroll;
		m_dwDuration = static_cast<VO_U32>(afpo.play_duration / 10000 - afpo.preroll);
		if(m_dwDuration < 0)
			m_dwDuration = 0;

		m_ullActualFileSize = afpo.file_size;
		/*added by danny*/
		m_maxBitrate = afpo.max_bitrate/8;

		if(afpo.min_data_packet_size == afpo.max_data_packet_size)
			m_dwPacketSize = afpo.min_data_packet_size;
	}
	else if(voIsEqualGUID(ao.id, ASF_Stream_Properties_Object))
	{
		if ( !ReadStreamPropObject( static_cast<VO_S32>(ao.size) ) )
			return 0;

		m_iStreamNum++;
	}
	else if(voIsEqualGUID(ao.id, ASF_Header_Extension_Object))
	{
		AsfHeaderExtObject aheo;
		ptr_read_guid(aheo.reserved_filed_1);
		ptr_read_word(aheo.reserved_filed_2);
		ptr_read_dword(aheo.data_size);
		//note: aheo.data_size inefficacy, the correct data size is (ao.size - Len_Object - Len_Header_Ext_Object);
		if (m_dwProtectionSystemIdentifierObject)
		{
			m_pIVInfos = NEW_OBJS(IVInfo, m_iStreamNum);
			if(!m_pIVInfos)
				return 0;

			memset(m_pIVInfos, 0, sizeof(IVInfo)*m_iStreamNum);
		}

		VO_S64 qwLeave = ao.size - Len_Object - Len_Header_Ext_Object;
		while(qwLeave > 0)
		{
			VO_S32 nSize = ReadExtHeaderInfo();
			if(nSize <= 0)
			{
				ptr_skip(qwLeave - Len_Object);
				qwLeave = 0;
			}
			else
				qwLeave -= nSize;

			if(qwLeave > 0 && qwLeave < Len_Object)
			{
				ptr_skip(qwLeave);
				qwLeave = 0;
			}
		}
	}
	else if(voIsEqualGUID(ao.id, ASF_Codec_List_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Script_Command_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Marker_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Bitrate_Mutual_Exclusion_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Error_Correction_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Content_Description_Object))
	{
		m_ullContentDescriptionObjectFilePos = m_pFileChunk->FGetFilePos();

		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Extended_Content_Description_Object))
	{
		m_ullExtendedContentDescriptionObjectFilePos = m_pFileChunk->FGetFilePos();

		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Content_Branding_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Stream_Bitrate_Properties_Object))
	{
		ptr_read_word(m_wBitrateInfoCount);
		m_paBitrateInfo = NEW_OBJS(AsfBitrateInfo, m_wBitrateInfoCount);
		VO_U16 wFlags;
		for(VO_U16 i = 0; i < m_wBitrateInfoCount; i++)
		{
			ptr_read_word(wFlags);
			m_paBitrateInfo[i].btStreamNum = VO_U8(wFlags & 0x7F);
			ptr_read_dword(m_paBitrateInfo[i].dwBitrate);
		}
	}
	else if(voIsEqualGUID(ao.id, ASF_Content_Encryption_Object))
	{
		SAFE_MEM_FREE(m_pContentEncryptionObject);

		m_dwContentEncryptionObject = static_cast<VO_U32>(ao.size - Len_Object);
		m_pContentEncryptionObject = NEW_BUFFER(m_dwContentEncryptionObject);
		if(!m_pContentEncryptionObject)
			return 0;

		ptr_read_pointer(m_pContentEncryptionObject, m_dwContentEncryptionObject);
		
//		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Extended_Content_Encryption_Object))
	{
		SAFE_MEM_FREE(m_pExtendedContentEncryptionObject);

		m_dwExtendedContentEncryptionObject = static_cast<VO_U32>(ao.size - Len_Object);
		m_pExtendedContentEncryptionObject = NEW_BUFFER(m_dwExtendedContentEncryptionObject);
		if(!m_pExtendedContentEncryptionObject)
			return 0;

		ptr_read_pointer(m_pExtendedContentEncryptionObject, m_dwExtendedContentEncryptionObject);

//		skip_asf_object();
	}
	else if (voIsEqualGUID(ao.id, ASF_Protection_System_Identifier_Object))
	{
		SAFE_MEM_FREE(m_pProtectionSystemIdentifierObject);

		ptr_read_guid(ao.id);//ASF_Content_Protection_System_Microsoft_PlayReady

		VO_U32 ui = 0;
		ptr_read_dword(ui);
		if (!voIsEqualGUID(ao.id, ASF_Content_Protection_System_Microsoft_PlayReady) || 1 != ui)
			return 0;

		ptr_read_dword(m_dwProtectionSystemIdentifierObject);

		m_pProtectionSystemIdentifierObject = NEW_BUFFER(m_dwProtectionSystemIdentifierObject);
		if(!m_pProtectionSystemIdentifierObject)
			return 0;

		ptr_read_pointer(m_pProtectionSystemIdentifierObject, m_dwProtectionSystemIdentifierObject);
	}
	else if(voIsEqualGUID(ao.id, ASF_Digital_Signature_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Padding_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Header_Object_Unknown_1))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Data_Object))
	{
		if(!m_pFileChunk->FBack(Len_Object))
			return 0;

		return 2;
	}
	else
	{
		//we cann't recognise this GUID
#ifdef VO_FILE_ASF_DEBUG_ERR
		//return false so that user will recognize it!!
		return 0;
#else	//VO_FILE_ASF_DEBUG_ERR
		//only skip so that file can playback correctly!!
		skip_asf_object();
#endif	//VO_FILE_ASF_DEBUG_ERR
	}

	return 1;
}

VO_U32 CAsfHeaderParser::ReadExtHeaderInfo()
{
	AsfObject ao;
	ptr_read_guid(ao.id);
	ptr_read_qword(ao.size);
	if(voIsEqualGUID(ao.id, ASF_Extended_Stream_Properties_Object))
	{
		AsfExtStreamPropObject aespo;
		ptr_read_qword(aespo.start_time);
		ptr_read_qword(aespo.end_time);
		ptr_read_dword(aespo.data_bitrate);
		ptr_read_dword(aespo.buffer_size);
		ptr_read_dword(aespo.init_buffer_fullness);
		ptr_read_dword(aespo.alternate_data_bitrate);
		ptr_read_dword(aespo.alternate_buffer_size);
		ptr_read_dword(aespo.alternate_init_buffer_fullness);
		ptr_read_dword(aespo.max_object_size);
		ptr_read_dword(aespo.flags);
		ptr_read_word(aespo.stream_number);
		ptr_read_word(aespo.stream_lang_id_index);
		ptr_read_qword(aespo.average_time_per_frame);
		ptr_read_word(aespo.stream_name_count);
		ptr_read_word(aespo.payload_ext_sys_count);

		PAsfStreamExtInfo pNewInfo = NEW_OBJ(AsfStreamExtInfo);
		pNewInfo->btStreamNum = VO_U8(aespo.stream_number & 0x7F);
		pNewInfo->dwBitrate = aespo.data_bitrate / 8;
		pNewInfo->dwMaxSampleSize = aespo.max_object_size;
		pNewInfo->dwFrameRate = aespo.average_time_per_frame ? static_cast<VO_U32>(1000000000 / aespo.average_time_per_frame) : 0;
		pNewInfo->next = VO_NULL;
		ADD_TO_CHAIN(AsfStreamExtInfo, m_paStreamExtInfo, pNewInfo);
		VO_S32 nLeave = VO_S32(ao.size - Len_Object - Len_Ext_Stream_Prop_Object);

		AsfStreamName asn;
		VO_U16 i = 0;
		for(i = 0; i < aespo.stream_name_count; i++)
		{
			ptr_read_word(asn.lang_id_index);
			ptr_read_word(asn.name_len);
			ptr_skip(asn.name_len);
			nLeave -= (Len_Stream_Name + asn.name_len);
		}

		VO_BOOL bFoundIV = VO_FALSE;

		AsfPayloadExtSys apes;
		for(i = 0; i < aespo.payload_ext_sys_count; i++)
		{
			ptr_read_guid(apes.id);
			ptr_read_word(apes.size);
			ptr_read_dword(apes.info_len);

			if (voIsEqualGUID(apes.id, ASF_Payload_Extension_System_Encryption_Sample_ID))
			{
				m_pIVInfos[pNewInfo->btStreamNum - 1].iLen = apes.size;

				bFoundIV = VO_TRUE;
			}
			else
			{
				if (VO_FALSE == bFoundIV && m_pIVInfos)
					m_pIVInfos[pNewInfo->btStreamNum - 1].iOffset += apes.size;
			}

			ptr_skip(apes.info_len);
			
			nLeave -= (Len_Payload_Ext_Sys + apes.info_len);
		}

		if(nLeave > 0)
		{
			AsfObject ao;
			ptr_read_guid(ao.id);
			ptr_read_qword(ao.size);

			if ( !ReadStreamPropObject( static_cast<VO_S32>(ao.size) ) )
				return 0;
		}
	}
	else if(voIsEqualGUID(ao.id, ASF_Advanced_Mutual_Exclusion_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Group_Mutual_Exclusion_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Stream_Prioritization_Object))	//0 or 1 in file
	{
		ptr_read_word(m_wPriorInfoCount);
		m_paPriorInfo = NEW_OBJS(AsfPriorInfo, m_wPriorInfoCount);
		VO_U16 wFlags;
		for(VO_U16 i = 0; i < m_wPriorInfoCount; i++)
		{
			ptr_read_word(wFlags);
			m_paPriorInfo[i].btStreamNum = static_cast<VO_U8>(wFlags & 0x7F);
			m_paPriorInfo[i].btPrior = i + 1;
			ptr_skip(2);
		}
	}
	else if(voIsEqualGUID(ao.id, ASF_Bandwidth_Sharing_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Language_List_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Metadata_Object))
	{
		m_ullMetadataObjectFilePos = m_pFileChunk->FGetFilePos();
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Metadata_Library_Object))
	{
		m_ullMetadataLibraryObjectFilePos = m_pFileChunk->FGetFilePos();
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Index_Parameters_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Media_Object_Index_Parameters_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Timecode_Index_Parameters_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Compatibility_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Advanced_Content_Encryption_Object))
	{
		skip_asf_object();
	}
	else if(voIsEqualGUID(ao.id, ASF_Padding_Object))
	{
		skip_asf_object();
	}
	else
	{
		skip_asf_object();
	}

	return (VO_U32)ao.size;
}

VO_BOOL CAsfHeaderParser::ReadStreamPropObject(VO_S32 iLenObject)
{
	//get streams' properties.
	AsfStreamPropObject aspo;
	ptr_read_guid(aspo.stream_type);
	ptr_read_guid(aspo.error_correction_type);
	ptr_read_qword(aspo.time_offset);
	ptr_read_dword(aspo.type_specific_data_len);
	ptr_read_dword(aspo.error_correction_data_len);
	ptr_read_word(aspo.flags);
	ptr_read_dword(aspo.reserved);

	//get stream's media type
	if(voIsEqualGUID(aspo.stream_type, ASF_Audio_Media))
	{
		AsfAudioMediaType aamt;
		ptr_read_word(aamt.codec_id);
		ptr_read_word(aamt.channels);
		ptr_read_dword(aamt.samples_per_second);
		ptr_read_dword(aamt.average_bytes_per_second);
		ptr_read_word(aamt.block_alignment);
		ptr_read_word(aamt.bits_per_sample);
		ptr_read_word(aamt.codec_data_len);

		PAsfTrackInfo pti = NewTrackInfo(VO_U8(aspo.flags << 1) >> 1, VO_FALSE, VO_WAVEFORMATEX_STRUCTLEN + aamt.codec_data_len);
		if(pti)
		{
			VO_WAVEFORMATEX* pwfe = (VO_WAVEFORMATEX*)pti->pPropBuffer;
			pwfe->wFormatTag = aamt.codec_id;
			pwfe->nChannels = aamt.channels;
			pwfe->nSamplesPerSec = aamt.samples_per_second;
			pwfe->nAvgBytesPerSec = aamt.average_bytes_per_second;
			pwfe->nBlockAlign = aamt.block_alignment;
			pwfe->wBitsPerSample = aamt.bits_per_sample;
			pwfe->cbSize = aamt.codec_data_len;

			if (aamt.codec_id == 0x5052)
			{
				ptr_read_pointer(pti->pPropBuffer + VO_WAVEFORMATEX_STRUCTLEN, aamt.codec_data_len - 2);
				ptr_read_word(pwfe->wFormatTag);
			}
			else
			{
				ptr_read_pointer(pti->pPropBuffer + VO_WAVEFORMATEX_STRUCTLEN, aamt.codec_data_len);
			}
			//add
			/*VO_PBYTE		pStreamType;
			ptr_read_pointer(pStreamType);
			VO_U8 channels = 0;
			ptr_read_pointer(channels);*/
			//
			ptr_skip(aspo.type_specific_data_len - VO_WAVEFORMATEX_STRUCTLEN - aamt.codec_data_len);

			//ptr_read_pointer(pti->pPropBuffer + VO_WAVEFORMATEX_STRUCTLEN, aamt.codec_data_len);
		}
		else
		{
			ptr_skip(aamt.codec_data_len);
		}
	}
	else if(voIsEqualGUID(aspo.stream_type, ASF_Video_Media))
	{
		AsfVideoMediaType avmt;
		ptr_read_dword(avmt.image_width);
		ptr_read_dword(avmt.image_height);
		ptr_read_byte(avmt.reserved);
		ptr_read_word(avmt.format_data_len);
		//get subtype according avmt.format_data.compression_id(FOURCC)
		VO_U32 dwBufSize = aspo.type_specific_data_len - Len_Video_Media_Type;//avmt.format_data_len - Len_Video_Format;

		PAsfTrackInfo pti = NewTrackInfo(VO_U8(aspo.flags << 1) >> 1, VO_TRUE, sizeof(VO_VIDEOINFOHEADER) + dwBufSize);
		if(pti)
		{
			VO_VIDEOINFOHEADER* pvih = (VO_VIDEOINFOHEADER*)pti->pPropBuffer;
			ptr_read_dword(pvih->bmiHeader.biSize);
			ptr_read_dword(pvih->bmiHeader.biWidth);
			ptr_read_dword(pvih->bmiHeader.biHeight);
			ptr_read_word(pvih->bmiHeader.biPlanes);
			ptr_read_word(pvih->bmiHeader.biBitCount);
			ptr_read_dword(pvih->bmiHeader.biCompression);
			ptr_read_dword(pvih->bmiHeader.biSizeImage);
			ptr_read_dword(pvih->bmiHeader.biXPelsPerMeter);
			ptr_read_dword(pvih->bmiHeader.biYPelsPerMeter);
			ptr_read_dword(pvih->bmiHeader.biClrUsed);
			ptr_read_dword(pvih->bmiHeader.biClrImportant);

			pvih->dwBitRate = pvih->bmiHeader.biSizeImage;
			pvih->dwBitErrorRate = 0;
			pvih->AvgTimePerFrame = 0;
			pvih->rcTarget.left = pvih->rcSource.left = 0;
			pvih->rcTarget.top = pvih->rcSource.top = 0;
			pvih->rcTarget.right = pvih->rcSource.right = avmt.image_width;
			pvih->rcTarget.bottom = pvih->rcSource.bottom = avmt.image_height;
			pvih->bmiHeader.biSizeImage = pvih->bmiHeader.biWidth * pvih->bmiHeader.biHeight * pvih->bmiHeader.biBitCount / 8;

			if (pvih->bmiHeader.biCompression == 0x59445250)
			{
				ptr_read_pointer(pvih + 1, dwBufSize - 4);
				ptr_read_dword(pvih->bmiHeader.biCompression);
			}
			else
			{
				ptr_read_pointer(pvih + 1, dwBufSize);
			}
		}
		else
		{
			ptr_skip(avmt.format_data_len);
		}
	}
	else
	{
		ptr_skip(aspo.type_specific_data_len);
	}
	//ptr_read_guid(aspo.stream_type);
	//VO_U8 channels = 0;
	//ptr_read_byte(channels);
	//VO_U16 size = 0;
	//ptr_read_word(size);
	ptr_skip(aspo.error_correction_data_len);
	ptr_skip(iLenObject - aspo.type_specific_data_len - aspo.error_correction_data_len - Len_Stream_Prop_Object - Len_Object)

	return VO_TRUE;
}

VO_VOID CAsfHeaderParser::ReleaseTmpInfo()
{
	SAFE_MEM_FREE(m_paBitrateInfo);
	m_wBitrateInfoCount = 0;

	SAFE_MEM_FREE(m_paPriorInfo);
	m_wPriorInfoCount = 0;

	SAFE_RELEASE_CHAIN(AsfStreamExtInfo, m_paStreamExtInfo);

	if(m_paTrackInfo)
	{
		PAsfTrackInfo pCur = m_paTrackInfo;
		PAsfTrackInfo pDel;
		while(pCur)
		{
			pDel = pCur;
			pCur = pCur->next;
			MemFree(pDel->pPropBuffer);
			MemFree(pDel);
		}
		m_paTrackInfo = VO_NULL;
	}
	m_nTrackInfoCount = 0;

	SAFE_MEM_FREE(m_pContentEncryptionObject);
	m_dwContentEncryptionObject = 0;

	SAFE_MEM_FREE(m_pExtendedContentEncryptionObject);
	m_dwExtendedContentEncryptionObject = 0;
}

PAsfTrackInfo CAsfHeaderParser::NewTrackInfo(VO_U8 btStreamNum, VO_BOOL bVideo, VO_U32 dwPropBufferSize)
{
	if(GetTrackInfo(btStreamNum))	//already occur
		return VO_NULL;

	PAsfTrackInfo pNewInfo = NEW_OBJ(AsfTrackInfo);
	if(!pNewInfo)
		return VO_NULL;

	pNewInfo->btStreamNum = btStreamNum;
	pNewInfo->bVideo = bVideo;
	pNewInfo->dwPropBufferSize = dwPropBufferSize;
	pNewInfo->pPropBuffer = NEW_BUFFER(dwPropBufferSize);
	pNewInfo->next = VO_NULL;
	ADD_TO_CHAIN(AsfTrackInfo, m_paTrackInfo, pNewInfo);
	m_nTrackInfoCount++;

	return pNewInfo;
}

PAsfTrackInfo CAsfHeaderParser::GetTrackInfo(VO_U8 btStreamNum)
{
	PAsfTrackInfo pTrackInfo = m_paTrackInfo;
	while(pTrackInfo)
	{
		if(pTrackInfo->btStreamNum == btStreamNum)
			return pTrackInfo;

		pTrackInfo = pTrackInfo->next;
	}

	return VO_NULL;
}

PAsfStreamExtInfo CAsfHeaderParser::GetTrackExtInfo(VO_U8 btStreamNum)
{
	PAsfStreamExtInfo pSEInfo = m_paStreamExtInfo;
	while(pSEInfo)
	{
		if(pSEInfo->btStreamNum == btStreamNum)
			return pSEInfo;

		pSEInfo = pSEInfo->next;
	}

	return VO_NULL;
}

VO_U8 CAsfHeaderParser::GetPrior(VO_U8 btStreamNum)
{
	for(VO_U16 i = 0; i < m_wPriorInfoCount; i++)
	{
		if(m_paPriorInfo[i].btStreamNum == btStreamNum)
			return m_paPriorInfo[i].btPrior;
	}

	return 0;
}


VO_U32 CAsfHeaderParser::GetBitrate(VO_U8 btStreamNum)
{
	for(VO_U16 i = 0; i < m_wBitrateInfoCount; i++)
	{
		if(m_paBitrateInfo[i].btStreamNum == btStreamNum)
			return m_paBitrateInfo[i].dwBitrate / 8;
	}

	return 0;
}

VO_BOOL CAsfHeaderParser::GetContentEncryptObjects(VO_PBYTE* ppContentEncryptionObject, VO_U32* pdwContentEncryptionObject, VO_PBYTE* ppExtendedContentEncryptionObject, VO_U32* pdwExtendedContentEncryptionObject)
{
	if(!m_pContentEncryptionObject && !m_pExtendedContentEncryptionObject)
		return VO_FALSE;

	if(ppContentEncryptionObject)
		*ppContentEncryptionObject = m_pContentEncryptionObject;

	if(pdwContentEncryptionObject)
		*pdwContentEncryptionObject = m_dwContentEncryptionObject;

	if(ppExtendedContentEncryptionObject)
		*ppExtendedContentEncryptionObject = m_pExtendedContentEncryptionObject;

	if(pdwExtendedContentEncryptionObject)
		*pdwExtendedContentEncryptionObject = m_dwExtendedContentEncryptionObject;

	return VO_TRUE;
}

VO_BOOL	CAsfHeaderParser::GetProtectionSystemIdentifierObject(VO_PBYTE *ppProtectionSystemIdentifierObject, VO_U32 *pdwProtectionSystemIdentifierObject)
{
	if (!m_pProtectionSystemIdentifierObject && !m_dwProtectionSystemIdentifierObject)
		return VO_FALSE;

	if (ppProtectionSystemIdentifierObject)
		*ppProtectionSystemIdentifierObject = m_pProtectionSystemIdentifierObject;

	if (pdwProtectionSystemIdentifierObject)
		*pdwProtectionSystemIdentifierObject = m_dwProtectionSystemIdentifierObject;

	return VO_TRUE;
}

VO_BOOL	CAsfHeaderParser::GetPayloadExtensionSystemEncryptionSampleID(IVInfo **ppInfo)
{
	if (!m_pIVInfos)
		return VO_FALSE;

	*ppInfo = m_pIVInfos;

	return VO_TRUE;
}

CAsfDataParser::CAsfDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileDataParser(pFileChunk, pMemOp)
	, m_dwPacketSize(0)
	, m_ullTimeOffset(0)
	, m_pIVInfos(VO_NULL)
{
}

CAsfDataParser::~CAsfDataParser()
{
}

VO_VOID CAsfDataParser::Init(VO_PTR pParam)
{
	PAsfDataParserInitParam pInitParam = (PAsfDataParserInitParam)pParam;

	m_dwPacketSize = pInitParam->dwPacketSize;
	m_ullTimeOffset = pInitParam->ullTimeOffset;
}

VO_BOOL CAsfDataParser::StepB()
{
	m_pFileChunk->FStartRecord();
	VO_U8 btLengthTypeFlags = 0;
	ptr_read_byte(btLengthTypeFlags);
	if(btLengthTypeFlags & 0x80)		//0x10000000, start with error correction data
	{
		//if the data packet start with error correction data, we have not processed it.
		ptr_skip(btLengthTypeFlags & 0xF);			//0x00001111
		ptr_read_byte(btLengthTypeFlags);
	}

	//process "length type flags"
	//EKKDDSSM
	//M: Multiple Payloads Present
	//S: Sequence Type
	//D: Padding Length Type
	//K: Packet Length Type
	//E: Error Correction Present

	//Sequence Type should be 0
	if(btLengthTypeFlags & 0x6)
		return VO_FALSE;

	//process "property flags"
	VO_U8 btPropertyFlags = 0;
	ptr_read_byte(btPropertyFlags);
	if(0x40 != (btPropertyFlags & 0xc0))
		return VO_FALSE;

	//process "packet length"
	VO_U32 dwPacketLen = 0;
	read_by_type(dwPacketLen, (btLengthTypeFlags >> 5) & 0x3);
	//must be 0 or value less than m_dwPacketSize!!
	if(dwPacketLen > m_dwPacketSize)
		return VO_FALSE;

	//process "sequence"
	//reserved for future use
	//should be 0
//	skip_by_type((btLengthTypeFlags >> 1) & 0x3);

	//process "padding length", skip
	VO_U32 dwPaddingLen = 0;
	read_by_type(dwPaddingLen, (btLengthTypeFlags >> 3) & 0x3);

	//process "send time", the send time of this packet, in millisecond units
	ptr_skip(6);

	if(btLengthTypeFlags & 0x1)
	{
		//Payload Flags
		/*		Number of Payloads		6 (LSB)
		Payload Length Type		2*/
		VO_U8 btPayloadFlags = 0;
		ptr_read_byte(btPayloadFlags);

		for(VO_U8 i = 0; i < (btPayloadFlags & 0x3F); i++)
		{
			if(!ReadPayloads(btPropertyFlags, dwPacketLen, dwPaddingLen, btPayloadFlags >> 6))
				return VO_FALSE;
		}

		//padding data
		//Padding Length is not accurate, but we should support them!!
		//East 2010/08/16
 		if(!m_dwPacketSize && dwPaddingLen > 0)
 			ptr_skip(dwPaddingLen);

		//payload data padding data
		VO_U64 qwLastRead = m_pFileChunk->FGetLastRead();
		if(m_dwPacketSize > qwLastRead)
			ptr_skip(m_dwPacketSize - qwLastRead);
	}
	else
	{
		if(!ReadPayloads(btPropertyFlags, dwPacketLen, dwPaddingLen, 0))
			return VO_FALSE;

		//padding data
		if(dwPaddingLen > 0)
			ptr_skip(dwPaddingLen);
	}

	return VO_TRUE;
}

VO_BOOL CAsfDataParser::ReadPayloads(VO_U8 btPropertyFlags, VO_U32 dwPacketLen, VO_U32 dwPaddingLen, VO_U8 btPayloadLenType)
{
	//process "property flags"
	//SSMMOORR
	//R: Replicated Data Length Type
	//O: Offset Into Media Object Length Type
	//M: Media Object Number Length Type
	//S: Stream Number Length Type

	//stream number
	VO_U8 btStreamNum = 0;
	ptr_read_byte(btStreamNum);

	//media object number
	skip_by_type((btPropertyFlags >> 4) & 0x3);

	//offset into media object or presentation time
	VO_U32 dwMediaObjOffset = 0;
	read_by_type(dwMediaObjOffset, (btPropertyFlags >> 2) & 0x3);

	//replicated data
	VO_U32 dwRepDataLen = 0;
	read_by_type(dwRepDataLen, btPropertyFlags & 0x3);

	if(dwRepDataLen == 0)		//compressed only one frame
	{
		VO_U32 dwPayloadLen = 0;
		VO_U32 dwInternalPaddingLen = 0;
		if(btPayloadLenType == 0)
		{
			dwInternalPaddingLen = dwPacketLen ? m_dwPacketSize - dwPacketLen : 0;
			VO_U64 qwLastRead = m_pFileChunk->FGetLastRead();
			dwPayloadLen = static_cast<VO_U32>( (m_dwPacketSize ? m_dwPacketSize : dwPacketLen) - 
				(qwLastRead + dwPaddingLen + dwInternalPaddingLen) );
		}
		else
			read_by_type(dwPayloadLen, btPayloadLenType);

		//this payload must be not first payload of frame
		//so frame size and timestamp is not important
		if(!OnPayload(btStreamNum, dwMediaObjOffset, dwPayloadLen, 0, 0))
			return VO_FALSE;

		if(dwInternalPaddingLen > 0)
			ptr_skip(dwInternalPaddingLen);
	}
	else if(dwRepDataLen == 1)	//compressed
	{
		/*		Stream Number	BYTE	8
		Media Object Number	BYTE, WORD or DWORD	0, 8, 16, 32
		Presentation Time	BYTE, WORD or DWORD	0, 8, 16, 32
		Replicated Data Length	BYTE, WORD or DWORD	0, 8, 16, 32
		Presentation Time Delta	BYTE	8
		Sub-Payload Data	BYTE	varies*/

		VO_U8 btPreTimeDelta = 0;
		ptr_read_byte(btPreTimeDelta);

		VO_U32 dwPayloadLen = 0;
		VO_U32 dwInternalPaddingLen = 0;
		if(btPayloadLenType == 0)
		{
			dwInternalPaddingLen = dwPacketLen ? m_dwPacketSize - dwPacketLen : 0;
			VO_U64 qwLastRead = m_pFileChunk->FGetLastRead();
			dwPayloadLen = static_cast<VO_U32>( (m_dwPacketSize ? m_dwPacketSize : dwPacketLen) - 
				(qwLastRead + dwPaddingLen + dwInternalPaddingLen) );
		}
		else
			read_by_type(dwPayloadLen, btPayloadLenType);

		//get media data at this position
		VO_U32 dwPacketRead = 0;
		VO_U8 btLen = 0;
		VO_S32 nIdx = 0;
		while(dwPacketRead < dwPayloadLen)
		{
			ptr_read_byte(btLen);
			dwPacketRead += (btLen + 1);

			//copy from ASF_Specification.doc page51 line12
			//Each payload represents an entire media object (rather than a fragment thereof).
			if(!OnPayload(btStreamNum, 0, btLen, btLen, dwMediaObjOffset + nIdx * btPreTimeDelta))
				return VO_FALSE;

			nIdx++;
		}

		if(dwInternalPaddingLen > 0)
			ptr_skip(dwInternalPaddingLen);
	}
	else
	{
		/*		Stream Number	BYTE	8
		Media Object Number	BYTE, WORD or DWORD	0, 8, 16, 32
		Offset Into Media Object	BYTE, WORD or DWORD	0, 8, 16, 32
		Replicated Data Length	BYTE, WORD or DWORD	0, 8, 16, 32
		Replicated Data	BYTE	varies
		Payload Length	BYTE, WORD or DWORD	8, 16, 32
		Payload Data	BYTE	varies*/

		VO_U32 dwMediaObjSize = 0, dwMediaObjTime = 0;
		ptr_read_dword(dwMediaObjSize);
		ptr_read_dword(dwMediaObjTime);

		dwRepDataLen -= 8;
		if (m_pIVInfos)
		{
			VO_S32 iCount = (btStreamNum & 0x7f) - 1;

			if (m_pIVInfos[iCount].iOffset)
				ptr_skip(m_pIVInfos[iCount].iOffset);

			memset(&m_sIV[iCount], 0, 8);
			ptr_read_pointer(&m_sIV[iCount], m_pIVInfos[iCount].iLen);

			dwRepDataLen = dwRepDataLen - m_pIVInfos[iCount].iOffset - m_pIVInfos[iCount].iLen;
		}

		if(dwRepDataLen > 0)
			ptr_skip(dwRepDataLen);

		//payload data
		VO_U32 dwPayloadLen = 0;
		VO_U32 dwInternalPaddingLen = 0;
		if(btPayloadLenType == 0)
		{
			dwInternalPaddingLen = dwPacketLen ? m_dwPacketSize - dwPacketLen : 0;
			VO_U64 qwLastRead = m_pFileChunk->FGetLastRead();
			dwPayloadLen = static_cast<VO_U32>( (m_dwPacketSize ? m_dwPacketSize : dwPacketLen) - 
				(qwLastRead + dwPaddingLen + dwInternalPaddingLen) );
		}
		else
			read_by_type(dwPayloadLen, btPayloadLenType);

		//get media data at this position
		if(!OnPayload(btStreamNum, dwMediaObjOffset, dwPayloadLen, dwMediaObjSize, dwMediaObjTime))
			return VO_FALSE;

		if(dwInternalPaddingLen > 0)
			ptr_skip(dwInternalPaddingLen);
	}

	return VO_TRUE;
}

VO_BOOL CAsfDataParser::OnPayload(VO_U8 btStreamNum, VO_U32 dwPosInMediaObj, VO_U32 dwLen, VO_U32 dwMediaObjSize, VO_U32 dwTimeStamp)
{
	dwTimeStamp = (dwTimeStamp > m_ullTimeOffset) ? static_cast<VO_U32>(dwTimeStamp - m_ullTimeOffset) : 0;

	if((m_btBlockStream == (btStreamNum & 0x7F)) && (!m_fOBCallback || dwPosInMediaObj || m_fOBCallback(m_pOBUser, m_pOBUserData, &dwTimeStamp)))
	{
		ptr_skip(dwLen);
		return VO_TRUE;
	}

	AsfDataParserSample sample;
	sample.btStreamNum = btStreamNum;
	sample.dwPosInMediaObj = dwPosInMediaObj;
	sample.dwLen = dwLen;
	sample.dwMediaObjSize = dwMediaObjSize;
	sample.dwTimeStamp = dwTimeStamp;
	sample.pFileChunk = m_pFileChunk;
	VO_U8 btRes = m_fODCallback(m_pODUser, m_pODUserData, &sample);

	if(!CBRT_IS_CONTINUABLE(btRes))
		return VO_FALSE;

	if(CBRT_IS_NEEDSKIP(btRes))
		ptr_skip(dwLen);

	return VO_TRUE;
}

VO_PBYTE CAsfDataParser::GetIVDataByStreamNum(VO_S32 iStreamNum)
{
	return m_sIV[iStreamNum - 1];
}

VO_BOOL CAsfDataParser::GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos)
{
	return m_pFileChunk->FRead2(*ppBuffer, nSize, nPos);
}
CAsfIndexParser::CAsfIndexParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileBaseParser(pFileChunk, pMemOp)
	, m_pIndexEntries(NULL)
	, m_dwPacketSize(0)
	, m_ullIndexObjectsPos(0)
	, m_bReadedIndex(VO_FALSE)
{
}

CAsfIndexParser::~CAsfIndexParser()
{
	if (m_pIndexEntries)
		SAFE_DELETE(m_pIndexEntries);
}

VO_BOOL CAsfIndexParser::ReadFromFile(VO_U64 ullIndexObjectsPos, VO_U32 dwPacketSize)
{
	//delayed read index for streaming
	m_ullIndexObjectsPos = ullIndexObjectsPos;

	m_dwPacketSize =  dwPacketSize;

	return VO_TRUE;
}

VO_BOOL CAsfIndexParser::IsValid()
{
	if (!m_bReadedIndex && m_ullIndexObjectsPos)
		ReadFromFileB();
	
	return (m_pIndexEntries) ? VO_TRUE : VO_FALSE; 
}

VO_BOOL CAsfIndexParser::ReadFromFileB()
{
	m_bReadedIndex = VO_TRUE;

	if(!m_pFileChunk->FLocate(m_ullIndexObjectsPos))
		return VO_FALSE;

	AsfObject ao;
	while(true)
	{
		ptr_read_guid(ao.id);
		ptr_read_qword(ao.size);
		if(voIsEqualGUID(ao.id, ASF_Simple_Index_Object))
		{
			ptr_skip(sizeof(VO_GUID));

			VO_U64 ullTimeInterval = 0;
			ptr_read_qword(ullTimeInterval);

			ptr_skip(sizeof(VO_U32));

			VO_U32 uiEntries = 0;
			ptr_read_dword(uiEntries);

			if(uiEntries > 0)
			{
				if (m_pIndexEntries)
					SAFE_DELETE(m_pIndexEntries);

				m_pIndexEntries = new CAsfSampleIndexObjectEntries(m_pMemOp, m_dwPacketSize);
				if (!m_pIndexEntries)
					return VO_FALSE;
				else if (0 != m_pIndexEntries->SetEntriesNum(uiEntries))
					return VO_FALSE;

				m_pIndexEntries->m_ullTimeInterval = ullTimeInterval / 10000;//NS -> MS

				CAsfSampleIndexObjectEntries *pSampleIndexObjectEntries = (CAsfSampleIndexObjectEntries *)m_pIndexEntries;
				for(VO_U32 i = 0; i < m_pIndexEntries->m_dwEntries; i++)
				{
					ptr_read_dword(pSampleIndexObjectEntries->m_pEntries[i].packet_number);
					ptr_read_word(pSampleIndexObjectEntries->m_pEntries[i].packet_count);
				}
			}
		}
		else if(voIsEqualGUID(ao.id, ASF_Index_Object))
		{
			// skip_asf_object();
			VO_U32 ullTimeInterval = 0;
			ptr_read_dword(ullTimeInterval);

			VO_U16 siSpecifiersCount = 0;
			ptr_read_word(siSpecifiersCount);

			VO_U32 uiBlocksCount = 0;
			ptr_read_dword(uiBlocksCount);
			for(int j = 0;j<siSpecifiersCount;j++)
			{
				VO_U16 siStreamNum = 0;
				ptr_read_word(siStreamNum);
				VO_U16 siIndexType = 0;
				ptr_read_word(siIndexType);
			}
			VO_U32 uiEntries = 0;
			ptr_read_dword(uiEntries);
			uiEntries*=siSpecifiersCount;
			//need to modify after get file
			for (VO_U32 i = 0; i < siSpecifiersCount; i++)
			{
				VO_U64 qBlockPostions = 0;
				ptr_read_qword(qBlockPostions);
			}
			//---
			if(uiEntries > 0)
			{
				if (m_pIndexEntries)
					SAFE_DELETE(m_pIndexEntries);

				m_pIndexEntries = new CAsfIndexObjectEntries(m_pMemOp);
				if (!m_pIndexEntries)
					return VO_FALSE;
				else if (m_pIndexEntries->SetEntriesNum(uiEntries) != 0)
					return VO_FALSE;

				m_pIndexEntries->m_ullTimeInterval = ullTimeInterval;
				CAsfIndexObjectEntries *pIndexObjectEntries = (CAsfIndexObjectEntries *)m_pIndexEntries;
				for(VO_U32 i = 0; i < pIndexObjectEntries->m_dwEntries; i++)	
				{
					ptr_read_dword(pIndexObjectEntries->m_pEntries[i]);
				}
			}
		}
		else if(voIsEqualGUID(ao.id, ASF_Media_Object_Index_Object))
		{
			skip_asf_object();
		}
		else if(voIsEqualGUID(ao.id, ASF_Timecode_Index_Object))
		{
			skip_asf_object();
		}
		else
			return VO_FALSE;
	}

	return VO_TRUE;
}

VO_U64 CAsfIndexParser::GetPacketPosByTime(VO_S64 llTimeStamp)
{
	return m_pIndexEntries->GetPacketPosByTime(llTimeStamp);
}

VO_U64 CAsfIndexParser::GetPacketPosByIndex(VO_U32 dwIndex)
{
	return m_pIndexEntries->GetPacketPosByIndex(dwIndex);
}

VO_U32 CAsfIndexParser::GetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp)
{
	return m_pIndexEntries->GetIndexByTime(bForward, llTimeStamp);
}

VO_S64 CAsfIndexParser::GetPrevKeyFrameTime(VO_S64 llTimeStamp)
{
	return m_pIndexEntries->GetPrevKeyFrameTime(llTimeStamp);
}

VO_S64 CAsfIndexParser::GetNextKeyFrameTime(VO_S64 llTimeStamp)
{
	return m_pIndexEntries->GetNextKeyFrameTime(llTimeStamp);
}
