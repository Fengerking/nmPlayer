/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef __JENCODER_H__
#define __JENCODER_H__

#include "jconfig.h"
#include "jcommon.h"
#include  "voJPEG.h"

/* in file encode.c*/
extern void		jpeg_set_quality(JPEG_QUANT_TBL *tbl_ptrs[NUM_BLOCK_TBLS], UINT32 quality);
extern INT32	Init_QuantTab(JPEG_QUANT_TABLE*	quant_tbl);
extern INT32	Init_HuffTab(JPEG_HUFF_ENTROPY* entropy);

/* in file jmark.c */
extern INT32	write_jpeg_header(JPEG_ENCOBJ* jpeg_obj);
extern INT32	write_jpeg_exif(JPEG_ENCOBJ* jpeg_obj);
extern INT32	write_jpeg_trailer(JPEG_STREAM*	bitstream);

/*  in file jsrcdata.c */
extern INT32	get444pblockdata(VO_VIDEO_BUFFER*	indata, 
								 JPEG_MCU_BLOCK*		outblock,
								 PICTURE_ATT*			picture,
								 UINT32					xi,
								 UINT32					yi);


extern INT32	get444iblockdata(VO_VIDEO_BUFFER*	indata, 
								 JPEG_MCU_BLOCK*		outblock,
								 PICTURE_ATT*			picture,
								 UINT32					xi,
								 UINT32					yi);


extern INT32	get422pblockdata(VO_VIDEO_BUFFER*	indata, 
								 JPEG_MCU_BLOCK*		outblock,
								 PICTURE_ATT*			picture,
								 UINT32					xi,
								 UINT32					yi);



extern INT32	get422iYUYVblockdata(VO_VIDEO_BUFFER*	indata, 
									JPEG_MCU_BLOCK*			outblock,
									 PICTURE_ATT*			picture,
									UINT32					xi,
									UINT32					yi);

extern INT32	get422iYVYUblockdata(VO_VIDEO_BUFFER*	indata, 
									JPEG_MCU_BLOCK*			outblock,
									 PICTURE_ATT*			picture,
									 UINT32					xi,
									 UINT32					yi);

extern INT32	get422iUYVYblockdata(VO_VIDEO_BUFFER*	indata, 
									 JPEG_MCU_BLOCK*		outblock,
									 PICTURE_ATT*			picture,
									 UINT32					xi,
									 UINT32					yi);

extern INT32	get422iVYUYblockdata(VO_VIDEO_BUFFER*	indata, 
									 JPEG_MCU_BLOCK*		outblock,
									 PICTURE_ATT*			picture,
									 UINT32					xi,
									 UINT32					yi);

extern INT32	get420pblockdata(VO_VIDEO_BUFFER*	indata, 
								 JPEG_MCU_BLOCK*		outblock,
								 PICTURE_ATT*			picture,
								 UINT32					xi,
								 UINT32					yi);

extern INT32	get420iblockdata(VO_VIDEO_BUFFER*	indata, 
								 JPEG_MCU_BLOCK*		outblock,
								 PICTURE_ATT*			picture,
								 UINT32					xi,
								 UINT32					yi);


extern INT32	getRGB888pblockdata(VO_VIDEO_BUFFER*		indata, 
									JPEG_MCU_BLOCK*		outblock,
									PICTURE_ATT*			picture,
									UINT32					xi,
									UINT32					yi);



extern INT32	getRGB888iblockdata(VO_VIDEO_BUFFER*		indata, 
									JPEG_MCU_BLOCK*			outblock,
									PICTURE_ATT*			picture,
									UINT32					xi,
									UINT32					yi);

extern INT32	getRGB565iblockdata(VO_VIDEO_BUFFER*		indata, 
									JPEG_MCU_BLOCK*			outblock,
									PICTURE_ATT*			picture,
									UINT32					xi,
									UINT32					yi);

/*  in file jresize.c */
extern INT32	YUV444pResize(VO_VIDEO_BUFFER* pindata, 
							  VO_VIDEO_BUFFER* poutdata, 
							  UINT32 oldWidth,
							  UINT32 oldHeight,
							  UINT32 newWidth,
							  UINT32 newHeight);


extern INT32	YUV444iResize(VO_VIDEO_BUFFER* pindata, 
							  VO_VIDEO_BUFFER* poutdata, 
							  UINT32 oldWidth,
							  UINT32 oldHeight,
							  UINT32 newWidth,
							  UINT32 newHeight);


extern INT32	YUV422pResize(VO_VIDEO_BUFFER* pindata, 
							  VO_VIDEO_BUFFER* poutdata, 
							  UINT32 oldWidth,
							  UINT32 oldHeight,
							  UINT32 newWidth,
							  UINT32 newHeight);


extern INT32	YUYV422iResize(VO_VIDEO_BUFFER* pindata, 
							   VO_VIDEO_BUFFER* poutdata, 
							   UINT32 oldWidth,
							   UINT32 oldHeight,
							   UINT32 newWidth,
							   UINT32 newHeight);


extern INT32	UYVY422iResize(VO_VIDEO_BUFFER* pindata, 
							   VO_VIDEO_BUFFER* poutdata, 
							   UINT32 oldWidth,
							   UINT32 oldHeight,
							   UINT32 newWidth,
							   UINT32 newHeight);


extern INT32	YUV420pResize(VO_VIDEO_BUFFER* pindata, 
							  VO_VIDEO_BUFFER* poutdata, 
							  UINT32 oldWidth,
							  UINT32 oldHeight,
							  UINT32 newWidth,
							  UINT32 newHeight);

extern INT32	YUV420iResize(VO_VIDEO_BUFFER* pindata, 
							  VO_VIDEO_BUFFER* poutdata, 
							  UINT32 oldWidth,
							  UINT32 oldHeight,
							  UINT32 newWidth,
							  UINT32 newHeight);


extern INT32	RGB565iResize(VO_VIDEO_BUFFER* pindata, 
							  VO_VIDEO_BUFFER* poutdata, 
							  UINT32 oldWidth,
							  UINT32 oldHeight,
							  UINT32 newWidth,
							  UINT32 newHeight);

/* in file jDctTrans.c */
extern void		jpeg_dctquantfwd(JPEG_MCU_BLOCK*		outblock,
								 JPEG_QUANT_TABLE*		quant_tbl);

/* in file jhuffum.c */ 
extern INT32	jpeg_encodehuffman(JPEG_STREAM*			bitstream,
								   JPEG_MCU_BLOCK*		outblock,
								   JPEG_HUFF_ENTROPY*	entropy);

/*  in file jexif.c */
extern INT32	jpeg_encthumbnail(JPEG_ENCOBJ* jpeg_obj, 
								  VO_VIDEO_BUFFER *pInData);


#endif