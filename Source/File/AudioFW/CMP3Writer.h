#ifndef _CMP3Writer_H_
#define _CMP3Writer_H_

#include "voSink.h"
//#include "voType.h"
#include "CvoBaseMemOpr.h"
#include "CvoBaseFileOpr.h"

class CMP3Writer : public CvoBaseMemOpr, public CvoBaseFileOpr
{
  public:
	CMP3Writer(VO_MEM_OPERATOR* pMemOp, VO_FILE_OPERATOR* pFileOp);
	virtual ~CMP3Writer();
	
	VO_U32  Open(VO_FILE_SOURCE *pSource, VO_SINK_OPENPARAM * pParam);
	VO_U32  Close();
	VO_U32  SetParam(VO_U32 uID, VO_PTR pParam);
	VO_U32  GetParam(VO_U32 uID, VO_PTR pParam);
	VO_U32  AddSample(VO_SINK_SAMPLE * pSample);

  private:
	VO_AUDIO_FORMAT		mAudioFormat;
	VO_U32              m_nDataSize;
	VO_PTR              m_hFile;
	VO_U32              mnTotalFrames;
	VO_U32              mAudioCodec;
};

#endif//_CMP3Writer_H_
