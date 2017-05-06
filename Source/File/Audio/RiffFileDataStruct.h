	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		RiffFileDataStruct.h

Contains:	Data Structure Of RIFF file

Written by:	East

Reference:	

Change History (most recent first):
2009-06-26		East			Create file

*******************************************************************************/
#ifndef __Riff_File_Data_Struct
#define __Riff_File_Data_Struct

#include "voType.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct tagRiffChunk
{
	VO_U32	fcc;
	VO_U32	size;
} RiffChunk, *PRiffChunk;

typedef struct tagRiffList
{
	VO_U32	fcc;
	VO_U32  size;
	VO_U32	fccListType;
} RiffList, *PRiffList;

#define FOURCC_RIFF					MAKEFOURCC('R', 'I', 'F', 'F')
#define FOURCC_fmt					MAKEFOURCC('f', 'm', 't', ' ')
#define FOURCC_DATA					MAKEFOURCC('D', 'A', 'T', 'A')
#define FOURCC_data					MAKEFOURCC('d', 'a', 't', 'a')

#ifdef _VONAMESPACE
}
#endif

#endif	//__Riff_File_Data_Struct
