	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		WavFileDataStruct.h

Contains:	Data Structure Of WAV file

Written by:	East

Reference:	

Change History (most recent first):
2009-06-26		East			Create file

*******************************************************************************/
#ifndef __Wav_File_Data_Struct
#define __Wav_File_Data_Struct

#include "RiffFileDataStruct.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define FOURCC_WAVE					MAKEFOURCC('W', 'A', 'V', 'E')

#ifdef _VONAMESPACE
}
#endif

#endif	//__Wav_File_Data_Struct
