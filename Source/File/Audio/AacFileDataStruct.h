	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		AacFileDataStruct.h

Contains:	Data Structure Of AAC file

Written by:	East

Reference:	AAC_ADTS.doc

Change History (most recent first):
2009-06-26		East			Create file

*******************************************************************************/
#ifndef __Aac_File_Data_Struct
#define __Aac_File_Data_Struct

#include "voType.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define FOURCC_ADIF					MAKEFOURCC('A', 'D', 'I', 'F')

#define DECLARE_USE_AAC_GLOBAL_VARIABLE\
	static const VO_U32	s_dwSamplingRates[16];\
	static const VO_U8	s_btChannels[8];

#define DEFINE_USE_AAC_GLOBAL_VARIABLE(cls)\
	const VO_U32 cls::s_dwSamplingRates[16] = \
	{\
		96000,	88200,	64000,	48000,\
		44100,	32000,	24000,	22050,\
		16000,	12000,	11025,	8000,\
		0,		0,		0,		0\
	};\
	const VO_U8 cls::s_btChannels[8] = \
	{\
		2, 1, 2, 3, 4, 5, 6, 7\
	};

#ifdef _VONAMESPACE
}
#endif
#endif	//__Aac_File_Data_Struct
