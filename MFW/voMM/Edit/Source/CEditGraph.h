/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CEditGraph.h

Contains:	CEditGraph header file

Written by:	 Yu Wei based on Bangfei's code

Change History (most recent first):
2010-04-26		YW			Create file

*******************************************************************************/

#ifndef __CEditGraph_H__
#define __CEditGraph_H__

#include "voFile.h"

#include "CPlayGraph.h"
#include "CFileSink.h"

#include "CBaseController.h"

class CEditGraph : public CPlayGraph
{
protected:
	CFileSink *				m_pFileSink;
	
	VO_FILE_SOURCE			m_Source;
	VO_TCHAR                m_szDumpFilePath[2048];
	VO_TCHAR                m_szDumpTmpDir[2048];

	VO_U32                  m_nVideoFirstSample;
	VO_U32                  m_nAudioFirstSample;

	VO_BYTE*                m_pAudioHeader;
	VO_BYTE*                m_pVideoHeader;
	VO_U32                  m_nAudioHeaderSize;
	VO_U32                  m_nVideoHeaderSize;

	VO_U32                  m_nDumpVideoTime;
	VO_U32                  m_nDumpAudioTime;

	CBaseController         m_Controller;
	
protected:
	VO_U32					DumpSample(VO_SOURCE_SAMPLE* pSourceSample, VO_U32 nIsVideo);

	 

public:
	// Used to control the image drawing
	CEditGraph (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
	virtual ~CEditGraph (void);

	virtual VO_U32 		Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);

	virtual VO_U32	PlaybackAudioFrame ();
	virtual VO_U32	PlaybackVideoFrame (void);



};

#endif // __CEditGraph_H__
