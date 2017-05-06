/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#include	"mem_align.h"
#include	"jdecoder.h"
#include	"voMJPEG.h"

//#define LOG_ENABLE 1
#ifdef LOG_ENABLE
//#define ANDROID_LOG 1
#ifdef VOANDROID
#define LOG_FILE "/data/local/log.txt"
#define LOG_TAG "VOPlayer"
#include <utils/Log.h>
#  define __VOLOG(fmt, args...) ({LOGD("->%d: %s(): " fmt, __LINE__, __FUNCTION__, ## args);})
#define __VOLOGPRT(fmt, args...)  printf("->%d: %s(): " fmt, __LINE__, __FUNCTION__, ## args);
#  define __VOLOGFILE(fmt, args...) ({FILE *fp =fopen(LOG_FILE, "a"); fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, ## args); fclose(fp);})
#elif VOWINXP
#define LOG_FILE "D:/log.txt"
#define LOG_TAG "VOPlayer"
#include "stdio.h"
#define __VOLOG(fmt, ...)  printf("[ %s ]->%d: %s(): "fmt"\n",LOG_TAG,__LINE__,__FUNCTION__,__VA_ARGS__);
#define __VOLOGFILE(fmt, ...) {FILE *fp =fopen(LOG_FILE, "a");fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, __VA_ARGS__); fclose(fp);}
#endif
#else
#define __VOLOGFILE
#define __VOLOG
#define __VOLOGPRT
#endif



VOMJPEGDECRETURNCODE	Initpicture(Jpeg_DecOBJ * decoder)
{
	UINT16	i, k;
	UINT32	MblockSize = 0;
	UINT8*	block_buf = NULL;
	UINT32	max_v_sample = 1;
	UINT32	max_h_sample = 1;
	JPEG_COMP_ITEM* comptr;
	JPEG_PARA *jpara;

	jpara = &decoder->jpara;

	if(jpara->num_component > JPEG_MAX_COLOR || jpara->num_component==0 ) //zou212
	{
		return VO_ERR_DEC_MJPEG_UNSUPPORT_FEATURE;
	}	

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

		if(jpara->MCUBlock.blockNum>JPEG_MAX_BLOCKS)
		{
			return -1;
		}
		
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
		return VO_ERR_DEC_MJPEG_UNSUPPORT_FEATURE;
	
	if(decoder->pre_BlockNum < jpara->MCUBlock.blockNum)
	{
		/*if (decoder->memoryOperator.Free)
		{
			decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, jpara->MCUBlock.WorkBlock[0]);
		} 
		else
		{
			mem_free(jpara->MCUBlock.WorkBlock[0]);
		}*/
        voMemFree(decoder,jpara->MCUBlock.WorkBlock[0]);

		
		for(i = 0; i < JPEG_MAX_BLOCKS; i++)
		{
			jpara->MCUBlock.WorkBlock[i] = NULL;
		}
		
		decoder->pre_BlockNum = jpara->MCUBlock.blockNum;		
		MblockSize = sizeof(INT16) * JPEG_BLOCK_SIZE;
		/*if (decoder->memoryOperator.Alloc)
		{
			VO_MEM_INFO MemInfo;

			MemInfo.Flag = 0;
			MemInfo.Size = decoder->pre_BlockNum * MblockSize;
			decoder->memoryOperator.Alloc(VO_INDEX_DEC_MJPEG, &MemInfo);
			block_buf = (UINT8 *)MemInfo.VBuffer;
		} 
		else
		{
			block_buf = (UINT8 *)mem_malloc(decoder->pre_BlockNum * MblockSize, DCACHE_ALIGN_NUM);
		}*/
		block_buf = voMemMalloc(decoder, decoder->pre_BlockNum * MblockSize, DCACHE_ALIGN_NUM);
		if(block_buf == NULL)
			return VO_ERR_OUTOF_MEMORY;
		for(i = 0; i < decoder->pre_BlockNum; i++)
		{
			jpara->MCUBlock.WorkBlock[i] = (INT16 *)(block_buf + MblockSize*i);
		}			
	}		
	
	return VO_ERR_NONE;
}

VOMJPEGDECRETURNCODE	UpdateParameter(Jpeg_DecOBJ * decoder, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutFormat,UINT32 progressMode)
{
	UINT16	i;
	UINT32	yuv_width[3], totalwidth;  
	UINT32	yuv_height[3], totalheight;
	UINT32	yuv_size[3] = {0}, totalsize;
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
					return VO_ERR_DEC_MJPEG_UNSUPPORT_FEATURE;
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

	if(!decoder->enableNormalChroma && !progressMode)
	{
		yuv_width[1] = yuv_width[2] = yuv_width[0]/2;/*huwei 20090725 normal chroma*/
		yuv_size[1]  = yuv_size[2]  = yuv_size[0]/4;
	}
	if(decoder->memoryShare.Init)/*huwei 20090923 memory share*/
	{
		if(decoder->imagedsize[0] < yuv_size[0]){
			VO_MEM_VIDEO_INFO VideoMem;

			decoder->imagedsize[0] = yuv_size[0];
			decoder->imagedsize[1] = yuv_size[1];
			decoder->imagedsize[2] = yuv_size[2];

			if(NULL != decoder->outBuffer[0])
			{
				decoder->memoryShare.Uninit(VO_INDEX_DEC_MJPEG);

				decoder->outBuffer[0] = NULL;
				decoder->outBuffer[1] = NULL;
				decoder->outBuffer[2] = NULL;      

			}

			VideoMem.Stride = yuv_width[0];
			VideoMem.Height = yuv_height[0];
			VideoMem.FrameCount = 1;
			VideoMem.ColorType = VO_COLOR_YUV_PLANAR420;

			decoder->memoryShare.Init(VO_INDEX_DEC_MJPEG, &VideoMem);

			decoder->outBuffer[0] = (UINT8 *)(VideoMem.VBuffer[0].Buffer[0]);
			decoder->outBuffer[1] = (UINT8 *)(VideoMem.VBuffer[0].Buffer[1]);
			decoder->outBuffer[2] = (UINT8 *)(VideoMem.VBuffer[0].Buffer[2]);

			decoder->widthstrid[0] = VideoMem.VBuffer[0].Stride[0];
			decoder->widthstrid[1] = VideoMem.VBuffer[0].Stride[1];
			decoder->widthstrid[2] = VideoMem.VBuffer[0].Stride[2];

			memset(decoder->outBuffer[0], 0, decoder->widthstrid[0]*yuv_height[0]);
			memset(decoder->outBuffer[1], 0, decoder->widthstrid[1]*yuv_height[1]);
			memset(decoder->outBuffer[2], 0, decoder->widthstrid[2]*yuv_height[2]);

		}
	} 
	else
	{
		for(i = 0; i < JPEG_MAX_COLOR; i++)
		{
			if(decoder->imagedsize[i] < yuv_size[i])
			{
				/*if(decoder->memoryOperator.Free)
				{
					decoder->memoryOperator.Free(VO_INDEX_DEC_MJPEG, decoder->outBuffer[i]);
				} 
				else
				{
					mem_free(decoder->outBuffer[i]);
				}*/
                voMemFree(decoder,decoder->outBuffer[i]);
				decoder->imagedsize[i] = yuv_size[i];

                decoder->outBuffer[i] = voMemMalloc(decoder,yuv_size[i], DCACHE_ALIGN_NUM);

				/*if(decoder->memoryOperator.Alloc)
				{
					VO_MEM_INFO MemInfo;

					MemInfo.Flag = 0;
					MemInfo.Size = yuv_size[i];
					decoder->memoryOperator.Alloc(VO_INDEX_DEC_MJPEG, &MemInfo);
					decoder->outBuffer[i] = (UINT8 *)MemInfo.VBuffer;
				} 
				else
				{
					decoder->outBuffer[i] = (UINT8 *)mem_malloc(yuv_size[i],  DCACHE_ALIGN_NUM);
				}*/

				if(decoder->outBuffer[i] == NULL)
					return VO_ERR_OUTOF_MEMORY; 		
			}

			decoder->widthstrid[i] = yuv_width[i];

            //voMemSet(decoder,void *mem_ptr, INT32 size, VO_U8 value);

			if(decoder->memoryOperator.Set)
			{
				decoder->memoryOperator.Set(VO_INDEX_DEC_MJPEG,decoder->outBuffer[i], 0, yuv_size[i]);
			} 
			else
			{
				memset(decoder->outBuffer[i], 0, yuv_size[i]);
			}
		}
	}

	
	if(jpara->num_component == 1)
	{
		if(decoder->memoryOperator.Set)
		{
			decoder->memoryOperator.Set(VO_INDEX_DEC_MJPEG, decoder->outBuffer[1], 128, yuv_size[1]);
			decoder->memoryOperator.Set(VO_INDEX_DEC_MJPEG, decoder->outBuffer[2], 128, yuv_size[2]);
		} 
		else
		{
			memset(decoder->outBuffer[1], 128, yuv_size[1]);
			memset(decoder->outBuffer[2], 128, yuv_size[2]);
		}
	}

	if(pOutData)
	{
		if(decoder->enableNormalChroma)
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
			default:
				pOutData->ColorType = jpara->image_type;	
			}
		} 
		else
		{
			pOutData->ColorType = VO_COLOR_YUV_PLANAR420;/*huwei 20090726 normal chroma*/
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
		pOutFormat->Format.Type   = VO_VIDEO_FRAME_I;
	}

	return VO_ERR_NONE;
}

