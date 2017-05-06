#pragma once

#include "CvoFileParser.h"
#include "fMacros.h"
#include "AsfIndex.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

struct IVInfo
{
	VO_S32 iOffset;
	VO_S32 iLen;
};

class CAsfHeaderParser : 
	public CvoFileHeaderParser
{
public:
	CAsfHeaderParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAsfHeaderParser();

public:
	//parse file header to get information
	virtual VO_BOOL			ReadFromFile();

protected:
	//0 - fail
	//1 - successful
	//2 - header lack
	virtual VO_S32			ReadHeaderInfo();
	virtual VO_U32			ReadExtHeaderInfo();
	virtual VO_BOOL			ReadStreamPropObject(VO_S32 iLenObject);

public:
	VO_VOID					ReleaseTmpInfo();

	PAsfTrackInfo			NewTrackInfo(VO_U8 btStreamNum, VO_BOOL bVideo, VO_U32 dwPropBufferSize);

	VO_U32					GetTrackInfoCount() {return m_nTrackInfoCount;}
	PAsfTrackInfo			GetTrackInfoPtr() {return m_paTrackInfo;}
	PAsfTrackInfo			GetTrackInfo(VO_U8 btStreamNum);
	PAsfStreamExtInfo		GetTrackExtInfo(VO_U8 btStreamNum);
	VO_U8					GetPrior(VO_U8 btStreamNum);
	VO_U32					GetBitrate(VO_U8 btStreamNum);
	/*added by danny*/
	VO_U32                  GetMaxBitrate(){return m_maxBitrate;};
	/*end*/
	VO_U64					GetTimeOffset() {return m_ullTimeOffset;}
	VO_U32					GetDuration() {return m_dwDuration;}
	VO_U32					GetPacketSize() {return m_dwPacketSize;}
	VO_BOOL					GetContentEncryptObjects(VO_PBYTE* ppContentEncryptionObject, VO_U32* pdwContentEncryptionObject, VO_PBYTE* ppExtendedContentEncryptionObject, VO_U32* pdwExtendedContentEncryptionObject);
	VO_BOOL					GetProtectionSystemIdentifierObject(VO_PBYTE *, VO_U32 *);
	VO_BOOL					GetPayloadExtensionSystemEncryptionSampleID(IVInfo**);


	VO_U64					GetIndexObjectsFilePos() {return m_ullIndexObjectsFilePos;}
	VO_U64					GetMediaDataFilePos() {return m_ullMediaDataFilePos;}

	VO_U64					GetContentDescriptionObjectFilePos() {return m_ullContentDescriptionObjectFilePos;}
	VO_U64					GetExtendedContentDescriptionObjectFilePos() {return m_ullExtendedContentDescriptionObjectFilePos;}
	VO_U64					GetMetadataObjectFilePos() {return m_ullMetadataObjectFilePos;}
	VO_U64					GetMetadataLibraryObjectFilePos() {return m_ullMetadataLibraryObjectFilePos;}

	VO_U64					GetFileRealSize() { return m_ullActualFileSize;}

protected:
	VO_U32                  m_maxBitrate;
	//temporary information
	VO_U32					m_nTrackInfoCount;
	PAsfTrackInfo			m_paTrackInfo;

	PAsfStreamExtInfo		m_paStreamExtInfo;

	VO_U16					m_wBitrateInfoCount;
	PAsfBitrateInfo			m_paBitrateInfo;

	VO_U16					m_wPriorInfoCount;
	PAsfPriorInfo			m_paPriorInfo;

	VO_PBYTE				m_pContentEncryptionObject;
	VO_U32					m_dwContentEncryptionObject;
	VO_PBYTE				m_pExtendedContentEncryptionObject;
	VO_U32					m_dwExtendedContentEncryptionObject;

	VO_PBYTE				m_pProtectionSystemIdentifierObject;
	VO_U32					m_dwProtectionSystemIdentifierObject;
	IVInfo*					m_pIVInfos;
	VO_S32					m_iStreamNum;

	VO_U64					m_ullTimeOffset;		//start time offset
	VO_U32					m_dwDuration;			//media duration of file<MS>
	VO_U32					m_dwPacketSize;			//size per packet,if not constant, it is zero

	VO_U64					m_ullIndexObjectsFilePos;
	VO_U64					m_ullMediaDataFilePos;

	VO_U64					m_ullContentDescriptionObjectFilePos;
	VO_U64					m_ullExtendedContentDescriptionObjectFilePos;
	VO_U64					m_ullMetadataObjectFilePos;
	VO_U64					m_ullMetadataLibraryObjectFilePos;

	VO_U64					m_ullActualFileSize;
};

typedef struct tagAsfDataParserInitParam
{
	VO_U32		dwPacketSize;
	VO_U64		ullTimeOffset;

	tagAsfDataParserInitParam(VO_U32 packet_size, VO_U64 time_offset)
		: dwPacketSize(packet_size)
		, ullTimeOffset(time_offset)
	{
	}
} AsfDataParserInitParam, *PAsfDataParserInitParam;

typedef struct tagAsfDataParserSample
{
	VO_U8			btStreamNum;
	VO_U32			dwPosInMediaObj;
	VO_U32			dwLen;
	VO_U32			dwMediaObjSize;
	VO_U32			dwTimeStamp;
	CGFileChunk*	pFileChunk;
} AsfDataParserSample, *PAsfDataParserSample;
class CAsfDataParser : 
	public CvoFileDataParser
{
public:
	CAsfDataParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAsfDataParser();

public:
	virtual VO_VOID			Init(VO_PTR pParam);
	VO_VOID					SetIVInfos(IVInfo* pInfos) { m_pIVInfos = pInfos; }

	VO_PBYTE				GetIVDataByStreamNum(VO_S32 iStreamNum);
	VO_BOOL                 GetThumbNailBuffer(VO_PBYTE* ppBuffer, VO_U32 nSize, VO_U32 nPos);

protected:
	virtual VO_BOOL			StepB();

	inline VO_BOOL			ReadPayloads(VO_U8 btPropertyFlags, VO_U32 dwPacketLen, VO_U32 dwPaddingLen, VO_U8 btPayloadLenType);
	inline VO_BOOL			OnPayload(VO_U8 btStreamNum, VO_U32 dwPosInMediaObj, VO_U32 dwLen, VO_U32 dwMediaObjSize, VO_U32 dwTimeStamp);

protected:
	VO_U32					m_dwPacketSize;
	VO_U64					m_ullTimeOffset;

	IVInfo*					m_pIVInfos;
	VO_BYTE					m_sIV[2][8];
};

class CAsfIndexParser : 
	public CvoFileBaseParser
{
public:
	CAsfIndexParser(CGFileChunk* pFileChunk, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAsfIndexParser();

public:
	VO_BOOL					ReadFromFile(VO_U64 ullIndexObjectsPos, VO_U32 dwPacketSize);
	VO_BOOL					IsValid();

	//if llTimeStamp too big, return last entry's packet number
	VO_U64					GetPacketPosByTime(VO_S64 llTimeStamp);
	//if dwIndex too big, return -1
	VO_U64					GetPacketPosByIndex(VO_U32 dwIndex);
	//bForward: get the next index near the time stamp
	VO_U32					GetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp);
	VO_S64					GetPrevKeyFrameTime(VO_S64 llTimeStamp);
	VO_S64					GetNextKeyFrameTime(VO_S64 llTimeStamp);

protected:
	VO_BOOL					ReadFromFileB();

private:
	CAsfIndexEntries *		m_pIndexEntries;

	VO_U32					m_dwPacketSize;

	VO_U64					m_ullIndexObjectsPos;
	VO_BOOL					m_bReadedIndex;
};

#ifdef _VONAMESPACE
}
#endif