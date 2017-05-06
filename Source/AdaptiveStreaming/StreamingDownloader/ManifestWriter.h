/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		ManifestWriter.h

	Contains:	CManifestWriter class file

	Written by:	Aiven

	Change History (most recent first):
	2013-09-02		Aiven			Create file

*******************************************************************************/

#ifndef _MANIFEST_WRITER_H_
#define _MANIFEST_WRITER_H_
#include "list_T.h"
#include "voSource2.h"
#include "StreamingDownloadStruct.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CManifestWriter
{
public:
	CManifestWriter();
	virtual ~CManifestWriter();
	
	virtual VO_U32 WriteManifest(list_T<MANIFEST_INFO*>* pManifestList, list_T<CHUNKINFO*>* pChunkList, VO_PCHAR pManifestID, MANIFEST_GROUP** ppNewManifestGroup,VO_BOOL bEnd);
protected:
private:
};

#ifdef _VONAMESPACE
}
#endif

#endif//_MANIFEST_WRITER_H_


