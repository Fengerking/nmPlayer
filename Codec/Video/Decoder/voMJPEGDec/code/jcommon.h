/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __JCOMMON_H__
#define __JCOMMON_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jconfig.h"
#include "voMJPEG.h"
#include "jexif.h"

#ifdef __cplusplus
extern "C" 
{
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
  
  M_ERROR = 0x100,
  M_MAX   = VO_MAX_ENUM_VALUE
} JPEG_MARKER;

typedef enum {
	F_START = 0,
	F_EXIFMARK,
	F_SOSMARK,
	F_RAWDATA,
	F_END,
	F_MAX  = VO_MAX_ENUM_VALUE
}FILE_POSITION;

typedef struct {
	UINT32	index;
	UINT32	com_index;
	UINT32	v_sample;
	UINT32	h_sample;
	UINT32	v_blocknum;
	UINT32	h_blocknum;
	UINT32	dc_tbl_no;
	UINT32	ac_tbl_no; 
	UINT32	quant_tbl_no;
} JPEG_COMP_ITEM;

typedef struct {
	UINT32	blockNum;
	INT16*	WorkBlock[JPEG_MAX_BLOCKS];
	INT16	dcImage[JPEG_MAX_BLOCKS];
	INT16	DC_pred[JPEG_MAX_COLOR];
} JPEG_MCU_BLOCK;

typedef struct {
	UINT8	quantrawval[JPEG_BLOCK_SIZE + 7];
	INT32	quantval[JPEG_BLOCK_SIZE + 7];		 		
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
  UINT32 huffcode[256];
  UINT32 huffval[256];
  UINT8	 huffsize[256];
} JPEG_ENC_HUM_SPEC;

typedef struct {
  JPEG_HUFF_TBL * dc_huff_tbl_ptrs[NUM_HUFF_TBLS];
  JPEG_HUFF_TBL * ac_huff_tbl_ptrs[NUM_HUFF_TBLS];
} JPEG_HUFF_TABLE;

typedef struct
{
	UINT16  *table;
	INT32	pos;
	INT32	size;
} VLC_TAB;

typedef struct 
{
	VLC_TAB	dc_vlc_tab[NUM_HUFF_TBLS];
	VLC_TAB	ac_vlc_tab[NUM_HUFF_TBLS];
} JPEG_HUFF_VLC;


typedef struct {
	UINT32	cache;
	UINT32	used_bits;
	UINT32	unread_marker;
} JPEG_CACHE;

typedef struct {
  JPEG_ENC_HUM_SPEC dc_huff_tbls[NUM_HUFF_TBLS];
  JPEG_ENC_HUM_SPEC ac_huff_tbls[NUM_HUFF_TBLS];
} JPEG_HUFF_ENTROPY;

typedef struct {
  FILE*	 inFile;
  UINT8	 *buffer_init;
  UINT8	 *next_output_byte;
  UINT8  *buffer_init_pos;//huwei 20091019 add input buffer
  UINT32 buffer_size;
  UINT32 input_data_size; //huwei 20091019 add input buffer
  INT32  free_in_buffer;	
  UINT32 bits_index;
}JPEG_STREAM;

typedef struct{
	JPEG_COMP_ITEM		compon[JPEG_MAX_COLOR];
	JPEG_QUANT_TABLE	quant_tbl;
	JPEG_HUFF_TABLE		huffum_tbl;
	JPEG_MCU_BLOCK		MCUBlock;
	JPEG_COMP_ITEM*		cur_com_ptr[JPEG_MAX_COLOR];
	UINT32				blockCount_v[JPEG_MAX_COLOR];
	UINT32				width;
	UINT32				heigth;
	UINT32				image_type;
	UINT32				MCU_width;
	UINT32				MCU_heigth;
	UINT32				MCU_XNum;
	UINT32				MCU_YNum;
	UINT32				num_component;
	UINT32				progressMode;
	UINT32				data_precision;
	UINT32				restart_intval;
	UINT32				restarts_to_go;
	UINT32				max_h_sample;
	UINT32				max_v_sample;
	UINT32				EOBRUN;
	UINT32				Ns;
	UINT32				Ss;
	UINT32				Se;
	UINT32				Ah;
	UINT32				Al;		
} JPEG_PARA;

typedef struct
{
	UINT32	start_X;
	UINT32	start_Y;
	UINT32  end_X;
	UINT32	end_Y;
} IMAGE_FIELD;

typedef struct{
	VO_HANDLE			exifhCodec;
	JPEG_STREAM			exifbitstream;	
	EXIFLibrary			exiflib;
	UINT32				bigend;
	UINT32				offset0;
	UINT32				offset1;
} JPEG_EXIF;

typedef void (*_IDCT_Block8x8)(INT16*, 
							   UINT8*, 
							   INT32, 
							   const UINT8*,
							   INT32 srcStride
							   );

typedef struct {	
	JPEG_STREAM			instream;
	JPEG_CACHE			b_cache;
	JPEG_PARA			jpara;
	JPEG_HUFF_VLC		Huff_vlc;
	JPEG_HUFF_ENTROPY	dentropy;
	JPEG_EXIF			exifMark;
	IMAGE_FIELD			imagefield;
	_IDCT_Block8x8		idct_block;
	UINT32				imagedsize[JPEG_MAX_COLOR];
	UINT32				widthstrid[JPEG_MAX_COLOR];
	UINT8				*outBuffer[JPEG_MAX_COLOR];
	UINT32				MCU_funct;
	UINT32				IsdecExif;
	UINT32				Isfield;
	UINT32				pre_BlockNum;
	UINT32				field_Num;
	UINT32				field_step;
	UINT32				field_hmax;
	UINT32				field_vmax;
	UINT32				Buffer_size;
	UINT32				Headerdone;
	UINT32				HeaderLength;
	UINT32				input_mode;
	UINT32				EnstepDec;
	UINT32				stepDec_num;
	UINT32				instepDec_num;
	UINT32				zoomout;
	UINT32				addstep;
	UINT32				block_Count;
	UINT32				break_mode;
	UINT32				Y_block;
	UINT32              enableNormalChroma;/*huwei 20090725 normal chroma*/
	VO_PTR              phCheck;//huwei 20090918 checkLib
	UINT8               *chromaBuffer;
	VO_MEM_VIDEO_OPERATOR memoryShare;
	VO_MEM_OPERATOR       memoryOperator;
	FILE_POSITION		fposition;
}Jpeg_DecOBJ;

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

extern INT32	initstream(JPEG_STREAM *stream, UINT8* outBuffer, UINT32 length);
extern INT32	initfstream(Jpeg_DecOBJ* decoder, JPEG_STREAM *stream,   char* pfilepath);
extern INT32	feekstream(JPEG_STREAM *stream,   UINT32 length);
extern INT32	seekstream(JPEG_STREAM *stream,   UINT32 length);//huwei 20091019 add input buffer
extern INT32	fillbuffer(JPEG_STREAM * stream);
extern INT32	releasestream(Jpeg_DecOBJ* decoder, JPEG_STREAM *stream);

extern INT32	getbyte(JPEG_STREAM *stream, UINT32* val, UINT32 bigend);
extern INT32	get2byte(JPEG_STREAM *stream, UINT32* val, UINT32 bigend);
extern INT32	get4byte(JPEG_STREAM *stream, UINT32* val, UINT32 bigend);



static void* vo_memmove(void *dest, const void *src,int n) 
{ 
	char *psrc = (char*)src; 
	char *pdest = (char*)dest;
	int i;

	if (n == 0) return 0; 
	if (dest == NULL) return 0; 
	if (src == NULL)    return 0; 

	if((pdest <= psrc) || (pdest >= psrc + n))
	{ 
		for(i=0; i < n; i++)
		{ 
			*pdest = *psrc; 
			psrc++; 
			pdest++; 
		} 
	} 
	else 
	{ 
		psrc += n; 
		pdest += n; 
		for(i = 0;i < n; i++) 
		{ 
			psrc--; 
			pdest--; 
			*pdest = *psrc; 
		} 
	} 
	return dest;
}

#ifdef __cplusplus
}
#endif

#endif 



