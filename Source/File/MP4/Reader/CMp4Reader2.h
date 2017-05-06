#ifndef __CMp4Reader_2_H__
#define __CMp4Reader_2_H__

#include "CBaseReader.h"
#include "CMp4FileStream.h"
#include "isomrd.h"
#include "CMp4Track2.h"
#include "CMp4TrackPushPlay.h"
/*
#ifdef _IOS_TEST
#ifdef _MP4_READER
using namespace _MP4;
#endif
#endif
*/

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMp4Reader2 :
	public CBaseReader
{
public:
	CMp4Reader2(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CMp4Reader2();

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();

	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);

	//2011/11/21,by leon ,fix 9158
	virtual CBaseTrack* GetTrackByIndex(VO_U32 nIndex);
	//2012/08/10, by leon for multiTrack
	virtual VO_U32 SelectDefaultTracks();
	virtual VO_U32 SetSelectTrack(VO_U32 nIndex, VO_BOOL bInUsed,VO_U64 llTs);
	VO_BOOL				IsPushPlay(){return m_bPushPlay;}
	VO_U32				SetParameter(VO_U32 uID, VO_PTR pParam);
	ReaderMSB*			GetPushReader(){return m_pReaderPushPlay;}
protected:
	virtual VO_U32		InitTracks(VO_U32 nSourceOpenFlags);

	virtual	VO_U32		GetFileHeadSize(VO_U32* pdwFileHeadSize);
	virtual VO_U32		GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam);
	virtual VO_U32		GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam);
	virtual VO_U32		GetSeekPosByTime(VO_FILE_MEDIATIMEFILEPOS* pParam);
	virtual VO_U32		GetSeekTimeByPos(VO_FILE_MEDIATIMEFILEPOS* pParam);

#ifdef _SUPPORT_PARSER_METADATA
	virtual VO_U32		InitMetaDataParser();
#endif	//_SUPPORT_PARSER_METADATA

protected:
	ReaderMSB*			m_pReader;
	CMp4FileStream*		m_pStream;
	Movie*				m_pMovie;
	VO_BOOL				m_bPushPlay;
	ReaderMSB*			m_pReaderPushPlay;
	CMp4FileStream*		m_pStreamPushPlay;

//09/20/2011,leon for PIFF playready
#ifdef _SUPPORT_PIFF
public:
	CvoDrmCallback *m_pPlayready_Drmcallback;
#endif
};

#ifdef _VONAMESPACE
}
#endif
#endif	//__CMp4Reader_2_H__
