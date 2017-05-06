/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CExportEdit.h

Contains:	CExportEdit header file

Written by:	 Yu Wei

Change History (most recent first):
2010-05-05		YW			Create file

*******************************************************************************/

#ifndef __CExportEdit_H__
#define __CExportEdit_H__

#include "CBaseEdit.h"



class CExportEdit : public CBaseEdit
{
protected:
	CFileSink *				m_pFileSink;

protected:
	virtual VO_U32		Release (void);

	VO_U32				DumpSample(VO_SOURCE_SAMPLE* pSourceSample, VO_U32 nIsVideo);



public:
	CExportEdit (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
	virtual ~CExportEdit (void);

	virtual VO_U32 		Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);

	virtual VO_U32 		Run (void);

	virtual VO_U32		PlaybackVideoFrame (void);
	virtual VO_U32		PlaybackAudioFrame ();

	virtual VO_U32		PlaybackAudioLoop (void);
	virtual VO_U32		PlaybackVideoLoop (void);

	virtual VO_U32		SetCurPos (VO_S32 nPos);

};

#endif // __CExportEdit_H__
