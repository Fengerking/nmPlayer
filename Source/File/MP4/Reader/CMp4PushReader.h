#ifndef __CMP4PUSHREADER_H__
#define __CMP4PUSHREADER_H__

#include "CBaseReader.h"
#include "CMp4PushFileStream.h"
#include "isomrd.h"
#include "CMp4Track2.h"
#include "voParser.h"
#include "voLiveSource.h"
#include "voSource.h"
#include "voDrmCallback.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
//setparam
#define VOSTREAMREADER_PID_DRM_CALLBACK		0x000001
#define VOSTREAMREADER_PID_DRM_INFO				0x000002

typedef struct
{
	VO_BOOL		isUsePiff;
	VO_BYTE		systemID[16];
	VO_U32		dataSize;
	VO_BYTE		*data;
}ProtectionHeader;

typedef struct  
{
	uint32 nDefaultSize;
	uint32 nDefaultDuration;
	uint32 nTrackID;
}TrackInfo;

class CMp4PushReader :
	public CBaseReader
{
public:
	CMp4PushReader();
	virtual ~CMp4PushReader();

public:
	virtual VO_U32 Open(PARSERPROC pProc, VO_VOID* pUserData);
	virtual VO_U32 Process(VO_PARSER_INPUT_BUFFER* pBuffer);
	virtual VO_U32		ParserHead(VO_PBYTE pBuf,VO_U64 bufsize);
	virtual VO_U32		ParserData(VO_PBYTE pBuf,VO_U64 bufsize);
	virtual VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 GetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		Close();

	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);

protected:
	virtual VO_U32		InitTracks(VO_U32 nSourceOpenFlags);

	virtual	VO_U32		GetFileHeadSize(VO_U32* pdwFileHeadSize);
	virtual VO_U32		GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam);
	virtual VO_U32		GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam);
	virtual VO_U32		GetSeekPosByTime(VO_FILE_MEDIATIMEFILEPOS* pParam);
	virtual VO_U32		GetSeekTimeByPos(VO_FILE_MEDIATIMEFILEPOS* pParam);
	//2011/11/21,by leon ,fix 9158
	virtual CBaseTrack* GetTrackByIndex(VO_U32 nIndex);
	
#ifdef _SUPPORT_PARSER_METADATA
	virtual VO_U32		InitMetaDataParser();
#endif	//_SUPPORT_PARSER_METADATA

protected:
	ReaderMSB*			m_pReader;
	CMp4PushFileStream*		m_pStream;
	Movie*				m_pMovie;
protected:
	PARSERPROC	m_pProc;
	VO_VOID*	m_pUserData;
	VO_U64		m_u64Starttime;
	VO_U32		m_u32StreamID;
	VO_U32		m_u32TimeScale;
	VO_LIVESRC_TRACK_INFOEX *m_trackInfo;
private:
	uint32 m_nDefaultSize_Video;
	uint32 m_nDefaultDuration_Video;
	uint32 m_nDefaultSize_Audio;
	uint32 m_nDefaultDuration_Audio;
	VO_U32 CreateTrackInfo(int id);
	VO_U32 CreateAudioTrackInfo();
	VO_VOID	IssueParseResult(VO_U32 nType, VO_VOID* pValue, VO_VOID* pReserved = NULL);
	VO_U32 CreateStream(VO_U32 nFlag,VO_PBYTE buf, VO_U32 nSize);
	VO_PBYTE m_pBuf_x;
	VO_BOOL m_bBufNew;
		
};
#ifdef _VONAMESPACE
}
#endif
#endif	//__CMp4Reader_2_H__