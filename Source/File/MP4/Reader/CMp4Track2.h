#ifndef __CMp4Track_2_H__
#define __CMp4Track_2_H__

#include "CBaseTrack.h"
#include "isomrd.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef VO_S64(* COMPARE_KEY)(VO_S64 key, const VO_PTR item);

struct SeekEntry
{
	VO_S64 time;
	VO_S64 addr;

	static VO_S64 CompareTime(VO_S64 key, const VO_PTR item)
	{
		return key - ((const SeekEntry*)item)->time;
	}

	static VO_S64 CompareAddr(VO_S64 key, const VO_PTR item)
	{
		return key - ((const SeekEntry*)item)->addr;
	}

};

class SeekTable
	: public CvoBaseMemOpr
{
public:
	SeekTable(VO_MEM_OPERATOR* pMemOp);
	virtual ~SeekTable();

public:
	VO_BOOL Init(Track* pTrack, VO_U32 step);

	VO_S64 FindByTime(VO_S64 key, SeekEntry** ppResult1, SeekEntry** ppResult2)
	{
		return m_count ? Find(key, SeekEntry::CompareTime, ppResult1, ppResult2) : -2;
	}

	VO_S64 FindByAddr(VO_S64 key, SeekEntry** ppResult1, SeekEntry** ppResult2)
	{
		return m_count ? Find(key, SeekEntry::CompareAddr, ppResult1, ppResult2) : -2;
	}

private:
	VO_U32		m_count;
	SeekEntry*	m_entries;

protected:
	VO_VOID	Release();
	VO_S64	Find(VO_S64 key, COMPARE_KEY compare, SeekEntry** ppResult1, SeekEntry** ppResult2);
};

class CMp4Track2 :
	public CBaseTrack
{
public:
	CMp4Track2(Track* pTrack, Reader* pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CMp4Track2();

public:
	// override
	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);
//2011/11/16, add by leon, for Eric, getthumbnail slowly.
	virtual VO_U32     SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 		GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
	virtual VO_U32		ResetIndex();
public:
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetFirstFrame(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		GetNextKeyFrame(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32		GetFrameNum(VO_U32* pdwFrameNum);
	virtual VO_U32		GetFrameTime(VO_U32* pdwFrameTime);
	virtual VO_U32		GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime);

	virtual VO_U32		GetMediaTimeByFilePos(VO_S64 llFilePos, VO_S64* pllMediaTime, VO_BOOL bStart);
	virtual VO_U32		GetFilePosByMediaTime(VO_S64 llMediaTime, VO_S64* pllFilePos, VO_BOOL bStart);
	        VO_U32		GetDataStartPos() { return m_pTrack->GetSampleAddress(0); }

	virtual VO_U64   GetRealDurationByFilePos(VO_U64 pos) {return m_pTrack->GetRealDurationByFilePos(pos);}

//12/19/2011, leon , add for text track
	virtual VO_U32 Prepare();
//12/27/2011, leon , add for text track
	virtual VO_U32 Unprepare();

//04/17/2012, Leon, add for headdata
	virtual VO_VOID  SetIsStreaming(VO_BOOL isStreaming){ m_bIsStreaming = isStreaming;}
	virtual VO_BOOL  GetIsStreaming(){return m_bIsStreaming;}
	VO_VOID	SetCencDrmFlag(VO_BOOL bValue){
		m_IsCencDrm = bValue;
	}
	
public:
	VO_U32	GetTimeScale() {return m_pTrack->GetTimeScale();}
	VO_BOOL IsCencDrm(){ 
		return m_IsCencDrm;
	}

protected:
	virtual VO_U32		Init(VO_U32 nSourceOpenFlags);
	virtual VO_U32		Uninit();

	//GetSample and SetPos implement of normal mode!!
	virtual VO_U32		GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPosN(VO_S64* pPos);
	//GetSample and SetPos implement of key frame mode!!
	virtual VO_U32		GetSampleK(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPosK(VO_S64* pPos);
	virtual VO_U32      GetThumbnailSyncSampleInfo(VO_PTR pParam);

protected:
	VO_U16				GetAudioChannelCount();
	void				PrepareOggHeadData(void* dsidata, int dsisize);

protected:
	Reader*				m_pReader;
	Track*				m_pTrack;

	VO_PBYTE			m_pHeadData;
	VO_U32				m_dwHeadData;

	VO_U32				m_dwMaxSampleSize;
	VO_U32				m_dwSampleCount;

	VO_S32				m_dwCurrIndex;
	VO_S64				m_qwCurrTime;
	VO_BOOL             m_IsCencDrm;

	SeekTable			m_seeker;
	
	VO_U32			m_nNALLengthSize;//07/25/2011 Leon

public:
	inline void	GetMatrix(VO_S32 matrix[9]) { return m_pTrack->GetMatrix(matrix); }

private:
	VO_U16				m_wFormatTag;
	VO_U16              m_wBitsPerSample;

//2011/11/16, add by leon, for Eric, getthumbnail slowly.
private:
	VO_S32				m_nCurThumbnailScanCount;
//2011/11/18, add by leon,
	VO_U32				m_nCurSyncFrame;

//2012/04/17,add by leon,
	VO_BOOL				m_bIsStreaming;

//2012/08/07,add by leon
public:
	virtual VO_U32 GetTrackId(){ return m_pTrack->GetTrackID();}


};
#ifdef _VONAMESPACE
}
#endif

#endif	//__CMp4Track_2_H__
