#include "CBaseMetaData.h"
#include "fMacros.h"
#ifdef _G1_FORCE_UTF16_2_UTF8
#include "voutf8conv.h"
#endif	//_G1_FORCE_UTF16_2_UTF8
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

VO_CHAR CBaseMetaData::m_arrGenre[GENRE_TOTALCOUNT][GENRE_LEN] =
{
	"Blues", "Classic Rock", "Country", "Dance", 
	"Disco", "Funk", "Grunge", "Hip-Hop", 
	"Jazz", "Metal", "New Age", "Oldies", 
	"Other", "Pop", "R&B", "Rap", 
	"Reggae", "Rock", "Techno", "Industrial", 
	"Alternative", "Ska", "Death Metal", "Pranks", 
	"Soundtrack", "Euro-Techno", "Ambient", "Trip Hop", 
	"Vocal", "Jazz Funk", "Fusion", "Trance", 
	"Classical", "Instrumental", "Acid", "House", 
	"Game", "Sound Clip", "Gospel", "Noise", 
	"Alternative Rock", "Bass", "Soul", "Punk", 
	"Space", "Meditative", "Instrumental Pop", "Instrumental Rock", 
	"Ethnic", "Gothic", "Darkwave", "Techno-Industrial", 
	"Electronic", "Pop Folk", "Eurodance", "Dream", 
	"Southern Rock", "Comedy", "Cult", "Gangsta", 
	"Top 40", "Christian Rap", "Pop Funk", "Jungle", 
	"Native American", "Cabaret", "New Wave", "Psychadelic", 
	"Rave", "ShowTunes", "Trailer", "Lo-Fi", 
	"Tribal", "Acid Punk", "Acid Jazz", "Polka", 
	"Retro", "Musical", "Rock & Roll", "Hard Rock", 
	"Folk", "Folk Rock", "National Folk", "Swing", 
	"Fast Fusion", "Bebob", "Latin", "Revival", 
	"Celtic", "Bluegrass", "Avantgarde", "Gothic Rock", 
	"Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock", 
	"Big Band", "Chorus", "Easy Listening", "Acoustic", 
	"Humour", "Speech", "Chanson", "Opera", 
	"Chamber Music", "Sonata", "Symphony", "Booty Bass", 
	"Primus", "Porn Groove", "Satire", "Slow Jam", 
	"Club", "Tango", "Samba", "Folklore", 
	"Ballad", "Power Ballad", "Rhytmic Soul", "Freestyle", 
	"Duet", "Punk Rock", "Drum Solo", "A Capella", 
	"Euro House", "Dance Hall", "Goa", "Drum & Bass", 
	"Club House", "Hardcore", "Terror", "Indie", 
	"BritPop", "Negerpunk", "Polsk Punk", "Beat", 
	"Christian Gangsta Rap", "Heavy Metal", "Black Metal", "Crossover", 
	"Contemporary Christian", "Christian Rock", "Merengue", "Salsa", 
	"Trash Metal", "Anime", "JPop", "SynthPop"
};

CBaseMetaData::CBaseMetaData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp)
	: CvoBaseFileOpr(pFileOp)
	, CvoBaseMemOpr(pMemOp)
	, m_llFileSize(0)
	, m_pFileChunk(VO_NULL)
	, m_pTmpText(VO_NULL)
	, m_dwTmpText(0)
{
}

CBaseMetaData::~CBaseMetaData()
{
	Close();
}

VO_U32 CBaseMetaData::Load(VO_FILE_SOURCE* pFileSource)
{
	pFileSource->nMode = VO_FILE_READ_ONLY;
	VO_PTR pFileHandle = FileOpen(pFileSource);
	if(!pFileHandle)
		return VO_ERR_METADATA_OPENFAIL;

	m_llFileSize = FileSize(pFileHandle);

	CGFileChunk chunk(m_pFileOp, m_pMemOp);
	if(!chunk.FCreate(pFileHandle, 0, 0x8000))	//32K
		return VO_ERR_METADATA_OPENFAIL;

	m_pFileChunk = &chunk;

	VO_U32 rc = ReadMetaDataB();

	chunk.FDestroy();
	m_pFileChunk = VO_NULL;

	FileClose(pFileHandle);

	return rc;
}

VO_U32 CBaseMetaData::Load(PMetaDataInitInfo pInitInfo)
{
	if(!pInitInfo)
		return VO_ERR_INVALID_ARG;

	m_llFileSize = pInitInfo->ullFileSize;

	VO_S64 llRecord = FileSeek(pInitInfo->hFile, 0, VO_FILE_CURRENT);

	FileSeek(pInitInfo->hFile, 0, VO_FILE_BEGIN);
	CGFileChunk chunk(m_pFileOp, m_pMemOp);
	if(!chunk.FCreate(pInitInfo->hFile, 0, 0x8000))	//32K
		return VO_ERR_METADATA_OPENFAIL;

	m_pFileChunk = &chunk;

	VO_U32 rc = ReadMetaDataB(pInitInfo);

	chunk.FDestroy();
	m_pFileChunk = VO_NULL;

	FileSeek(pInitInfo->hFile, llRecord, VO_FILE_BEGIN);

	return rc;
}

VO_U32 CBaseMetaData::Close()
{
	SAFE_MEM_FREE(m_pTmpText);

	return VO_ERR_METADATA_OK;
}

VO_U32 CBaseMetaData::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseMetaData::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	VO_U32 rc = VO_ERR_NOT_IMPLEMENT;
	PMetaDataString pString = 0;
	switch(uID)
	{
	case VO_PID_METADATA_TITLE:
		{
			pString = (PMetaDataString)pParam;
			rc = GetTitle(pString);
		}
		break;

	case VO_PID_METADATA_ARTIST:
		{
			pString = (PMetaDataString)pParam;
			rc = GetArtist(pString);
		}
		break;

	case VO_PID_METADATA_ALBUM:
		{
			pString = (PMetaDataString)pParam;
			rc = GetAlbum(pString);
		}
		break;

	case VO_PID_METADATA_GENRE:
		{
			pString = (PMetaDataString)pParam;
			rc = GetGenre(pString);
		}
		break;

	case VO_PID_METADATA_LYRICLINE:
		rc = GetLyricLine((VO_U32*)pParam);
		break;

	case VO_PID_METADATA_LYRICBYLINE:
		rc = GetLyricByLine((PMetaDataLyricInfo)pParam);
		break;

	case VO_PID_METADATA_LYRICBYTIME:
		rc = GetLyricByTime((PMetaDataLyricInfo)pParam);
		break;

	case VO_PID_METADATA_COMPOSER:
		{
			pString = (PMetaDataString)pParam;
			rc = GetComposer(pString);
		}
		break;

	case VO_PID_METADATA_TRACK:
		{
			pString = (PMetaDataString)pParam;
			rc = GetTrack(pString);
		}
		break;

	case VO_PID_METADATA_FRONTCOVER:
		rc = GetFrontCover((PMetaDataImage)pParam);
		break;

	case VO_PID_METADATA_GRACENOTETAGID:
		{
			pString = (PMetaDataString)pParam;
			rc = GetGraceNoteTagID(pString);
		}
		break;

	case VO_PID_METADATA_COMMENT:
		{
			pString = (PMetaDataString)pParam;
			rc = GetComment(pString);
		}
		break;

	case VO_PID_METADATA_DISK:
		{
			pString = (PMetaDataString)pParam;
			rc = GetDisk(pString);
		}
		break;

	case VO_PID_METADATA_PUBLISHER:
		{
			pString = (PMetaDataString)pParam;
			rc = GetPublisher(pString);
		}
		break;

	case VO_PID_METADATA_ISRC:
		{
			pString = (PMetaDataString)pParam;
			rc = GetIsrc(pString);
		}
		break;

	case VO_PID_METADATA_YEAR:
		{
			pString = (PMetaDataString)pParam;
			rc = GetYear(pString);
		}
		break;

	case VO_PID_METADATA_BAND:
		{
			pString = (PMetaDataString)pParam;
			rc = GetBand(pString);
		}
		break;

	case VO_PID_METADATA_CONDUCTOR:
		{
			pString = (PMetaDataString)pParam;
			rc = GetConductor(pString);
		}
		break;

	case VO_PID_METADATA_REMIXER:
		{
			pString = (PMetaDataString)pParam;
			rc = GetRemixer(pString);
		}
		break;

	case VO_PID_METADATA_LYRICIST:
		{
			pString = (PMetaDataString)pParam;
			rc = GetLyricist(pString);
		}
		break;

	case VO_PID_METADATA_RADIOSTATION:
		{
			pString = (PMetaDataString)pParam;
			rc = GetRadioStation(pString);
		}
		break;

	case VO_PID_METADATA_ORIGRELEASEYEAR:
		{
			pString = (PMetaDataString)pParam;
			rc = GetOrigReleaseYear(pString);
		}
		break;

	case VO_PID_METADATA_OWNER:
		{
			pString = (PMetaDataString)pParam;
			rc = GetOwner(pString);
		}
		break;

	case VO_PID_METADATA_ENCODER:
		{
			pString = (PMetaDataString)pParam;
			rc = GetEncoder(pString);
		}
		break;

	case VO_PID_METADATA_ENCODESETTINGS:
		{
			pString = (PMetaDataString)pParam;
			rc = GetEncodeSettings(pString);
		}
		break;

	case VO_PID_METADATA_COPYRIGHT:
		{
			pString = (PMetaDataString)pParam;
			rc = GetCopyright(pString);
		}
		break;

	case VO_PID_METADATA_ORIGARTIST:
		{
			pString = (PMetaDataString)pParam;
			rc = GetOrigArtist(pString);
		}
		break;

	case VO_PID_METADATA_ORIGALBUM:
		{
			pString = (PMetaDataString)pParam;
			rc = GetOrigAlbum(pString);
		}
		break;

	case VO_PID_METADATA_COMPILATION:
		{
			pString = (PMetaDataString)pParam;
			rc = GetCompilation(pString);
		}
		break;

	default:
		break;
	}

	if(VO_ERR_METADATA_OK != rc)
		return rc;

	if(pString)
	{
#ifdef _G1_FORCE_UTF16_2_UTF8
		if(VO_METADATA_TE_UTF16 == (pString->dwFlags & 0xFF))
		{
			if(!ConfirmTmpTextEnough(pString->dwBufferSize + 2))
				return VO_ERR_OUTOF_MEMORY;

			//VO_UnicodeToUTF8 call process no string terminate, so do not add terminate automatically avoid memory copy!!
			VO_S32 nBufSize = VO_UnicodeToUTF8((vowchar_t*)pString->pBuffer, pString->dwBufferSize / sizeof(vowchar_t), (char*)m_pTmpText, m_dwTmpText);

			VO_METADATA_SETTE(pString->dwFlags, VO_METADATA_TE_UTF8);
			pString->pBuffer = m_pTmpText;
			pString->dwBufferSize = nBufSize + 1;
			pString->dwFlags |= VO_METADATA_DYNAMICSTRING;
		}
#endif	//_G1_FORCE_UTF16_2_UTF8

		if(!(pString->dwFlags & VO_METADATA_NOTADDSTRINGTERMINATE))
		{
			//we should add terminate automatically!!
			if(VO_METADATA_TE_UTF16 == (pString->dwFlags & 0xFF))
			{
				//0x00 00
				if(pString->pBuffer && (pString->pBuffer[pString->dwBufferSize - 2] || pString->pBuffer[pString->dwBufferSize - 1]))
				{
					//make sure the temporary buffer is enough
					if(!ConfirmTmpTextEnough(pString->dwBufferSize + 2))
						return VO_ERR_OUTOF_MEMORY;

					//add 0x00 00
					MemCopy(m_pTmpText, pString->pBuffer, pString->dwBufferSize);
					m_pTmpText[pString->dwBufferSize] = m_pTmpText[pString->dwBufferSize + 1] = 0;
					pString->pBuffer = m_pTmpText;
					pString->dwBufferSize += 2;
					pString->dwFlags |= VO_METADATA_DYNAMICSTRING;
				}
			}
			else
			{
				//0x00
				if(pString->pBuffer && pString->pBuffer[pString->dwBufferSize - 1])
				{
					//make sure the temporary buffer is enough
					if(!ConfirmTmpTextEnough(pString->dwBufferSize + 1))
						return VO_ERR_OUTOF_MEMORY;

					//add 0x00 00
					MemCopy(m_pTmpText, pString->pBuffer, pString->dwBufferSize);
					m_pTmpText[pString->dwBufferSize] = 0;
					pString->pBuffer = m_pTmpText;
					pString->dwBufferSize++;
					pString->dwFlags |= VO_METADATA_DYNAMICSTRING;
				}
			}
		}
	}

	return VO_ERR_METADATA_OK;
}

VO_BOOL CBaseMetaData::GetGenreByIndex(VO_U8 btIdx, PMetaDataString pGenre)
{
	if(btIdx >= GENRE_TOTALCOUNT)
		return VO_FALSE;

	VO_METADATA_SETTE(pGenre->dwFlags, VO_METADATA_TE_ANSI);
	pGenre->pBuffer = (VO_PBYTE)m_arrGenre[btIdx];
	//pGenre->dwBufferSize = sizeof(m_arrGenre[btIdx]);	// = 4 permanently
	pGenre->dwBufferSize = 1 + strlen(m_arrGenre[btIdx]);

	return VO_TRUE;
}

VO_BOOL CBaseMetaData::ConfirmTmpTextEnough(VO_U32 dwSize)
{
	if(dwSize > m_dwTmpText)
	{
		m_dwTmpText = (dwSize + 0x1FFF) / 0x2000 * 0x2000;
		SAFE_MEM_FREE(m_pTmpText);
		m_pTmpText = NEW_BUFFER(m_dwTmpText);
		if(!m_pTmpText)
			return VO_FALSE;
	}

	return VO_TRUE;
}
