#ifndef __VO_HLS_MANAGER_H__

#define __VO_HLS_MANAGER_H__


//#ifdef WIN32
//#include "memorycheck.h"
//#endif
#include "hls_parser.h"
#include "voSource2.h"
#include "voAdaptiveStreamParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define INVALID_PLALIST_ID                     0xffffffff
#define MAX_X_MEDIA_GROUP_ID_LEN               64
#define MAX_X_STREAM_CODEC_DESC_LEN            64
#define MAX_X_MEDIA_NAME_DESC_LEN              64
#define MAX_X_MEDIA_LANGUAGE_DESC_LEN          64
#define MAX_STREAM_MAX_MEDIA_COUNT             32
#define MAX_REQUEST_CHUNCK_ITEM_COUNT          4
#define MAX_CHARACTERISTICS_LENGTH             1024

#define HLS_INDEX_OFFSET_MAIN_STREAM          0x1
#define HLS_INDEX_OFFSET_X_VIDEO              0x100
#define HLS_INDEX_OFFSET_X_AUDIO              0x200
#define HLS_INDEX_OFFSET_X_SUBTITLE           0x300
#define HLS_INDEX_OFFSET_X_CC                 0x400
#define HLS_INDEX_OFFSET_I_FRAME_STREAM       0x500

enum E_ITEM_FLAG_FROM_HLS
{
    E_NORMAL,
    E_DISCONTINUE,
    E_UNKNOWN,
};

enum E_CHUNCK_STATE
{
	E_CHUNCK_FORCE_NEW_STREAM,
	E_CHUNCK_NEW_STREAM,
	E_CHUNCK_SMOOTH_ADAPTION,
	E_CHUNCK_SMOOTH_ADAPTION_EX,
	E_CHUNCK_NORMAL,
	E_UNKNOWN_STATE,
};

enum E_PLAYLIST_TYPE
{
    E_VARIANT_STREAM,
    E_MAIN_STREAM,
    E_X_MEDIA_VIDEO_STREAM,
    E_X_MEDIA_AUDIO_STREAM,
    E_X_MEDIA_SUBTITLE_STREAM,
    E_X_MEDIA_CAPTION_STREAM,
    E_I_FRAME_STREAM,
    E_UNKNOWN_STREAM,
};

enum E_ITEM_TYPE
{
    E_NORMAL_WHOLE_CHUNCK_NODE,
    E_NORMAL_PART_CHUNCK_NODE,
    E_DISCONTINUITY_CHUNCK_NODE,
    E_PRIVATE_NODE,
    E_UNKNOWN_NODE,
};

typedef   struct S_MAIN_STREAM_ATTR
{
	VO_U32                      ulBitrate;
    VO_CHAR                     strCodecDesc[MAX_X_STREAM_CODEC_DESC_LEN];
    VO_CHAR                     strVideoAlterGroup[MAX_X_MEDIA_GROUP_ID_LEN];
    VO_CHAR                     strAudioAlterGroup[MAX_X_MEDIA_GROUP_ID_LEN];
    VO_CHAR                     strSubTitleAlterGroup[MAX_X_MEDIA_GROUP_ID_LEN];	
	VO_CHAR                     strClosedCaptionGroup[MAX_X_MEDIA_GROUP_ID_LEN];
    S_RESOLUTION                sResolution;
}S_MAIN_STREAM_ATTR;


typedef   struct S_CHUNCK_ITEM
{
    VO_CHAR               strChunkParentURL[1024];
    VO_CHAR               strChunckItemURL[1024];
    VO_U64                ullChunckOffset;
    VO_U64                ullChunckLen;
    VO_CHAR               strChunckItemTitle[64];
    VO_U32                ulDurationInMsec;
    VO_U32                ulChunckSequenceId;
    E_ITEM_TYPE           eChunckContentType;
    VO_U32                ulPrivateID;     //Dis Number
    VO_U32                ulPlayListId;
    VO_U32                ulDisSequenceId;
    VO_U64                ullBeginTime;
    VO_U64                ullEndTime;
    VO_U64                ullTimeStampOffset;
	VO_U64                ullProgramDateTime;
    VO_U32                nFlagForTimeStamp;
    VO_U32                ulSequenceIDForKey;
    VO_CHAR               strEXTKEYLine[1024];
    VO_BOOL               bDisOccur;
    VO_CHAR               strXMapURL[1024];
    VO_U64                ullXMapOffset;
    VO_U64                ullXMapLen;
    VO_VOID*              pReserve;
    E_CHUNCK_STATE        eChunkState;
    S_CHUNCK_ITEM*        pNext;
}S_CHUNCK_ITEM;

typedef   struct S_I_FRAME_STREAM_ATTR
{
    VO_U32                      ulBitrate;
    VO_CHAR                     strCodecDesc[MAX_X_STREAM_CODEC_DESC_LEN];
}S_I_FRAME_STREAM_ATTR;

typedef   struct S_X_MEDIA_STREAM_ATTR
{
    E_PLAYLIST_TYPE             eStreamType;
    VO_CHAR                     strGroupId[MAX_X_MEDIA_GROUP_ID_LEN];	
    VO_CHAR                     strName[MAX_X_MEDIA_NAME_DESC_LEN];
    VO_CHAR                     strLanguage[MAX_X_MEDIA_LANGUAGE_DESC_LEN];	
	VO_CHAR                     strAssocLanguage[MAX_X_MEDIA_LANGUAGE_DESC_LEN];	
	VO_CHAR                     strCharacteristics[MAX_CHARACTERISTICS_LENGTH];	
	VO_U32                      ulAutoSelect;
    VO_U32                      ulDefault;
    VO_U32                      ulForced;
    VO_U32                      ulInStreamId;
}S_X_MEDIA_STREAM_ATTR;

typedef   struct S_PLAYLIST_NODE
{
    S_CHUNCK_ITEM*    pChunkItemHeader;
    S_CHUNCK_ITEM*    pChunkItemTail;
	M3U_MANIFEST_TYPE    eManifestType;
	M3U_CHUNCK_PLAYLIST_TYPE    eChuckPlayListType;
    M3U_CHUNCK_PLAYLIST_TYPE_EX eChunkPlayListTypeEx;
	VO_CHAR                     strShortURL[1024];
    VO_CHAR                     strRootURL[1024];
    VO_CHAR                     strInputURL[1024];
    E_PLAYLIST_TYPE             ePlayListType;
	S_PLAYLIST_NODE*            pNext;
	union
	{
	    S_MAIN_STREAM_ATTR    sVarMainStreamAttr;
        S_I_FRAME_STREAM_ATTR sVarIFrameSteamAttr;
        S_X_MEDIA_STREAM_ATTR sVarXMediaStreamAttr;
	};
    VO_U32                      ulItemCount;
    VO_U32                      ulPlayListId;
    VO_U32                      ulExtraIFramePlayListId;
    VO_U32                      ulCurrentMinSequenceIdInDvrWindow;
    VO_U32                      ulCurrentMaxSequenceIdInDvrWindow;
    VO_U32                      ulCurrentDvrDuration;
    VO_U32                      ulTargetDuration;
    VO_U32                      ulLastChunkDuration;
    VO_U32                      ulXStartExist;
    VO_S32                      ilXStartValue;	
}S_PLAYLIST_NODE;


typedef    struct S_PLAY_SESSION
{
    S_PLAYLIST_NODE*    pStreamPlayListNode;
    VO_U32              ulMainStreamSequenceId;
    E_CHUNCK_STATE      eMainStreamInAdaptionStreamState;

	
    S_PLAYLIST_NODE*    pAlterVideoPlayListNode;
    VO_U32              ulAlterVideoSequenceId;
    E_CHUNCK_STATE      eAlterVideoInAdaptionStreamState;
	
    S_PLAYLIST_NODE*    pAlterAudioPlayListNode;
    VO_U32              ulAlterAudioSequenceId;
    E_CHUNCK_STATE      eAlterAudioInAdaptionStreamState;

    S_PLAYLIST_NODE*    pAlterSubTitlePlayListNode;
    VO_U32              ulAlterSubTitleSequenceId;
    E_CHUNCK_STATE      eAlterSubTitleInAdaptionStreamState;


    S_PLAYLIST_NODE*    pIFramePlayListNode;	
    VO_U32              ulIFrameSequenceId;
}S_PLAY_SESSION;


typedef   struct
{
    VO_CHAR    strStreamVideoGroupDesc[MAX_X_MEDIA_GROUP_ID_LEN];
    VO_U32     ulAlterVideoCount;
	VO_U32*    pAlterVideoPlayListIdArray[MAX_STREAM_MAX_MEDIA_COUNT];

	VO_CHAR    strStreamAudioGroupDesc[MAX_X_MEDIA_GROUP_ID_LEN];
    VO_U32     ulAlterAudioCount;
	VO_U32*    pstrAlterAudioDescArray[MAX_STREAM_MAX_MEDIA_COUNT];

    VO_CHAR    strStreamSubTitleGroupDesc[MAX_X_MEDIA_GROUP_ID_LEN];
    VO_U32     ulAlterSubTitleoCount;	
	VO_CHAR*   pstrAlterSubTitleDescArray[MAX_STREAM_MAX_MEDIA_COUNT];	
	
	VO_CHAR    strStreamClosedCaptionGroupDesc[MAX_X_MEDIA_GROUP_ID_LEN];
	VO_U32     ulAlterClosedCaptionCount;	
	VO_CHAR*   pstrAlterClosedCaptionDescArray[MAX_STREAM_MAX_MEDIA_COUNT];	

	VO_CHAR    strCodecDesc[MAX_X_STREAM_CODEC_DESC_LEN];
    VO_U32     ulBitrate;
	VO_U32     ulWidth;
	VO_U32     ulHeight;
}S_STREAM_INFO;


typedef   struct
{
    S_CHUNCK_ITEM   aChunckItems[MAX_REQUEST_CHUNCK_ITEM_COUNT];
    VO_U32          ulChunckCount;
}S_REQUEST_ITEM;

typedef   struct
{
    VO_U32              ulAvailable;
	VO_U32              ulCurrentMainStreamPlayListId;
    VO_U32              ulMainStreamSequenceId;	
    E_CHUNCK_STATE      eMainStreamInAdaptionStreamState;

	VO_U32              ulCurrentAlterVideoStreamPlayListId;
    VO_U32              ulAlterVideoStreamSequenceId;	
    E_CHUNCK_STATE      eAlterVideoStreamInAdaptionStreamState;

	VO_U32              ulCurrentAlterAudioStreamPlayListId;
    VO_U32              ulAlterAudioStreamSequenceId;	
    E_CHUNCK_STATE      eAlterAudioStreamInAdaptionStreamState;

    VO_U32              ulCurrentAlterSubTitleStreamPlayListId;
    VO_U32              ulAlterSubTitleStreamSequenceId;
    E_CHUNCK_STATE      eAlterSubTitleStreamInAdaptionStreamState;
}S_SESSION_CONTEXT;


class C_M3U_Manager
{
public:
    C_M3U_Manager();
    ~C_M3U_Manager();
    VO_U32    ParseManifest(VO_BYTE* pPlayListContent, VO_U32 ulPlayListContentLength, VO_CHAR*  pPlayListURL, VO_U32 ulPlayListId);
    VO_U32    BuildMasterPlayList(VO_CHAR*  pPlayListURL);
	VO_U32    BuildMediaPlayList(VO_CHAR*  pPlayListURL, VO_U32  ulPlayListId);
    S_PLAYLIST_NODE*    FindPlayListById(VO_U32  ulPlayListId);
    VO_BOOL   IsPlaySessionReady();
    VO_U32    GetCurReadyPlaySession(S_PLAY_SESSION**  ppPlaySession);
    VO_U32    SetStartPosForLiveStream();
    VO_U32    GetRootManifestType(M3U_MANIFEST_TYPE*  pRootManfestType);
    S_PLAYLIST_NODE*    GetPlayListNeedParseForSessionReady();
    VO_U32    SetPlayListToSession(VO_U32 ulPlayListId);
    VO_U32    AdjustXMedia();
	VO_U32    AdjustChunkPosInListForBA(VO_SOURCE2_ADAPTIVESTREAMING_CHUNKPOS sPrepareChunkPos);
	VO_U32    SetThePos(VO_U32   ulTime, VO_BOOL*   pbNeedResetParser,  VO_U32* pulTimeChunkOffset,  VO_ADAPTIVESTREAMPARSER_SEEKMODE sSeekMode);
	VO_U32    GetTheDuration(VO_U32* pTimeDuration);
    VO_VOID   ReleaseAllPlayList();
    VO_U32    GetTheEndTimeForLiveStream();
    VO_U32    GetTheDvrDurationForLiveStream();
    VO_U32    GetTheLiveTimeForLiveStream();
    VO_U32    GetChunkOffsetValueBySequenceId(VO_U32  ulSequenceId, VO_U32* pTimeOffset);
    VO_U32    GetTheDvrEndLengthForLiveStream(VO_U64*   pEndLength);	
    VO_U32    GetCurrentProgreamStreamType(VO_SOURCE2_PROGRAM_TYPE*   peProgramType);
    VO_VOID   SetUTCTime(VO_U64*   pUTCTime);
    VO_VOID   SetLiveLatencyValue(VO_U32*  pLiveLatencyValue);
    VO_VOID   SetCurrentSequenceIdForPlayList(VO_VOID*  pChunkInfo);
	VO_U32    GetChunckItemIntervalTime();
	VO_U32    GetMainStreamArray(S_PLAYLIST_NODE**  pPlayListNodeArray, VO_U32 ulArrayMaxSize, VO_U32*   pulArraySize);
	VO_U32    GetMainStreamCount(VO_U32*   pulArraySize);
	VO_U32    GetXMediaStreamArrayWithGroupAndType(S_PLAYLIST_NODE**  pPlayListNodeArray, VO_CHAR* pGroupId, E_PLAYLIST_TYPE  ePlayListType, VO_U32 ulArrayMaxSize, VO_U32*   pulArraySize);
	VO_U32    GetXMediaStreamCountWithGroupAndType(VO_CHAR* pGroupId, E_PLAYLIST_TYPE  ePlayListType, VO_U32*   pulArraySize);
    VO_U32    GetCurrentChunk(E_PLAYLIST_TYPE  ePlayListType, S_CHUNCK_ITEM*   pChunkItems);
    VO_VOID   ResetSessionContext();
    S_PLAYLIST_NODE*    FindTargetPlayListWithTrackTypeAndId(VO_SOURCE2_TRACK_TYPE nType, VO_U32 ulTrackId);
    VO_VOID   AdjustLiveTimeAndDeadTimeForLive(S_PLAYLIST_NODE* pPlayList);
    S_PLAYLIST_NODE*    FindTheFirstMainStream();
    VO_VOID   ResetPlayListContentForLiveUpdate(S_PLAYLIST_NODE* pPlayList);
    VO_U32    GetPlayListStartOffset(S_PLAYLIST_NODE* pPlayList);
    VO_U32    GetCurrentSessionDurationByChapterId(VO_U32 uChapterId, VO_U32*   pTimeOutput);	
    VO_U32    AdjustSequenceIdInSession();	
    VO_VOID   BackupCurrentContext();
    VO_VOID   ResetContextForUpdateURL();
	S_SESSION_CONTEXT*    GetSessionContext();
    VO_VOID   RestoreCurrentContext();
    VO_VOID   PrepareSessionByMainStreamDefaultSetting(S_PLAYLIST_NODE* pPlayList);
    VO_U32    SeekForOneTrackOnly(VO_U32  ulPlaylistId, VO_U64 ullTimeOffset, VO_U32* pNewOffset);

private:
	VO_VOID   ReleasePlayList(S_PLAYLIST_NODE*   pPlayListNode);
    VO_U32    CreatePlayList(S_PLAYLIST_NODE**   ppPlayListNode);
	VO_U32    MakeChunkPlayList(S_PLAYLIST_NODE*   ppPlayListNode);
    VO_U32    AssembleChunkItem(S_TAG_NODE* pTagInf, S_TAG_NODE* pURI, S_TAG_NODE* pTagNodeKeyLine, VO_S64  illProgramValue, 
		                        VO_S64 illOffset, VO_S64 illLength, VO_U32 ulSequenceId, VO_U32  ulDisSequence,
								VO_BOOL  bDisOccur, VO_U32  ulPlayListId, VO_CHAR*  pPlayListURI, S_TAG_NODE* pXMapTag);
    VO_U32    GetMediaTypeFromTagNode(E_PLAYLIST_TYPE* pePlayListType, S_TAG_NODE*pTagNode);
	VO_VOID   AddPlayListNode(S_PLAYLIST_NODE*  pPlayList);
    VO_VOID   FillPlayListInfo(S_PLAYLIST_NODE*  pPlayList, S_TAG_NODE* pTagNode);
	VO_VOID   FillIFramePlayListInfo(S_PLAYLIST_NODE*  pPlayList, S_TAG_NODE* pTagNode);
	VO_VOID   FillMainStreamPlayListInfo(S_PLAYLIST_NODE*  pPlayList, S_TAG_NODE* pTagNode);
	VO_VOID   FillXMediaPlayListInfo(S_PLAYLIST_NODE*  pPlayList, S_TAG_NODE* pTagNode);
	S_PLAYLIST_NODE*    FindPreferXMediaPlayListInGroup(VO_CHAR*  pGroupId, E_PLAYLIST_TYPE ePlayListType);
    VO_U32    GetPreferValueForPlayList(S_PLAYLIST_NODE*   pPlayList);
	VO_U32    GetChunkItem(S_PLAYLIST_NODE*  pPlayListNode, S_CHUNCK_ITEM*   pChunkItem, VO_U32 ulSeqenceId);
    VO_U32    FindPosInPlayList(VO_U32  ulTimeOffset, S_PLAYLIST_NODE*   pPlayList, VO_U32*  pulNewSequenceId, VO_U32* pNewOffset);
    VO_U32    GetPlayListChunkOffsetValueBySequenceId(S_PLAYLIST_NODE*   pPlayListNode, VO_U32  ulSequenceId, VO_U32* pTimeOffset);	
    VO_U32    GetPlayListDurationByChapterId(S_PLAYLIST_NODE* pPlayList, VO_U32 uChapterId, VO_U32*   pTimeOutput);
    VO_U32    AdjustSequenceIdByPlayListContext(E_PLAYLIST_TYPE  ePlayListType, S_PLAYLIST_NODE*  pPlayList);


    S_PLAY_SESSION    m_sPlaySession;
    S_PLAYLIST_NODE*     m_pPlayListNodeHeader;
	S_PLAYLIST_NODE*     m_pPlayListNodeTail;
    S_SESSION_CONTEXT    m_sSessionContext;

    M3U_MANIFEST_TYPE     m_eRootPlayListType;
    C_M3U_Parser          m_sParser;
    VO_U64                m_ullUTCTime;
    VO_U32                m_ulSystemtimeForUTC;
};

#ifdef _VONAMESPACE
}
#endif


#endif
