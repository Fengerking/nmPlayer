/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : q_gain_c.c
*      Purpose          : Scalar quantization of the innovative
*                       : codebook gain.
*
********************************************************************************
*/


/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "q_gain_c.h"
const char q_gain_c_id[] = "@(#)$Id $" q_gain_c_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "mode.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "log2.h"
#include "pow2.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "gains.tab"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*--------------------------------------------------------------------------*
* Function q_gain_code()                                                   *
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                  *
* Scalar quantization of the innovative codebook gain.                     *
*                                                                          *
*--------------------------------------------------------------------------*/
Word16 voAMRNBEnc_q_gain_code (        /* o  : quantization index,            Q0  */
					enum Mode mode,         /* i  : AMR mode                           */
					Word16 exp_gcode0,      /* i  : predicted CB gain (exponent),  Q0  */
					Word16 frac_gcode0,     /* i  : predicted CB gain (fraction),  Q15 */
					Word16 *gain,           /* i/o: quantized fixed codebook gain, Q1  */
					Word16 *qua_ener_MR122, /* o  : quantized energy error,        Q10 */
					/*      (for MR122 MA predictor update)    */
					Word16 *qua_ener        /* o  : quantized energy error,        Q10 */
					/*      (for other MA predictor update)    */
					)
{
	const Word16 *p;
	Word16 gcode0, err, err_min;
	Word16 g_q0;
	nativeInt i,index;
	g_q0 = 0;    
	if ((mode == MR122))
	{
		g_q0 = (*gain >> 1); /* Q1 -> Q0 */
		gcode0 = extract_l (voAMRNBEnc_Pow2(exp_gcode0, frac_gcode0));  /* predicted gain */
		gcode0 = (gcode0 << 4);
		p = &qua_gain_code[0]; 
		err_min = ((g_q0- (gcode0*( *p)>>15)));
		p++;
		if(err_min<0)
			err_min = - err_min;
		p += 2;                                  /* skip quantized energy errors */
		index = 0;              
		for (i = 1; i < NB_QUA_CODE; i++)
		{	      
			err = ((g_q0- (gcode0 * (*p)>>15)));
			p++;
			if(err<0)
				err = - err;
			p += 2;                              /* skip quantized energy error */
			if (err < err_min)
			{
				err_min = err;                  
				index = i;                      
			}
		}
		p = &qua_gain_code[(index<<1)+index]; 
		*gain = ((gcode0 * (*p)>>15)<< 1); 
		p++;
	}
	else
	{
		gcode0 = extract_l (voAMRNBEnc_Pow2(exp_gcode0, frac_gcode0));  /* predicted gain */
		gcode0 = shl(gcode0, 5);
		/*-------------------------------------------------------------------*
		*                   Search for best quantizer                        *
		*-------------------------------------------------------------------*/
		p = &qua_gain_code[0]; 
		err_min = abs_s (sub3 (*gain, mult3 (gcode0, *p++)));
		p += 2;                                  /* skip quantized energy errors */
		index = 0;              
		for (i = 1; i < NB_QUA_CODE; i++)
		{
			err = abs_s (sub3 (*gain, mult3 (gcode0, *p++)));
			p += 2;                              /* skip quantized energy error */
			if ((err < err_min))
			{
				err_min = err;                  
				index = i;                      
			}
		}
		p = &qua_gain_code[(index<<1)+index]; 
		*gain = mult (gcode0, *p++);
	}
	/*-------------------------------------------------------------------*
	*  predicted codebook gain                                          *
	*  ~~~~~~~~~~~~~~~~~~~~~~~                                          *
	*  gc0     = Pow2(int(d)+frac(d))                                   *
	*          = 2^exp + 2^frac                                         *
	*                                                                   *
	*-------------------------------------------------------------------*/
	/* quantized error energies (for MA predictor update) */
	*qua_ener_MR122 = *p++;                 
	*qua_ener = *p;                        
	return index;
}
