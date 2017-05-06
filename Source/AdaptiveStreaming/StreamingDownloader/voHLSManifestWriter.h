/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voHLSManifestWriter.h

	Contains:	voHLSManifestWriter class file

	Written by:	Terry

	Change History (most recent first):
	2013-09-23		Terry			Create file

*******************************************************************************/

#ifndef _VOHLSMANIFESTWRITER_
#define _VOHLSMANIFESTWRITER_

#include "stdio.h"
#include "ManifestWriter.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define MAX_HLS_MANIFEST_DUMP_COUNT   32


#define MASTER_PLAYLIST   1
#define MEDIA_PLAYLIST    2
#define HLS_WRITER_INDEX_OFFSET_MAIN_STREAM          0
#define HLS_WRITER_INDEX_OFFSET_X_VIDEO              0x100
#define HLS_WRITER_INDEX_OFFSET_X_AUDIO              0x200
#define HLS_WRITER_INDEX_OFFSET_X_SUBTITLE           0x300

#define HLS_WRITER_CHUNK_PLAYLISTTYPE_MAINSTREAM     0
#define HLS_WRITER_CHUNK_PLAYLISTTYPE_XAUDIO         1
#define HLS_WRITER_CHUNK_PLAYLISTTYPE_XVIDEO         2
#define HLS_WRITER_CHUNK_PLAYLISTTYPE_XSUBTITLE      3



class C_VoHLSManifestWriter:public CManifestWriter
{
public:
    C_VoHLSManifestWriter();
    ~C_VoHLSManifestWriter();
    
    virtual VO_U32 WriteManifest(list_T<MANIFEST_INFO*>* pManifestList, list_T<CHUNKINFO*>* pChunkList, VO_PCHAR pManifestID, MANIFEST_GROUP** ppNewManifestGroup, VO_BOOL bEnd);
private:
    VO_U32   GenerateMasterManifestPath();
    VO_U32   GeneratePlayListManifestPath();
	VO_U32   FindMediaPlayListAndCreateDumpPathFromChunkList(list_T<MANIFEST_INFO*>* pManifestList, list_T<CHUNKINFO*>* pChunkList, VO_PCHAR pManifestID);
    VO_U32   FindMasterPlayListAndGenerateDumpPath(list_T<MANIFEST_INFO*>* pManifestList, VO_PCHAR pManifestID);
	MANIFEST_INFO*   FindMediaPlayListByChunkRootURL(list_T<MANIFEST_INFO*>* pManifestList, VO_CHAR* pRootURL);
	VO_BOOL  JudgeManifestExistInLocal(VO_CHAR* pRootURL, VO_U32 ulPlayListId);
    VO_U32   FindURIExistInLocalByIndex(VO_CHAR* pRootURL, VO_U32 ulPlayListId);


    VO_U32   CreateManifestGroup(list_T<MANIFEST_INFO*>* pManifestList, list_T<CHUNKINFO*>* pChunkList, VO_PCHAR pManifestID);
    VO_VOID  DeleteManifestGroupContext();
    VO_U32   CreateManifest(MANIFEST_INFO* pManifestNew, MANIFEST_INFO* pManifestOriginal, list_T<CHUNKINFO*>* pChunkList, VO_U32  ulManifestType, VO_U32 ulIndex);
	VO_U32   DeleteManifest(MANIFEST_INFO* pManifest);
	VO_U32   JudgePlayListType(VO_BYTE*  pManifestData);
	VO_U32   GetLine(VO_CHAR * pStrSource , VO_CHAR * pStrDest, VO_U32 ulMaxDesSize);
	VO_U32   CreateManifestNode(MANIFEST_INFO** ppManifest);
	VO_U32   WriteMasterManifest(MANIFEST_INFO* pMasterManifestNew, MANIFEST_INFO* pMasterManifestOriginal);
	VO_U32   WriteMediaManifest(MANIFEST_INFO* pMediaManifestNew, MANIFEST_INFO* pMediaManifestOriginal, list_T<CHUNKINFO*>* pChunkList, VO_U32 ulIndex);
    VO_U32   JudgeXMediaLineAndReplaceURI(VO_CHAR*   pOriginalXMediaLine, VO_CHAR*  pNewXMediaLine, VO_U32 ulNewLineSize);
	VO_U32   JudgeXStreamURILineAndReplaceURI(VO_CHAR*   pOriginalStreamURI, VO_CHAR*  pNewStreamURI, VO_U32 ulNewLineSize);
    VO_VOID  AddEndTagToManifest(MANIFEST_INFO* pManifestNew);
    VO_U32   ConvertTheKeyLine(VO_CHAR*  pManifestDownLoadPath, VO_CHAR*   pOriginalLine, VO_CHAR*  pNewLine, VO_U32 ulNewLineSize);
    VO_U32   GetTheAbsolutePath(VO_CHAR* pstrDes, VO_CHAR* pstrInput, VO_CHAR* pstrRefer);
    VO_VOID  MakeCombinedStringForMaster(VO_CHAR*   pOutputString);   
    VO_U32   FindAttrValueByName(VO_CHAR*   pOriginalXMediaLine, VO_CHAR*  pAttrValue, VO_U32 ulAttrValueSize, VO_CHAR*   pAttrName);
    VO_U32   FindPlayListIdByType(VO_U32  ulType);
	
	MANIFEST_GROUP*    m_pManifestGroup;
    MANIFEST_INFO*     m_pMediaManifestArrayNew[MAX_HLS_MANIFEST_DUMP_COUNT];
    MANIFEST_INFO*     m_pMediaManifestArrayOriginal[MAX_HLS_MANIFEST_DUMP_COUNT];
    VO_CHAR*           m_pMediaManifestCompareString[MAX_HLS_MANIFEST_DUMP_COUNT];
    VO_U32             m_aMediaManifestId[MAX_HLS_MANIFEST_DUMP_COUNT];
    VO_U32             m_ulMediaManifestCount;
    VO_BOOL            m_bExistAudioSegment;
	VO_BOOL            m_bExistVideoSegment;
	VO_BOOL            m_bExistSubtitleSegment;

	MANIFEST_INFO*     m_pMasterManifestNew;
	MANIFEST_INFO*     m_pMasterManifestOriginal;
    
};

#ifdef _VONAMESPACE
}
#endif

#endif


