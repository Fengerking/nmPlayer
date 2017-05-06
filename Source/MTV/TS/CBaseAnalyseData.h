#pragma once

#include "voYYDef_TS.h"
#include "voAudio.h"
#include "voVideo.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CBaseAnalyseData
{
public:
	CBaseAnalyseData(void);
	virtual ~CBaseAnalyseData(void);


	virtual VO_BOOL	AnalyseData(VO_BYTE *pData , VO_U32 nDataLen) = 0;
	virtual VO_WAVEFORMATEX *GetAudioInfo();
	virtual VO_VIDEOINFOHEADER *GetVideoInfo();

	virtual VO_BYTE *GetSequenceHeader(VO_U32 &nDataLen);
	virtual VO_BOOL	IsKeyFrame(VO_BYTE *pData , VO_U32 nDataLen){return VO_FALSE;};

protected:
	virtual VO_BYTE *FindSyncPos(VO_BYTE *pData , VO_U32 nDataLen , VO_BYTE nMarkByte);
	virtual VO_BYTE *FindSyncPos(VO_BYTE *pData , VO_U32 nDataLen);

protected:
	VO_WAVEFORMATEX		mWaveFormat;
	VO_VIDEOINFOHEADER	mVideoInfoHeader;

	VO_U32				mnSyncLen;

	VO_BYTE *		mpSeqHeader;
	VO_U32			mnSeqLen;
};
#ifdef _VONAMESPACE
}
#endif