
//#include <sys/stat.h>

#include "global.h"
//#include "annexb.h"
#include "image.h"
#include "memalloc.h"
//#include "mc_prediction.h"
#include "mbuffer.h"
//#include "leaky_bucket.h"
//#include "fmo.h"
#include "output.h"
#include "cabac.h"
#include "parset.h"
#include "sei.h"
//#include "erc_api.h"
#include "quant.h"
#include "block.h"
#include "nalu.h"
//#include "img_io.h"
//#include "loopfilter.h"
//#include "mv_prediction.h"
#include "h264decoder.h"
#include "voLog.h"

#define LOGFILE     "log.dec"
#define DATADECFILE "dataDec.txt"
#define TRACEFILE   "trace_dec.txt"

#ifndef DISABLE_LICENSE
#include "voCheck.h"
#endif
// Prototypes of static functions
static void init        (H264DEC_G *pDecGlobal);
static void free_slice  (H264DEC_G *pDecGlobal, Slice *currSlice);

#if defined(WIN32)||defined(_WIN32_WCE)
void* hWin32Module = NULL;
#endif//

#if defined(VOSSSE3)
extern void perform_mc_init(void);
#endif



VO_U32 VO_API  voCheckLibInit2 (VO_PTR * phCheck, VO_U32 nID, VO_U32 nFlag, VO_HANDLE hInst,VO_LIB_OPERATOR* op)
{
	return 0;
}
VO_U32 VO_API  voCheckLibCheckVideo2 (VO_PTR hCheck, VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_FORMAT * pOutInfo)
{
	return 0;
}
VO_U32 VO_API  voCheckLibResetVideo2 (VO_PTR hCheck, VO_VIDEO_BUFFER * pOutBuffer)
{
	return 0;
}
VO_U32 VO_API  voCheckLibUninit2 (VO_PTR hCheck)
{
	return 0;
}

//VO_U32 VO_API  voCheckLibInit (void** phCheck, VO_U32 nID, VO_U32 nFlag, void* hInst,VO_LIB_OPERATOR*);
//VO_U32 VO_API  voCheckLibCheckVideo (void* hCheck, void* pOutBuffer, void* pOutInfo);
//VO_U32 VO_API  voCheckLibResetVideo (void* hCheck, void* pOutBuffer);
//VO_U32 VO_API  voCheckLibUninit (void* hCheck);



void error(char *text, int code)
{
  fprintf(stderr, "%s\n", text);
#if (MVC_EXTENSION_ENABLE)
  flush_dpb(pDecGlobal,p_Dec->pDecGlobal->p_Dpb, -1);
#else
//  flush_dpb(pDecGlobal,p_Dec->p_Vid->p_Dpb);
#endif
  exit(code);
}

static void reset_dpb(  DecodedPictureBuffer *p_Dpb )
{
  //p_Dpb->p_Vid = p_Vid;
  p_Dpb->init_done = 0;
}



static void alloc_golbal_params( H264DEC_G **pDecGlobal, VO_CODEC_INIT_USERDATA * pUserData)
{
//   int i;
  if ((*pDecGlobal	=  (H264DEC_G *) calloc(1,sizeof(H264DEC_G)))==NULL) 
    no_mem_exit("alloc_golbal_params: pDecGlobal");

  if(pUserData&&pUserData->memData)
  {
	  VO_MEM_OPERATOR* oper=(VO_MEM_OPERATOR*)pUserData->memData;
	  if((*pDecGlobal)->customMem2==NULL)
		  (*pDecGlobal)->customMem2 = (VOH264MEMOP*)calloc(1, sizeof(VOH264MEMOP));
	  (*pDecGlobal)->customMem2->Alloc4 = (int  (VOAPI * ) (int uID, VOMEM_INFO * pMemInfo))oper->Alloc;
	  (*pDecGlobal)->customMem2->Free4 = (int (VOAPI *) (int uID, void* pBuff))oper->Free;
	  (*pDecGlobal)->customMem2->Set	  = (int (VOAPI * ) (int uID, void* pBuff, unsigned char uValue, unsigned int uSize))oper->Set;
	  (*pDecGlobal)->customMem2->Copy	  = (int (VOAPI * ) (int uID, void* pDest, void* pSource, unsigned int uSize))oper->Copy;
	  (*pDecGlobal)->customMem2->Check = (int (VOAPI * ) (int uID, void* pBuffer, unsigned int uSize))oper->Check;
  }
  
  if (((*pDecGlobal)->p_Dpb =  (DecodedPictureBuffer*)voMalloc(*pDecGlobal,sizeof(DecodedPictureBuffer)))==NULL) 
    no_mem_exit("alloc_video_params: pDecGlobal->p_Dpb");
  reset_dpb((*pDecGlobal)->p_Dpb);
  if (((*pDecGlobal)->SeqParSet =  (seq_parameter_set_rbsp_t*)voMalloc(*pDecGlobal,MAXSPS*sizeof(seq_parameter_set_rbsp_t)))==NULL) 
	  no_mem_exit("alloc_video_params: pDecGlobal->SeqParSet");
  if (((*pDecGlobal)->PicParSet =  (pic_parameter_set_rbsp_t*)voMalloc(*pDecGlobal,MAXPPS*sizeof(pic_parameter_set_rbsp_t)))==NULL) 
	  no_mem_exit("alloc_video_params: pDecGlobal->PicParSet");
  if (((*pDecGlobal)->dec_ref_pic_marking_buffer =  (DecRefPicMarking_t*)voMalloc(*pDecGlobal,MAX_REFERENCE_FRAMES*sizeof(DecRefPicMarking_t)))==NULL) 
	  no_mem_exit("alloc_video_params: pDecGlobal->dec_ref_pic_marking_buffer");

  (*pDecGlobal)->p_output = (StorablePicture**)voMalloc(*pDecGlobal,32*sizeof(StorablePicture*));
  (*pDecGlobal)->output_size = 0;
  (*pDecGlobal)->global_init_done = 0;
  (*pDecGlobal)->spsNum = -1;
//   (*pDecGlobal)->old_pps_id2 = -1;
#if 0
  (*pDecGlobal)->pDecOuputPic = (DecodedPicList *)voMalloc(*pDecGlobal,32*sizeof(DecodedPicList));
#endif
}
void alloc_private_params( H264DEC_G *pDecGlobal)
{
	pDecGlobal->pCurSlice = malloc_slice(pDecGlobal);
	pDecGlobal->nalu = AllocNALU(pDecGlobal, MAX_CODED_FRAME_SIZE);
}

static int alloc_decoder(DecoderParams **p_Dec, VO_CODEC_INIT_USERDATA * pUserData)
{
  if ((*p_Dec = (DecoderParams *) calloc(1, sizeof(DecoderParams)))==NULL) 
  {
    fprintf(stderr, "alloc_decoder: p_Dec\n");
    return -1;
  }

  //alloc_video_params(&((*p_Dec)->p_Vid));
  alloc_golbal_params(&((*p_Dec)->pDecGlobal), pUserData);
  alloc_private_params((*p_Dec)->pDecGlobal);

#if COMBINE_FRAME
	(*p_Dec)->inBuf = (VO_U8 *)voMalloc((*p_Dec)->pDecGlobal, MAX_INPUT_SIZE*2);
	if ((*p_Dec)->inBuf == NULL)
	{
		no_mem_exit("alloc_decoder: inBuf");
	}
	(*p_Dec)->curPos = (*p_Dec)->inBuf;
	(*p_Dec)->lastPos = (*p_Dec)->inBuf;
	(*p_Dec)->leftBytes = 0;
	(*p_Dec)->usedBytes = 0;
	(*p_Dec)->bFindFirstSlice = 0;
	(*p_Dec)->bHasNext = 0;
	(*p_Dec)->bNeedFlush = 0;	
#endif
  (*p_Dec)->p_trace = NULL;
  (*p_Dec)->bufferSize = 0;
  (*p_Dec)->bitcounter = 0;
  return 0;
}

void free_private_param(H264DEC_G *pDecGlobal)
{
	if (pDecGlobal != NULL)
	{
		if(pDecGlobal->pCurSlice)
		{
			free_slice(pDecGlobal, pDecGlobal->pCurSlice);
			pDecGlobal->pCurSlice = NULL;
		}
		if(pDecGlobal->nalu)
		{
			FreeNALU(pDecGlobal, pDecGlobal->nalu);
			pDecGlobal->nalu=NULL;
		}
	}
}

static void free_img( H264DEC_G *pDecGlobal)
{
//   int i;
  //free_mem3Dint(p_Vid->fcf    ); 
  if (pDecGlobal != NULL)
  {

    if (pDecGlobal->p_Dpb != NULL)
    {
      voFree (pDecGlobal,pDecGlobal->p_Dpb);
      pDecGlobal->p_Dpb = NULL;
    }

	if (pDecGlobal->SeqParSet != NULL)
	{
		voFree (pDecGlobal,pDecGlobal->SeqParSet);
		pDecGlobal->SeqParSet = NULL;
	}
	if (pDecGlobal->PicParSet != NULL)
	{
		voFree (pDecGlobal,pDecGlobal->PicParSet);
		pDecGlobal->PicParSet = NULL;
	}
	if (pDecGlobal->dec_ref_pic_marking_buffer != NULL)
	{
		voFree (pDecGlobal,pDecGlobal->dec_ref_pic_marking_buffer);
		pDecGlobal->dec_ref_pic_marking_buffer = NULL;
	}

    //voFree memory;
#if 0
    if(pDecGlobal->pDecOuputPic)
	  voFree(pDecGlobal,pDecGlobal->pDecOuputPic);
#endif
    if(pDecGlobal->p_output)
	  voFree(pDecGlobal,pDecGlobal->p_output);

    //voFree (pDecGlobal,p_Vid);
    //p_Vid = NULL;
    if(pDecGlobal->customMem2)
		free(pDecGlobal->customMem2);
	free(pDecGlobal);
	pDecGlobal = NULL;
  }
}


static void init(H264DEC_G *pDecGlobal)  //!< video parameters
{
  pDecGlobal->recovery_point = 0;
  pDecGlobal->recovery_point_found = 0;
  pDecGlobal->recovery_poc = 0x7fffffff; /* set to a max value */


  pDecGlobal->number = 0;
  pDecGlobal->enbaleSEI = 1;
  pDecGlobal->type = I_SLICE;


#if (CAL_DEBLOCK)
  pDecGlobal->tot_deblock= 0;
#endif
#if (CAL_PARSER)
  pDecGlobal->tot_parser= 0;
#endif
#if (CAL_SLICE)
  pDecGlobal->tot_slice= 0;
#endif
#if (CAL_HEADER)
  pDecGlobal->tot_header= 0;
#endif

#if (CAL_MC)
  pDecGlobal->tot_mc= 0;
#endif
 
  pDecGlobal->dec_picture = NULL;



#if (MVC_EXTENSION_ENABLE)
  pDecGlobal->last_pic_width_in_mbs_minus1 = 0;
  pDecGlobal->last_pic_height_in_map_units_minus1 = 0;
  pDecGlobal->last_max_dec_frame_buffering = 0;
#endif

//   pDecGlobal->newframe = 0;
  pDecGlobal->previous_frame_num = 0;

  pDecGlobal->iLumaPadX = MCBUF_LUMA_PAD_X;
  pDecGlobal->iLumaPadY = MCBUF_LUMA_PAD_Y;
  pDecGlobal->iChromaPadX = MCBUF_CHROMA_PAD_X;
  pDecGlobal->iChromaPadY = MCBUF_CHROMA_PAD_Y;

  pDecGlobal->old_pps_id = 256;      //pps_id range from 0 to 255,so set to 256

  pDecGlobal->initDone = 1;
  pDecGlobal->bThumbnailMode = 0;
  pDecGlobal->bDirectOutputMode = 0;
}

DataPartition *AllocPartition(H264DEC_G *pDecGlobal, int n)
{
  DataPartition *partArr, *dataPart;
  int i;

  partArr = (DataPartition *) voMalloc(pDecGlobal,n*sizeof(DataPartition));
  if (partArr == NULL)
  {
    //snprintf(errortext, ET_SIZE, "AllocPartition: Memory allocation for Data Partition failed");
    error("AllocPartition: Memory allocation for Data Partition failed", 100);
  }

  for (i=0; i<n; ++i) // loop over all data partitions
  {
    dataPart = &(partArr[i]);
    dataPart->bitstream = (Bitstream *) voMalloc(pDecGlobal,sizeof(Bitstream));
    if (dataPart->bitstream == NULL)
    {
      //snprintf(errortext, ET_SIZE, "AllocPartition: Memory allocation for Bitstream failed");
      error("AllocPartition: Memory allocation for Bitstream failed", 100);
    }
  }
  return partArr;
}

void FreePartition (H264DEC_G *pDecGlobal, DataPartition *dp, int n)
{
  int i;

  assert (dp != NULL);
  assert (dp->bitstream != NULL);
//  assert (dp->bitstream->streamBuffer != NULL);
  for (i=0; i<n; ++i)
  {
    //voFree (pDecGlobal,dp[i].bitstream->streamBuffer);
    voFree (pDecGlobal,dp[i].bitstream);
  }
  voFree (pDecGlobal,dp);
}


Slice *malloc_slice(H264DEC_G *pDecGlobal)
{
  int i, j,k, memory_size = 0;
  Slice *currSlice;

  currSlice = (Slice *) voMalloc(pDecGlobal,sizeof(Slice));
  if ( currSlice  == NULL)
  {
    //snprintf(errortext, ET_SIZE, "Memory allocation for Slice datastruct in NAL-mode %d failed", p_Inp->FileFormat);
    error("Memory allocation for Slice datastruct in NAL-mode %d failed",100);
  }

  currSlice->partArr = AllocPartition(pDecGlobal, 1);
  memory_size += get_mem3D(pDecGlobal, &(currSlice->weighted_flag), 2, MAX_REFERENCE_PICTURES, 3);
  memory_size += get_mem3Dint(pDecGlobal, &(currSlice->wp_weight), 2, MAX_REFERENCE_PICTURES, 3);
  memory_size += get_mem3Dint(pDecGlobal, &(currSlice->wp_offset), 6, MAX_REFERENCE_PICTURES, 3);
  memory_size += get_mem4Dint(pDecGlobal, &(currSlice->wbp_weight), 6, MAX_REFERENCE_PICTURES, MAX_REFERENCE_PICTURES, 3);
  alloc_ref_pic_list_reordering_buffer(pDecGlobal, currSlice);

#if (MVC_EXTENSION_ENABLE)
  currSlice->view_id = MVC_INIT_VIEW_ID;
  currSlice->inter_view_flag = 0;
  currSlice->anchor_pic_flag = 0;
#endif
  currSlice->fs_ref = voMalloc(pDecGlobal,32*sizeof (FrameStore));
  if (NULL==currSlice->fs_ref)
    no_mem_exit("malloc_slice: currSlice->fs_ref");
  currSlice->ref_frames_in_buffer = 0;
  currSlice->fs_ltref = voMalloc(pDecGlobal,32*sizeof (FrameStore));
  if (NULL==currSlice->fs_ltref)
    no_mem_exit("malloc_slice: currSlice->fs_ltref");
  currSlice->ltref_frames_in_buffer = 0;

  currSlice->cur_storable_pic = (StorablePicture*)voMalloc(pDecGlobal,(16+32*pDecGlobal->interlace)*sizeof(StorablePicture));  
  if (NULL==currSlice->cur_storable_pic)
	  no_mem_exit("malloc_slice: currSlice->cur_storable_pic");
  for (j=0;j<32;j++)
    for (i = 0; i < 6; i++)
    {
      currSlice->listX[j][i] = voMalloc(pDecGlobal,MAX_LIST_SIZE*sizeof (StorablePicture*)); // +1 for reordering
      if (NULL==currSlice->listX[j][i])
        no_mem_exit("malloc_slice: currSlice->listX[i]");
    }
  for (k = 0; k < 32; k++)
    for (j = 0; j < 6; j++)
    {
      for (i = 0; i < MAX_LIST_SIZE; i++)
      {
        currSlice->listX[k][j][i] = NULL;
      }
      currSlice->listXsize[k][j]=0;
    }

  return currSlice;
}

static void free_slice(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  int i,j;

  //voFree(pDecGlobal,currSlice->mb_pred[0]);
  free_mem3D(pDecGlobal, currSlice->weighted_flag);
  free_mem3Dint(pDecGlobal, currSlice->wp_weight );
  free_mem3Dint(pDecGlobal, currSlice->wp_offset );
  free_mem4Dint(pDecGlobal, currSlice->wbp_weight);

  FreePartition (pDecGlobal, currSlice->partArr, 1);
  free_ref_pic_list_reordering_buffer(pDecGlobal, currSlice);

  if (currSlice->fs_ref)
  {
	  voFree(pDecGlobal, currSlice->fs_ref);
  }
  if (currSlice->fs_ltref)
  {
	  voFree(pDecGlobal, currSlice->fs_ltref);
  }
  if (currSlice->cur_storable_pic)
  {
	  voFree(pDecGlobal, currSlice->cur_storable_pic);
  }
  for (j=0; j<32; j++)
    for (i=0; i<6; i++)
    {
      if (currSlice->listX[j][i])
      {
        voFree (pDecGlobal,currSlice->listX[j][i]);
        currSlice->listX[j][i] = NULL;
      }
    }


  voFree(pDecGlobal,currSlice);
  currSlice = NULL;
}

int init_global_buffers(H264DEC_G *pDecGlobal)
{
  int memory_size=0;
  int i;

  if (pDecGlobal->global_init_done)
  {
    free_global_buffers(pDecGlobal);
  }

  if(((pDecGlobal->qp_frame_buffer) = (VO_U8 *) voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*sizeof(VO_U8))) == NULL)
      no_mem_exit("init_global_buffers: pDecGlobal->qp_frame_buffer");
  if(((pDecGlobal->qpc_frame_buffer) = (VO_U8 *) voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*2*sizeof(VO_U8))) == NULL)
      no_mem_exit("init_global_buffers: pDecGlobal->qpc_frame_buffer");
  //if(((pDecGlobal->mb_type0_frame_buffer) = (VO_S32 *) voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*sizeof(VO_S32))) == NULL)
  //    no_mem_exit("init_global_buffers: pDecGlobal->mb_type0_frame_buffer");
  if(((pDecGlobal->cbp_frame_buffer) = (VO_S32 *) voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*sizeof(VO_S32))) == NULL)
      no_mem_exit("init_global_buffers: pDecGlobal->cbp_frame_buffer");

  if(((pDecGlobal->b8direct_frame_buffer) = (VO_U8 *) voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*4*sizeof(VO_U8))) == NULL)
      no_mem_exit("init_global_buffers: pDecGlobal->b8direct_frame_buffer");
  if(((pDecGlobal->c_ipred_frame_buffer) = (VO_S8 *) voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*sizeof(VO_S8))) == NULL)
	  no_mem_exit("init_global_buffers: pDecGlobal->c_ipred_frame_buffer");
  if(((pDecGlobal->mvd_frame_buffer) = (VO_S32*) voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*16*sizeof(int))) == NULL)
      no_mem_exit("init_global_buffers: pDecGlobal->mvd_frame_buffer");
// #if(USE_MULTI_THREAD || ENABLE_DEBLOCK_MB)
  if(((pDecGlobal->intra_pix_buffer) = (VO_U8 *) voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*32*sizeof(VO_U8))) == NULL)
	  no_mem_exit("init_global_buffers: pDecGlobal->intra_pix_buffer");
// #endif
  if(((pDecGlobal->cof_yuv_row) = (VO_S16 *) voMalloc(pDecGlobal,MAX_THREAD_NUM*MB_BLOCK_SIZE*MB_BLOCK_SIZE*pDecGlobal->PicWidthInMbs*3*sizeof(VO_S16))) == NULL)
	  no_mem_exit("init_global_buffers: pDecGlobal->cof_yuv_row");
  if(((pDecGlobal->edge_buffer_alloc) = (VO_U8*) voMalloc(pDecGlobal,(pDecGlobal->width+64)*2*22*2)) == NULL)
	  no_mem_exit("init_global_buffers: pDecGlobal->edge_buffer_alloc");
  pDecGlobal->edge_buffer = pDecGlobal->edge_buffer_alloc + (pDecGlobal->width+64)*2*22;
  if(((pDecGlobal->edge_uvbuffer_alloc) = (VO_U8*) voMalloc(pDecGlobal,(pDecGlobal->width_cr+64)*2*10*4)) == NULL)
	  no_mem_exit("init_global_buffers: pDecGlobal->edge_uvbuffer_alloc");
  pDecGlobal->edge_ubuffer = pDecGlobal->edge_uvbuffer_alloc + (pDecGlobal->width_cr+64)*2*10;
  pDecGlobal->edge_vbuffer = pDecGlobal->edge_uvbuffer_alloc + (pDecGlobal->width_cr+64)*2*10*3;

#if defined(VOSSSE3)
   if(pDecGlobal->interlace)
   	   pDecGlobal->tmp_block = (VO_U8*) voMalloc(pDecGlobal, pDecGlobal->iLumaStride * 2 * MB_BLOCK_SIZE);
   else	
       pDecGlobal->tmp_block = (VO_U8*) voMalloc(pDecGlobal, pDecGlobal->iLumaStride * MB_BLOCK_SIZE);

   pDecGlobal->is_sse3 = 1;
#endif

  //memory_size += get_mem2Dint(&PicPos,p_Vid->FrameSizeInMbs + 1,2);  //! Helper array to access macroblock positions. We add 1 to also consider last MB.
  if(((pDecGlobal->PicPos) = (BlockPos*) voMalloc(pDecGlobal,(pDecGlobal->FrameSizeInMbs + 1)*sizeof(BlockPos))) == NULL)
    no_mem_exit("init_global_buffers: PicPos");


  for (i = 0; i < (int) pDecGlobal->FrameSizeInMbs + 1;++i)
  {
    pDecGlobal->PicPos[i].x = (short) (i % pDecGlobal->PicWidthInMbs);
    pDecGlobal->PicPos[i].y = (short) (i / pDecGlobal->PicWidthInMbs);
  }

  pDecGlobal->ipredmode_frame = voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*16*sizeof(VO_S8));

  pDecGlobal->nz_coeff_frame = voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs*24);
  pDecGlobal->slice_number_frame = voMalloc(pDecGlobal,pDecGlobal->FrameSizeInMbs);

  init_qp_process(pDecGlobal);

  pDecGlobal->global_init_done = 1;

  return (memory_size);
}


void free_global_buffers(H264DEC_G *pDecGlobal)
{  
  if (pDecGlobal->nz_coeff_frame != NULL)
  {
    voFree(pDecGlobal,pDecGlobal->nz_coeff_frame);
	pDecGlobal->nz_coeff_frame = NULL;
  }
  if (pDecGlobal->slice_number_frame != NULL)
  {
    voFree(pDecGlobal,pDecGlobal->slice_number_frame);
	pDecGlobal->slice_number_frame = NULL;
  }
  {
	if (pDecGlobal->qp_frame_buffer != NULL)
    {
      voFree(pDecGlobal,pDecGlobal->qp_frame_buffer);
      pDecGlobal->qp_frame_buffer = NULL;
    }
    if (pDecGlobal->qpc_frame_buffer != NULL)
    {
      voFree(pDecGlobal,pDecGlobal->qpc_frame_buffer);
      pDecGlobal->qpc_frame_buffer = NULL;
    }
	//if (pDecGlobal->mb_type0_frame_buffer != NULL)
    //{
    //  voFree(pDecGlobal,pDecGlobal->mb_type0_frame_buffer);
    //  pDecGlobal->mb_type0_frame_buffer = NULL;
    //}
	if (pDecGlobal->cbp_frame_buffer != NULL)
    {
      voFree(pDecGlobal,pDecGlobal->cbp_frame_buffer);
      pDecGlobal->cbp_frame_buffer = NULL;
    }
	if (pDecGlobal->b8direct_frame_buffer != NULL)
    {
      voFree(pDecGlobal,pDecGlobal->b8direct_frame_buffer);
      pDecGlobal->b8direct_frame_buffer = NULL;
    }
	if (pDecGlobal->c_ipred_frame_buffer != NULL)
    {
      voFree(pDecGlobal,pDecGlobal->c_ipred_frame_buffer);
      pDecGlobal->c_ipred_frame_buffer = NULL;
    }

// #if(USE_MULTI_THREAD || ENABLE_DEBLOCK_MB)
	if (pDecGlobal->cof_yuv_row != NULL)
    {
      voFree(pDecGlobal,pDecGlobal->cof_yuv_row);
      pDecGlobal->cof_yuv_row = NULL;
    }
	if (pDecGlobal->edge_buffer_alloc != NULL)
    {
      voFree(pDecGlobal,pDecGlobal->edge_buffer_alloc);
      pDecGlobal->edge_buffer_alloc = NULL;
    }
	if (pDecGlobal->edge_uvbuffer_alloc != NULL)
    {
      voFree(pDecGlobal,pDecGlobal->edge_uvbuffer_alloc);
      pDecGlobal->edge_uvbuffer_alloc = NULL;
    }
// #endif
	if (pDecGlobal->intra_pix_buffer != NULL)
    {
      voFree(pDecGlobal,pDecGlobal->intra_pix_buffer);
      pDecGlobal->intra_pix_buffer = NULL;
    }
// #endif
	if (pDecGlobal->mvd_frame_buffer != NULL)
    {
      voFree(pDecGlobal,pDecGlobal->mvd_frame_buffer);
      pDecGlobal->mvd_frame_buffer = NULL;
    }

	if(pDecGlobal->ipredmode_frame)
	{
		voFree(pDecGlobal,pDecGlobal->ipredmode_frame);
		pDecGlobal->ipredmode_frame= NULL;
	}

  }

#if defined(VOSSSE3)
  if(pDecGlobal->tmp_block)
  {
     voFree(pDecGlobal,pDecGlobal->tmp_block);
	 pDecGlobal->tmp_block = NULL;
  }
#endif  
  if(pDecGlobal->PicPos)
  {
    voFree(pDecGlobal,pDecGlobal->PicPos);
    pDecGlobal->PicPos=NULL;
  }
  free_qp_matrices(pDecGlobal);

  pDecGlobal->global_init_done = 0;
}


int OpenDecoder(DecoderParams ** phDec, VO_CODEC_INIT_USERDATA * pUserData)
{
#if (MVC_EXTENSION_ENABLE)
  int i;
#endif
  int iRet;
  VO_U32 flag=0;  
  void* pData=NULL;
  DecoderParams *pDecoder;

  if(pUserData!=NULL){
	  VOLOGINIT((VO_CHAR *)pUserData->reserved1);
  }

  VOLOGI(" ===> ");
#ifndef DISABLE_LICENSE
  VO_PTR  gHandle = NULL;
 if (pUserData)
 {
   if(pUserData->memflag&0x10)
   pData	 =	 pUserData->libOperator;
   flag	 =	 pUserData->memflag;
 }

 iRet = voCheckLibInit(&gHandle, VO_INDEX_DEC_H264, flag, NULL, pData);
 if(iRet != VO_ERR_NONE)
 {
  //*phDec = NULL;
  if(gHandle)
  {
   voCheckLibUninit(gHandle);
   gHandle = NULL;
  }
  return iRet;
 }
#endif
  iRet = alloc_decoder(phDec, pUserData);
  if(iRet)
  {
    return (iRet|DEC_ERRMASK);
  }
  pDecoder = *phDec;
  
  //init_old_slice(pDecoder->pDecGlobal->old_slice);

  init(pDecoder->pDecGlobal);
  pDecoder->FileFormat = VOH264_ANNEXB;
#if (MVC_EXTENSION_ENABLE)
  pDecoder->pDecGlobal->p_out = NULL;

  pDecoder->pDecGlobal->active_sps = NULL;
  pDecoder->pDecGlobal->active_subset_sps = NULL;
  init_subset_sps_list(pDecoder->pDecGlobal->SubsetSeqParSet, MAXSPS);
#endif

 

    pDecoder->pDecGlobal->licenseCheck = (TLicenseCheck2*)voMalloc( pDecoder->pDecGlobal,sizeof(TLicenseCheck2));

#ifndef DISABLE_LICENSE
#if defined(WIN32)||defined(_WIN32_WCE)
	pDecoder->pDecGlobal->licenseCheck->instance = hWin32Module;
#endif//
	

	pDecoder->pDecGlobal->licenseCheck->init					= (VO_U32 (VOAPI  *) (void** phCheck, VO_U32 nID, VO_U32 nFlag, void* hInst,void*))voCheckLibInit;
	pDecoder->pDecGlobal->licenseCheck->checkVideo		= (VO_U32 (VOAPI  *) (void* hCheck, void* pOutBuffer, void* pOutInfo))voCheckLibCheckVideo;
	pDecoder->pDecGlobal->licenseCheck->resetVideo		= (VO_U32 (VOAPI  *) (void* hCheck, void* pOutBuffer))voCheckLibResetVideo;
	pDecoder->pDecGlobal->licenseCheck->unInit				= (VO_U32 (VOAPI  *) (void* hCheck))voCheckLibUninit;
	//pDecoder->pDecGlobal->licenseCheck->init(&(pDecoder->pDecGlobal->licenseCheck->hCheck),VO_INDEX_DEC_H264,flag,pDecoder->pDecGlobal->licenseCheck->instance,pData);
	//printf("hcheck after init %x\r\n",pDecoder->pDecGlobal->licenseCheck->hCheck);
	pDecoder->pDecGlobal->licenseCheck->hCheck = gHandle;
#else//DISABLE_LICENSE
	pDecoder->pDecGlobal->licenseCheck->init = (VO_U32 (VOAPI  *) (void** phCheck, VO_U32 nID, VO_U32 nFlag, void* hInst,void*))voCheckLibInit2;
	pDecoder->pDecGlobal->licenseCheck->checkVideo=(VO_U32 (VOAPI  *) (void* hCheck, void* pOutBuffer, void* pOutInfo))voCheckLibCheckVideo2;
	pDecoder->pDecGlobal->licenseCheck->resetVideo = (VO_U32 (VOAPI  *) (void* hCheck, void* pOutBuffer))voCheckLibResetVideo2;
	pDecoder->pDecGlobal->licenseCheck->unInit			=(VO_U32 (VOAPI  *) (void* hCheck))voCheckLibUninit2;
#endif//DISABLE_LICENSE

#if defined(VOSSSE3)
    perform_mc_init();
#endif

  return DEC_OPEN_NOERR;
}
VO_S32 CheckNalu(NALU_t *nalu)
{
  VO_U8 *buf = nalu->buf;
  if(nalu->len<=4)
  	VOH264ERROR(VO_H264_ERR_NALU_SMALL);
  if(!buf[0]&&!buf[1]&&buf[2]==1)
  {
    nalu->buf+=3;
	nalu->len-=3;
  	return 0;
  }
  if(!buf[0]&&!buf[1]&&!buf[2]&&buf[3]==1)
  {
    nalu->buf+=4;
	nalu->len-=4;
  	return 0;
  }
  return 0;
}
#define H264_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define H264_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
static VO_U8* GetNextNalu(VO_U8* currPos,int size)
{
	VO_U8* p = currPos;  
	VO_U8* endPos = currPos+size-4;
	for (; p < endPos; p++)
	{
		if (H264_IS_ANNEXB(p))
			return p+3;
		else if(H264_IS_ANNEXB2(p))
			return p+4;
	}
	if(p>=endPos)
		p = NULL;

	return p;
}
#undef H264_IS_ANNEXB
#undef H264_IS_ANNEXB2

VO_S32 voH264DecProcessNALU(VO_HANDLE phDec)
{
  DecoderParams *pDecoder = phDec;
  H264DEC_G *pDecGlobal = pDecoder->pDecGlobal;  
  Slice *currSlice = pDecGlobal->pCurSlice; // = p_Vid->currentSlice;
  VO_S32 iRet;
  NALU_t *nalu = pDecGlobal->nalu;

  iRet = CheckNalu(nalu);
  
  if(iRet)
  	return iRet;

  CheckZeroByteNonVCL(pDecGlobal, nalu);
  nalu->forbidden_bit     = (*(nalu->buf) >> 7) & 1;
  nalu->nal_reference_idc = (NalRefIdc) ((*(nalu->buf) >> 5) & 3);
  nalu->nal_unit_type     = (NaluType) ((*(nalu->buf)) & 0x1f);

  iRet = process_one_nalu(pDecGlobal,currSlice);
  return iRet;
}
VO_S32 voH264DecProcessAnnexB(VO_HANDLE phDec)
{
  DecoderParams *pDecoder = phDec;
  H264DEC_G *pDecGlobal = pDecoder->pDecGlobal;  
  Slice *currSlice = pDecGlobal->pCurSlice; // = p_Vid->currentSlice;
  NALU_t *nalu = pDecoder->pDecGlobal->nalu;
  VO_S32 iRet = 0;
  VO_U8* inBuffer= nalu->buf;
  VO_U32 BufferLen = nalu->len; 
  VO_U8* nextFrame,*currFrame;
  VO_S32 leftSize=BufferLen,inSize;

  if (BufferLen <=4)
  {
	return VO_ERR_INPUT_BUFFER_SMALL;
  }
  nextFrame = currFrame = inBuffer;
  currFrame = GetNextNalu(currFrame,leftSize);
  if (currFrame!=NULL)
  {
    leftSize-=(currFrame-inBuffer);
  }
  while (leftSize>=3&&currFrame!=NULL)
  {
	nextFrame = GetNextNalu(currFrame,leftSize);
	if(nextFrame)
	{
	  inSize = nextFrame-currFrame;
	}
	else
	{
	  inSize = leftSize;
	}
	if(inSize >=2)
	{
	  nalu->buf = currFrame;
	  nalu->len = inSize;

	  CheckZeroByteNonVCL(pDecGlobal, nalu);
	  nalu->forbidden_bit     = (*(nalu->buf) >> 7) & 1;
	  nalu->nal_reference_idc = (NalRefIdc) ((*(nalu->buf) >> 5) & 3);
	  nalu->nal_unit_type     = (NaluType) ((*(nalu->buf)) & 0x1f);

	  iRet = process_one_nalu(pDecGlobal,currSlice);
	}
	leftSize-=inSize;
	currFrame=nextFrame;
  }
  return iRet;
}

int FinitDecoder(VO_HANDLE phDec)
{
  DecoderParams *pDecoder = phDec;
  if(!pDecoder)
    return DEC_GEN_NOERR;

  //ClearDecPicList(pDecoder->pDecGlobal);
  if (pDecoder->pDecGlobal->first_sps != 3)
  {
	  return VO_ERR_WRONG_STATUS;
  }

#if USE_FRAME_THREAD
  if (pDecoder->pDecGlobal->nThdNum > 1/*&&!pDecoder->pDecGlobal->interlace*/)
  {
	  DecodeFrameMain(pDecoder->pDecGlobal, NULL, NULL, 2);
  }
  else
#endif
  {
	  flush_dpb(pDecoder->pDecGlobal,pDecoder->pDecGlobal->p_Dpb);
  }

//   pDecoder->pDecGlobal->newframe = 0;
  pDecoder->pDecGlobal->previous_frame_num = 0;
  //*ppDecPicList = pDecoder->pDecGlobal->pDecOuputPic;
  return DEC_GEN_NOERR;
}

int CloseDecoder(VO_HANDLE phDec)
{
  DecoderParams *pDecoder = phDec;
  if(!pDecoder)
    return DEC_CLOSE_NOERR;
  pDecoder->pDecGlobal->licenseCheck->unInit(pDecoder->pDecGlobal->licenseCheck->hCheck);
  voFree( pDecoder->pDecGlobal, pDecoder->pDecGlobal->licenseCheck);

// #if USE_MULTI_THREAD
  if (pDecoder->pDecGlobal->nThdNum > 1&&pDecoder->pDecGlobal->first_sps&&pDecoder->pDecGlobal->thdinit)
  {
#if USE_FRAME_THREAD
	  ReleaseH264Threads2(pDecoder->pDecGlobal);
#else
	  ReleaseH264Threads(pDecoder->pDecGlobal);
#endif
  }
// #endif

  //FmoFinit(pDecoder->pDecGlobal);
  free_global_buffers(pDecoder->pDecGlobal);

  //remove by Really Yang 20110420
  //ercClose(pDecoder->p_Vid, pDecoder->p_Vid->erc_errorVar);
  //end of remove
  CleanUpPPS(pDecoder->pDecGlobal);
  if(pDecoder->pDecGlobal->dec_picture)
  {
	  int i = pDecoder->pDecGlobal->dec_picture->cur_buf_index;
	  free_storable_picture(pDecoder->pDecGlobal,pDecoder->pDecGlobal->dec_picture);
	  FrameBufCtl(&pDecoder->pDecGlobal->vFrameIndexFIFO, i, FIFO_WRITE);
  }
  free_dpb(pDecoder->pDecGlobal,pDecoder->pDecGlobal->p_Dpb);

  //uninit_out_buffer(pDecoder->pDecGlobal);
#if COMBINE_FRAME
  voFree(pDecoder->pDecGlobal, pDecoder->inBuf);
#endif
  free_private_param(pDecoder->pDecGlobal);
  free_img (pDecoder->pDecGlobal);
  free(pDecoder);
  return DEC_CLOSE_NOERR;
}
