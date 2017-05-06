/*
*  Copyright 2003 - 2010 by Visualon software Incorporated.
*  All rights reserved. Property of Visualon software Incorporated.
*  Restricted rights to use, duplicate or disclose this code are
*  granted through contract.
*/

/***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver         Description             Author
;*    -----------     --------     ------------------        -----------
;*    06-21-2010        1.0        File imported from        Huaping Liu
;*                                             
;**********************************************************************/

#ifndef __VOEQ_H__
#define __VOEA_H__

typedef   void*             HVOCODEC;
typedef   unsigned char     BYTE;
typedef   signed long		LONG;
#define   ERR_NONE			0

#define  FIX_CODE

typedef struct 
{
	BYTE *buffer;            /*!< Buffer pointer */
	LONG length;             /*!< Buffer size in byte */
}
VOCODECDATABUFFER;

#define   pi    3.1415926
/* EQ 10 bands Frequency */
//static int EQ_Band[10] = {31, 62, 125, 250, 500, 1000, 2000, 4000, 8000, 16000};
static int EQ_Band[10] = {100, 200, 400, 600, 1000, 3000, 6000, 12000, 14000, 16000};

/* Bands frequency Boost or cut dB */
static int Eq_Type[][10] = {
	{ 0,  3,   5,   6,   5,  0,  -1,  -1, -1,  -1},  /* Pop Music */
	{-2,  1,   4,   5,   5,  5,   2,  2,   1,   0},  /* Live */
	{ 0,  6,  -5,   2,   8,  7,   6,  1,   1,   1},  /* Jazz */
	{ 5,  3,   0,  -2,   0,  3,   6,  6,   6,   7},  /* Rock */
	{ 10, 9,   8,   4,   2,  0,  -2, -3,  -4,  -4}, /* Bass */
	{ 2,  2,   2,   2,   2,  2,  -2, -2,  -2,  -3},  /* Classical */
	{ -6, -6, -6,   -3,  1,  3,   9,  9,   9,   9},  /* Treble */
};

typedef struct 
{
	int     Eq_mode;                     /* setting by setparameter */
	int     sample_rate;
	int     channel;  
	int     Cal_flag;
	float   a[10][3];                    /* 2-Orders IIR filter a coefficients */
	float   b[10][3];                    /* 2-Orders IIR filter b coefficients */

#ifdef FIX_CODE
	short   xl_mem[10][2];
	short   xr_mem[10][2];

	short   ylo_mem[10][2];
	short   ylh_mem[10][2];

	short   yro_mem[10][2];
	short   yrh_mem[10][2];
#else

	float   xl_mem[10][2];               /* the left channel X past elements */
	double  yl_mem[10][2];               /* the left channel Y past elements */
	float   xr_mem[10][2];               /* the right channel X past elements */
	double  yr_mem[10][2];               /* the right channel Y past elements */
#endif

}voEq_State;

static int sr[9] = {48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000};

/* The sample rate of the PCM */

typedef enum
{
	sr_48000,     /* sample rate is 48k */
	sr_44100,     /* sample rate is 44.1k */
	sr_32000,     /* sample rate is 32k */
	sr_24000,     /* sample rate is 24k */
	sr_22050,     /* sample rate is 22.05k */
	sr_16000,     /* sample rate is 16k */
	sr_12000,     /* sample rate is 12k */
	sr_11025,     /* sample rate is 11k */
	sr_8000,      /* sample rate is  8k */
}EQ_SAMPLE_RATE;

/* The normal equalizer mode */

typedef enum
{
	NONE	= -1,   /* no mode */
	POP     = 0,    /* Pop Music */
	LIVE,		    /* Live Music */
	JAZZ,		    /* Jazz Music */
	ROCK,		    /* Rock */
	BASS,		    /* Bass */
	CLASS,          /* Classical */
	TREBLE,		    /* Treble */
}EQ_EQUAL_MODE;

/* The setParameter ID */

typedef enum
{
	VO_EQ_ONOFF			= 0,  /* set equalzier used or not, if set nonzero, used, else no used, default no used */
	VO_SAMPLERATE  		= 1,  /* set input pcm channel samplerate, see enum EQ_SAMPLE_RATE, default sr_44100 */
	VO_CHANNELNUM		= 2,  /* set input pcm channel number, default 2 channel */
	VO_EQMODE			= 10, /* set equalzier mode, see enum EQ_EQUAL_MODE,  default NEW_AGE */
}EQ_PARAMETER_ID;


/*!
 * initialize equalzier
 * This function should be called first once the graphic equalizer is modified.
 * \param phEQ [in/out] EQ instance handle;
 * \retval 0 successful,others fail
*/
int	voEQInit(HVOCODEC *phEQ);

/*!
 * notify the equalizer to process the input data 
 * \param phEQ [in/out] equalizer instance handle;
 * \param pInData [in/out] Input buffer pointer and length. 
 *  When the function return, pInData->length indicates the actual size consumed by eq.
 * \param pOutData [in/out] Output buffer pointer and length. The buffer alloc and release by caller
 *  When the function return 0, pOutData->length indicates the actual output size.
 * \retval 0 successful,others fail
*/
int	voEQProcess(HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECDATABUFFER *pOutData);
/*!
 * clean up equalizer;
 *\param phEQ [in/out] equalizer instance handle;
 * \retval 0 successful,others fail
*/
int voEQUnInit(HVOCODEC hCodec);

/**
 * Set parameter of the equalizer instance.
 * \param hCodec [in] equalizer  instance handle
 * \param nID [in] Parameter ID
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
int voEQSetParameter(HVOCODEC hCodec, LONG nID, LONG* plValue);

/**
 * Get parameter of the equalizer instance..
 * \param hCodec [in] Mequalizer  instance handle
 * \param nID [in] Parameter ID
 * \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
int voEQGetParameter(HVOCODEC hCodec, LONG nID, LONG* plValue);


#endif  //__VOEQ_H__



