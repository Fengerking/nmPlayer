#include "global.h"
#include "mbuffer.h"
#include "memalloc.h"
#define ROBUST_CHECK_BUF 1
#if !BUILD_WITHOUT_C_LIB
#include <memory.h>
#endif//BUILD_WITHOUT_C_LIB
#define ENABLE_MO_ALWAYS FEATURE_INTERLACE
#if FEATURE_BFRAME
void ResetMVRefBuf(ImageParameters *img,AVDStoredPicMotionInfo *motionInfo)
{
	// skipped mv are not updated either;
	int size = (img->dec_picture->size_x>>2);
	int size2 = motionInfo->alignedWidth8x8 * sizeof(avdUInt8) / sizeof(avdInt32);
	AVD_ALIGNED32_MEMSET(&motionInfo->mv0[0][0], 0, size);
	AVD_ALIGNED32_MEMSET(&motionInfo->mv0[1][0], 0, size);
	AVD_ALIGNED32_MEMSET(&motionInfo->mv0[2][0], 0, size);
	AVD_ALIGNED32_MEMSET(&motionInfo->mv0[3][0], 0, size);
	
	AVD_ALIGNED32_MEMSET(&motionInfo->ref_idx0[0][0], AVD_INVALID_REF_IDX_32, size2);
	AVD_ALIGNED32_MEMSET(&motionInfo->ref_idx0[1][0], AVD_INVALID_REF_IDX_32, size2);

	if(img->type==B_SLICE)
	{

		AVD_ALIGNED32_MEMSET(&motionInfo->mv1[0][0], 0, size);
		AVD_ALIGNED32_MEMSET(&motionInfo->mv1[1][0], 0, size);
		AVD_ALIGNED32_MEMSET(&motionInfo->mv1[2][0], 0, size);
		AVD_ALIGNED32_MEMSET(&motionInfo->mv1[3][0], 0, size);
	
		AVD_ALIGNED32_MEMSET(&motionInfo->ref_idx1[0][0], AVD_INVALID_REF_IDX_32, size2);
		AVD_ALIGNED32_MEMSET(&motionInfo->ref_idx1[1][0], AVD_INVALID_REF_IDX_32, size2);

	}
}

#endif//FEATURE_BFRAME
AVDStoredPicMotionInfo *alloc_motion_info(ImageParameters *img,SliceType typeIPB,PictureStructure structure )
{

	AVDStoredPicMotionInfo *motionInfo;// = (AVDStoredPicMotionInfo *)voH264AlignedMalloc(1, sizeof(AVDStoredPicMotionInfo));
	TSizeInfo	*sizeInfo = img->sizeInfo;
	avdNativeInt width4x4Mb  = (sizeInfo->width>>2);
	avdNativeInt height4x4Mb = (sizeInfo->height>>2);
	avdNativeInt moving4x4Width, list, alignedWidth8x8;
	avdInt32 buf,i;
#if FEATURE_INTERLACE	
	if(img->field_pic_flag&&structure==FRAME)
		height4x4Mb*=2;
#endif//#if FEATURE_INTERLACE	
	if (img->dpb->actualAllocatedMotion == MAX_REUSED_BUFFER*AVD_MOTION_INFO_SIZE)
	{
		avd_error(img,"abnormal:img->dpb->actualAllocatedMotion is over MAX_SIZE",100);
		return img->dpb->actualAllocatedMotionBuf[MAX_REUSED_BUFFER-1];
	}
	motionInfo = (AVDStoredPicMotionInfo *)voH264AlignedMalloc(img,1, sizeof(AVDStoredPicMotionInfo));CHECK_ERR_RET_NULL
	AvdLog2(LL_INFO, "Alloc motion_info,%d,%X,type=%d\n",img->dpb->actualAllocatedMotion,motionInfo,img->type);
	img->dpb->actualAllocatedMotionBuf[img->dpb->actualAllocatedMotion] = motionInfo;
	img->dpb->actualAllocatedMotion++;
	alignedWidth8x8 = (width4x4Mb>>1);
	// width 8x8 may not align on 4;
	alignedWidth8x8 += (alignedWidth8x8&3) ? (4 - ((alignedWidth8x8&3))) : 0;
	motionInfo->alignedWidth8x8 = alignedWidth8x8;
	list = (typeIPB == P_SLICE) ? 1 : 2;

	if(img->notSaveAllMV)
	{
		// just need one MB row plus top 4x4;
		

		if(img->mvRefIdxBuf==NULL)
		{
			i = 5 * (sizeof (AVDMotionVector *) + width4x4Mb * sizeof(AVDMotionVector))
				+ 3 * (sizeof (avdUInt8 *) + alignedWidth8x8 * sizeof(avdUInt8));	
#if FEATURE_BFRAME
			i <<= 1;
#endif //FEATURE_BFRAME
			if ((img->mvRefIdxBuf = (avdUInt8 *)voH264AlignedMalloc(img,9, i)) == NULL)
				no_mem_exit("motionInfo:allocMVnRef");
		}
		CHECK_ERR_RET_NULL
			// [-1 to 3]; [-1] for neighbor B;
		motionInfo->mv0     = (AVDMotionVector **)img->mvRefIdxBuf + 1;
		motionInfo->mv0[0]  = (AVDMotionVector *)(motionInfo->mv0 + 4);
		motionInfo->mv0[1]  = motionInfo->mv0[0] + width4x4Mb;
		motionInfo->mv0[2]  = motionInfo->mv0[1] + width4x4Mb;
		motionInfo->mv0[3]  = motionInfo->mv0[2] + width4x4Mb;
		motionInfo->mv0[-1] = motionInfo->mv0[3] + width4x4Mb;

		// [-1 to 1]; [-1] for neighbor B;
		motionInfo->ref_idx0 = (avdUInt8 **)((avdUInt8 *)(motionInfo->mv0[-1] + width4x4Mb) + 
			sizeof(avdUInt8 *));
		motionInfo->ref_idx0[0]  = (avdUInt8 *)(motionInfo->ref_idx0 + 2);
		motionInfo->ref_idx0[1]  = motionInfo->ref_idx0[0] + alignedWidth8x8;
		motionInfo->ref_idx0[-1] = motionInfo->ref_idx0[1] + alignedWidth8x8;
#if FEATURE_BFRAME
		if (list == 2){
			// [-1 to 3]; [-1] for neighbor B;
			motionInfo->mv1     = (AVDMotionVector **)(motionInfo->ref_idx0[-1] + alignedWidth8x8) + 1;
			motionInfo->mv1[0]  = (AVDMotionVector *)(motionInfo->mv1 + 4);
			motionInfo->mv1[1]  = motionInfo->mv1[0] + width4x4Mb;
			motionInfo->mv1[2]  = motionInfo->mv1[1] + width4x4Mb;
			motionInfo->mv1[3]  = motionInfo->mv1[2] + width4x4Mb;
			motionInfo->mv1[-1] = motionInfo->mv1[3] + width4x4Mb;

			// [-1 to 1]; [-1] for neighbor B;
			motionInfo->ref_idx1 = (avdUInt8 **)((avdUInt8 *)(motionInfo->mv1[-1] + width4x4Mb) + 
				sizeof(avdUInt8 *));
			motionInfo->ref_idx1[0]  = (avdUInt8 *)(motionInfo->ref_idx1 + 2);
			motionInfo->ref_idx1[1]  = motionInfo->ref_idx1[0] + alignedWidth8x8;
			motionInfo->ref_idx1[-1] = motionInfo->ref_idx1[1] + alignedWidth8x8;

			// since ref_idx1 is not updated in skipped MB;
			i = alignedWidth8x8 * sizeof(avdUInt8) / sizeof(avdInt32);
			AVD_ALIGNED32_MEMSET(&motionInfo->ref_idx0[0][0], AVD_INVALID_REF_IDX_32, i);
			AVD_ALIGNED32_MEMSET(&motionInfo->ref_idx0[1][0], AVD_INVALID_REF_IDX_32, i);
			AVD_ALIGNED32_MEMSET(&motionInfo->ref_idx1[0][0], AVD_INVALID_REF_IDX_32, i);
			AVD_ALIGNED32_MEMSET(&motionInfo->ref_idx1[1][0], AVD_INVALID_REF_IDX_32, i);
		}
#endif//FEATURE_BFRAME
	}
	else
	{
		get_mem2D (img,(avdUInt8 ***)&(motionInfo->ref_idx0), height4x4Mb>>1, alignedWidth8x8);
		get_mem2D (img,(avdUInt8 ***)&(motionInfo->mv0), height4x4Mb, 
			width4x4Mb * sizeof(AVDMotionVector));
	}
	
#if FEATURE_BFRAME
	// 8 4x4 blocks using one avdUInt8, the rest use another one avdUInt8;
	moving4x4Width = (width4x4Mb&0x7) ? (width4x4Mb>>3) + 1 : (width4x4Mb>>3);
	// make it align on 4;
	moving4x4Width += (moving4x4Width&3) ? (4 - ((moving4x4Width&3))) : 0;
	get_mem2D (img,&(motionInfo->moving4x4), height4x4Mb, moving4x4Width);

	if (img->notSaveAllMV==0&&list == 2){
		get_mem2D (img,(avdUInt8 ***)&(motionInfo->ref_idx1), height4x4Mb>>1, alignedWidth8x8);
		get_mem2D (img,(avdUInt8 ***)&(motionInfo->mv1), height4x4Mb, 
			width4x4Mb * sizeof(AVDMotionVector));
	}

#endif //VOI_H264D_NON_BASELINE
	motionInfo->height4x4Mb = height4x4Mb;

	return motionInfo;
}

AVDStoredPicMotionInfo *get_motion_info(ImageParameters *img,SliceType typeIPB,PictureStructure structure)
{
	//SliceType typeIPB = s->typeIPB;
	DecodedPictureBuffer *dpb = img->dpb;
	AVDStoredPicMotionInfo* motionInfo;

	if (typeIPB == I_SLICE)
	{
#if !ENABLE_MO_ALWAYS
		return NULL;
#else
		typeIPB = P_SLICE;//use P_slice for I_slice by default
#endif
	}

	if (structure==FRAME){
		if (!dpb->numReusedFrMotionInfo[typeIPB])
			return alloc_motion_info (img,typeIPB,structure);

		dpb->numReusedFrMotionInfo[typeIPB]--;
		motionInfo =  dpb->reusedFrMotionInfo[typeIPB][dpb->numReusedFrMotionInfo[typeIPB]];
		//AvdLog2(LL_INFO, "Get motion_info,type=%d,resued=%d,%X\n",typeIPB,dpb->numReusedFrMotionInfo[typeIPB],motionInfo);

		return motionInfo;
	}

#if FEATURE_INTERLACE
	if (!dpb->numReusedFldMotionInfo[typeIPB])
		return alloc_motion_info (img,typeIPB,structure);

	dpb->numReusedFldMotionInfo[typeIPB]--;
	return dpb->reusedFldMotionInfo[typeIPB][dpb->numReusedFldMotionInfo[typeIPB]];
#endif //FEATURE_INTERLACE
	return NULL;
}

void FreeMotionInfo(ImageParameters *img,AVDStoredPicMotionInfo *motionInfo, SliceType typeIPB)
{
	avdNativeInt list = (typeIPB == P_SLICE) ? 1 : 2;
	if(img->notSaveAllMV)
	{
		if(img->mvRefIdxBuf)
			SafevoH264AlignedFree(img,img->mvRefIdxBuf);//motionInfo->allocMVnRef);
	}
	else
	{
		free_mem2D(img,(avdUInt8 **)motionInfo->ref_idx0);
		free_mem2D(img,(avdUInt8 **)motionInfo->mv0);
	}


#if FEATURE_BFRAME

	if (list == 2&&img->notSaveAllMV==0){
		free_mem2D(img,(avdUInt8 **)motionInfo->mv1);
		free_mem2D(img,(avdUInt8 **)motionInfo->ref_idx1);
	}

	if (motionInfo->moving4x4) {
		free_mem2D(img,motionInfo->moving4x4);
		motionInfo->moving4x4=NULL;
	}
#endif //FEATURE_BFRAME
	SafevoH264AlignedFree(img,motionInfo);
}

void PutMotionInfo(ImageParameters *img,AVDStoredPicMotionInfo *mi,SliceType typeIPB,PictureStructure structure)
{
	DecodedPictureBuffer *dpb = img->dpb;
	if (!mi)
		return;
#if ENABLE_MO_ALWAYS
	if(typeIPB==I_SLICE)
		typeIPB = P_SLICE;
#endif
	if (structure==FRAME){
		if (dpb->numReusedFrMotionInfo[typeIPB] == MAX_REUSED_BUFFER){
			avd_error(img,"dpb->numReusedFrMotionInfo is too large,check it",100);
			return;
		}

		{
			int i;
			for (i=0;i<dpb->numReusedFrMotionInfo[typeIPB];i++)
			{
				if(dpb->reusedFrMotionInfo[typeIPB][i]==mi)
					break;//exist
			}
			if(i==dpb->numReusedFrMotionInfo[typeIPB])//not exist,add the buffer
			{
				dpb->reusedFrMotionInfo[typeIPB][dpb->numReusedFrMotionInfo[typeIPB]] = mi;
				dpb->numReusedFrMotionInfo[typeIPB]++;
			}
			//AvdLog2(LL_INFO, "Put motion_info,type=%d,resued=%d,poc=%d,%X\n",typeIPB,dpb->numReusedFrMotionInfo[typeIPB],s->poc,mi);

		}

	}
#if FEATURE_INTERLACE
	else{
		if (dpb->numReusedFldMotionInfo[typeIPB] == MAX_REUSED_BUFFER){
			avd_error(img,"Field:dpb->numReusedFldMotionInfo is too large,check it",100);
			return;
		}
		
		{
			int i;
			for (i=0;i<dpb->numReusedFldMotionInfo[typeIPB];i++)
			{
				if(dpb->reusedFldMotionInfo[typeIPB][i]==mi)
					break;//exist
			}
			if(i==dpb->numReusedFldMotionInfo[typeIPB])//not exist,add the buffer
			{
				dpb->reusedFldMotionInfo[typeIPB][dpb->numReusedFldMotionInfo[typeIPB]] = mi;
				dpb->numReusedFldMotionInfo[typeIPB]++;
			}
			//AvdLog2(LL_INFO, "Put motion_info,type=%d,resued=%d,poc=%d,%X\n",typeIPB,dpb->numReusedFrMotionInfo[typeIPB],s->poc,mi);

		}
	}
#endif //VOI_H264D_NON_BASELINE
}

void InitStorablePic(ImageParameters *img,StorablePicture *s, PictureStructure structure, SliceType typeIPB, 
					 avdNativeUInt size_x, avdNativeUInt size_y, avdNativeUInt size_x_cr, avdNativeUInt size_y_cr, 
					 avdNativeUInt createMotionInfo)
{
	avdNativeInt moving4x4Width;
	AVDStoredPicMotionInfo	*motionInfo;
	s->structure = structure;
	s->size_x = size_x;
	s->size_y = size_y;
	s->size_x_cr = size_x_cr;
	s->size_y_cr = size_y_cr;  
	s->top_field    =
	s->bottom_field =
	s->frame        = NULL;
	s->pic_num						=
	s->long_term_frame_idx			=
	s->long_term_pic_num			=
	s->used_for_reference			=
	s->is_long_term					=
	s->isCombinedOrSplit		    =
	s->coded_frame                  =
	s->mb_adaptive_frame_field_flag =
	s->non_existing					=
	s->is_output					= 0;
	s->typeIPB						= typeIPB;
	s->motionInfo					= NULL;
	s->error						= 0;

#if FORE_CREATE_MOTION_INFO
	//createMotionInfo  = 1;
#endif//FORE_CREATE_MOTION_INFO
#if !ENABLE_MO_ALWAYS
	if (createMotionInfo)//for main profile, create the motion info always
#endif
	{
		motionInfo = s->motionInfo = get_motion_info(img,typeIPB,structure);CHECK_ERR_RET_VOID
#if FEATURE_BFRAME //TODOBframe
		if (motionInfo && motionInfo->moving4x4){
			moving4x4Width = size_x>>2;
			moving4x4Width = (moving4x4Width&0x7) ? (moving4x4Width>>3) + 1 : (moving4x4Width>>3);
			// align on 4;
			moving4x4Width += (moving4x4Width&3) ? (4 - ((moving4x4Width&3))) : 0;
			memset(&motionInfo->moving4x4[0][0], 0, moving4x4Width * (size_y>>2) * sizeof(avdUInt8));
		}
#endif // VOI_H264D_NON_BASELINE
	}

#if TEST_TIME
	if (img->profile.curIdx>=MAX_CHECK_FRAMES)
		s->fprofile = 0;
	else
	{
		s->fprofile = &img->profile.list[img->profile.curIdx++];
		memset(s->fprofile,0,sizeof(TFrameProfile));
		s->fprofile->frameType = typeIPB;
		if (typeIPB==I_SLICE)
			img->profile.INum++;
		else if (typeIPB==P_SLICE)
			img->profile.PNum++;
		else
			img->profile.BNum++;
	}		
#endif//TEST_TIME
}

StorablePicture *GetStorablePic(ImageParameters *img,PictureStructure structure, SliceType typeIPB, 
								avdNativeUInt size_x, avdNativeUInt size_y, avdNativeUInt size_x_cr, 
								avdNativeUInt size_y_cr, avdNativeUInt createMotionInfo)
{
	StorablePicture *s;
	DecodedPictureBuffer *dpb = img->dpb;
	H264VdLibParam* params = img->vdLibPar;
	int	 numReused=dpb->numReusedFrBuf;
	if (structure == FRAME){
		if (dpb->numReusedFrBuf==0)
			return alloc_storable_picture (img,structure, typeIPB, size_x, size_y, 
					size_x_cr, size_y_cr, createMotionInfo);

		numReused--;
		s = dpb->reusedFrBuf[numReused];
		if(params->sharedMem&&params->sharedMem->GetBufByIndex)
		{
			int tryCount=numReused;
			while(1)
			{
				if(tryCount>=0)
				{
					if(0==params->sharedMem->GetBufByIndex(0,s->index))
					{
						
						if(tryCount!=numReused)
						{
							//numReused=dpb->numReusedFrBuf;
							//move the data
							int diff=numReused-tryCount;
							int i;
							for(i=tryCount;i<numReused;i++)
							{
								dpb->reusedFrBuf[i]=dpb->reusedFrBuf[i+1];
							}
						}
						//AvdLog2(LL_INFO,"Get reusedBuf,%d,left=%d\n",s->index,numReused);
						break;
					}
				}
				else
				{
					s=alloc_storable_picture (img,structure, typeIPB, size_x, size_y, 
						size_x_cr, size_y_cr, createMotionInfo);
					if(s)
					{
						return s;
					}
					else
					{
						AVD_ERROR_CHECK2(img,"no shared YUV Buffer\n",VOMEMRC_NO_YUV_BUFFER);
					}
				}
				///AvdLog2(LL_INFO,"Faile to getReusedBuf,%d\n",s->index);
				s = dpb->reusedFrBuf[--tryCount];
			}
		}
		
		dpb->numReusedFrBuf=numReused;
		InitStorablePic(img,s, structure, typeIPB, size_x, size_y, 
			size_x_cr, size_y_cr, createMotionInfo);
	}
#if FEATURE_INTERLACE
	else{
		if (!dpb->numReusedFldBuf)
			return alloc_storable_picture (img,structure, typeIPB, size_x, size_y, 
					size_x_cr, size_y_cr, createMotionInfo);

		dpb->numReusedFldBuf--;
		s = dpb->reusedFldBuf[dpb->numReusedFldBuf];
		InitStorablePic(img,s, structure, typeIPB, size_x, size_y, 
			size_x_cr, size_y_cr, createMotionInfo);
	}
#endif //VOI_H264D_NON_BASELINE
	return s;
}

void PutStorablePic(ImageParameters *img,StorablePicture *s)
{
	DecodedPictureBuffer *dpb;
	if (!s)
		return;

	dpb = img->dpb;
	if (s->structure == FRAME){
		if (dpb->numReusedFrBuf == MAX_REUSED_BUFFER){
			avd_error(img,"dpb->numReusedFrBuf is too large,check it",100);
			return;
		}
		PutMotionInfo(img,s->motionInfo,s->typeIPB,s->structure);

		{
			int i;
			///AvdLog2(LL_INFO,"save in reusedBuf,%d,total=%d\n",s->index,dpb->numReusedFrBuf+1);
			for (i=0;i<dpb->numReusedFrBuf;i++)
			{
				if(dpb->reusedFrBuf[i]==s)
					break;//exist
			}
			if(i==dpb->numReusedFrBuf)//not exist,add the buffer
			{
				dpb->reusedFrBuf[img->dpb->numReusedFrBuf] = s;
				
				dpb->numReusedFrBuf++;
			}

		}

		
	}
#if FEATURE_INTERLACE
	else{
		if (dpb->numReusedFldBuf == MAX_REUSED_BUFFER){
			avd_error(img,"Field:dpb->numReusedFldBuf is too large,check it",100);
			return;
		}
		PutMotionInfo(img,s->motionInfo,s->typeIPB,s->structure);
		{
			int i;
			///AvdLog2(LL_INFO,"save in reusedBuf,%d,total=%d\n",s->index,dpb->numReusedFrBuf+1);
			for (i=0;i<dpb->numReusedFldBuf;i++)
			{
				if(dpb->reusedFldBuf[i]==s)
					break;//exist
			}
			if(i==dpb->numReusedFldBuf)//not exist,add the buffer
			{
				dpb->reusedFldBuf[img->dpb->numReusedFldBuf] = s;

				dpb->numReusedFldBuf++;
			}

		}
	}
#endif // VOI_H264D_NON_BASELINE
}
