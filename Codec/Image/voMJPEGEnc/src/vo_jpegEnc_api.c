/*
**
** File:        "vo_facedet_api.c"
**
** Description:     Top-level source code for Face Detection
**
**
*/

#include "jconfig.h"
#include "jcommon.h"
#include "mem_align.h"
#include "jencoder.h"
#include "jexif.h"
#include  "voJPEG.h"

#ifdef LICENSEFILE 
#include "voCheck.h"
#endif

#ifdef VOARMV7
void jpeg_dctquantfwd_ARMv7(JPEG_MCU_BLOCK*, JPEG_QUANT_TABLE*);
#endif

void	jpeg_dctquantfwd1(JPEG_MCU_BLOCK*		outblock,
                          JPEG_QUANT_TABLE*		quant_tbl);

#ifndef VOMJPEGENC
VO_HANDLE g_hJPEGEncInst = NULL;
#endif

typedef struct{	
	VO_VOID* pDecCore;
	VO_VIDEO_BUFFER OutData;
	VO_CODECBUFFER  InputData;
	VO_VIDEO_OUTPUTINFO OutPutInfo;
}JPEGDEC_FRONT;
VO_U32 VO_API voJPEGEncSetParameter2(VO_HANDLE hCodec, VO_S32 nID, VO_PTR pValue);
VO_U32 VO_API voJPEGEncInit2(VO_HANDLE *phCodec, VO_IMAGE_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);

#ifdef VOARMV6 
void	jpeg_dctquantfwd_ARMv6(JPEG_MCU_BLOCK*		outblock, JPEG_QUANT_TABLE*		quant_tbl);
#endif

static VOJPEGENCRETURNCODE UpdataParameter2(JPEG_ENCOBJ* jpeg_obj)
{
	VOJPEGENCRETURNCODE retc;
	UINT32 image_type;
	PICTURE_ATT*		picture;
	JPEG_MCU_BLOCK*		MBlock;

	picture = jpeg_obj->picture;
	MBlock  = jpeg_obj->MBlock;

	image_type = picture->image_type;

	if(picture->width <= 0)
		return VO_ERR_JPEGENC_WIDTH_INVALID;

	if(picture->heigth <= 0)
		return VO_ERR_JPEGENC_HEIGHT_INVALID;

	switch(image_type) {
	case VO_COLOR_YUV_PLANAR444://VOYUV_PLANAR444:
	case VO_COLOR_YUV444_PACKED://VOYUV444_PACKED:
		picture->MCU_width	= 8;
		picture->MCU_heigth	= 8;
		picture->MCU_XNum = (picture->width + 7) >> 3;
		picture->MCU_YNum = (picture->heigth + 7) >> 3;
		picture->v_sample[0] = picture->h_sample[0] = 1;
		picture->v_sample[1] = picture->h_sample[1] = 1;
		picture->v_sample[2] = picture->h_sample[2] = 1;
		MBlock->lumblockNum = 1;
		MBlock->blockNum = 3;
		MBlock->QuantImage[0] = 0;
		MBlock->QuantImage[1] = 1;
		MBlock->QuantImage[2] = 1;
		MBlock->dcImage[0] = 0;
		MBlock->dcImage[1] = 1;
		MBlock->dcImage[2] = 2;	
		break;
	case VO_COLOR_YUV_PLANAR422_12://VOYUV_PLANAR422_12:
	case VO_COLOR_YUYV422_PACKED://VOYUYV422_PACKED:
	case VO_COLOR_YVYU422_PACKED:
	case VO_COLOR_UYVY422_PACKED:
	case VO_COLOR_VYUY422_PACKED:
		picture->MCU_width	= 16;	
		picture->MCU_heigth	=  8;
		picture->MCU_XNum = (picture->width + 15) >> 4;
		picture->MCU_YNum = (picture->heigth + 7) >> 3;
		picture->h_sample[0] = 2;
		picture->v_sample[0] = 1;
		picture->v_sample[1] = picture->h_sample[1] = 1;
		picture->v_sample[2] = picture->h_sample[2] = 1;
		MBlock->lumblockNum = 2;
		MBlock->blockNum = 4;
		MBlock->QuantImage[0] = 0;
		MBlock->QuantImage[1] = 0;
		MBlock->QuantImage[2] = 1;
		MBlock->QuantImage[3] = 1;
		MBlock->dcImage[0] = 0;
		MBlock->dcImage[1] = 0;
		MBlock->dcImage[2] = 1;
		MBlock->dcImage[3] = 2;
		break;
	case VO_COLOR_YUV_PLANAR420:
	case VO_COLOR_YVU_PLANAR420:
	case VO_COLOR_RGB888_PACKED:
	case VO_COLOR_RGB565_PACKED:
	case VO_COLOR_RGB888_PLANAR:
	case VO_COLOR_YUV_420_PACK:
	case VO_COLOR_YUV_420_PACK_2:
		picture->MCU_width	= 16;	
		picture->MCU_heigth	= 16;
		picture->MCU_XNum = (picture->width +  15) >> 4;
		picture->MCU_YNum = (picture->heigth + 15) >> 4;
		picture->v_sample[0] = picture->h_sample[0] = 2;
		picture->v_sample[1] = picture->h_sample[1] = 1;
		picture->v_sample[2] = picture->h_sample[2] = 1;
		MBlock->blockNum = 6;
		MBlock->lumblockNum = 4;
		MBlock->QuantImage[0] = 0;
		MBlock->QuantImage[1] = 0;
		MBlock->QuantImage[2] = 0;
		MBlock->QuantImage[3] = 0;
		MBlock->QuantImage[4] = 1;
		MBlock->QuantImage[5] = 1;
		MBlock->dcImage[0] = 0;
		MBlock->dcImage[1] = 0;
		MBlock->dcImage[2] = 0;
		MBlock->dcImage[3] = 0;
		MBlock->dcImage[4] = 1;
		MBlock->dcImage[5] = 2;
		break;
	default:
		return VO_ERR_JPEGENC_UNSUPPORT_COLOR;
	}

	switch(image_type) {
	case VO_COLOR_YUV_PLANAR444:
		jpeg_obj->jpeg_getdata = get444pblockdata;
		jpeg_obj->jpeg_resize  = YUV444pResize;
		break;
	case VO_COLOR_YUV444_PACKED:
		jpeg_obj->jpeg_getdata = get444iblockdata;
		jpeg_obj->jpeg_resize  = YUV444iResize;
		break;
	case VO_COLOR_YUV_PLANAR422_12:
		jpeg_obj->jpeg_getdata = get422pblockdata;
		jpeg_obj->jpeg_resize  = YUV422pResize;
		break;
	case VO_COLOR_YUYV422_PACKED:
		jpeg_obj->jpeg_getdata = get422iYUYVblockdata;
		jpeg_obj->jpeg_resize  = YUYV422iResize;
		break;
	case VO_COLOR_YVYU422_PACKED:
		jpeg_obj->jpeg_getdata = get422iYVYUblockdata;
		jpeg_obj->jpeg_resize  = YUYV422iResize;
		break;
	case VO_COLOR_UYVY422_PACKED:
		jpeg_obj->jpeg_getdata = get422iUYVYblockdata;
		jpeg_obj->jpeg_resize  = UYVY422iResize;
		break;
	case VO_COLOR_VYUY422_PACKED:
		jpeg_obj->jpeg_getdata = get422iVYUYblockdata;
		jpeg_obj->jpeg_resize  = UYVY422iResize;
		break;
	case VO_COLOR_YUV_PLANAR420:
	case VO_COLOR_YVU_PLANAR420:
		jpeg_obj->jpeg_getdata = get420pblockdata;
		jpeg_obj->jpeg_resize  = YUV420pResize;
		break;
	case VO_COLOR_YUV_420_PACK:
	case VO_COLOR_YUV_420_PACK_2:
		jpeg_obj->jpeg_getdata = get420iblockdata;
		jpeg_obj->jpeg_resize  = YUV420iResize;
		break;
	case VO_COLOR_RGB888_PACKED:
		jpeg_obj->jpeg_getdata = getRGB888iblockdata;
		jpeg_obj->jpeg_resize  = YUV444iResize;
		break;
	case VO_COLOR_RGB565_PACKED:
		jpeg_obj->jpeg_getdata = getRGB565iblockdata;
		jpeg_obj->jpeg_resize  = RGB565iResize;
		break;
	case VO_COLOR_RGB888_PLANAR:
		jpeg_obj->jpeg_getdata = getRGB888pblockdata;
		jpeg_obj->jpeg_resize  = YUV444pResize;
		break;
	default:
		return VO_ERR_JPEGENC_UNSUPPORT_COLOR;
	}

	if(jpeg_obj->thumb_support)
	{
		int thumb_quality = DEFAULT_THUMBPIC_QUALITY ;
		//retc = voJPEGEncInit2(&jpeg_obj->hThumbHand);
		retc = voJPEGEncInit2(&jpeg_obj->hThumbHand,VO_IMAGE_CodingJPEG,NULL);
		voJPEGEncSetParameter2(jpeg_obj->hThumbHand, VO_PID_JPEG_QUALITY, &thumb_quality);
	}
	
	memset(jpeg_obj->MBlock->DC_pred, 0, JPEG_MAX_COLOR * sizeof(INT16));


	return retc;
}

VO_U32 VO_API voJPEGEncInit2(VO_HANDLE *phCodec, VO_IMAGE_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	INT32 ret = VO_ERR_NONE;
	UINT32 i;
	UINT32 tabsize = 0;
	UINT8* buffer = NULL;
	JPEG_ENCOBJ*  jpeg_obj;
	EXIFLibrary*  exiflib;	

//	UINT32 imagetype = VO_COLOR_YUV_PLANAR420;
	//VOVIDEOTYPE imagetype = VOYUV_PLANAR420;
#ifdef LICENSEFILE    
  VO_PTR  gHandle = NULL;
  if(pUserData==NULL)
  {
    ret=voCheckLibInit(&gHandle, VO_INDEX_ENC_MJPEG, 2, g_hJPEGEncInst,NULL);
  }
  else
    if((pUserData->memflag & 0xF0) == 0x10  && pUserData->libOperator != NULL)
      ret=voCheckLibInit(&gHandle, VO_INDEX_ENC_MJPEG, 2, g_hJPEGEncInst,pUserData->libOperator);
    else
      ret=voCheckLibInit(&gHandle, VO_INDEX_ENC_MJPEG, 2, g_hJPEGEncInst,NULL);

  if(ret != VO_ERR_NONE)
   {
    //*phDec = NULL;
    if(gHandle)
    {
     voCheckLibUninit(gHandle);
     gHandle = NULL;
    }
    return ret;
   }
#endif

	if(VO_IMAGE_CodingJPEG != vType){
		return VO_ERR_JPEGDEC_DECODE_HEADER_ERR;
	}
	jpeg_obj = (JPEG_ENCOBJ*)mem_malloc(sizeof(JPEG_ENCOBJ), ALIGN_NUM_8);
	if(jpeg_obj == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(jpeg_obj, 0, sizeof(JPEG_ENCOBJ));
	
	jpeg_obj->picture = (PICTURE_ATT *)mem_malloc(sizeof(PICTURE_ATT), ALIGN_NUM_32);
	if(jpeg_obj->picture == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(jpeg_obj->picture, 0, sizeof(PICTURE_ATT));

	jpeg_obj->J_stream = (JPEG_STREAM *)mem_malloc(sizeof(JPEG_STREAM), ALIGN_NUM_8);
	if(jpeg_obj->J_stream == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(jpeg_obj->J_stream, 0, sizeof(JPEG_STREAM));

	jpeg_obj->Quant_tbl = (JPEG_QUANT_TABLE *)mem_malloc(sizeof(JPEG_QUANT_TABLE), ALIGN_NUM_32);
	if(jpeg_obj->Quant_tbl == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(jpeg_obj->Quant_tbl, 0, sizeof(JPEG_QUANT_TABLE));

	jpeg_obj->entropy = (JPEG_HUFF_ENTROPY *)mem_malloc(sizeof(JPEG_HUFF_ENTROPY), ALIGN_NUM_8);
	if(jpeg_obj->entropy == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(jpeg_obj->entropy, 0, sizeof(JPEG_HUFF_ENTROPY));

	jpeg_obj->MBlock = (JPEG_MCU_BLOCK *)mem_malloc(sizeof(JPEG_MCU_BLOCK), ALIGN_NUM_8);
	if(jpeg_obj->MBlock == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(jpeg_obj->MBlock, 0, sizeof(JPEG_MCU_BLOCK));

	exiflib = (EXIFLibrary *)mem_malloc(sizeof(EXIFLibrary), ALIGN_NUM_8);
	if(exiflib == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(exiflib, 0, sizeof(EXIFLibrary));
	jpeg_obj->jExifLibary = exiflib;
	
	jpeg_obj->exif_support = 0;
	jpeg_obj->headwrited = 0;
	jpeg_obj->XBlock = 0;
	jpeg_obj->YBlock = 0;
	jpeg_obj->picquality = DEFAULT_QUALITY;
	
	jpeg_obj->picture->width = 0;
	jpeg_obj->picture->heigth = 0;
	jpeg_obj->picture->image_type = -1;

	jpeg_obj->reversal_support = 0;
	

	tabsize = sizeof(INT16) * JPEG_BLOCK_SIZE;

	buffer = (UINT8 *)mem_malloc(tabsize * JPEG_MAX_BLOCKS, DCACHE_ALIGN_NUM);
	if(buffer == NULL)
		return VO_ERR_OUTOF_MEMORY;

	memset(buffer, 0, tabsize * JPEG_MAX_BLOCKS);
	for(i = 0; i < JPEG_MAX_BLOCKS; i++)
	{
		jpeg_obj->MBlock->WorkBlock[i] = (INT16 *)(buffer + tabsize*i);
	}

	memset(jpeg_obj->MBlock->DC_pred, 0, JPEG_MAX_COLOR * sizeof(INT16));

	tabsize = sizeof(JPEG_QUANT_TBL);
	
	buffer = (UINT8 *)mem_malloc(tabsize * NUM_BLOCK_TBLS, DCACHE_ALIGN_NUM);
	if(buffer == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(buffer, 0, tabsize * NUM_BLOCK_TBLS);

	for(i = 0; i < NUM_BLOCK_TBLS; i++)
	{
		jpeg_obj->Quant_tbl->Quant_tbl_ptrs[i] = (JPEG_QUANT_TBL *)(buffer + tabsize*i);
	}
	
	jpeg_set_quality(jpeg_obj->Quant_tbl->Quant_tbl_ptrs, DEFAULT_QUALITY);

	if(Init_QuantTab(jpeg_obj->Quant_tbl) < 0)
	{
		return VO_ERR_JPEGENC_UNKNOWN_ERR;
	}

	if(Init_HuffTab(jpeg_obj->entropy) < 0)
	{
		return VO_ERR_JPEGENC_UNKNOWN_ERR;
	}

	init_exif(exiflib);

	*phCodec = (VO_HANDLE)jpeg_obj;  

	buffer = NULL;
#ifdef LICENSEFILE 
	jpeg_obj->phCheck = gHandle;
#endif
	return ret;
}

VO_U32 VO_API voJPEGEncProcess2(VO_HANDLE hCodec, VO_VIDEO_BUFFER *pInData, VO_CODECBUFFER *pOutData,VO_VIDEO_OUTPUTINFO *pOutFormat)
{
	register UINT32	xi, yi;
	UINT32	xNum, yNum;
	JPEG_ENCOBJ*		jpeg_obj;
	JPEG_STREAM*		bitstream;
	PICTURE_ATT*		picture;
	JPEG_HUFF_ENTROPY*	entropy;
	JPEG_MCU_BLOCK*		MBlock;
	JPEG_QUANT_TABLE*	quantbl;
	_jpeg_getdata		jpeg_getdata;
	VOJPEGDECRETURNCODE ret;

	if(hCodec == NULL || pInData == NULL || pOutData == NULL)
		return VO_ERR_INVALID_ARG;

	jpeg_obj = (JPEG_ENCOBJ*)hCodec;
	bitstream = jpeg_obj->J_stream;
	entropy	 = jpeg_obj->entropy;
	MBlock	 = jpeg_obj->MBlock;
	picture	 = jpeg_obj->picture;
	quantbl= jpeg_obj->Quant_tbl;

	if(jpeg_obj->reversal_support 
		&& (pInData->ColorType == VO_COLOR_RGB565_PACKED)
		)
	{
		int i; //j;
		int stride = pInData->Stride[0];
		int heigth = jpeg_obj->picture->heigth;
		unsigned char*image = pInData->Buffer[0];
		unsigned char*row = (unsigned char*)malloc(sizeof(unsigned char)*stride);
		for(i=0; i<(heigth)/2; i++)
		{
			memcpy(row,     image+i*stride,   stride);
			memcpy(image+i*stride,    image+(heigth-i-1)*stride, stride);
			memcpy(image+(heigth-i-1)*stride,row,    stride);				
		}
		jpeg_obj->reversal_support= 0;
		free(row);
	}			

	initstream(bitstream, pOutData->Buffer, pOutData->Length);
#ifdef IPP_SUPPORT
	*bitstream->next_output_byte = bitstream->cache;			
#endif
	if(!jpeg_obj->headwrited)
	{
		bitstream->bits_index = 0;
		bitstream->cache = 0;
	
		jpeg_obj->picture->image_type = pInData->ColorType;

		ret = UpdataParameter2(jpeg_obj);
		//if(ret != VO_ERR_NONE )	//318	
			//return ret;

		if(jpeg_obj->thumb_support)
		{
			if(jpeg_encthumbnail(jpeg_obj, pInData) < 0)
				return VO_ERR_OUTOF_MEMORY;
		}
	
		write_jpeg_header(jpeg_obj);

		jpeg_obj->headwrited = 1;
	}	

	xNum = picture->MCU_XNum;
	yNum = picture->MCU_YNum;
	jpeg_getdata = jpeg_obj->jpeg_getdata;

	for(yi = jpeg_obj->YBlock; yi < yNum; yi++)
	{
		for(xi = jpeg_obj->XBlock; xi < xNum; xi++)
		{
			if(bitstream->free_in_buffer < JPEG_BREAK_LENTH)  //384
			{
				jpeg_obj->XBlock = xi;
				jpeg_obj->YBlock = yi;

				pOutData->Length = bitstream->buffer_size - bitstream->free_in_buffer;
#ifdef IPP_SUPPORT
				bitstream->cache = *bitstream->next_output_byte;			
#endif
				return VO_ERR_JPEGENC_ENCUNFINISHED;
			}

			if((*jpeg_getdata)(pInData, MBlock, picture, xi, yi) < 0)
			{
				pOutData->Length = bitstream->buffer_size - bitstream->free_in_buffer;
				return VO_ERR_INPUT_BUFFER_SMALL;
			}


#ifdef VOARMV7
			jpeg_dctquantfwd_ARMv7(MBlock, quantbl);
#else
	#ifdef VOARMV6
			jpeg_dctquantfwd_ARMv6(MBlock, quantbl);
    #else
			#ifdef VOARMV4 
				jpeg_dctquantfwd_ARMv4(MBlock, quantbl);
			#else
				jpeg_dctquantfwd1(MBlock, quantbl);
			#endif
	#endif
#endif	
			//jpeg_dctquantfwd_ARMv7(MBlock, quantbl);

			if(jpeg_encodehuffman(bitstream, MBlock, entropy) < 0)
			{
				pOutData->Length = bitstream->buffer_size - bitstream->free_in_buffer;

				return VO_ERR_INPUT_BUFFER_SMALL;
			}
		}
		jpeg_obj->XBlock = 0;
	}
	
	if(bitstream->bits_index)
	{
#ifndef IPP_SUPPORT
		*bitstream->next_output_byte = (UINT8)((bitstream->cache >> 16) & 0xFF);			
#endif		
		bitstream->next_output_byte++;
		bitstream->free_in_buffer--;
	}

	write_jpeg_trailer(bitstream);

	bitstream->bits_index = 0;
	bitstream->cache = 0;
	pOutData->Length = bitstream->buffer_size - bitstream->free_in_buffer;
	jpeg_obj->headwrited = 0;
	jpeg_obj->XBlock = 0;
	jpeg_obj->YBlock = 0;

	return VO_ERR_NONE;
}

VO_U32 VO_API voJPEGEncUninit2(VO_HANDLE hCodec)
{
	JPEG_ENCOBJ*		jpeg_obj;
	JPEG_HUFF_ENTROPY*	entropy;
	JPEG_MCU_BLOCK*		MBlock;
	JPEG_QUANT_TABLE*	quantbl;
	EXIFLibrary*		jExifLibary;
	VO_VIDEO_BUFFER*	pThumbInData;
	int i;

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	jpeg_obj = (JPEG_ENCOBJ*)hCodec;
	entropy	 = jpeg_obj->entropy;
	MBlock	 = jpeg_obj->MBlock;
	quantbl= jpeg_obj->Quant_tbl;
	jExifLibary = jpeg_obj->jExifLibary;
	pThumbInData = &jpeg_obj->ThumbInData;

#ifdef LICENSEFILE
	voCheckLibUninit(jpeg_obj->phCheck);//huwei 20090918 checkLib
#endif
		
	if(MBlock && MBlock->WorkBlock[0])
	{
		mem_free(MBlock->WorkBlock[0]);
		for (i = 0; i < JPEG_MAX_BLOCKS; i++)
		{
			MBlock->WorkBlock[i] = NULL;
		}
	}
	
	if(quantbl && quantbl->Quant_tbl_ptrs[0])
	{
		mem_free(quantbl->Quant_tbl_ptrs[0]);
		for (i = 0; i < NUM_BLOCK_TBLS; i++)
		{
			quantbl->Quant_tbl_ptrs[i]= NULL;
		}
	}

#ifdef IPP_SUPPORT
	if(entropy && entropy->dc_huff_tbls[0].pHuffTbl)
	{
		mem_free(entropy->dc_huff_tbls[0].pHuffTbl);
		entropy->dc_huff_tbls[0].pHuffTbl = NULL;
	}

	if(entropy && entropy->ac_huff_tbls[0].pHuffTbl)
	{
		mem_free(entropy->ac_huff_tbls[0].pHuffTbl);
		entropy->ac_huff_tbls[0].pHuffTbl = NULL;
	}

	if(entropy && entropy->dc_huff_tbls[1].pHuffTbl)
	{
		mem_free(entropy->dc_huff_tbls[1].pHuffTbl);
		entropy->dc_huff_tbls[1].pHuffTbl = NULL;
	}

	if(entropy && entropy->ac_huff_tbls[1].pHuffTbl)
	{
		mem_free(entropy->ac_huff_tbls[1].pHuffTbl);
		entropy->ac_huff_tbls[1].pHuffTbl = NULL;
	}
#endif

	for(i = 0; i < 3; i++)
	{
		if(pThumbInData->Buffer[i])
		{
			mem_free(pThumbInData->Buffer[i]);
			pThumbInData->Stride[i] = 0;
			pThumbInData->Buffer[i] = NULL;
		}
	}

	if(jpeg_obj->ThumbOutData.Buffer)
	{
		mem_free(jpeg_obj->ThumbOutData.Buffer);
		jpeg_obj->ThumbOutData.Buffer = NULL;
	}
	
	if(jpeg_obj->picture)
	{
		mem_free(jpeg_obj->picture);
		jpeg_obj->picture = NULL;
	}

	if(jpeg_obj->J_stream)
	{
		mem_free(jpeg_obj->J_stream);
		jpeg_obj->J_stream = NULL;
	}
	
	if(jpeg_obj->Quant_tbl)
	{
		mem_free(jpeg_obj->Quant_tbl);
		jpeg_obj->Quant_tbl = NULL;
	}

	if(jpeg_obj->entropy)
	{
		mem_free(jpeg_obj->entropy);
		jpeg_obj->entropy = NULL;
	}

	if(jpeg_obj->MBlock)
	{
		mem_free(jpeg_obj->MBlock);
		jpeg_obj->MBlock = NULL;
	}

	if(jpeg_obj->jExifLibary)
	{
		mem_free(jpeg_obj->jExifLibary);
		jpeg_obj->jExifLibary = NULL;
	}
	
	if(jpeg_obj->thumb_support)
	{
		voJPEGEncUninit2(jpeg_obj->hThumbHand);
	}

	mem_free(jpeg_obj);
	jpeg_obj = NULL;
	
	return VO_ERR_NONE;
}

VO_U32 VO_API voJPEGEncSetParameter2(VO_HANDLE hCodec, VO_S32 nID, VO_PTR pValue)
{
	JPEG_ENCOBJ*		jpeg_obj;
	UINT8*				ch;
	UINT32*				plValue = (UINT32*)pValue;
	UINT32              lValue = *plValue;
	UINT32				len;
	if(hCodec == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	jpeg_obj = (JPEG_ENCOBJ*)hCodec;
	

	switch(nID)
	{
		case VO_PID_JPEG_WIDTH:
			jpeg_obj->picture->width = lValue;
			break;
		case VO_PID_JPEG_HEIGHT:
			jpeg_obj->picture->heigth = lValue;
			break;
		case VO_PID_JPEG_QUALITY:
			if(lValue > 0 && lValue <= 100)
			{
				jpeg_obj->picquality = lValue;
				jpeg_set_quality(jpeg_obj->Quant_tbl->Quant_tbl_ptrs, lValue);
			
				if(Init_QuantTab(jpeg_obj->Quant_tbl) < 0)
				{
					return VO_ERR_JPEGENC_UNKNOWN_ERR;
				}
			}
			break;
		case VO_PID_JPEG_REVERSAL:
			jpeg_obj->reversal_support = lValue;
                        break;
		case VO_PID_EXIFINFO_SUPPT:
			jpeg_obj->exif_support = lValue;			
			break;
		case VO_PID_THUMPIC_SUPPT:
			jpeg_obj->thumb_support = lValue;
			break;
		case VO_PID_PICTURE_DATE:
			ch = (UINT8 *)pValue;
			len = strlen((INT8 *)ch);
			len = len < 20 ? len : 20;
				jpeg_obj->jExifLibary->enTIFFD |= 0x00000004;
			memcpy(jpeg_obj->jExifLibary->Tiffdir.DateTime, ch, len);
				jpeg_obj->jExifLibary->enEXIFDir |= 0x00000008;
			memcpy(jpeg_obj->jExifLibary->Exifdir.DataTimeOriginal, ch, len);
				jpeg_obj->jExifLibary->enEXIFDir |= 0x00000010;
			memcpy(jpeg_obj->jExifLibary->Exifdir.DataTimeDigitized , ch, len);
			break;
		case VO_PID_THUMPIC_WIDTH:
			if(lValue > 200 || lValue < 0)
				return VO_ERR_JPEGENC_THUMbWIDTH_INVALID;
			jpeg_obj->jExifLibary->thumWidth = lValue;
			break;
		case VO_PID_THUMPIC_HEIGHT:
			if(lValue > 200 || lValue < 0)
				return VO_ERR_JPEGENC_THUMbHEIGHT_INVALID;
			jpeg_obj->jExifLibary->thumHeight = lValue;
			break;
		case VO_PID_PICXRESOLUTION:
			if(plValue)
			{
				jpeg_obj->jExifLibary->enTIFFD |= 0x00000020;
				jpeg_obj->jExifLibary->Tiffdir.XResolution[0]  = *(plValue + 0);
				jpeg_obj->jExifLibary->Tiffdir.XResolution[1]  = *(plValue + 1);
				jpeg_obj->jExifLibary->enThumbEXIFDir |= 0x00000002;
				jpeg_obj->jExifLibary->thumbdir.XResolution[0] = *(plValue + 0);
				jpeg_obj->jExifLibary->thumbdir.XResolution[1] = *(plValue + 1);
			}
			break;
		case VO_PID_PICYRESOLUTION:
			if(plValue > 0)
			{
				jpeg_obj->jExifLibary->enTIFFD |= 0x00000040;
				jpeg_obj->jExifLibary->Tiffdir.YResolution[0]  = *(plValue + 0);
				jpeg_obj->jExifLibary->Tiffdir.YResolution[1]  = *(plValue + 1);
				jpeg_obj->jExifLibary->enThumbEXIFDir |= 0x00000004;
				jpeg_obj->jExifLibary->thumbdir.YResolution[0] = *(plValue + 0);
				jpeg_obj->jExifLibary->thumbdir.YResolution[1] = *(plValue + 1);
			}
			break;
		case VO_PID_MACHINE_MAKE:
			ch = (UINT8 *)pValue;
			len = strlen((INT8 *)ch);
			len = len < 20 ? len : 20;
				jpeg_obj->jExifLibary->enTIFFD |= 0x00000001;
			memcpy(jpeg_obj->jExifLibary->Tiffdir.CameraMake, ch, len);			
			break;
		case VO_PID_MACHINE_MODEL:
			ch = (UINT8 *)pValue;
			len = strlen((INT8 *)ch);
			len = len < 40 ? len : 40;
			jpeg_obj->jExifLibary->enTIFFD |= 0x00000002;
			memcpy(jpeg_obj->jExifLibary->Tiffdir.CameraModel, ch, len);
			break;
		case VO_PID_GPS_LATIREF:
			if(lValue == VO_NORTH_LATITITUDE || lValue == VO_SOUTH_LATITITUDE)
			{
				jpeg_obj->jExifLibary->Gpsdir.GPSLatitudeRef = lValue;
				jpeg_obj->jExifLibary->enGPSDir |= (0x00000001 | 0x00000002);
				jpeg_obj->jExifLibary->enTIFFD |= 0x00000400;
			}
			break;
		case VO_PID_GPS_LATITUDE:
			jpeg_obj->jExifLibary->enTIFFD |= 0x00000400;
			jpeg_obj->jExifLibary->enGPSDir |= (0x00000001 | 0x00000002 | 0x00000004);
			jpeg_obj->jExifLibary->Gpsdir.GPSLatitude[0] = *(plValue + 0);
			jpeg_obj->jExifLibary->Gpsdir.GPSLatitude[1] = *(plValue + 1);
			jpeg_obj->jExifLibary->Gpsdir.GPSLatitude[2] = *(plValue + 2);
			jpeg_obj->jExifLibary->Gpsdir.GPSLatitude[3] = *(plValue + 3);
			jpeg_obj->jExifLibary->Gpsdir.GPSLatitude[4] = *(plValue + 4);
			jpeg_obj->jExifLibary->Gpsdir.GPSLatitude[5] = *(plValue + 5);
			break;
		case VO_PID_GPS_LONGIREF:
			if(lValue == VO_EAST_LONGITUDE || lValue == VO_WEST_LONGITUDE)
			{
				jpeg_obj->jExifLibary->Gpsdir.GPSLongtiudeRef = lValue;
				jpeg_obj->jExifLibary->enGPSDir |= (0x00000001 | 0x00000008);
				jpeg_obj->jExifLibary->enTIFFD |= 0x00000400;
			}
			break;
		case VO_PID_GPS_LONGITUDE:
			jpeg_obj->jExifLibary->enTIFFD |= 0x00000400;
			jpeg_obj->jExifLibary->enGPSDir |= (0x00000001 | 0x00000008 | 0x00000010);
			jpeg_obj->jExifLibary->Gpsdir.GPSLongtiude[0] = *(plValue + 0);
			jpeg_obj->jExifLibary->Gpsdir.GPSLongtiude[1] = *(plValue + 1);
			jpeg_obj->jExifLibary->Gpsdir.GPSLongtiude[2] = *(plValue + 2);
			jpeg_obj->jExifLibary->Gpsdir.GPSLongtiude[3] = *(plValue + 3);
			jpeg_obj->jExifLibary->Gpsdir.GPSLongtiude[4] = *(plValue + 4);
			jpeg_obj->jExifLibary->Gpsdir.GPSLongtiude[5] = *(plValue + 5);
			break;
		case VO_PID_GPS_ALITUDEREF:
			jpeg_obj->jExifLibary->Gpsdir.GPSAltitudeRef = lValue;
			jpeg_obj->jExifLibary->enGPSDir |= (0x00000001 | 0x00000020);
			jpeg_obj->jExifLibary->enTIFFD |= 0x00000400;
			break;
		case VO_PID_GPS_ALITUDE:
			jpeg_obj->jExifLibary->enTIFFD |= 0x00000400;
			jpeg_obj->jExifLibary->enGPSDir |= (0x00000001 | 0x00000020 | 0x00000040);
			jpeg_obj->jExifLibary->Gpsdir.GPSAltitude[0] = *(plValue + 0);
			jpeg_obj->jExifLibary->Gpsdir.GPSAltitude[1] = *(plValue + 1);
			break;
		case VO_PID_GPS_MAPDATUM:
			ch = (UINT8 *)pValue;
			len = strlen((INT8 *)ch);
			len = len < 40 ? len : 40;
			jpeg_obj->jExifLibary->enTIFFD |= 0x00000400;
			jpeg_obj->jExifLibary->enGPSDir |= (0x00000001 | 0x00040000);
			memcpy(jpeg_obj->jExifLibary->Gpsdir.GPSMapDatum, ch, len);			
			break;
		case VO_PID_GPS_DATESTAMP:
			ch = (UINT8 *)pValue;
			len = strlen((INT8 *)ch);
			len = len < 10 ? len : 10;
			jpeg_obj->jExifLibary->enTIFFD |= 0x00000400;
			jpeg_obj->jExifLibary->enGPSDir |= (0x00000001 | 0x00080000);
			memcpy(jpeg_obj->jExifLibary->Gpsdir.GPSDatastemp, ch, len);	
			break;
		case VO_PID_JPEG_WINAUTHOR:
			ch = (UINT8 *)pValue;
			len = strlen((INT8 *)ch);
			len = len < 40 ? len : 40;
			jpeg_obj->jExifLibary->enTIFFD |= 0x00000800;
			memcpy(jpeg_obj->jExifLibary->Tiffdir.Author, ch, len);
			break;			
			
		default:
			return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voJPEGEncGetParameter2(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	JPEG_ENCOBJ*		jpeg_obj;
	INT32 *pValue=(INT32*)plValue;
	VO_CODECBUFFER*  databuffer= (VO_CODECBUFFER*)plValue;

	if(hCodec == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	jpeg_obj = (JPEG_ENCOBJ*)hCodec;

	switch(nID)
	{
		case VO_PID_JPEG_WIDTH:
			*pValue = jpeg_obj->picture->width;
			break;
		case VO_PID_JPEG_HEIGHT:
			*pValue = jpeg_obj->picture->heigth;
			break;
		case VO_PID_JPEG_QUALITY:
			*pValue = jpeg_obj->picquality;
			break;
		case VO_PID_PRE_JPEGSIZE:
			*pValue = jpeg_obj->picture->width * jpeg_obj->picture->heigth * jpeg_obj->picquality / 100;
			break;
		case VO_PID_THUMPIC_SUPPT:
			*pValue = jpeg_obj->exif_support;
			break;
		case VO_PID_GET_THUMDATA:
			databuffer = (VO_CODECBUFFER*)plValue;
			databuffer->Buffer = jpeg_obj->ThumbOutData.Buffer;
			databuffer->Length = jpeg_obj->ThumbOutData.Length;
			break;
		case VO_PID_THUMPIC_WIDTH:			
			*pValue = jpeg_obj->jExifLibary->thumWidth;
			break;
		case VO_PID_THUMPIC_HEIGHT:
			*pValue = jpeg_obj->jExifLibary->thumHeight;
			break;
		case VO_PID_PICXRESOLUTION:
			*pValue = jpeg_obj->jExifLibary->Tiffdir.XResolution[0];
			break;
		case VO_PID_PICYRESOLUTION:
			*pValue = jpeg_obj->jExifLibary->Tiffdir.YResolution[0]; 
			break;
		default:
			return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

#ifndef VOMJPEGENC
VO_S32 VO_API voGetJPEGEncAPI (VO_IMAGE_ENCAPI * pEncHandle)
{
	VO_IMAGE_ENCAPI *pJPEGEnc = pEncHandle;

	if(!pJPEGEnc)
		return VO_ERR_INVALID_ARG;

	pJPEGEnc->Init   = voJPEGEncInit2;
	pJPEGEnc->Uninit = voJPEGEncUninit2;
	pJPEGEnc->SetParam = voJPEGEncSetParameter2;
	pJPEGEnc->GetParam = voJPEGEncGetParameter2;
	pJPEGEnc->Process = voJPEGEncProcess2;

	return VO_ERR_NONE;
}
#endif