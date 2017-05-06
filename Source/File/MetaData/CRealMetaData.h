#pragma once
#include "CBaseMetaData.h"
#include "ID3v1DataStruct.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CAutoBuffer
	: public CvoBaseMemOpr
{
public:
	CAutoBuffer(VO_MEM_OPERATOR* pMemOp);
	virtual ~CAutoBuffer();

public:
	VO_PBYTE	m_pBuffer;
};

class CRealMetaData :
	public CBaseMetaData
{
public:
	CRealMetaData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp);
	virtual ~CRealMetaData();

public:
	virtual VO_U32		Close();

protected:
	virtual VO_U32		ReadMetaDataB();
	virtual VO_U32		ReadMetaDataB(PMetaDataInitInfo pInitInfo);

	VO_BOOL				Parse();
	VO_S32				ReadHeaderInfo(VO_BOOL& bHeaderCompleted);

	VO_BOOL				Parse_Content_Description_Header();
	VO_BOOL				Parse_Metadata_Section();
	VO_BOOL				Parse_Metadata_Property();
	VO_VOID				Parse_Metadata_Property_Base(VO_PCHAR pName, VO_U32 nType, VO_PBYTE pValue);

	virtual VO_U32		GetTitle(PMetaDataString pTitle);
	virtual VO_U32		GetArtist(PMetaDataString pArtist);
	virtual VO_U32		GetAlbum(PMetaDataString pAlbum);
	virtual VO_U32		GetGenre(PMetaDataString pGenre);
	virtual VO_U32		GetLyricLine(VO_U32* pLyricLine) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetLyricByLine(PMetaDataLyricInfo pLyricInfo) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetLyricByTime(PMetaDataLyricInfo pLyricInfo) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetComposer(PMetaDataString pComposer);
	virtual VO_U32		GetTrack(PMetaDataString pTrack);
	virtual VO_U32		GetDisk(PMetaDataString pDisk);
	virtual VO_U32		GetFrontCover(PMetaDataImage pFrontCover);
	virtual VO_U32		GetGraceNoteTagID(PMetaDataString pGraceNoteTagID);
	virtual VO_U32		GetComment(PMetaDataString pComment);
	virtual VO_U32		GetPublisher(PMetaDataString pPublisher);
	virtual VO_U32		GetIsrc(PMetaDataString pIsrc);
	virtual VO_U32		GetYear(PMetaDataString pYear);
	virtual VO_U32		GetConductor(PMetaDataString pConductor);
	virtual VO_U32		GetRemixer(PMetaDataString pRemixer);
	virtual VO_U32		GetLyricist(PMetaDataString pLyricist);
	virtual VO_U32		GetRadioStation(PMetaDataString pRadioStation);
	virtual VO_U32		GetOrigReleaseYear(PMetaDataString pOrigReleaseYear);
	virtual VO_U32		GetEncoder(PMetaDataString pEncoder);
	virtual VO_U32		GetEncodeSettings(PMetaDataString pEncodeSettings);
	virtual VO_U32		GetCopyright(PMetaDataString pCopyright);
	virtual VO_U32		GetOrigArtist(PMetaDataString pOrigArtist);
	virtual VO_U32		GetOrigAlbum(PMetaDataString pOrigAlbum);

protected:
	VO_PCHAR			m_pTitle;
	VO_PCHAR			m_pArtist;
	VO_PCHAR			m_pCopyright;
	VO_PCHAR			m_pComment;

	VO_PCHAR			m_pTrack;
	VO_PCHAR			m_pYear;
	VO_PCHAR			m_pCDTOC;

	PID3v1_1Info		m_pID3v1_1Info;
	VO_CHAR				m_szID3v1Track[4];
};

#ifdef _VONAMESPACE
}
#endif
