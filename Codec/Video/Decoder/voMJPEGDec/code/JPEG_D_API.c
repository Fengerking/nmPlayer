/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#include	"mem_align.h"
#include	"jdecoder.h"
#include	"jexif.h"
#include	"voJPEG.h"

#ifdef LICENSEFILE 
#include "voCheck.h"
#endif

VO_HANDLE g_hJPEGDecInst = NULL;/*huwei 20090922 checkLib*/

#ifdef MEM_LEAKAGE
int malloc_count = 0;
int free_count = 0;
#endif // MEM_LEAKAGE

VO_U32 VO_API voJPEGDecProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat);
VO_U32 VO_API voJPEGDecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue);

typedef struct{	
	VO_VOID* pDecCore;
	VO_VIDEO_BUFFER OutData;
	VO_CODECBUFFER  InputData;
	VO_VIDEO_OUTPUTINFO OutPutInfo;
}JPEGDEC_FRONT;

VO_U32 VO_API Initpicture(Jpeg_DecOBJ * decoder)
{
	UINT16	i, k;
	UINT32	MblockSize = 0;
	UINT8*	block_buf = NULL;
	UINT32	max_v_sample = 1;
	UINT32	max_h_sample = 1;
	JPEG_COMP_ITEM* comptr;
	JPEG_PARA *jpara;

	jpara = &decoder->jpara;

	if(jpara->num_component > JPEG_MAX_COLOR)
		return VO_ERR_JPEGDEC_UNSUPPORT_FEATURE;

	for(i = 0, comptr = jpara->compon; i < jpara->num_component; 
			i++, comptr++)
	{
		max_h_sample = MAX(comptr->h_sample, max_h_sample);
		max_v_sample = MAX(comptr->v_sample, max_v_sample);	
	}

	jpara->MCUBlock.blockNum = 0;
	jpara->max_h_sample = max_h_sample;
	jpara->max_v_sample = max_v_sample;	
	jpara->MCU_width  = max_h_sample * 8;
	jpara->MCU_heigth = max_v_sample * 8;
		
	decoder->stepDec_num = DIV(decoder->zoomout*decoder->instepDec_num, jpara->MCU_heigth);	
	jpara->MCU_XNum = DIV(jpara->width,  jpara->MCU_width );
	jpara->MCU_YNum = DIV(jpara->heigth, jpara->MCU_heigth);
	
	decoder->imagefield.start_X  = 0;
	decoder->imagefield.start_Y  = 0;
	decoder->imagefield.end_X  = jpara->MCU_XNum;
	decoder->imagefield.end_Y  = jpara->MCU_YNum;
	decoder->field_hmax	= decoder->imagefield.end_X;
	decoder->field_vmax	= decoder->imagefield.end_Y;

	k = 0;
	for(i = 0, comptr = jpara->compon; i < jpara->num_component;
		i++, comptr++)
	{
		comptr->h_blocknum = DIV(jpara->width * comptr->h_sample,  max_h_sample * 8); 
		comptr->v_blocknum = DIV(jpara->heigth * comptr->v_sample,  max_v_sample * 8);
		
		MblockSize = comptr->h_sample * comptr->v_sample;
		jpara->MCUBlock.blockNum +=  MblockSize;
		
		for( ; k < jpara->MCUBlock.blockNum; k++)
		{
			jpara->MCUBlock.dcImage[k] = (UINT16)(comptr->index);
		}
	}

	if(jpara->MCUBlock.dcImage[0] < 0)
	{
		for(k = 0; k < jpara->MCUBlock.blockNum; k++)
		{
			jpara->MCUBlock.dcImage[k] += 1;
		}
	}
	
	if(jpara->MCUBlock.blockNum > JPEG_MAX_BLOCKS)
		return VO_ERR_JPEGDEC_UNSUPPORT_FEATURE;
	
	if(decoder->pre_BlockNum < jpara->MCUBlock.blockNum)
	{
		if(decoder->memoryOperator.Free)
		{
			decoder->memoryOperator.Free(VO_INDEX_DEC_JPEG, jpara->MCUBlock.WorkBlock[0]);
		} 
		else
		{
			mem_free(jpara->MCUBlock.WorkBlock[0]);
		}
		
		for(i = 0; i < JPEG_MAX_BLOCKS; i++)
		{
			jpara->MCUBlock.WorkBlock[i] = NULL;
		}
		
		decoder->pre_BlockNum = jpara->MCUBlock.blockNum;		
		MblockSize = sizeof(INT16) * JPEG_BLOCK_SIZE;

		if(decoder->memoryOperator.Alloc){
			VO_MEM_INFO MemInfo;

			MemInfo.Flag = 0;
			MemInfo.Size = decoder->pre_BlockNum * MblockSize;
			decoder->memoryOperator.Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
			block_buf = (UINT8 *)MemInfo.VBuffer;
		}else{
			block_buf = (UINT8 *)mem_malloc(decoder->pre_BlockNum * MblockSize, DCACHE_ALIGN_NUM);
		}
		
		if(block_buf == NULL)
			return VO_ERR_OUTOF_MEMORY;
		for(i = 0; i < decoder->pre_BlockNum; i++)
		{
			jpara->MCUBlock.WorkBlock[i] = (INT16 *)(block_buf + MblockSize*i);
		}			
	}		
	
	return VO_ERR_NONE;
}

VO_U32 VO_API	UpdateParameter(Jpeg_DecOBJ * decoder, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat)
{
	UINT16	i;
	UINT32	yuv_width[3], totalwidth;  
	UINT32	yuv_height[3], totalheight;
	UINT32	yuv_size[3], totalsize;
	JPEG_COMP_ITEM	*comptr;
	JPEG_PARA		*jpara;
	IMAGE_FIELD		*field;
	
	jpara = &decoder->jpara;
	field = &decoder->imagefield;
	
	if(jpara->progressMode)
	{
		if(!decoder->field_Num)
		{			
			totalsize = 0;
			totalwidth = 0;
			for(i = 0, comptr = jpara->compon; i < jpara->num_component; 
			i++, comptr++)
			{
				yuv_width[i]  = (field->end_X - field->start_X) * comptr->h_sample * 8 * 2;
				yuv_height[i] = (field->end_Y - field->start_Y) * comptr->v_sample * 8;
				yuv_size[i]   = yuv_width[i] * yuv_height[i];	
				totalsize += yuv_size[i];
				totalwidth += yuv_width[i];
			}
			
			if(jpara->num_component == 1)
			{
				yuv_width[1] = yuv_width[2] = yuv_width[0]/2;
				yuv_size[1]  = yuv_size[2]  = yuv_size[0]/4;
				totalsize += yuv_size[1] + yuv_size[2];
				totalwidth += yuv_width[1] + yuv_width[2];
			}
			
			if(decoder->Buffer_size <= totalsize)
			{
				if(!totalwidth) 
					return VO_ERR_JPEGDEC_UNSUPPORT_FEATURE;
				totalheight = decoder->Buffer_size / totalwidth;
				totalheight = totalheight & ~15;
				if(!totalheight) 
					return VO_ERR_OUTOF_MEMORY;
				
				decoder->field_Num = DIV(yuv_height[0], totalheight);
				for(i = 0, comptr = jpara->compon; i < jpara->num_component; 
				i++, comptr++)
				{
					yuv_height[i] = DIV(totalheight * comptr->v_sample, jpara->max_v_sample);
					yuv_size[i]   = yuv_width[i] * yuv_height[i];	
				}
				
				decoder->field_step = totalheight;			
				field->end_Y = field->start_Y + DIV(totalheight,  jpara->max_v_sample * 8);
				decoder->Isfield = 1;
			}
			else
			{
				decoder->field_Num = 1;
				decoder->field_step = 0;
			}
		}
		else
		{
			for(i = 0, comptr = jpara->compon; i < jpara->num_component; 
			i++, comptr++)
			{
				yuv_width[i]  = (field->end_X - field->start_X) * comptr->h_sample * 8 * 2;
				yuv_height[i] = DIV(decoder->field_step * comptr->v_sample, jpara->max_v_sample);
				yuv_size[i]   = yuv_width[i] * yuv_height[i];	
			}
		}
	}
	else
	{
		for(i = 0, comptr = jpara->compon; i < jpara->num_component; 
			i++, comptr++)
		{
			yuv_width[i]  = (field->end_X - field->start_X) * comptr->h_sample * 8;
			yuv_height[i] = (field->end_Y - field->start_Y) * comptr->v_sample * 8;
			yuv_size[i]   = yuv_width[i] * yuv_height[i];	
		}
			
		if(jpara->num_component == 1)
		{
			yuv_width[1] = yuv_width[2] = yuv_width[0]/2;
			yuv_size[1]  = yuv_size[2]  = yuv_size[0]/4;
		}	
	}
	
	for(i = 0; i < JPEG_MAX_COLOR; i++)
	{
		if(decoder->imagedsize[i] < yuv_size[i])
		{
			if(decoder->memoryOperator.Free)
			{
				decoder->memoryOperator.Free(VO_INDEX_DEC_JPEG, decoder->outBuffer[i]);
			} 
			else
			{
				mem_free(decoder->outBuffer[i]);
			}
			
			decoder->outBuffer[i] = NULL;
			decoder->imagedsize[i] = yuv_size[i];		
		
			if(decoder->memoryOperator.Alloc)
			{
				VO_MEM_INFO MemInfo;

				MemInfo.Flag = 0;
				MemInfo.Size = yuv_size[i];
				decoder->memoryOperator.Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
				decoder->outBuffer[i] = (UINT8 *)MemInfo.VBuffer;
			} 
			else
			{
				decoder->outBuffer[i] = (UINT8 *)mem_malloc(yuv_size[i],  DCACHE_ALIGN_NUM);
			}
			
			if(decoder->outBuffer[i] == NULL)
				return VO_ERR_OUTOF_MEMORY; 		
		}
		
		decoder->widthstrid[i] = yuv_width[i];

		if(decoder->memoryOperator.Set)
		{
			decoder->memoryOperator.Set(VO_INDEX_DEC_JPEG, decoder->outBuffer[i], 0, yuv_size[i]);
		} 
		else
		{
			memset(decoder->outBuffer[i], 0, yuv_size[i]);
		}
		
	}
	
	if(jpara->num_component == 1)
	{
		if (decoder->memoryOperator.Set)
		{
			decoder->memoryOperator.Set(VO_INDEX_DEC_JPEG, decoder->outBuffer[1], 128, yuv_size[1]);
			decoder->memoryOperator.Set(VO_INDEX_DEC_JPEG, decoder->outBuffer[2], 128, yuv_size[2]);
		} 
		else
		{
			memset(decoder->outBuffer[1], 128, yuv_size[1]);
			memset(decoder->outBuffer[2], 128, yuv_size[2]);
		}

	}

	if(pOutData)
	{
		switch(jpara->image_type)
		{
		case 0x00000011:
			pOutData->ColorType = VO_COLOR_YUV_PLANAR420; /* VOGRAY_PLANARGRAY; */
			break;			
		case 0x00111111:
			pOutData->ColorType = VO_COLOR_YUV_PLANAR444;
			break;
		case 0x00111121:
			pOutData->ColorType = VO_COLOR_YUV_PLANAR422_12;
			break;
		case 0x00111112:
			pOutData->ColorType = VO_COLOR_YUV_PLANAR422_21;
			break;
		case 0x00111122:
			pOutData->ColorType = VO_COLOR_YUV_PLANAR420;
			break;
		case 0x00111141:
			pOutData->ColorType = VO_COLOR_YUV_PLANAR411;			
			break;
		case 0x00111114:
			pOutData->ColorType = VO_COLOR_YUV_PLANAR411V;			
			break;
		case 0x11111111:
			pOutData->ColorType = VO_COLOR_RGB888_PLANAR;			
			break;			
		default:
			pOutData->ColorType = jpara->image_type;	
		}
		
		for(i = 0; i < JPEG_MAX_COLOR; i++)
		{
			pOutData->Buffer[i] = decoder->outBuffer[i];
			pOutData->Stride[i] = decoder->widthstrid[i];
		}
	}

	if(pOutFormat)
	{
		pOutFormat->Format.Width  = (jpara->width + 1)  & ~1;
		pOutFormat->Format.Height = (jpara->heigth + 1) & ~1;
		pOutFormat->Format.Type = VO_VIDEO_FRAME_I;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voJPEGSUBDecInit(Jpeg_DecOBJ *dec,VO_HANDLE *phCodec)
{
	INT32 ret = VO_ERR_NONE;
	Jpeg_DecOBJ *decoder;

	if(dec->memoryOperator.Alloc)
	{
		VO_MEM_INFO MemInfo;

		MemInfo.Flag = 0;
		MemInfo.Size = sizeof(Jpeg_DecOBJ);
		dec->memoryOperator.Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
		decoder = (Jpeg_DecOBJ *)MemInfo.VBuffer;
		dec->memoryOperator.Set(VO_INDEX_DEC_JPEG, decoder, 0, sizeof(Jpeg_DecOBJ));

		decoder->memoryOperator.Alloc     = dec->memoryOperator.Alloc;
		decoder->memoryOperator.Check     = dec->memoryOperator.Check;
		decoder->memoryOperator.Compare   = dec->memoryOperator.Compare;
		decoder->memoryOperator.Copy      = dec->memoryOperator.Copy;
		decoder->memoryOperator.Free      = dec->memoryOperator.Free;
		decoder->memoryOperator.Move      = dec->memoryOperator.Move;
		decoder->memoryOperator.Set       = dec->memoryOperator.Set;
	} 
	else
	{
		decoder = (Jpeg_DecOBJ *)mem_malloc(sizeof(Jpeg_DecOBJ), DCACHE_ALIGN_NUM);
		memset(decoder, 0, sizeof(Jpeg_DecOBJ));
	}
	
	
	InitHuffTab(dec, &decoder->Huff_vlc, &decoder->dentropy, NULL);
	decoder->Buffer_size = MAX_BUUFER_SIZE;
	decoder->instepDec_num = 16;
	decoder->addstep = 8;
	decoder->zoomout = 1;
	decoder->enableNormalChroma = 1;
#if defined(VOARMV7)
	decoder->idct_block = IDCT_Block8x8_8X8_ARMv7;
#elif defined(VOARMV6)
	decoder->idct_block = IDCT_Block8x8_8X8_ARMv6;
#else
	decoder->idct_block = IDCT_Block8x8_c;
#endif
	*phCodec = (VO_HANDLE)decoder;
	
	return ret;
}

static VO_U32 VO_API voDecExifProcess(VO_HANDLE hCodec,VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat)
{
	VOJPEGDECRETURNCODE retc;
	Jpeg_DecOBJ *decoder;
	JPEG_STREAM	*bitstream;
	JPEG_STREAM	*exifbitstream;
	JPEG_EXIF	*jexif;
	EXIFLibrary *exiflib;
	VO_HANDLE	thumbhCodec;
	VO_CODECBUFFER thumbInData;
	INT32		len;
	UINT32		readlen;
	UINT32    zeroIndata;
	UINT8*		exifbegin;	

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;
	decoder = (Jpeg_DecOBJ *)hCodec;
	bitstream = &decoder->instream;

	jexif		  = &decoder->exifMark;
	exifbitstream = &jexif->exifbitstream;
	exiflib		  =	&jexif->exiflib;

	init_exif(decoder, jexif);
	
	if(find_exifMark(bitstream) < 0)
		return VO_ERR_JPEGDEC_NO_EXIFMARKER;

	GET2BYTE(bitstream, len);
	
	if(bitstream->free_in_buffer < len - 2)
	{
		fillbuffer(bitstream);
		if(bitstream->free_in_buffer < len - 2)
			return VO_ERR_JPEGDEC_UNKNOWN_ERR;
	}		

	exifbegin = bitstream->next_output_byte + 6;
	initstream(exifbitstream, exifbegin, len - 8);
	
	len = decoder_exif(jexif);
	if(len == -1)
		return VO_ERR_JPEGDEC_UNSUPPORT_FEATURE;
	else if(len == -2)
		return VO_ERR_JPEGDEC_NOTHUMPIC;

	
	thumbInData.Buffer = exifbegin + exiflib->thumbdir.JPEGOffSet;
	thumbInData.Length = exiflib->thumbdir.JPEGByteCount;
	
	thumbhCodec = jexif->exifhCodec;
	if(thumbhCodec == NULL)
	{
		retc = voJPEGSUBDecInit(decoder, &thumbhCodec);
		if(VO_ERR_NONE != retc)
			return retc;
		
		zeroIndata = 0;	
		retc = voJPEGDecSetParameter(thumbhCodec, VO_PID_EXIFINFO_SUPPT, (VO_PTR)(&zeroIndata));

		jexif->exifhCodec = thumbhCodec;
	}
	
	retc = voJPEGDecProcess(thumbhCodec, &thumbInData, pOutData, pOutFormat);
	if(VO_ERR_NONE != retc)
		return retc;
	
	
	decoder->fposition = F_EXIFMARK;

	return VO_ERR_NONE;
}

static VO_U32 VO_API voDecIMGProcess(VO_HANDLE hCodec,VO_CODECBUFFER *pInData,VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat)
{
	VOJPEGDECRETURNCODE retc;
	Jpeg_DecOBJ *decoder;
	JPEG_STREAM	*bitstream;
	JPEG_PARA	*pjpara;
	JPEG_CACHE	*pcache;
	UINT32  readlen;
	INT32	reti;
	UINT8	ch;	

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	decoder = (Jpeg_DecOBJ *)hCodec;
	bitstream = &decoder->instream;	
	pcache = &decoder->b_cache;
	pjpara = &decoder->jpara;
	
	if(decoder->Headerdone == 0)
	{		
		if(decoder->input_mode)
		{
			reti = initfstream(decoder, bitstream, pInData->Buffer);
			if(reti == -2)
				return VO_ERR_JPEGDEC_OPENFILE_ERROR;
			else if(reti == -1)
				return VO_ERR_OUTOF_MEMORY;
		}
		else
		{
			if(pInData->Buffer == NULL || pInData->Length == 0)
				return VO_ERR_JPEGDEC_INBUFFERPOINT_ERR;
			initstream(bitstream, pInData->Buffer, pInData->Length);			
		}
		
		InitHuffTab(decoder, &decoder->Huff_vlc, &decoder->dentropy, NULL);
		decoder->zoomout = 1;
#if defined(VOARMV7)
		decoder->idct_block = IDCT_Block8x8_8X8_ARMv7;
#elif defined(VOARMV6)
		decoder->idct_block = IDCT_Block8x8_8X8_ARMv6;
#else
		decoder->idct_block = IDCT_Block8x8_c;
#endif
		decoder->jpara.restart_intval = 0;
		decoder->jpara.restarts_to_go = 0;

		if((reti = read_jpeg_header(decoder, bitstream, pjpara)) < 0)
		{
			if(reti == -1)
				return VO_ERR_INPUT_BUFFER_SMALL;
			else if(reti == -2)
				return VO_ERR_JPEGDEC_UNSUPPORT_FEATURE;
		}
		
		if((retc = Initpicture(decoder)) < 0)
			return retc;

		if((retc = UpdateParameter(decoder, pOutData, pOutFormat)) < 0)
			return retc;
		
		if((reti = init_setup(decoder)) < 0)
			return VO_ERR_OUTOF_MEMORY;

		INITCACHE(pcache);
		UPDATACACHE2(bitstream, pcache, 0);

		decoder->stepDec_num = 
			DIV(decoder->zoomout*decoder->instepDec_num, pjpara->MCU_heigth);
		
		pjpara->blockCount_v[0] = pjpara->blockCount_v[1]
			= pjpara->blockCount_v[2] = 1;
		
		decoder->Headerdone = 1;
		decoder->block_Count = 0;
		decoder->Y_block = 0;
	}
	
	if(!pjpara->progressMode)
	{
		if(decoder->zoomout <= 8)
		{			
			if((reti = JPEG_interlmcu_dec(decoder, pOutData)) < 0)
			{
				decoder->fposition = F_RAWDATA;
				if(reti == -1)	
					return VO_ERR_INPUT_BUFFER_SMALL;
				else
					return VO_ERR_JPEGDEC_DECODE_UNFINISHED;
			}
		}
		else
		{
			if((reti = JPEG_interlmcu_dec8(decoder, pOutData)) < 0)
			{
				decoder->fposition = F_RAWDATA;
				if(reti == -1)	
					return VO_ERR_INPUT_BUFFER_SMALL;
				else
					return VO_ERR_JPEGDEC_DECODE_UNFINISHED;
			}
		}
	}
	else
	{
		if((reti = JPEG_progrmcu_dec(decoder, pOutData)) < 0)
		{
			decoder->fposition = F_RAWDATA;
			if(reti == -1)	
				return VO_ERR_INPUT_BUFFER_SMALL;
			else
				return VO_ERR_JPEGDEC_DECODE_UNFINISHED;
		}		
	}
	
	UPDATACACHE2(bitstream, pcache, 8 - pcache->used_bits & 7);	
	RELEASECACHE(pcache);		
	
	pInData->Length = bitstream->buffer_size - bitstream->free_in_buffer;
	pcache->unread_marker = 0;
	decoder->Headerdone = 0;
	decoder->zoomout = 1;
	
	return VO_ERR_NONE;
}


VO_U32 VO_API voJPEGDecInit(VO_HANDLE *phCodec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	INT32 ret = VO_ERR_NONE;
	Jpeg_DecOBJ *decoder;
	JPEGDEC_FRONT* pDecFront;

	if(VO_IMAGE_CodingJPEG != vType){
		return VO_ERR_JPEGDEC_DECODE_HEADER_ERR;
	}

	if(!pUserData){
		pDecFront = (JPEGDEC_FRONT *)mem_malloc(sizeof(JPEGDEC_FRONT), DCACHE_ALIGN_NUM);
		decoder = (Jpeg_DecOBJ *)mem_malloc(sizeof(Jpeg_DecOBJ), DCACHE_ALIGN_NUM);
		memset(decoder, 0, sizeof(Jpeg_DecOBJ));
		InitHuffTab(decoder, &decoder->Huff_vlc, &decoder->dentropy, NULL);
		decoder->Buffer_size = MAX_BUUFER_SIZE;
		decoder->instepDec_num = 16;
		decoder->addstep = 8;
		decoder->zoomout = 1;
		decoder->enableNormalChroma = 1;
#if defined(VOARMV7)
		decoder->idct_block = IDCT_Block8x8_8X8_ARMv7;
#elif defined(VOARMV6)
		decoder->idct_block = IDCT_Block8x8_8X8_ARMv6;
#else
		decoder->idct_block = IDCT_Block8x8_c;
#endif
		pDecFront->pDecCore = (VO_VOID *)decoder;
		*phCodec = (VO_HANDLE)pDecFront;
	}else if(VO_IMF_USERMEMOPERATOR == pUserData->memflag){
		VO_MEM_OPERATOR* pMemOperator;
		VO_MEM_INFO MemInfo;

		pMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;

		MemInfo.Flag = 0;
		MemInfo.Size = sizeof(JPEGDEC_FRONT);
		pMemOperator->Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
		pDecFront = (JPEGDEC_FRONT *)MemInfo.VBuffer;

		MemInfo.Size = sizeof(Jpeg_DecOBJ);
		pMemOperator->Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
		decoder = (Jpeg_DecOBJ *)MemInfo.VBuffer;

		pMemOperator->Set(VO_INDEX_DEC_JPEG, decoder, 0, sizeof(Jpeg_DecOBJ));
		InitHuffTab(decoder, &decoder->Huff_vlc, &decoder->dentropy, NULL);
		decoder->Buffer_size = MAX_BUUFER_SIZE;
		decoder->instepDec_num = 16;
		decoder->addstep = 8;
		decoder->zoomout = 1;
		decoder->enableNormalChroma = 1;
#if defined(VOARMV7)
		decoder->idct_block = IDCT_Block8x8_8X8_ARMv7;
#elif defined(VOARMV6)
		decoder->idct_block = IDCT_Block8x8_8X8_ARMv6;
#else
		decoder->idct_block = IDCT_Block8x8_c;
#endif

		decoder->memoryOperator.Alloc     = pMemOperator->Alloc;
		decoder->memoryOperator.Check     = pMemOperator->Check;
		decoder->memoryOperator.Compare   = pMemOperator->Compare;
		decoder->memoryOperator.Copy      = pMemOperator->Copy;
		decoder->memoryOperator.Free      = pMemOperator->Free;
		decoder->memoryOperator.Move      = pMemOperator->Move;
		decoder->memoryOperator.Set       = pMemOperator->Set;

		pDecFront->pDecCore = (VO_VOID *)decoder;
		*phCodec = (VO_HANDLE)pDecFront;
	}



#ifdef LICENSEFILE 
	voCheckLibInit(&decoder->phCheck, VO_INDEX_DEC_JPEG, 2, g_hJPEGDecInst);//huwei 20090922 checkLib	
#endif
	
	return ret;
}

VO_U32 VO_API voJPEGDecProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat)
{
	VOJPEGDECRETURNCODE retc;
	Jpeg_DecOBJ *decoder;
	JPEG_STREAM	*bitstream;
	INT32		reti;

	JPEGDEC_FRONT* pDecFront;

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	pDecFront = (JPEGDEC_FRONT *)hCodec;
	decoder = (Jpeg_DecOBJ *)pDecFront->pDecCore;
	
	if(decoder->IsdecExif)
	{
		bitstream = &decoder->instream;	
		if(decoder->input_mode)
		{
			reti = initfstream(decoder, bitstream, pInData->Buffer);
			if(reti == -2)
				return VO_ERR_JPEGDEC_OPENFILE_ERROR;
			else if(reti == -1)
				return VO_ERR_OUTOF_MEMORY;
		}
		else
		{	
			if(pInData->Buffer == NULL || pInData->Length == 0)
				return VO_ERR_JPEGDEC_INBUFFERPOINT_ERR;
			initstream(bitstream, pInData->Buffer, pInData->Length);
		}

		retc = voDecExifProcess(hCodec, pOutData, pOutFormat);
	}
	else
	{
		retc = voDecIMGProcess(hCodec, pInData, pOutData, pOutFormat);
	}

	return retc;
}

VO_U32 VO_API voJPEGDecUninit(VO_HANDLE hCodec)
{
	UINT32 i;
	Jpeg_DecOBJ *decoder;
	JPEG_PARA	*pjpara;
	JPEGDEC_FRONT* pDecFront;

	VO_MEM_OPERATOR MemOperator;

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	MemOperator.Free = NULL;

	pDecFront = (JPEGDEC_FRONT *)hCodec;
	decoder = (Jpeg_DecOBJ *)pDecFront->pDecCore;
	pjpara = &decoder->jpara;

	if(decoder->memoryOperator.Free)
	{
		MemOperator.Free = decoder->memoryOperator.Free;
	}

#ifdef LICENSEFILE
	voCheckLibUninit(decoder->phCheck);//huwei 20090922 checkLib
#endif

	FreeHuffTab(&decoder->Huff_vlc);

	if(MemOperator.Free)
	{
		for(i = 0; i < NUM_BLOCK_TBLS; i++)
		{
			MemOperator.Free(VO_INDEX_DEC_JPEG,pjpara->quant_tbl.Quant_tbl_ptrs[i]);
			pjpara->quant_tbl.Quant_tbl_ptrs[i] = NULL;
		}

		for(i = 0; i < NUM_HUFF_TBLS; i++)
		{
			MemOperator.Free(VO_INDEX_DEC_JPEG,pjpara->huffum_tbl.dc_huff_tbl_ptrs[i]);
			pjpara->huffum_tbl.dc_huff_tbl_ptrs[i] = NULL;

			MemOperator.Free(VO_INDEX_DEC_JPEG,pjpara->huffum_tbl.ac_huff_tbl_ptrs[i]);
			pjpara->huffum_tbl.ac_huff_tbl_ptrs[i] = NULL;
		}

		MemOperator.Free(VO_INDEX_DEC_JPEG,pjpara->MCUBlock.WorkBlock[0]);
	} 
	else
	{
		for(i = 0; i < NUM_BLOCK_TBLS; i++)
		{
			mem_free(pjpara->quant_tbl.Quant_tbl_ptrs[i]);
			pjpara->quant_tbl.Quant_tbl_ptrs[i] = NULL;
		}

		for(i = 0; i < NUM_HUFF_TBLS; i++)
		{
			mem_free(pjpara->huffum_tbl.dc_huff_tbl_ptrs[i]);
			pjpara->huffum_tbl.dc_huff_tbl_ptrs[i] = NULL;

			mem_free(pjpara->huffum_tbl.ac_huff_tbl_ptrs[i]);
			pjpara->huffum_tbl.ac_huff_tbl_ptrs[i] = NULL;
		}

		mem_free(pjpara->MCUBlock.WorkBlock[0]);
	}
	

	for(i = 0; i < JPEG_MAX_BLOCKS; i++)
	{
		pjpara->MCUBlock.WorkBlock[i] = NULL;
	}

	if (MemOperator.Free)
	{
		for(i = 0; i < JPEG_MAX_COLOR; i++)
		{
			MemOperator.Free(VO_INDEX_DEC_JPEG, decoder->outBuffer[i]);
			decoder->outBuffer[i] = NULL;
		}
	} 
	else
	{
		for(i = 0; i < JPEG_MAX_COLOR; i++)
		{
			mem_free(decoder->outBuffer[i]);
			decoder->outBuffer[i] = NULL;
		}
	}
	


	if(decoder->exifMark.exifhCodec)
	{
		voJPEGDecUninit(decoder->exifMark.exifhCodec);
	}

	releasestream(decoder, &decoder->instream);

	if(MemOperator.Free)
	{
		MemOperator.Free(VO_INDEX_DEC_JPEG, decoder);
		MemOperator.Free(VO_INDEX_DEC_JPEG, pDecFront);
	} 
	else
	{
		mem_free(decoder);
		mem_free(pDecFront);
	}



	return VO_ERR_NONE;
}

VO_U32 VO_API voJPEGDecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	VOJPEGDECRETURNCODE retc;
// 	VOCODECVIDEOBUFFER *pOutData;
	JPEG_IMAGE_FIELD *in_field;
	Jpeg_DecOBJ *decoder;
	JPEG_STREAM	*bitstream;
	JPEG_PARA	*pjpara;
	JPEG_CACHE	*pcache;
	INT8*		pfilepath;
	INT32		reti, i;
	UINT32*     pValue = (UINT32 *)plValue;

	JPEGDEC_FRONT* pDecFront;

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	pDecFront =(JPEGDEC_FRONT *)hCodec;
	decoder = (Jpeg_DecOBJ *)pDecFront->pDecCore;
	bitstream = &decoder->instream;	
	pcache = &decoder->b_cache;
	pjpara = &decoder->jpara;
	
	switch(nID)
	{
	case VO_JPEG_DECSTEP_ENABLE:
		decoder->EnstepDec = *pValue;
		break;
	case VO_JPEG_DECSTEP_NUMBER:
		if(*pValue > 16)
			decoder->instepDec_num = *pValue;/*(UINT32)lValue*/;
		break;
	case VO_PID_EXIFINFO_SUPPT:
		decoder->IsdecExif = *pValue;
		break;
	case VO_JPEG_INPUT_MODE:
		decoder->input_mode = *pValue;
		break;
	case VO_JPEG_BUFFER_LIMITED:
		decoder->Buffer_size = *pValue;
		break;
	case VO_JPEG_FILE_PATH:
		pfilepath =  (INT8 *)plValue;
		reti = initfstream(decoder, bitstream, pfilepath);
		decoder->input_mode = 1;/*huwei 20091019 add input buffer*/
		decoder->fposition = F_START;
		if(reti == -2)
			return VO_ERR_JPEGDEC_OPENFILE_ERROR;
		else if(reti == -1)
			return VO_ERR_OUTOF_MEMORY;

		decoder->Headerdone = 0;
		decoder->Buffer_size = MAX_BUUFER_SIZE;
		decoder->instepDec_num = 16;
		decoder->addstep = 8;
		decoder->zoomout = 1;
#if defined(VOARMV7)
		decoder->idct_block = IDCT_Block8x8_8X8_ARMv7;
#elif defined(VOARMV6)
		decoder->idct_block = IDCT_Block8x8_8X8_ARMv6;
#else
		decoder->idct_block = IDCT_Block8x8_c;
#endif
		decoder->jpara.restart_intval = 0;
		decoder->jpara.restarts_to_go = 0;

		init_exif(decoder,&decoder->exifMark);
		FreeHuffTab(&decoder->Huff_vlc);
		InitHuffTab(decoder, &decoder->Huff_vlc, &decoder->dentropy, NULL);

		if((reti = read_jpeg_header(decoder, bitstream, pjpara)) < 0)
		{
			if(reti == -1)
				return VO_ERR_INPUT_BUFFER_SMALL;
			else  if(reti == -2)
				return VO_ERR_JPEGDEC_UNSUPPORT_FEATURE;
		}

		decoder->HeaderLength = reti;
		decoder->fposition = F_SOSMARK;

		if((retc = Initpicture(decoder)) < 0)
			return retc;
		
		break;
	case VO_JPEG_INPUT_BUFFER:
		{
			VO_CODECBUFFER* pInData = (VO_CODECBUFFER *)plValue;//huwei 20091019 add input buffer

			reti = initstream(bitstream, pInData->Buffer, pInData->Length);
			decoder->fposition = F_START;
			decoder->input_mode = 0;
			if(reti == -2)
				return VO_ERR_JPEGDEC_OPENFILE_ERROR;
			else if(reti == -1)
				return VO_ERR_OUTOF_MEMORY;

			decoder->Headerdone = 0;
			decoder->Buffer_size = MAX_BUUFER_SIZE;
			decoder->instepDec_num = 16;
			decoder->addstep = 8;
			decoder->zoomout = 1;
#if defined(VOARMV7)
			decoder->idct_block = IDCT_Block8x8_8X8_ARMv7;
#elif defined(VOARMV6)
			decoder->idct_block = IDCT_Block8x8_8X8_ARMv6;
#else
			decoder->idct_block = IDCT_Block8x8_c;
#endif
			decoder->jpara.restart_intval = 0;
			decoder->jpara.restarts_to_go = 0;

			init_exif(decoder,&decoder->exifMark);
			FreeHuffTab(&decoder->Huff_vlc);
			InitHuffTab(decoder, &decoder->Huff_vlc, &decoder->dentropy, NULL);

			if((reti = read_jpeg_header(decoder, bitstream, pjpara)) < 0)
			{
				if(reti == -1)
					return VO_ERR_INPUT_BUFFER_SMALL;
				else  if(reti == -2)
					return VO_ERR_JPEGDEC_UNSUPPORT_FEATURE;
			}

			decoder->HeaderLength = reti;
			decoder->fposition = F_SOSMARK;

			if((retc = Initpicture(decoder)) < 0)
				return retc;
		}
		break;
	case VO_JPEG_OUTPUT_FIELD:
		if(plValue)
		{
			in_field = (JPEG_IMAGE_FIELD *)plValue;
			if(in_field->start_X < 0) in_field->start_X = 0;
			if(in_field->start_X > pjpara->width) in_field->start_X = pjpara->width;
			decoder->imagefield.start_X = DIV(in_field->start_X,  pjpara->max_h_sample * 8);

			if(in_field->start_Y < 0) in_field->start_Y = 0;
			if(in_field->start_Y > pjpara->heigth) in_field->start_Y = pjpara->heigth;
			decoder->imagefield.start_Y = DIV(in_field->start_Y,  pjpara->max_v_sample * 8);

			if(in_field->end_X < 0) in_field->end_X = 0;
			if(in_field->end_X > pjpara->width) in_field->end_X = pjpara->width;
			decoder->imagefield.end_X = DIV(in_field->end_X,  pjpara->max_h_sample * 8);
			decoder->field_hmax = decoder->imagefield.end_X;

			if(in_field->end_Y < 0) in_field->end_Y = 0;
			if(in_field->end_Y > pjpara->heigth) in_field->end_Y = pjpara->heigth;
			decoder->imagefield.end_Y = DIV(in_field->end_Y,  pjpara->max_v_sample * 8);
			decoder->field_vmax = decoder->imagefield.end_Y;

			decoder->Isfield = 1;
		}
		break;
	case VO_JPEG_OUTPUT_ZOOMOUT:
		decoder->zoomout = *pValue;/*(UINT32)lValue*/;
		switch(decoder->zoomout)
		{
		case 1:
			decoder->addstep = 8;
#if defined(VOARMV7)
			decoder->idct_block = IDCT_Block8x8_8X8_ARMv7;
#elif defined(VOARMV6)
			decoder->idct_block = IDCT_Block8x8_8X8_ARMv6;
#else
			decoder->idct_block = IDCT_Block8x8_c;
#endif
			break;
		case 2:
			decoder->addstep = 4;
#if defined(VOARMV7)
			decoder->idct_block = IDCT_Block8x8_4X4_ARMv7;
#elif defined(VOARMV6)
			decoder->idct_block = IDCT_Block8x8_4X4_ARMv6;
#else
			decoder->idct_block = IDCT_Block8x8_c2;
#endif
			break;
		case 4:
			decoder->addstep = 2;
#if defined(VOARMV7)
			decoder->idct_block = IDCT_Block8x8_2X2_ARMv7;
#elif defined(VOARMV6)
			decoder->idct_block = IDCT_Block8x8_2X2_ARMv6;
#else
			decoder->idct_block = IDCT_Block8x8_c4;
#endif
			break;
		case 8:
			decoder->addstep = 1;
#if defined(VOARMV7)
			decoder->idct_block = IDCT_Block8x8_1X1_ARMv7;
#elif defined(VOARMV6)
			decoder->idct_block = IDCT_Block8x8_1X1_ARMv6;
#else
			decoder->idct_block = IDCT_Block8x8_c8;
#endif
			break;
		default: /* > 8 */
			decoder->addstep = 1;
#if defined(VOARMV7)
			decoder->idct_block = IDCT_Block8x8_1X1_ARMv7;
#elif defined(VOARMV6)
			decoder->idct_block = IDCT_Block8x8_1X1_ARMv6;	
#else
			decoder->idct_block = IDCT_Block8x8_c8;			
#endif
			break;
		}
		break;
	case VO_JPEG_FLUSH_MEMORY:
		if(*pValue) {
			for(i = 0; i < JPEG_MAX_COLOR; i++)
			{
				if (decoder->memoryOperator.Free)
				{
					decoder->memoryOperator.Free(VO_INDEX_DEC_JPEG, decoder->outBuffer[i]);
				} 
				else
				{
					mem_free(decoder->outBuffer[i]);
				}
				
				decoder->outBuffer[i] = NULL;
				decoder->imagedsize[i] = 0;
			}

			FreeHuffTab(&decoder->Huff_vlc);
		}
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	
	return VO_ERR_NONE;
}

VO_U32 VO_API voJPEGDecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	VO_VIDEO_BUFFER *pOutData;
	VO_VIDEO_BUFFER OutData_bk;
	Jpeg_DecOBJ *decoder;
	JPEG_STREAM	*bitstream;
	JPEG_PARA	*pjpara;
	JPEG_CACHE	*pcache;
	JPEG_COMP_ITEM	*comptr;
	VOJPEGDECRETURNCODE retc;
	UINT32		readlen,  i;
	UINT32		H_step;
	INT32		reti;
	UINT8		ch;	

	JPEGDEC_FRONT* pDecFront;

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	pDecFront = (JPEGDEC_FRONT *)hCodec;
	decoder = (Jpeg_DecOBJ *)pDecFront->pDecCore;
	bitstream = &decoder->instream;	
	pcache = &decoder->b_cache;
	pjpara = &decoder->jpara;

	switch(nID)
	{
	case VO_PID_JPEG_WIDTH:
		readlen = decoder->exifMark.exiflib.Exifdir.PixelXDimension;
		if(readlen)
			*((UINT32*)plValue) = (readlen + 1) & ~1;		
		else
			*((UINT32*)plValue) = (pjpara->width + 1) & ~1;
		break;
	case VO_PID_JPEG_HEIGHT:
		readlen = decoder->exifMark.exiflib.Exifdir.PixelYDimension;
		if(readlen)
			*((UINT32*)plValue) = (readlen + 1) & ~1;		
		else
			*((UINT32*)plValue) = (pjpara->heigth + 1) & ~1;
		break;
	case VO_JPEG_VIDEO_TYPE:
		switch(pjpara->image_type)
		{
//		case 0x00000011:
//			*plValue = VOGRAY_PLANARGRAY;
//			break;			
		case 0x00111111:
			*((UINT32*)plValue) = VO_COLOR_YUV_PLANAR444;
			break;
		case 0x00111121:
			*((UINT32*)plValue) = VO_COLOR_YUV_PLANAR422_12;
			break;
		case 0x00111112:
			*((UINT32*)plValue) = VO_COLOR_YUV_PLANAR422_21;
			break;
		case 0x00000011:
		case 0x00111122:
			*((UINT32*)plValue) = VO_COLOR_YUV_PLANAR420;
			break;
		case 0x00111141:
			*((UINT32*)plValue) = VO_COLOR_YUV_PLANAR411;			
			break;
		case 0x00111114:
			*((UINT32*)plValue) = VO_COLOR_YUV_PLANAR411V;			
			break;
		case 0x11111111:
			*((UINT32*)plValue) = VO_COLOR_RGB888_PLANAR;
			break;
		default:
			*((UINT32*)plValue) = pjpara->image_type;	
		}
		break;
	case VO_JPEG_OUTPUT_BUFFER:
		if(!plValue)
		{
			return VO_ERR_INVALID_ARG;
		}
		
		pOutData = (VO_VIDEO_BUFFER *)plValue;
		if(decoder->Headerdone == 0)
		{		
			if(!pjpara->progressMode)
			{
				if(decoder->fposition != F_SOSMARK)
				{
					if(decoder->input_mode)
					{
						feekstream(bitstream, decoder->HeaderLength);
					} 
					else
					{
						seekstream(bitstream, decoder->HeaderLength);/*huwei 20091019 add input buffer*/
					}
					
					decoder->fposition = F_SOSMARK;
				}		
			}
			else
			{
				if(decoder->input_mode)
				{
					feekstream(bitstream, 0);
				} 
				else
				{
					seekstream(bitstream, 0);//huwei 20091019 add input buffer
				}
				
				decoder->fposition = F_START;
				
				if((reti = read_jpeg_header(decoder, bitstream, pjpara)) < 0)
				{
					if(reti == -1)
						return VO_ERR_INPUT_BUFFER_SMALL;
					else  if(reti == -2)
						return VO_ERR_JPEGDEC_UNSUPPORT_FEATURE;
				}
				
				decoder->HeaderLength = reti;
				decoder->fposition = F_SOSMARK;
				
				if((retc = UpdateParameter(decoder, NULL, NULL)) < 0)
					return retc;					

			}

			if((reti = init_setup(decoder)) < 0)
				return VO_ERR_OUTOF_MEMORY;

			INITCACHE(pcache);
			UPDATACACHE2(bitstream, pcache, 0);

			decoder->stepDec_num = 
				DIV(decoder->zoomout*decoder->instepDec_num, pjpara->MCU_heigth);
			
			pjpara->blockCount_v[0] = pjpara->blockCount_v[1]
				= pjpara->blockCount_v[2] = 1;

			decoder->Headerdone = 1;
			decoder->block_Count = 0;
			decoder->Y_block = 0;

			if(pjpara->image_type == 0x00000011)
			{
				H_step = (decoder->imagefield.end_Y - decoder->imagefield.start_Y) * pjpara->max_v_sample * 8/decoder->zoomout;

				if(decoder->memoryOperator.Set)
				{
					decoder->memoryOperator.Set(VO_INDEX_DEC_JPEG, pOutData->Buffer[1], 128, pOutData->Stride[1] * H_step / 2);
					decoder->memoryOperator.Set(VO_INDEX_DEC_JPEG, pOutData->Buffer[2], 128, pOutData->Stride[2] * H_step / 2);
				} 
				else
				{
					memset(pOutData->Buffer[1], 128, pOutData->Stride[1] * H_step / 2);
					memset(pOutData->Buffer[2], 128, pOutData->Stride[2] * H_step / 2);
				}

			}
		}
		
		if(!pjpara->progressMode)
		{
			if(decoder->zoomout <= 8)
			{			
				if((reti = JPEG_interlmcu_dec(decoder, pOutData)) < 0)
				{
					decoder->fposition = F_RAWDATA;
					if(reti == -1)	
						return VO_ERR_INPUT_BUFFER_SMALL;
					else
						return VO_ERR_JPEGDEC_DECODE_UNFINISHED;
				}
			}
			else
			{
				if((reti = JPEG_interlmcu_dec8(decoder, pOutData)) < 0)
				{
					decoder->fposition = F_RAWDATA;
					if(reti == -1)	
						return VO_ERR_INPUT_BUFFER_SMALL;
					else
						return VO_ERR_JPEGDEC_DECODE_UNFINISHED;
				}
			}
		}
		else
		{
			OutData_bk.ColorType = pOutData->ColorType;
			for(i = 0; i < 3; i++)
			{
				OutData_bk.Buffer[i] = pOutData->Buffer[i];
				OutData_bk.Stride[i]   = pOutData->Stride[i];
			}

			if((reti = JPEG_progrmcu_dec(decoder, &OutData_bk)) < 0)
			{
				decoder->fposition = F_RAWDATA;
			}

			decoder->Headerdone = 0;
			decoder->field_Num--;		
			if(decoder->field_Num)
			{
				H_step = DIV(decoder->field_step, pjpara->max_v_sample * 8);
				
				for(i = 0, comptr = pjpara->compon; i < pjpara->num_component; 
					i++, comptr++)
				{
					OutData_bk.Buffer[i] += OutData_bk.Stride[i] * H_step * comptr->v_sample * 8/decoder->zoomout;					
				}

				decoder->imagefield.start_Y += H_step;
				decoder->imagefield.end_Y	+= H_step;
				if(decoder->imagefield.start_Y > decoder->field_vmax) 
					decoder->imagefield.start_Y = decoder->field_vmax;
				if(decoder->imagefield.end_Y > decoder->field_vmax) 
					decoder->imagefield.end_Y = decoder->field_vmax;
				
				retc = voJPEGDecGetParameter(hCodec, VO_JPEG_OUTPUT_BUFFER, &OutData_bk);
				if(retc < 0) return retc;
			};					
		}

#ifdef LICENSEFILE
		{
			VO_VIDEO_FORMAT		Format;	
			

			Format.Height = ((pjpara->heigth + decoder->zoomout - 1) / decoder->zoomout + 1) & ~1;;
			Format.Width  = ((pjpara->width  + decoder->zoomout - 1) / decoder->zoomout + 1) & ~1;;
			Format.Type   = VO_VIDEO_FRAME_I;
			voCheckLibCheckImage(decoder->phCheck, pOutData, &Format);

		}
#endif
	
		decoder->fposition	= F_END;
		decoder->Headerdone = 0;
		decoder->zoomout = 1;
		decoder->field_Num = 0;
		break;
	case VO_PID_JPEG_QUALITY:
		*((UINT32*)plValue) = 100;
		break;
	case VO_PID_JPEG_PROGMODE:
		*((UINT32*)plValue) = pjpara->progressMode;
		break;
	case VO_PID_MACHINE_MAKE:
		if(decoder->exifMark.exiflib.enTIFFD & 0x00000001)
		{
			readlen = strlen(decoder->exifMark.exiflib.Tiffdir.CameraMake);
			if (decoder->memoryOperator.Copy)
			{
				decoder->memoryOperator.Copy(VO_INDEX_DEC_JPEG, plValue,decoder->exifMark.exiflib.Tiffdir.CameraMake, readlen+1);
			} 
			else
			{
				memcpy(plValue,decoder->exifMark.exiflib.Tiffdir.CameraMake, readlen+1);
			}
			
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_MACHINE_MODEL:
		if(decoder->exifMark.exiflib.enTIFFD & 0x00000002)
		{
			readlen = strlen(decoder->exifMark.exiflib.Tiffdir.CameraModel);
			if(decoder->memoryOperator.Copy)
			{
				decoder->memoryOperator.Copy(VO_INDEX_DEC_JPEG, plValue,decoder->exifMark.exiflib.Tiffdir.CameraModel, readlen+1);
			} 
			else
			{
				memcpy(plValue,decoder->exifMark.exiflib.Tiffdir.CameraModel, readlen+1);
			}
			
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_GPS_INFO:
		*((UINT32*)plValue) = decoder->exifMark.exiflib.enGPSDir;
		break;
	case VO_PID_PICTURE_DATE:
		if(decoder->exifMark.exiflib.enTIFFD & 0x00000004)
		{
			if(decoder->memoryOperator.Copy)
			{
				decoder->memoryOperator.Copy(VO_INDEX_DEC_JPEG, plValue,decoder->exifMark.exiflib.Tiffdir.DateTime, 20);
			} 
			else
			{
				memcpy(plValue,decoder->exifMark.exiflib.Tiffdir.DateTime, 20);
			}
			
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_PICXRESOLUTION:
		if(decoder->exifMark.exiflib.enTIFFD & 0x00000020)
		{
			*(((UINT32*)plValue) + 0) = decoder->exifMark.exiflib.Tiffdir.XResolution[0];
			*(((UINT32*)plValue) + 1) = decoder->exifMark.exiflib.Tiffdir.XResolution[1];
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_PICYRESOLUTION:
		if(decoder->exifMark.exiflib.enTIFFD & 0x00000040)
		{
			*(((UINT32*)plValue) + 0) = decoder->exifMark.exiflib.Tiffdir.YResolution[0]; 
			*(((UINT32*)plValue) + 1) = decoder->exifMark.exiflib.Tiffdir.YResolution[1];
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_GPS_LATIREF:
		if(decoder->exifMark.exiflib.enGPSDir & 0x00000002)
		{
			*((UINT32*)plValue) = decoder->exifMark.exiflib.Gpsdir.GPSLatitudeRef;
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_GPS_LATITUDE:
		if(decoder->exifMark.exiflib.enGPSDir & 0x00000004)
		{
			*(((UINT32*)plValue) + 0) = decoder->exifMark.exiflib.Gpsdir.GPSLatitude[0];
			*(((UINT32*)plValue) + 1) = decoder->exifMark.exiflib.Gpsdir.GPSLatitude[1];
			*(((UINT32*)plValue) + 2) = decoder->exifMark.exiflib.Gpsdir.GPSLatitude[2];
			*(((UINT32*)plValue) + 3) = decoder->exifMark.exiflib.Gpsdir.GPSLatitude[3];
			*(((UINT32*)plValue) + 4) = decoder->exifMark.exiflib.Gpsdir.GPSLatitude[4];
			*(((UINT32*)plValue) + 5) = decoder->exifMark.exiflib.Gpsdir.GPSLatitude[5];
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_GPS_LONGIREF:
		if(decoder->exifMark.exiflib.enGPSDir & 0x00000008)
		{
			*((UINT32*)plValue) = decoder->exifMark.exiflib.Gpsdir.GPSLongtiudeRef;
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_GPS_LONGITUDE:
		if(decoder->exifMark.exiflib.enGPSDir & 0x00000010)
		{
			*(((UINT32*)plValue) + 0) = decoder->exifMark.exiflib.Gpsdir.GPSLongtiude[0];
			*(((UINT32*)plValue) + 1) = decoder->exifMark.exiflib.Gpsdir.GPSLongtiude[1];
			*(((UINT32*)plValue) + 2) = decoder->exifMark.exiflib.Gpsdir.GPSLongtiude[2];
			*(((UINT32*)plValue) + 3) = decoder->exifMark.exiflib.Gpsdir.GPSLongtiude[3];
			*(((UINT32*)plValue) + 4) = decoder->exifMark.exiflib.Gpsdir.GPSLongtiude[4];
			*(((UINT32*)plValue) + 5) = decoder->exifMark.exiflib.Gpsdir.GPSLongtiude[5];
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_GPS_ALITUDEREF:
		if(decoder->exifMark.exiflib.enGPSDir & 0x00000020)
		{
			*((UINT32*)plValue) = decoder->exifMark.exiflib.Gpsdir.GPSAltitudeRef;
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_GPS_ALITUDE:
		if(decoder->exifMark.exiflib.enGPSDir & 0x00000040)
		{
			*(((UINT32*)plValue) + 0) = decoder->exifMark.exiflib.Gpsdir.GPSAltitude[0];
			*(((UINT32*)plValue) + 1) = decoder->exifMark.exiflib.Gpsdir.GPSAltitude[1];
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_GPS_MAPDATUM:
		if(decoder->exifMark.exiflib.enGPSDir & 0x00040000)
		{		
			readlen = strlen(decoder->exifMark.exiflib.Gpsdir.GPSMapDatum);
			if(decoder->memoryOperator.Copy)
			{
				decoder->memoryOperator.Copy(VO_INDEX_DEC_JPEG, plValue, decoder->exifMark.exiflib.Gpsdir.GPSMapDatum, readlen+1);
			} 
			else
			{
				memcpy(plValue, decoder->exifMark.exiflib.Gpsdir.GPSMapDatum, readlen+1);
			}

			
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	case VO_PID_GPS_DATESTAMP:
		if(decoder->exifMark.exiflib.enGPSDir & 0x00080000)
		{
			if(decoder->memoryOperator.Copy)
			{
				decoder->memoryOperator.Copy(VO_INDEX_DEC_JPEG, plValue, decoder->exifMark.exiflib.Gpsdir.GPSDatastemp, 11);
			} 
			else
			{
				memcpy(plValue, decoder->exifMark.exiflib.Gpsdir.GPSDatastemp, 11);
			}
			
		}
		else
		{
			return VO_ERR_JPEGDEC_NO_IDITEM;
		}
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}


VO_S32 VO_API voGetJPEGDecAPI (VO_IMAGE_DECAPI * pDecHandle)
{
	VO_IMAGE_DECAPI *pJPEGDec = pDecHandle;

	if(!pJPEGDec)
		return VO_ERR_INVALID_ARG;

	pJPEGDec->Init   = voJPEGDecInit;
	pJPEGDec->Uninit = voJPEGDecUninit;
	pJPEGDec->SetParam = voJPEGDecSetParameter;
	pJPEGDec->GetParam = voJPEGDecGetParameter;
	pJPEGDec->Process = voJPEGDecProcess;

	return VO_ERR_NONE;

}
