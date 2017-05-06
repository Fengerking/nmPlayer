/**********************************************************************/
/* QCELP Variable Rate Speech Codec - Simulation of TIA IS96-A, service */
/*     option one for TIA IS95, North American Wideband CDMA Digital  */
/*     Cellular Telephony.                                            */
/*                                                                    */
/* (C) Copyright 1993, QUALCOMM Incorporated                          */
/* QUALCOMM Incorporated                                              */
/* 10555 Sorrento Valley Road                                         */
/* San Diego, CA 92121                                                */
/*                                                                    */
/* Note:  Reproduction and use of this software for the design and    */
/*     development of North American Wideband CDMA Digital            */
/*     Cellular Telephony Standards is authorized by                  */
/*     QUALCOMM Incorporated.  QUALCOMM Incorporated does not         */
/*     authorize the use of this software for any other purpose.      */
/*                                                                    */
/*     The availability of this software does not provide any license */
/*     by implication, estoppel, or otherwise under any patent rights */
/*     of QUALCOMM Incorporated or others covering any use of the     */
/*     contents herein.                                               */
/*                                                                    */
/*     Any copies of this software or derivative works must include   */
/*     this and all other proprietary notices.                        */
/**********************************************************************/
/* init.c - initialization of all coder parameters. defaults here. */

/*****************************************************************************
* File:           qcelp13.cpp
*
* Version:        DSPSE Release    2.0
*
* Description:    initialization of all coder parameters
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

#include<stdlib.h>
#include<math.h>
#include"qcelp.h"
#include"qcelp13.h"
#include"q13_tabl.h"
//extern struct DEMO_CONTROL control;
//extern Int16 frame_num;

Int16 norm_s(Int16 var1)
{
    Int16 var_out;
	
    if (var1 == (Int16) 0)
    {
		var_out = (Int16) 0;
    }
    else
    {
		if (var1 == (Int16) 0xffff)
		{
			var_out = (Int16) 15;
		}
		else
		{
			if (var1 < (Int16) 0)
			{
				var1 = (Int16) ~var1;
			}
			
			for (var_out = (Int16) 0; var1 < (Int16) 0x4000; var_out++)
			{
				var1 <<= (Int16) 1;
			}
		}
    }
	
    return (var_out);
}

Int16 norm_l(Int32 L_var1)
{
    Int16 var_out;
	
    if (L_var1 == 0L)
    {
		var_out = (Int16) 0;
    }
    else
    {
		if (L_var1 == (Int32) 0xffffffffL)
		{
			var_out = (Int16) 31;
		}
		else
		{
			if (L_var1 < 0L)
			{
				L_var1 = ~L_var1;
			}
			
			for (var_out = (Int16) 0; L_var1 < (Int32) 0x40000000L; var_out++)
			{
				L_var1 <<= (Int16) 1L;
			}
		}
    }
	
    return (var_out);
}



