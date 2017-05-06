#ifndef __CMp4TrackPushPlay_H__
#define __CMp4TrackPushPlay_H__

#include "CMp4Track2.h"
#include "voCMutex.h"
#include "CMp4Reader2.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMp4TrackPushPlay :
	public CMp4Track2
{
public:
	CMp4TrackPushPlay(Track* pTrack, Reader* pReader, VO_MEM_OPERATOR* pMemOp, CMp4Reader2* pMp4Reader);
	virtual ~CMp4TrackPushPlay();
	VO_U32				GetParameter(VO_U32 uID, VO_PTR pParam);
protected:
	virtual VO_U32		GetSampleN(VO_SOURCE_SAMPLE* pSample);
	VO_U32				GetInfoByIndex(VO_U32 uIndex);
	VO_BOOL				IsFileEnd();
	VO_VOID				SetFileEnd(VO_BOOL bEnd);
	VO_U32				GetCurDuration(VO_S64& llDuration);
	VO_S32				GetCurIndex();
	VO_VOID				SetCurIndex(VO_S32 Index);
private:
	VO_S32				m_LastIndexNo;///<the last moof index
//	Reader*				m_pReader2;
	voCMutex			m_lock;
	CMp4Reader2*		m_pMp4Reader;
	VO_BOOL				m_bEndOfFile;
};
#ifdef _VONAMESPACE
}
#endif

#endif	//__CMp4TrackPushPlay_H__
