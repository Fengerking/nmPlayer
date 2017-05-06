 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265_decoder.c
    \brief    front decoder functions
    \author   Renjie Yu
	\change
*/

//TBD
//YU_TBD, Initilization
//YU_TBD, slice->sps, pps
//YU_TBD; clean up data type

#include <stdlib.h>
#include <assert.h>
// #include "h265_decoder.h"
#include "h265dec_slice.h"
#include "h265dec_mem.h"
#include "h265dec_list.h"
#include "h265dec_dequant.h"
#include "h265dec_cabac.h"
static const VO_U8 g_bitDepth = 8;

static const  VO_U32 bit_mask[ 16 ] = {
    0, 1, 3, 7, 15, 31, 63, 127, 255
};

/*!
 *************************************************************************************
 * \brief
 *    Get slice private buffer size
 *
 * \param p_slice [IN ] slice handle, with intilized properties values
 *
 * \return
 *    needed memory size
 *
 **************************************************************************************/
VO_U32 GetSliceMemSize( H265_DEC_SLICE *p_slice )
{
	//H265_DEC_SPS *p_sps       = p_slice->p_sps;
	//VO_U32 PicSizeInCtbsY     = p_slice->PicSizeInCtbsY;

	VO_U32 private_mem_size   = 0;	

	//----------CU
	private_mem_size += ( sizeof( VO_U8 ) * ( 1 << p_slice->log2_diff_max_min_luma ) + CACHE_LINE ); //left_depth_buffer
	private_mem_size += ( sizeof( VO_U8 ) * p_slice->PicWidthInMinCbsY + CACHE_LINE ); //top_depth_buffer
	private_mem_size += ( sizeof( VO_U8 ) * ( 1 << p_slice->log2_diff_max_min_luma ) + CACHE_LINE ); //left_skip_buffer
	private_mem_size += ( sizeof( VO_U8 ) * p_slice->PicWidthInMinCbsY + CACHE_LINE ); //top_skip_buffer

	//----------PU
	private_mem_size += ( sizeof( VO_S8 ) * p_slice->PicHeightInMinTbsY + CACHE_LINE ); //pu_para_left
	private_mem_size += ( sizeof( VO_S8 ) * p_slice->PicWidthInMinTbsY + CACHE_LINE ); //pu_para_top
	//private_mem_size += ( sizeof( VO_S8 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY + CACHE_LINE ); //slice_number removed by DTT
	
	//----------TU
	private_mem_size += ( sizeof( VO_U32 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY + CACHE_LINE ); //deblock_para

#if USE_3D_WAVE_THREAD
	if (p_slice->nThdNum > 1)
	{
	}
	else
#endif
	{
		//---------- temp edge 
		private_mem_size += ( sizeof( VO_U8 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) + CACHE_LINE );//edge_emu_buff0
		private_mem_size += ( sizeof( VO_U8 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) + CACHE_LINE );//edge_emu_buff1

		//----------COEFF
		//private_mem_size +=  ( sizeof( VO_U8 ) * PRED_CACHE_STRIDE * PRED_CACHE_STRIDE + CACHE_LINE );
		//private_mem_size +=  ( sizeof( VO_U8 ) * PRED_CACHE_STRIDE * PRED_CACHE_STRIDE + CACHE_LINE );
		//private_mem_size +=  ( sizeof( VO_U8 ) * PRED_CACHE_STRIDE * PRED_CACHE_STRIDE + CACHE_LINE );
		//private_mem_size +=  ( sizeof( VO_U8 ) * PRED_CACHE_STRIDE * PRED_CACHE_STRIDE + CACHE_LINE );
    
		//----------MC Buffer
		private_mem_size +=  ( sizeof( VO_S16 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) + CACHE_LINE ); // p_slice->asm_mc0_buff
		private_mem_size +=  ( sizeof( VO_S16 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) + CACHE_LINE ); // p_slice->asm_mc1_buff
	}

	//LHP for memory size for BS
	private_mem_size += 2 * ( sizeof( VO_U8 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY + CACHE_LINE);

	//buffer for intra pred
#if USE_3D_WAVE_THREAD
	{
// 		H265_DEC_SPS *p_sps = p_slice->p_sps;
// 		VO_U32 log2_ctb_size    = p_sps->log2_luma_coding_block_size;
// 		VO_U32 PicHeightInCtbsY = ( ( p_sps->pic_height_in_luma_samples + ( 1 << log2_ctb_size ) - 1 ) >> log2_ctb_size );
		VO_U32 PicHeightInCtbsY = p_slice->PicHeightInCtbsY - 1;

		private_mem_size += 3*(sizeof(VO_U8*)*PicHeightInCtbsY+CACHE_LINE); //locate the point of Y,U,V buffer

		private_mem_size += PicHeightInCtbsY*(sizeof(VO_U8)*(p_slice->PicWidthInCtbsY<<p_slice->CtbLog2SizeY)+CACHE_LINE); //max_cu_width equals to 1<<p_slice->CtbLog2SizeY
		private_mem_size += PicHeightInCtbsY*(sizeof(VO_U8)*(p_slice->PicWidthInCtbsY<<(p_slice->CtbLog2SizeY-1))+CACHE_LINE);
		private_mem_size += PicHeightInCtbsY*(sizeof(VO_U8)*(p_slice->PicWidthInCtbsY<<(p_slice->CtbLog2SizeY-1))+CACHE_LINE);

		//filterX
		private_mem_size += (PicHeightInCtbsY+1)*sizeof(VO_S32) + CACHE_LINE;
	}
#else
	private_mem_size += 2*(sizeof(VO_U8)*(p_slice->PicWidthInCtbsY<<p_slice->CtbLog2SizeY)+CACHE_LINE); //max_cu_width equals to 1<<p_slice->CtbLog2SizeY
	private_mem_size += 2*(sizeof(VO_U8)*(p_slice->PicWidthInCtbsY<<(p_slice->CtbLog2SizeY-1))+CACHE_LINE);
	private_mem_size += 2*(sizeof(VO_U8)*(p_slice->PicWidthInCtbsY<<(p_slice->CtbLog2SizeY-1))+CACHE_LINE);
#endif
	//coeff
	{
		VO_S32 pic_stride    = ( p_slice->p_sps->pic_width_in_luma_samples + 31 ) & 0xffffffe0;
		VO_S32 pic_height    = ( p_slice->p_sps->pic_height_in_luma_samples + 1 ) & 0xfffffffe;

		private_mem_size += 3*(sizeof(VO_S16)*pic_stride*pic_height + CACHE_LINE);
	}

	//rctask
	private_mem_size += sizeof(TReconTask)*p_slice->PicHeightInCtbsY*p_slice->PicWidthInCtbsY+ CACHE_LINE;

	//SAO 
  if(p_slice->p_sps->sample_adaptive_offset_enabled_flag)
  {
    VO_U32 SAOParam_mem_size = 0; 
    VO_U32 maxCUWidth = 1<<p_slice->p_sps->log2_luma_coding_block_size;  
    VO_U32 maxCUHeight = 1<<p_slice->p_sps->log2_luma_coding_block_size;
    //calculate memory size of one SAO param item firstly
    SAOParam_mem_size +=  sizeof(VO_U8)*(2*(p_slice->p_sps->pic_width_in_luma_samples + maxCUWidth)) + CACHE_LINE;
    SAOParam_mem_size +=  sizeof(VO_U8)*(2*(p_slice->p_sps->pic_width_in_luma_samples + maxCUWidth)) + CACHE_LINE;  
    SAOParam_mem_size +=  sizeof(VO_U8)*(2*(p_slice->p_sps->pic_width_in_luma_samples + maxCUWidth)) + CACHE_LINE;

    SAOParam_mem_size +=  sizeof(VO_U8)*(p_slice->p_sps->pic_width_in_luma_samples+16+CACHE_LINE)*3;

    SAOParam_mem_size +=  sizeof(VO_U8)*(maxCUHeight+64+68+CACHE_LINE)*3;  //3 stands y, u,v
    SAOParam_mem_size +=  sizeof(VO_U8)*(maxCUHeight+64+68+CACHE_LINE)*3;  //+64, up, low part; +68, last LCU row
    //end of calculate memory size of one SAO param item firstly

    private_mem_size += sizeof(VO_U32)*p_slice->PicHeightInCtbsY*p_slice->PicWidthInCtbsY+1+ CACHE_LINE;
    private_mem_size += sizeof(NDBFBlockInfo)*p_slice->PicHeightInCtbsY*p_slice->PicWidthInCtbsY+ CACHE_LINE;
    private_mem_size += (p_slice->PicHeightInCtbsY+1)*(sizeof(TComSampleAdaptiveOffset *)+CACHE_LINE);
    private_mem_size += (p_slice->PicHeightInCtbsY+1)*(sizeof(TComSampleAdaptiveOffset)+CACHE_LINE);
    private_mem_size += (p_slice->PicHeightInCtbsY+1)*(SAOParam_mem_size+CACHE_LINE);

    private_mem_size += sizeof(SaoLcuParam)*(3*p_slice->PicHeightInCtbsY*p_slice->PicWidthInCtbsY)+ CACHE_LINE;
    private_mem_size +=  sizeof(SaoOffsetTable)*(3*p_slice->PicHeightInCtbsY*p_slice->PicWidthInCtbsY+1)+ CACHE_LINE;
    private_mem_size +=  sizeof(VO_U8)*(1 << g_bitDepth)+ CACHE_LINE;
  }


	return ( private_mem_size + SAFETY );
}

/*!
 *************************************************************************************
 * \brief
 *    Initialize slice buffer
 *
 * \param p_slice [IN ] slice handle, with intilized properties values
 *
 * \return
 *    error code
 *
 **************************************************************************************/
static VO_S32 IniSliceBuf( H265_DEC_SLICE *p_slice )
{
	VO_U8 *p_priv_mem = p_slice->p_priv_mem;
    VO_U32 i;
	/* initialize internal memory*/	
	ALIGN_MEM(p_priv_mem);
	p_slice->left_depth_buffer = ( VO_U8* )p_priv_mem;
	p_priv_mem += ( sizeof( VO_U8 ) * ( 1 << p_slice->log2_diff_max_min_luma ) );

	ALIGN_MEM(p_priv_mem);
	p_slice->top_depth_buffer = ( VO_U8* )p_priv_mem;
	p_priv_mem += ( sizeof( VO_U8 ) * p_slice->PicWidthInMinCbsY );

	ALIGN_MEM(p_priv_mem);
	p_slice->left_skip_buffer = ( VO_U8* )p_priv_mem;
	p_priv_mem += ( sizeof( VO_U8 ) * ( 1 << p_slice->log2_diff_max_min_luma ) );

	ALIGN_MEM(p_priv_mem);
	p_slice->top_skip_buffer = ( VO_U8* )p_priv_mem;
	p_priv_mem += ( sizeof( VO_U8 ) * p_slice->PicWidthInMinCbsY );

	ALIGN_MEM(p_priv_mem);
	p_slice->pu_para_left = ( VO_S8* )p_priv_mem;
	p_priv_mem += ( sizeof( VO_S8 ) * p_slice->PicHeightInMinTbsY );

	ALIGN_MEM(p_priv_mem);
	p_slice->pu_para_top = ( VO_S8* )p_priv_mem;
	p_priv_mem += ( sizeof( VO_S8 ) * p_slice->PicWidthInMinTbsY );
    
    //removed by DTT
	//ALIGN_MEM(p_priv_mem);
	//p_slice->slice_number = ( VO_S8* )p_priv_mem;
	//p_priv_mem += ( sizeof( VO_S8 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY );

	ALIGN_MEM(p_priv_mem);
	p_slice->deblock_para = ( VO_U32* )p_priv_mem;
	p_priv_mem += ( sizeof( VO_U32 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY ); //deblock_para

#if USE_3D_WAVE_THREAD
	if (p_slice->nThdNum > 1)
	{
	}
	else
#endif
	{
		ALIGN_MEM(p_priv_mem);
		p_slice->edge_emu_buff0 = ( VO_U8* )p_priv_mem;
		p_priv_mem += ( sizeof( VO_U8 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) );

		ALIGN_MEM(p_priv_mem);
		p_slice->edge_emu_buff1 = ( VO_U8* )p_priv_mem;
		p_priv_mem += ( sizeof( VO_U8 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) );

		ALIGN_MEM(p_priv_mem);
		p_slice->asm_mc0_buff = ( VO_S16* )p_priv_mem;
		p_priv_mem += ( sizeof( VO_S16 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) );

		ALIGN_MEM(p_priv_mem);
		p_slice->asm_mc1_buff = ( VO_S16* )p_priv_mem;
		p_priv_mem += ( sizeof( VO_S16 )*( MAX_CU_SIZE + 7 ) * ( MAX_CU_SIZE + 7 ) );
	}

	//LHP m_puBsBuffer
	ALIGN_MEM(p_priv_mem);
	p_slice->p_pBsVer = (VO_U8*)p_priv_mem;
	p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY );

	ALIGN_MEM(p_priv_mem);
	p_slice->p_pBsHor = (VO_U8*)p_priv_mem;
	p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY);  //need check the buffer size issue

	///ALIGN_MEM(p_priv_mem);
	///p_dec->m_puiTileIdxMap = (VO_U32*)p_priv_mem;//new VO_U32[PicSizeInCtbsY+1];
	///p_priv_mem += sizeof(VO_U32) * (PicSizeInCtbsY );

	//ALIGN_MEM(p_priv_mem);
	//p_slice->m_puhDepth = (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->m_pbIPCMFlag = (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->m_CUTransquantBypass = (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->m_puhWidth = (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->m_puhHeight = (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->m_phQP = (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->qp_buffer = (VO_S8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_S8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->m_puhLumaIntraDir = (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);
	//
// 	ALIGN_MEM(p_priv_mem);
// 	p_slice->m_puhInterDir = (VO_U8*)p_priv_mem;
// 	p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->m_puhTrIdx = (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->m_skipFlag = (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->skip_buffer= (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//ALIGN_MEM(p_priv_mem);
	//p_slice->inter_dir_buffer = (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	///ALIGN_MEM(p_priv_mem);
	///p_slice->left_depth_buffer= (VO_U8*)p_priv_mem;
	///p_priv_mem += (sizeof(VO_U8) * p_slice->PicHeightInMinCbsY);

	///ALIGN_MEM(p_priv_mem);
	///p_slice->top_depth_buffer= (VO_U8*)p_priv_mem;
	///p_priv_mem += (sizeof(VO_U8) * p_slice->PicWidthInMinCbsY);

	///ALIGN_MEM(p_priv_mem);
	///p_slice->left_luma_intra_mode_buffer = (VO_U8*)p_priv_mem;
	///p_priv_mem += (sizeof(VO_U8) * p_dec->picHeightInMin4bs);

	///ALIGN_MEM(p_priv_mem);
	///p_slice->top_luma_intra_mode_buffer = (VO_U8*)p_priv_mem;
	///p_priv_mem += (sizeof(VO_U8) * p_dec->picWidthInMin4bs);



	//ALIGN_MEM(p_priv_mem);
	//p_slice->luma_cbf= (VO_U8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_U8) * p_slice->PicSizeInCtbsY * p_slice->partition_num);

	//buffer for intra pred
#if USE_3D_WAVE_THREAD
	{
// 		H265_DEC_SPS *p_sps = p_slice->p_sps;
// 		VO_U32 log2_ctb_size    = p_sps->log2_luma_coding_block_size;
// 		VO_U32 PicHeightInCtbsY = ( ( p_sps->pic_height_in_luma_samples + ( 1 << log2_ctb_size ) - 1 ) >> log2_ctb_size );

		VO_U32 PicHeightInCtbsY = p_slice->PicHeightInCtbsY - 1;

		ALIGN_MEM(p_priv_mem);
		p_slice->m_puBufForIntraY = (VO_U8**)p_priv_mem;
		p_priv_mem += sizeof(VO_U8*)*PicHeightInCtbsY;
		ALIGN_MEM(p_priv_mem);
		p_slice->m_puBufForIntraU = (VO_U8**)p_priv_mem;
		p_priv_mem += sizeof(VO_U8*)*PicHeightInCtbsY;
		ALIGN_MEM(p_priv_mem);
		p_slice->m_puBufForIntraV = (VO_U8**)p_priv_mem;
		p_priv_mem += sizeof(VO_U8*)*PicHeightInCtbsY;

		for (i = 0; i < PicHeightInCtbsY; ++i)
		{
			ALIGN_MEM(p_priv_mem);
			p_slice->m_puBufForIntraY[i] = (VO_U8*)p_priv_mem;
			p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInCtbsY<<p_slice->CtbLog2SizeY); //max_cu_width equals to 1<<p_slice->CtbLog2SizeY
		}

		for (i = 0; i < PicHeightInCtbsY; ++i)
		{
			ALIGN_MEM(p_priv_mem);
			p_slice->m_puBufForIntraU[i] = (VO_U8*)p_priv_mem;
			p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInCtbsY<<(p_slice->CtbLog2SizeY-1));
		}

		for (i = 0; i < PicHeightInCtbsY; ++i)
		{
			ALIGN_MEM(p_priv_mem);
			p_slice->m_puBufForIntraV[i] = (VO_U8*)p_priv_mem;
			p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInCtbsY<<(p_slice->CtbLog2SizeY-1));
		}
		//filterX
		ALIGN_MEM(p_priv_mem);
		p_slice->filterX_Pos = (VO_S32*)p_priv_mem;
		p_priv_mem +=  sizeof(VO_S32)*(PicHeightInCtbsY+1);
	}
#else
	ALIGN_MEM(p_priv_mem);
	p_slice->m_puBufForIntraY[0] = (VO_U8*)p_priv_mem;
	p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInCtbsY<<p_slice->CtbLog2SizeY); //max_cu_width equals to 1<<p_slice->CtbLog2SizeY

	ALIGN_MEM(p_priv_mem);
	p_slice->m_puBufForIntraY[1] = (VO_U8*)p_priv_mem;
	p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInCtbsY<<p_slice->CtbLog2SizeY);

	ALIGN_MEM(p_priv_mem);
	p_slice->m_puBufForIntraU[0] = (VO_U8*)p_priv_mem;
	p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInCtbsY<<(p_slice->CtbLog2SizeY-1));

	ALIGN_MEM(p_priv_mem);
	p_slice->m_puBufForIntraU[1] = (VO_U8*)p_priv_mem;
	p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInCtbsY<<(p_slice->CtbLog2SizeY-1));

	ALIGN_MEM(p_priv_mem);
	p_slice->m_puBufForIntraV[0] = (VO_U8*)p_priv_mem;
	p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInCtbsY<<(p_slice->CtbLog2SizeY-1));

	ALIGN_MEM(p_priv_mem);
	p_slice->m_puBufForIntraV[1] = (VO_U8*)p_priv_mem;
	p_priv_mem +=  (sizeof(VO_U8)*p_slice->PicWidthInCtbsY<<(p_slice->CtbLog2SizeY-1));
#endif

	{
		VO_S32 pic_stride    = ( p_slice->p_sps->pic_width_in_luma_samples + 31 ) & 0xffffffe0;
		VO_S32 pic_height    = ( p_slice->p_sps->pic_height_in_luma_samples + 1 ) & 0xfffffffe;
        ALIGN_MEM(p_priv_mem);
		p_slice->coeff_buf[0] = (VO_S16*)p_priv_mem;
		p_slice->p_coeff[0] = p_slice->coeff_buf[0];
		p_priv_mem += (pic_stride * pic_height*2);
		ALIGN_MEM(p_priv_mem);
		p_slice->coeff_buf[1] = (VO_S16*)p_priv_mem;
		p_slice->p_coeff[1] = p_slice->coeff_buf[1];
		p_priv_mem += (pic_stride * pic_height*2/4);
		ALIGN_MEM(p_priv_mem);
		p_slice->coeff_buf[2] = (VO_S16*)p_priv_mem;
		p_slice->p_coeff[2] = p_slice->coeff_buf[2];
		p_priv_mem += (pic_stride * pic_height*2/4);

	}

	ALIGN_MEM(p_priv_mem);
	p_slice->rc_task = (TReconTask*)p_priv_mem;
	p_priv_mem +=  p_slice->PicHeightInCtbsY*p_slice->PicWidthInCtbsY*(sizeof(TReconTask));

  if(p_slice->p_sps->sample_adaptive_offset_enabled_flag)
  {
    ALIGN_MEM(p_priv_mem);
    p_slice->m_sliceIDMapLCU = ( VO_U32* )p_priv_mem;
    p_priv_mem += ( sizeof(VO_U32)*p_slice->PicHeightInCtbsY*p_slice->PicWidthInCtbsY+1 );

    ALIGN_MEM(p_priv_mem);
    p_slice->m_vNDFBlock = (NDBFBlockInfo *)p_priv_mem;
    p_priv_mem += ( sizeof(NDBFBlockInfo )*p_slice->PicHeightInCtbsY*p_slice->PicWidthInCtbsY );

    ALIGN_MEM(p_priv_mem);
    p_slice->m_saoParam = (TComSampleAdaptiveOffset**)p_priv_mem;
    p_priv_mem +=  (p_slice->PicHeightInCtbsY+1)*(sizeof(TComSampleAdaptiveOffset *));

    ALIGN_MEM(p_priv_mem);
    for(i = 0;i<p_slice->PicHeightInCtbsY+1;i++)
    {	  
      p_slice->m_saoParam[i] = (TComSampleAdaptiveOffset*)p_priv_mem;
      p_priv_mem += sizeof(TComSampleAdaptiveOffset );
    }
   
    for(i = 0;i<p_slice->PicHeightInCtbsY+1;i++)
    {
      p_slice->m_saoParam[i]->m_iPicWidth = p_slice->p_sps->pic_width_in_luma_samples ;
      p_slice->m_saoParam[i]->m_iPicHeight = p_slice->p_sps->pic_height_in_luma_samples ;
      p_slice->m_saoParam[i]->m_uiMaxCUWidth = 1<<p_slice->p_sps->log2_luma_coding_block_size;  
      p_slice->m_saoParam[i]->m_uiMaxCUHeight = 1<<p_slice->p_sps->log2_luma_coding_block_size;

      p_slice->m_saoParam[i]->numCuInWidth = ( p_slice->p_sps->pic_width_in_luma_samples + (1<<p_slice->p_sps->log2_luma_coding_block_size) - 1 ) >> p_slice->p_sps->log2_luma_coding_block_size;
      p_slice->m_saoParam[i]->numCuInHeight = ( p_slice->p_sps->pic_height_in_luma_samples + (1<<p_slice->p_sps->log2_luma_coding_block_size) - 1 ) >> p_slice->p_sps->log2_luma_coding_block_size;

      ALIGN_MEM(p_priv_mem);
      p_slice->m_saoParam[i]->m_iUpBuff1All[0] = (VO_U8 *) p_priv_mem;
      p_priv_mem += 2*(p_slice->p_sps->pic_width_in_luma_samples + p_slice->m_saoParam[i]->m_uiMaxCUWidth);

      ALIGN_MEM(p_priv_mem);
      p_slice->m_saoParam[i]->m_iUpBuff1All[1] = (VO_U8 *) p_priv_mem;
      p_priv_mem += 2*(p_slice->p_sps->pic_width_in_luma_samples + p_slice->m_saoParam[i]->m_uiMaxCUWidth);

      ALIGN_MEM(p_priv_mem);
      p_slice->m_saoParam[i]->m_iUpBuff1All[2] = (VO_U8 *) p_priv_mem;
      p_priv_mem += 2*(p_slice->p_sps->pic_width_in_luma_samples + p_slice->m_saoParam[i]->m_uiMaxCUWidth);
     
      ALIGN_MEM(p_priv_mem);
      p_slice->m_saoParam[i]->m_pTmpU = (VO_U8 *) p_priv_mem;
      p_priv_mem += sizeof(VO_U8)*(p_slice->p_sps->pic_width_in_luma_samples+16)*3;
      
      ALIGN_MEM(p_priv_mem);
      p_slice->m_saoParam[i]->m_pTmpL[0] = (VO_U8 *) p_priv_mem;
      p_priv_mem += sizeof(VO_U8)*(p_slice->m_saoParam[i]->m_uiMaxCUHeight+64+68)*3;

      ALIGN_MEM(p_priv_mem);
      p_slice->m_saoParam[i]->m_pTmpL[1] = (VO_U8 *) p_priv_mem;
      p_priv_mem += sizeof(VO_U8)*(p_slice->m_saoParam[i]->m_uiMaxCUHeight+64+68)*3;
    }


    ALIGN_MEM(p_priv_mem);
    p_slice->saoLcuParam = (SaoLcuParam *)p_priv_mem;
    p_priv_mem += sizeof(SaoLcuParam)*(3*p_slice->PicHeightInCtbsY*p_slice->PicWidthInCtbsY);

    ALIGN_MEM(p_priv_mem);
    p_slice->pSaoOffsetTable = (SaoOffsetTable *)p_priv_mem;
    p_priv_mem += sizeof(SaoOffsetTable)*(3*p_slice->PicHeightInCtbsY*p_slice->PicWidthInCtbsY+1);
    p_slice->pSaoOffsetTable++;

    ALIGN_MEM(p_priv_mem);
    p_slice->m_lumaTableBo = (VO_U8 *)p_priv_mem;
    p_priv_mem += sizeof(VO_U8)*((1 << g_bitDepth));

    for (i=0; i< (1U << g_bitDepth); i++)
    {
      p_slice->m_lumaTableBo[i] = (VO_U8)(1 + (i>>(g_bitDepth - SAO_BO_BITS)));
    }
  }

	

	//ALIGN_MEM(p_priv_mem);
	//p_slice->m_intraPred.cache_stride = PRED_CACHE_STRIDE;
	//p_slice->m_intraPred.cache_height = PRED_CACHE_STRIDE;
	//p_slice->m_intraPred.cache_buf = (VO_U8*)p_priv_mem;
	//p_priv_mem += sizeof(VO_U8)* p_dec->slice.m_intraPred.cache_stride * p_dec->slice.m_intraPred.cache_height;

	///ALIGN_MEM(p_priv_mem);
	///p_slice->m_interCache.m_iPicWidth = p_dec->m_uiMaxCUWidth;
	///p_slice->m_interCache.m_iPicHeight = p_dec->m_uiMaxCUHeight;
	///p_slice->m_interCache.pic_stride[ 0 ] = PRED_CACHE_STRIDE;
	///p_slice->m_interCache.pic_stride[ 1 ] = PRED_CACHE_STRIDE;
	///p_slice->m_interCache.pic_buf[ 0 ] = (VO_U8*)p_priv_mem;
	///p_priv_mem += sizeof(VO_U8) * PRED_CACHE_STRIDE * PRED_CACHE_STRIDE ;

	///ALIGN_MEM(p_priv_mem);
	///p_slice->m_interCache.pic_buf[ 1 ] = (VO_U8*)p_priv_mem;
	///p_priv_mem += sizeof(VO_U8) * PRED_CACHE_STRIDE * PRED_CACHE_STRIDE ;

	///ALIGN_MEM(p_priv_mem);
	///p_slice->m_interCache.pic_buf[ 2 ] = (VO_U8*)p_priv_mem;
	///p_priv_mem += sizeof(VO_U8) * PRED_CACHE_STRIDE * PRED_CACHE_STRIDE ;


	//p_dec->mv_bufmask = ~0;
	//p_dec->mv_buf = (VO_S32*)p_priv_mem;
	//p_priv_mem += (4*sizeof(VO_S32) * (p_dec->nPosEnd+1))+ SAFETY;
	//p_dec->bFieldPred_buf = (VO_S8*)p_priv_mem;
	//p_priv_mem += (sizeof(VO_S8) * p_dec->nPosEnd)+ SAFETY;



	//if(p_dec->priv_mem_cfg & INNER_MEM_DATA_PARTITION){
	//
	//}

	//nLumOffset = (EDGE_SIZE * p_dec->nLumEXWidth + EDGE_SIZE);
	//nChrOffset = (EDGE_SIZE2 * p_dec->nChrEXWidth + EDGE_SIZE2);

	//p_dec->vFrameBufFIFO.r_idx = p_dec->vFrameBufFIFO.w_idx = 0;
	/* frame buffers*/
	//for(i = 0; i < FRAMEDEFAULT; i++){//TBD
	//	p_dec->frameBuffer[i].y += nLumOffset;
	//	p_dec->frameBuffer[i].u += nChrOffset;
	//	p_dec->frameBuffer[i].v +=  nChrOffset;
	//	FrameBufCtl(&p_dec->vFrameBufFIFO, &p_dec->frameBuffer[i], FIFO_WRITE);
	//}

	/*defalt frame buffer*/
	//p_dec->pRefFrame = FrameBufCtl(&p_dec->vFrameBufFIFO, NULL, FIFO_READ);
	//p_dec->pCurFrame = FrameBufCtl(&p_dec->vFrameBufFIFO, NULL, FIFO_READ);	
	
	return VO_ERR_NONE;
}

/*!
 *************************************************************************************
 * \brief
 *    Alloc slice private buffer and initialize slice buffer
 *
 * \param p_slice [IN ] slice handle, with intilized properties values
 *
 * \param p_user_op_all [IN ] private operation handle
 *
 * \return
 *    error code
 *
 **************************************************************************************/
static VO_S32 AllocSliceBuf( H265_DEC_SLICE *p_slice, 
	USER_OP_ALL* p_user_op_all )
{
	VO_U32 private_mem_size;
	
	private_mem_size = GetSliceMemSize( p_slice );	
		               
	p_slice->p_priv_mem = ( VO_U8 * )AlignMalloc( p_user_op_all->p_user_op, p_user_op_all->codec_id, private_mem_size , CACHE_LINE );

	if ( !p_slice->p_priv_mem )
		return VO_ERR_OUTOF_MEMORY;
	
	//set 0 for internal memory
	SetMem( p_user_op_all->p_user_op, p_user_op_all->codec_id, p_slice->p_priv_mem, 0, private_mem_size );

	IniSliceBuf( p_slice );

	
	return VO_ERR_NONE;
}

/*!
 *************************************************************************************
 * \brief
 *    Alloc DPB pool buffer and initialize DPB pool structure
 *
 * \param p_slice [IN ] slice handle, with intilized properties values
 *
 * \return
 *    needed memory size
 *
 **************************************************************************************/
static VO_S32 AllocDPBPool( H265_DEC* p_dec,
	H265_DEC_SPS *p_sps )
{
	VO_U8 *p_pic_buf;
	VO_U32 pic_num       = p_dec->dpb_list.node_num;
	VO_S32 pic_stride;//    = ( p_sps->pic_width_in_luma_samples + 31 ) & 0xffffffe0;
    //VO_S32 pic_stride    = ( p_sps->pic_width_in_luma_samples + 31 ) *32 / 32;
	VO_S32 pic_height;//    = ( p_sps->pic_height_in_luma_samples + 1 ) & 0xfffffffe;
	VO_S32 mv_field_size;// = sizeof( PicMvField ) * \
		                   ( ( p_sps->pic_width_in_luma_samples  + ( 1 << p_sps->log2_luma_coding_block_size ) - 1 ) >> 2  ) * \
		                   ( ( p_sps->pic_height_in_luma_samples  + ( 1 << p_sps->log2_luma_coding_block_size ) - 1 ) >> 2 ) ;
	VO_U32 i, dpb_buf_size;
    VO_U32 log2_ctb_size    = p_sps->log2_luma_coding_block_size;
// 	VO_U32 PicWidthInCtbsY  = ( ( p_sps->pic_width_in_luma_samples + ( 1 << log2_ctb_size ) - 1 ) >> log2_ctb_size );
// 	VO_U32 PicHeightInCtbsY = ( ( p_sps->pic_height_in_luma_samples + ( 1 << log2_ctb_size ) - 1 ) >> log2_ctb_size );
// 	VO_U32 PicSizeInCtbsY   = PicWidthInCtbsY * PicHeightInCtbsY;
	if ( p_dec->user_op_all.p_pic_buf_op ) {/* Shared frame memory*/
#if 0
		VO_MEM_VIDEO_INFO vbuf_info;
		VO_MEM_VIDEO_OPERATOR *p_pic_buf_op = p_dec->user_op_all.p_pic_buf_op;

		pic_buf_done = VO_TRUE;

		vbuf_info.ColorType = VO_COLOR_YUV_PLANER420;
		vbuf_info.FrameCount = pic_num;
		vbuf_info.Stride = inner_mem.expic_width;
		vbuf_info.Height = inner_mem.expic_height;


		if(p_pic_buf_op->Init(p_dec->user_op_all.codec_id, &vbuf_info)) 
			return VO_ERR_OUTOF_MEMORY;

		/* reset stride for shared frame buffer*/
		p_dec->nLumEXWidth = vbuf_info.VBuffer[0].Stride[0];
		p_dec->nChrEXWidth = vbuf_info.VBuffer[0].Stride[1];

		//for(i = 0; i < pic_num; i++){
		//	if(!vbuf_info.VBuffer[i].Buffer[0] || \
		//		!vbuf_info.VBuffer[i].Buffer[1] || \
		//		!vbuf_info.VBuffer[i].Buffer[2] )
		//		return VO_ERR_OUTOF_MEMORY;
		//	p_dec->frameBuffer[i].y = vbuf_info.VBuffer[i].Buffer[0];
		//	p_dec->frameBuffer[i].u = vbuf_info.VBuffer[i].Buffer[1];
		//	p_dec->frameBuffer[i].v = vbuf_info.VBuffer[i].Buffer[2];
		//}
#endif
		return VO_ERR_NOT_IMPLEMENT;
	}else if ( p_dec->user_op_all.p_user_op ) {/* pre-alloced memory*/
#if 0
		VO_CODEC_INIT_USERDATA *p_user_op = p_dec->user_op_all.p_user_op;

		if(p_user_op->memflag == VO_IMF_PREALLOCATEDBUFFER){
			VO_VIDEO_INNER_MEM *p_inner_mem = (VO_VIDEO_INNER_MEM *)p_user_op->memData;

			pic_buf_done = VO_TRUE;

			for(i = 0; i < pic_num; i++){
				if(!p_inner_mem->pic_buf[i][0] || \
					!p_inner_mem->pic_buf[i][1] || \
					!p_inner_mem->pic_buf[i][2] )
					return VO_ERR_OUTOF_MEMORY;
				p_dec->frameBuffer[i].y = p_inner_mem->pic_buf[i][0];
				p_dec->frameBuffer[i].u = p_inner_mem->pic_buf[i][1];
				p_dec->frameBuffer[i].v = p_inner_mem->pic_buf[i][2];
			}
		}

		return VO_ERR_NOT_IMPLEMENT;
#endif
	}

	if (!p_dec->using_BA_size)
	{
		pic_stride    = ( p_sps->pic_width_in_luma_samples + 31 ) & 0xffffffe0;
		pic_height    = ( p_sps->pic_height_in_luma_samples + 1 ) & 0xfffffffe;
		mv_field_size = sizeof( PicMvField ) * \
			( ( p_sps->pic_width_in_luma_samples  + ( 1 << p_sps->log2_luma_coding_block_size ) - 1 ) >> 2  ) * \
			( ( p_sps->pic_height_in_luma_samples  + ( 1 << p_sps->log2_luma_coding_block_size ) - 1 ) >> 2 ) ;
	}
	else
	{
		pic_stride    = ( p_dec->BA_max_width + 31 ) & 0xffffffe0;
		pic_height    = ( p_dec->BA_max_height + 1 ) & 0xfffffffe;
		mv_field_size = sizeof( PicMvField ) * \
			( ( p_dec->BA_max_width  +  MAX_CU_SIZE - 1) >> 2  ) * \
			( ( p_dec->BA_max_height  + MAX_CU_SIZE - 1 ) >> 2 ) ;
	}

	dpb_buf_size = ( pic_num * ( ( pic_stride * pic_height * 3 / 2 )/*+pic_stride * pic_height*3*/
		             /*+ PicSizeInCtbsY*sizeof(TReconTask)*/+ CACHE_LINE + mv_field_size + CACHE_LINE ) );//YU_TBD: support other yuv type + mv_field_size ) );          
	p_pic_buf = p_dec->dpb_buf = ( VO_U8 * )AlignMalloc( p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, dpb_buf_size, CACHE_LINE );

	if (!p_pic_buf )
		return VO_ERR_OUTOF_MEMORY;
	
	//set 0 for internal memory
	SetMem( p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_pic_buf, 0, dpb_buf_size );

	for ( i = 0; i < pic_num; i++ ) {
		p_dec->dpb_pool[ i ].m_iPicWidth     = p_sps->pic_width_in_luma_samples;//pic_stride;
		p_dec->dpb_pool[ i ].m_iPicHeight    = ( p_sps->pic_height_in_luma_samples + 1 ) & 0xfffffffe;
		p_dec->dpb_pool[ i ].pic_stride[ 0 ] = pic_stride;
		p_dec->dpb_pool[ i ].pic_stride[ 1 ] = pic_stride >> 1;
		p_dec->dpb_pool[ i ].pic_stride[ 2 ] = pic_stride >> 1;

		ALIGN_MEM(p_pic_buf);
		p_dec->dpb_pool[ i ].pic_buf[ 0 ]  = p_pic_buf; 
		p_pic_buf += ( pic_stride * pic_height );
		p_dec->dpb_pool[ i ].pic_buf[ 1 ]  = p_pic_buf; 
		p_pic_buf += ( pic_stride * pic_height ) >> 2;
		p_dec->dpb_pool[ i ].pic_buf[ 2 ]  = p_pic_buf;
		p_pic_buf += ( pic_stride * pic_height ) >> 2;
			
		ALIGN_MEM(p_pic_buf);
		p_dec->dpb_pool[i].motion_buffer= (PicMvField*)p_pic_buf;
		p_pic_buf += mv_field_size ;
//         ALIGN_MEM(p_pic_buf);
// 		p_dec->dpb_pool[ i ].coeff_buf[0] = (VO_S16*)p_pic_buf;
// 		p_dec->dpb_pool[ i ].p_coeff[0] = p_dec->dpb_pool[ i ].coeff_buf[0];
// 		p_pic_buf += (pic_stride * pic_height*2);
// 		ALIGN_MEM(p_pic_buf);
// 		p_dec->dpb_pool[ i ].coeff_buf[1] = (VO_S16*)p_pic_buf;
// 		p_dec->dpb_pool[ i ].p_coeff[1] = p_dec->dpb_pool[ i ].coeff_buf[1];
// 		p_pic_buf += (pic_stride * pic_height*2/4);
// 		ALIGN_MEM(p_pic_buf);
// 		p_dec->dpb_pool[ i ].coeff_buf[2] = (VO_S16*)p_pic_buf;
// 		p_dec->dpb_pool[ i ].p_coeff[2] = p_dec->dpb_pool[ i ].coeff_buf[2];
// 		p_pic_buf += (pic_stride * pic_height*2/4);
// 		ALIGN_MEM(p_pic_buf);
// 		p_dec->dpb_pool[ i ].rc_task = (TReconTask*)p_pic_buf;
// 		p_pic_buf += PicSizeInCtbsY*sizeof(TReconTask);
#if USE_3D_WAVE_THREAD
		if (pthread_mutex_init(&p_dec->dpb_pool[i].unBlockMutex, NULL))
		{
			return VO_ERR_FAILED;
		}
#endif
		//p_dec->dpb_pool[i].m_saoParam = allocSaoParam(p_dec);
	}

	return VO_ERR_NONE;
}



/*!
 *************************************************************************************
 * \brief
 *    Release decoder internal buffer and frame buffer
 *
 * \param p_dec [IN] decoder handle
 *
 * \return
 *    error code
 *
 **************************************************************************************/
VO_U32 FreeSliceBuf( H265_DEC_SLICE *p_slice, 
	USER_OP_ALL* p_user_op_all )
{
	//release internal memory
	AlignFree( p_user_op_all->p_user_op, p_user_op_all->codec_id, p_slice->p_priv_mem );

	p_slice->p_priv_mem = NULL;

	return VO_ERR_NONE;
}


/*!
 *************************************************************************************
 * \brief
 *    Release decoder DPB buffers
 *
 * \param p_dec [IN] decoder handle
 *
 * \return
 *    error code
 *
 **************************************************************************************/
static VO_U32 FreeDPBPool( H265_DEC *p_dec )
{
	VO_U32 voRC = VO_ERR_NONE;
	/* frame memory*/
	if ( p_dec->user_op_all.p_pic_buf_op ) {/* Shared frame memory*/
#if 0
		VO_MEM_VIDEO_OPERATOR *p_pic_buf_op = p_dec->user_op_all.p_pic_buf_op;

		if ( p_pic_buf_op->Uninit( p_dec->user_op_all.codec_id ) )
			return VO_ERR_INVALID_ARG;
#endif
	}
	
	if (p_dec->dpb_buf)
	{
#if USE_3D_WAVE_THREAD
		VO_U32 i;
		for (i = 0; i < p_dec->dpb_list.node_num; ++i)
		{
			if (pthread_mutex_destroy(&p_dec->dpb_pool[i].unBlockMutex))
			{
				voRC |= VO_ERR_FAILED;
			}
		}

#endif
		//release dpb_buf memory
		AlignFree( p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_dec->dpb_buf );
		p_dec->dpb_buf  = NULL;
	}

	return voRC;
}

//YU_TBD, only support unchanged resolution by now !!
VO_S32 ConfigDecoder( H265_DEC* p_dec,  
	H265_DEC_SLICE *p_slice,
	H265_DEC_SPS *p_sps )
{
	VO_U32 pic_width  = p_sps->pic_width_in_luma_samples;
	VO_U32 pic_height = p_sps->pic_height_in_luma_samples;
	VO_S32 ret        = VO_ERR_NONE;
	VO_U32 i;

	if ( p_dec->pic_width_in_luma_samples != pic_width 
		|| p_dec->pic_height_in_luma_samples != pic_height || !p_slice->init_done) {

		if (p_dec->pic_width_in_luma_samples != pic_width || p_dec->pic_height_in_luma_samples != pic_height)
		{
			p_dec->pic_width_in_luma_samples			= pic_width;
			p_dec->pic_height_in_luma_samples			= pic_height;
		}

		p_slice->MinCbLog2SizeY = p_sps->log2_min_luma_coding_block_size;
		p_slice->MinTbLog2SizeY = p_sps->log2_min_transform_block_size;
		//p_slice->MinCbLog2SizeMask = bit_mask[ p_sps->log2_min_luma_coding_block_size ];
		p_slice->CtbLog2SizeY   = p_sps->log2_luma_coding_block_size;
		p_slice->CtbLog2SizeYMask = bit_mask[ p_sps->log2_luma_coding_block_size ];
		p_slice->PicWidthInCtbsY = ( p_sps->pic_width_in_luma_samples  + ( 1 << p_slice->CtbLog2SizeY ) - 1 ) >> p_slice->CtbLog2SizeY;
		p_slice->PicHeightInCtbsY = ( p_sps->pic_height_in_luma_samples  + ( 1 << p_slice->CtbLog2SizeY ) - 1 ) >> p_slice->CtbLog2SizeY;
		p_slice->PicSizeInCtbsY = p_slice->PicWidthInCtbsY * p_slice->PicHeightInCtbsY;
		
		p_slice->PicWidthInMinCbsY = p_sps->pic_width_in_luma_samples >> p_slice->MinCbLog2SizeY;
    p_slice->PicWidthInMinTbsY = ( p_slice->PicWidthInCtbsY << p_slice->CtbLog2SizeY ) >> 2;    
		p_slice->PicHeightInMinCbsY = p_sps->pic_height_in_luma_samples >> p_slice->MinCbLog2SizeY;
    p_slice->PicHeightInMinTbsY = ( p_slice->PicHeightInCtbsY << p_slice->CtbLog2SizeY ) >> 2;    

		p_slice->log2_diff_max_min_luma = p_slice->CtbLog2SizeY - p_slice->MinCbLog2SizeY;
		p_slice->log2_diff_max_min_luma_mask = bit_mask[ p_slice->log2_diff_max_min_luma ];
		p_slice->log2_diff_max_min_luma_pu_mask = bit_mask[ p_slice->log2_diff_max_min_luma + 1 ];
		p_slice->partition_num = 1 << ( ( p_sps->log2_luma_coding_block_size - p_sps->log2_min_transform_block_size ) << 1 );


   
		FreeSliceBuf( p_slice, &p_dec->user_op_all );
		//YU_TBD: check error code
		ret = AllocSliceBuf( p_slice, &p_dec->user_op_all );
		if (ret)
		{
			return ret;
		}


    if(p_sps->scaling_list_enabled_flag){
      ret = alloc_mMulLevelScale(p_slice);
	  if (ret)
	  {
		  return ret;
	  }
// LOGI("alloc_mMulLevelScale done\n");
    }
    


		if (!p_dec->pic_buf_done)
		{
			FreeDPBPool( p_dec );
			ret = AllocDPBPool( p_dec, p_sps );
			if (ret)
			{
				return ret;
			}
// LOGI("AllocDPBPool done\n");
			CreateDpbList( &p_dec->dpb_list, p_dec->dpb_pool );

			p_dec->pic_buf_done = VO_TRUE;
		}
        p_slice->m_bUseNIF = VO_FALSE;
		p_slice->init_done = VO_TRUE;
#if 0
		p_dec->params_init_done = 0; //Harry: reset flag that pps init should restart
#endif
	}
#if 0
	//////////////////////////////////////////////////////////YU_TBD
    // Configure decoded picture buffer list. Temp solution now.
    for ( i = 0; i < p_dec->dpb_list->node_num; i++) {
  
        tmp_idx = g_auiCuOffsetY;
        InitCuOffsetY(p_dec->m_uiMaxCUWidth, p_dec->m_uiMaxCUHeight, p_slice->PicWidthInCtbsY, p_dec->m_uiHeightInCU, GetStride(&p_dec->dpb_pool[i]), &tmp_idx);
        tmp_idx = g_auiCuOffsetC;
        InitCuOffsetC(p_dec->m_uiMaxCUWidth, p_dec->m_uiMaxCUHeight, p_slice->PicWidthInCtbsY, p_dec->m_uiHeightInCU, GetCStride(&p_dec->dpb_pool[i]), &tmp_idx);
        tmp_idx = g_auiBuOffsetY;
        InitBuOffsetY(p_dec->m_uiMaxCUWidth, p_dec->m_uiMaxCUHeight, p_dec->m_uhTotalDepth, GetStride(&p_dec->dpb_pool[i]), &tmp_idx);
        tmp_idx = g_auiBuOffsetC;
        InitBuOffsetC(p_dec->m_uiMaxCUWidth, p_dec->m_uiMaxCUHeight, p_dec->m_uhTotalDepth, GetCStride(&p_dec->dpb_pool[i]), &tmp_idx);
    }

    // Configure inter cache buffer block map. Temp solution now.
    tmp_idx = g_auiCacheOffsetY;
    InitBuOffsetY(p_dec->m_uiMaxCUWidth, p_dec->m_uiMaxCUHeight, p_dec->m_uhTotalDepth, GetStride(p_slice->m_interCache), &tmp_idx);
    tmp_idx = g_auiCacheOffsetC;
    InitBuOffsetC(p_dec->m_uiMaxCUWidth, p_dec->m_uiMaxCUHeight, p_dec->m_uhTotalDepth, GetCStride(p_slice->m_interCache), &tmp_idx);
    //////////////////////////////////////////////////////////////////////////////////
#endif
	return ret;
}


VO_VOID GetDisPic( H265_DEC *p_dec, 
	VO_VIDEO_BUFFER * p_out_vbuf, 
	VO_VIDEO_OUTPUTINFO * p_out_vbuf_info )
{
	TComPic*  p_dis_pic = PopDisPic(p_dec, &p_dec->dpb_list,(VO_S32 *) &p_out_vbuf_info->Flag);
    
	if (p_dis_pic != NULL) {//There is display pic
        //printf("display frame POC = [%d].\n", p_dis_pic->m_iPOC);
		if (p_dis_pic->pic_type != VIDEO_FRAME_NULL) {//buffer not ready
			p_out_vbuf_info->Format.Width   = p_dis_pic->m_disWidth; //p_dec->sps->pic_width_in_luma_samples-p_dec->sps->conf_win_right_offset -p_dec->sps->conf_win_left_offset;//->p_out_pic->m_iPicWidth;
			p_out_vbuf_info->Format.Height  = p_dis_pic->m_disHeight; //p_dec->sps->pic_height_in_luma_samples-p_dec->sps->conf_win_bottom_offset-p_dec->sps->conf_win_top_offset;//p_out_pic->m_iPicHeight;
			
			p_out_vbuf_info->Format.Type    = p_dis_pic->pic_type;//YU_TBD
			p_out_vbuf->Buffer[0]			= p_dis_pic->pic_buf[ 0 ];
			p_out_vbuf->Buffer[1]			= p_dis_pic->pic_buf[ 1 ];
			p_out_vbuf->Buffer[2]			= p_dis_pic->pic_buf[ 2 ];
			p_out_vbuf->Stride[0]			= p_dis_pic->pic_stride[ 0 ];
			p_out_vbuf->Stride[1]			= p_dis_pic->pic_stride[ 1 ];
			p_out_vbuf->Stride[2]			= p_dis_pic->pic_stride[ 2 ];
			p_out_vbuf->Time				= p_dis_pic->Time;
			p_out_vbuf->ColorType           = VO_COLOR_YUV_PLANAR420;
			//Clean up dis_pic
#if !PAD_BUFF_ENABLED
			p_dis_pic->m_bIsBorderExtended   = VO_FALSE;
#endif
#if USE_CODEC_NEW_API
			p_out_vbuf->CodecData = p_dis_pic;
			p_dis_pic->m_bNeededForOutput    = PIC_OUT;
#else
			p_dis_pic->m_bNeededForOutput    = VO_FALSE;
#endif
// LOGI("output POC %d\n", p_dis_pic->m_iPOC);
			return;
		} 
	}
	p_out_vbuf_info->Format.Type = VO_VIDEO_FRAME_NULL;//buf not ready;

	//Harry: for old framework, need to set buffer[0] to NULL
	p_out_vbuf->Buffer[0]			= NULL;
// 	p_out_vbuf->Buffer[1]			= NULL;
// 	p_out_vbuf->Buffer[2]			= NULL;
}

#if 0
VO_VOID PrintStreamInfo(H265_DEC* p_dec){
  char *customer = "VisualOn";

  
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Customer                     : %s\n",customer);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Internal Format              : %dx%d %dHz\n", p_dec->pic_width_in_luma_samples, p_dec->pic_height_in_luma_samples, 0 );

  ///VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"CU size / depth              : %d / %d\n", p_dec->sps->max_cu_width, p_dec->sps->max_cu_depth );

  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"RQT trans. size (min / max)  : %d / %d\n", 1 << p_dec->sps->log2_min_transform_block_size,1 << (p_dec->sps->log2_min_transform_block_size+p_dec->sps->log2_max_transform_block_size) );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Max RQT depth inter          : %d\n", p_dec->sps->max_transform_hierarchy_depth_inter+1);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Max RQT depth intra          : %d\n", p_dec->sps->max_transform_hierarchy_depth_intra+1);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Min PCM size                 : %d\n", 1 << (p_dec->sps->log2_min_pcm_luma_coding_block_size));
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Motion search range          : %d\n", m_iSearchRange );
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Intra period                 : %d\n", m_iIntraPeriod );
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Decoding refresh type        : %d\n", m_iDecodingRefreshType );
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"QP                           : %5.2f\n", p_dec->slice.slice_qp );
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Max dQP signaling depth      : %d\n", m_iMaxCuDQPDepth);

  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Cb QP Offset                 : %d\n",  p_dec->slice.slice_qp_delta_cb   );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Cr QP Offset                 : %d\n",  p_dec->slice.slice_qp_delta_cr );

  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"QP adaptation                : %d (range=%d)\n", m_bUseAdaptiveQP, (m_bUseAdaptiveQP ? m_iQPAdaptationRange : 0) );
 // VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"GOP size                     : %d\n", m_iGOPSize );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Internal bit depth           : (Y:%d, C:%d)\n", p_dec->sps->bit_depth_luma, p_dec->sps->bit_depth_chroma );
  
  if(p_dec->sps->pcm_enabled_flag){
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"PCM sample bit depth         : (Y:%d, C:%d)\n", p_dec->sps->pcm_sample_bit_depth_luma_minus1+1, p_dec->sps->pcm_sample_bit_depth_chroma_minus1+1 );
  }else{
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"PCM sample bit depth         : (Y:%d, C:%d)\n", p_dec->sps->bit_depth_luma, p_dec->sps->bit_depth_chroma );
  }
  
#if 0 // no rage control info in decoder
 #if RATE_CONTROL_LAMBDA_DOMAIN
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"RateControl                  : %d\n", m_RCEnableRateControl );
  if(m_RCEnableRateControl)
  {
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"TargetBitrate                : %d\n", m_RCTargetBitrate );
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"KeepHierarchicalBit          : %d\n", m_RCKeepHierarchicalBit );
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"LCULevelRC                   : %d\n", m_RCLCULevelRC );
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"UseLCUSeparateModel          : %d\n", m_RCUseLCUSeparateModel );
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"InitialQP                    : %d\n", m_RCInitialQP );
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"ForceIntraQP                 : %d\n", m_RCForceIntraQP );
  }
  #else
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"RateControl                  : %d\n", m_enableRateCtrl);
  if(m_enableRateCtrl)
  {
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"TargetBitrate                : %d\n", m_targetBitrate);
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"NumLCUInUnit                 : %d\n", m_numLCUInUnit);
  }
  #endif
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Max Num Merge Candidates     : %d\n", m_maxNumMergeCand);
#endif
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"\n");


  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"TOOL CFG: ");
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"IBD:%d ", g_bitDepthY > m_inputBitDepthY || g_bitDepthC > m_inputBitDepthC);
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"HAD:%d ", m_bUseHADME           );
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"SRD:%d ", m_bUseSBACRD          );
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"RDQ:%d ", m_useRDOQ            );
  #if RDOQ_TRANSFORMSKIP
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"RDQTS:%d ", m_useRDOQTS        );
  #endif
  /*VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"SQP:%d ", m_uiDeltaQpRD         );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"ASR:%d ", m_bUseASR             );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"LComb:%d ", m_bUseLComb         );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"FEN:%d ", m_bUseFastEnc         );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"ECU:%d ", m_bUseEarlyCU         );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"FDM:%d ", m_useFastDecisionForMerge );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"CFM:%d ", m_bUseCbfFastMode         );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"ESD:%d ", m_useEarlySkipDetection  );*/
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"RQT:%d ", 1     );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"TransformSkip:%d ",     p_dec->pps->transform_skip_enabled_flag              );
  /*VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"TransformSkipFast:%d ", m_useTransformSkipFast       );*/
  /*VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"Slice: M=%d ", m_iSliceMode);
  if (m_iSliceMode!=0)
  {
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"A=%d ", m_iSliceArgument);
  }
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"DependentSlice: M=%d ",m_iDependentSliceMode);
  if (m_iDependentSliceMode!=0)
  {
    VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"A=%d ", m_iDependentSliceArgument);
  }*/
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"CIP:%d ", p_dec->pps->constrained_intra_pred_flag);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"SAO:%d ", p_dec->sps->sample_adaptive_offset_enabled_flag);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"PCM:%d ", p_dec->sps->pcm_enabled_flag);
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"SAOLcuBasedOptimization:%d ", (m_saoLcuBasedOptimization)?(1):(0));

  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"LosslessCuEnabled:%d ", p_dec->sps. );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"WPP:%d ", p_dec->pps->weighted_pred_flag);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"WPB:%d ", p_dec->pps->weighted_bipred_flag);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"PME:%d ", p_dec->pps->log2_parallel_merge_level_minus2+2);
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO," WaveFrontSynchro:%d WaveFrontSubstreams:%d",m_iWaveFrontSynchro, m_iWaveFrontSubstreams);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO," ScalingList:%d ", p_dec->pps->pps_scaling_list_data_present_flag );
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"TMVPMode:%d ", p_dec->sps->sps_temporal_mvp_enable_flag     );
  #if ADAPTIVE_QP_SELECTION
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"AQpS:%d", m_bUseAdaptQpSelect   );
  #endif

  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO," SignBitHidingFlag:%d ", p_dec->pps->sign_data_hiding_enabled_flag);
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"RecalQP:%d", m_recalculateQPAccordingToLambda ? 1 : 0 );


  //VO flags
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO," DeblockFilterControlPresentFlag:%d ", p_dec->pps->deblocking_filter_control_present_flag);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO," PPSDeblockingFilterFlag:%d ", p_dec->pps->deblocking_filter_override_enabled_flag);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO," DisableDeblockingFilterFlag:%d ", p_dec->pps->pps_deblocking_filter_disabled_flag);
  
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO," NumTileRowsMinus1:%d ", p_dec->pps->num_tile_rows_minus1);
  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO," NumTileColumnsMinus1:%d ", p_dec->pps->num_tile_columns_minus1);


  VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STREAM_INFO,"\n\n");


}
#endif

VO_S32 DecodeRawVideo(H265_DEC* p_dec, VO_U8* p_inbuf, VO_S32 buf_len)
{
	VO_S32 ret, left_len;
#if !USE_3D_WAVE_THREAD
	VO_BOOL new_pic  = VO_FALSE;
#endif
	left_len = buf_len;

#if USE_3D_WAVE_THREAD
	if ((ret = PrepareDecodeFrame(p_dec, &p_dec->slice, &p_inbuf, &left_len)) != VO_ERR_NONE)
	{
		return ret;
	}
	// printf("end prepare\n");
	if ((ret = DoDecodeFrame(p_dec, &p_dec->slice, p_inbuf, left_len)) != VO_ERR_NONE)
	{
		SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_dec->slice.rc_task, 0, p_dec->slice.PicSizeInCtbsY*sizeof(TReconTask));
	    p_dec->slice.cur_pic->errFlag = ret;
		return ret;
	}
#else
	//Decode nalus in pic
	while ( left_len > 3 ) {// at least 3byte sc(0x000001)
		VO_S32 used_len = 0;

		if ( ret = DecodeNalu(p_dec, p_inbuf, left_len, &used_len, &new_pic ) ) {
			return ret;//error
		}

		p_inbuf  += used_len;
		left_len -= used_len;		

		if ( new_pic ) {
			TComPic*  rpcPic = p_dec->slice.cur_pic;

			//if (!p_dec->m_already_print_stream_info) {
			//	PrintStreamInfo(p_dec); 
			//	p_dec->m_already_print_stream_info = VO_TRUE;
			//}
			rpcPic->m_bNeededForOutput = VO_TRUE;
			sortPicList( &p_dec->dpb_list ); 
      if (p_dec->out_mode == 0) {
        PushDisPic(p_dec, &p_dec->dpb_list, rpcPic);
      } else {
			xWriteOutput(p_dec, &p_dec->dpb_list, p_dec->slice.p_sps->sps_max_num_reorder_pics[p_dec->slice.m_uiTLayer] );
      }
			break;
		}
	}
#endif

	return VO_ERR_NONE;
}


/*create decoder engine
* and some initilization
*/
VO_U32 CreateH265Decoder( void **p_handle, 
	VO_CODEC_INIT_USERDATA* p_user_op, 
	VO_U32 codec_id )
{
	H265_DEC* p_dec = NULL;
    int i,j;

	/* malloc decoder engine*/
	p_dec = (H265_DEC*) AlignMalloc( p_user_op, codec_id, sizeof( H265_DEC ), CACHE_LINE );
	if ( !p_dec ) {
		return VO_ERR_OUTOF_MEMORY;
	}

	SetMem( p_user_op, codec_id, (VO_U8*)p_dec, 0, sizeof( H265_DEC ) );

	/* user operation*/
	p_dec->user_op_all.codec_id = codec_id;
	p_dec->user_op_all.p_user_op = p_user_op;
	p_dec->slice.p_user_op_all = &p_dec->user_op_all;

	/* global initilization, not rely on bitstream*/
	//Harry: Initialize the state of pps&sps
	p_dec->slice.slice_pic_parameter_set_id = (VO_U32)(-1); //None pps set
	p_dec->slice.slice_seq_parameter_set_id = (VO_U32)(-1); //None sps set
	//Harry: Init fast mode as 0:nomal
	p_dec->fast_mode = 0;
	p_dec->thumbnail_mode = 0; //VO_PID_VIDEO_THUMBNAIL_MODE = 1
	p_dec->dpb_list.node_num = MAX_NUM_REF_PICS + 2;//default

  
  for(i=0; i<16; i++){
    p_dec->vps[i].Valid = VO_FALSE;  //default
  }
  for(i=0; i<MAX_SPS_NUM; i++){
    p_dec->sps[i].Valid = VO_FALSE;  //default
  }
  for(i=0; i<MAX_PPS_NUM; i++){
    p_dec->pps[i].Valid = VO_FALSE;   //default
  }

  //DTT: Init cabac table
  p_dec->voCabacInitTab = (VO_U8*) AlignMalloc( p_user_op, codec_id, 3*CABAC_TABLE_SIZE_SLICE*sizeof( VO_U8 ), CACHE_LINE );
  if ( !p_dec->voCabacInitTab ) {
		return VO_ERR_OUTOF_MEMORY;
  }
  p_dec->slice.voCabacInitTab = p_dec->voCabacInitTab;
  for(j=0;j<3;j++)
  	for(i=0;i<52;i++)
  	{
  	    voCabacTableInit(i,j,p_dec->voCabacInitTab+j*CABAC_TABLE_SIZE_SLICE+i*MAX_NUM_CTX_MOD);
  	}
	//@Harry:dpb_pool,dpb_list
	//p_dec->dpb_list = (H265_DPB_LIST *) AlignMalloc(p_user_op, codec_id, sizeof(H265_DPB_LIST), CACHE_LINE);
	//if ( !p_dec->dpb_list ) {
	//
	//	return VO_ERR_OUTOF_MEMORY;
	//}
	//SetMem(p_user_op, codec_id, p_dec->dpb_list, 0, sizeof(H265_DPB_LIST));


	//p_dec->dpb_pool = (TComPic *) AlignMalloc(p_user_op, codec_id, MAX_DPB_SIZE*sizeof(TComPic), CACHE_LINE);
	//if ( !p_dec->dpb_pool ) {
	//	return VO_ERR_OUTOF_MEMORY;
	//}
	//SetMem(p_user_op, codec_id, p_dec->dpb_pool, 0, MAX_DPB_SIZE*sizeof(TComPic));

	*p_handle = p_dec;/*return back decoder handle*/

	return VO_ERR_NONE;
}


VO_U32 DeleteH265Decoder( void *p_handle )
{
	H265_DEC *p_dec = (H265_DEC *)p_handle;
	VO_U32 i;

	for ( i = 0; i < MAX_SPS_NUM; i++ ) {
		H265_DEC_SPS* p_sps = &p_dec->sps[ i ];



    if(p_sps->sps_scaling_list_data_present_flag){
      free_ScalingList(p_dec,&p_sps->spsScalingListEntity);
    }

    if(/*i==0&&*/p_sps->scaling_list_enabled_flag&&p_dec->slice.pSliceScalingListEntity){ //i==0, because only one instance in slice
	  if(!p_sps->sps_scaling_list_data_present_flag){
        free_ScalingList(p_dec,p_dec->slice.pSliceScalingListEntity);
	  }
      free_mMulLevelScale(&p_dec->slice);
    }
	}

	for ( i = 0; i < MAX_PPS_NUM; i++ ) {
		H265_DEC_PPS* p_pps = &p_dec->pps[ i ];

		if (p_pps->CtbAddrRsToTs)
		{
			AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_pps->CtbAddrRsToTs);
		}
		if (p_pps->CtbAddrTsToRs)
		{
			AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_pps->CtbAddrTsToRs);
		}
		if (p_pps->TileId)
		{
			AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_pps->TileId);
		}
        if (p_pps->ExtendMinTbAddrZs)
            AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_pps->ExtendMinTbAddrZs);
		
        if ( p_pps->row_height )
			AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_pps->row_height );

		if ( p_pps->column_width )
	        AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_pps->column_width );

    if(p_pps->pps_scaling_list_data_present_flag){
      free_ScalingList(p_dec,&p_pps->ppsScalingListEntity);
    }
	}

     //-----------Joyce add, for scalingList----
    //destroyScalingList(p_dec);


	/* Free decoder internal (private) memory*/
	FreeSliceBuf( &p_dec->slice, &p_dec->user_op_all );

// 	if (p_dec->pic_buf_done)
// 	{
		FreeDPBPool( p_dec );
// 	}
    if(p_dec->voCabacInitTab)
		AlignFree( p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_dec->voCabacInitTab );
	/* Free decoder handle*/
	AlignFree( p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_dec );

	return VO_ERR_NONE;
}


