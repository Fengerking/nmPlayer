#include "CRealMetaData.h"
#include "RMFileDataStruct.h"
#include "fMacros.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define read_string_from_file(p_str, buf_len)\
{\
	if(p_str)\
	{\
		if(strlen(p_str) == 0)\
		{\
			MemFree(p_str);\
			p_str = 0;\
		}\
		else\
			ptr_skip(buf_len);\
	}\
	if(!p_str)\
	{\
		p_str = (VO_PCHAR)NEW_BUFFER(buf_len + 1);\
		if(!p_str)\
			return VO_FALSE;\
		ptr_read_pointer(p_str, buf_len);\
		p_str[buf_len] = 0;\
	}\
}

#define copy_string(p_str, p_src)\
{\
	if(!p_str)\
	{\
		p_str = (VO_PCHAR)NEW_BUFFER(strlen(p_src) + 1);\
		if(p_str)\
			MemCopy(p_str, p_src, strlen(p_src) + 1);\
	}\
}

#define REALSTRING2METADATASTRING(as, mds)\
	if(as)\
	{\
		VO_METADATA_SETTE(mds->dwFlags, VO_METADATA_TE_ANSI);\
		mds->dwBufferSize = strlen(as) + 1;\
		mds->pBuffer = (VO_PBYTE)as;\
		return VO_ERR_METADATA_OK;\
	}

#define GET_ID3V1_TEXT(as, mds)\
	VO_METADATA_SETTE(mds->dwFlags, VO_METADATA_TE_ANSI);\
	mds->dwBufferSize = sizeof(as);\
	mds->pBuffer = (VO_PBYTE)as;

const VO_CHAR g_szRMTrack[] = "CD Track #";
const VO_CHAR g_szRMYear[] = "Year";
const VO_CHAR g_szRMCategory[] = "Category";
const VO_CHAR g_szRMCDTOC[] = "CD TOC";
const VO_CHAR g_szRMRating[] = "Rating";
const VO_CHAR g_szRMSubscription[] = "Subscription";

CAutoBuffer::CAutoBuffer(VO_MEM_OPERATOR* pMemOp)
	: CvoBaseMemOpr(pMemOp)
	, m_pBuffer(VO_NULL)
{
}

CAutoBuffer::~CAutoBuffer()
{
	if(m_pBuffer)
		MemFree(m_pBuffer);
}

CRealMetaData::CRealMetaData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp)
	: CBaseMetaData(pFileOp, pMemOp)
	, m_pTitle(VO_NULL)
	, m_pArtist(VO_NULL)
	, m_pCopyright(VO_NULL)
	, m_pComment(VO_NULL)
	, m_pTrack(VO_NULL)
	, m_pYear(VO_NULL)
	, m_pCDTOC(VO_NULL)
	, m_pID3v1_1Info(VO_NULL)
{
}

CRealMetaData::~CRealMetaData()
{
	Close();
}

VO_U32 CRealMetaData::Close()
{
	SAFE_MEM_FREE(m_pTitle);
	SAFE_MEM_FREE(m_pArtist);
	SAFE_MEM_FREE(m_pCopyright);
	SAFE_MEM_FREE(m_pComment);

	SAFE_MEM_FREE(m_pTrack);
	SAFE_MEM_FREE(m_pYear);
	SAFE_MEM_FREE(m_pCDTOC);

	SAFE_MEM_FREE(m_pID3v1_1Info);

	return CBaseMetaData::Close();
}

VO_U32 CRealMetaData::ReadMetaDataB()
{
	return Parse() ? VO_ERR_METADATA_OK : VO_ERR_METADATA_NOMETADATA;
}

VO_U32 CRealMetaData::ReadMetaDataB(PMetaDataInitInfo pInitInfo)
{
	if(VO_MAXU64 == pInitInfo->ullFilePositions[0] && 
		VO_MAXU64 == pInitInfo->ullFilePositions[1])
		return ReadMetaDataB();

	if(VO_MAXU64 != pInitInfo->ullFilePositions[0])
	{
		if(!m_pFileChunk->FLocate(pInitInfo->ullFilePositions[0]))
			return VO_ERR_METADATA_NOMETADATA;

		if(!Parse_Content_Description_Header())
			return VO_ERR_METADATA_NOMETADATA;
	}

	if(VO_MAXU64 != pInitInfo->ullFilePositions[1])
	{
		if(!m_pFileChunk->FLocate(pInitInfo->ullFilePositions[1]))
			return VO_ERR_METADATA_NOMETADATA;

		if(!Parse_Metadata_Section())
			return VO_ERR_METADATA_NOMETADATA;
	}

	return VO_ERR_METADATA_OK;
}

VO_BOOL CRealMetaData::Parse()
{
	use_big_endian_read

	RMFFChunk rc;
	ptr_read_fcc(rc.fcc);
	if(FOURCC_RMF != rc.fcc)
		return VO_FALSE;

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

	for(VO_U32 i = 0; i < rfh.num_headers; i++)
	{
		VO_BOOL bHeaderCompleted = VO_TRUE;
		VO_S32 nRes = ReadHeaderInfo(bHeaderCompleted);
		if(!bHeaderCompleted)
			i--;

		if(!nRes)
			return VO_FALSE;
		else if(2 == nRes)
			break;
	}

	Parse_Metadata_Section();

	return VO_TRUE;
}

VO_S32 CRealMetaData::ReadHeaderInfo(VO_BOOL& bHeaderCompleted)
{
	use_big_endian_read

	RMFFChunk rc;
	ptr_read_fcc(rc.fcc);
	ptr_read_dword(rc.size);
	if(rc.size < 8)
		return 0;

	switch(rc.fcc)
	{
	case FOURCC_CONT:
		{
			Parse_Content_Description_Header();
			return 1;
		}
		break;

	default:
		{
			ptr_skip(rc.size - 8);
			return 1;
		}
		break;
	}

	return 0;
}

VO_BOOL CRealMetaData::Parse_Content_Description_Header()
{
	use_big_endian_read

	VO_U16 wValue;
	ptr_read_word(wValue);
	if(0 == wValue)
	{
		ptr_read_word(wValue);
		if(wValue > 0) {
			read_string_from_file(m_pTitle, wValue);
		}

		ptr_read_word(wValue);
		if(wValue > 0) {
			read_string_from_file(m_pArtist, wValue);
		}

		ptr_read_word(wValue);
		if(wValue > 0) {
			read_string_from_file(m_pCopyright, wValue);
		}

		ptr_read_word(wValue);
		if(wValue > 0) {
			read_string_from_file(m_pComment, wValue);
		}
	}

	return VO_TRUE;
}

VO_BOOL CRealMetaData::Parse_Metadata_Section()
{
	use_big_endian_read

	//metadata section header
	RMFFChunk rc;
	ptr_read_fcc(rc.fcc);
	if(FOURCC_RMMD != rc.fcc)
	{
		return VO_FALSE;
	}
	ptr_read_dword(rc.size);

	//metadata tag
	RMFFMetaDataTag mdt;
	ptr_read_fcc(mdt.object_id);
	if(FOURCC_RJMD != mdt.object_id)
	{
		return VO_FALSE;
	}
	ptr_read_dword(mdt.object_version);

	if(!Parse_Metadata_Property())
		return VO_FALSE;

	//Metadata Section Footer
	RMFFMetaSectionFooter msf;
	ptr_read_fcc(msf.object_id);
	if(FOURCC_RMJE != msf.object_id)
	{
		return VO_FALSE;
	}
	ptr_read_dword(msf.object_version);
	ptr_read_dword(msf.size);

	//ID3V1 tag
	if(m_pID3v1_1Info)
	{
		MemFree(m_pID3v1_1Info);
	}
	m_pID3v1_1Info = NEW_OBJ(ID3v1_1Info);
	if(!m_pID3v1_1Info)
	{
		return VO_FALSE;
	}

	ptr_read_pointer(m_pID3v1_1Info, ID3V1_INFO_LEN);
	if(MemCompare(m_pID3v1_1Info->szTag, (VO_PTR)"TAG", 3))
	{
		SAFE_MEM_FREE(m_pID3v1_1Info);
	}

	return VO_TRUE;
}

VO_BOOL CRealMetaData::Parse_Metadata_Property()
{
	use_big_endian_read

	RMFFMetaDataProperty mdp;

	CAutoBuffer abName(m_pMemOp), abValue(m_pMemOp);

	ptr_read_dword(mdp.size);
	ptr_read_dword(mdp.type);
	ptr_read_dword(mdp.flags);
	ptr_read_dword(mdp.value_offset);
	ptr_read_dword(mdp.sub_properties_offset);
	ptr_read_dword(mdp.num_sub_properties);
	ptr_read_dword(mdp.name_length);
	if(mdp.name_length > 0)
	{
		if(mdp.name_length == 1)
		{
			ptr_skip(mdp.name_length);
		}
		else
		{
			abName.m_pBuffer = NEW_BUFFER(mdp.name_length);
			if(!abName.m_pBuffer)
			{
				return VO_FALSE;
			}
			ptr_read_pointer(abName.m_pBuffer, mdp.name_length);
		}
	}

	ptr_read_dword(mdp.value_length);
	if(mdp.value_length > 0)
	{
		abValue.m_pBuffer = NEW_BUFFER(mdp.value_length);
		if(!abValue.m_pBuffer)
		{
			return VO_FALSE;
		}
		ptr_read_pointer(abValue.m_pBuffer, mdp.value_length);
	}

	Parse_Metadata_Property_Base((VO_PCHAR)abName.m_pBuffer, mdp.type, abValue.m_pBuffer);

	RMFFPropListEntry ple;
	for(VO_U32 i = 0; i < mdp.num_sub_properties; i++)
	{
		ptr_read_dword(ple.offset);
		ptr_read_dword(ple.num_props_for_name);
	}

	for(VO_U32 i = 0; i < mdp.num_sub_properties; i++)
	{
		Parse_Metadata_Property();
	}

	return VO_TRUE;
}

VO_VOID CRealMetaData::Parse_Metadata_Property_Base(VO_PCHAR pName, VO_U32 nType, VO_PBYTE pValue)
{
	if(pName && strlen(pName) > 0 && pValue)
	{
		switch(nType)
		{
		case MPT_TEXT:
			{
				VO_PCHAR szValue = (VO_PCHAR)pValue;
				if(strlen(szValue) > 0)
				{
					if(!strcmp(pName, g_szRMTrack))
					{
						copy_string(m_pTrack, szValue);
					}
					else if(!strcmp(pName, g_szRMYear))
					{
						copy_string(m_pYear, szValue);
					}
// 					else if(!strcmp(pName, g_szRMCategory))
// 					{
// 					}
					else if(!strcmp(pName, g_szRMCDTOC))
					{
						copy_string(m_pCDTOC, szValue);
					}
// 					else if(!strcmp(pName, g_szRMRating))
// 					{
// 					}
// 					else if(!strcmp(pName, g_szRMSubscription))
// 					{
// 					}
				}
			}
			break;

		case MPT_TEXTLIST:
			break;

		case MPT_URL:
			break;

		case MPT_DATE:
			break;

		case MPT_FILENAME:
			break;

		case MPT_FLAG:
			break;

		case MPT_ULONG:
			break;

		case MPT_BINARY:
			break;

		case MPT_GROUPING:
			break;

		case MPT_REFERENCE:
			break;

		default:
			break;
		}
	}
}

VO_U32 CRealMetaData::GetTitle(PMetaDataString pTitle)
{
	REALSTRING2METADATASTRING(m_pTitle, pTitle);

	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(m_pID3v1_1Info->szTitle, pTitle);
		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetArtist(PMetaDataString pArtist)
{
	REALSTRING2METADATASTRING(m_pArtist, pArtist);

	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(m_pID3v1_1Info->szArtist, pArtist);
		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetAlbum(PMetaDataString pAlbum)
{
	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(m_pID3v1_1Info->szAlbum, pAlbum);
		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetGenre(PMetaDataString pGenre)
{
	if(m_pID3v1_1Info)
	{
		if(GetGenreByIndex(m_pID3v1_1Info->btGenre, pGenre))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetComposer(PMetaDataString pComposer)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetTrack(PMetaDataString pTrack)
{
	REALSTRING2METADATASTRING(m_pTrack, pTrack);

	if(m_pID3v1_1Info)
	{
		sprintf(m_szID3v1Track, "%d", m_pID3v1_1Info->btAlbumTrack);

		VO_METADATA_SETTE(pTrack->dwFlags, VO_METADATA_TE_ANSI);
		pTrack->pBuffer = (VO_PBYTE)m_szID3v1Track;
		pTrack->dwBufferSize = sizeof(m_szID3v1Track);

		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetDisk(PMetaDataString pDisk)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetFrontCover(PMetaDataImage pFrontCover)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetGraceNoteTagID(PMetaDataString pGraceNoteTagID)
{
	REALSTRING2METADATASTRING(m_pCDTOC, pGraceNoteTagID);

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetComment(PMetaDataString pComment)
{
	REALSTRING2METADATASTRING(m_pComment, pComment);

	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(m_pID3v1_1Info->szComment, pComment);
		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetPublisher(PMetaDataString pPublisher)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetIsrc(PMetaDataString pIsrc)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetYear(PMetaDataString pYear)
{
	REALSTRING2METADATASTRING(m_pYear, pYear);

	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(m_pID3v1_1Info->szYear, pYear);
		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetConductor(PMetaDataString pConductor)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetRemixer(PMetaDataString pRemixer)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetLyricist(PMetaDataString pLyricist)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetRadioStation(PMetaDataString pRadioStation)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetOrigReleaseYear(PMetaDataString pOrigReleaseYear)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetEncoder(PMetaDataString pEncoder)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetEncodeSettings(PMetaDataString pEncodeSettings)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetCopyright(PMetaDataString pCopyright)
{
	REALSTRING2METADATASTRING(m_pCopyright, pCopyright);

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetOrigArtist(PMetaDataString pOrigArtist)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CRealMetaData::GetOrigAlbum(PMetaDataString pOrigAlbum)
{
	return VO_ERR_METADATA_NOSUCHFRAME;
}
