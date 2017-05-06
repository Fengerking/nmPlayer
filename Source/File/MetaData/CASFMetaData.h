#pragma once
#include "CBaseMetaData.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct tagAsfString
{
	VO_U16		wSize;
	VO_PBYTE	pContent;
} AsfString, *PAsfString;

class CASFMetaData :
	public CBaseMetaData
{
public:
	CASFMetaData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp);
	virtual ~CASFMetaData();

public:
	virtual VO_U32		Close();

protected:
	virtual VO_U32		ReadMetaDataB();
	virtual VO_U32		ReadMetaDataB(PMetaDataInitInfo pInitInfo);

	VO_BOOL				Parse();
	VO_S32				ReadHeaderInfo();
	VO_U32				ReadExtHeaderInfo();

	VO_BOOL				Parse_Metadata(VO_PBYTE pName, VO_U16 wDataType, VO_U32 dwDataLen);

	VO_BOOL				Parse_Content_Description_Object();
	VO_BOOL				Parse_Extended_Content_Description_Object();
	VO_BOOL				Parse_Metadata_Object(VO_BOOL bLibrary);

	virtual VO_U32		GetTitle(PMetaDataString pTitle);
	virtual VO_U32		GetArtist(PMetaDataString pArtist);
	virtual VO_U32		GetAlbum(PMetaDataString pAlbum);
	virtual VO_U32		GetBand(PMetaDataString pBand);
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
	//ASF_Content_Description_Object
	PAsfString			m_pTitle;
	PAsfString			m_pArtist;
	PAsfString			m_pCopyright;
	PAsfString			m_pComment;

	//ASF_Extended_Content_Description_Object
	PAsfString			m_pGenre;
	PAsfString			m_pGenreID;
	PAsfString			m_pAlbum;
	PAsfString			m_pBand;	//Band(Album Artist)
	PAsfString			m_pComposer;
	PAsfString			m_pPublisher;
	PAsfString			m_pIsrc;
	PAsfString			m_pYear;
	PAsfString			m_pConductor;
	PAsfString			m_pRemixer;
	PAsfString			m_pLyricist;
	PAsfString			m_pRadioStation;
	PAsfString			m_pOrigReleaseYear;
	PAsfString			m_pEncoder;
	PAsfString			m_pEncodingSettings;
	PAsfString			m_pOrigArtist;
	PAsfString			m_pOrigAlbum;
	PAsfString			m_pSyncLyrics;
	PAsfString			m_pGraceNoteTagID;
	PAsfString			m_pTrack;
	PAsfString			m_pDisk;

	PMetaDataImage		m_pImage;

	VO_U32				m_nTrack;
	VO_CHAR				m_szTrack[11];	//for U32
};

#ifdef _VONAMESPACE
}
#endif
