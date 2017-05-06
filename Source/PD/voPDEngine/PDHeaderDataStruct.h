/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		PDHeaderDataStruct.h

Contains:	Data Struct Of Progressive Download Header

Written by:	East

Change History (most recent first):
2007-12-18		East			Create file

*******************************************************************************/
#ifndef _Progressive_Download_Header_Data_Struct_H_
#define _Progressive_Download_Header_Data_Struct_H_

/*
*FOURCC Define
*/
#define FOURCC_PD_Header					mmioFOURCC('V', 'O', 'P', 'D')
#define FOURCC_PD_Source_File_Url			mmioFOURCC('S', 'U', 'R', 'L')
#define FOURCC_PD_Download_Time				mmioFOURCC('T', 'I', 'M', 'E')
#define FOURCC_PD_Source_File_Length		mmioFOURCC('F', 'L', 'E', 'N')
#define FOURCC_PD_DFInfo					mmioFOURCC('M', 'A', 'P', '1')
#define FOURCC_PD_DFInfo2					mmioFOURCC('M', 'A', 'P', '2')

#define PD_Header_Chunk_Size				8
//base building block
typedef struct tagPDHChunk 
{
	FOURCC		fcc;
	DWORD		size;
} PDHChunk, *PPDHChunk;

//file fragment, -->DFFrag
typedef struct tagPDHFileFragment 
{
	DWORD		start;
	DWORD		length;
} PDHFileFragment, *PPDHFileFragment;

//file mapping 1, --> CDFInfo
typedef struct tagPDHFileMapping1 
{
	DWORD		frag_num;
	PDHFileFragment	frags;
} PDHFileMapping1, *PPDHFileMapping1;

//file mapping 2, --> CDFInfo2
typedef struct tagPDHFileMapping2 
{
	DWORD		cont_size;
	DWORD		cont_start_pos;
	DWORD		cont_end_pos;
	DWORD		file_offset;
} PDHFileMapping2, *PPDHFileMapping2;

#endif	//_Progressive_Download_Header_Data_Struct_H_