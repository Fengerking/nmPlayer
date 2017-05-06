#pragma once

#include "CBaseAudioReader.h"
#include "ape_datastruct.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class ape_reader :
	public CBaseAudioReader
{
public:
	ape_reader(VO_FILE_OPERATOR* pFileOp, 
		       VO_MEM_OPERATOR* pMemOp, 
			   VO_LIB_OPERATOR* pLibOP, 
			   VO_SOURCEDRM_CALLBACK* pDrmCB);

	~ape_reader(void);

	VO_U32 Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);

	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPos(VO_S64* pPos);
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);

protected:
	VO_BOOL analyze_header();

protected:
	APE_INFO m_info;
	APE_FRAME * m_ptr_frames;
	VO_S64 m_apedata_pos;

	VO_PBYTE m_ptr_headerdata;
	VO_S32 m_headerdata_size;
	VO_S32 m_SeekTableLen;
	VO_U32 m_bitrate;

	VO_U32 m_index;
};

#ifdef _VONAMESPACE
}
#endif
