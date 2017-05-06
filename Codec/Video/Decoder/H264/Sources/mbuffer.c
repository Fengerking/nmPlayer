

#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#endif
#include "global.h"
#include "mbuffer.h"
#include "memalloc.h"
#include "output.h"
#include "image.h"
#ifdef RVDS//ARMCC compiler can not use __cdecl
#define __cdecl
#endif

//-------- start new Buffer code -----------------------------
static void insert_picture_in_dpb(ImageParameters *img,FrameStore* fs, StorablePicture* p);
static void output_one_frame_from_dpb(ImageParameters *img);
static avdNativeInt is_used_for_reference(ImageParameters *img,FrameStore* fs);
static void get_smallest_poc(ImageParameters *img,avdInt32 *poc, avdNativeInt * pos);
static avdNativeInt  remove_unused_frame_from_dpb(ImageParameters *img);
static avdNativeInt  is_short_term_reference(ImageParameters *img,FrameStore* fs);
static avdNativeInt  is_long_term_reference(ImageParameters *img,FrameStore* fs);

/*!
 ************************************************************************
 * \brief
 *    Print out list of pictures in DPB. Used for debug purposes.
 ************************************************************************
 */


/*!
 ************************************************************************
 * \brief
 *    Returns the size of the dpb depending on level and picture size
 *
 *
 ************************************************************************
 */
#ifdef REFERENCE_CODE_COMPLIANT
avdNativeInt getDpbSize()
{
  seq_parameter_set_rbsp_t *active_sps = img->active_sps;
  avdNativeInt pic_size = (active_sps->pic_width_in_mbs_minus1 + 1) * 
	  (active_sps->pic_height_in_map_units_minus1 + 1) * 
	  (active_sps->frame_mbs_only_flag?1:2) * 384;

  switch (active_sps->level_idc)
  {
  case 10:
    return 152064 / pic_size;
  case 11:
    return 345600 / pic_size;
  case 12:
    return 912384 / pic_size;
  case 13:
    return 912384 / pic_size;
  case 20:
    return 912384 / pic_size;
  case 21:
    return 1824768 / pic_size;
  case 22:
    return 3110400 / pic_size;
  case 30:
    return 3110400 / pic_size;
  case 31:
    return 6912000 / pic_size;
  case 32:
    return 7864320 / pic_size;
  case 40:
    return 12582912 / pic_size;
  case 41:
    return 12582912 / pic_size;
  case 42:
    return 12582912 / pic_size;
  case 50:
    return 42393600 / pic_size;
  case 51:
    return 70778880 / pic_size;
  default:
    AVD_ERROR_CHECK(img,"undefined level", 500);
  }

  return 0;
}
#endif // REFERENCE_CODE_COMPLIANT

/*!
 ************************************************************************
 * \brief
 *    Allocate memory for decoded picture buffer an initialize with sane values.
 *
 ************************************************************************
 */
void init_dpb(ImageParameters *img)
{
	avdNativeUInt i,j, size, sizeFS;
	avdNativeInt idx;
	StorablePicture	**pt;
	StorablePicture	***listX;
	FrameStore **tmpFs;
	DecodedPictureBuffer *dpb = img->dpb;
	TSpecitialInfo	*specialInfo = img->specialInfo;
#if (DUMP_VERSION & DUMP_MEMORY)
	int memUsed = _totalMemory;
#endif //(DUMP_VERSION & DUMP_MEMORY)

	if (dpb->init_done)
	{
		free_dpb(img);
	}

	AVD_ALIGNED32_MEMSET(dpb, 0, sizeof(DecodedPictureBuffer) / sizeof (avdInt32));
#ifdef REFERENCE_CODE_COMPLIANT
	dpb->size      = min(16, getDpbSize());
#else // REFERENCE_CODE_COMPLIANT
	dpb->size      = img->active_sps->num_ref_frames;
#endif // REFERENCE_CODE_COMPLIANT``


	if(dpb->size>16 || dpb->size<0)
	{
		dpb->size = 10;
		AVD_ERROR_CHECK2(img,"dpb->size>16 || dpb->size<0",ERROR_InvalidDpbBufSize);
	}
	img->maxListSize = (dpb->size<<1) + 1;

	sizeFS = dpb->size * sizeof (FrameStore *);
	size = sizeFS * 3 + 
		img->maxListSize * sizeof (StorablePicture *) * AVD_MOTION_LIST_SIZE;

	dpb->fs       = (FrameStore **)voH264AlignedMalloc(img,22, size);CHECK_ERR_RET_VOID
	dpb->fs_ref   = (FrameStore **)((avdUInt8 *)dpb->fs + sizeFS);
	dpb->fs_ltref = (FrameStore **)((avdUInt8 *)dpb->fs_ref + sizeFS);
	pt = (StorablePicture **)((avdUInt8 *)dpb->fs_ltref + sizeFS);
	dpb->last_picture = NULL;

	idx = dpb->size;
	tmpFs = dpb->fs;
	while (idx>0){
		tmpFs[idx - 1]       = alloc_frame_store(img);
		idx--;
	} ;
	
	size = (img->maxListSize) * sizeof (StorablePicture *);
	listX = img->listX;
	i = AVD_MOTION_LIST_SIZE;
	do {
		// in reference code, img->listX[i] = voH264AlignedMalloc(img,MAX_LIST_SIZE * sizeof (StorablePicture*));
		//#if !(AVD_PLATFORM & AVD_TI_C55x) 
		listX[i - 1] = pt;
		pt = (StorablePicture **)((avdUInt8 *)pt + size); // +1 for reordering
		//#else
		//	img->listX[i] = (StorablePicture**)(Reference_Picture_Address + (avdUInt32)i*0x01000); 
		//#endif
	} while (--i);

#ifdef USE_JOBPOOL
       img->m_listX = (StorablePicture ****)voH264AlignedMalloc(img,22, MAX_SLICE_NUM * AVD_MOTION_LIST_SIZE * size);CHECK_ERR_RET_VOID;
       for(i = 0; i < MAX_SLICE_NUM; i++)
	   for(j = 0; j <  AVD_MOTION_LIST_SIZE; j ++)	
           img->mirror_listX[i][j] = (((avdUInt8 *)img->m_listX) + i * AVD_MOTION_LIST_SIZE * size + j * size);
#endif

	size = img->maxListSize;
	j = AVD_MOTION_LIST_SIZE - 1;
	do {
		i = size - 1;
		do {
			listX[j][i] = NULL;
		} while (i--);
		img->listXsize[j] = 0;
	} while (j--);

	dpb->last_output_poc = VO_INT_MIN;
	specialInfo->last_has_mmco_5 = 0;
	// from JM8.3
	dpb->used_size = 0;
	dpb->last_picture = NULL;
	dpb->ref_frames_in_buffer = 0;
	dpb->ltref_frames_in_buffer = 0;

	dpb->init_done = 1;
	dpb->actualAllocatedPic = 0;
	dpb->actualAllocatedMotion = 0;
#if (DUMP_VERSION & DUMP_MEMORY)
	AvdLog(DUMP_MEMORY, "\n init_dpb = %d, total = %d", 
	_totalMemory - memUsed, _totalMemory);
#endif //(DUMP_VERSION & DUMP_MEMORY)
}


/*!
 ************************************************************************
 * \brief
 *    Free memory for decoded picture buffer.
 ************************************************************************
 */
void free_dpb(ImageParameters *img)
{
	avdNativeInt i, j, n;
	DecodedPictureBuffer *dpb = img->dpb;
	FrameStore **tmpFs = dpb->fs;
	StorablePicture **tmpSP;
	AVDStoredPicMotionInfo **tmpSPM;
	if (tmpFs) {
		StorablePicture	***listX;
		i = dpb->size;
		while (i>0) {
			free_frame_store(img,tmpFs[i - 1]);
			i--;
		} ;
		SafevoH264AlignedFree(img,tmpFs);
		dpb->fs=NULL;
		dpb->fs_ref = NULL;
		dpb->fs_ltref = NULL;
		listX = img->listX;
		i = AVD_MOTION_LIST_SIZE;
		do {
			listX[i - 1] = NULL;
		} while (--i);
	}
#ifdef  USE_JOBPOOL
       SafevoH264AlignedFree(img, img->m_listX);
#endif
	dpb->last_output_poc = VO_INT_MIN;
#if 1

	for(i=0;i<dpb->actualAllocatedPic;i++)
	{
		free_storable_picture(img,dpb->actualAllocatedPicBuf[i]);
		dpb->actualAllocatedPicBuf[i] = NULL;
	}
	for(i=0;i<dpb->actualAllocatedMotion;i++)
	{
		FreeMotionInfo(img,dpb->actualAllocatedMotionBuf[i],AVD_MOTION_INFO_SIZE!=1);
		dpb->actualAllocatedMotionBuf[i] = NULL;
	}
#endif//NEW_BUF_MANAGEMENT
	dpb->init_done = 0;
	CleanOutDataQueue(img);
}


/*!
 ************************************************************************
 * \brief
 *    Allocate memory for decoded picture buffer frame stores an initialize with sane values.
 *
 * \return
 *    the allocated FrameStore structure
 ************************************************************************
 */
FrameStore* alloc_frame_store(ImageParameters *img)
{
	FrameStore *f;
	int size = sizeof(struct frame_store);

	f = (FrameStore *)voH264AlignedMalloc(img,26, size);CHECK_ERR_RET_NULL
#ifndef NDEBUG
	if (NULL==f) 
	no_mem_exit("alloc_frame_store: f");
#endif
	AVD_ALIGNED32_MEMSET(f, 0, size / sizeof (avdInt32));
	return f;
}


/*!
 ************************************************************************
 * \brief
 *    Allocate memory for a stored picture. 
 *
 * \param structure
 *    picture structure
 * \param size_x
 *    horizontal luma size
 * \param size_y
 *    vertical luma size
 * \param size_x_cr
 *    horizontal chroma size
 * \param size_y_cr
 *    vertical chroma size
 *
 * \return
 *    the allocated StorablePicture structure
 ************************************************************************
 */
 #ifdef USE_JOBPOOL
 #define ALIGN(ptr, bytePos) \
        (ptr + ( ((bytePos - (int)ptr) & (bytePos - 1)) / sizeof(*ptr) ))
 #endif

StorablePicture* alloc_storable_picture(ImageParameters *img,PictureStructure structure, SliceType typeIPB, 
										avdNativeUInt size_x, avdNativeUInt size_y, avdNativeUInt size_x_cr, 
										avdNativeUInt size_y_cr, avdNativeUInt createMotionInfo)
{
	avdNativeInt i, plnYSize,actualSize;
	StorablePicture *s;
	int		isNewSharedMem=0;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	H264VdLibParam* params = img->vdLibPar;
#if (DUMP_VERSION & DUMP_MEMORY)
	int memUsed = _totalMemory;
#endif //(DUMP_VERSION & DUMP_MEMORY)
	
	i=0;
	while (img->dpb->actualAllocatedPic == MAX_REUSED_BUFFER&&i<MAX_REUSED_BUFFER)
	{		
			AVD_ERROR_CHECK2(img,"img->dpb->actualAllocatedPic == MAX_REUSED_BUFFER",200);//printf a warning message in IA32 test
			s = img->dpb->actualAllocatedPicBuf[i++];
			if(s&&s->motionInfo)//it is safe to return a P frame buffer with the motion buffer for reuse
				return s;
	}
	//can not find a store_frame that has motionInfo,error
	if(i==MAX_REUSED_BUFFER)
		AVD_ERROR_CHECK2(img,"there is no store frame which is P frame",-200);
	s = (StorablePicture *)voH264AlignedMalloc(img,27, sizeof(StorablePicture));CHECK_ERR_RET_NULL
	//memset(s,0,sizeof(StorablePicture));//it is must 
	if(params->sharedMem&&params->sharedMem->GetBufByIndex)
	{
		isNewSharedMem = 1;
		if(params->allocatedSharedMem==params->sharedMemInfo->FrameCount)
		{
#ifdef X86_TEST
			printf("!!buffer are all allocated(%d)\n",params->allocatedSharedMem);
#endif//X86_TEST
			return NULL;
		}
		if(0==params->sharedMem->GetBufByIndex(0,params->allocatedSharedMem))
		{
			//int* tmpBuf=(int*)params->sharedMemInfo->VBuffer[params->allocatedSharedMem];
			int offset=params->allocatedSharedMem;//*sizeof(VOVIDEO_BUFFER);
			VOVIDEO_BUFFER*	buffer;
			buffer=params->sharedMemInfo->VBuffer+offset;//*(tmpBuf+params->allocatedSharedMem);
			params->allocatedSharedMem++;
			//AvdLog2(LL_INFO,"MBBuffer:Get:%X\n",sharedMemBuf);
			s->plnY = s->plnBuffer =buffer->Buffer[0];
			//printf("MBBuffer:Get:%X\n",s->plnBuffer);
			s->plnU = buffer->Buffer[1];
			s->plnV = buffer->Buffer[2];
			s->index=params->allocatedSharedMem-1;
			sizeInfo->yPlnPitch=buffer->Stride[0];
		}
		else//it is impossible
		{
			avd_error(img,"Unknown error:no shared memory\n",301);
			return NULL;
		}
	}
	s->valid = BIT_PROTECT_CHECK;
	img->dpb->actualAllocatedPicBuf[img->dpb->actualAllocatedPic]=s;
	img->dpb->actualAllocatedPic++;
	
#ifndef NDEBUG
	if (NULL==s) 
		no_mem_exit("alloc_storable_picture: s");
#endif

	InitStorablePic(img,s, structure, typeIPB, size_x, size_y, size_x_cr, size_y_cr, createMotionInfo);CHECK_ERR_RET_NULL


	
		#ifdef H264D_IPP_DEBLOCK_PATCH
			actualSize = size_x * size_y ;
			plnYSize = size_x * (size_y + 4);
			s->plnBuffer = (avdUInt8 *)voH264AlignedMalloc(img,28, plnYSize + (actualSize>>1)); // for YUV;
			s->plnY = s->plnBuffer + (size_x<<2);
			s->plnU = s->plnY + actualSize;
			s->plnV = s->plnU + (actualSize>>2)  ;
		#else//H264D_IPP_DEBLOCK_PATCH
#ifdef USE_JOBPOOL		
			i = (size_x + (XY_PADDING<<1));
			plnYSize = i * (size_y + (XY_PADDING<<1));
#else
			i = (size_x + (Y_PADDING<<1));
			plnYSize = i * (size_y + (Y_PADDING<<1));
#endif
#ifdef ARM11_MX31_DEBLOCK_USE_PHYMEM_E
			s->plnBuffer = ARM11_MX31_AllocOneFrame(plnYSize + (plnYSize>>1),16); // for YUV;
#else//ARM11_MX31_DEBLOCK_USE_PHYMEM
			if(params->customMem2==NULL||params->customMem2->Alloc4)
			{
				s->plnBuffer = voH264AlignedMalloc(img,YUV_BUF_ID, plnYSize + (plnYSize>>1)); // for YUV;
			}
#if 0//this is for old code
			else if(params->customMem2->Alloc)
			{
				int phyAddress=0;
				s->plnBuffer = params->customMem2->Alloc(plnYSize + (plnYSize>>1),(unsigned int*)&phyAddress); // for YUV;
				if(params->deblockFlag)
				{
					IMX31_AddOneFrame(img,s->plnBuffer,phyAddress);
				}
			}

			else if(params->customMem2->Alloc2)
			{
				VOCODECVIDEOBUFFER buffer;
				isNewSharedMem = 1;
				s->plnBuffer=params->customMem2->Alloc2(size_x,size_y,&buffer);
				if(s->plnBuffer)
				{
					s->plnY = buffer.data_buf[0];
					if(buffer.video_type==VOYVU_PLANAR420)
					{
						s->plnU = buffer.data_buf[2];
						s->plnV = buffer.data_buf[1];
					}
					else
					{
						s->plnU = buffer.data_buf[1];
						s->plnV = buffer.data_buf[2];
					}
					
				
				}
			}
			else if(params->customMem2->Alloc3)
			{
				int phyAddress=0;
				s->plnBuffer = params->customMem2->Alloc3(img,plnYSize + (plnYSize>>1),(unsigned int*)&phyAddress); // for YUV;
				if(params->deblockFlag)
				{
					IMX31_AddOneFrame(img,s->plnBuffer,phyAddress);
				}
			}
#endif//0		
				
			CHECK_ERR_RET_NULL
#endif//ARM11_MX31_DEBLOCK_USE_PHYMEM
			if(!isNewSharedMem )//params->customMem2&&params->customMem2->Alloc2==NULL&&params->sharedMem&&params->sharedMem->GetBufByIndex==NULL)
			{
#ifdef USE_JOBPOOL
				i *= XY_PADDING; // plnY offset
				s->plnY = s->plnBuffer + i + XY_PADDING;
				s->plnU = s->plnBuffer + plnYSize + (i>>2) + (XY_PADDING>>1);
				s->plnV = s->plnU + (plnYSize>>2);
                           s->plnY = ALIGN(s->plnY, 32);
                           s->plnU = ALIGN( s->plnU, 32);
			      s->plnV = ALIGN( s->plnV, 32);				
#else			
				i *= Y_PADDING; // plnY offset
				s->plnY = s->plnBuffer + i + Y_PADDING;
				s->plnU = s->plnBuffer + plnYSize + (i>>2) + (Y_PADDING>>1);
				s->plnV = s->plnU + (plnYSize>>2);
#endif				
			}
#ifdef USE_JOBPOOL
                     plnYSize = (size_x + (Y_PADDING<<1)) * (size_y + (Y_PADDING<<1));
#endif
			
		#endif//H264D_IPP_DEBLOCK_PATCH
	
	//default:make it black
	AvdLog2 (LL_INFO,"Allocating %d,addr=%X\n", img->dpb->actualAllocatedPic,s->plnY);
	memset(s->plnY,0,plnYSize);
	memset(s->plnU,128,plnYSize>>2);
	memset(s->plnV,128,plnYSize>>2);
#if (DUMP_VERSION & DUMP_MEMORY)
	AvdLog(DUMP_MEMORY, "\n alloc_storable_picture = %d, total = %d", 
		_totalMemory - memUsed, _totalMemory);
#endif //(DUMP_VERSION & DUMP_MEMORY)
	return s;
}

/*!
 ************************************************************************
 * \brief
 *    Free frame store memory.
 *
 * \param f
 *    FrameStore to be freed
 *
 ************************************************************************
 */
void free_frame_store(ImageParameters *img,FrameStore* f)
{
  if (f)
  {
#if FEATURE_INTERLACE
    if (f->frame)
    {
		PutStorablePic(img,f->frame);
      f->frame=NULL;
    }

    if (f->top_field)
    {
		PutStorablePic(img,f->top_field);
      f->top_field=NULL;
    }
    if (f->bottom_field)
    {
		PutStorablePic(img,f->bottom_field);
      f->bottom_field=NULL;
    }
#else//baseline
	PutStorablePic(img,f->frame);
	f->frame=NULL;
#endif //VOI_H264D_NON_BASELINE
    SafevoH264AlignedFree(img,f);
  }
}

/*!
 ************************************************************************
 * \brief
 *    Free picture memory.
 *
 * \param p
 *    Picture to be freed
 *
 ************************************************************************
 */
void free_storable_picture(ImageParameters *img,StorablePicture* p)
{
	H264VdLibParam* params = img->vdLibPar;
	if (!p)
		return;

	PutMotionInfo(img,p->motionInfo,p->typeIPB,p->structure);


	if (p->plnBuffer) {
#if ARM11_MX31_DEBLOCK_USE_PHYMEM_E
		ARM11_MX31_FreeOneFrame(p->plnBuffer);
#else//ARM11_MX31_DEBLOCK_USE_PHYMEM
		if(params->customMem2==NULL||params->customMem2->Free4)
		{
			SafevoH264AlignedFree(img,p->plnBuffer);
		}
#if ENABLE_OTHER_ALLOC
		else if(params->customMem2->Free)
		{
			params->customMem2->Free(p->plnBuffer); // for YUV;
		}
		else if(params->customMem2->Free3)
		{
			params->customMem2->Free3(img,p->plnBuffer); // for YUV;
		}
#endif
			
#endif//ARM11_MX31_DEBLOCK_USE_PHYMEM
		p->plnBuffer = NULL;
	}

	if(p->seiPicInfo.Buffer)
	{
		SafevoH264AlignedFree(img,p->seiPicInfo.Buffer);
	}

    SafevoH264AlignedFree(img,p);
}

/*!
 ************************************************************************
 * \brief
 *    mark FrameStore unused for reference
 *
 ************************************************************************
 */
static void unmark_for_reference(ImageParameters *img,FrameStore* fs)
{
#if FEATURE_INTERLACE
  if (fs->top_field && fs->is_used & 1)
  {
    fs->top_field->used_for_reference = 0;
  }
  if (fs->bottom_field && fs->is_used & 2)
  {
    fs->bottom_field->used_for_reference = 0;
  }
  if (fs->frame && fs->is_used == 3)
  {
    fs->frame->used_for_reference = 0;
	if (!img->active_sps->frame_mbs_only_flag){
		if (fs->top_field)
			fs->top_field->used_for_reference = 0;
		if (fs->bottom_field)
			fs->bottom_field->used_for_reference = 0;
	}
  }
#else //VOI_H264D_NON_BASELINE
  if (fs->frame && fs->is_used == 3)
    fs->frame->used_for_reference = 0;
#endif //VOI_H264D_NON_BASELINE

  fs->is_reference = 0;
}


/*!
 ************************************************************************
 * \brief
 *    mark FrameStore unused for reference and reset long term flags
 *
 ************************************************************************
 */
static void unmark_for_long_term_reference(FrameStore* fs)
{
#if FEATURE_INTERLACE
  if (fs->top_field && fs->is_used & 1)
  {
    fs->top_field->used_for_reference = 0;
    fs->top_field->is_long_term = 0;
  }
  if (fs->bottom_field && fs->is_used & 2)
  {
    fs->bottom_field->used_for_reference = 0;
    fs->bottom_field->is_long_term = 0;
  }
  if (fs->frame && fs->is_used == 3)
  {
	//Robust 
	if(fs->top_field)
	{
		fs->top_field->used_for_reference = 0;
		fs->top_field->is_long_term = 0;
	}
    
    if(fs->bottom_field)
	{
		fs->bottom_field->used_for_reference = 0;
		fs->bottom_field->is_long_term = 0;
	}
    fs->frame->used_for_reference = 0;
    fs->frame->is_long_term = 0;
  }
#else //VOI_H264D_NON_BASELINE
  if (fs->is_used == 3)
  {
    fs->frame->used_for_reference = 0;
    fs->frame->is_long_term = 0;
  }
#endif //VOI_H264D_NON_BASELINE

  fs->is_reference = 0;
  fs->is_long_term = 0;
}


/*!
 ************************************************************************
 * \brief
 *    compares two stored pictures by picture number for qsort in descending order
 *
 ************************************************************************
 */
static int __cdecl compare_pic_by_pic_num_desc( const void *arg1, const void *arg2 )
{
  if ( (*(StorablePicture**)arg1)->pic_num < (*(StorablePicture**)arg2)->pic_num)
    return 1;
  if ( (*(StorablePicture**)arg1)->pic_num > (*(StorablePicture**)arg2)->pic_num)
    return -1;
  else
    return 0;
}

/*!
 ************************************************************************
 * \brief
 *    compares two stored pictures by picture number for qsort in descending order
 *
 ************************************************************************
 */
static int __cdecl compare_pic_by_lt_pic_num_asc( const void *arg1, const void *arg2 )
{
  if ( (*(StorablePicture**)arg1)->long_term_pic_num < (*(StorablePicture**)arg2)->long_term_pic_num)
    return -1;
  if ( (*(StorablePicture**)arg1)->long_term_pic_num > (*(StorablePicture**)arg2)->long_term_pic_num)
    return 1;
  else
    return 0;
}

/*!
 ************************************************************************
 * \brief
 *    compares two frame stores by pic_num for qsort in descending order
 *
 ************************************************************************
 */
static int __cdecl compare_fs_by_frame_num_desc( const void *arg1, const void *arg2 )
{
  if ( (*(FrameStore**)arg1)->frame_num_wrap < (*(FrameStore**)arg2)->frame_num_wrap)
    return 1;
  if ( (*(FrameStore**)arg1)->frame_num_wrap > (*(FrameStore**)arg2)->frame_num_wrap)
    return -1;
  else
    return 0;
}


/*!
 ************************************************************************
 * \brief
 *    compares two frame stores by lt_pic_num for qsort in descending order
 *
 ************************************************************************
 */
static int __cdecl compare_fs_by_lt_pic_idx_asc( const void *arg1, const void *arg2 )
{
  if ( (*(FrameStore**)arg1)->long_term_frame_idx < (*(FrameStore**)arg2)->long_term_frame_idx)
    return -1;
  if ( (*(FrameStore**)arg1)->long_term_frame_idx > (*(FrameStore**)arg2)->long_term_frame_idx)
    return 1;
  else
    return 0;
}


/*!
 ************************************************************************
 * \brief
 *    compares two stored pictures by poc for qsort in ascending order
 *
 ************************************************************************
 */
static int __cdecl compare_pic_by_poc_asc( const void *arg1, const void *arg2 )
{
  if ( (*(StorablePicture**)arg1)->poc < (*(StorablePicture**)arg2)->poc)
    return -1;
  if ( (*(StorablePicture**)arg1)->poc > (*(StorablePicture**)arg2)->poc)
    return 1;
  else
    return 0;
}


static int __cdecl compare_pic_by_poc_desc( const void *arg1, const void *arg2 )
{
  if ( (*(StorablePicture**)arg1)->poc < (*(StorablePicture**)arg2)->poc)
    return 1;
  if ( (*(StorablePicture**)arg1)->poc > (*(StorablePicture**)arg2)->poc)
    return -1;
  else
    return 0;
}


static int __cdecl compare_fs_by_poc_asc( const void *arg1, const void *arg2 )
{
  if ( (*(FrameStore**)arg1)->poc < (*(FrameStore**)arg2)->poc)
    return -1;
  if ( (*(FrameStore**)arg1)->poc > (*(FrameStore**)arg2)->poc)
    return 1;
  else
    return 0;
}


static int __cdecl compare_fs_by_poc_desc( const void *arg1, const void *arg2 )
{
  if ( (*(FrameStore**)arg1)->poc < (*(FrameStore**)arg2)->poc)
    return 1;
  if ( (*(FrameStore**)arg1)->poc > (*(FrameStore**)arg2)->poc)
    return -1;
  else
    return 0;
}


avdNativeInt is_short_ref(StorablePicture *s)
{
  return ((s->used_for_reference) && (!(s->is_long_term)));
}


avdNativeInt is_long_ref(StorablePicture *s)
{
  return ((s->used_for_reference) && (s->is_long_term));
}


#if FEATURE_INTERLACE
static void gen_pic_list_from_frame_list(PictureStructure currStrcture, 
										 FrameStore **fs_list, int list_idx, 
										 StorablePicture **list, avdNativeUInt *list_size, 
										 int long_term)
{
  avdNativeInt top_idx = 0;
  avdNativeInt bot_idx = 0;

  avdNativeInt (*is_ref)(StorablePicture *s);

  if (long_term)
    is_ref=is_long_ref;
  else
    is_ref=is_short_ref;

  if (currStrcture == TOP_FIELD)
  {
    while ((top_idx<list_idx)||(bot_idx<list_idx))
    {
      for ( ; top_idx<list_idx; top_idx++)
      {
        if(fs_list[top_idx]->is_used & 1)
        {
          if(is_ref(fs_list[top_idx]->top_field))
          {
            // short term ref pic
            list[*list_size] = fs_list[top_idx]->top_field;
            (*list_size)++;
            top_idx++;
            break;
          }
        }
      }
      for ( ; bot_idx<list_idx; bot_idx++)
      {
        if(fs_list[bot_idx]->is_used & 2)
        {
          if(is_ref(fs_list[bot_idx]->bottom_field))
          {
            // short term ref pic
            list[*list_size] = fs_list[bot_idx]->bottom_field;
            (*list_size)++;
            bot_idx++;
            break;
          }
        }
      }
    }
  }
  if (currStrcture == BOTTOM_FIELD)
  {
    while ((top_idx<list_idx)||(bot_idx<list_idx))
    {
      for ( ; bot_idx<list_idx; bot_idx++)
      {
        if(fs_list[bot_idx]->is_used & 2)
        {
          if(is_ref(fs_list[bot_idx]->bottom_field))
          {
            // short term ref pic
            list[*list_size] = fs_list[bot_idx]->bottom_field;
            (*list_size)++;
            bot_idx++;
            break;
          }
        }
      }
      for ( ; top_idx<list_idx; top_idx++)
      {
        if(fs_list[top_idx]->is_used & 1)
        {
          if(is_ref(fs_list[top_idx]->top_field))
          {
            // short term ref pic
            list[*list_size] = fs_list[top_idx]->top_field;
            (*list_size)++;
            top_idx++;
            break;
          }
        }
      }
    }
  }
}
#endif //VOI_H264D_NON_BASELINE

void InitRefBuffer(ImageParameters *img)
{
	avdNativeInt i;
	DecodedPictureBuffer *dpb = img->dpb;
	TPOCInfo* pocInfo = img->pocInfo;
	H264VdLibParam* params = img->vdLibPar;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	for(i=0;i<=img->num_ref_idx_l0_active;i++)
	{
		if(NULL == img->listX[0][i])
		{
			int actualSize,plnYSize;
			TSpecitialInfo	*specialInfo = img->specialInfo;
			StorablePicture *s = alloc_storable_picture (img,0, 
				img->type, 
				sizeInfo->width, 
				sizeInfo->height, 
				sizeInfo->width_cr, 
				sizeInfo->height_cr, 
				(img->type != I_SLICE));CHECK_ERR_RET_VOID
			
			s->PicSizeInMbs = sizeInfo->PicSizeInMbs;
			//s->top_poc=
			//s->bottom_poc=
			//s->poc=
			s->poc=pocInfo->currPoc+i*2;

			s->typeIPB =(SliceType) (img->type);
			s->used_for_reference = (specialInfo->nal_reference_idc != 0);
			s->idr_flag = specialInfo->idr_flag;
			s->no_output_of_prior_pics_flag = specialInfo->no_output_of_prior_pics_flag;
			s->long_term_reference_flag = specialInfo->long_term_reference_flag;
			s->adaptive_ref_pic_buffering_flag = specialInfo->adaptive_ref_pic_buffering_flag;
			s->dec_ref_pic_marking_buffer = specialInfo->dec_ref_pic_marking_buffer;
			s->MbaffFrameFlag = GetMbAffFrameFlag(img);
			s->pic_num = specialInfo->frame_num-i;
			s->frame_num = specialInfo->frame_num-i;
			s->coded_frame = 1;
			s->is_output = 1;
			
			

#ifdef H264D_IPP_DEBLOCK_PATCH
			actualSize = sizeInfo->width * sizeInfo->height ;
			plnYSize = sizeInfo->width * (sizeInfo->height + 4)+ (actualSize>>1);
			memset(s->plnBuffer,128, plnYSize);
#else//H264D_IPP_DEBLOCK_PATCH
			plnYSize = sizeInfo->width * sizeInfo->height ;
#if ENABLE_OTHER_ALLOC
			if(params->customMem2->Alloc2)
			{
				memset(s->plnY,128, plnYSize);
				memset(s->plnU,128, plnYSize/4);
				memset(s->plnV,128, plnYSize/4);
			}
			else
#endif//
			{
				plnYSize += plnYSize/2 ;
				memset(s->plnBuffer,128, plnYSize);
			}
			
#endif//H264D_IPP_DEBLOCK_PATCH

			
			store_picture_in_dpb(img,s);CHECK_ERR_RET_VOID
			
		}
	}
}
void DumpReusedInfo(ImageParameters *img)
{
	AVDStoredPicMotionInfo *mi ;
	DecodedPictureBuffer *dpb = img->dpb;
	int i,j;
	AvdLog2(LL_INFO,"###Dump ReusedInfo\n");
	for (i=0;i<dpb->numReusedFrBuf;i++)
	{
		StorablePicture *s = dpb->reusedFrBuf[i];
		AvdLog2(LL_INFO,"###frame %d:%X,mot=%X\n",i,s,s->motionInfo);
	}
	for(j=0;j<2;j++)
		for (i=0;i<dpb->numReusedFrMotionInfo[j];i++)
		{
			mi = dpb->reusedFrMotionInfo[j][i];
			AvdLog2(LL_INFO,"###motion[%d]:%d=%X\n",j,i,mi);
		}
}
void DumpBufInfo(ImageParameters *img,TMBsProcessor* info);
void DumpRefList(ImageParameters *img,TMBsProcessor* info)
{
	int i;
#if (DUMP_VERSION & DUMP_SLICE)
	AvdLog(DUMP_SLICE,DUMP_DCORE "\nimg->listX[0] currPoc=%d (Poc_f=%d,t=%d,b=%d): ", img->pocInfo->currPoc,img->pocInfo->framepoc,
		img->pocInfo->toppoc,img->pocInfo->bottompoc);
	for (i=0; i<img->listXsize[0]; i++){
		if(img->listX[0][i])
		AvdLog(DUMP_SLICE,DUMP_DCORE "%d", img->listX[0][i]->poc);
		AvdLog(DUMP_SLICE,DUMP_DCORE "\n");
	} 
#if FEATURE_BFRAME
	AvdLog(DUMP_SLICE,DUMP_DCORE "\nimg->listX[1] currPoc=%d (Poc): ", img->pocInfo->currPoc);
	for (i=0; i<img->listXsize[1]; i++){
		if(img->listX[1][i])
		AvdLog(DUMP_SLICE,DUMP_DCORE "%d  ", img->listX[1][i]->poc);
		AvdLog(DUMP_SLICE,DUMP_DCORE "\n");
	} 
#endif
	
#endif //(DUMP_VERSION & DUMP_SLICE)
#if 0
	AvdLog2(LL_INFO, "\nimg->listX[0] currPoc=%d (Poc):\n ", pocInfo->currPoc);
	for (i=0; i<img->listXsize[0]; i++){
		AvdLog2(LL_INFO, "%d,%X,mot=%X  \n", img->listX[0][i]->poc,img->listX[0][i],img->listX[0][i]->motionInfo);
	} 

	AvdLog2(LL_INFO, "\nimg->listX[1] currPoc=%d (Poc): \n", pocInfo->currPoc);
	for (i=0; i<img->listXsize[1]; i++){
		AvdLog2(LL_INFO, "%d,%X,mot=%X  \n", img->listX[1][i]->poc,img->listX[1][i],img->listX[1][i]->motionInfo);
	} 
	DumpBufInfo(img,info);
#endif
}


void init_lists(ImageParameters *img)
{
  avdNativeInt add_top = 0, add_bottom = 0;
  avdNativeUInt i, j, list0idx_1, list0idx, listltidx;
  avdInt32 MaxFrameNum = 1 << (img->active_sps->log2_max_frame_num_minus4 + 4);
  avdNativeUInt diff;
  FrameStore **fs_list0;
  FrameStore **fs_list1;
  FrameStore **fs_listlt;
  StorablePicture *tmp_s;
  avdNativeUInt currSliceType = img->type;
  Slice * currSlice = img->currentSlice;
  PictureStructure currPicStructure = currSlice->structure;
  DecodedPictureBuffer *dpb = img->dpb;
  FrameStore **fs_ref = dpb->fs_ref;
  TSpecitialInfo	*specialInfo = img->specialInfo;
#if !FEATURE_BFRAME
  if(currSliceType != I_SLICE&&(fs_ref==NULL||fs_ref[0]==NULL))
  {
	  AVD_ERROR_CHECK2(img,"There is no ref frame in buffer!!",ERROR_REFFrameIsNULL);
  }
	  
	if (dpb->size == 1){
		if ((currSliceType == I_SLICE)||(currSliceType == SI_SLICE)){
			img->listXsize[0] = 0;
		}
		else{
			img->listX[0][0] = fs_ref[0]->frame;
			img->listXsize[0] = 1;
		}
		return;
	}
#endif //!VOI_H264D_NON_BASELINE&&!FEATURE_BFRAME

  list0idx_1 = list0idx = listltidx = 0;
  if(img->num_ref_idx_l0_active>MAX_REFERENCE_FRAMES)
  {
	  img->num_ref_idx_l0_active = 1;
	  AVD_ERROR_CHECK2(img,"init_lists,img->num_ref_idx_l0_active>16",ERROR_REFLISTGREAT16);
  }
#if FEATURE_INTERLACE
  if (currPicStructure == FRAME)  
#endif //VOI_H264D_NON_BASELINE
  {
    for (i=0; i<dpb->ref_frames_in_buffer; i++)
    {
      if (fs_ref[i]->is_used==3)
      {


        if ((fs_ref[i]->frame->used_for_reference)&&(!fs_ref[i]->frame->is_long_term))
        {
          if( fs_ref[i]->frame_num > specialInfo->frame_num )
          {
            fs_ref[i]->frame_num_wrap = fs_ref[i]->frame_num - MaxFrameNum;
          }
          else
          {
            fs_ref[i]->frame_num_wrap = fs_ref[i]->frame_num;
          }
          fs_ref[i]->frame->pic_num = fs_ref[i]->frame_num_wrap;
          fs_ref[i]->frame->order_num=list0idx;
        }
      }
    }
  }
#if FEATURE_INTERLACE
  else
  {
	  if (currPicStructure == TOP_FIELD)
	  {
		  add_top    = 1;
		  add_bottom = 0;
	  }
	  else
	  {
		  add_top    = 0;
		  add_bottom = 1;
	  }

	  for (i=0; i<dpb->ref_frames_in_buffer; i++)
	  {
		  if (fs_ref[i]->is_reference)
		  {
			  if( fs_ref[i]->frame_num > specialInfo->frame_num )
			  {
				  fs_ref[i]->frame_num_wrap = fs_ref[i]->frame_num - MaxFrameNum;
			  }
			  else
			  {
				  fs_ref[i]->frame_num_wrap = fs_ref[i]->frame_num;
			  }
			  if (fs_ref[i]->is_reference & 1)
			  {
				  fs_ref[i]->top_field->pic_num = (2 * fs_ref[i]->frame_num_wrap) + add_top;
			  }
			  if (fs_ref[i]->is_reference & 2)
			  {
				  fs_ref[i]->bottom_field->pic_num = (2 * fs_ref[i]->frame_num_wrap) + add_bottom;
			  }
		  }
	  }
  }
#endif //FEATURE_INTERLACE

  if ((currSliceType == I_SLICE)||(currSliceType == SI_SLICE))
  {
    img->listXsize[0] = 0;
#if FEATURE_BFRAME
    img->listXsize[1] = 0;
#endif //FEATURE_BFRAME
    return;
  }
  if (dpb->ref_frames_in_buffer==0)
  {
		AVD_ERROR_CHECK2(img,"There is no ref frame in buffer2!!",ERROR_REFFrameIsNULL);
  }
  if ((currSliceType == P_SLICE)||(currSliceType == SP_SLICE))
  {
    // Calculate FrameNumWrap and PicNum
   
    if (currPicStructure == FRAME)  
    {
      for (i=0; i<dpb->ref_frames_in_buffer; i++)
      {
        if (fs_ref[i]->is_used==3)
        {
          if ((fs_ref[i]->frame->used_for_reference)&&
			  (!fs_ref[i]->frame->is_long_term))
          {
            img->listX[0][list0idx++] = fs_ref[i]->frame;
          }
        }
      }
      // order list 0 by PicNum
      qsort((void *)img->listX[0], list0idx, sizeof(StorablePicture*), compare_pic_by_pic_num_desc);
      img->listXsize[0] = list0idx;
//      printf("img->listX[0] (PicNum): "); for (i=0; i<list0idx; i++){printf ("%d  ", img->listX[0][i]->pic_num);} printf("\n");

      // long term handling
      for (i=0; i<dpb->ltref_frames_in_buffer; i++)
      {
        if (dpb->fs_ltref[i]->is_used==3)
        {
          if (dpb->fs_ltref[i]->frame->is_long_term)
          {

            dpb->fs_ltref[i]->frame->long_term_pic_num = dpb->fs_ltref[i]->frame->long_term_frame_idx;
            dpb->fs_ltref[i]->frame->order_num=list0idx;
            img->listX[0][list0idx++]=dpb->fs_ltref[i]->frame;
          }
        }
      }
      qsort((void *)&img->listX[0][img->listXsize[0]], list0idx-img->listXsize[0], 
		  sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
      img->listXsize[0] = list0idx;
    }
#if FEATURE_INTERLACE
	else//below are field,not frame
	{
		fs_list0 = (FrameStore*)GetTmpBuf0(img);//voH264AlignedMalloc(img,31, dpb->size * sizeof (FrameStore*));
		fs_listlt = (avdUInt8*)fs_list0 + dpb->size * sizeof (FrameStore*);//voH264AlignedMalloc(img,32, dpb->size * sizeof (FrameStore*));
		
		for (i=0; i<dpb->ref_frames_in_buffer; i++)
		{
			if (fs_ref[i]->is_reference)
			{
				fs_list0[list0idx++] = fs_ref[i];
			}
		}

		qsort((void *)fs_list0, list0idx, sizeof(FrameStore*), compare_fs_by_frame_num_desc);

		//      printf("fs_list0 (FrameNum): "); for (i=0; i<list0idx; i++){printf ("%d  ", fs_list0[i]->frame_num_wrap);} printf("\n");

		img->listXsize[0] = 0;
		gen_pic_list_from_frame_list(currPicStructure, fs_list0, 
			list0idx, img->listX[0], &img->listXsize[0], 0);

		//      printf("img->listX[0] (PicNum): "); for (i=0; i<img->listXsize[0]; i++){printf ("%d  ", img->listX[0][i]->pic_num);} printf("\n");

		// long term handling
		for (i=0; i<dpb->ltref_frames_in_buffer; i++)
		{
			fs_listlt[listltidx++]=dpb->fs_ltref[i];
			if (dpb->fs_ltref[i]->is_long_term & 1)
			{
				dpb->fs_ltref[i]->top_field->long_term_pic_num = 
					2 * dpb->fs_ltref[i]->top_field->long_term_frame_idx + add_top;
			}
			if (dpb->fs_ltref[i]->is_long_term & 2)
			{
				dpb->fs_ltref[i]->bottom_field->long_term_pic_num = 
					2 * dpb->fs_ltref[i]->bottom_field->long_term_frame_idx + add_bottom;
			}
		}

		qsort((void *)fs_listlt, listltidx, sizeof(FrameStore*), compare_fs_by_lt_pic_idx_asc);

		gen_pic_list_from_frame_list(currPicStructure, fs_listlt, 
			listltidx, img->listX[0], &img->listXsize[0], 1);

		//SafevoH264AlignedFree(img,fs_list0);
		//SafevoH264AlignedFree(img,fs_listlt);
	}
#endif//FEATURE_INTERLACE
#if FEATURE_BFRAME
	img->listXsize[1] = 0;
#endif//FEATURE_BFRAME

  }
#if FEATURE_BFRAME
  else
  {
    // B-Slice
	TPOCInfo* pocInfo = img->pocInfo;
    if (currPicStructure == FRAME)  
    {
      for (i=0; i<dpb->ref_frames_in_buffer; i++)
      {
        if (fs_ref[i]->is_used==3)
        {
          if ((fs_ref[i]->frame->used_for_reference)&&
			  (!fs_ref[i]->frame->is_long_term))
          {
            if (pocInfo->currPoc >= fs_ref[i]->frame->poc)
            {
              fs_ref[i]->frame->order_num=list0idx;
              img->listX[0][list0idx++] = fs_ref[i]->frame;
            }
          }
        }
      }

      qsort((void *)img->listX[0], list0idx, sizeof(StorablePicture*), compare_pic_by_poc_desc);

      list0idx_1 = list0idx;
      for (i=0; i<dpb->ref_frames_in_buffer; i++)
      {
        if (fs_ref[i]->is_used==3)
        {
          if ((fs_ref[i]->frame->used_for_reference)&&
			  (!fs_ref[i]->frame->is_long_term))
          {
            if (pocInfo->currPoc < fs_ref[i]->frame->poc)
            {
              fs_ref[i]->frame->order_num=list0idx;
              img->listX[0][list0idx++] = fs_ref[i]->frame;
            }
          }
        }
      }


      qsort((void *)&img->listX[0][list0idx_1], list0idx-list0idx_1, 
		  sizeof(StorablePicture*), compare_pic_by_poc_asc);


      for (j=0; j<list0idx_1; j++)
      {
        img->listX[1][list0idx-list0idx_1+j]=img->listX[0][j];
      }
      for (j=list0idx_1; j<list0idx; j++)
      {
        img->listX[1][j-list0idx_1]=img->listX[0][j];
      }

      img->listXsize[0] = img->listXsize[1] = list0idx;


     
      // long term handling
      for (i=0; i<dpb->ltref_frames_in_buffer; i++)
      {
        if (dpb->fs_ltref[i]->is_used==3)
        {
          if (dpb->fs_ltref[i]->frame->is_long_term)
          {

            dpb->fs_ltref[i]->frame->long_term_pic_num = dpb->fs_ltref[i]->frame->long_term_frame_idx;
            dpb->fs_ltref[i]->frame->order_num=list0idx;
            
            img->listX[0][list0idx]  =dpb->fs_ltref[i]->frame;
            img->listX[1][list0idx++]=dpb->fs_ltref[i]->frame;
          }
        }
      }
      qsort((void *)&img->listX[0][img->listXsize[0]], list0idx-img->listXsize[0], 
		  sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
      qsort((void *)&img->listX[1][img->listXsize[0]], list0idx-img->listXsize[0], 
		  sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
      img->listXsize[0] = img->listXsize[1] = list0idx;
    }
#if FEATURE_INTERLACE
	else
	{
		fs_list0 = (FrameStore**)GetTmpBuf0(img);//voH264AlignedMalloc(img,33, dpb->size * sizeof (FrameStore*));
		fs_list1 = (avdUInt8*)fs_list0 + dpb->size* sizeof (FrameStore*);//voH264AlignedMalloc(img,34, dpb->size * sizeof (FrameStore*));
		fs_listlt = (avdUInt8*)fs_list1 + dpb->size * sizeof (FrameStore*);//voH264AlignedMalloc(img,35, dpb->size * sizeof (FrameStore*));

		img->listXsize[0] = 0;
		img->listXsize[1] = 1;

		for (i=0; i<dpb->ref_frames_in_buffer; i++)
		{
			if (fs_ref[i]->is_used)
			{
				if (pocInfo->currPoc >= fs_ref[i]->poc)
				{
					fs_list0[list0idx++] = fs_ref[i];
				}
			}
		}
		qsort((void *)fs_list0, list0idx, sizeof(FrameStore*), compare_fs_by_poc_desc);
		list0idx_1 = list0idx;
		for (i=0; i<dpb->ref_frames_in_buffer; i++)
		{
			if (fs_ref[i]->is_used)
			{
				if (pocInfo->currPoc < fs_ref[i]->poc)
				{
					fs_list0[list0idx++] = fs_ref[i];
				}
			}
		}
		qsort((void *)&fs_list0[list0idx_1], list0idx-list0idx_1, 
			sizeof(FrameStore*), compare_fs_by_poc_asc);

		for (j=0; j<list0idx_1; j++)
		{
			fs_list1[list0idx-list0idx_1+j]=fs_list0[j];
		}
		for (j=list0idx_1; j<list0idx; j++)
		{
			fs_list1[j-list0idx_1]=fs_list0[j];
		}

		//      printf("fs_list0 currPoc=%d (Poc): ", img->ThisPOC); for (i=0; i<list0idx; i++){printf ("%d  ", fs_list0[i]->poc);} printf("\n");
		//      printf("fs_list1 currPoc=%d (Poc): ", img->ThisPOC); for (i=0; i<list0idx; i++){printf ("%d  ", fs_list1[i]->poc);} printf("\n");

		img->listXsize[0] = 0;
		img->listXsize[1] = 0;
		gen_pic_list_from_frame_list(currPicStructure, fs_list0, 
			list0idx, img->listX[0], &img->listXsize[0], 0);
		gen_pic_list_from_frame_list(currPicStructure, fs_list1, 
			list0idx, img->listX[1], &img->listXsize[1], 0);

		//      printf("img->listX[0] currPoc=%d (Poc): ", img->framepoc); for (i=0; i<img->listXsize[0]; i++){printf ("%d  ", img->listX[0][i]->poc);} printf("\n");
		//      printf("img->listX[1] currPoc=%d (Poc): ", img->framepoc); for (i=0; i<img->listXsize[1]; i++){printf ("%d  ", img->listX[1][i]->poc);} printf("\n");

		// long term handling
		for (i=0; i<dpb->ltref_frames_in_buffer; i++)
		{
			fs_listlt[listltidx++]=dpb->fs_ltref[i];
			if (dpb->fs_ltref[i]->is_long_term & 1)
			{
				dpb->fs_ltref[i]->top_field->long_term_pic_num = 2 * dpb->fs_ltref[i]->top_field->long_term_frame_idx + add_top;
			}
			if (dpb->fs_ltref[i]->is_long_term & 2)
			{
				dpb->fs_ltref[i]->bottom_field->long_term_pic_num = 2 * dpb->fs_ltref[i]->bottom_field->long_term_frame_idx + add_bottom;
			}
		}

		qsort((void *)fs_listlt, listltidx, sizeof(FrameStore*), compare_fs_by_lt_pic_idx_asc);

		gen_pic_list_from_frame_list(currPicStructure, fs_listlt, listltidx, 
			img->listX[0], &img->listXsize[0], 1);
		gen_pic_list_from_frame_list(currPicStructure, fs_listlt, listltidx, 
			img->listX[1], &img->listXsize[1], 1);

	}
#endif//INTERLACE
  }

 

  if ((img->listXsize[0] == img->listXsize[1]) && (img->listXsize[0] > 1))
  {
    // check if lists are identical, if yes swap first two elements of img->listX[1]
    diff=0;
    for (j = 0; j< img->listXsize[0]; j++)
    {
      if (img->listX[0][j]!=img->listX[1][j])
        diff=1;
    }
    if (!diff)
    {
      tmp_s = img->listX[1][0];
      img->listX[1][0]=img->listX[1][1];
      img->listX[1][1]=tmp_s;
    }
  }

  img->listXsize[1] = min (img->listXsize[1], img->maxListSize);
  for (i=img->listXsize[1]; i< img->maxListSize; i++)
  {
    img->listX[1][i] = NULL;
  }
#endif //FEATURE_BFRAME

  // set max size
  img->listXsize[0] = min (img->listXsize[0], img->maxListSize);

  // set the unused list entries to NULL
#if ROBUST_CHECK_REF_BUFFER//hbfTodo20060628,if the img->num_ref_idx_l0_active is changed from 5 to 2,and then 4,that will cause error
  for (i=img->listXsize[0]; i< img->maxListSize ; i++)
  {
    img->listX[0][i] = NULL;
  }
#endif//

	// start reorder_lists
	if ((currSliceType != I_SLICE)&&(currSliceType != SI_SLICE))
	{
		
		if (currSlice->ref_pic_list_reordering_flag_l0)
		{
			reorder_ref_pic_list(img,img->listX[0], &img->listXsize[0], 
			   img->num_ref_idx_l0_active - 1, //hxy2010 TBD
			   currSlice->remapping_of_pic_nums_idc_l0, 
			   currSlice->abs_diff_pic_num_minus1_l0, 
			   currSlice->long_term_pic_idx_l0);CHECK_ERR_RET_VOID
		}
		
		
		// that's a definition
		if(img->listX[0][img->num_ref_idx_l0_active-1])
			img->listXsize[0] = img->num_ref_idx_l0_active;

		if (NULL == img->listX[0][img->listXsize[0]-1])
		{
			AVD_ERROR_CHECK2(img,"NULL==img->listX[0][img->listXsize[0]-1]",ERROR_RemappingPicIdc);
		}

	}
#if FEATURE_BFRAME
	if (currSliceType == B_SLICE)
	{
		if (currSlice->ref_pic_list_reordering_flag_l1)
		{
			reorder_ref_pic_list(img,img->listX[1], &img->listXsize[1], 
			   img->num_ref_idx_l1_active - 1, 
			   currSlice->remapping_of_pic_nums_idc_l1, 
			   currSlice->abs_diff_pic_num_minus1_l1, 
			   currSlice->long_term_pic_idx_l1);CHECK_ERR_RET_VOID
		}
		if (img->listX[1][img->num_ref_idx_l1_active-1])
			img->listXsize[1] = img->num_ref_idx_l1_active;
		if (NULL == img->listX[1][img->listXsize[1]-1])
		{
			AVD_ERROR_CHECK2(img,"NULL==img->listX[1][img->listXsize[1]-1]",ERROR_RemappingPicIdc);
		}
	}

#endif //FEATURE_BFRAME

	//remove the following code to free_slice,which is safer
	//if (currSlice->remapping_of_pic_nums_idc_l0) 
	//	free_ref_pic_list0_reordering_buffer(currSlice);
}

/*!
************************************************************************
* \brief
*    Initilaize img->listX[2..5] from lists 0 and 1
*    img->listX[2]: list0 for current_field==top
*    img->listX[3]: list1 for current_field==top
*    img->listX[4]: list0 for current_field==bottom
*    img->listX[5]: list1 for current_field==bottom
*
************************************************************************
*/
#if FEATURE_INTERLACE
void init_mbaff_lists(ImageParameters *img)
{
	avdNativeUInt j, i, i2;
	DecodedPictureBuffer *dpb = img->dpb;

	for (i=2;i<6;i++)
	{
		j = (dpb->size<<1);
		do {
			img->listX[i][j] = NULL;
		} while (--j);
		img->listXsize[i]=0;
	}

	for (i=0; i<img->listXsize[0]; i++)
	{
		i2 = (i<<1);
		img->listX[2][i2]  =img->listX[0][i]->top_field;
		img->listX[2][i2+1]=img->listX[0][i]->bottom_field;
		img->listX[4][i2]  =img->listX[0][i]->bottom_field;
		img->listX[4][i2+1]=img->listX[0][i]->top_field;
	}
	img->listXsize[2]=img->listXsize[4]=img->listXsize[0] * 2;

	for (i=0; i<img->listXsize[1]; i++)
	{
		i2 = (i<<1);
		img->listX[3][i2]  =img->listX[1][i]->top_field;
		img->listX[3][i2+1]=img->listX[1][i]->bottom_field;
		img->listX[5][i2]  =img->listX[1][i]->bottom_field;
		img->listX[5][i2+1]=img->listX[1][i]->top_field;
	}
	img->listXsize[3]=img->listXsize[5]=img->listXsize[1] * 2;
}
#endif //VOI_H264D_NON_BASELINE
 
 /*!
 ************************************************************************
 * \brief
 *    Returns short term pic with given picNum
 *
 ************************************************************************
 */
static StorablePicture*  get_short_term_pic(ImageParameters *img,avdInt32 picNum)
{
	avdNativeInt i;
	DecodedPictureBuffer *dpb = img->dpb;
	FrameStore **fs_ref = dpb->fs_ref;
	TPOCInfo* pocInfo = img->pocInfo;
	TSpecitialInfo* specialInfo = img->specialInfo;
#if FEATURE_INTERLACE
    if (specialInfo->structure==FRAME)
#endif //VOI_H264D_NON_BASELINE
    {
		for (i= dpb->ref_frames_in_buffer - 1; i >= 0; i--)
			if (fs_ref[i]->is_reference == 3)
				if ((!fs_ref[i]->frame->is_long_term)&&
					(fs_ref[i]->frame->pic_num == picNum))
					return fs_ref[i]->frame;
    }
#if FEATURE_INTERLACE
    else//below are fields
    {
		for (i= dpb->ref_frames_in_buffer - 1; i >= 0; i--){
			if (fs_ref[i]->is_reference & 1)
				if ((!fs_ref[i]->top_field->is_long_term)&&
					(fs_ref[i]->top_field->pic_num == picNum))
					return fs_ref[i]->top_field;
			if (fs_ref[i]->is_reference & 2)
					if ((!fs_ref[i]->bottom_field->is_long_term)&&
						(fs_ref[i]->bottom_field->pic_num == picNum))
						return fs_ref[i]->bottom_field;
		}
    }
#endif //VOI_H264D_NON_BASELINE
	return NULL;
}

/*!
 ************************************************************************
 * \brief
 *    Returns short term pic with given LongtermPicNum
 *
 ************************************************************************
 */
static StorablePicture*  get_long_term_pic(ImageParameters *img,avdInt32 LongtermPicNum)
{
	avdNativeInt i;
	DecodedPictureBuffer *dpb = img->dpb;
	TSpecitialInfo* specialInfo = img->specialInfo;
#if FEATURE_INTERLACE
    if (specialInfo->structure==FRAME)
#endif //VOI_H264D_NON_BASELINE
    {
		for (i = dpb->ltref_frames_in_buffer - 1; i >= 0; i--)
			if (dpb->fs_ltref[i]->is_reference == 3)
				if ((dpb->fs_ltref[i]->frame->is_long_term)&&
					(dpb->fs_ltref[i]->frame->long_term_pic_num == LongtermPicNum))
					return dpb->fs_ltref[i]->frame;
    }
#if FEATURE_INTERLACE
    else//below are fields
    {
		for (i = dpb->ltref_frames_in_buffer - 1; i >= 0; i--){
			if (dpb->fs_ltref[i]->is_reference & 1)
				if ((dpb->fs_ltref[i]->top_field->is_long_term)&&
					(dpb->fs_ltref[i]->top_field->long_term_pic_num == LongtermPicNum))
					return dpb->fs_ltref[i]->top_field;
			if (dpb->fs_ltref[i]->is_reference & 2)
				if ((dpb->fs_ltref[i]->bottom_field->is_long_term)&&
					(dpb->fs_ltref[i]->bottom_field->long_term_pic_num == LongtermPicNum))
					return dpb->fs_ltref[i]->bottom_field;
		}
    }
#endif //VOI_H264D_NON_BASELINE
	return NULL;
}

/*!
 ************************************************************************
 * \brief
 *    Reordering process for short-term reference pictures
 *
 ************************************************************************
 */
static void reorder_short_term(ImageParameters *img,StorablePicture **RefPicListX, 
							   avdNativeInt num_ref_idx_lX_active_minus1, 
							   avdInt32 picNumLX, avdNativeInt *refIdxLX)
{
  avdNativeInt cIdx, nIdx;

  StorablePicture *picLX;

  picLX = get_short_term_pic(img,picNumLX);
  if(picLX==NULL)
  {
	  AVD_ERROR_CHECK2(img,"get_short_term_pic NULL,there is frame gap\n",100);
	  return;
  }
#if 1//hxy2010 TBD
  for( cIdx = num_ref_idx_lX_active_minus1+1; cIdx > *refIdxLX; cIdx-- )
    RefPicListX[ cIdx ] = RefPicListX[ cIdx - 1];
  
  RefPicListX[ (*refIdxLX)++ ] = picLX;

  nIdx = *refIdxLX;

  for( cIdx = *refIdxLX; cIdx <= num_ref_idx_lX_active_minus1+1; cIdx++ )
    if (RefPicListX[ cIdx ])
	{
      if( (RefPicListX[ cIdx ]->is_long_term ) ||  (RefPicListX[ cIdx ]->pic_num != picNumLX ))
          RefPicListX[ nIdx++ ] = RefPicListX[ cIdx ];
	}
	
#else
  for( cIdx = num_ref_idx_lX_active_minus1; cIdx > *refIdxLX; cIdx-- )
	  RefPicListX[ cIdx ] = RefPicListX[ cIdx - 1];

  if(picLX)
	RefPicListX[ (*refIdxLX)++ ] = picLX;

  nIdx = *refIdxLX;

  for( cIdx = *refIdxLX; cIdx <= num_ref_idx_lX_active_minus1; cIdx++ )
	  if (RefPicListX[ cIdx ])
	  {
		  if( (RefPicListX[ cIdx ]->is_long_term ) ||  (RefPicListX[ cIdx ]->pic_num != picNumLX ))
			  RefPicListX[ nIdx++ ] = RefPicListX[ cIdx ];
	  }
	  else
		  break;
#endif

}


/*!
 ************************************************************************
 * \brief
 *    Reordering process for short-term reference pictures
 *
 ************************************************************************
 */
static void reorder_long_term(ImageParameters *img,StorablePicture **RefPicListX, 
							  avdNativeInt num_ref_idx_lX_active_minus1, 
							  avdInt32 LongTermPicNum, avdNativeInt *refIdxLX)
{
  avdNativeInt cIdx, nIdx;

  StorablePicture *picLX;

  picLX = get_long_term_pic(img,LongTermPicNum);
  if(picLX==NULL)
  {
	  AVD_ERROR_CHECK2(img,"get_long_term_pic NULL,there is frame gap\n",100);
	  return;
  }
#if 1//hxy2010 TBD
  for( cIdx = num_ref_idx_lX_active_minus1+1; cIdx > *refIdxLX; cIdx-- )
    RefPicListX[ cIdx ] = RefPicListX[ cIdx - 1];
  
  RefPicListX[ (*refIdxLX)++ ] = picLX;

  nIdx = *refIdxLX;

  for( cIdx = *refIdxLX; cIdx <= num_ref_idx_lX_active_minus1+1; cIdx++ )
  {
	  //RefPicListX[ cIdx ] && 
	  if(RefPicListX[ cIdx ])
	  {
		  if(( ( !RefPicListX[ cIdx ]->is_long_term ) ||  (RefPicListX[ cIdx ]->long_term_pic_num != LongTermPicNum )))
			  RefPicListX[ nIdx++ ] = RefPicListX[ cIdx ];
	  }
	
  }
#else
  for( cIdx = num_ref_idx_lX_active_minus1; cIdx > *refIdxLX; cIdx-- )
	  RefPicListX[ cIdx ] = RefPicListX[ cIdx - 1];

  if(picLX)
	RefPicListX[ (*refIdxLX)++ ] = picLX;

  nIdx = *refIdxLX;

  for( cIdx = *refIdxLX; cIdx <= num_ref_idx_lX_active_minus1; cIdx++ )
  {
	  //RefPicListX[ cIdx ] && 
	  if(RefPicListX[ cIdx ]==NULL)
		  AVD_ERROR_CHECK2(img,"reorder_long_term,RefPicListX[ cIdx ]==NULL",ERROR_REFFrameIsNULL);
	  if(( ( !RefPicListX[ cIdx ]->is_long_term ) ||  (RefPicListX[ cIdx ]->long_term_pic_num != LongTermPicNum )))
		  RefPicListX[ nIdx++ ] = RefPicListX[ cIdx ];
  }
#endif
}


/*!
 ************************************************************************
 * \brief
 *    Reordering process for reference picture lists
 *
 ************************************************************************
 */
void reorder_ref_pic_list(ImageParameters *img,StorablePicture **list, avdNativeUInt *list_size, 
						  avdNativeInt num_ref_idx_lX_active_minus1, avdNativeInt *remapping_of_pic_nums_idc, 
						  avdNativeInt *abs_diff_pic_num_minus1, avdNativeInt *long_term_pic_idx)
{
  avdNativeInt i;
  avdNativeInt refIdxLX = 0;

  avdInt32 maxPicNum, currPicNum, picNumLXNoWrap, picNumLXPred, picNumLX;
  TSpecitialInfo	*specialInfo = img->specialInfo;
  if (specialInfo->structure==FRAME)
  {
    maxPicNum  = specialInfo->MaxFrameNum;
    currPicNum = specialInfo->frame_num;
  }
  else
  {
	  maxPicNum  = 2 * specialInfo->MaxFrameNum;
	  currPicNum = 2 * specialInfo->frame_num + 1;
  }


  picNumLXPred = currPicNum;

  for (i=0; remapping_of_pic_nums_idc[i]!=3; i++)
  {
    if (remapping_of_pic_nums_idc[i]>3)
    {
	    remapping_of_pic_nums_idc[i] = 3;
		AVD_ERROR_CHECK2(img,"Invalid remapping_of_pic_nums_idc command", ERROR_RemappingPicIdc);
    }

    if (remapping_of_pic_nums_idc[i] < 2)
    {
      if (remapping_of_pic_nums_idc[i] == 0)
      {
        if( picNumLXPred - ( abs_diff_pic_num_minus1[i] + 1 ) < 0 )
          picNumLXNoWrap = picNumLXPred - ( abs_diff_pic_num_minus1[i] + 1 ) + maxPicNum;
        else
          picNumLXNoWrap = picNumLXPred - ( abs_diff_pic_num_minus1[i] + 1 );
      }
      else // (remapping_of_pic_nums_idc[i] == 1)
      {
        if( picNumLXPred + ( abs_diff_pic_num_minus1[i] + 1 )  >=  maxPicNum )
          picNumLXNoWrap = picNumLXPred + ( abs_diff_pic_num_minus1[i] + 1 ) - maxPicNum;
        else
          picNumLXNoWrap = picNumLXPred + ( abs_diff_pic_num_minus1[i] + 1 );
      }
      picNumLXPred = picNumLXNoWrap;

      if( picNumLXNoWrap > currPicNum )
        picNumLX = picNumLXNoWrap - maxPicNum;
      else
        picNumLX = picNumLXNoWrap;

      reorder_short_term(img,list, num_ref_idx_lX_active_minus1, picNumLX, &refIdxLX);
    }
    else //(remapping_of_pic_nums_idc[i] == 2)
    {
      reorder_long_term(img,list, num_ref_idx_lX_active_minus1, long_term_pic_idx[i], &refIdxLX);
    }
    CHECK_ERR_RET_VOID
  }
  // that's a definition
  *list_size = num_ref_idx_lX_active_minus1 + 1;
}



/*!
 ************************************************************************
 * \brief
 *    Update the list of frame stores that contain reference frames/fields
 *
 ************************************************************************
 */
void update_ref_list(ImageParameters *img)
{
  avdNativeUInt i, j;
  DecodedPictureBuffer *dpb = img->dpb;
  FrameStore **fs_ref = dpb->fs_ref;
  for (i=0, j=0; i<dpb->used_size; i++)
  {
    if (is_short_term_reference(img,dpb->fs[i]))
    {
      fs_ref[j++]=dpb->fs[i];
	  AvdLog(DUMP_SLICE,(img->mbsProcessor[0]->mb_y&1),"\nupdate_ref_list_%d", dpb->fs[i]->poc);
    }
  }

  dpb->ref_frames_in_buffer = j;

  while (j<dpb->size)
  {
    fs_ref[j++]=NULL;
  }
}


/*!
 ************************************************************************
 * \brief
 *    Update the list of frame stores that contain long-term reference 
 *    frames/fields
 *
 ************************************************************************
 */
void update_ltref_list(ImageParameters *img)
{
  avdNativeUInt i, j;
  DecodedPictureBuffer *dpb = img->dpb;
  for (i=0, j=0; i<dpb->used_size; i++)
  {
    if (is_long_term_reference(img,dpb->fs[i]))
    {
      dpb->fs_ltref[j++]=dpb->fs[i];
	  AvdLog(DUMP_SLICE,(img->mbsProcessor[0]->mb_y&1),"\nupdate_ltref_list_%d", dpb->fs[i]->poc);
    }
  }

  dpb->ltref_frames_in_buffer=j;

  while (j < dpb->size)
  {
    dpb->fs_ltref[j++]=NULL;
  }
}

/*!
 ************************************************************************
 * \brief
 *    Perform Memory management for idr pictures
 *
 ************************************************************************
 */
static void idr_memory_management(ImageParameters *img,StorablePicture* p)
{
  avdNativeInt i;
  DecodedPictureBuffer *dpb = img->dpb;

  assert (p->idr_flag);

  if (p->no_output_of_prior_pics_flag)
  {
    FrameStore **fs_ref = dpb->fs_ref;
    // free all stored pictures
    for (i = dpb->used_size - 1; i >= 0; i--)
    {
	  //streams with no_output_of_prior_pics_flag set failed since the FrameStore may
	  //not be deleted, only its content.
      free_frame_store(img,dpb->fs[i]);
	  dpb->fs[i] = alloc_frame_store(img);
    }
    for (i=0; i<dpb->ref_frames_in_buffer; i++)
    {
      fs_ref[i]=NULL;
    }
    for (i=0; i<dpb->ltref_frames_in_buffer; i++)
    {
      dpb->fs_ltref[i]=NULL;
    }
    dpb->used_size=0;
  }
  else
  {
    flush_dpb(img);CHECK_ERR_RET_VOID
  }
  dpb->last_picture = NULL;

  update_ref_list(img);
  update_ltref_list(img);
  dpb->last_output_poc = VO_INT_MIN;
  
  if (p->long_term_reference_flag)
  {
    dpb->max_long_term_pic_idx = 0;
    p->is_long_term           = 1;
    p->long_term_frame_idx    = 0;
  }
  else
  {
    dpb->max_long_term_pic_idx = -1;
    p->is_long_term           = 0;
  }
}

/*!
 ************************************************************************
 * \brief
 *    Perform Sliding window decoded reference picture marking process
 *
 ************************************************************************
 */
static void sliding_window_memory_management(ImageParameters *img,StorablePicture* p)
{
  avdNativeUInt i;
  DecodedPictureBuffer *dpb = img->dpb;

  assert (!p->idr_flag);
  // if this is a reference pic with sliding sliding window, unmark first ref frame
  if (dpb->ref_frames_in_buffer==img->active_sps->num_ref_frames - dpb->ltref_frames_in_buffer)
  {
    for (i=0; i<dpb->used_size;i++)
    {
      if (dpb->fs[i]->is_reference  && (!(dpb->fs[i]->is_long_term)))
      {
        unmark_for_reference(img,dpb->fs[i]);
        update_ref_list(img);
        break;
      }
    }
  }

  p->is_long_term = 0;
}

/*!
 ************************************************************************
 * \brief
 *    Calculate picNumX
 ************************************************************************
 */
static avdInt32 get_pic_num_x (ImageParameters *img,StorablePicture *p, avdInt32 difference_of_pic_nums_minus1)
{
#if FEATURE_INTERLACE
  avdInt32 currPicNum;
  if (p->structure == FRAME)
    currPicNum = p->frame_num;
  else 
    currPicNum = 2 * p->frame_num + 1;
  
  return currPicNum - (difference_of_pic_nums_minus1 + 1);
#else
  return p->frame_num - (difference_of_pic_nums_minus1 + 1);
#endif //VOI_H264D_NON_BASELINE
}


/*!
 ************************************************************************
 * \brief
 *    Adaptive Memory Management: Mark short term picture unused
 ************************************************************************
 */
static void mm_unmark_short_term_for_reference(ImageParameters *img,StorablePicture *p, 
											   avdInt32 difference_of_pic_nums_minus1)
{
  avdInt32 picNumX;

  avdNativeUInt i;
  DecodedPictureBuffer *dpb = img->dpb;
  FrameStore **fs_ref = dpb->fs_ref;

  picNumX = get_pic_num_x(img,p, difference_of_pic_nums_minus1);

  for (i=0; i<dpb->ref_frames_in_buffer; i++)
  {
    if (p->structure == FRAME)
    {
      if ((fs_ref[i]->is_reference==3) && (fs_ref[i]->is_long_term==0))
      {
        if (fs_ref[i]->frame&&fs_ref[i]->frame->pic_num == picNumX)
        {
          unmark_for_reference(img,fs_ref[i]);
          return;
        }
      }
    }
#if FEATURE_INTERLACE
    else//below are fields
    {
      if ((fs_ref[i]->is_reference & 1) && (!(fs_ref[i]->is_long_term & 1)))
      {
        if (fs_ref[i]->top_field->pic_num == picNumX)
        {
          fs_ref[i]->top_field->used_for_reference = 0;
          fs_ref[i]->is_reference &= 2;
          if (fs_ref[i]->is_used == 3&&fs_ref[i]->frame)
          {
            fs_ref[i]->frame->used_for_reference = 0;
          }
          return;
        }
      }
      if ((fs_ref[i]->is_reference & 2) && (!(fs_ref[i]->is_long_term & 2)))
      {
        if (fs_ref[i]->bottom_field->pic_num == picNumX)
        {
          fs_ref[i]->bottom_field->used_for_reference = 0;
          fs_ref[i]->is_reference &= 1;
          if (fs_ref[i]->is_used == 3&&fs_ref[i]->frame)
          {
            fs_ref[i]->frame->used_for_reference = 0;
          }
          return;
        }
      }
    }
#endif //VOI_H264D_NON_BASELINE
  }
}


/*!
 ************************************************************************
 * \brief
 *    Adaptive Memory Management: Mark long term picture unused
 ************************************************************************
 */
static void mm_unmark_long_term_for_reference(ImageParameters *img,StorablePicture *p, avdInt32 long_term_pic_num)
{
  avdNativeInt i;
  DecodedPictureBuffer *dpb = img->dpb;
  for (i = dpb->ltref_frames_in_buffer - 1; i >= 0; i--)
  {
    if (p->structure == FRAME)
    {
      if ((dpb->fs_ltref[i]->is_reference==3) && (dpb->fs_ltref[i]->is_long_term==3))
      {
        if (dpb->fs_ltref[i]->frame->long_term_pic_num == long_term_pic_num)
        {
          unmark_for_long_term_reference(dpb->fs_ltref[i]);
        }
      }
    }
#if FEATURE_INTERLACE
    else//below are fields
    {
      if ((dpb->fs_ltref[i]->is_reference & 1) && ((dpb->fs_ltref[i]->is_long_term & 1)))
      {
        if (dpb->fs_ltref[i]->top_field->long_term_pic_num == long_term_pic_num)
        {
          dpb->fs_ltref[i]->top_field->used_for_reference = 0;
          dpb->fs_ltref[i]->top_field->is_long_term = 0;
          dpb->fs_ltref[i]->is_reference &= 2;
          dpb->fs_ltref[i]->is_long_term &= 2;
          if (dpb->fs_ltref[i]->is_used == 3)
          {
            dpb->fs_ltref[i]->frame->used_for_reference = 0;
            dpb->fs_ltref[i]->frame->is_long_term = 0;
          }
          return;
        }
      }
      if ((dpb->fs_ltref[i]->is_reference & 2) && ((dpb->fs_ltref[i]->is_long_term & 2)))
      {
        if (dpb->fs_ltref[i]->bottom_field->long_term_pic_num == long_term_pic_num)
        {
          dpb->fs_ltref[i]->bottom_field->used_for_reference = 0;
          dpb->fs_ltref[i]->bottom_field->is_long_term = 0;
          dpb->fs_ltref[i]->is_reference &= 1;
          dpb->fs_ltref[i]->is_long_term &= 1;
          if (dpb->fs_ltref[i]->is_used == 3)
          {
            dpb->fs_ltref[i]->frame->used_for_reference = 0;
            dpb->fs_ltref[i]->frame->is_long_term = 0;
          }
          return;
        }
      }
    }
#endif //VOI_H264D_NON_BASELINE
  }
}


/*!
 ************************************************************************
 * \brief
 *    Mark a long-term reference frame or complementary field pair unused for referemce
 ************************************************************************
 */
static void unmark_long_term_frame_for_reference_by_frame_idx(ImageParameters *img,avdInt32 long_term_frame_idx)
{
	avdNativeInt i;
	DecodedPictureBuffer *dpb = img->dpb;
	for(i = dpb->ltref_frames_in_buffer - 1; i >= 0; i--)
	{
		if (dpb->fs_ltref[i]->long_term_frame_idx == long_term_frame_idx)
			unmark_for_long_term_reference(dpb->fs_ltref[i]);
	}
}

/*!
 ************************************************************************
 * \brief
 *    Mark a long-term reference field unused for reference only if it's not
 *    the complementary field of the picture indicated by picNumX
 ************************************************************************
 */
#if FEATURE_INTERLACE
static void unmark_long_term_field_for_reference_by_frame_idx(ImageParameters *img,PictureStructure structure, 
															  avdNativeInt long_term_frame_idx, 
															  avdNativeInt mark_current, 
															  avdNativeUInt curr_frame_num)
{
  avdNativeUInt i;
  DecodedPictureBuffer *dpb = img->dpb;

  if(structure==FRAME)
  {
	  AVD_ERROR_CHECK2(img,"structure==FRAME: unmark_long_term_field_for_reference_by_frame_idx",100);
	  return;
  }

  for(i=0; i<dpb->ltref_frames_in_buffer; i++)
  {
    if (dpb->fs_ltref[i]->long_term_frame_idx == long_term_frame_idx)
    {
      if (structure == TOP_FIELD)
      {
        if ((dpb->fs_ltref[i]->is_long_term == 3))
        {
          unmark_for_long_term_reference(dpb->fs_ltref[i]);
        }
        else
        {
          if ((dpb->fs_ltref[i]->is_long_term == 1))
          {
            unmark_for_long_term_reference(dpb->fs_ltref[i]);
          }
          else
          {
            if (mark_current)
            {
              if (dpb->last_picture)
              {
                if ( ( dpb->last_picture != dpb->fs_ltref[i] )|| dpb->last_picture->frame_num != curr_frame_num)
                  unmark_for_long_term_reference(dpb->fs_ltref[i]);
              }
              else
              {
                unmark_for_long_term_reference(dpb->fs_ltref[i]);
              }
            }
          }
        }
      }
      if (structure == BOTTOM_FIELD)
      {
        if ((dpb->fs_ltref[i]->is_long_term == 3))
        {
          unmark_for_long_term_reference(dpb->fs_ltref[i]);
        }
        else
        {
          if ((dpb->fs_ltref[i]->is_long_term == 2))
          {
            unmark_for_long_term_reference(dpb->fs_ltref[i]);
          }
          else
          {
            if (mark_current)
            {
              if (dpb->last_picture)
              {
                if ( ( dpb->last_picture != dpb->fs_ltref[i] )|| dpb->last_picture->frame_num != curr_frame_num)
                  unmark_for_long_term_reference(dpb->fs_ltref[i]);
              }
              else
              {
                unmark_for_long_term_reference(dpb->fs_ltref[i]);
              }
            }
          }
        }
      }
    }
  }
}
#endif //VOI_H264D_NON_BASELINE


/*!
 ************************************************************************
 * \brief
 *    mark a picture as long-term reference
 ************************************************************************
 */
static void mark_pic_long_term(ImageParameters *img,StorablePicture* p, avdNativeInt long_term_frame_idx, avdInt32 picNumX)
{
  avdNativeUInt i;
  avdNativeInt add_top, add_bottom;
  DecodedPictureBuffer *dpb = img->dpb;
  FrameStore **fs_ref = dpb->fs_ref;

  if (p->structure == FRAME)
  {
    for (i=0; i<dpb->ref_frames_in_buffer; i++)
    {
      if (fs_ref[i]->is_reference == 3)
      {
        if ((!fs_ref[i]->frame->is_long_term)&&(fs_ref[i]->frame->pic_num == picNumX))
        {
#if FEATURE_INTERLACE
          
		  
		  if(fs_ref[i]->top_field)
		  {
			  fs_ref[i]->top_field->long_term_frame_idx = long_term_frame_idx;
			  fs_ref[i]->top_field->long_term_pic_num= long_term_frame_idx;
			  fs_ref[i]->top_field->is_long_term = 1;
		  }
		  if(fs_ref[i]->bottom_field)
		  {
			  fs_ref[i]->bottom_field->long_term_frame_idx = long_term_frame_idx;
			  fs_ref[i]->bottom_field->long_term_pic_num = long_term_frame_idx;
			  fs_ref[i]->bottom_field->is_long_term = 1;
		  }
         
#endif // VOI_H264D_NON_BASELINE
          fs_ref[i]->long_term_frame_idx 
			  = fs_ref[i]->frame->long_term_frame_idx
              = long_term_frame_idx;
          fs_ref[i]->frame->long_term_pic_num 
              = long_term_frame_idx;
          fs_ref[i]->frame->is_long_term 
              = 1;

          fs_ref[i]->is_long_term = 3;
          return;
        }
      }
    }
    printf ("Warning: reference frame for long term marking not found\n");
  }
#if FEATURE_INTERLACE
  else//below are fields
  {
    if (p->structure == TOP_FIELD)
    {
      add_top    = 1;
      add_bottom = 0;
    }
    else
    {
      add_top    = 0;
      add_bottom = 1;
    }
    for (i=0; i<dpb->ref_frames_in_buffer; i++)
    {
      if (fs_ref[i]->is_reference & 1)
      {
        if ((!fs_ref[i]->top_field->is_long_term)&&
			(fs_ref[i]->top_field->pic_num == picNumX))
        {
          if ((fs_ref[i]->is_long_term) && 
			  (fs_ref[i]->long_term_frame_idx != long_term_frame_idx))
          {
              printf ("Warning: assigning long_term_frame_idx different from other field\n");
          }

          fs_ref[i]->long_term_frame_idx = fs_ref[i]->top_field->long_term_frame_idx 
                                             = long_term_frame_idx;
          fs_ref[i]->top_field->long_term_pic_num = 2 * long_term_frame_idx + add_top;
          fs_ref[i]->top_field->is_long_term = 1;
          fs_ref[i]->is_long_term |= 1;
          if (fs_ref[i]->is_long_term == 3)
          {
            fs_ref[i]->frame->is_long_term = 1;
            fs_ref[i]->frame->long_term_frame_idx = 
				fs_ref[i]->frame->long_term_pic_num = long_term_frame_idx;
          }
          return;
        }
      }
      if (fs_ref[i]->is_reference & 2)
      {
        if ((!fs_ref[i]->bottom_field->is_long_term)&&
			(fs_ref[i]->bottom_field->pic_num == picNumX))
        {
          if ((fs_ref[i]->is_long_term) && (fs_ref[i]->long_term_frame_idx 
			  != long_term_frame_idx))
          {
              printf ("Warning: assigning long_term_frame_idx different from other field\n");
          }

          fs_ref[i]->long_term_frame_idx = fs_ref[i]->bottom_field->long_term_frame_idx 
                                             = long_term_frame_idx;
          fs_ref[i]->bottom_field->long_term_pic_num = 2 * long_term_frame_idx + add_top;
          fs_ref[i]->bottom_field->is_long_term = 1;
          fs_ref[i]->is_long_term |= 2;
          if (fs_ref[i]->is_long_term == 3&&fs_ref[i]->frame)
          {
            fs_ref[i]->frame->is_long_term = 1;
            fs_ref[i]->frame->long_term_frame_idx = fs_ref[i]->frame->long_term_pic_num 
				= long_term_frame_idx;
          }
          return;
        }
      }
    }
    printf ("Warning: reference field for long term marking not found\n");
  }
#endif //VOI_H264D_NON_BASELINE
}


/*!
 ************************************************************************
 * \brief
 *    Assign a long term frame index to a short term picture
 ************************************************************************
 */
static void mm_assign_long_term_frame_idx(ImageParameters *img,StorablePicture* p, 
					avdInt32 difference_of_pic_nums_minus1, avdNativeInt long_term_frame_idx)
{
  avdInt32 picNumX;
  DecodedPictureBuffer *dpb = img->dpb;
  FrameStore **fs_ref = dpb->fs_ref;

  picNumX = get_pic_num_x(img,p, difference_of_pic_nums_minus1);

  // remove frames/fields with same long_term_frame_idx
#if FEATURE_INTERLACE
  if (p->structure == FRAME)
#endif //VOI_H264D_NON_BASELINE
  {
    unmark_long_term_frame_for_reference_by_frame_idx(img,long_term_frame_idx);
  }
#if FEATURE_INTERLACE
  else//below are fields
  {
    unsigned i;
    PictureStructure structure = FRAME;

    for (i=0; i<dpb->ref_frames_in_buffer; i++)
    {
      if (fs_ref[i]->is_reference & 1)
      {
        if (fs_ref[i]->top_field->pic_num == picNumX)
        {
          structure = TOP_FIELD;
          break;
        }
      }
      if (fs_ref[i]->is_reference & 2)
      {
        if (fs_ref[i]->bottom_field->pic_num == picNumX)
        {
          structure = BOTTOM_FIELD;
          break;
        }
      }
    }
    if (structure==FRAME)
    {
      AVD_ERROR_CHECK2(img,"field for long term marking not found",200);
    }
    
    unmark_long_term_field_for_reference_by_frame_idx(img,structure, long_term_frame_idx, 0, 0);
  }
#endif //VOI_H264D_NON_BASELINE

  mark_pic_long_term(img,p, long_term_frame_idx, picNumX);
}

/*!
 ************************************************************************
 * \brief
 *    Set new max long_term_frame_idx
 ************************************************************************
 */
void mm_update_max_long_term_frame_idx(ImageParameters *img,avdNativeInt max_long_term_frame_idx_plus1)
{
  avdNativeInt i;
  DecodedPictureBuffer *dpb = img->dpb;
  FrameStore **fs_ltref = dpb->fs_ltref;

  dpb->max_long_term_pic_idx = max_long_term_frame_idx_plus1 - 1;

  // check for invalid frames
  for (i=dpb->ltref_frames_in_buffer - 1; i >= 0; i--)
  {
    if (fs_ltref[i]->long_term_frame_idx > dpb->max_long_term_pic_idx)
    {
      unmark_for_long_term_reference(fs_ltref[i]);
    }
  }
}


/*!
 ************************************************************************
 * \brief
 *    Mark all long term reference pictures unused for reference
 ************************************************************************
 */
static void mm_unmark_all_long_term_for_reference (ImageParameters *img)
{
  mm_update_max_long_term_frame_idx(img,0);
}

/*!
 ************************************************************************
 * \brief
 *    Mark all short term reference pictures unused for reference
 ************************************************************************
 */
static void mm_unmark_all_short_term_for_reference (ImageParameters *img)
{
  avdNativeInt i;
  DecodedPictureBuffer *dpb = img->dpb;
  FrameStore **fs_ref = dpb->fs_ref;
  for (i=dpb->ref_frames_in_buffer - 1; i >= 0; i--)
  {
    unmark_for_reference(img,fs_ref[i]);
  }
  update_ref_list(img);
}

/*!
 ************************************************************************
 * \brief
 *    Mark the current picture used for long term reference
 ************************************************************************
 */
static void mm_mark_current_picture_long_term(ImageParameters *img,StorablePicture *p, avdNativeInt long_term_frame_idx)
{
  // remove long term pictures with same long_term_frame_idx
#if FEATURE_INTERLACE
  if (p->structure == FRAME)
#endif //VOI_H264D_NON_BASELINE
  {
    unmark_long_term_frame_for_reference_by_frame_idx(img,long_term_frame_idx);
  }
#if FEATURE_INTERLACE
  else//below are fields
  {
    unmark_long_term_field_for_reference_by_frame_idx(img,p->structure, long_term_frame_idx, 
		1, p->pic_num);
  }
#endif //VOI_H264D_NON_BASELINE

  p->is_long_term = 1;
  p->long_term_frame_idx = long_term_frame_idx;
}


/*!
 ************************************************************************
 * \brief
 *    Perform Adaptive memory control decoded reference picture marking process
 ************************************************************************
 */
static void adaptive_memory_management(ImageParameters *img,StorablePicture* p)
{
  DecRefPicMarking_t *tmp_drpm;
  TSpecitialInfo	*specialInfo = img->specialInfo;
  specialInfo->last_has_mmco_5 = 0;

  if(p->idr_flag||!p->adaptive_ref_pic_buffering_flag)
  {
	  AVD_ERROR_CHECK2(img,"adaptive_memory_management:idr flag or not adaptive\n",100);
	  return;
  }

  while (p->dec_ref_pic_marking_buffer)
  {
    tmp_drpm = p->dec_ref_pic_marking_buffer;
	//AvdLog2(LL_INFO,"AMM:%d\n",tmp_drpm->memory_management_control_operation);

    switch (tmp_drpm->memory_management_control_operation)
    {
      case 0:
        if (tmp_drpm->Next != NULL)
        {
          AVD_ERROR_CHECK2(img,"memory_management_control_operation = 0 not last operation in buffer", 500);
        }
        break;
      case 1:
        mm_unmark_short_term_for_reference(img,p, tmp_drpm->difference_of_pic_nums_minus1);
        update_ref_list(img);
        break;
      case 2:
        mm_unmark_long_term_for_reference(img,p, tmp_drpm->long_term_pic_num);
        update_ltref_list(img);
        break;
      case 3:
        mm_assign_long_term_frame_idx(img,p, tmp_drpm->difference_of_pic_nums_minus1, 
			tmp_drpm->long_term_frame_idx);CHECK_ERR_RET_VOID
        update_ref_list(img);
        update_ltref_list(img);
        break;
      case 4:
        mm_update_max_long_term_frame_idx (img,tmp_drpm->max_long_term_frame_idx_plus1);
        update_ltref_list(img);
        break;
      case 5:
        mm_unmark_all_short_term_for_reference(img);
        mm_unmark_all_long_term_for_reference(img);
        specialInfo->last_has_mmco_5 = 1;
        break;
      case 6:
        mm_mark_current_picture_long_term(img,p, tmp_drpm->long_term_frame_idx);
        break;
      default:
        AVD_ERROR_CHECK2(img,"invalid memory_management_control_operation in buffer", 500);
    }
    p->dec_ref_pic_marking_buffer = tmp_drpm->Next;
	
  }
  if ( specialInfo->last_has_mmco_5 )
  {
    p->frame_num = 0;
	p->pic_num = 0;
    p->poc = 0;
    flush_dpb(img);CHECK_ERR_RET_VOID
  }
}

//for special use,say,thumbnail of a video
/*!
 ************************************************************************
 * \brief
 *    Store a picture in DPB. This includes cheking for space in DPB and 
 *    flushing frames.
 *    If we received a frame, we need to check for a new store, if we
 *    got a field, check if it's the second field of an already allocated
 *    store.
 *
 * \param p
 *    Picture to be stored
 *
 ************************************************************************
 */
void store_picture_in_dpb(ImageParameters *img,StorablePicture* p)
{
  avdNativeUInt i;
  avdInt32 poc;
  avdNativeInt pos;
  DecodedPictureBuffer *dpb = img->dpb;
  H264VdLibParam* params = img->vdLibPar;
  TSpecitialInfo	*specialInfo = img->specialInfo;

  StartRunTimeClock(DT_STOREPICDPB);
  
  //OutputFirstFrame(img,p);
  
  // check for duplicate frame number in short term reference buffer
  if (p->structure==FRAME&&(p->used_for_reference)&&(!p->is_long_term)&&p->idr_flag==0)
  {
	  FrameStore **fs_ref = dpb->fs_ref;
	  for (i=0; i<dpb->ref_frames_in_buffer; i++)
	  {
		  if (fs_ref[i]->frame_num == p->frame_num)
		  {
			  //fs_ref[i]->frame_num = p->frame_num-1;
			  //AVD_ERROR_CHECK(img,"duplicate frame_num in short-term references", 500);
			  PutStorablePic(img,p);
			  return;
		  }
	  }
  }
#if FEATURE_INTERLACE
  if ((p->structure==TOP_FIELD)||(p->structure==BOTTOM_FIELD))
  {
	  // check for frame store with same pic_number
	  if (dpb->last_picture)
	  {
		  if ((int)dpb->last_picture->frame_num == p->pic_num)
		  {
			  if (((p->structure==TOP_FIELD)&&(dpb->last_picture->is_used==2))||((p->structure==BOTTOM_FIELD)&&(dpb->last_picture->is_used==1)))
			  {
				  if ((p->used_for_reference && (dpb->last_picture->is_orig_reference!=0))||
					  (!p->used_for_reference && (dpb->last_picture->is_orig_reference==0)))
				  {
					  insert_picture_in_dpb(img, dpb->last_picture, p);
					  update_ref_list(img);
					  update_ltref_list(img);
					  dpb->last_picture = NULL;
					  return;
				  }
			  }
		  }
	  }
  }
#endif//FEATURE_INTERLACE
#if !FEATURE_BFRAME
	if (dpb->size == 1 && p->used_for_reference&&params->sharedMem==NULL){
		FrameStore *fs = dpb->fs[0];
		if (fs->frame){
			write_stored_frame(img,fs);
			PutStorablePic(img,fs->frame);
		}
		fs->frame = p;
		fs->is_used = 3;
		fs->is_reference = 3;
		fs->poc = p->poc;
		fs->is_output = p->is_output;
		dpb->fs_ref[0] = fs;
		dpb->used_size = 1;
		return;
	}
#endif //VOI_H264D_NON_BASELINE

	
  //p->used_for_reference = (specialInfo->nal_reference_idc != 0);
	//Number 20110715: do not put the motionInfo to resue, because it may be used for colocated calculations
  //if (!p->used_for_reference) // save non-ref motion memory immediately to recycle;
	//  PutMotionInfo(img,p->motionInfo,p->typeIPB,p->structure);


  specialInfo->last_has_mmco_5=0;
#if FEATURE_INTERLACE
  specialInfo->last_pic_bottom_field = (p->structure==BOTTOM_FIELD);
#endif //VOI_H264D_NON_BASELINE
#if ENABLE_STRICT_IDR_CHECK
  if(specialInfo->idr_flag&&specialInfo->frame_num)//it is an error resilience
  {
	  AVD_ERROR_CHECK(img,"frame_num!=0,try to set the idr flag to zero for error resilience",WARN_GENERAL);
	  p->idr_flag = 0;
  }
#endif//ENABLE_STRICT_IDR_CHECK
  if (p->idr_flag)
    idr_memory_management(img,p);
  else
  {
    // adaptive memory management
	if(p->adaptive_ref_pic_buffering_flag)
	{
		if (p->used_for_reference)
			adaptive_memory_management(img,p);CHECK_ERR_RET_VOID
	}
  }


  // this is a frame or a field which has no stored complementatry field

  // sliding window, if necessary
  if ((!p->idr_flag)&&(p->used_for_reference && (!p->adaptive_ref_pic_buffering_flag)))
  {
    sliding_window_memory_management(img,p);
  } 

  // first try to remove unused frames
  if (dpb->used_size==dpb->size)
  {
    remove_unused_frame_from_dpb(img);
  }

  // then output frames until one can be removed
  while (dpb->used_size==dpb->size)
  {
    // non-reference frames may be output directly
    if (!p->used_for_reference)
    {
      get_smallest_poc(img,&poc, &pos);
      if ((-1==pos) || (p->poc < poc))
      {
        direct_output(img,p);
        return;
      }
    }
    // flush a frame
    output_one_frame_from_dpb(img);CHECK_ERR_RET_VOID
  }
  
  // store at end of buffer
//  printf ("store frame/field at pos %d\n",dpb->used_size);
  insert_picture_in_dpb(img,dpb->fs[dpb->used_size],p);
#if FEATURE_INTERLACE
  if (p->structure != FRAME)
    dpb->last_picture = dpb->fs[dpb->used_size];
  else
#endif //VOI_H264D_NON_BASELINE
    dpb->last_picture = NULL;

  dpb->used_size++;
  update_ref_list(img);
  update_ltref_list(img);
  //dump_dpb(dpb);

  
  EndRunTimeClock(DT_STOREPICDPB);	
}

/*!
 ************************************************************************
 * \brief
 *    Insert the picture into the DPB. A free DPB position is necessary
 *    for frames, .
 *
 * \param fs
 *    FrameStore into which the picture will be inserted
 * \param p
 *    StorablePicture to be inserted
 *
 ************************************************************************
 */
static void insert_picture_in_dpb(ImageParameters *img,FrameStore* fs, StorablePicture* p)
{
switch (p->structure)
{
case FRAME: 

    fs->frame = p;
    fs->is_used = 3;
  
    if (p->used_for_reference)
    {
      fs->is_reference = 3;
      fs->is_orig_reference = 3;
      if (p->is_long_term)
      {
        fs->is_long_term = 3;
      }
#if FEATURE_INTERLACE
	  // generate field views
	  // for frame_mbs_only_flag (implied need of field refernce and direct_8x8_inference_flag), 
	  // need to split first to get correct field mv before direct_8x8_inference_flag action;
	  // for img->MbaffFrameFlag: need to split first for correct mv in flags.mb_field for frame reference;
	  // if we just correct mv without split, bottom field mv will be lost causing error in split later.
	  if (!img->active_sps->frame_mbs_only_flag)
		  dpb_split_field(img,fs);
#endif //VOI_H264D_NON_BASELINE
    }
	fs->poc = fs->frame->poc;
	fs->long_term_frame_idx = fs->frame->long_term_frame_idx;
#if FEATURE_BFRAME
	if(img->notSaveAllMV==0)
		DpbFrPostProc(img,fs);
#endif
	break;
#if FEATURE_INTERLACE
	case TOP_FIELD:
			fs->top_field = p;
			fs->is_used |= 1;
			if (p->used_for_reference)
			{
				fs->is_reference |= 1;
				fs->is_orig_reference |= 1;
				if (p->is_long_term)
				{
					fs->is_long_term |= 1;
					fs->long_term_frame_idx = p->long_term_frame_idx;
				}
			}
			DpbFldPostProc(img,p);
			if (fs->is_used == 3)
			{
				// generate frame view
				fs->poc = min (fs->top_field->poc, fs->bottom_field->poc);
				dpb_combine_field(img,fs);
			} else
			{
				fs->poc = p->poc;
				// from JM8.3
				//gen_field_ref_ids(p);
			}
			break;
	case BOTTOM_FIELD:
		fs->bottom_field = p;
		fs->is_used |= 2;
		if (p->used_for_reference)
		{
			fs->is_reference |= 2;
			fs->is_orig_reference |= 2;
			if (p->is_long_term)
			{
				fs->is_long_term |= 2;
				fs->long_term_frame_idx = p->long_term_frame_idx;
			}
		}
		DpbFldPostProc(img,p);
		if (fs->is_used == 3)
		{
			// generate frame view
			fs->poc = min (fs->top_field->poc, fs->bottom_field->poc);
			dpb_combine_field(img,fs);
		} else
		{
			fs->poc = p->poc;
			// from JM8.3
			//gen_field_ref_ids(p);
		}
		
		break;
	#endif // VOI_H264D_NON_BASELINE
	}
	  fs->frame_num = p->pic_num;
	  fs->is_output = p->is_output;

}

/*!
 ************************************************************************
 * \brief
 *    Check if one of the frames/fields in frame store is used for reference
 ************************************************************************
 */
static avdNativeInt is_used_for_reference(ImageParameters *img,FrameStore* fs)
{
  if (fs->is_reference)
  {
    return 1;
  }
  
  if (fs->frame && fs->is_used==3) // frame
  {
    if (fs->frame->used_for_reference)
    {
      return 1;
    }
  }

#if FEATURE_INTERLACE
  if (fs->top_field && fs->is_used&1) // top field
  {
    if (fs->top_field->used_for_reference)
    {
      return 1;
    }
  }

  if (fs->bottom_field && fs->is_used&2) // bottom field
  {
    if (fs->bottom_field->used_for_reference)
    {
      return 1;
    }
  }
#endif //VOI_H264D_NON_BASELINE
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    Check if one of the frames/fields in frame store is used for short-term reference
 ************************************************************************
 */
static avdNativeInt is_short_term_reference(ImageParameters *img,FrameStore* fs)
{

  if (fs->frame && fs->is_used==3) // frame
  {
    if ((fs->frame->used_for_reference)&&(!fs->frame->is_long_term))
    {
      return 1;
    }
  }

#if FEATURE_INTERLACE
  if (fs->top_field && fs->is_used&1) // top field
  {
    if ((fs->top_field->used_for_reference)&&(!fs->top_field->is_long_term))
    {
      return 1;
    }
  }

  if (fs->bottom_field && fs->is_used&2) // bottom field
  {
    if ((fs->bottom_field->used_for_reference)&&(!fs->bottom_field->is_long_term))
    {
      return 1;
    }
  }
#endif //VOI_H264D_NON_BASELINE
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    Check if one of the frames/fields in frame store is used for short-term reference
 ************************************************************************
 */
static avdNativeInt is_long_term_reference(ImageParameters *img,FrameStore* fs)
{

  if (fs->frame && fs->is_used==3) // frame
  {
    if ((fs->frame->used_for_reference)&&(fs->frame->is_long_term))
    {
      return 1;
    }
  }

#if FEATURE_INTERLACE
  if (fs->top_field && fs->is_used&1) // top field
  {
    if ((fs->top_field->used_for_reference)&&(fs->top_field->is_long_term))
    {
      return 1;
    }
  }

  if (fs->bottom_field && fs->is_used&2) // bottom field
  {
    if ((fs->bottom_field->used_for_reference)&&(fs->bottom_field->is_long_term))
    {
      return 1;
    }
  }
#endif //VOI_H264D_NON_BASELINE
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    remove one frame from DPB
 ************************************************************************
 */
static void remove_frame_from_dpb(ImageParameters *img,avdNativeInt pos)
{
  DecodedPictureBuffer *dpb = img->dpb;
  FrameStore* fs = dpb->fs[pos];
  FrameStore* tmp;
  avdNativeUInt i;
  
//  printf ("remove frame with frame_num #%d\n", fs->frame_num);
  switch (fs->is_used)
  {
  case 3:
    PutStorablePic(img,fs->frame);
    fs->frame=NULL;
#if FEATURE_INTERLACE
    PutStorablePic(img,fs->top_field);
    PutStorablePic(img,fs->bottom_field);
    fs->top_field=NULL;
    fs->bottom_field=NULL;
#endif //VOI_H264D_NON_BASELINE
    break;

#if FEATURE_INTERLACE
  case 2:
    PutStorablePic(img,fs->bottom_field);
    fs->bottom_field=NULL;
    break;
  case 1:
    PutStorablePic(img,fs->top_field);
    fs->top_field=NULL;
    break;
#endif //VOI_H264D_NON_BASELINE

  case 0:
    break;
  default:
		  
    AVD_ERROR_CHECK2(img,"invalid frame store type",ERROE_InvalidFrameStoreType);
  }
  fs->is_used = 0;
  fs->is_long_term = 0;
  fs->is_reference = 0;
  fs->is_orig_reference = 0;

  // move empty frame store to end of buffer
  tmp = dpb->fs[pos];

  for (i=pos; i<dpb->used_size-1;i++)
  {
    dpb->fs[i] = dpb->fs[i+1];
  }
  dpb->fs[dpb->used_size-1] = tmp;
  dpb->used_size--;
}

/*!
 ************************************************************************
 * \brief
 *    find smallest POC in the DPB.
 ************************************************************************
 */
static void get_smallest_poc(ImageParameters *img,avdInt32 *poc,avdNativeInt * pos)
{
  avdNativeInt i;
  DecodedPictureBuffer *dpb = img->dpb;
  FrameStore **fs = dpb->fs;
  
  if (dpb->used_size<1)
  {
    AVD_ERROR_CHECK2(img,"Cannot determine smallest POC, DPB empty.",ERROR_DPBIsNULL);
  }


  *pos=-1;
  *poc = INT_MAX;
  for (i=dpb->used_size - 1; i >= 0; i--)
  {
    if ((*poc > fs[i]->poc)&&

		(fs[i]->frame&&!fs[i]->frame->is_output)

		//(!fs[i]->is_output)
		)
    {
      *poc = fs[i]->poc;
      *pos = i;
    }
  }
}

/*!
 ************************************************************************
 * \brief
 *    Remove a picture from DPB which is no longer needed.
 ************************************************************************
 */
static avdNativeInt remove_unused_frame_from_dpb(ImageParameters *img)
{
  avdNativeUInt i;
  DecodedPictureBuffer *dpb = img->dpb;

  // check for frames that were already output and no longer used for reference
  for (i=0; i<dpb->used_size; i++)
  {
    if ((dpb->fs[i]->frame==NULL//in interlace mode, it is somehow,field,sometimes
		||dpb->fs[i]->frame->is_output) && (!is_used_for_reference(img,dpb->fs[i])))
    {
      remove_frame_from_dpb(img,i);CHECK_ERR_RET_INT
      return 1;
    }
  }
  return 0;
}
void DumpPicBuf(ImageParameters *img)
{
	DecodedPictureBuffer *dpb = img->dpb;
	AvdLog2(LL_INFO,"$$numReuseNuf=%d\n",dpb->numReusedFrBuf);
}
void DumpBufInfo(ImageParameters *img,TMBsProcessor* info)
{
#ifdef DEBUG_INFO
	avdNativeUInt i;
	DecodedPictureBuffer *dpb = img->dpb;
	for (i=0; dpb && i<dpb->used_size; i++)
	{
		StorablePicture* pic = dpb->fs[i]->frame!=NULL?dpb->fs[i]->frame:dpb->fs[i]->top_field!=NULL?dpb->fs[i]->top_field:dpb->fs[i]->bottom_field;
		if(pic)
		{
			AvdLog2(LL_INFO,"###%d:poc=%d,type=%d,ref=%d,output=%d,err=%d,motion=%X,%X\n",i,pic->poc,pic->typeIPB,pic->used_for_reference,pic->is_output,pic->error,pic->motionInfo,pic);
#if (DUMP_VERSION & DUMP_SLICE)		
			AvdLog(DUMP_SLICE,DUMP_DCORE"###%d:poc=%d,type=%d,ref=%d,output=%d,err=%d\n",i,pic->poc,pic->typeIPB,pic->used_for_reference,pic->is_output,pic->error);
#endif//#if (DUMP_VERSION & DUMP_SLICE)
		}
		
	}
	AvdLog2(LL_INFO,"###img->dec_picture=%X\n",img->dec_picture);
	DumpPicBuf(img);
#endif	
}


/**
For the sake of smooth output,try to output one first,and reset its is_output but keep its reference
*/
void	Try_output_one_frame_from_dpb(ImageParameters *img,StorablePicture *currFrame)
{

	DecodedPictureBuffer *dpb = img->dpb;
	FrameStore **fs = dpb->fs;
	TPOCInfo* pocInfo = img->pocInfo;
	H264VdLibParam* params = img->vdLibPar;
#if 0
	{
		int i;
		DecodedPictureBuffer *dpb = img->dpb;
		FrameStore **fs = dpb->fs;
		AvdLog2(LL_INFO,"dpb:size=%d,curPOC=%d,type=%d\n",dpb->used_size,img->dec_picture->poc,img->type);
		for (i=0;i<dpb->used_size; i++)
		{
			AvdLog2(LL_INFO,"[%d]: o=%d poc=%d,ref=%d,used=%d\n",i,fs[i]->frame->is_output,fs[i]->poc,fs[i]->is_reference,fs[i]->is_used);
		}
	}
#endif
	//check if current frame can be output
	//the original design:after flush dpb,and current frame is I frame
	//but in some case, there is 0(IDR),-2,-4 like poc in buffer,but we need to guarantee the first frame can be output for thumbnail
#define MOTO_CASE 100
#if FEATURE_INTERLACE
	//AvdLog2(LL_INFO,"curpoc=%d,lastpoc=%d,structure=%d,type=%d\n",currFrame->poc,img->dpb->last_output_poc,currFrame->structure,currFrame->typeIPB);

	if(currFrame->structure!=FRAME)
		if(currFrame->isCombinedOrSplit==0||currFrame->frame==NULL)
			return;
		else
			currFrame = currFrame->frame;
#endif	
	if(currFrame->poc<0&&!pocInfo->isNegtivePocExisted)
		pocInfo->isNegtivePocExisted = 1;

	if(//params->enableOutputASAP==MOTO_CASE&&
		img->active_sps->profile_idc==66&&params->enableOutputASAP!=EOF_NORMAL_BASELINE//for rare baseline, it is not output by input, skip it
		||
		img->ioInfo->outNumber==0 //for thumbnail requirement, output the first absolutely
		||
		currFrame->poc==dpb->last_output_poc+pocInfo->poc_interval
		||
		currFrame->idr_flag&&!pocInfo->isNegtivePocExisted
		||
		params->enableOutputASAP==MOTO_CASE&&currFrame->poc<0
		)
	{
		 return write_picture(img,currFrame);
	}
	else if(dpb->used_size)
	{
		avdInt32			poc;
		avdNativeInt		pos;
		get_smallest_poc(img,&poc, &pos);
		if(pos>=0)
		{
			if(poc==dpb->last_output_poc+pocInfo->poc_interval
				||dpb->last_output_poc==VO_INT_MIN//pos 0 is IDR
				)
				return write_picture(img,fs[pos]->frame);
		}
		
	}

	

}
/*!
 ************************************************************************
 * \brief
 *    Output one picture stored in the DPB.
 ************************************************************************
 */

static void output_one_frame_from_dpb(ImageParameters *img)
{
  avdInt32 poc;
  avdNativeInt pos;
  DecodedPictureBuffer *dpb = img->dpb;
  H264VdLibParam* params = img->vdLibPar;		
 //diagnostics
  if (dpb->used_size<1)
  {
    AVD_ERROR_CHECK2(img,"Cannot output frame, DPB empty.",ERROR_DPBIsNULL);
  }


  // find smallest POC
  get_smallest_poc(img,&poc, &pos);


  if(pos==-1)
  {
	  H264Flushing(img);//there is error, reset it again
	  return;
  }
	
  if (dpb->last_output_poc > poc)
  {
	AVD_ERROR_CHECK2(img,"output POC must be in ascending order", 100);//ERROR_InvalidOutPOC);
  } 
  write_stored_frame(img,dpb->fs[pos]);
	
  // free frame store and move empty store to end of buffer
  if (!is_used_for_reference(img,dpb->fs[pos]))
  {
    remove_frame_from_dpb(img,pos);CHECK_ERR_RET_VOID
  }
}



/*!
 ************************************************************************
 * \brief
 *    All stored picture are output. Should be called to empty the buffer
 ************************************************************************
 */
void flush_dpb(ImageParameters *img)
{
  avdNativeInt i;
  DecodedPictureBuffer *dpb = img->dpb;
  H264VdLibParam *params = img->vdLibPar;	
  //diagnostics
//  printf("Flush remaining frames from dpb-> dpb->size=%d, dpb->used_size=%d\n",dpb->size,dpb->used_size);

  // mark all frames unused
  for (i=dpb->used_size - 1; i>=0; i--)
  {
    unmark_for_reference (img,dpb->fs[i]);
  }
  update_ltref_list(img);
  update_ref_list(img);
  while (remove_unused_frame_from_dpb(img)) ;
  
  // output frames in POC order
  while (dpb->used_size)
  {
	//img->vdLibPar->numFrDec = 0;
    output_one_frame_from_dpb(img);CHECK_ERR_RET_VOID
  }

  dpb->last_output_poc = VO_INT_MIN;
}

#define RSD(x) ((x&2)?(x|1):(x&(~1)))

/*!
 ************************************************************************
 * \brief
 *    Allocate memory for buffering of reference picture reordering commands
 ************************************************************************
 */

void alloc_ref_pic_list0_reordering_buffer(ImageParameters *img,Slice *currSlice)
{
	//avdNativeInt size = MAX_REFERENCE_FRAMES;//the max ref number is 16//img->num_ref_idx_l0_active+1;
	//if(currSlice->remapping_of_pic_nums_idc_l0==NULL)
	//	if ((currSlice->remapping_of_pic_nums_idc_l0 = (avdNativeInt *)voH264AlignedMalloc(img,36, size * sizeof(avdNativeInt)))==NULL) 
	//		no_mem_exit("alloc list0 remapping");
	//CHECK_ERR_RET_VOID
	//if(currSlice->abs_diff_pic_num_minus1_l0==NULL)
	//	if ((currSlice->abs_diff_pic_num_minus1_l0 = (avdNativeInt *)voH264AlignedMalloc(img,37, size * sizeof(avdNativeInt)))==NULL) 
	//		no_mem_exit("alloc list0 abs_diff");
	//if(currSlice->long_term_pic_idx_l0==NULL)
	//	if ((currSlice->long_term_pic_idx_l0 = (avdNativeInt *)voH264AlignedMalloc(img,38, size * sizeof(avdNativeInt)))==NULL) 
	//		no_mem_exit("alloc list0 lt_pic_idx");
	//CHECK_ERR_RET_VOID
}

/*!
 ************************************************************************
 * \brief
 *    Free memory for buffering of reference picture reordering commands
 ************************************************************************
 */
void free_ref_pic_list0_reordering_buffer(ImageParameters *img,Slice *currSlice)
{
	/*	SafevoH264AlignedFree(img,currSlice->remapping_of_pic_nums_idc_l0);
		SafevoH264AlignedFree(img,currSlice->abs_diff_pic_num_minus1_l0);
		SafevoH264AlignedFree(img,currSlice->long_term_pic_idx_l0);*/

}

#if FEATURE_BFRAME
#if (DUMP_VERSION & DUMP_SLICE)
void DumpMoving4(ImageParameters *img,TMBsProcessor* info)
{
	int i,j,i2,j2;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	int xSize = (sizeInfo->width>>2);
	int ySize = (sizeInfo->height>>2);
	StorablePicture	*dec_picture = img->listX[LIST_1 ][0];
	AvdLog(DUMP_SLICE,DUMP_DCORE"\n list1_0_poc = %d",dec_picture->poc);
	for (j=0 ; j< ySize; j++){
		AvdLog(DUMP_SLICE,DUMP_DCORE"\n%d ",j);
		for (i=0 ; i< xSize; i++){              
			int value = IsMoving4x4(dec_picture,j,i);
			AvdLog(DUMP_SLICE,DUMP_DCORE"(%d)%d,",i,value);
		}      
	}
	AvdLog(DUMP_SLICE,DUMP_DCORE"\n");
}
#endif//#if (DUMP_VERSION & DUMP_SLICE)

#endif //FEATURE_BFRAME

#if FEATURE_BFRAME
void alloc_ref_pic_list1_reordering_buffer(ImageParameters *img,Slice *currSlice)
{
	/*avdNativeInt size = MAX_REFERENCE_FRAMES;// img->num_ref_idx_l1_active+1;
	if(currSlice->remapping_of_pic_nums_idc_l1==NULL)
		if ((currSlice->remapping_of_pic_nums_idc_l1 = voH264AlignedMalloc(img,39, size * sizeof(avdNativeInt)))==NULL) 
			no_mem_exit("alloc list1 remapping");
	if(currSlice->abs_diff_pic_num_minus1_l1==NULL)
		if ((currSlice->abs_diff_pic_num_minus1_l1 = voH264AlignedMalloc(img,40, size * sizeof(avdNativeInt)))==NULL) 
			no_mem_exit("alloc list1 abs_diff");
	if(currSlice->long_term_pic_idx_l1==NULL)
		if ((currSlice->long_term_pic_idx_l1 = voH264AlignedMalloc(img,41, size * sizeof(avdNativeInt)))==NULL) 
			no_mem_exit("alloc list1 lt_pic_idx");*/
}

void free_ref_pic_list1_reordering_buffer(ImageParameters *img,Slice *currSlice)
{
		/*SafevoH264AlignedFree(img,currSlice->remapping_of_pic_nums_idc_l1);
		SafevoH264AlignedFree(img,currSlice->abs_diff_pic_num_minus1_l1);
		SafevoH264AlignedFree(img,currSlice->long_term_pic_idx_l1);	*/
}
#endif//FEATURE_BFRAME
void DpbFrPostProc2(ImageParameters *img,TMBsProcessor* info)
{
	// use only for calculate motion vectors of B_Skip and B_Direct;
	// TBD: need a better way to do this; may be check 16x16 block first;
	avdNativeUInt i,j,i2,j2, moving4x4StartY;
	avdNativeUInt is8x8Inference = !img->active_sps->frame_mbs_only_flag 
		|| img->active_sps->direct_8x8_inference_flag;
	StorablePicture	*dec_picture = img->dec_picture;
	AVDStoredPicMotionInfo *motionInfo = dec_picture->motionInfo;
	SliceType typeIPB = dec_picture->typeIPB;
	avdNativeUInt bFrame;
	AVDMotionVector **mv0;
	AVDMotionVector **mv1;
	avdUInt8 **list0, **list1;
	avdNativeUInt xSize, ySize;
	avdNativeInt tmpX, tmpY;
	TPosInfo* posInfo = GetPosInfo(info);
	if (typeIPB == I_SLICE || !dec_picture->used_for_reference)
		return;

	bFrame = (typeIPB == B_SLICE);
	mv0 = (typeIPB != I_SLICE) ? motionInfo->mv0 : NULL;
	list0 = motionInfo->ref_idx0;
	if (bFrame){
		mv1 = motionInfo->mv1;
		list1 = motionInfo->ref_idx1; //TBD: Is this a bug???
	}

	xSize = (dec_picture->size_x>>2);
	moving4x4StartY = posInfo->block_y;
	if (!is8x8Inference){
		for (j=0 ; j< 4; j++){
			j2 = (j>>1); 
			for (i=0 ; i< xSize; i++){                
				i2 = (i>>1);
				tmpX = list0[j2][i2];
				SetMoving4x4(!(((tmpX == 0) && 
					(mv0[j][i].x < 2 && mv0[j][i].x > -2 && 
					mv0[j][i].y < 2 && mv0[j][i].y > -2)) 
					|| ((tmpX == AVD_INVALID_REF_IDX) && 
					bFrame && (list1[j2][i2] == 0) && 
					mv1[j][i].x < 2 && mv1[j][i].x > -2 && 
					mv1[j][i].y < 2 && mv1[j][i].y > -2)
					), motionInfo, (moving4x4StartY+j), i);
			}      
		}
	}
	else{
		avdInt32 *mv0Pt0, *mv0Pt3, *mv1Pt0, *mv1Pt3;
		j = j2 = 0;
		AVD_ALIGNED32_MEMCPY(mv0[-1],mv0[3],xSize);
		mv0Pt0 = (avdInt32 *)&mv0[0][0];
		mv0Pt3 = (avdInt32 *)&mv0[-1][0];
		
	
		if (bFrame){
			AVD_ALIGNED32_MEMCPY(mv1[-1],mv1[3],xSize);
			mv1Pt0 = (avdInt32 *)&mv1[0][0];
			mv1Pt3 = (avdInt32 *)&mv1[-1][0];
		}

		for (i=0 ; i< xSize; i += 4){                
			//! Use inference flag to remap mvs/references
			mv0Pt0[1] = mv0Pt0[0];
			mv0Pt0[2] = mv0Pt0[3];
			mv0Pt3[1] = mv0Pt3[0];
			mv0Pt3[2] = mv0Pt3[3];
			if (bFrame){
				mv1Pt0[1] = mv1Pt0[0];
				mv1Pt0[2] = mv1Pt0[3];
				mv1Pt3[1] = mv1Pt3[0];
				mv1Pt3[2] = mv1Pt3[3];
				mv1Pt0 += 4;
				mv1Pt3 += 4;
			}
			i2 = (i>>1);
			tmpX = list0[j2][i2];
			if (!(((tmpX == 0) && 
				(mv0[j][i].x < 2 && mv0[j][i].x > -2 && 
				mv0[j][i].y < 2 && mv0[j][i].y > -2)) 
				|| ((tmpX == AVD_INVALID_REF_IDX) && bFrame &&
				(list1[j2][i2] == 0) && 
				mv1[j][i].x < 2 && mv1[j][i].x > -2 && 
				mv1[j][i].y < 2 && mv1[j][i].y > -2))){
					Set2Moving4x4s(motionInfo, moving4x4StartY+j, i);
					Set2Moving4x4s(motionInfo, moving4x4StartY+j+1, i);
			}

			tmpX = list0[j2][i2+1];
			if (!(((tmpX == 0) && 
				(mv0[j][i+3].x < 2 && mv0[j][i+3].x > -2 && 
				mv0[j][i+3].y < 2 && mv0[j][i+3].y > -2)) 
				|| ((tmpX == AVD_INVALID_REF_IDX) && bFrame &&
				(list1[j2][i2+1] == 0) && 
				mv1[j][i+3].x < 2 && mv1[j][i+3].x > -2 && 
				mv1[j][i+3].y < 2 && mv1[j][i+3].y > -2))){
					Set2Moving4x4s(motionInfo, moving4x4StartY+j, i+2);
					Set2Moving4x4s(motionInfo, moving4x4StartY+j+1, i+2);
			}

			tmpX = list0[j2+1][i2];
			if (!(((tmpX == 0) && 
				(mv0[j+3][i].x < 2 && mv0[j+3][i].x > -2 && 
				mv0[j+3][i].y < 2 && mv0[j+3][i].y > -2)) 
				|| ((tmpX == AVD_INVALID_REF_IDX) && bFrame &&
				(list1[j2+1][i2] == 0) && 
				mv1[j+3][i].x < 2 && mv1[j+3][i].x > -2 && 
				mv1[j+3][i].y < 2 && mv1[j+3][i].y > -2))){
					Set2Moving4x4s(motionInfo, moving4x4StartY+j+2, i);
					Set2Moving4x4s(motionInfo, moving4x4StartY+j+3, i);
			}

			tmpX = list0[j2+1][i2+1];
			if (!(((tmpX == 0) && 
				(mv0[j+3][i+3].x < 2 && mv0[j+3][i+3].x > -2 && 
				mv0[j+3][i+3].y < 2 && mv0[j+3][i+3].y > -2)) 
				|| ((tmpX == AVD_INVALID_REF_IDX) && bFrame &&
				(list1[j2+1][i2+1] == 0) && 
				mv1[j+3][i+3].x < 2 && mv1[j+3][i+3].x > -2 && 
				mv1[j+3][i+3].y < 2 && mv1[j+3][i+3].y > -2))){
					Set2Moving4x4s(motionInfo, moving4x4StartY+j+2, i+2);
					Set2Moving4x4s(motionInfo, moving4x4StartY+j+3, i+2);
			}
			mv0Pt0 += 4;
			mv0Pt3 += 4;
		} // i; 

	}
}
#if FEATURE_BFRAME//TBDh8f
void DpbFrPostProc(ImageParameters *img,FrameStore *fs)
{
	// use only for calculate motion vectors of B_Skip and B_Direct;
	// TBD: need a better way to do this; may be check 16x16 block first;
	avdNativeUInt i,j,i2,j2;
	avdNativeUInt is8x8Inference = img->active_sps->direct_8x8_inference_flag;
	AVDStoredPicMotionInfo *motionInfo = fs->frame->motionInfo;
	SliceType typeIPB = fs->frame->typeIPB;
	avdNativeUInt bFrame;
	AVDMotionVector **mv0;
	AVDMotionVector **mv1;
	avdUInt8 **list0, **list1;
	avdNativeUInt xSize, ySize;
	avdNativeInt tmpX, tmpY;
	if (typeIPB == I_SLICE || !fs->frame->used_for_reference)
		return;
	if (img->active_sps->profile_idc==66
#if OPT_ASSUME_DIRECTTYPE_IS_CONSISTENT
		||(!is8x8Inference&&currSlice->direct_type==B_DIRECT_TEMPORAL)
#endif//OPT_ASSUME_DIRECTTYPE_IS_CONSISTENT
		)
		return;

	bFrame = (typeIPB == B_SLICE);
	mv0 = (typeIPB != I_SLICE) ? motionInfo->mv0 : NULL;
	list0 = motionInfo->ref_idx0;
	if (bFrame){
		mv1 = motionInfo->mv1;
		list1 = motionInfo->ref_idx1;
	}

	xSize = (fs->frame->size_x>>2);
	ySize = (fs->frame->size_y>>2);
	if (!is8x8Inference){
		for (j=0 ; j< ySize; j++){
			j2 = (j>>1);
			for (i=0 ; i< xSize; i++){                
				i2 = (i>>1);
				tmpX = list0[j2][i2];
				SetMoving4x4(!(((tmpX == 0) && 
					(mv0[j][i].x < 2 && mv0[j][i].x > -2 && 
					mv0[j][i].y < 2 && mv0[j][i].y > -2)) 
					|| ((tmpX == AVD_INVALID_REF_IDX) && 
					bFrame && (list1[j2][i2] == 0) && 
					mv1[j][i].x < 2 && mv1[j][i].x > -2 && 
					mv1[j][i].y < 2 && mv1[j][i].y > -2)
					), motionInfo, j, i);
			}      
		}
	}
	else{
		avdInt32 *mv0Pt0, *mv0Pt3, *mv1Pt0, *mv1Pt3;
		mv0Pt0 = (avdInt32 *)&mv0[0][0];
		mv0Pt3 = (avdInt32 *)&mv0[3][0];
		if (bFrame){
			mv1Pt0 = (avdInt32 *)&mv1[0][0];
			mv1Pt3 = (avdInt32 *)&mv1[3][0];
		}

		for (j=0 ; j< ySize; j += 4){
			j2 = (j>>1);
			for (i=0 ; i< xSize; i += 4){                
				//! Use inference flag to remap mvs/references
				mv0Pt0[1] = mv0Pt0[0];
				mv0Pt0[2] = mv0Pt0[3];
				mv0Pt3[1] = mv0Pt3[0];
				mv0Pt3[2] = mv0Pt3[3];
				if (bFrame){
					mv1Pt0[1] = mv1Pt0[0];
					mv1Pt0[2] = mv1Pt0[3];
					mv1Pt3[1] = mv1Pt3[0];
					mv1Pt3[2] = mv1Pt3[3];
					mv1Pt0 += 4;
					mv1Pt3 += 4;
				}
				i2 = (i>>1);
				tmpX = list0[j2][i2];
				if (!(((tmpX == 0) && 
					(mv0[j][i].x < 2 && mv0[j][i].x > -2 && 
					mv0[j][i].y < 2 && mv0[j][i].y > -2)) 
					|| ((tmpX == AVD_INVALID_REF_IDX) && bFrame &&
					(list1[j2][i2] == 0) && 
					mv1[j][i].x < 2 && mv1[j][i].x > -2 && 
					mv1[j][i].y < 2 && mv1[j][i].y > -2))){
					Set2Moving4x4s(motionInfo, j, i);
					Set2Moving4x4s(motionInfo, j+1, i);
				}

				tmpX = list0[j2][i2+1];
				if (!(((tmpX == 0) && 
					(mv0[j][i+3].x < 2 && mv0[j][i+3].x > -2 && 
					mv0[j][i+3].y < 2 && mv0[j][i+3].y > -2)) 
					|| ((tmpX == AVD_INVALID_REF_IDX) && bFrame &&
					(list1[j2][i2+1] == 0) && 
					mv1[j][i+3].x < 2 && mv1[j][i+3].x > -2 && 
					mv1[j][i+3].y < 2 && mv1[j][i+3].y > -2))){
					Set2Moving4x4s(motionInfo, j, i+2);
					Set2Moving4x4s(motionInfo, j+1, i+2);
				}

				tmpX = list0[j2+1][i2];
				if (!(((tmpX == 0) && 
					(mv0[j+3][i].x < 2 && mv0[j+3][i].x > -2 && 
					mv0[j+3][i].y < 2 && mv0[j+3][i].y > -2)) 
					|| ((tmpX == AVD_INVALID_REF_IDX) && bFrame &&
					(list1[j2+1][i2] == 0) && 
					mv1[j+3][i].x < 2 && mv1[j+3][i].x > -2 && 
					mv1[j+3][i].y < 2 && mv1[j+3][i].y > -2))){
					Set2Moving4x4s(motionInfo, j+2, i);
					Set2Moving4x4s(motionInfo, j+3, i);
				}

				tmpX = list0[j2+1][i2+1];
				if (!(((tmpX == 0) && 
					(mv0[j+3][i+3].x < 2 && mv0[j+3][i+3].x > -2 && 
					mv0[j+3][i+3].y < 2 && mv0[j+3][i+3].y > -2)) 
					|| ((tmpX == AVD_INVALID_REF_IDX) && bFrame &&
					(list1[j2+1][i2+1] == 0) && 
					mv1[j+3][i+3].x < 2 && mv1[j+3][i+3].x > -2 && 
					mv1[j+3][i+3].y < 2 && mv1[j+3][i+3].y > -2))){
					Set2Moving4x4s(motionInfo, j+2, i+2);
					Set2Moving4x4s(motionInfo, j+3, i+2);
				}
				mv0Pt0 += 4;
				mv0Pt3 += 4;
			} // i; 	
			// copy line 1 and line 2;
			AVD_ALIGNED32_MEMCPY(mv0Pt0, mv0Pt0 - xSize, xSize);
			AVD_ALIGNED32_MEMCPY(mv0Pt0 + xSize, mv0Pt3 - xSize, xSize);
			mv0Pt0 = mv0Pt3;
			mv0Pt3 += xSize + (xSize<<1);
			if (bFrame){
				AVD_ALIGNED32_MEMCPY(mv1Pt0, mv1Pt0 - xSize, xSize );
				AVD_ALIGNED32_MEMCPY(mv1Pt0 + xSize, mv1Pt3 - xSize, xSize );
				mv1Pt0 = mv1Pt3;
				mv1Pt3 += xSize + (xSize<<1);
			}
		} // j;
	}
#if FEATURE_INTERLACE
	if (GetMbAffFrameFlag(img) && is8x8Inference)      
	{
		tmpX = ((fs->frame->size_x>>4)<<1);
		for (j=0 ; j < ySize; j++)      
		{                
			tmpY = tmpX * (j>>3) + ((j>>2)&1);
			for (i=0 ; i < xSize; i++)          
			{                
				int currentmb= tmpY + ((i>>2)<<1);
				if (mbIsMbField2(currentmb))
				{
					mv0[j][i].y += mv0[j][i].y;
					if (bFrame)
						mv1[j][i].y += mv1[j][i].y;
					if (!(j&1) && !(i&1)){
						if (list0[j>>1][i>>1] != AVD_INVALID_REF_IDX)
							list0[j>>1][i>>1] >>= 1;
						if (bFrame && list1[j>>1][i>>1] != AVD_INVALID_REF_IDX)
							list1[j>>1][i>>1] >>= 1;
					}
				}
			}
		}
	}
#endif//VOI_H264D_NON_BASELINE
}
#endif//FEATURE_BFRAME

#if FEATURE_INTERLACE
void dpb_split_field(ImageParameters *img,FrameStore *fs)
{
	TSizeInfo*	sizeInfo = img->sizeInfo;
	avdNativeInt uvPitch = (sizeInfo->yPlnPitch>>1);
	avdUInt8 *src1, *src2, *dst1, *dst2;
	avdNativeUInt i,j, flag, i2, j2;
	AVDStoredPicMotionInfo *mITop, *mIBot, *motionInfo = fs->frame->motionInfo, *mITmp;
	SliceType typeIPB = fs->frame->typeIPB;
	avdNativeInt bFrame = (typeIPB == B_SLICE);
	AVDMotionVector **mv0 = (typeIPB != I_SLICE) ? motionInfo->mv0 : NULL;
	AVDMotionVector **mv1 = (bFrame) ? motionInfo->mv1 : NULL;
	avdNativeUInt createMotionInfo = (typeIPB != I_SLICE);
	StorablePicture *topFld, *botFld, *frame;
	frame = fs->frame;
	topFld = fs->top_field = GetStorablePic(img,TOP_FIELD, fs->frame->typeIPB, fs->frame->size_x, 
		fs->frame->size_y/2, fs->frame->size_x_cr, fs->frame->size_y_cr/2, createMotionInfo);
	botFld = fs->bottom_field = GetStorablePic(img,BOTTOM_FIELD, fs->frame->typeIPB, fs->frame->size_x, 
		fs->frame->size_y/2, fs->frame->size_x_cr, fs->frame->size_y_cr/2, createMotionInfo);


	src1 = frame->plnY;
	dst1 = topFld->plnY;
	for (i=0; i<fs->frame->size_y/2; i++){
		memcpy(dst1, src1, frame->size_x * sizeof(avdUInt8));
		src1 += (sizeInfo->yPlnPitch<<1);
		dst1 += sizeInfo->yPlnPitch;
	}

	src1 = frame->plnU;
	dst1 = topFld->plnU;
	src2 = frame->plnV;
	dst2 = topFld->plnV;
	for (i=0; i<fs->frame->size_y_cr/2; i++){
		memcpy(dst1, src1, frame->size_x_cr * sizeof(avdUInt8));
		memcpy(dst2, src2, frame->size_x_cr * sizeof(avdUInt8));
		src1 += sizeInfo->yPlnPitch;
		dst1 += uvPitch;
		src2 += sizeInfo->yPlnPitch;
		dst2 += uvPitch;
	}

	src1 = frame->plnY + sizeInfo->yPlnPitch;
	dst1 = botFld->plnY;
	for (i=0; i<fs->frame->size_y/2; i++){
		memcpy(dst1, src1, frame->size_x * sizeof(avdUInt8));
		src1 += (sizeInfo->yPlnPitch<<1);
		dst1 += sizeInfo->yPlnPitch;
	}

	src1 = frame->plnU + uvPitch;
	dst1 = botFld->plnU;
	src2 = frame->plnV + uvPitch;
	dst2 = botFld->plnV;
	for (i=0; i<fs->frame->size_y_cr/2; i++){
		memcpy(dst1, src1, frame->size_x_cr * sizeof(avdUInt8));
		memcpy(dst2, src2, frame->size_x_cr * sizeof(avdUInt8));
		src1 += sizeInfo->yPlnPitch;
		dst1 += uvPitch;
		src2 += sizeInfo->yPlnPitch;
		dst2 += uvPitch;
	}


	// poc;
	fs->poc = topFld->poc = frame->poc;
	botFld->poc = frame->bottom_poc;
	// frame_poc;
	topFld->frame_poc = botFld->frame_poc = frame->frame_poc;
	// bottom_poc;
	topFld->bottom_poc = botFld->bottom_poc =  frame->bottom_poc;
	// top_poc;
	topFld->top_poc = botFld->top_poc = frame->top_poc;

	topFld->used_for_reference = botFld->used_for_reference 
		= frame->used_for_reference;
	topFld->is_long_term = botFld->is_long_term 
		= frame->is_long_term;
	topFld->long_term_frame_idx = botFld->long_term_frame_idx 
		= frame->long_term_frame_idx;
	topFld->coded_frame = botFld->coded_frame = 1;
	topFld->MbaffFrameFlag = botFld->MbaffFrameFlag = GetMbAffFrameFlag(frame);

	topFld->frame = botFld->frame = frame;
	frame->top_field = botFld->top_field = topFld;
	frame->bottom_field = topFld->bottom_field = botFld;
	frame->isCombinedOrSplit = 1;
	if (!createMotionInfo)
		return;

	//store reference picture index
	mITop = topFld->motionInfo;
	mIBot = botFld->motionInfo;
	if (!img->active_sps->frame_mbs_only_flag && motionInfo){
		for (i=0;i<img->listXsize[LIST_1];i++){
			i2 = (i<<1);
			topFld->ref_pic_num[LIST_1][i2]   = frame->ref_pic_num[2 + LIST_1][i2];
			topFld->ref_pic_num[LIST_1][i2+1] = frame->ref_pic_num[2 + LIST_1][i2+1];
			botFld->ref_pic_num[LIST_1][i2]   = frame->ref_pic_num[4 + LIST_1][i2];
			botFld->ref_pic_num[LIST_1][i2+1] = frame->ref_pic_num[4 + LIST_1][i2+1];
		}
		for (i=0;i<img->listXsize[LIST_0];i++){
			i2 = (i<<1);
			topFld->ref_pic_num[LIST_0][i2]   = frame->ref_pic_num[2 + LIST_0][i2];
			topFld->ref_pic_num[LIST_0][i2+1] = frame->ref_pic_num[2 + LIST_0][i2+1];
			botFld->ref_pic_num[LIST_0][i2]   = frame->ref_pic_num[4 + LIST_0][i2];
			botFld->ref_pic_num[LIST_0][i2+1] = frame->ref_pic_num[4 + LIST_0][i2+1] ;
		}
	}

	if (GetMbAffFrameFlag(frame)){
			for (i=0 ; i<frame->size_x/4 ; i++){                
				i2 = (i>>1);
				for (j=0 ; j<frame->size_y/8; j++){ 
					int idiv4=i/4, jdiv4=j/2;
					int currentmb=2*(frame->size_x/16)*(jdiv4/2)+ (idiv4)*2 + (jdiv4&1);
					j2 = (j>>1);
					// Assign field mvs attached to MB-Frame buffer to the proper buffer
					if (mbIsMbField2(currentmb)){
						mIBot->mv0[j][i].x = mv0[((j>>2)<<3) + (j&3) + 4][i].x;
						mIBot->mv0[j][i].y = mv0[((j>>2)<<3) + (j&3) + 4][i].y;
						mITop->mv0[j][i].x = mv0[((j>>2)<<3) + (j&3)][i].x;
						mITop->mv0[j][i].y = mv0[((j>>2)<<3) + (j&3)][i].y;
						if (bFrame){
							mIBot->mv1[j][i].x = mv1[((j>>2)<<3) + (j&3) + 4][i].x;
							mIBot->mv1[j][i].y = mv1[((j>>2)<<3) + (j&3) + 4][i].y;
							mITop->mv1[j][i].x = mv1[((j>>2)<<3) + (j&3)][i].x;
							mITop->mv1[j][i].y = mv1[((j>>2)<<3) + (j&3)][i].y;
						}
						if (!(i&1) && !(j&1)){
							mIBot->ref_idx0[j2][i2] = motionInfo->ref_idx0[
								(((j>>2)<<3) + (j&3) + 4)>>1][i2];
								mITop->ref_idx0[j2][i2] = motionInfo->ref_idx0[
									(((j>>2)<<3) + (j&3))>>1][i2];
									if (bFrame){
										mIBot->ref_idx1[j2][i2] = motionInfo->ref_idx1[
											(((j>>2)<<3) + (j&3) + 4)>>1][i2];
											mITop->ref_idx1[j2][i2] = motionInfo->ref_idx1[
												(((j>>2)<<3) + (j&3))>>1][i2];
									}
						}
					}
				}
			}             

			mITmp = (fs->bottom_field->poc>fs->top_field->poc) ? mITop : mIBot;
			for (j=0 ; j<frame->size_y/4 ; j++){  
				j2 = (j>>1);
				for (i=0 ; i<frame->size_x/4 ; i++){                
					int idiv4 = (i>>2), jdiv4 = (j>>2);
					int currentmb=2*(frame->size_x/16)*(jdiv4/2)+ (idiv4)*2 + (jdiv4&1);
					if (mbIsMbField2(currentmb)){ 
						//! Assign frame buffers for field MBs           
						mv0[j][i].x = mITmp->mv0[j2][i].x;
						mv0[j][i].y = mITmp->mv0[j2][i].y ;     
						if (bFrame){
							mv1[j][i].x = mITmp->mv1[j2][i].x;
							mv1[j][i].y = mITmp->mv1[j2][i].y ;           
						}
						if (!(i&1) && !(j&1)){
							i2 = (i>>1);
							motionInfo->ref_idx0[j2][i2] 
							= mITmp->ref_idx0[jdiv4][i2];
							if (bFrame)
								motionInfo->ref_idx1[j2][i2] 
							= mITmp->ref_idx1[jdiv4][i2];  
						}
					}
				}
			}             
	}

	//! Generate field MVs from Frame MVs
	for (i=0 ; i<frame->size_x/4 ; i++){
		int idiv4 = (i>>2);
		i2 = (i>>1);
		for (j=0 ; j<frame->size_y/8 ; j++){
			int currentmb;
			j2 = (j>>1);
			currentmb=2*(frame->size_x/16)*(j2/2)+ (idiv4)*2 + (j2&1);
			//! Do nothing if macroblock as field coded in MB-AFF
			if (!GetMbAffFrameFlag(frame)|| !mbIsMbField2(currentmb))//||!img->field_pic_flag)// 
			{
				mITop->mv0[j][i].x = mIBot->mv0[j][i].x 
					= mv0[2*RSD(j)][RSD(i)].x;
				mITop->mv0[j][i].y = mIBot->mv0[j][i].y 
					= (mv0[2*RSD(j)][RSD(i)].y);
				if (bFrame){
					mITop->mv1[j][i].x = mIBot->mv1[j][i].x 
						= mv1[2*RSD(j)][RSD(i)].x;
					mITop->mv1[j][i].y = mIBot->mv1[j][i].y 
						= (mv1[2*RSD(j)][RSD(i)].y);
				}
				if (!(i&1) && !(j&1)){
					// Scaling of references is done here since it will not affect spatial direct (2*0 =0)
					if (motionInfo->ref_idx0[(2*RSD(j))>>1][RSD(i)>>1] == AVD_INVALID_REF_IDX)      
						mITop->ref_idx0[j2][i2] = mIBot->ref_idx0[j2][i2] = AVD_INVALID_REF_IDX;
					else
						mITop->ref_idx0[j2][i2] = mIBot->ref_idx0[j2][i2] 
					= motionInfo->ref_idx0[(2*RSD(j))>>1][RSD(i)>>1] * 2;

					if (bFrame){
						if (motionInfo->ref_idx1[(2*RSD(j))>>1][RSD(i)>>1] == AVD_INVALID_REF_IDX)      
							mITop->ref_idx1[j2][i2] = mIBot->ref_idx1[j2][i2] = AVD_INVALID_REF_IDX;
						else
							mITop->ref_idx1[j2][i2] = mIBot->ref_idx1[j2][i2] 
						= motionInfo->ref_idx1[(2*RSD(j))>>1][RSD(i)>>1]*2;
					}
				}

				flag = !((mITop->ref_idx0[j2][i2] == 0 && 
					mITop->mv0[j][i].x < 2 && mITop->mv0[j][i].x > -2 &&
					mITop->mv0[j][i].y < 2 && mITop->mv0[j][i].y > -2) ||
					(mITop->ref_idx0[j2][i2] == AVD_INVALID_REF_IDX && bFrame &&
					mITop->ref_idx1[j2][i2] == 0 && 
					mITop->mv1[j][i].x < 2 && mITop->mv1[j][i].x > -2 &&
					mITop->mv1[j][i].y < 2 && mITop->mv1[j][i].y > -2));
				SetMoving4x4(flag, mITop, j, i);
				SetMoving4x4(flag, mIBot, j, i);
				mITop->mv0[j][i].y /= 2;
				mIBot->mv0[j][i].y /= 2;
				if (bFrame) {
					mITop->mv1[j][i].y /= 2;
					mIBot->mv1[j][i].y /= 2;
				}
			}
			else
			{
				mIBot->mv0[j][i].x = mIBot->mv0[RSD(j)][RSD(i)].x;
				mIBot->mv0[j][i].y = mIBot->mv0[RSD(j)][RSD(i)].y;
				mITop->mv0[j][i].x = mITop->mv0[RSD(j)][RSD(i)].x;
				mITop->mv0[j][i].y = mITop->mv0[RSD(j)][RSD(i)].y;
				if (bFrame) {
					mIBot->mv1[j][i].x = mIBot->mv1[RSD(j)][RSD(i)].x;
					mIBot->mv1[j][i].y = mIBot->mv1[RSD(j)][RSD(i)].y;
					mITop->mv1[j][i].x = mITop->mv1[RSD(j)][RSD(i)].x;
					mITop->mv1[j][i].y = mITop->mv1[RSD(j)][RSD(i)].y;
				}
				SetMoving4x4(!((mITop->ref_idx0[j2][i2] == 0 && 
					mITop->mv0[j][i].x < 2 && mITop->mv0[j][i].x > -2 &&
					mITop->mv0[j][i].y < 2 && mITop->mv0[j][i].y > -2) ||
					(mITop->ref_idx0[j2][i2] == AVD_INVALID_REF_IDX && bFrame &&
					mITop->ref_idx1[j2][i2] == 0 && 
					mITop->mv1[j][i].x < 2 && mITop->mv1[j][i].x > -2 &&
					mITop->mv1[j][i].y < 2 && mITop->mv1[j][i].y > -2)),
					mITop, j, i);
				SetMoving4x4(!((mIBot->ref_idx0[j2][i2] == 0 && 
					mIBot->mv0[j][i].x < 2 && mIBot->mv0[j][i].x > -2 &&
					mIBot->mv0[j][i].y < 2 && mIBot->mv0[j][i].y > -2) ||
					(mIBot->ref_idx0[j2][i2] == AVD_INVALID_REF_IDX && bFrame &&
					mIBot->ref_idx1[j2][i2] == 0 && 
					mIBot->mv1[j][i].x < 2 && mIBot->mv1[j][i].x > -2 &&
					mIBot->mv1[j][i].y < 2 && mIBot->mv1[j][i].y > -2)),
					mIBot, j, i);
			}
		}
	}
}

void DpbFldPostProc(ImageParameters *img,StorablePicture *p)
{
	avdNativeUInt i,j, i2, j2, is8x8Inference;
	AVDStoredPicMotionInfo *mI;
	SliceType typeIPB;
	if (!p->used_for_reference)
		return;

	typeIPB = p->typeIPB;
	if (typeIPB == I_SLICE)
		return;

	mI = p->motionInfo;
	is8x8Inference = (!img->active_sps->frame_mbs_only_flag || 
		img->active_sps->direct_8x8_inference_flag);
	for (j = 0 ; j < (p->size_y>>2) ; j++)
	{
		int tmpY = RSD(j);
		j2 = (j>>1);
		for (i = 0 ; i < (p->size_x>>2) ; i++)
		{
			int tmpX = RSD(i);
			i2 = (i>>1);
			if (is8x8Inference){
				mI->mv0[j][i].x=mI->mv0[tmpY][tmpX].x;
				mI->mv0[j][i].y=mI->mv0[tmpY][tmpX].y;
				if (typeIPB == B_SLICE){
					mI->mv1[j][i].x=mI->mv1[tmpY][tmpX].x;
					mI->mv1[j][i].y=mI->mv1[tmpY][tmpX].y;
				}
			}
			SetMoving4x4(!(((mI->ref_idx0[j2][i2] == 0) && 
				(abs(mI->mv0[j][i].x)>>1 == 0) && 
				(abs(mI->mv0[j][i].y)>>1 == 0)) || 
				((mI->ref_idx0[j2][i2] == AVD_INVALID_REF_IDX) && 
				(typeIPB == B_SLICE) &&
				(mI->ref_idx1[j2][i2] == 0) && 
				(abs(mI->mv1[j][i].x)>>1 == 0) && 
				(abs(mI->mv1[j][i].y)>>1 == 0))), 
				mI, j, i);
		}
	}    
}

#if USE_SEINTERLACE
#if 1//ndef NEON
#define vld1q_u8(y1) *(y1)
#define vst1q_u8(y2, Y2) *(y2)=(Y2)
#define vrhaddq_u8(Y1, Y3) (((Y1)+(Y3))>>1)
#define uint8x16_t short
#define STEP 1
#else
#define STEP 16
#include <arm_neon.h>
#endif//_LINUX_ANDROID

/************************************************************************
 Sony Ericson de-Interlace
 Number Huang
2011-10-28
************************************************************************/
static void seDeinterlace(ImageParameters *img,avdUInt8 *dstY,avdUInt8 *dstU,avdUInt8 *dstV, 
						  avdUInt8 *srcY,avdUInt8 *srcU,avdUInt8 *srcV,
						  avdUInt8 *botY,avdUInt8 *botU,avdUInt8 *botV,
						  int height, int width) {
	unsigned char *y1; 
	unsigned char *y2;
	unsigned char *y3;
	unsigned char *u1,*v1;
	unsigned char *u2,*v2;
	unsigned char *u3,*v3;
	unsigned char *tmpSrc,*tmpDst;
	int x = 0, y = 0,i,j;
	int alignedRowWidth = width - (width&15);
	int uvwidth = width/2;
	int uvheight = height/2;
	int height1,width1,tmpHeight,tmpWidth;
	int uvwidth1 = uvwidth - (STEP-1);
	width1 = alignedRowWidth-(STEP-1);
	//unsigned char* clp255 = img->clipInfo->clip255;
	
	//copy the top to frame first
	tmpSrc = srcY;
	tmpDst = dstY;
	tmpHeight = uvheight;
	tmpWidth  = width;
	for (j=0;j<tmpHeight;j++)
	{
		memcpy(tmpDst,tmpSrc,tmpWidth);
		tmpSrc+=tmpWidth;
		tmpDst+=tmpWidth*2;
	}

	tmpSrc = srcU;
	tmpDst = dstU;
	tmpHeight = uvheight/2;
	tmpWidth  = width/2;
	for (j=0;j<tmpHeight;j++)
	{
		memcpy(tmpDst,tmpSrc,tmpWidth);
		tmpSrc+=tmpWidth;
		tmpDst+=tmpWidth*2;
	}

	tmpSrc = srcV;
	tmpDst = dstV;
	
	for (j=0;j<tmpHeight;j++)
	{
		memcpy(tmpDst,tmpSrc,tmpWidth);
		tmpSrc+=tmpWidth;
		tmpDst+=tmpWidth*2;
	}

	y1 =   dstY;
	u1 =   dstY + height * width;
	v1 =   u1 + height * width/4;	
	//We process uv and parts of y to start
	height1 = height/2 -2;
	for (y = 0; y < height1; y+=2) {
		y2 = y1 + width;
		y3 = y2 + width;

		u2 = u1 + uvwidth;
		u3 = u2 + uvwidth;

		v2 = v1 + uvwidth;
		v3 = v2 + uvwidth;

		for (x = 0; x < uvwidth1; x += STEP) {
			uint8x16_t Y1 = vld1q_u8(y1+x);
			uint8x16_t Y3 = vld1q_u8(y3+x);
			uint8x16_t U1 = vld1q_u8(u1+x);
			uint8x16_t U3 = vld1q_u8(u3+x);
			uint8x16_t V1 = vld1q_u8(v1+x);
			uint8x16_t V3 = vld1q_u8(v3+x);

			uint8x16_t Y2 = vrhaddq_u8(Y1, Y3);
			uint8x16_t U2 = vrhaddq_u8(U1, U3);
			uint8x16_t V2 = vrhaddq_u8(V1, V3);
			vst1q_u8(y2+x, Y2);
			vst1q_u8(u2+x, U2);
			vst1q_u8(v2+x, V2);
		}
		memcpy(botU,u2,width);botU+=uvwidth;
		memcpy(botV,v2,width);botV+=uvwidth;
		trace_overflow(img);
		//continue with the rest of y
		for ( ;x < width1; x += STEP) {
			uint8x16_t Y1 = vld1q_u8(y1+x);
			uint8x16_t Y3 = vld1q_u8(y3+x);

			uint8x16_t Y2 = vrhaddq_u8(Y1, Y3);
			vst1q_u8(y2+x, Y2);

		}
		memcpy(botY,y2,width);botY+=width;
		trace_overflow(img);
		y1  = y3;
		u1  = u3;
		v1  = v3;
	}

	trace_overflow(img);
	u2 = u3 + uvwidth;
	memcpy(u2, u3, uvwidth);
	v2 = v3 + uvwidth;
	memcpy(v2, v3, uvwidth);
	
	memcpy(botU, u3, uvwidth);
	memcpy(botV, v3, uvwidth);

	for (y = 0; y < height1; y+=2) {
		y2 = y1 + width;
		y3 = y2 + width;
		trace_overflow(img);
		for (x = 0; x < width1; x += STEP) {
			uint8x16_t Y1 = vld1q_u8(y1+x);
			uint8x16_t Y3 = vld1q_u8(y3+x);

			uint8x16_t Y2 = vrhaddq_u8(Y1, Y3);
			vst1q_u8(y2+x, Y2);

		}
		memcpy(botY,y2,width);botY+=width;
		y1 = y3;
	}

	y2 = y3 + width;
	memcpy(y2, y3, width);
	memcpy(botY,y3,width);
	
	
	return;
}
void interpolate16(VOCODECVIDEOBUFFER *outData, int stride,int width,int height) {
	unsigned char *y1;
	unsigned char *y2;
	unsigned char *y3;
	unsigned char* u1;
	unsigned char* u2;
	unsigned char* u3;
	unsigned char* v1;
	unsigned char* v2;
	unsigned char* v3;

	int x = 0, y = 0;
	int alignedRowWidth = stride;
	int uvwidth = alignedRowWidth/2;
	int uvheight = height/2;
	
	if((width&15) != 0){
		alignedRowWidth = (((width >> 4) + 1) << 4);
	}
	width = stride;
	y1 = (unsigned char*) (outData->data_buf[0]);
	u1 = (unsigned char*) (outData->data_buf[1]);
	v1 = (unsigned char*) (outData->data_buf[2]);

	//We process uv and parts of y to start
	for (y = 0; y < height/2 -2; y+=2) { //loop 1
		y2 = y1 + width;
		y3 = y2 + width;

		u2 = u1 + uvwidth;
		u3 = u2 + uvwidth;
		v2 = v1 + uvwidth;
		v3 = v2 + uvwidth;

		for (x = 0; x < uvwidth-15; x += STEP) {   //loop 1.1
			uint8x16_t Y1 = vld1q_u8(y1+x);
			uint8x16_t Y3 = vld1q_u8(y3+x);
			uint8x16_t U1 = vld1q_u8(u1+x);
			uint8x16_t U3 = vld1q_u8(u3+x);
			uint8x16_t V1 = vld1q_u8(v1+x);
			uint8x16_t V3 = vld1q_u8(v3+x);


			uint8x16_t Y2 = vrhaddq_u8(Y1, Y3);
			uint8x16_t U2 = vrhaddq_u8(U1, U3);
			uint8x16_t V2 = vrhaddq_u8(V1, V3);

			vst1q_u8(y2+x, Y2);
			vst1q_u8(u2+x, U2);
			vst1q_u8(v2+x, V2);
		}
		//y1 = y3;

		//continue with the rest of y
		for (;x < alignedRowWidth-15; x += STEP) {   //loop 1.2
			uint8x16_t Y1 = vld1q_u8(y1+x);
			uint8x16_t Y3 = vld1q_u8(y3+x);

			uint8x16_t Y2 = vrhaddq_u8(Y1, Y3);
			vst1q_u8(y2+x, Y2);

		}
		y1 = y3;

		u1 = u3;
		v1 = v3;
	}


	u2 = u3 + uvwidth;
	memcpy(u2, u3, uvwidth);
	v2 = v3 + uvwidth;
	memcpy(v2, v3, uvwidth);


	for (y = 0; y < height/2 -2; y+=2) {  //loop 2
		y2 = y1 + width;
		y3 = y2 + width;

		for (x = 0; x < alignedRowWidth-15; x += STEP) {  //loop 2.1
			uint8x16_t Y1 = vld1q_u8(y1+x);
			uint8x16_t Y3 = vld1q_u8(y3+x);

			uint8x16_t Y2 = vrhaddq_u8(Y1, Y3);
			vst1q_u8(y2+x, Y2);

		}
		y1 = y3;
	}

	y2 = y3 + width;
	memcpy(y2, y3, width);
	return;
}
/*
void allocateDeInterlaceBuf(H264VdLibParam* params,int height,int stride)
{
	int size = height*stride;
	params->deInterlaceOutBuf[0] = (avdUInt8*)voH264AlignedMalloc(img,999,size*3/2);
	params->deInterlaceOutBuf[1] = params->deInterlaceOutBuf[0]+size;
	params->deInterlaceOutBuf[2] = params->deInterlaceOutBuf[1]+size/4;
}*/
#endif//USE_SEINTERLACE
void DpbCombineYUVFields(ImageParameters *img,FrameStore *fs)
{

	TSpecitialInfo* specialInfo = img->specialInfo;
	TSizeInfo*	sizeInfo = img->sizeInfo;
	avdNativeInt uvPitch = (sizeInfo->yPlnPitch>>1);
	avdUInt8 *src1, *src2, *src3, *src4, *dst1, *dst2;
	avdNativeUInt i, i2;
	// combined frame used topField property;
	SliceType typeIPB;
	if(img->error<0||fs->top_field==NULL||fs->bottom_field==NULL)//it may happens when StoreErrorFrame
			return;
	typeIPB = (fs->bottom_field->typeIPB == B_SLICE) ? B_SLICE : 
		(fs->top_field->typeIPB == P_SLICE || fs->top_field->typeIPB == SP_SLICE) ? P_SLICE : I_SLICE;
	if (!fs->frame)
		fs->frame = GetStorablePic(img,FRAME, typeIPB, fs->top_field->size_x, (fs->top_field->size_y<<1), 
		fs->top_field->size_x_cr, (fs->top_field->size_y_cr<<1), 
		(typeIPB != I_SLICE && fs->top_field->used_for_reference));

	fs->frame->error	 = fs->top_field->error;
	fs->frame->timestamp = fs->top_field->timestamp?fs->top_field->timestamp:fs->bottom_field->timestamp;

#if USE_SEINTERLACE//damn SE,remove the code in future
	if ((img->vdLibPar->optFlag&OPT_ENABLE_DEINTERLACE)&& !fs->frame->used_for_reference)
		 
	{
		H264VdLibParam* params = img->vdLibPar;
		int height = params->sizeInfo.frameHeight;
		int stride = img->sizeInfo->width;
		int width  = params->sizeInfo.frameWidth;
		//AvdLog2(LL_INFO,"@Interlace combinefield\n");
#if 1
		
		src1 = fs->top_field->plnY;
		//src2 = fs->bottom_field->plnY;
		dst1 = fs->frame->plnY;
		for (i=0; i<fs->top_field->size_y; i++)
		{
			memcpy(dst1, src1, fs->top_field->size_x * sizeof(avdUInt8));     // top field
			//memcpy(dst1 + sizeInfo->yPlnPitch, src2, fs->bottom_field->size_x * sizeof(avdUInt8)); // bottom field
			src1 += sizeInfo->yPlnPitch;
			//src2 += sizeInfo->yPlnPitch;
			dst1 += (sizeInfo->yPlnPitch<<1);
		}
		src1 = fs->top_field->plnU;
		//src2 = fs->bottom_field->plnU;
		src3 = fs->top_field->plnV;
		//src4 = fs->bottom_field->plnV;
		dst1 = fs->frame->plnU;
		dst2 = fs->frame->plnV;
		for (i=0; i<fs->top_field->size_y_cr; i++)
		{
			i2 = (i<<1);
			memcpy(dst1,     src1,    fs->top_field->size_x_cr * sizeof(avdUInt8));
			//memcpy(dst1 + uvPitch, src2, fs->bottom_field->size_x_cr * sizeof(avdUInt8));
			memcpy(dst2,     src3,    fs->top_field->size_x_cr * sizeof(avdUInt8));
			//memcpy(dst2 + uvPitch, src4, fs->bottom_field->size_x_cr * sizeof(avdUInt8));
			src1 += uvPitch;
			//src2 += uvPitch;
			src3 += uvPitch;
			//src4 += uvPitch;
			dst1 += sizeInfo->yPlnPitch;
			dst2 += sizeInfo->yPlnPitch;
		}
#else
		seDeinterlace(img,	 fs->frame->plnY,		fs->frame->plnU,		fs->frame->plnV, 
							 fs->top_field->plnY,	fs->top_field->plnU,	fs->top_field->plnV,
							 fs->bottom_field->plnY,fs->bottom_field->plnU, fs->bottom_field->plnV,
							 img->vdLibPar->sizeInfo.frameHeight, img->vdLibPar->sizeInfo.frameWidth);
#endif		
		return;
	}
#endif//
	src1 = fs->top_field->plnY;
	src2 = fs->bottom_field->plnY;
	dst1 = fs->frame->plnY;
	for (i=0; i<fs->top_field->size_y; i++)
	{
		memcpy(dst1, src1, fs->top_field->size_x * sizeof(avdUInt8));     // top field
		memcpy(dst1 + sizeInfo->yPlnPitch, src2, fs->bottom_field->size_x * sizeof(avdUInt8)); // bottom field
		src1 += sizeInfo->yPlnPitch;
		src2 += sizeInfo->yPlnPitch;
		dst1 += (sizeInfo->yPlnPitch<<1);
	}
	src1 = fs->top_field->plnU;
	src2 = fs->bottom_field->plnU;
	src3 = fs->top_field->plnV;
	src4 = fs->bottom_field->plnV;
	dst1 = fs->frame->plnU;
	dst2 = fs->frame->plnV;
	for (i=0; i<fs->top_field->size_y_cr; i++)
	{
		i2 = (i<<1);
		memcpy(dst1,     src1,    fs->top_field->size_x_cr * sizeof(avdUInt8));
		memcpy(dst1 + uvPitch, src2, fs->bottom_field->size_x_cr * sizeof(avdUInt8));
		memcpy(dst2,     src3,    fs->top_field->size_x_cr * sizeof(avdUInt8));
		memcpy(dst2 + uvPitch, src4, fs->bottom_field->size_x_cr * sizeof(avdUInt8));
		src1 += uvPitch;
		src2 += uvPitch;
		src3 += uvPitch;
		src4 += uvPitch;
		dst1 += sizeInfo->yPlnPitch;
		dst2 += sizeInfo->yPlnPitch;
	}

}

void dpb_combine_field(ImageParameters *img,FrameStore *fs)
{
	avdNativeUInt i,j, i2, j2, bFrame;
	avdInt64 m, n;
	AVDStoredPicMotionInfo *mITop, *mIBot, *motionInfo;
	SliceType topIPB, botIPB;
	StorablePicture *topFld, *botFld, *frame;
	frame = fs->frame;
	topFld = fs->top_field;
	botFld = fs->bottom_field;
	if(img->error<0)//it may happens when StoreErrorFrame
		return;
	if (!frame){
		DpbCombineYUVFields(img,fs);
		frame = fs->frame;
	}
	
	if (!topFld || !botFld || (frame&&frame->isCombinedOrSplit))
		return;

	// ToBeChecked below;
	fs->poc = frame->poc = frame->frame_poc = min (topFld->poc, botFld->poc);
	botFld->frame_poc = topFld->frame_poc = botFld->top_poc = 
		frame->top_poc = topFld->poc;
	frame->bottom_poc = topFld->bottom_poc = botFld->poc;

	frame->used_for_reference = (topFld->used_for_reference && 
		botFld->used_for_reference);

	frame->is_long_term = (topFld->is_long_term && 
		botFld->is_long_term);
	if (frame->is_long_term) 
		frame->long_term_frame_idx = fs->long_term_frame_idx;

	frame->top_field    = topFld;
	frame->bottom_field = botFld;
	topFld->frame = botFld->frame = frame;
	topFld->top_field = botFld->top_field = topFld;
	topFld->bottom_field = botFld->bottom_field = botFld;
	frame->isCombinedOrSplit = 1;
	// combined frame typeIPB is the same as top_field;
	topIPB = topFld->typeIPB;
	if (topIPB == I_SLICE || !topFld->used_for_reference)
		return;

	bFrame  = (topIPB == B_SLICE);
	botIPB = botFld->typeIPB;
	motionInfo = frame->motionInfo;
	mITop   = topFld->motionInfo;
	mIBot   = botFld->motionInfo;
	for (i=0;i<(img->listXsize[LIST_1]+1)/2;i++) {
		m = (topIPB != I_SLICE) ? (topFld->ref_pic_num[LIST_1][2*i]/2)*2 : INT_MAX;
		n = (botIPB != I_SLICE) ? (botFld->ref_pic_num[LIST_1][2*i]/2)*2 : INT_MAX;
		frame->ref_pic_num[LIST_1][i] = min (m, n);
	}

	for (i=0;i<(img->listXsize[LIST_0]+1)/2;i++)
	{
		m = (topIPB != I_SLICE) ? (topFld->ref_pic_num[LIST_0][2*i]/2)*2 : INT_MAX;
		n = (botIPB != I_SLICE) ? (botFld->ref_pic_num[LIST_0][2*i]/2)*2 : INT_MAX;
		frame->ref_pic_num[LIST_0][i] = min (m, n);
	}

	//! Use inference flag to remap mvs/references 
	//! Generate Frame parameters from field information.
	if (bFrame && motionInfo->mv1==NULL) 
		AVD_ERROR_CHECK2(img,"bFrame && motionInfo->mv1==NULL",ERROR_InvalidBitstream);
		
	if (botFld->poc > topFld->poc){
		for (i=0 ; i<topFld->size_x/4 ; i++){
			i2 = (i>>1);
			for (j=0 ; j<topFld->size_y/2 ; j++){        
				j2 = (j>>1);
				motionInfo->mv0[j][i].x = mITop->mv0[j2][i].x;
				motionInfo->mv0[j][i].y = mITop->mv0[j2][i].y;
				if (bFrame) {
					motionInfo->mv1[j][i].x = mITop->mv1[j2][i].x;
					motionInfo->mv1[j][i].y = mITop->mv1[j2][i].y; 
				}
				if (!(i&1) && !(j&1)){
					motionInfo->ref_idx0[j2][i2] = mITop->ref_idx0[j2>>1][i2];
					if (bFrame)
						motionInfo->ref_idx1[j2][i2] = 
						mITop->ref_idx1[j2>>1][i2];
				}
			}
		}
	}
	else {
		for (i=0 ; i<botFld->size_x/4 ; i++){
			i2 = (i>>1);
			for (j=0 ; j<botFld->size_y/2 ; j++){        
				j2 = (j>>1);
				motionInfo->mv0[j][i].x = mIBot->mv0[j2][i].x;
				motionInfo->mv0[j][i].y = mIBot->mv0[j2][i].y;
				if (bFrame) {
					motionInfo->mv1[j][i].x = mIBot->mv1[j2][i].x;
					motionInfo->mv1[j][i].y = mIBot->mv1[j2][i].y; 
				}
				if (!(i&1) && !(j&1)){
					motionInfo->ref_idx0[j2][i2] = mIBot->ref_idx0[j2>>1][i2];
					if (bFrame){
						motionInfo->ref_idx1[j2][i2] = 
							mIBot->ref_idx1[j2>>1][i2];
					}
				}
			}     
		}
	}

	for (i=0 ; i<topFld->size_x/4 ; i++){
		i2 = (i>>1);
		for (j=0 ; j<topFld->size_y/2 ; j++){
			j2 = (j>>1);
			//! Use inference flag to remap mvs/references
			motionInfo->mv0[j][i].x=motionInfo->mv0[RSD(j)][RSD(i)].x;
			motionInfo->mv0[j][i].y=motionInfo->mv0[RSD(j)][RSD(i)].y;
			if (bFrame) {
				motionInfo->mv1[j][i].x=motionInfo->mv1[RSD(j)][RSD(i)].x;
				motionInfo->mv1[j][i].y=motionInfo->mv1[RSD(j)][RSD(i)].y;        
			}

			SetMoving4x4(!(((motionInfo->ref_idx0[j2][i2] == 0) && 
				(motionInfo->mv0[j][i].x < 2 && motionInfo->mv0[j][i].x > -2 && 
				motionInfo->mv0[j][i].y < 2 && motionInfo->mv0[j][i].y > -2)) || 
				(motionInfo->ref_idx0[j2][i2] == AVD_INVALID_REF_IDX && bFrame &&
				motionInfo->ref_idx1[j2][i2] == 0 && 
				motionInfo->mv1[j][i].x < 2 && motionInfo->mv1[j][i].x > -2 && 
				motionInfo->mv1[j][i].y < 2 && motionInfo->mv1[j][i].y > -2)), 
				motionInfo, j, i);
		}      
	}

	//scaling of mvs/references needs to be done separately 
	for (i=0 ; i<topFld->size_x/4 ; i++){
		i2 = (i>>1);
		for (j=0 ; j<topFld->size_y/2 ; j++){
			j2 = (j>>1);
			motionInfo->mv0[j][i].y *= 2; 
			if (bFrame)
				motionInfo->mv1[j][i].y *= 2;
			if (!(i&1) && !(j&1)){
				if (fs->bottom_field->poc>fs->top_field->poc)
				{
					if (motionInfo->ref_idx0[j2][i2] != AVD_INVALID_REF_IDX)
						motionInfo->ref_idx0[j2][i2] >>= 1;
					if (bFrame && motionInfo->ref_idx1[j2][i2] != AVD_INVALID_REF_IDX)
						motionInfo->ref_idx1[j2][i2] >>= 1;
				}
				else {
					if (motionInfo->ref_idx0[j2][i2] > 1 && 
						(motionInfo->ref_idx0[j2][i2]&1))
						motionInfo->ref_idx0[j2][i2] = 
						(motionInfo->ref_idx0[j2][i2] - 2)>>1;            
					else if (motionInfo->ref_idx0[j2][i2] != AVD_INVALID_REF_IDX)
						motionInfo->ref_idx0[j2][i2] >>= 1;

					if (bFrame) {
						if (motionInfo->ref_idx1[j2][i2] > 1 && 
							(motionInfo->ref_idx1[j2][i2]&1))
							motionInfo->ref_idx1[j2][i2] = 
							(motionInfo->ref_idx1[j2][i2] - 2)>>1;
						else if (motionInfo->ref_idx1[j2][i2] != AVD_INVALID_REF_IDX)
							motionInfo->ref_idx1[j2][i2] >>= 1;
					}
				}
			}
		}     
	}
}
#endif//FEATURE_INTERLACE
