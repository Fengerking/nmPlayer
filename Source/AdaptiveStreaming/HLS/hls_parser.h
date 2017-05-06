#ifndef __VO_HLS_PARSER_H__
#define __VO_HLS_PARSER_H__

#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define HLS_ERR_NONE                               0
#define HLS_ERR_EMPTY_POINTER                      1
#define HLS_ERR_WRONG_MANIFEST_FORMAT              2
#define HLS_ERR_LACK_MEMORY                        3
#define HLS_UN_IMPLEMENT                           4
#define HLS_ERR_NOT_ENOUGH_BUFFER                  5
#define HLS_ERR_NOT_EXIST                          6
#define HLS_ERR_NOT_ENOUGH_PLAYLIST_PARSED         7
#define HLS_ERR_NEED_DOWNLOAD                      8
#define HLS_ERR_ALREADY_EXIST                      9
#define HLS_ERR_VOD_END                            10
#define HLS_PLAYLIST_END                           11

#define COMMON_TAG_HEADER                           "#EXT"

#define BEGIN_NAME_INDEX                             0
#define TARGETDURATION_NAME_INDEX                    1
#define MEDIA_SEQUENCE_NAME_INDEX                    2
#define BYTERANGE_NAME_INDEX                         3

#define INF_NAME_INDEX                               4
#define KEY_NAME_INDEX                               5
#define STREAM_INF_NAME_INDEX                        6
#define PROGRAM_DATE_TIME_NAME_INDEX                 7

#define I_FRAME_STREAM_INF_NAME_INDEX                8
#define ALLOW_CACHE_NAME_INDEX                       9
#define MEDIA_NAME_INDEX                             10
#define PLAYLIST_TYPE_NAME_INDEX                     11

#define I_FRAMES_ONLY_NAME_INDEX                     12
#define DISCONTINUITY_NAME_INDEX                     13
#define ENDLIST_NAME_INDEX                           14
#define VERSION_NAME_INDEX                           15

#define MAP_NAME_INDEX                               16
#define START_NAME_INDEX                             17
#define DISCONTINUITY_SEQUENCE_NAME_INDEX            18
#define NORMAL_URI_NAME_INDEX                        19

#define INVALID_TAG_INDEX                            20
#define V12_SPEC_TAG_TYPE_COUNT                      20
#define MAX_ATTR_COUNT                               16


//TargetDuration
#define TARGETDURATION_VALUE_ATTR_ID                 0
#define TARGETDURATION_MAX_ATTR_COUNT                1

//Media Sequence
#define MEDIA_SEQUENCE_VALUE_ATTR_ID                 0
#define MEDIA_SEQUENCE_MAX_ATTR_COUNT                1

//ByteRange
#define BYTERANGE_RANGE_ATTR_ID                      0
#define BYTERANGE_MAX_ATTR_COUNT                     1

//Inf
#define INF_DURATION_ATTR_ID                         0
#define INF_DESC_ATTR_ID                             1
#define INF_MAX_ATTR_COUNT                           2

//KEY   
#define KEY_LINE_CONTENT                             0
#define KEY_MAX_ATTR_COUNT                           1

//ProgramDataTime
#define PROGRAM_DATE_TIME_ATTR_ID                    0
#define PROGRAM_MAX_ATTR_COUNT                       1

//Allow Cache
#define ALLOW_CACHE_VALUE_ATTR_ID                    0
#define ALLOW_CACHE_MAX_ATTR_COUNT                   1

//PlayList Type
#define PALYLIST_TYPE_VALUE_ATTR_ID                  0
#define PALYLIST_TYPE_MAX_ATTR_COUNT                 1

//Media
#define MEDIA_TYPE_ATTR_ID                           0
#define MEDIA_GROUP_ID_ATTR_ID                       1
#define MEDIA_NAME_ATTR_ID                           2
#define MEDIA_DEFAULT_ATTR_ID                        3
#define MEDIA_URI_ATTR_ID                            4
#define MEDIA_AUTOSELECT_ATTR_ID                     5
#define MEDIA_LANGUAGE_ATTR_ID                       6
#define MEDIA_ASSOC_LANGUAGE_ATTR_ID                 7
#define MEDIA_FORCED_ATTR_ID                         8
#define MEDIA_INSTREAM_ATTR_ID                       9
#define MEDIA_CHARACTERISTICS_ATTR_ID                10
#define MEDIA_MAX_ATTR_COUNT                         11

//StreamInf
#define STREAM_INF_BANDWIDTH_ATTR_ID                 0
#define STREAM_INF_CODECS_ATTR_ID                    1
#define STREAM_INF_VIDEO_ATTR_ID                     2
#define STREAM_INF_AUDIO_ATTR_ID                     3
#define STREAM_INF_SUBTITLE_ATTR_ID                  4
#define STREAM_INF_CLOSED_CAPTIONS_ATTR_ID           5
#define STREAM_INF_PROGRAM_ID_ATTR_ID                6
#define STREAM_INF_RESOLUTION_ATTR_ID                7
#define STREAM_MAX_ATTR_COUNT                        8

//Discontinuity sequence
#define DISCONTINUITY_SEQUENCE_ATTR_ID               0
#define DISCONTINUITY_SEQUENCE_MAX_ATTR_COUNT        1

//XMAP
#define XMAP_URI_ATTR_ID                             0
#define XMAP_BYTERANGE_ATTR_ID                       1
#define XMAP_MAX_ATTR_COUNT                          2

//I FRAME StreamInf
#define IFRAME_STREAM_URI_ATTR_ID                    0
#define IFRAME_STREAM_BANDWIDTH_ATTR_ID              1
#define IFRAME_STREAM_CODECS_ATTR_ID                 2
#define IFRAME_STREAM_MAX_ATTR_COUNT                 3

//START
#define X_START_TIMEOFFSET_ATTR_ID                   0
#define X_START_PRECISE_ATTR_ID                      1
#define X_START_MAX_ATTR_COUNT                       2

//VERSION
#define VERSION_NUMBER_ATTR_ID                       0
#define VERSION_MAX_ATTR_COUNT                       1

//URI Line
#define URI_LINE_ATTR_ID                             0
#define URI_LINE_MAX_ATTR_COUNT                      1

enum M3U_MEDIA_STREAM_TYPE
{
	M3U_MEDIA_STREAM_X_TYPE,
	M3U_MEDIA_STREAM_MAIN_TYPE,
	M3U_MEDIA_STREAM_UNKNOWN_TYPE
};

enum M3U_MANIFEST_TYPE
{
	M3U_CHUNK_PLAYLIST,
	M3U_STREAM_PLAYLIST,
	M3U_UNKNOWN_PLAYLIST
};

enum M3U_CHUNCK_PLAYLIST_TYPE
{
	M3U_LIVE,
	M3U_VOD,
	M3U_EVENT,
	M3U_INVALID_CHUNK_PLAYLIST_TYPE
};

enum M3U_CHUNCK_PLAYLIST_TYPE_EX
{
	M3U_NORMAL,
	M3U_VISUALON_PD,
	M3U_INVALID_EX    
};

enum M3U_DATA_TYPE
{
	M3U_INT,
	M3U_FLOAT,
	M3U_STRING,
	M3U_HEX_DATA,
    M3U_BYTE_RANGE,
	M3U_DECIMAL_RESOLUTION,
	M3U_UNKNOWN
};

typedef struct
{
	VO_U32    ulHeight;
	VO_U32    ulWidth;
}S_RESOLUTION;

typedef struct
{
	VO_U64    ullLength;
	VO_U64    ullOffset;
}S_BYTE_RANGE;


typedef struct S_ATTR_VALUE
{
	M3U_DATA_TYPE   ulDataValueType;
	union
	{
		float           fFloatValue;
		VO_S64          illIntValue;
		VO_CHAR*        pString;
		VO_BYTE*        pHexData;
		S_RESOLUTION*   pResolution;
        S_BYTE_RANGE*   pRangeInfo;
	};
	VO_U32              ulDataLength;
}S_ATTR_VALUE;

typedef struct S_TAG_NODE
{
    VO_U32          ulTagIndex;
    VO_U32          ulAttrSet;
    VO_U32          ulAttrMaxCount;
    S_ATTR_VALUE**  ppAttrArray;
    S_TAG_NODE*     pNext;
}S_TAG_NODE;

class C_M3U_Parser
{
public:
    C_M3U_Parser();
    ~C_M3U_Parser();
    VO_U32   ParseManifest(VO_BYTE*   pManifestData, VO_U32 ulDataLength);
    VO_U32   GetTagList(S_TAG_NODE** ppTagNode);
	VO_U32   GetManifestType(M3U_MANIFEST_TYPE*  peManifestType, M3U_CHUNCK_PLAYLIST_TYPE* peChucklistType, M3U_CHUNCK_PLAYLIST_TYPE_EX*  peChunklistTypeEx);
	VO_VOID  ReleaseAllTagNode();
	VO_VOID  ResetContext();
    VO_U32   ParseLine(VO_CHAR* pManifestLine);

    
private:
    VO_U32   InitParseContext();
	VO_VOID  ReleaseParseContext();
    VO_U32   AddURILine(VO_CHAR*   pLine);
	VO_U32   ParseTagLine(VO_CHAR*   pLine);
	VO_U32   CheckWorkMemory(VO_U32  ulNewDataLength);
    VO_U32   VerifyHeader(VO_BYTE*   pManifestData);
    VO_U32   ReadNextLineWithoutCopy(VO_CHAR* pSrc, VO_CHAR* pEnd, VO_CHAR** ppLine, VO_CHAR** pNext);
	VO_U32   FindAttrValueByName(VO_CHAR*   pOriginalXMediaLine, VO_CHAR*  pAttrValue, VO_U32 ulAttrValueSize, VO_CHAR*   pAttrName);
    VO_U32   CreateTagNode(S_TAG_NODE**  ppTagNode, VO_U32  ulTagType);
	VO_VOID  ReleaseTagNode(S_TAG_NODE*  pTagNode);
    VO_VOID  AddTag(S_TAG_NODE*  pTagNode);
	VO_U32   GetTagType(VO_CHAR*   pLine);

	VO_U32   ParseTargeDuration(VO_CHAR*   pLine);
	VO_U32   ParseMediaSequence(VO_CHAR*   pLine);
	VO_U32   ParseByteRange(VO_CHAR*   pLine);
	VO_U32   ParseInf(VO_CHAR*   pLine);
	VO_U32   ParseKey(VO_CHAR*   pLine);
	VO_U32   ParseStreamInf(VO_CHAR*   pLine);
	VO_U32   ParseProgramDataTime(VO_CHAR*   pLine);
	VO_U32   ParseIFrameStreamInf(VO_CHAR*   pLine);
	VO_U32   ParseAllowCache(VO_CHAR*   pLine);
	VO_U32   ParseXMedia(VO_CHAR*   pLine);
	VO_U32   ParsePlayListType(VO_CHAR*   pLine);
	VO_U32   ParseIFrameOnly(VO_CHAR*   pLine);
    VO_U32   ParseDisContinuity(VO_CHAR*   pLine);
	VO_U32   ParseEndList(VO_CHAR*   pLine);
	VO_U32   ParseVersion(VO_CHAR*   pLine);
	VO_U32   ParseXMap(VO_CHAR*   pLine);
	VO_U32   ParseXStart(VO_CHAR*   pLine);
	VO_U32   ParseDisSequence(VO_CHAR*   pLine);
    
    VO_U32   ParseInt(VO_CHAR*   pLine, VO_CHAR* pAttrName, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex);
	VO_U32   ParseFloat(VO_CHAR*   pLine, VO_CHAR* pAttrName, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex);
	VO_U32   ParseString(VO_CHAR*   pLine, VO_CHAR* pAttrName, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex);
	VO_U32   ParseByteRangeInfo(VO_CHAR*   pLine, VO_CHAR* pAttrName, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex);
	VO_U32   ParseTotalLine(VO_CHAR*   pLine, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex);
	VO_U32   ParseResolution(VO_CHAR*   pLine, VO_CHAR* pAttrName, S_TAG_NODE*  pTagNode, VO_U32 ulAttrIndex);



    VO_BYTE*   m_pManifestData;
	VO_U32     m_ulManifestDataMaxLength;
    S_TAG_NODE*    m_pTagNodeHeader;
	S_TAG_NODE*    m_pTagNodeTail;
    M3U_MANIFEST_TYPE   m_eCurrentManifestType;
    M3U_CHUNCK_PLAYLIST_TYPE	 m_eCurrentChuckPlayListType;
    M3U_CHUNCK_PLAYLIST_TYPE_EX  m_eCurrentChuckPlayListTypeEx;

    VO_CHAR**                    m_ppTagName;
	VO_U32*                      m_pAttrMaxCountSet;
    VO_U32                       m_ulTagTypeCount;
};



#ifdef _VONAMESPACE
}
#endif

#endif
