	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2011				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		CAC3Reader.h

Contains:	CAC3Reader header file

Written by:	Lieyun

Change History (most recent first):
2011-08-09		Lieyun			Create file

*******************************************************************************/
#ifndef __CAC3READER__H__
#define __CAC3READER__H__
#include "CBaseAudioReader.h"
#include"AC3DataStruct.h"
#include"AC3Table.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define AC3_HEADER_LEN        20
class CAC3Reader: public CBaseAudioReader
{
public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32		SetPos(VO_S64* pPos);
	virtual	VO_U32		GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32		GetCodecCC(VO_U32* pCC);
	virtual VO_U32		GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32		GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32      GetChannelNum();
	virtual VO_S64      Getchannel_layout();
public:
	CAC3Reader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	~CAC3Reader();
protected:
	VO_U32 GetFirstFrmPos();
	
	VO_S32 vo_ac3_parse_header(GetBitContext *gbc, AC3HeaderInfo *hdr);
private:
   AC3HeaderInfo *m_pAC3HeaderInfo;
   VO_U32 m_FirstFrmPos;
   VO_U32  m_FrmSize;
   VO_U32  m_CurSampleIndex;
   VO_U32 m_curReadPos;
   VO_S32  m_PerFrmTime;    //ms per frame
   VO_BYTE m_AC3HeaderData[AC3_HEADER_LEN +1];
};

#ifdef _VONAMESPACE
}
#endif
#endif
