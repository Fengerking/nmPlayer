#include "voVP8DecReadDctTokens.h"
#include "voVP8DecMBlock.h"
#include "voVP8DecFrame.h"
#include "voVP8Memory.h"

//#define	CHAR_BIT 8
extern const int vp8_default_zig_zag1d[16];
extern const unsigned int vp8dx_bitreader_norm[256];

//const int vp8_block2left[25] =    { 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 0, 0, 1, 1, 0, 0, 1, 1, 0};
//const int vp8_block2above[25] =   { 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 0, 1, 0, 1, 0, 1, 0};
//const int vp8_block2context[25] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3};

const unsigned char vp8_block2left[25] =
{
    0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8
};
const unsigned char vp8_block2above[25] =
{
    0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 4, 5, 6, 7, 6, 7, 8
};

//static const TOKENEXTRABITS vp8d_token_extra_bits2[MAX_ENTROPY_TOKENS] =
//{
//    {  0, -1, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },  //ZERO_TOKEN
//    {  1, 0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //ONE_TOKEN
//    {  2, 0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //TWO_TOKEN
//    {  3, 0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //THREE_TOKEN
//    {  4, 0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //FOUR_TOKEN
//    {  5, 0, { 159, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },  //DCT_VAL_CATEGORY1
//    {  7, 1, { 145, 165, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } }, //DCT_VAL_CATEGORY2
//    { 11, 2, { 140, 148, 173, 0,  0,  0,  0,  0,  0,  0,  0,  0   } }, //DCT_VAL_CATEGORY3
//    { 19, 3, { 135, 140, 155, 176, 0,  0,  0,  0,  0,  0,  0,  0   } }, //DCT_VAL_CATEGORY4
//    { 35, 4, { 130, 134, 141, 157, 180, 0,  0,  0,  0,  0,  0,  0   } }, //DCT_VAL_CATEGORY5
//    { 67, 10, { 129, 130, 133, 140, 153, 177, 196, 230, 243, 254, 254, 0   } }, //DCT_VAL_CATEGORY6
//    {  0, -1, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },  // EOB TOKEN
//};
//static const TOKENEXTRABITS vp8d_token_extra_bits2[MAX_ENTROPY_TOKENS] =
//{
//    {  0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },  //ZERO_TOKEN
//    {  1, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //ONE_TOKEN
//    {  2, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //TWO_TOKEN
//    {  3, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //THREE_TOKEN
//    {  4, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //FOUR_TOKEN
//    {  5, { 159, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },  //DCT_VAL_CATEGORY1
//    {  7, { 145, 165, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } }, //DCT_VAL_CATEGORY2
//    { 11, { 140, 148, 173, 0,  0,  0,  0,  0,  0,  0,  0,  0   } }, //DCT_VAL_CATEGORY3
//    { 19, { 135, 140, 155, 176, 0,  0,  0,  0,  0,  0,  0,  0   } }, //DCT_VAL_CATEGORY4
//    { 35, { 130, 134, 141, 157, 180, 0,  0,  0,  0,  0,  0,  0   } }, //DCT_VAL_CATEGORY5
//    { 67, { 129, 130, 133, 140, 153, 177, 196, 230, 243, 254, 254, 0   } }, //DCT_VAL_CATEGORY6
//    {  0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },  // EOB TOKEN
//};

//static const unsigned char* vp8d_token_extra_bits2[MAX_ENTROPY_TOKENS] =
//{
//    {  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    },  //ZERO_TOKEN
//    {  1, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    },   //ONE_TOKEN
//    {  2, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    },   //TWO_TOKEN
//    {  3, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    },   //THREE_TOKEN
//    {  4, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    },   //FOUR_TOKEN
//    {  5, 159, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    },  //DCT_VAL_CATEGORY1
//    {  7, 145, 165, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0    }, //DCT_VAL_CATEGORY2
//    { 11, 140, 148, 173, 0,  0,  0,  0,  0,  0,  0,  0,  0    }, //DCT_VAL_CATEGORY3
//    { 19, 135, 140, 155, 176, 0,  0,  0,  0,  0,  0,  0,  0    }, //DCT_VAL_CATEGORY4
//    { 35, 130, 134, 141, 157, 180, 0,  0,  0,  0,  0,  0,  0    }, //DCT_VAL_CATEGORY5
//    { 67, 129, 130, 133, 140, 153, 177, 196, 230, 243, 254, 254, 0    }, //DCT_VAL_CATEGORY6
//    {  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0    },  // EOB TOKEN
//};

//static const unsigned char vp8d_token_extra_bits2_cat6[12]={ 67, 129, 130, 133, 140, 153, 177, 196, 230, 243, 254, 254}; //DCT_VAL_CATEGORY6
//static const unsigned char vp8d_token_extra_bits2_cat5[12]={ 35, 130, 134, 141, 157, 180, 0,  0,  0,  0,  0,  0};//DCT_VAL_CATEGORY5
//static const unsigned char vp8d_token_extra_bits2_cat4[12]={ 19, 135, 140, 155, 176, 0,  0,  0,  0,  0,  0,  0};//DCT_VAL_CATEGORY4
//static const unsigned char vp8d_token_extra_bits2_cat3[12]={ 11, 140, 148, 173, 0,  0,  0,  0,  0,  0,  0,  0};//DCT_VAL_CATEGORY3
//static const unsigned char vp8d_token_extra_bits2_cat2[12]={  7, 145, 165, 0,  0,  0,  0,  0,  0,  0,  0,  0}; //DCT_VAL_CATEGORY2
//static const unsigned char vp8d_token_extra_bits2_cat1[12]={  5, 159, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0}; //DCT_VAL_CATEGORY1

static const unsigned char vp8d_token_extra_bits2_cat6[12]={67,254,254,243,230,196,177,153,140,133,130,129}; //DCT_VAL_CATEGORY6
static const unsigned char vp8d_token_extra_bits2_cat5[6]={35,180,157,141,134,130};//DCT_VAL_CATEGORY5
static const unsigned char vp8d_token_extra_bits2_cat4[5]={19,176,155,140,135};//DCT_VAL_CATEGORY4
static const unsigned char vp8d_token_extra_bits2_cat3[4]={11,173,148,140};//DCT_VAL_CATEGORY3
static const unsigned char vp8d_token_extra_bits2_cat2[3]={7,165,145}; //DCT_VAL_CATEGORY2
static const unsigned char vp8d_token_extra_bits2_cat1[2]={5,159}; //DCT_VAL_CATEGORY1


#define OCB_X PREV_COEF_CONTEXTS * ENTROPY_NODES
unsigned short vp8_coef_bands_x[16]= { 0, 1 * OCB_X, 2 * OCB_X, 3 * OCB_X, 6 * OCB_X, 4 * OCB_X, 5 * OCB_X, 6 * OCB_X, 6 * OCB_X, 6 * OCB_X, 6 * OCB_X, 6 * OCB_X, 6 * OCB_X, 6 * OCB_X, 6 * OCB_X, 7 * OCB_X};

#define FILL \
    if(count < 0) \
        VP8DX_BOOL_DECODER_FILL(count, value, bufptr, bufend, pos);

#define NORMALIZE \
    if(range < 0x80)                            \
    { \
        shift = vp8dx_bitreader_norm[range]; \
        range <<= shift; \
        value <<= shift; \
        count -= shift; \
    }

#define DECODE_AND_APPLYSIGN(value_to_sign) \
    split = (range + 1) >> 1; \
    bigsplit = (VP8_BD_VALUE)split << (VP8_BD_VALUE_SIZE - 8); \
    FILL \
    if ( value < bigsplit ) \
    { \
        range = split; \
        v= value_to_sign; \
    } \
    else \
    { \
        range = range-split; \
        value = value-bigsplit; \
        v = -value_to_sign; \
    } \
    range +=range;                   \
    value +=value;                   \
    count--;

#define DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val) \
    DECODE_AND_APPLYSIGN(val) \
    Prob = coef_probs + (ENTROPY_NODES*2); \
    if(c < 15){\
        qcoeff_ptr [ scan[c] ] = (signed short) v; \
        ++c; \
        goto DO_WHILE; }\
    qcoeff_ptr [ scan[15] ] = (signed short) v; \
    goto BLOCK_FINISHED;

#define DECODE_AND_LOOP_IF_ZERO(probability,branch) \
    { \
        split = 1 + ((( probability*(range-1) ) ) >> 8); \
        bigsplit = (VP8_BD_VALUE)split << (VP8_BD_VALUE_SIZE - 8); \
        FILL \
        if ( value < bigsplit ) \
        { \
            range = split; \
            NORMALIZE \
            Prob = coef_probs; \
            if(c<15) {\
            ++c; \
            Prob += vp8_coef_bands_x[c]; \
            goto branch; \
            } goto BLOCK_FINISHED; /*for malformed input */\
        } \
        value -= bigsplit; \
        range = range - split; \
        NORMALIZE \
    }

#define DECODE_AND_BRANCH_IF_ZERO(probability,branch) \
    { \
        split = 1 +  ((( probability*(range-1) ) )>> 8); \
        bigsplit = (VP8_BD_VALUE)split << (VP8_BD_VALUE_SIZE - 8); \
        FILL \
        if ( value < bigsplit ) \
        { \
            range = split; \
            NORMALIZE \
            goto branch; \
        } \
        value -= bigsplit; \
        range = range - split; \
        NORMALIZE \
    }

#define DECODE_EXTRABIT_AND_ADJUST_VAL(table,index,bits_count)\
    split = 1 +  (((range-1) * table[index]) >> 8); \
    bigsplit = (VP8_BD_VALUE)split << (VP8_BD_VALUE_SIZE - 8); \
    FILL \
    if(value >= bigsplit)\
    {\
        range = range-split;\
        value = value-bigsplit;\
        val += ((unsigned short)1<<bits_count);\
    }\
    else\
    {\
        range = split;\
    }\
    NORMALIZE

void stop_token_decoder(VP8D_COMP *pbi)
{
    VP8_COMMON *pc = &pbi->common;

    if (pc->multi_token_partition != ONE_PARTITION)
		FreeMem(pbi->pUserData, pbi->nCodecIdx, pbi->mbc);
}
int vp8_decode_mb_tokens(VP8D_COMP *dx, MACROBLOCKD *x,MODE_INFO* pModeInfo)
{
	ENTROPY_CONTEXT *A = (ENTROPY_CONTEXT *)x->above_context;
    ENTROPY_CONTEXT *L = (ENTROPY_CONTEXT *)x->left_context;
    const VP8_COMMON *const oc = & dx->common;

    BOOL_DECODER *bc = x->current_bc;

    ENTROPY_CONTEXT *a;
    ENTROPY_CONTEXT *l;
    int i;//ia=0,il=0;

    int eobtotal = 0;

    register int count;

    const unsigned char *bufptr;
    const unsigned char *bufend;
    register unsigned int range;
	unsigned int pos;
    VP8_BD_VALUE value;
    const int *scan;
    register unsigned int shift;
    unsigned int  split;
    VP8_BD_VALUE bigsplit;
    signed short *qcoeff_ptr;

    const unsigned char *coef_probs;
    int type;
    int stop;
    signed short val;
    signed short c;
    signed short t;
    signed short v;
    const unsigned char *Prob;

	int mflag=0;

    //int *scan;
    type = 3;   
    stop = 16;
	i = 0;

    if (pModeInfo->mbmi.mode != B_PRED && pModeInfo->mbmi.mode != SPLITMV)
    {
        i = 24;
        stop = 24;
        type = 1;
        qcoeff_ptr = &x->qcoeff[24*16];
        scan = vp8_default_zig_zag1d;
        eobtotal -= 16;
    }
    else
    {
        scan = vp8_default_zig_zag1d;
        qcoeff_ptr = &x->qcoeff[0];
    }

    bufend  = bc->user_buffer_end;
    bufptr  = bc->user_buffer;
    value   = bc->value;
    count   = bc->count;
    range   = bc->range;
	pos      = bc->pos;
#ifdef STABILITY
	if(pos >= (unsigned int)(bufend-bufptr))  //414
		mflag =1;
#endif

    coef_probs = oc->fc.coef_probs [type] [0] [0];

BLOCK_LOOP:
	a = A + vp8_block2above[i];
    l = L + vp8_block2left[i];
    c = (signed short)(!type);

    VP8_COMBINEENTROPYCONTEXTS(t, *a, *l);
    Prob = coef_probs;
    Prob += t * ENTROPY_NODES;

DO_WHILE:
    Prob += vp8_coef_bands_x[c];
    DECODE_AND_BRANCH_IF_ZERO(Prob[EOB_CONTEXT_NODE], BLOCK_FINISHED);

CHECK_0_:
    DECODE_AND_LOOP_IF_ZERO(Prob[ZERO_CONTEXT_NODE], CHECK_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[ONE_CONTEXT_NODE], ONE_CONTEXT_NODE_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[LOW_VAL_CONTEXT_NODE], LOW_VAL_CONTEXT_NODE_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[HIGH_LOW_CONTEXT_NODE], HIGH_LOW_CONTEXT_NODE_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[CAT_THREEFOUR_CONTEXT_NODE], CAT_THREEFOUR_CONTEXT_NODE_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[CAT_FIVE_CONTEXT_NODE], CAT_FIVE_CONTEXT_NODE_0_);
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY6].min_val;

	val=vp8d_token_extra_bits2_cat6[0];
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 1,10);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 2,9);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 3,8);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 4,7);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 5,6);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 6,5);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 7,4);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 8,3);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 9,2);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 10,1);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 11,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

CAT_FIVE_CONTEXT_NODE_0_:
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY5].min_val;
	val=vp8d_token_extra_bits2_cat5[0];
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat5, 1,4);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat5, 2,3);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat5, 3,2);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat5, 4,1);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat5, 5,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

CAT_THREEFOUR_CONTEXT_NODE_0_:
    DECODE_AND_BRANCH_IF_ZERO(Prob[CAT_THREE_CONTEXT_NODE], CAT_THREE_CONTEXT_NODE_0_);
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY4].min_val;
	val=vp8d_token_extra_bits2_cat4[0];
    DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat4, 1,3);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat4, 2,2);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat4, 3,1);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat4, 4,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

CAT_THREE_CONTEXT_NODE_0_:
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY3].min_val;
	val=vp8d_token_extra_bits2_cat3[0];
    DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat3, 1,2);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat3, 2,1);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat3, 3,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

HIGH_LOW_CONTEXT_NODE_0_:
    DECODE_AND_BRANCH_IF_ZERO(Prob[CAT_ONE_CONTEXT_NODE], CAT_ONE_CONTEXT_NODE_0_);
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY2].min_val;
	val=vp8d_token_extra_bits2_cat2[0];
    DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat2, 1,1);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat2, 2,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

CAT_ONE_CONTEXT_NODE_0_:
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY1].min_val;
	val=vp8d_token_extra_bits2_cat1[0];
    DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat1,1,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

LOW_VAL_CONTEXT_NODE_0_:
    DECODE_AND_BRANCH_IF_ZERO(Prob[TWO_CONTEXT_NODE], TWO_CONTEXT_NODE_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[THREE_CONTEXT_NODE], THREE_CONTEXT_NODE_0_);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(4);

THREE_CONTEXT_NODE_0_:
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(3);

TWO_CONTEXT_NODE_0_:
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(2);

ONE_CONTEXT_NODE_0_:
    DECODE_AND_APPLYSIGN(1);
    Prob = coef_probs + ENTROPY_NODES;

    if (c < 15)
    {
        qcoeff_ptr [ scan[c] ] = (signed short) v;
        ++c;
        goto DO_WHILE;
    }
    qcoeff_ptr [ scan[15] ] = (signed short) v;

BLOCK_FINISHED:
	t = ((x->eob[i] = (char)c) != !type);   // any nonzero data?
    eobtotal += x->eob[i];
	*a = *l = (ENTROPY_CONTEXT)t;
    qcoeff_ptr += 16;

    i++;

    if (i < stop)
        goto BLOCK_LOOP;

    if (i == 25)
    {
        scan = vp8_default_zig_zag1d;//x->scan_order1d;
        type = 0;
        i = 0;
        stop = 16;
        coef_probs = oc->fc.coef_probs [type] [ 0 ] [0];
        qcoeff_ptr = &x->qcoeff[0];
        goto BLOCK_LOOP;
    }

    if (i == 16)
    {
        type = 2;
        coef_probs = oc->fc.coef_probs [type] [ 0 ] [0];
        stop = 24;
        goto BLOCK_LOOP;
    }

    FILL
    bc->value = value;
    bc->count = count;
    bc->range = range;
	bc->pos    = pos;

#ifdef STABILITY
	if(mflag==1 && eobtotal==0) //414
		return -1;
#endif

    return eobtotal;
}

int vp8_decode_mb_tokens_new(VP8D_COMP *dx, MACROBLOCKD *x,MACROBLOCKD* xdcol,BOOL_DECODER *bc,MODE_INFO *pModeInfo)
{
	ENTROPY_CONTEXT *A = (ENTROPY_CONTEXT *)x->above_context;
    ENTROPY_CONTEXT *L = (ENTROPY_CONTEXT *)x->left_context;
    const VP8_COMMON *const oc = & dx->common;

    //BOOL_DECODER *bc = x->current_bc;

	//MACROBLOCKD* x2 = xdcol;//&dx->mbrow[mb_col];

    ENTROPY_CONTEXT *a;
    ENTROPY_CONTEXT *l;
    int i;//ia=0,il=0;

    int eobtotal = 0;

    register int count;

    const unsigned char *bufptr;
    const unsigned char *bufend;
    register unsigned int range;
	unsigned int pos;
    VP8_BD_VALUE value;
    const int *scan;
    register unsigned int shift;
    unsigned int  split;
    VP8_BD_VALUE bigsplit;
    signed short *qcoeff_ptr;

    const unsigned char *coef_probs;
    int type;
    int stop;
    signed short val;
    signed short c;
    signed short t;
    signed short v;
    const unsigned char *Prob;

	int mflag=0;

    //int *scan;
    type = 3;   
    stop = 16;
	i = 0;

    if (pModeInfo->mbmi.mode != B_PRED && pModeInfo->mbmi.mode != SPLITMV)
    {
        i = 24;
        stop = 24;
        type = 1;

		qcoeff_ptr = &xdcol->qcoeff[24*16];
#if 0
        qcoeff_ptr = &x->qcoeff[24*16];
#endif
        scan = vp8_default_zig_zag1d;
        eobtotal -= 16;
    }
    else
    {
        scan = vp8_default_zig_zag1d;
		qcoeff_ptr = &xdcol->qcoeff[0];
#if 0
        qcoeff_ptr = &x->qcoeff[0];
#endif
    }

    bufend  = bc->user_buffer_end;
    bufptr  = bc->user_buffer;
    value   = bc->value;
    count   = bc->count;
    range   = bc->range;
	pos      = bc->pos;
#ifdef STABILITY
	if(pos >= (unsigned int)(bufend-bufptr))  //414
		mflag =1;
#endif

    coef_probs = oc->fc.coef_probs [type] [0] [0];

BLOCK_LOOP:
	//a =  A[ vp8_block2context[i] ] + vp8_block2above[i];
	//l  =  L[ vp8_block2context[i] ] + vp8_block2left[i];
	a = A + vp8_block2above[i];
    l = L + vp8_block2left[i];
    c = (signed short)(!type);

    VP8_COMBINEENTROPYCONTEXTS(t, *a, *l);
    Prob = coef_probs;
    Prob += t * ENTROPY_NODES;

DO_WHILE:
    Prob += vp8_coef_bands_x[c];
    DECODE_AND_BRANCH_IF_ZERO(Prob[EOB_CONTEXT_NODE], BLOCK_FINISHED);

CHECK_0_:
    DECODE_AND_LOOP_IF_ZERO(Prob[ZERO_CONTEXT_NODE], CHECK_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[ONE_CONTEXT_NODE], ONE_CONTEXT_NODE_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[LOW_VAL_CONTEXT_NODE], LOW_VAL_CONTEXT_NODE_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[HIGH_LOW_CONTEXT_NODE], HIGH_LOW_CONTEXT_NODE_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[CAT_THREEFOUR_CONTEXT_NODE], CAT_THREEFOUR_CONTEXT_NODE_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[CAT_FIVE_CONTEXT_NODE], CAT_FIVE_CONTEXT_NODE_0_);
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY6].min_val;

	val=vp8d_token_extra_bits2_cat6[0];
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 1,10);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 2,9);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 3,8);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 4,7);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 5,6);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 6,5);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 7,4);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 8,3);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 9,2);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 10,1);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat6, 11,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

CAT_FIVE_CONTEXT_NODE_0_:
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY5].min_val;
	val=vp8d_token_extra_bits2_cat5[0];
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat5, 1,4);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat5, 2,3);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat5, 3,2);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat5, 4,1);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat5, 5,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

CAT_THREEFOUR_CONTEXT_NODE_0_:
    DECODE_AND_BRANCH_IF_ZERO(Prob[CAT_THREE_CONTEXT_NODE], CAT_THREE_CONTEXT_NODE_0_);
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY4].min_val;
	val=vp8d_token_extra_bits2_cat4[0];
    DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat4, 1,3);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat4, 2,2);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat4, 3,1);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat4, 4,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

CAT_THREE_CONTEXT_NODE_0_:
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY3].min_val;
	val=vp8d_token_extra_bits2_cat3[0];
    DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat3, 1,2);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat3, 2,1);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat3, 3,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

HIGH_LOW_CONTEXT_NODE_0_:
    DECODE_AND_BRANCH_IF_ZERO(Prob[CAT_ONE_CONTEXT_NODE], CAT_ONE_CONTEXT_NODE_0_);
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY2].min_val;
	val=vp8d_token_extra_bits2_cat2[0];
    DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat2, 1,1);
	DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat2, 2,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

CAT_ONE_CONTEXT_NODE_0_:
    //val = vp8d_token_extra_bits2[DCT_VAL_CATEGORY1].min_val;
	val=vp8d_token_extra_bits2_cat1[0];
    DECODE_EXTRABIT_AND_ADJUST_VAL(vp8d_token_extra_bits2_cat1,1,0);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(val);

LOW_VAL_CONTEXT_NODE_0_:
    DECODE_AND_BRANCH_IF_ZERO(Prob[TWO_CONTEXT_NODE], TWO_CONTEXT_NODE_0_);
    DECODE_AND_BRANCH_IF_ZERO(Prob[THREE_CONTEXT_NODE], THREE_CONTEXT_NODE_0_);
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(4);

THREE_CONTEXT_NODE_0_:
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(3);

TWO_CONTEXT_NODE_0_:
    DECODE_SIGN_WRITE_COEFF_AND_CHECK_EXIT(2);

ONE_CONTEXT_NODE_0_:
    DECODE_AND_APPLYSIGN(1);
    Prob = coef_probs + ENTROPY_NODES;

    if (c < 15)
    {
        qcoeff_ptr [ scan[c] ] = (signed short) v;
        ++c;
        goto DO_WHILE;
    }
    qcoeff_ptr [ scan[15] ] = (signed short) v;

BLOCK_FINISHED:
	t = ((xdcol->eob[i] = (char)c) != !type);   // any nonzero data?
    eobtotal += xdcol->eob[i];
	*a = *l = (ENTROPY_CONTEXT)t;
    qcoeff_ptr += 16;

    i++;

    if (i < stop)
        goto BLOCK_LOOP;

    if (i == 25)
    {
        scan = vp8_default_zig_zag1d;//x->scan_order1d;
        type = 0;
        i = 0;
        stop = 16;
        coef_probs = oc->fc.coef_probs [type] [ 0 ] [0];
		qcoeff_ptr =  &xdcol->qcoeff[0];
#if 0
        qcoeff_ptr = &x->qcoeff[0];
#endif
        goto BLOCK_LOOP;
    }

    if (i == 16)
    {
        type = 2;
        coef_probs = oc->fc.coef_probs [type] [ 0 ] [0];
        stop = 24;
        goto BLOCK_LOOP;
    }

    FILL
    bc->value = value;
    bc->count = count;
    bc->range = range;
	bc->pos    = pos;

#ifdef STABILITY
	if(mflag==1 && eobtotal==0) //414
		return -1;
#endif

    return eobtotal;
}

static unsigned int read_partition_size(const unsigned char *cx_size)
{
    const unsigned int size =
        cx_size[0] + (cx_size[1] << 8) + (cx_size[2] << 16);
    return size;
}

vpx_codec_err_t setup_token_decoder(VP8D_COMP *pbi, const unsigned char *cx_data)
{
    int num_part;
    int i;
    VP8_COMMON          *pc = &pbi->common;
    const unsigned char *user_data_end = pbi->Source + pbi->source_sz;
    vp8_reader          *bool_decoder;
    const unsigned char *partition;

    /* Parse number of token partitions to use */
    pc->multi_token_partition = (TOKEN_PARTITION)vp8_read_literal(&pbi->bc, 2);
    num_part = 1 << pc->multi_token_partition;

    /* Set up pointers to the first partition */
    partition = cx_data;
    bool_decoder = &pbi->bc2;

    if (num_part > 1)
    {
		pbi->mbc = (vp8_reader*)MallocMem(pbi->pUserData, pbi->nCodecIdx, sizeof(vp8_reader)*num_part, CACHE_LINE);
        bool_decoder = pbi->mbc;
        partition += 3 * (num_part - 1); //zou ? ==> 3 bytes is indicate the partition_size;
    }

    for (i = 0; i < num_part; i++)
    {
        const unsigned char *partition_size_ptr = cx_data + i * 3; //while *3 ?   ==> the 3 bytes indicates the partition_size;
         int         partition_size;

        /* Calculate the length of this partition. The last partition
         * size is implicit.
         */
        if (i < num_part - 1) 
        {
            partition_size = (int)read_partition_size(partition_size_ptr);
        }
        else
        {
            partition_size = user_data_end - partition;
        }
#ifdef STABILITY
        if (partition + partition_size > user_data_end || partition_size<0 )
		{
			return VPX_CODEC_CORRUPT_FRAME; 
		}

#endif
           
        if (vp8dx_start_decode(bool_decoder,partition, partition_size))
			return VPX_CODEC_MEM_ERROR;

        /* Advance to the next partition */
        partition += partition_size;
        bool_decoder++;
    }


	return VPX_CODEC_OK;
}

