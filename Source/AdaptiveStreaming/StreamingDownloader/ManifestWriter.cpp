/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		ManifestWriter.cpp

	Contains:	CManifestWriter class file

	Written by:	Aiven

	Change History (most recent first):
	2013-09-02		Aiven			Create file

*******************************************************************************/

#include "ManifestWriter.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CManifestWriter::CManifestWriter()
{
}

CManifestWriter::~CManifestWriter()
{

}

VO_U32 CManifestWriter::WriteManifest(list_T<MANIFEST_INFO*>* pManifestList, list_T<CHUNKINFO*>* pChunkList, VO_PCHAR pManifestID, MANIFEST_GROUP** ppNewManifestGroup, VO_BOOL bEnd)
{
	VOLOGI("WriteManifest");
	return 0;
}


