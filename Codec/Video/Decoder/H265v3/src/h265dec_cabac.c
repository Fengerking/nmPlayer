#include "h265dec_cabac.h"
#include "h265dec_sbac.h"
/*CABAC init Entry*/

// ====================================================================================================================
// Tables
// ====================================================================================================================

// initial probability for cu_transquant_bypass flag
static const VO_U8
INIT_CU_TRANSQUANT_BYPASS_FLAG[3][NUM_CU_TRANSQUANT_BYPASS_FLAG_CTX] =
{
  { 154 }, 
  { 154 }, 
  { 154 }, 
};

// initial probability for split flag
static const VO_U8 
INIT_SPLIT_FLAG[3][NUM_SPLIT_FLAG_CTX] =  
{ 

  { 107,  139,  126, },
  { 107,  139,  126, }, 
  { 139,  141,  157, }, 
};

static const VO_U8 
INIT_SKIP_FLAG[3][NUM_SKIP_FLAG_CTX] =  
{
  { 197,  185,  201, }, 
  { 197,  185,  201, }, 
  { CNU,  CNU,  CNU, }, 
};
#if 0
static const VO_U8 
INIT_ALF_CTRL_FLAG[3][NUM_ALF_CTRL_FLAG_CTX] = 
{
  { 102, }, 
  { 102, }, 
  { 118, }, 
};
#endif
static const VO_U8 
INIT_MERGE_FLAG_EXT[3][NUM_MERGE_FLAG_EXT_CTX] = 
{
  { 154, }, 
  { 110, }, 
  { CNU, }, 
};

static const VO_U8 
INIT_MERGE_IDX_EXT[3][NUM_MERGE_IDX_EXT_CTX] =  
{
  { 137, }, 
  { 122, }, 
  { CNU, }, 
};

static const VO_U8 
INIT_PART_SIZE[3][NUM_PART_SIZE_CTX] =  
{
  { 154,  139,  CNU,  CNU, }, 
  { 154,  139,  CNU,  CNU, }, 
  { 184,  CNU,  CNU,  CNU, }, 
};

static const VO_U8 
INIT_CU_AMP_POS[3][NUM_CU_AMP_CTX] =  
{
  { 154, }, 
  { 154, }, 
  { CNU, }, 
};

static const VO_U8 
INIT_PRED_MODE[3][NUM_PRED_MODE_CTX] = 
{
  { 134, }, 
  { 149, }, 
  { CNU, }, 
};

static const VO_U8 
INIT_INTRA_PRED_MODE[3][NUM_ADI_CTX] = 
{
  { 183, }, 
  { 154, }, 
  { 184, }, 
};

static const VO_U8 
INIT_CHROMA_PRED_MODE[3][NUM_CHROMA_PRED_CTX] = 
{
  { 152,  139, }, 
  { 152,  139, }, 
  {  63,  139, }, 
};

static const VO_U8 
INIT_INTER_DIR[3][NUM_INTER_DIR_CTX] = 
{
  {  95,   79,   63,   31,  31, }, 
  {  95,   79,   63,   31,  31, }, 
  { CNU,  CNU,  CNU,  CNU, CNU, }, 
};

static const VO_U8 
INIT_MVD[3][NUM_MV_RES_CTX] =  
{
  { 169,  198, }, 
  { 140,  198, }, 
  { CNU,  CNU, }, 
};

#if REF_IDX_BYPASS
static const VO_U8 
INIT_REF_PIC[3][NUM_REF_NO_CTX] =  
{
  { 153,  153 }, 
  { 153,  153 }, 
  { CNU,  CNU }, 
};
#else
static const VO_U8 
INIT_REF_PIC[3][NUM_REF_NO_CTX] =  
{
  { 153,  153,  168,  CNU, }, 
  { 153,  153,  139,  CNU, }, 
  { CNU,  CNU,  CNU,  CNU, }, 
};
#endif

static const VO_U8 
INIT_DQP[3][NUM_DELTA_QP_CTX] = 
{
  { 154,  154,  154, }, 
  { 154,  154,  154, }, 
  { 154,  154,  154, }, 
};

static const VO_U8 
INIT_QT_CBF[3][2*NUM_QT_CBF_CTX] =  
{
  { 153,  111,  CNU,  CNU,  CNU,  149,   92,  167,  CNU,  CNU, }, 
  { 153,  111,  CNU,  CNU,  CNU,  149,  107,  167,  CNU,  CNU, }, 
  { 111,  141,  CNU,  CNU,  CNU,   94,  138,  182,  CNU,  CNU, }, 
};

static const VO_U8 
INIT_QT_ROOT_CBF[3][NUM_QT_ROOT_CBF_CTX] = 
{
  {  79, }, 
  {  79, }, 
  { CNU, }, 
};

static const VO_U8 
INIT_LAST[3][2*NUM_CTX_LAST_FLAG_XY] =  
{
  { 125,  110,  124,  110,   95,   94,  125,  111,  111,   79,  125,  126,  111,  111,   79,
    108,  123,   93,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU, 
  }, 
  { 125,  110,   94,  110,   95,   79,  125,  111,  110,   78,  110,  111,  111,   95,   94,
    108,  123,  108,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,
  }, 
  { 110,  110,  124,  125,  140,  153,  125,  127,  140,  109,  111,  143,  127,  111,   79, 
    108,  123,   63,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU, 
  }, 
};

static const VO_U8 
INIT_SIG_CG_FLAG[3][2 * NUM_SIG_CG_FLAG_CTX] =  
{
  { 121,  140,  
    61,  154, 
  }, 
  { 121,  140, 
    61,  154, 
  }, 
  {  91,  171,  
    134,  141, 
  }, 
};

static const VO_U8 
INIT_SIG_FLAG[3][NUM_SIG_FLAG_CTX] = 
{
#if REMOVAL_8x2_2x8_CG
  { 170,  154,  139,  153,  139,  123,  123,   63,  124,  166,  183,  140,  136,  153,  154,  166,  183,  140,  136,  153,  154,  166,  183,  140,  136,  153,  154,  170,  153,  138,  138,  122,  121,  122,  121,  167,  151,  183,  140,  151,  183,  140,  }, 
  { 155,  154,  139,  153,  139,  123,  123,   63,  153,  166,  183,  140,  136,  153,  154,  166,  183,  140,  136,  153,  154,  166,  183,  140,  136,  153,  154,  170,  153,  123,  123,  107,  121,  107,  121,  167,  151,  183,  140,  151,  183,  140,  }, 
  { 111,  111,  125,  110,  110,   94,  124,  108,  124,  107,  125,  141,  179,  153,  125,  107,  125,  141,  179,  153,  125,  107,  125,  141,  179,  153,  125,  140,  139,  182,  182,  152,  136,  152,  136,  153,  136,  139,  111,  136,  139,  111,  }, 
#else
  { 170,      154,  139,  153,  139,  123,  123,   63,  124,     153, 153, 152, 152, 152, 137, 152, 137, 137,      166,  183,  140,  136,  153,  154,      170,     153, 138, 138,  122, 121,   122, 121,   167,     153,  167,  136,  121,  122,  136,  121,  122,   91,      151,  183,  140,  }, 
  { 155,      154,  139,  153,  139,  123,  123,   63,  153,     153, 153, 152, 152, 152, 137, 152, 137, 122,      166,  183,  140,  136,  153,  154,      170,     153, 123, 123,  107, 121,   107, 121,   167,     153,  167,  136,  149,  107,  136,  121,  122,   91,      151,  183,  140,  }, 
  { 111,      111,  125,  110,  110,   94,  124,  108,  124,     139, 139, 139, 168, 124, 138, 124, 138, 107,      107,  125,  141,  179,  153,  125,      140,     139, 182, 182,  152, 136,   152, 136,   153,     182,  137,  149,  192,  152,  224,  136,   31,  136,      136,  139,  111,  }, 
#endif
};

static const VO_U8 
INIT_ONE_FLAG[3][NUM_ONE_FLAG_CTX] = 
{
  { 154,  196,  167,  167,  154,  152,  167,  182,  182,  134,  149,  136,  153,  121,  136,  122,  169,  208,  166,  167,  154,  152,  167,  182, }, 
  { 154,  196,  196,  167,  154,  152,  167,  182,  182,  134,  149,  136,  153,  121,  136,  137,  169,  194,  166,  167,  154,  167,  137,  182, }, 
  { 140,   92,  137,  138,  140,  152,  138,  139,  153,   74,  149,   92,  139,  107,  122,  152,  140,  179,  166,  182,  140,  227,  122,  197, }, 
};

static const VO_U8 
INIT_ABS_FLAG[3][NUM_ABS_FLAG_CTX] =  
{
  { 107,  167,   91,  107,  107,  167, }, 
  { 107,  167,   91,  122,  107,  167, }, 
  { 138,  153,  136,  167,  152,  152, }, 
};

static const VO_U8 
INIT_MVP_IDX[3][NUM_MVP_IDX_CTX] =  
{
  { 168,  CNU, }, 
  { 168,  CNU, }, 
  { CNU,  CNU, }, 
};
#if 0
static const VO_U8 
INIT_ALF_FLAG[3][NUM_ALF_FLAG_CTX] = 
{
  { 153, }, 
  { 153, }, 
  { 153, }, 
};

static const VO_U8 
INIT_ALF_UVLC[3][NUM_ALF_UVLC_CTX] = 
{
  { 154,  154, }, 
  { 154,  154, }, 
  { 140,  154, }, 
};

static const VO_U8 
INIT_ALF_SVLC[3][NUM_ALF_SVLC_CTX] =  
{
  { 141,  154,  159, }, 
  { 141,  154,  189, }, 
  { 187,  154,  159, }, 
};
#endif
#if !SAO_ABS_BY_PASS
static const VO_U8 
INIT_SAO_UVLC[3][NUM_SAO_UVLC_CTX] =  
{
  { 200,  140, }, 
  { 185,  140, }, 
  { 143,  140, }, 
};
#endif
#if SAO_MERGE_ONE_CTX
static const VO_U8 
INIT_SAO_MERGE_FLAG[3][NUM_SAO_MERGE_FLAG_CTX] = 
{
  { 153,  }, 
  { 153,  }, 
  { 153,  }, 
};
#else
static const VO_U8 
INIT_SAO_MERGE_LEFT_FLAG[3][NUM_SAO_MERGE_LEFT_FLAG_CTX] = 
{
#if SAO_SINGLE_MERGE
  { 153, }, 
  { 153, }, 
  { 153, }, 
#else
  { 153,  153,  153, }, 
  { 153,  153,  153, }, 
  { 153,  153,  153, }, 
#endif
};

static const VO_U8 
INIT_SAO_MERGE_UP_FLAG[3][NUM_SAO_MERGE_UP_FLAG_CTX] = 
{
  { 153, }, 
  { 153, }, 
  { 175, }, 
};
#endif

static const VO_U8 
INIT_SAO_TYPE_IDX[3][NUM_SAO_TYPE_IDX_CTX] = 
{
  { 160, },
  { 185, },
  { 200, },
};

#if TRANS_SPLIT_FLAG_CTX_REDUCTION
static const VO_U8
INIT_TRANS_SUBDIV_FLAG[3][NUM_TRANS_SUBDIV_FLAG_CTX] =
{
  { 224,  167,  122, },
  { 124,  138,   94, },
  { 153,  138,  138, },
};
#else
static const VO_U8 
INIT_TRANS_SUBDIV_FLAG[3][NUM_TRANS_SUBDIV_FLAG_CTX] = 
{
{ CNU,  153,  138,  138,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU, }, 
{ CNU,  124,  138,   94,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU, }, 
{ CNU,  224,  167,  122,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU, }, 
};
#endif

static const VO_U8
INIT_TRANSFORMSKIP_FLAG[3][2*NUM_TRANSFORMSKIP_FLAG_CTX] = 
{
  { 139,  139}, 
  { 139,  139}, 
  { 139,  139}, 
};

static VOINLINE VO_VOID voCabacInitEntry( VO_U32 n, const VO_S32 qp, VO_U8 *pucState, const VO_U8 *pInitValue )
{
    VO_U32 i;
    assert( (qp >= 0) && (qp <= 51) );

    for( i=0; i<n; i++ )
	{
        VO_S32  initValue = pInitValue[i];
        // [9.2.1.1]
        VO_S32  slopeIdx   = ( initValue >> 4);
        VO_S32  intersecIdx= ( initValue & 15 );
        VO_S32  m          = slopeIdx * 5 - 45;
        VO_S32  n          = ( intersecIdx << 3 ) - 16;
        VO_S32  initState  =  Clip3( 1, 126, ( ( ( m * qp ) >> 4 ) + n ) );
        VO_U32  valMPS     = (initState >= 64 );
        pucState[i]     = (VO_U8)(( (valMPS ? (initState - 64) : (63 - initState)) <<1) + valMPS);
    }
}

VO_VOID voCabacTableInit(VO_S32 iQp,VO_U32 nSlice,VO_U8* table)
{
    VO_U8 *pucState = table;
#define INIT_CABAC( n, m, v ) \
	voCabacInitEntry( (m)*(n), iQp, pucState, (v)[nSlice] ); \
	pucState += (n)*(m); 
	
	INIT_CABAC( 1, NUM_SPLIT_FLAG_CTX,          INIT_SPLIT_FLAG         );
	INIT_CABAC( 1, NUM_SKIP_FLAG_CTX,           INIT_SKIP_FLAG          );
    INIT_CABAC( 1, NUM_MERGE_FLAG_EXT_CTX,      INIT_MERGE_FLAG_EXT     );
    INIT_CABAC( 1, NUM_MERGE_IDX_EXT_CTX,       INIT_MERGE_IDX_EXT      );
    INIT_CABAC( 1, NUM_PART_SIZE_CTX,           INIT_PART_SIZE          );
    INIT_CABAC( 1, NUM_CU_AMP_CTX,              INIT_CU_AMP_POS         );
    INIT_CABAC( 1, NUM_PRED_MODE_CTX,           INIT_PRED_MODE          );
    INIT_CABAC( 1, NUM_ADI_CTX,                 INIT_INTRA_PRED_MODE    );
    INIT_CABAC( 1, NUM_CHROMA_PRED_CTX,         INIT_CHROMA_PRED_MODE   );
    INIT_CABAC( 1, NUM_DELTA_QP_CTX,            INIT_DQP                );
    INIT_CABAC( 1, NUM_INTER_DIR_CTX,           INIT_INTER_DIR          );
    INIT_CABAC( 1, NUM_REF_NO_CTX,              INIT_REF_PIC            );
    INIT_CABAC( 1, NUM_MV_RES_CTX,              INIT_MVD                );
    INIT_CABAC( 2, NUM_QT_CBF_CTX,              INIT_QT_CBF             );
    INIT_CABAC( 1, NUM_TRANS_SUBDIV_FLAG_CTX,   INIT_TRANS_SUBDIV_FLAG  );
    INIT_CABAC( 1, NUM_QT_ROOT_CBF_CTX,         INIT_QT_ROOT_CBF        );
    INIT_CABAC( 2, NUM_SIG_CG_FLAG_CTX,         INIT_SIG_CG_FLAG        );
    INIT_CABAC( 1, NUM_SIG_FLAG_CTX,            INIT_SIG_FLAG           );
    INIT_CABAC( 2, NUM_CTX_LAST_FLAG_XY,        INIT_LAST               );
    INIT_CABAC( 2, NUM_CTX_LAST_FLAG_XY,        INIT_LAST               );
    INIT_CABAC( 1, NUM_ONE_FLAG_CTX,            INIT_ONE_FLAG           );
    INIT_CABAC( 1, NUM_ABS_FLAG_CTX,            INIT_ABS_FLAG           );
    INIT_CABAC( 1, NUM_MVP_IDX_CTX,             INIT_MVP_IDX            );
    INIT_CABAC( 1, NUM_SAO_MERGE_FLAG_CTX,      INIT_SAO_MERGE_FLAG     );
    INIT_CABAC( 1, NUM_SAO_TYPE_IDX_CTX,        INIT_SAO_TYPE_IDX       );
    INIT_CABAC( 2, NUM_TRANSFORMSKIP_FLAG_CTX,        INIT_TRANSFORMSKIP_FLAG        );
    INIT_CABAC( 1, NUM_CU_TRANSQUANT_BYPASS_FLAG_CTX, INIT_CU_TRANSQUANT_BYPASS_FLAG );
#undef INIT_CABAC
    return;
}


VO_VOID voCabacInit( H265_DEC_SLICE* p_slice, VO_S32 entry)
{
	H265_DEC_CABAC   *p_cabac  = &p_slice->cabac[entry];
	BIT_STREAM *bs = &p_cabac->bs;
	//BIT_STREAM *bs = &p_slice->bs;
	H265_DEC_PPS     *p_pps  = p_slice->p_pps;
	VO_U32           nSlice  = p_slice->slice_type ;
	VO_S32           iQp     = p_slice->slice_qp;
	
	VO_U8            *pucState = p_cabac->contextModels;
	VO_U32           nOffset = 0;
	VO_U8            *src_table;

	if(p_pps->cabac_init_present_flag&&p_slice->cabac_init_flag)
	{// remove in future, kept now, maybe for debug.   lina
		switch(nSlice)
		{
		case P_SLICE:
			nSlice = B_SLICE; 
			break;
		case B_SLICE:
			nSlice = P_SLICE; 
			break;
		default     :
			assert(0);
		}
	}
    src_table = p_slice->voCabacInitTab+nSlice*CABAC_TABLE_SIZE_SLICE+iQp*MAX_NUM_CTX_MOD;
	CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, pucState,src_table , MAX_NUM_CTX_MOD);
/*#define INIT_CABAC( n, m, v ) \
	voCabacInitEntry( (m)*(n), iQp, pucState, (v)[nSlice] ); \
	pucState += (n)*(m); \
	nOffset  += (n)*(m);

	assert( nOffset == OFF_SPLIT_FLAG_CTX );
	INIT_CABAC( 1, NUM_SPLIT_FLAG_CTX,          INIT_SPLIT_FLAG         );

	assert( nOffset == OFF_SKIP_FLAG_CTX );
	INIT_CABAC( 1, NUM_SKIP_FLAG_CTX,           INIT_SKIP_FLAG          );

    assert( nOffset == OFF_MERGE_FLAG_EXT_CTX );
    INIT_CABAC( 1, NUM_MERGE_FLAG_EXT_CTX,      INIT_MERGE_FLAG_EXT     );

    assert( nOffset == OFF_MERGE_IDX_EXT_CTX );
    INIT_CABAC( 1, NUM_MERGE_IDX_EXT_CTX,       INIT_MERGE_IDX_EXT      );

    assert( nOffset == OFF_PART_SIZE_CTX );
    INIT_CABAC( 1, NUM_PART_SIZE_CTX,           INIT_PART_SIZE          );

    assert( nOffset == OFF_CU_AMP_CTX );
    INIT_CABAC( 1, NUM_CU_AMP_CTX,              INIT_CU_AMP_POS         );

    assert( nOffset == OFF_PRED_MODE_CTX );
    INIT_CABAC( 1, NUM_PRED_MODE_CTX,           INIT_PRED_MODE          );
    
	assert( nOffset == OFF_INTRA_PRED_CTX );
    INIT_CABAC( 1, NUM_ADI_CTX,                 INIT_INTRA_PRED_MODE    );

	assert( nOffset == OFF_CHROMA_PRED_CTX );
    INIT_CABAC( 1, NUM_CHROMA_PRED_CTX,         INIT_CHROMA_PRED_MODE   );

	assert( nOffset == OFF_DELTA_QP_CTX );
    INIT_CABAC( 1, NUM_DELTA_QP_CTX,            INIT_DQP                );

	assert( nOffset == OFF_INTER_DIR_CTX );
    INIT_CABAC( 1, NUM_INTER_DIR_CTX,           INIT_INTER_DIR          );

	assert( nOffset == OFF_REF_PIC_CTX );
    INIT_CABAC( 1, NUM_REF_NO_CTX,              INIT_REF_PIC            );

	assert( nOffset == OFF_MVD_CTX );
    INIT_CABAC( 1, NUM_MV_RES_CTX,              INIT_MVD                );

    assert( nOffset == OFF_QT_CBF_CTX );
    INIT_CABAC( 2, NUM_QT_CBF_CTX,              INIT_QT_CBF             );

	assert( nOffset == OFF_TRANS_SUBDIV_FLAG_CTX );
    INIT_CABAC( 1, NUM_TRANS_SUBDIV_FLAG_CTX,   INIT_TRANS_SUBDIV_FLAG  );

    assert( nOffset == OFF_QT_ROOT_CBF_CTX );
    INIT_CABAC( 1, NUM_QT_ROOT_CBF_CTX,         INIT_QT_ROOT_CBF        );

	assert( nOffset == OFF_SIG_CG_FLAG_CTX );
    INIT_CABAC( 2, NUM_SIG_CG_FLAG_CTX,         INIT_SIG_CG_FLAG        );

	assert( nOffset == OFF_SIG_FLAG_CTX );
    INIT_CABAC( 1, NUM_SIG_FLAG_CTX,            INIT_SIG_FLAG           );

	assert( nOffset == OFF_LAST_X_CTX );
    INIT_CABAC( 2, NUM_CTX_LAST_FLAG_XY,        INIT_LAST               );
    
	assert( nOffset == OFF_LAST_Y_CTX );
    INIT_CABAC( 2, NUM_CTX_LAST_FLAG_XY,        INIT_LAST               );

	assert( nOffset == OFF_ONE_FLAG_CTX );
    INIT_CABAC( 1, NUM_ONE_FLAG_CTX,            INIT_ONE_FLAG           );
    
	assert( nOffset == OFF_ABS_FLAG_CTX );
    INIT_CABAC( 1, NUM_ABS_FLAG_CTX,            INIT_ABS_FLAG           );
	
    assert( nOffset == OFF_MVP_IDX_CTX );
    INIT_CABAC( 1, NUM_MVP_IDX_CTX,             INIT_MVP_IDX            );

	assert(OFF_SAO_MERGE_FLAG_CTX);
    INIT_CABAC( 1, NUM_SAO_MERGE_FLAG_CTX,      INIT_SAO_MERGE_FLAG     );

    assert(OFF_SAO_TYPE_IDX_CTX);
    INIT_CABAC( 1, NUM_SAO_TYPE_IDX_CTX,        INIT_SAO_TYPE_IDX       );
	
	assert(OFF_TRANSFORMSKIP_FLAG_CTX);
    INIT_CABAC( 2, NUM_TRANSFORMSKIP_FLAG_CTX,        INIT_TRANSFORMSKIP_FLAG        );

	assert(OFF_CU_TRANSQUANT_BYPASS_FLAG_CTX);
    INIT_CABAC( 1, NUM_CU_TRANSQUANT_BYPASS_FLAG_CTX, INIT_CU_TRANSQUANT_BYPASS_FLAG );
#undef INIT_CABAC
*/
    //assert( nOffset < MAX_NUM_CTX_MOD );


    bs->first_byte = bs->p_bit_ptr - 4 -  bs->bit_pos/8 - bs->p_bit_start; //to check, lina       
    
	vo_init_cabac_decoder(p_cabac,
      bs->p_bit_ptr - 4 -  bs->bit_pos/8,
      bs->p_bit_end - bs->p_bit_ptr + 4 +  bs->bit_pos/8);

}

VO_VOID voCabacInitExt(  H265_DEC_SLICE *p_slice, VO_S32 entry)
{

  H265_DEC_CABAC   *p_cabac  = &p_slice->cabac[entry];
  BIT_STREAM *bs = &p_cabac->bs;
  //BIT_STREAM *bs = &p_slice->bs;
  //h->bit_stream.first_byte = h->bit_stream.p_bit_ptr - 4 -  h->bit_stream.bit_pos/8 - h->bit_stream.p_bit_start;   //to check, lina    
  bs->first_byte = bs->p_bit_ptr - 4 -  bs->bit_pos/8 - bs->p_bit_start; //to check, lina   
  vo_init_cabac_decoder(p_cabac,
    bs->p_bit_ptr - 4 -  bs->bit_pos/8,
    bs->p_bit_end - bs->p_bit_ptr + 4 +  bs->bit_pos/8);

}

VO_U32 ParseSaoTypeIdx( H265_DEC_CABAC *p_cabac/*,BIT_STREAM *p_bs*/ )
{
    if ( voCabacDecodeBin( p_cabac, /*p_bs,*/ OFF_SAO_TYPE_IDX_CTX ) == 0 )
        return SAO_NONE;

    if ( voCabacDecodeBinEP( p_cabac/*, p_bs*/ ) == 0 )
         return  SAO_BO;
    return SAO_EO_0;
}

VO_U32 ParserSaoOffsetAbs( H265_DEC_CABAC *p_cabac/*,*/
    /*BIT_STREAM *p_bs*/ )
{
    VO_U32 i = 0;

	//Only support 8 bit_depth
    while ( i < 7 && voCabacDecodeBinEP( p_cabac/*, p_bs*/ ) )
        i++;
    return i;
}

VOINLINE VO_BOOL DeriveZsBlockAvail( H265_DEC_SLICE * p_slice, 
    const VO_S32 xCb, 
    const VO_S32 yCb, 
    const VO_S32 xNbY, 
    const VO_S32 yNbY)
{
    VO_S32 minBlockAddrCurr = p_slice->p_pps->MinTbAddrZs[(VO_S32)(yCb * p_slice->p_pps->MinTbAddrZsStride + xCb)];
    VO_S32 minBlockAddrN = p_slice->p_pps->MinTbAddrZs[(VO_S32)(yNbY * p_slice->p_pps->MinTbAddrZsStride + xNbY)];
    if (/*minBlockAddrN < 0 ||*/ 
        minBlockAddrN > minBlockAddrCurr ||
        minBlockAddrN < p_slice->SliceMinTbAddrZs || 
        minBlockAddrN < p_slice->p_pps->tile_start_zs[p_slice->currEntryNum]){
            return VO_FALSE;
    } else {
        return VO_TRUE;
    }
}
VO_U32 GetCtxSplitInc( H265_DEC_SLICE* p_slice, 
    VO_U32 depth ,
    VO_S32 x,
    VO_S32 y)
{
    VO_U32 ctx = 0;

    //YU_TBD: support tile and slice
    // 	if ( x > 0 ) {/*x > p_slice->tile_start_x &&slice_number[-1]==p_slice->cur_slice_number */
    // 	    ctx = p_slice->left_depth_buffer[ ( y >> p_slice->MinCbLog2SizeY ) & p_slice->log2_diff_max_min_luma_mask] > depth;
    // 	}
    // 
    // 	if ( y > 0 ) {/*y > p_slice->tile_start_y &&slice_number[-p_dec->picWidthInMin4bs]==p_slice->cur_slice_number*/
    // 	    ctx += p_slice->top_depth_buffer[ x >> p_slice->MinCbLog2SizeY ] > depth;
    // 	}

    VO_U32 xCb = (x >> 2);//p_slice->MinTbLog2SizeY
    VO_U32 yCb = (y >> 2);//p_slice->MinTbLog2SizeY
    VO_BOOL lefAvail = DeriveZsBlockAvail(p_slice, xCb, yCb, xCb - 1, yCb);
    if (lefAvail) {
        ctx = p_slice->left_depth_buffer[ ( y >> p_slice->MinCbLog2SizeY ) & p_slice->log2_diff_max_min_luma_mask] > depth;
    }

    //if ( y > 0 ) {/*y > p_slice->tile_start_y &&slice_number[-p_dec->picWidthInMin4bs]==p_slice->cur_slice_number*/
    if (DeriveZsBlockAvail(p_slice, xCb, yCb, xCb, yCb - 1)) {
        ctx += p_slice->top_depth_buffer[ x >> p_slice->MinCbLog2SizeY ] > depth;
    }
    return ctx;    
}


VO_U32 GetCtxSkipFlag( H265_DEC_SLICE* p_slice, 
    VO_S32 x,
    VO_S32 y)
{
    VO_U32 ctx = 0;

    //YU_TBD: support tile and slice
    // 	if ( x > 0 ) {/*x > p_slice->tile_start_x &&slice_number[-1]==p_slice->cur_slice_number */
    // 	    ctx = p_slice->left_skip_buffer[ ( y >> p_slice->MinCbLog2SizeY ) & p_slice->log2_diff_max_min_luma_mask];
    // 	}
    // 
    // 	if ( y > 0 ) {/*y > p_slice->tile_start_y &&slice_number[-p_dec->picWidthInMin4bs]==p_slice->cur_slice_number*/
    // 	    ctx += p_slice->top_skip_buffer[ x >> p_slice->MinCbLog2SizeY ];
    // 	}
    VO_U32 xCb = (x >> 2);//p_slice->MinTbLog2SizeY
    VO_U32 yCb = (y >> 2);//p_slice->MinTbLog2SizeY
    VO_BOOL lefAvail = DeriveZsBlockAvail(p_slice, xCb, yCb, xCb - 1, yCb);
    if (lefAvail) {
        ctx = p_slice->left_skip_buffer[ ( y >> p_slice->MinCbLog2SizeY ) & p_slice->log2_diff_max_min_luma_mask];
    }

    //if ( y > 0 ) {/*y > p_slice->tile_start_y &&slice_number[-p_dec->picWidthInMin4bs]==p_slice->cur_slice_number*/
    if (DeriveZsBlockAvail(p_slice, xCb, yCb, xCb, yCb - 1)) {
        ctx += p_slice->top_skip_buffer[ x >> p_slice->MinCbLog2SizeY ];
    }

    return ctx;  
}

VO_U32 parseMergeIndex( H265_DEC_SLICE* p_slice,
	H265_DEC_CABAC *p_cabac/*,*/
	/*BIT_STREAM *p_bs*/ )
{
    VO_U32 uiNumCand;
    VO_U32 uiUnaryIdx;

    if ( !voCabacDecodeBin( p_cabac, /*p_bs ,*/ OFF_MERGE_IDX_EXT_CTX ) )
		return 0;

	uiNumCand = p_slice->max_num_merge_cand - 1;
    for ( uiUnaryIdx = 1; uiUnaryIdx < uiNumCand; ++uiUnaryIdx ) {
      if ( !voCabacDecodeBinEP( p_cabac/*, p_bs*/ ) )
          break;
    }

   return uiUnaryIdx;
}

VO_U32 parseRefFrmIdx( H265_DEC_SLICE *p_slice,
	H265_DEC_CABAC *p_cabac,
	VO_U32 max_ref_num )
{
	VO_U32 i = 0;
    VO_U32 max = max_ref_num - 1;
    VO_U32 max_ctx = MIN( max, 2 );
    
    while ( ( i < max_ctx ) && voCabacDecodeBin( p_cabac,  OFF_REF_PIC_CTX + i ) )
        i++;

    if ( i == 2 ) {
        while ( ( i < max ) && voCabacDecodeBinEP( p_cabac ) )
            i++;
    }
    return i;
}
