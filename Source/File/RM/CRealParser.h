#pragma once

#include "CvoFileParser.h"
#include "RMFileDataStruct.h"
#include "voList2Node.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define VO_RM_HEAD_FAILURE		0
#define VO_RM_HEAD_SUCCFUL		1
#define VO_RM_HEAD_SUCCEND		2

class CRealHeaderParser : 
	public CvoFileHeaderParser
{
public:
	CRealHeaderParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CRealHeaderParser();

public:
	//parse file header to get information
	virtual VO_BOOL			ReadFromFile();

	virtual VO_VOID			SetFileSize(VO_U64 ullFileSize) {m_ullFileSize = ullFileSize;}

protected:
	//VO_RM_HEAD_FAILURE - fail
	//VO_RM_HEAD_SUCCFUL - successful
	//VO_RM_HEAD_SUCCEND - successful end
	virtual VO_U32			ReadHeaderInfo(VO_BOOL& bHeaderCompleted);

public:
	VO_VOID					ReleaseTmpInfo();

	VO_U32					GetDuration() {return m_dwDuration;}
	VO_U32					GetMaxPacketSize() {return m_dwMaxPacketSize;}
	VO_U64					GetIndexObjectsFilePos() {return m_ullIndexObjectsFilePos;}
	VO_U64					GetMediaDataFilePos() {return m_ullMediaDataFilePos;}
	VO_U64					GetContentDescriptionHeaderFilePos() {return m_ullContentDescriptionHeaderFilePos;}
	VO_U64					GetMetadataSectionFilePos() {return m_ullMetadataSectionFilePos;}
	VO_U32					GetTrackInfoCount() {return m_nTrackInfoCount;}
	PRealTrackInfo			GetTrackInfoPtr() {return m_paTrackInfo;}
	PRealFileIndexInfo		GetIndexInfoPtr() {return m_paIndexInfo;}

private:
	VO_U32					ProcessFileHeader(RMFFChunk& rc , VO_U32& num_headers );
	VO_U32					ProcessBaseChunk(RMFFChunk& rc);
	VO_U32					ProcessFileproperties();
	VO_U32					ProcessMediaproperties();
	VO_U32					ProcessDataChunkHeader(RMFFChunk rc ,VO_BOOL& bHeaderCompleted);
	VO_U32					ProcessINDXChunkHeader(VO_BOOL& bHeaderCompleted);
	VO_U32					ProcessContentDescription();
	
	//we use one parameter to verify the other parameter, in order to make sure the rightness of stored parameter.
	VO_VOID					HeadinfoVerify();
protected:
	//temporary information
	VO_U32					m_nTrackInfoCount;
	PRealTrackInfo			m_paTrackInfo;
	PRealLogicalStream		m_paLogicalStreams[2];	//0 - audio, 1 - video
	PRealFileIndexInfo		m_paIndexInfo;

	VO_U32					m_dwDuration;			//media duration of file<MS>
	VO_U32					m_dwDurationtmp;		//media duration from media properties header
	VO_U32					m_dwMaxPacketSize;		//max size of packet
	VO_U32					m_dwMaxPacketSizetmp;	//max size of packet from media properties header

	VO_U64					m_ullFileSize;
	VO_U64					m_ullIndexObjectsFilePos;
	VO_U64					m_ullMediaDataFilePos;
	VO_U64					m_ullContentDescriptionHeaderFilePos;
	VO_U64					m_ullMetadataSectionFilePos;
	VO_BOOL					m_bLastheader;
	VO_U32					m_dwfourcc_data;
	VO_U32					m_dwfourcc_index;
	VO_U64					m_ullMediaDataFilePostmp;
	VO_U64					m_ullIndexObjectsFilePostmp;
	VO_U32					m_dwMediapropertynum;		//The total number of media properties headers in the main headers section
	VO_U32					m_dwMediapropertynumtmp;
};

typedef struct tagRealDataParserSample
{
	VO_U8			btStreamNum;
	VO_U8			btFlags;
	VO_U16			wPacketLen;
	VO_U32			dwTimeStamp;
	CGFileChunk*	pFileChunk;
} RealDataParserSample, *PRealDataParserSample;

typedef struct tagRealIframeIndexInfo
{
	VO_U64	llfilepos;
	VO_S32	lestimate_framesize;
	list_node i_list;
}RealIframeIndexInfo, *PRealIframeIndexInfo;

class CRealDataParser : 
	public CvoFileDataParser
{
public:
	CRealDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CRealDataParser();

public:
	virtual VO_VOID			Init(VO_PTR pParam);

	virtual VO_BOOL			SetStartFilePos(VO_U64 ullStartFilePos, VO_BOOL bInclHeader = VO_FALSE);

	VO_VOID					SetThumbnailFlag(VO_BOOL bThumbnail);

	VO_VOID					SetThumbnailScanCount(VO_S32 cnt);

	VO_VOID					SetStepBRangeForthumbnail( VO_U32 llimitsize);

	VO_BOOL					GetMaxSizeIframeIndex(VO_U64& llpos, VO_S32& limitsize);

	VO_BOOL					SetFileIOPos(VO_U64 ullStartFilePos);
protected:
	virtual VO_BOOL			StepB();

	VO_VOID					RelIframeIndex();
private:
	//all below is for thumbnail purpose
	VO_BOOL	m_IsThumbNail;
	VO_S32	m_ThumbnailScanCount;
	list_node	m_Iframe_index_headnode;
	VO_S32  m_nExistThumnnailCnt;
	VO_BOOL	m_formerIskeyframe;
	VO_U32	m_packetsize_sum;
	RealIframeIndexInfo* m_pIframenode;
	VO_U32	m_thumbnailstep_limitsize;
};

#ifdef _VONAMESPACE
}
#endif