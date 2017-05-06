/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : set_sign.c
*      Purpose          : Builds sign vector according to "dn[]" and "cn[]".
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "set_sign.h"
const char set_sign_id[] = "@(#)$Id $" set_sign_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "inv_sqrt.h"
#include "cnst.h" 

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*************************************************************************
*
*  FUNCTION  set_sign()
*
*  PURPOSE: Builds sign[] vector according to "dn[]" and "cn[]".
*           Also finds the position of maximum of correlation in each track
*           and the starting position for each pulse.
*
*************************************************************************/
void voAMRNBEnc_set_sign(Word16 dn[],   /* i/o : correlation between target and h[]    */
			  Word16 sign[], /* o   : sign of dn[]                          */
			  Word16 dn2[],  /* o   : maximum of correlation in each track. */
			  Word16 n       /* i   : # of maximum correlations in dn2[]    */
			  )
{
	Word16 i, j, k;
	Word16 val, min;
	Word16 pos = 0; /* initialization only needed to keep gcc silent */
	/* set sign according to dn[] */
	for (i = 0; i < L_CODE; i++) {
		val = dn[i];                        
		if (val >= 0) {
			sign[i] = 1;                
		} else {
			sign[i] = -1;                 
			val = negate(val);
		}
		dn[i] = val;   
		dn2[i] = val;  
	}
	/* keep 8-n maximum positions/8 of each track and store it in dn2[] */
	for (i = 0; i < NB_TRACK; i++)
	{
		for (k = 0; k < (8-n); k++)
		{
			min = 0x7fff;                           
			for (j = i; j < L_CODE; j += STEP)
			{
				if (dn2[j] >= 0)
				{
					val = dn2[j] - min;//sub(dn2[j], min);
					if (val < 0)
					{
						min = dn2[j];                   
						pos = j;                       
					}
				}
			}
			dn2[pos] = -1;                       
		}
	}
	return;
}
/*************************************************************************
*
*  FUNCTION  set_sign12k2()
*
*  PURPOSE: Builds sign[] vector according to "dn[]" and "cn[]", and modifies
*           dn[] to include the sign information (dn[i]=sign[i]*dn[i]).
*           Also finds the position of maximum of correlation in each track
*           and the starting position for each pulse.
*
*************************************************************************/

void set_sign12k2 (
				   Word16 dn[],
				   Word16 cn[],      /* i   : residual after long term prediction        */ 
				   Word16 sign[],
				   Word16 pos_max[],
				   Word16 ipos[]
				   )
{
	Word32  i, j;
	Word32  val, cor, k_cn, k_dn, max, max_of_all;
	Word32  pos = 0; /* initialization only needed to keep gcc silent */
	Word16  en[L_CODE];                  /* correlation vector */
	Word32  s, s1, temp;
	Word32  var3;

	/* calculate energy for normalization of cn[] and dn[] */
	s = 256; 
	s1 = 256; 
	temp = 0;
#ifdef ARMv7_OPT
	s  = sum_armv7_xx(s, cn, cn, L_CODE);
    s1 = sum_armv7_xx(s1, dn, dn, L_CODE);
#else
	for (i = 0; i < L_CODE; i++)
	{
		temp = (cn[i] * cn[i]) << 1;
		s  = L_add(s, temp);
		temp = (dn[i] * dn[i]) << 1;
		s1 = L_add(s1, temp);
	}
#endif
	s = Inv_sqrt (s); 
	k_cn = extract_h (L_shl2(s, 5));
	s1 = Inv_sqrt (s1); 
	k_dn = extract_h (L_shl2(s1, 5)); 
 
#ifdef C_OPT  //be bitmatch
for (i = 0; i < L_CODE; i++)
{
	val  = dn[i];							
	var3 = k_cn * cn[i];
	var3 = var3 + k_dn * val;
	var3 = L_shl2(var3, 11);
	cor = (var3 + 0x8000)>>16;
	if (cor >= 0)
	{
		sign[i] = 32767;				 
	}
	else
	{
		sign[i] = -32767;		  
		cor = -cor;
		val = -val;
	}
	/* modify dn[] according to the fixed sign */		 
	dn[i] = val; 
	en[i] = cor; 
}
#else
	for (i = 0; i < L_CODE; i++)
	{
		val = dn[i];                           
		cor = (L_shl2(L_add((k_cn * cn[i])<<1 ,(k_dn * val)<<1), 10) + 0x8000)>>16;
		if (cor >= 0)
		{
			sign[i] = 32767;                     /* sign = +1 */
		}
		else
		{
			sign[i] = -32767;                    /* sign = -1 */
			cor = negate (cor);
			val = negate (val);
		}
		/* modify dn[] according to the fixed sign */        
		dn[i] = val;                             
		en[i] = cor;                             
	}
#endif
	max_of_all = -1;   
	for (i = 0; i < NB_TRACK; i++)
	{
		max = -1;                                
		for (j = i; j < L_CODE; j += STEP)
		{
			cor = en[j];                        
			if (cor > max)
			{
				max = cor;                        
				pos = j;                        
			}
		}
		/* store maximum correlation position */
		pos_max[i] = pos;                       
		if (max > max_of_all)
		{
			max_of_all = max; /* starting position for i0 */            
			ipos[0] = i;                       
		}
	}

	/*----------------------------------------------------------------*
	*     Set starting position of each pulse.                       *
	*----------------------------------------------------------------*/
	pos = ipos[0];                               
	ipos[NB_TRACK] = pos;                        
	for (i = 1; i < NB_TRACK; i++)
	{
		pos = (pos + 1);
		if ((pos >= NB_TRACK))
		{
			pos = 0;                               
		}
		ipos[i] = pos;                           
		ipos[(i + NB_TRACK)] = pos;             
	}
}

void set_sign12k2_MR102 (
				   Word16 dn[],      /* i/o : correlation between target and h[]         */
				   Word16 cn[],      /* i   : residual after long term prediction        */
				   Word16 sign[],    /* o   : sign of d[n]                               */
				   Word16 pos_max[], /* o   : position of maximum correlation            */
				   Word16 nb_track,  /* i   : number of tracks tracks                    */        
				   Word16 ipos[],    /* o   : starting position for each pulse           */
				   Word16 step       /* i   : the step size in the tracks                */        
				   )
{
	Word32  i, j;
	Word32  val, cor, k_cn, k_dn, max, max_of_all;
	Word32  pos = 0; /* initialization only needed to keep gcc silent */
	Word16 en[L_CODE];                  /* correlation vector */
	Word32  s, s1, temp;

	/* calculate energy for normalization of cn[] and dn[] */
	s = 256; 
	s1 = 256;  
	for (i = 0; i < L_CODE; i++)
	{
		temp = (cn[i] * cn[i]) << 1;
		s  = L_add(s, temp);
		temp = (dn[i] * dn[i]) << 1;
		s1 = L_add(s1, temp);
	}
	s = Inv_sqrt (s); 
	k_cn = extract_h (L_shl2(s, 5));
	s1 = Inv_sqrt (s1); 
	k_dn = extract_h (L_shl2(s1, 5)); 
                               
	for (i = 0; i < L_CODE; i++)
	{
		val = dn[i];                           
		cor = (L_shl2(L_add((k_cn * cn[i])<<1 ,(k_dn * val)<<1), 10) + 0x8000)>>16;
		if (cor >= 0)
		{
			sign[i] = 32767;                     /* sign = +1 */
		}
		else
		{
			sign[i] = -32767;                    /* sign = -1 */
			cor = negate (cor);
			val = negate (val);
		}
		/* modify dn[] according to the fixed sign */        
		dn[i] = val;                             
		en[i] = cor;                             
	}
	max_of_all = -1;   
	for (i = 0; i < nb_track; i++)
	{
		max = -1;                                
		for (j = i; j < L_CODE; j += step)
		{
			cor = en[j];                        
			if (cor > max)
			{
				max = cor;                        
				pos = j;                        
			}
		}
		/* store maximum correlation position */
		pos_max[i] = pos;                       
		//val = sub (max, max_of_all);
		if (max > max_of_all)
		{
			max_of_all = max;                   
			/* starting position for i0 */            
			ipos[0] = i;                       
		}
	}

	/*----------------------------------------------------------------*
	*     Set starting position of each pulse.                       *
	*----------------------------------------------------------------*/
	pos = ipos[0];                               
	ipos[nb_track] = pos;                        

	for (i = 1; i < nb_track; i++)
	{
		pos = (pos + 1);
		if ((pos >= nb_track))
		{
			pos = 0;                               
		}
		ipos[i] = pos;                           
		ipos[(i + nb_track)] = pos;             
	}
}

