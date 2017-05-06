#include "CRealParser.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CRealHeaderParser::CRealHeaderParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileHeaderParser(pFileChunk, pMemOp)
	, m_nTrackInfoCount(0)
	, m_paTrackInfo(VO_NULL)
	, m_paIndexInfo(VO_NULL)
	, m_dwDuration(0)
	, m_dwDurationtmp(0)
	, m_dwMaxPacketSize(0)
	, m_dwMaxPacketSizetmp(0)
	, m_ullFileSize(0)
	, m_ullIndexObjectsFilePos(0)
	, m_ullMediaDataFilePos(0)
	, m_ullContentDescriptionHeaderFilePos(VO_MAXU64)
	, m_ullMetadataSectionFilePos(VO_MAXU64)
	, m_bLastheader(VO_FALSE)
	, m_dwfourcc_data(0)
	, m_dwfourcc_index(0)
	, m_ullMediaDataFilePostmp(0)
	, m_ullIndexObjectsFilePostmp(0)
	, m_dwMediapropertynum(0)
	, m_dwMediapropertynumtmp(0)
{
	for(VO_U8 i =0; i < 2; i++)
		m_paLogicalStreams[i] = VO_NULL;
}

CRealHeaderParser::~CRealHeaderParser()
{
	ReleaseTmpInfo();

	SAFE_RELEASE_CHAIN(RealFileIndexInfo, m_paIndexInfo);
}

VO_VOID CRealHeaderParser::ReleaseTmpInfo()
{
	if(m_paTrackInfo)
	{
		PRealTrackInfo pCur = m_paTrackInfo;
		PRealTrackInfo pDel;
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

	for(VO_U8 i =0; i < 2; i++)
	{
		if(m_paLogicalStreams[i])
		{
			SAFE_MEM_FREE(m_paLogicalStreams[i]->physical_stream_numbers);
			SAFE_MEM_FREE(m_paLogicalStreams[i]->data_offsets);

			MemFree(m_paLogicalStreams[i]);
			m_paLogicalStreams[i] = VO_NULL;
		}
	}
}

VO_U32 CRealHeaderParser::ProcessFileHeader(RMFFChunk& rc  , VO_U32& num_headers )
{
	use_big_endian_read

	ptr_read_fcc(rc.fcc);
	if(FOURCC_RMF != rc.fcc)
		return VO_RM_HEAD_FAILURE;

	ptr_read_dword(rc.size);

	RMFFFileHeader rfh;
	MemSet(&rfh, 0, sizeof(RMFFFileHeader));
	if(0x10 != rc.size)		
	{
		//has object version
		ptr_read_word(rfh.object_version);
	}

	ptr_read_dword(rfh.file_version);
	ptr_read_dword(rfh.num_headers);

	num_headers = rfh.num_headers;

	return VO_MAXU32;
}

VO_BOOL CRealHeaderParser::ReadFromFile()
{
	//RealMedia File Header must be the first chunk in the file
	VO_U32 num_headers = 0;
	RMFFChunk rc;
	memset( (void*)&rc, 0 , sizeof(RMFFChunk));
	VO_U32 ret = ProcessFileHeader(rc , num_headers );
	if( VO_MAXU32 != ret )
	{
		return VO_FALSE;
	}

	//all DATA section belong only 1 header!!
	//all INDX section belong only 1 header!!
	for(VO_U32 i = 0; i < num_headers; i++)
	{
		if( i == num_headers - 1)
		{
			m_bLastheader = VO_TRUE;
		}
		
		VO_BOOL bHeaderCompleted = VO_TRUE;
		VO_U32 nRes = ReadHeaderInfo(bHeaderCompleted);
		if(!bHeaderCompleted)
		{
			i--;
		}

		if(!nRes)
		{
			return VO_FALSE;
		}
		else if(VO_RM_HEAD_SUCCEND == nRes)
		{
			break;
		}
		else
		{
			continue;
		}
	}

	//we use one parameter to verify the other parameter, in order to make sure the rightness of stored parameter.
	HeadinfoVerify();


	if(m_pFileChunk->FRead(&rc.fcc, sizeof(VO_U32)) && FOURCC_RMMD == rc.fcc)
	{
		//it will be used in metadata routine
		//There is one metadata section in a RealMedia file and it is expected to be at the end of the file.
		m_ullMetadataSectionFilePos = m_pFileChunk->FGetFilePos() - 4;
	}

	return VO_TRUE;
}

//we use one parameter to verify the other parameter, in order to make sure the rightness of stored parameter.
VO_VOID	CRealHeaderParser::HeadinfoVerify()
{
	//if the property data_offset value is different with FOURCC_DATA identified, and we only found one FOURCC_DATA chunk,
	//then we should try to correct it
	if( m_ullMediaDataFilePos != m_ullMediaDataFilePostmp 
		&& m_dwfourcc_data == 1
		&& m_ullMediaDataFilePos != 0 
		&& m_ullMediaDataFilePostmp != 0)
	{
		m_ullMediaDataFilePos = m_ullMediaDataFilePostmp;
	}

	//same for index_offset
	if( m_ullIndexObjectsFilePos != m_ullIndexObjectsFilePostmp 
		&& m_dwfourcc_index == 1
		&& m_ullIndexObjectsFilePos != 0 
		&& m_ullIndexObjectsFilePostmp != 0)
	{
		m_ullIndexObjectsFilePos = m_ullIndexObjectsFilePostmp;
	}

	//same for  number of streams
	if( m_dwMediapropertynumtmp != 0 
		&& m_dwMediapropertynum != 0
		&& m_dwMediapropertynumtmp != m_dwMediapropertynum )
	{
		m_dwMediapropertynum = m_dwMediapropertynumtmp;
	}

	//same for max packet size
	if( m_dwMaxPacketSizetmp != 0
		&& m_dwMaxPacketSize != 0
		&& m_dwMaxPacketSizetmp > m_dwMaxPacketSize )
	{
		m_dwMaxPacketSize = m_dwMaxPacketSizetmp;
	}

	//same for duration
	if( m_dwDurationtmp != 0
		&& m_dwDuration != 0
		&& m_dwDurationtmp > m_dwDuration )
	{
		m_dwDuration = m_dwDurationtmp;
	}
}


VO_U32	CRealHeaderParser::ProcessFileproperties()
{
	use_big_endian_read

	RMFFProperties rp;
	ptr_read_word(rp.object_version);
	if(0 == rp.object_version)
	{
		ptr_read_dword(rp.max_bit_rate);
		ptr_read_dword(rp.avg_bit_rate);
		ptr_read_dword(rp.max_packet_size);
		ptr_read_dword(rp.avg_packet_size);
		ptr_read_dword(rp.num_packets);
		ptr_read_dword(rp.duration);
		ptr_read_dword(rp.preroll);
		ptr_read_dword(rp.index_offset);
		ptr_read_dword(rp.data_offset);
		ptr_read_word(rp.num_streams);
		ptr_read_word(rp.flags);

		m_dwDuration = rp.duration;
		m_dwMaxPacketSize = rp.max_packet_size;
		m_ullIndexObjectsFilePos = rp.index_offset;
		m_ullMediaDataFilePos = rp.data_offset;
		m_dwMediapropertynum = rp.num_streams;

		if( 0 == rp.index_offset )
		{
			VOLOGI(" no index chunk are present ");
		}

		if( rp.flags & 0x01 )
		{
			VOLOGI(" Save_Enabled ");
		}

		if( rp.flags & 0x02 )
		{
			VOLOGI(" Perfect_Play ");
		}

		if( rp.flags & 0x04 )
		{
			VOLOGI(" Live ");
		}
	}
	else
	{
		VOLOGI("real file object_version : %d" , rp.object_version );
	}

	return VO_RM_HEAD_SUCCFUL;
}

VO_U32 CRealHeaderParser::ProcessMediaproperties()
{
	use_big_endian_read

	m_dwMediapropertynumtmp++;

	RMFFMediaProperties rmp;
	ptr_read_word(rmp.object_version);
	if(0 == rmp.object_version)
	{
		ptr_read_word(rmp.stream_number);
		ptr_read_dword(rmp.max_bit_rate);
		ptr_read_dword(rmp.avg_bit_rate);

		ptr_read_dword(rmp.max_packet_size);
		if( rmp.max_packet_size > m_dwMaxPacketSizetmp )
		{
			m_dwMaxPacketSizetmp = rmp.max_packet_size;
		}

		ptr_read_dword(rmp.avg_packet_size);
		ptr_read_dword(rmp.start_time);
		ptr_read_dword(rmp.preroll);
		ptr_read_dword(rmp.duration);

		if( rmp.duration > m_dwDurationtmp )
		{
			m_dwDurationtmp = rmp.duration;
		}

		ptr_read_byte(rmp.stream_name_size);
		//skip stream name!!
		ptr_skip(rmp.stream_name_size);

		RM_MEDIA_MIME_TYPES nType = RMMT_UNKNOWN;
		ptr_read_byte(rmp.mime_type_size);
		rmp.mime_type = NEW_BUFFER(rmp.mime_type_size + 1);
		ptr_read_pointer(rmp.mime_type, rmp.mime_type_size);
		rmp.mime_type[rmp.mime_type_size] = 0;
		if(!strcmp((const char*)rmp.mime_type, MIME_TYPE_RA) || 
			!strcmp((const char*)rmp.mime_type, MIME_TYPE_RA_ENCRYPTED))
			nType = RMMT_AUDIO;
		else if(!strcmp((const char*)rmp.mime_type, MIME_TYPE_RV) || 
			!strcmp((const char*)rmp.mime_type, MIME_TYPE_RV_ENCRYPTED))
			nType = RMMT_VIDEO;
		else if(!strcmp((const char*)rmp.mime_type, MIME_TYPE_RLA))
			nType = RMMT_LOGICAL_AUDIO;
		else if(!strcmp((const char*)rmp.mime_type, MIME_TYPE_RLV))
			nType = RMMT_LOGICAL_VIDEO;
		else if(!strcmp((const char*)rmp.mime_type, MIME_TYPE_RLA_SUB))
			nType = RMMT_LOGICAL_AUDIO_SUB;
		else if(!strcmp((const char*)rmp.mime_type, MIME_TYPE_RLV_SUB))
			nType = RMMT_LOGICAL_VIDEO_SUB;
		MemFree(rmp.mime_type);

		ptr_read_dword(rmp.type_specific_len);

		switch(nType)
		{
		case RMMT_AUDIO:		//audio
			//for test purpose, when audio codec is not support, then we can avoid to call it.  issue 11250
#if 0
			{
				ptr_skip( rmp.type_specific_len );
			}
			break;
#endif
		case RMMT_VIDEO:		//video
			{
				PRealTrackInfo pNewInfo = NEW_OBJ(RealTrackInfo);
				if(!pNewInfo)
					return VO_RM_HEAD_FAILURE;

				pNewInfo->nType = nType;
				pNewInfo->wStreamNum = rmp.stream_number;
				pNewInfo->dwDuration = rmp.duration;
				//b/s -> B/s
				pNewInfo->dwBitrate = ((rmp.avg_bit_rate / 8) > 1024*1024 ? 1024 * 1024 : (rmp.avg_bit_rate / 8));	
				pNewInfo->dwMaxPacketSize = rmp.max_packet_size;

				pNewInfo->dwPropBufferSize = rmp.type_specific_len;
				pNewInfo->pPropBuffer = VO_NULL;
				pNewInfo->next = VO_NULL;
				pNewInfo->dwDataOffset = 0;

				ADD_TO_CHAIN(RealTrackInfo, m_paTrackInfo, pNewInfo);
				m_nTrackInfoCount++;

				pNewInfo->pPropBuffer = NEW_BUFFER(pNewInfo->dwPropBufferSize);
				if(pNewInfo->pPropBuffer)
				{
					//type_specific_data
					ptr_read_pointer(pNewInfo->pPropBuffer, pNewInfo->dwPropBufferSize);
				}
				else
				{
					ptr_skip(pNewInfo->dwPropBufferSize);
				}
			}
			break;

		case RMMT_LOGICAL_AUDIO:		//audio logical stream, 0 or 1, must ahead of RMMT_LOGICAL_AUDIO_SUB
		case RMMT_LOGICAL_VIDEO:		//video logical stream, 0 or 1, must ahead of RMMT_LOGICAL_VIDEO_SUB
			{
				VO_U8 i = RMMT_IS_AUDIO(nType) ? 0 : 1;

				if(!m_paLogicalStreams[i])
				{
					PRealLogicalStream pNewLS = NEW_OBJ(RealLogicalStream);
					if(!pNewLS)
					{
						return VO_RM_HEAD_FAILURE;
					}

					m_paLogicalStreams[i] = pNewLS;
					pNewLS->duration = rmp.duration;

					VO_S32 nLeft = rmp.type_specific_len;

					//skip size, because it is same as rmp.type_specific_len!!
					ptr_skip(4);
					VO_U16 wObjectVersion = 0;
					ptr_read_word(wObjectVersion);
					nLeft -= 6;

					if(0 == wObjectVersion)
					{
						ptr_read_word(pNewLS->num_physical_streams);
						if( pNewLS->num_physical_streams <= 0)
						{
							//make sure the number of physical streams is valid
							return VO_RM_HEAD_FAILURE;
						}

						pNewLS->physical_stream_numbers = NEW_OBJS(VO_U16, pNewLS->num_physical_streams);
						if(!pNewLS->physical_stream_numbers)
						{
							return VO_RM_HEAD_FAILURE;
						}

						pNewLS->data_offsets = NEW_OBJS(VO_U32, pNewLS->num_physical_streams);
						if(!pNewLS->data_offsets)
						{
							return VO_RM_HEAD_FAILURE;
						}

						for(VO_U16 i = 0; i < pNewLS->num_physical_streams; i++)
						{
							ptr_read_word(pNewLS->physical_stream_numbers[i]);
						}

						for(VO_U16 i = 0; i < pNewLS->num_physical_streams; i++)
						{
							ptr_read_dword(pNewLS->data_offsets[i]);
						}

						nLeft -= ( 2 + pNewLS->num_physical_streams * ( 2 + 4 ) );
					}

					if(nLeft > 0)
					{
						//ASM rule and properties has been skiped
						//the logical stream's language properties maybe contained in it.
						ptr_skip(nLeft);
					}
				}
				else
				{
					ptr_skip(rmp.type_specific_len);
				}
			}
			break;

		case RMMT_LOGICAL_AUDIO_SUB:		//actual audio of logical stream, 0 - ...
		case RMMT_LOGICAL_VIDEO_SUB:		//actual video of logical stream, 0 - ...
			{
				VO_U8 i = RMMT_IS_AUDIO(nType) ? 0 : 1;

				//the logical stream must be ahead of sub
				if(m_paLogicalStreams[i])
				{
					PRealTrackInfo pNewInfo = NEW_OBJ(RealTrackInfo);
					if(!pNewInfo)
					{
						return VO_RM_HEAD_FAILURE;
					}

					pNewInfo->nType = nType;
					pNewInfo->wStreamNum = rmp.stream_number;
					pNewInfo->dwDuration = (rmp.duration > 0) ? rmp.duration : m_paLogicalStreams[i]->duration;
					pNewInfo->dwBitrate = rmp.avg_bit_rate / 8;	//b/s -> B/s
					pNewInfo->dwMaxPacketSize = rmp.max_packet_size;
					pNewInfo->dwPropBufferSize = rmp.type_specific_len;
					pNewInfo->pPropBuffer = VO_NULL;
					pNewInfo->next = VO_NULL;

					for(VO_U16 j = 0; j < m_paLogicalStreams[i]->num_physical_streams; j++)
					{
						if(m_paLogicalStreams[i]->physical_stream_numbers[j] == rmp.stream_number)
						{
							pNewInfo->dwDataOffset = m_paLogicalStreams[i]->data_offsets[j];
							break;
						}
					}

					ADD_TO_CHAIN(RealTrackInfo, m_paTrackInfo, pNewInfo);
					m_nTrackInfoCount++;

					pNewInfo->pPropBuffer = NEW_BUFFER(pNewInfo->dwPropBufferSize);
					if(pNewInfo->pPropBuffer)
					{
						ptr_read_pointer(pNewInfo->pPropBuffer, pNewInfo->dwPropBufferSize);
					}
					else
					{
						ptr_skip(pNewInfo->dwPropBufferSize);
					}
				}
				else
				{
					ptr_skip(rmp.type_specific_len);
				}
			}
			break;

		default:	//other
			{
				ptr_skip(rmp.type_specific_len);
			}
			break;
		}
	}
	return VO_RM_HEAD_SUCCFUL;
}

VO_U32 CRealHeaderParser::ProcessBaseChunk(RMFFChunk& rc)
{
	use_big_endian_read

	//VO_U64 filepos_1 = m_pFileChunk->FGetFilePos();
	//check if already reach end of file
	if(m_ullFileSize > 0 && m_ullFileSize != (VO_U64)-1 
		&& m_pFileChunk->FGetFilePos() >= m_ullFileSize 
		&& m_bLastheader)
	{
		return VO_RM_HEAD_SUCCFUL;
	}

	ptr_read_fcc(rc.fcc);
	ptr_read_dword(rc.size);

	//stony add for dealing with padded zero data
	if( (rc.size == 0 || rc.fcc == 0) && m_bLastheader)
	{
		return VO_RM_HEAD_SUCCFUL;
	}

	if(rc.size < 8)
	{
		return VO_RM_HEAD_FAILURE;
	}

	return VO_MAXU32;
}

VO_U32 CRealHeaderParser::ProcessDataChunkHeader(RMFFChunk rc ,VO_BOOL& bHeaderCompleted)
{
	use_big_endian_read

	m_dwfourcc_data = m_dwfourcc_data + 1;
	m_ullMediaDataFilePostmp = m_pFileChunk->FGetFilePos() - 8;

	RMFFDataChunkHeader rdch;
	ptr_read_word(rdch.object_version);
	if(0 == rdch.object_version)
	{
		ptr_read_dword(rdch.num_packets);
		ptr_read_dword(rdch.next_data_header);
		if(rdch.next_data_header > 0)
		{
			bHeaderCompleted = VO_FALSE;
		}
	}

	if(m_ullFileSize < m_ullMediaDataFilePostmp + rc.size)
	{
		rc.size = (VO_U32)(m_ullFileSize - m_ullMediaDataFilePostmp);

		ptr_skip(rc.size - 18);
		return VO_RM_HEAD_SUCCEND;
	}
	else
	{
		ptr_skip(rc.size - 18);
		return VO_RM_HEAD_SUCCFUL;
	}
}

VO_U32 CRealHeaderParser::ProcessINDXChunkHeader(VO_BOOL& bHeaderCompleted)
{
	use_big_endian_read

	m_dwfourcc_index++;
	m_ullIndexObjectsFilePostmp = m_pFileChunk->FGetFilePos() - 8;

	RMFFIndexSectionHeader rish;
	ptr_read_word(rish.object_version);
	if(0 == rish.object_version)
	{
		ptr_read_dword(rish.num_indices);
		ptr_read_word(rish.stream_number);
		ptr_read_dword(rish.next_index_header);
		if(rish.next_index_header > 0)
		{
			bHeaderCompleted = VO_FALSE;
		}
	}

	PRealFileIndexInfo pNewInfo = NEW_OBJ(RealFileIndexInfo);
	if(!pNewInfo)
	{
		return VO_RM_HEAD_FAILURE;
	}

	pNewInfo->num_indices = rish.num_indices;
	pNewInfo->stream_number = rish.stream_number;
	pNewInfo->file_position = m_pFileChunk->FGetFilePos();
	pNewInfo->next = VO_NULL;

	ADD_TO_CHAIN(RealFileIndexInfo, m_paIndexInfo, pNewInfo);

	ptr_skip(rish.num_indices * 14);

	return VO_RM_HEAD_SUCCFUL;
}

//Content Description Header
//it has been implemented in metadata routine
VO_U32 CRealHeaderParser::ProcessContentDescription()
{
	use_big_endian_read

	RMFFContentDescription rfcd;
	memset( (void*)&rfcd , 0 , sizeof(rfcd) );
	ptr_read_word( rfcd.object_version );
	if( 0 == rfcd.object_version )
	{
		ptr_read_word( rfcd.title_len );
		if( rfcd.title_len <= 0 )
		{
			return VO_RM_HEAD_FAILURE;
		}

		rfcd.title = NEW_BUFFER( rfcd.title_len );
		if( rfcd.title )
		{
			ptr_read_pointer(rfcd.title, rfcd.title_len);
		}
		else
		{
			return VO_RM_HEAD_FAILURE;
		}

		ptr_read_word( rfcd.author_len );
		if( rfcd.author_len <= 0 )
		{
			return VO_RM_HEAD_FAILURE;
		}

		rfcd.author = NEW_BUFFER( rfcd.author_len );
		if( rfcd.author )
		{
			ptr_read_pointer(rfcd.author, rfcd.author_len);
		}
		else
		{
			return VO_RM_HEAD_FAILURE;
		}


		ptr_read_word( rfcd.copyright_len );
		if( rfcd.copyright_len <= 0 )
		{
			return VO_RM_HEAD_FAILURE;
		}

		rfcd.copyright = NEW_BUFFER( rfcd.copyright_len );
		if( rfcd.copyright )
		{
			ptr_read_pointer(rfcd.copyright, rfcd.copyright_len);
		}
		else
		{
			return VO_RM_HEAD_FAILURE;
		}
		
				
		ptr_read_word( rfcd.comment_len );
		if( rfcd.comment_len <= 0 )
		{
			return VO_RM_HEAD_FAILURE;
		}

		rfcd.comment = NEW_BUFFER( rfcd.comment_len );
		if( rfcd.comment )
		{
			ptr_read_pointer(rfcd.comment, rfcd.comment_len);
		}
		else
		{
			return VO_RM_HEAD_FAILURE;
		}
	}

	return VO_RM_HEAD_SUCCFUL;
}

VO_U32 CRealHeaderParser::ReadHeaderInfo(VO_BOOL& bHeaderCompleted)
{
	RMFFChunk rc;
	memset( (void*)&rc , 0 , sizeof(rc) );
	VO_U32 ret = ProcessBaseChunk(rc);
	if( VO_MAXU32 != ret)
	{
		return ret;
	}

	switch(rc.fcc)
	{
	case FOURCC_PROP:
		{
			return ProcessFileproperties();
		}
		break;

	case FOURCC_MDPR:
		{
			return ProcessMediaproperties();
		}
		break;

	case FOURCC_DATA:
		{
			return ProcessDataChunkHeader(rc ,bHeaderCompleted);
		}
		break;

	case FOURCC_INDX:
		{
			return ProcessINDXChunkHeader(bHeaderCompleted);
		}
		break;

	case FOURCC_CONT:
		{
			//The Content Description Header contains the title, author, copyright, and comments information for
			//the RealMedia file.

			m_ullContentDescriptionHeaderFilePos = m_pFileChunk->FGetFilePos();
			//ProcessContentDescription();
			ptr_skip(rc.size - 8);
			return VO_RM_HEAD_SUCCFUL;
		}
		break;

	default:
		{
			//stony: if it is last header, and the id type is unknown, we should not trust the size value.
			if(!m_bLastheader)
			{
				ptr_skip(rc.size - 8);
			}
			
			return VO_RM_HEAD_SUCCFUL;
		}
		break;
	}

	return VO_RM_HEAD_FAILURE;
}

CRealDataParser::CRealDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp)
	: CvoFileDataParser(pFileChunk, pMemOp)
	, m_IsThumbNail(VO_FALSE)
	, m_ThumbnailScanCount(10)
	, m_nExistThumnnailCnt(0)
	, m_formerIskeyframe(VO_FALSE)
	, m_packetsize_sum(0)
	, m_pIframenode(VO_NULL)
	, m_thumbnailstep_limitsize(0)
{
	list_init_ptr(&m_Iframe_index_headnode);
}

CRealDataParser::~CRealDataParser()
{
	RelIframeIndex();
}


VO_BOOL	CRealDataParser::GetMaxSizeIframeIndex(VO_U64& llpos, VO_S32& limitsize)
{
	llpos = 0;
	limitsize = 0;
	list_node *p = NULL;
	RealIframeIndexInfo* pnode = NULL;
	vo_list_for_each(p,&m_Iframe_index_headnode)
	{
		pnode = vo_list_entry(p,struct tagRealIframeIndexInfo,i_list);
		if(!pnode)
		{
			continue;
		}
		else
		{
			if( pnode->lestimate_framesize > limitsize )
			{
				llpos = pnode->llfilepos;
				limitsize = pnode->lestimate_framesize;
			}
		}
	}

	if( !llpos )
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_VOID	CRealDataParser::RelIframeIndex()
{
	list_node *p = NULL;
	list_node *pn = NULL;
	RealIframeIndexInfo* pnode = NULL;
	vo_list_for_each(p,&m_Iframe_index_headnode)
	{
		pnode = vo_list_entry(p,struct tagRealIframeIndexInfo,i_list);
		if(!pnode)
		{
			continue;
		}
		else
		{
			pn = p->next;
			vo_list_del(p);
			delete pnode;
			p = pn->prev;
		}
	}
}

VO_VOID CRealDataParser::Init(VO_PTR pParam)
{
//	list_init_ptr(&m_Iframe_index_headnode);
}

VO_BOOL	CRealDataParser::SetFileIOPos(VO_U64 ullStartFilePos)
{
	if(!CvoFileDataParser::SetStartFilePos(ullStartFilePos))
	{
		return VO_FALSE;
	}
	return VO_TRUE;
}

VO_BOOL CRealDataParser::SetStartFilePos(VO_U64 ullStartFilePos, VO_BOOL bInclHeader /* = VO_FALSE */)
{
	if(!CvoFileDataParser::SetStartFilePos(ullStartFilePos, bInclHeader))
	{
		return VO_FALSE;
	}

	if(!bInclHeader)
	{
		return VO_TRUE;
	}

	use_big_endian_read

	VO_U32 fcc;
	ptr_read_fcc(fcc);
	if(FOURCC_DATA != fcc)
	{
		return VO_FALSE;
	}

	//The size of the Data Chunk in bytes.
	VO_U32 size=0;
	ptr_read_dword(size);

	VO_U16 wVersion = 0;
	ptr_read_word(wVersion);
	if(0 == wVersion)
	{
		//Number of packets in the data chunk.
		VO_U32 num_packets=0;
		ptr_read_dword(num_packets);

		//next_data_header
		VO_U32 next_data_header=0;
		ptr_read_dword(next_data_header);
	}

	return VO_TRUE;
}

VO_VOID	CRealDataParser::SetStepBRangeForthumbnail( VO_U32 llimitsize)
{
	m_packetsize_sum = 0;
	m_thumbnailstep_limitsize = llimitsize;
	
	//we should get sample data now, so enable process sample data
	m_IsThumbNail = VO_FALSE;
}

VO_BOOL CRealDataParser::StepB()
{
	use_big_endian_read

	RMFFMediaPacketHeader rmph;
	ptr_read_word(rmph.object_version);
	if(0 == rmph.object_version || 1 == rmph.object_version)
	{
		ptr_read_word(rmph.length);
		ptr_read_word(rmph.stream_number);
		ptr_read_dword(rmph.time_stamp);

		VO_U16 wPacketDataSize = rmph.length;
		VO_U8 btFlags = 0;
		if(0 == rmph.object_version)
		{
			ptr_read_byte(rmph.packet_group);
			ptr_read_byte(rmph.flags);
			btFlags = rmph.flags | ((HX_KEYFRAME_FLAG & rmph.flags) ? RM_KEYFRAME_FLAG : 0);
			wPacketDataSize -= RM_PACKET_HEADER_SIZE;
		}
		else if(1 == rmph.object_version)
		{
			ptr_read_word(rmph.asm_rule);
			ptr_read_byte(rmph.asm_flags);
			btFlags = rmph.asm_flags | ((HX_KEYFRAME_RULE == rmph.asm_rule) ? RM_KEYFRAME_FLAG : 0);
			wPacketDataSize -= (RM_PACKET_HEADER_SIZE + 1);
		}

		if(wPacketDataSize == 0)
		{
			return VO_TRUE;
		}

		if((m_btBlockStream == rmph.stream_number) && (!m_fOBCallback || m_fOBCallback(m_pOBUser, m_pOBUserData, &rmph.time_stamp)))
		{
			ptr_skip(wPacketDataSize);

			return VO_TRUE;
		}

		RealDataParserSample sample;
		sample.btStreamNum = (VO_U8)rmph.stream_number;
		sample.btFlags = btFlags;
		sample.wPacketLen = wPacketDataSize;
		sample.dwTimeStamp = rmph.time_stamp;
		sample.pFileChunk = m_pFileChunk;

		//if it is for thumbnail purpose, we should not read any media sample data,but just the I frame index info.
		if(m_IsThumbNail)
		{
			//if it is key frame, we should record the info for later usage.
			if( btFlags & RM_KEYFRAME_FLAG )
			{
				//we won't record several consequent key frame packet, since it maybe mean one key frame separate to several packet.
				if( !m_formerIskeyframe )
				{
					m_formerIskeyframe = VO_TRUE;
					m_pIframenode = VO_NULL;
					m_pIframenode = new RealIframeIndexInfo;
					if(!m_pIframenode)
					{
						return VO_FALSE;
					}

					//for the convenience of processing, we adjust the filepos to right begin of data packet
					if(0 == rmph.object_version)
					{
						m_pIframenode->llfilepos = m_pFileChunk->FGetFilePos() - RM_PACKET_HEADER_SIZE;
					}
					else
					if(1 == rmph.object_version)
					{
						m_pIframenode->llfilepos = m_pFileChunk->FGetFilePos() - RM_PACKET_HEADER_SIZE - 1;
					}

					m_packetsize_sum = wPacketDataSize;
				}
				else
				{
					//it is key frame packet,but not first key frame packet, so sum the packet size.
					m_packetsize_sum += wPacketDataSize;
				}
			}
			else	//not key frame packet
			{
				if( m_formerIskeyframe && m_pIframenode )
				{
					m_pIframenode->lestimate_framesize = m_packetsize_sum;
					vo_list_add_tail(&m_pIframenode->i_list,&m_Iframe_index_headnode);
					if(++m_nExistThumnnailCnt > m_ThumbnailScanCount)
					{
						SetParseEnd(VO_TRUE);
					}
				}
				m_packetsize_sum = 0;
				m_formerIskeyframe = VO_FALSE;
			}
			
			//we should not read any media data to reduce I/O 
			ptr_skip(wPacketDataSize);
		}
		else
		{
			//it is for thumbnail purpose, if data parser had read the limited size of data, we should exit routine
			if( m_thumbnailstep_limitsize > 0 )
			{
				m_packetsize_sum += wPacketDataSize;
				if( m_packetsize_sum >= m_thumbnailstep_limitsize )
				{
					SetParseEnd(VO_TRUE);
				}
			}

			VO_U8 btRes = m_fODCallback(m_pODUser, m_pODUserData, &sample);

			if(!CBRT_IS_CONTINUABLE(btRes))
				return VO_FALSE;

			if(CBRT_IS_NEEDSKIP(btRes))
				ptr_skip(wPacketDataSize);
		}

		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_VOID	CRealDataParser::SetThumbnailFlag(VO_BOOL bThumbnail)
{
	m_IsThumbNail = bThumbnail;
}

VO_VOID	CRealDataParser::SetThumbnailScanCount( VO_S32 cnt )
{
	m_ThumbnailScanCount = cnt;
}