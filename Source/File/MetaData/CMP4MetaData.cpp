/*******************************************************************************
Reference:	1, http://code.google.com/p/mp4v2/wiki/iTunesMetadata
			2, 3GPP-TS-26.244-v6.2.0-2004.pdf
*******************************************************************************/
#include "CMP4MetaData.h"
#include "ISOBMFileDataStruct.h"
#include "fMacros.h"
#include "voLog.h"
#include "fCC2.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define Zero_1  0xFFFFFFFF

#define skip_box()		ptr_skip(b.size - ISOBM_BOXHEADSIZE)
#define skip_fullbox()	ptr_skip(b.size - ISOBM_FULLBOXHEADSIZE)

#define FOURCC_meta					MAKEFOURCC('m', 'e', 't', 'a')
#define FOURCC_ilst					MAKEFOURCC('i', 'l', 's', 't')
#define FOURCC_ID32					MAKEFOURCC('I', 'D', '3', '2')
#define FOURCC_data					MAKEFOURCC('d', 'a', 't', 'a')
#define FOURCC_mean					MAKEFOURCC('m', 'e', 'a', 'n')
#define FOURCC_name					MAKEFOURCC('n', 'a', 'm', 'e')


#define SAFE_FREE_MP4STRING(p)\
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

#define SAFE_FREE_MP4IMAGE(p)\
{\
	if(p)\
	{\
		if(p->pBuffer)\
		{\
			MemFree(p->pBuffer);\
			p->pBuffer = 0;\
		}\
		MemFree(p);\
		p = 0;\
	}\
}

#define NUMBER2STR(num, str)\
{\
	char sz[32];\
	sprintf(sz, "%d", num);\
	SAFE_FREE_MP4STRING(str);\
	str = NEW_OBJ(Mp4String);\
	if(!str)\
		return VO_FALSE;\
	str->btLang = VO_METADATA_TE_ANSI;\
	str->wSize = strlen(sz) + 1;\
	str->pContent = NEW_BUFFER(str->wSize);\
	if(!str->pContent)\
	{\
		SAFE_FREE_MP4STRING(str);\
		return VO_FALSE;\
	}\
	MemCopy(str->pContent, sz, str->wSize);\
}

#define MP4STRING2METADATASTRING(as, mds)\
	if(!as)\
		return VO_ERR_METADATA_NOSUCHFRAME;\
	VO_METADATA_SETTE(mds->dwFlags, as->btLang);\
	mds->dwBufferSize = as->wSize;\
	mds->pBuffer = as->pContent;\
	return VO_ERR_METADATA_OK;

#define MP4STRING2METADATASTRING2(as, mds)\
	if(as)\
	{\
		VO_METADATA_SETTE(mds->dwFlags, as->btLang);\
		mds->dwBufferSize = as->wSize;\
		mds->pBuffer = as->pContent;\
		return VO_ERR_METADATA_OK;\
	}

CMP4MetaData::CMP4MetaData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp)
	: CID3MetaData(pFileOp, pMemOp)
	, m_pTitle(VO_NULL)
	, m_pArtist(VO_NULL)
	, m_pAlbum(VO_NULL)
	, m_pBand(VO_NULL)
	, m_pGenre(VO_NULL)
	, m_pComment(VO_NULL)
	, m_pComposer(VO_NULL)
	, m_pCopyright(VO_NULL)
	, m_pYear(VO_NULL)
	, m_pGracenoteTagID(VO_NULL)
	, m_pPublisher(VO_NULL)
	, m_pEncoder(VO_NULL)
	, m_pEncodingSettings(VO_NULL)
	, m_pTrack(VO_NULL)
	, m_pDisk(VO_NULL)
	, m_pCover(VO_NULL)
{
}

CMP4MetaData::~CMP4MetaData()
{
	Close();
}

VO_U32 CMP4MetaData::Close()
{
	SAFE_FREE_MP4STRING(m_pTitle);
	SAFE_FREE_MP4STRING(m_pArtist);
	SAFE_FREE_MP4STRING(m_pAlbum);
	SAFE_FREE_MP4STRING(m_pBand);
	SAFE_FREE_MP4STRING(m_pGenre);
	SAFE_FREE_MP4STRING(m_pComment);
	SAFE_FREE_MP4STRING(m_pComposer);
	SAFE_FREE_MP4STRING(m_pCopyright);
	SAFE_FREE_MP4STRING(m_pYear);
	SAFE_FREE_MP4STRING(m_pGracenoteTagID);
	SAFE_FREE_MP4STRING(m_pPublisher);
	SAFE_FREE_MP4STRING(m_pEncoder);
	SAFE_FREE_MP4STRING(m_pEncodingSettings);
	SAFE_FREE_MP4STRING(m_pTrack);
	SAFE_FREE_MP4STRING(m_pDisk);
	SAFE_FREE_MP4IMAGE(m_pCover);

	return CBaseMetaData::Close();
}

VO_U32 CMP4MetaData::ReadMetaDataB()
{
	return Parse() ? VO_ERR_METADATA_OK : VO_ERR_METADATA_NOMETADATA;
}

VO_U32 CMP4MetaData::ReadMetaDataB(PMetaDataInitInfo pInitInfo)
{
	if(VO_MAXU64 == pInitInfo->ullFilePositions[0])
		return ReadMetaDataB();

	if(!m_pFileChunk->FLocate(pInitInfo->ullFilePositions[0]))
		return VO_ERR_METADATA_NOMETADATA;

	if(VO_METADATA_iTunes == pInitInfo->eType)
		return ParseTags_iTunes(pInitInfo->dwReserved) ? VO_ERR_METADATA_OK : VO_ERR_METADATA_NOMETADATA;
	else if(VO_METADATA_TS26244 == pInitInfo->eType)
		return ParseTags_TS26244(pInitInfo->dwReserved) ? VO_ERR_METADATA_OK : VO_ERR_METADATA_NOMETADATA;
	else
		return VO_ERR_METADATA_NOMETADATA;
}

VO_BOOL CMP4MetaData::Parse()
{
	VO_BOOL bRet = VO_FALSE;

	VO_U32 dwBoxContSize = FindBoxB(FOURCC_moov);
	if(Zero_1 == dwBoxContSize)
		return bRet;

	VO_U64 dwMoovFilePos = m_pFileChunk->FGetFilePos();
	VO_U64 dwMoovContSize = dwBoxContSize;

	dwBoxContSize = FindBoxB(FOURCC_udta, dwBoxContSize);
	if(Zero_1 != dwBoxContSize)
	{
		VO_U64 dwUdtaFilePos = m_pFileChunk->FGetFilePos();
		VO_U64 dwUdtaContSize = dwBoxContSize;

		dwBoxContSize = FindBoxB(FOURCC_meta, dwBoxContSize);
		if(Zero_1 == dwBoxContSize)
		{
			//not iTunes, try TS26244!!
			m_pFileChunk->FLocate(dwUdtaFilePos);
			if(ParseTags_TS26244(VO_U32(dwUdtaContSize)))
				bRet = VO_TRUE;
		}
		else
		{
			//meta box is full box!!
			ptr_skip(4);

			dwBoxContSize = FindBoxB(FOURCC_ilst, dwBoxContSize);
			if(Zero_1!= dwBoxContSize)
			{
				if(ParseTags_iTunes(dwBoxContSize))
					bRet = VO_TRUE;
			}
		}
	}

	m_pFileChunk->FLocate(dwMoovFilePos);
	dwBoxContSize = FindBoxB(FOURCC_meta, VO_U32(dwMoovContSize));
	if(Zero_1 != dwBoxContSize)
	{
		//meta box is full box!!
		ptr_skip(4);

		dwBoxContSize = FindBoxB(FOURCC_ID32, VO_U32(dwBoxContSize));
		if(Zero_1 != dwBoxContSize)
		{
/*			Field				Type				Details								Value
			BoxHeader.Size		Unsigned int(32)
			BoxHeader.Type		Unsigned int(32)										'ID32'
			BoxHeader.Version	Unsigned int(8)											0
			BoxHeader.Flags		Bit(24)													0
			Pad					Bit(1)													0
			Language			Unsigned int(5)[3]	Packed ISO-639-2/T language code
			ID3v2data			Unsigned int(8)[]	Complete ID3 version 2.x.x data
*/
			if(ParseV2(m_pFileChunk->FGetFilePos() + 6))
				bRet = VO_TRUE;
		}
	}

	return bRet;
}

VO_U32 CMP4MetaData::FindBoxB(VO_U32 fccType, VO_U32 dwSize /* = 0xFFFFFFFF */)
{
	use_big_endian_read
	m_pFileChunk->FStartRecord();

	Box b;
	while(m_pFileChunk->FGetLastRead() < dwSize)
	{
		ptr_read_dword_b(b.size);
		ptr_read_fcc(b.type);

		if(fccType == b.type)
			return (b.size < ISOBM_BOXHEADSIZE) ? 0 : (b.size - ISOBM_BOXHEADSIZE);
		else
			skip_box();
	}

	return -1;
}

VO_BOOL CMP4MetaData::ParseTags_iTunes(VO_U32 dwBoxContSize)
{
	use_big_endian_read
	m_pFileChunk->FStartRecord();

	Box b;
	while(m_pFileChunk->FGetLastRead() < dwBoxContSize)
	{
		ptr_read_dword_b(b.size);
		ptr_read_dword_b(b.type);

		switch(b.type)
		{
		//Name, UTF-8 string
		case FOURCC2_Xnam:
			{
				if(!ReadTagContent_iTunes(&m_pTitle))
					return VO_FALSE;
				VOLOGI("Name:%s", m_pTitle->pContent);
			}
			break;

		//Artist, UTF-8 string
		case FOURCC2_Xart:
		case FOURCC2_XART:
			{
				if(!ReadTagContent_iTunes(&m_pArtist))
					return VO_FALSE;
				VOLOGI("Artist:%s", m_pArtist->pContent);

			}

			break;

		//Album Artist, UTF-8 string
		case FOURCC2_aART:
			{
				if(!ReadTagContent_iTunes(&m_pBand))
					return VO_FALSE;
				VOLOGI("Album Artist:%s", m_pBand->pContent);

			}
			break;

		//Album, UTF-8 string
		case FOURCC2_Xalb:
			{
				if(!ReadTagContent_iTunes(&m_pAlbum))
					return VO_FALSE;
				VOLOGI("Album :%s", m_pAlbum->pContent);
			}
			break;

		//Composer, UTF-8 string
		case FOURCC2_Xwrt:
			{
				if(!ReadTagContent_iTunes(&m_pComposer))
					return VO_FALSE;
				VOLOGI("Composer :%s", m_pComposer->pContent);
			}
			break;

		//Comment, UTF-8 string
		case FOURCC2_Xcmt:
			{
				if(!ReadTagContent_iTunes(&m_pComment))
					return VO_FALSE;
				VOLOGI("Comment :%s", m_pComment->pContent);
			}
			break;

		//Genre(Predefined), enum
		//Standard Genres according to ID3, the iTunes tag is one greater the the corresponding ID3 tag
		case FOURCC2_gnre:
			{
				//# 8766 10/27/2011,by leon
				VO_U64 pos = m_pFileChunk->FGetFilePos();
				Box b1;
				ptr_read_dword_b(b1.size);
				ptr_read_fcc(b1.type);
				
				if(FOURCC2_data == b1.type )//&& b1.size > 16)
				{
					ptr_skip(8);
				}
				else
					m_pFileChunk->FLocate(pos);
			//# 8766 10/27/2011,by leon

				VO_U16 wGenre = 0;
				ptr_read_word_b(wGenre);

				if(wGenre > 0 && wGenre <= GENRE_TOTALCOUNT)
				{
					SAFE_FREE_MP4STRING(m_pGenre);

					//# 8766 10/27/2011,by leon
					m_pGenre = NEW_OBJ(Mp4String);
					//# 8766 10/27/2011,by leon

					m_pGenre->btLang = VO_METADATA_TE_ANSI;
					//#8946 11/07/2011,by leon
					m_pGenre->wSize = strlen(m_arrGenre[wGenre - 1]) +1;
					m_pGenre->pContent = NEW_BUFFER(m_pGenre->wSize);
					MemCopy(m_pGenre->pContent, m_arrGenre[wGenre - 1], m_pGenre->wSize);
					//#8946 11/07/2011,by leon
					VOLOGI("Genre(Predefined) :%s", m_pGenre->pContent);

				}
			}
			break;

		//Genre(User defined), UTF-8 string
		case FOURCC2_Xgen:
			{
				if(m_pGenre)	//Standard Genres is high priority!!
				{
					skip_box();
				}
				else
				{
					if(!ReadTagContent_iTunes(&m_pGenre))
						return VO_FALSE;
					VOLOGI("Genre(User defined) :%s", m_pGenre->pContent);

				}
			}
			break;

		//Release Date, UTF-8 string
		//YYYY-MM-DD format, may be incomplete (e.g. just the year)
		case FOURCC2_Xday:
			{ 
				if(!ReadTagContent_iTunes(&m_pYear))
					return VO_FALSE;
				VOLOGI("Release Date :%s", m_pYear->pContent);

			}
			break;

		//Track Number, binary
		case FOURCC2_trkn:
			{
				if(b.size == ISOBM_BOXHEADSIZE + 6)
				{
					VO_U32 dwTrack = 0;
					ptr_read_dword_b(dwTrack);
					ptr_skip(2);

					NUMBER2STR((int(dwTrack)), m_pTrack);
				}
				else
				{
					Box b1;
					ptr_read_dword_b(b1.size);
					ptr_read_fcc(b1.type);

					if(FOURCC2_data != b1.type || b1.size <= 16)
						return VO_FALSE;

					VO_U32 dwFlags = 0;
					ptr_read_dword_b(dwFlags);
					dwFlags &= 0x00FFFFFF;
					if(dwFlags != 0)
						return VO_FALSE;

					//4 bytes NULL space
					ptr_skip(4);

					if(b1.size - 16 == 8)
					{
						//2012.2.3, modify by leon
						//00 00 00 07 00 08 00 00
						//error tracknumber: 07 / 00 08 00 00
						//right tracknumber: 07/ 08
						VO_U32 nTrackPosition = 0;
						ptr_read_dword_b(nTrackPosition);
						//2012.2.3, modify by leon
						nTrackPosition  &= 0xff;

						VO_U32 nTrackTotal = 0;
						ptr_read_dword_b(nTrackTotal);
						//2012.2.3, modify by leon
						nTrackTotal = (nTrackTotal >>16) &0xff;

						VO_CHAR szTrack[64];
						sprintf(szTrack, "%d/%d", int(nTrackPosition), int(nTrackTotal) );

						SAFE_FREE_MP4STRING(m_pTrack);
						m_pTrack = NEW_OBJ(Mp4String);
						if(!m_pTrack)
							return VO_FALSE;
						
						m_pTrack->btLang = VO_METADATA_TE_ANSI;
						m_pTrack->wSize = strlen(szTrack) + 1;
						m_pTrack->pContent = NEW_BUFFER(m_pTrack->wSize);
						if(!m_pTrack->pContent)
						{
							SAFE_FREE_MP4STRING(m_pTrack);
							return VO_FALSE;
						}

						MemCopy(m_pTrack->pContent, szTrack, m_pTrack->wSize);
						VOLOGI("Track Number :%s", m_pTrack->pContent);

					}
					else
					{
						ptr_skip(b1.size - 16);
					}
				}
			}
			break;

		//Disk Number, binary
		case FOURCC2_disk:
			{
	
				//# 8766 10/27/2011,by leon
				if(b.size == ISOBM_BOXHEADSIZE + 6)//6?
				{
				//# 8766 10/27/2011,by leon

					VO_U32 dwDisk = 0;
					ptr_read_dword_b(dwDisk);

					NUMBER2STR((int(dwDisk)), m_pDisk);

				//# 8766 10/27/2011,by leon
				}
				else
				{
					Box b1;
					ptr_read_dword_b(b1.size);
					ptr_read_fcc(b1.type);

					if(FOURCC2_data != b1.type || b1.size <= 16)
						return VO_FALSE;
					VO_U32 dwFlags = 0;
					ptr_read_dword_b(dwFlags);
					dwFlags &= 0x00FFFFFF;
					if(dwFlags != 0)
						return VO_FALSE;

					//4 bytes NULL space
					ptr_skip(4);
					if(b1.size - 16 == 6)//6?
					{
						VO_U32 dwDisk = 0;
						ptr_read_dword_b(dwDisk);
						NUMBER2STR((int(dwDisk)), m_pDisk);
						ptr_skip(2);
						VOLOGI("Disk Number :%s", m_pDisk->pContent);

					}
					else
					{
						ptr_skip(b1.size - 16);
					}
				}
				//# 8766 10/27/2011,by leon
			}
			break;

		//Cover Art, JPEG/PNG/BMP
		//May have multiple data atoms
		case FOURCC2_covr:
			{
				SAFE_FREE_MP4IMAGE(m_pCover);

				m_pCover = NEW_OBJ(MetaDataImage);
				if(!m_pCover)
					return VO_FALSE;

				//# 8766 10/27/2011,by leon
				VO_U64 pos = m_pFileChunk->FGetFilePos();
				Box b1;
				ptr_read_dword_b(b1.size);
				ptr_read_fcc(b1.type);

				if(FOURCC2_data == b1.type )//&& b1.size > 16)
				{
					VO_U32 dwFlags = 0;
					ptr_read_dword_b(dwFlags);
// 					dwFlags &= 0x00FFFFFF;
// 					if(dwFlags != 0)
// 						return VO_FALSE;
					//4 bytes NULL space
					ptr_skip(4);
					b1.size -= 16; //ISOBM_BOXHEADSIZE + 4(flag) + 4(NULL)
					VO_U32 size = b1.size;
					m_pCover->dwBufferSize = size;
				}
				else
				{
					m_pFileChunk->FLocate(pos);
					m_pCover->dwBufferSize = b.size - ISOBM_BOXHEADSIZE;
					//# 8766 10/27/2011,by leon
				}				
		
				m_pCover->pBuffer = NEW_BUFFER(m_pCover->dwBufferSize);
				if(!m_pCover->pBuffer)
					return VO_FALSE;

				ptr_read_pointer(m_pCover->pBuffer, m_pCover->dwBufferSize);
// 				FILE *ff;
// 				ff = fopen("d://cover","wb");
// 				fwrite(m_pCover->pBuffer, 1,m_pCover->dwBufferSize, ff);
				VO_U32 dwFlag = 0xE0FFD8FF;
				if(!MemCompare(m_pCover->pBuffer, &dwFlag, 4))
					m_pCover->nImageType = VO_METADATA_IT_JPEG;
				else
					m_pCover->nImageType = VO_METADATA_IT_UNKNOWN;
				
			}
			break;

		//Encoding Tool, UTF-8 string
		case FOURCC2_Xtoo:
			{
				if(!ReadTagContent_iTunes(&m_pEncodingSettings))
					return VO_FALSE;
				VOLOGI("Encoding Tool :%s", m_pEncodingSettings->pContent);

			}
			break;

		//Encoded By, UTF-8 string
		case FOURCC2_Xenc:
			{
				if(!ReadTagContent_iTunes(&m_pEncoder))
					return VO_FALSE;
				VOLOGI("Encoding By :%s", m_pEncoder->pContent);

			}
			break;

		case FOURCC2_____:
			{
				if(!ReadUDTagContent_iTunes())
					return VO_FALSE;
			}
			break;

		//Grouping, UTF-8 string
		//like 'TIT1' in ID3
		case FOURCC2_grup:
		//Tempo, 16-bit integer
		//in beats per minute
		case FOURCC2_tmpo:
		//Compilation, boolean
		case FOURCC2_cpil:
		//TV Show Name, UTF-8 string
		case FOURCC2_tvsh:
		//TV Episode ID, UTF-8 string
		case FOURCC2_tven:
		//TV Season, 32-bit integer
		case FOURCC2_tvsn:
		//TV Episode, 32-bit integer
		case FOURCC2_tves:
		//TV Network, UTF-8 string
		case FOURCC2_tvnn:
		//Description, UTF-8 string
		//Short description
		case FOURCC2_desc:
		//Long description, UTF-8 string
		//Not limited to 255 bytes
		case FOURCC2_ldes:
		//Lyrics, UTF-8 string
		//Not limited to 255 bytes
		case FOURCC2_Xlyr:
		//Sort Name, UTF-8 string
		case FOURCC2_sonm:
		//Sort Artist, UTF-8 string
		case FOURCC2_soar:
		//Sort Album Artist, UTF-8 string
		case FOURCC2_soaa:
		//Sort Album, UTF-8 string
		case FOURCC2_soal:
		//Sort Composer, UTF-8 string
		case FOURCC2_soco:
		//Sort Show, UTF-8 string
		case FOURCC2_sosn:
		//Copyright, UTF-8 string
		//Only ISO standard tag
		case FOURCC2_cprt:
		//Purchase Date, UTF-8 string
		case FOURCC2_purd:
		//Podcast, boolean
		case FOURCC2_pcst:
		//Podcast URL
		case FOURCC2_purl:
		//Keywords, UTF-8 string
		//Used for Podcast
		case FOURCC2_keyw:
		//Category, UTF-8 string
		//Used for Podcast
		case FOURCC2_catg:
		//HD video, boolean
		case FOURCC2_hdvd:
		//Media Type, 8-bit integer(enum)
		case FOURCC2_stik:
		//Content Rating, 8-bit integer
		case FOURCC2_rtng:
		//Gapless Playback, boolean
		case FOURCC2_pgap:
		//Purchase Account, UTF-8 string
		case FOURCC2_apID:
		//Account Type, 8-bit integer
		case FOURCC2_akID:
		//32-bit integer
		//iTunes Catalog ID, used for combing SD and HD encodes in iTunes 
		case FOURCC2_cnID:
		//32-bit integer
		//Identifies in which iTunes Store a file was bought 
		case FOURCC2_sfID:
		//32-bit integer
		case FOURCC2_atID:
		//64-bit integer
		case FOURCC2_plID:
		//32-bit integer
		case FOURCC2_geID:
		//UTF-8 string
		case FOURCC2_Xst3:
		default:
			{
				skip_box();
			}
			break;
		}
	}
	
	return VO_TRUE;
}

VO_BOOL CMP4MetaData::ReadTagContent_iTunes(PMp4String* pString)
{
	use_big_endian_read

	Box b;
	ptr_read_dword_b(b.size);
	ptr_read_fcc(b.type);

	if(FOURCC_data != b.type || b.size <= 16)
		return VO_FALSE;

	VO_U32 dwFlags = 0;
	ptr_read_dword_b(dwFlags);
	dwFlags &= 0x00FFFFFF;

	//4 bytes NULL space
	ptr_skip(4);

	PMp4String pNewString = NEW_OBJ(Mp4String);
	if(!pNewString)
		return VO_FALSE;

	pNewString->wSize = VO_U16( b.size - 16);
	pNewString->pContent = NEW_BUFFER(pNewString->wSize);
	if(!pNewString->pContent)
	{
		SAFE_FREE_MP4STRING(pNewString);

		return VO_FALSE;
	}

	ptr_read_pointer(pNewString->pContent, pNewString->wSize);

	if(dwFlags)
		pNewString->btLang = VO_METADATA_TE_UTF8;
	else
		pNewString->btLang = VO_METADATA_TE_ANSI;
	
	SAFE_FREE_MP4STRING((*pString));
	*pString = pNewString;

	return VO_TRUE;
}

VO_BOOL CMP4MetaData::ReadUDTagContent_iTunes()
{
	use_big_endian_read

	Box b;

	//mean box!!
	ptr_read_dword_b(b.size);
	ptr_read_fcc(b.type);
	if(FOURCC_mean != b.type)
		return VO_FALSE;
	
	skip_box();	//skip 'mean' box content!!

	//name box!!
	ptr_read_dword_b(b.size);
	ptr_read_fcc(b.type);
	if(FOURCC_name != b.type || b.size <= 12)
		return VO_FALSE;
	ptr_skip(4);

	char szName[256];
	ptr_read_pointer(szName, b.size - 12);

	if(!MemCompare(szName, (VO_PTR)"gnid", 4))
		return ReadTagContent_iTunes(&m_pGracenoteTagID);
	else if(!MemCompare(szName, (VO_PTR)"yearrel", 7))
		return ReadTagContent_iTunes(&m_pYear);
	else if(!MemCompare(szName, (VO_PTR)"composer", 8))
		return ReadTagContent_iTunes(&m_pComposer);
	else if(!MemCompare(szName, (VO_PTR)"publisher", 8))
		return ReadTagContent_iTunes(&m_pPublisher);
	else if(!MemCompare(szName, (VO_PTR)"albumartist", 11))
		return ReadTagContent_iTunes(&m_pArtist);
	else
	{
		//skip data box
		ptr_read_dword_b(b.size);
		ptr_read_fcc(b.type);
		if(FOURCC2_data != b.type)
			return VO_FALSE;

		skip_box();
	}

	return VO_TRUE;
}

VO_BOOL CMP4MetaData::ParseTags_TS26244(VO_U32 dwBoxContSize)
{
	use_big_endian_read
	m_pFileChunk->FStartRecord();

	FullBox b;
	while(m_pFileChunk->FGetLastRead() < dwBoxContSize)
	{
		ptr_read_dword_b(b.size);
		ptr_read_dword_b(b.type);
		ptr_read_dword_b(b.version_flags);

		switch(b.type)
		{
		case FOURCC2_titl:
			{
				if(!ReadTagContent_TS26244(b.size - ISOBM_FULLBOXHEADSIZE, &m_pTitle))
					return VO_FALSE;
			}
			break;

		case FOURCC2_dscp:
			{
				if(!ReadTagContent_TS26244(b.size - ISOBM_FULLBOXHEADSIZE, &m_pComment))
					return VO_FALSE;
			}
			break;

		case FOURCC2_cprt:
			{
				if(!ReadTagContent_TS26244(b.size - ISOBM_FULLBOXHEADSIZE, &m_pCopyright))
					return VO_FALSE;
			}

			break;

		case FOURCC2_perf:
			{
				if(!ReadTagContent_TS26244(b.size - ISOBM_FULLBOXHEADSIZE, &m_pArtist))
					return VO_FALSE;
			}
			break;

		case FOURCC2_auth:
			{
				if(!ReadTagContent_TS26244(b.size - ISOBM_FULLBOXHEADSIZE, &m_pComposer))
					return VO_FALSE;
			}
			break;

		case FOURCC2_gnre:
			{
				if(!ReadTagContent_TS26244(b.size - ISOBM_FULLBOXHEADSIZE, &m_pGenre))
					return VO_FALSE;
			}
			break;

		case FOURCC2_albm:
			{
				if(!ReadTagContent_TS26244(b.size - ISOBM_FULLBOXHEADSIZE, &m_pAlbum))
					return VO_FALSE;
			}
			break;

		default:
			{
				skip_fullbox();
			}
			break;
		}
	}

	return VO_TRUE;
}

VO_BOOL CMP4MetaData::ReadTagContent_TS26244(VO_U32 dwBoxContSize, PMp4String* pString)
{
	if(dwBoxContSize < 2)
		return VO_FALSE;

	//Pad and Language
	ptr_skip(2);
	VO_U32 dwStrSize = dwBoxContSize - 2;
	if(dwStrSize > 2)
	{
		VO_BYTE btHead[2] = {0, 0};
		ptr_read_pointer(btHead, 2);
		if((btHead[0] == 0xFE) && (btHead[1] == 0xFF)) //UTF-16
			dwStrSize -= 2;
		else //UTF-8
			m_pFileChunk->FBack(2);
	}

	PMp4String pNewString = NEW_OBJ(Mp4String);
	if(!pNewString)
		return VO_FALSE;

	pNewString->wSize = VO_U16(dwStrSize);
	pNewString->pContent = NEW_BUFFER(pNewString->wSize);
	if(!pNewString->pContent)
	{
		SAFE_FREE_MP4STRING(pNewString);

		return VO_FALSE;
	}

	ptr_read_pointer(pNewString->pContent, pNewString->wSize);

	if(dwStrSize != (dwBoxContSize - 2))
		pNewString->btLang = VO_METADATA_TE_UTF16;
	else
		pNewString->btLang = VO_METADATA_TE_UTF8;

	SAFE_FREE_MP4STRING((*pString));
	*pString = pNewString;

	return VO_TRUE;
}

VO_U32 CMP4MetaData::GetTitle(PMetaDataString pTitle)
{
	MP4STRING2METADATASTRING2(m_pTitle, pTitle);

	return CID3MetaData::GetTitle(pTitle);
}

VO_U32 CMP4MetaData::GetArtist(PMetaDataString pArtist)
{
	MP4STRING2METADATASTRING2(m_pArtist, pArtist);

	return CID3MetaData::GetArtist(pArtist);
}

VO_U32 CMP4MetaData::GetAlbum(PMetaDataString pAlbum)
{
	MP4STRING2METADATASTRING2(m_pAlbum, pAlbum);

	return CID3MetaData::GetAlbum(pAlbum);
}

VO_U32 CMP4MetaData::GetBand(PMetaDataString pBand)
{
	MP4STRING2METADATASTRING2(m_pBand, pBand);

	return CID3MetaData::GetBand(pBand);
}

VO_U32 CMP4MetaData::GetGenre(PMetaDataString pGenre)
{
	MP4STRING2METADATASTRING2(m_pGenre, pGenre);

	return CID3MetaData::GetGenre(pGenre);
}

VO_U32 CMP4MetaData::GetComposer(PMetaDataString pComposer)
{
	VOLOGI("+GetComposer")
	MP4STRING2METADATASTRING2(m_pComposer, pComposer);
	return CID3MetaData::GetComposer(pComposer);
}

VO_U32 CMP4MetaData::GetTrack(PMetaDataString pTrack)
{
	MP4STRING2METADATASTRING2(m_pTrack, pTrack);

	return CID3MetaData::GetTrack(pTrack);
}

VO_U32 CMP4MetaData::GetFrontCover(PMetaDataImage pFrontCover)
{
	if(m_pCover)
	{
		pFrontCover->nImageType = m_pCover->nImageType;
		pFrontCover->dwBufferSize = m_pCover->dwBufferSize;
		pFrontCover->pBuffer = m_pCover->pBuffer;
		return VO_ERR_METADATA_OK;
	}

	return CID3MetaData::GetFrontCover(pFrontCover);
}

VO_U32 CMP4MetaData::GetGraceNoteTagID(PMetaDataString pGraceNoteTagID)
{
	MP4STRING2METADATASTRING2(m_pGracenoteTagID, pGraceNoteTagID);
	
	return CID3MetaData::GetGraceNoteTagID(pGraceNoteTagID);
}

VO_U32 CMP4MetaData::GetComment(PMetaDataString pComment)
{
	MP4STRING2METADATASTRING2(m_pComment, pComment);
	
	return CID3MetaData::GetComment(pComment);
}

VO_U32 CMP4MetaData::GetDisk(PMetaDataString pDisk)
{
	MP4STRING2METADATASTRING2(m_pDisk, pDisk);

	return CID3MetaData::GetDisk(pDisk);
}

VO_U32 CMP4MetaData::GetPublisher(PMetaDataString pPublisher)
{
	MP4STRING2METADATASTRING2(m_pPublisher, pPublisher);

	return CID3MetaData::GetPublisher(pPublisher);
}

VO_U32 CMP4MetaData::GetYear(PMetaDataString pYear)
{
	MP4STRING2METADATASTRING2(m_pYear, pYear);

	return CID3MetaData::GetYear(pYear);
}

VO_U32 CMP4MetaData::GetEncoder(PMetaDataString pEncoder)
{
	MP4STRING2METADATASTRING2(m_pEncoder, pEncoder);

	return CID3MetaData::GetEncoder(pEncoder);
}

VO_U32 CMP4MetaData::GetEncodeSettings(PMetaDataString pEncodeSettings)
{
	MP4STRING2METADATASTRING2(m_pEncodingSettings, pEncodeSettings);

	return CID3MetaData::GetEncodeSettings(pEncodeSettings);
}

VO_U32 CMP4MetaData::GetCopyright(PMetaDataString pCopyright)
{
	MP4STRING2METADATASTRING2(m_pCopyright, pCopyright);

	return CID3MetaData::GetCopyright(pCopyright);
}