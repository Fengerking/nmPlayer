/******************************************************************************************
*                                                                                         *
*  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
*                                                                                         *
*******************************************************************************************/
#include "h265dec_parser.h"
#include "h265dec_cabac.h"
#include "h265dec_sbac.h"
#include "h265dec_mvpred.h"
#include "h265dec_recon.h"
#include "HMLog.h"
#include "h265dec_idct.h"
#include "h265dec_vlc.h"
#include "h265dec_list.h"
#include "h265dec_slice.h"
#include <stdlib.h>
#include <string.h>  // for ios memset


#ifdef ARCH_X86 

#if defined (__cplusplus)
extern "C"{
#endif

#define IDCT_FUNC_DEF(x)	\
	extern  VO_VOID x(const VO_S16 * p_invtransform_buff,\
	VO_U8 * p_prediction_buff,\
		const VO_U32 prediction_stride,\
		VO_S16 * block_array);

	IDCT_FUNC_DEF(IDCT8X8_4X4_X86_sse4)
	IDCT_FUNC_DEF(IDCT8X8_X86_sse4)
	IDCT_FUNC_DEF(IDCT32X32_4X4_X86_sse4)
	IDCT_FUNC_DEF(IDCT32X32_8X8_X86_sse4)
	IDCT_FUNC_DEF(IDCT32X32_16X16_X86_sse4)
	IDCT_FUNC_DEF(IDCT32X32_X86_sse4)
	IDCT_FUNC_DEF(IDCT16X16_4X4_X86_sse4)
	IDCT_FUNC_DEF(IDCT16X16_8x8_X86_sse4)
	IDCT_FUNC_DEF(IDCT16X16_X86_sse4)
	IDCT_FUNC_DEF(IDCT4X4_X86_sse4)
	IDCT_FUNC_DEF(IDCT_NZ_1X1_X86_sse4)
	IDCT_FUNC_DEF(IDST4X4_X86_sse4)
	IDCT_FUNC_DEF(IDST4X4_1X1_X86_sse4)
	IDCT_FUNC_DEF(IDCT_NONE_X86_sse2)
	
#if defined (__cplusplus)
}
#endif

#endif

/** \file     h265dec_slice.c
\brief    slice decoding
\change
*/

/**
*Input : pixels position x, y
trafosize
yuv_idx
*dataflow:
*Output
*/
typedef VO_VOID (*P_FUNCTION_POINT_FOR_IDCT) (const VO_S16 * p_src_data, 
	VO_U8 * p_prediction_data, 
	const VO_U32 prediction_stride,
	VO_S16 * tmp_block_array);


extern void vo_init_cabac_decoder(H265_DEC_CABAC *c, const VO_U8 *buf, int buf_size);

typedef enum
{
	PRED_NULL,// No use
	PRED_L0,
	PRED_L1,
	PRED_BI
}INTER_PRED_IDC;

#define VOSWAP( type, a, b ) { type SWAP_tmp = b; b = a; a = SWAP_tmp; }

/*#if IDCT_ASM_ENABLED
const	P_FUNCTION_POINT_FOR_IDCT IDCT_function_pointer_array[15] = { 
    IDCT_NZ_1X1_ASMV7, IDCT4X4ASMV7, IDCT8X8ASMV7, IDCT_NZ_1X1_ASMV7, 
    IDCT16X16ASMV7, IDCT_NZ_1X1_ASMV7, IDCT16X16_4X4_ASMV7,0,
    IDCT32X32ASMV7, IDCT_NZ_1X1_ASMV7,IDCT32X32_4X4_ASMV7,
    IDST4X4_1X1_ASMV7, IDST4X4ASMV7,IDCT_NONE_1X1,IDCT_NONE};

#else//IDCT4X4
const	P_FUNCTION_POINT_FOR_IDCT IDCT_function_pointer_array[15] = {
    IDCT_NZ_1X1, IDCT4X4, IDCT8X8, IDCT_NZ_1X1, 
    IDCT16X16, IDCT_NZ_1X1, IDCT16X16_4X4,0,
    IDCT32X32, IDCT_NZ_1X1,IDCT32X32_4X4,
    IDST4X4_1X1, IDST4X4,IDCT_NONE_1X1,IDCT_NONE};

#endif*/
#if IDCT_ASM_ENABLED
const	P_FUNCTION_POINT_FOR_IDCT IDCT_function_pointer_array[24] = 
{
    IDCT4X4ASMV7,IDCT_NZ_1X1_ASMV7,0,0,0,
	IDCT8X8ASMV7,IDCT_NZ_1X1_ASMV7,IDCT8X8_4X4_ASMV7,0,0,
	IDCT16X16ASMV7, IDCT_NZ_1X1_ASMV7, IDCT16X16_4X4_ASMV7,IDCT16X16_8X8_ASMV7,0,
	IDCT32X32ASMV7, IDCT_NZ_1X1_ASMV7,IDCT32X32_4X4_ASMV7,IDCT32X32_8X8_ASMV7,IDCT32X32_16X16_ASMV7,
	IDST4X4ASMV7,IDST4X4_1X1_ASMV7 ,IDCT_NONE_1X1,IDCT_NONE
};
#else
#if defined (ARCH_X86)  //will remove it when ARCH_X86 is done
const	P_FUNCTION_POINT_FOR_IDCT IDCT_function_pointer_array[24] = 
{
	IDCT4X4_X86_sse4,IDCT_NZ_1X1_X86_sse4,0,0,0,
	IDCT8X8_X86_sse4,IDCT_NZ_1X1_X86_sse4,IDCT8X8_4X4_X86_sse4,0,0,
	IDCT16X16_X86_sse4, IDCT_NZ_1X1_X86_sse4, IDCT16X16_4X4_X86_sse4,IDCT16X16_8x8_X86_sse4,0,
	IDCT32X32_X86_sse4, IDCT_NZ_1X1_X86_sse4,IDCT32X32_4X4_X86_sse4,IDCT32X32_8X8_X86_sse4,IDCT32X32_16X16_X86_sse4,
	IDST4X4_X86_sse4,IDST4X4_1X1_X86_sse4,IDCT_NONE_1X1,IDCT_NONE_X86_sse2
};
#else
const	P_FUNCTION_POINT_FOR_IDCT IDCT_function_pointer_array[24] = 
{
    IDCT4X4,IDCT_NZ_1X1,0,0,0,
	IDCT8X8,IDCT_NZ_1X1,IDCT8X8_4x4,0,0,
	IDCT16X16, IDCT_NZ_1X1, IDCT16X16_4X4,IDCT16X16_8x8,0,
	IDCT32X32, IDCT_NZ_1X1,IDCT32X32_4X4,IDCT32X32_8x8,IDCT32X32_16x16,
	IDST4X4,IDST4X4_1X1 ,IDCT_NONE_1X1,IDCT_NONE
};
#endif
#endif

static const VO_U8 intra_chroma_dir_table[ 4 ] = { INTRA_PLANAR, INTRA_VER, INTRA_HOR, INTRA_DC };

static const VO_U8 scan2x2_inv[ 2 ][ 2 ] = {
	{ 0, 2, },
	{ 1, 3, },
};

static const VO_U8 scan4x4_inv[ 3 ][ 16 ] = {
	{ 0,  2,  5,  9,  1,  4,  8, 12,  3,  7, 11, 14,  6, 10, 13, 15 },//diag
	{ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 },//hor
	{ 0,  4,  8, 12,  1,  5,  9, 13,  2,  6, 10, 14,  3,  7, 11, 15 }//ver
};

static const VO_U8 diag_scan8x8_inv[ 8 ][ 8 ] = {
	{  0,  2,  5,  9, 14, 20, 27, 35, },
	{  1,  4,  8, 13, 19, 26, 34, 42, },
	{  3,  7, 12, 18, 25, 33, 41, 48, },
	{  6, 11, 17, 24, 32, 40, 47, 53, },
	{ 10, 16, 23, 31, 39, 46, 52, 57, },
	{ 15, 22, 30, 38, 45, 51, 56, 60, },
	{ 21, 29, 37, 44, 50, 55, 59, 62, },
	{ 28, 36, 43, 49, 54, 58, 61, 63, },
};

static const VO_U8 diag_scan8x8_x[ 64 ] = {
	0, 0, 1, 0,
	1, 2, 0, 1,
	2, 3, 0, 1,
	2, 3, 4, 0,
	1, 2, 3, 4,
	5, 0, 1, 2,
	3, 4, 5, 6,
	0, 1, 2, 3,
	4, 5, 6, 7,
	1, 2, 3, 4,
	5, 6, 7, 2,
	3, 4, 5, 6,
	7, 3, 4, 5,
	6, 7, 4, 5,
	6, 7, 5, 6,
	7, 6, 7, 7,
};


static const VO_U8 diag_scan8x8_y[ 64 ] = {
	0, 1, 0, 2,
	1, 0, 3, 2,
	1, 0, 4, 3,
	2, 1, 0, 5,
	4, 3, 2, 1,
	0, 6, 5, 4,
	3, 2, 1, 0,
	7, 6, 5, 4,
	3, 2, 1, 0,
	7, 6, 5, 4,
	3, 2, 1, 7,
	6, 5, 4, 3,
	2, 7, 6, 5,
	4, 3, 7, 6,
	5, 4, 7, 6,
	5, 7, 6, 7,
};

static const VO_U8 horiz_scan2x2_x[ 4 ] = {
	0, 1, 0, 1,
};

static const VO_U8 horiz_scan2x2_y[ 4 ] = {
	0, 0, 1, 1
};

static const VO_U8 horiz_scan4x4_x[ 16 ] = {
	0, 1, 2, 3,
	0, 1, 2, 3,
	0, 1, 2, 3,
	0, 1, 2, 3,
};

static const VO_U8 horiz_scan4x4_y[ 16 ] = {
	0, 0, 0, 0,
	1, 1, 1, 1,
	2, 2, 2, 2,
	3, 3, 3, 3,
};


static const VO_U8 diag_scan2x2_x[ 4 ] = {
	0, 0, 1, 1,
};

static const VO_U8 diag_scan2x2_y[ 4 ] = {
	0, 1, 0, 1,
};


static const VO_U8 diag_scan4x4_x[ 16 ] = {
	0, 0, 1, 0,
	1, 2, 0, 1,
	2, 3, 1, 2,
	3, 2, 3, 3,
};

static const VO_U8 diag_scan4x4_y[ 16 ] = {
	0, 1, 0, 2,
	1, 0, 3, 2,
	1, 0, 3, 2,
	1, 3, 2, 3,
};

static const VO_U8 ctxIndMap[ 16 ] = {
	0, 1, 4, 5,
	2, 3, 4, 5,
	6, 6, 8, 8,
	7, 7, 8, 8
};
const VO_S16 g_levelScale[6] =
{
	40,45,51,57,64,72
};

const VO_S32 idct_function_index[8] =
{
	0,0,0,5,10,15,20,0
};

static VO_S32 residual_coding( H265_DEC_SLICE* p_slice,
    const VO_U32 x0, 
    const VO_U32 y0, 
    const VO_U32 log2TrafoSize, 
    const VO_U32 cIdx ) 
{	
    VO_S32 n, i;
    //H265_DEC_SPS *p_sps = p_slice->p_sps;
    H265_DEC_PPS *p_pps = p_slice->p_pps;
    VO_U32 transform_skip_flag = 0;
    VO_S32 last_sig_coeff_x, last_sig_coeff_y;
    VO_S32 lastScanPos, lastSubBlock;
    VO_U32 scanIdx = SCAN_DIAG;
    VO_S32 ctx_offset, shift, cmax = ( log2TrafoSize << 1 ) - 1; 
    //VO_U32 nBaseCoeffGroupCtx;
    VO_U8 *scan_x_cg, *scan_y_cg, *scan_x_off, *scan_y_off;
    VO_U8 coded_sub_block_flag[ 64 ];
    VO_U32 Greater1Ctx = 1; 
    VO_U32 qp = p_slice->entries[p_slice->currEntryNum].dequant_qp[cIdx];  // TODO: get from p_slice
    VO_U32  per_for_qp = qp / 6;
    VO_U32  rem_for_qp = qp % 6; 
    VO_S32 scale =  (g_levelScale[rem_for_qp] << (per_for_qp + 4)); 
    VO_S32 bdShift = 3 + log2TrafoSize;//bit_depth + log2_width - 3; //bdShift = BitDepthY/C + Log2(nTbS) - 5
    //VO_S32  transform_skip_shift = 7 - log2TrafoSize;	//== 5 //MAX_TR_DYNAMIC_RANGE - bit_depth - uiLog2TrSize;  

    VO_U8 non_zero_block_flag;
    VO_U32 pred_stride = p_slice->cur_pic->pic_stride[cIdx];
    VO_U8* p_pred_buf = p_slice->cur_pic->pic_buf[cIdx] + x0 + y0 * pred_stride;


    //VO_S16 *TransCoeffLevel = p_slice->TransCoeffLevel;
	VO_S16 *TransCoeffLevel = p_slice->p_coeff[cIdx];
    TUTask* tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
	p_slice->p_coeff[cIdx]+=1 << (log2TrafoSize + log2TrafoSize);
#if H265_LOG
    // only for print log file in debug.
    SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, TransCoeffLevel,0, sizeof(VO_S16) << (log2TrafoSize + log2TrafoSize)); //width*width
#endif
    /* derive scanIdx 7.4.9.11*/
    if ( ( p_slice->CuPredMode == MODE_INTRA ) && \
        ( ( log2TrafoSize == 2 ) || ( log2TrafoSize == 3 && ( cIdx == 0 ) ) ) ) {
            VO_U32 predModeIntra = ( cIdx == 0 ) ? p_slice->IntraPredMode_Y : p_slice->IntraPredModeC;
            if ( predModeIntra >= 6 && predModeIntra <= 14 ) {
                scanIdx  = SCAN_VER;
            } else if ( predModeIntra >= 22 && predModeIntra <= 30) {        
                scanIdx  = SCAN_HOR;
            }
    }

    /*transform_skip_flag*/
    if ( p_pps->transform_skip_enabled_flag && !p_slice->cu_transquant_bypass_flag && ( log2TrafoSize  ==  2 ) ) {
        transform_skip_flag = \
            voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_TRANSFORMSKIP_FLAG_CTX + ( cIdx > 0 ? 1 : 0 ) );
    }

    /*last_sig_coeff_x and last_sig_coeff_y*/
    if ( cIdx == 0 ) {
        ctx_offset = 3 * ( log2TrafoSize - 2 ) + ( ( log2TrafoSize - 1 ) >> 2 );
        shift = ( log2TrafoSize + 1 ) >> 2;
    } else {
        ctx_offset = NUM_CTX_LAST_FLAG_XY;
        shift = log2TrafoSize - 2 ; 
    }

    for ( i = 0; i < cmax; i++ ) {
        if ( !voCabacDecodeBin( p_slice->pCabac,/* &p_slice->bs,*/ OFF_LAST_X_CTX + ctx_offset + ( i >> shift ) ) )
            break;
    }
    last_sig_coeff_x = i;

    for ( i = 0; i < cmax; i++ ) {
        if ( !voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_LAST_Y_CTX + ctx_offset + ( i >> shift ) ) )
            break;
    }
    last_sig_coeff_y = i;
    //g_caller++;
    if (last_sig_coeff_x == 0 && last_sig_coeff_y == 0) //DC
    {
        VO_U32 coeff_abs_level_greater2 = 0;
        VO_U32 coeff_sign;
        VO_S32 baseLevel;
        VO_S16 TransCoeffLevel_value;
        VO_U32 coeff_abs_level_greater1 = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_ONE_FLAG_CTX + 0/*( ctx_set << 2 )*/ + 1 + ( cIdx == 0 ? 0 : 16 ) ); // coeff_abs_level_greater1_flag
        //g_caller_1x1 ++;
        if ( coeff_abs_level_greater1  /*&&  ( lastGreater1ScanPos == -1 ) */){	//coeff_abs_level_greater1_flag[ n ]           
            coeff_abs_level_greater2 = \
                ( voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_ABS_FLAG_CTX/* + ctx_set*/ + ( ( cIdx == 0 ) ? 0 : 4 ) ) ) /*<< lastGreater1ScanPos*/ ;
        }
        coeff_sign = voCabacDecodeBinsEP( p_slice->pCabac, /*&p_slice->bs,*/1 /*nb_sig_coeff */);
        baseLevel = 1 + coeff_abs_level_greater1 + coeff_abs_level_greater2 ;
        if ( baseLevel  ==  3 ) {	
            VO_U32 uiGoRiceParam = 0;
            baseLevel += xReadCoefRemainExGolomb( p_slice->pCabac, /*&p_slice->bs,*/ uiGoRiceParam );
        }

        TransCoeffLevel_value = coeff_sign ? - baseLevel : baseLevel;
        if (!p_slice->cu_transquant_bypass_flag) {
            if (!transform_skip_flag){
                TransCoeffLevel[ 0 ] = Clip3(-32768, 32767, (TransCoeffLevel_value /**p_m[y * uiWidth + x]*/ * scale
                    + (1 << (bdShift - 1))) >> bdShift);
            } else {
#if DEQUANT_LOG
                TransCoeffLevel[ 0 ] = Clip3(-32768, 32767, (TransCoeffLevel_value /**p_m[y * uiWidth + x]*/ * scale
                    + (1 << (bdShift - 1))) >> bdShift);
#endif
                //p_pred_buf[0] = Clip(p_pred_buf[0] +  ((TransCoeffLevel_value /**p_m[y * uiWidth + x]*/ * scale
                //    + (1 << 9)) >> 10 ));
                TransCoeffLevel[ 0 ] = (TransCoeffLevel_value * scale+ (1 << 9)) >> 10 ;
            }
        } else{
            //p_pred_buf[0] = Clip(p_pred_buf[0] + TransCoeffLevel_value);
			TransCoeffLevel[ 0 ] = TransCoeffLevel_value;
        }
#if INTRA_LOG
        if(!transform_skip_flag&&p_slice->CuPredMode == MODE_INTRA){ //no pred data in V2, pred  = recon ; only intra buffer
            printReconInput(cIdx, p_slice->cur_pic->pic_buf[cIdx], pred_stride, x0, y0, TransCoeffLevel, log2TrafoSize, qp,  transform_skip_flag, p_slice->cu_transquant_bypass_flag);
        } 
#endif
#if DEQUANT_LOG
        printDeQuantOut(cIdx, qp, TransCoeffLevel, 1<<log2TrafoSize, 1<<log2TrafoSize, 1<<log2TrafoSize, x0, y0);
#endif

         {

            VO_S32 function_index;
            VO_U32 width = (1 << log2TrafoSize);
            VO_U32 trTpye  = (p_slice->CuPredMode == MODE_INTRA&&cIdx == 0 && log2TrafoSize==2) ? 4:0;
            //VO_S16  tmp_block_array[32 * 32];
            //Do IDCT
            if (p_slice->cu_transquant_bypass_flag||transform_skip_flag)
				function_index = 22;
			else
			{
                function_index = idct_function_index[log2TrafoSize+trTpye]+1;
                /*if (function_index == 1) {
                    function_index = trTpye ? (12 - 1) : (function_index - 1);

                } else {
                    function_index += 1;
                }*/
			}
            //tmp_block_array[0] =(VO_S16) width;

            // call the real IDCT
            //(IDCT_function_pointer_array[function_index])(TransCoeffLevel,
            //    p_pred_buf,p_pred_buf,pred_stride,pred_stride,tmp_block_array);
            tu_task->trans_flag = 1;
			tu_task->function_index = function_index;
			tu_task->width = width;
			tu_task->pred_stride = pred_stride;
			tu_task->TransCoeffLevel = TransCoeffLevel;
			tu_task->p_pred_buf = p_pred_buf;
			p_slice->cur_rctask->total_tu++;
        } //end if (flag_skip_transform)-else   



#if RECON_LOG
        if(p_slice->CuPredMode == MODE_INTRA){  //very difficult to get recon buffer in TU unit in HM
            printResidualOutput(cIdx, p_pred_buf, log2TrafoSize, pred_stride);
        }  
#endif


        return VO_ERR_NONE;
    } 

    if ( last_sig_coeff_x > 3 )	{
        VO_S32 z, len = ( last_sig_coeff_x - 2 ) >> 1, suffix = 0;

        for ( z = len - 1; z >= 0; z-- ) 
            suffix += ( voCabacDecodeBinEP( p_slice->pCabac/*, &p_slice->bs*/ ) << z );

        last_sig_coeff_x = ( 1 << ( ( last_sig_coeff_x  >>  1 ) - 1 ) ) * \
            ( 2 + ( last_sig_coeff_x & 1 ) ) + suffix;//last_sig_coeff_x_suffix
    }

    if ( last_sig_coeff_y > 3 )	{
        VO_S32 z, len = ( last_sig_coeff_y - 2 ) >> 1, suffix = 0;

        for ( z = len - 1; z >= 0; z-- ) 
            suffix += ( voCabacDecodeBinEP( p_slice->pCabac/*, &p_slice->bs*/ ) << z );
        last_sig_coeff_y = ( 1 << ( ( last_sig_coeff_y  >>  1 ) - 1 ) ) *	\
            ( 2 + ( last_sig_coeff_y & 1 ) ) + suffix; //last_sig_coeff_y_suffix
    }

    if ( scanIdx == SCAN_VER ) {
        VOSWAP( VO_U32, last_sig_coeff_x, last_sig_coeff_y );
    }

    lastScanPos = scan4x4_inv[ scanIdx ][ ( last_sig_coeff_y & 3 ) * 4 +  ( last_sig_coeff_x & 3 ) ];


    switch ( scanIdx ) {	
    case SCAN_DIAG: 
        scan_x_off = ( VO_U8* )diag_scan4x4_x;
        scan_y_off = ( VO_U8* )diag_scan4x4_y;
        //uiScanPosLast = diag_scan4x4_inv[last_y_c][last_x_c];

        if ( log2TrafoSize == 5 ) {
            scan_x_cg = ( VO_U8* )diag_scan8x8_x;
            scan_y_cg = ( VO_U8* )diag_scan8x8_y;
            lastSubBlock = diag_scan8x8_inv[ last_sig_coeff_y >> 2 ][ last_sig_coeff_x >> 2 ];
        } else if ( log2TrafoSize == 4 ) {
            scan_x_cg = ( VO_U8* )diag_scan4x4_x;
            scan_y_cg = ( VO_U8* )diag_scan4x4_y;
            lastSubBlock = scan4x4_inv[ SCAN_DIAG ][ ( last_sig_coeff_y >> 2 ) * 4 + ( last_sig_coeff_x >> 2 ) ];
        } else {
            scan_x_cg = ( VO_U8* )diag_scan2x2_x;
            scan_y_cg = ( VO_U8* )diag_scan2x2_y;
            lastSubBlock = scan2x2_inv[ last_sig_coeff_y >> 2 ][ last_sig_coeff_x >> 2 ];
        }

        break;
    case SCAN_HOR:
        scan_x_cg = ( VO_U8* )horiz_scan2x2_x;
        scan_y_cg = ( VO_U8* )horiz_scan2x2_y;
        scan_x_off = ( VO_U8* )horiz_scan4x4_x;
        scan_y_off = ( VO_U8* )horiz_scan4x4_y;
        lastSubBlock = scan2x2_inv[ last_sig_coeff_x >> 2 ][ last_sig_coeff_y >> 2 ];
        break;
    default: //SCAN_VER
        scan_x_cg = ( VO_U8* )horiz_scan2x2_y;
        scan_y_cg = ( VO_U8* )horiz_scan2x2_x;
        scan_x_off = ( VO_U8* )horiz_scan4x4_y;
        scan_y_off = ( VO_U8* )horiz_scan4x4_x;
        lastSubBlock = scan2x2_inv[ last_sig_coeff_y >> 2 ][ last_sig_coeff_x >> 2 ];
        break;
    }
    lastSubBlock = ( ( lastSubBlock << 4 ) + lastScanPos ) >> 4;

    // 统计信息
    // 	if (last_sig_coeff_x == 0 && last_sig_coeff_y == 0) //DC
    // 	{
#if 0
    // 	} else {
    if (log2TrafoSize > 3 && lastSubBlock == 0) {// TU = 16X16,32X32
        non_zero_block_flag = 2;
        if (!p_slice->cu_transquant_bypass_flag&&!transform_skip_flag) 
        {
#if defined(__GNUC__) && USE_INLINE_ASM
                __asm__ volatile
                    (
                    "   vmov.i32  d0, #0               \n\t"
                    "1: subs      %1, %1, #1           \n\t"
                    "   vst1.16  {d0},[%0], %2            \n\t"
                    "   bgt        1b                  \n\t"
                    :
                :"r"(TransCoeffLevel), "r"(1<< (log2TrafoSize)), "r"(1<< (log2TrafoSize+1))
                    :"memory", "d0"
                    ); 
#else
                SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, TransCoeffLevel,0, sizeof(VO_S16) << (log2TrafoSize + log2TrafoSize)); //width*width
#endif
            
        }
		else
        {
#if defined(__GNUC__) && USE_INLINE_ASM
                __asm__ volatile
                    (
                    "   vmov.i32  q0, #0               \n\t"
                    "   vmov.i32  q1, #0               \n\t"
                    "1: subs      %1, %1, #1           \n\t"
                    "   vstmia %0!, {q0-q1}            \n\t"
                    "   bgt        1b                  \n\t"
                    :
                :"r"(TransCoeffLevel), "r"(1<< (log2TrafoSize + log2TrafoSize - 4))
                    :"memory", "q0", "q1"
                    );
#else
                SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, TransCoeffLevel,0, sizeof(VO_S16) << (log2TrafoSize + log2TrafoSize)); //width*width
#endif

            }
    } else {
        non_zero_block_flag = 0;

#if defined(__GNUC__) && USE_INLINE_ASM
                __asm__ volatile
                    (
                    "   vmov.i32  q0, #0               \n\t"
                    "   vmov.i32  q1, #0               \n\t"
                    "1: subs      %1, %1, #1           \n\t"
                    "   vstmia %0!, {q0-q1}            \n\t"
                    "   bgt        1b                  \n\t"
                    :
                :"r"(TransCoeffLevel), "r"(1<< (log2TrafoSize + log2TrafoSize - 4))
                    :"memory", "q0", "q1"
                    );
#else
                SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, TransCoeffLevel,0, sizeof(VO_S16) << (log2TrafoSize + log2TrafoSize)); //width*width
#endif

    }      
#else
    if(log2TrafoSize==5&&lastSubBlock<=9&&lastSubBlock>=3)
    {
        non_zero_block_flag = 4;
    }
	else if(log2TrafoSize>3&&lastSubBlock<=2&&lastSubBlock>=1)
	{
	    non_zero_block_flag = 3;
    }
	else if(log2TrafoSize>=3&&lastSubBlock==0)
	{
	    non_zero_block_flag = 2;
    }
	else
	{
	    non_zero_block_flag = 0;
	}
    SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, TransCoeffLevel,0, sizeof(VO_S16) << (log2TrafoSize + log2TrafoSize));
#endif
    //YU_TBD, clean up coded_sub_block_flag;
    SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, coded_sub_block_flag, 0, sizeof( coded_sub_block_flag ) );
    for ( i = lastSubBlock; i >=  0; i-- ) {	
        VO_U8 inferSbDcSigCoeffFlag = 0;
        VO_U32 tu_width_in_cg = 1 << ( log2TrafoSize - 2 );
        VO_U32 xS = scan_x_cg[ i ];// = scanCG[ i ] / tu_width_in_cg;//= ScanOrder[ log2TrafoSize - 2 ][ scanIdx ][ i ][ 0 ];	
        VO_U32 yS = scan_y_cg[ i ];// = scanCG[ i ] % tu_width_in_cg;//= ScanOrder[ log2TrafoSize - 2 ][ scanIdx ][ i ][ 1 ];	

        //coded_sub_block_flag, exclue first and last block
        if ( ( i < lastSubBlock )  &&  ( i > 0 ) ) {
            VO_U32  offset = 0;

            if ( tu_width_in_cg != 1) {//4x4 case
                if ( xS < ( tu_width_in_cg - 1 ) ) {
                    offset = coded_sub_block_flag[ yS * tu_width_in_cg + xS + 1 ];
                }
                if ( yS < ( tu_width_in_cg - 1 ) ) {
                    offset |= coded_sub_block_flag[ ( yS  + 1 ) * tu_width_in_cg + xS ];
                }
            }
            coded_sub_block_flag[ yS * tu_width_in_cg + xS ] = \
                ( VO_U8 )voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ offset + ( ( cIdx == 0 ) ? OFF_SIG_CG_FLAG_CTX : ( OFF_SIG_CG_FLAG_CTX + NUM_SIG_CG_FLAG_CTX ) ) );
            inferSbDcSigCoeffFlag = 1;	
        } else {
            coded_sub_block_flag[ yS * tu_width_in_cg + xS ] = 1;//YU_TBD, check the value according to spec
        }
        //nz coeff
        if ( coded_sub_block_flag[ yS * tu_width_in_cg + xS ] ) {
            //VO_U32 sig_coeff_flag = 0;//nz flag
            VO_U32 coeff_sign  = 0;
            VO_U32 numSigCoeff = 0;
            VO_U32 sumAbsLevel = 0;
            VO_U32 signHidden  = 0;
            VO_U32 coeff_abs_level_greater1 = 0;
            VO_U32 coeff_abs_level_greater2 = 0;
            VO_U32 prevCsbf = 0;
            //VO_S32 firstSigScanPos = 16;	
            //VO_S32 lastSigScanPos = -1	;
            VO_U32 numGreater1Flag = 0;
            VO_S32 lastGreater1ScanPos = -1;

            VO_S32 k, nb_end, nb_sig_coeff = 0;
            VO_U8  sig_coeff_pos[ 16 ];// = { 0 };
            VO_U32 ctx_set;
            VO_U32 uiGoRiceParam = 0;
           
            //VO_U32 sig_idx;
            //VO_U32 sig_val;

            if ( i == lastSubBlock) {//first subblock
                nb_end = lastScanPos - 1;
                nb_sig_coeff = 1;
                sig_coeff_pos[ 0 ] = ( VO_U8 )lastScanPos;
            } else {
                nb_end = 15;
            }

            if ( xS < ( tu_width_in_cg - 1 ) ) 
                prevCsbf = coded_sub_block_flag[ yS * tu_width_in_cg + xS + 1 ];
            if ( yS < ( tu_width_in_cg - 1 ) ) 
                prevCsbf += ( coded_sub_block_flag[ ( yS  + 1 ) * tu_width_in_cg + xS ] << 1 );

            /*sig_coeff_flag*/
            for ( n = nb_end; n  >=  0; n-- ) {	
                VO_U32 xC = ( xS << 2 ) + ( scan_x_off[ n ] );
                VO_U32 yC = ( yS << 2 ) + ( scan_y_off[ n ] );
                VO_U32 sig_coeff_flag = 1;

                if ( ( n > 0  ||  !inferSbDcSigCoeffFlag ) ) {	
                    VO_S32 ctx_inc;

                    if ( log2TrafoSize == 2 ) {//4x4 case
                        ctx_inc = ctxIndMap[ ( yC << 2 ) + xC ];
                    } else if ( xC + yC == 0 ) {
                        ctx_inc = 0;
                    } else {
                        VO_U32 xP , yP;

                        xP = xC & 3;
                        yP = yC & 3;

                        if ( prevCsbf == 0 ) {
                            ctx_inc = xP + yP <= 2 ? ( xP + yP == 0 ? 2 : 1 ) : 0;
                        } else if ( prevCsbf == 1 ) {
                            ctx_inc = yP <= 1 ? ( yP == 0 ? 2 : 1 ) : 0;
                        } else if ( prevCsbf == 2 ) {
                            ctx_inc = xP <= 1 ? ( xP == 0 ? 2 : 1 ) : 0;
                        } else {
                            ctx_inc = 2;
                        }

                        ctx_inc += log2TrafoSize == 3 ? ( scanIdx == 0 ? 9 : 15 ) : ( cIdx == 0 ? 21 : 12 );
                        if ( cIdx == 0 ) {
                            ctx_inc += ( ( xC >> 2) + ( yC >> 2 ) > 0 ) ? 3 : 0;
                        }
                    }
                    sig_coeff_flag = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ ctx_inc + ( ( cIdx == 0 ) ? OFF_SIG_FLAG_CTX : ( OFF_SIG_FLAG_CTX + NUM_SIG_FLAG_CTX_LUMA ) ) );
                }
                if ( sig_coeff_flag ) {	//sig_coeff_flag
                    sig_coeff_pos[ nb_sig_coeff ] = (VO_U8)n;
                    nb_sig_coeff ++;
                    inferSbDcSigCoeffFlag = 0;	
                }	
            }

            /*coeff_abs_level_greater1_flag*/
            if (nb_sig_coeff ==0){
                continue;
            }
            //if (nb_sig_coeff >0)
            {
                n = sig_coeff_pos[ 0 ];
                /*if ( numGreater1Flag < 8 ) */{	
                    VO_U32 code;
                    ctx_set = ( i > 0 && cIdx == 0 ) ? 2 : 0;
                    if ( ( i != lastSubBlock ) && Greater1Ctx == 0 )
                        ctx_set++;
                    //Greater1Ctx = 1; 
                    code = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_ONE_FLAG_CTX + ( ctx_set << 2 ) + 1 + ( cIdx == 0 ? 0 : 16 ) ); // coeff_abs_level_greater1_flag
                    if ( code ) {
                        Greater1Ctx = 0;
                        lastGreater1ScanPos = n	;
                    } else {
                       Greater1Ctx = 2;
                    }
                    numGreater1Flag++;	
                    
                    coeff_abs_level_greater1 = ( code << n );
                }
            }
            for ( k = 1; k < nb_sig_coeff; k++ ) {	
                n = sig_coeff_pos[ k ];
                if ( numGreater1Flag < 8 ) {	
                    VO_U32 code;
//                     if ( k == 0 ) {
//                         ctx_set = ( i > 0 && cIdx == 0 ) ? 2 : 0;
//                         if ( ( i != lastSubBlock ) && Greater1Ctx == 0 )
//                             ctx_set++;
//                         Greater1Ctx = 1;
//                     }
                    code = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_ONE_FLAG_CTX + ( ctx_set << 2 ) + Greater1Ctx + ( cIdx == 0 ? 0 : 16 ) ); // coeff_abs_level_greater1_flag
                    if ( code ) {
                        Greater1Ctx = 0;
                    } else if ( Greater1Ctx > 0 && Greater1Ctx < 3 ) {
                        Greater1Ctx++;
                    }
                    numGreater1Flag++;	
                    if ( code  &&  ( lastGreater1ScanPos == -1 ) )	//coeff_abs_level_greater1_flag[ n ]
                        lastGreater1ScanPos = n	;
                    coeff_abs_level_greater1 |= ( code << n );
                }
                //if ( lastSigScanPos == -1 )	
                //	lastSigScanPos = n;	
                //firstSigScanPos = n	;
            }
            //if(nb_sig_coeff) 
            {
                signHidden = ( sig_coeff_pos[ 0 ] - sig_coeff_pos[ nb_sig_coeff - 1 ] > 3  &&  !p_slice->cu_transquant_bypass_flag );	
                if ( lastGreater1ScanPos  !=  -1 )	
                    coeff_abs_level_greater2 = \
                    ( voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_ABS_FLAG_CTX + ctx_set + ( ( cIdx == 0 ) ? 0 : 4 ) ) ) << lastGreater1ScanPos ;

                if ( !p_pps->sign_data_hiding_enabled_flag || !signHidden || ( n != sig_coeff_pos[ nb_sig_coeff - 1 ] ) ) {
                    coeff_sign = voCabacDecodeBinsEP( p_slice->pCabac, /*&p_slice->bs,*/ nb_sig_coeff );
                    coeff_sign <<= 32 - (nb_sig_coeff);        
                    //sig_idx = nb_sig_coeff - 1;
                } else {
                    coeff_sign = voCabacDecodeBinsEP( p_slice->pCabac, /*&p_slice->bs,*/ nb_sig_coeff - 1 );
                    coeff_sign <<= 32 - (nb_sig_coeff - 1);        
                    //sig_idx = nb_sig_coeff - 2;
                }
            }

            //if ( numGreater1Flag == 0 || nb_sig_coeff > C1FLAG_NUMBER ) {
            {
                VO_S32 baseLevel;
                VO_U32 xC;
                VO_U32 yC;                
                VO_S16 TransCoeffLevel_value;
                VO_U32 xC0 = ( xS << 2 );
                VO_U32 yC0 = ( yS << 2 );                
                for ( k = 0; k < (nb_sig_coeff-1); k++ ) {
                    n = sig_coeff_pos[ k ];
                    xC = xC0 + ( scan_x_off[ n ] );
                    yC = yC0 + ( scan_y_off[ n ] );

                    baseLevel = 1 + ( ( coeff_abs_level_greater1 & ( 1 << n ) ) >> n ) + \
                        ( ( coeff_abs_level_greater2 & ( 1 << n ) ) >> n );	
                    if ( baseLevel  ==  ( ( numSigCoeff < 8 ) ? \
                        ( (n  ==  lastGreater1ScanPos) ? 3 : 2 ) : 1 ) ) {	
                            baseLevel += xReadCoefRemainExGolomb( p_slice->pCabac, /*&p_slice->bs,*/ uiGoRiceParam );

                            if ( baseLevel > ( 3 * ( 1 << uiGoRiceParam ) ) ) {//YU_TBD: check spec
                                uiGoRiceParam = MIN( 4, uiGoRiceParam + 1);
                            }
                    }
                    TransCoeffLevel_value = ( coeff_sign >> 31 ) ? - baseLevel : baseLevel;
                    coeff_sign <<= 1;
                    sumAbsLevel  +=  baseLevel;
                    if (!p_slice->cu_transquant_bypass_flag) {
                        if (!transform_skip_flag){
                            TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = Clip3(-32768, 32767, (TransCoeffLevel_value /**p_m[y * uiWidth + x]*/ * scale
                                + (1 << (bdShift - 1))) >> bdShift);
                        } else {
#if DEQUANT_LOG
                            VO_S16 s16Coeff = Clip3(-32768, 32767, (TransCoeffLevel_value /**p_m[y * uiWidth + x]*/ * scale
                                + (1 << (bdShift - 1))) >> bdShift);
                            TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = s16Coeff;  // for checking error
#endif
                            // 						p_pred_buf[xC + yC * pred_stride] = Clip(p_pred_buf[xC + yC * pred_stride] + 
                            // 							((s16Coeff  + (1 << (transform_skip_shift -1))) >> transform_skip_shift));
                            //p_pred_buf[xC + yC * pred_stride] = Clip(p_pred_buf[xC + yC * pred_stride] +  ((TransCoeffLevel_value /**p_m[y * uiWidth + x]*/ * scale
                            //    + (1 << 9)) >> 10 ));
                            TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = ((TransCoeffLevel_value* scale+ (1 << 9)) >> 10 );
                        }
                    } else{
                        //p_pred_buf[xC + yC * pred_stride] = Clip(p_pred_buf[xC + yC * pred_stride] + TransCoeffLevel_value);
						TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = TransCoeffLevel_value;
                    }

                    numSigCoeff++;
                }	
                //if (nb_sig_coeff >0)
                {	
                    n = sig_coeff_pos[ k ];
                    xC = xC0 + ( scan_x_off[ n ] );
                    yC = yC0 + ( scan_y_off[ n ] );
                    baseLevel = 1 + ( ( coeff_abs_level_greater1 & ( 1 << n ) ) >> n ) + \
                        ( ( coeff_abs_level_greater2 & ( 1 << n ) ) >> n );	
                    if ( baseLevel  ==  ( ( numSigCoeff < 8 ) ? \
                        ( (n  ==  lastGreater1ScanPos) ? 3 : 2 ) : 1 ) ) {	
                            baseLevel += xReadCoefRemainExGolomb( p_slice->pCabac, /*&p_slice->bs,*/ uiGoRiceParam );
                    }
                    TransCoeffLevel_value = ( coeff_sign >> 31 ) ? - baseLevel : baseLevel;
                    
                    if ( p_pps->sign_data_hiding_enabled_flag  &&  signHidden ) {	
                        sumAbsLevel  +=  baseLevel;
                        if (/* ( k == ( nb_sig_coeff - 1 ) ) && */( ( sumAbsLevel & 1 ) == 1 ) )	//TransCoeffLevel[ x0 ][ y0 ][ cIdx ][ xC ][ yC ]
                            TransCoeffLevel_value = -TransCoeffLevel_value;
                    }	
                    //TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = TransCoeffLevel_value;
                    if (!p_slice->cu_transquant_bypass_flag) {
                        if (!transform_skip_flag){
                            TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = Clip3(-32768, 32767, (TransCoeffLevel_value /**p_m[y * uiWidth + x]*/ * scale
                                + (1 << (bdShift - 1))) >> bdShift);
                        } else {
#if DEQUANT_LOG
                            VO_S16 s16Coeff = Clip3(-32768, 32767, (TransCoeffLevel_value /**p_m[y * uiWidth + x]*/ * scale
                                + (1 << (bdShift - 1))) >> bdShift);
                            TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = s16Coeff;  // for checking error
#endif
                            //p_pred_buf[xC + yC * pred_stride] = Clip(p_pred_buf[xC + yC * pred_stride] +  ((TransCoeffLevel_value /**p_m[y * uiWidth + x]*/ * scale
                            //    + (1 << 9)) >> 10 ));
                            TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = ((TransCoeffLevel_value* scale+ (1 << 9)) >> 10 );
                        }
                    } else{
                        //p_pred_buf[xC + yC * pred_stride] = Clip(p_pred_buf[xC + yC * pred_stride] + TransCoeffLevel_value);
						TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = TransCoeffLevel_value;
                    }
                    //numSigCoeff++;
                }	
            }
            
            
        }
    }

#if INTRA_LOG
    if(!transform_skip_flag&&p_slice->CuPredMode == MODE_INTRA){ //no pred data in V2, pred  = recon ; only intra buffer
        printReconInput(cIdx, p_slice->cur_pic->pic_buf[cIdx], pred_stride, x0, y0, TransCoeffLevel, log2TrafoSize, qp,  transform_skip_flag, p_slice->cu_transquant_bypass_flag);
    } 
#endif
#if DEQUANT_LOG
    printDeQuantOut(cIdx, qp, TransCoeffLevel, 1<<log2TrafoSize, 1<<log2TrafoSize, 1<<log2TrafoSize, x0, y0);
#endif


    {
        VO_S32 function_index;
        VO_U32 width = (1 << log2TrafoSize);
        VO_U32 trTpye  = (p_slice->CuPredMode == MODE_INTRA&&cIdx == 0 && log2TrafoSize==2) ? 4:0;
        //VO_S16  tmp_block_array[32 * 32];
        //VO_S16*  tmp_block_array = p_slice->asm_mc0_buff;

        //Do IDCT
        if (p_slice->cu_transquant_bypass_flag||transform_skip_flag)
				function_index = 23;
		else
		{
            //function_index = width >> 2;
        // when TU is 4*4 size and mode != REG_DCT, it will call DST
            /*if (function_index == 1) {
                function_index = trTpye ? (12 - non_zero_block_flag) : (function_index - non_zero_block_flag);

            } else {
                function_index += non_zero_block_flag;
            }*/
            function_index = idct_function_index[log2TrafoSize+trTpye]+non_zero_block_flag;            
		}
        //tmp_block_array[0] =(VO_S16) width;
        //printf("function_index = [%d], width = [%d].\n",function_index, width);

        // call the real IDCT
        //(IDCT_function_pointer_array[function_index])(TransCoeffLevel,
        //    p_pred_buf,p_pred_buf,pred_stride,pred_stride,tmp_block_array);
        //printf("done.\n");
        tu_task->trans_flag = 1;
		tu_task->function_index = function_index;
		tu_task->width = width;
		tu_task->pred_stride = pred_stride;
		tu_task->TransCoeffLevel = TransCoeffLevel;
		tu_task->p_pred_buf = p_pred_buf;
		p_slice->cur_rctask->total_tu++;
    } //end if (flag_skip_transform)-else   


#if RECON_LOG
    if(p_slice->CuPredMode == MODE_INTRA){  //very difficult to get recon buffer in TU unit in HM
        printResidualOutput(cIdx, p_pred_buf, log2TrafoSize, pred_stride);
    }  
#endif

    return VO_ERR_NONE;
}	

static VO_S32 residual_coding_scaling_list( H265_DEC_SLICE* p_slice,
  const VO_U32 x0, 
  const VO_U32 y0, 
  const VO_U32 log2TrafoSize, 
  const VO_U32 cIdx ) 
{	
  VO_S32 n, i;
  //H265_DEC_SPS *p_sps = p_slice->p_sps;
  H265_DEC_PPS *p_pps = p_slice->p_pps;
  VO_U32 transform_skip_flag = 0;
  VO_S32 last_sig_coeff_x, last_sig_coeff_y;
  VO_S32 lastScanPos, lastSubBlock;
  VO_U32 scanIdx = SCAN_DIAG;
  VO_S32 ctx_offset, shift, cmax = ( log2TrafoSize << 1 ) - 1;
  //VO_U32 uiWidth = 1<<log2TrafoSize;
  VO_U8 *scan_x_cg, *scan_y_cg, *scan_x_off, *scan_y_off;
  VO_U8 coded_sub_block_flag[ 64 ];
  VO_U32 Greater1Ctx = 1; 
  VO_U32 qp = p_slice->entries[p_slice->currEntryNum].dequant_qp[cIdx];  // TODO: get from p_slice
  VO_U32  per_for_qp = qp / 6;
  //VO_U32  rem_for_qp = qp % 6; 
  VO_S32 scale =  (per_for_qp ); 
  VO_S32 bdShift = 3 + log2TrafoSize;//bit_depth + log2_width - 3; //bdShift = BitDepthY/C + Log2(nTbS) - 5


  VO_U8 non_zero_block_flag;
  VO_U32 pred_stride = p_slice->cur_pic->pic_stride[cIdx];
  VO_U8* p_pred_buf = p_slice->cur_pic->pic_buf[cIdx] + x0 + y0 * pred_stride;


  //VO_S16 *TransCoeffLevel = p_slice->TransCoeffLevel;
  VO_S16 *TransCoeffLevel = p_slice->p_coeff[cIdx];
  VO_U32 matrixId = (p_slice->CuPredMode == MODE_INTRA ? 0 : 3) + cIdx;
  VO_S16 * p_m = p_slice->MMulLevelScale[log2TrafoSize-2][matrixId][qp % 6];
  TUTask* tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
  p_slice->p_coeff[cIdx] += 1 << (log2TrafoSize + log2TrafoSize);
#if H265_LOG
  // only for print log file in debug.
  memset(TransCoeffLevel,0, sizeof(VO_S16) << (log2TrafoSize + log2TrafoSize)); //width*width
#endif
  /* derive scanIdx 7.4.9.11*/
  if ( ( p_slice->CuPredMode == MODE_INTRA ) && \
    ( ( log2TrafoSize == 2 ) || ( log2TrafoSize == 3 && ( cIdx == 0 ) ) ) ) {
      VO_U32 predModeIntra = ( cIdx == 0 ) ? p_slice->IntraPredMode_Y : p_slice->IntraPredModeC;
      if ( predModeIntra >= 6 && predModeIntra <= 14 ) {
        scanIdx  = SCAN_VER;
      } else if ( predModeIntra >= 22 && predModeIntra <= 30) {        
        scanIdx  = SCAN_HOR;
      }
  }

  /*transform_skip_flag*/
  if ( p_pps->transform_skip_enabled_flag && !p_slice->cu_transquant_bypass_flag && ( log2TrafoSize  ==  2 ) ) {
    transform_skip_flag = \
      voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_TRANSFORMSKIP_FLAG_CTX + ( cIdx > 0 ? 1 : 0 ) );
  }

  /*last_sig_coeff_x and last_sig_coeff_y*/
  if ( cIdx == 0 ) {
    ctx_offset = 3 * ( log2TrafoSize - 2 ) + ( ( log2TrafoSize - 1 ) >> 2 );
    shift = ( log2TrafoSize + 1 ) >> 2;
  } else {
    ctx_offset = NUM_CTX_LAST_FLAG_XY;
    shift = log2TrafoSize - 2 ; 
  }

  for ( i = 0; i < cmax; i++ ) {
    if ( !voCabacDecodeBin( p_slice->pCabac,/* &p_slice->bs,*/ OFF_LAST_X_CTX + ctx_offset + ( i >> shift ) ) )
      break;
  }
  last_sig_coeff_x = i;

  for ( i = 0; i < cmax; i++ ) {
    if ( !voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_LAST_Y_CTX + ctx_offset + ( i >> shift ) ) )
      break;
  }
  last_sig_coeff_y = i;
  //g_caller++;
  if (last_sig_coeff_x == 0 && last_sig_coeff_y == 0) //DC
  {
    VO_U32 coeff_abs_level_greater2 = 0;
    VO_U32 coeff_sign;
    VO_S32 baseLevel;
    VO_S16 TransCoeffLevel_value;
    VO_U32 coeff_abs_level_greater1 = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_ONE_FLAG_CTX + 0/*( ctx_set << 2 )*/ + 1 + ( cIdx == 0 ? 0 : 16 ) ); // coeff_abs_level_greater1_flag
    //g_caller_1x1 ++;
    if ( coeff_abs_level_greater1  /*&&  ( lastGreater1ScanPos == -1 ) */){	//coeff_abs_level_greater1_flag[ n ]           
      coeff_abs_level_greater2 = \
        ( voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_ABS_FLAG_CTX/* + ctx_set*/ + ( ( cIdx == 0 ) ? 0 : 4 ) ) ) /*<< lastGreater1ScanPos*/ ;
    }
    coeff_sign = voCabacDecodeBinsEP( p_slice->pCabac, /*&p_slice->bs,*/1 /*nb_sig_coeff */);
    baseLevel = 1 + coeff_abs_level_greater1 + coeff_abs_level_greater2 ;
    if ( baseLevel  ==  3 ) {	
      VO_U32 uiGoRiceParam = 0;
      baseLevel += xReadCoefRemainExGolomb( p_slice->pCabac, /*&p_slice->bs,*/ uiGoRiceParam );
    }

    TransCoeffLevel_value = coeff_sign ? - baseLevel : baseLevel;
    if (!p_slice->cu_transquant_bypass_flag) {
      if (!transform_skip_flag){
        TransCoeffLevel[ 0 ] = Clip3(-32768, 32767, ((TransCoeffLevel_value *p_m[0] << scale)
          + (1 << (bdShift - 1))) >> bdShift);
      } else {
#if DEQUANT_LOG
        TransCoeffLevel[ 0 ] = Clip3(-32768, 32767, ((TransCoeffLevel_value *p_m[0] << scale)
          + (1 << (bdShift - 1))) >> bdShift);
#endif
        //p_pred_buf[0] = Clip(p_pred_buf[0] +  (((TransCoeffLevel_value *p_m[0]  << scale)
        //  + (1 << 9)) >> 10 ));
        TransCoeffLevel[ 0 ] = ((TransCoeffLevel_value *p_m[0]  << scale)+ (1 << 9)) >> 10;
      }
    } else{
      //p_pred_buf[0] = Clip(p_pred_buf[0] + TransCoeffLevel_value);
	  TransCoeffLevel[ 0 ] = TransCoeffLevel_value;
    }
#if INTRA_LOG
    if(!transform_skip_flag&&p_slice->CuPredMode == MODE_INTRA){ //no pred data in V2, pred  = recon ; only intra buffer
      printReconInput(cIdx, p_slice->cur_pic->pic_buf[cIdx], pred_stride, x0, y0, TransCoeffLevel, log2TrafoSize, qp,  transform_skip_flag, p_slice->cu_transquant_bypass_flag);
    } 
#endif
#if DEQUANT_LOG
    printDeQuantOut(cIdx, qp, TransCoeffLevel, 1<<log2TrafoSize, 1<<log2TrafoSize, 1<<log2TrafoSize, x0, y0);
#endif

    {
      VO_S32 function_index;
      VO_U32 width = (1 << log2TrafoSize);
      VO_U32 trTpye  = (p_slice->CuPredMode == MODE_INTRA&&cIdx == 0 && log2TrafoSize==2) ? 4:0;
      //VO_S16  tmp_block_array[32 * 32];
    //VO_S16*  tmp_block_array = p_slice->asm_mc0_buff;

    //Do IDCT
      if (p_slice->cu_transquant_bypass_flag||transform_skip_flag)
				function_index = 22;
	  else
	  {
        /*function_index = width >> 2;
        if (function_index == 1) {
          function_index = trTpye ? (12 - 1) : (function_index - 1);

        } else {
          function_index += 1;
        }*/
          function_index = idct_function_index[log2TrafoSize+trTpye]+1; 
	  }
      //tmp_block_array[0] =(VO_S16) width;
    //printf("function_index = [%d], width = [%d].\n",function_index, width);

    // call the real IDCT
      //(IDCT_function_pointer_array[function_index])(TransCoeffLevel,
      //  p_pred_buf,p_pred_buf,pred_stride,pred_stride,tmp_block_array);
	  tu_task->trans_flag = 1;
	  tu_task->function_index = function_index;
	  tu_task->width = width;
	  tu_task->pred_stride = pred_stride;
	  tu_task->TransCoeffLevel = TransCoeffLevel;
	  tu_task->p_pred_buf = p_pred_buf;
	  p_slice->cur_rctask->total_tu++;
    } //end if (flag_skip_transform)-else   


#if RECON_LOG
    if(p_slice->CuPredMode == MODE_INTRA){  //very difficult to get recon buffer in TU unit in HM
      printResidualOutput(cIdx, p_pred_buf, log2TrafoSize, pred_stride);
    }  
#endif


    return VO_ERR_NONE;
  } 

  if ( last_sig_coeff_x > 3 )	{
    VO_S32 z, len = ( last_sig_coeff_x - 2 ) >> 1, suffix = 0;

    for ( z = len - 1; z >= 0; z-- ) 
      suffix += ( voCabacDecodeBinEP( p_slice->pCabac/*, &p_slice->bs*/ ) << z );

    last_sig_coeff_x = ( 1 << ( ( last_sig_coeff_x  >>  1 ) - 1 ) ) * \
      ( 2 + ( last_sig_coeff_x & 1 ) ) + suffix;//last_sig_coeff_x_suffix
  }

  if ( last_sig_coeff_y > 3 )	{
    VO_S32 z, len = ( last_sig_coeff_y - 2 ) >> 1, suffix = 0;

    for ( z = len - 1; z >= 0; z-- ) 
      suffix += ( voCabacDecodeBinEP( p_slice->pCabac/*, &p_slice->bs*/ ) << z );
    last_sig_coeff_y = ( 1 << ( ( last_sig_coeff_y  >>  1 ) - 1 ) ) *	\
      ( 2 + ( last_sig_coeff_y & 1 ) ) + suffix; //last_sig_coeff_y_suffix
  }

  if ( scanIdx == SCAN_VER ) {
    VOSWAP( VO_U32, last_sig_coeff_x, last_sig_coeff_y );
  }

  lastScanPos = scan4x4_inv[ scanIdx ][ ( last_sig_coeff_y & 3 ) * 4 +  ( last_sig_coeff_x & 3 ) ];


  switch ( scanIdx ) {	
  case SCAN_DIAG: 
    scan_x_off = ( VO_U8* )diag_scan4x4_x;
    scan_y_off = ( VO_U8* )diag_scan4x4_y;
    //uiScanPosLast = diag_scan4x4_inv[last_y_c][last_x_c];

    if ( log2TrafoSize == 5 ) {
      scan_x_cg = ( VO_U8* )diag_scan8x8_x;
      scan_y_cg = ( VO_U8* )diag_scan8x8_y;
      lastSubBlock = diag_scan8x8_inv[ last_sig_coeff_y >> 2 ][ last_sig_coeff_x >> 2 ];
    } else if ( log2TrafoSize == 4 ) {
      scan_x_cg = ( VO_U8* )diag_scan4x4_x;
      scan_y_cg = ( VO_U8* )diag_scan4x4_y;
      lastSubBlock = scan4x4_inv[ SCAN_DIAG ][ ( last_sig_coeff_y >> 2 ) * 4 + ( last_sig_coeff_x >> 2 ) ];
    } else {
      scan_x_cg = ( VO_U8* )diag_scan2x2_x;
      scan_y_cg = ( VO_U8* )diag_scan2x2_y;
      lastSubBlock = scan2x2_inv[ last_sig_coeff_y >> 2 ][ last_sig_coeff_x >> 2 ];
    }

    break;
  case SCAN_HOR:
    scan_x_cg = ( VO_U8* )horiz_scan2x2_x;
    scan_y_cg = ( VO_U8* )horiz_scan2x2_y;
    scan_x_off = ( VO_U8* )horiz_scan4x4_x;
    scan_y_off = ( VO_U8* )horiz_scan4x4_y;
    lastSubBlock = scan2x2_inv[ last_sig_coeff_x >> 2 ][ last_sig_coeff_y >> 2 ];
    break;
  default: //SCAN_VER
    scan_x_cg = ( VO_U8* )horiz_scan2x2_y;
    scan_y_cg = ( VO_U8* )horiz_scan2x2_x;
    scan_x_off = ( VO_U8* )horiz_scan4x4_y;
    scan_y_off = ( VO_U8* )horiz_scan4x4_x;
    lastSubBlock = scan2x2_inv[ last_sig_coeff_y >> 2 ][ last_sig_coeff_x >> 2 ];
    break;
  }
  lastSubBlock = ( ( lastSubBlock << 4 ) + lastScanPos ) >> 4;

  // 统计信息
  // 	if (last_sig_coeff_x == 0 && last_sig_coeff_y == 0) //DC
  // 	{
  // 		non_zero_block_flag = 1;
  // 	} else {
#if 0
  if (log2TrafoSize > 3 && lastSubBlock == 0) {// TU = 16X16,32X32
    non_zero_block_flag = 2;
    if (!p_slice->cu_transquant_bypass_flag&&!transform_skip_flag) 
    {
      
// #ifdef VOARMV7
#if defined(__GNUC__) && USE_INLINE_ASM
        __asm__ volatile
          (
          "   vmov.i32  d0, #0               \n\t"
          "1: subs      %1, %1, #1           \n\t"
          "   vst1.16  {d0},[%0], %2            \n\t"
          "   bgt        1b                  \n\t"
          :
        :"r"(TransCoeffLevel), "r"(1<< (log2TrafoSize)), "r"(1<< (log2TrafoSize+1))
          :"memory", "d0"
          );
// #else
//         {
//           uint8x16_t bzero =  veorq_u8(bzero, bzero);
//           for(i = 0; i < (1<< (log2TrafoSize)); i++)
//             vst1q_u8(TransCoeffLevel + (i << log2TrafoSize), bzero);
//         }
// #endif    
#else
        memset(TransCoeffLevel,0, sizeof(VO_S16) << (log2TrafoSize + log2TrafoSize)); //width*width
#endif
      
    }
	else
		{
// #ifdef VOARMV7
#if defined(__GNUC__) && USE_INLINE_ASM
							__asm__ volatile
								(
								"	vmov.i32  q0, #0			   \n\t"
								"	vmov.i32  q1, #0			   \n\t"
								"1: subs	  %1, %1, #1		   \n\t"
								"	vstmia %0!, {q0-q1} 		   \n\t"
								"	bgt 	   1b				   \n\t"
								:
							:"r"(TransCoeffLevel), "r"(1<< (log2TrafoSize + log2TrafoSize - 4))
								:"memory", "q0", "q1"
								);
// #else
// 							{ 
// 								uint8x16_t bzero =	veorq_u8(bzero, bzero);
// 								for(i = 0; i < (1<< (log2TrafoSize + log2TrafoSize - 3)); i++)
// 									vst1q_u8((VO_U8*)TransCoeffLevel + i * 16, bzero);
//       }
// #endif    
#else
							SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, TransCoeffLevel,0, sizeof(VO_S16) << (log2TrafoSize + log2TrafoSize)); //width*width
#endif

    }
  } else {
    non_zero_block_flag = 0;

// #ifdef VOARMV7
#if defined(__GNUC__) && USE_INLINE_ASM
        __asm__ volatile
          (
          "   vmov.i32  q0, #0               \n\t"
          "   vmov.i32  q1, #0               \n\t"
          "1: subs      %1, %1, #1           \n\t"
          "   vstmia %0!, {q0-q1}            \n\t"
          "   bgt        1b                  \n\t"
          :
        :"r"(TransCoeffLevel), "r"(1<< (log2TrafoSize + log2TrafoSize - 4))
          :"memory", "q0", "q1"
          );
// #else
//         { 
//           uint8x16_t bzero =  veorq_u8(bzero, bzero);
//           for(i = 0; i < (1<< (log2TrafoSize + log2TrafoSize - 3)); i++)
//             vst1q_u8((VO_U8*)TransCoeffLevel + i * 16, bzero);
//         }
// #endif    
#else
        memset(TransCoeffLevel,0, sizeof(VO_S16) << (log2TrafoSize + log2TrafoSize)); //width*width
#endif

  }      
  //}
#else
	  if(log2TrafoSize==5&&lastSubBlock<=9&&lastSubBlock>=3)
	  {
		  non_zero_block_flag = 4;
	  }
	  else if(log2TrafoSize>3&&lastSubBlock<=2&&lastSubBlock>=1)
	  {
		  non_zero_block_flag = 3;
	  }
	  else if(log2TrafoSize>=3&&lastSubBlock==0)
	  {
		  non_zero_block_flag = 2;
	  }
	  else
	  {
		  non_zero_block_flag = 0;
	  }
	  SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, TransCoeffLevel,0, sizeof(VO_S16) << (log2TrafoSize + log2TrafoSize));
#endif

  //YU_TBD, clean up coded_sub_block_flag;
  memset( coded_sub_block_flag, 0, sizeof( coded_sub_block_flag ) );
  for ( i = lastSubBlock; i >=  0; i-- ) {	
    VO_U8 inferSbDcSigCoeffFlag = 0;
    VO_U32 tu_width_in_cg = 1 << ( log2TrafoSize - 2 );
    VO_U32 xS = scan_x_cg[ i ];// = scanCG[ i ] / tu_width_in_cg;//= ScanOrder[ log2TrafoSize - 2 ][ scanIdx ][ i ][ 0 ];	
    VO_U32 yS = scan_y_cg[ i ];// = scanCG[ i ] % tu_width_in_cg;//= ScanOrder[ log2TrafoSize - 2 ][ scanIdx ][ i ][ 1 ];	

    //coded_sub_block_flag, exclue first and last block
    if ( ( i < lastSubBlock )  &&  ( i > 0 ) ) {
      VO_U32  offset = 0;

      if ( tu_width_in_cg != 1) {//4x4 case
        if ( xS < ( tu_width_in_cg - 1 ) ) {
          offset = coded_sub_block_flag[ yS * tu_width_in_cg + xS + 1 ];
        }
        if ( yS < ( tu_width_in_cg - 1 ) ) {
          offset |= coded_sub_block_flag[ ( yS  + 1 ) * tu_width_in_cg + xS ];
        }
      }
      coded_sub_block_flag[ yS * tu_width_in_cg + xS ] = \
        ( VO_U8 )voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ offset + ( ( cIdx == 0 ) ? OFF_SIG_CG_FLAG_CTX : ( OFF_SIG_CG_FLAG_CTX + NUM_SIG_CG_FLAG_CTX ) ) );
      inferSbDcSigCoeffFlag = 1;	
    } else {
      coded_sub_block_flag[ yS * tu_width_in_cg + xS ] = 1;//YU_TBD, check the value according to spec
    }
    //nz coeff
    if ( coded_sub_block_flag[ yS * tu_width_in_cg + xS ] ) {
      //VO_U32 sig_coeff_flag = 0;//nz flag
      VO_U32 coeff_sign  = 0;
      VO_U32 numSigCoeff = 0;
      VO_U32 sumAbsLevel = 0;
      VO_U32 signHidden  = 0;
      VO_U32 coeff_abs_level_greater1 = 0;
      VO_U32 coeff_abs_level_greater2 = 0;
      VO_U32 prevCsbf = 0;
      //VO_S32 firstSigScanPos = 16;	
      //VO_S32 lastSigScanPos = -1	;
      VO_U32 numGreater1Flag = 0;
      VO_S32 lastGreater1ScanPos = -1;

      VO_S32 k, nb_end, nb_sig_coeff = 0;
      VO_U8  sig_coeff_pos[ 16 ];// = { 0 };
      VO_U32 ctx_set;
      VO_U32 uiGoRiceParam = 0;

      //VO_U32 sig_idx;
      //VO_U32 sig_val;

      if ( i == lastSubBlock) {//first subblock
        nb_end = lastScanPos - 1;
        nb_sig_coeff = 1;
        sig_coeff_pos[ 0 ] = ( VO_U8 )lastScanPos;
      } else {
        nb_end = 15;
      }

      if ( xS < ( tu_width_in_cg - 1 ) ) 
        prevCsbf = coded_sub_block_flag[ yS * tu_width_in_cg + xS + 1 ];
      if ( yS < ( tu_width_in_cg - 1 ) ) 
        prevCsbf += ( coded_sub_block_flag[ ( yS  + 1 ) * tu_width_in_cg + xS ] << 1 );

      /*sig_coeff_flag*/
      for ( n = nb_end; n  >=  0; n-- ) {	
        VO_U32 xC = ( xS << 2 ) + ( scan_x_off[ n ] );
        VO_U32 yC = ( yS << 2 ) + ( scan_y_off[ n ] );
        VO_U32 sig_coeff_flag = 1;

        if ( ( n > 0  ||  !inferSbDcSigCoeffFlag ) ) {	
          VO_S32 ctx_inc;

          if ( log2TrafoSize == 2 ) {//4x4 case
            ctx_inc = ctxIndMap[ ( yC << 2 ) + xC ];
          } else if ( xC + yC == 0 ) {
            ctx_inc = 0;
          } else {
            VO_U32 xP , yP;

            xP = xC & 3;
            yP = yC & 3;

            if ( prevCsbf == 0 ) {
              ctx_inc = xP + yP <= 2 ? ( xP + yP == 0 ? 2 : 1 ) : 0;
            } else if ( prevCsbf == 1 ) {
              ctx_inc = yP <= 1 ? ( yP == 0 ? 2 : 1 ) : 0;
            } else if ( prevCsbf == 2 ) {
              ctx_inc = xP <= 1 ? ( xP == 0 ? 2 : 1 ) : 0;
            } else {
              ctx_inc = 2;
            }

            ctx_inc += log2TrafoSize == 3 ? ( scanIdx == 0 ? 9 : 15 ) : ( cIdx == 0 ? 21 : 12 );
            if ( cIdx == 0 ) {
              ctx_inc += ( ( xC >> 2) + ( yC >> 2 ) > 0 ) ? 3 : 0;
            }
          }
          sig_coeff_flag = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ ctx_inc + ( ( cIdx == 0 ) ? OFF_SIG_FLAG_CTX : ( OFF_SIG_FLAG_CTX + NUM_SIG_FLAG_CTX_LUMA ) ) );
        }
        if ( sig_coeff_flag ) {	//sig_coeff_flag
          sig_coeff_pos[ nb_sig_coeff ] = (VO_U8)n;
          nb_sig_coeff ++;
          inferSbDcSigCoeffFlag = 0;	
        }	
      }

      /*coeff_abs_level_greater1_flag*/
      if (nb_sig_coeff ==0){
        continue;
      }
      //if (nb_sig_coeff >0)
      {
        n = sig_coeff_pos[ 0 ];
        /*if ( numGreater1Flag < 8 ) */{	
          VO_U32 code;
          ctx_set = ( i > 0 && cIdx == 0 ) ? 2 : 0;
          if ( ( i != lastSubBlock ) && Greater1Ctx == 0 )
            ctx_set++;
          //Greater1Ctx = 1; 
          code = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_ONE_FLAG_CTX + ( ctx_set << 2 ) + 1 + ( cIdx == 0 ? 0 : 16 ) ); // coeff_abs_level_greater1_flag
          if ( code ) {
            Greater1Ctx = 0;
            lastGreater1ScanPos = n	;
          } else {
            Greater1Ctx = 2;
          }
          numGreater1Flag++;	

          coeff_abs_level_greater1 = ( code << n );
        }
      }
      for ( k = 1; k < nb_sig_coeff; k++ ) {	
        n = sig_coeff_pos[ k ];
        if ( numGreater1Flag < 8 ) {	
          VO_U32 code;
          //                     if ( k == 0 ) {
          //                         ctx_set = ( i > 0 && cIdx == 0 ) ? 2 : 0;
          //                         if ( ( i != lastSubBlock ) && Greater1Ctx == 0 )
          //                             ctx_set++;
          //                         Greater1Ctx = 1;
          //                     }
          code = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_ONE_FLAG_CTX + ( ctx_set << 2 ) + Greater1Ctx + ( cIdx == 0 ? 0 : 16 ) ); // coeff_abs_level_greater1_flag
          if ( code ) {
            Greater1Ctx = 0;
          } else if ( Greater1Ctx > 0 && Greater1Ctx < 3 ) {
            Greater1Ctx++;
          }
          numGreater1Flag++;	
          if ( code  &&  ( lastGreater1ScanPos == -1 ) )	//coeff_abs_level_greater1_flag[ n ]
            lastGreater1ScanPos = n	;
          coeff_abs_level_greater1 |= ( code << n );
        }
        //if ( lastSigScanPos == -1 )	
        //	lastSigScanPos = n;	
        //firstSigScanPos = n	;
      }
      //if(nb_sig_coeff) 
      {
        signHidden = ( sig_coeff_pos[ 0 ] - sig_coeff_pos[ nb_sig_coeff - 1 ] > 3  &&  !p_slice->cu_transquant_bypass_flag );	
        if ( lastGreater1ScanPos  !=  -1 )	
          coeff_abs_level_greater2 = \
          ( voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_ABS_FLAG_CTX + ctx_set + ( ( cIdx == 0 ) ? 0 : 4 ) ) ) << lastGreater1ScanPos ;

        if ( !p_pps->sign_data_hiding_enabled_flag || !signHidden || ( n != sig_coeff_pos[ nb_sig_coeff - 1 ] ) ) {
          coeff_sign = voCabacDecodeBinsEP( p_slice->pCabac, /*&p_slice->bs,*/ nb_sig_coeff );
          coeff_sign <<= 32 - (nb_sig_coeff);        
          //sig_idx = nb_sig_coeff - 1;
        } else {
          coeff_sign = voCabacDecodeBinsEP( p_slice->pCabac, /*&p_slice->bs,*/ nb_sig_coeff - 1 );
          coeff_sign <<= 32 - (nb_sig_coeff - 1);        
          //sig_idx = nb_sig_coeff - 2;
        }
      }

      //if ( numGreater1Flag == 0 || nb_sig_coeff > C1FLAG_NUMBER ) {
      {
        VO_S32 baseLevel;
        VO_U32 xC;
        VO_U32 yC;                
        VO_S16 TransCoeffLevel_value;
        VO_U32 xC0 = ( xS << 2 );
        VO_U32 yC0 = ( yS << 2 );                
        for ( k = 0; k < (nb_sig_coeff-1); k++ ) {
          n = sig_coeff_pos[ k ];
          xC = xC0 + ( scan_x_off[ n ] );
          yC = yC0 + ( scan_y_off[ n ] );

          baseLevel = 1 + ( ( coeff_abs_level_greater1 & ( 1 << n ) ) >> n ) + \
            ( ( coeff_abs_level_greater2 & ( 1 << n ) ) >> n );	
          if ( baseLevel  ==  ( ( numSigCoeff < 8 ) ? \
            ( (n  ==  lastGreater1ScanPos) ? 3 : 2 ) : 1 ) ) {	
              baseLevel += xReadCoefRemainExGolomb( p_slice->pCabac, /*&p_slice->bs,*/ uiGoRiceParam );

              if ( baseLevel > ( 3 * ( 1 << uiGoRiceParam ) ) ) {//YU_TBD: check spec
                uiGoRiceParam = MIN( 4, uiGoRiceParam + 1);
              }
          }
          TransCoeffLevel_value = ( coeff_sign >> 31 ) ? - baseLevel : baseLevel;
          coeff_sign <<= 1;
          sumAbsLevel  +=  baseLevel;
          if (!p_slice->cu_transquant_bypass_flag) {
            if (!transform_skip_flag){
              TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = Clip3(-32768, 32767, ((TransCoeffLevel_value *p_m[xC + (yC << log2TrafoSize)] << scale)
                + (1 << (bdShift - 1))) >> bdShift);
            } else {
#if DEQUANT_LOG
              VO_S16 s16Coeff = Clip3(-32768, 32767, ((TransCoeffLevel_value *p_m[xC + (yC << log2TrafoSize)] << scale)
                + (1 << (bdShift - 1))) >> bdShift);
              TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = s16Coeff;  // for checking error
#endif
              // 						p_pred_buf[xC + yC * pred_stride] = Clip(p_pred_buf[xC + yC * pred_stride] + 
              // 							((s16Coeff  + (1 << (transform_skip_shift -1))) >> transform_skip_shift));
              //p_pred_buf[xC + yC * pred_stride] = Clip(p_pred_buf[xC + yC * pred_stride] +  (((TransCoeffLevel_value *p_m[xC + (yC << log2TrafoSize)] << scale)
              //  + (1 << 9)) >> 10 ));
              TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = (((TransCoeffLevel_value *p_m[xC + (yC << log2TrafoSize)] << scale)
                + (1 << 9)) >> 10 );
            }
          } else{
            //p_pred_buf[xC + yC * pred_stride] = Clip(p_pred_buf[xC + yC * pred_stride] + TransCoeffLevel_value);
			TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = TransCoeffLevel_value;
          }

          numSigCoeff++;
        }	
        //if (nb_sig_coeff >0)
        {	
          n = sig_coeff_pos[ k ];
          xC = xC0 + ( scan_x_off[ n ] );
          yC = yC0 + ( scan_y_off[ n ] );
          baseLevel = 1 + ( ( coeff_abs_level_greater1 & ( 1 << n ) ) >> n ) + \
            ( ( coeff_abs_level_greater2 & ( 1 << n ) ) >> n );	
          if ( baseLevel  ==  ( ( numSigCoeff < 8 ) ? \
            ( (n  ==  lastGreater1ScanPos) ? 3 : 2 ) : 1 ) ) {	
              baseLevel += xReadCoefRemainExGolomb( p_slice->pCabac, /*&p_slice->bs,*/ uiGoRiceParam );
          }
          TransCoeffLevel_value = ( coeff_sign >> 31 ) ? - baseLevel : baseLevel;

          if ( p_pps->sign_data_hiding_enabled_flag  &&  signHidden ) {	
            sumAbsLevel  +=  baseLevel;
            if (/* ( k == ( nb_sig_coeff - 1 ) ) && */( ( sumAbsLevel & 1 ) == 1 ) )	//TransCoeffLevel[ x0 ][ y0 ][ cIdx ][ xC ][ yC ]
              TransCoeffLevel_value = -TransCoeffLevel_value;
          }	
          //TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = TransCoeffLevel_value;
          if (!p_slice->cu_transquant_bypass_flag) {
            if (!transform_skip_flag){
              TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = Clip3(-32768, 32767, ((TransCoeffLevel_value *p_m[xC + (yC << log2TrafoSize)] << scale)
                + (1 << (bdShift - 1))) >> bdShift);
            } else {
#if DEQUANT_LOG
              VO_S16 s16Coeff = Clip3(-32768, 32767, ((TransCoeffLevel_value *p_m[xC + (yC << log2TrafoSize)] << scale)
                + (1 << (bdShift - 1))) >> bdShift);
              TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = s16Coeff;  // for checking error
#endif
              //p_pred_buf[xC + yC * pred_stride] = Clip(p_pred_buf[xC + yC * pred_stride] +  (((TransCoeffLevel_value *p_m[xC + (yC << log2TrafoSize)] << scale)
              //  + (1 << 9)) >> 10 ));
              TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = (((TransCoeffLevel_value *p_m[xC + (yC << log2TrafoSize)] << scale)
                + (1 << 9)) >> 10 );
            }
          } else{
            //p_pred_buf[xC + yC * pred_stride] = Clip(p_pred_buf[xC + yC * pred_stride] + TransCoeffLevel_value);
			TransCoeffLevel[ xC + (yC << log2TrafoSize) ] = TransCoeffLevel_value;
          }
          //numSigCoeff++;
        }	
      }


    }
  }

#if INTRA_LOG
  if(!transform_skip_flag&&p_slice->CuPredMode == MODE_INTRA){ //no pred data in V2, pred  = recon ; only intra buffer
    printReconInput(cIdx, p_slice->cur_pic->pic_buf[cIdx], pred_stride, x0, y0, TransCoeffLevel, log2TrafoSize, qp,  transform_skip_flag, p_slice->cu_transquant_bypass_flag);
  } 
#endif
#if DEQUANT_LOG
  printDeQuantOut(cIdx, qp, TransCoeffLevel, 1<<log2TrafoSize, 1<<log2TrafoSize, 1<<log2TrafoSize, x0, y0);
#endif


  {
    VO_S32 function_index;
    VO_U32 width = (1 << log2TrafoSize);
    VO_U32 trTpye  = (p_slice->CuPredMode == MODE_INTRA&&cIdx == 0 && log2TrafoSize==2) ? 4:0;
    //VO_S16  tmp_block_array[32 * 32];
    //VO_S16*  tmp_block_array = p_slice->asm_mc0_buff;

    //Do IDCT
    if (p_slice->cu_transquant_bypass_flag||transform_skip_flag)
		function_index = 23;
	else
	{
        /*function_index = width >> 2;
    // when TU is 4*4 size and mode != REG_DCT, it will call DST
        if (function_index == 1) {
          function_index = trTpye ? (12 - non_zero_block_flag) : (function_index - non_zero_block_flag);

        } else {
          function_index += non_zero_block_flag;
        }*/
        function_index = idct_function_index[log2TrafoSize+trTpye]+non_zero_block_flag; 
	}
    //tmp_block_array[0] =(VO_S16) width;
    //printf("function_index = [%d], width = [%d].\n",function_index, width);

    // call the real IDCT
    //(IDCT_function_pointer_array[function_index])(TransCoeffLevel,
    //  p_pred_buf,p_pred_buf,pred_stride,pred_stride,tmp_block_array);
    //printf("done.\n");
    tu_task->trans_flag = 1;
	tu_task->function_index = function_index;
	tu_task->width = width;
	tu_task->pred_stride = pred_stride;
	tu_task->TransCoeffLevel = TransCoeffLevel;
	tu_task->p_pred_buf = p_pred_buf;
	p_slice->cur_rctask->total_tu++;
  } //end if (flag_skip_transform)-else   

#if RECON_LOG
  if(p_slice->CuPredMode == MODE_INTRA){  //very difficult to get recon buffer in TU unit in HM
    printResidualOutput(cIdx, p_pred_buf, log2TrafoSize, pred_stride);
  }  
#endif

  return VO_ERR_NONE;
}	
#if CHROMA_QP_EXTENSION
const VO_U8 g_aucChromaScale[58]=
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,
	17,18,19,20,21,22,23,24,25,26,27,28,29,29,30,31,32,
	33,33,34,34,35,35,36,36,37,37,38,39,40,41,42,43,44,
	45,46,47,48,49,50,51
};
#else
const VO_U8 g_aucChromaScale[52]=
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,
	12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
	28,29,29,30,31,32,32,33,34,34,35,35,36,36,37,37,
	37,38,38,38,39,39,39,39
};
#endif
VOINLINE VO_VOID set_qp(H265_DEC_SLICE *p_slice,VO_S32 qp)
{
	//VO_S32 iQPBDOffset = (p_slice->p_sps->bit_depth_chroma - 8) * 6; //0
	VO_S32 tmp_qp;
    VO_S32 *pDequant_qp_buff = p_slice->entries[p_slice->currEntryNum].dequant_qp;
	pDequant_qp_buff[0] = qp;
	tmp_qp = Clip3(0, 57, qp + p_slice->p_pps->pps_chroma_qp_offset[0] );
	pDequant_qp_buff[1] = g_aucChromaScale[ tmp_qp ] ;
	tmp_qp = Clip3(0, 57, qp + p_slice->p_pps->pps_chroma_qp_offset[1] );
	pDequant_qp_buff[2] = g_aucChromaScale[ tmp_qp ];
}


static VO_VOID StoreTuDeblockPara( H265_DEC_SLICE* p_slice,
	const VO_U32 x0, 
	const VO_U32 y0, 
	const VO_U32 log2TrafoSize )
{
	VO_U32 PicWidthInMinTbsY = p_slice->PicWidthInMinTbsY;
	VO_U32* deblock_para = p_slice->deblock_para  + ( y0 >> 2 ) * PicWidthInMinTbsY + ( x0 >> 2 );
	VO_U32 i, j, tu_in_store_size = 1 << ( log2TrafoSize - 2 );
	//store 4 para
	VO_U32 tu_deblock_para = ( p_slice->pcm_flag << 24 ) | (   p_slice->entries[p_slice->currEntryNum].dequant_qp[ 0 ] << 16 ) | 
		( p_slice->cbf_luma << 8 ) | p_slice->CuPredMode;

	for ( j = 0;j < tu_in_store_size; j++ ) {
		for ( i = 0; i < tu_in_store_size; i++ ) {
			*deblock_para++ = tu_deblock_para;
		}
		deblock_para += PicWidthInMinTbsY - tu_in_store_size;
	}
}

//Is there any way to remove this function?
static VO_VOID StoreTuDeblockParaOnlyQP( H265_DEC_SLICE* p_slice,
  const VO_U32 x0, 
  const VO_U32 y0, 
  const VO_U32 log2TrafoSize )
{
  VO_U32 PicWidthInMinTbsY = p_slice->PicWidthInMinTbsY;
  VO_U32* deblock_para = p_slice->deblock_para  + ( y0 >> 2 ) * PicWidthInMinTbsY + ( x0 >> 2 );
  VO_U32 i, j, tu_in_store_size = 1 << ( log2TrafoSize - 2 );  
  VO_U32 tu_deblock_para = 0;

  if( p_slice->entries[p_slice->currEntryNum].dequant_qp[ 0 ] == ((*deblock_para) & 0x00ff0000) >> 16) {//dequant_qp is not changed during TU process
    return;
  }

  for ( j = 0;j < tu_in_store_size; j++ ) {
    for ( i = 0; i < tu_in_store_size; i++ ) {
      tu_deblock_para = *deblock_para;
      tu_deblock_para &= 0xff00ffff;
      tu_deblock_para |=  p_slice->entries[p_slice->currEntryNum].dequant_qp[ 0 ] << 16;
      *deblock_para++ = tu_deblock_para;
    }
    deblock_para += PicWidthInMinTbsY - tu_in_store_size;
  }
}

static VO_S32 transform_unit( H265_DEC_SLICE* p_slice,
	const VO_U32 x0, 
	const VO_U32 y0, 
	const VO_U32 xBase, 
	const VO_U32 yBase, 
	const VO_U32 log2TrafoSize, 
	const VO_U32 trafoDepth, 
	const VO_U32 blkIdx,
	const VO_U32 MinTbAddr, 
	const VO_U32 BaseMinTbAddr,
	const VO_U32 cux,
	const VO_U32 cuy) 
{
	H265_DEC_SPS *p_sps = p_slice->p_sps;
	H265_DEC_PPS *p_pps = p_slice->p_pps;
    VO_U32 available_flag;
    TUTask* tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
	//YU_TBD: Check all [ x0 ][ y0 ]
	if ( p_slice->cbf_luma ||  p_slice->cbf_cb[ trafoDepth ][ MinTbAddr ] ||  
		p_slice->cbf_cr[ trafoDepth ][ MinTbAddr ] ) { //cbf_luma[ y0 * 16 + x0 ][ trafoDepth ]
			/*delta qp*/
			if ( p_pps->cu_qp_delta_enabled_flag && !p_slice->IsCuQpDeltaCoded ) {	
				VO_S32 CuQpDeltaVal	= xReadUnaryMaxSymbol( p_slice->pCabac, /*&p_slice->bs,*/ OFF_DELTA_QP_CTX, 1, CU_DQP_TU_CMAX );
        VO_S32 uiSymbol;
        if( CuQpDeltaVal >= CU_DQP_TU_CMAX)
        {
          uiSymbol = xReadEpExGolomb( p_slice->pCabac, CU_DQP_EG_k);         
          CuQpDeltaVal+=uiSymbol;
        }

				if ( CuQpDeltaVal )	{         
					VO_U32 cu_qp_delta_sign_flag = voCabacDecodeBinEP( p_slice->pCabac/*, &p_slice->bs*/);
					CuQpDeltaVal = CuQpDeltaVal * ( 1 - 2 * cu_qp_delta_sign_flag );      
          set_qp(p_slice,  (p_slice->entries[p_slice->currEntryNum].dequant_qp[0]+CuQpDeltaVal+52)%52); //spec:  8-258
				} 
        p_slice->IsCuQpDeltaCoded = 1; //(7 60)    
				//VOASSERT(0);				
			}	
	}
	if(p_slice->CuPredMode == MODE_INTRA) {
#define MIN_TB_ADDR_ZS( x, y )  ( *( MinTbAddrZs + ((VO_S32)(( y ) * PicWidthInMinTbsYAdd2)) + ((VO_S32)( x ) )) )
        VO_S32 *MinTbAddrZs = p_slice->p_pps->MinTbAddrZs;
        VO_U32 PicWidthInMinTbsYAdd2 = p_slice->p_pps->MinTbAddrZsStride;
        VO_S32 xMinTbY = x0 >> 2;//p_slice->MinTbLog2SizeY;
        VO_S32 yMinTbY = y0 >> 2;//p_slice->MinTbLog2SizeY;
        VO_S32 TbSYInMinTb = ( 1 << log2TrafoSize ) >> 2;//p_slice->MinTbLog2SizeY;
        VO_S32 start_zs = p_slice->p_pps->tile_start_zs[p_slice->currEntryNum];
        
        VO_S32 CurMinTbAddrY = MIN_TB_ADDR_ZS( xMinTbY, yMinTbY );
        VO_S32 BlMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY - 1, yMinTbY + TbSYInMinTb );
        VO_S32 TrMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY + TbSYInMinTb, yMinTbY - 1 );
        VO_S32 LeftMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY - 1, yMinTbY );
        VO_S32 TopMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY, yMinTbY - 1 );
        VO_S32 TopLeftMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY - 1, yMinTbY - 1 );
        //a, in tile: x_nb_tb >= x_start_tb && y_nb_tb <= y_end_tb
        //b, in slice : zscan cur_mintb >= slice_start
        //c, in same lcu, zscan cur_mintb > nb_mintb
        VO_U32 bl_avail = /*(xMinTbY > 0 ) &&*/( BlMinTbAddrY >= start_zs ) && 
            ( CurMinTbAddrY > BlMinTbAddrY ) &&
            ( BlMinTbAddrY >= p_slice->SliceMinTbAddrZs );
        VO_U32 l_avail  = /*(xMinTbY > 0) &&*/ ( LeftMinTbAddrY >= start_zs ) && ( LeftMinTbAddrY >= p_slice->SliceMinTbAddrZs );
        VO_U32 tl_avail =/* (xMinTbY > 0) && (yMinTbY > 0 )&&*/( TopLeftMinTbAddrY >= start_zs ) && (TopLeftMinTbAddrY >= p_slice->SliceMinTbAddrZs );
        VO_U32 t_avail  = /*(yMinTbY > 0 )&&*/ ( TopMinTbAddrY >= start_zs ) && ( TopMinTbAddrY >= p_slice->SliceMinTbAddrZs );
        VO_U32 tr_avail = /*(yMinTbY > 0 ) &&*/( TrMinTbAddrY >= start_zs ) &&
            ( CurMinTbAddrY > TrMinTbAddrY ) && 
            ( TrMinTbAddrY >=  p_slice->SliceMinTbAddrZs );
        available_flag = bl_avail;
        available_flag |= l_avail << 1;
        available_flag |= tl_avail << 2;
        available_flag |= t_avail << 3;
        available_flag |= tr_avail << 4;
		//H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize, p_slice->IntraPredMode_Y, 0, available_flag);  
		tu_task->intra_flag = 1;
		tu_task->x0 = x0;
		tu_task->y0 = y0;
		tu_task->log2TrafoSize = log2TrafoSize;
		tu_task->IntraPredMode = p_slice->IntraPredMode_Y;
		tu_task->cIdx = 0;
		tu_task->available_flag = available_flag;
	}

  if(!p_sps->scaling_list_enabled_flag){
    if ( p_slice->cbf_luma ) { //cbf_luma[ x0 ][ y0 ][ trafoDepth ]
      residual_coding( p_slice, x0, y0, log2TrafoSize, 0 );//Y
    }
	else if(tu_task->intra_flag)
	{
	  p_slice->cur_rctask->total_tu++;
	}

    if ( log2TrafoSize > 2 ) {	
	  tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
      if(p_slice->CuPredMode == MODE_INTRA) {        
        //H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize - 1, p_slice->IntraPredModeC, 1, available_flag); 
		tu_task->intra_flag = 1;
		tu_task->x0 = x0;
		tu_task->y0 = y0;
		tu_task->log2TrafoSize = log2TrafoSize-1;
		tu_task->IntraPredMode = p_slice->IntraPredModeC;
		tu_task->cIdx = 1;
		tu_task->available_flag = available_flag;
      }       
      if ( p_slice->cbf_cb[ trafoDepth ][ MinTbAddr ] )
        residual_coding( p_slice, x0 >> 1, y0 >> 1, log2TrafoSize - 1, 1 );//U
      else if(tu_task->intra_flag)
	    p_slice->cur_rctask->total_tu++;

      tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
      if(p_slice->CuPredMode == MODE_INTRA) {      
        //H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize - 1, p_slice->IntraPredModeC, 2, available_flag);
		tu_task->intra_flag = 1;
		tu_task->x0 = x0;
		tu_task->y0 = y0;
		tu_task->log2TrafoSize = log2TrafoSize-1;
		tu_task->IntraPredMode = p_slice->IntraPredModeC;
		tu_task->cIdx = 2;
		tu_task->available_flag = available_flag;
      }
      if ( p_slice->cbf_cr[ trafoDepth ][ MinTbAddr ] )	
        residual_coding( p_slice, x0 >> 1, y0 >> 1, log2TrafoSize - 1, 2 );//V
      else if(tu_task->intra_flag)
	    p_slice->cur_rctask->total_tu++;
    } else if ( blkIdx  ==  3 ) {	// reach tu node
      if(p_slice->CuPredMode == MODE_INTRA) {
#define MIN_TB_ADDR_ZS( x, y )  ( *( MinTbAddrZs + ((VO_S32)(( y ) * PicWidthInMinTbsYAdd2)) + ((VO_S32)( x ) )) )
          VO_S32 *MinTbAddrZs = p_slice->p_pps->MinTbAddrZs;
          VO_U32 PicWidthInMinTbsYAdd2 = p_slice->p_pps->MinTbAddrZsStride;
          VO_S32 xMinTbY = xBase >> 2;//p_slice->MinTbLog2SizeY;
          VO_S32 yMinTbY = yBase >> 2;//p_slice->MinTbLog2SizeY;
          VO_S32 TbSYInMinTb = ( 8 ) >> 2;//p_slice->MinTbLog2SizeY;
          VO_S32 start_zs = p_slice->p_pps->tile_start_zs[p_slice->currEntryNum];          
          VO_S32 CurMinTbAddrY = MIN_TB_ADDR_ZS( xMinTbY, yMinTbY );
          VO_S32 BlMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY - 1, yMinTbY + TbSYInMinTb );
          VO_S32 TrMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY + TbSYInMinTb, yMinTbY - 1 );
          VO_S32 LeftMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY - 1, yMinTbY );
          VO_S32 TopMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY, yMinTbY - 1 );
          VO_S32 TopLeftMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY - 1, yMinTbY - 1 );
          //a, in tile: x_nb_tb >= x_start_tb && y_nb_tb <= y_end_tb
          //b, in slice : zscan cur_mintb >= slice_start
          //c, in same lcu, zscan cur_mintb > nb_mintb
          VO_U32 bl_avail = /*(xMinTbY > 0 ) &&*/( BlMinTbAddrY >= start_zs ) && 
              ( CurMinTbAddrY > BlMinTbAddrY ) &&
              ( BlMinTbAddrY >= p_slice->SliceMinTbAddrZs );
          VO_U32 l_avail  = /*(xMinTbY > 0) &&*/ ( LeftMinTbAddrY >= start_zs ) && ( LeftMinTbAddrY >= p_slice->SliceMinTbAddrZs );
          VO_U32 tl_avail =/* (xMinTbY > 0) && (yMinTbY > 0 )&&*/( TopLeftMinTbAddrY >= start_zs ) && (TopLeftMinTbAddrY >= p_slice->SliceMinTbAddrZs );
          VO_U32 t_avail  = /*(yMinTbY > 0 )&&*/ ( TopMinTbAddrY >= start_zs ) && ( TopMinTbAddrY >= p_slice->SliceMinTbAddrZs );
          VO_U32 tr_avail = /*(yMinTbY > 0 ) &&*/( TrMinTbAddrY >= start_zs ) &&
              ( CurMinTbAddrY > TrMinTbAddrY ) && 
              ( TrMinTbAddrY >=  p_slice->SliceMinTbAddrZs );
          available_flag = bl_avail;
          available_flag |= l_avail << 1;
          available_flag |= tl_avail << 2;
          available_flag |= t_avail << 3;
          available_flag |= tr_avail << 4;
		  tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
          //H265_IntraPrediction(p_slice, xBase, yBase, log2TrafoSize, p_slice->IntraPredModeC, 1, available_flag);
		  tu_task->intra_flag = 1;
		  tu_task->x0 = xBase;
		  tu_task->y0 = yBase;
		  tu_task->log2TrafoSize = log2TrafoSize;
		  tu_task->IntraPredMode = p_slice->IntraPredModeC;
		  tu_task->cIdx = 1;
		  tu_task->available_flag = available_flag;
      }
      if ( p_slice->cbf_cb[ trafoDepth ][ BaseMinTbAddr ] )	
        residual_coding( p_slice, xBase >> 1, yBase >> 1, log2TrafoSize, 1 );	//U
      else if(tu_task->intra_flag)
	    p_slice->cur_rctask->total_tu++;
      tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
      if(p_slice->CuPredMode == MODE_INTRA) {
        //H265_IntraPrediction(p_slice, xBase, yBase, log2TrafoSize, p_slice->IntraPredModeC, 2, available_flag);
		tu_task->intra_flag = 1;
		tu_task->x0 = xBase;
		tu_task->y0 = yBase;
		tu_task->log2TrafoSize = log2TrafoSize;
		tu_task->IntraPredMode = p_slice->IntraPredModeC;
		tu_task->cIdx = 2;
		tu_task->available_flag = available_flag;
      }
      if ( p_slice->cbf_cr[ trafoDepth ][ BaseMinTbAddr ] )	
        residual_coding( p_slice, xBase >> 1, yBase >> 1, log2TrafoSize, 2 );	//V
      else if(tu_task->intra_flag)
	    p_slice->cur_rctask->total_tu++;
    }	

  }else{
    //scaling_list_enabled_flag case
    if ( p_slice->cbf_luma ) { //cbf_luma[ x0 ][ y0 ][ trafoDepth ]
      residual_coding_scaling_list( p_slice, x0, y0, log2TrafoSize, 0 );//Y
    }
    else if(tu_task->intra_flag)
	  p_slice->cur_rctask->total_tu++;
    if ( log2TrafoSize > 2 ) {	
	  tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
      if(p_slice->CuPredMode == MODE_INTRA) {        
        //H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize - 1, p_slice->IntraPredModeC, 1, available_flag);
		tu_task->intra_flag = 1;
		tu_task->x0 = x0;
		tu_task->y0 = y0;
		tu_task->log2TrafoSize = log2TrafoSize-1;
		tu_task->IntraPredMode = p_slice->IntraPredModeC;
		tu_task->cIdx = 1;
		tu_task->available_flag = available_flag;
      }       
      if ( p_slice->cbf_cb[ trafoDepth ][ MinTbAddr ] )
        residual_coding_scaling_list( p_slice, x0 >> 1, y0 >> 1, log2TrafoSize - 1, 1 );//U
      else if(tu_task->intra_flag)
	    p_slice->cur_rctask->total_tu++;
      tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
      if(p_slice->CuPredMode == MODE_INTRA) {      
        //H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize - 1, p_slice->IntraPredModeC, 2, available_flag);
		tu_task->intra_flag = 1;
		tu_task->x0 = x0;
		tu_task->y0 = y0;
		tu_task->log2TrafoSize = log2TrafoSize-1;
		tu_task->IntraPredMode = p_slice->IntraPredModeC;
		tu_task->cIdx = 2;
		tu_task->available_flag = available_flag;
      }
      if ( p_slice->cbf_cr[ trafoDepth ][ MinTbAddr ] )	
        residual_coding_scaling_list( p_slice, x0 >> 1, y0 >> 1, log2TrafoSize - 1, 2 );//V
      else if(tu_task->intra_flag)
	    p_slice->cur_rctask->total_tu++;
    } else if ( blkIdx  ==  3 ) {	// reach tu node
      if(p_slice->CuPredMode == MODE_INTRA) {
#define MIN_TB_ADDR_ZS( x, y )  ( *( MinTbAddrZs + ((VO_S32)(( y ) * PicWidthInMinTbsYAdd2)) + ((VO_S32)( x ) )) )
          VO_S32 *MinTbAddrZs = p_slice->p_pps->MinTbAddrZs;
          VO_U32 PicWidthInMinTbsYAdd2 = p_slice->p_pps->MinTbAddrZsStride;
          VO_S32 xMinTbY = xBase >> 2;//p_slice->MinTbLog2SizeY;
          VO_S32 yMinTbY = yBase >>2; // p_slice->MinTbLog2SizeY;
          VO_S32 TbSYInMinTb = ( 8 ) >> 2;// p_slice->MinTbLog2SizeY;
          VO_S32 start_zs = p_slice->p_pps->tile_start_zs[p_slice->currEntryNum];          
          VO_S32 CurMinTbAddrY = MIN_TB_ADDR_ZS( xMinTbY, yMinTbY );
          VO_S32 BlMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY - 1, yMinTbY + TbSYInMinTb );
          VO_S32 TrMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY + TbSYInMinTb, yMinTbY - 1 );
          VO_S32 LeftMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY - 1, yMinTbY );
          VO_S32 TopMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY, yMinTbY - 1 );
          VO_S32 TopLeftMinTbAddrY  = MIN_TB_ADDR_ZS( xMinTbY - 1, yMinTbY - 1 );
          //a, in tile: x_nb_tb >= x_start_tb && y_nb_tb <= y_end_tb
          //b, in slice : zscan cur_mintb >= slice_start
          //c, in same lcu, zscan cur_mintb > nb_mintb
          VO_U32 bl_avail = /*(xMinTbY > 0 ) &&*/( BlMinTbAddrY >= start_zs ) && 
              ( CurMinTbAddrY > BlMinTbAddrY ) &&
              ( BlMinTbAddrY >= p_slice->SliceMinTbAddrZs );
          VO_U32 l_avail  = /*(xMinTbY > 0) &&*/ ( LeftMinTbAddrY >= start_zs ) && ( LeftMinTbAddrY >= p_slice->SliceMinTbAddrZs );
          VO_U32 tl_avail =/* (xMinTbY > 0) && (yMinTbY > 0 )&&*/( TopLeftMinTbAddrY >= start_zs ) && (TopLeftMinTbAddrY >= p_slice->SliceMinTbAddrZs );
          VO_U32 t_avail  = /*(yMinTbY > 0 )&&*/ ( TopMinTbAddrY >= start_zs ) && ( TopMinTbAddrY >= p_slice->SliceMinTbAddrZs );
          VO_U32 tr_avail = /*(yMinTbY > 0 ) &&*/( TrMinTbAddrY >= start_zs ) &&
              ( CurMinTbAddrY > TrMinTbAddrY ) && 
              ( TrMinTbAddrY >=  p_slice->SliceMinTbAddrZs );
          available_flag = bl_avail;
          available_flag |= l_avail << 1;
          available_flag |= tl_avail << 2;
          available_flag |= t_avail << 3;
          available_flag |= tr_avail << 4;
		  tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
          //H265_IntraPrediction(p_slice, xBase, yBase, log2TrafoSize, p_slice->IntraPredModeC, 1, available_flag);
		  tu_task->intra_flag = 1;
		  tu_task->x0 = xBase;
		  tu_task->y0 = yBase;
		  tu_task->log2TrafoSize = log2TrafoSize;
		  tu_task->IntraPredMode = p_slice->IntraPredModeC;
		  tu_task->cIdx = 1;
		  tu_task->available_flag = available_flag;
      }
      if ( p_slice->cbf_cb[ trafoDepth ][ BaseMinTbAddr ] )	
        residual_coding_scaling_list( p_slice, xBase >> 1, yBase >> 1, log2TrafoSize, 1 );	//U
      else if(tu_task->intra_flag)
	    p_slice->cur_rctask->total_tu++;
      tu_task = &p_slice->cur_rctask->tu_task[p_slice->cur_rctask->total_tu];
      if(p_slice->CuPredMode == MODE_INTRA) {
        //H265_IntraPrediction(p_slice, xBase, yBase, log2TrafoSize, p_slice->IntraPredModeC, 2, available_flag);
		tu_task->intra_flag = 1;
		tu_task->x0 = xBase;
		tu_task->y0 = yBase;
		tu_task->log2TrafoSize = log2TrafoSize;
		tu_task->IntraPredMode = p_slice->IntraPredModeC;
		tu_task->cIdx = 2;
		tu_task->available_flag = available_flag;
      }
      if ( p_slice->cbf_cr[ trafoDepth ][ BaseMinTbAddr ] )	
        residual_coding_scaling_list( p_slice, xBase >> 1, yBase >> 1, log2TrafoSize, 2 );	//V
      else if(tu_task->intra_flag)
	    p_slice->cur_rctask->total_tu++;
    }	
  }
	
	// 	}	else {
	//     if(p_slice->CuPredMode == MODE_INTRA) {
	//       H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize, p_slice->IntraPredMode_Y, 0);
	//       if( log2TrafoSize > 2 ) {
	//         H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize - 1, p_slice->IntraPredModeC, 1);
	//         H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize - 1, p_slice->IntraPredModeC, 2);
	//       } else {
	//         if (blkIdx == 3) {
	//           H265_IntraPrediction(p_slice, xBase, yBase, log2TrafoSize, p_slice->IntraPredModeC, 1);
	//           H265_IntraPrediction(p_slice, xBase, yBase, log2TrafoSize, p_slice->IntraPredModeC, 2);
	//         }
	//       }
	//     }
	//   } 

	//-----------TU level storage for deblock
	if (  ( p_slice->fast_mode & VO_FM_DEBLOCK ) == 0 ) { 
		StoreTuDeblockPara( p_slice, x0, y0, log2TrafoSize );
		//LHP add deblock filter
		{
			int tu_in_store_size = 1 << ( log2TrafoSize - 2); //p_slice->MinTbLog2SizeY
			/*if(cux != x0  || cuy != y0)
			 {
				  if(cux == x0 )
			       voSetEdgefilterTU((void*)p_slice, tu_in_store_size,x0>>2,y0>>2, 0, 1);
				  else if(cuy == y0 )
			        voSetEdgefilterTU((void*)p_slice, tu_in_store_size,x0>>2,y0>>2, 1, 0);
				  else
					voSetEdgefilterTU((void*)p_slice, tu_in_store_size,x0>>2,y0>>2, 1, 1);
			 }*/
			if(cux != x0  || cuy != y0)
            {
              VO_U32 x4b = x0>>2;
      		  VO_U32 y4b = y0>>2;
      		  VO_U32 nIndex = y4b*p_slice->PicWidthInMinTbsY+x4b;
              if(cux == x0 )
              {
                if (!(y4b & 0x1))
      		    voSetEdgefilterTUHor((void*)p_slice, tu_in_store_size,nIndex);
              }
              else if(cuy == y0 )
              {
                if (!(x4b & 0x1))
                  voSetEdgefilterTUVer((void*)p_slice, tu_in_store_size,nIndex);
              }
              else
              {
                if (!(x4b & 0x1))
                  voSetEdgefilterTUVer((void*)p_slice, tu_in_store_size,nIndex);
      		  if (!(y4b & 0x1))
      	        voSetEdgefilterTUHor((void*)p_slice, tu_in_store_size,nIndex);
              }
            }
		}
	}
	return VO_ERR_NONE;
}	


/**
* Get mvd
* \param pDecHandle [IN/OUT] Return the H265 Decoder API handle.
* \param uFlag,reserved
* \retval VO_ERR_OK Succeeded.
*/
static VO_S32 mvd_coding( H265_DEC_CABAC *p_cabac,
	/*BIT_STREAM *p_bs,*/
	const VO_U32 x0, 
	const VO_U32 y0, 
	const VO_U32 refList,
	VO_S16 *mv_d )
{	
	VO_S32 abs_mvd_h, abs_mvd_v;

	abs_mvd_h = voCabacDecodeBin( p_cabac, /*p_bs,*/ OFF_MVD_CTX );
	abs_mvd_v = voCabacDecodeBin( p_cabac, /*p_bs,*/ OFF_MVD_CTX );

	if ( abs_mvd_h )
		abs_mvd_h += voCabacDecodeBin( p_cabac, /*p_bs,*/ OFF_MVD_CTX + 1 );
	if ( abs_mvd_v )	
		abs_mvd_v += voCabacDecodeBin( p_cabac, /*p_bs,*/ OFF_MVD_CTX + 1 );

	if ( abs_mvd_h ) {	
		if ( abs_mvd_h == 2 )	
			abs_mvd_h += xReadEpExGolomb( p_cabac, /*p_bs,*/ 1 );

		mv_d[0] = (VO_S16)( voCabacDecodeBinEP( p_cabac/*, p_bs*/ ) ? -abs_mvd_h : abs_mvd_h );
	}
	if( abs_mvd_v ) {	
		if ( abs_mvd_v == 2 )	
			abs_mvd_v += xReadEpExGolomb( p_cabac, /*p_bs,*/ 1 );

		mv_d[1] = (VO_S16)( voCabacDecodeBinEP( p_cabac/*, p_bs*/ ) ? -abs_mvd_v : abs_mvd_v );
	}
	return VO_ERR_NONE;
}	
//VO_U32 motioninfo_num;
static VO_VOID StorePuMotionInfo( H265_DEC_SLICE* p_slice,
	const VO_U32 xPb, 
	const VO_U32 yPb, 
	const VO_U32 nPbW, 
	const VO_U32 nPbH,
	PicMvField *CurMvField )
{
	VO_U32 i, j;
	VO_U32  *SrcCurMvField         = ( VO_U32* )CurMvField;	
	const VO_U32 PicWidthInMinTbsY = p_slice->PicWidthInMinTbsY;
	const VO_U32 puWidthInMinPbs   = nPbW >> 2; //MinTbLog2SizeY;
	const VO_U32 puHeightInMinPbs  = nPbH >> 2; //MinTbLog2SizeY;
	VO_U32 *PbMvField              = ( VO_U32 * )( p_slice->p_motion_field + \
		( yPb >> 2 ) * PicWidthInMinTbsY + ( xPb >> 2 ) );

	//Note: the size of PicMvField is three VO_U32 !!!
	for ( j = 0; j < puHeightInMinPbs; j++ ) {
		for ( i = 0; i < puWidthInMinPbs; i++ ) {
			*( PbMvField++ ) = *SrcCurMvField;
			*( PbMvField++ ) = *(SrcCurMvField + 1 );
			*( PbMvField++ ) = *(SrcCurMvField + 2 );
		}
		PbMvField -= puWidthInMinPbs * 3;
		PbMvField += PicWidthInMinTbsY * 3;
	}

#if MOTION_INFO_LOG
  if(p_slice->slice_type!=I_SLICE){
    printMotionInfo(CurMvField->pred_mode, CurMvField->inter_dir, xPb, yPb, CurMvField->m_iRefIdx[0], CurMvField->m_iRefIdx[1], CurMvField->m_acMv[0], CurMvField->m_acMv[1]); 
  }
  
#endif
}

static VO_S32 prediction_unit( H265_DEC_SLICE* p_slice,
	const VO_U32 xCb,
	const VO_U32 yCb,
	const VO_U32 nCbS,
	const VO_U32 xPb, 
	const VO_U32 yPb, 
	const VO_U32 nPbW, 
	const VO_U32 nPbH,
	const VO_U32 partIdx ) 
{
    PUTask* pu_task = &p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu];
	if ( p_slice->cu_skip_flag ) {/* MODE_SKIP*/ //YU_TBD: check cu_skip_flag[ x0 ][ y0 ]
		PicMvField cMvFieldNeighbours[ MRG_MAX_NUM_CANDS ];
		VO_U32 merge_idx = 0;
		if ( p_slice->max_num_merge_cand > 1 ) {
			merge_idx = parseMergeIndex( p_slice, p_slice->pCabac/*, &p_slice->bs*/ );//merge_idx[ x0 ][ y0 ]
		}
// 		if(motioninfo_num==594)
// 		    motioninfo_num=motioninfo_num;

		GetInterMergeCandidates( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, partIdx, merge_idx ,cMvFieldNeighbours );
		cMvFieldNeighbours[ merge_idx ].pred_mode = MODE_SKIP;

		//mc:YU_TBD: any special for skip mode??
		//H265_MotionCompensation( p_slice, xPb, yPb, nPbW, nPbH, cMvFieldNeighbours + merge_idx ) ;
        pu_task->x0 = xPb;
		pu_task->y0 = yPb;
		pu_task->nPbH = nPbH;
		pu_task->nPbW = nPbW;
		pu_task->pb_mvfield = cMvFieldNeighbours[merge_idx];
		p_slice->cur_rctask->total_pu++;
		//store motion info
		StorePuMotionInfo( p_slice, xPb, yPb, nPbW, nPbH, cMvFieldNeighbours + merge_idx );

		p_slice->merge_flag = 1;//default 7.4.9.6
	} else { /* MODE_INTER */
		p_slice->merge_flag = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_MERGE_FLAG_EXT_CTX );


		if ( p_slice->merge_flag ) {
			PicMvField cMvFieldNeighbours[ MRG_MAX_NUM_CANDS ];
			VO_U32 merge_idx = 0;
			if ( p_slice->max_num_merge_cand > 1 ) {
				merge_idx = parseMergeIndex( p_slice, p_slice->pCabac/*, &p_slice->bs*/ );// merge_idx[ x0 ][ y0 ]
			}
// 			if(motioninfo_num==594)
// 		        motioninfo_num=motioninfo_num;
			//YU_TBD: check log2_parallel_merge_level_minus2
			GetInterMergeCandidates( p_slice, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, partIdx, merge_idx ,cMvFieldNeighbours );

			//YU_TBD: check spec
			if ( cMvFieldNeighbours[ merge_idx ].inter_dir == 3 && nCbS == 8 && ( nPbW < 8 || nPbH < 8 ) ) {
				cMvFieldNeighbours[ merge_idx ].inter_dir      =  1;
				cMvFieldNeighbours[ merge_idx ].m_acMv[ 1 ]    =  0;
				cMvFieldNeighbours[ merge_idx ].m_iRefIdx[ 1 ] = -1;
			}

			cMvFieldNeighbours[ merge_idx ].pred_mode = MODE_INTER;

			//H265_MotionCompensation( p_slice, xPb, yPb, nPbW, nPbH, cMvFieldNeighbours + merge_idx ) ;
			pu_task->x0 = xPb;
		    pu_task->y0 = yPb;
		    pu_task->nPbH = nPbH;
		    pu_task->nPbW = nPbW;
		    pu_task->pb_mvfield = cMvFieldNeighbours[merge_idx];
		    p_slice->cur_rctask->total_pu++;
			//store motion info
			StorePuMotionInfo( p_slice, xPb, yPb, nPbW, nPbH, cMvFieldNeighbours + merge_idx );

		} else {
			PicMvField pb_mvfield;
			//parser inter_pred_idc
			VO_U32 inter_pred_idc = PRED_L0; //default 7.4.96
			VO_U32 ref_list_idx = 0;

			if ( p_slice->slice_type == B_SLICE ) {
				if ( nPbW + nPbH == 12 ) {
					inter_pred_idc = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_INTER_DIR_CTX + 4 ) + 1;
				} else if ( voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_INTER_DIR_CTX + p_slice->depth ) ) {
					inter_pred_idc = PRED_BI;
				} else {
					inter_pred_idc = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_INTER_DIR_CTX + 4 ) + 1;
				}
			}

			for ( ref_list_idx = 0; ref_list_idx < 2; ref_list_idx++ ) {
				if ( inter_pred_idc & ( 1 << ref_list_idx ) ) {//inter_pred_idc[ x0 ][ y0 ]
					VO_S16 Mvd[2] = { 0, 0 };
					VO_U32 mvp_flag, ref_idx = 0;
					VO_S32 m_acMvCand[ AMVP_MAX_NUM_CANDS_MEM ];

// 				   if(motioninfo_num==594)
// 			           motioninfo_num=motioninfo_num;
					//YU_TBD :ref_idx_l0[ x0 ][ y0 ]
					if ( p_slice->num_ref_idx[ ref_list_idx ] > 1 ) // num_ref_idx_l0_active_minus1 > 0
						ref_idx = \
						parseRefFrmIdx( p_slice, p_slice->pCabac, /*&p_slice->bs,*/ p_slice->num_ref_idx[ ref_list_idx ] );

					if ( !( p_slice->mvd_l1_zero_flag  &&  inter_pred_idc == PRED_BI && ref_list_idx == 1 ) )
						mvd_coding( p_slice->pCabac, /*&p_slice->bs,*/ xPb, yPb, ref_list_idx, Mvd );

					mvp_flag = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_MVP_IDX_CTX );//YU_TBD: mvp_l0_flag[ x0 ][ y0 ]
					FillMvpCand( p_slice, ( RefPicList )ref_list_idx, xCb, yCb, nCbS, xPb, yPb, nPbW, nPbH, ref_idx, partIdx, m_acMvCand );

					pb_mvfield.m_iRefIdx[ ref_list_idx ] = ( VO_S8) ref_idx;
					pb_mvfield.m_acMv[ ref_list_idx ] = ( ( ( m_acMvCand[ mvp_flag ] << 16 >> 16 ) + Mvd[ 0 ] ) & 0xFFFF ) | \
						( ( ( m_acMvCand[ mvp_flag ] >> 16 ) + Mvd[ 1 ] ) << 16 );
				} else { //YU_TBD: may not use
					pb_mvfield.m_iRefIdx[ ref_list_idx ] = -1;
					pb_mvfield.m_acMv[ ref_list_idx ] = 0;
				}
			}

			pb_mvfield.inter_dir = (VO_U8)inter_pred_idc;
			pb_mvfield.pred_mode = MODE_INTER;

			//H265_MotionCompensation( p_slice, xPb, yPb, nPbW, nPbH, &pb_mvfield ) ;
			pu_task->x0 = xPb;
		    pu_task->y0 = yPb;
		    pu_task->nPbH = nPbH;
		    pu_task->nPbW = nPbW;
		    pu_task->pb_mvfield = pb_mvfield;
		    p_slice->cur_rctask->total_pu++;
			StorePuMotionInfo( p_slice, xPb, yPb, nPbW, nPbH, &pb_mvfield );
		}

		//----MC

	}
	return VO_ERR_NONE;
}


static VO_U32 transform_tree( H265_DEC_SLICE* p_slice,
	const VO_U32 x0, 
	const VO_U32 y0, 
	const VO_U32 xBase, 
	const VO_U32 yBase, 
	const VO_U32 log2TrafoSize, 
	const VO_U32 trafoDepth, 
	const VO_U32 blkIdx,
	const VO_U32 cux,
	const VO_U32 cuy) 
{	
	H265_DEC_SPS* p_sps = p_slice->p_sps;
	VO_U32 split_transform_flag = 0;

#if 1
	//drive cbf addr
	const VO_U32 CtbLog2SizeYMask = p_slice->CtbLog2SizeYMask;
	const VO_U32 MinTbLog2SizeY   = p_slice->MinTbLog2SizeY;
	const VO_U32 MinTbAddr = ( ( y0 & CtbLog2SizeYMask ) >> MinTbLog2SizeY ) * 16 + \
		( ( x0 & CtbLog2SizeYMask ) >> MinTbLog2SizeY ) ;
	const VO_U32 BaseMinTbAddr = ( ( yBase & CtbLog2SizeYMask ) >> MinTbLog2SizeY ) * 16 + \
		( ( xBase & CtbLog2SizeYMask ) >> MinTbLog2SizeY );
#else
	const VO_U32 MinTbAddr = ( y0 * 16 + x0) ;
	const VO_U32 BaseMinTbAddr = ( yBase * 16 +  xBase );
#endif
	//if (MinTbAddr1 != MinTbAddr || BaseMinTbAddr1 != BaseMinTbAddr )
	//	split_transform_flag=split_transform_flag;
	//printf("\n tu depth=%d, range=%d,value=%d",  trafoDepth, p_slice->cabac.uiRange, p_slice->cabac.uiValue );
	//if ( p_slice->cabac.uiRange == 321 && p_slice->cabac.uiValue == 30020 )
	//	p_slice->cabac.uiValue =p_slice->cabac.uiValue;

	//VO_LOG( LOG_OUTTYPE_ALL,  LOG_PARSER | LOG_INFO_ALL,"\n tu depth=%d, range=%d,value=%d",  \
	//	trafoDepth, p_slice->cabac.uiRange, p_slice->cabac.uiValue );

	/* Prepare for scanIdx */
	if ( p_slice->IntraSplitFlag ) {
		if ( trafoDepth == 1 )
			p_slice->IntraPredMode_Y = p_slice->IntraPredModeY[ blkIdx ];
	} else {
		p_slice->IntraPredMode_Y = p_slice->IntraPredModeY[ 0 ];
	}

	/*split flag*/
	if ( log2TrafoSize <= p_sps->log2_max_transform_block_size &&
		log2TrafoSize > p_sps->log2_min_transform_block_size &&
		trafoDepth < p_slice->MaxTrafoDepth && !( p_slice->IntraSplitFlag && ( trafoDepth  ==  0 ) ) )	{
			split_transform_flag = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_TRANS_SUBDIV_FLAG_CTX + 5 - log2TrafoSize );//YU_TBD: split_transform_flag[ x0 ][ y0 ][ trafoDepth ]
	} else {
		VO_U32 InterSplitFlag = 0;

		if ( p_sps->max_transform_hierarchy_depth_inter == 0 && 
			p_slice->CuPredMode == MODE_INTER && //p_slice->CuPredMode[ x0 ][ y0 ]
			p_slice->CuPartMode != PART_2Nx2N &&
			trafoDepth == 0 ) {
				InterSplitFlag = 1;
		}

		if ( InterSplitFlag || \
			( log2TrafoSize > p_sps->log2_max_transform_block_size ) || \
			( p_slice->IntraSplitFlag && ( trafoDepth == 0 ) ) ) {
				split_transform_flag = 1;
		}
	}

	/*cbf_cb, cbf_cr*/
	//default value
	if ( trafoDepth > 0 && log2TrafoSize == 2 ) {
		p_slice->cbf_cb[ trafoDepth ][ MinTbAddr ] = 
			p_slice->cbf_cb[ trafoDepth - 1 ][ BaseMinTbAddr ];
		p_slice->cbf_cr[ trafoDepth ][ MinTbAddr ] = 
			p_slice->cbf_cr[ trafoDepth - 1 ][ BaseMinTbAddr ];
	} else {
		p_slice->cbf_cb[ trafoDepth ][ MinTbAddr ] = 0;
		p_slice->cbf_cr[ trafoDepth ][ MinTbAddr ] = 0;
	}

	if ( log2TrafoSize > 2 ) {	
		if ( trafoDepth == 0 || ( p_slice->cbf_cb[ trafoDepth - 1 ][ BaseMinTbAddr ] ) )
			p_slice->cbf_cb[ trafoDepth ][ MinTbAddr ] = \
			(VO_U8)voCabacDecodeBin( p_slice->pCabac,/* &p_slice->bs,*/ \
			OFF_QT_CBF_CTX + ( TEXT_CHROMA_U ? TEXT_CHROMA: TEXT_CHROMA_U ) * NUM_QT_CBF_CTX + trafoDepth );
		if ( trafoDepth == 0 || ( p_slice->cbf_cr[ trafoDepth - 1 ][ BaseMinTbAddr ] ) )	
			p_slice->cbf_cr[ trafoDepth ][ MinTbAddr ] = \
			(VO_U8) voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ \
			OFF_QT_CBF_CTX + ( TEXT_CHROMA_V ? TEXT_CHROMA: TEXT_CHROMA_V ) * NUM_QT_CBF_CTX  + trafoDepth );
	}

	if ( split_transform_flag ) {	//YU_TBD: split_transform_flag[ x0 ][ y0 ][ trafoDepth ]
		VO_U32 x1 = x0 + ( 1 << ( log2TrafoSize - 1 ) );
		VO_U32 y1 = y0 + ( 1 << ( log2TrafoSize - 1 ) );

		transform_tree( p_slice, x0, y0, x0, y0, log2TrafoSize - 1, trafoDepth + 1, 0, cux, cuy );	
		transform_tree( p_slice, x1, y0, x0, y0, log2TrafoSize - 1, trafoDepth + 1, 1, cux, cuy );	
		transform_tree( p_slice, x0, y1, x0, y0, log2TrafoSize - 1, trafoDepth + 1, 2, cux, cuy );	
		transform_tree( p_slice, x1, y1, x0, y0, log2TrafoSize - 1, trafoDepth + 1, 3, cux, cuy );	
	} else {	
		/*cbf_y*/
		if ( p_slice->CuPredMode ==  MODE_INTRA || trafoDepth  !=  0 || //CuPredMode[ x0 ][ y0 ]
			( p_slice->cbf_cb[ trafoDepth ][ MinTbAddr ] ) || ( p_slice->cbf_cr[ trafoDepth ][ MinTbAddr ] ) ) {	
				p_slice->cbf_luma = \
					voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_QT_CBF_CTX + ( ( trafoDepth == 0 ? 1 : 0 ) ) );//cbf_luma[ x0 ][ y0 ][ trafoDepth ]
		} else {
			p_slice->cbf_luma = 1;
		}

		//if ( p_slice->CuPredMode == MODE_INTRA ) {
		//	H265_IntraPrediction( p_slice, x0, y0, log2TrafoSize, p_slice->IntraPredMode_Y, 0 );//Y
		//	H265_IntraPrediction( p_slice, x0 >> 1, y0 >> 1, log2TrafoSize, p_slice->IntraPredModeC , 1 );//U
		//	H265_IntraPrediction( p_slice, x0 >> 1, x0 >> 1, log2TrafoSize, p_slice->IntraPredModeC, 2 );//V
		//}
		//     if(p_slice->CuPredMode == MODE_INTRA) {
		//       H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize, p_slice->IntraPredMode_Y, 0);
		//       if( log2TrafoSize > 2 ) {
		//         H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize - 1, p_slice->IntraPredModeC, 1);
		//         H265_IntraPrediction(p_slice, x0, y0, log2TrafoSize - 1, p_slice->IntraPredModeC, 2);
		//       } else {
		//         if (blkIdx == 3) {
		//         H265_IntraPrediction(p_slice, xBase, yBase, log2TrafoSize, p_slice->IntraPredModeC, 1);
		//         H265_IntraPrediction(p_slice, xBase, yBase, log2TrafoSize, p_slice->IntraPredModeC, 2);
		//         }
		//       }
		//     }

		transform_unit( p_slice, x0, y0, xBase, yBase, log2TrafoSize, trafoDepth, blkIdx, MinTbAddr, BaseMinTbAddr, cux, cuy);	
	}	
	return VO_ERR_NONE;
}	


static VO_S32 pcm_sample( H265_DEC_SLICE* p_slice, BIT_STREAM* p_bs, const VO_U32 x0, 
	const VO_U32 y0, 
	const VO_U32 log2CbSize ) 
{
  VO_U32 uiSampleBits,uiX, uiY;
  VO_U8* piPCMSample;
  VO_U32 uiBitDepth = p_slice->p_sps->bit_depth_luma;
  VO_S32 stride = p_slice->cur_pic->pic_stride[0];
  VO_U32 uiPcmLeftShiftBit;
  VO_U32 nPSW , nPSH  ;
  H265_DEC_SPS *p_sps = p_slice->p_sps;
  nPSW = nPSH = (1<<log2CbSize);
   
  piPCMSample = p_slice->cur_pic->pic_buf[0]+y0*stride+x0;
  uiSampleBits = p_sps->pcm_sample_bit_depth_luma_minus1+1;
  uiPcmLeftShiftBit = uiBitDepth  - uiSampleBits;
  for(uiY = 0; uiY < nPSH; uiY++)
  {
    for(uiX = 0; uiX < nPSW; uiX++)
    {
      VO_U32 uiSample;   
      uiSample= GetBits(p_bs,uiSampleBits);
      UPDATE_CACHE(p_bs);
      piPCMSample[uiX] = uiSample<<uiPcmLeftShiftBit;
    }
    piPCMSample += stride;
  }

  stride = p_slice->cur_pic->pic_stride[1];
  uiBitDepth = p_sps->bit_depth_chroma ;
  piPCMSample = p_slice->cur_pic->pic_buf[1]+(y0>>1)*stride+(x0>>1);
  uiSampleBits = p_sps->pcm_sample_bit_depth_chroma_minus1+1;
  uiPcmLeftShiftBit = uiBitDepth  - uiSampleBits;

  for(uiY = 0; uiY < (nPSH>>1); uiY++)
  {
    for(uiX = 0; uiX < (nPSW>>1); uiX++)
    {
      VO_U32 uiSample;
      uiSample= GetBits(p_bs,uiSampleBits);
      UPDATE_CACHE(p_bs);
      piPCMSample[uiX] = uiSample<<uiPcmLeftShiftBit;
    }
    piPCMSample += stride;
  }


  piPCMSample = p_slice->cur_pic->pic_buf[2]+(y0>>1)*stride+(x0>>1);
  for(uiY = 0; uiY < (nPSH>>1); uiY++)
  {
    for(uiX = 0; uiX < (nPSW>>1); uiX++)
    {
      VO_U32 uiSample;
      uiSample= GetBits(p_bs,uiSampleBits);
      UPDATE_CACHE(p_bs);
      piPCMSample[uiX] = uiSample<<uiPcmLeftShiftBit;
    }
    piPCMSample += stride;
  }

  
	return VO_ERR_NONE;
}

//only called at the beginning of minCuQpDeltaSize
VO_S8  getPredQP( H265_DEC_SLICE* p_slice,VO_S32 x_4b,VO_S32 y_4b, VO_U32 minCuQpDeltaSize )
{
 
  VO_S32  tempQPL, tempQPA;
  VO_U32 cULeft;
  VO_U32 cUAbove;
#if 1
  const VO_U32 prev_qp =  p_slice->entries[p_slice->currEntryNum].dequant_qp[0] ;

 /* x_4b &= ~(((1<<(p_slice->CtbLog2SizeY-p_slice->p_pps->diff_cu_qp_delta_depth))>>2)-1);
  y_4b &= ~(((1<<(p_slice->CtbLog2SizeY-p_slice->p_pps->diff_cu_qp_delta_depth))>>2)-1);*/

  cULeft = (x_4b%((1<<p_slice->CtbLog2SizeY)>>2))!=0;
  if (cULeft)
  {
    tempQPL = (p_slice->deblock_para[y_4b*p_slice->PicWidthInMinTbsY+x_4b-1] & 0x00ff0000)>>16;
  }else
  {
    tempQPL = prev_qp;
  }

  cUAbove = (y_4b%((1<<p_slice->CtbLog2SizeY)>>2))!=0;
  if (cUAbove)
  {
    tempQPA = (p_slice->deblock_para[(y_4b-1)*p_slice->PicWidthInMinTbsY+x_4b] & 0x00ff0000)>>16;
  }else
  {
    tempQPA = prev_qp;
  }
#endif
  return (VO_S8)((tempQPL+tempQPA+1)>>1);
}


static VO_S32 clz(VO_S32 in)
{
  int i = 0;
  for(i = 0; i < 31; i++)
    if(in & (1<<i))
      break;
  if(i < 31)
    return i + 1;
  else
    return 32;
}

// VO_U32 intramode_num;
static VO_S32 coding_unit( H265_DEC_SLICE* p_slice,
	const VO_U32 x0, 
	const VO_U32 y0, 
	const VO_U32 log2CbSize ) 
{
	H265_DEC_PPS *p_pps = p_slice->p_pps;
	H265_DEC_SPS *p_sps = p_slice->p_sps;
	BIT_STREAM *p_bs    = &p_slice->cabac->bs;
	VO_U32 nCbS;
	//const VO_U32 cb_size_pixel = 1 << log2CbSize; 
  
	//PartMode CuPartMode;
	//VO_U32 p_slice->pcm_flag = 0; //YU_TBD, need to store for intra pred mode prediction

  if(p_pps->cu_qp_delta_enabled_flag ) {
      VO_U32 minCuQpDeltaSize = 1 << (p_slice->CtbLog2SizeY-p_slice->p_pps->diff_cu_qp_delta_depth);

      if ( x0%minCuQpDeltaSize==0 && y0%minCuQpDeltaSize==0){
        //minCuQpDelta starts 
        set_qp(p_slice, getPredQP(p_slice, x0>>2, y0>>2, minCuQpDeltaSize));
      }
  }

	p_slice->pcm_flag = 0;
	/*transquant_bypass*/
	if ( p_pps->transquant_bypass_enabled_flag )
		p_slice->cu_transquant_bypass_flag = \
		voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs, */OFF_CU_TRANSQUANT_BYPASS_FLAG_CTX );

	/*skip flag*/
	if ( p_slice->slice_type  !=  I_SLICE ) {
		p_slice->cu_skip_flag  = \
			voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_SKIP_FLAG_CTX + GetCtxSkipFlag( p_slice, x0, y0 ) );
	}else{
    p_slice->cu_skip_flag = 0;
  }

	nCbS = ( 1 << log2CbSize );
	if ( p_slice->cu_skip_flag ) {
		p_slice->CuPredMode = MODE_SKIP;
		prediction_unit( p_slice, x0, y0, nCbS, x0, y0, nCbS, nCbS, 0 );

		if ( ( p_slice->fast_mode & VO_FM_DEBLOCK ) == 0) { 
		    p_slice->pcm_flag  = 0;
			p_slice->CuPredMode = MODE_INTER;
			p_slice->cbf_luma = 0;
		    p_slice->CuPartMode  = PART_2Nx2N;

			StoreTuDeblockPara( p_slice, x0, y0, log2CbSize );
			//LHP handle CU
		    voSetEdgefilterCU( ( VO_VOID* )p_slice, nCbS >> 2, x0 >> 2, y0 >> 2, 0 );
		}
		} else {
		p_slice->IntraSplitFlag = 0;

		/*pred mode, inter or intra*/
		if ( p_slice->slice_type  !=  I_SLICE ) {
			p_slice->CuPredMode = \
				( PredMode )voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_PRED_MODE_CTX ); //CuPredMode[ x0 ][ y0 ]
		} else {
			p_slice->CuPredMode = MODE_INTRA;
		}
		/*part mode, pu mode*/
		if ( p_slice->CuPredMode != MODE_INTRA || log2CbSize == p_slice->MinCbLog2SizeY ) { //CuPredMode[ x0 ][ y0 ] 
			PartMode eMode;
			if ( p_slice->CuPredMode == MODE_INTRA ) {
				eMode = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_PART_SIZE_CTX) ? PART_2Nx2N : PART_NxN;
			} else {
				VO_U32 i, mode = 0, MaxNumBits = 2;

				if ( log2CbSize == p_slice->MinCbLog2SizeY && ( log2CbSize !=  3) )
					MaxNumBits++;

				for ( i = 0; i < MaxNumBits; i++ ) {
					if ( voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_PART_SIZE_CTX + i ) )
						break;
					mode++;
				}
				eMode = (PartMode) mode;

				if ( p_sps->amp_enabled_flag && ( log2CbSize != p_slice->MinCbLog2SizeY ) ) { //YU_TBD: check spec
					if ( eMode == PART_2NxN ) {
						if ( voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_CU_AMP_CTX) == 0 ){
							eMode = ( voCabacDecodeBinEP( p_slice->pCabac/*, &p_slice->bs*/ ) == 0 ? PART_2NxnU : PART_2NxnD );
						}
					} else if ( eMode == PART_Nx2N ) {
						if ( voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_CU_AMP_CTX) == 0) {
							eMode = ( voCabacDecodeBinEP( p_slice->pCabac/*, &p_slice->bs*/ ) == 0 ? PART_nLx2N : PART_nRx2N );
						}
					}
				}
			}

			p_slice->CuPartMode = eMode;
		} else {
			p_slice->CuPartMode = PART_2Nx2N;
		}

		if ( p_slice->CuPredMode == MODE_INTRA ) { //CuPredMode[ x0 ][ y0 ]
			/*pcm flag*/
			if ( p_slice->CuPartMode == PART_2Nx2N && p_sps->pcm_enabled_flag  &&  
				log2CbSize >= p_sps->log2_min_pcm_luma_coding_block_size  &&  
				log2CbSize <= p_sps->log2_max_pcm_luma_coding_block_size )
				p_slice->pcm_flag = voCabacDecodeBinTrm( p_slice->pCabac/*, &p_slice->bs */);//pcm_flag[ x0 ][ y0 ] 

			if ( p_slice->pcm_flag ) {//pcm_flag[ x0 ][ y0 ]       
        PicMvField CurMvField;
        VO_S32 bit1_pos;
        p_slice->pcm_flag = 1;				
        bit1_pos = clz(p_slice->pCabac->low_bits);
        bit1_pos = (CABAC_BITS + 1) - bit1_pos;
        p_bs->p_bit_ptr = p_bs->p_bit_ptr -4 - p_bs->bit_pos/8 - bit1_pos/8;			
        InitBitStream(p_bs, p_bs->p_bit_ptr, p_bs->p_bit_end - p_bs->p_bit_ptr);

        pcm_sample( p_slice, p_bs, x0, y0, log2CbSize );

        vo_init_cabac_decoder(p_slice->pCabac,
          p_bs->p_bit_ptr - 4 - p_bs->bit_pos/8,
          p_bs->p_bit_end - p_bs->p_bit_ptr + 4 +  p_bs->bit_pos/8);

        CurMvField.pred_mode = MODE_INTRA;
        StorePuMotionInfo( p_slice, x0 , y0, 1<<log2CbSize, 1<<log2CbSize, &CurMvField );

			} else {
				VO_U32 i, j, k, pbOffset, pu_in_minpu ;
				VO_U32 prev_intra_luma_pred_flag[ 4 ];//YU_TBD: check size

				/*intra luma mode 8.4.2*/
				//pbOffset = ( p_slice->CuPartMode  ==  PART_NxN ) ? 1 : 2;
				pbOffset = ( p_slice->CuPartMode  ==  PART_NxN ) ? ( nCbS / 2 ) : nCbS;
				pu_in_minpu = pbOffset >> 2;//p_slice->MinTbLog2SizeY;

				for( k = j = 0; j < nCbS; j += pbOffset ) {
					for( i = 0; i < nCbS; i += pbOffset ) {
						prev_intra_luma_pred_flag[ k ] = \
							voCabacDecodeBin( p_slice->pCabac, /*p_bs,*/ OFF_INTRA_PRED_CTX  );
						k++;
					}
				}

				for ( k = j = 0; j < nCbS; j += pbOffset ) {
					for ( i = 0; i < nCbS; i += pbOffset ) {
						VO_U32 candModeList[ 3 ], IntraPredModeY;
						PicMvField CurMvField;

						VO_U32 xPU = ( x0 + i ) >> 2;//p_slice->MinTbLog2SizeY	
						VO_U32 yPU = ( y0 + j ) >> 2;//p_slice->MinTbLog2SizeY 
                        VO_BOOL availableLeft, availableTop;


						//left
                       availableLeft = DeriveZsBlockAvail(p_slice, xPU, yPU, xPU - 1, yPU);
                        if ( !availableLeft ) 
							candModeList[ 0 ] = INTRA_DC;
						//else if ( p_cu_para[ 0 ] != MODE_INTRA || p_cu_para[ 0 ] == 1 ) // CuPredModeL and  pcm_flag
						//	candModeList[ 0 ] = INTRA_DC;
						else 
							candModeList[ 0 ] = p_slice->pu_para_left[ yPU ]; //IntraPredModeYLeft

						//top
                        availableTop = DeriveZsBlockAvail(p_slice, xPU, yPU, xPU, yPU - 1);
                        if ( !availableTop )
							candModeList[ 1 ] = INTRA_DC;
						//else if ( p_cu_para[ 0 ] != MODE_INTRA || p_cu_para[ 0 ] == 1 )
						//	candModeList[ 1 ] = INTRA_DC;
						else if ( ( y0 + j - 1 ) <= ( ( y0 >> p_slice->CtbLog2SizeY ) << p_slice->CtbLog2SizeY ) ) //YU_TBD; need check
							candModeList[ 1 ] = INTRA_DC;
						else 
							candModeList[ 1 ] = p_slice->pu_para_top[ xPU ]; //IntraPredModeYTop

						if ( candModeList[ 0 ] == candModeList[ 1 ] ) {
							if ( candModeList[ 0 ] < 2 ) {
								candModeList[ 0 ] = INTRA_PLANAR;//	(8 15)
								candModeList[ 1 ] = INTRA_DC;//		(8 16)
								candModeList[ 2 ] = INTRA_ANGULAR26;//	(8 17)
							} else {
								candModeList[ 1 ] = 2 + ( ( candModeList[ 0 ] + 29 ) % 32 );//	(8 19)
								candModeList[ 2 ] = 2 + ( ( candModeList[ 0 ] - 2 + 1 ) % 32 );//	(8 20)
							} 
						} else {
							if ( candModeList[ 0 ] != INTRA_PLANAR && candModeList[ 1 ] != INTRA_PLANAR ) {
								candModeList[ 2 ] = INTRA_PLANAR;
								//} else if ( candModeList[ 0 ] == INTRA_DC && candModeList[ 1 ] == INTRA_DC ) {
								//	candModeList[ 2 ] = INTRA_DC; //YU_TBD: check spec
							} else {
								//candModeList[ 2 ] = INTRA_ANGULAR26;//YU_TBD: check spec
								candModeList[ 2 ] =  ( candModeList[ 0 ] + candModeList[ 1 ] ) < 2 ? INTRA_VER : INTRA_DC;
							}
						}

						if ( prev_intra_luma_pred_flag[ k ] ) {
							VO_U32 mpm_idx = voCabacDecodeBinEP( p_slice->pCabac/*, &p_slice->bs*/ );
							if ( mpm_idx ) {
								mpm_idx = voCabacDecodeBinEP( p_slice->pCabac/*, &p_slice->bs*/ );
								mpm_idx++;
							}
							IntraPredModeY = candModeList[ mpm_idx ]; //mpm_idx[ x0 + i ][ y0 + j ]	
						} else {
							VO_U32 h;

							IntraPredModeY = voCabacDecodeBinsEP( p_slice->pCabac,/* &p_slice->bs,*/ 5 );//rem_intra_luma_pred_mode[ x0 + i ][ y0 + j ]

							if ( candModeList[ 0 ] > candModeList[ 1 ] )
								VOSWAP( VO_U32, candModeList[ 0 ], candModeList[ 1 ] );
							if ( candModeList[ 0 ] > candModeList[ 2 ] )
								VOSWAP( VO_U32, candModeList[ 0 ], candModeList[ 2 ] );
							if ( candModeList[ 1 ] > candModeList[ 2 ] )
								VOSWAP( VO_U32, candModeList[ 1 ], candModeList[ 2 ] );

							for ( h = 0; h <= 2; h++ ) {
								if ( IntraPredModeY >= candModeList[ h ] )
									IntraPredModeY++;
							}
						}
						p_slice->IntraPredModeY[ k ] = ( VO_U8 )IntraPredModeY;
						k++;

						//printf("\n intramode_num=%d, pux =%d, pu_y=%d, mode = %d, %d, %d, %d ",intramode_num++,xPU, yPU, IntraPredModeY, candModeList[0],candModeList[1],candModeList[2] );
						//set pu buffer
						SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_slice->pu_para_top + xPU, IntraPredModeY, pu_in_minpu );
						SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_slice->pu_para_left + yPU, IntraPredModeY, pu_in_minpu );

						CurMvField.pred_mode = MODE_INTRA;
						StorePuMotionInfo( p_slice, x0 + i, y0 + j, pbOffset, pbOffset, &CurMvField );

					}
				}

				//---8.4.3	Derivation process for chroma intra prediction mode
				/*chroma pred mode*/
				if ( voCabacDecodeBin( p_slice->pCabac, /*p_bs,*/ OFF_CHROMA_PRED_CTX ) == 0 )
					p_slice->IntraPredModeC = p_slice->IntraPredModeY[ 0 ];
				else {
					VO_U32 intra_chroma_pred_mode = intra_chroma_dir_table[ voCabacDecodeBinsEP( p_slice->pCabac,/* p_bs,*/ 2 ) ] ;

					//YU_TBD: check intra_chroma_pred_mode derivation and usage
					p_slice->IntraPredModeC = p_slice->IntraPredModeY[ 0 ] ==  intra_chroma_pred_mode  ? \
						34 : intra_chroma_pred_mode;
				}
			}

			if ( p_slice->CuPartMode == PART_NxN ) { //CuPredMode[ x0 ][ y0 ]
				p_slice->IntraSplitFlag = 1; //Table 7 10
			}
		} else {
			p_slice->merge_flag = 0;// default

			switch ( p_slice->CuPartMode ) {
			case PART_2Nx2N :
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0, nCbS, nCbS, 0 );
				break;
			case PART_2NxN :
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0, nCbS, nCbS / 2, 0 );
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0 + ( nCbS / 2 ), nCbS, nCbS / 2, 1 );
				/*{
					VO_U32 *cmp0 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].pb_mvfield;
					VO_U32 *cmp1 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-1].pb_mvfield;
					if(cmp0[0]==cmp1[0]&&cmp0[1]==cmp1[1]&&cmp0[2]==cmp1[2])
					{
					  p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].nPbH = nCbS;
					  p_slice->cur_rctask->total_pu--;
					}
					//printf("inter skip: %d\n",intramode_num++);
				}*/
				break;
			case PART_Nx2N :
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0, nCbS / 2, nCbS, 0 );
				prediction_unit( p_slice, x0, y0, nCbS, x0 + ( nCbS / 2 ), y0, nCbS / 2, nCbS, 1 );
				/*{
					VO_U32 *cmp0 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].pb_mvfield;
					VO_U32 *cmp1 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-1].pb_mvfield;
					if(cmp0[0]==cmp1[0]&&cmp0[1]==cmp1[1]&&cmp0[2]==cmp1[2])
					{
					  p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].nPbW = nCbS;
					  p_slice->cur_rctask->total_pu--;
					}
					//printf("inter skip: %d\n",intramode_num++);
				}*/
				break;
			case PART_2NxnU :
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0, nCbS, nCbS / 4, 0 );
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0 + ( nCbS / 4 ), nCbS, nCbS * 3 / 4, 1 );
				/*{
					VO_U32 *cmp0 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].pb_mvfield;
					VO_U32 *cmp1 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-1].pb_mvfield;
					if(cmp0[0]==cmp1[0]&&cmp0[1]==cmp1[1]&&cmp0[2]==cmp1[2])
					{
					  p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].nPbH = nCbS;
					  p_slice->cur_rctask->total_pu--;
					}
					//printf("inter skip: %d\n",intramode_num++);
				}*/
				break;
			case PART_2NxnD :
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0, nCbS, nCbS * 3 / 4, 0 );
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0 + ( nCbS * 3 / 4 ), nCbS, nCbS / 4, 1 );
				/*{
					VO_U32 *cmp0 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].pb_mvfield;
					VO_U32 *cmp1 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-1].pb_mvfield;
					if(cmp0[0]==cmp1[0]&&cmp0[1]==cmp1[1]&&cmp0[2]==cmp1[2])
					{
					  p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].nPbH = nCbS;
					  p_slice->cur_rctask->total_pu--;
					}
					//printf("inter skip: %d\n",intramode_num++);
				}*/
				break;
			case PART_nLx2N : 
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0, nCbS / 4, nCbS, 0 );
				prediction_unit( p_slice, x0, y0, nCbS, x0 + ( nCbS / 4 ), y0, nCbS * 3 / 4, nCbS, 1 );
				/*{
					VO_U32 *cmp0 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].pb_mvfield;
					VO_U32 *cmp1 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-1].pb_mvfield;
					if(cmp0[0]==cmp1[0]&&cmp0[1]==cmp1[1]&&cmp0[2]==cmp1[2])
					{
					  p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].nPbW = nCbS;
					  p_slice->cur_rctask->total_pu--;
					}
					//printf("inter skip: %d\n",intramode_num++);
				}*/
				break;
			case PART_nRx2N :
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0, nCbS * 3 / 4, nCbS, 0 );
				prediction_unit( p_slice, x0, y0, nCbS, x0 + ( nCbS * 3 / 4 ), y0, nCbS / 4, nCbS, 1 );
				/*{
					VO_U32 *cmp0 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].pb_mvfield;
					VO_U32 *cmp1 = (VO_U32*)&p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-1].pb_mvfield;
					if(cmp0[0]==cmp1[0]&&cmp0[1]==cmp1[1]&&cmp0[2]==cmp1[2])
					{
					  p_slice->cur_rctask->pu_task[p_slice->cur_rctask->total_pu
						-2].nPbW = nCbS;
					  p_slice->cur_rctask->total_pu--;
					}
					//printf("inter skip: %d\n",intramode_num++);
				}*/
				break;
			case PART_NxN :
				prediction_unit( p_slice, x0, y0, nCbS, x0, y0, nCbS / 2, nCbS / 2, 0 );
				prediction_unit( p_slice, x0, y0, nCbS, x0 + ( nCbS / 2 ), y0, nCbS / 2, nCbS / 2, 1 );

				prediction_unit( p_slice, x0, y0, nCbS, x0, y0 + ( nCbS / 2 ), nCbS / 2, nCbS / 2, 2 );
				prediction_unit( p_slice, x0, y0, nCbS, x0 + ( nCbS / 2 ), y0 + ( nCbS / 2 ), nCbS / 2, nCbS / 2, 3 );

				break;
			default :
				break;
			}
			
		}
		if ( !p_slice->pcm_flag ) {//pcm_flag[ x0 ][ y0 ]
			VO_U32 rqt_root_cbf = 1; //default
		
			/*root cbf*/
			if ( p_slice->CuPredMode !=  MODE_INTRA  && //CuPredMode[ x0 ][ y0 ] 
				!( p_slice->CuPartMode  ==  PART_2Nx2N  &&  p_slice->merge_flag ) )
				rqt_root_cbf = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_QT_ROOT_CBF_CTX );

			if ( rqt_root_cbf ) {
				p_slice->MaxTrafoDepth = ( p_slice->CuPredMode == MODE_INTRA ? \
					( p_sps->max_transform_hierarchy_depth_intra + p_slice->IntraSplitFlag ) : \
					p_sps->max_transform_hierarchy_depth_inter );
				//p_slice->x0 = x0; 
				//p_slice->y0 = y0;//pass cu addr to residual_coding
				transform_tree( p_slice, x0, y0, x0, y0, log2CbSize, 0, 0, x0, y0);
                if (p_pps->cu_qp_delta_enabled_flag) {
                    StoreTuDeblockParaOnlyQP( p_slice, x0, y0, log2CbSize );
                }
        
        
			} else {
				//-----------No root cbf level storage for deblock
			    if (  ( p_slice->fast_mode & VO_FM_DEBLOCK ) == 0) { 
		            p_slice->pcm_flag = 0;
					p_slice->cbf_luma = 0;

					StoreTuDeblockPara( p_slice, x0, y0, log2CbSize );
				}
			}
		} else {
			//-----------PCM storage for deblock
			if ( ( p_slice->fast_mode & VO_FM_DEBLOCK ) == 0) { 
				StoreTuDeblockPara( p_slice, x0, y0, log2CbSize );
			}
		}
	}

	//set CU buffer, intra mode prediction //YU_TBD: initilize at B P slice level, it needs frame level buffer
	if ( !( ( p_slice->CuPredMode == MODE_INTRA ) && ( !p_slice->pcm_flag ) ) ) {
		VO_U32 xMinTb = x0 >> 2;//p_slice->MinTbLog2SizeY	
		VO_U32 yMinTb = y0 >> 2;//p_slice->MinTbLog2SizeY
		VO_U32 cu_in_mintb = 1 << ( log2CbSize - 2 ); //p_slice->MinTbLog2SizeY

		SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_slice->pu_para_top + xMinTb, INTRA_DC, cu_in_mintb );
		SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_slice->pu_para_left + yMinTb, INTRA_DC, cu_in_mintb );
	}

	// LHP 
	if(p_slice->CuPredMode != MODE_INTRA)
	{
	  if (p_slice->slice_deblocking_filter_disabled_flag != 1 && ( p_slice->fast_mode & VO_FM_DEBLOCK ) == 0)
	  {
		  if(p_slice->CuPartMode  !=  PART_2Nx2N )   
			  voSetEdgefilterPU ((void *)p_slice, nCbS>>2, x0>>2, y0>>2, 0);
	  }
	}
	if (p_slice->slice_deblocking_filter_disabled_flag != 1 && ( p_slice->fast_mode & VO_FM_DEBLOCK ) == 0)
	  voSetEdgefilterCU((void *)p_slice, nCbS>>2, x0>>2, y0>>2, 0);

	return VO_ERR_NONE;
}


static VO_S32 UpdateCUCabacBuf( H265_DEC_SLICE* p_slice, 
	const VO_U32 x0, 
	const VO_U32 y0, 
	const VO_U32 log2CbSize ) 
{
	VO_U32 x = x0 >> p_slice->MinCbLog2SizeY;// mincb x offset in picutre
	VO_U32 y = ( y0 >> p_slice->MinCbLog2SizeY ) & p_slice->log2_diff_max_min_luma_mask;// mincb y offset in picture
	VO_U32 cu_with_in_mincb = 1 << ( log2CbSize - p_slice->MinCbLog2SizeY );
	//VO_U32 cu_with_in_minpb = 1 << ( log2CbSize - p_slice->MinTbLog2SizeY );
	//VO_U32 PicWidthInMinTbsY = p_slice->PicWidthInMinTbsY;
	VO_U32 i;
	VO_U32 value  = p_slice->depth;
	VO_U8* p_left = &p_slice->left_depth_buffer[ y ];
	VO_U8* p_top  = &p_slice->top_depth_buffer[ x ];
	//removed by DTT
	//VO_S8* slice_number = p_slice->slice_number + ( y0 >> p_slice->MinTbLog2SizeY ) * PicWidthInMinTbsY + ( x0 >> p_slice->MinTbLog2SizeY );
	//VO_S8 cur_slice_number = ( VO_S8 )p_slice->cur_slice_number;

	//---For cabac
	/* For coding_quadtree() split flag*/
	for ( i = 0; i < cu_with_in_mincb; i++ ) {
		*p_left++ = ( VO_U8 )value;
		*p_top++  = ( VO_U8 )value;
	}

	value  = p_slice->cu_skip_flag;
	p_left = &p_slice->left_skip_buffer[ y ];
	p_top  = &p_slice->top_skip_buffer[ x ];

	/* For coding_unit() skip flag*/
	for ( i = 0; i < cu_with_in_mincb; i++ ) {
		*p_left++ = ( VO_U8 )value;
		*p_top++  = ( VO_U8 )value;
	}

	//---For availability
	//YU_TBD: merge with tu level storage
	//removed by DTT
	//if ( p_slice->CuPredMode != MODE_INTRA ) 
	//{
	//    for ( j = 0;j < cu_with_in_minpb; j++ ) {
	//        for ( i = 0; i < cu_with_in_minpb; i++ ) {
	//            *slice_number++ = (VO_S8)cur_slice_number;
	//	    }
	//        slice_number += PicWidthInMinTbsY - cu_with_in_minpb;
	//    }
	//}

	return VO_ERR_NONE;
}

static VO_S32 coding_quadtree( H265_DEC_SLICE* p_slice, 
	const VO_U32 x0, 
	const VO_U32 y0, 
	const VO_U32 log2CbSize, 
	const VO_U32 cqtDepth ) 
{ 
	//printf("\n x=%d, y=%d, depth=%d, range=%d,value=%d", x0, y0, cqtDepth, p_slice->cabac.uiRange, p_slice->cabac.uiValue );
	//if ( p_slice->m_iPOC == 4 && p_slice->cabac.uiRange == 256 && p_slice->cabac.uiValue == 18528 )
	//	p_slice=p_slice;
	//VO_LOG( LOG_OUTTYPE_ALL,  LOG_PARSER | LOG_INFO_ALL,"\n x=%d, y=%d, depth=%d, range=%d,value=%d", \
	//	x0, y0, cqtDepth, p_slice->cabac.uiRange, p_slice->cabac.uiValue);

	/*split flag*/
	if ( x0 + ( 1 <<  log2CbSize ) <= p_slice->p_sps->pic_width_in_luma_samples &&
		y0 + ( 1 <<  log2CbSize ) <= p_slice->p_sps->pic_height_in_luma_samples &&
		log2CbSize > p_slice->MinCbLog2SizeY ) {
			p_slice->split_cu_flag = voCabacDecodeBin( p_slice->pCabac,
				/*&p_slice->bs, */
				OFF_SPLIT_FLAG_CTX + GetCtxSplitInc( p_slice, cqtDepth, x0, y0 ) );  //YU_TBD: level to store split , split_cu_flag[ x0 ][ y0 ]
	} else {
		p_slice->split_cu_flag = log2CbSize > p_slice->MinCbLog2SizeY;
	}

	/* delta qp flag*/
	if ( p_slice->p_pps->cu_qp_delta_enabled_flag &&  
		log2CbSize  >= ( p_slice->CtbLog2SizeY - p_slice->p_pps->diff_cu_qp_delta_depth ) ) {
			p_slice->IsCuQpDeltaCoded = 0;
			//CuQpDeltaVal = 0;
			//VOASSERT(0);
	}

	if ( p_slice->split_cu_flag ) { //YU_TBD: store split_cu_flag[ x0 ][ y0 ] ?
		VO_U32 x1, y1;

		x1 = x0 + ( 1 << ( log2CbSize - 1 ) );
		y1 = y0 + ( 1 << ( log2CbSize - 1 ) );
		coding_quadtree( p_slice, x0, y0, log2CbSize - 1, cqtDepth + 1 );
		if ( x1 < p_slice->p_sps->pic_width_in_luma_samples ) 
			coding_quadtree( p_slice, x1, y0, log2CbSize - 1, cqtDepth + 1 );
		if ( y1 < p_slice->p_sps->pic_height_in_luma_samples )
			coding_quadtree( p_slice, x0, y1, log2CbSize - 1, cqtDepth + 1 );
		if ( x1 < p_slice->p_sps->pic_width_in_luma_samples  &&  y1 < p_slice->p_sps->pic_height_in_luma_samples )
			coding_quadtree( p_slice, x1, y1, log2CbSize - 1, cqtDepth + 1 );
	} else {
		p_slice->depth = cqtDepth;
		coding_unit( p_slice, x0, y0, log2CbSize );

		/* update cu buffer*/
		UpdateCUCabacBuf( p_slice, x0, y0, log2CbSize ); 
	}
	return VO_ERR_NONE;
}

//VO_U32 sao_num = 0;
static VO_S32 sao( H265_DEC_SLICE *p_slice,
	const VO_U32 rx, 
	const VO_U32 ry,
	const VO_U32 ctb_addr_in_rs,
	const VO_U32 ctb_addr_in_ts)
{
	H265_DEC_PPS * p_pps = p_slice->p_pps;
// 	TComSampleAdaptiveOffset *pSAO = p_slice->m_saoParam[ry];
	SaoLcuParam*  pCurLcuSAOParam = &p_slice->saoLcuParam[ctb_addr_in_rs];
  VO_U32 sao_merge_flag = 0;
	VO_S32 cIdx, i;

	//if( sao_num==231)
	//	sao_num=sao_num;

	/*sao_merge_left_flag*/
	if ( rx > 0 ) {
		VO_U32 leftCtbInSliceSeg = ctb_addr_in_rs > p_slice->SliceAddrRs;
		VO_U32 leftCtbInTile = p_pps->TileId[ ctb_addr_in_ts ] == p_pps->TileId[ p_pps->CtbAddrRsToTs[ ctb_addr_in_rs - 1 ] ];

		if( leftCtbInSliceSeg  &&  leftCtbInTile ) { 
			sao_merge_flag = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_SAO_MERGE_FLAG_CTX );
		}
	}

	/*sao_merge_up_flag*/
	if ( ry > 0  && !sao_merge_flag ) {
		VO_U32 upCtbInSliceSeg = ( ctb_addr_in_rs - p_slice->PicWidthInCtbsY ) >= p_slice->SliceAddrRs;
		VO_U32 upCtbInTile = p_pps->TileId[ ctb_addr_in_ts ] == p_pps->TileId[ p_pps->CtbAddrRsToTs[ ctb_addr_in_rs - p_slice->PicWidthInCtbsY ] ];

		if ( upCtbInSliceSeg  &&  upCtbInTile ) {
			sao_merge_flag = voCabacDecodeBin( p_slice->pCabac, /*&p_slice->bs,*/ OFF_SAO_MERGE_FLAG_CTX ) << 1;
		}
	}

	if ( !sao_merge_flag ) {
		for ( cIdx = 0; cIdx < 3; cIdx++ ) {
			if ( ( p_slice->slice_sao_luma_flag  &&  cIdx ==  0 ) || 
			   ( p_slice->slice_sao_chroma_flag  &&  cIdx > 0 ) ) {
			    if ( cIdx != 2 ) {
			        pCurLcuSAOParam->sao_type_idx[ cIdx ] =  ParseSaoTypeIdx( p_slice->pCabac/*, &p_slice->bs*/ );
				} else {
					pCurLcuSAOParam->sao_type_idx[ 2 ] = pCurLcuSAOParam->sao_type_idx[ 1 ];
				}

				if ( pCurLcuSAOParam->sao_type_idx[ cIdx ] != SAO_NONE ) {
					for ( i = 0; i < 4; i++ ) {
						pCurLcuSAOParam->sao_offset_abs[ cIdx ][ i ] = ParserSaoOffsetAbs( p_slice->pCabac/*, &p_slice->bs*/ );
					}
					if ( pCurLcuSAOParam->sao_type_idx[ cIdx ] != SAO_BO ) {
						if ( cIdx != 2) {
							pCurLcuSAOParam->sao_type_idx[ cIdx ] = voCabacDecodeBinsEP( p_slice->pCabac, /*&p_slice->bs,*/ 2 );
						} else {
							pCurLcuSAOParam->sao_type_idx[ 2 ] = pCurLcuSAOParam->sao_type_idx[ 1 ] ;
						}
					} else {
						for ( i = 0; i < 4; i++ ) {
							if ( pCurLcuSAOParam->sao_offset_abs[ cIdx ][ i ] != 0 ) {
								if ( voCabacDecodeBinEP( p_slice->pCabac/*, &p_slice->bs*/ ) ) {
									pCurLcuSAOParam->sao_offset_abs[ cIdx ][ i ] = -pCurLcuSAOParam->sao_offset_abs[ cIdx ][ i ] ;
								}								
							}
						}	
						pCurLcuSAOParam->sao_band_position[ cIdx ] = voCabacDecodeBinsEP( p_slice->pCabac, /*&p_slice->bs,*/ 5 );
					}//end of else if ( pCurLcuSAOParam->sao_type_idx == SAO_EO ) 
				}//end of if ( pCurLcuSAOParam->sao_type_idx != SAO_NONE )
			}//end of if ( ( p_slice->slice_sao_luma_flag  &&  cIdx ==  0 ) || ( p_slice->slice_sao_chroma_flag  &&  cIdx > 0 ) )
		}//end of for ( cIdx = 0; cIdx < 3; cIdx++ )
	} else {//end of if ( !pCurLcuSAOParam->sao_merge_up_flag && !pCurLcuSAOParam->sao_merge_left_flag )
		if ( sao_merge_flag == 1 ) {//left
			CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,  pCurLcuSAOParam, pCurLcuSAOParam - 1, sizeof( *pCurLcuSAOParam ) );
		} else {
			CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,  pCurLcuSAOParam, pCurLcuSAOParam - p_slice->PicWidthInCtbsY, sizeof( *pCurLcuSAOParam ) );
		}
	}

	//printf("\n sao sao_num=%d, range=%d,value=%d",  sao_num++, p_slice->cabac.uiRange, p_slice->cabac.uiValue );
# if SAO_LOG
	printSAOParam(p_slice, ctb_addr_in_rs);
#endif
	return VO_ERR_NONE;
}


VO_S32 ReconTask(TReconTask* cur_rctask, VO_U32 nRow, VO_U32 nColumn)
{
	VO_S32 i;
	H265_DEC_SLICE *p_slice = cur_rctask->p_slice;
	const VO_U32 log2_luma_coding_block_size = p_slice->p_sps->log2_luma_coding_block_size;
// 	p_slice->cur_doingTsak = cur_rctask;

	if (p_slice->errFlag)
	{
		return 0;
	}

    for(i=cur_rctask->start_pu;i<cur_rctask->total_pu;i++)
    {
      PUTask* pu = &cur_rctask->pu_task[i];
	  cur_rctask->start_pu = i;
      if (H265_MotionCompensation( p_slice, pu->x0, pu->y0, pu->nPbW, pu->nPbH, &pu->pb_mvfield, cur_rctask))
      {

// printf("MC not finished POC %d rs %d\n", p_slice->m_iPOC, cur_rctask->ctb_addr_in_rs);
		  return 1; //not finished
      }
#if USE_3D_WAVE_THREAD
		if (p_slice->nThdNum > 1 && cur_rctask->pThdInfo->errFlag)
		{
			if (!p_slice->errFlag)
			{
				p_slice->errFlag = cur_rctask->pThdInfo->errFlag;
			}
  
			return 0;
		}
#endif
    }
	
    for(i=0;i<cur_rctask->total_tu;i++)
    {
        TUTask* tu = &cur_rctask->tu_task[i];
		if(tu->intra_flag)
		{
		    H265_IntraPrediction(p_slice, tu->x0, tu->y0, tu->log2TrafoSize, tu->IntraPredMode, tu->cIdx, tu->available_flag);
			tu->intra_flag=0;
		}
		if(tu->trans_flag)
		{
		    VO_S16  tmp_block_array[32 * 32];
			tmp_block_array[0] =(VO_S16) tu->width;
            (IDCT_function_pointer_array[tu->function_index])(tu->TransCoeffLevel,
                tu->p_pred_buf,tu->pred_stride,tmp_block_array);
			tu->trans_flag=0;
		}
    }

	//backup one row pixel
	if(nRow < p_slice->PicHeightInCtbsY-1 &&p_slice->dbk_flag)
	{
		TComPic*	cur_pic = p_slice->cur_pic;
		VO_U32 stride_y = cur_pic->pic_stride[0];
		VO_U32 stride_uv =  cur_pic->pic_stride[1];
		VO_U32 pixel_row = ((nRow + 1)<<p_slice->CtbLog2SizeY) -1;
		VO_U32 pixel_col = nColumn<<p_slice->CtbLog2SizeY;
// 		VO_S32 next_entry = p_pps->tiles_or_entropy_coding_sync_idc ? entry + 1 : entry;

		VO_U8* pSrcY = cur_pic->pic_buf[0] + pixel_row * stride_y + pixel_col;
		VO_U8* pSrcU = cur_pic->pic_buf[1] + (pixel_row>>1) * stride_uv + (pixel_col>>1);
		VO_U8* pSrcV = cur_pic->pic_buf[2] + (pixel_row>>1) * stride_uv + (pixel_col>>1);
#if USE_3D_WAVE_THREAD
		VO_U8* pDstY = p_slice->m_puBufForIntraY[nRow] + pixel_col;
		VO_U8* pDstU = p_slice->m_puBufForIntraU[nRow] + (pixel_col>>1);
		VO_U8* pDstV = p_slice->m_puBufForIntraV[nRow] + (pixel_col>>1);
#else
		VO_U8* pDstY = p_slice->m_puBufForIntraY[nRow&1] + pixel_col;
		VO_U8* pDstU = p_slice->m_puBufForIntraU[nRow&1] + (pixel_col>>1);
		VO_U8* pDstV = p_slice->m_puBufForIntraV[nRow&1] + (pixel_col>>1);
#endif
#if USE_INLINE_ASM
		if (log2_luma_coding_block_size > 4)	//not suitable for 16x16 LCU
		{
			VO_S32 width = sizeof(VO_U8)<<log2_luma_coding_block_size;
			__asm__ volatile
				(
				"   asr      r0, %6, #1                     \n\t"
				"   mov      r1, r0                         \n\t"
				"1: vld1.u8 {d0, d1, d2, d3}, [%0]!         \n\t"               
				"   vst1.u8 {d0, d1, d2, d3}, [%3]!         \n\t"
				"   subs      %6, %6, #32                   \n\t"
				"   bgt       1b                            \n\t"
				"2: vld1.u8 {d4, d5}, [%1]!                 \n\t"                
				"   vst1.u8 {d4, d5}, [%4]!                 \n\t"
				"   subs      r0, r0, #16                   \n\t"
				"   bgt       2b                            \n\t"
				"3: vld1.u8 {d6, d7}, [%2]!                 \n\t"
				"   vst1.u8 {d6, d7}, [%5]!                 \n\t"
				"   subs      r1, r1, #16                   \n\t"
				"   bgt       3b                            \n\t"
				:"+r"(pSrcY),"+r"(pSrcU),"+r"(pSrcV),"+r"(pDstY),"+r"(pDstU),"+r"(pDstV), "+r"(width)
				: 
			:"memory","r0","r1", "q0","q1","q2","q3"
				);
		}
		else
#endif
		{
			CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, pDstY, pSrcY, sizeof(VO_U8)<<log2_luma_coding_block_size);
			CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, pDstU, pSrcU, sizeof(VO_U8)<<(log2_luma_coding_block_size-1));
			CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, pDstV, pSrcV, sizeof(VO_U8)<<(log2_luma_coding_block_size-1));
		}
	}

#if USE_3D_WAVE_THREAD
	//finish recon, then do loop filter, SAO TBD
	//deblock
	if (p_slice->dbk_flag)
	{
		voDeblockingFilterLCUStepNew(p_slice, nRow);
	}

	//SAO
	if (p_slice->sao_flag)
	{
		TComSampleAdaptiveOffset* const pSAO = p_slice->m_saoParam[nRow];

		if((nColumn+1)!=p_slice->PicWidthInCtbsY) 
		{
			if (nColumn)
			{
				//SAOProcess for one LCU row
				if (nRow)
				{
// 					(pSAO-1)->uiStartXByLCU = nColumn - 1;
// 					(pSAO-1)->uiEndXByLCU = nColumn;
// 					(pSAO-1)->uiLCURowStartY = (1<<log2_luma_coding_block_size)-10;
// 					(pSAO-1)->uiLCURowEndY = 1<<log2_luma_coding_block_size;
// 					SAOProcessOneLCU(p_slice, pSAO-1, cur_rctask->ctb_addr_in_rs - p_slice->PicWidthInCtbsY - 1);   
					pSAO->uiStartXByLCU = nColumn - 1;
					pSAO->uiEndXByLCU = nColumn;
					pSAO->uiLCURowStartY = (1<<log2_luma_coding_block_size)-10;
					pSAO->uiLCURowEndY = 1<<log2_luma_coding_block_size;
					SAOProcessOneLCU(p_slice, pSAO, cur_rctask->ctb_addr_in_rs - p_slice->PicWidthInCtbsY - 1);   
				}

				pSAO->uiStartXByLCU = nColumn - 1;
				pSAO->uiEndXByLCU = nColumn;
				pSAO->uiLCURowStartY = 0;

				if (nRow < pSAO->numCuInHeight-1)
				{
					pSAO->uiLCURowEndY = (1<<log2_luma_coding_block_size)-10;
				}
				else
				{
					pSAO->uiLCURowEndY = pSAO->m_iPicHeight-((pSAO->numCuInHeight-1)<<log2_luma_coding_block_size);
				}

				SAOProcessOneLCU(p_slice, pSAO, cur_rctask->ctb_addr_in_rs - 1);  
			}
		}
		else
		{
			//SAOProcess for last 2 LCU

			//1. first LCU
			if (nRow)
			{
// 				(pSAO-1)->uiStartXByLCU = nColumn - 1;
// 				(pSAO-1)->uiEndXByLCU = nColumn;
// 				(pSAO-1)->uiLCURowStartY = (1<<log2_luma_coding_block_size)-10;
// 				(pSAO-1)->uiLCURowEndY = 1<<log2_luma_coding_block_size;
// 				SAOProcessOneLCU(p_slice, pSAO-1, cur_rctask->ctb_addr_in_rs - p_slice->PicWidthInCtbsY - 1);   
				pSAO->uiStartXByLCU = nColumn - 1;
				pSAO->uiEndXByLCU = nColumn;
				pSAO->uiLCURowStartY = (1<<log2_luma_coding_block_size)-10;
				pSAO->uiLCURowEndY = 1<<log2_luma_coding_block_size;
				SAOProcessOneLCU(p_slice, pSAO, cur_rctask->ctb_addr_in_rs - p_slice->PicWidthInCtbsY - 1);   
			}

			pSAO->uiStartXByLCU = nColumn - 1;
			pSAO->uiEndXByLCU = nColumn;
			pSAO->uiLCURowStartY = 0;

			if (nRow < pSAO->numCuInHeight-1)
			{
				pSAO->uiLCURowEndY = (1<<log2_luma_coding_block_size)-10;
			}
			else
			{
				pSAO->uiLCURowEndY = pSAO->m_iPicHeight-((pSAO->numCuInHeight-1)<<log2_luma_coding_block_size);
			}

			SAOProcessOneLCU(p_slice, pSAO, cur_rctask->ctb_addr_in_rs - 1);  

			//2. last LCU
			if (nRow)
			{
// 				(pSAO-1)->uiStartXByLCU = nColumn;
// 				(pSAO-1)->uiEndXByLCU = nColumn+1;
// 				(pSAO-1)->uiLCURowStartY = (1<<log2_luma_coding_block_size)-10;
// 				(pSAO-1)->uiLCURowEndY = 1<<log2_luma_coding_block_size;
// 				SAOProcessOneLCU(p_slice, pSAO-1, cur_rctask->ctb_addr_in_rs - p_slice->PicWidthInCtbsY);   
				pSAO->uiStartXByLCU = nColumn;
				pSAO->uiEndXByLCU = nColumn+1;
				pSAO->uiLCURowStartY = (1<<log2_luma_coding_block_size)-10;
				pSAO->uiLCURowEndY = 1<<log2_luma_coding_block_size;
				SAOProcessOneLCU(p_slice, pSAO, cur_rctask->ctb_addr_in_rs - p_slice->PicWidthInCtbsY);   
			}

			pSAO->uiStartXByLCU = nColumn;
			pSAO->uiEndXByLCU = nColumn + 1; //p_slice->PicWidthInCtbsY
			pSAO->uiLCURowStartY = 0;

			if (nRow < pSAO->numCuInHeight-1)
			{
				pSAO->uiLCURowEndY = (1<<log2_luma_coding_block_size)-10;
			}
			else
			{
				pSAO->uiLCURowEndY = pSAO->m_iPicHeight-((pSAO->numCuInHeight-1)<<log2_luma_coding_block_size);
			}

			SAOProcessOneLCU(p_slice, pSAO, cur_rctask->ctb_addr_in_rs);  
		}
	}
#endif

    return 0;  //finished
}

//DecodeSliceData_rasterOrder
static VO_S32 DecodeSliceData( H265_DEC_SLICE *p_slice,  VO_BOOL *p_new_pic )
{
    VO_S32 entry=0, gap, offset = 0, emul_3, fist_byte, total_gap = 0;
    VO_U32 emulation_prevention_byte_cnt,emulation_prevention_byte_offset_i;
    const H265_DEC_PPS * const p_pps = p_slice->p_pps;
//     const VO_U32 log2_luma_coding_block_size = p_slice->p_sps->log2_luma_coding_block_size;
    VO_U32 nRow, nColumn;
    VO_U32 end_of_slice_segment_flag;
    VO_U32 ctb_addr_in_rs = p_slice->slice_segment_address;

    VO_U32 PicSizeInCtbsY = p_slice->PicSizeInCtbsY; 
	
//     TComSampleAdaptiveOffset * pSAO;

    //VO_S16 TransCoeffLevel[ 32 * 32];
    //VO_BOOL sao_flag = VO_FALSE;
    p_slice->dbk_flag = p_slice->sao_flag = 0;

//     VO_BOOL dbk_flag = VO_FALSE;
#if SLICE_LOG
    printSliceInfo(p_slice->slice_type, p_slice->m_iPOC);
#endif

    //p_slice->TransCoeffLevel = TransCoeffLevel;

    ///VO_U32 iCUAddr;
    ///VO_U32 iStartCUEncOrder = MAX(p_slice->m_uiSliceCurStartCUAddr / p_slice->partition_num, p_slice->slice_segment_address / p_slice->partition_num);
    ///VO_U32 iStartCUAddr =  (iStartCUEncOrder >= p_slice->PicSizeInCtbsY ? p_slice->PicSizeInCtbsY : iStartCUEncOrder);//rpcPic->getPicSym()->getCUOrderMap(iStartCUEncOrder);

    ///VO_U32	iEndCUAddr = p_slice->PicSizeInCtbsY;

    ///VO_BOOL slice_end = 0;



     if (p_pps->transquant_bypass_enabled_flag != 1 && p_pps->pps_deblocking_filter_disabled_flag != 1 && (p_slice->fast_mode & VO_FM_DEBLOCK)==0)
     {
		    p_slice->dbk_flag = 1;
     }

    if (p_pps->transquant_bypass_enabled_flag != 1 && p_slice->p_sps->sample_adaptive_offset_enabled_flag && (p_slice->fast_mode & VO_FM_SAO)==0){
        if(p_slice->slice_sao_luma_flag||p_slice->slice_sao_chroma_flag){			
            p_slice->sao_flag = 1;
        }
    }

    //init sub-stream offset
//     if(p_pps->m_iNumSubstreams>1&&p_pps->entropy_coding_sync_enabled_flag)
//     {
// 		p_slice->bs.p_bit_start = bytepos(&(p_slice->bs)); //Harry: org is "p_bs->p_bit_ptr-(32+p_bs->bit_pos)/8;"
//     }

    ///p_slice->tile_start_x= (p_dec->m_apcTComTile[p_dec->m_puiTileIdxMap[iStartCUAddr]].m_uiFirstCUAddr % p_slice->PicWidthInCtbsY)*p_slice->p_sps->max_cu_width>>2;
    ///p_slice->tile_start_y = p_dec->m_apcTComTile[p_dec->m_puiTileIdxMap[iStartCUAddr]].m_uiFirstCUAddr / p_slice->PicWidthInCtbsY*p_slice->p_sps->max_cu_height>>2;
    p_slice->cabac[entry].bs = p_slice->bs;
    voCabacInit( p_slice, entry++); //YU_TBD: check right place?

    if(p_pps->entropy_coding_sync_enabled_flag)
    {
      if (p_slice->PicSizeInCtbsY/p_slice->PicWidthInCtbsY>p_slice->num_entry_point_offsets + 1
        ||p_slice->num_entry_point_offsets>p_slice->PicHeightInCtbsY)
        VOH265ERROR(VO_H265_ERR_ENTRYERROR);
    }

    if(p_pps->tiles_enabled_flag)
    {
        if (!p_slice->num_entry_point_offsets
            ||p_slice->num_entry_point_offsets+1!=(p_pps->num_tile_rows_minus1 + 1)*(p_pps->num_tile_columns_minus1+ 1))
            VOH265ERROR(VO_H265_ERR_ENTRYERROR);
	}

#if WPP_SKIP_BYTE_OFFSET
	if(p_pps->entropy_coding_sync_enabled_flag || p_pps->tiles_enabled_flag)
#else
	if(p_pps->tiles_enabled_flag)
#endif
	{
        fist_byte = p_slice->cabac[entry-1].bs.first_byte;

        for(emul_3 = 0; emul_3 <  p_slice->emulation_prevention_byte_count; emul_3++)
        {
            if(p_slice->emulation_prevention_byte_offset[emul_3] >= fist_byte)
                break;
        }
    }
    emulation_prevention_byte_cnt=0,emulation_prevention_byte_offset_i=0;
    for(; entry < p_slice->num_entry_point_offsets + 1; entry++)
    {
        H265_DEC_CABAC	 *p_cabac  = &p_slice->cabac[entry-1];    

        //contain more than one subset case: calculate number of emulation prevention byte in this subset      
//         while( p_pps->tiles_enabled_flag && p_slice->emulation_prevention_byte_count>emulation_prevention_byte_offset_i&&
//             p_slice->emulation_prevention_byte_offset[emulation_prevention_byte_offset_i++]<(p_slice->entry_point_offset[entry-1]>>3)){         
//                 emulation_prevention_byte_cnt++;
//         }   
//         p_slice->bs.p_bit_ptr = p_cabac->stream_start + (p_slice->entry_point_offset[entry-1]>>3) - emulation_prevention_byte_cnt + 4; 

        gap = 0;

#if WPP_SKIP_BYTE_OFFSET
		if(p_pps->entropy_coding_sync_enabled_flag || p_pps->tiles_enabled_flag)
#else
        if(p_pps->tiles_enabled_flag)
#endif
        {
//             total_gap = 0;
            offset += p_slice->entries[entry-1].entry_point_offset;
            for(; emul_3 < p_slice->emulation_prevention_byte_count; emul_3++, gap++, total_gap++)
            {
                if((p_slice->emulation_prevention_byte_offset[emul_3] - fist_byte + total_gap + 1) > offset)
                {
                    break;
                }
            }
        }
		p_slice->cabac[entry].bs = p_slice->bs;
        //p_slice->bs.p_bit_ptr = p_cabac->stream_start + p_slice->entries[entry-1].entry_point_offset - gap + 4;  
        p_slice->cabac[entry].bs.p_bit_ptr = p_cabac->stream_start + p_slice->entries[entry-1].entry_point_offset - gap;  
		InitBitStream(&p_slice->cabac[entry].bs,p_slice->cabac[entry].bs.p_bit_ptr,p_slice->bs.p_bit_end-p_slice->cabac[entry].bs.p_bit_ptr);
        if (EofBits(&p_slice->bs))
        {
          VOH265ERROR(VO_H265_ERR_ENTRYERROR);
        }
        p_slice->bs.bit_pos = 0;  
        if(p_pps->entropy_coding_sync_enabled_flag )
            voCabacInitExt(p_slice, entry);
        else	
            voCabacInit(p_slice, entry);


        
    }
    // for first quantization group in a tile
    //set_qp(p_slice,p_slice->slice_qp);  for entry
    for (entry = 0; entry <= p_slice->num_entry_point_offsets; entry++)
    {
        VO_S32 *pDequant_qp_buff = p_slice->entries[entry].dequant_qp;
        VO_S32 qp = p_slice->slice_qp;
        VO_S32 tmp_qp;
        pDequant_qp_buff[0] = qp;
        tmp_qp = Clip3(0, 57, qp + p_slice->p_pps->pps_chroma_qp_offset[0] );
        pDequant_qp_buff[1] = g_aucChromaScale[ tmp_qp ] ;
        tmp_qp = Clip3(0, 57, qp + p_slice->p_pps->pps_chroma_qp_offset[1] );
        pDequant_qp_buff[2] = g_aucChromaScale[ tmp_qp ];
    }
    end_of_slice_segment_flag = 0;
    for (;!end_of_slice_segment_flag && ( ctb_addr_in_rs < PicSizeInCtbsY );  ctb_addr_in_rs++) 
    {
        VO_U32 x_ctb, y_ctb;
        VO_U32 ctb_addr_in_ts = p_pps->CtbAddrRsToTs[ ctb_addr_in_rs ];
        VO_LOG(LOG_PRINT,  LOG_MODULE_ALL | LOG_INFO_ALL, "\n cu = %d", ctb_addr_in_ts);
		nRow = ctb_addr_in_rs/p_slice->PicWidthInCtbsY;
		nColumn = ctb_addr_in_rs%p_slice->PicWidthInCtbsY;
		//ctb_addr_in_rs = p_pps->CtbAddrTsToRs[ ctb_addr_in_ts ];
		p_slice->m_uiCUAddr = ctb_addr_in_rs;  // for multi-core
		p_slice->cur_rctask = &p_slice->rc_task[ctb_addr_in_rs];
		p_slice->cur_rctask->p_slice = p_slice;
		p_slice->cur_rctask->ctb_addr_in_rs = ctb_addr_in_rs;
		p_slice->cur_rctask->nRow = nRow;
		p_slice->cur_rctask->nColumn = nColumn;
		p_slice->cur_rctask->start_pu = 0;
		p_slice->cur_rctask->total_pu = 0;
		p_slice->cur_rctask->start_tu = 0;
		p_slice->cur_rctask->total_tu = 0;
#if LCU_LOG
        printLCUInfo(ctb_addr_in_rs);
#endif

        entry = p_pps->entropy_coding_sync_enabled_flag ?  nRow : \
            (p_slice->num_entry_point_offsets==0?p_slice->num_entry_point_offsets:p_pps->TileId[ctb_addr_in_ts]);
        p_slice->currEntryNum = entry;
        p_slice->pCabac = &p_slice->cabac[entry];

        x_ctb = nColumn << p_slice->CtbLog2SizeY;//x pos in pixels
        y_ctb = nRow << p_slice->CtbLog2SizeY;//y pos in pixels
//         pSAO = p_slice->m_saoParam[nRow];
        if ( p_slice->slice_sao_luma_flag  ||  p_slice->slice_sao_chroma_flag )
            sao( p_slice, nColumn, nRow, ctb_addr_in_rs, ctb_addr_in_ts );

        if(p_pps->cu_qp_delta_enabled_flag) 
        {// for optimazation. because when p_pps->cu_qp_delta_enabled_flag==0, then the qp is always equal to slice_qp
            if ( /*(p_pps->tiles_enabled_flag  &&  p_pps->TileId[ ctb_addr_in_ts ] !=  p_pps->TileId[ pre_ctb_addr_in_ts] )||*/
                (p_pps->entropy_coding_sync_enabled_flag && ctb_addr_in_rs%p_slice->PicWidthInCtbsY==0 )){
                    //WPP and multi-tile case
                    set_qp(p_slice, p_slice->slice_qp); // check ?????
            } 
//             else {
//                 set_qp(p_slice, p_slice->entries[entry].last_coded_qp);
//             }
        }
        //do parser
        if(p_slice->pCabac->stream > p_slice->pCabac->stream_end)
			VOH265ERROR(VO_H265_ERR_ENTRYERROR);
        coding_quadtree( p_slice, x_ctb, y_ctb, p_slice->CtbLog2SizeY, 0);

        end_of_slice_segment_flag = voCabacDecodeBinTrm ( p_slice->pCabac/*, &p_slice->bs*/ ); 


		if(p_slice->sao_flag&&!p_pps->pps_loop_filter_across_slices_enabled_flag){
			//care slice boundary case
// 			nColumn = ctb_addr_in_rs%p_slice->PicWidthInCtbsY;
			p_slice->m_sliceIDMapLCU [ctb_addr_in_rs] =  p_slice->m_sliceIDMapLCU[p_slice->slice_segment_address];
			if(end_of_slice_segment_flag&&ctb_addr_in_ts<PicSizeInCtbsY-1){
				//current slice ends and has next slice, m_sliceIDMapLCU increases 1
				p_slice->m_sliceIDMapLCU [p_pps->CtbAddrTsToRs[ ctb_addr_in_ts+1 ]] = p_slice->m_sliceIDMapLCU[p_slice->slice_segment_address] + 1;
			}
		}

		if ( (ctb_addr_in_rs%p_slice->PicWidthInCtbsY == 1) &&  (  p_slice->dependent_slice_segment_flag ||  (p_pps->entropy_coding_sync_enabled_flag && p_pps->m_iNumSubstreams>1) ) )
		{
			VO_U32 i;
			for(i=0;i<MAX_NUM_CTX_MOD;i++)
				p_slice->cabac[entry+1].contextModels[i] = p_slice->cabac[entry].contextModels[i] ;	
		}

#if USE_3D_WAVE_THREAD
		if (p_slice->nThdNum > 1)
		{
			if ( ctb_addr_in_rs+1 != PicSizeInCtbsY ) 
			{
				p_slice->cur_pic->nProgress_mv = ctb_addr_in_rs;
			}
			else
			{
				p_slice->cur_pic->nProgress_mv = VO_MAX_INT;
			}

			/*if((nColumn+1)==p_slice->PicWidthInCtbsY) 
			{
				TReconTask* rctask = p_slice->rc_task + nRow*p_slice->PicWidthInCtbsY;
				h265_wave3DThdInfo *pThdInfo = p_slice->pThdInfo;

				pthread_mutex_lock(pThdInfo->quaryTaskMutex);
				PushTaskQ(pThdInfo->reconTaskQ, rctask);
				pthread_mutex_unlock(pThdInfo->quaryTaskMutex);
				pthread_cond_signal(pThdInfo->quaryTaskCond);
			}*/
		}
		else
#endif
		{
			//do recon
			ReconTask(p_slice->cur_rctask, nRow, nColumn);
		}
    } 

    // open for forcing output 
    p_slice->slice_end_address = ctb_addr_in_rs;
    if ( ctb_addr_in_rs == PicSizeInCtbsY ) {
        *p_new_pic = VO_TRUE;
    }
// 	else
// 	{
// 		ctb_addr_in_rs = ctb_addr_in_rs;
// 	}
    return VO_ERR_NONE; 
}

/*retire ref error pic from dpb_list, so these pic will not be output
  PrepareDecodeFrame for frame x,   such pic will be inserted into p_dpb_list
  DoDecodeFrame for frame x,        found such pic can NOT get ref pic, then set errFlag = VO_H265_ERR_InvalidRefPic
  PrepareDecodeFrame for frame x+1, retire pic with errFlag = VO_H265_ERR_InvalidRefPic
 */
static VO_VOID xRetireRefErrorPic( H265_DEC* const p_dec,H265_DEC_SLICE *p_slice, 
  H265_DPB_LIST *p_dpb_list )
{
  VO_U32 list_size       = p_dpb_list->size;  
  H265_LIST_NODE*  p_node = p_dpb_list->head;
  H265_LIST_NODE* const p_base = p_dpb_list->list_node_pool;

  while (list_size) {
    TComPic*  rpcPic = p_node->data;    

    if ( rpcPic->errFlag) { // found VO_H265_ERR_InvalidRefPic, retire it
      //printf("retire pic: %x\n",rpcPic);
	  if ( p_dec->nThdNum> 1 ) {
    
      //wait for other thread finish decoding
		h265_wave3DThdPool * const pWave3DThdPool = p_dec->pWave3DThdPool;
		pthread_mutex_lock(&pWave3DThdPool->freeFrameMutex);
		while (pWave3DThdPool->freeFrameList.size+1 < pWave3DThdPool->freeFrameList.szPool) //+1 means all other frames are finished excluding cur frame
		{
			pthread_cond_wait(&pWave3DThdPool->freeFrameCond, &pWave3DThdPool->freeFrameMutex);
        }		
		pthread_mutex_unlock(&pWave3DThdPool->freeFrameMutex);
    
      }
      //rpcPic->errFlag = 0;                             //reset errFlag
      if (p_node == p_dpb_list->head) {// first pic
        p_dpb_list->head = p_base + p_dpb_list->head->next;
        p_node = p_dpb_list->head;
      } else {
        H265_LIST_NODE* p_previous = p_base + p_dpb_list->head->previous;
        H265_LIST_NODE* p_next = p_base + p_node->next;

        //delete p_node
        (p_base + p_node->previous)->next = p_node->next;
        p_next->previous = p_node->previous;
        if (p_node == p_dpb_list->tail)
        {
          p_dpb_list->tail = p_base + p_node->previous;
        }

        //insert p_node before p_dpb_list->head
        p_previous->next = p_node - p_base;
        p_node->previous = p_dpb_list->head->previous;
        p_node->next = p_dpb_list->head - p_base;
        p_dpb_list->head->previous = p_node - p_base;

        p_node = p_next;
      }

      p_dpb_list->size--;
      //break;
      list_size--;
      continue; //retire as possible as we can. (For multi-core)
    }

    p_node = p_base + p_node->next;
    list_size--;
  }
  	

}

static TComPic* xGetNewPicBuffer(H265_DEC* const p_dec, H265_DEC_SLICE *p_slice, 
	H265_DPB_LIST *p_dpb_list )
{
	//H265_DEC_SLICE *p_slice = &p_dec->slice;
	//TComPic* p_pic;
	//YU_TBD, must cowork dpb_list and p_fifo
	VO_U32 max_ref_pic_num = p_slice->p_sps->sps_max_dec_pic_buffering_minus1[p_slice->m_uiTLayer] + 1;// +1 to have space for the picture currently being decoded
	VO_U32 list_size       = p_dpb_list->real_size;  
	H265_LIST_NODE* const p_base = p_dpb_list->list_node_pool;

	/* retire one dpb from head*/
	if (list_size >= max_ref_pic_num) {//list is already sorted by POC
		//YU_TBD is it standard compliant??
		//VO_BOOL bBufferIsAvailable = VO_FALSE;
		VO_BOOL in_loop = VO_FALSE;
		H265_LIST_NODE*  p_node = p_dpb_list->real_head;

		while (list_size) {
			TComPic*  rpcPic = p_node->data;
			//VO_S32  isReference = 0;//i,
			if (p_node == p_dpb_list->head) {// first pic
				in_loop = VO_TRUE;
			}

			if ( rpcPic->m_bNeededForOutput == VO_FALSE && rpcPic->m_bRefenced == VO_FALSE) {// output and no referenced pic
	
				if (p_node == p_dpb_list->real_head)
				{
					if (p_node == p_dpb_list->head) {// first pic
						p_dpb_list->head = p_base + p_dpb_list->head->next;
					}
					p_dpb_list->real_head = p_base + p_dpb_list->real_head->next;
					p_node = p_dpb_list->real_head;
				}
				else
				{
					H265_LIST_NODE* p_previous = p_base + p_dpb_list->real_head->previous;
					H265_LIST_NODE* p_next = p_base + p_node->next;

					if (p_node == p_dpb_list->head) {// first pic
						p_dpb_list->head = p_base + p_dpb_list->head->next;
					}

					//delete p_node
					(p_base + p_node->previous)->next = p_node->next;
					p_next->previous = p_node->previous;
					if (p_node == p_dpb_list->tail)
					{
						p_dpb_list->tail = p_base + p_node->previous;
					}

					//insert p_node before p_dpb_list->real_head
					p_previous->next = p_node - p_base;
					p_node->previous = p_dpb_list->real_head->previous;
					p_node->next = p_dpb_list->real_head - p_base;
					p_dpb_list->real_head->previous = p_node - p_base;

					p_node = p_next;
				}

// 				if (p_node == p_dpb_list->head) {// first pic
// 					p_dpb_list->head = p_base + p_dpb_list->head->next;
// 					p_node = p_dpb_list->head;
// 				} else {
// 					H265_LIST_NODE* p_previous = p_base + p_dpb_list->head->previous;
// 					H265_LIST_NODE* p_next = p_base + p_node->next;
// 
// 					//delete p_node
// 					(p_base + p_node->previous)->next = p_node->next;
// 					p_next->previous = p_node->previous;
// 					if (p_node == p_dpb_list->tail)
// 					{
// 						p_dpb_list->tail = p_base + p_node->previous;
// 					}
// 
// 					//insert p_node before p_dpb_list->head
// 					p_previous->next = p_node - p_base;
// 					p_node->previous = p_dpb_list->head->previous;
// 					p_node->next = p_dpb_list->head - p_base;
// 					p_dpb_list->head->previous = p_node - p_base;
// 
// 					p_node = p_next;
// 				}

				if (in_loop)
				{
					p_dpb_list->size--;
				}
				p_dpb_list->real_size--;
				//break;
				list_size--;
				continue; //Harry:to retire as possible as we can. (For multi-core)
			}

			p_node = p_base + p_node->next;
			list_size--;
		}


		//PushDpbFifo(p_dbp_fifo, rpcPic);
	}


	/* pop one dpb in fifo and insert to list*/
	//if (p_dpb_list->p_fifo != p_dpb_list->head ) {
	//H265_LIST_NODE* p_fifo = p_dpb_list->p_fifo;
	//p_pic = p_dpb_list->p_fifo->data;

	// 	if (p_dpb_list->p_fifo == p_dpb_list->head) {//No empty pic
	//return NULL;
	// 	}
	/* Get an empty pic from fifo and inset to tail*/
	if (p_dpb_list->tail == NULL) {//it's empty after initilization and refresh 
		p_dpb_list->tail = p_dpb_list->head = p_dpb_list->real_head = p_dpb_list->p_fifo;

		/*update fifo*/
		p_dpb_list->p_fifo = p_base + p_dpb_list->p_fifo->next;
	}else if(p_dpb_list->p_fifo == p_dpb_list->real_head){ //TBD for new API
	    if ( p_dec->nThdNum> 1 ) 
		{
            //wait for other thread finish decoding
		    h265_wave3DThdPool * const pWave3DThdPool = p_dec->pWave3DThdPool;
		    pthread_mutex_lock(&pWave3DThdPool->freeFrameMutex);
		    while (pWave3DThdPool->freeFrameList.size+1 < pWave3DThdPool->freeFrameList.szPool) //+1 means all other frames are finished excluding cur frame
		    {
	            pthread_cond_wait(&pWave3DThdPool->freeFrameCond, &pWave3DThdPool->freeFrameMutex);
            }		
		    pthread_mutex_unlock(&pWave3DThdPool->freeFrameMutex);
        }
		xWriteOutput( p_dec, p_dpb_list, 0 );//flush dis pics
		ClearDpbList( p_dpb_list );	
		p_dpb_list->tail = p_dpb_list->head = p_dpb_list->p_fifo;
		p_dpb_list->p_fifo = p_base + p_dpb_list->p_fifo->next;
	} else if (p_dpb_list->tail->next != p_dpb_list->p_fifo - p_base) {//insert to tail
		H265_LIST_NODE* p_fifo = p_dpb_list->p_fifo;
		H265_LIST_NODE* p_next = p_base + p_dpb_list->tail->next;

		/*update fifo*/
		p_dpb_list->p_fifo = p_base + p_fifo->next;

		//delete p_fifo
		(p_base + p_fifo->previous)->next = p_fifo->next;
		(p_base + p_fifo->next)->previous = p_fifo->previous;

		//insert p_fifo after p_dpb_list->tail
		p_next->previous  = p_fifo - p_base;
		p_fifo->previous = p_dpb_list->tail - p_base;
		p_fifo->next = p_dpb_list->tail->next;
		p_dpb_list->tail->next = p_fifo - p_base;	

		p_dpb_list->tail = p_fifo;
	} else {
	    
		p_dpb_list->tail = p_dpb_list->p_fifo;

		/*update fifo*/
		p_dpb_list->p_fifo = p_base + p_dpb_list->p_fifo->next;
	}

	p_dpb_list->size++;
	p_dpb_list->real_size++;

	// reset pciture buffer parameters
	// TODO: in future need to add reset_pic_buffer() methods.
	return p_dpb_list->tail->data;
}


static TComPic* xGetRefPic( const H265_DPB_LIST* const rcListPic, 
	VO_S32 poc)                             
{
	//H265_LIST_NODE*   p_tail = rcListPic->tail;//rcListPic.begin();
	const H265_LIST_NODE*        p_head  = rcListPic->head;
	const H265_LIST_NODE* const  p_base  = rcListPic->list_node_pool;
	VO_U32 list_size               = rcListPic->size;
	TComPic* pcPic                 = p_head->data;

	while ( list_size ) {
		if ( pcPic->m_iPOC == poc ) {
			return  pcPic;
		}
		p_head = p_base + p_head->next;
		pcPic = p_head->data;
		list_size--;
	}

	return  NULL;
}


static TComPic* xGetLongTermRefPic( H265_DEC_SLICE * p_slice, 
	const H265_DPB_LIST* const rcListPic, 
	VO_S32 poc)                                  
{
	const H265_LIST_NODE*        p_head  = rcListPic->head;
	const H265_LIST_NODE* const  p_base  = rcListPic->list_node_pool;
	VO_U32 list_size               = rcListPic->size;

	while ( list_size ) {
		TComPic* pcPic = p_head->data;

		if ( pcPic->m_bIsLongTerm && \
			( pcPic->m_iPOC % ( 1 << p_slice->p_sps->log2_max_pic_order_cnt_lsb ) ) == ( poc % ( 1 << p_slice->p_sps->log2_max_pic_order_cnt_lsb ) ) ) {
				return pcPic;
		}

		p_head = p_base + p_head->next;
		list_size--;
	}

	return  NULL;

}


static VO_S32 setRefPicList( H265_DEC_SLICE *p_slice, 
	const H265_DPB_LIST* const rcListPic )
{
	H265_DEC_RPS* p_rps = p_slice->p_rps;
	TComPic*  pcRefPic = NULL;
	TComPic*  RefPicSetStCurr0[ 16 ];
	TComPic*  RefPicSetStCurr1[ 16 ];
	TComPic*  RefPicSetLtCurr[ 16 ];
	VO_U32 NumPocStCurr0 = 0;
	VO_U32 NumPocStCurr1 = 0;
	VO_U32 NumPocLtCurr  = 0;
	VO_S32 numPocTotalCurr;
	VO_U32 i;

	if ( p_slice->slice_type == I_SLICE) {
		SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,  p_slice->ref_pic_list, 0, sizeof ( p_slice->ref_pic_list ) );//ref list
		SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,  p_slice->num_ref_idx,  0, sizeof ( p_slice->num_ref_idx ) );//ref num
		return VO_ERR_NONE;
	}

  if(p_rps==NULL)
    VOH265ERROR(VO_H265_ERR_RPSERROR);

	p_slice->ref_poc_list = &( p_slice->cur_pic->m_aiRefPOCList );

	for ( i = 0; i < p_rps->num_negative_pics; i++ ) {
		if ( p_rps->used[ i ] ) {
			pcRefPic = xGetRefPic( rcListPic, p_slice->m_iPOC + p_rps->delta_poc[ i ] );//YU_TBD error check
      if(pcRefPic==NULL)
      {      
        VOH265ERROR(VO_H265_ERR_InvalidRefPic);
      }
			pcRefPic->m_bIsLongTerm = VO_FALSE;
			RefPicSetStCurr0[ NumPocStCurr0++ ] = pcRefPic;
		}
	}

	for ( ; i < p_rps->num_negative_pics + p_rps->num_positive_pics; i++ ) {
		if ( p_rps->used[ i ] ) {
			pcRefPic = xGetRefPic( rcListPic, p_slice->m_iPOC + p_rps->delta_poc[ i ] );//YU_TBD error check
      if(pcRefPic==NULL)
      {     
        VOH265ERROR(VO_H265_ERR_InvalidRefPic);
      }
			pcRefPic->m_bIsLongTerm = VO_FALSE;
			RefPicSetStCurr1[ NumPocStCurr1++ ] = pcRefPic;
		}
	}

	for ( i = p_rps->num_negative_pics + p_rps->num_positive_pics + p_rps->num_long_term_pics - 1;
		i > p_rps->num_negative_pics + p_rps->num_positive_pics - 1; i-- ) {
			if ( p_rps->used[ i ] ) {
				pcRefPic = xGetLongTermRefPic( p_slice, rcListPic, p_rps->m_POC[ i ] );//YU_TBD error check
        if(pcRefPic==NULL){         
          VOH265ERROR(VO_H265_ERR_InvalidRefPic);
        }
				pcRefPic->m_bIsLongTerm = VO_TRUE;
				RefPicSetLtCurr[ NumPocLtCurr++ ] = pcRefPic;
			}
	}

	// ref_pic_list_init
	numPocTotalCurr = NumPocStCurr0 + NumPocStCurr1 + NumPocLtCurr;

	if ( numPocTotalCurr ){
		VO_U32 cIdx = 0;
		TComPic*  rpsCurrList0[ MAX_NUM_REF_PICS + 1 ]={0};
		TComPic*  rpsCurrList1[ MAX_NUM_REF_PICS + 1 ]={0};

		for ( i = 0; i < NumPocStCurr0; i++, cIdx++ ) {
			rpsCurrList0[ cIdx ] = RefPicSetStCurr0[ i ];
		}
		for ( i = 0; i < NumPocStCurr1; i++, cIdx++ ) {
			rpsCurrList0[ cIdx ] = RefPicSetStCurr1[ i ];
		}
		for ( i = 0; i < NumPocLtCurr; i++, cIdx++ ) {
			rpsCurrList0[ cIdx ] = RefPicSetLtCurr[ i ];
		}

		if ( p_slice->slice_type == B_SLICE ) {
			cIdx = 0;

			for ( i = 0; i < NumPocStCurr1; i++, cIdx++ ) {
				rpsCurrList1[ cIdx ] = RefPicSetStCurr1[ i ];
			}
			for ( i = 0; i < NumPocStCurr0; i++, cIdx++ ) {
				rpsCurrList1[ cIdx ] = RefPicSetStCurr0[ i ];
			}
			for ( i = 0; i < NumPocLtCurr;  i++, cIdx++ ) {
				rpsCurrList1[ cIdx ] = RefPicSetLtCurr[ i ];
			}
		}

		for ( i = 0; i < p_slice->num_ref_idx[ 0 ]; i++ ) {
			p_slice->ref_pic_list[ 0 ][ i ] = p_slice->ref_pic_list_modification_flag_l0 ? \
				rpsCurrList0[ p_slice->list_entry_l0[ i ] ] : rpsCurrList0[ i % numPocTotalCurr ];
            if(p_slice->ref_pic_list[ 0 ][ i ]==NULL)
				VOH265ERROR(VO_H265_ERR_InvalidRefPic);
			( *(p_slice->ref_poc_list ) )[ 0 ][ i ] = p_slice->ref_pic_list[ 0 ][ i ]->m_iPOC;//ref_poc_list,YU_TBD, check usage with ref_pic_list
		}

		for ( ; i < MAX_NUM_REF_PICS + 1; i++ ) {
			p_slice->ref_pic_list[ 0 ][ i ] = NULL;
		}

		if ( p_slice->slice_type == P_SLICE ) { 
			p_slice->num_ref_idx[ 1 ] = 0;
			SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_slice->ref_pic_list[ 1 ], 0, sizeof( p_slice->ref_pic_list[ 1 ] ) );
		} else {
			for ( i = 0; i < p_slice->num_ref_idx[ 1 ]; i++ ) {
				p_slice->ref_pic_list[ 1 ][ i ] = p_slice->ref_pic_list_modification_flag_l1 ? 
					rpsCurrList1[ p_slice->list_entry_l1[ i ] ] : rpsCurrList1[ i % numPocTotalCurr ];
                if(p_slice->ref_pic_list[ 1 ][ i ]==NULL)
				  VOH265ERROR(VO_H265_ERR_InvalidRefPic);
				( *(p_slice->ref_poc_list ) )[ 1 ][ i ] = p_slice->ref_pic_list[ 1 ][ i ]->m_iPOC;//ref_poc_list
			}

			for ( ; i < MAX_NUM_REF_PICS + 1; i++ ) {
				p_slice->ref_pic_list[ 1 ][ i ] = NULL;
			}
		}
	}
  else
  {
    VOH265ERROR(VO_H265_ERR_RPSERROR);
  }
   return VO_ERR_NONE;
}

/** Function for applying picture marking based on the Reference Picture Set in pReferencePictureSet.
*/
static VO_S32 applyReferencePictureSet(H265_DEC_SLICE* p_slice, 
	H265_DPB_LIST* rcListPic, 
	H265_DEC_RPS* pReferencePictureSet )
{
	//if (rcListPic->size ) {
	// loop through all pictures in the reference picture buffer
	H265_LIST_NODE* p_head = rcListPic->head;
	H265_LIST_NODE* const  p_base  = rcListPic->list_node_pool;
	//H265_LIST_NODE* p_tail = rcListPic->tail;
	VO_U32 list_size       = rcListPic->size;

	while (list_size) {
		TComPic*  rpcPic = p_head->data;
		VO_U32 i, isReference = 0;

		// loop through all pictures in the Reference Picture Set
		// to see if the picture should be kept as reference picture
		// long term
		for ( i = 0; i < pReferencePictureSet->num_positive_pics + pReferencePictureSet->num_negative_pics; i++) {
			if (rpcPic->m_iPOC == p_slice->m_iPOC + pReferencePictureSet->delta_poc[i]) {
        if(rpcPic->m_bIsLongTerm)
          VOH265ERROR(VO_H265_ERR_RERERENCEERROR);
				assert(!rpcPic->m_bIsLongTerm);
				isReference = 1;
				// 					rpcPic->m_bUsedByCurr = pReferencePictureSet->used[i];
				// 					rpcPic->m_bIsLongTerm = VO_FALSE;
				// 					rpcPic->m_bIsUsedAsLongTerm = VO_FALSE;
			}
		}

		// long term
		for ( ;i < pReferencePictureSet->m_numberOfPictures; i++ ) {
			if ( pReferencePictureSet->m_bCheckLTMSB[i] == VO_TRUE ) {
				if ( ( rpcPic->m_iPOC ) == pReferencePictureSet->m_POC[ i ] ) {
          if(!rpcPic->m_bIsLongTerm)
            VOH265ERROR(VO_H265_ERR_RERERENCEERROR);
					assert(rpcPic->m_bIsLongTerm);
					isReference = 1;
					// 						rpcPic->m_bUsedByCurr = pReferencePictureSet->used[i];
				}
			} else {
				if((rpcPic->m_iPOC % (1 << p_slice->p_sps->log2_max_pic_order_cnt_lsb )) == pReferencePictureSet->m_POC[i] % (1 << p_slice->p_sps->log2_max_pic_order_cnt_lsb)) {
          if(!rpcPic->m_bIsLongTerm)
            VOH265ERROR(VO_H265_ERR_RERERENCEERROR);
					assert(rpcPic->m_bIsLongTerm);
					isReference = 1;
					// 						rpcPic->m_bUsedByCurr = pReferencePictureSet->used[i];
				}
			}
		}
		// mark the picture as "unused for reference" if it is not in
		// the Reference Picture Set
		if (rpcPic->m_iPOC != p_slice->m_iPOC && isReference == 0) {      
// LOGI("POC %d not ref\n", rpcPic->m_iPOC);
			rpcPic->m_bRefenced = VO_FALSE;
			// 				rpcPic->m_bIsLongTerm = VO_FALSE;
		}
		p_head = p_base + p_head->next;
		list_size--;
	}
	//}
	return VO_ERR_NONE;
}

#if !(USE_3D_WAVE_THREAD)

static VO_S32 DecodeSlice( H265_DEC* p_dec, 
	H265_DEC_SLICE *p_slice,
	BIT_STREAM *p_bs, 
	const VO_U32 nalu_type, 
	const VO_U32 nuh_temporal_id_plus1, 
	VO_BOOL *p_new_pic )
{
	//H265_DEC_SLICE *p_slice = &p_dec->slice;
	*p_new_pic = VO_FALSE;

	p_slice->nal_unit_type = nalu_type;
	p_slice->m_uiTLayer    = nuh_temporal_id_plus1;

	CopyBitStream( &p_slice->bs, p_bs );
	p_bs = &p_slice->bs;
	
	if ( ParseSliceParamID(p_dec, p_slice, p_bs))
	{
		return VO_ERR_DEC_H265_BASE;//YU_TBD: dedicated error code
	}
	if ( ParseSliceHeader( /*p_dec,*/ p_slice, p_bs, p_dec->rps, p_dec->m_prevPOC  ) )//p_dec,
	{
		return VO_ERR_DEC_H265_BASE;//YU_TBD: dedicated error code
	}
	//LHP Set 0 for BS
	if (!p_slice->slice_deblocking_filter_disabled_flag)
	{
		SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_slice->p_pBsVer, 0, sizeof( VO_U8 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY);
		SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_slice->p_pBsHor, 0, sizeof( VO_U8 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY);
	}

	/* Picture level initilization */
	if ( p_slice->first_slice_segment_in_pic_flag ) {
		p_dec->m_prevPOC = p_slice->m_iPOC;

		/* Flush display pic if IDR or retire pics in dbp*/
		if ( p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR
			|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP
			|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP
			|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLANT
			|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA ) {
				xWriteOutput( p_dec, &p_dec->dpb_list, 0 );//flush dis pics
				ClearDpbList( &p_dec->dpb_list );
		} else {
			applyReferencePictureSet( p_slice, &p_dec->dpb_list, p_slice->p_rps );
		}

		/* Get an empty pic buffer */
		p_slice->cur_pic              = xGetNewPicBuffer( p_dec,p_slice, &p_dec->dpb_list );
		p_slice->cur_pic->Time        = p_slice->timestamp;
		p_slice->cur_pic->m_iPOC      = p_slice->m_iPOC;//YU_TBD right position?
		p_slice->cur_pic->m_bRefenced = VO_TRUE; //default

		//start tile raster scan position 
		//p_slice->tile_start_x = p_slice->tile_start_y = 0;
		p_slice->tile_start_zs = 0;

		p_slice->p_motion_field = p_slice->cur_pic->motion_buffer;

		VO_LOG(LOG_OUTTYPE_ALL,  LOG_MOTION | LOG_INFO_ALL, "\n POC = %d", p_slice->m_iPOC);

		//YU_TBD: 
		/* Pic level buffer intilization*/
	}

	/* Slice level initlization, not dependent slice*/
	if ( !p_slice->dependent_slice_segment_flag ) {
		setRefPicList( p_slice, &p_dec->dpb_list);
#if 0
		// For generalized B
		// note: maybe not existed case (always L0 is copied to L1 if L1 is empty)//YU_TBD check spec
		if (p_slice->slice_type == B_SLICE && p_slice->m_aiNumRefIdx[REF_PIC_LIST_1] == 0) {
			VO_S32 iNumRefIdx = p_slice->m_aiNumRefIdx[REF_PIC_LIST_0];
			VO_S32 iRefIdx;

			p_slice->m_aiNumRefIdx[REF_PIC_LIST_1] = iNumRefIdx;

			for ( iRefIdx = 0; iRefIdx < iNumRefIdx; iRefIdx++) {
				p_slice->m_apcRefPicList[REF_PIC_LIST_1][iRefIdx] = p_slice->m_apcRefPicList[REF_PIC_LIST_0][iRefIdx];
			}
		}
#endif
		//use to drive listCol, YU_TBD: different from spec 8.5.3.2.8	Derivation process for collocated motion vectors
		if ( p_slice->slice_type == B_SLICE && p_slice->enable_temporal_mvp_flag ) {
			VO_BOOL bLowDelay = VO_TRUE;
			VO_S32  iCurrPOC  = p_slice->m_iPOC;
			VO_U32 list, iRefIdx ;

			for ( list = 0; list < 2; list++ ) {
				for ( iRefIdx = 0; iRefIdx < p_slice->num_ref_idx[ list ]; iRefIdx++) {
					if ( p_slice->ref_pic_list[ list ][ iRefIdx ]->m_iPOC > iCurrPOC ) {
						bLowDelay = VO_FALSE;
						break;
					}
				}
			}
			p_slice->m_bCheckLDC = bLowDelay;            
		}
	}

	// actual decoding starts here
	//IniDecodeSlice(p_dec);//YU_TBD: remove it

	//  Decode a picture
	DecodeSliceData( p_slice, p_new_pic );

	// Dirty hack.
	//shouldn't be used to deside if a new pic is appear,only can deside if current pic is finished. DTT

	//YU_TBD, more serious
	///if (p_slice->m_uiCUAddr + 1 >= p_slice->PicSizeInCtbsY) {
	///	*p_new_pic = VO_TRUE;
	//}

	return VO_ERR_NONE;
}
#endif


/** Find a nalu and do ebsptorbsp, don't need to find 2rd sc
* \param p_dec
* \returns error code
*/
static VO_S32 Ebsp2Rbsp( H265_DEC_SLICE * const p_slice, VO_U8 *p_inbuf, 
	VO_S32 inbuf_len, 
	VO_U8 **p_nal,
	VO_U32 *p_nal_len,
	VO_S32 *used_len )	
{
	VO_BOOL first_sc = VO_FALSE;//, escape = VO_FALSE;
	VO_S32 i,  j, nalu_start_pos = 0;

	*used_len = 0;
	/*ebsp to rbsp*/
	//find 0x00 00 03 or 0x00 00 1(2)
	for ( i = 0; i <= inbuf_len - 2 ; ) {
		if ( p_inbuf[ i + 2 ] > 0x3 ) {//check [2]
			i += 3;
			continue;
		} else if ( p_inbuf[ i ] == 0 && p_inbuf [ i + 1 ] == 0 ) {//check [0] and [1]
			if ( p_inbuf[ i + 2 ] == 0x3 ){ //escape
				//escape = VO_TRUE;
				break;
			} else if ( p_inbuf[ i + 2 ] > 0 ) {//next start code
				if ( first_sc == VO_FALSE ) {//find first start code
					i += 3;
					nalu_start_pos = i;
					first_sc = VO_TRUE;
					continue;
				}
				*used_len = i;
				goto FIND_NALU;//break;//find 2rd sc 
			}
		}
		i++;
	}

	if ( first_sc == VO_FALSE ) {// Didn's find 1st sc, no nalu
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	//process escape and left buffer
  p_slice->emulation_prevention_byte_count = 0;
	for ( j = i; j <= inbuf_len - 2 ; ) {
		if ( p_inbuf[ j + 2 ] > 0x3 ) {// find 0x 00 00 
			p_inbuf[ i++ ] = p_inbuf[ j++ ];
			p_inbuf[ i++ ] = p_inbuf[ j++ ];
		} else if ( p_inbuf[ j ] == 0 && p_inbuf[ j + 1 ] == 0 ) {
			if ( p_inbuf[ j + 2 ] == 0x3 ){ //escape
				p_inbuf[ i++ ] = 0;
				p_inbuf[ i++ ] = 0;
        p_slice->emulation_prevention_byte_offset[p_slice->emulation_prevention_byte_count++] = i - nalu_start_pos - 2;//j+2;
				j += 3;
				continue;
			} else if ( p_inbuf[ j + 2 ] > 0 ) {//next start code
				break;
			}
		}

		p_inbuf[ i++ ] = p_inbuf[ j++ ];
	}
	if ( j + 2 > inbuf_len ) {
		while ( j < inbuf_len ) {
			p_inbuf[ i++ ] = p_inbuf[ j++ ];
		}
	}
	*used_len = j;

FIND_NALU:
	(*p_nal)  = p_inbuf + nalu_start_pos;
	(*p_nal_len) = i - nalu_start_pos;

	return VO_ERR_NONE;
}



#if USE_3D_WAVE_THREAD

VO_S32 PrepareDecodeFrame( H265_DEC* const p_dec, 
    H265_DEC_SLICE * const p_slice, 
    VO_U8** pp_inbuf, 
    VO_S32 *buf_len )
{
	VO_S32 used_inbuf_len;
	VO_U8 *p_nal;
	VO_S32 nal_len;
	VO_U32 nal_unit_type, nuh_temporal_id_plus1;
	VO_S32 left_len = *buf_len;
	VO_U8* p_inbuf = *pp_inbuf;
	VO_S32 ret = VO_ERR_NONE;
	VO_S32 i;
	H265_DEC_SPS *p_sps;
	//parse until first slice
	while ( left_len > 3 ) {// at least 3byte sc(0x000001)
// LOGI("Ebsp2Rbsp start\n");
        ret = Ebsp2Rbsp(p_slice, p_inbuf, left_len, &p_nal,(VO_U32 *) &nal_len, &used_inbuf_len );
		if ( ret ) {
			return ret;
		}
		nal_unit_type		  = ( p_nal[ 0 ] >> 1 ) & 0x3f;
// LOGI("Ebsp2Rbsp end\n");

		//skip start position and nalu header(2byte)
		InitBitStream( &p_dec->bit_stream, p_nal + 2, nal_len - 2 );
// LOGI("InitBitStream end\n");

		switch ( nal_unit_type ) {
		case NAL_UNIT_CODED_SLICE_RADL_N:
		case NAL_UNIT_CODED_SLICE_TRAIL_R:
		case NAL_UNIT_CODED_SLICE_TRAIL_N:
		case NAL_UNIT_CODED_SLICE_TLA:
		case NAL_UNIT_CODED_SLICE_TSA_N:
		case NAL_UNIT_CODED_SLICE_STSA_R:
		case NAL_UNIT_CODED_SLICE_STSA_N:
		case NAL_UNIT_CODED_SLICE_BLA:
		case NAL_UNIT_CODED_SLICE_BLANT:
		case NAL_UNIT_CODED_SLICE_BLA_N_LP:
		case NAL_UNIT_CODED_SLICE_IDR:
		case NAL_UNIT_CODED_SLICE_IDR_N_LP:
		case NAL_UNIT_CODED_SLICE_CRA:
		case NAL_UNIT_CODED_SLICE_DLP:
        case NAL_UNIT_CODED_SLICE_RASL_N:
		case NAL_UNIT_CODED_SLICE_TFD:
			goto FindFirstSlice;
			break;
		case NAL_UNIT_ACCESS_UNIT_DELIMITER:
			//TBD
			break;
		case NAL_UNIT_SEI:
			///YU_TBD
			break;
		case NAL_UNIT_SEI_SUFFIX:
			//TBD
			break;
		case NAL_UNIT_VPS:
// LOGI("ParseVPS start\n");
			ret = ParseVPS( p_dec, p_dec->vps, &p_dec->bit_stream );//YU_TBD: pass vps_array
// LOGI("ParseVPS end, ret %x\n", ret);
			break;
		case NAL_UNIT_SPS:
// LOGI("ParseSPS start\n");
			ret = ParseSPS( p_dec, p_dec->sps, p_dec->rps, &p_dec->bit_stream );
// LOGI("ParseSPS end, ret %x\n", ret);
			break;
		case NAL_UNIT_PPS:
// LOGI("ParsePPS start\n");
			ret = ParsePPS( p_dec, p_dec->sps, p_dec->pps, &p_dec->bit_stream );
// LOGI("ParsePPS end, ret %x\n", ret);
			break;
		case NAL_UNIT_RESERVED_10:
        case NAL_UNIT_RESERVED_11:
        case NAL_UNIT_RESERVED_12:
        case NAL_UNIT_RESERVED_13:
        case NAL_UNIT_RESERVED_14:
        case NAL_UNIT_RESERVED_15:
        case NAL_UNIT_RESERVED_22:
        case NAL_UNIT_RESERVED_23:
        case NAL_UNIT_RESERVED_24:
        case NAL_UNIT_RESERVED_25:
        case NAL_UNIT_RESERVED_26:
        case NAL_UNIT_RESERVED_27:
        case NAL_UNIT_RESERVED_28:
        case NAL_UNIT_RESERVED_29:
        case NAL_UNIT_RESERVED_30:
        case NAL_UNIT_RESERVED_31: 
        case NAL_UNIT_EOS:                  
        case NAL_UNIT_EOB:                   
        case NAL_UNIT_FILLER_DATA:   
			break;
		default:
LOGI("unknow nal type\n");
      VOH265ERROR(VO_H265_ERR_NALUERROR);
			break;
		}

		p_inbuf  += used_inbuf_len;
		left_len -= used_inbuf_len;		

	}

	//at here, left_len <= 3 , return buffer_small
	VOASSERT( left_len <= 3 );
LOGI("left len <= 3\n");
	return VO_ERR_INPUT_BUFFER_SMALL;

FindFirstSlice:
// LOGI("find first slice\n");
	//parse first slice header
//	nuh_temporal_id_plus1 = ( p_nal[ 1 ] & 0x7 ) - 1;
	nuh_temporal_id_plus1 = (p_nal[ 1 ] & 0x7);

	if( !nuh_temporal_id_plus1 )
		VOH265ERROR(VO_H265_ERR_NALUERROR);

	--nuh_temporal_id_plus1;

	p_slice->nal_unit_type = nal_unit_type;
	p_slice->m_uiTLayer    = nuh_temporal_id_plus1;

	CopyBitStream( &p_slice->bs, &p_dec->bit_stream );

	if ( ParseSliceParamID( p_dec, p_slice, &p_slice->bs ) ) {
		return VO_ERR_DEC_H265_BASE;//YU_TBD: dedicated error code
	}

	p_sps = p_slice->p_sps;
    if ( p_sps->sample_adaptive_offset_enabled_flag ) {
        setNDBBorderAvailabilityPerPPS(p_slice, p_slice->p_pps, p_slice->m_saoParam[0], p_slice->p_pps->TileId);
    }

	if ( ParseSliceHeader( /*p_dec,*/ p_slice, &p_slice->bs, p_dec->rps, p_dec->m_prevPOC  ) ) {//p_dec,
		return VO_ERR_DEC_H265_BASE;//YU_TBD: dedicated error code
	}

	//here must be the first slice
	if ( !p_slice->first_slice_segment_in_pic_flag ) {
		return VO_ERR_DEC_H265_BASE; //TBD
	}

	if ( p_dec->out_mode == 0 ) {
// LOGI("write output\n");
		//display order    
		xWriteOutput(p_dec, &p_dec->dpb_list, p_sps->sps_max_num_reorder_pics[nuh_temporal_id_plus1] );	
	}

	/* Picture level initilization */
	p_dec->m_prevPOC = p_slice->m_iPOC;

	/* Flush display pic if IDR or retire pics in dbp*/
	if ( p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR
		|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP
		|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP
		|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLANT
		|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA ) {
			xWriteOutput( p_dec, &p_dec->dpb_list, 0 );//flush dis pics
			ClearDpbList( &p_dec->dpb_list );
	} else {
		ret = applyReferencePictureSet( p_slice, &p_dec->dpb_list, p_slice->p_rps );
		if(ret)
		    return ret;
	}

#if USE_CODEC_NEW_API
	if (p_slice->cur_pic)
	{
	}
	else
#endif
	{
// LOGI("first frame, Get new pic\n");
		//retire ref error pic from dpb_list, so these pic will not be output
		xRetireRefErrorPic(p_dec,p_slice, &p_dec->dpb_list);

		/* Get an empty pic buffer */
		p_slice->cur_pic              = xGetNewPicBuffer( p_dec,p_slice, &p_dec->dpb_list );
	}

// LOGI("prepare POC %d\n", p_slice->m_iPOC);
	p_slice->cur_pic->m_iPicWidth     = p_sps->pic_width_in_luma_samples;//pic_stride;
	p_slice->cur_pic->m_iPicHeight    = ( p_sps->pic_height_in_luma_samples + 1 ) & 0xfffffffe;
	p_slice->cur_pic->m_disWidth = p_sps->pic_width_in_luma_samples-p_sps->conf_win_right_offset -p_sps->conf_win_left_offset;
	p_slice->cur_pic->m_disHeight = p_sps->pic_height_in_luma_samples-p_sps->conf_win_bottom_offset-p_sps->conf_win_top_offset;
	p_slice->cur_pic->m_iPOC      = p_slice->m_iPOC;//YU_TBD right position?
	p_slice->cur_pic->m_bRefenced = VO_TRUE; //default
	p_slice->cur_pic->p_slice = p_slice;
    //printf("get pic poc:%d slice: %x pic: %x\n", p_slice->cur_pic->m_iPOC,p_slice,p_slice->cur_pic);
// 	p_slice->cur_pic->nReady_y = -100;
// 	p_slice->cur_pic->nProgress = -100;	//Harry: progress init
	p_slice->cur_pic->nProgress_mv = -100;	//Harry: progress init
	p_slice->cur_pic->errFlag = VO_ERR_NONE; //Harry: error flag init
	p_slice->cur_pic->bFinishDecode = 0;

	//VO_LOG(LOG_OUTTYPE_ALL,  LOG_MOTION | LOG_INFO_ALL, "\n POC = %d", p_slice->m_iPOC);
	p_slice->cur_pic->Time        = p_slice->timestamp;

	//SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_slice->rc_task, 0, p_slice->PicSizeInCtbsY*sizeof(TReconTask));
	for(i=0;i< p_slice->PicSizeInCtbsY;i++)
	{
	  p_slice->rc_task[i].bFinish=0;
	  p_slice->rc_task[i].blocked=NULL;
	  p_slice->rc_task[i].next_blocked=NULL;
	}
    //SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_slice->rc_task_flag, 0, p_slice->PicSizeInCtbsY*sizeof(TReconTaskFlag));
	//backup dpb_list into slice
	p_slice->dpb_list_s = p_dec->dpb_list;
	p_slice->dpb_list_s.head = p_slice->dpb_list_s.list_node_pool + (p_dec->dpb_list.head - p_dec->dpb_list.list_node_pool);
	p_slice->dpb_list_s.tail = p_slice->dpb_list_s.list_node_pool + (p_dec->dpb_list.tail - p_dec->dpb_list.list_node_pool);
	p_slice->dpb_list_s.p_fifo = p_slice->dpb_list_s.list_node_pool + (p_dec->dpb_list.p_fifo - p_dec->dpb_list.list_node_pool);

// 	{
// 		H265_LIST_NODE* nod = p_slice->dpb_list_s.head;
//		LOGI("\n New frame dpb_list_s poc:%d list_size:%d, real size %d\n",p_slice->m_iPOC, p_slice->dpb_list_s.size, p_slice->dpb_list_s.real_size);
// 		while(nod!=p_slice->dpb_list_s.tail)
// 		{
//			LOGI("poc %d,",nod->data->m_iPOC);
// 			nod = p_slice->dpb_list_s.list_node_pool+nod->next;
// 		}
//		LOGI("poc %d\n", nod->data->m_iPOC);
// 	}

	p_slice->cur_pic->m_bNeededForOutput = VO_TRUE;
	sortPicList( &p_dec->dpb_list ); 

// 	{
// 		H265_LIST_NODE* nod = p_dec->dpb_list.head;
// 		printf("dpb_list poc:%d list_size:%d\n",p_slice->m_iPOC, p_dec->dpb_list.size);
// 		while(nod!=p_dec->dpb_list.tail)
// 		{
// 			printf("poc %d,",nod->data->m_iPOC);
// 			nod = p_dec->dpb_list.list_node_pool+nod->next;
// 		}
// 		printf("poc %d\n", nod->data->m_iPOC);
// 	}

	if (p_dec->out_mode == 0) {
    //display order    
//      xWriteOutput(p_dec, &p_dec->dpb_list, p_slice->p_sps->sps_max_num_reorder_pics[nuh_temporal_id_plus1] );		
	} else {
    //decoding order
    PushDisPic(p_dec, &p_dec->dpb_list, p_slice->cur_pic);
	}

	//output the next nalu pos
	*pp_inbuf = p_inbuf  + used_inbuf_len;
	*buf_len = left_len - used_inbuf_len;	

	return VO_ERR_NONE;
}

VO_S32 DoDecodeFrame(const H265_DEC* const p_dec, H265_DEC_SLICE * const p_slice, VO_U8* p_inbuf, VO_S32 buf_len)
{
	VO_S32 ret = VO_ERR_NONE;
	VO_BOOL new_pic  = VO_FALSE;

	//YU_TBD: 
	/* Pic level buffer intilization*/

	//start tile raster scan position 
	//p_slice->tile_start_zs = 0;
    p_slice->currEntryNum = 0;
	p_slice->p_coeff[0] = p_slice->coeff_buf[0];
	p_slice->p_coeff[1] = p_slice->coeff_buf[1];
	p_slice->p_coeff[2] = p_slice->coeff_buf[2];
    

	p_slice->p_motion_field = p_slice->cur_pic->motion_buffer;
	SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_slice->filterX_Pos, 0, sizeof( VO_S32 ) * p_slice->PicHeightInCtbsY);

	//LHP Set 0 for BS
	if (!p_slice->slice_deblocking_filter_disabled_flag && (p_slice->fast_mode & VO_FM_DEBLOCK)==0)
	{
		SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_slice->p_pBsVer, 0, sizeof( VO_U8 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY);
		SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_slice->p_pBsHor, 0, sizeof( VO_U8 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY);
	}

	/* Slice level initlization, not dependent slice*/
// 	if ( !p_slice->dependent_slice_segment_flag ) {
		ret = setRefPicList( p_slice, &p_slice->dpb_list_s); //Harry: use the dbp_list in slice 
    if(ret)
      return ret;
#if 0
		// For generalized B
		// note: maybe not existed case (always L0 is copied to L1 if L1 is empty)//YU_TBD check spec
		if (p_slice->slice_type == B_SLICE && p_slice->m_aiNumRefIdx[REF_PIC_LIST_1] == 0) {
			VO_S32 iNumRefIdx = p_slice->m_aiNumRefIdx[REF_PIC_LIST_0];
			VO_S32 iRefIdx;

			p_slice->m_aiNumRefIdx[REF_PIC_LIST_1] = iNumRefIdx;

			for ( iRefIdx = 0; iRefIdx < iNumRefIdx; iRefIdx++) {
				p_slice->m_apcRefPicList[REF_PIC_LIST_1][iRefIdx] = p_slice->m_apcRefPicList[REF_PIC_LIST_0][iRefIdx];
			}
		}
#endif
		//use to drive listCol, YU_TBD: different from spec 8.5.3.2.8	Derivation process for collocated motion vectors
    if (p_slice->slice_type != I_SLICE)// fix bug:#30905
    {
      VO_BOOL bLowDelay = VO_TRUE;
      VO_S32  iCurrPOC  = p_slice->m_iPOC;
      VO_U32  iRefIdx ;
      for ( iRefIdx = 0; iRefIdx < p_slice->num_ref_idx[ 0 ]; iRefIdx++) {
        if(p_slice->ref_pic_list[REF_PIC_LIST_0][iRefIdx]==NULL)
          VOH265ERROR(VO_H265_ERR_RERERENCEERROR);
        if ( p_slice->ref_pic_list[ 0 ][ iRefIdx ]->m_iPOC > iCurrPOC ) {
          bLowDelay = VO_FALSE;
          break;
        }
      }
      //use to drive listCol, YU_TBD: different from spec 8.5.3.2.8	Derivation process for collocated motion vectors
      if ( p_slice->slice_type == B_SLICE /*&& p_slice->enable_temporal_mvp_flag*/ ) {                 
        for ( iRefIdx = 0; iRefIdx < p_slice->num_ref_idx[ 1 ]; iRefIdx++) {
          if(p_slice->ref_pic_list[REF_PIC_LIST_1][iRefIdx]==NULL)
            VOH265ERROR(VO_H265_ERR_RERERENCEERROR);
          if ( p_slice->ref_pic_list[ 1 ][ iRefIdx ]->m_iPOC > iCurrPOC ) {
            bLowDelay = VO_FALSE;
            break;
          }
        }             
      }
      p_slice->m_bCheckLDC = bLowDelay;  

    }
// 	}

	// actual decoding starts here
	//IniDecodeSlice(p_dec);//YU_TBD: remove it

	//  Decode a picture
	ret = DecodeSliceData( p_slice, &new_pic );
    if(ret)
      return ret;

	if (!new_pic && buf_len > 3) //there has multi slice
	{
		VO_S32 used_len;
		VO_U8 *p_nal;
		VO_S32 nal_len;
		VO_U32 nuh_temporal_id_plus1;

		do {// at least 3byte sc(0x000001)

// 			if ( ret = DecodeNalu(p_dec, p_inbuf, buf_len, &used_len, &new_pic ) ) {
// 				return ret;//error
// 			}
            ret = Ebsp2Rbsp(p_slice, p_inbuf, buf_len, &p_nal,(VO_U32 *) &nal_len, &used_len );
			if ( ret ) {
				return ret;
			}
			p_slice->nal_unit_type		  = ( p_nal[ 0 ] >> 1 ) & 0x3f;

			//check nal type, make sure it is a slice nal
			VOASSERT(p_slice->nal_unit_type <= NAL_UNIT_CODED_SLICE_CRA); //exclude reserved types from 22 to 31

			nuh_temporal_id_plus1 = (p_nal[ 1 ] & 0x7);
			if(!nuh_temporal_id_plus1)
				VOH265ERROR(VO_H265_ERR_NALUERROR);
			--nuh_temporal_id_plus1;
			//skip start position and nalu header(2byte)
			InitBitStream( &p_slice->bs, p_nal + 2, nal_len - 2 );

			new_pic = VO_FALSE;

			p_slice->m_uiTLayer    = nuh_temporal_id_plus1;

			p_slice->first_slice_segment_in_pic_flag = READ_FLAG( &p_slice->bs, "first_slice_segment_in_pic_flag" );

			if ( p_slice->nal_unit_type >= NAL_UNIT_CODED_SLICE_BLA && \
				p_slice->nal_unit_type <=  NAL_UNIT_RESERVED_23 ) {
					READ_FLAG( &p_slice->bs, "no_output_of_prior_pics_flag" );
			}

			////////////////////////////////////////////////////YU_TBD
			//@Harry: check pps id, return error if it changed
			if (p_slice->slice_pic_parameter_set_id != READ_UEV( &p_slice->bs, "slice_pic_parameter_set_id" ))
			{
				return VO_ERR_DEC_H265_BASE;
			}

			if ( ParseSliceHeader( /*p_dec,*/ p_slice, &p_slice->bs, p_dec->rps, p_dec->m_prevPOC  ) )//p_dec,
			{
				return VO_ERR_DEC_H265_BASE;//YU_TBD: dedicated error code
			}

			//LHP Set 0 for BS
// 			if (!p_slice->slice_deblocking_filter_disabled_flag)
// 			{
// 				SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_slice->p_pBsVer, 0, sizeof( VO_U8 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY);
// 				SetMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_slice->p_pBsHor, 0, sizeof( VO_U8 ) * p_slice->PicWidthInMinTbsY * p_slice->PicHeightInMinTbsY);
// 			}

// 			VOASSERT(!p_slice->first_slice_segment_in_pic_flag);
			if (p_slice->first_slice_segment_in_pic_flag)
			{
				return VO_H265_ERR_NALUERROR;
			}

			/* Slice level initlization, not dependent slice*/
			if ( !p_slice->dependent_slice_segment_flag ) {
				ret = setRefPicList( p_slice, &p_slice->dpb_list_s); 
        
        if(ret)
          return ret;
#if 0
				// For generalized B
				// note: maybe not existed case (always L0 is copied to L1 if L1 is empty)//YU_TBD check spec
				if (p_slice->slice_type == B_SLICE && p_slice->m_aiNumRefIdx[REF_PIC_LIST_1] == 0) {
					VO_S32 iNumRefIdx = p_slice->m_aiNumRefIdx[REF_PIC_LIST_0];
					VO_S32 iRefIdx;

					p_slice->m_aiNumRefIdx[REF_PIC_LIST_1] = iNumRefIdx;

					for ( iRefIdx = 0; iRefIdx < iNumRefIdx; iRefIdx++) {
						p_slice->m_apcRefPicList[REF_PIC_LIST_1][iRefIdx] = p_slice->m_apcRefPicList[REF_PIC_LIST_0][iRefIdx];
					}
				}
#endif
				//use to drive listCol, YU_TBD: different from spec 8.5.3.2.8	Derivation process for collocated motion vectors
        if (p_slice->slice_type != I_SLICE) //fix bug:#30905
        {
          VO_BOOL bLowDelay = VO_TRUE;
          VO_S32  iCurrPOC  = p_slice->m_iPOC;
          VO_U32  iRefIdx ;
          for ( iRefIdx = 0; iRefIdx < p_slice->num_ref_idx[ 0 ]; iRefIdx++) {
            if ( p_slice->ref_pic_list[ 0 ][ iRefIdx ]->m_iPOC > iCurrPOC ) {
              bLowDelay = VO_FALSE;
              break;
            }
          }
          //use to drive listCol, YU_TBD: different from spec 8.5.3.2.8	Derivation process for collocated motion vectors
          if ( p_slice->slice_type == B_SLICE /*&& p_slice->enable_temporal_mvp_flag*/ ) {                 
            for ( iRefIdx = 0; iRefIdx < p_slice->num_ref_idx[ 1 ]; iRefIdx++) {
              if ( p_slice->ref_pic_list[ 1 ][ iRefIdx ]->m_iPOC > iCurrPOC ) {
                bLowDelay = VO_FALSE;
                break;
              }
            }             
          }
          p_slice->m_bCheckLDC = bLowDelay;  

        }
      }

			ret = DecodeSliceData( p_slice, &new_pic );
            if(ret)
              return ret;
			p_inbuf  += used_len;
			buf_len -= used_len;		
		}while ( buf_len > 3 );
	}

	if ( !new_pic ) {
LOGI("warning: not whole frame decoded!!!\n");
 		return VO_ERR_DEC_H265_BASE; //not whole frame decoded
	}
    //printf("decoded a frame POC = [%d].\n", p_slice->m_iPOC);
	return VO_ERR_NONE;
}

#else

/** Decode a nalue
* \param p_dec
* \returns error code
*/
VO_S32 DecodeNalu( H265_DEC* p_dec, 
	VO_U8 *p_inbuf, 
	VO_S32 inbuf_len, 
	VO_S32 *used_len, 
	VO_BOOL *p_new_pic )
{
	VO_S32 ret = VO_ERR_NONE;
	VO_U8 *p_nal;
	VO_U32 nal_len;
	VO_U32 nal_unit_type, nuh_temporal_id_plus1;

	if ( ret = Ebsp2Rbsp( p_inbuf, inbuf_len, &p_nal, &nal_len, used_len ) ) {
		return ret;
	}

	nal_unit_type		  = ( p_nal[ 0 ] >> 1 ) & 0x3f;
	nuh_temporal_id_plus1 = ( p_nal[ 1 ] & 0x7 ) - 1;

	//skip start position and nalu header(2byte)
	InitBitStream( &p_dec->bit_stream, p_nal + 2, nal_len - 2 );

	switch ( nal_unit_type ) {
	case NAL_UNIT_VPS:
		ret = ParseVPS( p_dec->vps, &p_dec->bit_stream );//YU_TBD: pass vps_array
		break;
	case NAL_UNIT_SPS:
		ret = ParseSPS( p_dec->sps, p_dec->rps, &p_dec->bit_stream );
		break;
	case NAL_UNIT_PPS:
		ret = ParsePPS( &p_dec->slice, p_dec->sps, p_dec->pps, &p_dec->bit_stream );
		break;
	case NAL_UNIT_SEI:
		///YU_TBD
		break;

	case NAL_UNIT_CODED_SLICE_TRAIL_R:
	case NAL_UNIT_CODED_SLICE_TRAIL_N:
	case NAL_UNIT_CODED_SLICE_TLA:
	case NAL_UNIT_CODED_SLICE_TSA_N:
	case NAL_UNIT_CODED_SLICE_STSA_R:
	case NAL_UNIT_CODED_SLICE_STSA_N:
	case NAL_UNIT_CODED_SLICE_BLA:
	case NAL_UNIT_CODED_SLICE_BLANT:
	case NAL_UNIT_CODED_SLICE_BLA_N_LP:
	case NAL_UNIT_CODED_SLICE_IDR:
	case NAL_UNIT_CODED_SLICE_IDR_N_LP:
	case NAL_UNIT_CODED_SLICE_CRA:
	case NAL_UNIT_CODED_SLICE_DLP:
	case NAL_UNIT_CODED_SLICE_TFD:
		ret = DecodeSlice( p_dec, &p_dec->slice, &p_dec->bit_stream, nal_unit_type, nuh_temporal_id_plus1, p_new_pic );
		break;
	default:
		break;
	}
	return ret;
}

#endif
