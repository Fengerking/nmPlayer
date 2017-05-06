/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include	"mem_align.h"
#include	"jdecoder.h"
#include    "voMJPEG.h"

#ifdef LICENSEFILE 
#include "voCheck.h"
#endif

VO_HANDLE g_hMJPEGDecInst = NULL;/*huwei 20090917 checkLib*/

#ifdef MEM_LEAKAGE
int malloc_count = 0;
int free_count = 0;
#endif // MEM_LEAKAGE

typedef struct{	
	VO_VOID* pDecCore;
	VO_VIDEO_BUFFER OutData;
	VO_CODECBUFFER  InputData;
	VO_VIDEO_OUTPUTINFO OutPutInfo;
	UINT32 InputUsed;/*huwei 20090715 multi-frame*/
	UINT8* pInputData;
}MJPEGDEC_FRONT;

VO_U32 VO_API voMJPEGDecInit(VO_HANDLE *phCodec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	UINT32	ret;
	Jpeg_DecOBJ *decoder;
	MJPEGDEC_FRONT* pDecFront;

	if(VO_VIDEO_CodingMJPEG != vType){
		return VO_ERR_DEC_MJPEG_DECODE_HEADER;
	}

	if(!pUserData){
		pDecFront = (MJPEGDEC_FRONT*)mem_malloc(sizeof(MJPEGDEC_FRONT), DCACHE_ALIGN_NUM);
		pDecFront->pDecCore = (VO_VOID *)mem_malloc(sizeof(Jpeg_DecOBJ), DCACHE_ALIGN_NUM);
		decoder = (Jpeg_DecOBJ *)pDecFront->pDecCore;
		memset(decoder, 0, sizeof(Jpeg_DecOBJ));
		InitHuffTab(decoder,&decoder->Huff_vlc, &decoder->dentropy, NULL);
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
		decoder->chromaBuffer = (UINT8 *)mem_malloc(JPEG_BLOCK_SIZE,DCACHE_ALIGN_NUM);/*huwei 20090725 normal charom*/
		if(NULL == decoder->chromaBuffer)
			return VO_ERR_OUTOF_MEMORY;
		decoder->enableNormalChroma = 0;
	}else if(VO_IMF_USERMEMOPERATOR == pUserData->memflag){
		VO_MEM_OPERATOR* pMemOperator;
		VO_MEM_INFO MemInfo;

		pMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;

		MemInfo.Flag = 0;
		MemInfo.Size = sizeof(MJPEGDEC_FRONT);
		pMemOperator->Alloc(VO_INDEX_DEC_MJPEG, &MemInfo);
		pDecFront = (MJPEGDEC_FRONT*)MemInfo.VBuffer;

		MemInfo.Size = sizeof(Jpeg_DecOBJ);
		pMemOperator->Alloc(VO_INDEX_DEC_MJPEG, &MemInfo);
		pDecFront->pDecCore = (VO_VOID *)MemInfo.VBuffer;

		decoder = (Jpeg_DecOBJ *)pDecFront->pDecCore;
		pMemOperator->Set(VO_INDEX_DEC_MJPEG, decoder, 0, sizeof(Jpeg_DecOBJ));
		InitHuffTab(decoder, &decoder->Huff_vlc, &decoder->dentropy, NULL);
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
		MemInfo.Size = JPEG_BLOCK_SIZE;
		pMemOperator->Alloc(VO_INDEX_DEC_MJPEG, &MemInfo);
		decoder->chromaBuffer = (UINT8 *)MemInfo.VBuffer;/*huwei 20090725 normal charom*/
		if(NULL == decoder->chromaBuffer)
			return VO_ERR_OUTOF_MEMORY;
		decoder->enableNormalChroma = 0;

		decoder->memoryOperator.Alloc     = pMemOperator->Alloc;
		decoder->memoryOperator.Check     = pMemOperator->Check;
		decoder->memoryOperator.Compare   = pMemOperator->Compare;
		decoder->memoryOperator.Copy      = pMemOperator->Copy;
		decoder->memoryOperator.Free      = pMemOperator->Free;
		decoder->memoryOperator.Move      = pMemOperator->Move;
		decoder->memoryOperator.Set       = pMemOperator->Set;
	}

#ifdef LICENSEFILE 
	voCheckLibInit(&decoder->phCheck, VO_INDEX_DEC_MJPEG, 0, g_hMJPEGDecInst);//huwei 20090918 checkLib
#endif

	*phCodec = (VO_HANDLE)pDecFront;

	return VO_ERR_NONE;
}

VO_U32 VO_API voMJPEGDecProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat)
{
	VOMJPEGDECRETURNCODE retc;
	MJPEGDEC_FRONT* pDecFront;
	Jpeg_DecOBJ *decoder;
	JPEG_STREAM	*bitstream;
	JPEG_PARA	*pjpara;
	JPEG_CACHE	*pcache;
	UINT32  readlen;
	INT32	reti;
	UINT8	ch;	

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	pDecFront = (MJPEGDEC_FRONT *)hCodec;
	decoder = (Jpeg_DecOBJ *)pDecFront->pDecCore;
	pInData->Buffer = pDecFront->pInputData;/*huwei 20090715 multi-frame*/


	bitstream = &decoder->instream;	
	pcache = &decoder->b_cache;
	pjpara = &decoder->jpara;

	if(decoder->Headerdone == 0)
	{		
		if(pInData->Buffer == NULL || pInData->Length == 0)
			return VO_ERR_DEC_MJPEG_INBUFFERADR;
		initstream(bitstream, pInData->Buffer, pInData->Length);			

		if((reti = read_jpeg_header(decoder, bitstream, pjpara)) < 0)
		{
			if(reti == -1)
				return VO_ERR_INPUT_BUFFER_SMALL;
			else if(reti == -2)
				return VO_ERR_DEC_MJPEG_UNSUPPORT_FEATURE;
		}

		if((retc = Initpicture(decoder)) < 0)
			return retc;

		if((retc = UpdateParameter(decoder, pOutData, pOutFormat,pjpara->progressMode)) < 0)
			return retc;

		if((reti = init_setup(decoder)) < 0)
			return VO_ERR_OUTPUT_BUFFER_SMALL;

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
			}
		}
		else
		{
			if((reti = JPEG_interlmcu_dec8(decoder, pOutData)) < 0)
			{
				decoder->fposition = F_RAWDATA;
				if(reti == -1)	
					return VO_ERR_INPUT_BUFFER_SMALL;
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
		}		
	}

	UPDATACACHE2(bitstream, pcache, 8 - pcache->used_bits & 7);	
	RELEASECACHE(pcache);		

	pInData->Length = bitstream->buffer_size - bitstream->free_in_buffer;
	decoder->Headerdone = 0;
	//pOutFormat->InputUsed = pInData->Length;
	pOutData->Time = pInData->Time;

	pDecFront->pInputData += pInData->Length;/*huwei 20090715 multi-frame*/
	pDecFront->InputUsed += pInData->Length;
	pOutFormat->InputUsed = pDecFront->InputUsed;

	return VO_ERR_NONE;
}

VO_U32 VO_API voMJPEGDecUninit(VO_HANDLE hCodec)
{
	UINT32 i;
	Jpeg_DecOBJ *decoder;
	JPEG_PARA	*pjpara;
	MJPEGDEC_FRONT* pDecFront;

	VO_MEM_OPERATOR MemOperator;

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	MemOperator.Free = NULL;

	pDecFront = (MJPEGDEC_FRONT *)hCodec;
	decoder = (Jpeg_DecOBJ *)pDecFront->pDecCore;
	pjpara = &decoder->jpara;

	if(decoder->memoryOperator.Free)
	{
		MemOperator.Free = decoder->memoryOperator.Free;
	}

#ifdef LICENSEFILE
	voCheckLibUninit(decoder->phCheck);//huwei 20090918 checkLib
#endif

	FreeHuffTab(&decoder->Huff_vlc);

	if(decoder->memoryOperator.Free)
	{
		for(i = 0; i < NUM_BLOCK_TBLS; i++)
		{
			decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, pjpara->quant_tbl.Quant_tbl_ptrs[i]);
			pjpara->quant_tbl.Quant_tbl_ptrs[i] = NULL;
		}

		for(i = 0; i < NUM_HUFF_TBLS; i++)
		{
			decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, pjpara->huffum_tbl.dc_huff_tbl_ptrs[i]);
			pjpara->huffum_tbl.dc_huff_tbl_ptrs[i] = NULL;

			decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, pjpara->huffum_tbl.ac_huff_tbl_ptrs[i]);
			pjpara->huffum_tbl.ac_huff_tbl_ptrs[i] = NULL;
		}

		decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, pjpara->MCUBlock.WorkBlock[0]);
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

	if(decoder->memoryShare.Uninit)/*huwei 20090923 memory share*/
	{
		decoder->memoryShare.Uninit(VO_INDEX_DEC_MJPEG);
		decoder->outBuffer[0] = NULL;
		decoder->outBuffer[1] = NULL;
		decoder->outBuffer[2] = NULL;

		mem_free(decoder->chromaBuffer);/*huwei 20090725 normal chroma*/
	} 
	else
	{
		if(decoder->memoryOperator.Free)
		{
			for(i = 0; i < JPEG_MAX_COLOR; i++)
			{
				decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, decoder->outBuffer[i]);
				decoder->outBuffer[i] = NULL;
			}

			decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, decoder->chromaBuffer);/*huwei 20090725 normal chroma*/
		} 
		else
		{
			for(i = 0; i < JPEG_MAX_COLOR; i++)
			{
				mem_free(decoder->outBuffer[i]);
				decoder->outBuffer[i] = NULL;
			}

			mem_free(decoder->chromaBuffer);/*huwei 20090725 normal chroma*/
		}
	}




	decoder->chromaBuffer = NULL;


	releasestream(decoder, &decoder->instream);

	if(decoder->memoryOperator.Free)
	{
		decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, decoder);
		decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, pDecFront);

	}
	else
	{
		mem_free(decoder);
		mem_free(pDecFront);

	}


	return VO_ERR_NONE;
}

VO_U32 VO_API voMJPEGDecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	Jpeg_DecOBJ *decoder;
	JPEG_PARA	*pjpara;
	MJPEGDEC_FRONT* pDecFront;

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	pDecFront = (MJPEGDEC_FRONT *)hCodec;
	decoder = (Jpeg_DecOBJ *)pDecFront->pDecCore;
	pjpara = &decoder->jpara;

	switch (nID)
	{
	case VO_PID_DEC_MJPEG_SET_NORMAL_CHROMA:
		{
			INT32 enableNormalCharoma = *((INT32 *)plValue);/*huwei 20090726 normal chroma*/

			if ( 0 != enableNormalCharoma)
			{
				decoder->enableNormalChroma = 1;
			}

		}
		break;
	case VO_PID_VIDEO_VIDEOMEMOP:
		{
			VO_MEM_VIDEO_OPERATOR *pMemShare;

			pMemShare = (VO_MEM_VIDEO_OPERATOR *)plValue;

			decoder->memoryShare.GetBufByIndex = pMemShare->GetBufByIndex;
			decoder->memoryShare.Init = pMemShare->Init;
			decoder->memoryShare.Uninit = pMemShare->Uninit;
		}
		break;
	default:
		return VO_ERR_NONE;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voMJPEGDecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	Jpeg_DecOBJ *decoder;
	JPEG_PARA	*pjpara;
	MJPEGDEC_FRONT* pDecFront;

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;

	pDecFront = (MJPEGDEC_FRONT *)hCodec;
	decoder = (Jpeg_DecOBJ *)pDecFront->pDecCore;
	pjpara = &decoder->jpara;

	switch(nID)
	{
	case VO_PID_DEC_MJPEG_GET_VIDEO_WIDTH:
		*((UINT32 *)plValue) = (pjpara->width + 1) & ~1;
		break;
	case VO_PID_DEC_MJPEG_GET_VIDEO_HEIGHT:
		*((UINT32 *)plValue) = (pjpara->heigth + 1) & ~1;
		break;
	case VO_PID_VIDEO_FRAMETYPE:
		{
			VO_CODECBUFFER* pInData = (VO_CODECBUFFER*)plValue;/*huwei 20090715 get frame type*/
			pInData->Time = VO_VIDEO_FRAME_I;
		}
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voMJPEGDecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	MJPEGDEC_FRONT* pFront = (MJPEGDEC_FRONT*)hDec;

	if(!pFront)
		return VO_ERR_INVALID_ARG;

	pFront->InputData.Buffer = pInput->Buffer;
	pFront->InputData.Length = pInput->Length;
	pFront->InputData.Time   = pInput->Time;

	pFront->InputUsed  = 0;/*huwei 20090715 multi-frame*/
	pFront->pInputData = pInput->Buffer;

	return VO_ERR_NONE;
}

VO_U32 VO_API voMJPEGDecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutPutInfo)
{
	MJPEGDEC_FRONT* pFront = (MJPEGDEC_FRONT*)hDec;
	Jpeg_DecOBJ *decoder;

	if(!pFront)
		return VO_ERR_INVALID_ARG;

	voMJPEGDecProcess(hDec, &(pFront->InputData), &(pFront->OutData), &(pFront->OutPutInfo));

#ifdef LICENSEFILE
	decoder = (Jpeg_DecOBJ*)pFront->pDecCore;
	voCheckLibCheckVideo(decoder->phCheck, &(pFront->OutData), &(pFront->OutPutInfo.Format));
#endif

	*pOutput = pFront->OutData;
	*pOutPutInfo = pFront->OutPutInfo;


	return VO_ERR_NONE;
}

VO_S32 VO_API voGetMJPEGDecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VO_VIDEO_DECAPI *pMJPEGDec = pDecHandle;

	if(!pMJPEGDec)
		return VO_ERR_INVALID_ARG;

	pMJPEGDec->Init   = voMJPEGDecInit;
	pMJPEGDec->Uninit = voMJPEGDecUninit;
	pMJPEGDec->SetParam = voMJPEGDecSetParameter;
	pMJPEGDec->GetParam = voMJPEGDecGetParameter;
	pMJPEGDec->SetInputData = voMJPEGDecSetInputData;
	pMJPEGDec->GetOutputData = voMJPEGDecGetOutputData;

	return VO_ERR_NONE;

}