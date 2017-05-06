#ifndef __CH264TRACK_H__
#define __CH264TRACK_H__
#include "CBaseStreamFileReader.h"

class CH264Track :
	public CBaseTrack//CBaseStreamFileTrack
{
	friend class CH264RawData;
public:
	CH264Track(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CH264RawData *pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CH264Track();
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetMaxSampleSize(VO_U32* pdwMaxSampleSize){return 0;}
	virtual VO_U32		GetCodecCC(VO_U32* pCC) ;
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate) {return 0;}
	virtual VO_U32		GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPosN(VO_S64* pPos);
	virtual VO_U32		GetSampleK(VO_SOURCE_SAMPLE* pSample) { return 0;}
	virtual VO_U32		SetPosK(VO_S64* pPos) { return 0;}

protected:
	CH264RawData        *m_pH264RawData;

};
#endif