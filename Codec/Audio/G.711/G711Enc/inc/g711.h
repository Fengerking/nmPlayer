/****************************************************************************************
**
**   ITU-T G.711 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
**   ?2004 Polycom, Inc.
**
**	 All rights reserved.
**
****************************************************************************************/

#ifndef  __G711_H__
#define  __G711_H__

#include "cmnMemory.h"

#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)		/* Number of A-law segments. */
#define	SEG_SHIFT	(4)		/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */
#define	BIAS		(0x84)		/* Bias for linear code. */
#define G711_MEM_ALIGN  32
#define G711_MEM_ALIGN_MASK 31

typedef struct{
     short    *input_buffer;
	 unsigned long    input_len;
	 unsigned long    output_len;
	 short    Workmode;
     unsigned char *output_buffer;
	 VO_MEM_OPERATOR  *pvoMemop;
	 VO_MEM_OPERATOR  voMemoprator;	
	 void *hCheck;
}G711EncStat;

typedef struct{
     short   *output_buffer;
	 short   output_len;
	 short   input_len;
	 short   Workmode;
	 unsigned char *input_buffer;
	 VO_MEM_OPERATOR  *pvoMemop;
	 VO_MEM_OPERATOR  voMemoprator;	
	 void *hCheck;
}G711DecStat;

#if 0
/* copy from CCITT G.711 specifications */
static unsigned char _u2a[128] = {			/* u- to A-law conversions */
	1,	1,	2,	2,	3,	3,	4,	4,
	5,	5,	6,	6,	7,	7,	8,	8,
	9,	10,	11,	12,	13,	14,	15,	16,
	17,	18,	19,	20,	21,	22,	23,	24,
	25,	27,	29,	31,	33,	34,	35,	36,
	37,	38,	39,	40,	41,	42,	43,	44,
	46,	48,	49,	50,	51,	52,	53,	54,
	55,	56,	57,	58,	59,	60,	61,	62,
	64,	65,	66,	67,	68,	69,	70,	71,
	72,	73,	74,	75,	76,	77,	78,	79,
	81,	82,	83,	84,	85,	86,	87,	88,
	89,	90,	91,	92,	93,	94,	95,	96,
	97,	98,	99,	100,	101,	102,	103,	104,
	105,	106,	107,	108,	109,	110,	111,	112,
	113,	114,	115,	116,	117,	118,	119,	120,
	121,	122,	123,	124,	125,	126,	127,	128};

static unsigned char _a2u[128] = {			/* A- to u-law conversions */
	1,	3,	5,	7,	9,	11,	13,	15,
	16,	17,	18,	19,	20,	21,	22,	23,
	24,	25,	26,	27,	28,	29,	30,	31,
	32,	32,	33,	33,	34,	34,	35,	35,
	36,	37,	38,	39,	40,	41,	42,	43,
	44,	45,	46,	47,	48,	48,	49,	49,
	50,	51,	52,	53,	54,	55,	56,	57,
	58,	59,	60,	61,	62,	63,	64,	64,
	65,	66,	67,	68,	69,	70,	71,	72,
	73,	74,	75,	76,	77,	78,	79,	79,
	80,	81,	82,	83,	84,	85,	86,	87,
	88,	89,	90,	91,	92,	93,	94,	95,
	96,	97,	98,	99,	100,	101,	102,	103,
	104,	105,	106,	107,	108,	109,	110,	111,
	112,	113,	114,	115,	116,	117,	118,	119,
	120,	121,	122,	123,	124,	125,	126,	127};

#endif
//G.711 encoder function
unsigned char voG711EncLinear2alaw(short pcm_val);      //linear pcm to alaw
unsigned char voG711EncLinear2ulaw(short pcm_val);      //linear pcm to ulaw

//G.711 decoder function
short ulaw2linear(unsigned char u_val);        //ulaw to linear pcm
short alaw2linear(unsigned char a_val);        //alaw to linear pcm

//A-law to u-law conversion
unsigned char alaw2ulaw(unsigned char aval); 

//u-law to A-law conversion
unsigned char ulaw2alaw(unsigned char uval);

#endif //__G711_H__

