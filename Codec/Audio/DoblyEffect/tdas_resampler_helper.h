/******************************************************************************
TDAS Implementation Kit

  Confidential Information - Limited distribution to authorized persons only.
  This material is protected under international copyright laws as an 
  unpublished work. Do not copy. 
  Copyright (C) 2004-2010 Dolby Laboratories Inc. All rights reserved.

	File:           audistry_resampler_helper.h
	Contributors:	Quang Thai
	Description:    C implementation for DSPs
	  
******************************************************************************/


#ifndef AUDISTRY_RESAMPLER_HELPER_H
#define AUDISTRY_RESAMPLER_HELPER_H


/*
 * A lookup table to obtain the HIGHEST multiplier value available, 
 * and the corresponding TDAS sample rate, given a value of the
 * input signal sample rate.
 * - First array index is for the input signal sample rate.
 * - Second array index is for the multiplier value, followed by the
 *   output sample rate.
 */
static const int
multiplier_tdasrate_map[N_SAMPLERATES][2] =
{
    {SAMPLERATE_MULTIPLIER_1, SAMPLERATE_48000},  /* 48000 Hz input */
    {SAMPLERATE_MULTIPLIER_1, SAMPLERATE_44100},  /* 44100 Hz input */
    {SAMPLERATE_MULTIPLIER_1, SAMPLERATE_32000},  /* 32000 Hz input */
    {SAMPLERATE_MULTIPLIER_2, SAMPLERATE_48000},  /* 24000 Hz input */
    {SAMPLERATE_MULTIPLIER_2, SAMPLERATE_44100},  /* 22050 Hz input */
    {SAMPLERATE_MULTIPLIER_2, SAMPLERATE_32000},  /* 16000 Hz input */
    {SAMPLERATE_MULTIPLIER_4, SAMPLERATE_48000},  /* 12000 Hz input */
    {SAMPLERATE_MULTIPLIER_4, SAMPLERATE_44100},  /* 11025 Hz input */
    {SAMPLERATE_MULTIPLIER_4, SAMPLERATE_32000}   /*  8000 Hz input */
};

/*
 * A lookup table to obtain the LOWEST multiplier value available, and
 * the corresponding TDAS sample rate, given a value of the
 * input signal sample rate.
 * - First array index is for the input signal sample rate.
 * - Second array index is for the multiplier value, followed by the
 *   output sample rate.
 */
static const int
multiplier_tdasrate_min_map[N_SAMPLERATES][2] =
{
    {SAMPLERATE_MULTIPLIER_1, SAMPLERATE_48000},  /* 48000 Hz input */
    {SAMPLERATE_MULTIPLIER_1, SAMPLERATE_44100},  /* 44100 Hz input */
    {SAMPLERATE_MULTIPLIER_1, SAMPLERATE_32000},  /* 32000 Hz input */
    {SAMPLERATE_MULTIPLIER_1, SAMPLERATE_24000},  /* 24000 Hz input */
    {SAMPLERATE_MULTIPLIER_2, SAMPLERATE_44100},  /* 22050 Hz input */
    {SAMPLERATE_MULTIPLIER_2, SAMPLERATE_32000},  /* 16000 Hz input */
    {SAMPLERATE_MULTIPLIER_2, SAMPLERATE_24000},  /* 12000 Hz input */
    {SAMPLERATE_MULTIPLIER_4, SAMPLERATE_44100},  /* 11025 Hz input */
    {SAMPLERATE_MULTIPLIER_4, SAMPLERATE_32000}   /*  8000 Hz input */
};




/* 
 * A lookup table that defines the validity between output sample rate values
 * and resampler multiplier values.
 * The first index is the output sample rate.
 * The second index is the resampler multiplier ratio.
 * A 0 indicates an invalid combination, a 1 indicates a valid combination.
 */
static const int
outputrate_multiplier_validity_map[N_SAMPLERATES][TDAS_N_SAMPLERATE_MULTIPLIERS] =
{
    {1, 1, 1},  /* 48000 Hz output */
    {1, 1, 1},  /* 44100 Hz output */
    {1, 1, 1},  /* 32000 Hz output */
    {1, 1, 0},  /* 24000 Hz output */
    {1, 0, 0},  /* 22050 Hz output */
    {1, 0, 0},  /* 16000 Hz output */
    {1, 0, 0},  /* 12000 Hz output */
    {1, 0, 0},  /* 11025 Hz output */
    {1, 0, 0},  /*  8000 Hz output */
};


/*
 * A lookup table to convert a sample rate multiplier identifier into 
 * its actual multiplier value.
 */
static const int
multiplier_value_map[TDAS_N_SAMPLERATE_MULTIPLIERS] = {1, 2, 4};




/* This is a helper function for the TDAS upsampler.  It helps the user determine,
 * given an input sample rate parameter (SAMPLERATE_XXXXX), the HIGHEST available
 * output sample rate that the input can be upsampled to.
 * 
 * The return value is an TDAS sample rate parameter to use (SAMPLERATE_XXXXX).
 * This can be used to set the TDAS_SAMPLERATE parameter of the TDAS_BUNDLE feature.
 * input_rate_value is returned if the input signal can not be upsampled.
 * -1 is returned if the requested input sample rate cannot be supported.
 */
static inline int
tdas_resampler_get_output_samplerate(int input_rate_value)
{
    int m = -1;


    if ((input_rate_value >= 0) && (input_rate_value < N_SAMPLERATES))
    {
        m = multiplier_tdasrate_map[input_rate_value][1];
    }

    return m;
}

/* This is a helper function for the TDAS upsampler.  It helps the user determine,
 * given an input sample rate parameter (SAMPLERATE_XXXXX), the LOWEST available
 * output sample rate that the input can be upsampled to.
 * 
 * The return value is an TDAS sample rate parameter to use (SAMPLERATE_XXXXX).
 * This can be used to set the TDAS_SAMPLERATE parameter of the TDAS_BUNDLE feature.
 * input_rate_value is returned if the input signal can not be upsampled.
 * -1 is returned if the requested input sample rate cannot be supported.
 */
static inline int
tdas_resampler_get_min_output_samplerate(int input_rate_value)
{
    int m = -1;


    if ((input_rate_value >= 0) && (input_rate_value < N_SAMPLERATES))
    {
        m = multiplier_tdasrate_min_map[input_rate_value][1];
    }

    return m;
}


/* This is a helper function for the TDAS upsampler.  It helps the user determine,
 * given an input sample rate parameter (SAMPLERATE_XXXXX), the HIGHEST available
 * multiplier value for the TDAS upsampler (SAMPLERATE_MULTIPLIER_X) that can be
 * applied to the input.
 * 
 * The return value is an TDAS upsampler multiplier parameter to use (SAMPLERATE_MULTIPLIER_X).
 * This can be used to set the TDAS_RATE_MULTIPLIER parameter of the TDAS_BUNDLE feature.
 * SAMPLERATE_MULTIPLIER_1 is returned if the input signal can not be upsampled.
 * -1 is returned if no multiplier is available to support the requested input
 * sample rate.
 */
static inline int
tdas_resampler_get_multiplier(int input_rate_value)
{
    int m = -1;


    if ((input_rate_value >= 0) && (input_rate_value < N_SAMPLERATES))
    {
        m = multiplier_tdasrate_map[input_rate_value][0];
    }

    return m;
}

/* This is a helper function for the TDAS upsampler.  It helps the user determine,
 * given an input sample rate parameter (SAMPLERATE_XXXXX), the LOWEST available
 * multiplier value for the TDAS upsampler (SAMPLERATE_MULTIPLIER_X) that can be
 * applied to the input.
 * 
 * The return value is an TDAS upsampler multiplier parameter to use (SAMPLERATE_MULTIPLIER_X).
 * This can be used to set the TDAS_RATE_MULTIPLIER parameter of the TDAS_BUNDLE feature.
 * SAMPLERATE_MULTIPLIER_1 is returned if the input signal can not be upsampled.
 * -1 is returned if no multiplier is available to support the requested input
 * sample rate.
 */
static inline int
tdas_resampler_get_min_multiplier(int input_rate_value)
{
    int m = -1;


    if ((input_rate_value >= 0) && (input_rate_value < N_SAMPLERATES))
    {
        m = multiplier_tdasrate_min_map[input_rate_value][0];
    }

    return m;
}


/* This is a helper function for the TDAS upsampler.  It maps a multiplier
 * parameter value (SAMPLERATE_MULTIPLIER_X) into a 'real' multiplier ratio value.
 * 0 is returned if the supplied multiplier parameter value is invalid.
 */
static inline int
tdas_resampler_real_multiplier(int multiplier_id)
{
    int m = 0;


    if ((multiplier_id >= 0) && (multiplier_id < TDAS_N_SAMPLERATE_MULTIPLIERS))
    {
        m = multiplier_value_map[multiplier_id];
    }

    return m;
}



#endif


