#include <limits.h>

#include "global.h"
//#include "erc_api.h"
#include "header.h"
#include "image.h"
#include "mbuffer.h"
#include "memalloc.h"
#include "output.h"
//#include "mbuffer_mvc.h"
#include "ifunctions.h"
#include  "output.h"


static void insert_picture_in_dpb    (H264DEC_G *pDecGlobal, FrameStore* fs, StorablePicture* p);

static VO_S32 output_one_frame_from_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb);
static VO_S32 get_smallest_poc         (DecodedPictureBuffer *p_Dpb, int *poc,int * pos);
static int  remove_unused_frame_from_dpb (H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb);

//static void gen_field_ref_ids        (H264DEC_G *pDecGlobal, StorablePicture *p);
static int  is_used_for_reference    (FrameStore* fs);
static int  is_short_term_reference  (FrameStore* fs);
static int  is_long_term_reference   (FrameStore* fs);

#define MAX_LIST_SIZE 33



int getDpbSize(seq_parameter_set_rbsp_t *active_sps)
{
  int pic_size = (active_sps->pic_width_in_mbs_minus1 + 1) * (active_sps->pic_height_in_map_units_minus1 + 1) * (active_sps->frame_mbs_only_flag?1:2) * 384;

  int size = 0;

  switch (active_sps->level_idc)
  {
  case 9:
    size = 152064;
    break;
  case 10:
    size = 152064;
    break;
  case 11:
    if (!IS_FREXT_PROFILE(active_sps->profile_idc) && (active_sps->constrained_set3_flag == 1))
      size = 152064;
    else
      size = 345600;
    break;
  case 12:
    size = 912384;
    break;
  case 13:
    size = 912384;
    break;
  case 20:
    size = 912384;
    break;
  case 21:
    size = 1824768;
    break;
  case 22:
    size = 3110400;
    break;
  case 30:
    size = 3110400;
    break;
  case 31:
    size = 6912000;
    break;
  case 32:
    size = 7864320;
    break;
  case 40:
    size = 12582912;
    break;
  case 41:
    size = 12582912;
    break;
  case 42:
    size = 13369344;
    break;
  case 50:
    size = 42393600;
    break;
  case 51:
    size = 70778880;
    break;
  default:
    size = 70778880;//warning_dtt
    break;
  }

  size /= pic_size;
  size = imin( size, 16);
  size = imax( size, 1);

  if (active_sps->vui_parameters_present_flag && active_sps->vui_seq_parameters.bitstream_restriction_flag)
  {
    int size_vui;
    //if ((int)active_sps->vui_seq_parameters.max_dec_frame_buffering > size)
    //warning_dtt
    size_vui = imax (1, active_sps->vui_seq_parameters.max_dec_frame_buffering);
    if(size_vui < size)
    {
      //printf("Warning: max_dec_frame_buffering(%d) is less than DPB size(%d) calculated from Profile/Level.\n", size_vui, size);
	  size_vui = size;
    }
    size = size_vui;    
	size = imin( size, 16);
	size = imax( size, 1);
  }

  return size;
}

void check_num_ref(DecodedPictureBuffer *p_Dpb)
{
  if ((int)(p_Dpb->ltref_frames_in_buffer +  p_Dpb->ref_frames_in_buffer ) > (imax(1, p_Dpb->num_ref_frames)))
  {
    error ("Max. number of reference frames exceeded. Invalid stream.", 500);
  }
}

void init_dpb(H264DEC_G *pDecGlobal, DecodedPictureBuffer *p_Dpb)
{
  unsigned i; 
  seq_parameter_set_rbsp_t *active_sps = pDecGlobal->pCurSlice->active_sps;
  VO_U32 size;
  //p_Dpb->p_Vid = p_Vid;

  if (p_Dpb->init_done)
  {
#if USE_FRAME_THREAD
	  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	  {
		  return;
	  }
#endif
    free_dpb(pDecGlobal,p_Dpb);
	if (pDecGlobal->dec_picture)
	{
	  pDecGlobal->dec_picture = NULL;
	}
  }
  pDecGlobal->pic_head = pDecGlobal->pic_end = 0;
#if (MVC_EXTENSION_ENABLE)
  if(pDecGlobal->profile_idc == MVC_HIGH || pDecGlobal->profile_idc == STEREO_HIGH)
    p_Dpb->size = GetMaxDecFrameBuffering(pDecGlobal) + 2;
  else
    p_Dpb->size = getDpbSize(active_sps);

  if(active_sps->profile_idc == MVC_HIGH || active_sps->profile_idc == STEREO_HIGH)
    p_Dpb->size = (p_Dpb->size<<1) + 2;
#else
  p_Dpb->size = getDpbSize(active_sps);

  
#endif

  //p_Dpb->size = active_sps->frame_mbs_only_flag?active_sps->num_ref_frames:active_sps->num_ref_frames*2;
  p_Dpb->num_ref_frames = active_sps->num_ref_frames; 

  if((active_sps->pic_width_in_mbs_minus1 + 1) * (active_sps->pic_height_in_map_units_minus1 + 1) * (active_sps->frame_mbs_only_flag?1:2)>640*480)
    p_Dpb->size = active_sps->num_ref_frames;//warning_dtt

  if(active_sps->frame_mbs_only_flag)
    size = p_Dpb->size + 2*pDecGlobal->nThdNum + 1 + pDecGlobal->extra_dpb;
  else
  	size = p_Dpb->size + 2*pDecGlobal->nThdNum + 1 + pDecGlobal->extra_dpb;
 //LOGI("fifo size %d\n", size);
  if(pDecGlobal->pCurSlice->active_sps->frame_mbs_only_flag)
  {
  p_Dpb->storable_pic_buffer[0] = (StorablePicture*)voMalloc(pDecGlobal,size*sizeof(StorablePicture));  
  //p_Dpb->ref_pic_buffer = (StorablePicture**)voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*16*2*size*sizeof(StorablePicture*));  
  p_Dpb->ref_idx_buffer = voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*16*2*size);  
  p_Dpb->mv_buffer = (VO_S32*)voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*16*2*size*sizeof(VO_S32));  
  p_Dpb->imgY_buffer = (VO_U8*)voMalloc(pDecGlobal,size*(pDecGlobal->height+2*pDecGlobal->iLumaPadY)*(pDecGlobal->width+2*pDecGlobal->iLumaPadX)); 
  p_Dpb->imgUV_buffer[0] = (VO_U8*)voMalloc(pDecGlobal,size*(pDecGlobal->height_cr+2*pDecGlobal->iChromaPadY)*(pDecGlobal->width_cr+2*pDecGlobal->iChromaPadX)); 
  p_Dpb->imgUV_buffer[1] = (VO_U8*)voMalloc(pDecGlobal,size*(pDecGlobal->height_cr+2*pDecGlobal->iChromaPadY)*(pDecGlobal->width_cr+2*pDecGlobal->iChromaPadX));  
  p_Dpb->mb_type_buffer = (VO_S32*)voMalloc(pDecGlobal,size*pDecGlobal->FrameSizeInMbs*sizeof(VO_S32));
  {
		StorablePicture* pStPic0 = p_Dpb->storable_pic_buffer[0];
		//StorablePicture** pRefPic = p_Dpb->ref_pic_buffer;
		VO_S8* pRefIdx = p_Dpb->ref_idx_buffer;
		VO_S32* pMv = p_Dpb->mv_buffer;
		VO_S32* pType = p_Dpb->mb_type_buffer;
		const VO_U32 stride = pDecGlobal->FrameSizeInMbs*16;
		VO_U8 *pImgY, *pImgU, *pImgV;
		VO_U32 stride_y, stride_uv;

		pImgY = p_Dpb->imgY_buffer + pDecGlobal->iLumaPadY*pDecGlobal->iLumaStride+pDecGlobal->iLumaPadX;
		stride_y = (pDecGlobal->height+2*pDecGlobal->iLumaPadY)*pDecGlobal->iLumaStride;
		stride_uv = pDecGlobal->iChromaPadY*pDecGlobal->iChromaStride+pDecGlobal->iChromaPadX;
		pImgU = p_Dpb->imgUV_buffer[0] + stride_uv;
		pImgV = p_Dpb->imgUV_buffer[1] + stride_uv;
		stride_uv = (pDecGlobal->height_cr+2*pDecGlobal->iChromaPadY)*pDecGlobal->iChromaStride;

	  for (i = size; i > 0; --i )
	  {
		  pStPic0->ref_idx[0] = pRefIdx;
		  pStPic0->ref_idx[1] = pRefIdx +stride;
		  pRefIdx += 2*stride;

		  //pStPic0->ref_pic[0] = pRefPic;
		  //pStPic0->ref_pic[1] = pRefPic + stride;
		  //pRefPic += 2*stride;

		  pStPic0->mv[0] = pMv;
		  pStPic0->mv[1] = pMv + stride;
		  pMv += 2*stride;
		  
          pStPic0->refY = pStPic0->imgY = pImgY;
		  pImgY += stride_y;
		  pStPic0->refUV[0] = pStPic0->imgUV[0] = pImgU;	  
		  pImgU += stride_uv;
		  pStPic0->refUV[1] = pStPic0->imgUV[1] = pImgV;		  
		  pImgV += stride_uv;

		  pStPic0->mb_type_buffer = pType;
		  pType+=(stride>>4);
		  ++pStPic0;

	  }
  }  
  }
  else
  {
  p_Dpb->storable_pic_buffer[0] = (StorablePicture*)voMalloc(pDecGlobal,3*size*sizeof(StorablePicture));  
  p_Dpb->storable_pic_buffer[1] = p_Dpb->storable_pic_buffer[0] + size;
  p_Dpb->storable_pic_buffer[2] = p_Dpb->storable_pic_buffer[1] + size;
  p_Dpb->ref_pic_buffer = (StorablePicture**)voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*16*2*size*sizeof(StorablePicture*));  
  p_Dpb->ref_idx_buffer = voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*16*2*size);  
  p_Dpb->mv_buffer = (VO_S32*)voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*16*2*size*sizeof(VO_S32));  
  p_Dpb->mb_type_buffer = (VO_S32*)voMalloc(pDecGlobal,size*pDecGlobal->FrameSizeInMbs*sizeof(VO_S32));
  p_Dpb->imgY_buffer = (VO_U8*)voMalloc(pDecGlobal,size*(pDecGlobal->height+2*pDecGlobal->iLumaPadY)*(pDecGlobal->width+2*pDecGlobal->iLumaPadX)); 
  p_Dpb->imgUV_buffer[0] = (VO_U8*)voMalloc(pDecGlobal,size*(pDecGlobal->height_cr+2*pDecGlobal->iChromaPadY)*(pDecGlobal->width_cr+2*pDecGlobal->iChromaPadX)); 
  p_Dpb->imgUV_buffer[1] = (VO_U8*)voMalloc(pDecGlobal,size*(pDecGlobal->height_cr+2*pDecGlobal->iChromaPadY)*(pDecGlobal->width_cr+2*pDecGlobal->iChromaPadX));  

  {
		StorablePicture* pStPic0 = p_Dpb->storable_pic_buffer[0];
		StorablePicture* pStPic1 = p_Dpb->storable_pic_buffer[1];
		StorablePicture* pStPic2 = p_Dpb->storable_pic_buffer[2];
		StorablePicture** pRefPic = p_Dpb->ref_pic_buffer;
		VO_S8* pRefIdx = p_Dpb->ref_idx_buffer;
		VO_S32* pMv = p_Dpb->mv_buffer;
		VO_S32* pType = p_Dpb->mb_type_buffer;
		const VO_U32 stride = pDecGlobal->FrameSizeInMbs*8;

		VO_U8 *pImgY, *pImgU, *pImgV;
		VO_U32 stride_y, stride_uv;

		pImgY = p_Dpb->imgY_buffer + pDecGlobal->iLumaPadY*pDecGlobal->iLumaStride+pDecGlobal->iLumaPadX;
		stride_y = (pDecGlobal->height+2*pDecGlobal->iLumaPadY)*pDecGlobal->iLumaStride;
		stride_uv = pDecGlobal->iChromaPadY*pDecGlobal->iChromaStride+pDecGlobal->iChromaPadX;
		pImgU = p_Dpb->imgUV_buffer[0] + stride_uv;
		pImgV = p_Dpb->imgUV_buffer[1] + stride_uv;
		stride_uv = (pDecGlobal->height_cr+2*pDecGlobal->iChromaPadY)*pDecGlobal->iChromaStride;
	  for (i = size; i > 0; --i )
	  {
		  pStPic0->ref_idx[0] = pStPic1->ref_idx[0] = pStPic2->ref_idx[0] = pRefIdx;
		  pStPic0->ref_idx[1] = pStPic1->ref_idx[1] = pStPic2->ref_idx[1] = pRefIdx += 2*stride;
		  pRefIdx += 2*stride;

          pStPic0->ref_pic[0] = pStPic1->ref_pic[0] = pStPic2->ref_pic[0] = pRefPic;
		  pStPic0->ref_pic[1] = pStPic1->ref_pic[1] = pStPic2->ref_pic[1] = pRefPic += 2*stride;
		  pRefPic += 2*stride;

		  pStPic0->mv[0] = pStPic1->mv[0] = pStPic2->mv[0] = pMv;
		  pStPic0->mv[1] = pStPic1->mv[1] = pStPic2->mv[1] = pMv += 2*stride;
		  pMv += 2*stride;
		  

		  pStPic0->mb_type_buffer = pType;
		  pStPic1->mb_type_buffer = pType;
		  pStPic2->mb_type_buffer = pType;
		  pType += (stride>>3);

		  pStPic1->refY = pStPic0->refY = pStPic2->imgY = pStPic1->imgY = pStPic0->imgY = pImgY;
		  pStPic2->refY = pImgY+pDecGlobal->iLumaStride;	//2nd line
		  pImgY += stride_y;

		  pStPic1->refUV[0] = pStPic0->refUV[0] = pStPic2->imgUV[0] = pStPic1->imgUV[0] = pStPic0->imgUV[0] = pImgU;
		  pStPic2->refUV[0] = pImgU + pDecGlobal->iChromaStride;  //2nd line
		  pImgU += stride_uv;
		  pStPic1->refUV[1] = pStPic0->refUV[1] = pStPic2->imgUV[1] = pStPic1->imgUV[1] = pStPic0->imgUV[1] = pImgV;
		  pStPic2->refUV[1] = pImgV + pDecGlobal->iChromaStride;  //2nd line
		  pImgV += stride_uv;

		  ++pStPic0;
		  ++pStPic1;
		  ++pStPic2;
	  }
  }
  }
  pDecGlobal->vFrameIndexFIFO.r_idx = pDecGlobal->vFrameIndexFIFO.w_idx = 0;

  for(i = 0; i < size; i++)
  {
	FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
  }
  p_Dpb->used_size = 0;
  p_Dpb->used_ref_index = 0;
  p_Dpb->last_picture = NULL;
  p_Dpb->remove_size = 0;
  p_Dpb->ref_frames_in_buffer = 0;
  p_Dpb->ltref_frames_in_buffer = 0;

  p_Dpb->fs = voMalloc(pDecGlobal,p_Dpb->size*sizeof (FrameStore*));
  if (NULL==p_Dpb->fs)
    no_mem_exit("init_dpb: p_Dpb->fs");

  p_Dpb->fs_ref = voMalloc(pDecGlobal,p_Dpb->size*sizeof (FrameStore*));
  if (NULL==p_Dpb->fs_ref)
    no_mem_exit("init_dpb: p_Dpb->fs_ref");

  p_Dpb->fs_ltref = voMalloc(pDecGlobal,p_Dpb->size*sizeof (FrameStore*));
  if (NULL==p_Dpb->fs_ltref)
    no_mem_exit("init_dpb: p_Dpb->fs_ltref");
  if (pDecGlobal->nThdNum==0)
  {
	  pDecGlobal->nThdNum = 1;
  }
  p_Dpb->p_remove= voMalloc(pDecGlobal,32*sizeof (StorablePicture*));
  if (NULL==p_Dpb->p_remove)
    no_mem_exit("init_dpb: p_Dpb->fs_remove");

  for (i = 0; i < p_Dpb->size; i++)
  {
    p_Dpb->fs[i]       = alloc_frame_store(pDecGlobal);
    p_Dpb->fs_ref[i]   = NULL;
    p_Dpb->fs_ltref[i] = NULL;
	p_Dpb->p_remove[i] = NULL;
  }



  p_Dpb->last_output_poc = INT_MIN;

#if (MVC_EXTENSION_ENABLE)
  p_Dpb->last_output_view_id = -1;
#endif

  pDecGlobal->last_has_mmco_5 = 0;

  p_Dpb->init_done = 1;

}

void free_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb)
{
  //VideoParameters *p_Vid = p_Dpb->p_Vid;
  unsigned i;
  if (p_Dpb->fs)
  {
    for (i=0; i<p_Dpb->size; i++)
    {
      free_frame_store(pDecGlobal,p_Dpb->fs[i]);
    }
    voFree (pDecGlobal,p_Dpb->fs);
    p_Dpb->fs=NULL;
  }
  if (p_Dpb->fs_ref)
  {
    voFree (pDecGlobal,p_Dpb->fs_ref);
  }
  if (p_Dpb->fs_ltref)
  {
    voFree (pDecGlobal,p_Dpb->fs_ltref);
  }
  if (p_Dpb->p_remove)
  {
    voFree (pDecGlobal,p_Dpb->p_remove);
  }
  p_Dpb->last_output_poc = INT_MIN;
#if (MVC_EXTENSION_ENABLE)
  p_Dpb->last_output_view_id = -1;
#endif

  p_Dpb->init_done = 0;

  if(p_Dpb->ref_pic_buffer)
  {
    voFree(pDecGlobal,p_Dpb->ref_pic_buffer);
    p_Dpb->ref_pic_buffer = NULL;
  }
  if(p_Dpb->mb_type_buffer)
  {
    voFree(pDecGlobal,p_Dpb->mb_type_buffer);
    p_Dpb->mb_type_buffer = NULL;
  }
  if(p_Dpb->ref_idx_buffer)
  {
    voFree(pDecGlobal,p_Dpb->ref_idx_buffer);
    p_Dpb->ref_idx_buffer = NULL;
  }
  if(p_Dpb->mv_buffer)
  {
    voFree(pDecGlobal,p_Dpb->mv_buffer);
    p_Dpb->mv_buffer = NULL;
  }
  if(p_Dpb->imgY_buffer)
  {
    voFree(pDecGlobal,p_Dpb->imgY_buffer);
    p_Dpb->imgY_buffer = NULL;
  }
  if(p_Dpb->imgUV_buffer[0])
  {
    voFree(pDecGlobal,p_Dpb->imgUV_buffer[0]);
    p_Dpb->imgUV_buffer[0] = NULL;
  }
  if(p_Dpb->imgUV_buffer[1])
  {
    voFree(pDecGlobal,p_Dpb->imgUV_buffer[1]);
    p_Dpb->imgUV_buffer[1] = NULL;
  }
  if (p_Dpb->storable_pic_buffer[0])
  {
	  voFree(pDecGlobal,p_Dpb->storable_pic_buffer[0]);
	  
  }
}

VO_U32 FrameBufCtl(FIFOTYPE *priv, VO_U32 img_index , const VO_U32 flag)
{
    VO_U32 temp = (VO_U32)-1;
    VO_U32 w_idx = 0;
    if(img_index==(VO_U32)-1)
	  return img_index;
    w_idx = priv->w_idx;    /* Save the old index before proceeding */
    if (flag == FIFO_WRITE){ /*write data*/
 //printf("write %d\n", img_index);
        /* Save it to buffer */
        if (((priv->w_idx + 1) % FIFO_NUM) == priv->r_idx) {
            /* Adjust read index since buffer is full */
            /* Keep the latest one and drop the oldest one */
            priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
        }
        priv->img_index[priv->w_idx] = img_index;
        priv->w_idx = (priv->w_idx + 1) % FIFO_NUM;

    }else{/*read data*/
		if ((priv->r_idx == w_idx)){
            return temp;/*there is no voFree buffer*/
		}else{
            temp = priv->img_index[priv->r_idx];
            priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
 //printf("read %d\n", temp);
            }
        }
     return temp;

}

FrameStore* alloc_frame_store(H264DEC_G *pDecGlobal)
{
  FrameStore *f;

  f = voMalloc(pDecGlobal,sizeof(FrameStore));
  if (NULL==f)
    no_mem_exit("alloc_frame_store: f");

  f->is_used      = 0;
  f->is_reference = 0;
  f->is_long_term = 0;
  f->is_orig_reference = 0;

  f->is_output = 0;

  f->frame        = NULL;;
  f->top_field    = NULL;
  f->bottom_field = NULL;

  return f;
}


void update_ref_list(DecodedPictureBuffer *p_Dpb)
{
  unsigned i, j;
  for (i=0, j=0; i<p_Dpb->used_size; i++)
  {
    if (is_short_term_reference(p_Dpb->fs[i]))
    {
      p_Dpb->fs_ref[j++]=p_Dpb->fs[i];
    }
  }

  p_Dpb->ref_frames_in_buffer = j;

  while (j<p_Dpb->size)
  {
    p_Dpb->fs_ref[j++]=NULL;
  }
}


#if (MVC_EXTENSION_ENABLE)
void update_ltref_list(DecodedPictureBuffer *p_Dpb, int curr_view_id)
{
  unsigned i, j;
  for (i=0, j=0; i<p_Dpb->used_size; i++)
  {
    if (is_long_term_reference(p_Dpb->fs[i]) && p_Dpb->fs[i]->view_id == curr_view_id)
    {
      p_Dpb->fs_ltref[j++] = p_Dpb->fs[i];
    }
  }

  p_Dpb->ltref_frames_in_buffer = j;

  while (j<p_Dpb->size)
  {
    p_Dpb->fs_ltref[j++]=NULL;
  }
}
#else
void update_ltref_list(DecodedPictureBuffer *p_Dpb)
{
  unsigned i, j;
  for (i=0, j=0; i<p_Dpb->used_size; i++)
  {
    if (is_long_term_reference(p_Dpb->fs[i]))
    {
      p_Dpb->fs_ltref[j++] = p_Dpb->fs[i];
    }
  }

  p_Dpb->ltref_frames_in_buffer = j;

  while (j<p_Dpb->size)
  {
    p_Dpb->fs_ltref[j++]=NULL;
  }
}
#endif

StorablePicture* alloc_storable_picture(H264DEC_G *pDecGlobal, PictureStructure structure)
{
	VO_S32 stride = pDecGlobal->FrameSizeInMbs*16;
	StorablePicture *s;
	DecodedPictureBuffer *p_Dpb = pDecGlobal->p_Dpb;
	imgpel *     imgY;         //!< Y picture component
	imgpel *     imgUV[2];        //!< U and V picture components
	imgpel *     refY;         
	imgpel *     refUV[2];        
	VO_S8 *ref_idx[2];
	struct storable_picture **ref_pic[2];
	VO_S32* mv[2];
	VO_U8* field;
	VO_S32* type;
	VO_U32 i,j=0;

// 	for(j = 0;j <p_Dpb->remove_size;j++)
	while (j<p_Dpb->remove_size)
	{
	  //LOGI("remove_size:%d\r\n",p_Dpb->remove_size);
	  if(p_Dpb->p_remove[j]->bFinishOutput && p_Dpb->p_remove[j]->removable)
	  {
	    i = p_Dpb->p_remove[j]->cur_buf_index;
// if (p_Dpb->p_remove[j]->errFlag)
// {
// 	printf("remove %d\n",i);
// }
        if(pDecGlobal->interlace)
        {
          VO_S32 w_idx = pDecGlobal->vFrameIndexFIFO.w_idx;
		  VO_S32 r_idx = pDecGlobal->vFrameIndexFIFO.r_idx;
		  VO_S32 l,k;
		  VO_S32 find = 0;
		  if(w_idx<r_idx)
		  	w_idx+=FIFO_NUM;
		  for(l=r_idx;l < w_idx;l++)
		  {
		    k = l%FIFO_NUM;
			if(pDecGlobal->vFrameIndexFIFO.img_index[k]==i)
			{
			  find = 1;
			  break;
			}
		  }
		  if(find)
		  {
		    j++;
		  	continue;
		  }
        }
	    FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
		//LOGI("write i:%d\r\n",p_Dpb->p_remove[j]->cur_buf_index);

		for(i=j;i<p_Dpb->remove_size-1;i++)
		  p_Dpb->p_remove[i]=p_Dpb->p_remove[i+1];
		p_Dpb->remove_size--;
		
	  }	  
	  else
		j++;
	}
    if (pDecGlobal->direct_set)
    {
        //LOGI("pDecGlobal->dec_picture");
		if (structure == FRAME||pDecGlobal->p_Dpb->direct_out->type!=pDecGlobal->pCurSlice->slice_type
			||pDecGlobal->p_Dpb->direct_out->structure==structure)
		{
            i = pDecGlobal->p_Dpb->direct_out->cur_buf_index;
			FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
			pDecGlobal->direct_set=0;
			goto new_pic;
		}
      i = pDecGlobal->p_Dpb->direct_out->cur_buf_index;
    }
	else if (!p_Dpb->last_picture||structure == FRAME)
	{
new_pic:
		i = FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, 0, FIFO_READ);
		//if(i!=-1)
		//LOGI("get i:%d\r\n",i);
		if(i==-1)
		{
#if FLUSH_ERROR
			flush_dpb(pDecGlobal, p_Dpb);
			update_ref_list(p_Dpb);
			update_ltref_list(p_Dpb);
#else
			VO_U32 size = p_Dpb->size + 3*pDecGlobal->nThdNum + 1 + pDecGlobal->extra_dpb;

			for (i = 0; i < p_Dpb->ref_frames_in_buffer; i++)
				p_Dpb->fs_ref[i]=NULL;
			for (i=0; i<p_Dpb->ltref_frames_in_buffer; i++)
				p_Dpb->fs_ltref[i]=NULL;

			pDecGlobal->vFrameIndexFIFO.r_idx = pDecGlobal->vFrameIndexFIFO.w_idx = 0;

			for(i = 0; i < size; i++)
			{
				FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
			}
			p_Dpb->used_size=0;
			//LOGI("remove all\n");
			p_Dpb->remove_size=0;
			update_ref_list(p_Dpb);
			update_ltref_list(p_Dpb);
			p_Dpb->last_output_poc = INT_MIN;
#endif
			return NULL;
		}
		s = p_Dpb->storable_pic_buffer[0] + i;
		if(!pDecGlobal->pCurSlice->active_sps->frame_mbs_only_flag)
		  memset(s->ref_idx[0],-1,stride*2);
		else
		  memset(s->ref_idx[0],-1,stride*2);
		//memset(s->ref_pic[0],0,stride*4*sizeof(StorablePicture*));
		//memset(s->mv[0],0,stride*4*sizeof(VO_S32));
	}
	else
	{
	  StorablePicture *tmp = *(&(p_Dpb->last_picture->bottom_field)-(structure%3)+1);
	  if(tmp == NULL||tmp->errFlag)
	  {
#if FLUSH_ERROR
		  flush_dpb(pDecGlobal, p_Dpb);
		  update_ref_list(p_Dpb);
		  update_ltref_list(p_Dpb);
#else
		  VO_U32 size = p_Dpb->size + 3*pDecGlobal->nThdNum + 1 + pDecGlobal->extra_dpb;

		  for (i = 0; i < p_Dpb->ref_frames_in_buffer; i++)
			  p_Dpb->fs_ref[i]=NULL;
		  for (i=0; i<p_Dpb->ltref_frames_in_buffer; i++)
			  p_Dpb->fs_ltref[i]=NULL;

		  pDecGlobal->vFrameIndexFIFO.r_idx = pDecGlobal->vFrameIndexFIFO.w_idx = 0;

		  for(i = 0; i < size; i++)
		  {
			  FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
		  }
		  p_Dpb->used_size=0;
		  //printf("remove all\n");
		  p_Dpb->remove_size=0;
		  update_ref_list(p_Dpb);
		  update_ltref_list(p_Dpb);
		  p_Dpb->last_output_poc = INT_MIN;
#endif
		  return NULL;
	  }
	  i = tmp->cur_buf_index;
	}
	s = p_Dpb->storable_pic_buffer[structure%3] + i;
    
	imgY = s->imgY;
	imgUV[0] = s->imgUV[0];
	imgUV[1] = s->imgUV[1];
	refY = s->refY;
	refUV[0] = s->refUV[0];
	refUV[1] = s->refUV[1];
	if(pDecGlobal->interlace)
	{
	  ref_pic[0] = s->ref_pic[0];
	  ref_pic[1] = s->ref_pic[1];
	}
	ref_idx[0] = s->ref_idx[0];
	ref_idx[1] = s->ref_idx[1];
	type = s->mb_type_buffer;
	mv[0] = s->mv[0];
	mv[1] = s->mv[1];
	memset(s, 0, sizeof(StorablePicture));
	
	s->imgY = imgY ;
	s->imgUV[0] = imgUV[0];
	s->imgUV[1] = imgUV[1];
	s->refY = refY ;
	s->refUV[0] = refUV[0];
	s->refUV[1] = refUV[1];
	if(pDecGlobal->interlace)
	{
	  s->ref_pic[0] = ref_pic[0];
	  s->ref_pic[1] = ref_pic[1];
	}
	s->ref_idx[0] = ref_idx[0];
	s->ref_idx[1] = ref_idx[1];
	s->mv[0] = mv[0];
	s->mv[1] = mv[1];
    s->mb_type_buffer = type;
	s->nProgress[1] = s->nProgress[0] = -100;
// 	s->pic_num=0;
// 	s->frame_num=0;
// 	s->long_term_frame_idx=0;
// 	s->long_term_pic_num=0;
// 	s->used_for_reference=0;
// 	s->is_long_term=0;
// 	s->non_existing=0;
// 	s->is_output = 0;

	s->cur_buf_index = i;
#if (MVC_EXTENSION_ENABLE)
	s->view_id = -1;
#endif
	s->structure=structure;
	s->top_field    = NULL;
	s->bottom_field = NULL;
	s->frame        = NULL;
    

  return s;
}

void free_frame_store(H264DEC_G *pDecGlobal,FrameStore* f)
{
  if (f)
  {
	  VO_U32 i = (VO_U32)-1;
    if (f->frame)
    {
		i = f->frame->cur_buf_index;
      free_storable_picture(pDecGlobal,f->frame);
//       f->frame=NULL;
    }
    if (f->top_field)
    {
		if (i == (VO_U32)-1)
		{
			i = f->top_field->cur_buf_index;
		}
      free_storable_picture(pDecGlobal,f->top_field);
//       f->top_field=NULL;
    }
    if (f->bottom_field)
    {
		if (i == (VO_U32)-1)
		{
			i = f->bottom_field->cur_buf_index;
		}
      free_storable_picture(pDecGlobal,f->bottom_field);
//       f->bottom_field=NULL;
    }
	if (i != (VO_U32)-1)
	{
	  //FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
	  pDecGlobal->p_Dpb->p_remove[pDecGlobal->p_Dpb->remove_size++]=f->frame;
	  //f->frame->removable = 1;
	}
    voFree(pDecGlobal,f);
  }
}


void free_storable_picture(H264DEC_G *pDecGlobal,StorablePicture* p)
{
  //int nplane;
  if (p)
  {
	  assert(p->cur_buf_index!=-1);
  }
}

static void unmark_for_reference(FrameStore* fs)
{

  if (fs->is_used & 1)
  {
    if (fs->top_field)
    {
      fs->top_field->used_for_reference = 0;
    }
  }
  if (fs->is_used & 2)
  {
    if (fs->bottom_field)
    {
      fs->bottom_field->used_for_reference = 0;
    }
  }
  if (fs->is_used == 3)
  {
    if (fs->top_field && fs->bottom_field)
    {
      fs->top_field->used_for_reference = 0;
      fs->bottom_field->used_for_reference = 0;
    }
    fs->frame->used_for_reference = 0;
  }

  fs->is_reference = 0;

  if(fs->frame)
  {
//    free_pic_motion(&fs->frame->motion);
  }

  if (fs->top_field)
  {
//    free_pic_motion(&fs->top_field->motion);
  }

  if (fs->bottom_field)
  {
//    free_pic_motion(&fs->bottom_field->motion);
  }
}

static void unmark_for_long_term_reference(FrameStore* fs)
{

  if (fs->is_used & 1)
  {
    if (fs->top_field)
    {
      fs->top_field->used_for_reference = 0;
      fs->top_field->is_long_term = 0;
    }
  }
  if (fs->is_used & 2)
  {
    if (fs->bottom_field)
    {
      fs->bottom_field->used_for_reference = 0;
      fs->bottom_field->is_long_term = 0;
    }
  }
  if (fs->is_used == 3)
  {
    if (fs->top_field && fs->bottom_field)
    {
      fs->top_field->used_for_reference = 0;
      fs->top_field->is_long_term = 0;
      fs->bottom_field->used_for_reference = 0;
      fs->bottom_field->is_long_term = 0;
    }
    fs->frame->used_for_reference = 0;
    fs->frame->is_long_term = 0;
  }

  fs->is_reference = 0;
  fs->is_long_term = 0;
}

static inline int compare_pic_by_pic_num_desc( const void *arg1, const void *arg2 )
{
  int pic_num1 = (*(StorablePicture**)arg1)->pic_num;
  int pic_num2 = (*(StorablePicture**)arg2)->pic_num;

  if (pic_num1 < pic_num2)
    return 1;
  if (pic_num1 > pic_num2)
    return -1;
  else
    return 0;
}


static inline int compare_pic_by_lt_pic_num_asc( const void *arg1, const void *arg2 )
{
  int long_term_pic_num1 = (*(StorablePicture**)arg1)->long_term_pic_num;
  int long_term_pic_num2 = (*(StorablePicture**)arg2)->long_term_pic_num;

  if ( long_term_pic_num1 < long_term_pic_num2)
    return -1;
  if ( long_term_pic_num1 > long_term_pic_num2)
    return 1;
  else
    return 0;
}

static inline int compare_fs_by_frame_num_desc( const void *arg1, const void *arg2 )
{
  int frame_num_wrap1 = (*(FrameStore**)arg1)->frame_num_wrap;
  int frame_num_wrap2 = (*(FrameStore**)arg2)->frame_num_wrap;
  if ( frame_num_wrap1 < frame_num_wrap2)
    return 1;
  if ( frame_num_wrap1 > frame_num_wrap2)
    return -1;
  else
    return 0;
}

static inline int compare_fs_by_lt_pic_idx_asc( const void *arg1, const void *arg2 )
{
  int long_term_frame_idx1 = (*(FrameStore**)arg1)->long_term_frame_idx;
  int long_term_frame_idx2 = (*(FrameStore**)arg2)->long_term_frame_idx;

  if ( long_term_frame_idx1 < long_term_frame_idx2)
    return -1;
  if ( long_term_frame_idx1 > long_term_frame_idx2)
    return 1;
  else
    return 0;
}

static inline int compare_pic_by_poc_asc( const void *arg1, const void *arg2 )
{
  int poc1 = (*(StorablePicture**)arg1)->poc;
  int poc2 = (*(StorablePicture**)arg2)->poc;

  if ( poc1 < poc2)
    return -1;  
  if ( poc1 > poc2)
    return 1;
  else
    return 0;
}


static inline int compare_pic_by_poc_desc( const void *arg1, const void *arg2 )
{
  int poc1 = (*(StorablePicture**)arg1)->poc;
  int poc2 = (*(StorablePicture**)arg2)->poc;

  if (poc1 < poc2)
    return 1;
  if (poc1 > poc2)
    return -1;
  else
    return 0;
}

static inline int compare_fs_by_poc_asc( const void *arg1, const void *arg2 )
{
  int poc1 = (*(FrameStore**)arg1)->poc;
  int poc2 = (*(FrameStore**)arg2)->poc;

  if (poc1 < poc2)
    return -1;
  if (poc1 > poc2)
    return 1;
  else
    return 0;
}


static inline int compare_fs_by_poc_desc( const void *arg1, const void *arg2 )
{
  int poc1 = (*(FrameStore**)arg1)->poc;
  int poc2 = (*(FrameStore**)arg2)->poc;

  if (poc1 < poc2)
    return 1;
  if (poc1 > poc2)
    return -1;
  else
    return 0;
}

int is_short_ref(StorablePicture *s)
{
  return ((s->used_for_reference) && (!(s->is_long_term)));
}

int is_long_ref(StorablePicture *s)
{
  return ((s->used_for_reference) && (s->is_long_term));
}

void gen_pic_list_from_frame_list(PictureStructure currStructure, FrameStore **fs_list, int list_idx, StorablePicture **list, char *list_size, int long_term)
{
  int top_idx = 0;
  int bot_idx = 0;

  int (*is_ref)(StorablePicture *s);

  if (long_term)
    is_ref=is_long_ref;
  else
    is_ref=is_short_ref;

#if 1
  if (currStructure == TOP_FIELD)
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
            list[(short) *list_size] = fs_list[top_idx]->top_field;
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
            list[(short) *list_size] = fs_list[bot_idx]->bottom_field;
            (*list_size)++;
            bot_idx++;
            break;
          }
        }
      }
    }
  }
  if (currStructure == BOTTOM_FIELD)
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
            list[(short) *list_size] = fs_list[bot_idx]->bottom_field;
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
            list[(short) *list_size] = fs_list[top_idx]->top_field;
            (*list_size)++;
            top_idx++;
            break;
          }
        }
      }
    }
  }
#endif
}

void update_pic_num(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  unsigned int i;
  //VideoParameters *p_Vid = currSlice->p_Vid;
  DecodedPictureBuffer *p_Dpb = pDecGlobal->p_Dpb;
  seq_parameter_set_rbsp_t *active_sps = currSlice->active_sps;

  int add_top = 0, add_bottom = 0;
  int MaxFrameNum = 1 << (active_sps->log2_max_frame_num_minus4 + 4);

  if (currSlice->structure == FRAME)
  {
    for (i=0; i<p_Dpb->ref_frames_in_buffer; i++)
    {
      if (p_Dpb->fs_ref[i]->is_used==3)
      {
        if ((p_Dpb->fs_ref[i]->frame->used_for_reference)&&(!p_Dpb->fs_ref[i]->frame->is_long_term))
        {
          if( p_Dpb->fs_ref[i]->frame_num > currSlice->frame_num )
          {
            p_Dpb->fs_ref[i]->frame_num_wrap = p_Dpb->fs_ref[i]->frame_num - MaxFrameNum;
          }
          else
          {
            p_Dpb->fs_ref[i]->frame_num_wrap = p_Dpb->fs_ref[i]->frame_num;
          }
          p_Dpb->fs_ref[i]->frame->pic_num = p_Dpb->fs_ref[i]->frame_num_wrap;
        }
      }
    }
    // update long_term_pic_num
    for (i = 0; i < p_Dpb->ltref_frames_in_buffer; i++)
    {
      if (p_Dpb->fs_ltref[i]->is_used==3)
      {
        if (p_Dpb->fs_ltref[i]->frame->is_long_term)
        {
          p_Dpb->fs_ltref[i]->frame->long_term_pic_num = p_Dpb->fs_ltref[i]->frame->long_term_frame_idx;
        }
      }
    }
  }
#if 1
  else
  {
    if (currSlice->structure == TOP_FIELD)
    {
      add_top    = 1;
      add_bottom = 0;
    }
    else
    {
      add_top    = 0;
      add_bottom = 1;
    }

    for (i=0; i<p_Dpb->ref_frames_in_buffer; i++)
    {
      if (p_Dpb->fs_ref[i]->is_reference)
      {
        if( p_Dpb->fs_ref[i]->frame_num > currSlice->frame_num )
        {
          p_Dpb->fs_ref[i]->frame_num_wrap = p_Dpb->fs_ref[i]->frame_num - MaxFrameNum;
        }
        else
        {
          p_Dpb->fs_ref[i]->frame_num_wrap = p_Dpb->fs_ref[i]->frame_num;
        }
        if (p_Dpb->fs_ref[i]->is_reference & 1)
        {
          p_Dpb->fs_ref[i]->top_field->pic_num = (2 * p_Dpb->fs_ref[i]->frame_num_wrap) + add_top;
        }
        if (p_Dpb->fs_ref[i]->is_reference & 2)
        {
          p_Dpb->fs_ref[i]->bottom_field->pic_num = (2 * p_Dpb->fs_ref[i]->frame_num_wrap) + add_bottom;
        }
      }
    }
    // update long_term_pic_num
    for (i=0; i<p_Dpb->ltref_frames_in_buffer; i++)
    {
      if (p_Dpb->fs_ltref[i]->is_long_term & 1)
      {
        p_Dpb->fs_ltref[i]->top_field->long_term_pic_num = 2 * p_Dpb->fs_ltref[i]->top_field->long_term_frame_idx + add_top;
      }
      if (p_Dpb->fs_ltref[i]->is_long_term & 2)
      {
        p_Dpb->fs_ltref[i]->bottom_field->long_term_pic_num = 2 * p_Dpb->fs_ltref[i]->bottom_field->long_term_frame_idx + add_bottom;
      }
    }
  }
#endif
}


void init_lists_i_slice(H264DEC_G *pDecGlobal,Slice *currSlice)
{

  currSlice->listXsize[currSlice->slice_number][0] = 0;
  currSlice->listXsize[currSlice->slice_number][1] = 0;
}

void init_lists_p_slice(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  //VideoParameters *p_Vid = currSlice->p_Vid;
  DecodedPictureBuffer *p_Dpb = pDecGlobal->p_Dpb;

  unsigned int i;

  int list0idx = 0;
  int listltidx = 0;

  FrameStore **fs_list0;
  FrameStore **fs_listlt;
  VO_S32 slice_number = currSlice->slice_number;
  
  if (currSlice->structure == FRAME)
  {
    for (i=0; i<currSlice->ref_frames_in_buffer; i++)
    {
      if (currSlice->fs_ref[i].is_used==3)
      {
        //if ((currSlice->fs_ref[i].frame->used_for_reference)&&(!currSlice->fs_ref[i].frame->is_long_term))
        {
          currSlice->listX[slice_number][0][list0idx++] = currSlice->fs_ref[i].frame;
        }
      }
    }
    // order list 0 by PicNum
    qsort((void *)currSlice->listX[slice_number][0], list0idx, sizeof(StorablePicture*), compare_pic_by_pic_num_desc);
    currSlice->listXsize[slice_number][0] = (char) list0idx;
    //printf("listX[0] (PicNum): "); for (i=0; i<list0idx; i++){printf ("%d  ", currSlice->listX[0][i]->pic_num);} printf("\n");

    // long term handling
    for (i=0; i<currSlice->ltref_frames_in_buffer; i++)
    {
      if (currSlice->fs_ltref[i].is_used==3)
      {
        //if (currSlice->fs_ltref[i].frame->is_long_term)
        {
          currSlice->listX[slice_number][0][list0idx++]=currSlice->fs_ltref[i].frame;
        }
      }
    }
    qsort((void *)&currSlice->listX[slice_number][0][(short) currSlice->listXsize[slice_number][0]], list0idx - currSlice->listXsize[slice_number][0], sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
    currSlice->listXsize[slice_number][0] = (char) list0idx;
  }
  else
  {
    fs_list0 = voMalloc(pDecGlobal,p_Dpb->size*sizeof (FrameStore*));
    if (NULL==fs_list0)
      no_mem_exit("init_lists: fs_list0");
    fs_listlt = voMalloc(pDecGlobal,p_Dpb->size*sizeof (FrameStore*));
    if (NULL==fs_listlt)
      no_mem_exit("init_lists: fs_listlt");

    for (i=0; i<currSlice->ref_frames_in_buffer; i++)
    {
      if (currSlice->fs_ref[i].is_reference)
      {
        fs_list0[list0idx++] = &currSlice->fs_ref[i];
      }
    }

    qsort((void *)fs_list0, list0idx, sizeof(FrameStore*), compare_fs_by_frame_num_desc);

    //printf("fs_list0 (FrameNum): "); for (i=0; i<list0idx; i++){printf ("%d  ", fs_list0[i]->frame_num_wrap);} printf("\n");

    currSlice->listXsize[slice_number][0] = 0;
    gen_pic_list_from_frame_list(currSlice->structure, fs_list0, list0idx, currSlice->listX[slice_number][0], &currSlice->listXsize[slice_number][0], 0);

    //printf("listX[0] (PicNum): "); for (i=0; i < currSlice->listXsize[0]; i++){printf ("%d  ", currSlice->listX[0][i]->pic_num);} printf("\n");

    // long term handling
    for (i=0; i<currSlice->ltref_frames_in_buffer; i++)
    {
        fs_listlt[listltidx++]=&currSlice->fs_ltref[i];
    }

    qsort((void *)fs_listlt, listltidx, sizeof(FrameStore*), compare_fs_by_lt_pic_idx_asc);

    gen_pic_list_from_frame_list(currSlice->structure, fs_listlt, listltidx, currSlice->listX[slice_number][0], &currSlice->listXsize[slice_number][0], 1);

    voFree(pDecGlobal,fs_list0);
    voFree(pDecGlobal,fs_listlt);
  }
  currSlice->listXsize[slice_number][1] = 0;    

  // set max size
  currSlice->listXsize[slice_number][0] = (char) imin (currSlice->listXsize[slice_number][0], currSlice->num_ref_idx_active[LIST_0]);
  currSlice->listXsize[slice_number][1] = (char) imin (currSlice->listXsize[slice_number][1], currSlice->num_ref_idx_active[LIST_1]);

  // set the unused list entries to NULL
  for (i=currSlice->listXsize[slice_number][0]; i< (MAX_LIST_SIZE) ; i++)
  {
    currSlice->listX[slice_number][0][i] = NULL;
  }
  for (i=currSlice->listXsize[slice_number][1]; i< (MAX_LIST_SIZE) ; i++)
  {
    currSlice->listX[slice_number][1][i] = NULL;
  }
}

void init_lists_b_slice(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  //VideoParameters *p_Vid = currSlice->p_Vid;
  DecodedPictureBuffer *p_Dpb = pDecGlobal->p_Dpb;

  unsigned int i;
  int j;

  int list0idx = 0;
  int list0idx_1 = 0;
  int listltidx = 0;

  FrameStore **fs_list0;
  FrameStore **fs_list1;
  FrameStore **fs_listlt;
  VO_S32 slice_number = currSlice->slice_number;
  {
    // B-Slice
    if (currSlice->structure == FRAME)
    {
      for (i=0; i<currSlice->ref_frames_in_buffer; i++)
      {
        if (currSlice->fs_ref[i].is_used==3)
        {
          //if ((currSlice->fs_ref[i].frame->used_for_reference)&&(!currSlice->fs_ref[i].frame->is_long_term))
          {
            if (currSlice->framepoc >= currSlice->fs_ref[i].frame->poc) //!KS use >= for error concealment
              //            if (p_Vid->framepoc > p_Dpb->fs_ref[i]->frame->poc)
            {
              currSlice->listX[slice_number][0][list0idx++] = currSlice->fs_ref[i].frame;
            }
          }
        }
      }
      qsort((void *)currSlice->listX[slice_number][0], list0idx, sizeof(StorablePicture*), compare_pic_by_poc_desc);
      list0idx_1 = list0idx;
      for (i=0; i<currSlice->ref_frames_in_buffer; i++)
      {
        if (currSlice->fs_ref[i].is_used==3)
        {
          //if ((currSlice->fs_ref[i].frame->used_for_reference)&&(!currSlice->fs_ref[i].frame->is_long_term))
          {
            if (currSlice->framepoc < currSlice->fs_ref[i].frame->poc)
            {
              currSlice->listX[slice_number][0][list0idx++] = currSlice->fs_ref[i].frame;
            }
          }
        }
      }
      qsort((void *)&currSlice->listX[slice_number][0][list0idx_1], list0idx-list0idx_1, sizeof(StorablePicture*), compare_pic_by_poc_asc);

      for (j=0; j<list0idx_1; j++)
      {
        currSlice->listX[slice_number][1][list0idx-list0idx_1+j]=currSlice->listX[slice_number][0][j];
      }
      for (j=list0idx_1; j<list0idx; j++)
      {
        currSlice->listX[slice_number][1][j-list0idx_1]=currSlice->listX[slice_number][0][j];
      }

      currSlice->listXsize[slice_number][0] = currSlice->listXsize[slice_number][1] = (char) list0idx;

      for (i=0; i<currSlice->ltref_frames_in_buffer; i++)
      {
        if (currSlice->fs_ltref[i].is_used==3)
        {
          //if (currSlice->fs_ltref[i].frame->is_long_term)
          {
            currSlice->listX[slice_number][0][list0idx]   = currSlice->fs_ltref[i].frame;
            currSlice->listX[slice_number][1][list0idx++] = currSlice->fs_ltref[i].frame;
          }
        }
      }
      qsort((void *)&currSlice->listX[slice_number][0][(short) currSlice->listXsize[slice_number][0]], list0idx - currSlice->listXsize[slice_number][0], sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
      qsort((void *)&currSlice->listX[slice_number][1][(short) currSlice->listXsize[slice_number][0]], list0idx - currSlice->listXsize[slice_number][0], sizeof(StorablePicture*), compare_pic_by_lt_pic_num_asc);
      currSlice->listXsize[slice_number][0] = currSlice->listXsize[slice_number][1] = (char) list0idx;
    }
    else
    {
      fs_list0 = voMalloc(pDecGlobal,p_Dpb->size*sizeof (FrameStore*));
      if (NULL==fs_list0)
        no_mem_exit("init_lists: fs_list0");
      fs_list1 = voMalloc(pDecGlobal,p_Dpb->size*sizeof (FrameStore*));
      if (NULL==fs_list1)
        no_mem_exit("init_lists: fs_list1");
      fs_listlt = voMalloc(pDecGlobal,p_Dpb->size*sizeof (FrameStore*));
      if (NULL==fs_listlt)
        no_mem_exit("init_lists: fs_listlt");

      currSlice->listXsize[slice_number][0] = 0;
      currSlice->listXsize[slice_number][1] = 1;

      for (i=0; i<currSlice->ref_frames_in_buffer; i++)
      {
        if (currSlice->fs_ref[i].is_used)
        {
          if (currSlice->ThisPOC >= currSlice->fs_ref[i].poc)
          {
            fs_list0[list0idx++] = &currSlice->fs_ref[i];
          }
        }
      }
      qsort((void *)fs_list0, list0idx, sizeof(FrameStore*), compare_fs_by_poc_desc);
      list0idx_1 = list0idx;
      for (i=0; i<currSlice->ref_frames_in_buffer; i++)
      {
        if (currSlice->fs_ref[i].is_used)
        {
          if (currSlice->ThisPOC < currSlice->fs_ref[i].poc)
          {
            fs_list0[list0idx++] = &currSlice->fs_ref[i];
          }
        }
      }
      qsort((void *)&fs_list0[list0idx_1], list0idx-list0idx_1, sizeof(FrameStore*), compare_fs_by_poc_asc);

      for (j=0; j<list0idx_1; j++)
      {
        fs_list1[list0idx-list0idx_1+j]=fs_list0[j];
      }
      for (j=list0idx_1; j<list0idx; j++)
      {
        fs_list1[j-list0idx_1]=fs_list0[j];
      }

      //      printf("fs_list0 currPoc=%d (Poc): ", currSlice->ThisPOC); for (i=0; i<list0idx; i++){printf ("%d  ", fs_list0[i]->poc);} printf("\n");
      //      printf("fs_list1 currPoc=%d (Poc): ", currSlice->ThisPOC); for (i=0; i<list0idx; i++){printf ("%d  ", fs_list1[i]->poc);} printf("\n");

      currSlice->listXsize[slice_number][0] = 0;
      currSlice->listXsize[slice_number][1] = 0;
      gen_pic_list_from_frame_list(currSlice->structure, fs_list0, list0idx, currSlice->listX[slice_number][0], &currSlice->listXsize[slice_number][0], 0);
      gen_pic_list_from_frame_list(currSlice->structure, fs_list1, list0idx, currSlice->listX[slice_number][1], &currSlice->listXsize[slice_number][1], 0);

      //      printf("currSlice->listX[0] currPoc=%d (Poc): ", p_Vid->framepoc); for (i=0; i<currSlice->listXsize[0]; i++){printf ("%d  ", currSlice->listX[0][i]->poc);} printf("\n");
      //      printf("currSlice->listX[1] currPoc=%d (Poc): ", p_Vid->framepoc); for (i=0; i<currSlice->listXsize[1]; i++){printf ("%d  ", currSlice->listX[1][i]->poc);} printf("\n");

      // long term handling
      for (i=0; i<currSlice->ltref_frames_in_buffer; i++)
      {
          fs_listlt[listltidx++]=&currSlice->fs_ltref[i];
      }

      qsort((void *)fs_listlt, listltidx, sizeof(FrameStore*), compare_fs_by_lt_pic_idx_asc);

      gen_pic_list_from_frame_list(currSlice->structure, fs_listlt, listltidx, currSlice->listX[slice_number][0], &currSlice->listXsize[slice_number][0], 1);
      gen_pic_list_from_frame_list(currSlice->structure, fs_listlt, listltidx, currSlice->listX[slice_number][1], &currSlice->listXsize[slice_number][1], 1);

      voFree(pDecGlobal,fs_list0);
      voFree(pDecGlobal,fs_list1);
      voFree(pDecGlobal,fs_listlt);
    }
  }

  if ((currSlice->listXsize[slice_number][0] == currSlice->listXsize[slice_number][1]) && (currSlice->listXsize[slice_number][0] > 1))
  {
    // check if lists are identical, if yes swap first two elements of currSlice->listX[1]
    int diff=0;
    for (j = 0; j< currSlice->listXsize[slice_number][0]; j++)
    {
      if (currSlice->listX[slice_number][0][j] != currSlice->listX[slice_number][1][j])
      {
        diff = 1;
        break;
      }
    }
    if (!diff)
    {
      StorablePicture *tmp_s = currSlice->listX[slice_number][1][0];
      currSlice->listX[slice_number][1][0]=currSlice->listX[slice_number][1][1];
      currSlice->listX[slice_number][1][1]=tmp_s;
    }
  }

  // set max size
  currSlice->listXsize[slice_number][0] = (char) imin (currSlice->listXsize[slice_number][0], currSlice->num_ref_idx_active[LIST_0]);
  currSlice->listXsize[slice_number][1] = (char) imin (currSlice->listXsize[slice_number][1], currSlice->num_ref_idx_active[LIST_1]);

  // set the unused list entries to NULL
  for (i=currSlice->listXsize[slice_number][0]; i< (MAX_LIST_SIZE) ; i++)
  {
    currSlice->listX[slice_number][0][i] = NULL;
  }
  for (i=currSlice->listXsize[slice_number][1]; i< (MAX_LIST_SIZE) ; i++)
  {
    currSlice->listX[slice_number][1][i] = NULL;
  }
}

VO_S32 init_mbaff_lists(H264DEC_G *pDecGlobal, Slice *currSlice)
{
  unsigned j;
  int i;
  VO_S32 slice_number = currSlice->slice_number;
  pDecGlobal->field_offset = 0;
  
  if (currSlice->structure==FRAME)
  {
    for (i=2;i<6;i++)
    {
      for (j=0; j<MAX_LIST_SIZE; j++)
      {
        currSlice->listX[slice_number][i][j] = NULL;
      }
      currSlice->listXsize[slice_number][i]=0;
    }
  
    for (i=0; i<currSlice->listXsize[slice_number][0]; i++)
    {
  	if(currSlice->listX[slice_number][0][i]==NULL)
  	  VOH264ERROR(VO_H264_ERR_INIT_LIST);
      currSlice->listX[slice_number][2][2*i  ] = currSlice->listX[slice_number][0][i]->top_field;
      currSlice->listX[slice_number][2][2*i+1] = currSlice->listX[slice_number][0][i]->bottom_field;
      currSlice->listX[slice_number][4][2*i  ] = currSlice->listX[slice_number][0][i]->bottom_field;
      currSlice->listX[slice_number][4][2*i+1] = currSlice->listX[slice_number][0][i]->top_field;
    }
    currSlice->listXsize[slice_number][2]=currSlice->listXsize[slice_number][4]=currSlice->listXsize[slice_number][0] * 2;
  
    for (i=0; i<currSlice->listXsize[slice_number][1]; i++)
    {
      if(currSlice->listX[slice_number][1][i]==NULL)
  	  VOH264ERROR(VO_H264_ERR_INIT_LIST);
      currSlice->listX[slice_number][3][2*i  ] = currSlice->listX[slice_number][1][i]->top_field;
      currSlice->listX[slice_number][3][2*i+1] = currSlice->listX[slice_number][1][i]->bottom_field;
      currSlice->listX[slice_number][5][2*i  ] = currSlice->listX[slice_number][1][i]->bottom_field;
      currSlice->listX[slice_number][5][2*i+1] = currSlice->listX[slice_number][1][i]->top_field;
    } 
    currSlice->listXsize[slice_number][3]=currSlice->listXsize[slice_number][5]=currSlice->listXsize[slice_number][1] * 2;
	if(currSlice->listXsize[slice_number][1]&&currSlice->listX[slice_number][1][0]->top_field&&currSlice->listX[slice_number][1][0]->bottom_field)
	  pDecGlobal->mbaff_dis = (iabs(currSlice->listX[slice_number][1][0]->top_field->poc - currSlice->ThisPOC) >= iabs(currSlice->listX[slice_number][1][0]->bottom_field->poc - currSlice->ThisPOC));
  }
  else
  {
    if(currSlice->listXsize[slice_number][1]&&!(pDecGlobal->dec_picture->structure & currSlice->listX[slice_number][1][0]->structure)
		&& currSlice->listX[slice_number][1][0]->iCodingType!=FRAME_MB_PAIR_CODING)
	{ 
	  pDecGlobal->field_offset = pDecGlobal->PicWidthInMbs*(2*(currSlice->listX[slice_number][1][0]->structure) - 3);
    }
  }
  return 0;
}

StorablePicture*  get_short_term_pic(H264DEC_G *pDecGlobal,Slice *p_Dpb, int picNum)
{
   //VideoParameters *p_Vid = p_Dpb->p_Vid;
  unsigned i;

  for (i = 0; i < p_Dpb->ref_frames_in_buffer; i++)
  {
    if (pDecGlobal->structure == FRAME)
    {
      if (p_Dpb->fs_ref[i].is_reference == 3)
        if ((!p_Dpb->fs_ref[i].frame->is_long_term)&&(p_Dpb->fs_ref[i].frame->pic_num == picNum))
          return p_Dpb->fs_ref[i].frame;
    }
    else
    {
      if (p_Dpb->fs_ref[i].is_reference & 1)
        if ((!p_Dpb->fs_ref[i].top_field->is_long_term)&&(p_Dpb->fs_ref[i].top_field->pic_num == picNum))
          return p_Dpb->fs_ref[i].top_field;
      if (p_Dpb->fs_ref[i].is_reference & 2)
        if ((!p_Dpb->fs_ref[i].bottom_field->is_long_term)&&(p_Dpb->fs_ref[i].bottom_field->pic_num == picNum))
          return p_Dpb->fs_ref[i].bottom_field;
    }
  }

  return NULL;
}

StorablePicture*  get_long_term_pic(H264DEC_G *pDecGlobal,Slice *p_Dpb, int LongtermPicNum)
{
  //VideoParameters *p_Vid = p_Dpb->p_Vid;
  unsigned i;

  for (i=0; i<p_Dpb->ltref_frames_in_buffer; i++)
  {
    if (pDecGlobal->structure==FRAME)
    {
      if (p_Dpb->fs_ltref[i].is_reference == 3)
        if ((p_Dpb->fs_ltref[i].frame->is_long_term)&&(p_Dpb->fs_ltref[i].frame->long_term_pic_num == LongtermPicNum))
          return p_Dpb->fs_ltref[i].frame;
    }
    else
    {
      if (p_Dpb->fs_ltref[i].is_reference & 1)
        if ((p_Dpb->fs_ltref[i].top_field->is_long_term)&&(p_Dpb->fs_ltref[i].top_field->long_term_pic_num == LongtermPicNum))
          return p_Dpb->fs_ltref[i].top_field;
      if (p_Dpb->fs_ltref[i].is_reference & 2)
        if ((p_Dpb->fs_ltref[i].bottom_field->is_long_term)&&(p_Dpb->fs_ltref[i].bottom_field->long_term_pic_num == LongtermPicNum))
          return p_Dpb->fs_ltref[i].bottom_field;
    }
  }
  return NULL;
}


#if (!MVC_EXTENSION_ENABLE)
static VO_S32 reorder_short_term(H264DEC_G *pDecGlobal,Slice *currSlice, int cur_list, int num_ref_idx_lX_active_minus1, int picNumLX, int *refIdxLX)
{
  StorablePicture **RefPicListX = currSlice->listX[currSlice->slice_number][cur_list]; 
  int cIdx, nIdx;

  StorablePicture *picLX;

  picLX = get_short_term_pic(pDecGlobal,currSlice, picNumLX);
  if (picLX==NULL)
  {
    VOH264ERROR(VO_H264_ERR_REORDER_LIST);
  }
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
  return 0;
}

static VO_S32 reorder_long_term(H264DEC_G *pDecGlobal,Slice *currSlice, StorablePicture **RefPicListX, int num_ref_idx_lX_active_minus1, int LongTermPicNum, int *refIdxLX)
{
  int cIdx, nIdx;

  StorablePicture *picLX;

  picLX = get_long_term_pic(pDecGlobal,currSlice, LongTermPicNum);
  if (picLX==NULL)
  {
	  VOH264ERROR(VO_H264_ERR_REORDER_LIST);
  }
  for( cIdx = num_ref_idx_lX_active_minus1+1; cIdx > *refIdxLX; cIdx-- )
    RefPicListX[ cIdx ] = RefPicListX[ cIdx - 1];

  RefPicListX[ (*refIdxLX)++ ] = picLX;

  nIdx = *refIdxLX;

  for( cIdx = *refIdxLX; cIdx <= num_ref_idx_lX_active_minus1+1; cIdx++ )
  {
    if (RefPicListX[ cIdx ])
    {
      if( (!RefPicListX[ cIdx ]->is_long_term ) ||  (RefPicListX[ cIdx ]->long_term_pic_num != LongTermPicNum ))
        RefPicListX[ nIdx++ ] = RefPicListX[ cIdx ];
    }
  }
  return 0;
}
#endif

VO_S32 reorder_ref_pic_list(H264DEC_G *pDecGlobal,Slice *currSlice, int cur_list)
{
  int *reordering_of_pic_nums_idc = currSlice->reordering_of_pic_nums_idc[cur_list];
  int *abs_diff_pic_num_minus1 = currSlice->abs_diff_pic_num_minus1[cur_list];
  int *long_term_pic_idx = currSlice->long_term_pic_idx[cur_list];
  int num_ref_idx_lX_active_minus1 = currSlice->num_ref_idx_active[cur_list] - 1;
  VO_S32 ret;
  //VideoParameters *p_Vid = currSlice->p_Vid;
  int i;

  int maxPicNum, currPicNum, picNumLXNoWrap, picNumLXPred, picNumLX;
  int refIdxLX = 0;

  if (pDecGlobal->structure==FRAME)
  {
    maxPicNum  = pDecGlobal->MaxFrameNum;
    currPicNum = currSlice->frame_num;
  }
  else
  {
    maxPicNum  = 2 * pDecGlobal->MaxFrameNum;
    currPicNum = 2 * currSlice->frame_num + 1;
  }

  picNumLXPred = currPicNum;

  for (i=0; reordering_of_pic_nums_idc[i]!=3; i++)
  {
    if (reordering_of_pic_nums_idc[i]>3)
    {
      VOH264ERROR(VO_H264_ERR_REORDER_LIST);
    }

    if (reordering_of_pic_nums_idc[i] < 2)
    {
      if (reordering_of_pic_nums_idc[i] == 0)
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

#if (MVC_EXTENSION_ENABLE)
      ret = reorder_short_term(pDecGlobal,currSlice, cur_list, num_ref_idx_lX_active_minus1, picNumLX, &refIdxLX, -1);
#else
      ret = reorder_short_term(pDecGlobal,currSlice, cur_list, num_ref_idx_lX_active_minus1, picNumLX, &refIdxLX);
#endif
    if (ret)
    {
		return ret;
    }
    }
    else //(remapping_of_pic_nums_idc[i] == 2)
    {
#if (MVC_EXTENSION_ENABLE)
      ret = reorder_long_term(pDecGlobal,currSlice, currSlice->listX[currSlice->slice_number][cur_list], num_ref_idx_lX_active_minus1, long_term_pic_idx[i], &refIdxLX, -1);
#else
      ret = reorder_long_term(pDecGlobal,currSlice, currSlice->listX[currSlice->slice_number][cur_list], num_ref_idx_lX_active_minus1, long_term_pic_idx[i], &refIdxLX);
#endif
	  if (ret)
	  {
		  return ret;
	  }
    }

  }
  // that's a definition
  currSlice->listXsize[currSlice->slice_number][cur_list] = (char) (num_ref_idx_lX_active_minus1 + 1);
  return 0;
}

static void remove_frame_from_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb, int pos)
{
  FrameStore* fs = p_Dpb->fs[pos];
  FrameStore* tmp;
  VO_U32 i;

//  printf ("remove frame with frame_num #%d\n", fs->frame_num);
  switch (fs->is_used)
  {
  case 3:
  	if((fs->frame == fs->top_field&&fs->frame == fs->bottom_field))
	{
		i = fs->frame->cur_buf_index;
      free_storable_picture(pDecGlobal,fs->frame);	
	}
    else if(fs->frame == fs->top_field)
    {
		i = fs->frame->cur_buf_index;
		if (i == (VO_U32)-1)
		{
			i = fs->bottom_field->cur_buf_index;
		}
      free_storable_picture(pDecGlobal,fs->frame);
	  free_storable_picture(pDecGlobal,fs->bottom_field);
    }
	else
	{
		i = fs->frame->cur_buf_index;
		if (i == (VO_U32)-1)
		{
			i = fs->top_field->cur_buf_index;
		}
		if (i == (VO_U32)-1)
		{
			i = fs->bottom_field->cur_buf_index;
		}
	  free_storable_picture(pDecGlobal,fs->frame);
      free_storable_picture(pDecGlobal,fs->top_field);
	  free_storable_picture(pDecGlobal,fs->bottom_field);
	}
	//FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
    p_Dpb->p_remove[p_Dpb->remove_size++]=fs->frame;
	//pDecGlobal->last_output = fs->frame;
	//fs->frame->removable = 1;
    fs->frame=NULL;
    fs->top_field=NULL;
    fs->bottom_field=NULL;
    break;
  case 2:
	  i = fs->bottom_field->cur_buf_index;
    free_storable_picture(pDecGlobal,fs->bottom_field);
	//FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
	p_Dpb->p_remove[p_Dpb->remove_size++]=fs->bottom_field;
	fs->bottom_field->bFinishOutput=1;
	fs->bottom_field->removable = 1;
    fs->bottom_field=NULL;
    break;
  case 1:
	  i = fs->top_field->cur_buf_index;
    free_storable_picture(pDecGlobal,fs->top_field);
	//FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
	p_Dpb->p_remove[p_Dpb->remove_size++]=fs->top_field;
	fs->top_field->bFinishOutput=1;
	fs->top_field->removable = 1;
    fs->top_field=NULL;
    break;
  case 0:
    break;
  default:
    //error("invalid frame store type",500);
    break;//warning_dtt
  }
  fs->is_used = 0;
  fs->is_long_term = 0;
  fs->is_reference = 0;
  fs->is_orig_reference = 0;

  // move empty framestore to end of buffer
  tmp = p_Dpb->fs[pos];

  for (i=pos; i<p_Dpb->used_size-1;i++)
  {
    p_Dpb->fs[i] = p_Dpb->fs[i+1];
  }
  p_Dpb->fs[p_Dpb->used_size-1] = tmp;
  p_Dpb->used_size--;
}

static int remove_unused_frame_from_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb)
{
  unsigned i;

  // check for frames that were already output and no longer used for reference
  for (i = 0; i < p_Dpb->used_size; i++)
  {
    if (p_Dpb->fs[i]->is_output && (!is_used_for_reference(p_Dpb->fs[i])))
    {
      remove_frame_from_dpb(pDecGlobal,p_Dpb, i);
      return 1;
    }
  }
  return 0;
}

static void idr_memory_management(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb, StorablePicture* p)
{
  unsigned i,j=0;
#if (MVC_EXTENSION_ENABLE)
  //VideoParameters *p_Vid = p_Dpb->p_Vid;
  int size = 0;
	int iVOIdx = GetVOIdx(pDecGlobal, p->view_id);
  int svc_extension_flag = pDecGlobal->pCurSlice->svc_extension_flag;
#endif

  //assert (p->idr_flag);

  if (pDecGlobal->no_output_of_prior_pics_flag)
  {
    for (i = 0; i < p_Dpb->ref_frames_in_buffer; i++)
    {
#if (MVC_EXTENSION_ENABLE)
      if (svc_extension_flag == 0 || p_Dpb->fs_ref[i]->view_id == p->view_id)
#endif
      p_Dpb->fs_ref[i]=NULL;
    }
    for (i=0; i<p_Dpb->ltref_frames_in_buffer; i++)
    {
#if (MVC_EXTENSION_ENABLE)
      if (svc_extension_flag == 0 || p_Dpb->fs_ltref[i]->view_id == p->view_id)
#endif
      p_Dpb->fs_ltref[i]=NULL;
    }
#if (MVC_EXTENSION_ENABLE)
    p_Dpb->used_size -= size;
#else
	for (i = 0; i < p_Dpb->used_size; i++)
	{
	  remove_frame_from_dpb(pDecGlobal,p_Dpb, i);
	}
	for (i = 0; i < p_Dpb->remove_size; i++)
	{
	  p_Dpb->p_remove[i]->bFinishOutput = 1;
	}
    p_Dpb->used_size=0;
#endif
  }
  else
  {
#if (MVC_EXTENSION_ENABLE)
    
    if(pDecGlobal->profile_idc == MVC_HIGH || pDecGlobal->profile_idc == STEREO_HIGH) //if (svc_extension_flag == 0)
      flush_dpb(pDecGlobal,p_Dpb, -1);
    else
      flush_dpb(pDecGlobal,p_Dpb, p->view_id);
#else
    flush_dpb(pDecGlobal,p_Dpb);
#endif
  }
  p_Dpb->last_picture = NULL;

#if (MVC_EXTENSION_ENABLE)
  update_ref_list(p_Dpb, p->view_id);
	update_ltref_list(p_Dpb, p->view_id);
	p_Dpb->last_output_poc = INT_MIN;
	p_Dpb->last_output_view_id = -1;

	if (pDecGlobal->long_term_reference_flag)
	{
		p_Dpb->max_long_term_pic_idx[iVOIdx] = 0;
		p->is_long_term           = 1;
		p->long_term_frame_idx    = 0;
	}
	else
	{
		p_Dpb->max_long_term_pic_idx[iVOIdx] = -1;
		p->is_long_term           = 0;
	}
#else
  update_ref_list(p_Dpb);
  update_ltref_list(p_Dpb);
  p_Dpb->last_output_poc = INT_MIN;

  if (pDecGlobal->long_term_reference_flag)
  {
    p_Dpb->max_long_term_pic_idx = 0;
    p->is_long_term           = 1;
    p->long_term_frame_idx    = 0;
  }
  else
  {
    p_Dpb->max_long_term_pic_idx = -1;
    p->is_long_term           = 0;
  }
#endif
}

static void sliding_window_memory_management(DecodedPictureBuffer *p_Dpb, StorablePicture* p)
{
  unsigned i;
  if (p_Dpb->ref_frames_in_buffer==p_Dpb->num_ref_frames - p_Dpb->ltref_frames_in_buffer)
  {
    for (i=0; i<p_Dpb->used_size;i++)
    {
      if (p_Dpb->fs[i]->is_reference && (!(p_Dpb->fs[i]->is_long_term)))
      {
        unmark_for_reference(p_Dpb->fs[i]);
        update_ref_list(p_Dpb);
        break;
      }
    }
  }

  p->is_long_term = 0;
}

static int get_pic_num_x (StorablePicture *p, int difference_of_pic_nums_minus1)
{
  int currPicNum;

  if (p->structure == FRAME)
    currPicNum = p->frame_num;
  else
    currPicNum = 2 * p->frame_num + 1;

  return currPicNum - (difference_of_pic_nums_minus1 + 1);
}

static void mm_unmark_short_term_for_reference(DecodedPictureBuffer *p_Dpb, StorablePicture *p, int difference_of_pic_nums_minus1)
{
  int picNumX;

  unsigned i;

  picNumX = get_pic_num_x(p, difference_of_pic_nums_minus1);

  for (i=0; i<p_Dpb->ref_frames_in_buffer; i++)
  {
    if (p->structure == FRAME)
    {
      if ((p_Dpb->fs_ref[i]->is_reference==3) && (p_Dpb->fs_ref[i]->is_long_term==0))
      {
        if (p_Dpb->fs_ref[i]->frame->pic_num == picNumX)
        {
          unmark_for_reference(p_Dpb->fs_ref[i]);
          return;
        }
      }
    }
    else
    {
      if ((p_Dpb->fs_ref[i]->is_reference & 1) && (!(p_Dpb->fs_ref[i]->is_long_term & 1)))
      {
        if (p_Dpb->fs_ref[i]->top_field->pic_num == picNumX)
        {
          p_Dpb->fs_ref[i]->top_field->used_for_reference = 0;
          p_Dpb->fs_ref[i]->is_reference &= 2;
          if (p_Dpb->fs_ref[i]->is_used == 3)
          {
            p_Dpb->fs_ref[i]->frame->used_for_reference = 0;
          }
          return;
        }
      }
      if ((p_Dpb->fs_ref[i]->is_reference & 2) && (!(p_Dpb->fs_ref[i]->is_long_term & 2)))
      {
        if (p_Dpb->fs_ref[i]->bottom_field->pic_num == picNumX)
        {
          p_Dpb->fs_ref[i]->bottom_field->used_for_reference = 0;
          p_Dpb->fs_ref[i]->is_reference &= 1;
          if (p_Dpb->fs_ref[i]->is_used == 3)
          {
            p_Dpb->fs_ref[i]->frame->used_for_reference = 0;
          }
          return;
        }
      }
    }
  }
}

static void mm_unmark_long_term_for_reference(DecodedPictureBuffer *p_Dpb, StorablePicture *p, int long_term_pic_num)
{
  unsigned i;
  for (i=0; i<p_Dpb->ltref_frames_in_buffer; i++)
  {
    if (p->structure == FRAME)
    {
      if ((p_Dpb->fs_ltref[i]->is_reference==3) && (p_Dpb->fs_ltref[i]->is_long_term==3))
      {
        if (p_Dpb->fs_ltref[i]->frame->long_term_pic_num == long_term_pic_num)
        {
          unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
        }
      }
    }
    else
    {
      if ((p_Dpb->fs_ltref[i]->is_reference & 1) && ((p_Dpb->fs_ltref[i]->is_long_term & 1)))
      {
        if (p_Dpb->fs_ltref[i]->top_field->long_term_pic_num == long_term_pic_num)
        {
          p_Dpb->fs_ltref[i]->top_field->used_for_reference = 0;
          p_Dpb->fs_ltref[i]->top_field->is_long_term = 0;
          p_Dpb->fs_ltref[i]->is_reference &= 2;
          p_Dpb->fs_ltref[i]->is_long_term &= 2;
          if (p_Dpb->fs_ltref[i]->is_used == 3)
          {
            p_Dpb->fs_ltref[i]->frame->used_for_reference = 0;
            p_Dpb->fs_ltref[i]->frame->is_long_term = 0;
          }
          return;
        }
      }
      if ((p_Dpb->fs_ltref[i]->is_reference & 2) && ((p_Dpb->fs_ltref[i]->is_long_term & 2)))
      {
        if (p_Dpb->fs_ltref[i]->bottom_field->long_term_pic_num == long_term_pic_num)
        {
          p_Dpb->fs_ltref[i]->bottom_field->used_for_reference = 0;
          p_Dpb->fs_ltref[i]->bottom_field->is_long_term = 0;
          p_Dpb->fs_ltref[i]->is_reference &= 1;
          p_Dpb->fs_ltref[i]->is_long_term &= 1;
          if (p_Dpb->fs_ltref[i]->is_used == 3)
          {
            p_Dpb->fs_ltref[i]->frame->used_for_reference = 0;
            p_Dpb->fs_ltref[i]->frame->is_long_term = 0;
          }
          return;
        }
      }
    }
  }
}


static void unmark_long_term_frame_for_reference_by_frame_idx(DecodedPictureBuffer *p_Dpb, int long_term_frame_idx)
{
  unsigned i;
  for(i=0; i<p_Dpb->ltref_frames_in_buffer; i++)
  {
    if (p_Dpb->fs_ltref[i]->long_term_frame_idx == long_term_frame_idx)
      unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
  }
}


static void unmark_long_term_field_for_reference_by_frame_idx(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb, PictureStructure structure, int long_term_frame_idx, int mark_current, unsigned curr_frame_num, int curr_pic_num)
{
  //VideoParameters *p_Vid = p_Dpb->p_Vid;
  unsigned i;

  assert(structure!=FRAME);
  if (curr_pic_num<0)
    curr_pic_num += (2 * pDecGlobal->MaxFrameNum);

  for(i=0; i<p_Dpb->ltref_frames_in_buffer; i++)
  {
#if (MVC_EXTENSION_ENABLE)
    if (p_Dpb->fs_ltref[i]->view_id == curr_view_id)
    {
#endif
    if (p_Dpb->fs_ltref[i]->long_term_frame_idx == long_term_frame_idx)
    {
#if 0
      if (structure == TOP_FIELD)
      {
        if ((p_Dpb->fs_ltref[i]->is_long_term == 3))
        {
          unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
        }
        else
        {
          if ((p_Dpb->fs_ltref[i]->is_long_term == 1))
          {
            unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
          }
          else
          {
            if (mark_current)
            {
              if (p_Dpb->last_picture)
              {
                if ( ( p_Dpb->last_picture != p_Dpb->fs_ltref[i] )|| p_Dpb->last_picture->frame_num != curr_frame_num)
                  unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
              }
              else
              {
                unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
              }
            }
            else
            {
              if ((p_Dpb->fs_ltref[i]->frame_num) != (unsigned)(curr_pic_num >> 1))
              {
                unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
              }
            }
          }
        }
      }
      if (structure == BOTTOM_FIELD)
      {
        if ((p_Dpb->fs_ltref[i]->is_long_term == 3))
        {
          unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
        }
        else
        {
          if ((p_Dpb->fs_ltref[i]->is_long_term == 2))
          {
            unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
          }
          else
          {
            if (mark_current)
            {
              if (p_Dpb->last_picture)
              {
                if ( ( p_Dpb->last_picture != p_Dpb->fs_ltref[i] )|| p_Dpb->last_picture->frame_num != curr_frame_num)
                  unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
              }
              else
              {
                unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
              }
            }
            else
            {
              if ((p_Dpb->fs_ltref[i]->frame_num) != (unsigned)(curr_pic_num >> 1))
              {
                unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
              }
            }
          }
        }
      }
#endif
    }
#if (MVC_EXTENSION_ENABLE)
    }
#endif
  }
}

static VO_S32 mark_pic_long_term(DecodedPictureBuffer *p_Dpb, StorablePicture* p, int long_term_frame_idx, int picNumX)
{
  unsigned i;
  //int add_top, add_bottom;

  if (p->structure == FRAME)
  {
    for (i=0; i<p_Dpb->ref_frames_in_buffer; i++)
    {
      if (p_Dpb->fs_ref[i]->is_reference == 3)
      {
        if ((!p_Dpb->fs_ref[i]->frame->is_long_term)&&(p_Dpb->fs_ref[i]->frame->pic_num == picNumX))
        {
          p_Dpb->fs_ref[i]->long_term_frame_idx = p_Dpb->fs_ref[i]->frame->long_term_frame_idx
                                             = long_term_frame_idx;
          p_Dpb->fs_ref[i]->frame->long_term_pic_num = long_term_frame_idx;
          p_Dpb->fs_ref[i]->frame->is_long_term = 1;

          if (p_Dpb->fs_ref[i]->top_field && p_Dpb->fs_ref[i]->bottom_field)
          {
            p_Dpb->fs_ref[i]->top_field->long_term_frame_idx = p_Dpb->fs_ref[i]->bottom_field->long_term_frame_idx
                                                          = long_term_frame_idx;
            p_Dpb->fs_ref[i]->top_field->long_term_pic_num = long_term_frame_idx;
            p_Dpb->fs_ref[i]->bottom_field->long_term_pic_num = long_term_frame_idx;

            p_Dpb->fs_ref[i]->top_field->is_long_term = p_Dpb->fs_ref[i]->bottom_field->is_long_term
                                                   = 1;

          }
          p_Dpb->fs_ref[i]->is_long_term = 3;
          return 0;
        }
      }
    }
    //printf ("Warning: reference frame for long term marking not found\n");
	VOH264ERROR(VO_H264_ERR_NUMREFFRAME);
  }
  return 0;
#if 0
  else
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
    for (i=0; i<p_Dpb->ref_frames_in_buffer; i++)
    {
#if (MVC_EXTENSION_ENABLE)
      if (p_Dpb->fs_ref[i]->view_id == p->view_id)
      {
#endif
      if (p_Dpb->fs_ref[i]->is_reference & 1)
      {
        if ((!p_Dpb->fs_ref[i]->top_field->is_long_term)&&(p_Dpb->fs_ref[i]->top_field->pic_num == picNumX))
        {
          if ((p_Dpb->fs_ref[i]->is_long_term) && (p_Dpb->fs_ref[i]->long_term_frame_idx != long_term_frame_idx))
          {
              printf ("Warning: assigning long_term_frame_idx different from other field\n");
          }

          p_Dpb->fs_ref[i]->long_term_frame_idx = p_Dpb->fs_ref[i]->top_field->long_term_frame_idx
                                             = long_term_frame_idx;
          p_Dpb->fs_ref[i]->top_field->long_term_pic_num = 2 * long_term_frame_idx + add_top;
          p_Dpb->fs_ref[i]->top_field->is_long_term = 1;
          p_Dpb->fs_ref[i]->is_long_term |= 1;
          if (p_Dpb->fs_ref[i]->is_long_term == 3)
          {
            p_Dpb->fs_ref[i]->frame->is_long_term = 1;
            p_Dpb->fs_ref[i]->frame->long_term_frame_idx = p_Dpb->fs_ref[i]->frame->long_term_pic_num = long_term_frame_idx;
          }
          return;
        }
      }
      if (p_Dpb->fs_ref[i]->is_reference & 2)
      {
        if ((!p_Dpb->fs_ref[i]->bottom_field->is_long_term)&&(p_Dpb->fs_ref[i]->bottom_field->pic_num == picNumX))
        {
          if ((p_Dpb->fs_ref[i]->is_long_term) && (p_Dpb->fs_ref[i]->long_term_frame_idx != long_term_frame_idx))
          {
              printf ("Warning: assigning long_term_frame_idx different from other field\n");
          }

          p_Dpb->fs_ref[i]->long_term_frame_idx = p_Dpb->fs_ref[i]->bottom_field->long_term_frame_idx
                                             = long_term_frame_idx;
          p_Dpb->fs_ref[i]->bottom_field->long_term_pic_num = 2 * long_term_frame_idx + add_bottom;
          p_Dpb->fs_ref[i]->bottom_field->is_long_term = 1;
          p_Dpb->fs_ref[i]->is_long_term |= 2;
          if (p_Dpb->fs_ref[i]->is_long_term == 3)
          {
            p_Dpb->fs_ref[i]->frame->is_long_term = 1;
            p_Dpb->fs_ref[i]->frame->long_term_frame_idx = p_Dpb->fs_ref[i]->frame->long_term_pic_num = long_term_frame_idx;
          }
          return;
        }
      }
#if (MVC_EXTENSION_ENABLE)
      }
#endif
    }
    printf ("Warning: reference field for long term marking not found\n");
  }
#endif
}


static VO_S32 mm_assign_long_term_frame_idx(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb, StorablePicture* p, int difference_of_pic_nums_minus1, int long_term_frame_idx)
{
  int picNumX = get_pic_num_x(p, difference_of_pic_nums_minus1);

  // remove frames/fields with same long_term_frame_idx
  if (p->structure == FRAME)
  {
    unmark_long_term_frame_for_reference_by_frame_idx(p_Dpb, long_term_frame_idx);
  }
  else
  {
    unsigned i;
    PictureStructure structure = FRAME;

    for (i=0; i<p_Dpb->ref_frames_in_buffer; i++)
    {
      if (p_Dpb->fs_ref[i]->is_reference & 1)
      {
        if (p_Dpb->fs_ref[i]->top_field->pic_num == picNumX)
        {
          structure = TOP_FIELD;
          break;
        }
      }
      if (p_Dpb->fs_ref[i]->is_reference & 2)
      {
        if (p_Dpb->fs_ref[i]->bottom_field->pic_num == picNumX)
        {
          structure = BOTTOM_FIELD;
          break;
        }
      }
    }
    if (structure==FRAME)
      VOH264ERROR(VO_H264_ERR_MEMORY_MANAGEMENT);

    unmark_long_term_field_for_reference_by_frame_idx(pDecGlobal,p_Dpb, structure, long_term_frame_idx, 0, 0, picNumX);
  }

  return mark_pic_long_term(p_Dpb, p, long_term_frame_idx, picNumX);
}


void mm_update_max_long_term_frame_idx(DecodedPictureBuffer *p_Dpb, int max_long_term_frame_idx_plus1)
{
  unsigned i;

  p_Dpb->max_long_term_pic_idx = max_long_term_frame_idx_plus1 - 1;

  // check for invalid frames
  for (i=0; i<p_Dpb->ltref_frames_in_buffer; i++)
  {
    if (p_Dpb->fs_ltref[i]->long_term_frame_idx > p_Dpb->max_long_term_pic_idx)
    {
      unmark_for_long_term_reference(p_Dpb->fs_ltref[i]);
    }
  }
}

static void mm_unmark_all_long_term_for_reference (DecodedPictureBuffer *p_Dpb)
{
  mm_update_max_long_term_frame_idx(p_Dpb, 0);
}


static void mm_unmark_all_short_term_for_reference (DecodedPictureBuffer *p_Dpb)
{
  unsigned int i;
  for (i=0; i<p_Dpb->ref_frames_in_buffer; i++)
  {
    unmark_for_reference(p_Dpb->fs_ref[i]);
  }
  update_ref_list(p_Dpb);
}



static void mm_mark_current_picture_long_term(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb, StorablePicture *p, int long_term_frame_idx)
{
  // remove long term pictures with same long_term_frame_idx
  if (p->structure == FRAME)
  {
    unmark_long_term_frame_for_reference_by_frame_idx(p_Dpb, long_term_frame_idx);
  }
  else
  {
    unmark_long_term_field_for_reference_by_frame_idx(pDecGlobal,p_Dpb, p->structure, long_term_frame_idx, 1, p->pic_num, 0);
  }

  p->is_long_term = 1;
  p->long_term_frame_idx = long_term_frame_idx;
}


static VO_S32 adaptive_memory_management(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb, StorablePicture* p)
{
  DecRefPicMarking_t *tmp_drpm;
  //VideoParameters *p_Vid = p_Dpb->p_Vid;
  VO_S32 ret;
  pDecGlobal->last_has_mmco_5 = 0;

  //assert (!p->idr_flag);
  //assert (p->adaptive_ref_pic_buffering_flag);

  while (p->dec_ref_pic_marking_buffer)
  {
    tmp_drpm = p->dec_ref_pic_marking_buffer;
    switch (tmp_drpm->memory_management_control_operation)
    {
      case 0:
        if (tmp_drpm->Next != NULL)
          VOH264ERROR(VO_H264_ERR_MEMORY_MANAGEMENT);
        break;
      case 1:
        mm_unmark_short_term_for_reference(p_Dpb, p, tmp_drpm->difference_of_pic_nums_minus1);
        update_ref_list(p_Dpb);
        break;
      case 2:
        mm_unmark_long_term_for_reference(p_Dpb, p, tmp_drpm->long_term_pic_num);
        update_ltref_list(p_Dpb);
        break;
      case 3:
        ret = mm_assign_long_term_frame_idx(pDecGlobal,p_Dpb, p, tmp_drpm->difference_of_pic_nums_minus1, tmp_drpm->long_term_frame_idx);
		if(ret)
		  return ret;
        update_ref_list(p_Dpb);
        update_ltref_list(p_Dpb);
        break;
      case 4:
        mm_update_max_long_term_frame_idx (p_Dpb, tmp_drpm->max_long_term_frame_idx_plus1);
        update_ltref_list(p_Dpb);
        break;
      case 5:
        mm_unmark_all_short_term_for_reference(p_Dpb);
        mm_unmark_all_long_term_for_reference(p_Dpb);
       pDecGlobal->last_has_mmco_5 = 1;
        break;
      case 6:
        mm_mark_current_picture_long_term(pDecGlobal,p_Dpb, p, tmp_drpm->long_term_frame_idx);
        if ((int)(p_Dpb->ltref_frames_in_buffer +  p_Dpb->ref_frames_in_buffer ) > (imax(1, p_Dpb->num_ref_frames)))
          VOH264ERROR(VO_H264_ERR_NUMREFFRAME);
        break;
      default:
        VOH264ERROR(VO_H264_ERR_MEMORY_MANAGEMENT);
    }
    p->dec_ref_pic_marking_buffer = tmp_drpm->Next;
    //voFree (pDecGlobal,tmp_drpm);
  }
  if ( pDecGlobal->last_has_mmco_5 )
  {
    p->pic_num = p->frame_num = 0;

    switch (p->structure)
    {
#if 1
    case TOP_FIELD:
      {
        //p->poc = p->top_poc = p_Vid->toppoc =0;
        p->poc = p->top_poc = 0;
        break;
      }
    case BOTTOM_FIELD:
      {
        //p->poc = p->bottom_poc = p_Vid->bottompoc = 0;
        p->poc = p->bottom_poc = 0;
        break;
      }
#endif
    case FRAME:
      {
        p->top_poc    -= p->poc;
        p->bottom_poc -= p->poc;

        //p_Vid->toppoc = p->top_poc;
        //p_Vid->bottompoc = p->bottom_poc;

        p->poc = imin (p->top_poc, p->bottom_poc);
        //p_Vid->framepoc = p->poc;
        break;
      }
    }
    //currSlice->ThisPOC = p->poc;
#if (MVC_EXTENSION_ENABLE)
    flush_dpb(pDecGlobal,p_Dpb, p->view_id);
#else
    flush_dpb(pDecGlobal,p_Dpb);
#endif
  }
  return 0;
}

VO_S32 store_picture_in_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb, StorablePicture* p)
{
  //VideoParameters *p_Vid = p_Dpb->p_Vid;
  unsigned i,j=0,ret;
  int poc, pos;
  assert (p!=NULL);

  pDecGlobal->last_has_mmco_5=0;
  pDecGlobal->last_pic_bottom_field = (p->structure == BOTTOM_FIELD);

  if (pDecGlobal->idr_flag)
  {
    idr_memory_management(pDecGlobal,p_Dpb, p);
  }
  else
  {
    // adaptive memory management
    if (p->used_for_reference && (p->adaptive_ref_pic_buffering_flag))
    {
      ret = adaptive_memory_management(pDecGlobal,p_Dpb, p);
	  if(ret)
	  	return ret;
    }
  }
  if (pDecGlobal->direct_set && p->structure == FRAME)
  {
	  pDecGlobal->direct_set = 0;
	  VOH264ERROR(VO_H264_ERR_PIC_STRUCT);
  }
//   if ((p->structure==TOP_FIELD)||(p->structure==BOTTOM_FIELD))
//   {
      if(pDecGlobal->direct_set)
      {
		assert(p->structure==TOP_FIELD || p->structure==BOTTOM_FIELD);
        pDecGlobal->direct_set = 0;
// printf("direct_set poc %d\n", p->poc);

		if(p->structure==BOTTOM_FIELD)
		  p->top_field=pDecGlobal->p_Dpb->direct_out;
		else if(p->structure==TOP_FIELD)
		  p->bottom_field=pDecGlobal->p_Dpb->direct_out;

		output_one_picture (pDecGlobal, p);
 	    //FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, p->cur_buf_index, FIFO_WRITE);
 	    pDecGlobal->p_Dpb->direct_out = NULL;
		p_Dpb->p_remove[p_Dpb->remove_size++]=p;
	    //p->removable = 1;
		return 0;
      }
	  if (p->structure == FRAME && p_Dpb->last_picture)
	  {
		  p_Dpb->last_picture = NULL;
	  }
	  if (p_Dpb->last_picture)
	  {
		  if ((int)p_Dpb->last_picture->frame_num == p->pic_num)
		  {
// 			  if (((p->structure==TOP_FIELD)&&(p_Dpb->last_picture->is_used==2))||((p->structure==BOTTOM_FIELD)&&(p_Dpb->last_picture->is_used==1)))
			  assert(p->structure==TOP_FIELD || p->structure==BOTTOM_FIELD);
			  if((int)(p->structure) + p_Dpb->last_picture->is_used == 3)
			  {
				  assert(p_Dpb->last_picture->is_orig_reference<3);
// 				  if ((p->used_for_reference && (p_Dpb->last_picture->is_orig_reference!=0))||
// 					  (!p->used_for_reference && (p_Dpb->last_picture->is_orig_reference==0)))
				  if(!p->used_for_reference == !p_Dpb->last_picture->is_orig_reference)
				  {
					  insert_picture_in_dpb(pDecGlobal, p_Dpb->last_picture,p);
					  update_ref_list(p_Dpb);
					  update_ltref_list(p_Dpb);
					  p_Dpb->last_picture = NULL;
					  if (pDecGlobal->bDirectOutputMode == 1 || pDecGlobal->bThumbnailMode == 1)
						  output_one_picture(pDecGlobal, p);
					  return 0;
				  }
			  }
		  }
	  }
//   }
  // sliding window, if necessary
  if ((!pDecGlobal->idr_flag)&&(p->used_for_reference && (!p->adaptive_ref_pic_buffering_flag)))
  {
    sliding_window_memory_management(p_Dpb, p);
  }
  /*if(p_Dpb->used_size)
  {
    get_smallest_poc(p_Dpb, &poc, &pos);
    if (!p->used_for_reference)
    {
      if ((-1==pos) || (p->poc < poc))
      {
	    p_Dpb->last_output_poc = p->poc;
        direct_output(pDecGlobal, p, pDecGlobal->p_out);
		
        return;
      }
    }
    if(poc > p_Dpb->last_output_poc&& poc <= p_Dpb->last_output_poc+2)
    {
      write_stored_frame(pDecGlobal, p_Dpb->fs[pos], pDecGlobal->p_out);
	  p_Dpb->last_output_poc = poc;
    }
  }
  else
	 p_Dpb->last_output_poc = -1;*/
  // first try to remove unused frames
  if (p_Dpb->used_size==p_Dpb->size)
  {
#if (MVC_EXTENSION_ENABLE)
    remove_unused_frame_from_dpb(pDecGlobal,p_Dpb, p->view_id);
#else
    remove_unused_frame_from_dpb(pDecGlobal,p_Dpb);
#endif
  }
  
  // then output frames until one can be removed
  while (p_Dpb->used_size == p_Dpb->size)
  {
	  ret = get_smallest_poc(p_Dpb, &poc, &pos);
	  if(ret)
		  return ret;
	if (!p->used_for_reference)
    {
      if ((-1==pos) || (p->poc < poc))
      {
        direct_output(pDecGlobal, p);
        return 0;
      }
    }
    // flush a frame
    //output_one_frame_from_dpb(pDecGlobal,p_Dpb);
	if (pos == -1)
	{
#if FLUSH_ERROR
		flush_dpb(pDecGlobal, p_Dpb);
		update_ref_list(p_Dpb);
		update_ltref_list(p_Dpb);

#else
		VO_U32 size = p_Dpb->size + 3*pDecGlobal->nThdNum + 1 + pDecGlobal->extra_dpb;

		for (i = 0; i < p_Dpb->ref_frames_in_buffer; i++)
			p_Dpb->fs_ref[i]=NULL;
		for (i=0; i<p_Dpb->ltref_frames_in_buffer; i++)
			p_Dpb->fs_ltref[i]=NULL;

		pDecGlobal->vFrameIndexFIFO.r_idx = pDecGlobal->vFrameIndexFIFO.w_idx = 0;

		for(i = 0; i < size; i++)
		{
			FrameBufCtl(&pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
		}

		p_Dpb->used_size=0;
		//printf("remove all\n");
		p_Dpb->remove_size=0;
		update_ref_list(p_Dpb);
		update_ltref_list(p_Dpb);
		p_Dpb->last_output_poc = INT_MIN;
#endif
	  if (pDecGlobal->long_term_reference_flag)
      {
        p_Dpb->max_long_term_pic_idx = 0;
        p->is_long_term           = 1;
        p->long_term_frame_idx    = 0;
      }
      else
      {
        p_Dpb->max_long_term_pic_idx = -1;
        p->is_long_term           = 0;
      }
	  //VOH264ERROR(VO_H264_ERR_POS); 
	  //printf("pos error Flush DPB\r\n");
	  break;
	}
// printf("store_picture_in_dpb poc:%d\n",poc);
	write_stored_frame(pDecGlobal, p_Dpb->fs[pos]);
	p_Dpb->last_output_poc = poc;
    // voFree frame store and move empty store to end of buffer
    if (!is_used_for_reference(p_Dpb->fs[pos]))
    {
      remove_frame_from_dpb(pDecGlobal,p_Dpb, pos);
    }
  }
  // check for duplicate frame number in short term reference buffer
  if ((p->used_for_reference)&&(!p->is_long_term))
  {
    for (i=0; i<p_Dpb->ref_frames_in_buffer; i++)
    {
#if (MVC_EXTENSION_ENABLE)
      //if (p_Dpb->fs_ref[i]->frame_num == p->frame_num && p_Dpb->fs_ref[i]->view_id == p->view_id)
#else
      if (p_Dpb->fs_ref[i]->frame_num == p->frame_num)
#endif
      { //VOH264ERROR(VO_H264_ERR_DUPLICATE_FRAMENUM);
		//printf("duplicate frame number in short term reference buffer\r\n");
      }
    }

  }
  // store at end of buffer
  //printf("used_size:%d\r\n",p_Dpb->used_size);
  if(!p->errFlag)
    insert_picture_in_dpb(pDecGlobal, p_Dpb->fs[p_Dpb->used_size],p);

  if (p->structure != FRAME)
  {
    p_Dpb->last_picture = p_Dpb->fs[p_Dpb->used_size];
  }
  else
  {
    p_Dpb->last_picture = NULL;
  }
  //if (pDecGlobal->idr_flag&&p->structure == FRAME)
  //{
  //  write_stored_frame(pDecGlobal, p_Dpb->fs[p_Dpb->used_size]);
  //}
  p_Dpb->used_size++;
  if ((pDecGlobal->bDirectOutputMode == 1 || pDecGlobal->bThumbnailMode == 1)&& p->structure == FRAME)
	  output_one_picture(pDecGlobal, p);

#if (MVC_EXTENSION_ENABLE)
  update_ref_list(p_Dpb, p->view_id);
  update_ltref_list(p_Dpb, p->view_id);
#else
  update_ref_list(p_Dpb);
  update_ltref_list(p_Dpb);
#endif


  return 0;
}

static void insert_picture_in_dpb(H264DEC_G *pDecGlobal, FrameStore* fs, StorablePicture* p)
{
//  printf ("insert (%s) pic with frame_num #%d, poc %d\n", (p->structure == FRAME)?"FRAME":(p->structure == TOP_FIELD)?"TOP_FIELD":"BOTTOM_FIELD", p->pic_num, p->poc);
  assert (p!=NULL);
  assert (fs!=NULL);
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
        fs->long_term_frame_idx = p->long_term_frame_idx;
      }
    }
#if (MVC_EXTENSION_ENABLE)
    fs->view_id = p->view_id;
		fs->inter_view_flag[0] = fs->inter_view_flag[1] = p->inter_view_flag;
		fs->anchor_pic_flag[0] = fs->anchor_pic_flag[1] = p->anchor_pic_flag;
#endif
    // generate field views
    dpb_split_field(pDecGlobal, fs);
    break;
#if 1
  case TOP_FIELD:
    fs->top_field = p;
    fs->is_used |= 1;
#if (MVC_EXTENSION_ENABLE)
    fs->view_id = p->view_id;
		fs->inter_view_flag[0] = p->inter_view_flag;
		fs->anchor_pic_flag[0] = p->anchor_pic_flag;
#endif
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
    if (fs->is_used == 3)
    {
      // generate frame view
      dpb_combine_field_yuv(pDecGlobal, fs);
    }
    else
    {
      fs->poc = p->poc;
      //gen_field_ref_ids(pDecGlobal, p);
    }
    break;
  case BOTTOM_FIELD:
    fs->bottom_field = p;
    fs->is_used |= 2;
#if (MVC_EXTENSION_ENABLE)
    fs->view_id = p->view_id;
		fs->inter_view_flag[1] = p->inter_view_flag;
		fs->anchor_pic_flag[1] = p->anchor_pic_flag;
#endif
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
    if (fs->is_used == 3)
    {
      // generate frame view
      dpb_combine_field_yuv(pDecGlobal, fs);
    }
    else
    {
      fs->poc = p->poc;
      //gen_field_ref_ids(pDecGlobal, p);
    }
    break;
#endif
  }
  fs->frame_num = p->pic_num;
  fs->recovery_frame = pDecGlobal->recovery_frame;

  fs->is_output = p->is_output;

}

static int is_used_for_reference(FrameStore* fs)
{
  if (fs->is_reference)
  {
    return 1;
  }

  if (fs->is_used == 3) // frame
  {
    if (fs->frame->used_for_reference)
    {
      return 1;
    }
  }

  if (fs->is_used & 1) // top field
  {
    if (fs->top_field)
    {
      if (fs->top_field->used_for_reference)
      {
        return 1;
      }
    }
  }

  if (fs->is_used & 2) // bottom field
  {
    if (fs->bottom_field)
    {
      if (fs->bottom_field->used_for_reference)
      {
        return 1;
      }
    }
  }
  return 0;
}

static int is_short_term_reference(FrameStore* fs)
{

  if (fs->is_used==3) // frame
  {
    if ((fs->frame->used_for_reference)&&(!fs->frame->is_long_term))
    {
      return 1;
    }
  }

  if (fs->is_used & 1) // top field
  {
    if (fs->top_field)
    {
      if ((fs->top_field->used_for_reference)&&(!fs->top_field->is_long_term))
      {
        return 1;
      }
    }
  }

  if (fs->is_used & 2) // bottom field
  {
    if (fs->bottom_field)
    {
      if ((fs->bottom_field->used_for_reference)&&(!fs->bottom_field->is_long_term))
      {
        return 1;
      }
    }
  }
  return 0;
}


static int is_long_term_reference(FrameStore* fs)
{

  if (fs->is_used==3) // frame
  {
    if ((fs->frame->used_for_reference)&&(fs->frame->is_long_term))
    {
      return 1;
    }
  }

  if (fs->is_used & 1) // top field
  {
    if (fs->top_field)
    {
      if ((fs->top_field->used_for_reference)&&(fs->top_field->is_long_term))
      {
        return 1;
      }
    }
  }

  if (fs->is_used & 2) // bottom field
  {
    if (fs->bottom_field)
    {
      if ((fs->bottom_field->used_for_reference)&&(fs->bottom_field->is_long_term))
      {
        return 1;
      }
    }
  }
  return 0;
}


static VO_S32 get_smallest_poc(DecodedPictureBuffer *p_Dpb, int *poc,int * pos)
{
  unsigned i;

  if (p_Dpb->used_size<1)
    VOH264ERROR(VO_H264_ERR_DPB_EMPTY);

  *pos=-1;
  *poc = INT_MAX;
  for (i = 0; i < p_Dpb->used_size; i++)
  {
    if ((*poc > p_Dpb->fs[i]->poc)&&(!p_Dpb->fs[i]->is_output))
    {
      *poc = p_Dpb->fs[i]->poc;
      *pos=i;
    }
  }
  return 0;
}




static VO_S32 output_one_frame_from_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb)
{
  //VideoParameters *p_Vid = p_Dpb->p_Vid;
  int poc, pos,ret;
  //diagnostics
  if (p_Dpb->used_size<1)
    VOH264ERROR(VO_H264_ERR_DPB_NO_OUTPUT);

  // find smallest POC
  ret = get_smallest_poc(p_Dpb, &poc, &pos);
  if(ret)
  	return ret;
  if(pos==-1)
    VOH264ERROR(VO_H264_ERR_DPB_NO_OUTPUT);
  write_stored_frame(pDecGlobal, p_Dpb->fs[pos]);

  // picture error concealment
#if (MVC_EXTENSION_ENABLE)
  //if(p_Vid->conceal_mode == 0) //remove by Really Yang 20110420
    if (p_Dpb->last_output_poc >= poc && p_Dpb->fs[pos]->view_id == p_Dpb->last_output_view_id)
      VOH264ERROR(VO_H264_ERR_DPB_NO_OUTPUT);
  p_Dpb->last_output_poc = poc;
  p_Dpb->last_output_view_id = p_Dpb->fs[pos]->view_id;
  // voFree frame store and move empty store to end of buffer
  if (!is_used_for_reference(p_Dpb->fs[pos]))
  {
    remove_frame_from_dpb(p_Dpb, pos);
  }

  return 1;
#else
  //if(p_Vid->conceal_mode == 0)  //remove by Really Yang 20110420
    if (p_Dpb->last_output_poc >= poc)
    {
      //error ("output POC must be in ascending order", 150);
      //warning_dtt
    }
  p_Dpb->last_output_poc = poc;
  // voFree frame store and move empty store to end of buffer
  if (!is_used_for_reference(p_Dpb->fs[pos]))
  {
    remove_frame_from_dpb(pDecGlobal,p_Dpb, pos);
  }
#endif
  return 0;
}


VO_S32 flush_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb)
{
  //VideoParameters *p_Vid = p_Dpb->p_Vid;
  unsigned i;
  VO_S32 ret;

  // mark all frames unused
  for (i=0; i<p_Dpb->used_size; i++)
  {
    unmark_for_reference (p_Dpb->fs[i]);
  }

  while (remove_unused_frame_from_dpb(pDecGlobal,p_Dpb)) ;

  // output frames in POC order
  while (p_Dpb->used_size)
  {
    ret = output_one_frame_from_dpb(pDecGlobal,p_Dpb);
	if(ret)
	  return ret;
  }

  p_Dpb->last_output_poc = INT_MIN;
  return 0;
}

#if 0
static void gen_field_ref_ids(H264DEC_G *pDecGlobal, StorablePicture *p)
{
  int i;

  //copy the list;
  if(p->listX[LIST_0])
  {
    p->listXsize[LIST_0] =  pDecGlobal->pCurSlice->listXsize[LIST_0];
    for(i=0; i<p->listXsize[LIST_0]; i++)
      p->listX[LIST_0][i] = pDecGlobal->pCurSlice->listX[LIST_0][i];
  }
  if(p->listX[LIST_1])
  {
    p->listXsize[LIST_1] =  pDecGlobal->pCurSlice->listXsize[LIST_1];
    for(i=0; i<p->listXsize[LIST_1]; i++)
      p->listX[LIST_1][i] = pDecGlobal->pCurSlice->listX[LIST_1][i];
  }
  
}
#endif

void dpb_split_field(H264DEC_G *pDecGlobal, FrameStore *fs)
{
  StorablePicture *fs_top, *fs_btm; 
  StorablePicture *frame = fs->frame;

  fs->poc = frame->poc;
  if (pDecGlobal->pCurSlice->active_sps->frame_mbs_only_flag)
  {
	  fs->top_field = frame;
	  fs->bottom_field=frame;
	  frame->top_field=frame;
	  frame->bottom_field=frame;
	  frame->frame = frame;
	  //fs_btm->iCodingType= fs_top->iCodingType = frame->iCodingType;
  }
  else
  {
	  VO_S32 i,j,jj;

	  i = fs->frame->cur_buf_index;
	  //fs_top = fs->top_field    = voMalloc(pDecGlobal,sizeof(StorablePicture));
	  //fs_btm = fs->bottom_field = voMalloc(pDecGlobal,sizeof(StorablePicture));
	  fs_top = fs->top_field  = pDecGlobal->p_Dpb->storable_pic_buffer[1] + i;
	  fs_btm = fs->bottom_field  = pDecGlobal->p_Dpb->storable_pic_buffer[2] + i;
	  fs_top->structure = TOP_FIELD;
	  //fs_top->imgY = frame->imgY;
	  //fs_top->imgUV[0] = frame->imgUV[0];
	  //fs_top->imgUV[1] = frame->imgUV[1];
	  //fs_top->ref_idx[0] = frame->ref_idx[0];
	  //fs_top->ref_idx[1] = frame->ref_idx[1];
	  //fs_top->ref_pic[0] = frame->ref_pic[0];
	  //fs_top->ref_pic[1] = frame->ref_pic[1];
	  //fs_top->mv[0] = frame->mv[0];
	  //fs_top->mv[1] = frame->mv[1];
	  //fs_top->mb_type_buffer = frame->mb_type_buffer;
	  fs_top->cur_buf_index = i;
	  //fs_top->mb_field_frame_buffer = (VO_U8 *) voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs/2*sizeof(VO_U8));
	  fs_btm->structure = BOTTOM_FIELD;
	  //fs_btm->imgY = frame->imgY+pDecGlobal->iLumaStride;
	  //fs_btm->imgUV[0] = frame->imgUV[0]+pDecGlobal->iChromaStride;
	  //fs_btm->imgUV[1] = frame->imgUV[1]+pDecGlobal->iChromaStride;
	  //fs_btm->ref_idx[0] = frame->ref_idx[0];
	  //fs_btm->ref_idx[1] = frame->ref_idx[1];
	  //fs_btm->ref_pic[0] = frame->ref_pic[0];
	  //fs_btm->ref_pic[1] = frame->ref_pic[1];
	  //fs_btm->mv[0] = frame->mv[0];
	  //fs_btm->mv[1] = frame->mv[1];
	  //fs_btm->mb_type_buffer = frame->mb_type_buffer;
	  fs_btm->cur_buf_index = i;
	  //fs_btm->mb_field_frame_buffer = (VO_U8 *) voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs/2*sizeof(VO_U8));

	  fs_top->poc = frame->top_poc;
	  fs_btm->poc = frame->bottom_poc;


	  fs_top->frame_poc =  frame->frame_poc;

	  fs_top->bottom_poc = fs_btm->bottom_poc =  frame->bottom_poc;
	  fs_top->top_poc    = fs_btm->top_poc    =  frame->top_poc;
	  fs_btm->frame_poc  = frame->frame_poc;

	  fs_top->used_for_reference = fs_btm->used_for_reference
		  = frame->used_for_reference;
	  fs_top->is_long_term = fs_btm->is_long_term
		  = frame->is_long_term;
	  fs->long_term_frame_idx = fs_top->long_term_frame_idx
		  = fs_btm->long_term_frame_idx
		  = frame->long_term_frame_idx;

	  fs_top->coded_frame = fs_btm->coded_frame = 1;
	  //fs_top->mb_aff_frame_flag = fs_btm->mb_aff_frame_flag
	  //                    = frame->mb_aff_frame_flag;

	  frame->top_field    = fs_top;
	  frame->bottom_field = fs_btm;
	  frame->frame         = frame;
	  fs_top->bottom_field = fs_btm;
	  fs_top->frame        = frame;
	  fs_top->top_field = fs_top;
	  fs_btm->top_field = fs_top;
	  fs_btm->frame     = frame;
	  fs_btm->bottom_field = fs_btm;

	  fs_top->iCodingType = fs_btm->iCodingType = frame->iCodingType;

	  //fs_btm->iCodingType= fs_top->iCodingType = frame->iCodingType;
	  //fs_top->need_prepare_param = fs_btm->need_prepare_param = 1;
	  /*for(i = 0;i <6;i++)
	  {
		  if(pDecGlobal->pCurSlice->listXsize[i])
		  {
			  frame->listXsize[i] =  pDecGlobal->pCurSlice->listXsize[i];
			  for(j=0; j<frame->listXsize[i]; j++)
				  frame->listX[i][j] = pDecGlobal->pCurSlice->listX[i][j];
		  }
	  }*/
#if USE_FRAME_THREAD
	  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	  {
	      fs->frame->frame_set=fs->frame->top_set=fs->frame->bottom_set=1;
	      fs->top_field->frame_set=fs->top_field->top_set=fs->top_field->bottom_set=1;
	      fs->bottom_field->frame_set=fs->bottom_field->top_set=fs->bottom_field->bottom_set=1;
		  fs->top_field->nProgress[0]=fs->bottom_field->nProgress[0]=-100;
		  return;
	  }
#endif
#if 1
	  /*for (j = 0; j < pDecGlobal->FrameHeightInMbs>>1; ++j)
	  {
		  VO_S32 offset_j = (j<<4)*pDecGlobal->PicWidthInMbs;
		  for (i = 0; i < pDecGlobal->PicWidthInMbs; ++i)
		  {
			  VO_S32 idx_0 = offset_j + (i<<4);
			  VO_S32 idx_1_up = (offset_j<<1) + (i<<4);
			  VO_S32 idx_1_down = idx_1_up + (pDecGlobal->PicWidthInMbs<<4);
			  if (frame->mbaff_flag && IS_INTERLACED(frame->mb_type_buffer[idx_1_up>>4]))
			  {
				  for (jj = 0; jj < 16; ++jj)
				  {
					  split_4x4(fs_top, frame, idx_0+jj, idx_1_up+jj, 2);
					  split_4x4(fs_btm, frame, idx_0+jj, idx_1_down+jj, 4);
				  }
			  }
			  else
			  {
				  for (jj = 0; jj < 16; ++jj)
				  {
					  VO_S32 idx_1 = (jj&8)?(idx_1_down+8):idx_1_up;
					  idx_1 += (jj&2)?3:0;

					  split_4x4(fs_top, frame, idx_0+jj, idx_1, 0);
					  split_4x4(fs_btm, frame, idx_0+jj, idx_1, 0);
				  }
			  }
		  }
	  }*/
#elif 1
	  for (j = 0; j < pDecGlobal->FrameHeightInMbs>>1; ++j)
	  {
		  for (i = 0; i < pDecGlobal->PicWidthInMbs; ++i)
		  {
			  split_mb(fs_top, i, j, pDecGlobal->PicWidthInMbs);
			  split_mb(fs_btm, i, j, pDecGlobal->PicWidthInMbs);
		  }
	  }
#else
	  for (j=0 ; j< (pDecGlobal->FrameHeightInMbs>>1); j++)
	  {
		  jj = j*2;
		  jj4 = jj+1;
		  field_buffer = frame->mb_field_frame_buffer + jj*pDecGlobal->PicWidthInMbs;
		  for (i=0 ; i < pDecGlobal->PicWidthInMbs; i++)
		  {

			  if (!(pDecGlobal->pCurSlice->mb_aff_frame_flag && (*field_buffer++)))
			  {
				  VO_S32 index = (j*pDecGlobal->PicWidthInMbs+i)<<4;
				  VO_S32 index_top = (jj*pDecGlobal->PicWidthInMbs+i)<<4;
				  VO_S32 index_bottom = ((jj4*pDecGlobal->PicWidthInMbs+i)<<4)+8;
				  VO_S32 list;
				  for(list=0;list<2;list++)
				  {
					  fs_top->mv[list][index]=fs_top->mv[list][index+1]=
						  fs_top->mv[list][index+4]=fs_top->mv[list][index+5]=
						  fs_btm->mv[list][index]=fs_btm->mv[list][index+1]=
						  fs_btm->mv[list][index+4]=fs_btm->mv[list][index+5]=frame->mv[list][index_top];
					  fs_top->mv[list][index+2]=fs_top->mv[list][index+3]=
						  fs_top->mv[list][index+6]=fs_top->mv[list][index+7]=
						  fs_btm->mv[list][index+2]=fs_btm->mv[list][index+3]=
						  fs_btm->mv[list][index+6]=fs_btm->mv[list][index+7]=frame->mv[list][index_top+3];
					  fs_top->mv[list][index+8]=fs_top->mv[list][index+9]=
						  fs_top->mv[list][index+12]=fs_top->mv[list][index+13]=
						  fs_btm->mv[list][index+8]=fs_btm->mv[list][index+9]=
						  fs_btm->mv[list][index+12]=fs_btm->mv[list][index+13]=frame->mv[list][index_bottom];
					  fs_top->mv[list][index+10]=fs_top->mv[list][index+11]=
						  fs_top->mv[list][index+14]=fs_top->mv[list][index+15]=
						  fs_btm->mv[list][index+10]=fs_btm->mv[list][index+11]=
						  fs_btm->mv[list][index+14]=fs_btm->mv[list][index+15]=frame->mv[list][index_bottom+3];

					  if(frame->ref_idx[list][index_top]==-1)
					  {
						  fs_top->ref_idx[list][index]=fs_top->ref_idx[list][index+1]=
							  fs_top->ref_idx[list][index+4]=fs_top->ref_idx[list][index+5]=
							  fs_btm->ref_idx[list][index]=fs_btm->ref_idx[list][index+1]=
							  fs_btm->ref_idx[list][index+4]=fs_btm->ref_idx[list][index+5]=-1;
					  }
					  else
					  {
						  fs_top->ref_idx[list][index]=fs_top->ref_idx[list][index+1]=
							  fs_top->ref_idx[list][index+4]=fs_top->ref_idx[list][index+5]=
							  fs_btm->ref_idx[list][index]=fs_btm->ref_idx[list][index+1]=
							  fs_btm->ref_idx[list][index+4]=fs_btm->ref_idx[list][index+5]=frame->ref_idx[list][index_top];
						  fs_top->ref_pic[list][index]=fs_top->ref_pic[list][index+1]=
							  fs_top->ref_pic[list][index+4]=fs_top->ref_pic[list][index+5]=
							  fs_btm->ref_pic[list][index]=fs_btm->ref_pic[list][index+1]=
							  fs_btm->ref_pic[list][index+4]=fs_btm->ref_pic[list][index+5]=pDecGlobal->pCurSlice->listX[list][frame->ref_idx[list][index_top]];
					  }
					  if(frame->ref_idx[list][index_top+3]==-1)
					  {
						  fs_top->ref_idx[list][index+2]=fs_top->ref_idx[list][index+3]=
							  fs_top->ref_idx[list][index+6]=fs_top->ref_idx[list][index+7]=
							  fs_btm->ref_idx[list][index+2]=fs_btm->ref_idx[list][index+3]=
							  fs_btm->ref_idx[list][index+6]=fs_btm->ref_idx[list][index+7]=-1;
					  }
					  else
					  {
						  fs_top->ref_idx[list][index+2]=fs_top->ref_idx[list][index+3]=
							  fs_top->ref_idx[list][index+6]=fs_top->ref_idx[list][index+7]=
							  fs_btm->ref_idx[list][index+2]=fs_btm->ref_idx[list][index+3]=
							  fs_btm->ref_idx[list][index+6]=fs_btm->ref_idx[list][index+7]=frame->ref_idx[list][index_top+3];
						  fs_top->ref_pic[list][index+2]=fs_top->ref_pic[list][index+3]=
							  fs_top->ref_pic[list][index+6]=fs_top->ref_pic[list][index+7]=
							  fs_btm->ref_pic[list][index+2]=fs_btm->ref_pic[list][index+3]=
							  fs_btm->ref_pic[list][index+6]=fs_btm->ref_pic[list][index+7]=pDecGlobal->pCurSlice->listX[list][frame->ref_idx[list][index_top+3]];
					  }
					  if(frame->ref_idx[list][index_bottom]==-1)
					  {
						  fs_top->ref_idx[list][index+8]=fs_top->ref_idx[list][index+9]=
							  fs_top->ref_idx[list][index+12]=fs_top->ref_idx[list][index+13]=
							  fs_btm->ref_idx[list][index+8]=fs_btm->ref_idx[list][index+9]=
							  fs_btm->ref_idx[list][index+12]=fs_btm->ref_idx[list][index+13]=-1;
					  }
					  else
					  {
						  fs_top->ref_idx[list][index+8]=fs_top->ref_idx[list][index+9]=
							  fs_top->ref_idx[list][index+12]=fs_top->ref_idx[list][index+13]=
							  fs_btm->ref_idx[list][index+8]=fs_btm->ref_idx[list][index+9]=
							  fs_btm->ref_idx[list][index+12]=fs_btm->ref_idx[list][index+13]=frame->ref_idx[list][index_bottom];
						  fs_top->ref_pic[list][index+8]=fs_top->ref_pic[list][index+9]=
							  fs_top->ref_pic[list][index+12]=fs_top->ref_pic[list][index+13]=
							  fs_btm->ref_pic[list][index+8]=fs_btm->ref_pic[list][index+9]=
							  fs_btm->ref_pic[list][index+12]=fs_btm->ref_pic[list][index+13]=pDecGlobal->pCurSlice->listX[list][frame->ref_idx[list][index_bottom]];
					  }
					  if(frame->ref_idx[list][index_bottom+3]==-1)
					  {
						  fs_top->ref_idx[list][index+10]=fs_top->ref_idx[list][index+11]=
							  fs_top->ref_idx[list][index+14]=fs_top->ref_idx[list][index+15]=
							  fs_btm->ref_idx[list][index+10]=fs_btm->ref_idx[list][index+11]=
							  fs_btm->ref_idx[list][index+14]=fs_btm->ref_idx[list][index+15]=-1;
					  }
					  else
					  {
						  fs_top->ref_idx[list][index+10]=fs_top->ref_idx[list][index+11]=
							  fs_top->ref_idx[list][index+14]=fs_top->ref_idx[list][index+15]=
							  fs_btm->ref_idx[list][index+10]=fs_btm->ref_idx[list][index+11]=
							  fs_btm->ref_idx[list][index+14]=fs_btm->ref_idx[list][index+15]=frame->ref_idx[list][index_bottom+3];
						  fs_top->ref_pic[list][index+10]=fs_top->ref_pic[list][index+11]=
							  fs_top->ref_pic[list][index+14]=fs_top->ref_pic[list][index+15]=
							  fs_btm->ref_pic[list][index+10]=fs_btm->ref_pic[list][index+11]=
							  fs_btm->ref_pic[list][index+14]=fs_btm->ref_pic[list][index+15]=pDecGlobal->pCurSlice->listX[list][frame->ref_idx[list][index_bottom+3]];
					  }
				  }
			  }
			  else
			  {
					
				  VO_S32 index = (j*pDecGlobal->PicWidthInMbs+i)<<4;
				  VO_S32 index_top = (jj*pDecGlobal->PicWidthInMbs+i)<<4;
				  VO_S32 index_bottom = (jj4*pDecGlobal->PicWidthInMbs+i)<<4;
				  VO_S32 j0,i0;
				  memcpy(fs_top->mv[LIST_0]+index,frame->mv[LIST_0]+index_top,16*sizeof(VO_S32));
				  memcpy(fs_top->mv[LIST_1]+index,frame->mv[LIST_1]+index_top,16*sizeof(VO_S32));
				  memcpy(fs_top->ref_idx[LIST_0]+index,frame->ref_idx[LIST_0]+index_top,16);
				  memcpy(fs_top->ref_idx[LIST_1]+index,frame->ref_idx[LIST_1]+index_top,16);
				  for(j0=0;j0<4;j0++)
				  {
					  for(i0=0;i0<4;i0++)
					  {
						  if(fs_top->ref_idx[LIST_0][index+(j0<<2)+i0]>=0)
							  fs_top->ref_pic[LIST_0][index+(j0<<2)+i0] = pDecGlobal->pCurSlice->listX[2][fs_top->ref_idx[LIST_0][index+(j0<<2)+i0]];
						  else
							  fs_top->ref_pic[LIST_0][index+(j0<<2)+i0] = NULL;
						  if(fs_top->ref_idx[LIST_1][index+(j0<<2)+i0]>=0)
							  fs_top->ref_pic[LIST_1][index+(j0<<2)+i0] = pDecGlobal->pCurSlice->listX[3][fs_top->ref_idx[LIST_1][index+(j0<<2)+i0]];
						  else
							  fs_top->ref_pic[LIST_1][index+(j0<<2)+i0] = NULL;
					  }
				  }
				  memcpy(fs_btm->mv[LIST_0]+index,frame->mv[LIST_0]+index_bottom,16*sizeof(VO_S32));
				  memcpy(fs_btm->mv[LIST_1]+index,frame->mv[LIST_1]+index_bottom,16*sizeof(VO_S32));
				  memcpy(fs_btm->ref_idx[LIST_0]+index,frame->ref_idx[LIST_0]+index_bottom,16);
				  memcpy(fs_btm->ref_idx[LIST_1]+index,frame->ref_idx[LIST_1]+index_bottom,16);
				  for(j0=0;j0<4;j0++)
				  {
					  for(i0=0;i0<4;i0++)
					  {
						  if(fs_btm->ref_idx[LIST_0][index+(j0<<2)+i0]>=0)
							  fs_btm->ref_pic[LIST_0][index+(j0<<2)+i0] = pDecGlobal->pCurSlice->listX[4][fs_btm->ref_idx[LIST_0][index+(j0<<2)+i0]];
						  else
							  fs_btm->ref_pic[LIST_0][index+(j0<<2)+i0] = NULL;
						  if(fs_btm->ref_idx[LIST_1][index+(j0<<2)+i0]>=0)
							  fs_btm->ref_pic[LIST_1][index+(j0<<2)+i0] = pDecGlobal->pCurSlice->listX[5][fs_btm->ref_idx[LIST_1][index+(j0<<2)+i0]];
						  else
							  fs_btm->ref_pic[LIST_1][index+(j0<<2)+i0] = NULL;
					  }
				  }
			  }
		  }
	  }
#endif

	  if (0)
	  {
		  int i, j, k;
		  //printf("split\n");
		  //printf("top\n");
		  for (j = 0; j < pDecGlobal->FrameHeightInMbs>>1; ++j)
		  {
			  for (i = 0; i < pDecGlobal->PicWidthInMbs; ++i)
			  {
				  int mb_nr = j*pDecGlobal->PicWidthInMbs + i;
				  //printf("%d,%d: ", i, j);
				  for (k = 0; k < 16; ++k)
				  {
					  //printf("(%d,%d,%d,%d,%d,%d) ", fs_top->mv[LIST_0][(mb_nr<<4)+k], fs_top->mv[LIST_1][(mb_nr<<4)+k], 
						  //fs_top->ref_idx[LIST_0][(mb_nr<<4)+k], fs_top->ref_idx[LIST_1][(mb_nr<<4)+k],
						  //fs_top->ref_pic[LIST_0][(mb_nr<<4)+k]?fs_top->ref_pic[LIST_0][(mb_nr<<4)+k]-pDecGlobal->p_Dpb->storable_pic_buffer[0]:-1,
						  //fs_top->ref_pic[LIST_1][(mb_nr<<4)+k]?fs_top->ref_pic[LIST_1][(mb_nr<<4)+k]-pDecGlobal->p_Dpb->storable_pic_buffer[0]:-1);
				  }
				  //printf("\n");
			  }
		  }
		 // printf("btm\n");
		  for (j = 0; j < pDecGlobal->FrameHeightInMbs>>1; ++j)
		  {
			  for (i = 0; i < pDecGlobal->PicWidthInMbs; ++i)
			  {
				  int mb_nr = j*pDecGlobal->PicWidthInMbs + i;
				  //printf("%d,%d: ", i, j);
				  for (k = 0; k < 16; ++k)
				  {
					  //printf("(%d,%d,%d,%d,%d,%d) ", fs_btm->mv[LIST_0][(mb_nr<<4)+k], fs_btm->mv[LIST_1][(mb_nr<<4)+k],
						  //fs_btm->ref_idx[LIST_0][(mb_nr<<4)+k], fs_btm->ref_idx[LIST_1][(mb_nr<<4)+k],
						  //fs_btm->ref_pic[LIST_0][(mb_nr<<4)+k]?fs_btm->ref_pic[LIST_0][(mb_nr<<4)+k]-pDecGlobal->p_Dpb->storable_pic_buffer[0]:-1,
						  //fs_btm->ref_pic[LIST_1][(mb_nr<<4)+k]?fs_btm->ref_pic[LIST_1][(mb_nr<<4)+k]-pDecGlobal->p_Dpb->storable_pic_buffer[0]:-1);
				  }
				  //printf("\n");
			  }
		  }
	  }
  }
}

void dpb_combine_field_yuv(H264DEC_G *pDecGlobal, FrameStore *fs)
{
  int i,j;
  if (!fs->frame)
  {
    i = fs->top_field->cur_buf_index;
    //fs->frame = voMalloc(pDecGlobal,1*sizeof(StorablePicture));
    fs->frame = pDecGlobal->p_Dpb->storable_pic_buffer[0] + i;
    //fs->frame = fs->top_field;
    fs->frame->cur_buf_index = i;//-1;
  }
  fs->frame->structure = FRAME;  
  fs->frame->iCodingType = fs->top_field->iCodingType;
  fs->frame->imgY = fs->top_field->imgY;
  fs->frame->imgUV[0] = fs->top_field->imgUV[0];
  fs->frame->imgUV[1] = fs->top_field->imgUV[1];
  fs->frame->type = fs->top_field->type;
  fs->poc=fs->frame->poc =fs->frame->frame_poc = imin (fs->top_field->poc, fs->bottom_field->poc);
  fs->frame->Time = fs->top_field->Time>fs->bottom_field->Time?fs->top_field->Time:fs->bottom_field->Time;
  fs->frame->UserData = fs->top_field->UserData!=NULL ? fs->top_field->UserData:fs->bottom_field->UserData;
  if(fs->top_field->errFlag||fs->bottom_field->errFlag)
  {
    fs->frame->errFlag = fs->top_field->errFlag?fs->top_field->errFlag:fs->bottom_field->errFlag;
  }
  fs->bottom_field->frame_poc=fs->top_field->frame_poc=fs->poc;

  fs->bottom_field->top_poc=fs->frame->top_poc=fs->top_field->poc;
  fs->top_field->bottom_poc=fs->frame->bottom_poc=fs->bottom_field->poc;

  fs->frame->used_for_reference = (fs->top_field->used_for_reference && fs->bottom_field->used_for_reference );
  fs->frame->is_long_term = (fs->top_field->is_long_term && fs->bottom_field->is_long_term );

  if (fs->frame->is_long_term)
	  fs->frame->long_term_frame_idx = fs->long_term_frame_idx;

  fs->frame->top_field    = fs->top_field;
  fs->frame->bottom_field = fs->bottom_field;
  fs->frame->frame = fs->frame;

  //fs->frame->coded_frame = 0;

  fs->top_field->frame = fs->bottom_field->frame = fs->frame;
  fs->top_field->bottom_field = fs->bottom_field;
  fs->top_field->top_field = fs->top_field;
  fs->bottom_field->top_field = fs->top_field;
  fs->bottom_field->bottom_field = fs->bottom_field;

  //fs->frame->need_prepare_param = 1;
  
  //fs->frame->mbaff_flag = 0;
#if USE_FRAME_THREAD
  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
  {
    fs->frame->frame_set=fs->frame->top_set=fs->frame->bottom_set=1;
	fs->top_field->frame_set=fs->top_field->top_set=fs->top_field->bottom_set=1;
	fs->bottom_field->frame_set=fs->bottom_field->top_set=fs->bottom_field->bottom_set=1;
	fs->frame->nProgress[0]=-100;
	return;
  }
  else
    fs->frame->removable=1;
#endif

#if 1
  /*for (j = 0; j < pDecGlobal->FrameHeightInMbs; ++j)
  {
	  for (i = 0; i < pDecGlobal->PicWidthInMbs; ++i)
	  {
		  combine_mb(fs->frame, i, j, pDecGlobal->PicWidthInMbs);
	  }
  }*/
#elif 0
  for (j=0 ; j < (pDecGlobal->FrameHeightInMbs<<1) ; j++)
  {
	  VO_S32 k;
	  VO_S32 jj = (j<<1);
	  VO_S32 jj4 = jj + 1;
	  for (i=0 ; i< (pDecGlobal->PicWidthInMbs<<2) ; i++)
	  {
		  VO_S32 indexc = (j/4*pDecGlobal->PicWidthInMbs+i/4)*16+(j&3)*4+(i&3);
		  VO_S32 index0 = (jj/4*pDecGlobal->PicWidthInMbs+i/4)*16+(jj&3)*4+(i&3);
		  VO_S32 index1 = (jj4/4*pDecGlobal->PicWidthInMbs+i/4)*16+(jj4&3)*4+(i&3);
		  fs->frame->mv[LIST_0][index0] = fs->top_field->mv[LIST_0][indexc];
		  fs->frame->mv[LIST_1][index0] = fs->top_field->mv[LIST_1][indexc];

		  fs->frame->ref_idx[LIST_0][index0] = fs->top_field->ref_idx[LIST_0][indexc];
		  fs->frame->ref_idx[LIST_1][index0] = fs->top_field->ref_idx[LIST_1][indexc];

		  if(fs->top_field->listXsize[LIST_0] >0)
		  {
			  k = fs->top_field->ref_idx[LIST_0][indexc];
			  fs->frame->ref_pic[LIST_0][index0] = k>=0? fs->top_field->listX[LIST_0][k]: NULL;  
			  k = fs->top_field->ref_idx[LIST_1][indexc];
			  fs->frame->ref_pic[LIST_1][index0] = k>=0? fs->top_field->listX[LIST_1][k]: NULL;
		  }
		  else
		  {
			  k = fs->top_field->ref_idx[LIST_0][indexc];
        fs->frame->ref_pic[LIST_0][index0] = k>=0?fs->bottom_field->listX[LIST_0][k]: NULL;  
			  k = fs->top_field->ref_idx[LIST_1][indexc];
        fs->frame->ref_pic[LIST_1][index0] = k>=0?fs->bottom_field->listX[LIST_1][k]: NULL;
		  }

		  //! association with id already known for fields.
		  fs->frame->mv[LIST_0][index1] = fs->bottom_field->mv[LIST_0][indexc];
		  fs->frame->mv[LIST_1][index1] = fs->bottom_field->mv[LIST_1][indexc];

		  fs->frame->ref_idx[LIST_0][index1]  = fs->bottom_field->ref_idx[LIST_0][indexc];
		  fs->frame->ref_idx[LIST_1][index1]  = fs->bottom_field->ref_idx[LIST_1][indexc];

		  if(fs->bottom_field->listXsize[LIST_0]>0)
		  {
			  k = fs->bottom_field->ref_idx[LIST_0][indexc];
			  fs->frame->ref_pic[LIST_0][index1] = k>=0? fs->bottom_field->listX[LIST_0][k]: NULL;
			  k = fs->bottom_field->ref_idx[LIST_1][indexc];
			  fs->frame->ref_pic[LIST_1][index1] = k>=0? fs->bottom_field->listX[LIST_1][k]: NULL;
		  }
		  else
		  {
			  k = fs->bottom_field->ref_idx[LIST_0][indexc];
        fs->frame->ref_pic[LIST_0][index1] = k>=0? fs->top_field->listX[LIST_0][k]: NULL;
			  k = fs->bottom_field->ref_idx[LIST_1][indexc];       
        fs->frame->ref_pic[LIST_1][index1] = k>=0? fs->top_field->listX[LIST_1][k]: NULL;
		  }
		  //fs->top_field->mv_info[j][i].field_frame = 1;
		  //fs->bottom_field->mv_info[j][i].field_frame = 1;
	  }
  }

#else
  {
	  const VO_S32 stride = pDecGlobal->PicWidthInMbs<<4;
	  struct storable_picture ** ref_pic_t0;
	  struct storable_picture ** ref_pic_t1;
	  struct storable_picture ** ref_pic_b0;
	  struct storable_picture ** ref_pic_b1;
	  if (fs->top_field->listXsize[LIST_0] > 0)
	  {
		  ref_pic_t0 = fs->top_field->listX[LIST_0];
		  ref_pic_t1 = fs->top_field->listX[LIST_1];
	  }
	  else
	  {
		  ref_pic_t0 = pDecGlobal->pCurSlice->listX[LIST_0];
		  ref_pic_t1 = pDecGlobal->pCurSlice->listX[LIST_1];
	  }
	  if (fs->bottom_field->listXsize[LIST_0] > 0)
	  {
		  ref_pic_b0 = fs->bottom_field->listX[LIST_0];
		  ref_pic_b1 = fs->bottom_field->listX[LIST_1];
	  }
	  else
	  {
		  ref_pic_b0 = pDecGlobal->pCurSlice->listX[LIST_0];
		  ref_pic_b1 = pDecGlobal->pCurSlice->listX[LIST_1];
	  }
	  for (j=0 ; j < (pDecGlobal->FrameHeightInMbs<<1) ; j++)
	  {
		  VO_S32 offset_j = stride*(j>>1) + ((j&1)<<3);
		  VO_S32 *mv_f0 = fs->frame->mv[LIST_0] + offset_j;
		  VO_S32 *mv_f1 = fs->frame->mv[LIST_1] + offset_j;
		  VO_S8 *ref_idx_f0 = fs->frame->ref_idx[LIST_0] + offset_j;
		  VO_S8 *ref_idx_f1 = fs->frame->ref_idx[LIST_1] + offset_j;
		  struct storable_picture ** ref_pic_f0 = fs->frame->ref_pic[LIST_0] + offset_j;
		  struct storable_picture ** ref_pic_f1 = fs->frame->ref_pic[LIST_1] + offset_j;
		  VO_S32 *mv_t0;
		  VO_S32 *mv_t1;
		  VO_S8 *ref_idx_t0;
		  VO_S8 *ref_idx_t1;
		  VO_S32 *mv_b0;
		  VO_S32 *mv_b1;
		  VO_S8 *ref_idx_b0;
		  VO_S8 *ref_idx_b1;

		  offset_j = stride*(j>>2) + ((j&3)<<2);
		  mv_t0 = fs->top_field->mv[LIST_0] + offset_j;
		  mv_t1 = fs->top_field->mv[LIST_1] + offset_j;
		  ref_idx_t0 = fs->top_field->ref_idx[LIST_0] + offset_j;
		  ref_idx_t1 = fs->top_field->ref_idx[LIST_1] + offset_j;
		  mv_b0 = fs->bottom_field->mv[LIST_0] + offset_j;
		  mv_b1 = fs->bottom_field->mv[LIST_1] + offset_j;
		  ref_idx_b0 = fs->bottom_field->ref_idx[LIST_0] + offset_j;
		  ref_idx_b1 = fs->bottom_field->ref_idx[LIST_1] + offset_j;

		  for (i = 0; i < pDecGlobal->PicWidthInMbs; ++i)
		  {
			  VO_S32 offset_i = i<<4;
			  {
				  VO_S32 *mv_f0_i = mv_f0 + offset_i;
				  VO_S32 *mv_t0_i = mv_t0 + offset_i;
				  VO_S32 *mv_b0_i = mv_b0 + offset_i;
				  *(mv_f0_i++) = *(mv_t0_i++);
				  *(mv_f0_i++) = *(mv_t0_i++);
				  *(mv_f0_i++) = *(mv_t0_i++);
				  *(mv_f0_i++) = *mv_t0_i;
				  *(mv_f0_i++) = *(mv_b0_i++);
				  *(mv_f0_i++) = *(mv_b0_i++);
				  *(mv_f0_i++) = *(mv_b0_i++);
				  *mv_f0_i = *mv_b0_i;
			  }

			  {
				  VO_S32 *mv_f1_i = mv_f1 + offset_i;
				  VO_S32 *mv_t1_i = mv_t1 + offset_i;
				  VO_S32 *mv_b1_i = mv_b1 + offset_i;
				  *(mv_f1_i++) = *(mv_t1_i++);
				  *(mv_f1_i++) = *(mv_t1_i++);
				  *(mv_f1_i++) = *(mv_t1_i++);
				  *(mv_f1_i++) = *mv_t1_i;
				  *(mv_f1_i++) = *(mv_b1_i++);
				  *(mv_f1_i++) = *(mv_b1_i++);
				  *(mv_f1_i++) = *(mv_b1_i++);
				  *mv_f1_i = *mv_b1_i;
			  }

			  {
				  VO_S8 *ref_idx_f0_i = ref_idx_f0 + offset_i;
				  VO_S8 *ref_idx_t0_i = ref_idx_t0 + offset_i;
				  VO_S8 *ref_idx_b0_i = ref_idx_b0 + offset_i;
				  struct storable_picture ** ref_pic_f0_i = ref_pic_f0 + offset_i;
				  VO_S32 k;

				  k = *(ref_idx_f0_i++) = *(ref_idx_t0_i++);
				  *(ref_pic_f0_i++) = k>=0 ? ref_pic_t0[k] : NULL;

				  k = *(ref_idx_f0_i++) = *(ref_idx_t0_i++);
				  *(ref_pic_f0_i++) = k>=0 ? ref_pic_t0[k] : NULL;

				  k = *(ref_idx_f0_i++) = *(ref_idx_t0_i++);
				  *(ref_pic_f0_i++) = k>=0 ? ref_pic_t0[k] : NULL;

				  k = *(ref_idx_f0_i++) = *ref_idx_t0_i;
				  *(ref_pic_f0_i++) = k>=0 ? ref_pic_t0[k] : NULL;

				  k = *(ref_idx_f0_i++) = *(ref_idx_b0_i++);
				  *(ref_pic_f0_i++) = k>=0 ? ref_pic_b0[k] : NULL;

				  k = *(ref_idx_f0_i++) = *(ref_idx_b0_i++);
				  *(ref_pic_f0_i++) = k>=0 ? ref_pic_b0[k] : NULL;

				  k = *(ref_idx_f0_i++) = *(ref_idx_b0_i++);
				  *(ref_pic_f0_i++) = k>=0 ? ref_pic_b0[k] : NULL;

				  k = *ref_idx_f0_i = *ref_idx_b0_i;
				  *ref_pic_f0_i = k>=0 ? ref_pic_b0[k] : NULL;
			  }

			  {
				  VO_S8 *ref_idx_f1_i = ref_idx_f1 + offset_i;
				  VO_S8 *ref_idx_t1_i = ref_idx_t1 + offset_i;
				  VO_S8 *ref_idx_b1_i = ref_idx_b1 + offset_i;
				  struct storable_picture ** ref_pic_f1_i = ref_pic_f1 + offset_i;
				  VO_S32 k;

				  k = *(ref_idx_f1_i++) = *(ref_idx_t1_i++);
				  *(ref_pic_f1_i++) = k>=0 ? ref_pic_t1[k] : NULL;

				  k = *(ref_idx_f1_i++) = *(ref_idx_t1_i++);
				  *(ref_pic_f1_i++) = k>=0 ? ref_pic_t1[k] : NULL;

				  k = *(ref_idx_f1_i++) = *(ref_idx_t1_i++);
				  *(ref_pic_f1_i++) = k>=0 ? ref_pic_t1[k] : NULL;

				  k = *(ref_idx_f1_i++) = *ref_idx_t1_i;
				  *(ref_pic_f1_i++) = k>=0 ? ref_pic_t1[k] : NULL;

				  k = *(ref_idx_f1_i++) = *(ref_idx_b1_i++);
				  *(ref_pic_f1_i++) = k>=0 ? ref_pic_b1[k] : NULL;

				  k = *(ref_idx_f1_i++) = *(ref_idx_b1_i++);
				  *(ref_pic_f1_i++) = k>=0 ? ref_pic_b1[k] : NULL;

				  k = *(ref_idx_f1_i++) = *(ref_idx_b1_i++);
				  *(ref_pic_f1_i++) = k>=0 ? ref_pic_b1[k] : NULL;

				  k = *ref_idx_f1_i = *ref_idx_b1_i;
				  *ref_pic_f1_i = k>=0 ? ref_pic_b1[k] : NULL;
			  }
		  }
	  }
  }
#endif

}

#if 1 //USE_FRAME_THREAD

/*void inline split_4x4(struct storable_picture *field, struct storable_picture *frame, VO_S32 idx0, VO_S32 idx1, VO_S32 list_idx)
{
	VO_S32 list;
	VO_S32 k;
	for (list = 0; list < 2; ++list)
	{
		field->mv[list][idx0] = frame->mv[list][idx1];
		k = field->ref_idx[list][idx0] = frame->ref_idx[list][idx1];
		if (k >= 0)
		{
			field->ref_pic[list][idx0] = frame->listX[list_idx + list][k];
		}
		else
		{
			field->ref_pic[list][idx0] = NULL;
		}
	}
}*/


/*void inline combine_4x4(struct storable_picture *frame, struct storable_picture *field0, struct storable_picture *field1, VO_S32 idx0, VO_S32 idx1)
{
	VO_S32 k;
	struct storable_picture ** ref_pic0;
	struct storable_picture ** ref_pic1;
	assert(frame->structure == FRAME);

	if (field0->listXsize[LIST_0] > 0)
	{
		ref_pic0 = field0->ref_pic[LIST_0];
		ref_pic1 = field0->ref_pic[LIST_1];
	}
	else
	{
		ref_pic0 = field1->ref_pic[LIST_0];
		ref_pic1 = field1->ref_pic[LIST_1];
	}
	frame->mv[0][idx0] = field0->mv[0][idx1];
	frame->mv[1][idx0] = field0->mv[1][idx1];

	k = frame->ref_idx[0][idx0] = field0->ref_idx[0][idx1];
	frame->ref_pic[0][idx0] = k>=0 ? ref_pic0[k] : NULL;

	k = frame->ref_idx[1][idx0] = field0->ref_idx[1][idx1];
	frame->ref_pic[1][idx0] = k>=0 ? ref_pic1[k] : NULL;
}*/

/*void combine_mb(struct storable_picture *frame, VO_S32 mb_x, VO_S32 mb_y, VO_S32 mbWidth)
{
	VO_S32 i;
	struct storable_picture *top = frame->top_field;
	struct storable_picture *btm = frame->bottom_field;
	VO_S32 idx_0 = (mb_y*mbWidth+mb_x)<<4;
	VO_S32 idx_1 = (((mb_y>>1)*mbWidth + mb_x)<<4) + ((mb_y&1)<<3);

	assert(frame->structure == FRAME);
	for (i = 2; i > 0; --i)
	{
		combine_4x4(frame, top, btm, idx_0++, idx_1);
		combine_4x4(frame, top, btm, idx_0++, idx_1+1);
		combine_4x4(frame, top, btm, idx_0++, idx_1+2);
		combine_4x4(frame, top, btm, idx_0++, idx_1+3);
		combine_4x4(frame, btm, top, idx_0++, idx_1++);
		combine_4x4(frame, btm, top, idx_0++, idx_1++);
		combine_4x4(frame, btm, top, idx_0++, idx_1++);
		combine_4x4(frame, btm, top, idx_0++, idx_1++);
	}
}*/

#endif

void alloc_ref_pic_list_reordering_buffer(H264DEC_G *pDecGlobal, Slice *currSlice)
{
  //VideoParameters *p_Vid = currSlice->p_Vid;
  int size = 18;

  //if (pDecGlobal->type!=I_SLICE && pDecGlobal->type!=SI_SLICE)
  {
    if ((currSlice->reordering_of_pic_nums_idc[LIST_0] = voMalloc(pDecGlobal,size*sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: reordering_of_pic_nums_idc_l0");
    if ((currSlice->abs_diff_pic_num_minus1[LIST_0] = voMalloc(pDecGlobal,size*sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: abs_diff_pic_num_minus1_l0");
    if ((currSlice->long_term_pic_idx[LIST_0] = voMalloc(pDecGlobal,size*sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: long_term_pic_idx_l0");
#if (MVC_EXTENSION_ENABLE)
    if ((currSlice->abs_diff_view_idx_minus1[LIST_0] = voMalloc(size,sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: abs_diff_view_idx_minus1_l0");
#endif
  }


  size = 18;

  //if (pDecGlobal->type==B_SLICE)
  {
    if ((currSlice->reordering_of_pic_nums_idc[LIST_1] = voMalloc(pDecGlobal,size*sizeof(int)))==NULL) 
      no_mem_exit("alloc_ref_pic_list_reordering_buffer: reordering_of_pic_nums_idc_l1");
    if ((currSlice->abs_diff_pic_num_minus1[LIST_1] = voMalloc(pDecGlobal,size*sizeof(int)))==NULL) 
      no_mem_exit("alloc_ref_pic_list_reordering_buffer: abs_diff_pic_num_minus1_l1");
    if ((currSlice->long_term_pic_idx[LIST_1] = voMalloc(pDecGlobal,size*sizeof(int)))==NULL) 
      no_mem_exit("alloc_ref_pic_list_reordering_buffer: long_term_pic_idx_l1");
#if (MVC_EXTENSION_ENABLE)
    if ((currSlice->abs_diff_view_idx_minus1[LIST_1] = voMalloc(size,sizeof(int)))==NULL) no_mem_exit("alloc_ref_pic_list_reordering_buffer: abs_diff_view_idx_minus1_l1");
#endif
  }

}


void free_ref_pic_list_reordering_buffer(H264DEC_G *pDecGlobal, Slice *currSlice)
{
  if (currSlice->reordering_of_pic_nums_idc[LIST_0])
    voFree(pDecGlobal,currSlice->reordering_of_pic_nums_idc[LIST_0]);
  if (currSlice->abs_diff_pic_num_minus1[LIST_0])
    voFree(pDecGlobal,currSlice->abs_diff_pic_num_minus1[LIST_0]);
  if (currSlice->long_term_pic_idx[LIST_0])
    voFree(pDecGlobal,currSlice->long_term_pic_idx[LIST_0]);

  currSlice->reordering_of_pic_nums_idc[LIST_0] = NULL;
  currSlice->abs_diff_pic_num_minus1[LIST_0] = NULL;
  currSlice->long_term_pic_idx[LIST_0] = NULL;

  if (currSlice->reordering_of_pic_nums_idc[LIST_1])
    voFree(pDecGlobal,currSlice->reordering_of_pic_nums_idc[LIST_1]);
  if (currSlice->abs_diff_pic_num_minus1[LIST_1])
    voFree(pDecGlobal,currSlice->abs_diff_pic_num_minus1[LIST_1]);
  if (currSlice->long_term_pic_idx[LIST_1])
    voFree(pDecGlobal,currSlice->long_term_pic_idx[LIST_1]);

  currSlice->reordering_of_pic_nums_idc[LIST_1] = NULL;
  currSlice->abs_diff_pic_num_minus1[LIST_1] = NULL;
  currSlice->long_term_pic_idx[LIST_1] = NULL;

#if (MVC_EXTENSION_ENABLE)
  if (currSlice->abs_diff_view_idx_minus1[LIST_0])
    voFree(pDecGlobal,currSlice->abs_diff_view_idx_minus1[LIST_0]);
  currSlice->abs_diff_view_idx_minus1[LIST_0] = NULL;
  if (currSlice->abs_diff_view_idx_minus1[LIST_1])
    voFree(pDecGlobal,currSlice->abs_diff_view_idx_minus1[LIST_1]);
  currSlice->abs_diff_view_idx_minus1[LIST_1] = NULL;
#endif
}

VO_S32 compute_colocated (H264DEC_G *pDecGlobal,Slice *currSlice, StorablePicture **listX[6])
{
  int i,j;

  //VideoParameters *p_Vid = currSlice->p_Vid;

  if (currSlice->direct_spatial_mv_pred_flag == 0)
  {    
    for (j = 0; j < 2 + (currSlice->mb_aff_frame_flag * 4); j += 2)
    {
      for (i=0; i<currSlice->listXsize[currSlice->slice_number][j];i++)
      {
        int prescale, iTRb, iTRp;
		StorablePicture *list = listX[LIST_0 + j][i];
		if (!list)
		{
			VOH264ERROR(VO_H264_ERR_NUMREFFRAME);
		}
        if (j==0)
        {
          iTRb = iClip3( -128, 127, pDecGlobal->dec_picture->poc - list->poc );
        }
        else if (j == 2)
        {
          iTRb = iClip3( -128, 127, pDecGlobal->dec_picture->top_poc - list->poc );
        }
        else
        {
          iTRb = iClip3( -128, 127, pDecGlobal->dec_picture->bottom_poc - list->poc );
        }

        iTRp = iClip3( -128, 127,  listX[LIST_1 + j][0]->poc - list->poc);

        if (iTRp!=0)
        {
          prescale = ( 16384 + iabs( iTRp / 2 ) ) / iTRp;
          currSlice->mvscale[j][i] = iClip3( -1024, 1023, ( iTRb * prescale + 32 ) >> 6 ) ;
        }
        else
        {
          currSlice->mvscale[j][i] = 9999;
        }
      }
    }
  }
  return 0;
}

