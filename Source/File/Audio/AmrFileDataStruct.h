	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		AmrFileDataStruct.h

Contains:	Data Structure Of AMR and AWB file

Written by:	East

Reference:	

Change History (most recent first):
2009-06-26		East			Create file

*******************************************************************************/
#ifndef __Amr_File_Data_Struct
#define __Amr_File_Data_Struct

#include "voType.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define AMR_TAG_SIZE		6
#define AWB_TAG_SIZE		9
#define AMR_MAXPACKETSIZE	32
#define AWB_MAXPACKETSIZE	61

#define DECLARE_USE_AMR_GLOBAL_VARIABLE\
	static const VO_U16	s_wAMRPacketSizes[16];\
	static const VO_U16	s_wAWBPacketSizes[16];

#define DEFINE_USE_AMR_GLOBAL_VARIABLE(cls)\
	const VO_U16 cls::s_wAMRPacketSizes[16] = {12, 13, 15, 17, 19, 20, 26, 31,  5,  0,  0,  0,  0,  0,  0,  0};\
	const VO_U16 cls::s_wAWBPacketSizes[16] = {17, 23, 32, 36, 40, 46, 50, 58, 60,  5,  5,  0,  0,  0,  0,  0};\
	const char* AMR_TAG_STR = "#!AMR\n";\
	const char* AWB_TAG_STR = "#!AMR-WB\n";

//head byte
//0xXSSSSXXX
//S: packet size bit

#define AMR_SAMPLETIME				20	//20<ms/sample>

#ifdef _VONAMESPACE
}
#endif
#endif	//__Amr_File_Data_Struct
