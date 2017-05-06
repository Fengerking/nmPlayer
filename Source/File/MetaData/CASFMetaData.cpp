#include "CASFMetaData.h"
#include "AsfFileDataStruct.h"
#ifndef _NOT_USE_ASFFILEGUID
#include "AsfFileGuid.h"
#endif	//_NOT_USE_ASFFILEGUID
#include "fMacros.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//WM/Lyrics
static const VO_BYTE g_wszWMLyrics[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x4c, 0x00, 
	0x79, 0x00, 0x72, 0x00, 0x69, 0x00, 0x63, 0x00, 
	0x73, 0x00
};
//WM/Lyrics_Synchronised
static const VO_BYTE g_wszWMLyrics_Synchronised[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x4c, 0x00, 
	0x79, 0x00, 0x72, 0x00, 0x69, 0x00, 0x63, 0x00, 
	0x73, 0x00, 0x5f, 0x00, 0x53, 0x00, 0x79, 0x00, 
	0x6e, 0x00, 0x63, 0x00, 0x68, 0x00, 0x72, 0x00, 
	0x6f, 0x00, 0x6e, 0x00, 0x69, 0x00, 0x73, 0x00, 
	0x65, 0x00, 0x64, 0x00
};
//WM/AlbumTitle
static const VO_BYTE g_wszWMAlbum[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x41, 0x00, 
	0x6c, 0x00, 0x62, 0x00, 0x75, 0x00, 0x6d, 0x00, 
	0x54, 0x00, 0x69, 0x00, 0x74, 0x00, 0x6c, 0x00, 
	0x65, 0x00
};
//WM/AlbumArtist
static const VO_BYTE g_wszWMAlbumArtist[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x41, 0x00, 
	0x6c, 0x00, 0x62, 0x00, 0x75, 0x00, 0x6d, 0x00, 
	0x41, 0x00, 0x72, 0x00, 0x74, 0x00, 0x69, 0x00, 
	0x73, 0x00, 0x74, 0x00
};
//WM/Genre
static const VO_BYTE g_wszWMGenre[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x47, 0x00, 
	0x65, 0x00, 0x6e, 0x00, 0x72, 0x00, 0x65, 0x00
};
//WM/GenreID
static const VO_BYTE g_wszWMGenreID[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x47, 0x00, 
	0x65, 0x00, 0x6e, 0x00, 0x72, 0x00, 0x65, 0x00, 
	0x49, 0x00, 0x44, 0x00
};
//WM/Composer
static const VO_BYTE g_wszWMComposer[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x43, 0x00, 
	0x6f, 0x00, 0x6d, 0x00, 0x70, 0x00, 0x6f, 0x00, 
	0x73, 0x00, 0x65, 0x00, 0x72, 0x00
};
//WM/Publisher
static const VO_BYTE g_wszWMPublisher[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x50, 0x00, 
	0x75, 0x00, 0x62, 0x00, 0x6c, 0x00, 0x69, 0x00, 
	0x73, 0x00, 0x68, 0x00, 0x65, 0x00, 0x72, 0x00
};
//WM/ISRC
static const VO_BYTE g_wszWMISRC[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x49, 0x00, 
	0x53, 0x00, 0x52, 0x00, 0x43, 0x00
};
//WM/Year
static const VO_BYTE g_wszWMYear[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x59, 0x00, 
	0x65, 0x00, 0x61, 0x00, 0x72, 0x00
};
//WM/Conductor
static const VO_BYTE g_wszWMConductor[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x43, 0x00, 
	0x6f, 0x00, 0x6e, 0x00, 0x64, 0x00, 0x75, 0x00, 
	0x63, 0x00, 0x74, 0x00, 0x6f, 0x00, 0x72, 0x00
};
//WM/ModifiedBy
static const VO_BYTE g_wszWMModifiedBy[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x4d, 0x00, 
	0x6f, 0x00, 0x64, 0x00, 0x69, 0x00, 0x66, 0x00, 
	0x69, 0x00, 0x65, 0x00, 0x64, 0x00, 0x42, 0x00, 
	0x79, 0x00
};
//WM/Writer
static const VO_BYTE g_wszWMWriter[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x57, 0x00, 
	0x72, 0x00, 0x69, 0x00, 0x74, 0x00, 0x65, 0x00, 
	0x72, 0x00
};
//WM/RadioStationName
static const VO_BYTE g_wszWMRadioStationName[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x52, 0x00, 
	0x61, 0x00, 0x64, 0x00, 0x69, 0x00, 0x6f, 0x00, 
	0x53, 0x00, 0x74, 0x00, 0x61, 0x00, 0x74, 0x00, 
	0x69, 0x00, 0x6f, 0x00, 0x6e, 0x00, 0x4e, 0x00, 
	0x61, 0x00, 0x6d, 0x00, 0x65, 0x00
};
//WM/OriginalReleaseYear
static const VO_BYTE g_wszWMOriginalReleaseYear[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x4f, 0x00, 
	0x72, 0x00, 0x69, 0x00, 0x67, 0x00, 0x69, 0x00, 
	0x6e, 0x00, 0x61, 0x00, 0x6c, 0x00, 0x52, 0x00, 
	0x65, 0x00, 0x6c, 0x00, 0x65, 0x00, 0x61, 0x00, 
	0x73, 0x00, 0x65, 0x00, 0x59, 0x00, 0x65, 0x00, 
	0x61, 0x00, 0x72, 0x00
};
//WM/EncodedBy
static const VO_BYTE g_wszWMEncodedBy[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x45, 0x00, 
	0x6e, 0x00, 0x63, 0x00, 0x6f, 0x00, 0x64, 0x00, 
	0x65, 0x00, 0x64, 0x00, 0x42, 0x00, 0x79, 0x00
};
//WM/EncodingSettings
static const VO_BYTE g_wszWMEncodingSettings[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x45, 0x00, 
	0x6e, 0x00, 0x63, 0x00, 0x6f, 0x00, 0x64, 0x00, 
	0x69, 0x00, 0x6e, 0x00, 0x67, 0x00, 0x53, 0x00, 
	0x65, 0x00, 0x74, 0x00, 0x74, 0x00, 0x69, 0x00, 
	0x6e, 0x00, 0x67, 0x00, 0x73, 0x00
};
//GN/UniqueFileIdentifier
static const VO_BYTE g_wszGNUfid[] = {
	0x47, 0x00, 0x4e, 0x00, 0x2f, 0x00, 0x55, 0x00, 
	0x6e, 0x00, 0x69, 0x00, 0x71, 0x00, 0x75, 0x00, 
	0x65, 0x00, 0x46, 0x00, 0x69, 0x00, 0x6c, 0x00, 
	0x65, 0x00, 0x49, 0x00, 0x64, 0x00, 0x65, 0x00, 
	0x6e, 0x00, 0x74, 0x00, 0x69, 0x00, 0x66, 0x00, 
	0x69, 0x00, 0x65, 0x00, 0x72, 0x00
};
//WM/OriginalArtist
static const VO_BYTE g_wszWMOriginalArtist[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x4f, 0x00, 
	0x72, 0x00, 0x69, 0x00, 0x67, 0x00, 0x69, 0x00, 
	0x6e, 0x00, 0x61, 0x00, 0x6c, 0x00, 0x41, 0x00, 
	0x72, 0x00, 0x74, 0x00, 0x69, 0x00, 0x73, 0x00, 
	0x74, 0x00
};
//WM/OriginalAlbumTitle
static const VO_BYTE g_wszWMOriginalAlbumTitle[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x4f, 0x00, 
	0x72, 0x00, 0x69, 0x00, 0x67, 0x00, 0x69, 0x00, 
	0x6e, 0x00, 0x61, 0x00, 0x6c, 0x00, 0x41, 0x00, 
	0x6c, 0x00, 0x62, 0x00, 0x75, 0x00, 0x6d, 0x00, 
	0x54, 0x00, 0x69, 0x00, 0x74, 0x00, 0x6c, 0x00, 
	0x65, 0x00
};
//WM/TrackNumber
static const VO_BYTE g_wszWMTrackNumber[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x54, 0x00, 
	0x72, 0x00, 0x61, 0x00, 0x63, 0x00, 0x6b, 0x00, 
	0x4e, 0x00, 0x75, 0x00, 0x6d, 0x00, 0x62, 0x00, 
	0x65, 0x00, 0x72, 00
};
//WM/Track
static const VO_BYTE g_wszWMTrack[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x54, 0x00, 
	0x72, 0x00, 0x61, 0x00, 0x63, 0x00, 0x6b, 0x00
};
//WM/PartOfSet
static const VO_BYTE g_wszWMPartOfSet[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x50, 0x00, 
	0x61, 0x00, 0x72, 0x00, 0x74, 0x00, 0x4f, 0x00, 
	0x66, 0x00, 0x53, 0x00, 0x65, 0x00, 0x74, 0x00
};
//WM/Picture
static const VO_BYTE g_wszWMPicture[] = {
	0x57, 0x00, 0x4d, 0x00, 0x2f, 0x00, 0x50, 0x00, 
	0x69, 0x00, 0x63, 0x00, 0x74, 0x00, 0x75, 0x00, 
	0x72, 0x00, 0x65, 0x00
};
static const VO_BYTE g_wszMimeTypeJpg[] = {
	0x69, 0x00, 0x6d, 0x00, 0x61, 0x00, 0x67, 0x00, 
	0x65, 0x00, 0x2f, 0x00, 0x6a, 0x00, 0x70, 0x00, 
	0x67, 0x00
};

static const VO_BYTE g_wszMimeTypeJpeg[] = {
	0x69, 0x00, 0x6d, 0x00, 0x61, 0x00, 0x67, 0x00, 
	0x65, 0x00, 0x2f, 0x00, 0x6a, 0x00, 0x70, 0x00, 
	0x65, 0x00, 0x67, 0x00
};

static const VO_BYTE g_wszMimeTypeBmp[] = {
	0x69, 0x00, 0x6d, 0x00, 0x61, 0x00, 0x67, 0x00, 
	0x65, 0x00, 0x2f, 0x00, 0x62, 0x00, 0x6d, 0x00, 
	0x70, 0x00
};

static const VO_BYTE g_wszMimeTypePng[] = {
	0x69, 0x00, 0x6d, 0x00, 0x61, 0x00, 0x67, 0x00, 
	0x65, 0x00, 0x2f, 0x00, 0x70, 0x00, 0x6e, 0x00, 
	0x67, 0x00
};

static const VO_BYTE g_wszMimeTypeGif[] = {
	0x69, 0x00, 0x6d, 0x00, 0x61, 0x00, 0x67, 0x00, 
	0x65, 0x00, 0x2f, 0x00, 0x67, 0x00, 0x69, 0x00, 
	0x66, 0x00
};

#define voIsEqualString(s1, s2)		(!MemCompare((VO_PTR)s1, s2, sizeof(s1)))

#define SAFE_FREE_ASFSTRING(p)\
{\
	if(p)\
	{\
		if(p->pContent)\
		{\
			MemFree(p->pContent);\
			p->pContent = 0;\
		}\
		MemFree(p);\
		p = 0;\
	}\
}

#define read_string_from_file(p_str, buf_len)\
{\
	if(p_str == VO_NULL)\
	{\
		p_str = NEW_OBJ(AsfString);\
		if(!p_str)\
			return VO_FALSE;\
		p_str->pContent = NEW_BUFFER(buf_len);\
		if(!p_str->pContent)\
			return VO_FALSE;\
		p_str->wSize = static_cast<VO_U16>(buf_len);\
		ptr_read_pointer(p_str->pContent, buf_len);\
	}\
	else\
		ptr_skip(buf_len);\
}

#define ASFSTRING2METADATASTRING(as, mds)\
	if(!as)\
		return VO_ERR_METADATA_NOSUCHFRAME;\
	VO_METADATA_SETTE(mds->dwFlags, VO_METADATA_TE_UTF16);\
	mds->dwBufferSize = as->wSize;\
	mds->pBuffer = as->pContent;\
	return VO_ERR_METADATA_OK;

CASFMetaData::CASFMetaData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp)
	: CBaseMetaData(pFileOp, pMemOp)
	, m_pTitle(VO_NULL)
	, m_pArtist(VO_NULL)
	, m_pCopyright(VO_NULL)
	, m_pComment(VO_NULL)
	, m_pGenre(VO_NULL)
	, m_pGenreID(VO_NULL)
	, m_pAlbum(VO_NULL)
	, m_pBand(VO_NULL)
	, m_pComposer(VO_NULL)
	, m_pPublisher(VO_NULL)
	, m_pIsrc(VO_NULL)
	, m_pYear(VO_NULL)
	, m_pConductor(VO_NULL)
	, m_pRemixer(VO_NULL)
	, m_pLyricist(VO_NULL)
	, m_pRadioStation(VO_NULL)
	, m_pOrigReleaseYear(VO_NULL)
	, m_pEncoder(VO_NULL)
	, m_pEncodingSettings(VO_NULL)
	, m_pOrigArtist(VO_NULL)
	, m_pOrigAlbum(VO_NULL)
	, m_pSyncLyrics(VO_NULL)
	, m_pGraceNoteTagID(VO_NULL)
	, m_pTrack(VO_NULL)
	, m_pDisk(VO_NULL)
	, m_pImage(VO_NULL)
	, m_nTrack(-1)
{
}

CASFMetaData::~CASFMetaData()
{
	Close();
}

VO_U32 CASFMetaData::Close()
{
	SAFE_FREE_ASFSTRING(m_pTitle);
	SAFE_FREE_ASFSTRING(m_pArtist);
	SAFE_FREE_ASFSTRING(m_pCopyright);
	SAFE_FREE_ASFSTRING(m_pComment);
	SAFE_FREE_ASFSTRING(m_pGenre);
	SAFE_FREE_ASFSTRING(m_pGenreID);
	SAFE_FREE_ASFSTRING(m_pAlbum);
	SAFE_FREE_ASFSTRING(m_pBand);
	SAFE_FREE_ASFSTRING(m_pComposer);
	SAFE_FREE_ASFSTRING(m_pPublisher);
	SAFE_FREE_ASFSTRING(m_pIsrc);
	SAFE_FREE_ASFSTRING(m_pYear);
	SAFE_FREE_ASFSTRING(m_pConductor);
	SAFE_FREE_ASFSTRING(m_pRemixer);
	SAFE_FREE_ASFSTRING(m_pLyricist);
	SAFE_FREE_ASFSTRING(m_pRadioStation);
	SAFE_FREE_ASFSTRING(m_pOrigReleaseYear);
	SAFE_FREE_ASFSTRING(m_pEncoder);
	SAFE_FREE_ASFSTRING(m_pEncodingSettings);
	SAFE_FREE_ASFSTRING(m_pOrigArtist);
	SAFE_FREE_ASFSTRING(m_pOrigAlbum);
	SAFE_FREE_ASFSTRING(m_pSyncLyrics);
	SAFE_FREE_ASFSTRING(m_pGraceNoteTagID);
	SAFE_FREE_ASFSTRING(m_pTrack);
	SAFE_FREE_ASFSTRING(m_pDisk);

	if(m_pImage)
	{
		SAFE_MEM_FREE(m_pImage->pBuffer);

		MemFree(m_pImage);
		m_pImage = VO_NULL;
	}

	return CBaseMetaData::Close();
}

VO_U32 CASFMetaData::ReadMetaDataB()
{
	return Parse() ? VO_ERR_METADATA_OK : VO_ERR_METADATA_NOMETADATA;
}

VO_U32 CASFMetaData::ReadMetaDataB(PMetaDataInitInfo pInitInfo)
{
	if(VO_MAXU64 == pInitInfo->ullFilePositions[0] && 
		VO_MAXU64 == pInitInfo->ullFilePositions[1] && 
		VO_MAXU64 == pInitInfo->ullFilePositions[2] && 
		VO_MAXU64 == pInitInfo->ullFilePositions[3])
		return ReadMetaDataB();

	if(VO_MAXU64 != pInitInfo->ullFilePositions[0])
	{
		if(!m_pFileChunk->FLocate(pInitInfo->ullFilePositions[0]))
			return VO_ERR_METADATA_NOMETADATA;

		if(!Parse_Content_Description_Object())
			return VO_ERR_METADATA_NOMETADATA;
	}

	if(VO_MAXU64 != pInitInfo->ullFilePositions[1])
	{
		if(!m_pFileChunk->FLocate(pInitInfo->ullFilePositions[1]))
			return VO_ERR_METADATA_NOMETADATA;

		if(!Parse_Extended_Content_Description_Object())
			return VO_ERR_METADATA_NOMETADATA;
	}

	if(VO_MAXU64 != pInitInfo->ullFilePositions[2])
	{
		if(!m_pFileChunk->FLocate(pInitInfo->ullFilePositions[2]))
			return VO_ERR_METADATA_NOMETADATA;

		if(!Parse_Metadata_Object(VO_FALSE))
			return VO_ERR_METADATA_NOMETADATA;
	}

	if(VO_MAXU64 != pInitInfo->ullFilePositions[3])
	{
		if(!m_pFileChunk->FLocate(pInitInfo->ullFilePositions[3]))
			return VO_ERR_METADATA_NOMETADATA;

		if(!Parse_Metadata_Object(VO_TRUE))
			return VO_ERR_METADATA_NOMETADATA;
	}

	return VO_ERR_METADATA_OK;
}

VO_BOOL CASFMetaData::Parse()
{
#ifdef _NOT_USE_ASFFILEGUID
	return VO_FALSE;
#else	//_NOT_USE_ASFFILEGUID
	AsfObject ao;
	ptr_read_guid(ao.id);
	ptr_read_qword(ao.size);
	if(!voIsEqualGUID(ao.id, ASF_Header_Object))
		return VO_FALSE;

	AsfHeaderObject aho;
	ptr_read_dword(aho.header_objects_number);
	ptr_read_byte(aho.reserved1);
	ptr_read_byte(aho.reserved2);
	for(VO_U32 i = 0; i < aho.header_objects_number; i++)
	{
		VO_S32 nRet = ReadHeaderInfo();
		if(!nRet)
			return VO_FALSE;
		else if(2 == nRet)
			break;
	}

	return VO_TRUE;
#endif	//_NOT_USE_ASFFILEGUID
}

VO_BOOL CASFMetaData::Parse_Content_Description_Object()
{
	AsfContentDescObject acdo;
	ptr_read_word(acdo.title_len);
	ptr_read_word(acdo.author_len);
	ptr_read_word(acdo.copyright_len);
	ptr_read_word(acdo.descr_len);
	ptr_read_word(acdo.rating_len);

	if(acdo.title_len > 0) {
		read_string_from_file(m_pTitle, acdo.title_len);
	}

	if(acdo.author_len > 0) {
		read_string_from_file(m_pArtist, acdo.author_len);
	}

	if(acdo.copyright_len > 0) {
		read_string_from_file(m_pCopyright, acdo.copyright_len);
	}

	if(acdo.descr_len > 0) {
		read_string_from_file(m_pComment, acdo.descr_len);
	}

	ptr_skip(acdo.rating_len);

	return VO_TRUE;
}

VO_BOOL CASFMetaData::Parse_Metadata(VO_PBYTE pName, VO_U16 wDataType, VO_U32 dwDataLen)
{
	if(wDataType == 0x0000)	//UNICODE STRING
	{
		if(voIsEqualString(g_wszWMGenre, pName))	//genre info
		{
			read_string_from_file(m_pGenre, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMGenreID, pName))
		{
			read_string_from_file(m_pGenreID, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMAlbum, pName)) {	//album info
			read_string_from_file(m_pAlbum, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMAlbumArtist, pName)) {	//album artist
			read_string_from_file(m_pBand, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMComposer, pName)) {
			read_string_from_file(m_pComposer, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMPublisher, pName)) {
			read_string_from_file(m_pPublisher, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMISRC, pName)) {
			read_string_from_file(m_pIsrc, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMYear, pName)) {
			read_string_from_file(m_pYear, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMConductor, pName)) {
			read_string_from_file(m_pConductor, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMModifiedBy, pName)) {
			read_string_from_file(m_pRemixer, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMWriter, pName)) {
			read_string_from_file(m_pLyricist, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMRadioStationName, pName)) {
			read_string_from_file(m_pRadioStation, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMOriginalReleaseYear, pName)) {
			read_string_from_file(m_pOrigReleaseYear, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMEncodedBy, pName)) {
			read_string_from_file(m_pEncoder, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMEncodingSettings, pName)) {
			read_string_from_file(m_pEncodingSettings, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMOriginalArtist, pName)) {
			read_string_from_file(m_pOrigArtist, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMOriginalAlbumTitle, pName)) {
			read_string_from_file(m_pOrigAlbum, dwDataLen);
		}
/*		else if(voIsEqualString(g_wszWMLyrics, pName)) {
			//this lyrics is not needed.
			ptr_skip(dwDataLen);
		}*/
		else if(voIsEqualString(g_wszWMTrackNumber, pName)) {
			read_string_from_file(m_pTrack, dwDataLen);
		}
/*		else if(voIsEqualString(g_wszWMTrack, pName)) {
			ptr_skip(dwDataLen);
		}*/
		else if(voIsEqualString(g_wszWMPartOfSet, pName)) {
			read_string_from_file(m_pDisk, dwDataLen);
		}
		else {
			ptr_skip(dwDataLen);
		}
	}
	else if(wDataType == 0x0001) {	//BYTE ARRAY
		if(voIsEqualString(g_wszWMLyrics_Synchronised, pName)) {	//synchronized lyrics info
			//here you can get synchronized lyrics info
			read_string_from_file(m_pSyncLyrics, dwDataLen);
		}
		else if(voIsEqualString(g_wszGNUfid, pName)) {
			read_string_from_file(m_pGraceNoteTagID, dwDataLen);
		}
		else if(voIsEqualString(g_wszWMPicture, pName)) {
			/*
			1. type ID(1) VO_METADATA_PT_XXX
			2. data length(4)
			3. mime type(varies, end with 0x00 0x00)
			4. description(2)
			5. data
			*/

			VO_U8 btTypeID = VO_METADATA_PT_OTHER;
			ptr_read_byte(btTypeID);
			if(VO_METADATA_PT_FRONTCOVER != btTypeID && VO_METADATA_PT_OTHER != btTypeID)
			{
				ptr_skip(dwDataLen - 1);
				return VO_TRUE;
			}

			if(m_pImage)
			{
				SAFE_MEM_FREE(m_pImage->pBuffer);

				MemFree(m_pImage);
			}
			m_pImage = NEW_OBJ(MetaDataImage);
			if(!m_pImage)
				return VO_FALSE;

			m_pImage->nImageType = VO_METADATA_IT_UNKNOWN;
			m_pImage->dwBufferSize = 0;
			m_pImage->pBuffer = VO_NULL;

			ptr_read_dword(m_pImage->dwBufferSize);
			//mime type length
			VO_U32 dwMimeTypeLen = dwDataLen - 7 - m_pImage->dwBufferSize;
			VO_PBYTE pMimeType = NEW_BUFFER(dwMimeTypeLen);
			if(!pMimeType)
				return VO_FALSE;
			ptr_read_pointer(pMimeType, dwMimeTypeLen);

			if(voIsEqualString(g_wszMimeTypeJpg, pMimeType))
				m_pImage->nImageType = VO_METADATA_IT_JPEG;
			else if(voIsEqualString(g_wszMimeTypeJpeg, pMimeType))
				m_pImage->nImageType = VO_METADATA_IT_JPEG;
			else if(voIsEqualString(g_wszMimeTypeBmp, pMimeType))
				m_pImage->nImageType = VO_METADATA_IT_BMP;
			else if(voIsEqualString(g_wszMimeTypePng, pMimeType))
				m_pImage->nImageType = VO_METADATA_IT_PNG;
			else if(voIsEqualString(g_wszMimeTypeGif, pMimeType))
				m_pImage->nImageType = VO_METADATA_IT_GIF;

			MemFree(pMimeType);

			ptr_skip(2);
			m_pImage->pBuffer = NEW_BUFFER(m_pImage->dwBufferSize);
			if(!m_pImage->pBuffer)
				return VO_FALSE;

			ptr_read_pointer(m_pImage->pBuffer, m_pImage->dwBufferSize);
		}
		else if(voIsEqualString(g_wszWMTrackNumber, pName)) {
			if(dwDataLen == 4) {
				ptr_read_dword(m_nTrack);
			}
		}
		else {
			ptr_skip(dwDataLen);
		}
	}
	else {
		ptr_skip(dwDataLen);
	}

	return VO_TRUE;
}

VO_BOOL CASFMetaData::Parse_Extended_Content_Description_Object()
{
	VO_U16 wContentDescsCount = 0;
	ptr_read_word(wContentDescsCount);
	AsfContentDesc acd;
	for(int i = 0; i < wContentDescsCount; i++)
	{
		ptr_read_word(acd.name_len);
		VO_PBYTE pName = NEW_BUFFER(acd.name_len);
		if(!pName)
			return VO_FALSE;
		ptr_read_pointer(pName, acd.name_len);
		ptr_read_word(acd.value_data_type);
		ptr_read_word(acd.value_len);

		if(!Parse_Metadata(pName, acd.value_data_type, acd.value_len))
		{
			MemFree(pName);

			return VO_FALSE;
		}

		MemFree(pName);
	}

	return VO_TRUE;
}

VO_BOOL CASFMetaData::Parse_Metadata_Object(VO_BOOL bLibrary)
{
	VO_U16 wContentDescsCount = 0;
	ptr_read_word(wContentDescsCount);
	AsfMetadata am;
	for(int i = 0; i < wContentDescsCount; i++)
	{
		ptr_read_word(am.reserved);
		ptr_read_word(am.stream_number);
		ptr_read_word(am.name_len);
		VO_PBYTE pName = NEW_BUFFER(am.name_len);
		if(!pName)
			return VO_FALSE;
		ptr_read_word(am.data_type);
		ptr_read_dword(am.data_len);
		ptr_read_pointer(pName, am.name_len);

		if(!Parse_Metadata(pName, am.data_type, am.data_len))
		{
			MemFree(pName);

			return VO_FALSE;
		}

		MemFree(pName);
	}

	return VO_TRUE;
}

VO_S32 CASFMetaData::ReadHeaderInfo()
{
#ifdef _NOT_USE_ASFFILEGUID
	return 0;
#else	//_NOT_USE_ASFFILEGUID
	AsfObject ao;
	ptr_read_guid(ao.id);
	ptr_read_qword(ao.size);
	if(voIsEqualGUID(ao.id, ASF_Content_Description_Object))
	{
		if(!Parse_Content_Description_Object())
			return 0;
	}
	else if(voIsEqualGUID(ao.id, ASF_Extended_Content_Description_Object))
	{
		if(!Parse_Extended_Content_Description_Object())
			return 0;
	}
	else if(voIsEqualGUID(ao.id, ASF_Header_Extension_Object))
	{
		AsfHeaderExtObject aheo;
		ptr_read_guid(aheo.reserved_filed_1);
		ptr_read_word(aheo.reserved_filed_2);
		ptr_read_dword(aheo.data_size);
		//note: aheo.data_size inefficacy, the correct data size is (ao.size - Len_Object - Len_Header_Ext_Object);
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
	else if(voIsEqualGUID(ao.id, ASF_Data_Object))
		return 2;
	else
		ptr_skip(ao.size - Len_Object);

	return 1;
#endif	//_NOT_USE_ASFFILEGUID
}

VO_U32 CASFMetaData::ReadExtHeaderInfo()
{
#ifdef _NOT_USE_ASFFILEGUID
	return 0;
#else	//_NOT_USE_ASFFILEGUID
	AsfObject ao;
	ptr_read_guid(ao.id);
	ptr_read_qword(ao.size);
	if(voIsEqualGUID(ao.id, ASF_Metadata_Object))
		Parse_Metadata_Object(VO_FALSE);
	else if(voIsEqualGUID(ao.id, ASF_Metadata_Library_Object))
		Parse_Metadata_Object(VO_TRUE);
	else
	{
		ptr_skip(ao.size - Len_Object);
	}

	return (VO_U32)ao.size;
#endif	//_NOT_USE_ASFFILEGUID
}

VO_U32 CASFMetaData::GetTitle(PMetaDataString pTitle)
{
	ASFSTRING2METADATASTRING(m_pTitle, pTitle);
}

VO_U32 CASFMetaData::GetArtist(PMetaDataString pArtist)
{
	ASFSTRING2METADATASTRING(m_pArtist, pArtist);
}

VO_U32 CASFMetaData::GetAlbum(PMetaDataString pAlbum)
{
	ASFSTRING2METADATASTRING(m_pAlbum, pAlbum);
}

VO_U32 CASFMetaData::GetBand(PMetaDataString pBand)
{
	ASFSTRING2METADATASTRING(m_pBand, pBand);
}

VO_U32 CASFMetaData::GetGenre(PMetaDataString pGenre)
{
	if(m_pGenre)
	{
		VO_METADATA_SETTE(pGenre->dwFlags, VO_METADATA_TE_UTF16);
		pGenre->dwBufferSize = m_pGenre->wSize;
		pGenre->pBuffer = m_pGenre->pContent;

		return VO_ERR_METADATA_OK;
	}

	if(m_pGenreID)
	{
		VO_METADATA_SETTE(pGenre->dwFlags, VO_METADATA_TE_UTF16);
		pGenre->dwBufferSize = m_pGenreID->wSize;
		pGenre->pBuffer = m_pGenreID->pContent;

		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CASFMetaData::GetComposer(PMetaDataString pComposer)
{
	ASFSTRING2METADATASTRING(m_pComposer, pComposer);
}

VO_U32 CASFMetaData::GetTrack(PMetaDataString pTrack)
{
	if(m_pTrack)
	{
		VO_METADATA_SETTE(pTrack->dwFlags, VO_METADATA_TE_UTF16);
		pTrack->dwBufferSize = m_pTrack->wSize;
		pTrack->pBuffer = m_pTrack->pContent;
		
		return VO_ERR_METADATA_OK;
	}

	if(m_nTrack != VO_MAXU32)
	{
		sprintf(m_szTrack, "%lu", m_nTrack);

		VO_METADATA_SETTE(pTrack->dwFlags, VO_METADATA_TE_ANSI);
		pTrack->pBuffer = (VO_PBYTE)m_szTrack;
		pTrack->dwBufferSize = strlen(m_szTrack) + 1;

		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CASFMetaData::GetDisk(PMetaDataString pDisk)
{
	ASFSTRING2METADATASTRING(m_pDisk, pDisk);
}

VO_U32 CASFMetaData::GetFrontCover(PMetaDataImage pFrontCover)
{
	if(!m_pImage)
		return VO_ERR_METADATA_NOSUCHFRAME;

	pFrontCover->nImageType = m_pImage->nImageType;
	pFrontCover->dwBufferSize = m_pImage->dwBufferSize;
	pFrontCover->pBuffer = m_pImage->pBuffer;

	return VO_ERR_METADATA_OK;
}

VO_U32 CASFMetaData::GetGraceNoteTagID(PMetaDataString pGraceNoteTagID)
{
	if(!m_pGraceNoteTagID)
		return VO_ERR_METADATA_NOSUCHFRAME;

	VO_METADATA_SETTE(pGraceNoteTagID->dwFlags, VO_METADATA_TE_UTF8);
	pGraceNoteTagID->dwBufferSize = m_pGraceNoteTagID->wSize;
	pGraceNoteTagID->pBuffer = m_pGraceNoteTagID->pContent;

	return VO_ERR_METADATA_OK;
}

VO_U32 CASFMetaData::GetComment(PMetaDataString pComment)
{
	ASFSTRING2METADATASTRING(m_pComment, pComment);
}

VO_U32 CASFMetaData::GetPublisher(PMetaDataString pPublisher)
{
	ASFSTRING2METADATASTRING(m_pPublisher, pPublisher);
}

VO_U32 CASFMetaData::GetIsrc(PMetaDataString pIsrc)
{
	ASFSTRING2METADATASTRING(m_pIsrc, pIsrc);
}

VO_U32 CASFMetaData::GetYear(PMetaDataString pYear)
{
	ASFSTRING2METADATASTRING(m_pYear, pYear);
}

VO_U32 CASFMetaData::GetConductor(PMetaDataString pConductor)
{
	ASFSTRING2METADATASTRING(m_pConductor, pConductor);
}

VO_U32 CASFMetaData::GetRemixer(PMetaDataString pRemixer)
{
	ASFSTRING2METADATASTRING(m_pRemixer, pRemixer);
}

VO_U32 CASFMetaData::GetLyricist(PMetaDataString pLyricist)
{
	ASFSTRING2METADATASTRING(m_pLyricist, pLyricist);
}

VO_U32 CASFMetaData::GetRadioStation(PMetaDataString pRadioStation)
{
	ASFSTRING2METADATASTRING(m_pRadioStation, pRadioStation);
}

VO_U32 CASFMetaData::GetOrigReleaseYear(PMetaDataString pOrigReleaseYear)
{
	ASFSTRING2METADATASTRING(m_pOrigReleaseYear, pOrigReleaseYear);
}

VO_U32 CASFMetaData::GetEncoder(PMetaDataString pEncoder)
{
	ASFSTRING2METADATASTRING(m_pEncoder, pEncoder);
}

VO_U32 CASFMetaData::GetEncodeSettings(PMetaDataString pEncodeSettings)
{
	ASFSTRING2METADATASTRING(m_pEncodingSettings, pEncodeSettings);
}

VO_U32 CASFMetaData::GetCopyright(PMetaDataString pCopyright)
{
	ASFSTRING2METADATASTRING(m_pCopyright, pCopyright);
}

VO_U32 CASFMetaData::GetOrigArtist(PMetaDataString pOrigArtist)
{
	ASFSTRING2METADATASTRING(m_pOrigArtist, pOrigArtist);
}

VO_U32 CASFMetaData::GetOrigAlbum(PMetaDataString pOrigAlbum)
{
	ASFSTRING2METADATASTRING(m_pOrigAlbum, pOrigAlbum);
}
