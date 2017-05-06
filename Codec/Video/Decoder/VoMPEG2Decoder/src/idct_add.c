/* Reference_IDCT.c, Inverse Discrete Fourier Transform, double precision          */

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

/*
* Disclaimer of Warranty
*
* These software programs are available to the user without any license fee or
* royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
* any and all warranties, whether express, implied, or statuary, including any
* implied warranties or merchantability or of fitness for a particular
* purpose.  In no event shall the copyright-holder be liable for any
* incidental, punitive, or consequential damages of any kind whatsoever
* arising from the use of these programs.
*
* This disclaimer of warranty extends to the user of these programs and user's
* customers, employees, agents, transferees, successors, and assigns.
*
* The MPEG Software Simulation Group does not represent or warrant that the
* programs furnished hereunder are free of infringement of any third-party
* patents.
*
* Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
* are subject to royalty fees to patent holders.  Many of these patents are
* general enough such that they are unavoidable regardless of implementation
* design.
*
*/

/*  Perform IEEE 1180 reference (64-bit floating point, separable 8x1
*  direct matrix multiply) Inverse Discrete Cosine Transform
*/


/* Here we use math.h to generate constants.  Compiler results may
vary a little */

#include <math.h>
#include "voMpeg2Decoder.h"
#ifdef REF_DCT
//#include "config.h"


#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif

/* global declarations */
VO_VOID Initialize_Fast_IDCTref(VO_VOID);
VO_VOID Reference_IDCT(short *block);

/* private data */

/* cosine transform matrix for 8x1 IDCT */
static double c[8][8];

/* initialize DCT coefficient matrix */
VO_U8 *Clip;
VO_VOID Initialize_Reference_IDCT()
{
	int freq, time;
	double scale;
	
	for (freq=0; freq < 8; freq++)
	{
		scale = (freq == 0) ? sqrt(0.125) : 0.5;
		for (time=0; time<8; time++)
			c[freq][time] = scale*cos((PI/8.0)*freq*(time + 0.5));
	}
}

VO_VOID Initialize_Decoder()
{
	int i;
	
	/* Clip table */
	if (!(Clip=(VO_U8 *)malloc(1024)))
		return;
	
	Clip += 384;
	
	for (i=-384; i<640; i++)
		Clip[i] = (i<0) ? 0 : ((i>255) ? 255 : i);
	
	/* IDCT */
	//	if (Reference_IDCT_Flag)
	Initialize_Reference_IDCT();
	//	else
	//		Initialize_Fast_IDCT();
	
}



/* perform IDCT matrix multiply for 8x8 coefficient block */

VO_VOID Reference_IDCT(short *block)
{
	int i, j, k, v;
	double partial_product;
	double tmp[64];
	
	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
		{
			partial_product = 0.0;
			
			for (k=0; k<8; k++)
				partial_product+= c[k][j]*block[8*i+k];
			
			tmp[8*i+j] = partial_product;
		}
		
		/* Transpose operation is integrated into address mapping by switching 
		loop order of i and j */
		
		for (j=0; j<8; j++)
			for (i=0; i<8; i++)
			{
				partial_product = 0.0;
				
				for (k=0; k<8; k++)
					partial_product+= c[k][i]*tmp[8*k+j];
				
				v = (int) floor(partial_product+0.5);
				block[8*i+j] = (v<-256) ? -256 : ((v>255) ? 255 : v);
			}
}


VO_VOID Add_Block(short *src, VO_U8 *dst, int dst_stride, int addflag, VO_U8 *src1, VO_S32 src_stride)
{
	int i;
	
	if (addflag){
		for (i=0; i<8; i++){
			dst[0] = Clip[src[0] + src1[0]];
			dst[1] = Clip[src[1] + src1[1]];
			dst[2] = Clip[src[2] + src1[2]];
			dst[3] = Clip[src[3] + src1[3]];
			dst[4] = Clip[src[4] + src1[4]];
			dst[5] = Clip[src[5] + src1[5]];
			dst[6] = Clip[src[6] + src1[6]];
			dst[7] = Clip[src[7] + src1[7]];
			src += 8;
			src1 += src_stride;
			dst += dst_stride;
		}
    }else{
		for (i=0; i<8; i++){
			dst[0] = Clip[src[0]];
			dst[1] = Clip[src[1]];
			dst[2] = Clip[src[2]];
			dst[3] = Clip[src[3]];
			dst[4] = Clip[src[4]];
			dst[5] = Clip[src[5]];
			dst[6] = Clip[src[6]];
			dst[7] = Clip[src[7]];
			src += 8;
			dst += dst_stride;
		}
	}
}

#endif

