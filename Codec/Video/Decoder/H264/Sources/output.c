
/*!
 ************************************************************************
 * \file output.c
 *
 * \brief
 *    Output an image and Trance support
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Karsten Suehring               <suehring@hhi.de>
 ************************************************************************
 */

#include "contributors.h"
#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#endif
#include "global.h"
#include "mbuffer.h"
#include "image.h"
#include "nalucommon.h"

void AvdLogBitmapFile(StorablePicture *p)
{

}
#if (DUMP_VERSION & DUMP_TIMESTAMP)
void DumpTimeStampOfTS(int decodedNum,int frameNum,int inStamp,int poc,int typeIPB,int idr_flag,int outTimeStamp)
{
	char type[3]={'P','B','I'};
	static int num=0;
	if(_dmpTsFp==NULL)
		return;
	
	if((num&255)==0)
	{
		fprintf(_dmpTsFp,"outNum		frameNum		inStamp			outStamp	poc	IPB	idr\n");
	}
	
		fprintf(_dmpTsFp,"%d		%d			%d			%d		%d	 %C	%d\n",
							num,		frameNum,	inStamp,	outTimeStamp,poc,type[typeIPB],idr_flag);
		fflush(_dmpTsFp);
		num++;
}
DumpTimeStampOfTS2(int num,int inStamp,
				   int	poc,int typeIPB ,int outStamp)
{
	char type[3]={'P','B','I'};
	if(_dmpTsFp==NULL)
		return;
	if(num==0)
	{
		fprintf(_dmpTsFp,"frameNum	inStamp		outStamp	poc	IPB	\n");
	}

	fprintf(_dmpTsFp,"%d		%d			%d			%d		%C	\n",
					 num,	inStamp,	outStamp,  poc,type[typeIPB]);
	fflush(_dmpTsFp);

}
#endif//(DUMP_VERSION & DUMP_TIMESTAMP)
extern int imgType[3];

/*!
 ************************************************************************
 * \brief
 *    Writes out a storable picture
 * \param p
 *    Picture to be written
 * \param p_out
 *    Output file
 ************************************************************************
 */
//avdNativeInt disableDeblock = 0; 

void write_picture(ImageParameters *img,StorablePicture *p)
{

	// TODO: add cropping as below;
	H264VdLibParam *params = img->vdLibPar;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPOCInfo* pocInfo = img->pocInfo;
	TIOInfo	*ioInfo = img->ioInfo;
	//DumpBufInfo(img,img->mbsProcessor[0]);
	if(p->is_output)
		return;

	if((p->error&&params->enableJump2I)//if error,but the Jump2I is not enabled, do not discard the frame
		||p->structure!=FRAME)
	{
		if(p->error||p->structure!=FRAME)
			AvdLog2(LL_INFO,"Drop the frame,poc=%d,num=%d,error=%d,structure=%d\n",p->poc,p->frame_num,p->error,p->structure);
		goto EXIT_OUTPUT;
	}
	if(params->enableOutputASAP)
	{
		if (p->poc!=0)
		{
			int diff =  img->dpb->last_output_poc-p->poc;
			if (diff>0)
			{
				AvdLog2(LL_INFO,"warn:output POC must be in ascending order(%d,%d)\n",img->dpb->last_output_poc,p->poc);
				if ((diff&1) && pocInfo->poc_interval==2)
				{
					AvdLog2(LL_INFO,"warn:change the pocInterval to 1");
					pocInfo->poc_interval= 1;
				}
			}
			
		} 
	}


	params->outBufArr[0] = p->plnY;
	params->outBufArr[1] = p->plnU;
	params->outBufArr[2] = p->plnV;
	params->outBufDataLen[0] = sizeInfo->height*sizeInfo->yPlnPitch;
	params->outBufDataLen[1] = params->outBufDataLen[2] = (params->outBufDataLen[0]>>2);
	
	if(img->active_sps->frame_cropping_flag)
	{
		seq_parameter_set_rbsp_t * sps = img->active_sps;
		avdUInt32 left   = SubWidthC[sps->chroma_format_idc] * sps->frame_cropping_rect_left_offset;
		avdUInt32 top    = SubHeightC[sps->chroma_format_idc]* sps->frame_cropping_rect_top_offset;
		avdNativeInt sizeOffset = sizeInfo->width*top+left;
		avdNativeInt sizeOffset2;// = (sizeInfo->width*top)/4+left/2;//(sizeInfo->width-left-right)*top;
		
		left   = sps->frame_cropping_rect_left_offset;
		top    = ( 2 - sps->frame_mbs_only_flag ) * sps->frame_cropping_rect_top_offset;
		sizeOffset2 = sizeInfo->width_cr*top + left;
		if(sizeOffset)
		{
			params->outBufArr[0]+=sizeOffset;
			params->outBufArr[1]+=sizeOffset2;
			params->outBufArr[2]+=sizeOffset2;
		}
		//params->sizeInfo.frameHeight = sizeInfo->height-top-bottom;
		//params->sizeInfo.frameWidth  = sizeInfo->width-left-right;
	}
	
	//params->numFrDec++;
#if ENABLE_OTHER_ALLOC
	if(params->customMem2&&params->customMem2->Unlock)
	{
		params->customMem2->Unlock(p->plnBuffer);
	}
#endif	
	
	{

		
		
				OutDataQueueItem item;
				item.outData.data_buf[0] = params->outBufArr[0];
				item.outData.data_buf[1] = params->outBufArr[1];
				item.outData.data_buf[2] = params->outBufArr[2];
				item.outData.stride[0] = sizeInfo->yPlnPitch;
				item.outData.stride[1] = 
				item.outData.stride[2] = sizeInfo->yPlnPitch/2;
				item.type	=	imgType[p->typeIPB];
				item.time	=	p->timestamp;//p->poc;//
				item.picture = p;
				item.outData.video_type = (VOVIDEOTYPE)p->poc;
				item.flag = 0;
#if TEST_DEINTERLACE
				if((params->optFlag&OPT_ENABLE_DEINTERLACE)
					//&&(
					//GetMbAffFrameFlag(img)||//for mbaff, deinterlace anyway
					//(img->active_sps->frame_mbs_only_flag==0&&p->used_for_reference)//for normal field,deinterlace on ref output frame
					//)
					)
				{
					item.flag |=VO_H264FLAG_DEINTERLACE;
				}
#endif
				PushOneOutData(img,&item);
				//AvdLog2(LL_INFO,"img->REALOutTimeStamp = %d\n",img->REALOutTimeStamp);
		//if(params->enableOutputASAP)
		{
#if ENABLE_TRACE
		
			{
				static int outToQueueNum = 0;
				char frameType[]={'P','B','I'};
				AvdLog2(LL_INOUT,"%d: @!@framenum_poc_type=%d_%d_%c,idr=%d\n",outToQueueNum++,p->pic_num,p->poc,frameType[p->typeIPB],p->idr_flag);
				//DumpBufInfo(img,img->mbsProcessor[0]);
			}
#endif //ENABLE_TRACE
			if(p->used_for_reference)
				pocInfo->prevPoc=p->poc;
		}
	  }
	
EXIT_OUTPUT:	
	if(img->dpb->last_output_poc!=VO_INT_MIN)
	{
		int isOdd = (p->poc-img->dpb->last_output_poc)&1;
		if(!isOdd&&pocInfo->poc_interval==1
			&&img->ioInfo->outNumber<20)//only check the first 20 output,because in some cases, the poc interval may change,so keep it as 1 is safe
		{
			AVD_ERROR_CHECK(img,"@!@poc interval is changed to 2\n",100);	
			pocInfo->poc_interval = 2;
		}
		/*
		if(isOdd&&pocInfo->poc_interval==2)
		{
			AVD_ERROR_CHECK(img,"@!@poc interval is changed to 1\n",100);	
			pocInfo->poc_interval = 1;
		}*/
	}
	img->dpb->last_output_poc = p->poc;
	p->is_output = 1;
	

}

/*!
 ************************************************************************
 * \brief
 *    Initialize output buffer for direct output
 ************************************************************************
 */
void init_out_buffer(ImageParameters *img)
{
	TIOInfo	*ioInfo = img->ioInfo;
	if(ioInfo->out_buffer==NULL)
		ioInfo->out_buffer = alloc_frame_store(img);
}

/*!
 ************************************************************************
 * \brief
 *    Uninitialize output buffer for direct output
 ************************************************************************
 */
void uninit_out_buffer(ImageParameters *img)
{
  TIOInfo	*ioInfo = img->ioInfo;
  FrameStore* out_buffer = (FrameStore*)ioInfo->out_buffer;
  free_frame_store(img,out_buffer);
  ioInfo->out_buffer=NULL;
}

/*!
 ************************************************************************
 * \brief
 *    Initialize picture memory with (Y:0,U:128,V:128)
 ************************************************************************
 */

/*!
 ************************************************************************
 * \brief
 *    Write out unpaired fields from output buffer.
 ************************************************************************
 */
void flush_direct_output(ImageParameters *img)
{
	TIOInfo	*ioInfo = img->ioInfo;
 FrameStore* out_buffer = (FrameStore*)ioInfo->out_buffer;
#if FEATURE_INTERLACE
  
  //write_unpaired_field(img,out_buffer); //this shoud be an error
  if (out_buffer->is_used&&out_buffer->is_used!=3)
  {
	  AVD_ERROR_CHECK2(img,"unpaired_field,this shoud be an error\n",100);
  }
  PutStorablePic(img,out_buffer->top_field);
  out_buffer->top_field = NULL;
  PutStorablePic(img,out_buffer->bottom_field);
  out_buffer->bottom_field = NULL;
  out_buffer->is_used = 0;
#endif //VOI_H264D_NON_BASELINE
  PutStorablePic(img,out_buffer->frame);
  out_buffer->frame = NULL;
}


/*!
 ************************************************************************
 * \brief
 *    Write a frame (from FrameStore)
 * \param fs
 *    FrameStore containing the frame
 ************************************************************************
 */
void write_stored_frame( ImageParameters *img,FrameStore *fs)
{
  // make sure no direct output field is pending
  flush_direct_output(img);

#if FEATURE_INTERLACE
  if (fs->is_used<3)
  {
   // write_unpaired_field(img,fs);
	 AVD_ERROR_CHECK2(img,"something wrong,unpaired_field,this should be an error\n",100);
  }
  else
  {
	if (!fs->frame)
		DpbCombineYUVFields(img,fs);
    write_picture(img,fs->frame);
  }
#else //VOI_H264D_NON_BASELINE
  if (fs->is_used == 3)
    write_picture(img,fs->frame);
#endif //VOI_H264D_NON_BASELINE

  fs->is_output = 1;
}

/*!
 ************************************************************************
 * \brief
 *    Directly output a picture without storing it in the DPB. Fields 
 *    are buffered before they are written to the file.
 * \param p
 *    Picture for output
 ************************************************************************
 */
void direct_output(ImageParameters *img,StorablePicture *p)
{
	TIOInfo	*ioInfo = img->ioInfo;
  FrameStore* out_buffer = (FrameStore*)ioInfo->out_buffer;
  if (p->structure==FRAME)
  {
    // we have a frame (or complementary field pair)
    // so output it directly
    flush_direct_output(img);
    write_picture (img,p);
	PutStorablePic(img,p);
    return;
  }

#if FEATURE_INTERLACE
  if (p->structure == TOP_FIELD)
  {
    if (out_buffer->is_used &1)
      flush_direct_output(img);
    out_buffer->top_field = p;
    out_buffer->is_used |= 1;
  }

  if (p->structure == BOTTOM_FIELD)
  {
    if (out_buffer->is_used &2)
      flush_direct_output(img);
    out_buffer->bottom_field = p;
    out_buffer->is_used |= 2;
  }

  if (out_buffer->is_used == 3)
  {
	// TBD: need to combine YUV only, make frame motion info only if necessary;
	// we have both fields, so output them
	DpbCombineYUVFields(img,out_buffer);
	out_buffer->frame->poc = out_buffer->top_field->poc;
    write_picture (img,out_buffer->frame);

    PutStorablePic(img,out_buffer->frame);
    out_buffer->frame = NULL;
    PutStorablePic(img,out_buffer->top_field);
    out_buffer->top_field = NULL;
    PutStorablePic(img,out_buffer->bottom_field);
    out_buffer->bottom_field = NULL;
    out_buffer->is_used = 0;
  }
#endif //VOI_H264D_NON_BASELINE
}
void CleanOutDataQueue(ImageParameters *img)
{
	int i=0;
	TIOInfo	*ioInfo = img->ioInfo;
	for (;i<MAX_OUTDATA_QUEUE_SIZE;i++)
	{
		ioInfo->outDataQueue.queue[i].type=0;
	}
	ioInfo->outDataQueue.frameNum=
		ioInfo->outDataQueue.writeIndex = ioInfo->outDataQueue.readIndex = 0;
}
#define FRAME_TYPE_ADJUST 4
int IsThereOtherOutput(ImageParameters *img)
{
	//int i=0;
	int result=img->ioInfo->outDataQueue.frameNum;

	return result;
}
void PopOneOutData(ImageParameters *img,OutDataQueueItem* item)
{
	int i=0;
	OutDataQueue* pOutDataQueue=&img->ioInfo->outDataQueue;
	if(pOutDataQueue->frameNum<=0)
		return;
	i=pOutDataQueue->readIndex++;
	if(pOutDataQueue->readIndex==MAX_OUTDATA_QUEUE_SIZE)
		pOutDataQueue->readIndex=0;
	pOutDataQueue->frameNum--;
	memcpy(&item->outData,&pOutDataQueue->queue[i].outData,sizeof(VOCODECVIDEOBUFFER));
	item->type		= pOutDataQueue->queue[i].type-FRAME_TYPE_ADJUST;
	pOutDataQueue->queue[i].type	=	0;
	item->time		= pOutDataQueue->queue[i].time;
	item->flag		= pOutDataQueue->queue[i].flag;
	pOutDataQueue->queue[i].time	=	0;
	item->picture   = pOutDataQueue->queue[i].picture;
	//AvdLog2(LL_INFO,"T35_pop_%d,%X\n",i,item->picture);
	pOutDataQueue->queue[i].picture = NULL;
	//AvdLog2(LL_INFO,"outPOC2=%d",item->outData.video_type);
}
void  PushOneOutData(ImageParameters *img,OutDataQueueItem* item)
{
	int i=0;
	TIOInfo	*ioInfo = img->ioInfo;
	OutDataQueue* pOutDataQueue=&ioInfo->outDataQueue;

	i=pOutDataQueue->writeIndex++;
	if(pOutDataQueue->writeIndex==MAX_OUTDATA_QUEUE_SIZE)
		pOutDataQueue->writeIndex=0;
	pOutDataQueue->frameNum++;
	if (pOutDataQueue->frameNum>MAX_OUTDATA_QUEUE_SIZE)
	{
		avd_error(img,"i==MAX_OUTDATA_QUEUE_SIZE",100);
		return;
	}
	pOutDataQueue->queue[i].type=item->type+FRAME_TYPE_ADJUST;
	pOutDataQueue->queue[i].time = item->time;
	pOutDataQueue->queue[i].flag = item->flag;
	pOutDataQueue->queue[i].picture = item->picture;
	//AvdLog2(LL_INFO,"T35_push_%d,%X\n",i,item->picture);
	memcpy(&pOutDataQueue->queue[i].outData,&item->outData,sizeof(VOCODECVIDEOBUFFER));
	return;


}
/*!
************************************************************************
* \brief
*    Write out not paired direct output fields. A second empty field is generated
*    and combined into the frame buffer.
* \param fs
*    FrameStore that contains a single field
************************************************************************
*/
#if FEATURE_INTERLACE
void write_unpaired_field(ImageParameters *img,FrameStore* fs)
{
#if 0 //not used any longer
	StorablePicture *p;
	//AVD_ERROR_CHECK2(img,"disable write_unpaired_field\n",100);
	if(fs->is_used==0)
		return;
	if(fs->is_used==3)
	{
		AVD_ERROR_CHECK2(img,"write_unpaired_field: cur is not frame\n",100)
		return;
	}
	if(fs->is_used &1)
	{
		// we have a top field
		// construct an empty bottom field
		p = fs->top_field;
		// use I_SLCIE to avoid used motionInfo memory;
		fs->bottom_field =(StorablePicture*) alloc_storable_picture(img,BOTTOM_FIELD, I_SLICE, p->size_x, 
			p->size_y, p->size_x_cr, p->size_y_cr, AVC_FALSE);CHECK_ERR_RET_VOID
			//clear_picture(img,fs->bottom_field);//TBD
		DpbCombineYUVFields(img,fs);
		write_picture (img,fs->frame);
	}

	if(fs->is_used &2)
	{
		// we have a bottom field
		// construct an empty top field
		p = fs->bottom_field;
		// use I_SLCIE to avoid used motionInfo memory;
		fs->top_field = (StorablePicture*) alloc_storable_picture(img,TOP_FIELD, I_SLICE, p->size_x, p->size_y, 
			p->size_x_cr, p->size_y_cr, AVC_FALSE);CHECK_ERR_RET_VOID
			// BUG: should be fs->top_field
			//clear_picture(img,fs->bottom_field);//TBD
		DpbCombineYUVFields(img,fs);
		write_picture (img,fs->frame);
	}
	fs->is_used=3;
#endif
}
#endif//