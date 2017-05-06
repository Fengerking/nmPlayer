	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2010		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEMetadataMediaFormat.h

	Contains:	CVOMEMetadataMediaFormat header file

	Written by:	Neo Yang

	Change History (most recent first):
	2010-07-21		Neo			Create file

*******************************************************************************/

#ifndef __CVOMEMetadataMediaFormat_H__
#define __CVOMEMetadataMediaFormat_H__

#include "voSource.h"
#include "voType.h"

#include "CDllLoad.h"

#define VO_METADATA_MF_SUPPORTED_STR       "supported"
#define VO_METADATA_MF_NOTSUPPORTED_STR    "notsupported"
#define VO_METADATA_MF_NOTEXIST_STR        "notexist"

enum {
	METADATA_MF_SUPPORTED     = 0,
	METADATA_MF_NOTSUPPORTED  = 1,
	METADATA_MF_NOTEXIST      = 2, 
};

enum {
	METADATA_MF_GET_AUDIO     = 0,
	METADATA_MF_GET_VIDEO     = 1,
};

typedef struct VO_Codec_Map{
	int         m_nCodecType;
	VO_PCHAR    m_pCodecName;
} VO_Codec_Map;


class CVOMEMetadataMediaFormat : public CDllLoad 
{
public:
	CVOMEMetadataMediaFormat(VO_SOURCE_READAPI funFileRead, VO_PTR hSourceFile);
	virtual ~CVOMEMetadataMediaFormat();
	
	VO_U32		CheckIsSupportedAudio(); 
	VO_U32		CheckIsSupportedVideo(); 

protected:
	VO_S32		GetMediaCodec(VO_U32 nFlag);
	VO_U32		CheckSupportedCodec(VO_U32 nCodecID, VO_Codec_Map *pCodecMap);
	VO_BOOL		CheckIsShareLibExits(VO_PCHAR pCodecName);

private:
	VO_SOURCE_READAPI		m_funFileRead;
	VO_PTR					m_hSourceFile;
};


#endif


