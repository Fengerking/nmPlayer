#pragma once

#include "CBaseStreamFileReader.h"
#include "CRealTrack.h"
#include "CRealParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CRealReader :
	public CBaseStreamFileReader
{
public:
	CRealReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CRealReader();

public:
	virtual VO_U32		init(VO_U32 nSourceOpenFlags, VO_LIB_OPERATOR* pLibOP, VO_U32 uModuleID , VO_TCHAR* pWorkingPath);
	virtual VO_U32		uninit();
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();

public:
	virtual VO_VOID		OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser);

	inline VO_U8		OnPacket(CGFileChunk* pFileChunk, VO_U8 btStreamNum, VO_U8 btFlags, VO_U16 wLen, VO_U32 dwTimeStamp);

	virtual VO_U32		GetDuration();

protected:
	virtual VO_U32		InitTracks(VO_U32 nSourceOpenFlags);
	virtual VO_U32		PrepareTracks();

	virtual CRealTrack*	NewTrack(PRealTrackInfo pTrackInfo);

#ifdef _SUPPORT_PARSER_METADATA
	virtual VO_U32		InitMetaDataParser();
#endif	//_SUPPORT_PARSER_METADATA

public:
	virtual VO_U32		MoveTo(VO_S64 llTimeStamp);
	virtual VO_U32		OnTrackSetPosN(CRealTrack* pTrack, VO_S64 llTimeStamp);
	CRealDataParser*	GetDataParser(){ return &m_DataParser; }

protected:
	CRealHeaderParser	m_HeaderParser;
	CRealDataParser		m_DataParser;

protected:
	//if audio data and video data occur in the file interleave
	VO_BOOL				m_bAVInterleave;
	//the parser is open for get thumbnail or not 
	VO_BOOL				m_bOpenForthumbnail;
};

#ifdef _VONAMESPACE
}
#endif