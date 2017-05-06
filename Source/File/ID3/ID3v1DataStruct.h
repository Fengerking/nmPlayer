	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		ID3v1DataStruct.h

Contains:	Data Structure Of ID3v1

Written by:	East

Reference:	http://www.id3.org/

Change History (most recent first):
2009-06-19		East			Create file

*******************************************************************************/
#ifndef __ID3V1_Data_Struct
#define __ID3V1_Data_Struct

#include "voType.h"

#define ID3V1_INFO_LEN			128

// ID3V1TAG structure
typedef struct tagID3v1Info
{
	VO_CHAR		szTag[3];		// "TAG"
	VO_CHAR		szTitle[30];
	VO_CHAR		szArtist[30];
	VO_CHAR		szAlbum[30];
	VO_CHAR		szYear[4];
	VO_CHAR		szComment[30];
	VO_U8		btGenre;
} ID3v1Info, *PID3v1Info;


// ID3V1TAG structure
typedef struct tagID3v1_1Info
{
	VO_CHAR		szTag[3];		// "TAG"
	VO_CHAR		szTitle[30];
	VO_CHAR		szArtist[30];
	VO_CHAR		szAlbum[30];
	VO_CHAR		szYear[4];
	VO_CHAR		szComment[28];
	VO_U8		btReserved;
	VO_U8		btAlbumTrack;
	VO_U8		btGenre;
} ID3v1_1Info, *PID3v1_1Info;

#endif	//__ID3V1_Data_Struct