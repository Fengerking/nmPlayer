/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : syn_filt.c
*      Purpose          : Perform synthesis filtering through 1/A(z).
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "syn_filt.h"
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/*
*--------------------------------------*
* Constants (defined in cnst.h         *
*--------------------------------------*
*  M         : LPC order               *
*--------------------------------------*
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
void Syn_filt (
    Word16 a[],     /* (i)     : a[M+1] prediction coefficients   (M=10)  */
    Word16 x[],     /* (i)     : input signal                             */
    Word16 y[],     /* (o)     : output signal                            */
    Word16 lg,      /* (i)     : size of filtering                        */
    Word16 mem[],   /* (i/o)   : memory associated with this filtering.   */
    Word16 update   /* (i)     : 0=no update, 1=update of memory.         */
)
#ifdef C_OPT
{
	nativeInt  i;
	Word32 s;
	Word16 tmp[50];   /* This is usually done by memory allocation (lg+M) */
	Word16 *yy,*tmpA;
    memcpy(tmp, mem, 20);
	yy = tmp + 10;  
	/* Copy mem[] to yy[] */
	for (i = 0; i < lg; i++)
	{
		tmpA = a; 
		s =  (x[i]* (*(tmpA++)));
		s -=((*(tmpA++))* yy[-1]);
		s -=((*(tmpA++))* yy[-2]);
		s -=((*(tmpA++))* yy[-3]);
		s -=((*(tmpA++))* yy[-4]);
		s -=((*(tmpA++))* yy[-5]);
		s -=((*(tmpA++))* yy[-6]);
		s -=((*(tmpA++))* yy[-7]);
		s -=((*(tmpA++))* yy[-8]);
		s -=((*(tmpA++))* yy[-9]);
		s -=((*tmpA)* yy[-10]); 

		s = L_shl2(s, 4);
		*(y+i) = vo_round(s);		
		*yy++ = *(y+i);            
	}
	/* Update of memory if update==1 */
	if(update !=0)
	{	       
		memcpy(&mem[0], &y[lg-10], 20);
	}
}
#else
{
    Word16 i, j;
    Word32 s;
    Word16 tmp[80];   /* This is usually done by memory allocation (lg+M) */
    Word16 *yy;
    /* Copy mem[] to yy[] */
    yy = tmp;                       //    move16 (); 

    for (i = 0; i < M; i++)
    {
        *yy++ = mem[i];              //   move16 (); 
    } 

    /* Do the filtering. */
    for (i = 0; i < lg; i++)
    {
        s = L_mult (x[i], a[0]);
        for (j = 1; j <= M; j++)
        {
            s = L_msu (s, a[j], yy[-j]);
        }
        s = L_shl (s, 3);
        *yy++ = vo_round (s);           
    }
    for (i = 0; i < lg; i++)
    {
        y[i] = tmp[i + M];          
    }
    /* Update of memory if update==1 */

    if (update != 0)
    {
        for (i = 0; i < M; i++)
        {
            mem[i] = y[lg - M + i];   
        }
    }
    return;
}
#endif





















