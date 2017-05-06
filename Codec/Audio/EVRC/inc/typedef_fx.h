
 
/*_____________________
 |                     |
 | Basic types.        |
 |_____________________|
*/

#ifndef __TYPEDEF_FX
 #define __TYPEDEF_FX

#define LW_SIGN (long)0x80000000       /* sign bit */
#define LW_MIN (long)0x80000000
#define LW_MAX (long)0x7fffffff

#define SW_SIGN (short)0x8000          /* sign bit for Word16 type */
#define SW_MIN (short)0x8000           /* smallest Ram */
#define SW_MAX (short)0x7fff           /* largest Ram */

/* Definition of Types *
 ***********************/

#if defined(__BORLANDC__) || defined (__WATCOMC__) || defined(_MSC_VER) || defined(__ZTC__) || defined(__HIGHC__) || defined (__CYGWIN32__)
typedef short int Word16;           /* 16 bit "register"  (sw*) */
typedef unsigned short int UNS_Word16;  /* 16 bit "register"  (sw*) */
typedef short int Word16Rom;        /* 16 bit ROM data    (sr*) */
typedef long int Word32;             /* 32 bit "accumulator" (L_*) */
typedef long int Word32Rom;          /* 32 bit ROM data    (L_r*)  */
typedef double Word40;		     /* 40 bit accumulator        */
#elif defined( __sun)
typedef short Word16;           /* 16 bit "register"  (sw*) */
typedef unsigned short UNS_Word16;  /* 16 bit "register"  (sw*) */
typedef short Word16Rom;        /* 16 bit ROM data    (sr*) */
typedef long Word32;             /* 32 bit "accumulator" (L_*) */
typedef long Word32Rom;          /* 32 bit ROM data    (L_r*)  */
typedef double Word40;				   /* 40 bit accumulator        */
#elif defined(__unix__) || defined(__unix)
typedef short Word16;           /* 16 bit "register"  (sw*) */
typedef unsigned short UNS_Word16;  /* 16 bit "register"  (sw*) */
typedef short Word16Rom;        /* 16 bit ROM data    (sr*) */
typedef int Word32;             /* 32 bit "accumulator" (L_*) */
typedef int Word32Rom;          /* 32 bit ROM data    (L_r*)  */
typedef double Word40;				   /* 40 bit accumulator        */
#elif defined(VMS) || defined(__VMS)
typedef short Word16;           /* 16 bit "register"  (sw*) */
typedef unsigned short UNS_Word16;  /* 16 bit "register"  (sw*) */
typedef short Word16Rom;        /* 16 bit ROM data    (sr*) */
typedef long Word32;             /* 32 bit "accumulator" (L_*) */
typedef long Word32Rom;          /* 32 bit ROM data    (L_r*)  */
typedef double Word40;				   /* 40 bit accumulator        */
#else
typedef short int Word16;           /* 16 bit "register"  (sw*) */
typedef unsigned short int UNS_Word16;  /* 16 bit "register"  (sw*) */
typedef short int Word16Rom;        /* 16 bit ROM data    (sr*) */
typedef long int Word32;             /* 32 bit "accumulator" (L_*) */
typedef long int Word32Rom;          /* 32 bit ROM data    (L_r*)  */
typedef double Word40;		     /* 40 bit accumulator        */
#endif

/* global variables */
/********************/

//extern int giFrmCnt;                   /* 0,1,2,3,4..... */
//extern int giSfrmCnt;                  /* 0,1,2,3 */

//extern int giDTXon;                    /* DTX Mode on/off */








//basic operation functions optimization flags
#define SATRUATE_IS_MACRO               0   //define saturate as MACRO
#define SATRUATE_IS_INLINE              1   //define saturate as inline function

#define ABS_S_IS_MACRO                  1   //define abs_s as MACRO
#define ABS_S_IS_INLINE                 0   //define abs_s as inline function

//for add operation, (0, 0) is the best
//add can't be MACRO
//when add is inline, decoding is slower.
#define ADD_IS_MACRO                    0   //define add as MACRO
#define ADD_IS_INLINE                   1   //define add as inline function

#define EXTRACT_H_IS_MACRO              0   //define extract_h as MACRO
#define EXTRACT_H_IS_INLINE             1   //define extract_h as inline function

#define EXTRACT_L_MACRO                 0   //define extract_l as MACRO
#define EXTRACT_L_IS_INLINE             1   //define extract_l as inline function

//for L_abs operation, (0, 0) is the best
#define L_ABS_IS_MACRO                  0   //define L_abs as MACRO
#define L_ABS_IS_INLINE                 0   //define L_abs as inline function

#define L_DEPOSIT_H_MACRO               1   //define L_deposit_h as MACRO
#define L_DEPOSIT_H_IS_INLINE           0   //define L_deposit_h as inline function

#define L_DEPOSIT_L_MACRO               1   //define L_deposit_l as MACRO
#define L_DEPOSIT_L_INLINE              0   //define L_deposit_l as inline function

#define L_MAC_IS_MACRO                  0   //define L_mac as MACRO
#define L_MAC_IS_INLINE                 1   //define L_mac as inline function

#define L_MSU_IS_MACRO                  0   //define L_msu as MACRO
#define L_MSU_IS_INLINE                 1   //define L_msu as inline function

// affect the performance 40s to 34s for decoding 336.7s speech segment
#define L_MULT_IS_MACRO                 0   //define L_mult as MACRO        //when it is macro, error in bit-exact test
#define L_MULT_IS_INLINE                1   //define L_mult as inline function

//for L_negate operation, (0, 0) is the best
#define L_NEGATE_IS_MACRO               0   //define L_negate as MACRO
#define L_NEGATE_IS_INLINE              0   //define L_negate as inline function

//for L_shl operation, (0, 0) is the best
#define L_SHL_IS_MACRO                  0   //define L_shl as MACRO
#define L_SHL_IS_INLINE                 0   //define L_shl as inline function
#define L_SHR_IS_MACRO                  0   //define L_shr as MACRO
#define L_SHR_IS_INLINE                 0   //define L_shr as inline function

//for L_sub operation, (0, 0) is the best
#define L_SUB_IS_MACRO                  0   //define L_sub as MACRO
#define L_SUB_IS_INLINE                 0   //define L_sub as inline function

#define MULT_IS_MACRO                   0   //define mult as MACRO
#define MULT_IS_INLINE                  1   //define mult as inline function

//that mult_r is inline or not does't matter. so select (0, 0)
#define MULT_R_IS_MACRO                 0   //define mult_r as MACRO
#define MULT_R_IS_INLINE                0   //define mult_r as inline function

#define NEGATE_IS_MACRO                 1   //define negate as MACRO
#define NEGATE_IS_INLINE                0   //define negate as inline function

#define NORM_L_IS_MACRO                 0   //define norm_l as MACRO
#define NORM_L_IS_INLINE                1   //define norm_l as inline function

#define NORM_S_IS_MACRO                 0   //define norm_s as MACRO
#define NORM_S_IS_INLINE                1   //define norm_s as inline function

//for round32, inline and func is better. little difference between
#define ROUND_IS_MACRO                  0   //define round32 as MACRO
#define ROUND_IS_INLINE                 0   //define round32 as inline function

//for shl operation, (0, 0) is the best
#define SHL_IS_MACRO                    0   //define shl as MACRO
#define SHL_IS_INLINE                   0  //define shl as inline function

//for shr operation, (0, 0) is the best
#define SHR_IS_MACRO                    0   //define shr as MACRO
#define SHR_IS_INLINE                   0   //define shr as inline function

//for sub operation, (0, 0) is the best
#define SUB_IS_MACRO                    0   //define sub as MACRO
#define SUB_IS_INLINE                   0   //define sub as inline function //sub, inline is the best

#define L_ADD_IS_MACRO                  0   //define L_add as MACRO
#define L_ADD_IS_INLINE                 1   //define L_add as inline function






//performance
#define     FUNC_IIR_OPTIMIZE               1   //iir()
#define     FUNC_FIR_OPTIMIZE               1   //fir()
#define     FUNC_COR_H_VEC_OPTIMIZE         1   //cor_h_vec()
#define     FUNC_BL_INTRP_OPTIMIZE          1   //bl_intrp()
#define     FUNC_LSPMAQ_OPTIMIZE            1   //lspmaq()
#define     FUNC_IMPULSERZP_OPTIMIZE        1   //ImpulseRzp()
#define     FUNC_CONVOLVEIMPULSER_OPTIMIZE  1   //ConvolveImpulseR()
#define     FUNC_CODE_8I55_35BITS_OPT       1   //code_8i55_35bits()
#define     FUNC_FNDPPF_35BITS_OPT          1   //fndppf()      //when it is macro, error in bit-exact test
#define     FUNC_C_FFT_OPT                  1   //c_fft()
#define     FUNC_SEARCH_IXIY_OPT            1   //search_ixiy()
#define     FUNC_AUTOCORRELATION_OPT        1   //autocorrelation()
#define     FUNC_BQIIR_OPT                  1   //bqiir()
#define     FUNC_GETRESIDUAL_OPT            1   //GetResidual()
#define     FUNC_A2LSP_OPT                  1   //a2lsp()
#define     FUNC_COR_H_X_OPT                1   //cor_h_x()
#define     FUNC_LSP2A_OPT                  1   //lsp2a()
#define     FUNC_SYNTHESISFILTER_OPT        1   //SynthesisFilter()
#define     FUNC_WEIGHT2RES_OPT             1   //Weight2Res()
#define     FUNC_NOISE_SUPRS_OPT            1   //noise_suprs()
#define     FUNC_APF_OPT                    1   //apf()
#define     FUNC_DURBIN_OPT                 1   //durbin()
#define     FUNC_DECODE_FER_OPT             1   //decode_fer()
#define     FUNC_DECODE_NO_FER_OPT          1   //decode_no_fer()
#define     FUNC_ENCODE_OPT                 1   //encode()
#define     FUNC_FCB_GAINQ_OPT              1   //fcb_gainq()
#define     FUNC_RAN_G_OPT                  1   //ran_g()
#define     FUNC_E_RAN_G_OPT                1   //e_ran_g()
#define     FUNC_GETEXC800BPS_OPT           1   //GetExc800bps()
#define     FUNC_GETEXC800BPS_DEC_OPT       1   //GetExc800bps_dec()
#define     FUNC_GETGAIN_OPT                1   //getgain()



//warning
#define     FUNC_CSHIFTFRAME_OPT            1   //cshiftframe()



//file
#define     FILE_INTERPOL_OPT               1   //Interpol()



//advanced basic functions optimization
#define     FUNC_L_MPY_LL_OPT               1   //L_mpy_ll()
#define     FUNC_L_MPY_LS_OPT               1   //L_mpy_ls()
#define     L_MPY_LS_IS_INLINE              1   //L_mpy_ls()
#define     FUNC_L_DIVIDE_OPT               1   //L_divide()
#define     FUNC_SQROOT_OPT                 1   //sqroot()
#define     FUNC_FNLOG2_OPT                 1   //fnLog2()
#define     FUNC_FNEXP2_OPT                 1   //fnExp2()
#define     FUNC_FNEXP10_OPT                1   //fnExp10()



#endif
