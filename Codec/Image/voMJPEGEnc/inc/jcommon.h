/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __JCOMMON_H__
#define __JCOMMON_H__

#include <string.h>
#include <stdlib.h>
#include "jconfig.h"
#include "jexif.h"
#include "voJPEG.h"
//#include "sdkbase.h"

#ifdef IPP_SUPPORT
#include "ippdefs.h"
#ifdef _WIN32_WCE
#include "ippSP.h"
#include "ippIP.h"
#include "ippJP.h"
#else
#include "ippcore.h"
#include "ippalign.h"
#include "ipps.h"
#include "ippi.h"
#include "ippj.h"
#endif
#endif

typedef enum {			
  M_SOF0  = 0xc0,
  M_SOF1  = 0xc1,
  M_SOF2  = 0xc2,
  M_SOF3  = 0xc3,
  
  M_SOF5  = 0xc5,
  M_SOF6  = 0xc6,
  M_SOF7  = 0xc7,
  
  M_JPG   = 0xc8,
  M_SOF9  = 0xc9,
  M_SOF10 = 0xca,
  M_SOF11 = 0xcb,
  
  M_SOF13 = 0xcd,
  M_SOF14 = 0xce,
  M_SOF15 = 0xcf,
  
  M_DHT   = 0xc4,
  
  M_DAC   = 0xcc,
  
  M_RST0  = 0xd0,
  M_RST1  = 0xd1,
  M_RST2  = 0xd2,
  M_RST3  = 0xd3,
  M_RST4  = 0xd4,
  M_RST5  = 0xd5,
  M_RST6  = 0xd6,
  M_RST7  = 0xd7,
  
  M_SOI   = 0xd8,
  M_EOI   = 0xd9,
  M_SOS   = 0xda,
  M_DQT   = 0xdb,
  M_DNL   = 0xdc,
  M_DRI   = 0xdd,
  M_DHP   = 0xde,
  M_EXP   = 0xdf,
  
  M_APP0  = 0xe0,
  M_APP1  = 0xe1,
  M_APP2  = 0xe2,
  M_APP3  = 0xe3,
  M_APP4  = 0xe4,
  M_APP5  = 0xe5,
  M_APP6  = 0xe6,
  M_APP7  = 0xe7,
  M_APP8  = 0xe8,
  M_APP9  = 0xe9,
  M_APP10 = 0xea,
  M_APP11 = 0xeb,
  M_APP12 = 0xec,
  M_APP13 = 0xed,
  M_APP14 = 0xee,
  M_APP15 = 0xef,
  
  M_JPG0  = 0xf0,
  M_JPG13 = 0xfd,
  M_COM   = 0xfe,
  
  M_TEM   = 0x01,
  
  M_ERROR = 0x100
} JPEG_MARKER;

typedef struct {
	INT32 width;
	INT32 heigth;
	UINT32 image_type;
	UINT32 MCU_width;
	UINT32 MCU_heigth;
	UINT32 MCU_XNum;
	UINT32 MCU_YNum;
	UINT32 v_sample[3];
	UINT32 h_sample[3];
} PICTURE_ATT;

typedef struct {
	UINT16	blockNum;
	UINT16	lumblockNum;
	INT16*	WorkBlock[JPEG_MAX_BLOCKS];
	INT16	QuantImage[JPEG_MAX_BLOCKS];
	INT16	dcImage[JPEG_MAX_BLOCKS];
	INT16	DC_pred[JPEG_MAX_COLOR];
} JPEG_MCU_BLOCK;

typedef struct {
	UINT8	quantrawval[JPEG_BLOCK_SIZE];
	UINT16	quantval[JPEG_BLOCK_SIZE];		 		
	UINT16	invquantval[JPEG_BLOCK_SIZE];		 		
} JPEG_QUANT_TBL;

typedef struct {
  JPEG_QUANT_TBL *Quant_tbl_ptrs[NUM_BLOCK_TBLS];
} JPEG_QUANT_TABLE;

/* Huffman coding tables. */
typedef struct {  
  UINT8 huffval[256];
  UINT8 bits[18];
} JPEG_HUFF_TBL;

typedef struct {
#ifdef IPP_SUPPORT
  IppiEncodeHuffmanSpec* pHuffTbl;
#else
  UINT16 hufcode[16];
  UINT8	 hufsize[16];
 #endif
} JPEG_ENC_HUM_DC_SPEC;

typedef struct {
#ifdef IPP_SUPPORT
  IppiEncodeHuffmanSpec* pHuffTbl;
#else
  UINT16 hufcode[256];
  UINT8	 hufsize[256];
 #endif
} JPEG_ENC_HUM_AC_SPEC;

typedef struct {
   JPEG_ENC_HUM_AC_SPEC ac_huff_tbls[NUM_BLOCK_TBLS];
   JPEG_ENC_HUM_DC_SPEC dc_huff_tbls[NUM_BLOCK_TBLS];
} JPEG_HUFF_ENTROPY;

extern const UINT8 std_luminance_quant_tbl[JPEG_BLOCK_SIZE];
extern const UINT8 std_chrominance_quant_tbl[JPEG_BLOCK_SIZE];
extern const UINT8 bits_dc_luminance[17];
extern const UINT8 val_dc_luminance[];
extern const UINT8 bits_dc_chrominance[17];
extern const UINT8 val_dc_chrominance[];
extern const UINT8 bits_ac_luminance[17];
extern const UINT8 val_ac_luminance[];
extern const UINT8 bits_ac_chrominance[17];
extern const UINT8 val_ac_chrominance[];
extern const UINT8 zig_zag_tab_index[JPEG_BLOCK_SIZE];

typedef struct {
  UINT8  *next_output_byte;
  UINT32 buffer_size;
  UINT32 free_in_buffer;	
  UINT32 bits_index;
  UINT32 cache;
}JPEG_STREAM;

typedef INT32 (*_jpeg_getdata)(VO_VIDEO_BUFFER*, 
							   JPEG_MCU_BLOCK*,
							   PICTURE_ATT*,
							   UINT32,
							   UINT32);

typedef INT32 (*_jpeg_resize)(VO_VIDEO_BUFFER*, 
							  VO_VIDEO_BUFFER*, 
							  UINT32,
							  UINT32,
							  UINT32,
						      UINT32);


typedef struct {
	JPEG_STREAM*		J_stream;
	PICTURE_ATT*		picture;
	JPEG_QUANT_TABLE*	Quant_tbl;
	JPEG_HUFF_ENTROPY*	entropy;
	JPEG_MCU_BLOCK*		MBlock;
	EXIFLibrary*		jExifLibary;
	VO_HANDLE			hThumbHand;
	VO_VIDEO_BUFFER	ThumbInData;
	VO_CODECBUFFER	ThumbOutData;
	_jpeg_getdata		jpeg_getdata;
	_jpeg_resize		jpeg_resize;
	UINT32				picquality;
	UINT32				exif_support;
	UINT32				thumb_support;
	UINT32				headwrited;
	UINT32				XBlock;
	UINT32				YBlock;
	UINT32              reversal_support; //zou 1/5/2009
	VO_PTR              phCheck;//huwei 20090918 checkLib

}JPEG_ENCOBJ;


extern int	initstream(JPEG_STREAM *stream, UINT8* outBuffer, UINT32 length);
extern int	putbyte(JPEG_STREAM *stream, UINT32 val);
extern int	put2byte(JPEG_STREAM *stream, UINT32 val, UINT32 endianflag);
extern int	put4byte(JPEG_STREAM *stream, UINT32 val, UINT32 endianflag);
extern int	putmarker(JPEG_STREAM *stream, JPEG_MARKER mark);
extern int	putbits(JPEG_STREAM *stream, UINT32 val, UINT32 n);

#endif 