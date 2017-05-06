/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : levinson.c
*      Purpose          : Levinson-Durbin algorithm in double precision.
*                       : To compute the LP filter parameters from the
*                       : speech autocorrelations.
*
*****************************************************************************
*/


/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "levinson.h"
const char levinson_id[] = "@(#)$Id $" levinson_h;

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/
/*---------------------------------------------------------------*
*    Constants (defined in "cnst.h")                            *
*---------------------------------------------------------------*
* M           : LPC order
*---------------------------------------------------------------*/

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  Function:   voAMRNBEnc_Levinson_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int voAMRNBEnc_Levinson_init (LevinsonState **state, VO_MEM_OPERATOR *pMemOP)
{
	LevinsonState* s;
	if (state == (LevinsonState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (LevinsonState *) voAMRNBEnc_mem_malloc(pMemOP, sizeof(LevinsonState), 32)) == NULL){
		return -1;
	}
	voAMRNBEnc_Levinson_reset(s);
	*state = s;

	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_Levinson_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int voAMRNBEnc_Levinson_reset (LevinsonState *state)
{
	Word16 i;
	if (state == (LevinsonState *) NULL){
		return -1;
	}
	state->old_A[0] = 4096;
	for(i = 1; i < M + 1; i++)
		state->old_A[i] = 0;
	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_Levinson_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void voAMRNBEnc_Levinson_exit (LevinsonState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	/* deallocate memory */
	voAMRNBEnc_mem_free(pMemOP, *state);
	*state = NULL;
	return;
}

/*************************************************************************
*
*   FUNCTION:  Levinson()
*
*   PURPOSE:  Levinson-Durbin algorithm in double precision. To compute the
*             LP filter parameters from the speech autocorrelations.
*
*   DESCRIPTION:
*       R[i]    autocorrelations.
*       A[i]    filter coefficients.
*       K       reflection coefficients.
*       Alpha   prediction gain.
*
*       Initialisation:
*               A[0] = 1
*               K    = -R[1]/R[0]
*               A[1] = K
*               Alpha = R[0] * (1-K**2]
*
*       Do for  i = 2 to M
*
*            S =  SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i]
*
*            K = -S / Alpha
*
*            An[j] = A[j] + K*A[i-j]   for j=1 to i-1
*                                      where   An[i] = new A[i]
*            An[i]=K
*
*            Alpha=Alpha * (1-K**2)
*
*       END
*
*************************************************************************/
int voAMRNBEnc_Levinson (
			  LevinsonState *st,
			  Word16 Rh[],       /* i : Rh[m+1] Vector of autocorrelations (msb) */
			  Word16 Rl[],       /* i : Rl[m+1] Vector of autocorrelations (lsb) */
			  Word16 A[],        /* o : A[m]    LPC coefficients  (m = 10)       */
			  Word16 rc[]        /* o : rc[4]   First 4 reflection coefficients  */
)
{
	Word32   hi, lo;
	Word32   Kh, Kl;                /* reflexion coefficient; hi and lo      */
	Word32   alp_h, alp_l, alp_exp; /* Prediction gain; hi lo and exponent   */
	Word32   i,j;
	Word32   t0, t1, t2,t3;            /* temporary variable                    */
	Word16   Ah[M + 1];
	Word16   Al[M + 1];
	Word16   Anh[M + 1];
	Word16   Anl[M + 1];

#ifdef C_OPT   //bit match
	/* K = A[1] = -R[1] / R[0] */
	t1 =  Rh[1]<<16;
	t1 += (Rl[1]<<1);
	// t1 = L_Comp (Rh[1], Rl[1]);
	t2 = t1>0?t1:-t1;                    /* abs R[1]         */
	t0 = Div_32 (t2, Rh[0], Rl[0]);     /* R[1]/R[0]        */

	if (t1 > 0)
		t0 = - (t0);             /* -R[1]/R[0]       */
	Kh = t0 >> 16;
	Kl = (t0 - (Kh << 16))>>1;
	// L_Extract (t0, &Kh, &Kl);           /* K in DPF         */

	rc[0] = (t0+0x00008000)>>16;//round (t0);                

	t0 = (t0 >> 4);                 /* A[1] in          */
	Ah[1] = t0 >> 16;
	Al[1]  = (t0 - (Ah[1]<<16))>>1;
	//L_Extract (t0, &Ah[1], &Al[1]);     /* A[1] in DPF      */

	/*  Alpha = R[0] * (1-K**2) */

	t0 = (Kh*Kh<<1) + (((Kh*Kl>>15) )<<2);  
	//t0 = Mpy_32 (Kh, Kl, Kh, Kl);       /* K*K             */L_32 = (hi1*hi2<<1) + (( (h i1*lo2>>15) + (lo1*hi2>>15) )<<1)       
	if(t0<0)
		t0 = - (t0);                    /* Some case <0 !! */
	t0 = ((Word32) 0x7fffffffL- t0); /* 1 - K*K        */
	hi = t0>>16;
	lo = (t0-(hi<<16))>>1;
	//L_Extract (t0, &hi, &lo);           /* DPF format      */
	t0 = (Rh[0]*hi<<1) + (( (Rh[0]*lo>>15) + (Rl[0]*hi>>15) )<<1); 
	// t0 = Mpy_32 (Rh[0], Rl[0], hi, lo); /* Alpha in        */

	/* Normalize Alpha */
	alp_exp = norm_l (t0);
	t0 = t0<< alp_exp;//L_shl2 (t0, alp_exp);
	alp_h = t0>>16;
	alp_l = (t0- (alp_h << 16))>>1;
	// L_Extract (t0, &alp_h, &alp_l);     /* DPF format    */

	/*--------------------------------------*
	* ITERATIONS  I=2 to M                 *
	*--------------------------------------*/
	for (i = 2; i <= M; i++)
	{
		/* t0 = SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i] */
		t0 = 0;                         
		for (j = 1; j < i; j++)
		{ 
			t1 = (Rh[j]*Ah[i - j]<<1) + (( (Rh[j]*Al[i - j]>>15) + (Rl[j]*Ah[i - j]>>15) )<<1);
			t0 = t0 + t1;//(t0, Mpy_32 (Rh[j], Rl[j], Ah[i - j], Al[i - j]));
		}
		t0 = (t0<< 4);

		t1 = Rh[i]<<16;
		t1 = t1 + (Rl[i]<<1);
		//t1 = L_Comp (Rh[i], Rl[i]);
		t0 = (t0 + t1);            /* add R[i]        */

		/* K = -t0 / Alpha */

		t1 = t0>=0?t0:-t0;//L_abs (t0);
		t2 = Div_32 (t1, alp_h, alp_l); /* abs(t0)/Alpha              */

		if (t0 > 0)
			t2 = - (t2);         /* K =-t0/Alpha                */
		t2 = (t2<< alp_exp);       /* denormalize; compare to Alpha */
		Kh = t2>>16;
		Kl  = (t2-(Kh<<16))>>1;
		//L_Extract (t2, &Kh, &Kl);       /* K in DPF                      */       

		if (i < 5)
		{
			rc[i - 1] = (t2+0x00008000)>>16;//round (t2);     
		}
		/* Test for unstable filter. If unstable keep old A(z) */

		t3 = Kh>0?Kh:-Kh;

		if (t3 > 32750)
		{
			for (j = 0; j <= M; j++)
			{
				A[j] = st->old_A[j];        
			}
			for (j = 0; j < 4; j++)
			{
				rc[j] = 0;            
			}
			return 0;
		}
		/*------------------------------------------*
		*  Compute new LPC coeff. -> An[i]         *
		*  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
		*  An[i]= K                                *
		*------------------------------------------*/

		for (j = 1; j < i; j++)
		{
			t0 = (Kh*Ah[i - j]<<1) + (( (Kh*Al[i - j]>>15) + (Kl* Ah[i - j]>>15))<<1)  ;//Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j]);
			t3 = Ah[j]<<16;
			t3 = t3 + (Al[j]<<1);
			t0 = t0 + t3;//L_add(t0, L_Comp(Ah[j], Al[j]));
			Anh[j] = t0>>16;
			Anl[j]  = (t0-(Anh[j]<<16))>>1;
			//L_Extract (t0, &Anh[j], &Anl[j]);
		}
		t2 =  (t2>> 4);
		Anh[i] = t2>>16;
		Anl[i]  = (t2-(Anh[j]<<16))>>1;
		//L_Extract (t2, &Anh[i], &Anl[i]);

		/*  Alpha = Alpha * (1-K**2) */
		t0 = (Kh*Kh<<1) + (( (Kh*Kl>>15)  )<<2);//Mpy_32 (Kh, Kl, Kh, Kl);           /* K*K             */
		if(t0<0)
			t0 = -(t0);                        /* Some case <0 !! */
		t0 = ((Word32) 0x7fffffffL- t0);  /* 1 - K*K        */
		hi = t0>>16;
		lo  = (t0-(hi<<16))>>1;
		//L_Extract (t0, &hi, &lo);               /* DPF format      */
		t0 = (alp_h* hi<<1) + (( (alp_h*lo>>15) + (alp_l* hi>>15) )<<1);//Mpy_32 (alp_h, alp_l, hi, lo);
		/* Normalize Alpha */
		j = norm_l(t0);
		t0 =  (t0 << j);

		alp_h = t0>>16;
		alp_l = (t0-(alp_h<<16))>>1;
		//L_Extract (t0, &alp_h, &alp_l);         /* DPF format    */
		alp_exp = alp_exp + j;//add (alp_exp, j);    
		/* A[j] = An[j] */
		for (j = 1; j <= i; j++)
		{
			Ah[j] = Anh[j];                    
			Al[j] = Anl[j];                     
		}
	}

	A[0] = 4096;                                
	for (i = 1; i <= M; i++)
	{
		t0 = Ah[i]<<16;
		t0 = t0 + (Al[i]<<1);
		t0<<=1;
		//t0 = L_Comp (Ah[i], Al[i]);
		st->old_A[i] = A[i] = (t0+0x00008000)>>16;//round (L_shl2 (t0, 1));
	}
#else//C_OPT
	/* K = A[1] = -R[1] / R[0] */
	t1 = L_Comp (Rh[1], Rl[1]);
	t2 = L_abs (t1);                                       /* abs R[1]         */
	t0 = Div_32 (t2, Rh[0], Rl[0]);                         /* R[1]/R[0]        */
	if (t1 > 0)
		t0 = L_negate (t0);                               /* -R[1]/R[0]       */
	L_Extract (t0, (Word16*)&Kh, (Word16*)&Kl);           /* K in DPF */

	rc[0] = vo_round (t0);                
	t0 = (t0>> 4);                                       /* A[1] in          */
	L_Extract (t0, &Ah[1], &Al[1]);                      /* A[1] in DPF      */
	/*  Alpha = R[0] * (1-K**2) */
	t0 = Mpy_32 (Kh, Kl, Kh, Kl);                        /* K*K  */
	t0 = L_abs (t0);                                     /* Some case <0 !! */
	t0 = ((Word32) 0x7fffffffL- t0);                     /* 1 - K*K */
	L_Extract (t0, (Word16*)&hi, (Word16*)&lo);           /* DPF format  */
	t0 = Mpy_32 (Rh[0], Rl[0], hi, lo); /* Alpha in        */

	/* Normalize Alpha */

	alp_exp = norm_l (t0);
	t0 = L_shl2 (t0, alp_exp);
	L_Extract (t0, (Word16*)&alp_h, (Word16*)&alp_l);     /* DPF format    */

	/*--------------------------------------*
	* ITERATIONS  I=2 to M                 *
	*--------------------------------------*/

	for (i = 2; i <= M; i++)
	{
		/* t0 = SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i] */

		t0 = 0;                         
		for (j = 1; j < i; j++)
		{
			t0 = L_add (t0, Mpy_32 (Rh[j], Rl[j], Ah[i - j], Al[i - j]));
		}
		t0 = L_shl2 (t0, 4);

		t1 = L_Comp (Rh[i], Rl[i]);
		t0 = L_add (t0, t1);            /* add R[i]        */

		/* K = -t0 / Alpha */

		t1 = L_abs (t0);
		t2 = Div_32 (t1, alp_h, alp_l); /* abs(t0)/Alpha              */

		if (t0 > 0)
			t2 = L_negate (t2);         /* K =-t0/Alpha                */
		t2 = L_shl2 (t2, alp_exp);       /* denormalize; compare to Alpha */
		L_Extract (t2, (Word16*)&Kh, (Word16*)&Kl);       /* K in DPF                      */


		if ((i- 5) < 0)
		{
			rc[i - 1] = vo_round (t2);     
		}
		/* Test for unstable filter. If unstable keep old A(z) */


		if ((abs_s (Kh)-32750) > 0)
		{
			for (j = 0; j <= M; j++)
			{
				A[j] = st->old_A[j];        
			}

			for (j = 0; j < 4; j++)
			{
				rc[j] = 0;            
			}

			return 0;
		}
		/*------------------------------------------*
		*  Compute new LPC coeff. -> An[i]         *
		*  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
		*  An[i]= K                                *
		*------------------------------------------*/

		for (j = 1; j < i; j++)
		{
			t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j]);
			t0 = L_add(t0, L_Comp(Ah[j], Al[j]));
			L_Extract (t0, &Anh[j], &Anl[j]);
		}
		t2 =  (t2>> 4);
		L_Extract (t2, &Anh[i], &Anl[i]);

		/*  Alpha = Alpha * (1-K**2) */

		t0 = Mpy_32 (Kh, Kl, Kh, Kl);           /* K*K             */
		t0 = L_abs (t0);                        /* Some case <0 !! */
		t0 = ((Word32) 0x7fffffffL- t0);  /* 1 - K*K        */
		L_Extract (t0, (Word16*)&hi, (Word16*)&lo);               /* DPF format      */
		t0 = Mpy_32 (alp_h, alp_l, hi, lo);

		/* Normalize Alpha */

		j = norm_l (t0);
		t0 = L_shl2 (t0, j);
		L_Extract (t0, (Word16*)&alp_h, (Word16*)&alp_l);         /* DPF format    */
		alp_exp = add (alp_exp, j);             /* Add normalization to
												alp_exp */

		/* A[j] = An[j] */

		for (j = 1; j <= i; j++)
		{
			Ah[j] = Anh[j];                    
			Al[j] = Anl[j];                     
		}
	}

	A[0] = 4096;                                
	for (i = 1; i <= M; i++)
	{
		t0 = L_Comp (Ah[i], Al[i]);
		st->old_A[i] = A[i] = vo_round (L_shl2 (t0, 1));
	}
#endif //C_OPT 
	return 0;
}
