/*
*
*      File             : typedefs.h
*      Description      : Definition of platform independent data
*                         types and constants
*
*
*      The following platform independent data types and corresponding
*      preprocessor (#define) constants are defined:
*
*        defined type  meaning           corresponding constants
*        ----------------------------------------------------------
*        Char          character         (none)
*        Bool          boolean           true, false
*        Word8         8-bit signed      minWord8,   maxWord8
*        UWord8        8-bit unsigned    minUWord8,  maxUWord8
*        Word16        16-bit signed     minWord16,  maxWord16
*        UWord16       16-bit unsigned   minUWord16, maxUWord16
*        Word32        32-bit signed     minWord32,  maxWord32
*        UWord32       32-bit unsigned   minUWord32, maxUWord32
*        Float         floating point    minFloat,   maxFloat
*
*
*      The following compile switches are #defined:
*
*        PLATFORM      string indicating platform progam is compiled on
*                      possible values: "OSF", "PC", "SUN"
*
*        OSF           only defined if the current platform is an Alpha
*        PC            only defined if the current platform is a PC
*        SUN           only defined if the current platform is a Sun
*        
*        LSBFIRST      is defined if the byte order on this platform is
*                      "least significant byte first" -> defined on DEC Alpha
*                      and PC, undefined on Sun
*
********************************************************************************
*/
#ifndef typedefs_h
#define typedefs_h "$Id $"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/

#ifndef CHAR_BIT
#define CHAR_BIT      8         /* number of bits in a char */
#endif

#ifndef SHRT_MIN
#define SHRT_MIN    (-32768)        /* minimum (signed) short value */
#endif

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef assert
#define assert(_Expression)     ((void)0)
#endif

#define INT_BITS   32
/*
********************************************************************************
*                         DEFINITION OF CONSTANTS 
********************************************************************************
*/
/*
 ********* define char type
 */
typedef char Char;

/*
 ********* define 8 bit signed/unsigned types & constants
 */
typedef signed char Word8;
typedef unsigned char UWord8;
/*
 ********* define 16 bit signed/unsigned types & constants
 */
typedef short Word16;
typedef unsigned short UWord16;

/*
 ********* define 32 bit signed/unsigned types & constants
 */
typedef long Word32;
typedef unsigned long UWord32;



#ifdef LINUX
typedef long long Word64;
typedef unsigned long long UWord64;
#elif _IOS
typedef long long Word64;
typedef unsigned long long UWord64;
#else
typedef __int64 Word64;
typedef unsigned __int64 UWord64;
#endif

#ifndef min
#define min(a,b) ( a < b ? a : b)
#endif

#ifndef max
#define max(a,b) ( a > b ? a : b)
#endif

#ifdef ARM_INASM
#ifdef ARMV5_INASM
#define ARMV5E_INASM	1
#endif
#define ARMV4_INASM		1
#endif

#if ARMV4_INASM
	#define ARMV5TE_SAT           1
    #define ARMV5TE_ADD           1
    #define ARMV5TE_SUB           1
	#define ARMV5TE_SHL           1
    #define ARMV5TE_SHR           1
	#define ARMV5TE_L_SHL         1
    #define ARMV5TE_L_SHR         1
#endif//ARMV4
#if ARMV5E_INASM
    #define ARMV5TE_L_ADD         1
    #define ARMV5TE_L_SUB         1
    #define ARMV5TE_L_MULT        1
    #define ARMV5TE_L_MAC         1
    #define ARMV5TE_L_MSU         1
   
    
    #define ARMV5TE_DIV_S         1
    #define ARMV5TE_ROUND         1
    #define ARMV5TE_MULT          1
    
    #define ARMV5TE_NORM_S        1
    #define ARMV5TE_NORM_L        1
	#define ARMV5TE_L_MPY_LS      1
#endif

//basic operation functions optimization flags
#define SATRUATE_IS_MACRO               0   //define saturate as MACRO
#define SATRUATE_IS_INLINE              1   //define saturate as inline function
#define SHL_IS_MACRO                    0   //define shl as MACRO
#define SHL_IS_INLINE                   1  //define shl as inline function
#define SHR_IS_MACRO                    0   //define shr as MACRO
#define SHR_IS_INLINE                   1   //define shr as inline function
#define L_MULT_IS_MACRO                 0   //define L_mult as MACRO
#define L_MULT_IS_INLINE                1   //define L_mult as inline function
#define L_MSU_IS_MACRO                  0   //define L_msu as MACRO
#define L_MSU_IS_INLINE                 1   //define L_msu as inline function
#define L_SUB_IS_MACRO                  0   //define L_sub as MACRO
#define L_SUB_IS_INLINE                 1   //define L_sub as inline function
#define L_SHL_IS_MACRO                  0   //define L_shl as MACRO
#define L_SHL_IS_INLINE                 1   //define L_shl as inline function
#define L_SHR_IS_MACRO                  0   //define L_shr as MACRO
#define L_SHR_IS_INLINE                 1   //define L_shr as inline function
#define ADD_IS_MACRO                    0   //define add as MACRO
#define ADD_IS_INLINE                   1   //define add as inline function //add, inline is the best
#define SUB_IS_MACRO                    0   //define sub as MACRO
#define SUB_IS_INLINE                   1   //define sub as inline function //sub, inline is the best
#define DIV_S_IS_MACRO                  0   //define div_s as MACRO
#define DIV_S_IS_INLINE                 1   //define div_s as inline function
#define MULT_IS_MACRO                   0   //define mult as MACRO
#define MULT_IS_INLINE                  1   //define mult as inline function
#define NORM_S_IS_MACRO                 0   //define norm_s as MACRO
#define NORM_S_IS_INLINE                1   //define norm_s as inline function
#define NORM_L_IS_MACRO                 0   //define norm_l as MACRO
#define NORM_L_IS_INLINE                1   //define norm_l as inline function
#define ROUND_IS_MACRO                  0   //define round as MACRO
#define ROUND_IS_INLINE                 1   //define round as inline function
#define L_MAC_IS_MACRO                  0   //define L_mac as MACRO
#define L_MAC_IS_INLINE                 1   //define L_mac as inline function
#define L_ADD_IS_MACRO                  0   //define L_add as MACRO
#define L_ADD_IS_INLINE                 1   //define L_add as inline function
//
#define ABS_S_IS_MACRO                  1   //define abs_s as MACRO
#define ABS_S_IS_INLINE                 0   //define abs_s as inline function
#define EXTRACT_H_IS_MACRO              0   //define extract_h as MACRO        //???
#define EXTRACT_H_IS_INLINE             1   //define extract_h as inline function        //???
#define EXTRACT_L_MACRO                 0   //define extract_l as MACRO        //???
#define EXTRACT_L_IS_INLINE             1   //define extract_l as inline function        //???
#define L_DEPOSIT_H_MACRO               1   //define L_deposit_h as MACRO
#define L_DEPOSIT_H_IS_INLINE           0   //define L_deposit_h as inline function
#define L_DEPOSIT_L_MACRO               1   //define L_deposit_l as MACRO
#define L_DEPOSIT_L_INLINE              0   //define L_deposit_l as inline function
#define L_ABS_IS_MACRO                  1   //define L_abs as MACRO
#define L_ABS_IS_INLINE                 0   //define L_abs as inline function

#define NEGATE_IS_MACRO                 1   //define negate as MACRO
#define NEGATE_IS_INLINE                0   //define negate as inline function
//#define L_MACNS_IS_MACRO                0   //define L_macNs as MACRO
//#define L_MACNS_IS_INLINE               0   //define L_macNs as inline function
//#define L_MSUNS_IS_MACRO                0   //define L_msuNs as MACRO
//#define L_MSUNS_IS_INLINE               0   //define L_msuNs as inline function
//#define L_ADD_C_IS_MACRO                0   //define L_add_c as MACRO
//#define L_ADD_C_IS_INLINE               0   //define L_add_c as inline function
//#define L_SUB_C_IS_MACRO                0   //define L_sub_c as MACRO
//#define L_SUB_C_IS_INLINE               0   //define L_sub_c as inline function
#define L_NEGATE_IS_MACRO               1   //define L_negate as MACRO
#define L_NEGATE_IS_INLINE              0   //define L_negate as inline function
#define MULT_R_IS_MACRO                 0   //define mult_r as MACRO
#define MULT_R_IS_INLINE                1   //define mult_r as inline function
#define SHR_R_IS_MACRO                  0   //define shr_r as MACRO
#define SHR_R_IS_INLINE                 1   //define shr_r as inline function
#define MAC_R_IS_MACRO                  0   //define mac_r as MACRO
#define MAC_R_IS_INLINE                 1   //define mac_r as inline function
#define MSU_R_IS_MACRO                  0   //define msu_r as MACRO
#define MSU_R_IS_INLINE                 1   //define msu_r as inline function
#define L_SHR_R_IS_MACRO                0   //define L_shr_r as MACRO
#define L_SHR_R_IS_INLINE               1   //define L_shr_r as inline function
//#define L_SAT_IS_MACRO                  0   //define L_sat as MACRO
//#define L_SAT_IS_INLINE                 1   //define L_sat as inline function
//#define L_EXTRACT_IS_MACRO              1   //define voAMRWBPDecL_Extract as MACRO
#define L_EXTRACT_OPT                   1   //optimize voAMRWBPDecL_Extract as inline function
#define MPY_32_IS_MACRO                 1   //define Mpy_32 as MACRO
#define MPY_32_OPT                      1   //optimize Mpy_32 as inline function
#define FUNC_MPY_32_16_MACRO            1   //when this flag is set, define Mpy_32_16 as Macro

#define     FUNC_L_MPY_LL_OPT               1   //L_mpy_ll()
#define     FUNC_L_MPY_LS_OPT               1   //L_mpy_ls()
#define     L_MPY_LS_IS_INLINE              1   //L_mpy_ls()
#define     FUNC_L_DIVIDE_OPT               1   //L_divide()


//opt
#define FUNC_SIMPLE_FRAME_LIMITER_OPT   1   //when this flag is set, optimize function Simple_frame_limiter()
#define FUNC_BASS_POSTFILTER_OPT        1   //when this flag is set, optimize function Bass_postfilter()
#define FUNC_RESIDU_OPT                 1   //when this flag is set, optimize function Residu()
#define FUNC_RESIDU2_OPT                1   //when this flag is set, optimize function Residu2()
#define FUNC_SOFT_EXC_HF_OPT            1   //when this flag is set, optimize function Soft_exc_hf()
#define FUNC_FIR_FILT_OPT               1   //when this flag is set, optimize function Fir_filt()
#define FUNC_C_FFT_FX_OPT               1   //when this flag is set, optimize function c_fft_fx()
#define FUNC_RE8_DECODE_RANK_OF_PERMUTATION_OPT               1   //when this flag is set, optimize function Re8_decode_rank_of_permutation()
#define FUNC_PRED_LT4_OPT               1   //when this flag is set, optimize function Pred_lt4()
#define FUNC_SHORT_PITCH_TRACKER_OPT    1   //when this flag is set, optimize function Short_pitch_tracker()
#define FUNC_CROSSCORR_2_OPT            1   //when this flag is set, optimize function Crosscorr_2()
#define FUNC_BIN2INT_OPT                1   //when this flag is set, optimize function Bin2int()
#define FUNC_DECODER_LF_OPT             1   //when this flag is set, optimize function Decoder_lf()
#define FUNC_DECODER_TCX_OPT            1   //when this flag is set, optimize function Decoder_tcx()
#define FUNC_SYN_FILT_32_OPT            1   //when this flag is set, optimize function Syn_filt_32()
#define FUNC_SYN_FILT_OPT               1   //when this flag is set, optimize function Syn_filt()
#define FUNC_DECODER_ACELP_OPT          1   //when this flag is set, optimize function Decoder_acelp()
#define FUNC_FILE_6K_7K_OPT             1   //when this flag is set, optimize function Filt_6k_7k()
#define FUNC_HP400_12K8_OPT             1   //when this flag is set, optimize function HP400_12k8()
#define FUNC_HP50_12K8_OPT              1   //when this flag is set, optimize function HP50_12k8()

#define FUNC_INTERPOL_MEM_OPT           1   //when this flag is set, optimize function Interpol_mem()
#define FUNC_INTERPOL_MEM_FOR12K8       1   //when this flag is set, optimize function Interpol_mem_for12k8() is used
#define FUNC_MINTERPOL_MEM_OPT          1   //when this flag is set, optimize function mInterpol_mem()
#define FUNC_INTERPOL_OPT               1   //when this flag is set, optimize function Interpol()
#define FUNC_SYN_FILT_S_OPT             1   //when this flag is set, optimize function Syn_filt_s()
#define FUNC_ISQRT_OPT                  1   //when this flag is set, optimize function voAMRWBPDecIsqrt()
#define FUNC_ISQRT_N_OPT                1   //when this flag is set, optimize function voAMRWBPDecIsqrt_n()
#define FUNC_POW2_OPT                   1   //when this flag is set, optimize function voAMRWBPDecPow2()
#define FUNC_DOT_PRODUCT12_OPT          1   //when this flag is set, optimize function voAMRWBPDecDot_product12()
#define FUNC_ISP_AZ_OPT                 1   //when this flag is set, optimize function Isp_Az()
#define FILE_DECIM54_FX_OPT             1   //when this flag is set, optimize function Decim_12k8(), Oversamp_16k(), Down_samp(), Up_samp()
#define FUNC_SCALE_SIG_OPT              1   //when this flag is set, optimize function Scale_sig()
#define FUNC_AGC2_OPT                   1   //when this flag is set, optimize function agc2()
#define FILE_DEEMPH_FX_OPT              1   //when this flag is set, optimize function Deemph_32()
#define FUNC_ISF_EXTRAPOLATION_OPT      1   //when this flag is set, optimize function Isf_Extrapolation()
#define FUNC_PHASE_DISPERSION_OPT       1   //when this flag is set, optimize function Phase_dispersion()
#define FUNC_RANDOME_OPT                1   //when this flag is set, optimize function Random()
#define FUNC_DTX_DEC_ACTIVITY_UPDATE_OPT    1   //when this flag is set, optimize function dtx_dec_activity_update()
#define FUNC_D_IF_MMS_CONVERSION_FX_OPT 1   //when this flag is set, optimize function D_IF_mms_conversion_fx()
#define FUNC_FILT_7K_OPT                1   //when this flag is set, optimize function Filt_7k()
#define FUNC_DEC_ACELP_2T64_OPT         0   //when this flag is set, optimize function DEC_ACELP_2t64_fx()
#define FUNC_DEC_ACELP_4T64_OPT         0   //when this flag is set, optimize function DEC_ACELP_4t64_fx()
#define FILE_Q_PULSE_FX_OPT             1   //when this flag is set, optimize file q_pulse_fx.c
#define FUNC_INTERPOL_ST_OPT            1   //when this flag is set, optimize function Interpol_st()
#define FUNC_JOIN_OVER_12K8_OPT         1   //when this flag is set, optimize function Join_over_12k8()
#define FUNC_RE8_DEC_OPT                1   //when this flag is set, optimize function RE8_Dec()
#define FUNC_R_FFT_FX_OPT               1   //when this flag is set, optimize function r_fft_fx()
#define FUNC_SMOOTH_ENER_HF_OPT         1   //when this flag is set, optimize function Smooth_ener_hf()
#define FUNC_Decoder_HF_OPT             1   //when this flag is set, optimize function Decoder_hf()
#define FUNC_IFFT9_FX_OPT               1   //when this flag is set, optimize function ifft9_fx()
#define FUNC_GET_EXC_OPT                1   //when this flag is set, optimize function Get_exc()
#define FUNC_AVQ_DEMUXDEC_OPT           1   //when this flag is set, optimize function AVQ_Demuxdec()
#define FUNC_GET_EXC_WIN_OPT            1   //when this flag is set, optimize function Get_exc_win()
#define FUNC_RE8_PPV_FX_OPT             1   //when this flag is set, optimize function RE8_PPV_FX()
#define FILE_PREEMPH_FX_OPT             1   //when this flag is set, optimize file preemp_fx.c, Preemph() and Preemph2()
#define FUNC_DEEMPH1K6_OPT              1   //when this flag is set, optimize function Deemph1k6()
#define FUNC_SCALE_MEM_TCX_OPT          1   //when this flag is set, optimize function Scale_mem_tcx


//macro
#define FUNC_RANDOM_MACRO               1   //when this flag is set, define Random as Macro
 
                                         
//large array as static or global        
#define LARGE_VAR_OPT                   1

//inline flag
//when Bin2int, Set_zero and Copy are inline functions, performance is lower


//asm
#define FUNC_SCALE_SIG_ASM              0   //when this flag is set, rewrite Scale_sig() with assembly
#define FUNC_RESIDU_ASM                 0   //when this flag is set, rewrite Residu() with assembly
#define FUNC_INTERPOL_MEM_ASM           0   //when this flag is set, rewrite Interpol_mem() with assembly


#ifdef  MSVC_AMRWBPLUS
#if FUNC_SCALE_SIG_ASM
#undef FUNC_SCALE_SIG_ASM
#define FUNC_SCALE_SIG_ASM 0
#endif
#if FUNC_RESIDU_ASM
#undef FUNC_RESIDU_ASM
#define FUNC_RESIDU_ASM 0
#endif
#if FUNC_INTERPOL_MEM_ASM
#undef FUNC_INTERPOL_MEM_ASM
#define FUNC_INTERPOL_MEM_ASM 0
#endif
#endif


#ifdef EVC_AMRWBPLUS
#define MAX_OPT         1           //When this flag is set, some bit compliant tests can't pass, but maximum speed can be got
#endif


//following is flag for AMRWB optimization(above are for amrwbplus)
#define AWB_FUNC_DECODER_OPT            1       //when this flag is set, function decoder() is optimized.
#define AWB_FUNC_SYNTHESIS_OPT          1       //when this flag is set, function synthesis() is optimized.
#define FUNC_INTERPOL_WB_OPT            1   //when this flag is set, optimize the static function Interpol()



#endif

