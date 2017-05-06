	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
File:		AuFileDataStruct.h

Contains:	Data Structure Of AU file

Written by:	East

Reference:	http://wwwzpt.tele.pw.edu.pl/~ptomasze/ucyf/2004z/2/Sun%20_au%20sound%20file%20format.htm
			http://en.wikipedia.org/wiki/Au_file_format

Change History (most recent first):
2009-10-21		East			Create file

*******************************************************************************/
#ifndef __Au_File_Data_Struct
#define __Au_File_Data_Struct

#include "voType.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define FOURCC_snd					MAKEFOURCC('.', 's', 'n', 'd')

enum
{	
	AU_ENCODING_ULAW_8					= 1,	/* 8-bit u-law samples */

	AU_ENCODING_PCM_8					= 2,	/* 8-bit linear samples */
	AU_ENCODING_PCM_16					= 3,	/* 16-bit linear samples */
	AU_ENCODING_PCM_24					= 4,	/* 24-bit linear samples */
	AU_ENCODING_PCM_32					= 5,	/* 32-bit linear samples */

	AU_ENCODING_FLOAT					= 6,	/* floating-point samples */
	AU_ENCODING_DOUBLE					= 7,	/* double-precision float samples */
	AU_ENCODING_INDIRECT				= 8,	/* fragmented sampled data */
	AU_ENCODING_NESTED					= 9,	/* ? */
	AU_ENCODING_DSP_CORE				= 10,	/* DSP program */
	AU_ENCODING_DSP_DATA_8				= 11,	/* 8-bit fixed-point samples */
	AU_ENCODING_DSP_DATA_16				= 12,	/* 16-bit fixed-point samples */
	AU_ENCODING_DSP_DATA_24				= 13,	/* 24-bit fixed-point samples */
	AU_ENCODING_DSP_DATA_32				= 14,	/* 32-bit fixed-point samples */

	AU_ENCODING_DISPLAY					= 16,	/* non-audio display data */
	AU_ENCODING_MULAW_SQUELCH			= 17,	/* ? */
	AU_ENCODING_EMPHASIZED				= 18,	/* 16-bit linear with emphasis */
	AU_ENCODING_NEXT					= 19,	/* 16-bit linear with compression (NEXT) */
	AU_ENCODING_COMPRESSED_EMPHASIZED	= 20,	/* A combination of the two above */
	AU_ENCODING_DSP_COMMANDS			= 21,	/* Music Kit DSP commands */
	AU_ENCODING_DSP_COMMANDS_SAMPLES	= 22,	/* ? */

	AU_ENCODING_ADPCM_G721_32			= 23,	/* G721 32 kbs ADPCM - 4 bits per sample. */
	AU_ENCODING_ADPCM_G722				= 24,	/* G722 64 kbs ADPCM - 8 bits per sample  */
	AU_ENCODING_ADPCM_G723_24			= 25,	/* G723 24 kbs ADPCM - 3 bits per sample. */
	AU_ENCODING_ADPCM_G723_40			= 26,	/* G723 40 kbs ADPCM - 5 bits per sample. */

	AU_ENCODING_ALAW_8					= 27,	/* 8-bit a-law samples */

	AU_ENCODING_TYPE_MAX				= VO_MAX_ENUM_VALUE
};

typedef struct tagAUFileHdr
{
	VO_U32 magic;		/* magic number */
	VO_U32 hdr_size;	/* size of this header */
	VO_U32 data_size;	/* length of data (optional) */
	VO_U32 encoding;	/* data encoding format */
	VO_U32 sample_rate;	/* samples per second */
	VO_U32 channels;	/* number of interleaved channels */
} AUFileHdr, *PAUFileHdr;

#ifdef _VONAMESPACE
}
#endif

#endif	//__Au_File_Data_Struct
