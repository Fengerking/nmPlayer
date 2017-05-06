/******************************************************************************
TDAS Implementation Kit

  Confidential Information - Limited distribution to authorized persons only.
  This material is protected under international copyright laws as an 
  unpublished work. Do not copy. 
  Copyright (C) 2004-2010 Dolby Laboratories Inc. All rights reserved.

	File:           tdas_client.h
	Contributors:	Roger Butler
	Description:    C implementation for DSPs
	  
******************************************************************************/

#ifndef TDAS_H
#define TDAS_H
#include "bundle_features.h"

/* TDAS supports two different opening and initialisation techniques:
 *
 * Option 1 Simplified API- using tdas_open and tdas_close:
 * TDAS internally allocates, initialises and deallocates it's own memory.
 * Example: 
 * tdas_state = tdas_open();
 * tdas_process(tdas_state,...)
 * tdas_close(tdas_state);
 *
 * Option 2 Extended API- using tdas_query_memory, tdas_init and tdas_uninit:
 * The calling function allocates memory for TDAS, by first querying
 * the TDAS memory requirements, allocating the memory, then calling TDAS to 
 * initialise the memory area. The calling function is responsible for 
 * deallocating the memory once it is no longer needed.
 * Example:
 * tdas_query_memory(&persistent_mem_required, &scratch_mem_required);
 * tdas_p_mem = malloc(persistent_mem_required);
 * tdas_s_mem = malloc(scratch_mem_required);
 * tdas_state = tdas_init(tdas_p_mem, tdas_s_mem);
 * tdas_process(tdas_state,...);
 * tdas_uninit(tdas_state); //Optional, but good practice.
 * free(tdas_p_mem);
 * free(tdas_s_mem);
 *
 ******************************************************************************
 *    Calls to tdas_open and tdas_close should not be mixed with calls to     *
 *    tdas_init and tdas_uninit.                                              *
 ******************************************************************************
 */

/* Query the memory requirements of TDAS. This function determines the 
 * amount of persistent and scratch memory required by TDAS. 
 * Persistent memory must be maintained while TDAS is running, scratch memory
 * may be cleared or used for other purposes between calls to tdas_process.
 */
#ifdef ENABLE_SIMPLIFIED_API
#ifdef __cplusplus
extern "C" {
#endif
#ifdef TDAS_DLL
IMPORT_C
#endif
void tdas_query_memory(unsigned int *n_persist, unsigned int *n_scratch);
#endif
#ifdef __cplusplus
}
#endif
/* Initialise TDAS memory areas. This function requires the correct amount of 
 * memory to be allocated by the calling function then pass in as arguments.
 * v_persist: pointer to base address of allocated persistent memory.
 * v_scratch: pointer to base address of allocated scratch memory.
 * Persistent memory must be maintained while TDAS is running, scratch memory
 * may be cleared or used for other purposes between calls to tdas_process.
 *
 * A pointer to the state will be returned, which must be passed into the other
 * functions in this API.
 */
#ifdef ENABLE_SIMPLIFIED_API
#ifdef __cplusplus
extern "C" {
#endif
#ifdef TDAS_DLL
IMPORT_C
#endif
void *tdas_init(void *v_persist, void* v_scratch);
#endif
#ifdef __cplusplus
}
#endif
/* Uinitialise TDAS memory areas. This function clears the memory areas used 
 * by TDAS. Pass in the state pointer allocated by tdas_init(). 
 * Use of this function is optional.
 */
#ifdef __cplusplus
extern "C" {
#endif
#ifdef TDAS_DLL
IMPORT_C
#endif
void tdas_uninit(void *v_tdas);
#ifdef __cplusplus
}
#endif

/* Open TDAS. Memory is allocated by this function. If a particular DSP 
 * implementation is not allowed to allocate memory then this function must
 * be overridden for that platform. A pointer to the state will be returned,
 * which must be passed into the other functions in this API.
 */
#ifdef __cplusplus
extern "C" {
#endif
#ifdef TDAS_DLL
IMPORT_C
#endif
void *tdas_open(void);
#ifdef __cplusplus
}
#endif

/* This must be called to close TDAS. Pass in the state pointer allocated
 * by tdas_open().
 */
#ifdef __cplusplus
extern "C" {
#endif
#ifdef TDAS_DLL
IMPORT_C
#endif
void tdas_close(void *v_tdas);
#ifdef __cplusplus
}
#endif

/* This function re-initialises TDAS to its default state.
 * Pass in the state pointer returned by tdas_open() or tdas_init().
 */
#ifdef __cplusplus
extern "C" {
#endif
#ifdef TDAS_DLL
IMPORT_C
#endif
void tdas_reset(void *v_tdas);
#ifdef __cplusplus
}
#endif

/* This function processes a block of samples. v_tdas must have been 
 * returned from tdas_open(). 
 * 
 * The input buffer (in) is a pre-allocated buffer, that can contain
 * either interleaved or non-interleaved samples from each channel.  The number
 * of input channels is set through the TDAS_IN_CHANS bundle parameter,
 * while the channel format (interleaved/non-interleaved) is set through the
 * TDAS_IN_CHAN_FORMAT bundle parameter.
 *
 * The ouput buffer (out) is also pre-allocated.  The output samples can also
 * be interleaved or non-interleaved, depending on what has been set for
 * the TDAS_IN_CHAN_FORMAT bundle parameter.
 *
 * n_samples is the number of samples in the input buffer, per channel.
 *
 * When the input buffer contains interleaved samples, corresponding samples
 * from each channel are placed contiguously.  e.g. for stereo input and
 * n_samples = n, the samples in the input buffer are laid out as follows:
 * L0 R0 L1 R1 L2 R2 ... Ln Rn
 *
 * When the input buffer contains non-interleaved samples, all available 
 * samples for a channel are contiguous.  e.g. for stereo input and
 * n_samples = n, the samples in the input buffer are laid out as follows:
 * L0 L1 L2 ... Ln R0 R1 R2 ... Rn
 *
 * Similar reasoning is applied for the logical positioning of samples
 * in the output buffer.  The number of output channels is two (stereo) in
 * usual operating conditions.  
 *
 * NOTE: If bypass mode 2 is used, then the number of output channels equals
 *       the number of input channels while bypass mode 2 is activated. 
 *       Ensure that the output buffer is of a suitable size (atleast as 
 *       large as the input buffer).
 *
 * The amount of memory that needs to be allocated for the input buffer is:
 *      <no. input channels> * n_samples * sizeof(IO_DATUM)
 * 
 * The amount of memory that needs to be allocated for the output buffer is:
 *      <no. output channels> * n_samples * sizeof(IO_DATUM) * <rate multiplier>
 * where <rate multiplier> is the ratio between the output sample rate and
 * the input sample rate, specified through the TDAS_RATE_MULTIPLIER
 * bundle parameter.  <rate multiplier> thus also affects the number of
 * samples per channel for the output buffer:
 *      <no. output samples per channel> = n_samples * <rate multiplier>
 * If you intend to use bypass mode 2 (hard bypass), this buffer must be:
 *      <no. input channels> * n_samples * sizeof(IO_DATUM) * <rate multiplier>
 *
 * 
 * It is permissible for in and out to point to the same memory location, as
 * long as all of the following conditions are met:
 * - The number of input channels is greater than or equal to the number of
 *   output channels.
 * - If using bypass mode 2, please observe the minimum output buffer size
 *   restriction above.
 * - The input sample rate is equal to the output sample rate.
 *
 *  Return codes:
 *  STATUS_OK(0):			No error
 *  ERR_NOT_MUL_BLKSZ (-1): Error - n_samples is not a multiple of BLK_SZ
 *  ERR_POINTER_NULL (-2):  Error - v_tdas points to NULL
 */
#ifdef __cplusplus
extern "C" {
#endif
#ifdef TDAS_DLL
IMPORT_C
#endif
int tdas_process(void *v_tdas, IO_DATUM *in, IO_DATUM *out, int n_samples);
#ifdef __cplusplus
}
#endif

/* Features */
#define TDAS_BUNDLE 0
#define TDAS_M2S 1
#define TDAS_SSE 2
#define TDAS_SSH 3 
#define TDAS_SLC 4
#define TDAS_VOL 5
#define TDAS_NB 6
#define TDAS_GEQ 7
#define TDAS_MUP 8
#define TDAS_MSR 9
#define TDAS_HFE 10
#define TDAS_FADE 11
#define TDAS_SEQ 12
#define TDAS_N_FEATURES 13 /* TDAS_N_FEATURES includes the TDAS bundle itself. */

/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define TDAS_VERSION                   0 /* set on initialisation, don't overwrite */
#define TDAS_SAMPLERATE                1
#define TDAS_IN_CHANS                  2
#define TDAS_IN_MATRIX                 3
#define TDAS_BYPASS                    4
#define TDAS_RATE_MULTIPLIER           5
#define TDAS_IN_CHAN_FORMAT            6
#define TDAS_DOWNMIX_CONFIG            7 /* DIAGNOSTIC USE: For testing downmixer */
#define TDAS_LFE_MIX_LEVEL             8
#define TDAS_MSR_MAX_PROFILE           9
#define TDAS_REVISION                  10
#define TDAS_PORTABLEMODE              11
#define TDAS_PORTABLEMODE_GAIN         12
#define TDAS_PORTABLEMODE_ORL          13
#define TDAS_PULSEDOWNMIX              14
#define TDAS_PULSEDOWNMIX_ATTEN        15
#define TDAS_LIMITER_TESTMODE          16 /* DIAGNOSTIC USE: For testing the limiter */
#define TDAS_OUT_CHANS		           17
#define TDAS_PARAMLIST_LEN             18

/* TDAS_SAMPLERATE - indices of supported sample rates */
#define SAMPLERATE_48000               0
#define SAMPLERATE_44100               1
#define SAMPLERATE_32000               2
#define SAMPLERATE_24000               3
#define SAMPLERATE_22050               4
#define SAMPLERATE_16000               5
#define SAMPLERATE_12000               6
#define SAMPLERATE_11025               7
#define SAMPLERATE_8000                8
#define N_SAMPLERATES	               9

#define SAMPLERATE_MAX                 SAMPLERATE_48000

/* TDAS feature processing can take place at the following
 * sample rates: 48, 44.1, 32, 24 kHz.
 */
#define TDAS_N_SAMPLERATES             (SAMPLERATE_24000 + 1)

/* TDAS_RATE_MULTIPLIER -
 * Identifiers for the supported upsampler multiplier values. */
#define SAMPLERATE_MULTIPLIER_1        0
#define SAMPLERATE_MULTIPLIER_2        1
#define SAMPLERATE_MULTIPLIER_4        2
#define TDAS_N_SAMPLERATE_MULTIPLIERS  3 

/* TDAS_IN_CHANS - refer to channel enumeration in docs */
/* Note - the ordering of the channel configurations in this
 * enum has been deliberately kept consistent with those in
 * Dolby Digital and Dolby Digital Plus.
 */
#define CHANS_RESERVED                 0
#define CHANS_C	                       1
#define CHANS_L_R                      2
#define CHANS_L_C_R                    3
#define CHANS_L_R_S                    4
#define CHANS_L_C_R_S                  5
#define CHANS_L_R_Ls_Rs                6
#define CHANS_L_C_R_Ls_Rs              7
#define CHANS_L_R_Ls_Rs_Cs             8
#define CHANS_L_R_Ls_Rs_Lb_Rb          9
#define CHANS_L_C_R_Ls_Rs_Cs           10
#define CHANS_L_R_Ls_Rs_Ts_Cs          11
#define CHANS_L_C_R_Ls_Rs_Cvh_Cs       12
#define CHANS_L_C_R_Ls_Rs_Ts_Cs        13
#define CHANS_L_C_R_Ls_Rs_Lb_Rb        14
#define CHANS_RESERVED_2               15
#define CHANS_C_LFE                    16
#define CHANS_L_R_LFE                  17
#define CHANS_L_C_R_LFE                18
#define CHANS_L_R_S_LFE                19
#define CHANS_L_C_R_S_LFE              20
#define CHANS_L_R_Ls_Rs_LFE            21
#define CHANS_L_C_R_Ls_Rs_LFE          22
#define CHANS_L_R_Ls_Rs_Cs_LFE         23
#define CHANS_L_R_Ls_Rs_Lb_Rb_LFE      24
#define CHANS_L_C_R_Ls_Rs_Cs_LFE       25
#define CHANS_L_R_Ls_Rs_Ts_Cs_LFE      26
#define CHANS_L_C_R_Ls_Rs_Cvh_Cs_LFE   27
#define CHANS_L_C_R_Ls_Rs_Ts_Cs_LFE    28
#define CHANS_L_C_R_Ls_Rs_Lb_Rb_LFE    29
#define TDAS_N_IN_CHANS                30

#define TDAS_MAX_IN_CHANS              8  /* Maximum is 7.1 channel input */

/*
 * TDAS_IN_MATRIX - Determines, for 2-channel input (TDAS_IN_CHANS = CHANS_L_R),
 *                      how they are mixed to form the 2-channel signal processed by TDAS.
 *      0: stereo - left and right input channels are treated normally.
 *                  i.e. TDAS processes L, R
 *      1: channel one mono - right channel is replaced by left input channel internally.
 *                  i.e. TDAS processes L, L
 *      2: channel two mono - left channel is replaced by the right input channel internally.
 *                  i.e. TDAS processes R, R
 *      3: dual mono - both channels are summed and replicated to both channels internally.
 *                  i.e. TDAS processes (L+R), (L+R)
 *      4: Dolby Pro-Logic II Encoded stereo content- left and right channels are treated as 
 *                  Pro-Logic II encoded. This is only applicable for use with mobile surround.
 */	
#define	MATRIX_STEREO                  0
#define	MATRIX_LEFT                    1
#define MATRIX_RIGHT                   2
#define MATRIX_DUALMONO	               3
#define	MATRIX_PL2                     4

#define TDAS_N_IN_MATRICES             5

/*
 * TDAS_IN_CHAN_FORMAT - 0: interlaced channels
 *                       1: de-interlaced channels
 */
#define CHAN_FORMAT_INTERLACED         0
#define CHAN_FORMAT_DEINTERLACED       1
#define TDAS_N_CHAN_FORMATS            2

/* Identifiers for the valid internal TDAS channel configurations. 
 * They are also used to specify the TDAS_DOWNMIX_CONFIG bundle
 * parameter.
 *
 * TDAS_DOWNMIX_CONFIG - 0: downmix to shuffled L/R channels (2 channels)
 *                       1: downmix to L, R channels (2 channels)
 *                       2: downmix to shuffled L/R, shuffled Ls/Rs channels (4 channels)
 *                       3: downmix to L, R, Ls, Rs channels (4 channels)
 *                       4: downmix to shuffled L/R, shuffled Ls/Rs, unshuffled C (5 channels)
 *                       5: downmix to L, C, R, Ls, Rs (5 channels)
 *                       6: downmix to shuffled L/R, shuffled Ls/Rs, unshuffled C, shuffled Lb/Rb (7 channels)
 *                       7: downmix to L, C, R, Ls, Rs, Lb, Rb (7 channels)
 */
#define INTERNAL_CHANS_SHUFFLED_L_R                  0
#define INTERNAL_CHANS_UNSHUFFLED_L_R                1
#define INTERNAL_CHANS_SHUFFLED_L_R_Ls_Rs            2
#define INTERNAL_CHANS_UNSHUFFLED_L_R_Ls_Rs          3
#define INTERNAL_CHANS_SHUFFLED_L_C_R_Ls_Rs          4
#define INTERNAL_CHANS_UNSHUFFLED_L_C_R_Ls_Rs        5
#define INTERNAL_CHANS_SHUFFLED_L_C_R_Ls_Rs_Lb_Rb    6
#define INTERNAL_CHANS_UNSHUFFLED_L_C_R_Ls_Rs_Lb_Rb  7
#define TDAS_N_INTERNAL_CHAN_CONFIGS                 8

#define SHUFFLED_CHANS                 0
#define UNSHUFFLED_CHANS               1

/*
 * Downmixer LFE limit mixing values
 * Min mix level is -21 dB
 * Max mix level is +10 dB
 * Actual mix level is calculated as (10 - DMX_LFE_MIX_LEVEL) dB
 * Note: there's a systematic 4.5dB attenuation
 */
#define DMX_LFE_MIX_MIN				31
#define DMX_LFE_MIX_MAX				0
#define DMX_LFE_MIX_DBMAX			10		/* Max LFE gain at doewnmixing stage (DD+) */
#define DMX_LFE_MIX_PAD				18		/* LFE pad for downmix to stereo * 4 (4.5 dB) */
#define DMX_LFE_6DB_PAD				24		/* defined for downmixing procedure optimization */
#define DMX_LFE_MIX_DEFAULT			10		/* Default LFE downmix level */

/* Under normal operation, TDAS produces stereo output,
 * but can also generate 4-channel (L, R, Ls, Rs) output,
 * as well as 5.0 and 7.0
 */
#define TDAS_N_OUT_CHANS_MAX        7
#define TDAS_N_OUT_CHANS_STEREO		2

/* TDAS_BYPASS - bypass defeated (0), soft bypass on (1), hard bypass on (2) */
#define TDAS_N_BYPASSES			3
#define TDAS_DEF_BYPASS                0 /*Default bypass off*/

#define TDAS_DB_SCALE			5 /* right shifts used to represent a 
									   * dB integer (up to 31dB) as a coeff 
									   */ 

#define TDAS_PORTABLEMODE_DISABLE      0     /* Default, Portable Mode Normalisation Disabled. */
#define TDAS_PORTABLEMODE_ENABLE       1     /* Portable Mode Normalisation Enabled */

#define TDAS_PORTABLEMODE_GAIN_SCALE   2     /* Right shifts used to represent a dB integer for PORTABLEMODE_GAIN */
#define TDAS_PORTABLEMODE_GAIN_MIN     -44   /* -11dB is the minimum portable mode gain */
#define TDAS_PORTABLEMODE_GAIN_MAX     160   /* 40dB is the maximum portable mode gain */
#define TDAS_PORTABLEMODE_GAIN_DEF     0     /* 0dB is the default portable mode gain */

#define TDAS_PORTABLEMODE_ORL_8DB      0     /* -8dB Portable Mode ORL */
#define TDAS_PORTABLEMODE_ORL_11DB     1     /* Default, -11dB Portable Mode ORL */
#define TDAS_PORTABLEMODE_ORL_14DB     2     /* -14dB Portable Mode ORL */
#define TDAS_PORTABLEMODE_ORL_DEF      1     /* Default, -11dB Portable Mode ORL */

#define TDAS_PULSEDOWNMIX_DISABLE      0     /* Default, account for Pulse Downmixer Attenuation disabled. */
#define TDAS_PULSEDOWNMIX_ENABLE       1     /* Account for Pulse Downmixer Attenuation enabled. */

#define TDAS_PULSEDOWNMIX_ATTEN_SCALE  2     /* Left shifts used to represent a dB integer for PULSEDOWNMIX_ATTEN */
#define TDAS_PULSEDOWNMIX_ATTEN_MAX    -40   /* -10dB is the maximum Pulse Downmixer attenuation */
#define TDAS_PULSEDOWNMIX_ATTEN_MIN    0     /* 0dB is the minimum Pulse Downmixer attenuation */
#define TDAS_PULSEDOWNMIX_ATTEN_DEF    0     /* 0dB is the default Pulse Downmixer attenuation */

#define TDAS_LIMITER_TESTMODE_DISABLE    0     /* DIAGNOSTIC USE. Default (disabled). */
#define TDAS_LIMITER_TESTMODE_ENABLE     1     /* DIAGNOSTIC USE. Enabled */

/* 
 * Mobile Surround Maximum Profile settings
 *
 */
#define TDAS_MSR_MAX_PROFILE_STEREO  0
#define TDAS_MSR_MAX_PROFILE_FOUR    1
#define TDAS_MSR_MAX_PROFILE_FIVE    2
#define TDAS_MSR_MAX_PROFILE_SEVEN   3
#define TDAS_MSR_N_MAX_PROFILES      4
#define TDAS_MSR_DEF_MAX_PROFILE     TDAS_MSR_MAX_PROFILE_FIVE

/* TRUE/FALSE */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*---------------------------------------------------------------------------*/

/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define M2S_ENABLE			0	/* all features have enable as first param */
#define M2S_DETECTOR		1
#define M2S_PARAMLIST_LEN	2

/* M2S_ENABLE - 0 is feature off, 1 is feature on */
#define M2S_N_ENABLES		2

/* M2S_DETECTOR - 0 is mono detection off, 1 is mono detection on */
#define M2S_N_DETECTORS	2

/*---------------------------------------------------------------------------*/

/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define SSH_ENABLE			0	/* all features have enable as first param */
#define SSH_MODE			1
#define SSH_PARAMLIST_LEN	2

/* SSH_ENABLE - 0 is feature off, 1 is feature on */
#define SSH_N_ENABLES		2

/* SSH_MODE - the modes are a combination of distance and width, as 
 * described in the system integration manual
 */
#define SSH_N_MODES		7	/* 7 distance/width combinations */

/*---------------------------------------------------------------------------*/

/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define SSE_ENABLE			0	/* all features have enable as first param */
#define SSE_WIDTH			1
#define SSE_SPK_MODE		2
#define SSE_PARAMLIST_LEN	3

/* SSE_ENABLE - 0 is feature off, 1 is feature on */
#define SSE_N_ENABLES		2

/* SSE_WIDTH - 0 is half width, 1 is full width */
#define SSE_N_WIDTHS		2

/* Define mode index */
#define SSE_HALF_MODE		0
#define SSE_FULL_MODE		1

/* SSE_SPK_MODE - 0 is normal, 1 is narrow and 2 is binarual mode */
#define SSE_N_SPK_MODES	3

#define SSE_NORMAL_MODE   0
#define SSE_NARROW_MODE	1
#define SSE_BINAURAL_MODE 2

/*---------------------------------------------------------------------------*/

/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define SLC_ENABLE			0	/* all features have enable as first param */
#define SLC_LEVEL			1
#define SLC_DEPTH			2
#define SLC_PARAMLIST_LEN	3

/* SLC_ENABLE - 0 is feature off, 1 is feature on */
#define SLC_N_ENABLES		2

/* SLC_LEVEL - discrete input audio levels, 0-6, 0 is really loud music,
 *                   6 is quiet film
 */
#define SLC_N_LEVELS		7

/* SLC_DEPTH - depth of effect, 0 is light, 1 is medium, 2 is 
 * comprehensive
 */
#define SLC_N_DEPTHS		3	/* night, evening, and midnight */

/*---------------------------------------------------------------------------*/

/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define VOL_ENABLE			0	/* all features have enable as first param */
#define VOL_GAIN_INTERNAL	    1
#define VOL_GAIN_EXTERNAL	2
#define VOL_GAIN_TYPE		3
#define VOL_BALANCE			4
#define VOL_MUTE			5				
#define VOL_PARAMLIST_LEN	6

/* VOL_ENABLE - 0 is feature off, 1 is feature on */
#define VOL_N_ENABLES		2

/*
 * Volume is defined as 2^(VOL_BITS_IN_GAIN-1) discrete steps.
 */
#define VOL_BITS_IN_GAIN		8
#define VOL_N_GAINS_TDAS	    (1<<(VOL_BITS_IN_GAIN))
#define VOL_N_GAINS_EXTERNAL	(1<<(VOL_BITS_IN_GAIN))

/* VOL_GAIN_TYPE - 0 is linear gain, 1 is log gain */
#define VOL_N_GAIN_TYPES		2

#define VOL_LINEAR              0
#define VOL_LOGARITHMIC         1

/* VOL_BALANCE
 * This is specified as a signed integer value from -128 to 127.
 * -128 corresponds with audio fully panned to the left,
 * 127 corresponds with audio fully panned to the right.
 */
#define VOL_BITS_IN_BALANCE	(VOL_BITS_IN_GAIN-1)
#define VOL_BALANCE_MIN		(-1<<VOL_BITS_IN_BALANCE)
#define VOL_BALANCE_MAX		((1<<VOL_BITS_IN_BALANCE)-1)


/* VOL_MUTE - 0 is not muted, 1 is muted */
#define VOL_N_MUTES		2

/*---------------------------------------------------------------------------*/

/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define NB_ENABLE			0	/* all features have enable as first param */
#define NB_CUTOFF			1
#define NB_BOOST			2
#define NB_LEVEL			3
#define NB_PARAMLIST_LEN	4

/* NB_ENABLE - 0 is feature off, 1 is feature on */
#define NB_N_ENABLES		2

/* NB_CUTOFF - discrete cutoff frequencies, as described in the 
 * system integration manual 
 */
#define NB_N_CUTOFFS		24

/* NB_BOOST - discrete boost levels, as described in the 
 * system integration manual
 */
#define NB_N_BOOSTS		7

/* NB_LEVEL - defines the output capabilities of your speakers, as 
 * described in the system integration manual
 */
#define NB_N_LEVELS		7

/*---------------------------------------------------------------------------*/

/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define GEQ_ENABLE			0	/* all features have enable as first param */
#define GEQ_NBANDS			1
#define GEQ_PREAMP			2
#define GEQ_MAXBOOST		3
#define GEQ_BAND1			4   /* all geq bands must be sequential */
#define GEQ_BAND2			5
#define GEQ_BAND3			6
#define GEQ_BAND4			7
#define GEQ_BAND5			8
#define GEQ_BAND6			9
#define GEQ_BAND7			10
#define GEQ_PARAMLIST_LEN	11

#define GEQ_STEP_BITS		2	
#define GEQ_STEP			(1<<GEQ_STEP_BITS)	  /* 0.25 dB step size */
			
#define GEQ_MINGAIN			(-12<<GEQ_STEP_BITS)  /* minimum allowed gain in dB */
#define GEQ_MAXGAIN			(12<<GEQ_STEP_BITS)   /* maximum allowed gain in dB */

/* limits are applied to the range of preamp and maxboost */
#define GEQ_MIN_PREAMP		GEQ_MINGAIN	
#define GEQ_MAX_PREAMP		0

#define GEQ_MIN_MAXBOOST	0
#define GEQ_MAX_MAXBOOST	GEQ_MAXGAIN

/* Default GEQ settings */
#define GEQ_DEFAULT_PREAMP  0					/* default to 0dB */
#define GEQ_DEFAULT_MAXBOOST (6<<GEQ_STEP_BITS) /* default to 6dB */

/* GEQ_ENABLE - 0 is feature off, 1 is feature on */
#define GEQ_N_ENABLES		2

/* GEQ_NBANDS */
#define GEQ_N_BANDS_MIN		2
#define GEQ_N_BANDS_MAX		7

/*---------------------------------------------------------------------------*/

/* 
 * Mobile Surround Upmixer Feature Parameters
 * 
 * Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */

/* DIAGNOSTIC USE ONLY: For testing the mobile upmixer. */
#define MUP_ENABLE		0	/* all features have enable as first param */
#define MUP_PARAMLIST_LEN	1

/* DIAGNOSTIC USE ONLY: For testing the mobile upmixer. */
/* Number of MUP enable modes - 0 is feature off, 1 is feature on */
#define MUP_N_ENABLES		2

/*---------------------------------------------------------------------------*/
/* 
 * Mobile Surround Feature Parameters
 *
 * Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */

/* Number of MSR enable modes - 0 is feature off, 1 is feature on */

#define MSR_ENABLE          0	/* all features have enable as first param */
#define MSR_ROOM			1
#define MSR_BRIGHT			2
#define MSR_PARAMLIST_LEN   3

/* Number of MSR enable modes - 0 is feature off, 1 is feature on */
#define MSR_N_ENABLES		2

#ifdef MOBILE_SURROUND_EX
	/* Number of MSR room modes */
	#define MSR_N_ROOMS			5
	
	/* Number of MSR brightness modes */
	#define MSR_N_BRIGHTS		5
	
	/* Default MSR room mode */
	#define MSR_DEF_ROOM		2

	/* Default MSR brightness */
	#define MSR_DEF_BRIGHT		2
#else
	/* Number of MSR room modes */
	#define MSR_N_ROOMS			3
	
	/* Number of MSR brightness modes */
	#define MSR_N_BRIGHTS		3

	/* Default MSR room mode */
	#define MSR_DEF_ROOM		1

	/* Default MSR brightness */
	#define MSR_DEF_BRIGHT		1
#endif


/*---------------------------------------------------------------------------*/
/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define HFE_ENABLE			0	/* all features have enable as first param */
#define HFE_DEPTH			1
#define HFE_PARAMLIST_LEN	2

/* HFE_ENABLE - 0 is feature off, 1 is feature on */
#define HFE_N_ENABLES		2

#define HFE_N_DEPTHS		7

/*---------------------------------------------------------------------------*/

/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define FADE_ENABLE			0	/* all features have enable as first param */
#define FADE_TARGET			1
#define FADE_TIME			2
#define FADE_PARAMLIST_LEN	3

/* FADE_ENABLE - 0 is feature off, 1 is feature on */
#define FADE_N_ENABLES		2

#define FADE_TIME_LUT_SCALE_SHIFT	5												/* 1 << 5 = 32 msec increments */
#define FADE_TIME_LUT_MAX_INDEX		31

#define FADE_TIME_LUT_MAX_VAL		(FADE_TIME_LUT_MAX_INDEX << FADE_TIME_LUT_SCALE_SHIFT)

#define FADE_TIME_MIN (1 << FADE_TIME_LUT_SCALE_SHIFT)								/* 32   msec */
#define FADE_TIME_MAX ((FADE_TIME_LUT_MAX_INDEX << FADE_TIME_LUT_SCALE_SHIFT) * 4)	/* 3968 msec */

/*---------------------------------------------------------------------------*/

/* Parameters are indexed in the state structure by their position in the 
 * array. Each parameter is of type INTEGER.
 */
#define SPKEQ_ENABLE					0	/* all features have enable as first param */
#define SPKEQ_CHGAIN_DB1			1	/* User post-processing gain on channel 1 (dB)*/
#define SPKEQ_CHGAIN_DB2			2	/* User post-processing gain on channel 2 (dB)*/
#define SPKEQ_PARAMLIST_LEN		3	

#define SPKEQ_STEP_BITS			2	/* 2 bits -> 4 steps -> 0.25 dB per step */
#define SPKEQ_STEP				(1<<SPKEQ_STEP_BITS)	/* 0.25 dB step size */

/* SPKEQ Min/Max settings */
#define SPKEQ_MIN_CHGAIN_DB		(-6<<SPKEQ_STEP_BITS)  /* minimum allowed gain in dB */
#define SPKEQ_MAX_CHGAIN_DB		(6<<SPKEQ_STEP_BITS)   /* maximum allowed gain in dB */

/* Default SPKEQ settings */
#define SPKEQ_DEFAULT_CHGAIN_DB	0	/* defaults to 0 dB for both channels */

/* SPKEQ_ENABLE - 0 is feature off, 1 is feature on */
#define SPKEQ_N_ENABLES			2

/* Maximum allowed number of biquads is 4 */
#define SPKEQ_MAX_N_FILTERS		4

/* Coefs table size, one such table for each sampling frequency */
#define SPKEQ_COEF_TABLE_SZ		(2+2*SPKEQ_MAX_N_FILTERS*(5+1))

/* Error Return codes */
#define STATUS_OK			0
#define ERR_NOT_MUL_BLKSZ	-1
#define ERR_POINTER_NULL	-2

#endif /* TDAS_H */



