#include "jconfig.h"
#include "jcommon.h"
#include "mem_align.h"
#include "jencoder.h"
//#include "mjpegenc.h"
#include "voMJPEG.h"
//#include "voChkLcsLib.h"

//int VOI_limit= 0x7fffffff;
//int FrameNum = 0;

#ifdef VOARMV7
void jpeg_dctquantfwd_ARMv7(JPEG_MCU_BLOCK*, JPEG_QUANT_TABLE*);
#endif

#ifdef VOARMV6 
void	jpeg_dctquantfwd_ARMv6(JPEG_MCU_BLOCK*		outblock, JPEG_QUANT_TABLE*		quant_tbl);
#endif

void	jpeg_dctquantfwd1(JPEG_MCU_BLOCK*		outblock,
                          JPEG_QUANT_TABLE*		quant_tbl);


VO_HANDLE g_hMJPEGEncInst = NULL;

static VOMJPEGENCRETURNCODE UpdataParameter(JPEG_ENCOBJ* jpeg_obj)
{
	UINT32 image_type;
	PICTURE_ATT*		picture;
	JPEG_MCU_BLOCK*		MBlock;

	picture = jpeg_obj->picture;
	MBlock  = jpeg_obj->MBlock;

	image_type = picture->image_type;

	if(picture->width <= 0)
		return VO_ERR_ENC_MJPEG_WIDTH_INVALID;

	if(picture->heigth <= 0)
		return VO_ERR_ENC_MJPEG_HEIGHT_INVALID;

	switch(image_type) {
	case VO_COLOR_YUV_PLANAR444:
	case VO_COLOR_YUV444_PACKED:
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
	case VO_COLOR_YUV_PLANAR422_12:
	case VO_COLOR_YUYV422_PACKED:
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
		picture->MCU_width	= 16;	
		picture->MCU_heigth	= 16;
		picture->MCU_XNum = (picture->width +  15) >> 4;
		picture->MCU_YNum = (picture->heigth + 15) >> 4;
		picture->v_sample[0] = picture->h_sample[0] = 2;
		picture->v_sample[1] = picture->h_sample[1] = 1;
		picture->v_sample[2] = picture->h_sample[2] = 1;
		MBlock->lumblockNum = 4;
		MBlock->blockNum = 6;
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
		return VO_ERR_ENC_MJPEG_UNSUPPORT_COLOR;
	}

	switch(image_type) {
	case VO_COLOR_YUV_PLANAR444:
		jpeg_obj->jpeg_getdata = get444pblockdata;		
		break;
	case VO_COLOR_YUV444_PACKED:
		jpeg_obj->jpeg_getdata = get444iblockdata;		
		break;
	case VO_COLOR_YUV_PLANAR422_12:
		jpeg_obj->jpeg_getdata = get422pblockdata;
		break;
	case VO_COLOR_YUYV422_PACKED:
		jpeg_obj->jpeg_getdata = get422iYUYVblockdata;
		break;
	case VO_COLOR_YVYU422_PACKED:
		jpeg_obj->jpeg_getdata = get422iYVYUblockdata;
		break;
	case VO_COLOR_UYVY422_PACKED:
		jpeg_obj->jpeg_getdata = get422iUYVYblockdata;
		break;
	case VO_COLOR_VYUY422_PACKED:
		jpeg_obj->jpeg_getdata = get422iVYUYblockdata;
		break;
	case VO_COLOR_YUV_PLANAR420:
	case VO_COLOR_YVU_PLANAR420:
		jpeg_obj->jpeg_getdata = get420pblockdata;
		break;
	case VO_COLOR_RGB888_PACKED:
		jpeg_obj->jpeg_getdata = getRGB888iblockdata;
		break;
	case VO_COLOR_RGB565_PACKED:
		jpeg_obj->jpeg_getdata = getRGB565iblockdata;
		break;
	case VO_COLOR_RGB888_PLANAR:
		jpeg_obj->jpeg_getdata = getRGB888pblockdata;
		break;
	default:
		return VO_ERR_ENC_MJPEG_UNSUPPORT_COLOR;
	}

	memset(jpeg_obj->MBlock->DC_pred, 0, JPEG_MAX_COLOR * sizeof(INT16));

	return VO_ERR_NONE;
}

VO_U32 VO_API voMJPEGEncInit(VO_HANDLE *phCodec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
//VOMJPEGENCRETURNCODE VOCODECAPI voMJPEGEncInit(HVOCODEC *phCodec)
{
	INT32  ret =0;	
	UINT32 i;
	UINT32 tabsize = 0;
	UINT8* buffer = NULL;
	JPEG_ENCOBJ*  jpeg_obj;
//	UINT32 imagetype = VO_COLOR_YUV_PLANAR420;
#ifdef LICENSEFILE 
  VO_PTR  gHandle = NULL;
  if((pUserData->memflag & 0xF0) == 0x10)
    ret=voCheckLibInit(&gHandle, VO_INDEX_ENC_MJPEG, 2, g_hMJPEGEncInst,pUserData->libOperator);//huwei 20090922 checkLib
  else
    ret=voCheckLibInit(&gHandle, VO_INDEX_ENC_MJPEG, 2, g_hMJPEGEncInst,NULL);//huwei 20090922 checkLib

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

	jpeg_obj = (JPEG_ENCOBJ*)mem_malloc(sizeof(JPEG_ENCOBJ), ALIGN_NUM_8);
	if(jpeg_obj == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(jpeg_obj, 0, sizeof(JPEG_ENCOBJ));

	
	jpeg_obj->picture = (PICTURE_ATT *)mem_malloc(sizeof(PICTURE_ATT), ALIGN_NUM_8);
	if(jpeg_obj->picture == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(jpeg_obj->picture, 0, sizeof(PICTURE_ATT));


	jpeg_obj->J_stream = (JPEG_STREAM *)mem_malloc(sizeof(JPEG_STREAM), ALIGN_NUM_8);
	if(jpeg_obj->J_stream == NULL)
		return VO_ERR_OUTOF_MEMORY;
	memset(jpeg_obj->J_stream, 0, sizeof(JPEG_STREAM));


	jpeg_obj->Quant_tbl = (JPEG_QUANT_TABLE *)mem_malloc(sizeof(JPEG_QUANT_TABLE), ALIGN_NUM_8);
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

	jpeg_obj->exif_support = 0;
	jpeg_obj->headwrited = 0;
	jpeg_obj->XBlock = 0;
	jpeg_obj->YBlock = 0;
	jpeg_obj->picquality = DEFAULT_QUALITY;
	
	jpeg_obj->picture->width = 0;
	jpeg_obj->picture->heigth = 0;
	jpeg_obj->picture->image_type = -1;
	

	tabsize = sizeof(INT16) * JPEG_BLOCK_SIZE;

	buffer = (UINT8 *)mem_malloc(tabsize * JPEG_MAX_BLOCKS, DCACHE_ALIGN_NUM);
	if(buffer == NULL)
		return VO_ERR_OUTOF_MEMORY;

	memset(buffer, 0, tabsize * JPEG_MAX_BLOCKS);

	for(i = 0; i < JPEG_MAX_BLOCKS; i++)
	{
		jpeg_obj->MBlock->WorkBlock[i] = (INT16 *)(buffer + tabsize*i);
	}

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

#ifdef LICENSEFILE 
	jpeg_obj->phCheck = gHandle;
	
#endif	

	*phCodec = (VO_HANDLE)jpeg_obj;

	buffer = NULL;
	return ret;	
}
//VO_U32 (VO_API * Process)      (VO_HANDLE hDec, VO_VIDEO_BUFFER * pInput, VO_CODECBUFFER * pOutput, VO_VIDEO_FRAMETYPE * pType);
VO_U32 VO_API voMJPEGEncProcess(VO_HANDLE hCodec, VO_VIDEO_BUFFER *pInData, VO_CODECBUFFER *pOutData,VO_VIDEO_FRAMETYPE * pType)
//VOMJPEGENCRETURNCODE VOCODECAPI voMJPEGEncProcess(HVOCODEC hCodec, VOCODECVIDEOBUFFER *pInData, VOCODECDATABUFFER *pOutData)
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
	VOMJPEGENCRETURNCODE ret;

	if(hCodec == NULL || pInData == NULL || pOutData == NULL)
		return VO_ERR_INVALID_ARG;

	jpeg_obj = (JPEG_ENCOBJ*)hCodec;
	bitstream = jpeg_obj->J_stream;
	entropy	 = jpeg_obj->entropy;
	MBlock	 = jpeg_obj->MBlock;
	picture	 = jpeg_obj->picture;
	quantbl= jpeg_obj->Quant_tbl;

	//if(FrameNum > VOI_limit)
		//return VORC_MJPEGENC_UNKNOWN_ERR;	

	initstream(bitstream, pOutData->Buffer, pOutData->Length);

#ifdef IPP_SUPPORT
	*bitstream->next_output_byte = bitstream->cache;			
#endif
	if(!jpeg_obj->headwrited)
	{
		jpeg_obj->picture->image_type = pInData->ColorType;
		

		ret = UpdataParameter(jpeg_obj);
		if(ret != VO_ERR_NONE)		
			return ret;

		write_jpeg_header(jpeg_obj);

		jpeg_obj->headwrited = 1;
	}	

	xNum = picture->MCU_XNum;
	yNum = picture->MCU_YNum;
	jpeg_getdata = jpeg_obj->jpeg_getdata;
	
	/*{
		FILE *fp=fopen ("\\Program Files\\vomjpegEncTst\\L.txt", "a");
		fprintf(fp,"%d\n",bitstream->free_in_buffer);
		fclose(fp);
	}*/
	for(yi = jpeg_obj->YBlock; yi < yNum; yi++)
	{
		for(xi = jpeg_obj->XBlock; xi < xNum; xi++)
		{
			if(bitstream->free_in_buffer < JPEG_BREAK_LENTH)
			{
				jpeg_obj->XBlock = xi;
				jpeg_obj->YBlock = yi;

				pOutData->Length = bitstream->buffer_size - bitstream->free_in_buffer;
#ifdef IPP_SUPPORT
				bitstream->cache = *bitstream->next_output_byte;			
#endif
				return VO_ERR_ENC_MJPEG_ENCUNFINISHED;
			}

			if((*jpeg_getdata)(pInData, MBlock, picture, xi, yi) < 0)
			{
				pOutData->Length = bitstream->buffer_size - bitstream->free_in_buffer;
				return VO_ERR_INPUT_BUFFER_SMALL;
			}

//#ifdef WIN32 
//			jpeg_dctquantfwd1(MBlock, quantbl);
//#else
//			jpeg_dctquantfwd(MBlock, quantbl);
//#endif

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

				return VO_ERR_OUTPUT_BUFFER_SMALL;
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

	bitstream->bits_index = 0;
	bitstream->cache = 0;

	write_jpeg_trailer(bitstream);

	if(bitstream->free_in_buffer < 0)
	{
		pOutData->Length = 0;
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}

	pOutData->Length = bitstream->buffer_size - bitstream->free_in_buffer;

	/*{
		FILE *fp=fopen ("\\Program Files\\vomjpegEncTst\\Length.txt", "a");
		fprintf(fp,"%d %d %d\n",bitstream->buffer_size,bitstream->free_in_buffer,pOutData->Length);
		fclose(fp);
	}*/

	pOutData->Length = (pOutData->Length + 31) & ~31;

	jpeg_obj->headwrited = 0;
	jpeg_obj->XBlock	 = 0;
	jpeg_obj->YBlock	 = 0;
	//FrameNum++;

	return VO_ERR_NONE;
}

VO_U32 VO_API voMJPEGEncUninit(VO_HANDLE hCodec)
//VOMJPEGENCRETURNCODE voMJPEGEncUninit(HVOCODEC hCodec)
{
	JPEG_ENCOBJ*		jpeg_obj;
	JPEG_HUFF_ENTROPY*	entropy;
	JPEG_MCU_BLOCK*		MBlock;
	JPEG_QUANT_TABLE*	quantbl;
	int i;

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	jpeg_obj = (JPEG_ENCOBJ*)hCodec;
	entropy	 = jpeg_obj->entropy;
	MBlock	 = jpeg_obj->MBlock;
	quantbl= jpeg_obj->Quant_tbl;

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

	
	mem_free(jpeg_obj);
	jpeg_obj = NULL;
	
	return VO_ERR_NONE;
}
VO_U32 VO_API voMJPEGEncSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR pValue)
//VOMJPEGENCRETURNCODE voMJPEGEncSetParameter(HVOCODEC hCodec, LONG nID, LONG lValue)
{
	JPEG_ENCOBJ*		jpeg_obj;
	UINT32*				plValue = (UINT32*)pValue;
	UINT32              lValue = *plValue;
	if(hCodec == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	jpeg_obj = (JPEG_ENCOBJ*)hCodec;

	switch(nID)
	{
		case VO_PID_ENC_MJPEG_WIDTH:
			jpeg_obj->picture->width = lValue;
			break;
		case VO_PID_ENC_MJPEG_HEIGHT:
			jpeg_obj->picture->heigth = lValue;
			break;
		case VO_PID_ENC_MJPEG_QUANLITY:
			if(lValue > 0 && lValue < 100)
			{
				jpeg_obj->picquality = lValue;
				jpeg_set_quality(jpeg_obj->Quant_tbl->Quant_tbl_ptrs, lValue);
			
				if(Init_QuantTab(jpeg_obj->Quant_tbl) < 0)
				{
					return VO_ERR_ENC_MJPEG_UNKNOWN;
				}
			}
			break;
		default:
			return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voMJPEGEncGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
//VOMJPEGENCRETURNCODE voMJPEGEncGetParameter(HVOCODEC hCodec, LONG nID, LONG *plValue)
{
	JPEG_ENCOBJ*		jpeg_obj;
	INT32 *pValue=(INT32*)plValue;
	if(hCodec == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	jpeg_obj = (JPEG_ENCOBJ*)hCodec;

	switch(nID)
	{
		case VO_PID_ENC_MJPEG_WIDTH:
			*pValue = jpeg_obj->picture->width;
			break;
		case VO_PID_ENC_MJPEG_HEIGHT:
			*pValue = jpeg_obj->picture->heigth;
			break;
		case VO_PID_ENC_MJPEG_QUANLITY:
			*pValue = jpeg_obj->picquality;
			break;
		default:
			return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_S32 VO_API voGetMJPEGEncAPI (VO_VIDEO_ENCAPI * pEncHandle)
{
	VO_VIDEO_ENCAPI *pMJPEGDec = pEncHandle;

	if(!pMJPEGDec)
		return VO_ERR_INVALID_ARG;

	pMJPEGDec->Init   = voMJPEGEncInit;
	pMJPEGDec->Uninit = voMJPEGEncUninit;
	pMJPEGDec->SetParam = voMJPEGEncSetParameter;
	pMJPEGDec->GetParam = voMJPEGEncGetParameter;
	pMJPEGDec->Process = voMJPEGEncProcess;

	return VO_ERR_NONE;
}
