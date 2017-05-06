/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/*******************************************************************************
File:		CAviReader.h

Contains:	provides the interfaces to get the right data from an avi file  

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#pragma once

#include "CBaseStreamFileReader.h"
#include "CAviTrack.h"
#include "CAviParser.h"
#include "voDivXDRM.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

//#define g_dwFRModuleID	VO_INDEX_SRC_AVI

class CAviReader :
	public CBaseStreamFileReader
{
public:
	CAviReader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CAviReader();

public:
	static VO_VOID		GOnIdx1IndexEntry(VO_PTR pParent, VO_PTR pParam, PAviOriginalIndexEntry pEntry, VO_U32 dwCurrIndexNum);

public:
	virtual VO_U32		Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32		Close();

	virtual VO_U32		GetFileHeadDataInfo(headerdata_info* pHeadDataInfo);

public:
	VO_U64				GetMediaDataSize() {return m_HeaderParser.GetMediaDataSize();}
	inline VO_U8		OnFrame(CGFileChunk* pFileChunk, VO_U16 wStreamTwocc, VO_U32 dwLen);

	virtual VO_VOID		OnNewDataParser(CBaseTrack* pRqstTrack, CvoFileDataParser** ppDataParser);
	virtual VO_BOOL		IsDivXDRM() {return m_bDivXDRM;}

protected:
	VO_VOID				OnIdx1IndexEntry(PAviOriginalIndexEntry pEntry, VO_U32 dwCurrIndexNum);
	
	inline CAviTrack*	GetTrack(VO_U16 wStreamTwocc);

protected:
	virtual VO_U32		InitTracks(VO_U32 nSourceOpenFlags);
	virtual VO_U32		PrepareTracks();
	virtual CAviTrack*	NewTrack(PAviTrackInfo pTrackInfo);
	virtual VO_U32		GetSeekable(VO_BOOL* pIsSeekable);
	virtual VO_U32		PrepareIndx();
	virtual VO_U32		PrepareIdx1();

protected:
	CAviHeaderParser	m_HeaderParser;
	CAviDataParser		m_DataParser;		//only used when file has no index(indx and idx1)

	VO_BOOL				m_bDivXDRM;
};

#ifdef _VONAMESPACE
}
#endif
