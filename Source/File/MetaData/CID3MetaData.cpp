#include "CID3MetaData.h"
#include "fMacros.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
#define TEXTENCODING2VO(te, flags)\
{\
	if(0 == te)\
		VO_METADATA_SETTE(flags, VO_METADATA_TE_ANSI);\
	else if(1 == te || 2 == te)\
		VO_METADATA_SETTE(flags, VO_METADATA_TE_UTF16);\
	else if(3 == te)\
		VO_METADATA_SETTE(flags, VO_METADATA_TE_UTF8);\
	else\
		VO_METADATA_SETTE(flags, VO_METADATA_TE_UNKNOWN);\
}

CID3v2Info::CID3v2Info(VO_MEM_OPERATOR* pMemOp)
	: CvoBaseMemOpr(pMemOp)
	, m_dwFrameCount(0)
	, m_pFrameHead(VO_NULL)
	, m_pFrameTail(VO_NULL)
	, m_pFileChunk(VO_NULL)
{
	MemSet(&m_Header, 0, sizeof(m_Header));
	MemSet(&m_ExtHeader, 0, sizeof(m_ExtHeader));
}

CID3v2Info::~CID3v2Info()
{
	ReleaseFrames();
}

VO_VOID CID3v2Info::ReleaseFrames()
{
	if(m_pFrameHead)
	{
		PID3v2Frame pCur = m_pFrameHead;
		PID3v2Frame pDel;
		while(pCur)
		{
			pDel = pCur;
			pCur = pCur->pNext;
			if(pDel->pData)
				MemFree(pDel->pData);
			MemFree(pDel);
		}
		m_pFrameHead = VO_NULL;
	}

	m_pFrameTail = VO_NULL;
	m_dwFrameCount = 0;
}

VO_BOOL CID3v2Info::Parse(CGFileChunk* pFileChunk, VO_U64 ullFilePos /* = 0 */)
{
	ReleaseFrames();

	m_pFileChunk = pFileChunk;
	if(!m_pFileChunk->FLocate(ullFilePos))
		return VO_FALSE;

	ptr_read_pointer(&m_Header, sizeof(ID3v2Header));
	if(MemCompare(m_Header.szTag, (VO_PTR)"ID3", 3))
		return VO_FALSE;

	VO_U32 dwSize = CALCULATE_SIZE_SYNCHSAFE(m_Header.btSize);
	VO_S32 nLeft = dwSize;
	if(m_Header.btFlags & 0x40)	//has ext header!!
	{
		ptr_read_pointer(&m_ExtHeader, sizeof(ID3v2ExtHeader));
		VO_U32 dwExtHeaderSize = CALCULATE_SIZE_SYNCHSAFE(m_ExtHeader.btSize);
		ptr_skip(dwExtHeaderSize - sizeof(ID3v2ExtHeader));

		nLeft -= dwExtHeaderSize;
	}

	VO_BOOL bFirstFrame = VO_TRUE;
	while(nLeft > 0)
	{
		PID3v2Frame pFrame = NEW_OBJ(ID3v2Frame);
		if(!pFrame)
			return VO_FALSE;

		pFrame->pData = VO_NULL;
		pFrame->pNext = VO_NULL;

		VO_S32 nRet = ParseFrame(pFrame, nLeft, bFirstFrame);
		bFirstFrame = VO_FALSE;
		if(nRet <= 0)	//error
		{
			MemFree(pFrame);

			if(0 == nRet)
				break;
			else
			{
				if(m_dwFrameCount == 0)
				{
					ReleaseFrames();

					return VO_FALSE;
				}
				else
				{
					break;
				}
			}
		}

		nLeft -= nRet;

		if(m_pFrameHead)
			m_pFrameTail = m_pFrameTail->pNext = pFrame;
		else
			m_pFrameTail = m_pFrameHead = pFrame;

		m_dwFrameCount++;
	}

	return VO_TRUE;
}

VO_S32 CID3v2Info::ParseFrame(PID3v2Frame pFrame, VO_U32 dwMaxFrameSize, VO_BOOL bFirstFrame)
{
	VO_U32 dwDataSize = 0, dwFrameSize = 0;
	if(m_Header.btVersion[0] > 2)
	{
		ptr_read_pointer(&pFrame->Header, VO_ID3V2_FRAME_HEADER_LEN);

		if(m_Header.btVersion[0] > 3)	//2.4.0 later use 32 bit synchsafe integer for frame size
			dwDataSize = CALCULATE_SIZE_SYNCHSAFE(pFrame->Header.btSize);
		else
			dwDataSize = CALCULATE_SIZE(pFrame->Header.btSize);

		dwFrameSize = dwDataSize + VO_ID3V2_FRAME_HEADER_LEN;
	}
	else
	{
		MemSet(&pFrame->Header, 0, VO_ID3V2_FRAME_HEADER_LEN);
		ptr_read_pointer(pFrame->Header.btID, 3);
		ptr_read_pointer(pFrame->Header.btSize + 1, 3);

		dwDataSize = CALCULATE_SIZE(pFrame->Header.btSize);
		dwFrameSize = dwDataSize + VO_ID3V2_FRAME_HEADER_LEN_20BEFORE;
	}

	static VO_BYTE g_btNullID[] = {0, 0, 0, 0};
	if(!dwDataSize && !MemCompare(g_btNullID, pFrame->Header.btID, sizeof(g_btNullID)))
		return 0;

	if(dwFrameSize > dwMaxFrameSize && bFirstFrame == VO_FALSE)
		return -1;

	if(dwDataSize > 0)
	{
		pFrame->pData = NEW_BUFFER(dwDataSize);
		if(!pFrame->pData)
			return 0;

		ptr_read_pointer(pFrame->pData, dwDataSize);
	}

	return dwFrameSize;
}

PID3v2Frame CID3v2Info::GetFrame(VO_U32 dwID, PID3v2Frame pStartFrame /* = 0 */)
{
	PID3v2Frame pCur = pStartFrame ? pStartFrame : m_pFrameHead;
	while(pCur)
	{
		if(!MemCompare(pCur->Header.btID, &dwID, 4))
			return pCur;

		pCur = pCur->pNext;
	}

	return VO_NULL;
}

CID3MetaData::CID3MetaData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp)
	: CBaseMetaData(pFileOp, pMemOp)
	, m_pID3v1Info(VO_NULL)
	, m_pID3v1_1Info(VO_NULL)
	, m_pID3v2Info(VO_NULL)
{
}

CID3MetaData::~CID3MetaData()
{
	Close();
}

VO_U32 CID3MetaData::Close()
{
	SAFE_MEM_FREE(m_pID3v1Info);
	SAFE_MEM_FREE(m_pID3v1_1Info);
	SAFE_DELETE(m_pID3v2Info);

	return CBaseMetaData::Close();
}

VO_U32 CID3MetaData::ReadMetaDataB()
{
	VO_BOOL rc1 = ParseV2();
	VO_BOOL rc2 = ParseV1_1();
	VO_BOOL rc3 = ParseV1();

	return (rc1 || rc2 || rc3) ? VO_ERR_METADATA_OK : VO_ERR_METADATA_NOMETADATA;
}

VO_U32 CID3MetaData::ReadMetaDataB(PMetaDataInitInfo pInitInfo)
{
	return ReadMetaDataB();
}

VO_BOOL CID3MetaData::ParseV1()
{
	if(m_llFileSize < ID3V1_INFO_LEN)
		return VO_FALSE;

	if(!m_pFileChunk->FLocate(m_llFileSize - ID3V1_INFO_LEN))
		return VO_FALSE;

	if(m_pID3v1Info)
		MemFree(m_pID3v1Info);
	m_pID3v1Info = NEW_OBJ(ID3v1Info);
	if(!m_pID3v1Info)
		return VO_FALSE;

	ptr_read_pointer(m_pID3v1Info, ID3V1_INFO_LEN);
	if(MemCompare(m_pID3v1Info->szTag, (VO_PTR)"TAG", 3))
	{
		SAFE_MEM_FREE(m_pID3v1Info);
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL	CID3MetaData::ParseV1_1()
{
	if(m_llFileSize < ID3V1_INFO_LEN)
		return VO_FALSE;

	if(!m_pFileChunk->FLocate(m_llFileSize - ID3V1_INFO_LEN))
		return VO_FALSE;

	if(m_pID3v1_1Info)
		MemFree(m_pID3v1_1Info);
	m_pID3v1_1Info = NEW_OBJ(ID3v1_1Info);
	if(!m_pID3v1_1Info)
		return VO_FALSE;

	ptr_read_pointer(m_pID3v1_1Info, ID3V1_INFO_LEN);
	if(MemCompare(m_pID3v1_1Info->szTag, (VO_PTR)"TAG", 3))
	{
		SAFE_MEM_FREE(m_pID3v1_1Info);
		return VO_FALSE;
	}
	
	if (m_pID3v1_1Info->btReserved != 0) {
		SAFE_MEM_FREE(m_pID3v1_1Info);
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CID3MetaData::ParseV2(VO_U64 ullFilePos /* = 0 */)
{
	if(m_pID3v2Info)
		delete m_pID3v2Info;
	m_pID3v2Info = new CID3v2Info(m_pMemOp);
	if(!m_pID3v2Info)
		return VO_FALSE;

	if(!m_pID3v2Info->Parse(m_pFileChunk, ullFilePos))
	{
		SAFE_DELETE(m_pID3v2Info);

		return VO_FALSE;
	}
	return VO_TRUE;
}

#define GET_ID3V1_TEXT(pDst, szSrc)\
{\
	VO_METADATA_SETTE(pDst->dwFlags, VO_METADATA_TE_ANSI);\
	pDst->pBuffer = (VO_PBYTE)szSrc;\
	pDst->dwBufferSize = sizeof(szSrc);\
}

VO_U32 CID3MetaData::GetTitle(PMetaDataString pTitle)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_TITLE_20BEFORE : FOURCC_TITLE);
		if(pFrame && GetStringByID3v2Frame(pFrame, pTitle))
			return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(pTitle, m_pID3v1_1Info->szTitle);

		return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1Info)
	{
		GET_ID3V1_TEXT(pTitle, m_pID3v1Info->szTitle);

		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetArtist(PMetaDataString pArtist)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_ARTIST_20BEFORE : FOURCC_ARTIST);
		if(pFrame && GetStringByID3v2Frame(pFrame, pArtist))
			return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(pArtist, m_pID3v1_1Info->szArtist);

		return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1Info)
	{
		GET_ID3V1_TEXT(pArtist, m_pID3v1Info->szArtist);

		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetAlbum(PMetaDataString pAlbum)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_ALBUM_20BEFORE : FOURCC_ALBUM);
		if(pFrame && GetStringByID3v2Frame(pFrame, pAlbum))
			return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(pAlbum, m_pID3v1_1Info->szAlbum);

		return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1Info)
	{
		GET_ID3V1_TEXT(pAlbum, m_pID3v1Info->szAlbum);

		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetGenre(PMetaDataString pGenre)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_GENRE_20BEFORE : FOURCC_GENRE);
		if(pFrame && GetStringByID3v2Frame(pFrame, pGenre))	{// Parsing ID3v2 Genre.
			if (pGenre->pBuffer[0] == '(' && pGenre->pBuffer[pGenre->dwBufferSize-1] == ')') {
				VO_U32 genreIndex = 0;
				for (VO_U8 i = 1; i < pGenre->dwBufferSize - 1; i++) {
					VO_U8 digit = pGenre->pBuffer[i] - 0x30;
					if ( digit <= 9 ){
						genreIndex = genreIndex * 10 + digit;
					}
				}
				// genreIndex qualification check is necessary, don't remove it and change the check order.
				if ((0 <= genreIndex && genreIndex < GENRE_TOTALCOUNT)  && GetGenreByIndex((VO_U8)genreIndex, pGenre))
					return VO_ERR_METADATA_OK;
			}
			return VO_ERR_METADATA_OK;
		}
	}

	if(m_pID3v1_1Info)
	{
		if(GetGenreByIndex(m_pID3v1_1Info->btGenre, pGenre))
			return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1Info)
	{
		if(GetGenreByIndex(m_pID3v1Info->btGenre, pGenre))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetComposer(PMetaDataString pComposer)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_COMPOSER_20BEFORE : FOURCC_COMPOSER);
		if(pFrame && GetStringByID3v2Frame(pFrame, pComposer))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetTrack(PMetaDataString pTrack)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_TRACK_20BEFORE : FOURCC_TRACK);
		if(pFrame && GetStringByID3v2Frame(pFrame, pTrack))
			return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1_1Info)
	{
		sprintf(m_szID3v1Track, "%d", m_pID3v1_1Info->btAlbumTrack);

		VO_METADATA_SETTE(pTrack->dwFlags, VO_METADATA_TE_ANSI);
		pTrack->pBuffer = (VO_PBYTE)m_szID3v1Track;
		pTrack->dwBufferSize = sizeof(m_szID3v1Track);

		return VO_ERR_METADATA_OK;
	}

	// ID3v1.0 no track info.

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetFrontCover(PMetaDataImage pFrontCover)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = VO_NULL;
		while(true)
		{
			pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_PICTURE_20BEFORE : FOURCC_PICTURE, pFrame);
			if(!pFrame)
				break;

			VO_METADATA_PICTURETYPE pt = VO_METADATA_PT_MAX;
			VO_BOOL bRet = GetPictureType(pFrame, &pt);
			if(bRet && (VO_METADATA_PT_FRONTCOVER == pt || VO_METADATA_PT_OTHER == pt) && GetImageByID3v2Frame(pFrame, pFrontCover))
				return VO_ERR_METADATA_OK;
			else
			{
				pFrame = pFrame->pNext;
				if(!pFrame)
					break;
			}
		}
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

/*
Owner identifier        <text string> $00
Identifier              <up to 64 bytes binary data>
*/
VO_U32 CID3MetaData::GetGraceNoteTagID(PMetaDataString pGraceNoteTagID)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_FILEID_20BEFORE : FOURCC_FILEID);
		if(pFrame)
		{
			VO_PBYTE p = pFrame->pData;
			//skip Owner identifier
			while(*p)
				p++;
			p++;

			VO_METADATA_SETTE(pGraceNoteTagID->dwFlags, VO_METADATA_TE_ANSI);
			pGraceNoteTagID->pBuffer = p;
			pGraceNoteTagID->dwBufferSize = CALCULATE_SIZE(pFrame->Header.btSize) - (p - pFrame->pData);

			return VO_ERR_METADATA_OK;
		}
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

/*
Text encoding          $xx
Language               $xx xx xx
Short content descrip. <text string according to encoding> $00 (00)
The actual text        <full text string according to encoding>
*//*
VO_U32 CID3MetaData::GetComment(PMetaDataString pComment)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_COMMENT_20BEFORE : FOURCC_COMMENT);
		if(pFrame)
		{
			VO_PBYTE p = pFrame->pData;

			//Text encoding
			VO_U8 btTextEncoding = *(p++);

			//skip Language
			p += 3;

			//skip Short content descrip
			if(TEXTENCODING_IS_BOMINHEAD(btTextEncoding))
				p += 2;

			if(TEXTENCODING_IS_WIDECHAR(btTextEncoding))	//end is 0x00 00
			{
				while(*p || p[1])
					p += 2;
				p += 2;
			}
			else											//end is 0x00
			{
				while(*p)
					p++;
				p++;
			}

			//The actual text
			if(TEXTENCODING_IS_BOMINHEAD(btTextEncoding)) {
				p += 2;
			}

			TEXTENCODING2VO(btTextEncoding, pComment->dwFlags);
			pComment->pBuffer = p;
			pComment->dwBufferSize = CALCULATE_SIZE(pFrame->Header.btSize) - (p - pFrame->pData);
			if (pComment->dwBufferSize == 0)
				pComment->pBuffer = 0;

			return VO_ERR_METADATA_OK;
		}
	}

	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(pComment, m_pID3v1_1Info->szComment);

		return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1Info)
	{
		GET_ID3V1_TEXT(pComment, m_pID3v1Info->szComment);

		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}*/

// rewritten edition of GetComment()
VO_U32 CID3MetaData::GetComment(PMetaDataString pComment)
{
	if (pComment->pBuffer)
		SAFE_MEM_FREE(pComment->pBuffer);

	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_COMMENT_20BEFORE : FOURCC_COMMENT);
		if(pFrame)
		{
			VO_PBYTE p = pFrame->pData;
			VO_U32 commentSize = CALCULATE_SIZE(pFrame->Header.btSize);	// comment frame size or comment text size
			VO_PBYTE pStartOfComment = NULL;	// point to the possible start of comment
			VO_PBYTE pEndOfComment = pFrame->pData + commentSize - 1;	// point to the end of a comment.
			VO_U8 btTextEncoding = *p;		// Text encoding

			switch (btTextEncoding) {
			case TEXT_ENCODING_ANSI:
			case TEXT_ENCODING_UTF8:
				if (commentSize <= 5) return VO_ERR_METADATA_NOSUCHFRAME;
				p += 5;						// skip ("3 bytes of language" + "1 byte of short content description")
				while (!*p && (p <= pEndOfComment)) p++;	// skip 0x00 bytes if exists
				if (p > pEndOfComment) return VO_ERR_METADATA_NOSUCHFRAME;
				pStartOfComment = p;
				break;
			case TEXT_ENCODING_UTF16_WITH_BOM:
			case TEXT_ENCODING_UTF16_WITHOUT_BOM:
			{
				if (commentSize <= 6) return VO_ERR_METADATA_NOSUCHFRAME;
				p += 6;						// skip ("3 bytes of language" + "2 bytes of short content description")
				pStartOfComment = p;
				VO_PBYTE temp = pEndOfComment - 1;
				while (!IsUnicodeMagic(temp) && (temp >= pStartOfComment))
				{
					temp--;		// search unicode magic number in a comment frame, skip it if exists
				}
				pStartOfComment = (temp < pStartOfComment) ? pStartOfComment : (pStartOfComment+2);
				while (!pStartOfComment[0] && !pStartOfComment[1] && (pStartOfComment < pEndOfComment))
				{
					pStartOfComment+=2;		// skip 0x0000 if exists
				}
				if (pStartOfComment >= pEndOfComment) return VO_ERR_METADATA_NOSUCHFRAME;
				break;
			}
			default:
				return VO_ERR_METADATA_ERRORDATA;
			}

			pComment->pBuffer = pStartOfComment;
			commentSize = pEndOfComment - pStartOfComment + 1;
			pComment->dwBufferSize = commentSize;
			TEXTENCODING2VO(btTextEncoding, pComment->dwFlags);

			return VO_ERR_METADATA_OK;
		}
	}

	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(pComment, m_pID3v1_1Info->szComment);

		return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1Info)
	{
		GET_ID3V1_TEXT(pComment, m_pID3v1Info->szComment);

		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetDisk(PMetaDataString pDisk)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_DISK_20BEFORE : FOURCC_DISK);
		if(pFrame && GetStringByID3v2Frame(pFrame, pDisk))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetPublisher(PMetaDataString pPublisher)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_PUBLISHER_20BEFORE : FOURCC_PUBLISHER);
		if(pFrame && GetStringByID3v2Frame(pFrame, pPublisher))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetIsrc(PMetaDataString pIsrc)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_ISRC_20BEFORE : FOURCC_ISRC);
		if(pFrame && GetStringByID3v2Frame(pFrame, pIsrc))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetYear(PMetaDataString pYear)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_YEAR_20BEFORE : FOURCC_YEAR);
		if(pFrame && GetStringByID3v2Frame(pFrame, pYear))
			return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1_1Info)
	{
		GET_ID3V1_TEXT(pYear, m_pID3v1_1Info->szYear);

		return VO_ERR_METADATA_OK;
	}

	if(m_pID3v1Info)
	{
		GET_ID3V1_TEXT(pYear, m_pID3v1Info->szYear);

		return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetBand(PMetaDataString pBand)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_BAND_20BEFORE : FOURCC_BAND);
		if(pFrame && GetStringByID3v2Frame(pFrame, pBand))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetConductor(PMetaDataString pConductor)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_CONDUCTOR_20BEFORE : FOURCC_CONDUCTOR);
		if(pFrame && GetStringByID3v2Frame(pFrame, pConductor))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetRemixer(PMetaDataString pRemixer)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_REMIXER_20BEFORE : FOURCC_REMIXER);
		if(pFrame && GetStringByID3v2Frame(pFrame, pRemixer))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetLyricist(PMetaDataString pLyricist)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_LYRICIST_20BEFORE : FOURCC_LYRICIST);
		if(pFrame && GetStringByID3v2Frame(pFrame, pLyricist))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetRadioStation(PMetaDataString pRadioStation)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->Is20Before() ? VO_NULL : m_pID3v2Info->GetFrame(FOURCC_RADIOSTATION);
		if(pFrame && GetStringByID3v2Frame(pFrame, pRadioStation))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetOrigReleaseYear(PMetaDataString pOrigReleaseYear)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = VO_NULL;
		if(m_pID3v2Info->Is20Before())
			pFrame = m_pID3v2Info->GetFrame(FOURCC_ORIGRELYEAR_20BEFORE);
		else
		{
			pFrame = m_pID3v2Info->GetFrame(FOURCC_ORIGRELYEAR);
			if(!pFrame)
				pFrame = m_pID3v2Info->GetFrame(FOURCC_ORIGRELYEAR_30);
		}

		if(pFrame && GetStringByID3v2Frame(pFrame, pOrigReleaseYear))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetOwner(PMetaDataString pOwner)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->Is20Before() ? VO_NULL : m_pID3v2Info->GetFrame(FOURCC_OWNER);
		if(pFrame && GetStringByID3v2Frame(pFrame, pOwner))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetEncoder(PMetaDataString pEncoder)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_ENCODER_20BEFORE : FOURCC_ENCODER);
		if(pFrame && GetStringByID3v2Frame(pFrame, pEncoder))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetEncodeSettings(PMetaDataString pEncodeSettings)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_ENCSETTING_20BEFORE : FOURCC_ENCSETTING);
		if(pFrame && GetStringByID3v2Frame(pFrame, pEncodeSettings))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetCopyright(PMetaDataString pCopyright)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_COPYRIGHT_20BEFORE : FOURCC_COPYRIGHT);
		if(pFrame && GetStringByID3v2Frame(pFrame, pCopyright))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetOrigArtist(PMetaDataString pOrigArtist)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_ORIGARTIST_20BEFORE : FOURCC_ORIGARTIST);
		if(pFrame && GetStringByID3v2Frame(pFrame, pOrigArtist))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetOrigAlbum(PMetaDataString pOrigAlbum)
{
	if(m_pID3v2Info)
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(m_pID3v2Info->Is20Before() ? FOURCC_ORIGALBUM_20BEFORE : FOURCC_ORIGALBUM);
		if(pFrame && GetStringByID3v2Frame(pFrame, pOrigAlbum))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_U32 CID3MetaData::GetCompilation(PMetaDataString pCompilation)
{
	if(m_pID3v2Info && VO_FALSE == m_pID3v2Info->Is20Before())
	{
		PID3v2Frame pFrame = m_pID3v2Info->GetFrame(FOURCC_COMPILATION);
		if(pFrame && GetStringByID3v2Frame(pFrame, pCompilation))
			return VO_ERR_METADATA_OK;
	}

	return VO_ERR_METADATA_NOSUCHFRAME;
}

VO_BOOL CID3MetaData::GetStringByID3v2Frame(PID3v2Frame pFrame, PMetaDataString pString)
{
	if(!pFrame || !pString)
		return VO_FALSE;

	VO_U32 dwSkipBytes = 0;
	VO_U32 dwActualFrameDataSize = 0;
	VO_U32 dwFrameDataSize = CALCULATE_SIZE(pFrame->Header.btSize);
	VO_U8 btTextEncoding;

	if ((pFrame->Header.btFlags[1] & 0x1) == 1) {	// for ID3v2.4.0
		dwActualFrameDataSize = (VO_U32)pFrame->pData[0]<<24 | (VO_U32)pFrame->pData[1]<<16 | (VO_U32)pFrame->pData[2]<<8 | (VO_U32)pFrame->pData[3];
		dwSkipBytes = 5;
		btTextEncoding = pFrame->pData[4];
	} else {			// for ID3v2.3.0
		dwSkipBytes = 1;
		btTextEncoding = pFrame->pData[0];
	}

	if(TEXTENCODING_IS_BOMINHEAD(btTextEncoding))
	{
		if(dwFrameDataSize < 4)
			return VO_FALSE;
		
		if ((pFrame->pData[1] == 0xFF) && (pFrame->pData[2] == 0xFE))
			pString->pBuffer = pFrame->pData + dwSkipBytes + 2;		// skip "encoding flag byte + 0xFF + 0xFE"
		else
			pString->pBuffer = pFrame->pData + dwSkipBytes;			// skip "encoding flag byte" only
	}
	else
	{
		if(dwFrameDataSize < 2)
			return VO_FALSE;

		pString->pBuffer = pFrame->pData + dwSkipBytes;
	}

	if ((pFrame->Header.btFlags[1] & 0x1) == 1) {	// for ID3v2.4.0
		pString->dwBufferSize = dwActualFrameDataSize - 1; // subtract "encoding flag byte"
		TEXTENCODING2VO(btTextEncoding, pString->dwFlags);
	} else {
		pString->dwBufferSize = dwFrameDataSize - (pString->pBuffer - pFrame->pData);
		TEXTENCODING2VO(btTextEncoding, pString->dwFlags);
	}

	return VO_TRUE;
}

/*
Text encoding      $xx
MIME type          <text string> $00
Picture type       $xx
Description        <text string according to encoding> $00 (00)
Picture data       <binary data>
*/
VO_BOOL CID3MetaData::GetPictureType(PID3v2Frame pFrame, VO_METADATA_PICTURETYPE* pPicType)
{
	if(!pFrame || !pPicType)
		return VO_FALSE;

	VO_PBYTE p = pFrame->pData;
	//skip Text encoding
	p++;

	//skip MIME type
	while(*p)
		p++;
	p++;

	//Picture type
	*pPicType = (*p > VO_METADATA_PT_LAST) ? VO_METADATA_PT_OTHER : VO_METADATA_PICTURETYPE(*p);

	return VO_TRUE;
}

VO_BOOL CID3MetaData::GetImageByID3v2Frame(PID3v2Frame pFrame, PMetaDataImage pImage)
{
	if(!pFrame || !pImage)
		return VO_FALSE;

	VO_U32 dwFrameDataSize = 0;

	VO_PBYTE p = pFrame->pData;
	if(m_pID3v2Info)
	{
		if(m_pID3v2Info->Is40())	// ID3v2.4.0
		{
			dwFrameDataSize = CALCULATE_SIZE_SYNCHSAFE(pFrame->Header.btSize);
			if (pFrame->Header.btFlags[1]&1)
			{
				p += 4;		// skip the actual frame data size
			}
		}
		else
		{
			dwFrameDataSize = CALCULATE_SIZE(pFrame->Header.btSize);
		}
	}
	
	//skip Text encoding
	VO_U8 btTextEncoding = *(p++);

	//MIME type
	VO_BOOL bOmitMode = VO_TRUE;
	if(!MemCompare(p, (VO_PTR)"image/", 6) || !MemCompare(p, (VO_PTR)"IMAGE/", 6))
	{
		bOmitMode = VO_FALSE;
		p += 6;
	}

	if((!MemCompare(p, (VO_PTR)"jpg", 3) || !MemCompare(p, (VO_PTR)"JPG", 3)) && !p[3])
	{
		pImage->nImageType = VO_METADATA_IT_JPEG;
		p += 4;
	}
	else if((!MemCompare(p, (VO_PTR)"jpeg", 4) || !MemCompare(p, (VO_PTR)"JPEG", 4)) && !p[4])
	{
		pImage->nImageType = VO_METADATA_IT_JPEG;
		p += 5;
	}
	else if((!MemCompare(p, (VO_PTR)"bmp", 3) || !MemCompare(p, (VO_PTR)"BMP", 3)) && !p[3])
	{
		pImage->nImageType = VO_METADATA_IT_BMP;
		p += 4;
	}
	else if((!MemCompare(p, (VO_PTR)"png", 3) || !MemCompare(p, (VO_PTR)"PNG", 3)) && !p[3])
	{
		pImage->nImageType = VO_METADATA_IT_PNG;
		p += 4;
	}
	else if((!MemCompare(p, (VO_PTR)"gif", 3) || !MemCompare(p, (VO_PTR)"GIF", 3)) && !p[3])
	{
		pImage->nImageType = VO_METADATA_IT_GIF;
		p += 4;
	}
	else
		return VO_FALSE;

	//skip Picture type
	if(!bOmitMode)
		p++;

	//skip Description
	if(TEXTENCODING_IS_WIDECHAR(btTextEncoding))	//end is 0x00 00
	{
		while(*p || p[1])
			p += 2;
		p += 2;
	}
	else											//end is 0x00
	{
		while(*p)
			p++;
		p++;
	}

	pImage->pBuffer = p;
	pImage->dwBufferSize = dwFrameDataSize - (p - pFrame->pData);
	return VO_TRUE;
}


VO_BOOL	CID3MetaData::IsUnicodeMagic(VO_BYTE magicBytes[2])
{
	if ((magicBytes[0]==0xFE && magicBytes[1]==0xFF) ||		// BE(Big Endian)
		(magicBytes[0]==0xFF && magicBytes[1]==0xFE))		// LE(Little Endian)
	{
		return VO_TRUE;
	}

	return VO_FALSE;
}
