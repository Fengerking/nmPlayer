#pragma once
#include "CBaseMetaData.h"
#include "../ID3/ID3v1DataStruct.h"
#include "../ID3/ID3v2DataStruct.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CID3v2Info
	: public CvoBaseObject
	, public CvoBaseMemOpr
{
public:
	CID3v2Info(VO_MEM_OPERATOR* pMemOp);
	virtual ~CID3v2Info();

public:
	VO_BOOL				Parse(CGFileChunk* pFileChunk, VO_U64 ullFilePos = 0);
	VO_BOOL				Is20Before() {return (m_Header.btVersion[0] > 2) ? VO_FALSE : VO_TRUE;}
	VO_BOOL				Is40() {return (m_Header.btVersion[0] == 4) ? VO_TRUE : VO_FALSE;}
	PID3v2Frame			GetFrame(VO_U32 dwID, PID3v2Frame pStartFrame = 0);

protected:
	/* return value:
	0:	error
	-1:	error(beyond max frame size)
	>0:	success, return frame size
	*/
	VO_S32				ParseFrame(PID3v2Frame pFrame, VO_U32 dwMaxFrameSize, VO_BOOL bFirstFrame);
	VO_VOID				ReleaseFrames();

protected:
	//headers
	ID3v2Header			m_Header;
	ID3v2ExtHeader		m_ExtHeader;

	//frames
	VO_U32				m_dwFrameCount;
	PID3v2Frame			m_pFrameHead;
	PID3v2Frame			m_pFrameTail;

	CGFileChunk*		m_pFileChunk;
};

class CID3MetaData :
	public CBaseMetaData
{
public:
	CID3MetaData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp);
	virtual ~CID3MetaData();

public:
	virtual VO_U32		Close();

protected:
	virtual VO_U32		ReadMetaDataB();
	virtual VO_U32		ReadMetaDataB(PMetaDataInitInfo pInitInfo);

	VO_BOOL				ParseV1();
	VO_BOOL				ParseV1_1();
	VO_BOOL				ParseV2(VO_U64 ullFilePos = 0);

	virtual VO_U32		GetTitle(PMetaDataString pTitle);
	virtual VO_U32		GetArtist(PMetaDataString pArtist);
	virtual VO_U32		GetAlbum(PMetaDataString pAlbum);
	virtual VO_U32		GetGenre(PMetaDataString pGenre);
	virtual VO_U32		GetLyricLine(VO_U32* pLyricLine) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetLyricByLine(PMetaDataLyricInfo pLyricInfo) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetLyricByTime(PMetaDataLyricInfo pLyricInfo) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetComposer(PMetaDataString pComposer);
	virtual VO_U32		GetTrack(PMetaDataString pTrack);
	virtual VO_U32		GetFrontCover(PMetaDataImage pFrontCover);
	virtual VO_U32		GetGraceNoteTagID(PMetaDataString pGraceNoteTagID);
	virtual VO_U32		GetComment(PMetaDataString pComment);
	virtual VO_U32		GetDisk(PMetaDataString pDisk);
	virtual VO_U32		GetPublisher(PMetaDataString pPublisher);
	virtual VO_U32		GetIsrc(PMetaDataString pIsrc);
	virtual VO_U32		GetYear(PMetaDataString pYear);
	virtual VO_U32		GetBand(PMetaDataString pBand);
	virtual VO_U32		GetConductor(PMetaDataString pConductor);
	virtual VO_U32		GetRemixer(PMetaDataString pRemixer);
	virtual VO_U32		GetLyricist(PMetaDataString pLyricist);
	virtual VO_U32		GetRadioStation(PMetaDataString pRadioStation);
	virtual VO_U32		GetOrigReleaseYear(PMetaDataString pOrigReleaseYear);
	virtual VO_U32		GetOwner(PMetaDataString pOwner);
	virtual VO_U32		GetEncoder(PMetaDataString pEncoder);
	virtual VO_U32		GetEncodeSettings(PMetaDataString pEncodeSettings);
	virtual VO_U32		GetCopyright(PMetaDataString pCopyright);
	virtual VO_U32		GetOrigArtist(PMetaDataString pOrigArtist);
	virtual VO_U32		GetOrigAlbum(PMetaDataString pOrigAlbum);
	virtual VO_U32		GetCompilation(PMetaDataString pCompilation);

	VO_BOOL				GetStringByID3v2Frame(PID3v2Frame pFrame, PMetaDataString pString);
	VO_BOOL				GetPictureType(PID3v2Frame pFrame, VO_METADATA_PICTURETYPE* pPicType);
	VO_BOOL				GetImageByID3v2Frame(PID3v2Frame pFrame, PMetaDataImage pImage);

	VO_BOOL				IsUnicodeMagic(VO_BYTE magicBytes[2]);
protected:
	PID3v1Info			m_pID3v1Info;
	PID3v1_1Info		m_pID3v1_1Info;
	CID3v2Info*			m_pID3v2Info;

	VO_CHAR				m_szID3v1Track[4];
};
#ifdef _VONAMESPACE
}
#endif