
#include <math.h>

#include "block.h"
#include "global.h"
#include "mbuffer.h"
//#include "mbuffer_mvc.h"
//#include "elements.h"
//#include "errorconcealment.h"
#include "macroblock.h"
#include "fmo.h"
#include "vlc.h"
#include "image.h"
#include "mb_access.h"
#include "cabac.h"
#include "transform.h"
#include "mc_prediction.h"
#include "quant.h"
#include "mv_prediction.h"
#include "mb_prediction.h"
#include "fast_memory.h"
#include "loopfilter.h"
#include "voH264Readbits.h"
#include "biaridecod.h"
#include "ifunctions.h"

#define TRACE_STRING(s)
#define TRACE_DECBITS(i)
#define TRACE_PRINTF(s) 
#define TRACE_STRING_P(s)

static VO_S32 read_CBP_and_coeffs_from_NAL_CABAC_420 (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 dct8x8);
static VO_S32 read_CBP_and_coeffs_from_NAL_CAVLC_420 (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 dct8x8);
static VO_S32 read_motion_info_from_NAL_p_slice  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0);
static VO_S32 read_motion_info_from_NAL_b_slice  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0);
static VO_S32 read_motion_info_from_NAL_p_slice_cavlc  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0);
static VO_S32 read_motion_info_from_NAL_b_slice_cavlc  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0);
static VO_S32 read_motion_info_from_NAL_p_slice_interlace  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0);
static VO_S32 read_motion_info_from_NAL_b_slice_interlace  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0);
static VO_S32 read_motion_info_from_NAL_p_slice_cavlc_interlace  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0);
static VO_S32 read_motion_info_from_NAL_b_slice_cavlc_interlace  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0);

static void read_ipred_modes_cabac                   (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
static void read_ipred_modes_cabac_8x8             (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
static void read_IPCM_coeffs_from_NAL          (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, struct datapartition *dP);
static VO_S32 read_one_macroblock_i_slice        (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
//static void read_one_macroblock_i_slice_cavlc        (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
static VO_S32 read_one_macroblock_p_slice        (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
static VO_S32 read_one_macroblock_cavlc        (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
static VO_S32 read_one_macroblock_b_slice        (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
//static void read_one_macroblock_b_slice_cavlc        (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
static int  decode_one_component_p_slice       (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture);
extern void update_direct_types                (H264DEC_G *pDecGlobal,Slice *currSlice);

static const byte SNGL_SCAN[16][2] =
{
	{0,0},{1,0},{0,1},{0,2},
	{1,1},{2,0},{3,0},{2,1},
	{1,2},{0,3},{1,3},{2,2},
	{3,1},{3,2},{2,3},{3,3}
};

//! field scan pattern
static const byte FIELD_SCAN[16][2] =
{
	{0,0},{0,1},{1,0},{0,2},
	{0,3},{1,1},{1,2},{1,3},
	{2,0},{2,1},{2,2},{2,3},
	{3,0},{3,1},{3,2},{3,3}
};
static const byte FIELD_SCAN_COF[16] =
{
	0  ,64 ,16 ,128,
	192 ,80 ,144 ,208 ,
	32,96,160,224,
	48,112,176,240
};

//! single scan pattern
static const byte SNGL_SCAN_COF[16] =
{
	0  ,16 ,64 ,128,
	80 ,32 ,48 ,96 ,
	144,192,208,160,
	112,176,224,240
};

static const byte SNGL_SCAN_4X4[16] =
{
	0 ,1 ,4 ,8 ,
	5 ,2 ,3 ,6 ,
	9 ,12,13,10,
	7 ,11,14,15
};

static const byte FIELD_SCAN_4X4[16] =
{
	0, 4, 1, 8,
	12, 5, 9, 13,
	2, 6, 10, 14,
	3, 7, 11, 15,
};


static const byte SNGL_SCAN_8X8_CABAC[64] =
{
	0,   1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

static const byte FIELD_SCAN_8X8_CABAC[64]=
{
	0+0*8, 0+1*8, 0+2*8, 1+0*8,
	1+1*8, 0+3*8, 0+4*8, 1+2*8,
	2+0*8, 1+3*8, 0+5*8, 0+6*8,
	0+7*8, 1+4*8, 2+1*8, 3+0*8,
	2+2*8, 1+5*8, 1+6*8, 1+7*8,
	2+3*8, 3+1*8, 4+0*8, 3+2*8,
	2+4*8, 2+5*8, 2+6*8, 2+7*8,
	3+3*8, 4+1*8, 5+0*8, 4+2*8,
	3+4*8, 3+5*8, 3+6*8, 3+7*8,
	4+3*8, 5+1*8, 6+0*8, 5+2*8,
	4+4*8, 4+5*8, 4+6*8, 4+7*8,
	5+3*8, 6+1*8, 6+2*8, 5+4*8,
	5+5*8, 5+6*8, 5+7*8, 6+3*8,
	7+0*8, 7+1*8, 6+4*8, 6+5*8,
	6+6*8, 6+7*8, 7+2*8, 7+3*8,
	7+4*8, 7+5*8, 7+6*8, 7+7*8
};



static const byte SNGL_SCAN_8X8[64] =
{
	0+0*8, 1+1*8, 1+2*8, 2+2*8,
	4+1*8, 0+5*8, 3+3*8, 7+0*8,
	3+4*8, 1+7*8, 5+3*8, 6+3*8,
	2+7*8, 6+4*8, 5+6*8, 7+5*8,
	1+0*8, 2+0*8, 0+3*8, 3+1*8,
	3+2*8, 0+6*8, 4+2*8, 6+1*8,
	2+5*8, 2+6*8, 6+2*8, 5+4*8,
	3+7*8, 7+3*8, 4+7*8, 7+6*8,
	0+1*8, 3+0*8, 0+4*8, 4+0*8,
	2+3*8, 1+5*8, 5+1*8, 5+2*8,
	1+6*8, 3+5*8, 7+1*8, 4+5*8,
	4+6*8, 7+4*8, 5+7*8, 6+7*8,
	0+2*8, 2+1*8, 1+3*8, 5+0*8,
	1+4*8, 2+4*8, 6+0*8, 4+3*8,
	0+7*8, 4+4*8, 7+2*8, 3+6*8,
	5+5*8, 6+5*8, 6+6*8, 7+7*8,
};

static const byte FIELD_SCAN_8X8[64]=
{
	0+0*8, 1+1*8, 2+0*8, 0+7*8,
	2+2*8, 2+3*8, 2+4*8, 3+3*8,
	3+4*8, 4+3*8, 4+4*8, 5+3*8,
	5+5*8, 7+0*8, 6+6*8, 7+4*8,
	0+1*8, 0+3*8, 1+3*8, 1+4*8,
	1+5*8, 3+1*8, 2+5*8, 4+1*8,
	3+5*8, 5+1*8, 4+5*8, 6+1*8,
	5+6*8, 7+1*8, 6+7*8, 7+5*8,
	0+2*8, 0+4*8, 0+5*8, 2+1*8,
	1+6*8, 4+0*8, 2+6*8, 5+0*8,
	3+6*8, 6+0*8, 4+6*8, 6+2*8,
	5+7*8, 6+4*8, 7+2*8, 7+6*8,
	1+0*8, 1+2*8, 0+6*8, 3+0*8,
	1+7*8, 3+2*8, 2+7*8, 4+2*8,
	3+7*8, 5+2*8, 4+7*8, 5+4*8,
	6+3*8, 6+5*8, 7+3*8, 7+7*8,
};

static const byte QP_SCALE_CR[52]=
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,
   12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
   28,29,29,30,31,32,32,33,34,34,35,35,36,36,37,37,
   37,38,38,38,39,39,39,39

};

void update_qp(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int qp)
{
  pDecLocal->qp_scaled[0] = qp;
  pDecLocal->qp_scaled[1] = QP_SCALE_CR[iClip3(0,51,qp+pDecGlobal->chroma_qp_offset[0])];
  pDecLocal->qp_scaled[2] = QP_SCALE_CR[iClip3(0,51,qp+pDecGlobal->chroma_qp_offset[1])];
}

static VO_S32 read_delta_quant(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, DataPartition *dP)
{
  Slice *currSlice = pDecLocal->p_Slice;
  VO_S32 delta_quant;
  delta_quant = read_dQuant_CABAC(currSlice, &dP->c);
  if ((delta_quant < -26) || (delta_quant > 25 ))
    VOH264ERROR(VO_H264_ERR_QP_TOO_LARGE);

  currSlice->qp = ((currSlice->qp + delta_quant + 52)%52);
  //update_qp(pDecGlobal,pDecLocal, currSlice->qp);
  return 0;
}

static inline void cleanblock(short* src) {

	int *pSrc = (int *)src;
	int i = 128*3/2;

	do{
		*(pSrc++) = 0;
		*(pSrc++) = 0;
		*(pSrc++) = 0;
		*(pSrc++) = 0;
		*(pSrc++) = 0;
		*(pSrc++) = 0;
		*(pSrc++) = 0;
		*(pSrc++) = 0;
	}while(i -= 8);

}

void start_macroblock(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Slice *currSlice)
{

  VO_S32 mb_nr;
  //BlockPos mb;
  VO_S32 mb_x = pDecGlobal->mb_x;
  VO_S32 mb_y = pDecGlobal->mb_y;

//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//  {
//	mb_nr = pDecLocal->mbAddrX;
//  }
//  else
//  {
	pDecLocal->mbAddrX = mb_nr = currSlice->current_mb_nr;
//  }

  if (currSlice->mb_aff_frame_flag)
  {
    //int cur_mb_pair = mb_nr >> 1;
    //mb.x = (short) (   (mb_nr) % ((2*pDecGlobal->width) / MB_BLOCK_SIZE));
    //mb.y = (short) (2*((mb_nr) / ((2*pDecGlobal->width) / MB_BLOCK_SIZE)));

    //mb.y += (mb.x & 0x01);
    //mb.x >>= 1;
	
	pDecLocal->direct_mode = pDecLocal->direct_mode_row + 4*mb_x + (mb_y&1)*MB_X_MAX4;

    //mbAddrA = 2 * (cur_mb_pair - 1);
    //mbAddrB = 2 * (cur_mb_pair - pDecGlobal->PicWidthInMbs);
    //mbAddrC = 2 * (cur_mb_pair - pDecGlobal->PicWidthInMbs + 1);
    //mbAddrD = 2 * (cur_mb_pair - pDecGlobal->PicWidthInMbs - 1);

    //pDecLocal->mbAvailA = (Boolean) (mb_is_available(pDecGlobal,pDecLocal,mbAddrA) && ((pDecGlobal->PicPos[cur_mb_pair    ].x)!=0));
    //pDecLocal->mbAvailB = (Boolean) (mb_is_available(pDecGlobal,pDecLocal,mbAddrB));
    //pDecLocal->mbAvailC = (Boolean) (mb_is_available(pDecGlobal,pDecLocal,mbAddrC) && ((pDecGlobal->PicPos[cur_mb_pair + 1].x)!=0));
    //pDecLocal->mbAvailD = (Boolean) (mb_is_available(pDecGlobal,pDecLocal,mbAddrD) && ((pDecGlobal->PicPos[cur_mb_pair    ].x)!=0));
  }
  else
  {
    //mb = pDecGlobal->PicPos[mb_nr];
	pDecLocal->direct_mode = pDecLocal->direct_mode_row + 4*mb_x;
  }
  
  pDecLocal->mb.x = mb_x;
  pDecLocal->mb.y = mb_y;
  pDecLocal->mb_xy = mb_y*pDecGlobal->PicWidthInMbs+mb_x;
  pDecLocal->cbp             = 0;    
 
//#if USE_FRAME_THREAD
//  if (!pDecGlobal->interlace||pDecGlobal->nThdNum <= 1)
//#else
//  if (pDecGlobal->nThdNum <= 1)
//#endif
//  {
    pDecLocal->pix_x       = mb_x * MB_BLOCK_SIZE;        /* horizontal luma pixel position */
    pDecLocal->pix_y       = mb_y * MB_BLOCK_SIZE;        /* vertical luma pixel position */
    pDecLocal->pix_c_x     = mb_x * 8;  /* horizontal chroma pixel position */
    pDecLocal->pix_c_y     = mb_y * 8;  /* vertical chroma pixel position */
	if(currSlice->is_reset_coeff == FALSE)
	{
	  cleanblock(pDecLocal->cof_yuv);
	  currSlice->is_reset_coeff = TRUE;
	}
//  }
  
  if(currSlice->mb_aff_frame_flag&&mb_x==0&&(mb_y&1)==0&&mb_y!=0)
  {
    //int mb_xy= mb.x + mb.y*pDecGlobal->PicWidthInMbs;
    pDecLocal->mb_field = !!IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[(mb_y-1)*pDecGlobal->PicWidthInMbs]);
  }
}

void exit_macroblock(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Slice *currSlice)
{
  int mb_nb = pDecLocal->mb_xy;

  //pDecGlobal->mb_type0_frame_buffer[mb_nb] = pDecLocal->mb_type0;
  pDecGlobal->cbp_frame_buffer[mb_nb] = pDecLocal->cbp;
  
  pDecGlobal->qp_frame_buffer[mb_nb] = pDecLocal->qp_scaled[0];
  pDecGlobal->qpc_frame_buffer[mb_nb*2] = pDecLocal->qp_scaled[1];
  pDecGlobal->qpc_frame_buffer[mb_nb*2+1] = pDecLocal->qp_scaled[2];
  pDecGlobal->slice_number_frame[mb_nb] = currSlice->slice_number;
  

  if(pDecGlobal->pCurSlice->active_pps->entropy_coding_mode_flag == CABAC)
  {
    VO_U8* b8direct_frame_buffer = &pDecGlobal->b8direct_frame_buffer[mb_nb*4];
    if(pDecLocal->mb_type0&(VO_SKIP|VO_IPCM|VO_DIRECT))//(pDecLocal->mb_type==0||pDecLocal->mb_type==IPCM)
    {
      M32(b8direct_frame_buffer) = 0x01010101;
    }
	else
	{
	  VO_S8* b8mode = pDecLocal->b8mode;
	  VO_S8* b8pdir = pDecLocal->b8pdir;
      b8direct_frame_buffer[0] = b8mode[0]==0&&b8pdir[0]==2;
	  b8direct_frame_buffer[1] = b8mode[1]==0&&b8pdir[1]==2;
	  b8direct_frame_buffer[2] = b8mode[2]==0&&b8pdir[2]==2;
	  b8direct_frame_buffer[3] = b8mode[3]==0&&b8pdir[3]==2;
	}
	//memcpy(&pDecGlobal->mvd_frame_buffer[mbx*64],&pDecLocal->mvd[0][0][0][0],64*sizeof(short));
  }
}

static VO_S32 interpret_mb_mode_P(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
  if(mb_type0 > 31||mb_type0<=0)
  	VOH264ERROR(VO_H264_ERR_MB_TYPE);
  if(mb_type0 < 4)
  {
	M32(pDecLocal->b8mode) = mb_type0*0x01010101;
	M32(pDecLocal->b8pdir) = 0x00000000;
  }
  else if((mb_type0 == 4 || mb_type0 == 5))
  {
    pDecLocal->p_Slice->allrefzero = (mb_type0 == 5);
  }
  else if(mb_type0 == 6)
  {
	M32(pDecLocal->b8mode) = 0x0b0b0b0b;
	M32(pDecLocal->b8pdir) = 0xffffffff;
  }
  else
  {
	M32(pDecLocal->b8mode) = 0x00000000;
	M32(pDecLocal->b8pdir) = 0xffffffff;
  }
  return 0;
}


static VO_S32 interpret_mb_mode_I(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
  if(mb_type0 > 25||mb_type0<0)
  	VOH264ERROR(VO_H264_ERR_MB_TYPE);
  if (mb_type0 == 0)
  {    
	M32(pDecLocal->b8mode) = 0x0b0b0b0b;
	M32(pDecLocal->b8pdir) = 0xffffffff;
  }
  else
  {
	M32(pDecLocal->b8mode) = 0x00000000;
	M32(pDecLocal->b8pdir) = 0xffffffff;
  }
  return 0;
}

static VO_S32 interpret_mb_mode_B(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
  static const short offset2pdir16x16[12]   = {0, 0, 1, 2, 0,0,0,0,0,0,0,0};
  static const short offset2pdir16x8[22][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,1},{0,0},{0,1},{0,0},{1,0},
                                             {0,0},{0,2},{0,0},{1,2},{0,0},{2,0},{0,0},{2,1},{0,0},{2,2},{0,0}};
  static const short offset2pdir8x16[22][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,1},{0,0},{0,1},{0,0},
                                             {1,0},{0,0},{0,2},{0,0},{1,2},{0,0},{2,0},{0,0},{2,1},{0,0},{2,2}};

  VO_S32 i;
  if(mb_type0 > 48||mb_type0<0)
  	VOH264ERROR(VO_H264_ERR_MB_TYPE);
  //--- set mbtype, b8type, and b8pdir ---
  if (mb_type0 == 0)       // direct
  {
	M32(pDecLocal->b8mode) = 0x00000000;
	M32(pDecLocal->b8pdir) = 0x02020202;
  }
  else if (mb_type0 == 23) // intra4x4
  {
	M32(pDecLocal->b8mode) = 0x0b0b0b0b;
	M32(pDecLocal->b8pdir) = 0xffffffff;
  }
  else if ((mb_type0 > 23) && (mb_type0 < 48) ) // intra16x16
  {
    M32(pDecLocal->b8mode) = 0x00000000;
	M32(pDecLocal->b8pdir) = 0xffffffff;
  }
  else if (mb_type0 == 22) // 8x8(+split)
  {
  }
  else if (mb_type0 < 4)   // 16x16
  {
	M32(pDecLocal->b8mode) = 0x01010101;
	M32(pDecLocal->b8pdir) = offset2pdir16x16[mb_type0]*0x01010101;
  }
  else if(mb_type0 == 48)
  {
    M32(pDecLocal->b8mode) = 0x00000000;
	M32(pDecLocal->b8pdir) = 0xffffffff;
  }

  else if ((mb_type0 & 0x01) == 0) // 16x8
  {
	M32(pDecLocal->b8mode) = 0x02020202;
    for(i=0;i<4;++i)
    {
      pDecLocal->b8pdir[i] = (char) offset2pdir16x8 [mb_type0][i>>1];
    }
  }
  else
  {
	M32(pDecLocal->b8mode) = 0x03030303;
    for(i=0;i<4; ++i)
    {
      pDecLocal->b8pdir[i] = (char) offset2pdir8x16 [mb_type0][i&0x01];
    }
  }
  return 0;
}

static void init_macroblock_b(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  VO_S32 i;
  VO_S32* mv_cache0 = &pDecLocal->mv_cache[0][12];
  VO_S32* mv_cache1 = &pDecLocal->mv_cache[1][12];
  VO_S8* ref_cache0 = &pDecLocal->ref_cache[0][12];
  VO_S8* ref_cache1 = &pDecLocal->ref_cache[1][12];
  
  for(i = 4;i!= 0;i--)
  {
    mv_cache0[0]=mv_cache0[1]=mv_cache0[2]=mv_cache0[3]=0;
	mv_cache1[0]=mv_cache1[1]=mv_cache1[2]=mv_cache1[3]=0;
	mv_cache0+=8;
	mv_cache1+=8;
	M32(ref_cache0) = M32(ref_cache1) = 0xffffffff;
	ref_cache0+=8;
	ref_cache1+=8;
  }
  if(pDecGlobal->interlace)
  {
    VO_S32* pic_cache0 = (VO_S32*)&pDecLocal->pic_cache[0][0];
    VO_S32* pic_cache1 = (VO_S32*)&pDecLocal->pic_cache[1][0];
	for(i = 4;i!= 0;i--)
    {
      pic_cache0[0]=pic_cache0[1]=pic_cache0[2]=pic_cache0[3]=0;
	  pic_cache1[0]=pic_cache1[1]=pic_cache1[2]=pic_cache1[3]=0;
	  pic_cache0+=4;
	  pic_cache1+=4;
    }
  }
  //memset( &pDecLocal->pic_cache[0][0],0,16*sizeof(StorablePicture*));
  //memset( &pDecLocal->pic_cache[1][0],0,16*sizeof(StorablePicture*));
}

void setup_slice_methods(H264DEC_G *pDecGlobal,Slice *currSlice)
{
  switch (currSlice->slice_type)
  {
  case P_SLICE: 
  case SP_SLICE:
    pDecGlobal->interpret_mb_mode         = interpret_mb_mode_P;
	if(currSlice->active_pps->entropy_coding_mode_flag)
	{
      pDecGlobal->read_motion_info_from_NAL = pDecGlobal->interlace?
                                              read_motion_info_from_NAL_p_slice_interlace:
                                              read_motion_info_from_NAL_p_slice;
	}
	else
    {
      pDecGlobal->read_motion_info_from_NAL = pDecGlobal->interlace?
                                              read_motion_info_from_NAL_p_slice_cavlc_interlace:
                                              read_motion_info_from_NAL_p_slice_cavlc;
	}
    pDecGlobal->read_one_macroblock       = currSlice->active_pps->entropy_coding_mode_flag?
                                            read_one_macroblock_p_slice:
                                            read_one_macroblock_cavlc;
    //pDecGlobal->list_count                = 1;
#if (MVC_EXTENSION_ENABLE)
    pDecGlobal->init_lists                = currSlice->view_id ? init_lists_p_slice_mvc : init_lists_p_slice;
#else
    pDecGlobal->init_lists                = init_lists_p_slice;
#endif
    break;
  case B_SLICE:
    pDecGlobal->interpret_mb_mode         = interpret_mb_mode_B;
    if(currSlice->active_pps->entropy_coding_mode_flag)
	{
      pDecGlobal->read_motion_info_from_NAL = pDecGlobal->interlace?
                                              read_motion_info_from_NAL_b_slice_interlace:
                                              read_motion_info_from_NAL_b_slice;
	}
	else
    {
      pDecGlobal->read_motion_info_from_NAL = pDecGlobal->interlace?
                                              read_motion_info_from_NAL_b_slice_cavlc_interlace:
                                              read_motion_info_from_NAL_b_slice_cavlc;
	}
    pDecGlobal->read_one_macroblock       = currSlice->active_pps->entropy_coding_mode_flag?
                                            read_one_macroblock_b_slice:
                                            read_one_macroblock_cavlc;

    update_direct_types(pDecGlobal,currSlice);
	//pDecGlobal->list_count                = 2;
#if (MVC_EXTENSION_ENABLE)
    pDecGlobal->init_lists                = currSlice->view_id ? init_lists_b_slice_mvc : init_lists_b_slice;
#else
    pDecGlobal->init_lists                = init_lists_b_slice;
#endif
    break;
  case I_SLICE: 
    pDecGlobal->interpret_mb_mode         = interpret_mb_mode_I;
    //pDecGlobal->read_motion_info_from_NAL = NULL;
    pDecGlobal->read_one_macroblock       = currSlice->active_pps->entropy_coding_mode_flag?
                                            read_one_macroblock_i_slice:
                                            read_one_macroblock_cavlc;
    //pDecGlobal->list_count                = 1;
#if (MVC_EXTENSION_ENABLE)
    pDecGlobal->init_lists                = currSlice->view_id ? init_lists_i_slice_mvc : init_lists_i_slice;
#else
    pDecGlobal->init_lists                = init_lists_i_slice;
#endif
    break;
  case SI_SLICE: 
    //pDecGlobal->interpret_mb_mode         = interpret_mb_mode_SI;
    //pDecGlobal->read_motion_info_from_NAL = NULL;
    pDecGlobal->read_one_macroblock       = currSlice->active_pps->entropy_coding_mode_flag?
                                            read_one_macroblock_i_slice:
                                            read_one_macroblock_cavlc;
    //pDecGlobal->list_count                = 1;
#if (MVC_EXTENSION_ENABLE)
    pDecGlobal->init_lists                = currSlice->view_id ? init_lists_i_slice_mvc : init_lists_i_slice;
#else
    pDecGlobal->init_lists                = init_lists_i_slice;
#endif
    break;
  default:
    //printf("Unsupported slice type\n");
    break;
  }
#if 0
  if (currSlice->mb_aff_frame_flag)
  {
   // currSlice->intrapred_chroma = intrapred_chroma_mbaff;        
  }
  else
#endif
    pDecGlobal->intrapred_chroma = intrapred_chroma;   

  switch(currSlice->active_pps->entropy_coding_mode_flag)
  {
  case CABAC:
      pDecGlobal->read_CBP_and_coeffs_from_NAL = read_CBP_and_coeffs_from_NAL_CABAC_420;
    break;
  case CAVLC:
      pDecGlobal->read_CBP_and_coeffs_from_NAL = read_CBP_and_coeffs_from_NAL_CAVLC_420;
    break;
  default:
    //printf("Unsupported entropy coding mode\n");
    break;
  }
}

VO_S32 SetB8Mode (H264DEC_L *pDecLocal, int value, int i)
{
  Slice* currSlice = pDecLocal->p_Slice;
  static const char p_v2b8 [ 5] = {4, 5, 6, 7, IBLOCK};
  static const char p_v2pd [ 5] = {0, 0, 0, 0, -1};
  static const char b_v2b8 [14] = {0, 4, 4, 4, 5, 6, 5, 6, 5, 6, 7, 7, 7, IBLOCK};
  static const char b_v2pd [14] = {2, 0, 1, 2, 0, 0, 1, 1, 2, 2, 0, 1, 2, -1};

  if (currSlice->slice_type==B_SLICE)
  {
    if(value>=13||value<0)
	  VOH264ERROR(VO_H264_ERR_B8MODE);
    pDecLocal->b8mode[i] = b_v2b8[value];
    pDecLocal->b8pdir[i] = b_v2pd[value];
	pDecLocal->b8sub[i] = b_sub_mb_mode_info[value].partition_number;
	pDecLocal->b8type[i] = b_sub_mb_mode_info[value].mb_type;
  }
  else
  {
    if(value>>2)
	  VOH264ERROR(VO_H264_ERR_B8MODE);
    pDecLocal->b8mode[i] = p_v2b8[value];
    pDecLocal->b8pdir[i] = p_v2pd[value];
	pDecLocal->b8sub[i] = p_sub_mb_mode_info[value].partition_number;
	pDecLocal->b8type[i] = p_sub_mb_mode_info[value].mb_type;
  }
  return 0;
}


void writeMotion(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  VO_S32 j,i;
  VO_S32 b_end = pDecGlobal->pCurSlice->slice_type==B_SLICE?2:1;
  assert(pDecLocal->mb_xy < pDecGlobal->FrameSizeInMbs);

  for(j = 0;j < b_end;j++)
  {
    VO_S32 *mv_out1 = pDecGlobal->dec_picture->mv[j]+pDecLocal->mb_xy*16;
    VO_S32 *mv_cache1 = &pDecLocal->mv_cache[j][12];
	VO_S8 *ref_out1 = pDecGlobal->dec_picture->ref_idx[j]+pDecLocal->mb_xy*16;
    VO_S8 *ref_cache1 = &pDecLocal->ref_cache[j][12];
	if(pDecGlobal->interlace)
	{
	  StorablePicture** pic_buffer_list1 = pDecGlobal->dec_picture->ref_pic[j]+pDecLocal->mb_xy*16;
	  memcpy( pic_buffer_list1,&pDecLocal->pic_cache[j][0],16*sizeof(StorablePicture*));
	}
	for(i = 4;i!= 0;i--)
    {
	  *mv_out1++ = *mv_cache1++;
	  *mv_out1++ = *mv_cache1++;
	  *mv_out1++ = *mv_cache1++;
	  *mv_out1++ = *mv_cache1++;
	  CP32(ref_out1,ref_cache1);
	  mv_cache1+=4;
	  ref_out1+=4;
	  ref_cache1+=8;
	}
    if(pDecGlobal->pCurSlice->active_pps->entropy_coding_mode_flag == CABAC)
    {
      int *mvd_out = (int*)&pDecGlobal->mvd_frame_buffer[pDecLocal->mb_xy*16+j*8];
  	  int *mvd_cache = (int*)&pDecLocal->mvd_cache[j][0];
  	  
  	  mvd_out[0]=mvd_cache[36];
	  mvd_out[1]=mvd_cache[37];
	  mvd_out[2]=mvd_cache[38];
	  mvd_out[3]=mvd_cache[39];
	  mvd_out[4]=mvd_cache[31];
	  mvd_out[5]=mvd_cache[23];
	  mvd_out[6]=mvd_cache[15];
    }
  }
}


void skip_macroblock(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  short pred_mv[2];
  int   j;
  VO_S32* mv_cache0 = (VO_S32*)&pDecLocal->mv_cache[0][12];
  VO_S8* ref_cache0 = &pDecLocal->ref_cache[0][12];
  int list_offset = (pDecLocal->mb_field&&!pDecLocal->p_Slice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
  StorablePicture *cur_pic = pDecLocal->p_Slice->listX[pDecLocal->p_Slice->slice_number][LIST_0+list_offset][0];
  VO_S32 *mv_out1 = pDecGlobal->dec_picture->mv[0]+pDecLocal->mb_xy*16;
  VO_S8 *ref_out1 = pDecGlobal->dec_picture->ref_idx[0]+pDecLocal->mb_xy*16;
  
  VO_U8* nz_coeff = pDecGlobal->nz_coeff_frame+pDecLocal->mb_xy*24;

  assert(pDecLocal->mb_xy < pDecGlobal->FrameSizeInMbs);

  if (!(pDecLocal->mbAvailA&&pDecLocal->mbAvailB&&(ref_cache0[-1]|mv_cache0[-1])&&(ref_cache0[-8]|mv_cache0[-8])))
  {
    M32(pred_mv) = 0;
  }
  else
  {
    GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, pred_mv,0,LIST_0);
  }
  if(pDecGlobal->interlace)
  {
    StorablePicture** pic_buffer_list1 = pDecGlobal->dec_picture->ref_pic[0]+pDecLocal->mb_xy*16;
    for(j = 4;j!= 0;j--)
    {
      pic_buffer_list1[0] = pic_buffer_list1[1] = pic_buffer_list1[2] = pic_buffer_list1[3] = cur_pic;
	  pic_buffer_list1+=4;
    }
  }
  for(j = 4;j!= 0;j--)
  {
    mv_out1[0]=mv_out1[1]=mv_out1[2]=mv_out1[3]=M32(&pred_mv);
	
	M32(ref_out1) = 0;
	M32(nz_coeff) = 0;
	nz_coeff+=4;
	ref_out1+=4;
	mv_out1+=4;
	
  }
  M32(nz_coeff)=M32(nz_coeff+4)=0;
  pDecLocal->pre_skip = 1;
}

void writeIntrapredMode(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
	VO_S8 *ipredmode_out = pDecGlobal->ipredmode_frame + (pDecLocal->mb_xy<<4);
	VO_S8 *ipredmode = pDecLocal->ipredmode;
	CP32(ipredmode_out,ipredmode+12);
	CP32(ipredmode_out+4,ipredmode+20);
	CP32(ipredmode_out+8,ipredmode+28);
	CP32(ipredmode_out+12,ipredmode+36);
}

void writeNZCoeff(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  VO_U8 *nz_coeff_frame = pDecGlobal->nz_coeff_frame+pDecLocal->mb_xy*24;
  VO_U8 *nz_coeff = pDecLocal->nz_coeff;
  CP32(nz_coeff_frame,nz_coeff+12);
  CP32(nz_coeff_frame+4,nz_coeff+20);
  CP32(nz_coeff_frame+8,nz_coeff+28);
  CP32(nz_coeff_frame+12,nz_coeff+36);
  *(nz_coeff_frame+16) = *(nz_coeff+9);
  *(nz_coeff_frame+17) = *(nz_coeff+10);
  *(nz_coeff_frame+18) = *(nz_coeff+17);
  *(nz_coeff_frame+19) = *(nz_coeff+18);
  *(nz_coeff_frame+20) = *(nz_coeff+33);
  *(nz_coeff_frame+21) = *(nz_coeff+34);
  *(nz_coeff_frame+22) = *(nz_coeff+41);
  *(nz_coeff_frame+23) = *(nz_coeff+42);
}

VO_S32 get_intra_dc_mode(H264DEC_L *pDecLocal, int predmode)
{
  static const VO_S8 top_mode [7]= {DC_LEFT_PRED_16, 1,-1,-1};
  static const VO_S8 left_mode[7]= {DC_TOP_PRED_16,-1, 2,-1,DC_128_PRED_16};

  if(predmode > 6U)
    return -1;

  if(!(pDecLocal->intra_ava_top&0x8000))
  {
    predmode= top_mode[ predmode ];
    if(predmode<0)
	  return -1;
  }

  if((pDecLocal->intra_ava_left&0x8080) != 0x8080)
  {
    predmode= left_mode[ predmode ];
    if(predmode<0)
	  return -1;
  }

  return predmode;
}

VO_S32 get_intra4x4_dc_mode(H264DEC_G *pDecGlobal, H264DEC_L *pDecLocal)
{
  static const VO_S8 top_mode [12]= {-1, 0,DC_LEFT_PRED,-1,-1,-1,-1,-1, 0};
  static const VO_S8 left_mode[12]= { 0,-1, DC_TOP_PRED, 0,-1,-1,-1, 0,-1,DC_128_PRED};
  int i;

  const VO_S8 *ipred_g = pDecGlobal->ipredmode_frame + pDecLocal->mb_xy*16;
  VO_S8 *ipredmode = pDecLocal->ipredmode + cache_scan[0];
  CP32(ipredmode,ipred_g);
  CP32(ipredmode+8,ipred_g+4);
  CP32(ipredmode+16,ipred_g+8);
  CP32(ipredmode+24,ipred_g+12);

  if(!(pDecLocal->intra_ava_top&0x8000))
  {
	VO_S8 *ipredmode = pDecLocal->ipredmode + cache_scan[0];
    for(i=0; i<4; i++)
	{
		int mode= top_mode[ *(ipredmode) ];
      if(mode<0)
        return -1;
      else if(mode)
        *(ipredmode) = mode;
	  ++ipredmode;
    }
  }

  if((pDecLocal->intra_ava_left&0x8888)!=0x8888)
  {
    static const int pos_mask[4]={0x8000,0x2000,0x80,0x20};
	VO_S8 *ipredmode = pDecLocal->ipredmode + cache_scan[0];
    for(i=0; i<4; i++)
	{
      if(!(pDecLocal->intra_ava_left&pos_mask[i]))
	  {
		  int mode= left_mode[ *ipredmode ];
        if(mode<0)
          return -1;
        else if(mode)
           *ipredmode = mode;
      }
	  ipredmode += 8;
    }
  }

  return 0;
} //FIXME cleanup like ff_h264_check_intra_pred_mode

static void update_pos(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Slice *currSlice)
{
  int mb_xy = pDecLocal->mb_xy;//= pDecLocal->mb.y*pDecGlobal->PicWidthInMbs+pDecLocal->mb.x;
  int topleft_xy, top_xy, topright_xy, left_xy[2];
  static const VO_U8 left_block_options[4][16]=
  {
    {0,1,2,3,7,10,8,11,3+0*4, 3+1*4, 3+2*4, 3+3*4, 17, 21, 19, 23},
    {2,2,3,3,8,11,8,11,3+2*4, 3+2*4, 3+3*4, 3+3*4, 19, 23, 19, 23},
    {0,0,1,1,7,10,7,10,3+0*4, 3+0*4, 3+1*4, 3+1*4, 17, 21, 17, 21},
    {0,2,0,2,7,10,7,10,3+0*4, 3+2*4, 3+0*4, 3+2*4, 17, 21, 17, 21}
  };
  int mb_stride = pDecGlobal->PicWidthInMbs;
  VO_S32 *mb_type0_frame_buffer = pDecGlobal->mb_type0_frame_buffer;
  top_xy     = mb_xy  - (mb_stride << pDecLocal->mb_field);
  topleft_xy = top_xy - 1;
  topright_xy= top_xy + 1;
  left_xy[1] = left_xy[0] = mb_xy-1;
  pDecLocal->left_block = left_block_options[0];
  pDecLocal->topleft_pos = -1;
  if(pDecLocal->mb_field&&pDecLocal->mb.y&1)
  {
    pDecLocal->pix_y -= 15;
	pDecLocal->pix_c_y -= 7;
  }
  if(currSlice->mb_aff_frame_flag)
  {
    int left_mb_field = !!IS_INTERLACED(mb_type0_frame_buffer[mb_xy-1]);
    int curr_mb_field = pDecLocal->mb_field;
    if(pDecLocal->mb.y&1)
	{
      if (left_mb_field != curr_mb_field) 
	  {
        left_xy[1] = left_xy[0] = mb_xy - mb_stride - 1;
        if (curr_mb_field) 
		{
          left_xy[1] += mb_stride;
          pDecLocal->left_block = left_block_options[3];
        } 
		else 
		{
          topleft_xy += mb_stride;
		  pDecLocal->topleft_pos = 0;
          pDecLocal->left_block = left_block_options[1];
        }
      }
    }
	else
	{
      if(top_xy>=0&&curr_mb_field)
	  {
        topleft_xy  += mb_stride & ((!!IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[top_xy-1])&1)-1);
        topright_xy += mb_stride & ((!!IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[top_xy+1])&1)-1);
        top_xy      += mb_stride & ((!!IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[top_xy])&1)-1);
      }
      if (left_mb_field != curr_mb_field) 
	  {
        if (curr_mb_field) 
		{
          left_xy[1] += mb_stride;
          pDecLocal->left_block = left_block_options[3];
        } 
		else 
		{
          pDecLocal->left_block = left_block_options[2];
        }
      }
    }
  }

  pDecLocal->topleft_mb_xy = topleft_xy;
  pDecLocal->top_mb_xy     = top_xy;
  pDecLocal->topright_mb_xy= topright_xy;
  pDecLocal->left_mb_xy[0] = left_xy[0];
  pDecLocal->left_mb_xy[1] = left_xy[1];
  pDecLocal->top_type     = top_xy>=0?mb_type0_frame_buffer[top_xy]:0;
  pDecLocal->left_type[0] = mb_type0_frame_buffer[left_xy[0]] ;
  pDecLocal->left_type[1] = mb_type0_frame_buffer[left_xy[1]] ;
  if (currSlice->mb_aff_frame_flag)
  {
    pDecLocal->mbAvailA = (Boolean) (mb_is_available(pDecGlobal,pDecLocal,left_xy[0]) && pDecLocal->mb.x);
    pDecLocal->mbAvailB = (Boolean) (mb_is_available_mbaff(pDecGlobal,pDecLocal,top_xy));
    pDecLocal->mbAvailC = (Boolean) (mb_is_available_mbaff(pDecGlobal,pDecLocal,topright_xy) && ((pDecGlobal->PicPos[mb_xy + 1].x)!=0)&&((!(pDecLocal->mb.y&1))||(pDecLocal->mb.y>1&&pDecLocal->mb_field&&pDecLocal->mb.y&1)));
    pDecLocal->mbAvailD = (Boolean) (mb_is_available_mbaff(pDecGlobal,pDecLocal,topleft_xy) && ((pDecLocal->mb.x)!=0));
  }
  else
  {
    pDecLocal->mbAvailA = (Boolean) (mb_is_available(pDecGlobal,pDecLocal,left_xy[0]) && ((pDecLocal->mb.x)!=0));
    pDecLocal->mbAvailD = (Boolean) (mb_is_available(pDecGlobal,pDecLocal,topleft_xy) && ((pDecLocal->mb.x)!=0));
    pDecLocal->mbAvailC = (Boolean) (mb_is_available(pDecGlobal,pDecLocal,topright_xy) && ((pDecGlobal->PicPos[mb_xy + 1].x)!=0));
    pDecLocal->mbAvailB = (Boolean) (mb_is_available(pDecGlobal,pDecLocal,top_xy));
  }
  //if(!pDecLocal->mbAvailB)
  //	pDecLocal->top_type = 0;
  //if(!pDecLocal->mbAvailA)
  //	pDecLocal->left_type[1]=pDecLocal->left_type[0] = 0;
}

static void update_cache_b_direct(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Slice *currSlice)
{ 
  VO_S32 left_ava = pDecLocal->mbAvailA;
  VO_S32 top_ava = pDecLocal->mbAvailB;
  VO_S8 *ipredmode = pDecLocal->ipredmode;
  VO_S32 cabac = currSlice->active_pps->entropy_coding_mode_flag == CABAC;
  VO_S32 i;
  VO_S32 top_xy = pDecLocal->top_mb_xy;
  VO_S32* left_xy = pDecLocal->left_mb_xy;
  const VO_U8* left_block = pDecLocal->left_block;

  {
    VO_S32 j;
		
	VO_S32* mb_type0_frame_buffer = pDecGlobal->mb_type0_frame_buffer;
	for(j=0;j < 2;j++)
	{
      VO_S32 *mv_cache0 = &pDecLocal->mv_cache[j][0]; 
      VO_S8 *ref_cache0 = &pDecLocal->ref_cache[j][0];
	  VO_S32* mv_out0 = pDecGlobal->dec_picture->mv[j];
      VO_S8* ref_out0 = pDecGlobal->dec_picture->ref_idx[j];
      VO_S32* mv_out = pDecGlobal->dec_picture->mv[j]+top_xy*16+12;
      VO_S8* ref_out = pDecGlobal->dec_picture->ref_idx[j]+top_xy*16+12;
      if(pDecLocal->mbAvailB&&!IS_INTRA0(pDecLocal->top_type))
  	  {
        mv_cache0[4] = mv_out[0];
        mv_cache0[5] = mv_out[1];
        mv_cache0[6] = mv_out[2];
        mv_cache0[7] = mv_out[3];
        M32(&ref_cache0[4]) = M32(&ref_out[0]);
      }
  	  else
  	  {
        mv_cache0[4] = mv_cache0[5] = mv_cache0[6] = mv_cache0[7] = 0;
  	    M32(&ref_cache0[4]) = pDecLocal->mbAvailB?0xffffffff:0xfefefefe;
      }
  
  	  //if(mb_type & (MB_TYPE_16x8|MB_TYPE_8x8))
  	  {
        for(i=0; i<2; i++)
  	    {
          VO_S32 c_idx = 11 + i*16;
          if(pDecLocal->mbAvailA&&!IS_INTRA0(pDecLocal->left_type[i]))
  		  {
            VO_S32 l_xy = left_xy[i]*16+3;
            mv_cache0[c_idx  ]= mv_out0[l_xy+4*left_block[0+i*2]];
            mv_cache0[c_idx+8]= mv_out0[l_xy+4*left_block[1+i*2]];
            ref_cache0[c_idx  ]= ref_out0[l_xy+4*left_block[0+i*2]];
            ref_cache0[c_idx+8]= ref_out0[l_xy+4*left_block[1+i*2]];
          }
  		  else
  		  {
            mv_cache0[c_idx  ] = 0;
            mv_cache0[c_idx+8] = 0;
            ref_cache0[c_idx  ]=ref_cache0[c_idx+8]= pDecLocal->mbAvailA?-1:-2;
          }
        }
      }
	  if(pDecLocal->mbAvailC&&!IS_INTRA0(mb_type0_frame_buffer[pDecLocal->topright_mb_xy]))
	  {
        mv_cache0[8]= mv_out0[pDecLocal->topright_mb_xy*16+12];
        ref_cache0[8]= ref_out0[pDecLocal->topright_mb_xy*16+12];
      }
	  else
	  {
        mv_cache0[8] = 0;
        ref_cache0[8] = pDecLocal->mbAvailC?-1:-2;
      }
      if(ref_cache0[8] < 0)
	  {
        if(pDecLocal->mbAvailD&&!IS_INTRA0(mb_type0_frame_buffer[pDecLocal->topleft_mb_xy]))
		{
          mv_cache0[3]= mv_out0[pDecLocal->topleft_mb_xy*16+7+(pDecLocal->topleft_pos&8)];
          ref_cache0[3]= ref_out0[pDecLocal->topleft_mb_xy*16+7+(pDecLocal->topleft_pos&8)];
        }
		else
		{
          mv_cache0[3] = 0;
          ref_cache0[3] = pDecLocal->mbAvailD?-1:-2;
        }
      }
	  ref_cache0[16] = -2;
      ref_cache0[24] = -2;
      ref_cache0[32] = -2;

	  if(currSlice->mb_aff_frame_flag)
	  { 
	    VO_S16 *mv;
		
	    if(pDecLocal->mb_field)
	    {
	      if(pDecLocal->topleft_mb_xy>=0&&!IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[pDecLocal->topleft_mb_xy])&& ref_cache0[3] >= 0)
	      {
	        ref_cache0[3] <<= 1;
			mv = (VO_S16*)&mv_cache0[3];
			mv[1]/=2;
			
	      }
		  if(!IS_INTERLACED(pDecLocal->top_type))
	      {
	        for(i=4;i<8;i++)
	        {
 	          if(ref_cache0[i] >= 0)
 	          {
   	            ref_cache0[i] <<= 1;
   			    mv = (VO_S16*)&mv_cache0[i];
   			    mv[1]/=2;
   			    
 	          }
	        }			
	      }
		  if(pDecLocal->topright_mb_xy>0&&!IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[pDecLocal->topright_mb_xy])&& ref_cache0[8] >= 0)
	      {
	        ref_cache0[8] <<= 1;
			mv = (VO_S16*)&mv_cache0[8];
			mv[1]/=2;
			
	      }
		  if(!IS_INTERLACED(pDecLocal->left_type[0]))
	      {
 	        if(ref_cache0[11] >= 0)
 	        {
   	          ref_cache0[11] <<= 1;
   			  mv = (VO_S16*)&mv_cache0[11];
   			  mv[1]/=2;
   			  
 	        }
			if(ref_cache0[19] >= 0)
 	        {
   	          ref_cache0[19] <<= 1;
   			  mv = (VO_S16*)&mv_cache0[19];
   			  mv[1]/=2;
   			  
 	        }
	      }
		  if(!IS_INTERLACED(pDecLocal->left_type[1]))
	      {
 	        if(ref_cache0[27] >= 0)
 	        {
   	          ref_cache0[27] <<= 1;
   			  mv = (VO_S16*)&mv_cache0[27];
   			  mv[1]/=2;
   			  
 	        }
			if(ref_cache0[35] >= 0)
 	        {
   	          ref_cache0[35] <<= 1;
   			  mv = (VO_S16*)&mv_cache0[35];
   			  mv[1]/=2;
   			  
 	        }
	      }
	    }
		else
		{
		  if(pDecLocal->topleft_mb_xy>=0&&IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[pDecLocal->topleft_mb_xy])&& ref_cache0[3] >= 0)
	      {
	        ref_cache0[3] >>= 1;
			mv = (VO_S16*)&mv_cache0[3];
			mv[1]<<=1;
			
	      }
		  if(IS_INTERLACED(pDecLocal->top_type))
	      {
	        for(i=4;i<8;i++)
	        {
 	          if(ref_cache0[i] >= 0)
 	          {
   	            ref_cache0[i] >>= 1;
   			    mv = (VO_S16*)&mv_cache0[i];
   			    mv[1]<<=1;
   			    
 	          }
	        }			
	      }
		  if(pDecLocal->topright_mb_xy>0&&IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[pDecLocal->topright_mb_xy])&& ref_cache0[8] >= 0)
	      {
	        ref_cache0[8] >>= 1;
			mv = (VO_S16*)&mv_cache0[8];
			mv[1]<<=1;
			
	      }
		  if(IS_INTERLACED(pDecLocal->left_type[0]))
	      {
 	        if(ref_cache0[11] >= 0)
 	        {
   	          ref_cache0[11] >>= 1;
   			  mv = (VO_S16*)&mv_cache0[11];
   			  mv[1]<<=1;
   			  
 	        }
			if(ref_cache0[19] >= 0)
 	        {
   	          ref_cache0[19] >>= 1;
   			  mv = (VO_S16*)&mv_cache0[19];
   			  mv[1]<<=1;
   			  
 	        }
	      }
		  if(IS_INTERLACED(pDecLocal->left_type[1]))
	      {
 	        if(ref_cache0[27] >= 0)
 	        {
   	          ref_cache0[27] >>= 1;
   			  mv = (VO_S16*)&mv_cache0[27];
   			  mv[1]<<=1;
   			  
 	        }
			if(ref_cache0[35] >= 0)
 	        {
   	          ref_cache0[35] >>= 1;
   			  mv = (VO_S16*)&mv_cache0[35];
   			  mv[1]<<=1;
   			  
 	        }
	      }
		}
	  }
	}
  }
  
}


static void update_cache(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Slice *currSlice,VO_S32 mb_type0)
{
  VO_U8* nz_coeff = pDecLocal->nz_coeff; 
  VO_S32 left_ava = pDecLocal->mbAvailA;
  VO_S32 top_ava = pDecLocal->mbAvailB;
  VO_S8 *ipredmode = pDecLocal->ipredmode;
  VO_S32 cabac = currSlice->active_pps->entropy_coding_mode_flag == CABAC;
  VO_S32 i;
  VO_S32 top_xy = pDecLocal->top_mb_xy;
  VO_S32* left_xy = pDecLocal->left_mb_xy;
  const VO_U8* left_block = pDecLocal->left_block;

  if(!IS_SKIP(mb_type0))
  {
  if(cabac)
  {
    VO_S32 *cbp_frame_buffer = pDecGlobal->cbp_frame_buffer;
    pDecLocal->lu_transform8x8_flag = left_ava?!!IS_8x8DCT(pDecLocal->left_type[0]):0;
    pDecLocal->lu_transform8x8_flag += top_ava?!!IS_8x8DCT(pDecLocal->top_type):0;
	if(top_ava)
      pDecLocal->cbp_up = cbp_frame_buffer[top_xy];
    else 
      pDecLocal->cbp_up = IS_INTRA0(mb_type0) ? 0x1CF : 0x00F;
    if (left_ava)
      pDecLocal->cbp_left = (cbp_frame_buffer[left_xy[0]] & 0x1f0)
                        |  ((cbp_frame_buffer[left_xy[0]]>>(left_block[0]&(~1)))&2)
                        | (((cbp_frame_buffer[left_xy[1]]>>(left_block[2]&(~1)))&2) << 2);
    else 
      pDecLocal->cbp_left = IS_INTRA0(mb_type0) ? 0x1CF : 0x00F;
         
  }
 
  //if(mb_type != 0)
  {
    if(left_ava)
    {
      for(i = 0;i < 2;i++)
	  {
        VO_U8 *nz_coeff_frame = pDecGlobal->nz_coeff_frame+left_xy[i]*24;
        nz_coeff[3+8*1 + 2*8*i]= nz_coeff_frame[left_block[8+0+2*i]];
        nz_coeff[3+8*2 + 2*8*i]= nz_coeff_frame[left_block[8+1+2*i]];
        nz_coeff[0+8*1 +   8*i]= nz_coeff_frame[left_block[8+4+2*i]];
        nz_coeff[0+8*4 +   8*i]= nz_coeff_frame[left_block[8+5+2*i]];
      }
    }
	else if(cabac && !IS_INTRA0(mb_type0))
	{
	  nz_coeff[11] = 0;
      nz_coeff[19] = 0;
      nz_coeff[27] = 0;
      nz_coeff[35] = 0;
      nz_coeff[8 ] = 0;
      nz_coeff[16] = 0;
      nz_coeff[32] = 0;
      nz_coeff[40] = 0;
	}
	else
	{
	  nz_coeff[11] = 64;
      nz_coeff[19] = 64;
      nz_coeff[27] = 64;
      nz_coeff[35] = 64;
      nz_coeff[8 ] = 64;
      nz_coeff[16] = 64;
      nz_coeff[32] = 64;
      nz_coeff[40] = 64;
	}
	if(top_ava)
    {
      VO_U8 *nz_coeff_frame = pDecGlobal->nz_coeff_frame+top_xy*24;
	  VO_U32 tmp;
	  CP32(nz_coeff+4,nz_coeff_frame+12);
	  tmp = M16(nz_coeff_frame+18);
	  nz_coeff[1] = (VO_U8)(tmp&255);
	  nz_coeff[2] = (VO_U8)(tmp>>8);
	  tmp = M16(nz_coeff_frame+22);
	  nz_coeff[25] = (VO_U8)(tmp&255);
	  nz_coeff[26] = (VO_U8)(tmp>>8);
//       CP16(nz_coeff+1,nz_coeff_frame+18);
//       CP16(nz_coeff+25,nz_coeff_frame+22);
	}
	else if(cabac && !IS_INTRA0(mb_type0))
	{
	  M32(nz_coeff+4) = 0;
	  nz_coeff[1] = 0;
	  nz_coeff[2] = 0;
	  nz_coeff[25] = 0;
	  nz_coeff[26] = 0;
// 	  M16(nz_coeff+1) = 0;
// 	  M16(nz_coeff+25) = 0;
	}
	else
	{
	  M32(nz_coeff+4) = 0x40404040;
	  nz_coeff[1] = 0x40;
	  nz_coeff[2] = 0x40;
	  nz_coeff[25] = 0x40;
	  nz_coeff[26] = 0x40;
// 	  M16(nz_coeff+1) = 0x4040;
// 	  M16(nz_coeff+25) = 0x4040;
	}
  }
  M32(nz_coeff+12) = 0;
  M32(nz_coeff+20) = 0;
  M32(nz_coeff+28) = 0;
  M32(nz_coeff+36) = 0;
  nz_coeff[9] = 0;
  nz_coeff[10] = 0;
  nz_coeff[17] = 0;
  nz_coeff[18] = 0;
  nz_coeff[33] = 0;
  nz_coeff[34] = 0;
  nz_coeff[41] = 0;
  nz_coeff[42] = 0;

  if(IS_INTRA0(mb_type0))
  {
    VO_S32 constrained_intra_pred_flag = currSlice->active_pps->constrained_intra_pred_flag;
	VO_S32* mb_type0_frame_buffer = pDecGlobal->mb_type0_frame_buffer;
    VO_S32 top_ava = constrained_intra_pred_flag ?IS_INTRA0(pDecLocal->top_type)&&pDecLocal->mbAvailB:pDecLocal->mbAvailB;
    VO_S32 left_ava[2];
	int left_mb_field = pDecLocal->mbAvailA?!!IS_INTERLACED(pDecLocal->left_type[0]):0;
    int curr_mb_field = pDecLocal->mb_field;
    left_ava[0] = constrained_intra_pred_flag ?IS_INTRA0(pDecLocal->left_type[0])&&pDecLocal->mbAvailA:pDecLocal->mbAvailA;
    left_ava[1] = constrained_intra_pred_flag ?IS_INTRA0(pDecLocal->left_type[1])&&pDecLocal->mbAvailA:pDecLocal->mbAvailA;
    //VO_S32 intra_mask = pDecGlobal->active_pps->constrained_intra_pred_flag ? 0x07 : -1;
    pDecLocal->intra_ava_left = pDecLocal->intra_ava_topleft
		                         = pDecLocal->intra_ava_top = 0xFFFF;
    pDecLocal->intra_ava_topright = 0xEEEA;

	if(!top_ava)
	{
       pDecLocal->intra_ava_topleft= 0xB3FF;
       pDecLocal->intra_ava_top= 0x33FF;
       pDecLocal->intra_ava_topright= 0x26EA;
    }
	if(curr_mb_field != left_mb_field)
	{
      if(curr_mb_field)
	  {
        if(!left_ava[0])
		{
          pDecLocal->intra_ava_topleft&= 0xDFFF;
          pDecLocal->intra_ava_left&= 0x5FFF;
        }
        if(!left_ava[1])
		{
          pDecLocal->intra_ava_topleft&= 0xFF5F;
          pDecLocal->intra_ava_left&= 0xFF5F;
        }
      }
	  else
	  {
		int left_ava_tmp = constrained_intra_pred_flag ?IS_INTRA0(mb_type0_frame_buffer[left_xy[0]+pDecGlobal->PicWidthInMbs])&&pDecLocal->mbAvailA:pDecLocal->mbAvailA;
        if(!(left_ava_tmp && left_ava[1]))
		{
          pDecLocal->intra_ava_topleft&= 0xDF5F;
          pDecLocal->intra_ava_left&= 0x5F5F;
        }
      }
    }
	else
	{
      if(!left_ava[0])
	  {
        pDecLocal->intra_ava_topleft&= 0xDF5F;
        pDecLocal->intra_ava_left&= 0x5F5F;
      }
    }

	if(constrained_intra_pred_flag)
	{
	  if(!(pDecLocal->mbAvailC&&IS_INTRA0(mb_type0_frame_buffer[pDecLocal->topright_mb_xy])))
	  	pDecLocal->intra_ava_topright&= 0xFBFF;
	  if(!(pDecLocal->mbAvailD&&IS_INTRA0(mb_type0_frame_buffer[pDecLocal->topleft_mb_xy])))
	  	pDecLocal->intra_ava_topleft&= 0x7FFF;
	}
	else
	{
	  if(!pDecLocal->mbAvailC)
	  	pDecLocal->intra_ava_topright&= 0xFBFF;
	  if(!pDecLocal->mbAvailD)
	  	pDecLocal->intra_ava_topleft&= 0x7FFF;
	}
			
    if(IS_INTRA4x4(mb_type0))
    {
      if(top_ava)
      {
		  VO_S8 *ipredmodeline= pDecGlobal->ipredmode_frame+16*top_xy+12;
		  CP32(ipredmode+4,ipredmodeline);
  	    if (constrained_intra_pred_flag)
        {
          if(!IS_INTRA0(pDecLocal->top_type))
            M32(ipredmode+4) = 0xffffffff;
  		  else if(pDecLocal->top_type&6)
            M32(ipredmode+4) = 0x02020202;
  	    }
  	    else if(!IS_INTRA4x4(pDecLocal->top_type))
          M32(ipredmode+4) = 0x02020202;
      }
      else
      {
    	M32(ipredmode+4) = 0xffffffff;
      }
      if(left_ava[0])
      {
        for(i = 0;i < 2;i++)
    	{
    	  if(IS_INTRA4x4(pDecLocal->left_type[i]))
          {
			  VO_S8 *ipredmodeline= pDecGlobal->ipredmode_frame+16*left_xy[i];
			  ipredmode[3+8*1 + 2*8*i]= ipredmodeline[3+left_block[0+2*i]*4];
			  ipredmode[3+8*2 + 2*8*i]= ipredmodeline[3+left_block[1+2*i]*4];
    	  }
    	  else
    	  {
    		ipredmode[3+8*1 + 2*8*i]=ipredmode[3+8*2 + 2*8*i]
    		= 2-3*(constrained_intra_pred_flag&!IS_INTRA0(pDecLocal->left_type[i]));
    	  }
    	}
      }
      else
      {
    	ipredmode[11] = ipredmode[19] = ipredmode[27] = ipredmode[35] = -1;
      }
    }
  }
}
  if((mb_type0)&(VO_16x16|VO_16x8|VO_8x16|VO_8x8|VO_SKIP|VO_DIRECT))
  {
    VO_S32 j;
	VO_S32 b_end = currSlice->slice_type==B_SLICE?2:1;
		
	VO_S32* mb_type0_frame_buffer = pDecGlobal->mb_type0_frame_buffer;
	for(j=0;j < b_end;j++)
	{
      VO_S32 *mv_cache0 = &pDecLocal->mv_cache[j][0]; 
      VO_S8 *ref_cache0 = &pDecLocal->ref_cache[j][0];
	  VO_S32* mv_out0 = pDecGlobal->dec_picture->mv[j];
      VO_S8* ref_out0 = pDecGlobal->dec_picture->ref_idx[j];
      VO_S32* mv_out = pDecGlobal->dec_picture->mv[j]+top_xy*16+12;
      VO_S8* ref_out = pDecGlobal->dec_picture->ref_idx[j]+top_xy*16+12;
      if(pDecLocal->mbAvailB&&!IS_INTRA0(pDecLocal->top_type))
  	  {
        mv_cache0[4] = mv_out[0];
        mv_cache0[5] = mv_out[1];
        mv_cache0[6] = mv_out[2];
        mv_cache0[7] = mv_out[3];
        M32(&ref_cache0[4]) = M32(&ref_out[0]);
      }
  	  else
  	  {
        mv_cache0[4] = mv_cache0[5] = mv_cache0[6] = mv_cache0[7] = 0;
  	    M32(&ref_cache0[4]) = pDecLocal->mbAvailB?0xffffffff:0xfefefefe;
      }
  
  	  //if(mb_type & (MB_TYPE_16x8|MB_TYPE_8x8))
  	  {
        for(i=0; i<2; i++)
  	    {
          VO_S32 c_idx = 11 + i*16;
          if(pDecLocal->mbAvailA&&!IS_INTRA0(pDecLocal->left_type[i]))
  		  {
            VO_S32 l_xy = left_xy[i]*16+3;
            mv_cache0[c_idx  ]= mv_out0[l_xy+4*left_block[0+i*2]];
            mv_cache0[c_idx+8]= mv_out0[l_xy+4*left_block[1+i*2]];
            ref_cache0[c_idx  ]= ref_out0[l_xy+4*left_block[0+i*2]];
            ref_cache0[c_idx+8]= ref_out0[l_xy+4*left_block[1+i*2]];
          }
  		  else
  		  {
            mv_cache0[c_idx  ] = 0;
            mv_cache0[c_idx+8] = 0;
            ref_cache0[c_idx  ]=ref_cache0[c_idx+8]= pDecLocal->mbAvailA?-1:-2;
          }
        }
      }
	  if(pDecLocal->mbAvailC&&!IS_INTRA0(mb_type0_frame_buffer[pDecLocal->topright_mb_xy]))
	  {
        mv_cache0[8]= mv_out0[pDecLocal->topright_mb_xy*16+12];
        ref_cache0[8]= ref_out0[pDecLocal->topright_mb_xy*16+12];
      }
	  else
	  {
        mv_cache0[8] = 0;
        ref_cache0[8] = pDecLocal->mbAvailC?-1:-2;
      }
      if(ref_cache0[8] < 0)
	  {
        if(pDecLocal->mbAvailD&&!IS_INTRA0(mb_type0_frame_buffer[pDecLocal->topleft_mb_xy]))
		{
          mv_cache0[3]= mv_out0[pDecLocal->topleft_mb_xy*16+7+(pDecLocal->topleft_pos&8)];
          ref_cache0[3]= ref_out0[pDecLocal->topleft_mb_xy*16+7+(pDecLocal->topleft_pos&8)];
        }
		else
		{
          mv_cache0[3] = 0;
          ref_cache0[3] = pDecLocal->mbAvailD?-1:-2;
        }
      }
	  ref_cache0[16] = -2;
      ref_cache0[24] = -2;
      ref_cache0[32] = -2;
	  if(!IS_SKIP(mb_type0))
	  {
	    mv_cache0[14] = 0;
		mv_cache0[30] = 0;
        ref_cache0[14] = -2;
		ref_cache0[30] = -2;
	  }

	  if( cabac &&!IS_SKIP(mb_type0)) 
	  {
	    VO_S32 *mvd_cache =&pDecLocal->mvd_cache[j][0];
		VO_S32 *mvd_frame_buffer = pDecGlobal->mvd_frame_buffer;		
        if(pDecLocal->mbAvailB&&!IS_INTRA0(pDecLocal->top_type)&&!IS_SKIP(pDecLocal->top_type))
		{
          VO_S32 *mvd_out = &mvd_frame_buffer[top_xy*16+8*j];
	      mvd_cache[4] = mvd_out[0];
          mvd_cache[5] = mvd_out[1];
          mvd_cache[6] = mvd_out[2];
          mvd_cache[7] = mvd_out[3];
        }
		else
		{
          mvd_cache[4] = mvd_cache[5] = mvd_cache[6] = mvd_cache[7] = 0;
        }
        if(pDecLocal->mbAvailA&&!IS_INTRA0(pDecLocal->left_type[0])&&!IS_SKIP(pDecLocal->left_type[0]))
		{
          VO_S32 b_xy0= left_xy[0]*16+8*j+6;
          mvd_cache[11] = mvd_frame_buffer[b_xy0-left_block[0]];
          mvd_cache[19] = mvd_frame_buffer[b_xy0-left_block[1]];
        }
		else
		{
          mvd_cache[11] = mvd_cache[19] = 0;
        }
		if(pDecLocal->mbAvailA&&!IS_INTRA0(pDecLocal->left_type[1])&&!IS_SKIP(pDecLocal->left_type[1]))
		{
		  VO_S32 b_xy1= left_xy[1]*16+8*j+6;
		  mvd_cache[27] = mvd_frame_buffer[b_xy1-left_block[2]];
          mvd_cache[35] = mvd_frame_buffer[b_xy1-left_block[3]];	
        }
		else
		{
          mvd_cache[27] = mvd_cache[35] = 0;
        }
		mvd_cache[12] = mvd_cache[13] = mvd_cache[14] = mvd_cache[15] = 0;
		mvd_cache[20] = mvd_cache[21] = mvd_cache[22] = mvd_cache[23] = 0;
		mvd_cache[28] = mvd_cache[29] = mvd_cache[30] = mvd_cache[31] = 0;
		mvd_cache[36] = mvd_cache[37] = mvd_cache[38] = mvd_cache[39] = 0;
		if(currSlice->slice_type == B_SLICE)
		{
		  VO_S8* b8cache = pDecLocal->b8cache;
		  VO_U8 *b8direct_frame_buffer = pDecGlobal->b8direct_frame_buffer;
		  if(IS_SKIP(mb_type0))
		  {
		    b8cache[13]=b8cache[20]=
			b8cache[22]=b8cache[29]=1;
		  }
		  else
		  {
		    b8cache[13]=b8cache[20]=pDecLocal->b8mode[0]==0&&pDecLocal->b8pdir[0]==2;
			b8cache[22]=pDecLocal->b8mode[1]==0&&pDecLocal->b8pdir[1]==2;
			b8cache[29]=pDecLocal->b8mode[2]==0&&pDecLocal->b8pdir[2]==2;			
		  }
		  if(pDecLocal->mbAvailB)
		  {
		  	b8cache[4]=b8direct_frame_buffer[top_xy*4+2];
			b8cache[6]=b8direct_frame_buffer[top_xy*4+3];
		  }
		  else
		  	b8cache[4]=b8cache[6]=0;
		  if(pDecLocal->mbAvailA)
		  {
		  	b8cache[11]=b8direct_frame_buffer[left_xy[0]*4+1+(left_block[0]&~1)];
			b8cache[27]=b8direct_frame_buffer[left_xy[1]*4+1+(left_block[2]&~1)];
		  }
		  else
		  	b8cache[11]=b8cache[27]=0;
		}
	  }
	  if(currSlice->mb_aff_frame_flag)
	  { 
	    VO_S16 *mv;
		VO_S16 *mvd;
		VO_S32 *mvd_cache =&pDecLocal->mvd_cache[j][0];
	    if(pDecLocal->mb_field)
	    {
	      if(pDecLocal->topleft_mb_xy>=0&&!IS_INTERLACED(mb_type0_frame_buffer[pDecLocal->topleft_mb_xy])&& ref_cache0[3] >= 0)
	      {
	        ref_cache0[3] <<= 1;
			mv = (VO_S16*)&mv_cache0[3];
			mv[1]/=2;
			mvd = (VO_S16*)&mvd_cache[3];
			mvd[1]>>=1;
	      }
		  if(!IS_INTERLACED(pDecLocal->top_type))
	      {
	        for(i=4;i<8;i++)
	        {
 	          if(ref_cache0[i] >= 0)
 	          {
   	            ref_cache0[i] <<= 1;
   			    mv = (VO_S16*)&mv_cache0[i];
   			    mv[1]/=2;
   			    mvd = (VO_S16*)&mvd_cache[i];
   			    mvd[1]>>=1;
 	          }
	        }			
	      }
		  if(pDecLocal->topright_mb_xy>0&&!IS_INTERLACED(mb_type0_frame_buffer[pDecLocal->topright_mb_xy])&& ref_cache0[8] >= 0)
	      {
	        ref_cache0[8] <<= 1;
			mv = (VO_S16*)&mv_cache0[8];
			mv[1]/=2;
			mvd = (VO_S16*)&mvd_cache[8];
			mvd[1]>>=1;
	      }
		  if(!IS_INTERLACED(pDecLocal->left_type[0]))
	      {
 	        if(ref_cache0[11] >= 0)
 	        {
   	          ref_cache0[11] <<= 1;
   			  mv = (VO_S16*)&mv_cache0[11];
   			  mv[1]/=2;
   			  mvd = (VO_S16*)&mvd_cache[11];
   			  mvd[1]>>=1;
 	        }
			if(ref_cache0[19] >= 0)
 	        {
   	          ref_cache0[19] <<= 1;
   			  mv = (VO_S16*)&mv_cache0[19];
   			  mv[1]/=2;
   			  mvd = (VO_S16*)&mvd_cache[19];
   			  mvd[1]>>=1;
 	        }
	      }
		  if(!IS_INTERLACED(pDecLocal->left_type[1]))
	      {
 	        if(ref_cache0[27] >= 0)
 	        {
   	          ref_cache0[27] <<= 1;
   			  mv = (VO_S16*)&mv_cache0[27];
   			  mv[1]/=2;
   			  mvd = (VO_S16*)&mvd_cache[27];
   			  mvd[1]>>=1;
 	        }
			if(ref_cache0[35] >= 0)
 	        {
   	          ref_cache0[35] <<= 1;
   			  mv = (VO_S16*)&mv_cache0[35];
   			  mv[1]/=2;
   			  mvd = (VO_S16*)&mvd_cache[35];
   			  mvd[1]>>=1;
 	        }
	      }
	    }
		else
		{
		  if(pDecLocal->topleft_mb_xy>=0&&IS_INTERLACED(mb_type0_frame_buffer[pDecLocal->topleft_mb_xy])&& ref_cache0[3] >= 0)
	      {
	        ref_cache0[3] >>= 1;
			mv = (VO_S16*)&mv_cache0[3];
			mv[1]<<=1;
			mvd = (VO_S16*)&mvd_cache[3];
			mvd[1]<<=1;
	      }
		  if(IS_INTERLACED(pDecLocal->top_type))
	      {
	        for(i=4;i<8;i++)
	        {
 	          if(ref_cache0[i] >= 0)
 	          {
   	            ref_cache0[i] >>= 1;
   			    mv = (VO_S16*)&mv_cache0[i];
   			    mv[1]<<=1;
   			    mvd = (VO_S16*)&mvd_cache[i];
   			    mvd[1]<<=1;
 	          }
	        }			
	      }
		  if(pDecLocal->topright_mb_xy>0&&IS_INTERLACED(mb_type0_frame_buffer[pDecLocal->topright_mb_xy])&& ref_cache0[8] >= 0)
	      {
	        ref_cache0[8] >>= 1;
			mv = (VO_S16*)&mv_cache0[8];
			mv[1]<<=1;
			mvd = (VO_S16*)&mvd_cache[8];
			mvd[1]<<=1;
	      }
		  if(IS_INTERLACED(pDecLocal->left_type[0]))
	      {
 	        if(ref_cache0[11] >= 0)
 	        {
   	          ref_cache0[11] >>= 1;
   			  mv = (VO_S16*)&mv_cache0[11];
   			  mv[1]<<=1;
   			  mvd = (VO_S16*)&mvd_cache[11];
   			  mvd[1]<<=1;
 	        }
			if(ref_cache0[19] >= 0)
 	        {
   	          ref_cache0[19] >>= 1;
   			  mv = (VO_S16*)&mv_cache0[19];
   			  mv[1]<<=1;
   			  mvd = (VO_S16*)&mvd_cache[19];
   			  mvd[1]<<=1;
 	        }
	      }
		  if(IS_INTERLACED(pDecLocal->left_type[1]))
	      {
 	        if(ref_cache0[27] >= 0)
 	        {
   	          ref_cache0[27] >>= 1;
   			  mv = (VO_S16*)&mv_cache0[27];
   			  mv[1]<<=1;
   			  mvd = (VO_S16*)&mvd_cache[27];
   			  mvd[1]<<=1;
 	        }
			if(ref_cache0[35] >= 0)
 	        {
   	          ref_cache0[35] >>= 1;
   			  mv = (VO_S16*)&mv_cache0[35];
   			  mv[1]<<=1;
   			  mvd = (VO_S16*)&mvd_cache[35];
   			  mvd[1]<<=1;
 	        }
	      }
		}
	  }
	}
  }
  
}

static VO_S32 read_one_macroblock_i_slice(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  Slice *currSlice = pDecLocal->p_Slice;
  //VideoParameters *p_Vid = currMB->p_Vid;
  VO_S32 ret;
  VO_U32 mb_type0;
  DataPartition *dP = &currSlice->partArr[0];
  VO_S32 dct8x8 = currSlice->active_pps->transform_8x8_mode_flag;
  if (currSlice->mb_aff_frame_flag && (pDecLocal->mb.y&0x01)==0)
  {
    pDecLocal->mb_field = readFieldModeInfo_CABAC(pDecGlobal,pDecLocal,&dP->c);
  }
  pDecLocal->mb_field |= currSlice->field_pic_flag;
  update_pos(pDecGlobal,pDecLocal,currSlice);
  //  read MB mode *****************************************************************
  mb_type0 = readMB_typeInfo_CABAC_i_slice(pDecLocal, &dP->c);
        
  ret = pDecGlobal->interpret_mb_mode(pDecGlobal,pDecLocal,mb_type0);
  if(ret)
  	return ret;
  pDecLocal->i16mode= i_mb_mode[mb_type0].mode;
  pDecLocal->cbp = i_mb_mode[mb_type0].cbp;
  mb_type0= i_mb_mode[mb_type0].mb_type;
  if(pDecLocal->mb_field)
  	mb_type0 |= VO_INTERLACED;
  if(IS_PCM(mb_type0))
  {
    read_IPCM_coeffs_from_NAL(pDecGlobal,pDecLocal, dP);
	pDecLocal->cbp = 0x1ef;
	update_qp(pDecGlobal,pDecLocal, 0);
    memset(pDecGlobal->nz_coeff_frame+pDecLocal->mb_xy*24,16,24);
    pDecLocal->skip_flag = 0;
    currSlice->last_dquant = 0;
	pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] = 0;
	currSlice->is_reset_coeff = FALSE;
	pDecLocal->mb_type0 = mb_type0;
	pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = mb_type0;
	return 0;
  }
  
  update_cache(pDecGlobal,pDecLocal,currSlice,mb_type0);

  if (IS_INTRA4x4( mb_type0 ) )
  {
    if(dct8x8&&readMB_transform_size_flag_CABAC(pDecLocal,&dP->c))
    {
      mb_type0 |= VO_8x8DCT;
      M32(pDecLocal->b8mode) = 0x0d0d0d0d;
	  M32(pDecLocal->b8pdir) = 0xffffffff;
      read_ipred_modes_cabac_8x8(pDecGlobal,pDecLocal);
    }
	else
	{
	  read_ipred_modes_cabac(pDecGlobal,pDecLocal);
	}
	writeIntrapredMode(pDecGlobal,pDecLocal);
  }

	  	
  pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] =
  (VO_S8)(pDecLocal->c_ipred_mode = readCIPredMode_CABAC(pDecGlobal,pDecLocal, &dP->c));

  if (pDecLocal->c_ipred_mode < DC_PRED_8 || pDecLocal->c_ipred_mode > DC_128_PRED_8)
  {
    pDecLocal->c_ipred_mode = DC_128_PRED_8;//warning_dtt
  }
  // read CBP and Coeffs  ***************************************************************
  pDecLocal->mb_type0 = mb_type0;
  ret = pDecGlobal->read_CBP_and_coeffs_from_NAL (pDecGlobal,pDecLocal,dct8x8);
  if(ret)
	return ret;

  currSlice->is_reset_coeff = FALSE;
  writeNZCoeff(pDecGlobal,pDecLocal);
  pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = pDecLocal->mb_type0;
  return 0;
}

static VO_S32 read_one_macroblock_p_slice(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  Slice *currSlice = pDecLocal->p_Slice;
  //VideoParameters *p_Vid = currMB->p_Vid;

  int i,ret;
  int mbaff =currSlice->mb_aff_frame_flag; 
  //int  mb_nr = pDecLocal->mbAddrX; 
  VO_S32 mb_type0,intra = 0;
  VO_S32 p_number;
  VO_S32 dct8x8 = currSlice->active_pps->transform_8x8_mode_flag;
  DataPartition *dP= &currSlice->partArr[0];
  
  
  if( mbaff && (pDecLocal->mb.y&0x01)==1 && pDecLocal->pre_skip )
    pDecLocal->skip_flag = pDecLocal->next_skip;
  else
    pDecLocal->skip_flag = read_skip_flag_CABAC(pDecGlobal,pDecLocal, &dP->c,pDecLocal->mb.x,pDecLocal->mb.y);
  
  // read MB type
  if (pDecLocal->skip_flag )	 
  {
    mb_type0 = 0;
    if( mbaff && (pDecLocal->mb.y&1)==0 )
	{
	  pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = (VO_S32)VO_SKIP;
      pDecLocal->next_skip = read_skip_flag_CABAC(pDecGlobal,pDecLocal, &dP->c,pDecLocal->mb.x,pDecLocal->mb.y+1);;
      if(!pDecLocal->next_skip)
        pDecLocal->mb_field = readFieldModeInfo_CABAC(pDecGlobal,pDecLocal,&dP->c);
    }
	mb_type0 |= VO_16x16|VO_P0L0|VO_P1L0|VO_SKIP;
	pDecLocal->mb_field |= currSlice->field_pic_flag;
	if(pDecLocal->mb_field)
	  mb_type0 |= VO_INTERLACED;
	update_pos(pDecGlobal,pDecLocal,currSlice);
    M32(pDecLocal->b8mode) = 0;
	M32(pDecLocal->b8pdir) = 0;
	update_cache(pDecGlobal,pDecLocal,currSlice,mb_type0);
	skip_macroblock(pDecGlobal,pDecLocal);
	pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] = 0;
	currSlice->last_dquant = 0;
	update_qp(pDecGlobal,pDecLocal, currSlice->qp);
	pDecLocal->mb_type0 = mb_type0;
	pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = mb_type0;
	return 0;  
  }
  if (mbaff && (pDecLocal->mb.y&0x01)==0)
  {
    pDecLocal->mb_field = readFieldModeInfo_CABAC(pDecGlobal,pDecLocal,&dP->c);
  }
  pDecLocal->mb_field |= currSlice->field_pic_flag;
  update_pos(pDecGlobal,pDecLocal,currSlice);
  mb_type0 = readMB_typeInfo_CABAC_p_slice(pDecLocal,&dP->c,&intra);
  
  pDecLocal->pre_skip = 0;
  ret = pDecGlobal->interpret_mb_mode(pDecGlobal,pDecLocal,mb_type0);
  if(ret)
  	return ret;
  if(!intra)
  {
    mb_type0--;
    p_number = p_mb_mode_info[mb_type0].partition_number;
    mb_type0 = p_mb_mode_info[mb_type0].mb_type;
  }
  else
  {
    mb_type0-=6;
    p_number = 0;
    pDecLocal->i16mode= i_mb_mode[mb_type0].mode;
    pDecLocal->cbp = i_mb_mode[mb_type0].cbp;
    mb_type0= i_mb_mode[mb_type0].mb_type;
  }
  if(pDecLocal->mb_field)
  	mb_type0 |= VO_INTERLACED;
  
  if(IS_PCM(mb_type0))
  {
    read_IPCM_coeffs_from_NAL(pDecGlobal,pDecLocal, dP);
	pDecLocal->cbp = 0x1ef;
	update_qp(pDecGlobal,pDecLocal, 0);
    memset(pDecGlobal->nz_coeff_frame+pDecLocal->mb_xy*24,16,24);
    pDecLocal->skip_flag = 0;
    currSlice->last_dquant = 0;
	pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] = 0;
	currSlice->is_reset_coeff = FALSE;
	pDecLocal->mb_type0 = mb_type0;
	pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = mb_type0;
	return 0;
  }
  
  update_cache(pDecGlobal,pDecLocal,currSlice,mb_type0);
  if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
  {
    currSlice->num_ref_idx_active[LIST_0] <<= 1;
    currSlice->num_ref_idx_active[LIST_1] <<= 1;
  }
  if (IS_INTRA0( mb_type0 ))
  {
    if (IS_INTRA4x4( mb_type0 ) )
    {
      if(dct8x8&&readMB_transform_size_flag_CABAC(pDecLocal,&dP->c))
      {
        mb_type0 |= VO_8x8DCT;
        M32(pDecLocal->b8mode) = 0x0d0d0d0d;
	    M32(pDecLocal->b8pdir) = 0xffffffff;
		read_ipred_modes_cabac_8x8(pDecGlobal,pDecLocal);
      }
	  else
	  {
	    read_ipred_modes_cabac(pDecGlobal,pDecLocal);
	  }
	  writeIntrapredMode(pDecGlobal,pDecLocal);
    }
	pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] =
    (VO_S8)(pDecLocal->c_ipred_mode = readCIPredMode_CABAC(pDecGlobal,pDecLocal, &dP->c));

    if (pDecLocal->c_ipred_mode < DC_PRED_8 || pDecLocal->c_ipred_mode > DC_128_PRED_8)
    {
      pDecLocal->c_ipred_mode = DC_128_PRED_8;//warning_dtt
    }
	currSlice->is_reset_coeff = FALSE;
  }
  else
  {
    if (p_number == 4)
    {
      for (i = 0; i < 4; ++i)
      {
        ret = SetB8Mode (pDecLocal, readB8_typeInfo_CABAC_p_slice(pDecLocal,&dP->c), i);
	    if(ret)
	  	  return ret;

        dct8x8 &= (pDecLocal->b8mode[i]==0 && currSlice->active_sps->direct_8x8_inference_flag) ||
          (pDecLocal->b8mode[i]==4);
      }
    
    }
    pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] = 0;
	/*if(dct8x8)
	{
      if(currSlice->active_sps->direct_8x8_inference_flag)
        dct8x8 = !(M64(pDecLocal->b8type) & ((VO_16x8|VO_8x16|VO_8x8          )*0x0001000100010001));
      else
        dct8x8 = !(M64(pDecLocal->b8type) & ((VO_16x8|VO_8x16|VO_8x8|VO_DIRECT)*0x0001000100010001));
	}*/
    ret = pDecGlobal->read_motion_info_from_NAL (pDecGlobal,pDecLocal,mb_type0);
	if(ret)
  	  return ret;
	writeMotion(pDecGlobal,pDecLocal);
  }
    // read CBP and Coeffs  ***************************************************************
  pDecLocal->mb_type0 = mb_type0;
  
  ret = pDecGlobal->read_CBP_and_coeffs_from_NAL (pDecGlobal,pDecLocal,dct8x8);
  if(ret)
	return ret;
  
  writeNZCoeff(pDecGlobal,pDecLocal);
  if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
  {
    currSlice->num_ref_idx_active[LIST_0] >>= 1;
	currSlice->num_ref_idx_active[LIST_1] >>= 1;
  }
  pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = pDecLocal->mb_type0;
  return 0;
}

static VO_S32 read_one_macroblock_b_slice(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  Slice *currSlice = pDecLocal->p_Slice;
  //VideoParameters *p_Vid = currMB->p_Vid;
  int i,ret;
  int mbaff =currSlice->mb_aff_frame_flag;
  //int mb_nr = pDecLocal->mbAddrX; 
  VO_S32 mb_type0,intra = 0;
  VO_S32 p_number;
  VO_S32 dct8x8 = currSlice->active_pps->transform_8x8_mode_flag;
  DataPartition *dP = &currSlice->partArr[0];
  update_qp(pDecGlobal,pDecLocal, currSlice->qp);

  //  read MB mode *****************************************************************
  if( mbaff && (pDecLocal->mb.y&0x01)==1 && pDecLocal->pre_skip )
    pDecLocal->skip_flag = pDecLocal->next_skip;
  else
    pDecLocal->skip_flag = read_skip_flag_CABAC(pDecGlobal,pDecLocal, &dP->c,pDecLocal->mb.x,pDecLocal->mb.y);

  pDecLocal->cbp = !pDecLocal->skip_flag;

  if (pDecLocal->skip_flag)
  {	
    VO_U8* nz_coeff = pDecGlobal->nz_coeff_frame+pDecLocal->mb_xy*24; 
	VO_U32 j;
    //currSlice->cod_counter=-1;
	currSlice->last_dquant = 0;
	mb_type0 = 0;
	if( mbaff && (pDecLocal->mb.y&1)==0 )
	{
	  pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = (VO_S32)VO_SKIP;
      pDecLocal->next_skip = read_skip_flag_CABAC(pDecGlobal,pDecLocal, &dP->c,pDecLocal->mb.x,pDecLocal->mb.y+1);;
      if(!pDecLocal->next_skip)
        pDecLocal->mb_field = readFieldModeInfo_CABAC(pDecGlobal,pDecLocal,&dP->c);
    }
	mb_type0|= VO_L0L1|VO_DIRECT|VO_SKIP;
	pDecLocal->mb_field |= currSlice->field_pic_flag;
    //update_pos(pDecGlobal,pDecLocal,currSlice);
    if(pDecLocal->mb_field)
    {
	  if (!currSlice->field_pic_flag)
	  {
		currSlice->num_ref_idx_active[LIST_0] <<= 1;
		currSlice->num_ref_idx_active[LIST_1] <<= 1;
	  }
	  mb_type0 |= VO_INTERLACED;
    }	 
    M32(pDecLocal->b8mode) = 0x00000000;
	M32(pDecLocal->b8pdir) = 0x02020202;
    update_pos(pDecGlobal,pDecLocal,currSlice);
	//update_cache(pDecGlobal,pDecLocal,currSlice,mb_type0);
    //update_cache(pDecGlobal,pDecLocal,currSlice);
	//currSlice->NoMbPartLessThan8x8Flag = (!(pDecGlobal->active_sps->direct_8x8_inference_flag))? FALSE: TRUE;
	
	if (currSlice->direct_spatial_mv_pred_flag == 0)
	{
	  pDecGlobal->mb_pred_b (pDecGlobal,pDecLocal,&mb_type0);
	}
	else
	{
	  update_cache_b_direct(pDecGlobal,pDecLocal,currSlice);
	  pDecGlobal->mb_pred_b (pDecGlobal,pDecLocal,&mb_type0);
	}
	pDecLocal->pre_skip = 1;
	
	//writeMotion(pDecGlobal,pDecLocal);
	pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] = 0;
    for(j = 6;j!= 0;j--)
    {
      M32(nz_coeff) = 0;
      nz_coeff+=4;
    }
	if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
    {
      currSlice->num_ref_idx_active[LIST_0] >>= 1;
      currSlice->num_ref_idx_active[LIST_1] >>= 1;
    }
	mb_type0|=VO_SKIP;
	pDecLocal->mb_type0 = mb_type0;
	pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = mb_type0;
	return 0;
  	
  }
  if (currSlice->mb_aff_frame_flag && (pDecLocal->mb.y&0x01)==0)
  {
    pDecLocal->mb_field = readFieldModeInfo_CABAC(pDecGlobal,pDecLocal,&dP->c);
  }
  pDecLocal->mb_field |= currSlice->field_pic_flag;
  update_pos(pDecGlobal,pDecLocal,currSlice);

  mb_type0 = readMB_typeInfo_CABAC_b_slice(pDecLocal, &dP->c,&intra);
  
  pDecLocal->pre_skip = 0;
  ret = pDecGlobal->interpret_mb_mode(pDecGlobal,pDecLocal,mb_type0);
  if(ret)
  	return ret;
  if(!intra)
  {
    p_number = b_mb_mode_info[mb_type0].partition_number;
    mb_type0 = b_mb_mode_info[mb_type0].mb_type;
  }
  else
  {
    mb_type0-=23;
    p_number = 0;
    pDecLocal->i16mode= i_mb_mode[mb_type0].mode;
    pDecLocal->cbp = i_mb_mode[mb_type0].cbp;
    mb_type0= i_mb_mode[mb_type0].mb_type;
  }

  if(pDecLocal->mb_field)
  	mb_type0 |= VO_INTERLACED;
  
  if(IS_PCM(mb_type0))
  {
    read_IPCM_coeffs_from_NAL(pDecGlobal,pDecLocal, dP);
	pDecLocal->cbp = 0x1ef;
	update_qp(pDecGlobal,pDecLocal, 0);
    memset(pDecGlobal->nz_coeff_frame+pDecLocal->mb_xy*24,16,24);
    pDecLocal->skip_flag = 0;
    currSlice->last_dquant = 0;
	pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] = 0;
	currSlice->is_reset_coeff = FALSE;
	pDecLocal->mb_type0 = mb_type0;
	pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = mb_type0;
	return 0;
  }
  if(!IS_INTRA0(mb_type0))
    init_macroblock_b(pDecGlobal,pDecLocal);
  
  if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
  {
    currSlice->num_ref_idx_active[LIST_0] <<= 1;
    currSlice->num_ref_idx_active[LIST_1] <<= 1;
  }
  update_cache(pDecGlobal,pDecLocal,currSlice,mb_type0);
  
  if (IS_INTRA0( mb_type0 ))
  {
    if (IS_INTRA4x4( mb_type0 ) )
    {
      if(dct8x8&&readMB_transform_size_flag_CABAC(pDecLocal,&dP->c))
      {
        mb_type0 |= VO_8x8DCT;
        M32(pDecLocal->b8mode) = 0x0d0d0d0d;
	    M32(pDecLocal->b8pdir) = 0xffffffff;
		read_ipred_modes_cabac_8x8(pDecGlobal,pDecLocal);
      }
	  else
	  {
	    read_ipred_modes_cabac(pDecGlobal,pDecLocal);
	  }
	  writeIntrapredMode(pDecGlobal,pDecLocal);
    }
	pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] =
    (VO_S8)(pDecLocal->c_ipred_mode = readCIPredMode_CABAC(pDecGlobal,pDecLocal, &dP->c));

    if (pDecLocal->c_ipred_mode < DC_PRED_8 || pDecLocal->c_ipred_mode > DC_128_PRED_8)
    {
      pDecLocal->c_ipred_mode = DC_128_PRED_8;//warning_dtt
    }
	currSlice->is_reset_coeff = FALSE;
  }
  else if (p_number == 4)
  {
    VO_S8* b8cache = pDecLocal->b8cache;
    //currSE.reading = readB8_typeInfo_CABAC_b_slice;

    for (i = 0; i < 4; ++i)
    {
      ret = SetB8Mode (pDecLocal, readB8_typeInfo_CABAC_b_slice(pDecLocal, &dP->c), i);
	  if(ret)
	  	return ret;
      dct8x8 &= (pDecLocal->b8mode[i]==0 && currSlice->active_sps->direct_8x8_inference_flag) ||
        (pDecLocal->b8mode[i]==4);
    }
    b8cache[13]=b8cache[20]=pDecLocal->b8mode[0]==0&&pDecLocal->b8pdir[0]==2;
	b8cache[22]=pDecLocal->b8mode[1]==0&&pDecLocal->b8pdir[1]==2;
	b8cache[29]=pDecLocal->b8mode[2]==0&&pDecLocal->b8pdir[2]==2;
	ret = pDecGlobal->read_motion_info_from_NAL (pDecGlobal,pDecLocal,mb_type0);
	if(ret)
  	  return ret;
	writeMotion(pDecGlobal,pDecLocal);

	pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] = 0;
  }
  else
  {  
    if (IS_DIRECT_VO(mb_type0))
    {
      VO_S32 j;
      pDecGlobal->mb_pred_b (pDecGlobal,pDecLocal,&mb_type0);
	  for(j = 0;j < 2;j++)
      {
        if(currSlice->active_pps->entropy_coding_mode_flag == CABAC)
        {
          int *mvd_out = (int*)&pDecGlobal->mvd_frame_buffer[pDecLocal->mb_xy*16+j*8];
  	      int *mvd_cache = (int*)&pDecLocal->mvd_cache[j][0];
  	  
  	      mvd_out[0]=mvd_cache[36];
	      mvd_out[1]=mvd_cache[37];
	      mvd_out[2]=mvd_cache[38];
	      mvd_out[3]=mvd_cache[39];
	      mvd_out[4]=mvd_cache[31];
	      mvd_out[5]=mvd_cache[23];
	      mvd_out[6]=mvd_cache[15];
        }
      }
	  dct8x8 &= currSlice->active_sps->direct_8x8_inference_flag;
    }
    else 
    {
      ret = pDecGlobal->read_motion_info_from_NAL (pDecGlobal,pDecLocal,mb_type0);
	  if(ret)
  	    return ret;
	  writeMotion(pDecGlobal,pDecLocal);
    }
	pDecGlobal->c_ipred_frame_buffer[pDecLocal->mb_xy] = 0;
  }
  pDecLocal->mb_type0 = mb_type0;
  
  ret = pDecGlobal->read_CBP_and_coeffs_from_NAL (pDecGlobal,pDecLocal,dct8x8);
  if(ret)
	return ret;
  	
  writeNZCoeff(pDecGlobal,pDecLocal);
  if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
  {
    currSlice->num_ref_idx_active[LIST_0] >>= 1;
    currSlice->num_ref_idx_active[LIST_1] >>= 1;
  }
  pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = pDecLocal->mb_type0;
  return 0;
}

static VO_S32 read_one_macroblock_cavlc(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  Slice *currSlice = pDecLocal->p_Slice;
  int i,ret;
  DataPartition *dP;
  VO_S32 mb_type0,mb_type;
  VO_S32 p_number;
  VO_S32 dct8x8 = currSlice->active_pps->transform_8x8_mode_flag;
  update_qp(pDecGlobal,pDecLocal, currSlice->qp);

  //  read MB mode *****************************************************************
  dP = &currSlice->partArr[0];
  //currSE.mapping = linfo_ue;
  //skip
  
  if(currSlice->slice_type!=I_SLICE && currSlice->slice_type!=SI_SLICE)
  {
    if(currSlice->cod_counter == -1)
    {
	  currSlice->cod_counter = ue_v(pDecGlobal);
    }

    if (currSlice->cod_counter--)
    { 
	  mb_type0 = 0;
      //pDecLocal->ei_flag = 0;
      pDecLocal->skip_flag = 1;
	  if(currSlice->mb_aff_frame_flag && (pDecLocal->mb.y&1)==0)
	  {
        if(currSlice->cod_counter==0)
          pDecLocal->mb_field = u_v (1, pDecGlobal);
      }
	  pDecLocal->mb_field |= currSlice->field_pic_flag;
	  if(pDecLocal->mb_field)
      {
	    if (!currSlice->field_pic_flag)
	    {
		  currSlice->num_ref_idx_active[LIST_0] <<= 1;
		  currSlice->num_ref_idx_active[LIST_1] <<= 1;
	    }
	    mb_type0 |= VO_INTERLACED;
      }	 
	  if(currSlice->slice_type == B_SLICE)
	  {
	    VO_U8* nz_coeff = pDecGlobal->nz_coeff_frame+pDecLocal->mb_xy*24; 
		VO_U32 j;
	    pDecLocal->cbp = 0;
	    M32(pDecLocal->b8mode) = 0x00000000;
	    M32(pDecLocal->b8pdir) = 0x02020202;
	    mb_type0|= VO_L0L1|VO_DIRECT|VO_SKIP;
	    update_pos(pDecGlobal,pDecLocal,currSlice);
		if (currSlice->direct_spatial_mv_pred_flag == 0)
	    {
	      /*if(currSlice->mb_aff_frame_flag&&pDecLocal->mb.y&1)
          {
            pDecLocal->pix_y -= 15*pDecLocal->mb_field;
	        pDecLocal->pix_c_y -= 7*pDecLocal->mb_field;
	      }*/
	      pDecGlobal->mb_pred_b (pDecGlobal,pDecLocal,&mb_type0);
	  	}
		else
		{
		  update_cache_b_direct(pDecGlobal,pDecLocal,currSlice);
	      pDecGlobal->mb_pred_b (pDecGlobal,pDecLocal,&mb_type0);
		}
		for(j = 6;j!= 0;j--)
        {
      	  M32(nz_coeff) = 0;
      	  nz_coeff+=4;
        }
		pDecLocal->pre_skip = 1;
		mb_type0|=VO_SKIP;
	  }
	  else
	  {
	    mb_type0 |= VO_16x16|VO_P0L0|VO_P1L0|VO_SKIP;
	    update_pos(pDecGlobal,pDecLocal,currSlice);
	    M32(pDecLocal->b8mode) = 0;
	    M32(pDecLocal->b8pdir) = 0;
	    update_cache(pDecGlobal,pDecLocal,currSlice,mb_type0);
	    //reset_coeffs(pDecGlobal); 
	    skip_macroblock(pDecGlobal,pDecLocal);
		
	  }
	  
	  if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
      {
        currSlice->num_ref_idx_active[LIST_0] >>= 1;
        currSlice->num_ref_idx_active[LIST_1] >>= 1;
      }
	  pDecLocal->mb_type0 = mb_type0;
	  pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = mb_type0;
	  
	  return 0;     
    }
  }
  if (currSlice->mb_aff_frame_flag && (pDecLocal->mb.y&0x01)==0)
  {
    pDecLocal->mb_field = u_v (1, pDecGlobal);
  }
  pDecLocal->mb_field |= currSlice->field_pic_flag;
  
  update_pos(pDecGlobal,pDecLocal,currSlice);
  mb_type0=mb_type = ue_v(pDecGlobal);
  if(currSlice->slice_type == P_SLICE)
    mb_type++;
  ret = pDecGlobal->interpret_mb_mode(pDecGlobal,pDecLocal,mb_type);
  if(ret)
  	return ret;
  if(currSlice->slice_type == B_SLICE)
  {
    if(mb_type0 < 23)
	{
      p_number = b_mb_mode_info[mb_type0].partition_number;
      mb_type0 = b_mb_mode_info[mb_type0].mb_type;
    }
	else
	{
      mb_type0 -= 23;
      goto vo_intra;
    }
  }
  else if(currSlice->slice_type == P_SLICE)
  {
    if(mb_type0 < 5)
	{
      p_number = p_mb_mode_info[mb_type0].partition_number;
      mb_type0 = p_mb_mode_info[mb_type0].mb_type;
    }
	else
	{
      mb_type0 -= 5;
      goto vo_intra;
    }
  }
  else
  {
    assert(currSlice->slice_type == I_SLICE);
vo_intra:
    p_number = 0;
    pDecLocal->i16mode= i_mb_mode[mb_type0].mode;
    pDecLocal->cbp = i_mb_mode[mb_type0].cbp;
    mb_type0= i_mb_mode[mb_type0].mb_type;
  }
  if(pDecLocal->mb_field)
  	mb_type0 |= VO_INTERLACED;
  
  pDecLocal->skip_flag = 0;
  
  
  if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
  {
	  currSlice->num_ref_idx_active[LIST_0] <<= 1;
	  currSlice->num_ref_idx_active[LIST_1] <<= 1;	  
  }
  if(currSlice->slice_type == B_SLICE&&!IS_INTRA0(mb_type0))
  	init_macroblock_b(pDecGlobal,pDecLocal);
  update_cache(pDecGlobal,pDecLocal,currSlice,mb_type0);
  
  if(IS_PCM(mb_type0))
  {
	read_IPCM_coeffs_from_NAL(pDecGlobal,pDecLocal, dP);
	update_qp(pDecGlobal,pDecLocal, 0);

    memset(pDecGlobal->nz_coeff_frame+pDecLocal->mb_xy*24,16,24);
  // for CABAC decoding of MB skip flag
    pDecLocal->skip_flag = 0;
    currSlice->last_dquant = 0;
    currSlice->is_reset_coeff = FALSE;
	pDecLocal->mb_type0 = mb_type0;
	pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = mb_type0;
	return 0;
  }
  pDecLocal->pre_skip = 0;


  if (IS_INTRA0( mb_type0 ))
  {
    if(IS_INTRA4x4( mb_type0 ))
    {
      int b8,i,dec;
      int mostProbableIntraPredMode;
      int upIntraPredMode;
      int leftIntraPredMode;
      VO_S8 *ipredmode = pDecLocal->ipredmode;
      int idx = 1;
      if(dct8x8&&GetBits(pDecGlobal,1))
      {
    	mb_type0 |= VO_8x8DCT;
		M32(pDecLocal->b8mode) = 0x0d0d0d0d;
		M32(pDecLocal->b8pdir) = 0xffffffff;
		idx = 4;
      }

      for(i = 0;i < 16;i+=idx)
      {
        b8 = cache_scan[i];
        dec = readSyntaxElement_Intra4x4PredictionMode(pDecGlobal);
	    upIntraPredMode   = ipredmode[b8-8];
        leftIntraPredMode = ipredmode[b8-1];
	    mostProbableIntraPredMode  = (upIntraPredMode < 0 || leftIntraPredMode < 0) ? DC_PRED : upIntraPredMode < leftIntraPredMode ? upIntraPredMode : leftIntraPredMode;
	    dec = (dec == -1) ? mostProbableIntraPredMode : dec + (dec >= mostProbableIntraPredMode);
        ipredmode[b8] = dec;
	    if(idx == 4)
	    {
	      ipredmode[b8+1]=ipredmode[b8+8]=ipredmode[b8+9]=dec;
	    }
      }
	  writeIntrapredMode(pDecGlobal,pDecLocal);
	  
    }
	
    pDecLocal->c_ipred_mode = ue_v(pDecGlobal);
    if (pDecLocal->c_ipred_mode < DC_PRED_8 || pDecLocal->c_ipred_mode > DC_128_PRED_8)
    {
      pDecLocal->c_ipred_mode = DC_128_PRED_8;//warning_dtt
    }
	currSlice->is_reset_coeff = FALSE;
  }
  else if (p_number == 4)
  {
    for (i = 0; i < 4; ++i)
    {
      int sub_type = ue_v(pDecGlobal);
      ret = SetB8Mode (pDecLocal, sub_type, i);
	  if(ret)
	  	return ret;

      dct8x8 &= (pDecLocal->b8mode[i]==0 && currSlice->active_sps->direct_8x8_inference_flag) ||
        (pDecLocal->b8mode[i]==4);
    }
    ret = pDecGlobal->read_motion_info_from_NAL (pDecGlobal,pDecLocal,mb_type0);
	if(ret)
  	  return ret;
	writeMotion(pDecGlobal,pDecLocal);
  }
  else if(IS_DIRECT_VO(mb_type0))
  {
    pDecGlobal->mb_pred_b (pDecGlobal,pDecLocal,&mb_type0);
	pDecLocal->pre_skip = 1;
	dct8x8 &= currSlice->active_sps->direct_8x8_inference_flag;
  }
  else 
  {
    ret = pDecGlobal->read_motion_info_from_NAL (pDecGlobal,pDecLocal,mb_type0);
	if(ret)
  	  return ret;
    writeMotion(pDecGlobal,pDecLocal);
  }
  pDecLocal->mb_type0 = mb_type0;
  
  // read CBP and Coeffs  ***************************************************************
  ret = pDecGlobal->read_CBP_and_coeffs_from_NAL (pDecGlobal,pDecLocal,dct8x8);
  if(ret)
	return ret;
  writeNZCoeff(pDecGlobal,pDecLocal);
  if(pDecLocal->mb_field&&!currSlice->field_pic_flag)
  {
    currSlice->num_ref_idx_active[LIST_0] >>= 1;
    currSlice->num_ref_idx_active[LIST_1] >>= 1;
  }
  pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy] = pDecLocal->mb_type0;
  return 0;
}

static void read_IPCM_coeffs_from_NAL(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, struct datapartition *dP)
{
  int i;
	VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//	if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//	if (pDecGlobal->nThdNum > 1)
//#endif
//	{
//		cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//	}

  if(pDecGlobal->pCurSlice->active_pps->entropy_coding_mode_flag  == CABAC)
  {
    const VO_U8 *buf = dP->c.stream;
    if(dP->c.low_bits&0x1) buf--;
    if(dP->c.low_bits&0x1FF) buf--;
	for(i=0;i<256+128;++i)
	{
		*cof++ = *buf++;
	}
    vo_start_cabac(&dP->c, buf, dP->c.stream_end - buf);
  }
  else
  {
    const VO_U8* buf;
	int len;
    bytealign(pDecGlobal);
    buf = bytepos(pDecGlobal);
	for(i=0;i<256+128;++i)
	{
		*cof++ = *buf++;
	}
	len = pDecGlobal->pBitEnd - buf - 8;
	InitBitStream(pDecGlobal,buf,len);
  }
}

static void read_ipred_modes_cabac(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  int b8,i,dec;
  DataPartition *dP;
  Slice *currSlice = pDecLocal->p_Slice;
  int mostProbableIntraPredMode;
  int upIntraPredMode;
  int leftIntraPredMode;
  VO_S8 *ipredmode = pDecLocal->ipredmode;

  //currSE.type = SE_INTRAPREDMODE;

  //TRACE_STRING("intra4x4_pred_mode");
  dP = &currSlice->partArr[0];

  for(i = 0;i < 16;i++)
  {
    b8 = cache_scan[i];
      //currSE.context=(b8<<2)+(j<<1) +i;
    dec = readIntraPredMode_CABAC(pDecLocal, &dP->c);
	upIntraPredMode   = ipredmode[b8-8];
    leftIntraPredMode = ipredmode[b8-1];
	mostProbableIntraPredMode  = (upIntraPredMode < 0 || leftIntraPredMode < 0) ? DC_PRED : upIntraPredMode < leftIntraPredMode ? upIntraPredMode : leftIntraPredMode;
	dec = (dec == -1) ? mostProbableIntraPredMode : dec + (dec >= mostProbableIntraPredMode);
    ipredmode[b8] = dec;
  }
}

static void read_ipred_modes_cabac_8x8(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  int b8,i,dec;
  DataPartition *dP;
  Slice *currSlice = pDecLocal->p_Slice;
  int mostProbableIntraPredMode;
  int upIntraPredMode;
  int leftIntraPredMode;
  VO_S8 *ipredmode = pDecLocal->ipredmode;
  dP = &currSlice->partArr[0];

  for(i = 0;i < 16;i+=4)
  {
    b8 = cache_scan[i];
      //currSE.context=(b8<<2)+(j<<1) +i;
    dec = readIntraPredMode_CABAC(pDecLocal, &dP->c);
	upIntraPredMode   = ipredmode[b8-8];
    leftIntraPredMode = ipredmode[b8-1];
	mostProbableIntraPredMode  = (upIntraPredMode < 0 || leftIntraPredMode < 0) ? DC_PRED : upIntraPredMode < leftIntraPredMode ? upIntraPredMode : leftIntraPredMode;
	dec = (dec == -1) ? mostProbableIntraPredMode : dec + (dec >= mostProbableIntraPredMode);
    ipredmode[b8]=ipredmode[b8+1]=ipredmode[b8+8]=ipredmode[b8+9]=dec;
  }
}


static VO_U8 ref_index[4] = {0,2,8,10};

static VO_S32 read_motion_info_from_NAL_p_slice (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
  Slice *currSlice = pDecLocal->p_Slice;
  DataPartition *dP = &currSlice->partArr[0];
  int j4,j,i =0;
  VO_S32 ref_tmp[4];
  //StorablePicture** ref_pic = &pDecLocal->pic_cache[0][0];
  int refidx_present = (!IS_8X8(mb_type0)) || (!currSlice->allrefzero);
  //int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
  StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0];
  VO_S8 *ref_cache = &pDecLocal->ref_cache[LIST_0][12];
  //VO_S32 num_ref = 16;
  //=====  READ REFERENCE PICTURE INDICES =====
  if(IS_8X8(mb_type0))
  {
    VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0]>1?refidx_present?1:0:0;
	VO_S8 *ref_cache = &pDecLocal->ref_cache[LIST_0][0];
    for(j = 0;j < 4;j++)
	{
	  VO_S32 ref_in;
	  if(!ref_active)
      {
        ref_in = 0;				
      }
	  else
      {
		ref_in = readRefFrame_CABAC_p_slice(pDecLocal,&dP->c,cache_scan[4*j],ref_cache);
		if(ref_in == -1)
		  VOH264ERROR(VO_H264_ERR_REF_NULL);
	  }
	  ref_tmp[j] = ref_in;
	  i = ref_index[j];
	  //ref_pic[i] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = list0[ref_in];
	  i = cache_scan[4*j];
	  //M16(ref_cache+i) = M16(ref_cache+i+8) = ref_tmp[j]*0x0101;
	  ref_cache[i+1]=ref_cache[i+8]=ref_cache[i+9]=(VO_S8)ref_tmp[j];
	}	
    //currSE.reading = read_MVD_CABAC;
    //readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0); 
    for(j = 0;j < 4;j++)
	{
	  VO_S32 sub_type = pDecLocal->b8type[j];
	  VO_S32 block_width = (sub_type & (VO_16x16|VO_16x8))?2:1;  
	  ref_cache[cache_scan[4*j]]=ref_cache[cache_scan[4*j]+1];
	  for(i = 0;i < pDecLocal->b8sub[j];i++)
	  {
	    short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
		VO_S32 index= 4*j + block_width*i;
		VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][cache_scan[index]];
		VO_S32 *mvd_cache = &pDecLocal->mvd_cache[LIST_0][cache_scan[index]];
		GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,cache_scan[index], block_width, pred_mv,(short)ref_tmp[j],LIST_0);
		//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,cache_scan[index], &pred_mv, ref_tmp[j], LIST_0, block_i[index], block_j[index], step_h4, step_v4);
		curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,cache_scan[index],(short*)(mvd_cache-cache_scan[index]), 0,&st_mvd[0]);
		curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,cache_scan[index],(short*)(mvd_cache-cache_scan[index]), 1,&st_mvd[1]);
	    curr_mv[0] = pred_mv[0]+curr_mvd[0];
	    curr_mv[1] = pred_mv[1]+curr_mvd[1];
		if(IS_SUB_8X8(sub_type))
		{
          mv_cache[1] = mv_cache[8] = mv_cache[9] = M32(&curr_mv);
		  mvd_cache[1] = mvd_cache[8] = mvd_cache[9] = M32(&st_mvd);
		}
		else if(IS_SUB_8X4(sub_type))
		{
          mv_cache[1] = M32(&curr_mv);
		  mvd_cache[1] = M32(&st_mvd);
		}
        else if(IS_SUB_4X8(sub_type))
        {
          mv_cache[8] = M32(&curr_mv);
		  mvd_cache[8] = M32(&st_mvd);
        }
        mv_cache[0]= M32(&curr_mv);
		mvd_cache[0]= M32(&st_mvd);
	  }
	}

  }
  else if(IS_16X16(mb_type0))
  {
    VO_S32 ref_in;
	VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
	short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
	VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
	VO_S32 *mvd_cache = &pDecLocal->mvd_cache[LIST_0][12];
	if(ref_active <= 1)
    {
      ref_in = 0;		
    }
	else
    {
      ref_in = readRefFrame_CABAC_p_slice(pDecLocal,&dP->c,12,ref_cache-12);
	  if(ref_in == -1)
		  VOH264ERROR(VO_H264_ERR_REF_NULL);
	}
	/*for(j4 = 4; j4 != 0;j4--)
    {
	  *ref_pic++ = list0[ref_in];
	  *ref_pic++ = list0[ref_in];
	  *ref_pic++ = list0[ref_in];
	  *ref_pic++ = list0[ref_in];
    }*/
	M32(ref_cache) = M32(ref_cache+8) = M32(ref_cache+16) = M32(ref_cache+24) = ref_in*0x01010101;

	//GetMotionVectorPredictor(pDecGlobal,pDecLocal, &pred_mv, ref_in, LIST_0);
	GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, pred_mv,(short)ref_in,LIST_0);
	curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,0,(short*)mvd_cache, 0,&st_mvd[0]);
	curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,0,(short*)mvd_cache, 1,&st_mvd[1]);
	curr_mv[0] = pred_mv[0]+curr_mvd[0];
	curr_mv[1] = pred_mv[1]+curr_mvd[1];
	mv_cache[0] = mv_cache[1] = mv_cache[2] = mv_cache[3] = M32(&curr_mv);
	mv_cache[8] = mv_cache[9] = mv_cache[10] = mv_cache[11] = M32(&curr_mv);
	mv_cache[16] = mv_cache[17] = mv_cache[18] = mv_cache[19] = M32(&curr_mv);
	mv_cache[24] = mv_cache[25] = mv_cache[26] = mv_cache[27] = M32(&curr_mv);
	mvd_cache[0] = mvd_cache[1] = mvd_cache[2] = mvd_cache[3] = M32(&st_mvd);
	mvd_cache[8] = mvd_cache[9] = mvd_cache[10] = mvd_cache[11] = M32(&st_mvd);
	mvd_cache[16] = mvd_cache[17] = mvd_cache[18] = mvd_cache[19] = M32(&st_mvd);
	mvd_cache[24] = mvd_cache[25] = mvd_cache[26] = mvd_cache[27] = M32(&st_mvd);
  }
  else if(IS_16X8(mb_type0))
  {
    VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
	ref_cache = &pDecLocal->ref_cache[0][12];
	for(j = 0;j < 2;j++)
	{
	  VO_S32 ref_in;
	  i = j*16;
	  if(ref_active <= 1)
	  {
        ref_in = 0;
      }
	  else
      {
        ref_in = readRefFrame_CABAC_p_slice(pDecLocal,&dP->c,i,ref_cache);
		if(ref_in == -1)
		  VOH264ERROR(VO_H264_ERR_REF_NULL);
	  }
	  ref_tmp[j] = ref_in;
	  i = j*8;
	  //ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+2] = ref_pic[i+3] = 
	  //ref_pic[i+4] = ref_pic[i+5] = ref_pic[i+6] = ref_pic[i+7] = list0[ref_in];
	  i = j*16;
	  ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+2] = ref_cache[i+3] = 
	  ref_cache[i+8] = ref_cache[i+9] = ref_cache[i+10] = ref_cache[i+11] = (VO_S8)ref_tmp[j];
	}
	//currSE.reading = read_MVD_CABAC;
    //readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0);
	for(j = 0;j < 2;j++)
	{
	  short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
	  VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
	  VO_S32 *mvd_cache = &pDecLocal->mvd_cache[LIST_0][12];
	  i = j*16;
	  //pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_tmp[j], LIST_0, 0, i>>1, 16, 8);
      GetMotionVectorPredictor16x8(pDecGlobal,pDecLocal,12+i,LIST_0,(short)ref_tmp[j],pred_mv);
	  curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 0,&st_mvd[0]);
	  curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 1,&st_mvd[1]);
	  curr_mv[0] = pred_mv[0]+curr_mvd[0];
	  curr_mv[1] = pred_mv[1]+curr_mvd[1];
	  mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+2] = mv_cache[i+3] = 
	  mv_cache[i+8] = mv_cache[i+9] = mv_cache[i+10] = mv_cache[i+11] = M32(&curr_mv);
	  mvd_cache[i+0] = mvd_cache[i+1] = mvd_cache[i+2] = mvd_cache[i+3] = 
	  mvd_cache[i+8] = mvd_cache[i+9] = mvd_cache[i+10] = mvd_cache[i+11] = M32(&st_mvd);
	}
	//currSE.mapping = linfo_se;
    //readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0);
  }
  else //8x16
  {
    VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
	ref_cache = &pDecLocal->ref_cache[0][12];
	for(j = 0;j < 2;j++)
	{
	  VO_S32 ref_in;
	  i = j*2;
	  if(ref_active <= 1)
	  {
        ref_in = 0;
      }
	  else
      {
        ref_in = readRefFrame_CABAC_p_slice(pDecLocal,&dP->c,i,ref_cache);
		if(ref_in == -1)
		  VOH264ERROR(VO_H264_ERR_REF_NULL);
	  }
	  ref_tmp[j] = ref_in;
	  i = j*2;
	  //ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = 
	  //ref_pic[i+8] = ref_pic[i+9] = ref_pic[i+12] = ref_pic[i+13] = list0[ref_in];
	  ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+8] = ref_cache[i+9] = 
	  ref_cache[i+16] = ref_cache[i+17] = ref_cache[i+24] = ref_cache[i+25] = (VO_S8)ref_tmp[j];
	}
	//currSE.reading = read_MVD_CABAC;
    //readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0);
	for(j = 0;j < 2;j++)
	{
	  short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
	  VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
	  VO_S32 *mvd_cache = &pDecLocal->mvd_cache[LIST_0][12];
	  i = j*2;
	  //pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_tmp[j], LIST_0, i*4,0 , 8, 16);
      GetMotionVectorPredictor8x16(pDecGlobal,pDecLocal,12+i,LIST_0,(short)ref_tmp[j],pred_mv);
	  curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 0,&st_mvd[0]);
	  curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 1,&st_mvd[1]);
	  curr_mv[0] = pred_mv[0]+curr_mvd[0];
	  curr_mv[1] = pred_mv[1]+curr_mvd[1];
	  mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+8] = mv_cache[i+9] = 
	  mv_cache[i+16] = mv_cache[i+17] = mv_cache[i+24] = mv_cache[i+25] = M32(&curr_mv);
	  mvd_cache[i+0] = mvd_cache[i+1] = mvd_cache[i+8] = mvd_cache[i+9] = 
	  mvd_cache[i+16] = mvd_cache[i+17] = mvd_cache[i+24] = mvd_cache[i+25] = M32(&st_mvd);
	}
  }
  return 0;
}


static VO_S32 read_motion_info_from_NAL_p_slice_interlace (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
	Slice *currSlice = pDecLocal->p_Slice;
	DataPartition *dP = &currSlice->partArr[0];
	int j4,j,i =0;
	VO_S32 ref_tmp[4];
	StorablePicture** ref_pic = &pDecLocal->pic_cache[0][0];
	int refidx_present = (!IS_8X8(mb_type0)) || (!currSlice->allrefzero);
	int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
	StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0+list_offset];
	VO_S8 *ref_cache = &pDecLocal->ref_cache[LIST_0][12];
	//VO_S32 num_ref = 16;
	//=====  READ REFERENCE PICTURE INDICES =====
	if(IS_8X8(mb_type0))
	{
		VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0]>1?refidx_present?1:0:0;
		VO_S8 *ref_cache = &pDecLocal->ref_cache[LIST_0][0];
		for(j = 0;j < 4;j++)
		{
			VO_S32 ref_in;
			if(!ref_active)
			{
				ref_in = 0;				
			}
			else
			{
				ref_in = readRefFrame_CABAC_p_slice(pDecLocal,&dP->c,cache_scan[4*j],ref_cache);
				if(ref_in == -1)
					VOH264ERROR(VO_H264_ERR_REF_NULL);
			}
			ref_tmp[j] = ref_in;
			i = ref_index[j];
			ref_pic[i] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = list0[ref_in];
			i = cache_scan[4*j];
			//M16(ref_cache+i) = M16(ref_cache+i+8) = ref_tmp[j]*0x0101;
			ref_cache[i+1]=ref_cache[i+8]=ref_cache[i+9]=(VO_S8)ref_tmp[j];
		}	
		//currSE.reading = read_MVD_CABAC;
		//readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0); 
		for(j = 0;j < 4;j++)
		{
			VO_S32 sub_type = pDecLocal->b8type[j];
			VO_S32 block_width = (sub_type & (VO_16x16|VO_16x8))?2:1;  
			ref_cache[cache_scan[4*j]]=ref_cache[cache_scan[4*j]+1];
			for(i = 0;i < pDecLocal->b8sub[j];i++)
			{
				short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
				VO_S32 index= 4*j + block_width*i;
				VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][cache_scan[index]];
				VO_S32 *mvd_cache = &pDecLocal->mvd_cache[LIST_0][cache_scan[index]];
				GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,cache_scan[index], block_width, pred_mv,(short)ref_tmp[j],LIST_0);
				//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,cache_scan[index], &pred_mv, ref_tmp[j], LIST_0, block_i[index], block_j[index], step_h4, step_v4);
				curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,cache_scan[index],(short*)(mvd_cache-cache_scan[index]), 0,&st_mvd[0]);
				curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,cache_scan[index],(short*)(mvd_cache-cache_scan[index]), 1,&st_mvd[1]);
				curr_mv[0] = pred_mv[0]+curr_mvd[0];
				curr_mv[1] = pred_mv[1]+curr_mvd[1];
				if(IS_SUB_8X8(sub_type))
				{
					mv_cache[1] = mv_cache[8] = mv_cache[9] = M32(&curr_mv);
					mvd_cache[1] = mvd_cache[8] = mvd_cache[9] = M32(&st_mvd);
				}
				else if(IS_SUB_8X4(sub_type))
				{
					mv_cache[1] = M32(&curr_mv);
					mvd_cache[1] = M32(&st_mvd);
				}
				else if(IS_SUB_4X8(sub_type))
				{
					mv_cache[8] = M32(&curr_mv);
					mvd_cache[8] = M32(&st_mvd);
				}
				mv_cache[0]= M32(&curr_mv);
				mvd_cache[0]= M32(&st_mvd);
			}
		}

	}
	else if(IS_16X16(mb_type0))
	{
		VO_S32 ref_in;
		VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
		short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
		VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
		VO_S32 *mvd_cache = &pDecLocal->mvd_cache[LIST_0][12];
		if(ref_active <= 1)
		{
			ref_in = 0;		
		}
		else
		{
			ref_in = readRefFrame_CABAC_p_slice(pDecLocal,&dP->c,12,ref_cache-12);
			if(ref_in == -1)
				VOH264ERROR(VO_H264_ERR_REF_NULL);
		}
		for(j4 = 4; j4 != 0;j4--)
		{
			*ref_pic++ = list0[ref_in];
			*ref_pic++ = list0[ref_in];
			*ref_pic++ = list0[ref_in];
			*ref_pic++ = list0[ref_in];
		}
		M32(ref_cache) = M32(ref_cache+8) = M32(ref_cache+16) = M32(ref_cache+24) = ref_in*0x01010101;

		//GetMotionVectorPredictor(pDecGlobal,pDecLocal, &pred_mv, ref_in, LIST_0);
		GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, pred_mv,(short)ref_in,LIST_0);
		curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,0,(short*)mvd_cache, 0,&st_mvd[0]);
		curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,0,(short*)mvd_cache, 1,&st_mvd[1]);
		curr_mv[0] = pred_mv[0]+curr_mvd[0];
		curr_mv[1] = pred_mv[1]+curr_mvd[1];
		mv_cache[0] = mv_cache[1] = mv_cache[2] = mv_cache[3] = M32(&curr_mv);
		mv_cache[8] = mv_cache[9] = mv_cache[10] = mv_cache[11] = M32(&curr_mv);
		mv_cache[16] = mv_cache[17] = mv_cache[18] = mv_cache[19] = M32(&curr_mv);
		mv_cache[24] = mv_cache[25] = mv_cache[26] = mv_cache[27] = M32(&curr_mv);
		mvd_cache[0] = mvd_cache[1] = mvd_cache[2] = mvd_cache[3] = M32(&st_mvd);
		mvd_cache[8] = mvd_cache[9] = mvd_cache[10] = mvd_cache[11] = M32(&st_mvd);
		mvd_cache[16] = mvd_cache[17] = mvd_cache[18] = mvd_cache[19] = M32(&st_mvd);
		mvd_cache[24] = mvd_cache[25] = mvd_cache[26] = mvd_cache[27] = M32(&st_mvd);
	}
	else if(IS_16X8(mb_type0))
	{
		VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
		ref_cache = &pDecLocal->ref_cache[0][12];
		for(j = 0;j < 2;j++)
		{
			VO_S32 ref_in;
			i = j*16;
			if(ref_active <= 1)
			{
				ref_in = 0;
			}
			else
			{
				ref_in = readRefFrame_CABAC_p_slice(pDecLocal,&dP->c,i,ref_cache);
				if(ref_in == -1)
					VOH264ERROR(VO_H264_ERR_REF_NULL);
			}
			ref_tmp[j] = ref_in;
			i = j*8;
			ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+2] = ref_pic[i+3] = 
				ref_pic[i+4] = ref_pic[i+5] = ref_pic[i+6] = ref_pic[i+7] = list0[ref_in];
			i = j*16;
			ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+2] = ref_cache[i+3] = 
				ref_cache[i+8] = ref_cache[i+9] = ref_cache[i+10] = ref_cache[i+11] = (VO_S8)ref_tmp[j];
		}
		//currSE.reading = read_MVD_CABAC;
		//readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0);
		for(j = 0;j < 2;j++)
		{
			short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
			VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
			VO_S32 *mvd_cache = &pDecLocal->mvd_cache[LIST_0][12];
			i = j*16;
			//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_tmp[j], LIST_0, 0, i>>1, 16, 8);
			GetMotionVectorPredictor16x8(pDecGlobal,pDecLocal,12+i,LIST_0,(short)ref_tmp[j],pred_mv);
			curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 0,&st_mvd[0]);
			curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 1,&st_mvd[1]);
			curr_mv[0] = pred_mv[0]+curr_mvd[0];
			curr_mv[1] = pred_mv[1]+curr_mvd[1];
			mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+2] = mv_cache[i+3] = 
				mv_cache[i+8] = mv_cache[i+9] = mv_cache[i+10] = mv_cache[i+11] = M32(&curr_mv);
			mvd_cache[i+0] = mvd_cache[i+1] = mvd_cache[i+2] = mvd_cache[i+3] = 
				mvd_cache[i+8] = mvd_cache[i+9] = mvd_cache[i+10] = mvd_cache[i+11] = M32(&st_mvd);
		}
		//currSE.mapping = linfo_se;
		//readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0);
	}
	else //8x16
	{
		VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
		ref_cache = &pDecLocal->ref_cache[0][12];
		for(j = 0;j < 2;j++)
		{
			VO_S32 ref_in;
			i = j*2;
			if(ref_active <= 1)
			{
				ref_in = 0;
			}
			else
			{
				ref_in = readRefFrame_CABAC_p_slice(pDecLocal,&dP->c,i,ref_cache);
				if(ref_in == -1)
					VOH264ERROR(VO_H264_ERR_REF_NULL);
			}
			ref_tmp[j] = ref_in;
			i = j*2;
			ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = 
				ref_pic[i+8] = ref_pic[i+9] = ref_pic[i+12] = ref_pic[i+13] = list0[ref_in];
			ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+8] = ref_cache[i+9] = 
				ref_cache[i+16] = ref_cache[i+17] = ref_cache[i+24] = ref_cache[i+25] = (VO_S8)ref_tmp[j];
		}
		//currSE.reading = read_MVD_CABAC;
		//readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0);
		for(j = 0;j < 2;j++)
		{
			short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
			VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
			VO_S32 *mvd_cache = &pDecLocal->mvd_cache[LIST_0][12];
			i = j*2;
			//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_tmp[j], LIST_0, i*4,0 , 8, 16);
			GetMotionVectorPredictor8x16(pDecGlobal,pDecLocal,12+i,LIST_0,(short)ref_tmp[j],pred_mv);
			curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 0,&st_mvd[0]);
			curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 1,&st_mvd[1]);
			curr_mv[0] = pred_mv[0]+curr_mvd[0];
			curr_mv[1] = pred_mv[1]+curr_mvd[1];
			mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+8] = mv_cache[i+9] = 
				mv_cache[i+16] = mv_cache[i+17] = mv_cache[i+24] = mv_cache[i+25] = M32(&curr_mv);
			mvd_cache[i+0] = mvd_cache[i+1] = mvd_cache[i+8] = mvd_cache[i+9] = 
				mvd_cache[i+16] = mvd_cache[i+17] = mvd_cache[i+24] = mvd_cache[i+25] = M32(&st_mvd);
		}
	}
	return 0;
}


static VO_S32 read_motion_info_from_NAL_p_slice_cavlc (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
  Slice *currSlice = pDecLocal->p_Slice;
  int i,k,j4,j =0;
  //StorablePicture** ref_pic = &pDecLocal->pic_cache[0][0];
  VO_S32 ref_tmp[4];
  //int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
  StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0];
  VO_S8 *ref_cache = &pDecLocal->ref_cache[0][12];  
  //VO_S32 num_ref = currSlice->listXsize[0];
  BEGIN_BITS_POS(pDecGlobal);
  if(IS_8X8(mb_type0))
  {
    //ref 
    VO_S32 ref_active = currSlice->allrefzero?1:currSlice->num_ref_idx_active[LIST_0];
	ref_cache = &pDecLocal->ref_cache[0][0];
	for(j = 0;j < 4;j++)
	{
	  VO_S32 ref_in;
	  if(ref_active == 1)
	  {
        ref_in = 0;
      }
	  else if(ref_active == 2)
      {
        ref_in = 1-GET_BITS_1_POS;		
		UPDATE_CACHE_POS(pDecGlobal);		
      }
	  else
      {
        VO_U32 len = 0,inf;
		inf = SHOW_BITS_POS(32);
        if(inf >=(1<<27))
        {
          inf >>= 32-9;
      	  FLUSH_BITS_POS(VO_VLC_LEN[inf]);
      	  UPDATE_CACHE_POS(pDecGlobal);
      	  ref_in = VO_UE_VAL[inf];
        }
        else
        {
          len = 2*vo_big_len(inf)-31;
      	  inf >>= len;
      	  inf--;
      	  FLUSH_BITS_POS(32-len);
      	  UPDATE_CACHE_POS(pDecGlobal);
      	  ref_in = inf;
		  if (ref_in<0)
		  {
			  return VO_H264_ERR_NUMREFFRAME;
		  }
        }
	  }
	  ref_tmp[j] = ref_in;
	  i = ref_index[j];
	  //ref_pic[i] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = list0[ref_in];
	}	
	for(j = 0;j < 4;j++)
	{
	  VO_S32 sub_type = pDecLocal->b8type[j];
	  VO_S32 block_width = (sub_type & (VO_16x16|VO_16x8))?2:1;
	  i = cache_scan[4*j];
	  M16(ref_cache+i) = M16(ref_cache+i+8) = (VO_U16)ref_tmp[j]*0x0101;	  
	  for(i = 0;i < pDecLocal->b8sub[j];i++)
	  {
	    short pred_mv[2], curr_mv[2];
		VO_S32 index= 4*j + block_width*i;
		VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][cache_scan[index]];
		//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,cache_scan[index], &pred_mv, ref_tmp[j], LIST_0, block_i[index], block_j[index], step_h4, step_v4);
        GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,cache_scan[index], block_width, pred_mv,(short)ref_tmp[j],LIST_0);
		for (k=0; k < 2; ++k)
        {
          unsigned int value,len=0;
		  value = SHOW_BITS_POS(32);
          if(value >=(1<<27))
          {
            value >>= 32-9;
        	FLUSH_BITS_POS(VO_VLC_LEN[value]);
        	UPDATE_CACHE_POS(pDecGlobal);
        	value = VO_SE_VAL[value];
          }
          else
          {
            len = 2*vo_big_len(value)-31;
        	value >>= len;
        	
        	FLUSH_BITS_POS(32-len);
        	UPDATE_CACHE_POS(pDecGlobal);
        	if(value&1)
        	  value = 0-(value>>1);
        	else
        	  value = (value>>1);
          }
          curr_mv[k] = pred_mv[k]+ value;              
        }
		if(IS_SUB_8X8(sub_type))
          mv_cache[1] = mv_cache[8] = mv_cache[9] = M32(&curr_mv);
		else if(IS_SUB_8X4(sub_type))
          mv_cache[1] = M32(&curr_mv);
        else if(IS_SUB_4X8(sub_type))
          mv_cache[8] = M32(&curr_mv);
        mv_cache[0]= M32(&curr_mv);
	  }
	}
  }
  else if(IS_16X16(mb_type0))
  {
    VO_S32 ref_in;
	VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
	short pred_mv[2], curr_mv[2];
	VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
	if(ref_active == 1)
	{
      ref_in = 0;
    }
	else if(ref_active == 2)
    {
      ref_in = 1-GET_BITS_1_POS;		
	  UPDATE_CACHE_POS(pDecGlobal);		
    }
	else
    {
      VO_U32 len = 0,inf;
      inf = SHOW_BITS_POS(32);
      if(inf >=(1<<27))
      {
        inf >>= 32-9;
      	FLUSH_BITS_POS(VO_VLC_LEN[inf]);
      	UPDATE_CACHE_POS(pDecGlobal);
      	ref_in = VO_UE_VAL[inf];
      }
      else
      {
        len = 2*vo_big_len(inf)-31;
      	inf >>= len;
      	inf--;
      	FLUSH_BITS_POS(32-len);
      	UPDATE_CACHE_POS(pDecGlobal);
      	ref_in = inf;
		if (ref_in<0)
		{
			return VO_H264_ERR_NUMREFFRAME;
		}
      }
	}
	/*for(j4 = 4; j4 != 0;j4--)
    {
	  *ref_pic++ = list0[ref_in];
	  *ref_pic++ = list0[ref_in];
	  *ref_pic++ = list0[ref_in];
	  *ref_pic++ = list0[ref_in];
    }*/
	M32(ref_cache) = M32(ref_cache+8) = M32(ref_cache+16) = M32(ref_cache+24) = ref_in*0x01010101;
	//currSE.mapping = linfo_se;
    //readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0);
	//GetMotionVectorPredictor(pDecGlobal,pDecLocal, &pred_mv, ref_in, LIST_0);
	GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, pred_mv,(short)ref_in,LIST_0);
	for (k=0; k < 2; ++k)
    {
      unsigned int value,len=0;
	  value = SHOW_BITS_POS(32);
      if(value >=(1<<27))
      {
        value >>= 32-9;
        FLUSH_BITS_POS(VO_VLC_LEN[value]);
        UPDATE_CACHE_POS(pDecGlobal);
        value = VO_SE_VAL[value];
      }
      else
      {
        len = 2*vo_big_len(value)-31;
        value >>= len;
        	
        FLUSH_BITS_POS(32-len);
        UPDATE_CACHE_POS(pDecGlobal);
        if(value&1)
          value = 0-(value>>1);
        else
          value = (value>>1);
      }
      curr_mv[k] = pred_mv[k]+ value;               
    }
	mv_cache[0] = mv_cache[1] = mv_cache[2] = mv_cache[3] = M32(&curr_mv);
	mv_cache[8] = mv_cache[9] = mv_cache[10] = mv_cache[11] = M32(&curr_mv);
	mv_cache[16] = mv_cache[17] = mv_cache[18] = mv_cache[19] = M32(&curr_mv);
	mv_cache[24] = mv_cache[25] = mv_cache[26] = mv_cache[27] = M32(&curr_mv);
  }
  else if(IS_16X8(mb_type0))
  {
    VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
	ref_cache = &pDecLocal->ref_cache[0][12];
	for(j = 0;j < 2;j++)
	{
	  VO_S32 ref_in;
	  if(ref_active == 1)
	  {
        ref_in = 0;
      }
	  else if(ref_active == 2)
      {
        ref_in = 1-GET_BITS_1_POS;		
		UPDATE_CACHE_POS(pDecGlobal);		
      }
	  else
      {
        VO_U32 len = 0,inf;
        inf = SHOW_BITS_POS(32);
        if(inf >=(1<<27))
        {
          inf >>= 32-9;
        	FLUSH_BITS_POS(VO_VLC_LEN[inf]);
        	UPDATE_CACHE_POS(pDecGlobal);
        	ref_in = VO_UE_VAL[inf];
        }
        else
        {
          len = 2*vo_big_len(inf)-31;
        	inf >>= len;
        	inf--;
        	FLUSH_BITS_POS(32-len);
        	UPDATE_CACHE_POS(pDecGlobal);
        	ref_in = inf;
			if (ref_in<0)
			{
				return VO_H264_ERR_NUMREFFRAME;
			}
        }
	  }
	  ref_tmp[j] = ref_in;
	  i = j*8;
	  //ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+2] = ref_pic[i+3] = 
	  //ref_pic[i+4] = ref_pic[i+5] = ref_pic[i+6] = ref_pic[i+7] = list0[ref_in];
	}
	for(j = 0;j < 2;j++)
	{
	  short pred_mv[2], curr_mv[2];
	  VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
	  i = j*16;
	  ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+2] = ref_cache[i+3] = 
	  ref_cache[i+8] = ref_cache[i+9] = ref_cache[i+10] = ref_cache[i+11] = (VO_S8)ref_tmp[j];
	  //pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_tmp[j], LIST_0, 0, i>>1, 16, 8);
      GetMotionVectorPredictor16x8(pDecGlobal,pDecLocal,12+i,LIST_0,ref_tmp[j],pred_mv);
	  for (k=0; k < 2; ++k)
      {
        unsigned int value,len=0;
  	    value = SHOW_BITS_POS(32);
        if(value >=(1<<27))
        {
          value >>= 32-9;
          FLUSH_BITS_POS(VO_VLC_LEN[value]);
          UPDATE_CACHE_POS(pDecGlobal);
          value = VO_SE_VAL[value];
        }
        else
        {
          len = 2*vo_big_len(value)-31;
          value >>= len;
          	
          FLUSH_BITS_POS(32-len);
          UPDATE_CACHE_POS(pDecGlobal);
          if(value&1)
            value = 0-(value>>1);
          else
            value = (value>>1);
        }
        curr_mv[k] = pred_mv[k]+ value;            
      }
	  mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+2] = mv_cache[i+3] = 
	  mv_cache[i+8] = mv_cache[i+9] = mv_cache[i+10] = mv_cache[i+11] = M32(&curr_mv);
	}
	//currSE.mapping = linfo_se;
    //readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0);
  }
  else//8x16
  {
    VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
	ref_cache = &pDecLocal->ref_cache[0][12];
	for(j = 0;j < 2;j++)
	{
	  VO_S32 ref_in;
	  if(ref_active == 1)
	  {
        ref_in = 0;
      }
	  else if(ref_active == 2)
      {
        ref_in = 1-GET_BITS_1_POS;		
		UPDATE_CACHE_POS(pDecGlobal);		
      }
	  else
      {
        VO_U32 len = 0,inf;
        inf = SHOW_BITS_POS(32);
        if(inf >=(1<<27))
        {
          inf >>= 32-9;
        	FLUSH_BITS_POS(VO_VLC_LEN[inf]);
        	UPDATE_CACHE_POS(pDecGlobal);
        	ref_in = VO_UE_VAL[inf];
        }
        else
        {
          len = 2*vo_big_len(inf)-31;
        	inf >>= len;
        	inf--;
        	FLUSH_BITS_POS(32-len);
        	UPDATE_CACHE_POS(pDecGlobal);
        	ref_in = inf;
			if (ref_in<0)
			{
				return VO_H264_ERR_NUMREFFRAME;
			}
        }
	  }
	  ref_tmp[j] = ref_in;
	  i = j*2;
	  //ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = 
	  //ref_pic[i+8] = ref_pic[i+9] = ref_pic[i+12] = ref_pic[i+13] = list0[ref_in];
	}
	for(j = 0;j < 2;j++)
	{
	  short pred_mv[2], curr_mv[2];
	  VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
	  i = j*2;
	  ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+8] = ref_cache[i+9] = 
	  ref_cache[i+16] = ref_cache[i+17] = ref_cache[i+24] = ref_cache[i+25] = (VO_S8)ref_tmp[j];
	  //pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_tmp[j], LIST_0, i*4, 0, 8, 16);
	  GetMotionVectorPredictor8x16(pDecGlobal,pDecLocal,12+i,LIST_0,ref_tmp[j],pred_mv);
	  for (k=0; k < 2; ++k)
      {
        unsigned int value,len=0;
  	    value = SHOW_BITS_POS(32);
        if(value >=(1<<27))
        {
          value >>= 32-9;
          FLUSH_BITS_POS(VO_VLC_LEN[value]);
          UPDATE_CACHE_POS(pDecGlobal);
          value = VO_SE_VAL[value];
        }
        else
        {
          len = 2*vo_big_len(value)-31;
          value >>= len;
          	
          FLUSH_BITS_POS(32-len);
          UPDATE_CACHE_POS(pDecGlobal);
          if(value&1)
            value = 0-(value>>1);
          else
            value = (value>>1);
        }
        curr_mv[k] = pred_mv[k]+ value;                  
      }
	  mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+8] = mv_cache[i+9] = 
	  mv_cache[i+16] = mv_cache[i+17] = mv_cache[i+24] = mv_cache[i+25] = M32(&curr_mv);
	}
  }
  END_BITS_POS(pDecGlobal);
  return 0;
}


static VO_S32 read_motion_info_from_NAL_p_slice_cavlc_interlace (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
	Slice *currSlice = pDecLocal->p_Slice;
	int i,k,j4,j =0;
	StorablePicture** ref_pic = &pDecLocal->pic_cache[0][0];
	VO_S32 ref_tmp[4];
	int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
	StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0+list_offset];
	VO_S8 *ref_cache = &pDecLocal->ref_cache[0][12];  
	//VO_S32 num_ref = currSlice->listXsize[0];
	BEGIN_BITS_POS(pDecGlobal);
	if(IS_8X8(mb_type0))
	{
		//ref 
		VO_S32 ref_active = currSlice->allrefzero?1:currSlice->num_ref_idx_active[LIST_0];
		ref_cache = &pDecLocal->ref_cache[0][0];
		for(j = 0;j < 4;j++)
		{
			VO_S32 ref_in;
			if(ref_active == 1)
			{
				ref_in = 0;
			}
			else if(ref_active == 2)
			{
				ref_in = 1-GET_BITS_1_POS;		
				UPDATE_CACHE_POS(pDecGlobal);		
			}
			else
			{
				VO_U32 len = 0,inf;
				inf = SHOW_BITS_POS(32);
				if(inf >=(1<<27))
				{
					inf >>= 32-9;
					FLUSH_BITS_POS(VO_VLC_LEN[inf]);
					UPDATE_CACHE_POS(pDecGlobal);
					ref_in = VO_UE_VAL[inf];
				}
				else
				{
					len = 2*vo_big_len(inf)-31;
					inf >>= len;
					inf--;
					FLUSH_BITS_POS(32-len);
					UPDATE_CACHE_POS(pDecGlobal);
					ref_in = inf;
					if (ref_in<0)
					{
						return VO_H264_ERR_NUMREFFRAME;
					}
				}
			}
			ref_tmp[j] = ref_in;
			i = ref_index[j];
			ref_pic[i] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = list0[ref_in];
		}	
		for(j = 0;j < 4;j++)
		{
			VO_S32 sub_type = pDecLocal->b8type[j];
			VO_S32 block_width = (sub_type & (VO_16x16|VO_16x8))?2:1;
			i = cache_scan[4*j];
			M16(ref_cache+i) = M16(ref_cache+i+8) = (VO_U16)ref_tmp[j]*0x0101;	  
			for(i = 0;i < pDecLocal->b8sub[j];i++)
			{
				short pred_mv[2], curr_mv[2];
				VO_S32 index= 4*j + block_width*i;
				VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][cache_scan[index]];
				//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,cache_scan[index], &pred_mv, ref_tmp[j], LIST_0, block_i[index], block_j[index], step_h4, step_v4);
				GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,cache_scan[index], block_width, pred_mv,(short)ref_tmp[j],LIST_0);
				for (k=0; k < 2; ++k)
				{
					unsigned int value,len=0;
					value = SHOW_BITS_POS(32);
					if(value >=(1<<27))
					{
						value >>= 32-9;
						FLUSH_BITS_POS(VO_VLC_LEN[value]);
						UPDATE_CACHE_POS(pDecGlobal);
						value = VO_SE_VAL[value];
					}
					else
					{
						len = 2*vo_big_len(value)-31;
						value >>= len;

						FLUSH_BITS_POS(32-len);
						UPDATE_CACHE_POS(pDecGlobal);
						if(value&1)
							value = 0-(value>>1);
						else
							value = (value>>1);
					}
					curr_mv[k] = pred_mv[k]+ value;              
				}
				if(IS_SUB_8X8(sub_type))
					mv_cache[1] = mv_cache[8] = mv_cache[9] = M32(&curr_mv);
				else if(IS_SUB_8X4(sub_type))
					mv_cache[1] = M32(&curr_mv);
				else if(IS_SUB_4X8(sub_type))
					mv_cache[8] = M32(&curr_mv);
				mv_cache[0]= M32(&curr_mv);
			}
		}
	}
	else if(IS_16X16(mb_type0))
	{
		VO_S32 ref_in;
		VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
		short pred_mv[2], curr_mv[2];
		VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
		if(ref_active == 1)
		{
			ref_in = 0;
		}
		else if(ref_active == 2)
		{
			ref_in = 1-GET_BITS_1_POS;		
			UPDATE_CACHE_POS(pDecGlobal);		
		}
		else
		{
			VO_U32 len = 0,inf;
			inf = SHOW_BITS_POS(32);
			if(inf >=(1<<27))
			{
				inf >>= 32-9;
				FLUSH_BITS_POS(VO_VLC_LEN[inf]);
				UPDATE_CACHE_POS(pDecGlobal);
				ref_in = VO_UE_VAL[inf];
			}
			else
			{
				len = 2*vo_big_len(inf)-31;
				inf >>= len;
				inf--;
				FLUSH_BITS_POS(32-len);
				UPDATE_CACHE_POS(pDecGlobal);
				ref_in = inf;
				if (ref_in<0)
				{
					return VO_H264_ERR_NUMREFFRAME;
				}
			}
		}
		for(j4 = 4; j4 != 0;j4--)
		{
			*ref_pic++ = list0[ref_in];
			*ref_pic++ = list0[ref_in];
			*ref_pic++ = list0[ref_in];
			*ref_pic++ = list0[ref_in];
		}
		M32(ref_cache) = M32(ref_cache+8) = M32(ref_cache+16) = M32(ref_cache+24) = ref_in*0x01010101;
		//currSE.mapping = linfo_se;
		//readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0);
		//GetMotionVectorPredictor(pDecGlobal,pDecLocal, &pred_mv, ref_in, LIST_0);
		GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, pred_mv,(short)ref_in,LIST_0);
		for (k=0; k < 2; ++k)
		{
			unsigned int value,len=0;
			value = SHOW_BITS_POS(32);
			if(value >=(1<<27))
			{
				value >>= 32-9;
				FLUSH_BITS_POS(VO_VLC_LEN[value]);
				UPDATE_CACHE_POS(pDecGlobal);
				value = VO_SE_VAL[value];
			}
			else
			{
				len = 2*vo_big_len(value)-31;
				value >>= len;

				FLUSH_BITS_POS(32-len);
				UPDATE_CACHE_POS(pDecGlobal);
				if(value&1)
					value = 0-(value>>1);
				else
					value = (value>>1);
			}
			curr_mv[k] = pred_mv[k]+ value;               
		}
		mv_cache[0] = mv_cache[1] = mv_cache[2] = mv_cache[3] = M32(&curr_mv);
		mv_cache[8] = mv_cache[9] = mv_cache[10] = mv_cache[11] = M32(&curr_mv);
		mv_cache[16] = mv_cache[17] = mv_cache[18] = mv_cache[19] = M32(&curr_mv);
		mv_cache[24] = mv_cache[25] = mv_cache[26] = mv_cache[27] = M32(&curr_mv);
	}
	else if(IS_16X8(mb_type0))
	{
		VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
		ref_cache = &pDecLocal->ref_cache[0][12];
		for(j = 0;j < 2;j++)
		{
			VO_S32 ref_in;
			if(ref_active == 1)
			{
				ref_in = 0;
			}
			else if(ref_active == 2)
			{
				ref_in = 1-GET_BITS_1_POS;		
				UPDATE_CACHE_POS(pDecGlobal);		
			}
			else
			{
				VO_U32 len = 0,inf;
				inf = SHOW_BITS_POS(32);
				if(inf >=(1<<27))
				{
					inf >>= 32-9;
					FLUSH_BITS_POS(VO_VLC_LEN[inf]);
					UPDATE_CACHE_POS(pDecGlobal);
					ref_in = VO_UE_VAL[inf];
				}
				else
				{
					len = 2*vo_big_len(inf)-31;
					inf >>= len;
					inf--;
					FLUSH_BITS_POS(32-len);
					UPDATE_CACHE_POS(pDecGlobal);
					ref_in = inf;
					if (ref_in<0)
					{
						return VO_H264_ERR_NUMREFFRAME;
					}
				}
			}
			ref_tmp[j] = ref_in;
			i = j*8;
			ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+2] = ref_pic[i+3] = 
				ref_pic[i+4] = ref_pic[i+5] = ref_pic[i+6] = ref_pic[i+7] = list0[ref_in];
		}
		for(j = 0;j < 2;j++)
		{
			short pred_mv[2], curr_mv[2];
			VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
			i = j*16;
			ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+2] = ref_cache[i+3] = 
				ref_cache[i+8] = ref_cache[i+9] = ref_cache[i+10] = ref_cache[i+11] = (VO_S8)ref_tmp[j];
			//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_tmp[j], LIST_0, 0, i>>1, 16, 8);
			GetMotionVectorPredictor16x8(pDecGlobal,pDecLocal,12+i,LIST_0,ref_tmp[j],pred_mv);
			for (k=0; k < 2; ++k)
			{
				unsigned int value,len=0;
				value = SHOW_BITS_POS(32);
				if(value >=(1<<27))
				{
					value >>= 32-9;
					FLUSH_BITS_POS(VO_VLC_LEN[value]);
					UPDATE_CACHE_POS(pDecGlobal);
					value = VO_SE_VAL[value];
				}
				else
				{
					len = 2*vo_big_len(value)-31;
					value >>= len;

					FLUSH_BITS_POS(32-len);
					UPDATE_CACHE_POS(pDecGlobal);
					if(value&1)
						value = 0-(value>>1);
					else
						value = (value>>1);
				}
				curr_mv[k] = pred_mv[k]+ value;            
			}
			mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+2] = mv_cache[i+3] = 
				mv_cache[i+8] = mv_cache[i+9] = mv_cache[i+10] = mv_cache[i+11] = M32(&curr_mv);
		}
		//currSE.mapping = linfo_se;
		//readMBMotionVectors (pDecGlobal,pDecLocal,&currSE, dP, LIST_0, step_h0, step_v0);
	}
	else//8x16
	{
		VO_S32 ref_active = currSlice->num_ref_idx_active[LIST_0];
		ref_cache = &pDecLocal->ref_cache[0][12];
		for(j = 0;j < 2;j++)
		{
			VO_S32 ref_in;
			if(ref_active == 1)
			{
				ref_in = 0;
			}
			else if(ref_active == 2)
			{
				ref_in = 1-GET_BITS_1_POS;		
				UPDATE_CACHE_POS(pDecGlobal);		
			}
			else
			{
				VO_U32 len = 0,inf;
				inf = SHOW_BITS_POS(32);
				if(inf >=(1<<27))
				{
					inf >>= 32-9;
					FLUSH_BITS_POS(VO_VLC_LEN[inf]);
					UPDATE_CACHE_POS(pDecGlobal);
					ref_in = VO_UE_VAL[inf];
				}
				else
				{
					len = 2*vo_big_len(inf)-31;
					inf >>= len;
					inf--;
					FLUSH_BITS_POS(32-len);
					UPDATE_CACHE_POS(pDecGlobal);
					ref_in = inf;
					if (ref_in<0)
					{
						return VO_H264_ERR_NUMREFFRAME;
					}
				}
			}
			ref_tmp[j] = ref_in;
			i = j*2;
			ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = 
				ref_pic[i+8] = ref_pic[i+9] = ref_pic[i+12] = ref_pic[i+13] = list0[ref_in];
		}
		for(j = 0;j < 2;j++)
		{
			short pred_mv[2], curr_mv[2];
			VO_S32 *mv_cache = &pDecLocal->mv_cache[LIST_0][12];
			i = j*2;
			ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+8] = ref_cache[i+9] = 
				ref_cache[i+16] = ref_cache[i+17] = ref_cache[i+24] = ref_cache[i+25] = (VO_S8)ref_tmp[j];
			//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_tmp[j], LIST_0, i*4, 0, 8, 16);
			GetMotionVectorPredictor8x16(pDecGlobal,pDecLocal,12+i,LIST_0,ref_tmp[j],pred_mv);
			for (k=0; k < 2; ++k)
			{
				unsigned int value,len=0;
				value = SHOW_BITS_POS(32);
				if(value >=(1<<27))
				{
					value >>= 32-9;
					FLUSH_BITS_POS(VO_VLC_LEN[value]);
					UPDATE_CACHE_POS(pDecGlobal);
					value = VO_SE_VAL[value];
				}
				else
				{
					len = 2*vo_big_len(value)-31;
					value >>= len;

					FLUSH_BITS_POS(32-len);
					UPDATE_CACHE_POS(pDecGlobal);
					if(value&1)
						value = 0-(value>>1);
					else
						value = (value>>1);
				}
				curr_mv[k] = pred_mv[k]+ value;                  
			}
			mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+8] = mv_cache[i+9] = 
				mv_cache[i+16] = mv_cache[i+17] = mv_cache[i+24] = mv_cache[i+25] = M32(&curr_mv);
		}
	}
	END_BITS_POS(pDecGlobal);
	return 0;
}

static VO_S32 read_motion_info_from_NAL_b_slice (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
  Slice *currSlice = pDecLocal->p_Slice;
  //VideoParameters *p_Vid = currMB->p_Vid;
  DataPartition *dP = &currSlice->partArr[0];
  //int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
  //const byte *partMap = assignSE2partition[currSlice->dp_mode];
  int j4,i4,list,i,j;
  //int list_offset = pDecLocal->list_offset; // ((currSlice->mb_aff_frame_flag)&&(currMB->mb_field))? (mb_nr&0x01) ? 4 : 2 : 0;
  StorablePicture **cur_list;
  VO_S8 *ref_cache;
//   VO_S32 num_ref = currSlice->active_sps->frame_mbs_only_flag?currSlice->active_sps->num_ref_frames:currSlice->active_sps->num_ref_frames<<1;
  if (IS_8X8(mb_type0))
  {
    if( IS_DIRECT_VO(pDecLocal->b8type[0]|pDecLocal->b8type[1]|
                     pDecLocal->b8type[2]|pDecLocal->b8type[3]) )
      pDecGlobal->update_direct_mv_info(pDecGlobal,pDecLocal,&mb_type0); 
	if((pDecLocal->b8mode[0] == 0) | (pDecLocal->b8mode[1] == 0) | (pDecLocal->b8mode[2] == 0) | (pDecLocal->b8mode[3] == 0))
	{
	  pDecLocal->ref_cache[0][14]=pDecLocal->ref_cache[0][30]=
	  pDecLocal->ref_cache[1][14]=pDecLocal->ref_cache[1][30]= -2;
	}
	for(list = 0;list < 2;list++)
    {      
	  int k, j0, i0;
  	  int refframe;
	  ref_cache = &pDecLocal->ref_cache[list][12];
	  for (j0 = 0; j0 < 4; j0 += 2)
      {
        for (i0 = 0; i0 < 4; i0 += 2)
        {      
          k = 2 * (j0 >> 1) + (i0 >> 1);

          if ((pDecLocal->b8pdir[k] == list || pDecLocal->b8pdir[k] == BI_PRED) && !IS_DIRECT_VO(pDecLocal->b8type[k]))
          {
		    if(pDecLocal->p_Slice->num_ref_idx_active[list] > 1)
		    {
 			  refframe = readRefFrame_CABAC(pDecLocal, &dP->c,list,12+j0*8+i0);
			  if(refframe==-1)
		        VOH264ERROR(VO_H264_ERR_REF_NULL);
		    }
			else
		      refframe = 0; 
			//M16(&ref_cache[j0*8+i0])=M16(&ref_cache[j0*8+i0+8]) = refframe*0x0101;
			ref_cache[j0*8+i0+1]=ref_cache[j0*8+i0+8]=ref_cache[j0*8+i0+9]=refframe;
          }
        }
      }
	     
    }
	for(list = 0;list < 2;list++)
    {
      //StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
	  cur_list = currSlice->listX[currSlice->slice_number][list];
      ref_cache = &pDecLocal->ref_cache[list][0];
      for(j = 0;j < 4;j++)
	  {
	    ref_cache[cache_scan[4*j]] = ref_cache[cache_scan[4*j]+1];
	    if (!IS_DIRECT_VO(pDecLocal->b8type[j]))
	    {
	      if(pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED)
	      {
	        
	        VO_S32 sub_type = pDecLocal->b8type[j];
	        VO_S32 block_width = (sub_type & (VO_16x16|VO_16x8))?2:1;  
			
	        for(i = 0;i < pDecLocal->b8sub[j];i++)
	        {
	          short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
		      VO_S32 index= 4*j + block_width*i;
		      VO_S32 *mv_cache = &pDecLocal->mv_cache[list][cache_scan[index]];
		      VO_S32 *mvd_cache = &pDecLocal->mvd_cache[list][cache_scan[index]];
		      //pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,cache_scan[index], &pred_mv, ref_cache[cache_scan[index]], list, block_i[index], block_j[index], step_h4, step_v4);
              GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,cache_scan[index], block_width, pred_mv,ref_cache[cache_scan[index]],list);
		      curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,cache_scan[index],(short*)(mvd_cache-cache_scan[index]), 0,&st_mvd[0]);
		      curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,cache_scan[index],(short*)(mvd_cache-cache_scan[index]), 1,&st_mvd[1]);
	          curr_mv[0] = pred_mv[0]+curr_mvd[0];
	          curr_mv[1] = pred_mv[1]+curr_mvd[1];
		      if(IS_SUB_8X8(sub_type))
		      {
                mv_cache[1] = mv_cache[8] = mv_cache[9] = M32(&curr_mv);
		        mvd_cache[1] = mvd_cache[8] = mvd_cache[9] = M32(&st_mvd);
		      }
		      else if(IS_SUB_8X4(sub_type))
		      {
                mv_cache[1] = M32(&curr_mv);
		        mvd_cache[1] = M32(&st_mvd);
		      }
              else if(IS_SUB_4X8(sub_type))
              {
                mv_cache[8] = M32(&curr_mv);
		        mvd_cache[8] = M32(&st_mvd);
              }
              mv_cache[0]= M32(&curr_mv);
		      mvd_cache[0]= M32(&st_mvd);
	      	}
	      }
	    }
	  }
	  /*for(j4 = 0,i4=12; j4 < 16;)
      { 
	    ref_pic[j4++] = (ref_cache[i4] >= 0) ? cur_list[ref_cache[i4]] : NULL;
	    i4++;
	    ref_pic[j4++] = (ref_cache[i4] >= 0) ? cur_list[ref_cache[i4]] : NULL;
	    i4++;
	    ref_pic[j4++] = (ref_cache[i4] >= 0) ? cur_list[ref_cache[i4]] : NULL;
	    i4++;
	    ref_pic[j4++] = (ref_cache[i4] >= 0) ? cur_list[ref_cache[i4]] : NULL;
	    i4+=5;
      }  */
    }
  }
  else if (IS_16X16(mb_type0))
  {
    for(list = 0;list < 2;list++)
	{
	  if ((pDecLocal->b8pdir[0] == list || pDecLocal->b8pdir[0] == BI_PRED))
	  {
		//StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
		//StorablePicture* tmp_pic;
		int refframe;
		ref_cache = &pDecLocal->ref_cache[list][12];
		cur_list = currSlice->listX[currSlice->slice_number][list];
        if (pDecLocal->b8mode[0] != 0)
        {
		  if(pDecLocal->p_Slice->num_ref_idx_active[list] > 1)
		  {
		    refframe = readRefFrame_CABAC(pDecLocal, &dP->c,list,12);
			if(refframe==-1)
		        VOH264ERROR(VO_H264_ERR_REF_NULL);
		  }
	      else
		    refframe = 0; 
		  M32(ref_cache)=M32(ref_cache+8)=M32(ref_cache+16)=M32(ref_cache+24) = refframe*0x01010101;
        }
		
		/*tmp_pic = (ref_cache[0] >= 0) ? cur_list[ref_cache[0]] : NULL;
		for(j4 = 4; j4 != 0;j4--)
    	{
	  	  *ref_pic++ = tmp_pic;
	  	  *ref_pic++ = tmp_pic;
	  	  *ref_pic++ = tmp_pic;
	  	  *ref_pic++ = tmp_pic;
    	}	*/	
      }
    }
	for(list = 0;list < 2;list++)
	{
	  if ((pDecLocal->b8pdir[0] == list || pDecLocal->b8pdir[0] == BI_PRED))
	  {
	    short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
		VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
		VO_S32 *mvd_cache = &pDecLocal->mvd_cache[list][12];
		ref_cache = &pDecLocal->ref_cache[list][12];
	    //GetMotionVectorPredictor(pDecGlobal,pDecLocal, &pred_mv, ref_cache[0], list);
	    GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, pred_mv,ref_cache[0],list);
		curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,0,(short*)mvd_cache, 0,&st_mvd[0]);
		curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,0,(short*)mvd_cache, 1,&st_mvd[1]);
		curr_mv[0] = pred_mv[0]+curr_mvd[0];
		curr_mv[1] = pred_mv[1]+curr_mvd[1];
		mv_cache[0] = mv_cache[1] = mv_cache[2] = mv_cache[3] = M32(&curr_mv);
		mv_cache[8] = mv_cache[9] = mv_cache[10] = mv_cache[11] = M32(&curr_mv);
		mv_cache[16] = mv_cache[17] = mv_cache[18] = mv_cache[19] = M32(&curr_mv);
		mv_cache[24] = mv_cache[25] = mv_cache[26] = mv_cache[27] = M32(&curr_mv);
		mvd_cache[0] = mvd_cache[1] = mvd_cache[2] = mvd_cache[3] = M32(&st_mvd);
		mvd_cache[8] = mvd_cache[9] = mvd_cache[10] = mvd_cache[11] = M32(&st_mvd);
		mvd_cache[16] = mvd_cache[17] = mvd_cache[18] = mvd_cache[19] = M32(&st_mvd);
		mvd_cache[24] = mvd_cache[25] = mvd_cache[26] = mvd_cache[27] = M32(&st_mvd);
	  }
	}
  }
  else if(IS_16X8(mb_type0))
  {
    for(list = 0;list < 2;list++)
	{
	  //StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
	  int k, j0;
  	  int refframe;
	  ref_cache = &pDecLocal->ref_cache[list][12];
	  cur_list = currSlice->listX[currSlice->slice_number][list];
  
	  for (j0 = 0; j0 < 4; j0 += 2)
      {
        k = 2 * (j0 >> 1);

        if ((pDecLocal->b8pdir[k] == list || pDecLocal->b8pdir[k] == BI_PRED) && pDecLocal->b8mode[k] != 0)
        {
		  if(pDecLocal->p_Slice->num_ref_idx_active[list] > 1)
		  {
		    refframe = readRefFrame_CABAC(pDecLocal, &dP->c,list,12+j0*8);
			if(refframe==-1)
		        VOH264ERROR(VO_H264_ERR_REF_NULL);
		  }
		  else
		    refframe = 0; 
		  M32(&ref_cache[j0*8])=M32(&ref_cache[j0*8+8]) = refframe*0x01010101;
        }
      }
	  //ref_pic[0] = ref_pic[1] = ref_pic[2] = ref_pic[3] = 
	  //ref_pic[4] = ref_pic[5] = ref_pic[6] = ref_pic[7] = ref_cache[0] >=0?cur_list[ref_cache[0]]:NULL;
	  //ref_pic[8] = ref_pic[9] = ref_pic[10] = ref_pic[11] = 
	  //ref_pic[12] = ref_pic[13] = ref_pic[14] = ref_pic[15] = ref_cache[16] >=0?cur_list[ref_cache[16]]:NULL;	  
    }
	for(list = 0;list < 2;list++)
	{
	  ref_cache = &pDecLocal->ref_cache[list][12];
	  for(j = 0;j < 2;j++)
	  {
	    if ((pDecLocal->b8pdir[j*2] == list || pDecLocal->b8pdir[j*2] == BI_PRED))
	    {
	      short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
	  	  VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
	  	  VO_S32 *mvd_cache = &pDecLocal->mvd_cache[list][12];
	  	  i = j*16;
	  	  //pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_cache[i], list, 0, i>>1, 16, 8);
          GetMotionVectorPredictor16x8(pDecGlobal,pDecLocal,12+i,list,ref_cache[i],pred_mv);
	  	  curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 0,&st_mvd[0]);
	  	  curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 1,&st_mvd[1]);
	  	  curr_mv[0] = pred_mv[0]+curr_mvd[0];
	  	  curr_mv[1] = pred_mv[1]+curr_mvd[1];
	  	  mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+2] = mv_cache[i+3] = 
	  	  mv_cache[i+8] = mv_cache[i+9] = mv_cache[i+10] = mv_cache[i+11] = M32(&curr_mv);
	  	  mvd_cache[i+0] = mvd_cache[i+1] = mvd_cache[i+2] = mvd_cache[i+3] = 
	  	  mvd_cache[i+8] = mvd_cache[i+9] = mvd_cache[i+10] = mvd_cache[i+11] = M32(&st_mvd);
	    }
	  }
	}
  }
  else
  {
    for(list = 0;list < 2;list++)
	{
	  //StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
	  int k,i0;
  	  int refframe;
	  ref_cache = &pDecLocal->ref_cache[list][12];
	  cur_list = currSlice->listX[currSlice->slice_number][list];

  
      for (i0 = 0; i0 < 4; i0 += 2)
      {      
        k = (i0 >> 1);

        if ((pDecLocal->b8pdir[k] == list || pDecLocal->b8pdir[k] == BI_PRED) && pDecLocal->b8mode[k] != 0)
        {
		  if(pDecLocal->p_Slice->num_ref_idx_active[list] > 1)
		  {
		    refframe = readRefFrame_CABAC(pDecLocal, &dP->c,list,12+i0);
			if(refframe==-1)
		        VOH264ERROR(VO_H264_ERR_REF_NULL);
		  }
		  else
		    refframe = 0; 
		  M16(&ref_cache[i0])=M16(&ref_cache[i0+8])=M16(&ref_cache[i0+16])=M16(&ref_cache[i0+24]) = refframe*0x0101;
        }
      }
	  //ref_pic[0] = ref_pic[1] = ref_pic[4] = ref_pic[5] = 
	  //ref_pic[8] = ref_pic[9] = ref_pic[12] = ref_pic[13] = ref_cache[0] >=0?cur_list[ref_cache[0]]:NULL;
	  //ref_pic[2] = ref_pic[3] = ref_pic[6] = ref_pic[7] = 
	  //ref_pic[10] = ref_pic[11] = ref_pic[14] = ref_pic[15] = ref_cache[2] >=0?cur_list[ref_cache[2]]:NULL;	  
    }
	for(list = 0;list < 2;list++)
	{
	  ref_cache = &pDecLocal->ref_cache[list][12];
	  for(j = 0;j < 2;j++)
	  {
	    if ((pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED))
	    {
	      short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
	  	  VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
	  	  VO_S32 *mvd_cache = &pDecLocal->mvd_cache[list][12];
	  	  i = j*2;
	  	  //pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_cache[i], list, i*4, 0, 8, 16);
          GetMotionVectorPredictor8x16(pDecGlobal,pDecLocal,12+i,list,ref_cache[i],pred_mv);
	  	  curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 0,&st_mvd[0]);
	  	  curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 1,&st_mvd[1]);
	  	  curr_mv[0] = pred_mv[0]+curr_mvd[0];
	  	  curr_mv[1] = pred_mv[1]+curr_mvd[1];
	  	  mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+8] = mv_cache[i+9] = 
	  	  mv_cache[i+16] = mv_cache[i+17] = mv_cache[i+24] = mv_cache[i+25] = M32(&curr_mv);
	  	  mvd_cache[i+0] = mvd_cache[i+1] = mvd_cache[i+8] = mvd_cache[i+9] = 
	  	  mvd_cache[i+16] = mvd_cache[i+17] = mvd_cache[i+24] = mvd_cache[i+25] = M32(&st_mvd);
	    }
	  }
	}
  }
  return 0;
}


static VO_S32 read_motion_info_from_NAL_b_slice_interlace (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
	Slice *currSlice = pDecLocal->p_Slice;
	//VideoParameters *p_Vid = currMB->p_Vid;
	DataPartition *dP = &currSlice->partArr[0];
	int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
	//const byte *partMap = assignSE2partition[currSlice->dp_mode];
	int j4,i4,list,i,j;
	//int list_offset = pDecLocal->list_offset; // ((currSlice->mb_aff_frame_flag)&&(currMB->mb_field))? (mb_nr&0x01) ? 4 : 2 : 0;
	StorablePicture **cur_list;
	VO_S8 *ref_cache;
	//   VO_S32 num_ref = currSlice->active_sps->frame_mbs_only_flag?currSlice->active_sps->num_ref_frames:currSlice->active_sps->num_ref_frames<<1;
	if (IS_8X8(mb_type0))
	{
		if( IS_DIRECT_VO(pDecLocal->b8type[0]|pDecLocal->b8type[1]|
			pDecLocal->b8type[2]|pDecLocal->b8type[3]) )
			pDecGlobal->update_direct_mv_info(pDecGlobal,pDecLocal,&mb_type0); 
		if((pDecLocal->b8mode[0] == 0) | (pDecLocal->b8mode[1] == 0) | (pDecLocal->b8mode[2] == 0) | (pDecLocal->b8mode[3] == 0))
		{
			pDecLocal->ref_cache[0][14]=pDecLocal->ref_cache[0][30]=
				pDecLocal->ref_cache[1][14]=pDecLocal->ref_cache[1][30]= -2;
		}
		for(list = 0;list < 2;list++)
		{      
			int k, j0, i0;
			int refframe;
			ref_cache = &pDecLocal->ref_cache[list][12];
			for (j0 = 0; j0 < 4; j0 += 2)
			{
				for (i0 = 0; i0 < 4; i0 += 2)
				{      
					k = 2 * (j0 >> 1) + (i0 >> 1);

					if ((pDecLocal->b8pdir[k] == list || pDecLocal->b8pdir[k] == BI_PRED) && !IS_DIRECT_VO(pDecLocal->b8type[k]))
					{
						if(pDecLocal->p_Slice->num_ref_idx_active[list] > 1)
						{
							refframe = readRefFrame_CABAC(pDecLocal, &dP->c,list,12+j0*8+i0);
							if(refframe==-1)
								VOH264ERROR(VO_H264_ERR_REF_NULL);
						}
						else
							refframe = 0; 
						//M16(&ref_cache[j0*8+i0])=M16(&ref_cache[j0*8+i0+8]) = refframe*0x0101;
						ref_cache[j0*8+i0+1]=ref_cache[j0*8+i0+8]=ref_cache[j0*8+i0+9]=refframe;
					}
				}
			}

		}
		for(list = 0;list < 2;list++)
		{
			StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
			cur_list = currSlice->listX[currSlice->slice_number][list+list_offset];
			ref_cache = &pDecLocal->ref_cache[list][0];
			for(j = 0;j < 4;j++)
			{
				ref_cache[cache_scan[4*j]] = ref_cache[cache_scan[4*j]+1];
				if (!IS_DIRECT_VO(pDecLocal->b8type[j]))
				{
					if(pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED)
					{

						VO_S32 sub_type = pDecLocal->b8type[j];
						VO_S32 block_width = (sub_type & (VO_16x16|VO_16x8))?2:1;  

						for(i = 0;i < pDecLocal->b8sub[j];i++)
						{
							short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
							VO_S32 index= 4*j + block_width*i;
							VO_S32 *mv_cache = &pDecLocal->mv_cache[list][cache_scan[index]];
							VO_S32 *mvd_cache = &pDecLocal->mvd_cache[list][cache_scan[index]];
							//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,cache_scan[index], &pred_mv, ref_cache[cache_scan[index]], list, block_i[index], block_j[index], step_h4, step_v4);
							GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,cache_scan[index], block_width, pred_mv,ref_cache[cache_scan[index]],list);
							curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,cache_scan[index],(short*)(mvd_cache-cache_scan[index]), 0,&st_mvd[0]);
							curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,cache_scan[index],(short*)(mvd_cache-cache_scan[index]), 1,&st_mvd[1]);
							curr_mv[0] = pred_mv[0]+curr_mvd[0];
							curr_mv[1] = pred_mv[1]+curr_mvd[1];
							if(IS_SUB_8X8(sub_type))
							{
								mv_cache[1] = mv_cache[8] = mv_cache[9] = M32(&curr_mv);
								mvd_cache[1] = mvd_cache[8] = mvd_cache[9] = M32(&st_mvd);
							}
							else if(IS_SUB_8X4(sub_type))
							{
								mv_cache[1] = M32(&curr_mv);
								mvd_cache[1] = M32(&st_mvd);
							}
							else if(IS_SUB_4X8(sub_type))
							{
								mv_cache[8] = M32(&curr_mv);
								mvd_cache[8] = M32(&st_mvd);
							}
							mv_cache[0]= M32(&curr_mv);
							mvd_cache[0]= M32(&st_mvd);
						}
					}
				}
			}
			for(j4 = 0,i4=12; j4 < 16;)
			{ 
				ref_pic[j4++] = (ref_cache[i4] >= 0) ? cur_list[ref_cache[i4]] : NULL;
				i4++;
				ref_pic[j4++] = (ref_cache[i4] >= 0) ? cur_list[ref_cache[i4]] : NULL;
				i4++;
				ref_pic[j4++] = (ref_cache[i4] >= 0) ? cur_list[ref_cache[i4]] : NULL;
				i4++;
				ref_pic[j4++] = (ref_cache[i4] >= 0) ? cur_list[ref_cache[i4]] : NULL;
				i4+=5;
			}  
		}
	}
	else if (IS_16X16(mb_type0))
	{
		for(list = 0;list < 2;list++)
		{
			if ((pDecLocal->b8pdir[0] == list || pDecLocal->b8pdir[0] == BI_PRED))
			{
				StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
				StorablePicture* tmp_pic;
				int refframe;
				ref_cache = &pDecLocal->ref_cache[list][12];
				cur_list = currSlice->listX[currSlice->slice_number][list+list_offset];
				if (pDecLocal->b8mode[0] != 0)
				{
					if(pDecLocal->p_Slice->num_ref_idx_active[list] > 1)
					{
						refframe = readRefFrame_CABAC(pDecLocal, &dP->c,list,12);
						if(refframe==-1)
							VOH264ERROR(VO_H264_ERR_REF_NULL);
					}
					else
						refframe = 0; 
					M32(ref_cache)=M32(ref_cache+8)=M32(ref_cache+16)=M32(ref_cache+24) = refframe*0x01010101;
				}

				tmp_pic = (ref_cache[0] >= 0) ? cur_list[ref_cache[0]] : NULL;
				for(j4 = 4; j4 != 0;j4--)
				{
					*ref_pic++ = tmp_pic;
					*ref_pic++ = tmp_pic;
					*ref_pic++ = tmp_pic;
					*ref_pic++ = tmp_pic;
				}		
			}
		}
		for(list = 0;list < 2;list++)
		{
			if ((pDecLocal->b8pdir[0] == list || pDecLocal->b8pdir[0] == BI_PRED))
			{
				short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
				VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
				VO_S32 *mvd_cache = &pDecLocal->mvd_cache[list][12];
				ref_cache = &pDecLocal->ref_cache[list][12];
				//GetMotionVectorPredictor(pDecGlobal,pDecLocal, &pred_mv, ref_cache[0], list);
				GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, pred_mv,ref_cache[0],list);
				curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,0,(short*)mvd_cache, 0,&st_mvd[0]);
				curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,0,(short*)mvd_cache, 1,&st_mvd[1]);
				curr_mv[0] = pred_mv[0]+curr_mvd[0];
				curr_mv[1] = pred_mv[1]+curr_mvd[1];
				mv_cache[0] = mv_cache[1] = mv_cache[2] = mv_cache[3] = M32(&curr_mv);
				mv_cache[8] = mv_cache[9] = mv_cache[10] = mv_cache[11] = M32(&curr_mv);
				mv_cache[16] = mv_cache[17] = mv_cache[18] = mv_cache[19] = M32(&curr_mv);
				mv_cache[24] = mv_cache[25] = mv_cache[26] = mv_cache[27] = M32(&curr_mv);
				mvd_cache[0] = mvd_cache[1] = mvd_cache[2] = mvd_cache[3] = M32(&st_mvd);
				mvd_cache[8] = mvd_cache[9] = mvd_cache[10] = mvd_cache[11] = M32(&st_mvd);
				mvd_cache[16] = mvd_cache[17] = mvd_cache[18] = mvd_cache[19] = M32(&st_mvd);
				mvd_cache[24] = mvd_cache[25] = mvd_cache[26] = mvd_cache[27] = M32(&st_mvd);
			}
		}
	}
	else if(IS_16X8(mb_type0))
	{
		for(list = 0;list < 2;list++)
		{
			StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
			int k, j0;
			int refframe;
			ref_cache = &pDecLocal->ref_cache[list][12];
			cur_list = currSlice->listX[currSlice->slice_number][list+list_offset];

			for (j0 = 0; j0 < 4; j0 += 2)
			{
				k = 2 * (j0 >> 1);

				if ((pDecLocal->b8pdir[k] == list || pDecLocal->b8pdir[k] == BI_PRED) && pDecLocal->b8mode[k] != 0)
				{
					if(pDecLocal->p_Slice->num_ref_idx_active[list] > 1)
					{
						refframe = readRefFrame_CABAC(pDecLocal, &dP->c,list,12+j0*8);
						if(refframe==-1)
							VOH264ERROR(VO_H264_ERR_REF_NULL);
					}
					else
						refframe = 0; 
					M32(&ref_cache[j0*8])=M32(&ref_cache[j0*8+8]) = refframe*0x01010101;
				}
			}
			ref_pic[0] = ref_pic[1] = ref_pic[2] = ref_pic[3] = 
				ref_pic[4] = ref_pic[5] = ref_pic[6] = ref_pic[7] = ref_cache[0] >=0?cur_list[ref_cache[0]]:NULL;
			ref_pic[8] = ref_pic[9] = ref_pic[10] = ref_pic[11] = 
				ref_pic[12] = ref_pic[13] = ref_pic[14] = ref_pic[15] = ref_cache[16] >=0?cur_list[ref_cache[16]]:NULL;	  
		}
		for(list = 0;list < 2;list++)
		{
			ref_cache = &pDecLocal->ref_cache[list][12];
			for(j = 0;j < 2;j++)
			{
				if ((pDecLocal->b8pdir[j*2] == list || pDecLocal->b8pdir[j*2] == BI_PRED))
				{
					short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
					VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
					VO_S32 *mvd_cache = &pDecLocal->mvd_cache[list][12];
					i = j*16;
					//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_cache[i], list, 0, i>>1, 16, 8);
					GetMotionVectorPredictor16x8(pDecGlobal,pDecLocal,12+i,list,ref_cache[i],pred_mv);
					curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 0,&st_mvd[0]);
					curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 1,&st_mvd[1]);
					curr_mv[0] = pred_mv[0]+curr_mvd[0];
					curr_mv[1] = pred_mv[1]+curr_mvd[1];
					mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+2] = mv_cache[i+3] = 
						mv_cache[i+8] = mv_cache[i+9] = mv_cache[i+10] = mv_cache[i+11] = M32(&curr_mv);
					mvd_cache[i+0] = mvd_cache[i+1] = mvd_cache[i+2] = mvd_cache[i+3] = 
						mvd_cache[i+8] = mvd_cache[i+9] = mvd_cache[i+10] = mvd_cache[i+11] = M32(&st_mvd);
				}
			}
		}
	}
	else
	{
		for(list = 0;list < 2;list++)
		{
			StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
			int k,i0;
			int refframe;
			ref_cache = &pDecLocal->ref_cache[list][12];
			cur_list = currSlice->listX[currSlice->slice_number][list+list_offset];


			for (i0 = 0; i0 < 4; i0 += 2)
			{      
				k = (i0 >> 1);

				if ((pDecLocal->b8pdir[k] == list || pDecLocal->b8pdir[k] == BI_PRED) && pDecLocal->b8mode[k] != 0)
				{
					if(pDecLocal->p_Slice->num_ref_idx_active[list] > 1)
					{
						refframe = readRefFrame_CABAC(pDecLocal, &dP->c,list,12+i0);
						if(refframe==-1)
							VOH264ERROR(VO_H264_ERR_REF_NULL);
					}
					else
						refframe = 0; 
					M16(&ref_cache[i0])=M16(&ref_cache[i0+8])=M16(&ref_cache[i0+16])=M16(&ref_cache[i0+24]) = refframe*0x0101;
				}
			}
			ref_pic[0] = ref_pic[1] = ref_pic[4] = ref_pic[5] = 
				ref_pic[8] = ref_pic[9] = ref_pic[12] = ref_pic[13] = ref_cache[0] >=0?cur_list[ref_cache[0]]:NULL;
			ref_pic[2] = ref_pic[3] = ref_pic[6] = ref_pic[7] = 
				ref_pic[10] = ref_pic[11] = ref_pic[14] = ref_pic[15] = ref_cache[2] >=0?cur_list[ref_cache[2]]:NULL;	  
		}
		for(list = 0;list < 2;list++)
		{
			ref_cache = &pDecLocal->ref_cache[list][12];
			for(j = 0;j < 2;j++)
			{
				if ((pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED))
				{
					short pred_mv[2], curr_mv[2],curr_mvd[2],st_mvd[2];
					VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
					VO_S32 *mvd_cache = &pDecLocal->mvd_cache[list][12];
					i = j*2;
					//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_cache[i], list, i*4, 0, 8, 16);
					GetMotionVectorPredictor8x16(pDecGlobal,pDecLocal,12+i,list,ref_cache[i],pred_mv);
					curr_mvd[0] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 0,&st_mvd[0]);
					curr_mvd[1] = (short)read_MVD_CABAC(pDecLocal, &dP->c,i,(short*)mvd_cache, 1,&st_mvd[1]);
					curr_mv[0] = pred_mv[0]+curr_mvd[0];
					curr_mv[1] = pred_mv[1]+curr_mvd[1];
					mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+8] = mv_cache[i+9] = 
						mv_cache[i+16] = mv_cache[i+17] = mv_cache[i+24] = mv_cache[i+25] = M32(&curr_mv);
					mvd_cache[i+0] = mvd_cache[i+1] = mvd_cache[i+8] = mvd_cache[i+9] = 
						mvd_cache[i+16] = mvd_cache[i+17] = mvd_cache[i+24] = mvd_cache[i+25] = M32(&st_mvd);
				}
			}
		}
	}
	return 0;
}


static VO_S32 read_motion_info_from_NAL_b_slice_cavlc (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
  Slice *currSlice = pDecLocal->p_Slice;
  int j,i,k,list;
  //int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
  StorablePicture **cur_list;
  VO_S8 *ref_cache;
  BEGIN_BITS_POS(pDecGlobal);
  if (IS_8X8(mb_type0))
  {
    pDecGlobal->update_direct_mv_info(pDecGlobal,pDecLocal,&mb_type0); 
	
	for(list = 0;list < 2;list++)
	{
      VO_S32 ref_active = currSlice->num_ref_idx_active[list];
	  //StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
	  ref_cache = &pDecLocal->ref_cache[list][0];
	  cur_list = currSlice->listX[currSlice->slice_number][list];
	  for(j = 0;j < 4;j++)
	  {
	    VO_S32 ref_in;
		if (!IS_DIRECT_VO(pDecLocal->b8type[j]))
	    {
	      if (pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED)
	      {
	        if(ref_active == 2)
	        {
              ref_in = 1-GET_BITS_1_POS;		
		      UPDATE_CACHE_POS(pDecGlobal);
            }
	        else if(ref_active > 1)
            {
              VO_U32 len = 0,inf;
	          inf = SHOW_BITS_POS(32);
              if(inf >=(1<<27))
              {
                inf >>= 32-9;
              	FLUSH_BITS_POS(VO_VLC_LEN[inf]);
              	UPDATE_CACHE_POS(pDecGlobal);
              	ref_in = VO_UE_VAL[inf];
              }
              else
              {
                len = 2*vo_big_len(inf)-31;
              	inf >>= len;
              	inf--;
              	FLUSH_BITS_POS(32-len);
              	UPDATE_CACHE_POS(pDecGlobal);
              	ref_in = inf;
				if (ref_in<0)
				{
					return VO_H264_ERR_NUMREFFRAME;
				}
              }	
              
            }
	        else
            {		    
			  ref_in = 0;
	        }		
	        i = ref_index[j];
	        //ref_pic[i] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = ref_in >=0?cur_list[ref_in]:NULL;
		    i = cache_scan[4*j];
  	        //M16(ref_cache+i) = M16(ref_cache+i+8) = ref_in*0x0101;
			ref_cache[i+1]=ref_cache[i+8]=ref_cache[i+9]=(VO_S8)ref_in;
	      }
		  else
		  {
		    i = ref_index[j];
		    //ref_pic[i] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = NULL;
		  }
		}
		else
		{
		  i = ref_index[j];
		  //ref_pic[i] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = ref_cache[cache_scan[4*j]] >=0?cur_list[ref_cache[cache_scan[4*j]]]:NULL;
		}
	  }	
	}	
	if((pDecLocal->b8mode[0] == 0) | (pDecLocal->b8mode[1] == 0) | (pDecLocal->b8mode[2] == 0) | (pDecLocal->b8mode[3] == 0))
	{
	  pDecLocal->ref_cache[0][14]=pDecLocal->ref_cache[0][30]=
	  pDecLocal->ref_cache[1][14]=pDecLocal->ref_cache[1][30]= -2;
	}
	for(list = 0;list < 2;list++)
	{
	  ref_cache = &pDecLocal->ref_cache[list][0];
	  for(j = 0;j < 4;j++)
	  {
	    ref_cache[cache_scan[4*j]] = ref_cache[cache_scan[4*j]+1];
	    if ((pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED) && !IS_DIRECT_VO(pDecLocal->b8type[j]))
	    {
	      VO_S32 sub_type = pDecLocal->b8type[j];
	      VO_S32 block_width = (sub_type & (VO_16x16|VO_16x8))?2:1;  
	      for(i = 0;i < pDecLocal->b8sub[j];i++)
	      {
	        short pred_mv[2], curr_mv[2];
		    VO_S32 index= 4*j + block_width*i;
		    VO_S32 *mv_cache = &pDecLocal->mv_cache[list][cache_scan[index]];
		    //pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,cache_scan[index], &pred_mv, ref_cache[cache_scan[index]], list, block_i[index], block_j[index], step_h4, step_v4);
            GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,cache_scan[index], block_width, pred_mv,ref_cache[cache_scan[index]],list);
		    for (k=0; k < 2; ++k)
            {
              unsigned int value,len=0;
        	  value = SHOW_BITS_POS(32);
              if(value >=(1<<27))
              {
                value >>= 32-9;
                FLUSH_BITS_POS(VO_VLC_LEN[value]);
                UPDATE_CACHE_POS(pDecGlobal);
                value = VO_SE_VAL[value];
              }
              else
              {
                len = 2*vo_big_len(value)-31;
                value >>= len;
                	
                FLUSH_BITS_POS(32-len);
                UPDATE_CACHE_POS(pDecGlobal);
                if(value&1)
                  value = 0-(value>>1);
                else
                  value = (value>>1);
              }
	          curr_mv[k] = pred_mv[k]+ value;                    
            }
		    if(IS_SUB_8X8(sub_type))
              mv_cache[1] = mv_cache[8] = mv_cache[9] = M32(&curr_mv);
		    else if(IS_SUB_8X4(sub_type))
              mv_cache[1] = M32(&curr_mv);
            else if(IS_SUB_4X8(sub_type))
              mv_cache[8] = M32(&curr_mv);
            mv_cache[0]= M32(&curr_mv);
	      }
	    }
	  }
	}
  }
  else if (IS_16X16(mb_type0))
  {
    for(list = 0;list < 2;list++)
	{
	  if (pDecLocal->b8pdir[0] == list || pDecLocal->b8pdir[0] == BI_PRED)
	  {
        VO_S32 ref_in;
	    VO_S32 ref_active = currSlice->num_ref_idx_active[list];	     
	    //StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
	    ref_cache = &pDecLocal->ref_cache[list][12];
	    cur_list = currSlice->listX[currSlice->slice_number][list];
	    if(ref_active == 2)
	    {
          ref_in = 1-GET_BITS_1_POS;		
		  UPDATE_CACHE_POS(pDecGlobal);
        }
	    else if(ref_active > 1)
        {
          VO_U32 len = 0,inf;
          inf = SHOW_BITS_POS(32);
          if(inf >=(1<<27))
          {
            inf >>= 32-9;
          	FLUSH_BITS_POS(VO_VLC_LEN[inf]);
          	UPDATE_CACHE_POS(pDecGlobal);
          	ref_in = VO_UE_VAL[inf];
          }
          else
          {
            len = 2*vo_big_len(inf)-31;
          	inf >>= len;
          	inf--;
          	FLUSH_BITS_POS(32-len);
          	UPDATE_CACHE_POS(pDecGlobal);
          	ref_in = inf;
			if (ref_in<0)
			{
				return VO_H264_ERR_NUMREFFRAME;
			}
          }
        }
	    else
        {		    
		  ref_in = 0;
	    }		
	    //ref_pic[0] = ref_pic[1] = ref_pic[2] = ref_pic[3] = 
	    //ref_pic[4] = ref_pic[5] = ref_pic[6] = ref_pic[7] = 
	    //ref_pic[8] = ref_pic[9] = ref_pic[10] = ref_pic[11] = 
	    //ref_pic[12] = ref_pic[13] = ref_pic[14] = ref_pic[15] = ref_in >=0?cur_list[ref_in]:NULL;
	    M32(ref_cache) = M32(ref_cache+8) = M32(ref_cache+16) = M32(ref_cache+24) = ref_in*0x01010101;	      
      }
	  else
	  {
	    //StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
		//ref_pic[0] = ref_pic[1] = ref_pic[2] = ref_pic[3] = 
	    //ref_pic[4] = ref_pic[5] = ref_pic[6] = ref_pic[7] = 
	    //ref_pic[8] = ref_pic[9] = ref_pic[10] = ref_pic[11] = 
	    //ref_pic[12] = ref_pic[13] = ref_pic[14] = ref_pic[15] = NULL;
	  }
    }
	for(list = 0;list < 2;list++)
	{
	  if (pDecLocal->b8pdir[0] == list || pDecLocal->b8pdir[0] == BI_PRED)
	  {
	    short pred_mv[2], curr_mv[2];
	    VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
		//GetMotionVectorPredictor(pDecGlobal,pDecLocal, &pred_mv, pDecLocal->ref_cache[list][12], list);
        GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, pred_mv, pDecLocal->ref_cache[list][12],list);
	    for (k=0; k < 2; ++k)
        {
          unsigned int value,len=0;
    	  value = SHOW_BITS_POS(32);
          if(value >=(1<<27))
          {
            value >>= 32-9;
            FLUSH_BITS_POS(VO_VLC_LEN[value]);
            UPDATE_CACHE_POS(pDecGlobal);
            value = VO_SE_VAL[value];
          }
          else
          {
            len = 2*vo_big_len(value)-31;
            value >>= len;
            	
            FLUSH_BITS_POS(32-len);
            UPDATE_CACHE_POS(pDecGlobal);
            if(value&1)
              value = 0-(value>>1);
            else
              value = (value>>1);
          }
          curr_mv[k] = pred_mv[k]+ value;                    
        }
	    mv_cache[0] = mv_cache[1] = mv_cache[2] = mv_cache[3] = M32(&curr_mv);
	    mv_cache[8] = mv_cache[9] = mv_cache[10] = mv_cache[11] = M32(&curr_mv);
	    mv_cache[16] = mv_cache[17] = mv_cache[18] = mv_cache[19] = M32(&curr_mv);
	    mv_cache[24] = mv_cache[25] = mv_cache[26] = mv_cache[27] = M32(&curr_mv);
	  }
	}
  }
  else if(IS_16X8(mb_type0))
  {
    for(list = 0;list < 2;list++)
	{
      VO_S32 ref_active = currSlice->num_ref_idx_active[list];	  
	  //StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
	  ref_cache = &pDecLocal->ref_cache[list][12];
	  cur_list = currSlice->listX[currSlice->slice_number][list];
	  for(j = 0;j < 2;j++)
	  {
	    if ((pDecLocal->b8pdir[j*2] == list || pDecLocal->b8pdir[j*2] == BI_PRED))
	    {
	      VO_S32 ref_in;
	      if(ref_active == 2)
	      {
            ref_in = 1-GET_BITS_1_POS;		
		    UPDATE_CACHE_POS(pDecGlobal);
          }
	      else if(ref_active > 1)
          {
            VO_U32 len = 0,inf;
            inf = SHOW_BITS_POS(32);
            if(inf >=(1<<27))
            {
              inf >>= 32-9;
              FLUSH_BITS_POS(VO_VLC_LEN[inf]);
              UPDATE_CACHE_POS(pDecGlobal);
              ref_in = VO_UE_VAL[inf];
            }
            else
            {
              len = 2*vo_big_len(inf)-31;
              inf >>= len;
              inf--;
              FLUSH_BITS_POS(32-len);
              UPDATE_CACHE_POS(pDecGlobal);
              ref_in = inf;
			  if (ref_in<0)
			  {
				  return VO_H264_ERR_NUMREFFRAME;
			  }
            }	
          }
	      else
          {		    
		    ref_in = 0;
	      }		
	      i = j*8;
	      //ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+2] = ref_pic[i+3] = 
	      //ref_pic[i+4] = ref_pic[i+5] = ref_pic[i+6] = ref_pic[i+7] = ref_in >=0?cur_list[ref_in]:NULL;
	      i = j*16;
	      ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+2] = ref_cache[i+3] = 
	      ref_cache[i+8] = ref_cache[i+9] = ref_cache[i+10] = ref_cache[i+11] = (VO_S8)ref_in;
	    }
		else
		{
		  //i = j*8;
	      //ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+2] = ref_pic[i+3] = 
	      //ref_pic[i+4] = ref_pic[i+5] = ref_pic[i+6] = ref_pic[i+7] = NULL;
		}
	  }	  
    }
	for(list = 0;list < 2;list++)
	{
	  VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
	  ref_cache = &pDecLocal->ref_cache[list][12];
	  for(j = 0;j < 2;j++)
	  {
	    if ((pDecLocal->b8pdir[j*2] == list || pDecLocal->b8pdir[j*2] == BI_PRED))
	    {
	      short pred_mv[2], curr_mv[2];
	      i = j*16;
	      //pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_cache[i], list, 0, i>>1, 16, 8);
          GetMotionVectorPredictor16x8(pDecGlobal,pDecLocal,12+i,list,ref_cache[i],pred_mv);
	      for (k=0; k < 2; ++k)
          {
            unsigned int value,len=0;
      	    value = SHOW_BITS_POS(32);
            if(value >=(1<<27))
            {
              value >>= 32-9;
              FLUSH_BITS_POS(VO_VLC_LEN[value]);
              UPDATE_CACHE_POS(pDecGlobal);
              value = VO_SE_VAL[value];
            }
            else
            {
              len = 2*vo_big_len(value)-31;
              value >>= len;
              	
              FLUSH_BITS_POS(32-len);
              UPDATE_CACHE_POS(pDecGlobal);
              if(value&1)
                value = 0-(value>>1);
              else
                value = (value>>1);
            }
	        curr_mv[k] = pred_mv[k]+ value;                    
          }
	      mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+2] = mv_cache[i+3] = 
	      mv_cache[i+8] = mv_cache[i+9] = mv_cache[i+10] = mv_cache[i+11] = M32(&curr_mv);
	    }
	  }
	}
  }
  else
  {
    for(list = 0;list < 2;list++)
	{
      VO_S32 ref_active = currSlice->num_ref_idx_active[list];
	  //StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
	  ref_cache = &pDecLocal->ref_cache[list][12];
	  cur_list = currSlice->listX[currSlice->slice_number][list];
	  for(j = 0;j < 2;j++)
	  {
	    if ((pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED))
	    {
	      VO_S32 ref_in;
	      if(ref_active == 2)
	      {
            ref_in = 1-GET_BITS_1_POS;		
		    UPDATE_CACHE_POS(pDecGlobal);
          }
	      else if(ref_active > 1)
          {
            VO_U32 len = 0,inf;
            inf = SHOW_BITS_POS(32);
            if(inf >=(1<<27))
            {
              inf >>= 32-9;
              FLUSH_BITS_POS(VO_VLC_LEN[inf]);
              UPDATE_CACHE_POS(pDecGlobal);
              ref_in = VO_UE_VAL[inf];
            }
            else
            {
              len = 2*vo_big_len(inf)-31;
              inf >>= len;
              inf--;
              FLUSH_BITS_POS(32-len);
              UPDATE_CACHE_POS(pDecGlobal);
              ref_in = inf;
			  if (ref_in<0)
			  {
				  return VO_H264_ERR_NUMREFFRAME;
			  }
            }			
          }
	      else
          {		    
		    ref_in = 0;
	      }		
	      i = j*2;
	      //ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = 
	  	  //ref_pic[i+8] = ref_pic[i+9] = ref_pic[i+12] = ref_pic[i+13] = ref_in >=0?cur_list[ref_in]:NULL;
	      ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+8] = ref_cache[i+9] = 
	      ref_cache[i+16] = ref_cache[i+17] = ref_cache[i+24] = ref_cache[i+25] = (VO_S8)ref_in;
	    }
		else
		{
		  //i = j*2;
	      //ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = 
	  	  //ref_pic[i+8] = ref_pic[i+9] = ref_pic[i+12] = ref_pic[i+13] = NULL;
		}
	  }	  
    }
	for(list = 0;list < 2;list++)
	{
	  VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
	  ref_cache = &pDecLocal->ref_cache[list][12];
	  for(j = 0;j < 2;j++)
	  {
	    if ((pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED))
	    {
	      short pred_mv[2], curr_mv[2];
	      i = j*2;
	      //pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_cache[i], list, i*4, 0, 8, 16);
          GetMotionVectorPredictor8x16(pDecGlobal,pDecLocal,12+i,list,ref_cache[i],pred_mv);
	      for (k=0; k < 2; ++k)
          {
            unsigned int value,len=0;
      	    value = SHOW_BITS_POS(32);
            if(value >=(1<<27))
            {
              value >>= 32-9;
              FLUSH_BITS_POS(VO_VLC_LEN[value]);
              UPDATE_CACHE_POS(pDecGlobal);
              value = VO_SE_VAL[value];
            }
            else
            {
              len = 2*vo_big_len(value)-31;
              value >>= len;
              	
              FLUSH_BITS_POS(32-len);
              UPDATE_CACHE_POS(pDecGlobal);
              if(value&1)
                value = 0-(value>>1);
              else
                value = (value>>1);
            }
	        curr_mv[k] = pred_mv[k]+ value;                   
          }
	      mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+8] = mv_cache[i+9] = 
	      mv_cache[i+16] = mv_cache[i+17] = mv_cache[i+24] = mv_cache[i+25] = M32(&curr_mv);
	    }
	  }
	}
  }
  END_BITS_POS(pDecGlobal);
  return 0;
}


static VO_S32 read_motion_info_from_NAL_b_slice_cavlc_interlace (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0)
{
	Slice *currSlice = pDecLocal->p_Slice;
	int j,i,k,list;
	int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
	StorablePicture **cur_list;
	VO_S8 *ref_cache;
	BEGIN_BITS_POS(pDecGlobal);
	if (IS_8X8(mb_type0))
	{
		pDecGlobal->update_direct_mv_info(pDecGlobal,pDecLocal,&mb_type0); 

		for(list = 0;list < 2;list++)
		{
			VO_S32 ref_active = currSlice->num_ref_idx_active[list];
			StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
			ref_cache = &pDecLocal->ref_cache[list][0];
			cur_list = currSlice->listX[currSlice->slice_number][list+list_offset];
			for(j = 0;j < 4;j++)
			{
				VO_S32 ref_in;
				if (!IS_DIRECT_VO(pDecLocal->b8type[j]))
				{
					if (pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED)
					{
						if(ref_active == 2)
						{
							ref_in = 1-GET_BITS_1_POS;		
							UPDATE_CACHE_POS(pDecGlobal);
						}
						else if(ref_active > 1)
						{
							VO_U32 len = 0,inf;
							inf = SHOW_BITS_POS(32);
							if(inf >=(1<<27))
							{
								inf >>= 32-9;
								FLUSH_BITS_POS(VO_VLC_LEN[inf]);
								UPDATE_CACHE_POS(pDecGlobal);
								ref_in = VO_UE_VAL[inf];
							}
							else
							{
								len = 2*vo_big_len(inf)-31;
								inf >>= len;
								inf--;
								FLUSH_BITS_POS(32-len);
								UPDATE_CACHE_POS(pDecGlobal);
								ref_in = inf;
								if (ref_in<0)
								{
									return VO_H264_ERR_NUMREFFRAME;
								}
							}	

						}
						else
						{		    
							ref_in = 0;
						}		
						i = ref_index[j];
						ref_pic[i] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = ref_in >=0?cur_list[ref_in]:NULL;
						i = cache_scan[4*j];
						//M16(ref_cache+i) = M16(ref_cache+i+8) = ref_in*0x0101;
						ref_cache[i+1]=ref_cache[i+8]=ref_cache[i+9]=(VO_S8)ref_in;
					}
					else
					{
						i = ref_index[j];
						ref_pic[i] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = NULL;
					}
				}
				else
				{
					i = ref_index[j];
					ref_pic[i] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = ref_cache[cache_scan[4*j]] >=0?cur_list[ref_cache[cache_scan[4*j]]]:NULL;
				}
			}	
		}	
		if((pDecLocal->b8mode[0] == 0) | (pDecLocal->b8mode[1] == 0) | (pDecLocal->b8mode[2] == 0) | (pDecLocal->b8mode[3] == 0))
		{
			pDecLocal->ref_cache[0][14]=pDecLocal->ref_cache[0][30]=
				pDecLocal->ref_cache[1][14]=pDecLocal->ref_cache[1][30]= -2;
		}
		for(list = 0;list < 2;list++)
		{
			ref_cache = &pDecLocal->ref_cache[list][0];
			for(j = 0;j < 4;j++)
			{
				ref_cache[cache_scan[4*j]] = ref_cache[cache_scan[4*j]+1];
				if ((pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED) && !IS_DIRECT_VO(pDecLocal->b8type[j]))
				{
					VO_S32 sub_type = pDecLocal->b8type[j];
					VO_S32 block_width = (sub_type & (VO_16x16|VO_16x8))?2:1;  
					for(i = 0;i < pDecLocal->b8sub[j];i++)
					{
						short pred_mv[2], curr_mv[2];
						VO_S32 index= 4*j + block_width*i;
						VO_S32 *mv_cache = &pDecLocal->mv_cache[list][cache_scan[index]];
						//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,cache_scan[index], &pred_mv, ref_cache[cache_scan[index]], list, block_i[index], block_j[index], step_h4, step_v4);
						GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,cache_scan[index], block_width, pred_mv,ref_cache[cache_scan[index]],list);
						for (k=0; k < 2; ++k)
						{
							unsigned int value,len=0;
							value = SHOW_BITS_POS(32);
							if(value >=(1<<27))
							{
								value >>= 32-9;
								FLUSH_BITS_POS(VO_VLC_LEN[value]);
								UPDATE_CACHE_POS(pDecGlobal);
								value = VO_SE_VAL[value];
							}
							else
							{
								len = 2*vo_big_len(value)-31;
								value >>= len;

								FLUSH_BITS_POS(32-len);
								UPDATE_CACHE_POS(pDecGlobal);
								if(value&1)
									value = 0-(value>>1);
								else
									value = (value>>1);
							}
							curr_mv[k] = pred_mv[k]+ value;                    
						}
						if(IS_SUB_8X8(sub_type))
							mv_cache[1] = mv_cache[8] = mv_cache[9] = M32(&curr_mv);
						else if(IS_SUB_8X4(sub_type))
							mv_cache[1] = M32(&curr_mv);
						else if(IS_SUB_4X8(sub_type))
							mv_cache[8] = M32(&curr_mv);
						mv_cache[0]= M32(&curr_mv);
					}
				}
			}
		}
	}
	else if (IS_16X16(mb_type0))
	{
		for(list = 0;list < 2;list++)
		{
			if (pDecLocal->b8pdir[0] == list || pDecLocal->b8pdir[0] == BI_PRED)
			{
				VO_S32 ref_in;
				VO_S32 ref_active = currSlice->num_ref_idx_active[list];	     
				StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
				ref_cache = &pDecLocal->ref_cache[list][12];
				cur_list = currSlice->listX[currSlice->slice_number][list+list_offset];
				if(ref_active == 2)
				{
					ref_in = 1-GET_BITS_1_POS;		
					UPDATE_CACHE_POS(pDecGlobal);
				}
				else if(ref_active > 1)
				{
					VO_U32 len = 0,inf;
					inf = SHOW_BITS_POS(32);
					if(inf >=(1<<27))
					{
						inf >>= 32-9;
						FLUSH_BITS_POS(VO_VLC_LEN[inf]);
						UPDATE_CACHE_POS(pDecGlobal);
						ref_in = VO_UE_VAL[inf];
					}
					else
					{
						len = 2*vo_big_len(inf)-31;
						inf >>= len;
						inf--;
						FLUSH_BITS_POS(32-len);
						UPDATE_CACHE_POS(pDecGlobal);
						ref_in = inf;
						if (ref_in<0)
						{
							return VO_H264_ERR_NUMREFFRAME;
						}
					}
				}
				else
				{		    
					ref_in = 0;
				}		
				ref_pic[0] = ref_pic[1] = ref_pic[2] = ref_pic[3] = 
					ref_pic[4] = ref_pic[5] = ref_pic[6] = ref_pic[7] = 
					ref_pic[8] = ref_pic[9] = ref_pic[10] = ref_pic[11] = 
					ref_pic[12] = ref_pic[13] = ref_pic[14] = ref_pic[15] = ref_in >=0?cur_list[ref_in]:NULL;
				M32(ref_cache) = M32(ref_cache+8) = M32(ref_cache+16) = M32(ref_cache+24) = ref_in*0x01010101;	      
			}
			else
			{
				StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
				ref_pic[0] = ref_pic[1] = ref_pic[2] = ref_pic[3] = 
					ref_pic[4] = ref_pic[5] = ref_pic[6] = ref_pic[7] = 
					ref_pic[8] = ref_pic[9] = ref_pic[10] = ref_pic[11] = 
					ref_pic[12] = ref_pic[13] = ref_pic[14] = ref_pic[15] = NULL;
			}
		}
		for(list = 0;list < 2;list++)
		{
			if (pDecLocal->b8pdir[0] == list || pDecLocal->b8pdir[0] == BI_PRED)
			{
				short pred_mv[2], curr_mv[2];
				VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
				//GetMotionVectorPredictor(pDecGlobal,pDecLocal, &pred_mv, pDecLocal->ref_cache[list][12], list);
				GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, pred_mv, pDecLocal->ref_cache[list][12],list);
				for (k=0; k < 2; ++k)
				{
					unsigned int value,len=0;
					value = SHOW_BITS_POS(32);
					if(value >=(1<<27))
					{
						value >>= 32-9;
						FLUSH_BITS_POS(VO_VLC_LEN[value]);
						UPDATE_CACHE_POS(pDecGlobal);
						value = VO_SE_VAL[value];
					}
					else
					{
						len = 2*vo_big_len(value)-31;
						value >>= len;

						FLUSH_BITS_POS(32-len);
						UPDATE_CACHE_POS(pDecGlobal);
						if(value&1)
							value = 0-(value>>1);
						else
							value = (value>>1);
					}
					curr_mv[k] = pred_mv[k]+ value;                    
				}
				mv_cache[0] = mv_cache[1] = mv_cache[2] = mv_cache[3] = M32(&curr_mv);
				mv_cache[8] = mv_cache[9] = mv_cache[10] = mv_cache[11] = M32(&curr_mv);
				mv_cache[16] = mv_cache[17] = mv_cache[18] = mv_cache[19] = M32(&curr_mv);
				mv_cache[24] = mv_cache[25] = mv_cache[26] = mv_cache[27] = M32(&curr_mv);
			}
		}
	}
	else if(IS_16X8(mb_type0))
	{
		for(list = 0;list < 2;list++)
		{
			VO_S32 ref_active = currSlice->num_ref_idx_active[list];	  
			StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
			ref_cache = &pDecLocal->ref_cache[list][12];
			cur_list = currSlice->listX[currSlice->slice_number][list+list_offset];
			for(j = 0;j < 2;j++)
			{
				if ((pDecLocal->b8pdir[j*2] == list || pDecLocal->b8pdir[j*2] == BI_PRED))
				{
					VO_S32 ref_in;
					if(ref_active == 2)
					{
						ref_in = 1-GET_BITS_1_POS;		
						UPDATE_CACHE_POS(pDecGlobal);
					}
					else if(ref_active > 1)
					{
						VO_U32 len = 0,inf;
						inf = SHOW_BITS_POS(32);
						if(inf >=(1<<27))
						{
							inf >>= 32-9;
							FLUSH_BITS_POS(VO_VLC_LEN[inf]);
							UPDATE_CACHE_POS(pDecGlobal);
							ref_in = VO_UE_VAL[inf];
						}
						else
						{
							len = 2*vo_big_len(inf)-31;
							inf >>= len;
							inf--;
							FLUSH_BITS_POS(32-len);
							UPDATE_CACHE_POS(pDecGlobal);
							ref_in = inf;
							if (ref_in<0)
							{
								return VO_H264_ERR_NUMREFFRAME;
							}
						}	
					}
					else
					{		    
						ref_in = 0;
					}		
					i = j*8;
					ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+2] = ref_pic[i+3] = 
						ref_pic[i+4] = ref_pic[i+5] = ref_pic[i+6] = ref_pic[i+7] = ref_in >=0?cur_list[ref_in]:NULL;
					i = j*16;
					ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+2] = ref_cache[i+3] = 
						ref_cache[i+8] = ref_cache[i+9] = ref_cache[i+10] = ref_cache[i+11] = (VO_S8)ref_in;
				}
				else
				{
					i = j*8;
					ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+2] = ref_pic[i+3] = 
						ref_pic[i+4] = ref_pic[i+5] = ref_pic[i+6] = ref_pic[i+7] = NULL;
				}
			}	  
		}
		for(list = 0;list < 2;list++)
		{
			VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
			ref_cache = &pDecLocal->ref_cache[list][12];
			for(j = 0;j < 2;j++)
			{
				if ((pDecLocal->b8pdir[j*2] == list || pDecLocal->b8pdir[j*2] == BI_PRED))
				{
					short pred_mv[2], curr_mv[2];
					i = j*16;
					//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_cache[i], list, 0, i>>1, 16, 8);
					GetMotionVectorPredictor16x8(pDecGlobal,pDecLocal,12+i,list,ref_cache[i],pred_mv);
					for (k=0; k < 2; ++k)
					{
						unsigned int value,len=0;
						value = SHOW_BITS_POS(32);
						if(value >=(1<<27))
						{
							value >>= 32-9;
							FLUSH_BITS_POS(VO_VLC_LEN[value]);
							UPDATE_CACHE_POS(pDecGlobal);
							value = VO_SE_VAL[value];
						}
						else
						{
							len = 2*vo_big_len(value)-31;
							value >>= len;

							FLUSH_BITS_POS(32-len);
							UPDATE_CACHE_POS(pDecGlobal);
							if(value&1)
								value = 0-(value>>1);
							else
								value = (value>>1);
						}
						curr_mv[k] = pred_mv[k]+ value;                    
					}
					mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+2] = mv_cache[i+3] = 
						mv_cache[i+8] = mv_cache[i+9] = mv_cache[i+10] = mv_cache[i+11] = M32(&curr_mv);
				}
			}
		}
	}
	else
	{
		for(list = 0;list < 2;list++)
		{
			VO_S32 ref_active = currSlice->num_ref_idx_active[list];
			StorablePicture** ref_pic = &pDecLocal->pic_cache[list][0];
			ref_cache = &pDecLocal->ref_cache[list][12];
			cur_list = currSlice->listX[currSlice->slice_number][list+list_offset];
			for(j = 0;j < 2;j++)
			{
				if ((pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED))
				{
					VO_S32 ref_in;
					if(ref_active == 2)
					{
						ref_in = 1-GET_BITS_1_POS;		
						UPDATE_CACHE_POS(pDecGlobal);
					}
					else if(ref_active > 1)
					{
						VO_U32 len = 0,inf;
						inf = SHOW_BITS_POS(32);
						if(inf >=(1<<27))
						{
							inf >>= 32-9;
							FLUSH_BITS_POS(VO_VLC_LEN[inf]);
							UPDATE_CACHE_POS(pDecGlobal);
							ref_in = VO_UE_VAL[inf];
						}
						else
						{
							len = 2*vo_big_len(inf)-31;
							inf >>= len;
							inf--;
							FLUSH_BITS_POS(32-len);
							UPDATE_CACHE_POS(pDecGlobal);
							ref_in = inf;
							if (ref_in<0)
							{
								return VO_H264_ERR_NUMREFFRAME;
							}
						}			
					}
					else
					{		    
						ref_in = 0;
					}		
					i = j*2;
					ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = 
						ref_pic[i+8] = ref_pic[i+9] = ref_pic[i+12] = ref_pic[i+13] = ref_in >=0?cur_list[ref_in]:NULL;
					ref_cache[i+0] = ref_cache[i+1] = ref_cache[i+8] = ref_cache[i+9] = 
						ref_cache[i+16] = ref_cache[i+17] = ref_cache[i+24] = ref_cache[i+25] = (VO_S8)ref_in;
				}
				else
				{
					i = j*2;
					ref_pic[i+0] = ref_pic[i+1] = ref_pic[i+4] = ref_pic[i+5] = 
						ref_pic[i+8] = ref_pic[i+9] = ref_pic[i+12] = ref_pic[i+13] = NULL;
				}
			}	  
		}
		for(list = 0;list < 2;list++)
		{
			VO_S32 *mv_cache = &pDecLocal->mv_cache[list][12];
			ref_cache = &pDecLocal->ref_cache[list][12];
			for(j = 0;j < 2;j++)
			{
				if ((pDecLocal->b8pdir[j] == list || pDecLocal->b8pdir[j] == BI_PRED))
				{
					short pred_mv[2], curr_mv[2];
					i = j*2;
					//pDecGlobal->GetMVPredictor (pDecGlobal,pDecLocal,12+i, &pred_mv, ref_cache[i], list, i*4, 0, 8, 16);
					GetMotionVectorPredictor8x16(pDecGlobal,pDecLocal,12+i,list,ref_cache[i],pred_mv);
					for (k=0; k < 2; ++k)
					{
						unsigned int value,len=0;
						value = SHOW_BITS_POS(32);
						if(value >=(1<<27))
						{
							value >>= 32-9;
							FLUSH_BITS_POS(VO_VLC_LEN[value]);
							UPDATE_CACHE_POS(pDecGlobal);
							value = VO_SE_VAL[value];
						}
						else
						{
							len = 2*vo_big_len(value)-31;
							value >>= len;

							FLUSH_BITS_POS(32-len);
							UPDATE_CACHE_POS(pDecGlobal);
							if(value&1)
								value = 0-(value>>1);
							else
								value = (value>>1);
						}
						curr_mv[k] = pred_mv[k]+ value;                   
					}
					mv_cache[i+0] = mv_cache[i+1] = mv_cache[i+8] = mv_cache[i+9] = 
						mv_cache[i+16] = mv_cache[i+17] = mv_cache[i+24] = mv_cache[i+25] = M32(&curr_mv);
				}
			}
		}
	}
	END_BITS_POS(pDecGlobal);
	return 0;
}


int predict_nnz(VO_U8 *nz_coeff, int n)
{
  int pred_nnz = 0;
  int index = cache_scan[n];

  pred_nnz = nz_coeff[index-1];
  pred_nnz += nz_coeff[index-8];

  if(pred_nnz < 64)
  	pred_nnz = (pred_nnz+1)>>1;

  return pred_nnz&31;
}

static const byte Run[6][8] =
{
  {0x11,0x01},
  {0x22,0x12,0x01,0x01},
  {0x32,0x22,0x12,0x02},
  {0x43,0x33,0x22,0x22,0x12,0x12,0x02,0x02},
  {0x53,0x43,0x33,0x23,0x12,0x12,0x02,0x02},
  {0x13,0x23,0x43,0x33,0x63,0x53,0x02,0x02}
};
static const byte Run_readlen[6] = {5,4,4,3,3,3};
static const byte TZ4[5][16] = 
{   
  {0x04,0x14,0x23,0x23,0x33,0x33,0x53,0x53,
  0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41},
  {0x04,0x14,0x43,0x43,0x22,0x22,0x22,0x22,
  0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31},
  {0x03,0x13,0x32,0x32,0x21,0x21,0x21,0x21},
  {0x02,0x12,0x21,0x21},
  {0x01,0x11}
};
static const byte TZ4_readlen[5] = {2,2,3,4,5};
static const byte TZ6[9][22] = 
{
  {0x23,0x23,0x52,0x52,0x52,0x52,
  0x42,0x42,0x42,0x42,0x32,0x32,0x32,0x32,
  0x15,0x15,0x05,0x05,0x64,0x64,0x64,0x64},
  {0x53,0x53,0x62,0x62,0x62,0x62,
   0x42,0x42,0x42,0x42,0x32,0x32,0x32,0x32,
   0x16,0x06,0x75,0x75,0x24,0x24,0x24,0x24},
  {0x73,0x73,0x63,0x63,0x33,0x33,
   0x52,0x52,0x52,0x52,0x42,0x42,0x42,0x42,
   0x86,0x06,0x25,0x25,0x14,0x14,0x14,0x14},
  {0x83,0x83,0x63,0x63,0x43,0x43,
   0x33,0x33,0x23,0x23,0x52,0x52,0x52,0x52,
   0x96,0x06,0x15,0x15,0x74,0x74,0x74,0x74},
  {0x93,0x93,0x73,0x73,0x63,0x63,
   0x53,0x53,0x43,0x43,0x33,0x33,0x23,0x23,
   0xa6,0x06,0x15,0x15,0x84,0x84,0x84,0x84},
  {0x84,0x24,0x14,0x04,0x73,0x73,
   0x63,0x63,0x53,0x53,0x43,0x43,0x33,0x33,
   0xb5,0xb5,0x95,0x95,0xa4,0xa4,0xa4,0xa4},
  {0x94,0x74,0x34,0x24,0x83,0x83,
   0x63,0x63,0x53,0x53,0x43,0x43,0x13,0x13,
   0xc5,0xc5,0xb5,0xb5,0xa5,0xa5,0x05,0x05},
  {0x84,0x54,0x44,0x04,0x73,0x73,
   0x63,0x63,0x33,0x33,0x23,0x23,0x13,0x13,
   0xd6,0xb6,0xc5,0xc5,0xa5,0xa5,0x95,0x95},
  {0x84,0x74,0x64,0x54,0x43,0x43,
   0x33,0x33,0x23,0x23,0x13,0x13,0x03,0x03,
   0xe6,0xd6,0xc6,0xb6,0xa5,0xa5,0x95,0x95}
};
static const byte TZ0[61] = 
{ 
  0x65,0x55,0x44,0x44,0x34,0x34,
  0x23,0x23,0x23,0x23,0x13,0x13,0x13,0x13,
  0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
  0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
  	   0xf9,0xe9,0xd9,0xc8,0xc8,0xb8,0xb8,
  0xa7,0xa7,0xa7,0xa7,0x97,0x97,0x97,0x97,
  0x86,0x86,0x86,0x86,0x86,0x86,0x86,0x86,
  0x76,0x76,0x76,0x76,0x76,0x76,0x76,0x76
};
static const byte TZ_ChromaDC[3][8] =
{
  {0x33,0x23,0x12,0x12,0x01,0x01,0x01,0x01},
  {0x22,0x12,0x01,0x01},
  {0x11,0x01}
};
static const VO_U16 NC_ChromaDC[40] = 
{
  0x0000,0x4203,0x0002,0x0002,0x2101,0x2101,0x2101,0x2101,
  0x6407,0x6407,0x4408,0x2408,0x4307,0x4307,0x2307,0x2307,
  0x0406,0x0406,0x0406,0x0406,0x0306,0x0306,0x0306,0x0306,
  0x0206,0x0206,0x0206,0x0206,0x6306,0x6306,0x6306,0x6306,
  0x2206,0x2206,0x2206,0x2206,0x0106,0x0106,0x0106,0x0106
};
static const VO_U16 NC0[188]= 
{
  0x0000,0x4203,0x2102,0x2102,0x0001,0x0001,0x0001,0x0001,
							  0x6709,0x4509,0x2409,0x0309,
  0x6608,0x6608,0x4408,0x4408,0x2308,0x2308,0x0208,0x0208,
  0x6507,0x6507,0x6507,0x6507,0x4307,0x4307,0x4307,0x4307,
  0x6406,0x6406,0x6406,0x6406,0x6406,0x6406,0x6406,0x6406,
  0x2206,0x2206,0x2206,0x2206,0x2206,0x2206,0x2206,0x2206,
  0x0106,0x0106,0x0106,0x0106,0x0106,0x0106,0x0106,0x0106,
  0x6305,0x6305,0x6305,0x6305,0x6305,0x6305,0x6305,0x6305,
  0x6305,0x6305,0x6305,0x6305,0x6305,0x6305,0x6305,0x6305,
  0x080d,0x490d,0x280d,0x070d,0x6a0d,0x480d,0x270d,0x060d,
  0x690b,0x690b,0x690b,0x690b,0x470b,0x470b,0x470b,0x470b,
  0x260b,0x260b,0x260b,0x260b,0x050b,0x050b,0x050b,0x050b,
  0x680a,0x680a,0x680a,0x680a,0x680a,0x680a,0x680a,0x680a,
  0x460a,0x460a,0x460a,0x460a,0x460a,0x460a,0x460a,0x460a,
  0x250a,0x250a,0x250a,0x250a,0x250a,0x250a,0x250a,0x250a,
  0x040a,0x040a,0x040a,0x040a,0x040a,0x040a,0x040a,0x040a,			  	
  0x0000,0x0000,0x2d0f,0x2d0f,0x1010,0x5010,0x3010,0x0f10,
  0x7010,0x4f10,0x2f10,0x0e10,0x6f10,0x4e10,0x2e10,0x0d10,
  0x6e0f,0x6e0f,0x4d0f,0x4d0f,0x2c0f,0x2c0f,0x0c0f,0x0c0f,
  0x6d0f,0x6d0f,0x4c0f,0x4c0f,0x2b0f,0x2b0f,0x0b0f,0x0b0f,
  0x6c0e,0x6c0e,0x6c0e,0x6c0e,0x4b0e,0x4b0e,0x4b0e,0x4b0e,
  0x2a0e,0x2a0e,0x2a0e,0x2a0e,0x0a0e,0x0a0e,0x0a0e,0x0a0e,
  0x6b0e,0x6b0e,0x6b0e,0x6b0e,0x4a0e,0x4a0e,0x4a0e,0x4a0e,
  0x290e,0x290e,0x290e,0x290e,0x090e,0x090e,0x090e,0x090e
};
static const VO_U16 NC1[216]= 
{   					 	
                              0x6706,0x4406,0x2406,0x0206,
  0x6606,0x4306,0x2306,0x0106,0x6505,0x6505,0x2205,0x2205,
  0x6404,0x6404,0x6404,0x6404,0x6304,0x6304,0x6304,0x6304,
  0x4203,0x4203,0x4203,0x4203,0x4203,0x4203,0x4203,0x4203,
  0x2102,0x2102,0x2102,0x2102,0x2102,0x2102,0x2102,0x2102,
  0x2102,0x2102,0x2102,0x2102,0x2102,0x2102,0x2102,0x2102,
  0x0002,0x0002,0x0002,0x0002,0x0002,0x0002,0x0002,0x0002,
  0x0002,0x0002,0x0002,0x0002,0x0002,0x0002,0x0002,0x0002,
							  0x6909,0x4709,0x2709,0x0609,
  0x0508,0x0508,0x4608,0x4608,0x2608,0x2608,0x0408,0x0408,
  0x6807,0x6807,0x6807,0x6807,0x4507,0x4507,0x4507,0x4507,
  0x2507,0x2507,0x2507,0x2507,0x0307,0x0307,0x0307,0x0307,
  0x0000,0x0000,0x6f0d,0x6f0d,0x700e,0x500e,0x300e,0x100e,
  0x2f0e,0x0f0e,0x4f0e,0x2e0e,0x4e0d,0x4e0d,0x0e0d,0x0e0d,
  0x6e0d,0x6e0d,0x4d0d,0x4d0d,0x2d0d,0x2d0d,0x0d0d,0x0d0d,
  0x6d0d,0x6d0d,0x4c0d,0x4c0d,0x2c0d,0x2c0d,0x0c0d,0x0c0d,
  0x0b0c,0x0b0c,0x0b0c,0x0b0c,0x4b0c,0x4b0c,0x4b0c,0x4b0c,
  0x2b0c,0x2b0c,0x2b0c,0x2b0c,0x0a0c,0x0a0c,0x0a0c,0x0a0c,
  0x6c0c,0x6c0c,0x6c0c,0x6c0c,0x4a0c,0x4a0c,0x4a0c,0x4a0c,
  0x2a0c,0x2a0c,0x2a0c,0x2a0c,0x090c,0x090c,0x090c,0x090c,
  0x6b0b,0x6b0b,0x6b0b,0x6b0b,0x6b0b,0x6b0b,0x6b0b,0x6b0b,
  0x490b,0x490b,0x490b,0x490b,0x490b,0x490b,0x490b,0x490b,
  0x290b,0x290b,0x290b,0x290b,0x290b,0x290b,0x290b,0x290b,
  0x080b,0x080b,0x080b,0x080b,0x080b,0x080b,0x080b,0x080b,
  0x6a0b,0x6a0b,0x6a0b,0x6a0b,0x6a0b,0x6a0b,0x6a0b,0x6a0b,
  0x480b,0x480b,0x480b,0x480b,0x480b,0x480b,0x480b,0x480b,
  0x280b,0x280b,0x280b,0x280b,0x280b,0x280b,0x280b,0x280b,
  0x070b,0x070b,0x070b,0x070b,0x070b,0x070b,0x070b,0x070b
};
static const VO_U16 NC2[184]= 
{
  0x0707,0x0607,0x4907,0x0507,0x6a07,0x4807,0x2807,0x0407,
  0x0306,0x0306,0x4706,0x4706,0x2706,0x2706,0x0206,0x0206,
  0x6906,0x6906,0x4606,0x4606,0x2606,0x2606,0x0106,0x0106,
  0x2505,0x2505,0x2505,0x2505,0x4505,0x4505,0x4505,0x4505,
  0x2405,0x2405,0x2405,0x2405,0x4405,0x4405,0x4405,0x4405,
  0x2305,0x2305,0x2305,0x2305,0x6805,0x6805,0x6805,0x6805,
  0x4305,0x4305,0x4305,0x4305,0x2205,0x2205,0x2205,0x2205,
  0x6704,0x6704,0x6704,0x6704,0x6704,0x6704,0x6704,0x6704,
  0x6604,0x6604,0x6604,0x6604,0x6604,0x6604,0x6604,0x6604,
  0x6504,0x6504,0x6504,0x6504,0x6504,0x6504,0x6504,0x6504,
  0x6404,0x6404,0x6404,0x6404,0x6404,0x6404,0x6404,0x6404,
  0x6304,0x6304,0x6304,0x6304,0x6304,0x6304,0x6304,0x6304,
  0x4204,0x4204,0x4204,0x4204,0x4204,0x4204,0x4204,0x4204,
  0x2104,0x2104,0x2104,0x2104,0x2104,0x2104,0x2104,0x2104,
  0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,
  0x0000,0x100a,0x700a,0x500a,0x300a,0x0f0a,0x6f0a,0x4f0a,
  0x2f0a,0x0e0a,0x6e0a,0x4e0a,0x2e0a,0x0d0a,0x2d09,0x2d09,
  0x0c09,0x0c09,0x4d09,0x4d09,0x2c09,0x2c09,0x0b09,0x0b09,
  0x6d09,0x6d09,0x4c09,0x4c09,0x2b09,0x2b09,0x0a09,0x0a09,
  0x6c08,0x6c08,0x6c08,0x6c08,0x4b08,0x4b08,0x4b08,0x4b08,
  0x2a08,0x2a08,0x2a08,0x2a08,0x0908,0x0908,0x0908,0x0908,
  0x6b08,0x6b08,0x6b08,0x6b08,0x4a08,0x4a08,0x4a08,0x4a08,
  0x2908,0x2908,0x2908,0x2908,0x0808,0x0808,0x0808,0x0808
};
static const byte NC3[64] = 
{
  0x01,0x21,0x21,0x00,0x02,0x22,0x42,0x42,
  0x03,0x23,0x43,0x63,0x04,0x24,0x44,0x64,
  0x05,0x25,0x45,0x65,0x06,0x26,0x46,0x66,
  0x07,0x27,0x47,0x67,0x08,0x28,0x48,0x68,
  0x09,0x29,0x49,0x69,0x0a,0x2a,0x4a,0x6a,
  0x0b,0x2b,0x4b,0x6b,0x0c,0x2c,0x4c,0x6c,
  0x0d,0x2d,0x4d,0x6d,0x0e,0x2e,0x4e,0x6e,
  0x0f,0x2f,0x4f,0x6f,0x10,0x30,0x50,0x70
};

static VO_S32 readCoeff4x4_CAVLC (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, 
                                int block_type,int qp,int shift_qp,
                                int n,const VO_U8 *scan,int* inv,short* cof,
                                int *number_coefficients,int max_coeff_num)
{
  int k,i,j, vlcnum,coeffindex;
  int numcoeff = 0, numtrailingones;
  int level_two_or_higher;
  int numones, totzeros, cdc=0;
  int levarr[16];
  //char type[15];
  static const int incVlc[] = {0, 3, 6, 12, 24, 48, 32768};    // maximum vlc = 6
  VO_U8 *nz_coeff = pDecLocal->nz_coeff;
  BEGIN_BITS_POS(pDecGlobal);

  cdc = block_type == CHROMA_DC;

  if (!cdc)
  { 
    int nnz = predict_nnz(nz_coeff, n);
//edit by Really Yang 20110314
    if (nnz < 2)
    {
	  //readSyntaxElement_NumCoeffTrailingOnes_nC0(pDecGlobal,&currSE);
	  int code,tcode;
      code = SHOW_BITS_POS(16);
	  tcode = code>>13;
	  if(tcode==0)
	  {
	    code&=0x1fff;
		tcode = code>>7;
		if(tcode>3)
		{
	  	  tcode += 4;
		}
		else
		{
		  code&=0x1ff;
		  tcode = code>>3;
		  if(tcode>7)
	  	    tcode += 60;
		  else
		  	tcode = (code&0x3f)+124;
		}
	  }
	  numtrailingones = NC0[tcode]>>13;
	  numcoeff = (NC0[tcode]>>8)&0x1f;
	  FLUSH_BITS_POS(NC0[tcode]&0x00ff);
	  UPDATE_CACHE_POS( pDecGlobal );
    }
    else if (nnz < 4)
    {
	  //readSyntaxElement_NumCoeffTrailingOnes_nC1(pDecGlobal,&currSE);
	  int code,tcode;
	  code = SHOW_BITS_POS(14);
	  tcode = code>>8;
	  if(tcode>3)
	  {
	    tcode-=4;
	  }
	  else
	  {
	    code&=0x3ff;
		tcode = code>>5;
		if(tcode>3)
	  	  tcode += 56;
		else
		  tcode = (code&0x7f)+88;
	  }
	  numtrailingones = NC1[tcode]>>13;
	  numcoeff = (NC1[tcode]>>8)&0x1f;
	  FLUSH_BITS_POS(NC1[tcode]&0x000f);
	  UPDATE_CACHE_POS( pDecGlobal );
    }
    else if (nnz < 8)
    {
      //currSE.value1 = 2;
	  //readSyntaxElement_NumCoeffTrailingOnes_nC2(pDecGlobal,&currSE);
	  int code,tcode;
      code = SHOW_BITS_POS(10);
      tcode = code>>3;
      if(tcode>7)
	    tcode-=8;
	  else
	  	tcode = (code&0x3f)+120;
	  numtrailingones = NC2[tcode]>>13;
	  numcoeff = (NC2[tcode]>>8)&0x1f;
	  FLUSH_BITS_POS(NC2[tcode]&0x000f);
	  UPDATE_CACHE_POS( pDecGlobal );
    }
    else //
    {
	  //readSyntaxElement_NumCoeffTrailingOnes_nC3(pDecGlobal,&currSE);
	  int code;
	  code = SHOW_BITS_POS(6);
	  FLUSH_BITS_POS(6);
	  UPDATE_CACHE_POS( pDecGlobal );
	  numtrailingones = NC3[code]>>5;
	  numcoeff = NC3[code]&0x1f;
    }
//end of edit

    if(block_type!=LUMA_INTRA16x16DC)
	{  
	  nz_coeff[cache_scan[n]] = (byte) numcoeff;
    }      
  }
  else
  {
	//readSyntaxElement_NumCoeffTrailingOnesChromaDC420(pDecGlobal,&currSE);
    int code,tcode;
    code = SHOW_BITS_POS(8);
    tcode = code>>5;
    if(tcode==0)
  	  tcode = (code&0x1f)+8;

    numtrailingones = NC_ChromaDC[tcode]>>13;
    numcoeff = (NC_ChromaDC[tcode]>>8)&0x1f;
    FLUSH_BITS_POS(NC_ChromaDC[tcode]&0x000f);
    UPDATE_CACHE_POS( pDecGlobal );
  }

  numones = numtrailingones;
  *number_coefficients = numcoeff;

  if (numcoeff)
  {
    int shift,sb,code,len,level,sign,levabs;
    if (numtrailingones)
    {      
      //currSE.len = numtrailingones;

      //readSyntaxElement_FLC (pDecGlobal,&currSE);
      code = SHOW_BITS_POS(3);
	  FLUSH_BITS_POS(numtrailingones);
      //currSE.value1 = currSE.inf;
      UPDATE_CACHE_POS( pDecGlobal );
      levarr[0] = 1-((code&4)>>1);
      levarr[1] = 1-((code&2)   );
      levarr[2] = 1-((code&1)<<1);
      //code = currSE.inf;
      /*ntr = numtrailingones;
      for (k = numcoeff - 1; k > numcoeff - 1 - numtrailingones; k--)
      {
        ntr --;
        //levarr[k] = (code>>ntr)&1 ? -1 : 1;
		levarr[k] = 1-(((code>>ntr)&1)<<1);
      }*/
    }

    // decode levels
    level_two_or_higher = (numcoeff > 3 && numtrailingones == 3)? 0 : 1;
    vlcnum = (numcoeff > 10 && numtrailingones < 3) ? 1 : 0;

    for (k = numtrailingones; k < numcoeff; k++)
    {
      if (vlcnum == 0)
      {
        len = 1;		
#if defined(__GNUC__) &&  defined(ARM)
        code = SHOW_BITS_POS(32);
			  __asm__ __volatile__ (
						"clz %0, %1 \n\t"
						:"=r"(len)
						:"r"(code)
			 );
		len++;	  
        FLUSH_BITS_POS(len)
		UPDATE_CACHE_POS( pDecGlobal );
#else

	    while (!GET_BITS_1_POS)
	    {
	      len++;
		  if(len==32)
		  {
		    VOH264ERROR(VO_H264_ERR_TOTALZEROS_CAVLC);
		    END_BITS_POS( pDecGlobal )
		  }
	    }
	    UPDATE_CACHE_POS( pDecGlobal );
#endif	  
	    if (len < 15)
	    {
	      sign  = (len - 1) & 1;
	      level = ((len - 1) >> 1) + 1;
	    }
	    else if (len == 15)
	    {
	      // escape code
	      code = 1<<4;
	      code |= SHOW_BITS_POS(4);
		  FLUSH_BITS_POS(4);
	      //len  += 4;
	      //frame_bitoffset += 4;
	      sign = (code & 0x01);
	      level = ((code >> 1) & 0x07) + 8;
	    }
	    else if (len >= 16)
	    {
	      // escape code
	      int addbit = (len - 16);
	      int offset = (2048 << addbit) - 2032;
	      len   -= 4;
	      code   = SHOW_BITS_POS(len);
		  FLUSH_BITS_POS(len);
	      sign   = (code & 0x01);
	      //frame_bitoffset += len;    
	      level = (code >> 1) + offset;

	      //code |= (1 << (len)); // for display purpose only
	      //len += addbit + 16;
	    }
	    UPDATE_CACHE_POS( pDecGlobal );
	    levarr[k] = (sign) ? -level : level ;
      }
      else
      {
        //readSyntaxElement_Level_VLCN(pDecGlobal,&currSE, vlcnum);
	    len = 1;
	    
	    shift = vlcnum - 1;
#if defined(__GNUC__) &&  defined(ARM)
				code = SHOW_BITS_POS(32);
					  __asm__ __volatile__ (
								"clz %0, %1 \n\t"
								:"=r"(len)
								:"r"(code)
					 );
				len++;	  
				FLUSH_BITS_POS(len)
				UPDATE_CACHE_POS( pDecGlobal );
#else

	    // read pre zeros
	    while (!GET_BITS_1_POS)
	    {
	      len++;
		  if(len==32)
		  {
		    VOH264ERROR(VO_H264_ERR_TOTALZEROS_CAVLC);
		    END_BITS_POS( pDecGlobal )
		  }
	    }
	    UPDATE_CACHE_POS( pDecGlobal );
#endif
        code = 1;//, sb;
	    if (len < 16)
	    {
	      levabs = ((len - 1) << shift) + 1;

	      // read (vlc-1) bits -> suffix
	      if (shift)
	      {
	        sb =  SHOW_BITS_POS(shift);
			FLUSH_BITS_POS(shift);
		    //UPDATE_CACHE(pDecGlobal);
	        code = (code << (shift) )| sb;
	        levabs += sb;
	        //len += (shift);
	      }

	      // read 1 bit -> sign
	      sign = GET_BITS_1_POS;
	      code = (code << 1)| sign;
	      //len ++;
	    }
	    else // escape
	    {
	      int addbit = len - 5;
	      int offset = (1 << addbit) + (15 << shift) - 2047;

	      sb = SHOW_BITS_POS(addbit);
		  FLUSH_BITS_POS(addbit);
		  //UPDATE_CACHE(pDecGlobal);
	      code = (code << addbit ) | sb;
	      //len   += addbit;

	      levabs = sb + offset;
	    
	      // read 1 bit -> sign
	      sign = GET_BITS_1_POS;

	      code = (code << 1)| sign;

	      //len++;
	    }
	    UPDATE_CACHE_POS( pDecGlobal );
	    levarr[k] = (sign)? -levabs : levabs;
      }

      if (level_two_or_higher)
      {
        levarr[k] += (levarr[k] > 0) ? 1 : -1;
        level_two_or_higher = 0;
      }

      //levarr[k] = currSE.inf;
      levabs = iabs(levarr[k]);
      if (levabs  == 1)
        ++numones;

      // update VLC table
      if (levabs  > incVlc[vlcnum])
        ++vlcnum;

      if (k ==  numtrailingones && levabs >3)
        vlcnum = 2;      
    }

    if (numcoeff < max_coeff_num)
    {
      // decode total run
      vlcnum = numcoeff - 1;
      if (cdc)
      {
        int code;
	    const byte *DC_table = TZ_ChromaDC[vlcnum];
	    int read_len = vlcnum+3;

	    code = SHOW_BITS_POS(6);
	    code >>= read_len;
	    totzeros = DC_table[code]>>4;
	  
	    FLUSH_BITS_POS(DC_table[code]&0x0f);
	    UPDATE_CACHE_POS( pDecGlobal );
      }
      else
      {
        if(vlcnum == 0)
        {
          int code,tcode;
		  code = SHOW_BITS_POS(9);
		  tcode = code>>4;
		  if(tcode>1)
		    tcode-=2;
		  else
		  	tcode = (code&0x1f)+29;
			
		  totzeros = TZ0[tcode]>>4;
		  FLUSH_BITS_POS(TZ0[tcode]&0x0f);
		  UPDATE_CACHE_POS( pDecGlobal );
        }
		else if(vlcnum <= 9)
        {
          int code,tcode;
  		  const byte *TZ_table;
  		  TZ_table = TZ6[9-vlcnum];
  		  code = SHOW_BITS_POS(6);
  		  tcode = code >> 2;
  		  if(tcode > 1)
  			tcode-=2;
  		  else
    		tcode = (code&0x07)+14; 

  		  totzeros = TZ_table[tcode]>>4;
  		  FLUSH_BITS_POS(TZ_table[tcode]&0x0f);
  		  UPDATE_CACHE_POS( pDecGlobal );
        }
		else
		{
          int code;
  		  const byte *TZ_table = TZ4[vlcnum - 10];
  		  int read_len = TZ4_readlen[vlcnum - 10];
  		  code = SHOW_BITS_POS(6);
  		  code >>= read_len;
  		  totzeros = TZ_table[code]>>4;
  		  FLUSH_BITS_POS(TZ_table[code]&0x0f);
  		  UPDATE_CACHE_POS( pDecGlobal );
		}
      }

      //totzeros = currSE.value1;
    }
    else
    {
      totzeros = 0;
    }

    // decode run before each coefficient
    //zerosleft = totzeros;
    coeffindex = totzeros + numcoeff - 1;
    j = scan[coeffindex];
    if(block_type== CHROMA_DC||block_type==LUMA_INTRA16x16DC)
    {
      int code,len=0;
	  const byte *Run_table;
      cof[j] = levarr[0];
      for(i=1;i<numcoeff;i++) 
	  {
	    VO_S32 tmp_run;
	    if(totzeros<=0)
	    {
	      tmp_run = 0;
	    }
	    else
        {
        vlcnum = imin(totzeros - 1, RUNBEFORE_NUM_M1);
		
		if(vlcnum > 5)
		{          
  		  code = SHOW_BITS_POS(6);
    	  code >>= 3;
  		  UPDATE_CACHE_POS( pDecGlobal );
  		  if(code > 0)
  		  {
  			tmp_run= 7-code;
			FLUSH_BITS_POS(3);
  		  }
  		  else
  		  {   		
			FLUSH_BITS_POS(3);
			UPDATE_CACHE_POS( pDecGlobal );
			code = SHOW_BITS_POS(8);
			while(len < 7 && code!=1)
			{
			  code >>= 1;
			  len++;
			}
			tmp_run = 14 - len;
			FLUSH_BITS_POS(8-len);
  		  } 
  		  UPDATE_CACHE_POS( pDecGlobal );
		}
		else
		{
  		  Run_table = Run[vlcnum];
  		  len = Run_readlen[vlcnum];

  		  code = SHOW_BITS_POS(6);
  		  code >>= len;
  		  tmp_run= Run_table[code]>>4;
  		  FLUSH_BITS_POS(Run_table[code]&0x0f);
  		  UPDATE_CACHE_POS( pDecGlobal );
		}
	    }
        totzeros -= tmp_run;
		coeffindex -= 1 + tmp_run;
        j= scan[ coeffindex ];
        cof[j]= levarr[i];
      }
    }
	else
	{
	  int code,len=0;
	  const byte *Run_table;
      cof[j] = rshift_rnd_sf((levarr[0] * inv[j])<<qp, shift_qp);
      for(i=1;i<numcoeff;i++) 
	  {
	    VO_S32 tmp_run;
		if(totzeros<=0)
	    {
	      tmp_run = 0;
	    }
	    else
	    {
		    vlcnum = imin(totzeros - 1, RUNBEFORE_NUM_M1);
			if(vlcnum > 5)
			{          
  			  code = SHOW_BITS_POS(6);
	  	  	  code >>= 3;
	  		  UPDATE_CACHE_POS( pDecGlobal );
	  		  if(code > 0)
	  		  {
	  			tmp_run= 7-code;
				FLUSH_BITS_POS(3);
	  		  }
	  		  else
	  		  {   		
				FLUSH_BITS_POS(3);
				UPDATE_CACHE_POS( pDecGlobal );
				code = SHOW_BITS_POS(8);
				while(len < 7 && code!=1)
				{
				  code >>= 1;
				  len++;
				}
				tmp_run = 14 - len;
				FLUSH_BITS_POS(8-len);
	  		  } 
	  		  UPDATE_CACHE_POS( pDecGlobal );
			}
			else
			{
	  		  Run_table = Run[vlcnum];
	  		  len = Run_readlen[vlcnum];
	
	  		  code = SHOW_BITS_POS(6);
	  		  code >>= len;
	  		  tmp_run= Run_table[code]>>4;
	  		  FLUSH_BITS_POS(Run_table[code]&0x0f);
	  		  UPDATE_CACHE_POS( pDecGlobal );
			}
	    }
        totzeros -= tmp_run;
		coeffindex -= 1 + tmp_run;
        j= scan[ coeffindex ];
        cof[j]= rshift_rnd_sf((levarr[i] * inv[j])<<qp, shift_qp);
      }
	}
    
	if(totzeros < 0)
	{
	  END_BITS_POS( pDecGlobal )
	  VOH264ERROR(VO_H264_ERR_TOTALZEROS_CAVLC);
	}

    //runarr[i] = totzeros;
	//for(k = 0;k < i;k++)
  	//  runarr[k] = 0;
  } // if numcoeff
  
  END_BITS_POS( pDecGlobal )
  return 0;
}

static void readCompCoeff4x4MB_CABAC (H264DEC_G *pDecGlobal, H264DEC_L *pDecLocal, int *inv, int qp_per, int cbp,VO_S32 mb_type0)
{
	static const int ypos_idx[4] = {0,32,128,160};
  Slice *currSlice = pDecLocal->p_Slice;
  DataPartition *dP = &currSlice->partArr[0];
  int n;
  int max = 16; 
  int scan_idx = 0;
  int type = 2;
  const VO_U8* scan = ((currSlice->structure == FRAME) && (!pDecLocal->mb_field)) ? SNGL_SCAN_4X4 : FIELD_SCAN_4X4;
  VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//  {
//	  cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//  }

  if(IS_INTRA16x16(mb_type0))
  {
    max = 15;
	scan_idx = 1;
	type = 1;
  }

  for(n = 0; n < 4;n++)
  {
    if (cbp & (1 << n))
    {
      int ypos = ypos_idx[n];
      readRunLevel_CABAC(pDecLocal, type, &dP->c,cof+ypos,0,n<<2,max,&scan[scan_idx],inv,qp_per);
	  readRunLevel_CABAC(pDecLocal, type, &dP->c,cof+ypos+16,0,(n<<2)+1,max,&scan[scan_idx],inv,qp_per);
	  readRunLevel_CABAC(pDecLocal, type, &dP->c,cof+ypos+64,0,(n<<2)+2,max,&scan[scan_idx],inv,qp_per);
	  readRunLevel_CABAC(pDecLocal, type, &dP->c,cof+ypos+80,0,(n<<2)+3,max,&scan[scan_idx],inv,qp_per);
    }
  }
}

static void readCompCoeff8x8MB_CABAC (H264DEC_G *pDecGlobal, H264DEC_L *pDecLocal, int *inv, int qp_per, int cbp)
{
	static const int ypos_idx[4] = {0,64,128,192};
  Slice *currSlice = pDecLocal->p_Slice;
  DataPartition *dP = &currSlice->partArr[0];
  int n;
  const VO_U8* scan = ((currSlice->structure == FRAME) && (!pDecLocal->mb_field)) ? SNGL_SCAN_8X8_CABAC : FIELD_SCAN_8X8_CABAC;
  VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//  {
//	  cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//  }

  for(n = 0; n < 4;n++)
  {
    if (cbp & (1 << n))
    {
      readRunLevel_CABAC_8x8(pDecLocal, &dP->c,cof+ypos_idx[n],n<<2,scan,inv,qp_per);
    }
  }
}


static VO_S32 readCompCoeff4x4MB_CAVLC (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int (*InvLevelScale4x4)[4], int qp_per, int cbp,VO_S32 mb_type0)
{
	static const VO_U8 ypos_table[16] = {0,16,64,80,32,48,96,112,128,144,192,208,160,176,224,240};
  int ret;
  int numcoeff;
  int cur_context; 
  int *inv = InvLevelScale4x4[0];
  int max_coeff_num;
  int n = 0;
  const VO_U8* xpos = ((pDecLocal->p_Slice->structure == FRAME) && (!pDecLocal->mb_field)) ? SNGL_SCAN_4X4 : FIELD_SCAN_4X4;
  VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//  {
//	  cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//  }

  if (IS_INTRA16x16(mb_type0))
  {
      cur_context = LUMA_INTRA16x16AC;
	  max_coeff_num = 15;
	  xpos += 1;
  }
  else
  {
      cur_context = LUMA;
	  max_coeff_num = 16;
  }
    
    
  for(n = 0;n < 16;n++)
  {
    if(cbp & (1 << (n>>2)))
    {
	  short* cof_in = cof+ypos_table[n];
	  const VO_U8 *xpos_in = xpos;
	  //memset(runarr,0,16*4);
      ret = readCoeff4x4_CAVLC(pDecGlobal,pDecLocal, cur_context,qp_per,4, n, xpos_in, inv,cof_in, &numcoeff,max_coeff_num);
      if(ret)
	  	return ret;
      /*for (k = 0; k < numcoeff; ++k)
      {
	    xpos_in += runarr[k];
        *cur_cbp |= i64_power2(j + (i >> 2));
        cof_in[*xpos_in]= (short)rshift_rnd_sf((levarr[k] * inv[*xpos_in])<<qp_per, 4);
		xpos_in++;
      }*/
    }
  }
  return 0;
}

static VO_S32 readCompCoeff8x8MB_CAVLC (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int (*InvLevelScale8x8)[8], int qp_per, int cbp,VO_S32 mb_type0)
{
	static const VO_U8 ypos_table_8x8[4] = {0,64,128,192};
  int ret;
  int numcoeff;
  int cur_context; 
  int *inv = InvLevelScale8x8[0];
  int max_coeff_num;
  int n = 0,n1;
  const VO_U8* xpos = ((pDecLocal->p_Slice->structure == FRAME) && (!pDecLocal->mb_field)) ? SNGL_SCAN_8X8 : FIELD_SCAN_8X8;
  VO_U8 *nz_coeff = pDecLocal->nz_coeff;
  VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//  {
//	  cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//  }
  if (IS_INTRA16x16(mb_type0))
  {
      cur_context = LUMA_INTRA16x16AC;
	  max_coeff_num = 15;
	  xpos += 1;
  }
  else
  {
      cur_context = LUMA;
	  max_coeff_num = 16;
  }
  
    
  for(n1 = 0;n1 < 4;n1++)
  {
    for(n = (n1<<2);n < (n1<<2)+4;n++)
    {
    if(cbp & (1 << (n>>2)))
    {
	  short* cof_in = cof+ypos_table_8x8[n1];
	  const VO_U8 *xpos_in = xpos+(n-(n1<<2))*16;
      ret = readCoeff4x4_CAVLC(pDecGlobal,pDecLocal, cur_context,qp_per,6, n, xpos_in, inv,cof_in, &numcoeff,max_coeff_num);
      if(ret)
	  	return ret;
      }
    }

  }
  return 0;
}


static VO_S32 read_CBP_and_coeffs_from_NAL_CABAC_420(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 dct8x8)
{
  int cbp,ret;
  Slice *currSlice = pDecLocal->p_Slice;
  DataPartition *dP = &currSlice->partArr[0];

  int qp_per, qp_rem,i;
  int qp_per_uv[2];
  int qp_rem_uv[2];
  int mb_type0 = pDecLocal->mb_type0;
  int intra = IS_INTRA0(mb_type0);  

  int (*InvLevelScale4x4)[4] = NULL;
  int (*InvLevelScale8x8)[8] = NULL;

  if (!IS_INTRA16x16 (mb_type0))
  {
    int need_transform_size_flag;
    //=====   C B P   =====
    //---------------------
	
    pDecLocal->cbp = cbp = read_CBP_CABAC(pDecLocal, &dP->c);


    //============= Transform size flag for INTER MBs =============
    //-------------------------------------------------------------
    need_transform_size_flag = dct8x8
							   &&currSlice->Transform8x8Mode && (cbp&15) && !IS_INTRA0(mb_type0);
    

    if (need_transform_size_flag)
    {
	  pDecLocal->mb_type0 = mb_type0 = mb_type0|(readMB_transform_size_flag_CABAC(pDecLocal,&dP->c)*VO_8x8DCT);
    }

    //=====   DQUANT   =====
    //----------------------
    // Delta quant only if nonzero coeffs
    if (cbp !=0)
    {
      ret = read_delta_quant(pDecGlobal,pDecLocal, dP);
	  if(ret)
	  	return ret;
    }
	update_qp(pDecGlobal,pDecLocal, currSlice->qp);
  }
  else // read DC coeffs for new intra modes
  {
    const VO_U8* scan = ((currSlice->structure == FRAME) && (!pDecLocal->mb_field)) ? SNGL_SCAN_COF : FIELD_SCAN_COF;
	VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//	if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//	if (pDecGlobal->nThdNum > 1)
//#endif
//	{
//		cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//	}
	cbp = pDecLocal->cbp;
  
    ret = read_delta_quant(pDecGlobal,pDecLocal, dP);
	if(ret)
	  return ret;
	update_qp(pDecGlobal,pDecLocal, currSlice->qp);
	readRunLevel_CABAC(pDecLocal, 0, &dP->c,cof,1,0,16,scan,NULL,0);                         // just to get inside the loop
	  
    itrans_2(pDecGlobal,pDecLocal);// transform new intra DC
  }

  //update_qp(pDecGlobal,pDecLocal, currSlice->qp);

  qp_per = pDecGlobal->qp_per_matrix[ pDecLocal->qp_scaled[0] ];
  qp_rem = pDecGlobal->qp_rem_matrix[ pDecLocal->qp_scaled[0] ];

  // luma coefficients
  //======= Other Modes & CABAC ========
  //------------------------------------          
  if (cbp)
  {
    if(IS_8x8DCT(mb_type0)) 
    {
      //======= 8x8 transform size & CABAC ========
      InvLevelScale8x8 = intra? pDecGlobal->InvLevelScale8x8_Intra[0][qp_rem] : pDecGlobal->InvLevelScale8x8_Inter[0][qp_rem];
      readCompCoeff8x8MB_CABAC (pDecGlobal, pDecLocal, InvLevelScale8x8[0], qp_per, cbp);
    }
    else
    {
      InvLevelScale4x4 = intra? pDecGlobal->InvLevelScale4x4_Intra[0][qp_rem] : pDecGlobal->InvLevelScale4x4_Inter[0][qp_rem];
      readCompCoeff4x4MB_CABAC (pDecGlobal, pDecLocal, InvLevelScale4x4[0], qp_per, cbp,mb_type0);        
    }
  }

  for(i=0; i < 2; ++i)
  {
    qp_per_uv[i] = pDecGlobal->qp_per_matrix[ pDecLocal->qp_scaled[i + 1] ];
    qp_rem_uv[i] = pDecGlobal->qp_rem_matrix[ pDecLocal->qp_scaled[i + 1] ];
  }
  if(cbp>15)
  {
    int uv;
	VO_U8 cdc_table[4] = {0,1,2,3};
	VO_S16 *cof = pDecLocal->cof_yuv + MB_BLOCK_SIZE*MB_BLOCK_SIZE;
//#if USE_FRAME_THREAD
//	if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//	if (pDecGlobal->nThdNum > 1)
//#endif
//	{
//		cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//	}

    for (uv=0;uv<2;uv++)
    {
      short tempDC[4] = {0};

      InvLevelScale4x4 = intra ? pDecGlobal->InvLevelScale4x4_Intra[uv + 1][qp_rem_uv[uv]] : pDecGlobal->InvLevelScale4x4_Inter[uv + 1][qp_rem_uv[uv]];
          
	  readRunLevel_CABAC(pDecLocal, 3, &dP->c,tempDC,1,uv,4,cdc_table,NULL,qp_per);

	  ihadamard2x2(tempDC, cof,InvLevelScale4x4[0][0],qp_per_uv[uv]);
	  cof += 64;         
    }      
  }

  if (cbp >31)
  {
    //currSE.reading = readRunLevel_CABAC;
    const VO_U8* scan = ((currSlice->structure == FRAME) && (!pDecLocal->mb_field)) ? SNGL_SCAN_4X4 : FIELD_SCAN_4X4;
    //if(currMB->is_lossless == FALSE)
    {
      int b4,uv;
      for (uv=0; uv < 2; ++uv)
      {
		int *inv;
		VO_S16 *cof = pDecLocal->cof_yuv + uv*MB_BLOCK_SIZE*MB_BLOCK_SIZE/4 + MB_BLOCK_SIZE*MB_BLOCK_SIZE;
//#if USE_FRAME_THREAD
//		if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//		if (pDecGlobal->nThdNum > 1)
//#endif
//		{
//			cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//		}
		inv = intra ? pDecGlobal->InvLevelScale4x4_Intra[uv + 1][qp_rem_uv[uv]][0] : pDecGlobal->InvLevelScale4x4_Inter[uv + 1][qp_rem_uv[uv]][0];
        for (b4 = 0; b4 < 4; ++b4)
        {
			readRunLevel_CABAC(pDecLocal, 4, &dP->c,cof,0,16+uv*4+b4,15,&scan[1],inv,qp_per_uv[uv]);
			cof += 16;
        }
      }
    }
  }
  if (pDecLocal->cbp != 0)
    currSlice->is_reset_coeff = FALSE;
  return 0;
}

static const byte NCBP[48][2]=
{
    {47, 0},{31,16},{15, 1},{ 0, 2},{23, 4},{27, 8},{29,32},{30, 3},{ 7, 5},{11,10},{13,12},{14,15},
    {39,47},{43, 7},{45,11},{46,13},{16,14},{ 3, 6},{ 5, 9},{10,31},{12,35},{19,37},{21,42},{26,44},
    {28,33},{35,34},{37,36},{42,40},{44,39},{ 1,43},{ 2,45},{ 4,46},{ 8,17},{17,18},{18,20},{20,24},
    {24,19},{ 6,21},{ 9,26},{22,28},{25,23},{32,27},{33,29},{34,30},{36,22},{40,25},{38,38},{41,41}
};

static VO_S32 read_CBP_and_coeffs_from_NAL_CAVLC_420(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 dct8x8)
{
  int i,ret;
  //int mb_nr = pDecLocal->mbAddrX;
  int cbp;
  Slice *currSlice = pDecLocal->p_Slice;    
  int numcoeff;

  int qp_per, qp_rem;
  int mb_type0 = pDecLocal->mb_type0;
  int uv; 
  int qp_per_uv[2];
  int qp_rem_uv[2];

  int intra = IS_INTRA0(mb_type0);
  int b4;

  int need_transform_size_flag;

  int (*InvLevelScale4x4)[4] = NULL;
  int (*InvLevelScale8x8)[8] = NULL;
  // select scan type
  const byte (*pos_scan4x4)[2] = SNGL_SCAN;//((pDecGlobal->structure == FRAME) && (!pDecLocal->mb_field)) ? SNGL_SCAN : FIELD_SCAN;
  const byte *pos_scan_4x4 = pos_scan4x4[0];
	VO_S16 *cof_y = pDecLocal->cof_yuv;
	VO_S16 *cof_uv[2];
//#if USE_FRAME_THREAD
//	if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//	if (pDecGlobal->nThdNum > 1)
//#endif
//	{
//		cof_y += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//	}
	 cof_uv[0] = cof_y+MB_BLOCK_SIZE*MB_BLOCK_SIZE;
	 cof_uv[1] =cof_y+MB_BLOCK_SIZE*MB_BLOCK_SIZE*5/4;
  // read CBP if not new intra mode
  if (!IS_INTRA16x16 (mb_type0))
  {
    //=====   C B P   =====
    //---------------------

    int idx = !IS_INTRA4x4 (mb_type0);
	int len = 0;
	VO_U32 value;
    value = SHOW_BITS(pDecGlobal,32);
    if(value >=(1<<27))
    {
      value >>= 32-9;
  	  FLUSH_BITS(pDecGlobal,VO_VLC_LEN[value]);
  	  UPDATE_CACHE(pDecGlobal);
  	  value = VO_UE_VAL[value];
    }
    else
    {
      len = 2*vo_big_len(value)-31;
  	  value >>= len;
  	  value--;
  	  FLUSH_BITS(pDecGlobal,32-len);
  	  UPDATE_CACHE(pDecGlobal);
    }
	if(value > 47)
	  VOH264ERROR(VO_H264_ERR_CBP_TOO_LARGE);
    cbp=NCBP[value][idx];
    pDecLocal->cbp = cbp ;


    need_transform_size_flag = dct8x8&&currSlice->Transform8x8Mode && (cbp&15) && !IS_INTRA0(mb_type0);

    if (need_transform_size_flag)
    {
	  pDecLocal->mb_type0 = mb_type0 = mb_type0|(GetBits(pDecGlobal,1)*VO_8x8DCT);
    }


    if (cbp !=0)
    {
      VO_S32 delta_quant;
      //read_delta_quant(pDecGlobal,pDecLocal,&currSE, dP, partMap, ((pDecLocal->is_intra_block == FALSE)) ? SE_DELTA_QUANT_INTER : SE_DELTA_QUANT_INTRA);
      delta_quant = se_v(pDecGlobal);
	  if ((delta_quant < -26) || (delta_quant > 25))
        VOH264ERROR(VO_H264_ERR_QP_TOO_LARGE);
      currSlice->qp = ((currSlice->qp + delta_quant + 52)%52);
    }
	update_qp(pDecGlobal,pDecLocal, currSlice->qp);
  }
  else
  {
    VO_S32 delta_quant;
    cbp = pDecLocal->cbp;  
    //read_delta_quant(pDecGlobal,pDecLocal,&currSE, dP, partMap, SE_DELTA_QUANT_INTRA);
	delta_quant = se_v(pDecGlobal);
	if ((delta_quant < -26) || (delta_quant > 25))
      VOH264ERROR(VO_H264_ERR_QP_TOO_LARGE);
    currSlice->qp = ((currSlice->qp + delta_quant + 52)%52);
    update_qp(pDecGlobal,pDecLocal, currSlice->qp);
	
    pos_scan_4x4 = ((currSlice->structure == FRAME) && (!pDecLocal->mb_field)) ? SNGL_SCAN_COF : FIELD_SCAN_COF;
    ret = readCoeff4x4_CAVLC(pDecGlobal,pDecLocal, LUMA_INTRA16x16DC,0,0, 0, pos_scan_4x4, NULL,cof_y, &numcoeff,16);
    if(ret)
	  return ret;

	if(numcoeff)
    itrans_2(pDecGlobal,pDecLocal);// transform new intra DC
  }

  //update_qp(pDecGlobal,pDecLocal, currSlice->qp);

  qp_per = pDecGlobal->qp_per_matrix[ pDecLocal->qp_scaled[0] ];
  qp_rem = pDecGlobal->qp_rem_matrix[ pDecLocal->qp_scaled[0] ];


    for(i=0; i < 2; ++i)
    {
      qp_per_uv[i] = pDecGlobal->qp_per_matrix[ pDecLocal->qp_scaled[i + 1] ];
      qp_rem_uv[i] = pDecGlobal->qp_rem_matrix[ pDecLocal->qp_scaled[i + 1] ];
    }


  if (cbp)
  {
    if (!IS_8x8DCT(mb_type0)) // 4x4 transform
    {
      InvLevelScale4x4 = intra? pDecGlobal->InvLevelScale4x4_Intra[0][qp_rem] : pDecGlobal->InvLevelScale4x4_Inter[0][qp_rem];
      ret = readCompCoeff4x4MB_CAVLC (pDecGlobal,pDecLocal, InvLevelScale4x4, qp_per, cbp,mb_type0);
	  if(ret)
	  	return ret;
    }
    else // 8x8 transform
    {
      
	  InvLevelScale8x8 = intra? pDecGlobal->InvLevelScale8x8_Intra[0][qp_rem] : pDecGlobal->InvLevelScale8x8_Inter[0][qp_rem];
      ret = readCompCoeff8x8MB_CAVLC (pDecGlobal,pDecLocal, InvLevelScale8x8, qp_per, cbp,mb_type0);
	  if(ret)
	  	return ret;
    }
  }
  

  
    if(cbp>>4)
    {
      VO_U8 cdc_table[4] = {0,1,2,3};
      //if (dec_picture->chroma_format_idc == YUV420)
      {
        for (uv=0;uv<2;uv++)
        {
		  short tempDC[4] = {0};
          InvLevelScale4x4 = intra ? pDecGlobal->InvLevelScale4x4_Intra[uv + 1][qp_rem_uv[uv]] : pDecGlobal->InvLevelScale4x4_Inter[uv + 1][qp_rem_uv[uv]];
          ret = readCoeff4x4_CAVLC(pDecGlobal,pDecLocal, CHROMA_DC,0,0, 0, cdc_table, NULL,tempDC,&numcoeff,4);
		  if(ret)
		  	return ret;
		  ihadamard2x2(tempDC, cof_uv[uv],InvLevelScale4x4[0][0],qp_per_uv[uv]);      
        }
      }
    }

    
    if (cbp>>5)
    {
      //if(currMB->is_lossless == FALSE)
      //{
        const VO_U8* scan = ((currSlice->structure == FRAME) && (!pDecLocal->mb_field)) ? SNGL_SCAN_4X4: FIELD_SCAN_4X4;
        for (uv=0; uv < 2; ++uv)
        {
          int ypos = 0;
		  int *inv;
		  inv = intra ? pDecGlobal->InvLevelScale4x4_Intra[uv + 1][qp_rem_uv[uv]][0] : pDecGlobal->InvLevelScale4x4_Inter[uv + 1][qp_rem_uv[uv]][0];
          for (b4=0; b4 < 4; ++b4)
          {
            ret = readCoeff4x4_CAVLC(pDecGlobal,pDecLocal, CHROMA_AC,qp_per_uv[uv],4, 16+uv*4+b4, scan+1, inv,cof_uv[uv]+ypos,&numcoeff,15);
			if(ret)
			  return ret;

			ypos += 16;
          }
        }        
    } //if (dec_picture->chroma_format_idc != YUV400)

  if (pDecLocal->cbp != 0)
    currSlice->is_reset_coeff = FALSE;
  return 0;
}

#define SWAP32(a,b) {VO_U32 tmp = a; a = b; b = tmp;}
#define SWAP64(a,b) {VO_U64 tmp = a; a = b; b = tmp;}
// #if USE_MULTI_THREAD
static inline void change_intra_border(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture)
{
	VO_U32 filterLeftMbEdgeFlag;
	VO_U8 *up_intra = pDecGlobal->intra_pix_buffer+(pDecLocal->mb_xy-(pDecGlobal->PicWidthInMbs<<pDecGlobal->pCurSlice->field_pic_flag))*32;
	VO_U8 *upleft_intra = up_intra - 32;
	VO_S32 iLumaStride = pDecGlobal->iLumaStride<<pDecGlobal->pCurSlice->field_pic_flag;
	VO_S32 iChromaStride = pDecGlobal->iChromaStride<<pDecGlobal->pCurSlice->field_pic_flag;
	VO_U8 *src_y = dec_picture->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride-iLumaStride+pDecLocal->pix_x;
	VO_U8 *src_u = dec_picture->imgUV[0]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride-iChromaStride+pDecLocal->pix_c_x;
	VO_U8 *src_v = dec_picture->imgUV[1]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride-iChromaStride+pDecLocal->pix_c_x;
    VO_S32 start_y = dec_picture->structure==BOTTOM_FIELD?1:0;
	filterLeftMbEdgeFlag = (pDecLocal->mb.x != 0);
	if(pDecLocal->mb.y > start_y)
	{
		if(filterLeftMbEdgeFlag)
		{
			SWAP32(M32(upleft_intra+12), M32(src_y-4));			  
			SWAP32(M32(upleft_intra+20), M32(src_u-4));
			SWAP32(M32(upleft_intra+28), M32(src_v-4));
			
		}
		SWAP64(M64(up_intra), M64(src_y));
		SWAP64(M64(up_intra+8), M64(src_y+8));
		SWAP64(M64(up_intra+16), M64(src_u));
		SWAP64(M64(up_intra+24), M64(src_v));
		if(pDecLocal->mb.x+1 < pDecGlobal->PicWidthInMbs)
			SWAP64(M64(up_intra+32), M64(src_y+16));
	}
#if ENABLE_DEBLOCK_MB
#if USE_FRAME_THREAD
	//if (!pDecGlobal->interlace||pDecGlobal->nThdNum <= 1)
#else
	if (pDecGlobal->nThdNum <= 1)
#endif
	{
		return;
	}
#endif

	if (filterLeftMbEdgeFlag)
	{
		VO_U32 i;
		VO_U32 tmp;
		VO_U8 *left = pDecLocal->left_intra;
		src_y += iLumaStride -1;
		src_u += iChromaStride -1;
		src_v += iChromaStride -1;

		i = 3;
		do 
		{
			tmp = *left;
			*left++ = *src_y;
			*src_y = (VO_U8)tmp;
			src_y += iLumaStride;
			tmp = *left;
			*left++ = *src_y;
			*src_y = (VO_U8)tmp;
			src_y += iLumaStride;
			tmp = *left;
			*left++ = *src_y;
			*src_y = (VO_U8)tmp;
			src_y += iLumaStride;
			tmp = *left;
			*left++ = *src_y;
			*src_y = (VO_U8)tmp;
			src_y += iLumaStride;
		} while (--i);
		tmp = *left;
		*left++ = *src_y;
		*src_y = (VO_U8)tmp;
		src_y += iLumaStride;
		tmp = *left;
		*left++ = *src_y;
		*src_y = (VO_U8)tmp;
		src_y += iLumaStride;
		tmp = *left;
		*left++ = *src_y;
		*src_y = (VO_U8)tmp;
		src_y += iLumaStride;
		tmp = *left;
		*left++ = *src_y;
		*src_y = (VO_U8)tmp;

		i = 3;
		do 
		{
			tmp = *left;
			*left++ = *src_u;
			*src_u = (VO_U8)tmp;
			src_u += iChromaStride;
			tmp = *left;
			*left++ = *src_u;
			*src_u = (VO_U8)tmp;
			src_u += iChromaStride;
		} while (--i);
		tmp = *left;
		*left++ = *src_u;
		*src_u = (VO_U8)tmp;
		src_u += iChromaStride;
		tmp = *left;
		*left++ = *src_u;
		*src_u = (VO_U8)tmp;

		i = 3;
		do 
		{
			tmp = *left;
			*left++ = *src_v;
			*src_v = (VO_U8)tmp;
			src_v += iChromaStride;
			tmp = *left;
			*left++ = *src_v;
			*src_v = (VO_U8)tmp;
			src_v += iChromaStride;
		} while (--i);
		tmp = *left;
		*left++ = *src_v;
		*src_v = (VO_U8)tmp;
		src_v += iChromaStride;
		tmp = *left;
		*left++ = *src_v;
		*src_v = (VO_U8)tmp;
	}

	return;
}
static inline void change_intra_border_mbaff(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture)
{
	VO_U32 filterLeftMbEdgeFlag;
	VO_U8 *up_intra = pDecGlobal->intra_pix_buffer+((pDecLocal->mbAddrX>>1)-pDecGlobal->PicWidthInMbs)*64;
	VO_U8 *upleft_intra = up_intra - 64;
	VO_U8 *src_y0 = dec_picture->imgY+((pDecLocal->pix_y&~31)-2)*pDecGlobal->iLumaStride+pDecLocal->pix_x;
	VO_U8 *src_y1 = src_y0 + pDecGlobal->iLumaStride;
	VO_U8 *src_u0 = dec_picture->imgUV[0]+((pDecLocal->pix_c_y&~15)-2)*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	VO_U8 *src_u1 = src_u0 + pDecGlobal->iChromaStride;
	VO_U8 *src_v0 = dec_picture->imgUV[1]+((pDecLocal->pix_c_y&~15)-2)*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	VO_U8 *src_v1 = src_v0 + pDecGlobal->iChromaStride;

	filterLeftMbEdgeFlag = (pDecLocal->mb.x != 0);

	if(pDecLocal->mb.y > 1)
	{
		if(filterLeftMbEdgeFlag)
		{
			
			SWAP32(M32(upleft_intra+12), M32(src_y0-4));
			SWAP32(M32(upleft_intra+28), M32(src_y1-4));
			SWAP32(M32(upleft_intra+36), M32(src_u0-4));
			SWAP32(M32(upleft_intra+44), M32(src_u1-4));
			SWAP32(M32(upleft_intra+52), M32(src_v0-4));
			SWAP32(M32(upleft_intra+60), M32(src_v1-4));
		}
		SWAP64(M64(up_intra), M64(src_y0));
		SWAP64(M64(up_intra+8), M64(src_y0+8));
		SWAP64(M64(up_intra+16), M64(src_y1));
		SWAP64(M64(up_intra+24), M64(src_y1+8));
		SWAP64(M64(up_intra+32), M64(src_u0));
		SWAP64(M64(up_intra+40), M64(src_u1));
		SWAP64(M64(up_intra+48), M64(src_v0));
		SWAP64(M64(up_intra+56), M64(src_v1));
		if(pDecLocal->mb.x+1 < pDecGlobal->PicWidthInMbs)
		{
			SWAP64(M64(up_intra+64), M64(src_y0+16));
			SWAP64(M64(up_intra+80), M64(src_y1+16));
		}

	}
#if ENABLE_DEBLOCK_MB
#if USE_FRAME_THREAD
	//if (!pDecGlobal->interlace||pDecGlobal->nThdNum <= 1)
#else
	if (pDecGlobal->nThdNum <= 1)
#endif
	{
		return;
	}
#endif

	if (filterLeftMbEdgeFlag)
	{
		VO_U32 i;
		VO_U32 tmp;
		VO_U8 *left = pDecLocal->left_intra;
		src_y0 += 2*pDecGlobal->iLumaStride -1;
		src_u0 += 2*pDecGlobal->iChromaStride -1;
		src_v0 += 2*pDecGlobal->iChromaStride -1;

		i = 7;
		do 
		{
			tmp = *left;
			*left++ = *src_y0;
			*src_y0 = (VO_U8)tmp;
			src_y0 += pDecGlobal->iLumaStride;
			tmp = *left;
			*left++ = *src_y0;
			*src_y0 = (VO_U8)tmp;
			src_y0 += pDecGlobal->iLumaStride;
			tmp = *left;
			*left++ = *src_y0;
			*src_y0 = (VO_U8)tmp;
			src_y0 += pDecGlobal->iLumaStride;
			tmp = *left;
			*left++ = *src_y0;
			*src_y0 = (VO_U8)tmp;
			src_y0 += pDecGlobal->iLumaStride;
		} while (--i);
		tmp = *left;
		*left++ = *src_y0;
		*src_y0 = (VO_U8)tmp;
		src_y0 += pDecGlobal->iLumaStride;
		tmp = *left;
		*left++ = *src_y0;
		*src_y0 = (VO_U8)tmp;
		src_y0 += pDecGlobal->iLumaStride;
		tmp = *left;
		*left++ = *src_y0;
		*src_y0 = (VO_U8)tmp;
		src_y0 += pDecGlobal->iLumaStride;
		tmp = *left;
		*left++ = *src_y0;
		*src_y0 = (VO_U8)tmp;

		i = 7;
		do 
		{
			tmp = *left;
			*left++ = *src_u0;
			*src_u0 = (VO_U8)tmp;
			src_u0 += pDecGlobal->iChromaStride;
			tmp = *left;
			*left++ = *src_u0;
			*src_u0 = (VO_U8)tmp;
			src_u0 += pDecGlobal->iChromaStride;
		} while (--i);
		tmp = *left;
		*left++ = *src_u0;
		*src_u0 = (VO_U8)tmp;
		src_u0 += pDecGlobal->iChromaStride;
		tmp = *left;
		*left++ = *src_u0;
		*src_u0 = (VO_U8)tmp;

		i = 7;
		do 
		{
			tmp = *left;
			*left++ = *src_v0;
			*src_v0 = (VO_U8)tmp;
			src_v0 += pDecGlobal->iChromaStride;
			tmp = *left;
			*left++ = *src_v0;
			*src_v0 = (VO_U8)tmp;
			src_v0 += pDecGlobal->iChromaStride;
		} while (--i);
		tmp = *left;
		*left++ = *src_v0;
		*src_v0 = (VO_U8)tmp;
		src_v0 += pDecGlobal->iChromaStride;
		tmp = *left;
		*left++ = *src_v0;
		*src_v0 = (VO_U8)tmp;
	}

	return;
}

int decode_one_component_p_slice(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture)
{
  //For residual DPCM
  Slice *currSlice = pDecLocal->p_Slice;
  VO_S8 *b8pdir = pDecLocal->b8pdir;
  VO_S32 mb_type0 = pDecLocal->mb_type0;
  //pDecLocal->ipmode_DPCM = NO_INTRA_PMODE; 
  if(!IS_INTRA0(mb_type0)) 
  {
    //int smb = (currMB->p_Vid->type == SP_SLICE);
	//int smb = (currSlice->slice_type == SP_SLICE);
  	if (IS_16X16(mb_type0))
  	{
      //mb_pred_p_inter16x16(currMB, curr_plane, dec_picture);  
      perform_mc(pDecGlobal,pDecLocal, b8pdir[0], 0, 0,0);
    }
    else if (IS_16X8(mb_type0))
    {
      //mb_pred_p_inter16x8(currMB, curr_plane, dec_picture);
      perform_mc(pDecGlobal,pDecLocal, b8pdir[0], 0, 0,1);
  	  perform_mc(pDecGlobal,pDecLocal, b8pdir[2], 0, 2,1);
	}
    else if (IS_8X16(mb_type0))
    {
      //mb_pred_p_inter8x16(currMB, curr_plane, dec_picture);
      perform_mc(pDecGlobal,pDecLocal, b8pdir[0], 0, 0,2);
  	  perform_mc(pDecGlobal,pDecLocal, b8pdir[1], 2, 0,2);
    }
    else if (IS_SKIP(mb_type0)&&currSlice->slice_type != B_SLICE)
    {
      //mb_pred_sp_skip(currMB, curr_plane, dec_picture);
      perform_mc(pDecGlobal,pDecLocal, LIST_0, 0, 0,0);	
    }
	else 
	{
	  if(currSlice->slice_type == B_SLICE)//YU_TBD
		mb_pred_b_inter8x8(pDecGlobal,pDecLocal, dec_picture);
	  else
		mb_pred_p_inter8x8(pDecGlobal,pDecLocal, dec_picture);

	}
	
	if ((IS_SKIP(mb_type0)&&currSlice->slice_type != B_SLICE)||(pDecLocal->cbp == 0))
    {
	 
    }
    else
    {

	  if((pDecLocal->cbp & 15) != 0)
		iTransformLuma(pDecGlobal,pDecLocal);
	  
	  iTransformChroma(pDecGlobal,pDecLocal);
    }
  }
  else if(IS_PCM(mb_type0))
    mb_pred_ipcm(pDecGlobal,pDecLocal);
  else
  {
// #if (USE_MULTI_THREAD || ENABLE_DEBLOCK_MB)
#if !ENABLE_DEBLOCK_MB
	  if (pDecGlobal->nThdNum > 1)
#endif
	  {
		  if(pDecGlobal->iDeblockMode)
		  {
			  if (!currSlice->mb_aff_frame_flag)
			  {
				  change_intra_border(pDecGlobal,pDecLocal, dec_picture);
			  }
			  else
			  {
				  change_intra_border_mbaff(pDecGlobal,pDecLocal, dec_picture);
			  }
		  }
	  }
    pDecLocal->c_ipred_mode = get_intra_dc_mode(pDecLocal,pDecLocal->c_ipred_mode);
  	if (IS_INTRA16x16(mb_type0))
	{
		pDecLocal->i16mode = get_intra_dc_mode(pDecLocal,pDecLocal->i16mode);
      mb_pred_intra16x16(pDecGlobal,pDecLocal, dec_picture);
	}
    else 
	{
	  get_intra4x4_dc_mode(pDecGlobal, pDecLocal);
	  if (IS_8x8DCT(mb_type0))
        mb_pred_intra8x8(pDecGlobal,pDecLocal, dec_picture);
	  else
        mb_pred_intra4x4(pDecGlobal,pDecLocal, dec_picture);
	}
// #if (USE_MULTI_THREAD || ENABLE_DEBLOCK_MB)
#if !ENABLE_DEBLOCK_MB
	if (pDecGlobal->nThdNum > 1)
#endif
	{
		if(pDecGlobal->iDeblockMode)
		{
			if (!currSlice->mb_aff_frame_flag)
			{
				change_intra_border(pDecGlobal,pDecLocal, dec_picture);
			}
			else
			{
				change_intra_border_mbaff(pDecGlobal,pDecLocal, dec_picture);
			}
		}
	}
  }
  
  return 1;
}
//end of edit


int decode_one_macroblock(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture)
{
	decode_one_component_p_slice(pDecGlobal,pDecLocal, dec_picture);    
	return 0;
}


