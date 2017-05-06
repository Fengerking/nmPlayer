	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CPlayFile.h

	Contains:	CPlayFile header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CPlayFile_H__
#define __CPlayFile_H__

#include "voFile.h"

#include "CPlayGraph.h"

class CPlayFile : public CPlayGraph
{
public:
	// Used to control the image drawing
	CPlayFile (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
	virtual ~CPlayFile (void);

	bool				CheckBlackFrame (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize);
	virtual VO_U32 		Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength, int nWidth, int nHeight, VO_PTR hBitmap);
};

#endif // __CPlayFile_H__
