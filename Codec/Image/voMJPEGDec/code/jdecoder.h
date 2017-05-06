/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __JDECODER_H__
#define __JDECODER_H__

#include "vojconfig.h"
#include "jcommon.h"
#include "jexif.h"
#include "bitstream.h"
#include "jdHuffum.h"

#ifdef __cplusplus
extern "C" 
{
#endif

extern	INT32	read_jpeg_header(Jpeg_DecOBJ *decoder,
								 JPEG_STREAM* bitstream,
								 JPEG_PARA*	jpara);
extern	INT32	read_marker(Jpeg_DecOBJ *decoder,
							JPEG_STREAM* bitstream,
							JPEG_PARA*	jpara, 
							UINT32	Marker);
extern	INT32	synic_header(JPEG_STREAM* bitstream);
extern	INT32	find_nextMark(JPEG_STREAM* bitstream, INT32* Marker);
extern	INT32	find_exifMark(JPEG_STREAM* bitstream);

extern INT32	init_exif(Jpeg_DecOBJ *decoder, JPEG_EXIF	*jexif);
extern INT32	decoder_exif(JPEG_EXIF	*jexif);

extern INT32	init_setup(Jpeg_DecOBJ *decoder); 

extern	INT32	JPEG_interlmcu_dec(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData);
extern	INT32	JPEG_interlmcu_dec8(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData);
extern	INT32	JPEG_progrmcu_dec(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData);


extern	INT32	JPEG_idct_dec(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData);
extern	INT32	JPEG_idct_dec8(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData);

#if defined(VOARMV7)
extern	void	IDCT_Block8x8_c(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_c2(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_c4(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_c8(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);

extern	void	IDCT_Block8x8_8X8_ARMv7(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_4X4_ARMv7(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_2X2_ARMv7(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_1X1_ARMv7(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);

extern	void	IDCT_Block8x8_8X8_ARMv6(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src,INT32 srcStride);
extern	void	IDCT_Block8x8_4X4_ARMv6(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_2X2_ARMv6(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src,INT32 srcStride);
extern	void	IDCT_Block8x8_1X1_ARMv6(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);

#define voIDCT8x8_8x8  IDCT_Block8x8_8X8_ARMv7
#define voIDCT8x8_4x4  IDCT_Block8x8_4X4_ARMv7
#define voIDCT8x8_2x2  IDCT_Block8x8_2X2_ARMv7
#define voIDCT8x8_1x1  IDCT_Block8x8_1X1_ARMv7

#define voIDCT8x8_8x8_V6  IDCT_Block8x8_8X8_ARMv6
#define voIDCT8x8_4x4_V6  IDCT_Block8x8_4X4_ARMv6
#define voIDCT8x8_2x2_V6  IDCT_Block8x8_2X2_ARMv6
#define voIDCT8x8_1x1_V6  IDCT_Block8x8_1X1_ARMv6

#elif defined(VOARMV6)
extern	void	IDCT_Block8x8_8X8_ARMv6(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src,INT32 srcStride);
extern	void	IDCT_Block8x8_4X4_ARMv6(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_2X2_ARMv6(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src,INT32 srcStride);
extern	void	IDCT_Block8x8_1X1_ARMv6(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);

#define voIDCT8x8_8x8  IDCT_Block8x8_8X8_ARMv6
#define voIDCT8x8_4x4  IDCT_Block8x8_4X4_ARMv6
#define voIDCT8x8_2x2  IDCT_Block8x8_2X2_ARMv6
#define voIDCT8x8_1x1  IDCT_Block8x8_1X1_ARMv6

#define voIDCT8x8_8x8_V6  IDCT_Block8x8_8X8_ARMv6
#define voIDCT8x8_4x4_V6  IDCT_Block8x8_4X4_ARMv6
#define voIDCT8x8_2x2_V6  IDCT_Block8x8_2X2_ARMv6
#define voIDCT8x8_1x1_V6  IDCT_Block8x8_1X1_ARMv6

#else
extern	void	IDCT_Block8x8_c(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_c2(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_c4(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);
extern	void	IDCT_Block8x8_c8(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride);

#define voIDCT8x8_8x8  IDCT_Block8x8_c
#define voIDCT8x8_4x4  IDCT_Block8x8_c2
#define voIDCT8x8_2x2  IDCT_Block8x8_c4
#define voIDCT8x8_1x1  IDCT_Block8x8_c8

#define voIDCT8x8_8x8_V6  IDCT_Block8x8_c
#define voIDCT8x8_4x4_V6  IDCT_Block8x8_c2
#define voIDCT8x8_2x2_V6  IDCT_Block8x8_c4
#define voIDCT8x8_1x1_V6  IDCT_Block8x8_c8

#endif

#if defined(VOJPEGFLAGE)
extern VOMJPEGDECRETURNCODE VO_API Initpicture(Jpeg_DecOBJ * decoder);
extern VOMJPEGDECRETURNCODE VO_API UpdateParameter(Jpeg_DecOBJ * decoder, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat);
#else
extern VOMJPEGDECRETURNCODE	Initpicture(Jpeg_DecOBJ * decoder);
extern VOMJPEGDECRETURNCODE	UpdateParameter(Jpeg_DecOBJ * decoder, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat,UINT32 progressMode);
#endif

#ifdef __cplusplus
}
#endif


#endif //__JDECODER_H__

