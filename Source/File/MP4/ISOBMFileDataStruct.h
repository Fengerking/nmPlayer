	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		ISOBMFileDataStruct.h

Contains:	Data Struct Of ISO Base Media File Format

Written by:	East

Reference:	ISO_IEC_14496-12-2004.pdf

Change History (most recent first):
2006-08-21		East			Create file

*******************************************************************************/
#ifndef __ISO_Base_Media_File_Data_Struct
#define __ISO_Base_Media_File_Data_Struct

//FOURCC
#define FOURCC_uuid					MAKEFOURCC('u', 'u', 'i', 'd')	//use extended type(GUID)

#define FOURCC_ftyp					MAKEFOURCC('f', 't', 'y', 'p')	//file type and compatibility

#define FOURCC_moov					MAKEFOURCC('m', 'o', 'o', 'v')	//container for all the metadata
#define FOURCC_mvhd					MAKEFOURCC('m', 'v', 'h', 'd')	//movie header, overall declarations
#define FOURCC_trak					MAKEFOURCC('t', 'r', 'a', 'k')	//container for an individual track or stream
#define FOURCC_tkhd					MAKEFOURCC('t', 'k', 'h', 'd')	//track header, overall information about the track
#define FOURCC_tref					MAKEFOURCC('t', 'r', 'e', 'f')	//track reference container
#define FOURCC_edts					MAKEFOURCC('e', 'd', 't', 's')	//edit list container
#define FOURCC_elst					MAKEFOURCC('e', 'l', 's', 't')	//an edit list
#define FOURCC_mdia 				MAKEFOURCC('m', 'd', 'i', 'a')	//container for the media information in a track
#define FOURCC_mdhd 				MAKEFOURCC('m', 'd', 'h', 'd')	//media header, overall information about the media
#define FOURCC_hdlr 				MAKEFOURCC('h', 'd', 'l', 'r')	//handler, declares the media (handler) type
#define FOURCC_minf 				MAKEFOURCC('m', 'i', 'n', 'f')	//media information container
#define FOURCC_vmhd 				MAKEFOURCC('v', 'm', 'h', 'd')	//video media header, overall information (video track only)
#define FOURCC_smhd 				MAKEFOURCC('s', 'm', 'h', 'd')	//sound media header, overall information (sound track only)
#define FOURCC_hmhd 				MAKEFOURCC('h', 'm', 'h', 'd')	//hint media header, overall information (hint track only)
#define FOURCC_nmhd 				MAKEFOURCC('n', 'm', 'h', 'd')	//Null media header, overall information (some tracks only)
#define FOURCC_dinf 				MAKEFOURCC('d', 'i', 'n', 'f')	//data information box, container
#define FOURCC_dref 				MAKEFOURCC('d', 'r', 'e', 'f')	//data reference box, declares source(s) of media data in track
#define FOURCC_stbl 				MAKEFOURCC('s', 't', 'b', 'l')	//sample table box, container for the time/space map
#define FOURCC_stsd 				MAKEFOURCC('s', 't', 's', 'd')	//sample descriptions (codec types, initialization etc.)
#define FOURCC_stts 				MAKEFOURCC('s', 't', 't', 's')	//(decoding) time-to-sample
#define FOURCC_ctts 				MAKEFOURCC('c', 't', 't', 's')	//(composition) time to sample
#define FOURCC_stsc 				MAKEFOURCC('s', 't', 's', 'c')	//sample-to-chunk, partial data-offset information
#define FOURCC_stsz 				MAKEFOURCC('s', 't', 's', 'z')	//sample sizes (framing)
#define FOURCC_stz2 				MAKEFOURCC('s', 't', 'z', '2')	//compact sample sizes (framing)
#define FOURCC_stco 				MAKEFOURCC('s', 't', 'c', 'o')	//chunk offset, partial data-offset information
#define FOURCC_co64 				MAKEFOURCC('c', 'o', '6', '4')	//64-bit chunk offset
#define FOURCC_stss 				MAKEFOURCC('s', 't', 's', 's')	//sync sample table (random access points)
#define FOURCC_stsh 				MAKEFOURCC('s', 't', 's', 'h')	//shadow sync sample table
#define FOURCC_padb 				MAKEFOURCC('p', 'a', 'd', 'b')	//sample padding bits
#define FOURCC_stdp 				MAKEFOURCC('s', 't', 'd', 'p')	//sample degradation priority
#define FOURCC_mvex 				MAKEFOURCC('m', 'v', 'e', 'x')	//movie extends box
#define FOURCC_mehd 				MAKEFOURCC('m', 'e', 'h', 'd')	//movie extends header box
#define FOURCC_trex 				MAKEFOURCC('t', 'r', 'e', 'x')	//track extends defaults

#define FOURCC_moof 				MAKEFOURCC('m', 'o', 'o', 'f')	//movie fragment
#define FOURCC_mfhd 				MAKEFOURCC('m', 'f', 'h', 'd')	//movie fragment header
#define FOURCC_traf 				MAKEFOURCC('t', 'r', 'a', 'f')	//track fragment
#define FOURCC_tfhd 				MAKEFOURCC('t', 'f', 'h', 'd')	//track fragment header
#define FOURCC_trun 				MAKEFOURCC('t', 'r', 'u', 'n')	//track fragment run

#define FOURCC_mfra 				MAKEFOURCC('m', 'f', 'r', 'a')	//movie fragment random access
#define FOURCC_tfra 				MAKEFOURCC('t', 'f', 'r', 'a')	//track fragment random access
#define FOURCC_mfro 				MAKEFOURCC('m', 'f', 'r', 'o')	//movie fragment random access offset

#define FOURCC_mdat 				MAKEFOURCC('m', 'd', 'a', 't')	//media data container

#define FOURCC_free 				MAKEFOURCC('f', 'r', 'e', 'e')	//free space

#define FOURCC_skip 				MAKEFOURCC('s', 'k', 'i', 'p')	//free space
#define FOURCC_udta 				MAKEFOURCC('u', 'd', 't', 'a')	//user-data
#define FOURCC_cprt 				MAKEFOURCC('c', 'p', 'r', 't')	//copyright etc.
#define FOURCC_sinf 				MAKEFOURCC('s', 'i', 'n', 'f')	//copyright etc.
#define FOURCC_frma 				MAKEFOURCC('f', 'r', 'm', 'a')	//copyright etc.
#define FOURCC_schi 				MAKEFOURCC('s', 'c', 'h', 'i')	//copyright etc.
#define FOURCC_esds 				MAKEFOURCC('e', 's', 'd', 's')	//copyright etc.
#define FOURCC_wave 				MAKEFOURCC('w', 'a', 'v', 'e')	//copyright etc.


#define ISOBM_BOXHEADSIZE		8
typedef struct tagBox
{
	//entire size of the box, including the size and type header, fields, and all contained boxes
	//1 - use large size, 0 - last one, extend to the end of file
	VO_U32	size;
	//'uuid' - use extended type
	VO_U32	type;
} Box, *PBox;

#define ISOBM_FULLBOXHEADSIZE	12
typedef struct tagFullBox
	: public Box
{
	VO_U32	version_flags;
} FullBox, *PFullBox;
#endif	//__ISO_Base_Media_File_Data_Struct