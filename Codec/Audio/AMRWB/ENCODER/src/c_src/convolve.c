/***********************************************************************
*                                                                      *
*         VisualOn, Inc. Confidential and Proprietary, 2003-2010       *
*                                                                      *
************************************************************************/
/***********************************************************************
       File: convolve.c

	   Description:Perform the convolution between two vectors x[] and h[]
	               and write the result in the vector y[]

************************************************************************/

#include "typedef.h"
#include "basic_op.h"

#if 1
void Convolve (
    Word16 x[],        /* (i)     : input vector                           */
    Word16 h[],        /* (i)     : impulse response                       */
    Word16 y[],        /* (o)     : output vector                          */
    Word16 L           /* (i)     : vector size                            */
)
{
	Word32  i, n;
    Word16 *tmpH,*tmpX;
    Word32 s;
   for (n = 0; n < 64;)
   {
	   tmpH = h+n;
	   tmpX = x;
	   i=n+1;
	   s = (*tmpX++) * (*tmpH--);i--;
	   while(i>0)
	   {
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   i -= 4;
	   }
	   y[n] = ((s<<1) + 0x8000)>>16;   
	   n++;

	   tmpH = h+n;
	   tmpX = x;
	   i=n+1;
	   s = (*tmpX++) * (*tmpH--);i--;
	   s += (*tmpX++) * (*tmpH--);i--;

	   while(i>0)
	   {
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   i -= 4;
	   }
	   y[n] = ((s<<1) + 0x8000)>>16;    
	   n++;

	   tmpH = h+n;
	   tmpX = x;
	   i=n+1;
	   s = (*tmpX++) * (*tmpH--);i--;
	   s += (*tmpX++) * (*tmpH--);i--;
	   s += (*tmpX++) * (*tmpH--);i--;

	   while(i>0)
	   {
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   i -= 4;
	   }
	   y[n] = ((s<<1) + 0x8000)>>16;   
	   n++;

	   s = 0;
	   tmpH = h+n;
	   tmpX = x;
	   i=n+1;
	   while(i>0)
	   {
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   s += (*tmpX++) * (*tmpH--);
		   i -= 4;
	   }
	   y[n] = ((s<<1) + 0x8000)>>16;   
	   n++;        
   }
   return;
}
#else
void Convolve(
			  Word16 x[],                           /* (i)        : input vector                           */
			  Word16 h[],                           /* (i) Q15    : impulse response                       */
			  Word16 y[],                           /* (o) 12 bits: output vector                          */
			  Word16 L                              /* (i)        : vector size                            */
			  )
{
	Word16 i, n;
	Word32 L_sum;

	for (n = 0; n < L; n++)
	{
		L_sum = 0L;                        
		for (i = 0; i <= n; i++)
			L_sum += (x[i] * h[n - i])<<1;

		y[n] = (L_sum + 0x8000)>>16;              
	}

	return;
}
#endif
