/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __JDHUFFUM_H__
#define __JDHUFFUM_H__

#include "jconfig.h"
#include "jcommon.h"
#include "bitstream.h"


typedef INT32 (*_decode_MCU)(Jpeg_DecOBJ *);

extern INT32	InitHuffTab(Jpeg_DecOBJ* decoder,
							JPEG_HUFF_VLC		*huff_vlc,
							JPEG_HUFF_ENTROPY	*entropy,
							JPEG_HUFF_TABLE		*huff_tbl);

extern INT32	InitpHuffTab(Jpeg_DecOBJ *decoder);

extern INT32	InitVLCTab(Jpeg_DecOBJ* decoder,
						   VLC_TAB				*vtab,
						   JPEG_ENC_HUM_SPEC	*huf_spec,
						   const UINT8			*bits,
						   const UINT8			*val);

extern INT32	decode_MCU(Jpeg_DecOBJ *decoder);	

extern INT32	decode_MCU_DC_first(Jpeg_DecOBJ *decoder);
extern INT32	decode_MCU_AC_first(Jpeg_DecOBJ *decoder);
extern INT32	decode_MCU_DC_refine(Jpeg_DecOBJ *decoder);
extern INT32	decode_MCU_AC_refine(Jpeg_DecOBJ *decoder);
						
extern void		FreeHuffTab(JPEG_HUFF_VLC		*huff_vlc);

#endif //__JDHUFFUM_H__

