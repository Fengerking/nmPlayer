/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		FileSinkCtrl.h

Contains:	FileSinkCtrl header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/

#ifndef __CFileSinkCtrl_H__
#define __CFileSinkCtrl_H__
#pragma once


#include "voFile.h"
#include "CFileSink.h"

#include "videoEditorType.h"
#include "voedtType.h"

class CFileSinkCtrl
{
public:
	CFileSinkCtrl(void);
	~CFileSinkCtrl(void);
public:
	VO_S32 SetFileSink(VO_TCHAR *dumpFilePath,VO_TCHAR *dumpTmpDir
									,VO_U32 nPacketType
									,VO_U32 nAudioCodec,VO_AUDIO_FORMAT *fmtAudio
									,VO_U32 nVideoCodec,VO_VIDEO_FORMAT *fmtVideo
									,VO_LIB_OPERATOR* pLibop);
	VO_U32 AddASample(VOEDT_ENCODER_SAMPLE *sample); 
	VO_S32 SetParam(VO_S32 nPid,void* pValue);
	VO_S32 Start();
	VO_U32 Stop();
private:
	CFileSink *m_pFileSink;
	VO_FILE_SOURCE		m_Source;
};
#endif