#ifndef __CMP4MetaData_H__
#define __CMP4MetaData_H__

#include "CID3MetaData.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct tagMp4String
{
	VO_U8		btLang;		//VO_METADATA_TE_XXX
	VO_U16		wSize;
	VO_PBYTE	pContent;
} Mp4String, *PMp4String;

class CMP4MetaData :
	public CID3MetaData
{
public:
	CMP4MetaData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp);
	virtual ~CMP4MetaData();

public:
	virtual VO_U32		Close();

protected:
	virtual VO_U32		ReadMetaDataB();
	virtual VO_U32		ReadMetaDataB(PMetaDataInitInfo pInitInfo);

	virtual VO_U32		GetTitle(PMetaDataString pTitle);
	virtual VO_U32		GetArtist(PMetaDataString pArtist);
	virtual VO_U32		GetAlbum(PMetaDataString pAlbum);
	virtual VO_U32		GetBand(PMetaDataString pBand);
	virtual VO_U32		GetGenre(PMetaDataString pGenre);
	virtual VO_U32		GetComposer(PMetaDataString pComposer);
	virtual VO_U32		GetTrack(PMetaDataString pTrack);
	virtual VO_U32		GetFrontCover(PMetaDataImage pFrontCover);
	virtual VO_U32		GetGraceNoteTagID(PMetaDataString pGraceNoteTagID);
	virtual VO_U32		GetComment(PMetaDataString pComment);
	virtual VO_U32		GetDisk(PMetaDataString pDisk);
	virtual VO_U32		GetPublisher(PMetaDataString pPublisher);
	virtual VO_U32		GetYear(PMetaDataString pYear);
	virtual VO_U32		GetEncoder(PMetaDataString pEncoder);
	virtual VO_U32		GetEncodeSettings(PMetaDataString pEncodeSettings);
	virtual VO_U32		GetCopyright(PMetaDataString pCopyright);

	VO_BOOL				Parse();

	//base find box function
	//return value: box content size(not box size!!), -1 mean can not find
	VO_U32				FindBoxB(VO_U32 fccType, VO_U32 dwSize = 0xFFFFFFFF);

	VO_BOOL				ParseTags_iTunes(VO_U32 dwBoxContSize);
	VO_BOOL				ReadTagContent_iTunes(PMp4String* pString);
	VO_BOOL				ReadUDTagContent_iTunes();

	VO_BOOL				ParseTags_TS26244(VO_U32 dwBoxContSize);
	VO_BOOL				ReadTagContent_TS26244(VO_U32 dwBoxContSize, PMp4String* pString);

protected:
	PMp4String			m_pTitle;
	PMp4String			m_pArtist;
	PMp4String			m_pAlbum;
	PMp4String			m_pBand;	// Band(Album Artist)
	PMp4String			m_pGenre;
	PMp4String			m_pComment;
	PMp4String			m_pComposer;
	PMp4String			m_pCopyright;
	PMp4String			m_pYear;
	PMp4String			m_pGracenoteTagID;
	PMp4String			m_pPublisher;
	PMp4String			m_pEncoder;
	PMp4String			m_pEncodingSettings;

	PMp4String			m_pTrack;
	PMp4String			m_pDisk;

	PMetaDataImage		m_pCover;
};
#ifdef _VONAMESPACE
}
#endif
#endif	//__CMP4MetaData_H__