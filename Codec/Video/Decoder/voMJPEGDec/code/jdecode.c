#include "jdecoder.h"
#include "mem_align.h"


INT32	init_setup(Jpeg_DecOBJ *decoder)
{	
	JPEG_COMP_ITEM	*comptr;
	JPEG_PARA		*jpara;
	UINT32	  i, MblockSize;	

	jpara = &decoder->jpara;
	
	if(jpara->Ns == 1)  {
		jpara->MCU_width  = 8;
		jpara->MCU_heigth = 8;
		jpara->MCUBlock.blockNum = 1;
		comptr = jpara->cur_com_ptr[0];
		jpara->MCU_XNum = DIV(jpara->width * comptr->h_sample, jpara->max_h_sample * 8);
		jpara->MCU_YNum = DIV(jpara->heigth * comptr->v_sample, jpara->max_v_sample * 8);
	}
	else
	{
		jpara->MCU_width  = jpara->max_h_sample * 8;
		jpara->MCU_heigth = jpara->max_v_sample * 8;
		
		jpara->MCUBlock.blockNum = 0;
		for(i = 0; i < jpara->Ns; i++)
		{
			comptr = jpara->cur_com_ptr[i];
			MblockSize = comptr->h_sample * comptr->v_sample;
			jpara->MCUBlock.blockNum +=  MblockSize;
		}
		
		jpara->MCU_XNum = DIV(jpara->width,  jpara->MCU_width );
		jpara->MCU_YNum = DIV(jpara->heigth, jpara->MCU_heigth);
	}

	if(!jpara->progressMode)
	{
		jpara->MCUBlock.DC_pred[0] = jpara->MCUBlock.DC_pred[1] =
			jpara->MCUBlock.DC_pred[2] = 1024;
		
		if(InitHuffTab(decoder, &decoder->Huff_vlc, &decoder->dentropy, &jpara->huffum_tbl) < 0)
			return -1;
	}
	else
	{
		jpara->MCUBlock.DC_pred[0] = jpara->MCUBlock.DC_pred[1] =
		jpara->MCUBlock.DC_pred[2] = 0;
		
		if(InitpHuffTab(decoder) < 0)
			return -1;
	}

	return 0;
}

//INT32	JPEG_interlmcu_dec(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData)
//{
//	register UINT32	i, j, k;
//	UINT32	blockindex, h_index, v_index;
//	JPEG_MCU_BLOCK	*mcublock;
//	JPEG_COMP_ITEM	*cinfo;
//    JPEG_PARA		*jpara;
//	IMAGE_FIELD		*field;
//	_IDCT_Block8x8  idct_block8x8;
//	UINT8	*dst, *dstbk;
//
//	jpara = &decoder->jpara;
//	mcublock = &jpara->MCUBlock;
//	idct_block8x8 = decoder->idct_block;
//	field = &decoder->imagefield;
//	
//	for(i = decoder->Y_block; i < jpara->MCU_YNum; i++)
//	{
//#ifdef SUPPORT_BREAK
//		if(decoder->EnstepDec && i >= field->start_Y)
//		{
//			decoder->block_Count++;
//			if(decoder->block_Count > decoder->stepDec_num)
//			{
//				decoder->Y_block = i;
//				decoder->block_Count = 0;
//				return -2;
//			}
//		}
//		else if(i >= field->end_Y)
//		{
//			return 0;
//		}
//#endif
//		for(j = 0; j < jpara->MCU_XNum; j++)
//		{
//			if(i == field->end_Y - 1 && j >= field->end_X)
//				return 0;
//			memset(mcublock->WorkBlock[0], 0, sizeof(INT16)*JPEG_BLOCK_SIZE*mcublock->blockNum);
//			if(decode_MCU(decoder) < 0)
//				return -1;
//
//			if(j >= field->start_X && j < field->end_X && i >= field->start_Y && i < field->end_Y)	{
//				blockindex = 0;			
//				for(k = 0; k < jpara->Ns; k++)	{
//					cinfo = jpara->cur_com_ptr[k];					
//					dst = pOutData->Buffer[k]
//						+ cinfo->v_sample * decoder->addstep * pOutData->Stride[k] * (i - field->start_Y)
//						+ cinfo->h_sample * decoder->addstep * (j - field->start_X);
//					
//					dstbk = dst;
//					for(v_index = 0; v_index < cinfo->v_sample; v_index++)	{
//						dst = dstbk + pOutData->Stride[k] * v_index * decoder->addstep;					
//						for(h_index = 0; h_index < cinfo->h_sample; h_index++)	{
//							
//							(*idct_block8x8)(mcublock->WorkBlock[blockindex], dst, pOutData->Stride[k], NULL);							
//							dst += decoder->addstep;
//							blockindex++;
//						}
//					}
//				}
//			}
//		}
//	}
//	
//	return 0;
//}

/*huwei 20090726 normal chroma*/
static void JPEG_PLANAR444_to_PLANAR420(UINT8 *src, INT32 SrcStride, UINT8 *dst, INT32 DestStride)
{
	UINT32 i;

	for(i = 0; i < 4 ;i++ )
	{
		dst[0] = (src[0] + src[0 + SrcStride] + 1) >> 1;
		dst[1] = (src[2] + src[2 + SrcStride] + 1) >> 1;
		dst[2] = (src[4] + src[4 + SrcStride] + 1) >> 1;
		dst[3] = (src[6] + src[6 + SrcStride] + 1) >> 1;

		src += (SrcStride << 1);
		dst += DestStride;
	}

}
static void JPEG_PLANAR422_12_to_PLANAR420(UINT8 *src, INT32 SrcStride, UINT8 *dst, INT32 DestStride)
{
	UINT32 i;

	for(i = 0; i < 4 ; i++)
	{
		dst[0] = (src[0] + src[0 + SrcStride] + 1) >> 1;
		dst[1] = (src[1] + src[1 + SrcStride] + 1) >> 1;
		dst[2] = (src[2] + src[2 + SrcStride] + 1) >> 1;
		dst[3] = (src[3] + src[3 + SrcStride] + 1) >> 1;

		dst[4] = (src[4] + src[4 + SrcStride] + 1) >> 1;
		dst[5] = (src[5] + src[5 + SrcStride] + 1) >> 1;
		dst[6] = (src[6] + src[6 + SrcStride] + 1) >> 1;
		dst[7] = (src[7] + src[7 + SrcStride] + 1) >> 1;

		src += (SrcStride << 1);
		dst += DestStride;
	}

}
static void JPEG_PLANAR422_21_to_PLANAR420(UINT8 *src, INT32 SrcStride, UINT8 *dst, INT32 DestStride)
{
	UINT32 i;

	for(i = 0; i < 8 ; i++)
	{
		dst[0] = src[0];
		dst[1] = src[2];
		dst[2] = src[4];
		dst[3] = src[6];

		src += (SrcStride << 1);
		dst += DestStride;
	}

}
static void JPEG_PLANAR411_to_PLANAR420(UINT8 *src, INT32 SrcStride, UINT8 *dst, INT32 DestStride)
{
	UINT32 i;

	for(i = 0; i < 4 ; i++)
	{
		dst[0] = dst[1] = (src[0] + src[0 + SrcStride] + 1) >> 1;
		dst[2] = dst[3] = (src[1] + src[1 + SrcStride] + 1) >> 1;
		dst[4] = dst[5] = (src[2] + src[2 + SrcStride] + 1) >> 1;
		dst[6] = dst[7] = (src[3] + src[3 + SrcStride] + 1) >> 1;

		dst[8] = dst[9] = (src[4] + src[4 + SrcStride] + 1) >> 1;
		dst[10] = dst[11] = (src[5] + src[5 + SrcStride] + 1) >> 1;
		dst[12] = dst[13] = (src[6] + src[6 + SrcStride] + 1) >> 1;
		dst[14] = dst[15] = (src[7] + src[7 + SrcStride] + 1) >> 1;

		src += (SrcStride << 1);
		dst += DestStride;
	}

}

static void JPEG_PLANAR411V_to_PLANAR420(UINT8 *src, INT32 SrcStride, UINT8 *dst, INT32 DestStride)
{
	UINT32 i;

	for(i = 0; i < 4 ; i++)
	{
		dst[0] = dst[0 +DestStride] = (src[0] + src[1] + 1) >> 1;
		dst[1] = dst[1 +DestStride] = (src[2] + src[3] + 1) >> 1;
		dst[2] = dst[2 +DestStride] = (src[4] + src[5] + 1) >> 1;
		dst[3] = dst[3 +DestStride] = (src[6] + src[7] + 1) >> 1;

		src += SrcStride;
		dst += (DestStride << 1);
	}

}

INT32	JPEG_interlmcu_dec(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData)
{
	register UINT32	i, j, k;
	UINT32	blockindex, h_index, v_index;
	JPEG_MCU_BLOCK	*mcublock;
	JPEG_COMP_ITEM	*cinfo;
    JPEG_PARA		*jpara;
	IMAGE_FIELD		*field;
	_IDCT_Block8x8  idct_block8x8;
	UINT8	*dst, *dstbk;

	jpara = &decoder->jpara;
	mcublock = &jpara->MCUBlock;
	idct_block8x8 = decoder->idct_block;
	field = &decoder->imagefield;
	
	for(i = decoder->Y_block; i < jpara->MCU_YNum; i++)
	{
#ifdef SUPPORT_BREAK
		if(decoder->EnstepDec && i >= field->start_Y)
		{
			decoder->block_Count++;
			if(decoder->block_Count > decoder->stepDec_num)
			{
				decoder->Y_block = i;
				decoder->block_Count = 0;
				return -2;
			}
		}
		else if(i >= field->end_Y)
		{
			return 0;
		}
#endif
		for(j = 0; j < jpara->MCU_XNum; j++)
		{
			if(i == field->end_Y - 1 && j >= field->end_X)
				return 0;

			if(decoder->memoryOperator.Set)
			{
#ifdef VOJPEGFLAGE
				decoder->memoryOperator.Set(VO_INDEX_DEC_JPEG, mcublock->WorkBlock[0], 0, sizeof(INT16)*JPEG_BLOCK_SIZE*mcublock->blockNum);
#else
				decoder->memoryOperator.Set(VO_INDEX_DEC_MJPEG, mcublock->WorkBlock[0], 0, sizeof(INT16)*JPEG_BLOCK_SIZE*mcublock->blockNum);
#endif // VOJPEGFLAGE
				
			} 
			else
			{
				memset(mcublock->WorkBlock[0], 0, sizeof(INT16)*JPEG_BLOCK_SIZE*mcublock->blockNum);
			}
			
			if(decode_MCU(decoder) < 0)
				return -1;

			if(j >= field->start_X && j < field->end_X && i >= field->start_Y && i < field->end_Y)	{
				blockindex = 0;			
				for(k = 0; k < jpara->Ns; k++)	{
					cinfo = jpara->cur_com_ptr[k];					

					if(decoder->enableNormalChroma ||(0x00000011 == jpara->image_type)
						||(0x00111122 == jpara->image_type))/* enableNormalChroma or YUV420 format */
					{
						dst = pOutData->Buffer[k]
							+ cinfo->v_sample * decoder->addstep * pOutData->Stride[k] * (i - field->start_Y)
							+ cinfo->h_sample * decoder->addstep * (j - field->start_X);
					} 
					else
					{
						if (0 == k)
						{
							dst = pOutData->Buffer[k]
							+ cinfo->v_sample * decoder->addstep * pOutData->Stride[k] * (i - field->start_Y)
								+ cinfo->h_sample * decoder->addstep * (j - field->start_X);
						} 
						else
						{
							switch(jpara->image_type)
							{			
							case 0x00111111:/*VO_COLOR_YUV_PLANAR444*/
								dst = pOutData->Buffer[k]
								+ cinfo->v_sample * (decoder->addstep >> 1) * pOutData->Stride[k] * (i - field->start_Y)
									+ cinfo->h_sample * (decoder->addstep >> 1) * (j - field->start_X);
								break;
							case 0x00111121:/*VO_COLOR_YUV_PLANAR422_12*/
								dst = pOutData->Buffer[k]
								+ cinfo->v_sample * (decoder->addstep >> 1) * pOutData->Stride[k] * (i - field->start_Y)
									+ cinfo->h_sample * decoder->addstep * (j - field->start_X);
								break;
							case 0x00111112:/*VO_COLOR_YUV_PLANAR422_21*/
								dst = pOutData->Buffer[k]
								+ cinfo->v_sample * decoder->addstep * pOutData->Stride[k] * (i - field->start_Y)
									+ cinfo->h_sample * (decoder->addstep >> 1) * (j - field->start_X);
								break;
							case 0x00111141:/*VO_COLOR_YUV_PLANAR411*/
								dst = pOutData->Buffer[k]
								+ cinfo->v_sample * (decoder->addstep >> 1) * pOutData->Stride[k] * (i - field->start_Y)
									+ cinfo->h_sample * decoder->addstep * 2 * (j - field->start_X);
								break;
							case 0x00111114:/*VO_COLOR_YUV_PLANAR411V*/
								dst = pOutData->Buffer[k]
								+ cinfo->v_sample * 2 * decoder->addstep * pOutData->Stride[k] * (i - field->start_Y)
									+ cinfo->h_sample  * (decoder->addstep >> 1) * (j - field->start_X);
								break;
							default:
								return VO_ERR_INVALID_ARG;	
							}
						}
					}
					
					dstbk = dst;
					for(v_index = 0; v_index < cinfo->v_sample; v_index++)	{
						dst = dstbk + pOutData->Stride[k] * v_index * decoder->addstep;					
						for(h_index = 0; h_index < cinfo->h_sample; h_index++)	{
							
							if(decoder->enableNormalChroma ||(0x00000011 == jpara->image_type)
								||(0x00111122 == jpara->image_type))/* enableNormalChroma or YUV420 format */
							{
								(*idct_block8x8)(mcublock->WorkBlock[blockindex], dst, pOutData->Stride[k], NULL, 0);							
								dst += decoder->addstep;
								blockindex++;
							} 
							else
							{
								if (0 == k)
								{
									(*idct_block8x8)(mcublock->WorkBlock[blockindex], dst, pOutData->Stride[k], NULL, 0);							
									dst += decoder->addstep;
									blockindex++;
								} 
								else
								{						
									(*idct_block8x8)(mcublock->WorkBlock[blockindex], decoder->chromaBuffer, 8, NULL, 0);

									switch(jpara->image_type)
									{			
									case 0x00111111:/*VO_COLOR_YUV_PLANAR444*/
										JPEG_PLANAR444_to_PLANAR420(decoder->chromaBuffer, 8, dst, pOutData->Stride[k]);
										dst += 4;
										break;
									case 0x00111121:/*VO_COLOR_YUV_PLANAR422_12*/
										JPEG_PLANAR422_12_to_PLANAR420(decoder->chromaBuffer, 8, dst, pOutData->Stride[k]);
										dst += 8;
										break;
									case 0x00111112:/*VO_COLOR_YUV_PLANAR422_21*/
										JPEG_PLANAR422_21_to_PLANAR420(decoder->chromaBuffer, 8, dst, pOutData->Stride[k]);
										dst += 4;
										break;
									case 0x00111141:/*VO_COLOR_YUV_PLANAR411*/
										JPEG_PLANAR411_to_PLANAR420(decoder->chromaBuffer, 8, dst, pOutData->Stride[k]);
										dst += 16;
										break;
									case 0x00111114:/*VO_COLOR_YUV_PLANAR411V*/
										JPEG_PLANAR411V_to_PLANAR420(decoder->chromaBuffer, 8, dst, pOutData->Stride[k]);
										dst += 4;
										break;
									default:
										return VO_ERR_INVALID_ARG;	
									}

									blockindex++;
								}
							}
						}
					}
				}
			}
		}
	}
	
	return 0;
}

INT32	JPEG_interlmcu_dec8(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData)
{
	register UINT32	i, j, k;
	UINT32	blockindex, h_index, v_index;
	UINT32	zoom;
	UINT32 h_bloclk[3];
	UINT32 v_bloclk[3];
	JPEG_MCU_BLOCK	*mcublock;
	JPEG_COMP_ITEM	*cinfo;
    JPEG_PARA		*jpara;
	IMAGE_FIELD		*field;
	_IDCT_Block8x8  idct_block8x8;
	UINT8	*dst, *dstbk;

	jpara = &decoder->jpara;
	mcublock = &jpara->MCUBlock;
	idct_block8x8 = decoder->idct_block;
	field = &decoder->imagefield;
	zoom = decoder->zoomout/8;
	
	for(i = decoder->Y_block; i < jpara->MCU_YNum; i++)
	{
#ifdef SUPPORT_BREAK
		if(decoder->EnstepDec && i >= field->start_Y)
		{
			decoder->block_Count++;
			if(decoder->block_Count > decoder->stepDec_num)
			{
				decoder->Y_block = i;
				decoder->block_Count = 0;
				return -2;
			}
		}
		else if(i >= field->end_Y)
		{
			return 0;
		}
#endif
		h_bloclk[0] = h_bloclk[1] = h_bloclk[2] = 1;
		for(j = 0; j < jpara->MCU_XNum; j++)
		{
			if(decoder->memoryOperator.Set)
			{
#ifdef VOJPEGFLAGE
				decoder->memoryOperator.Set(VO_INDEX_DEC_JPEG, mcublock->WorkBlock[0], 0, sizeof(INT16)*JPEG_BLOCK_SIZE*mcublock->blockNum);
#else
				decoder->memoryOperator.Set(VO_INDEX_DEC_MJPEG, mcublock->WorkBlock[0], 0, sizeof(INT16)*JPEG_BLOCK_SIZE*mcublock->blockNum);
#endif // VOJPEGFLAGE
				
			} 
			else
			{
				memset(mcublock->WorkBlock[0], 0, sizeof(INT16)*JPEG_BLOCK_SIZE*mcublock->blockNum);
			}
			
			if(decode_MCU(decoder) < 0)
				return -1;

			if(j >= field->start_X && j < field->end_X && i >= field->start_Y && i < field->end_Y)
			{
				blockindex = 0;			
				for(k = 0; k < jpara->Ns; k++)	{
					cinfo = jpara->cur_com_ptr[k];
					v_bloclk[k] = jpara->blockCount_v[k];

					dst = pOutData->Buffer[k] 
						+ pOutData->Stride[k] * ((cinfo->v_sample  * (i - field->start_Y)) / zoom)  
						+ cinfo->h_sample * (j - field->start_X)/zoom;
					
					dstbk = dst;
					for(v_index = 0; v_index < cinfo->v_sample; v_index++) {
						v_bloclk[k]--;
						if(v_bloclk[k])	continue;

						v_bloclk[k] = zoom;						
						dst = dstbk + pOutData->Stride[k] * (v_index/zoom);					
						for(h_index = 0; h_index < cinfo->h_sample; h_index++)	{
							h_bloclk[k]--;
							if(h_bloclk[k])	continue;
							
							(*idct_block8x8)(mcublock->WorkBlock[blockindex], dst , pOutData->Stride[k], NULL, 0);
							
							dst++; blockindex++;													
							h_bloclk[k] = zoom;
						}
					}
				}
			}
		}
		jpara->blockCount_v[0] = v_bloclk[0];
		jpara->blockCount_v[1] = v_bloclk[1];
		jpara->blockCount_v[2] = v_bloclk[2];		
	}
	
	return 0;	
}


INT32	JPEG_progrmcu_dec(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData)
{
	register UINT32	i, j, k, l;
	UINT32	blockindex, h_index, v_index, readlen;
	UINT32	b_index, stride, nstride, nstride_k;
	IMAGE_FIELD		lfield;
	JPEG_MCU_BLOCK	*mcublock;
	JPEG_COMP_ITEM	*cinfo;
	JPEG_CACHE		*bcache;
	JPEG_STREAM		*bitstream;
    JPEG_PARA		*jpara;
	IMAGE_FIELD		*field;
	_IDCT_Block8x8  idct_block8x8;
	_decode_MCU		decode_MCU_prog;
	INT16			*dst16, *dstbk16;
	INT16			*src16, *block16;
	UINT8			ch;


	jpara = &decoder->jpara;
	mcublock = &jpara->MCUBlock;
	idct_block8x8 = decoder->idct_block;
	bcache = &decoder->b_cache;
	bitstream = &decoder->instream;
	field = &decoder->imagefield;

	for( ; ; )
	{
		switch(decoder->MCU_funct)
		{
		case 0:
			decode_MCU_prog = decode_MCU_DC_first;
			break;
		case 1:
			decode_MCU_prog = decode_MCU_AC_first;
			break;
		case 2:
			decode_MCU_prog = decode_MCU_DC_refine;
			break;
		case 3:
			decode_MCU_prog = decode_MCU_AC_refine;
			break;
		default:
			return -1;
		}

		if(jpara->Ns == 1)
		{
			cinfo = jpara->cur_com_ptr[0];
			lfield.start_X	= field->start_X * cinfo->h_sample;
			lfield.end_X	= field->end_X * cinfo->h_sample;
			lfield.start_Y	= field->start_Y * cinfo->v_sample;
			lfield.end_Y	= field->end_Y * cinfo->v_sample;				
		}
		else
		{
			lfield.start_X	= field->start_X;
			lfield.end_X	= field->end_X;
			lfield.start_Y	= field->start_Y;
			lfield.end_Y	= field->end_Y;	
		}
		
		for(i = 0; i < jpara->MCU_YNum; i++)
		{
			for(j = 0; j < jpara->MCU_XNum; j++)
			{
				if(decoder->memoryOperator.Set)
				{
#ifdef VOJPEGFLAGE
					decoder->memoryOperator.Set(VO_INDEX_DEC_JPEG, mcublock->WorkBlock[0], 0, sizeof(INT16)*JPEG_BLOCK_SIZE*mcublock->blockNum);
#else
					decoder->memoryOperator.Set(VO_INDEX_DEC_MJPEG, mcublock->WorkBlock[0], 0, sizeof(INT16)*JPEG_BLOCK_SIZE*mcublock->blockNum);
#endif // VOJPEGFLAGE
					
				} 
				else
				{
					memset(mcublock->WorkBlock[0], 0, sizeof(INT16)*JPEG_BLOCK_SIZE*mcublock->blockNum);
				}
				
				if(decoder->MCU_funct == 3)
				{
					if(decoder->Isfield)
						goto BEGIN_IDCT;
					cinfo = jpara->cur_com_ptr[0];
					b_index = cinfo->index;
					stride = decoder->widthstrid[b_index];

					dst16 = (INT16 *)(decoder->outBuffer[b_index]
							+ 8 * stride * (i - field->start_Y)
							+ 8 * (j - field->start_X) * 2);
					block16 = mcublock->WorkBlock[0];

					stride /= 2;
					for(l = 8; l; l--)	{
						*(block16 + 0) = *(dst16 + 0);
						*(block16 + 1) = *(dst16 + 1);
						*(block16 + 2) = *(dst16 + 2);
						*(block16 + 3) = *(dst16 + 3);
						*(block16 + 4) = *(dst16 + 4);
						*(block16 + 5) = *(dst16 + 5);
						*(block16 + 6) = *(dst16 + 6);
						*(block16 + 7) = *(dst16 + 7);
						
						dst16 += stride;
						block16 += 8;
					}					
				}

				if((*decode_MCU_prog)(decoder) < 0)
					return -1;
				
				if(j >= lfield.start_X && j < lfield.end_X && i >= lfield.start_Y && i < lfield.end_Y)
				{
					if(jpara->Ns == 3) 
					{
						blockindex = 0;			
						for(k = 0; k < jpara->Ns; k++)	{
							cinfo = jpara->cur_com_ptr[k];
							b_index = cinfo->index;
							stride = decoder->widthstrid[b_index];
							dst16 = (INT16 *)(decoder->outBuffer[b_index]
								+ cinfo->v_sample * 8 * stride * (i - field->start_Y)
								+ cinfo->h_sample * 8 * (j - field->start_X) * 2);

							stride /= 2;  dstbk16 = dst16;
							for(v_index = 0; v_index < cinfo->v_sample; v_index++)	{
								dst16 = dstbk16 + stride * v_index * 8;					
								for(h_index = cinfo->h_sample; h_index ; h_index--)	{									
									block16 = mcublock->WorkBlock[blockindex];
									if(decoder->MCU_funct == 0) {									
										*dst16 = *block16;
									}
									else if(decoder->MCU_funct == 2)
									{
										*dst16 |= *block16;									
									}
									blockindex++;	dst16 += 8;
								}
							}						
						}
					}
					else
					{
						for(k = 0; k < jpara->Ns; k++)	{
							cinfo = jpara->cur_com_ptr[k];
							b_index = cinfo->index;
							stride = decoder->widthstrid[b_index];
							dst16 = (INT16 *)(decoder->outBuffer[b_index]
								+ 8 * stride * (i - lfield.start_Y)
								+ 8 * (j - lfield.start_X) * 2);
							
							stride /= 2;
							src16 = dst16;
							if(decoder->MCU_funct == 0) {
								block16 = mcublock->WorkBlock[0];
								*src16 = *block16;
							}
							else if(decoder->MCU_funct == 2) {
									block16 = mcublock->WorkBlock[0];
									*src16 |= *block16;
							}
							else if(decoder->MCU_funct == 1 || decoder->MCU_funct == 3) {
								block16 = mcublock->WorkBlock[0];
								nstride_k = 0;
								for(l = jpara->Ss; l <= jpara->Se; l++)
								{
									nstride = l >> 3;
									if(nstride != nstride_k)
									{
										src16 = dst16 + stride * nstride;
										nstride_k = nstride;
									}
									
									*(src16 + (l & 7)) = *(block16 +  l);
								}						
							}
						}
					}
				}
			}
		}
		
		if(bcache->unread_marker == M_EOI)
			break;
		else
		{
			if(read_marker(decoder, bitstream, jpara, bcache->unread_marker) < 0)
				return -1;
			
			if(init_setup(decoder) < 0)
				return -1;

			INITCACHE(bcache);
			UPDATACACHE2(bitstream, bcache, 0);
			
			jpara->restarts_to_go = jpara->restart_intval;
		}
	}

BEGIN_IDCT:
	if(decoder->zoomout <= 8)
	{
		return JPEG_idct_dec(decoder, pOutData);
	}
	else
	{
		return JPEG_idct_dec8(decoder, pOutData);
	}
		
	return 0;		
}

//INT32	JPEG_idct_dec(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData)
//{
//	register UINT32	i, j, k, l;
//	UINT32	h_blocks, v_blocks, n;
//	UINT32	stride, stride_bk;
//	JPEG_MCU_BLOCK	*mcublock;
//	JPEG_COMP_ITEM	*cinfo;
//    JPEG_PARA		*jpara;
//	JPEG_QUANT_TBL	*qtbl;
//	IMAGE_FIELD		*field;
//	_IDCT_Block8x8  idct_block8x8;
//	INT16			*block16;
//	INT16			*src16, *srcbk16;
//	INT8			*quanraw8, *dst;
//	UINT8			*zig;
//
//	jpara = &decoder->jpara;
//	mcublock = &jpara->MCUBlock;
//	idct_block8x8 = decoder->idct_block;
//	field = &decoder->imagefield;
//	
//	for(k = 0, cinfo = jpara->compon; k < jpara->num_component; k++, cinfo++)	{
//		h_blocks = (field->end_X - field->start_X)*cinfo->h_sample;
//		v_blocks = (field->end_Y - field->start_Y)*cinfo->v_sample;	
//		qtbl = jpara->quant_tbl.Quant_tbl_ptrs[cinfo->quant_tbl_no];
//		stride_bk = decoder->widthstrid[k];
//
//		for(i = 0; i < v_blocks; i++)
//		{
//			for(j = 0 ; j < h_blocks; j++)
//			{					
//				stride = stride_bk;
//				dst = pOutData->Buffer[k]
//					+ decoder->addstep * pOutData->Stride[k] * i + decoder->addstep * j;				
//				src16 = (INT16 *)(decoder->outBuffer[k]
//					+ 8 * stride * i + 8 * j * 2);
//				
//				stride /= 2;
//				srcbk16 = src16;
//				block16 = mcublock->WorkBlock[0];
//				quanraw8 = (INT8*)SAMPLE_ALIGN8(qtbl->quantrawval);
//				zig = (UINT8*)zig_zag_tab_index;
//
//				for(l = 8; l; l--)	{
//					n = *zig++;
//					*(block16 + n) = *(srcbk16 + 0) * (*(quanraw8 + 0));
//					n = *zig++;
//					*(block16 + n) = *(srcbk16 + 1) * (*(quanraw8 + 1));
//					n = *zig++;
//					*(block16 + n) = *(srcbk16 + 2) * (*(quanraw8 + 2));
//					n = *zig++;
//					*(block16 + n) = *(srcbk16 + 3) * (*(quanraw8 + 3));
//					n = *zig++;
//					*(block16 + n) = *(srcbk16 + 4) * (*(quanraw8 + 4));
//					n = *zig++;
//					*(block16 + n) = *(srcbk16 + 5) * (*(quanraw8 + 5));
//					n = *zig++;
//					*(block16 + n) = *(srcbk16 + 6) * (*(quanraw8 + 6));
//					n = *zig++;
//					*(block16 + n) = *(srcbk16 + 7) * (*(quanraw8 + 7));
//
//					quanraw8 += 8;					
//					srcbk16 += stride;
//				}
//
//				*block16 += 1024;				
//				(*idct_block8x8)(block16, dst, pOutData->Stride[k], NULL);
//			}	
//		}
//	}
//	
//	return 0;		
//}

/*huwei 20090726 normal chroma*/
INT32	JPEG_idct_dec(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData)
{
	register UINT32	i, j, k, l;
	UINT32	h_blocks, v_blocks, n;
	UINT32	stride, stride_bk;
	JPEG_MCU_BLOCK	*mcublock;
	JPEG_COMP_ITEM	*cinfo;
	JPEG_PARA		*jpara;
	JPEG_QUANT_TBL	*qtbl;
	IMAGE_FIELD		*field;
	_IDCT_Block8x8  idct_block8x8;
	INT16			*block16;
	INT16			*src16, *srcbk16;
	INT8			*quanraw8, *dst;
	UINT8			*zig;

	jpara = &decoder->jpara;
	mcublock = &jpara->MCUBlock;
	idct_block8x8 = decoder->idct_block;
	field = &decoder->imagefield;

	for(k = 0, cinfo = jpara->compon; k < jpara->num_component; k++, cinfo++)	{
		h_blocks = (field->end_X - field->start_X)*cinfo->h_sample;
		v_blocks = (field->end_Y - field->start_Y)*cinfo->v_sample;	
		qtbl = jpara->quant_tbl.Quant_tbl_ptrs[cinfo->quant_tbl_no];
		stride_bk = decoder->widthstrid[k];

		if(decoder->enableNormalChroma && (0 != k))
		{
			pOutData->Stride[k] = pOutData->Stride[k]/2;
		}

		for(i = 0; i < v_blocks; i++)
		{
			for(j = 0 ; j < h_blocks; j++)
			{					
				stride = stride_bk;
				src16 = (INT16 *)(decoder->outBuffer[k]
				+ 8 * stride * i + 8 * j * 2);		

				if(decoder->enableNormalChroma ||(0x00000011 == jpara->image_type)
					||(0x00111122 == jpara->image_type))/* enableNormalChroma or YUV420 format */
				{
					dst = pOutData->Buffer[k]
					+ decoder->addstep * pOutData->Stride[k] * i + decoder->addstep * j;	
				} 
				else
				{
					if (0 == k)
					{
						dst = pOutData->Buffer[k]
						+ decoder->addstep * pOutData->Stride[k] * i + decoder->addstep * j;
					} 
					else
					{
						switch(jpara->image_type)
						{			
						case 0x00111111:/*VO_COLOR_YUV_PLANAR444*/
							dst = pOutData->Buffer[k]
							+ 4 * pOutData->Stride[k] * i + 4 * j;
							break;
						case 0x00111121:/*VO_COLOR_YUV_PLANAR422_12*/
							dst = pOutData->Buffer[k]
							+ 4 * pOutData->Stride[k] * i + decoder->addstep * j;
							break;
						case 0x00111112:/*VO_COLOR_YUV_PLANAR422_21*/
							dst = pOutData->Buffer[k]
							+ decoder->addstep * pOutData->Stride[k] * i + 4 * j;
							break;
						case 0x00111141:/*VO_COLOR_YUV_PLANAR411*/
							dst = pOutData->Buffer[k]
							+ 4 * pOutData->Stride[k] * i + decoder->addstep * 2 * j;
							break;
						case 0x00111114:/*VO_COLOR_YUV_PLANAR411V*/
							dst = pOutData->Buffer[k]
							+ 4 * pOutData->Stride[k] * i + decoder->addstep * 2 * j;
							break;
						default:
							return VO_ERR_INVALID_ARG;	
						}
					}
				}

				stride /= 2;
				srcbk16 = src16;
				block16 = mcublock->WorkBlock[0];
				quanraw8 = (INT8*)SAMPLE_ALIGN8(qtbl->quantrawval);
				zig = (UINT8*)zig_zag_tab_index;

				for(l = 8; l; l--)	{
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 0) * (*(quanraw8 + 0));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 1) * (*(quanraw8 + 1));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 2) * (*(quanraw8 + 2));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 3) * (*(quanraw8 + 3));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 4) * (*(quanraw8 + 4));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 5) * (*(quanraw8 + 5));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 6) * (*(quanraw8 + 6));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 7) * (*(quanraw8 + 7));

					quanraw8 += 8;					
					srcbk16 += stride;
				}

				*block16 += 1024;	

				if(decoder->enableNormalChroma ||(0x00000011 == jpara->image_type)
					||(0x00111122 == jpara->image_type))
				{
					(*idct_block8x8)(block16, dst, pOutData->Stride[k], NULL, 0);
				}
				else
				{
					if (0 == k)
					{
						(*idct_block8x8)(block16, dst, pOutData->Stride[k], NULL, 0);
					}
					else
					{
						(*idct_block8x8)(block16, decoder->chromaBuffer, 8, NULL, 0);

						switch(jpara->image_type)
						{			
						case 0x00111111:/*VO_COLOR_YUV_PLANAR444*/
							JPEG_PLANAR444_to_PLANAR420(decoder->chromaBuffer, 8, dst, pOutData->Stride[k]);
							break;
						case 0x00111121:/*VO_COLOR_YUV_PLANAR422_12*/
							JPEG_PLANAR422_12_to_PLANAR420(decoder->chromaBuffer, 8, dst, pOutData->Stride[k]);
							break;
						case 0x00111112:/*VO_COLOR_YUV_PLANAR422_21*/
							JPEG_PLANAR422_21_to_PLANAR420(decoder->chromaBuffer, 8, dst, pOutData->Stride[k]);
							break;
						case 0x00111141:/*VO_COLOR_YUV_PLANAR411*/
							JPEG_PLANAR411_to_PLANAR420(decoder->chromaBuffer, 8, dst, pOutData->Stride[k]);
							break;
						case 0x00111114:/*VO_COLOR_YUV_PLANAR411V*/
							JPEG_PLANAR411V_to_PLANAR420(decoder->chromaBuffer, 8, dst, pOutData->Stride[k]);
							break;
						default:
							return VO_ERR_INVALID_ARG;	
						}
					}
				}
			}	
		}
	}

	return 0;		
}

INT32	JPEG_idct_dec8(Jpeg_DecOBJ *decoder, VO_VIDEO_BUFFER *pOutData)
{
	register UINT32	i, j, k, l;
	UINT32	h_blocks, v_blocks, n;
	UINT32	stride, stride_bk;
	UINT32	zoom, step;
	JPEG_MCU_BLOCK	*mcublock;
	JPEG_COMP_ITEM	*cinfo;
    JPEG_PARA		*jpara;
	JPEG_QUANT_TBL	*qtbl;
	IMAGE_FIELD		*field;
	_IDCT_Block8x8  idct_block8x8;
	INT16			*block16;
	INT16			*src16, *srcbk16;
	INT8			*quanraw8, *zig, *dst_bk;
	UINT8			*dst;

	jpara = &decoder->jpara;
	mcublock = &jpara->MCUBlock;
	idct_block8x8 = decoder->idct_block;
	field = &decoder->imagefield;
	zoom = decoder->zoomout/8;
	step = decoder->zoomout;

	for(k = 0, cinfo = jpara->compon; k < jpara->num_component; k++, cinfo++)	{
		h_blocks = DIV((field->end_X - field->start_X)*cinfo->h_sample, zoom);
		v_blocks = DIV((field->end_Y - field->start_Y)*cinfo->v_sample, zoom);	
		qtbl = jpara->quant_tbl.Quant_tbl_ptrs[cinfo->quant_tbl_no];
		stride_bk = decoder->widthstrid[k];

		dst = pOutData->Buffer[k];
		for(i = 0; i < v_blocks; i++)
		{
			dst_bk = dst + pOutData->Stride[k] * i;
			for(j = 0 ; j < h_blocks; j++)
			{					
				stride = stride_bk;
				src16  = (INT16 *)(decoder->outBuffer[k]
					   + step * stride * i + step * j * 2);
				
				stride /= 2;
				srcbk16 = src16;
				block16 = mcublock->WorkBlock[0];
				quanraw8 = (INT8*)SAMPLE_ALIGN8(qtbl->quantrawval);
				zig = (UINT8*)zig_zag_tab_index;

				for(l = 8; l; l--)	{
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 0) * (*(quanraw8 + 0));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 1) * (*(quanraw8 + 1));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 2) * (*(quanraw8 + 2));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 3) * (*(quanraw8 + 3));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 4) * (*(quanraw8 + 4));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 5) * (*(quanraw8 + 5));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 6) * (*(quanraw8 + 6));
					n = *zig++;
					*(block16 + n) = *(srcbk16 + 7) * (*(quanraw8 + 7));

					quanraw8 += 8;					
					srcbk16 += stride;
				}

				*block16 += 1024;				
				(*idct_block8x8)(block16, dst_bk, pOutData->Stride[k], NULL, 0);

				dst_bk++;
			}	
		}
	}
	
	return 0;	
}
