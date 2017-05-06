#ifndef __CBaseMetaData_H__
#define __CBaseMetaData_H__

#include "voMetaData.h"
#include "CvoBaseFileOpr.h"
#include "CvoBaseMemOpr.h"
#include "CvoBaseObject.h"
#include "CGFileChunk.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define GENRE_TOTALCOUNT		148
#define GENRE_LEN				50

#define VO_METADATA_SETTE(flags, te)	flags = (flags & 0xFFFFFF00) | te

/**
* meta data type
*/
typedef enum
{
	VO_METADATA_UNKNOWN		= 0, 
	VO_METADATA_ID3, 
	VO_METADATA_WM,	
	VO_METADATA_iTunes, 
	VO_METADATA_TS26244, 
	VO_METADATA_RM, 
	VO_METADATA_TYPE_MAX	= VO_MAX_ENUM_VALUE
} VO_METADATA_TYPE;

/**
* Initialize Information Structure
*/
typedef struct tagMetaDataInitInfo 
{
	VO_METADATA_TYPE	eType;
	/* VO_MAXU64 mean not valid
1.	[ID3]ID3v2, file position
	[WM]ASF_Content_Description_Object
	[iTunes][TS26244]file position
2.	[WM]ASF_Extended_Content_Description_Object
3.	[WM]ASF_Metadata_Object
4.	[WM]ASF_Metadata_Library_Object
	*/
	VO_U64				ullFilePositions[8];
	VO_U64				ullFileSize;
	//reserved 0
	//[iTunes][TS26244]box size
	VO_U32				dwReserved;
	VO_PTR				hFile;
} MetaDataInitInfo, *PMetaDataInitInfo;

/**
* Picture Type
*/
#define VO_METADATA_PT_LAST			VO_METADATA_PT_STUDIOLOGO
typedef enum
{
	VO_METADATA_PT_OTHER = 0,		//Other
	VO_METADATA_PT_FILEICON,		//32x32 pixels 'file icon' (PNG only)
	VO_METADATA_PT_OTHERFILEICON,	//Other file icon
	VO_METADATA_PT_FRONTCOVER,		//Cover (front)
	VO_METADATA_PT_BACKCOVER,		//Cover (back)
	VO_METADATA_PT_LEAFLET,			//Leaflet page
	VO_METADATA_PT_MEDIA,			//Media (e.g. label side of CD)
	VO_METADATA_PT_LEADARTIST,		//Lead artist/lead performer/soloist
	VO_METADATA_PT_ARTIST,			//Artist/performer
	VO_METADATA_PT_CONDUCTOR,		//Conductor
	VO_METADATA_PT_BAND,			//Band/Orchestra
	VO_METADATA_PT_COMPOSER,		//Composer
	VO_METADATA_PT_LYRICIST,		//Lyricist/text writer
	VO_METADATA_PT_RECLOCATION,		//Recording Location
	VO_METADATA_PT_DURINGREC,		//During recording
	VO_METADATA_PT_DURINGPERF,		//During performance
	VO_METADATA_PT_SCREENCAP,		//Movie/video screen capture
	VO_METADATA_PT_FISH,			//A bright colored fish
	VO_METADATA_PT_ILLUSTRATION,	//Illustration
	VO_METADATA_PT_BANDLOGO,		//Band/artist logotype
	VO_METADATA_PT_STUDIOLOGO,		//Publisher/Studio logotype
	VO_METADATA_PT_MAX	= VO_MAX_ENUM_VALUE
} VO_METADATA_PICTURETYPE;

class CBaseMetaData
	: public CvoBaseFileOpr
	, public CvoBaseMemOpr
	, public CvoBaseObject
{
public:
	CBaseMetaData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp);
	virtual ~CBaseMetaData();

public:
	virtual VO_U32		Load(VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Load(PMetaDataInitInfo pInitInfo);
	virtual VO_U32		Close();
	virtual VO_U32		SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);

protected:
	virtual VO_U32		ReadMetaDataB() = 0;
	virtual VO_U32		ReadMetaDataB(PMetaDataInitInfo pInitInfo) = 0;

	virtual VO_U32		GetTitle(PMetaDataString pTitle) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetArtist(PMetaDataString pArtist) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetAlbum(PMetaDataString pAlbum) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetGenre(PMetaDataString pGenre) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetLyricLine(VO_U32* pLyricLine) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetLyricByLine(PMetaDataLyricInfo pLyricInfo) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetLyricByTime(PMetaDataLyricInfo pLyricInfo) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetComposer(PMetaDataString pComposer) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetTrack(PMetaDataString pTrack) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetFrontCover(PMetaDataImage pFrontCover) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetGraceNoteTagID(PMetaDataString pGraceNoteTagID) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetComment(PMetaDataString pComment) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetDisk(PMetaDataString pDisk) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetPublisher(PMetaDataString pPublisher) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetIsrc(PMetaDataString pIsrc) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetYear(PMetaDataString pYear) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetBand(PMetaDataString pBand) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetConductor(PMetaDataString pConductor) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetRemixer(PMetaDataString pRemixer) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetLyricist(PMetaDataString pLyricist) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetRadioStation(PMetaDataString pRadioStation) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetOrigReleaseYear(PMetaDataString pOrigReleaseYear) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetOwner(PMetaDataString pOwner) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetEncoder(PMetaDataString pEncoder) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetEncodeSettings(PMetaDataString pEncodeSettings) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetCopyright(PMetaDataString pCopyright) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetOrigArtist(PMetaDataString pOrigArtist) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetOrigAlbum(PMetaDataString pOrigAlbum) {return VO_ERR_METADATA_NOSUCHFRAME;}
	virtual VO_U32		GetCompilation(PMetaDataString pCompilation) {return VO_ERR_METADATA_NOSUCHFRAME;}

	VO_BOOL				GetGenreByIndex(VO_U8 btIdx, PMetaDataString pGenre);

	inline VO_BOOL		ConfirmTmpTextEnough(VO_U32 dwSize);

protected:
	static VO_CHAR		m_arrGenre[GENRE_TOTALCOUNT][GENRE_LEN];

	VO_S64				m_llFileSize;
	CGFileChunk*		m_pFileChunk;

	VO_PBYTE			m_pTmpText;
	VO_U32				m_dwTmpText;
};
#ifdef _VONAMESPACE
}
#endif
#endif	//__CBaseMetaData_H__