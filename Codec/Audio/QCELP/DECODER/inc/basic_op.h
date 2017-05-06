/*___________________________________________________________________________
|                                                                           |
|   Constants and Globals                                                   |
|                                                                           |
| $Id $
|___________________________________________________________________________|
*/
#ifndef __BASIC_OP_H__
#define __BASIC_OP_H__

#include "types.h"
#define MAX_32 (Int32)0x7fffffffL
#define MIN_32 (Int32)0x80000000L
#define MAX_16 (Int16)+32767	/* 0x7fff */
#define MIN_16 (Int16)-32768	/* 0x8000 */


#ifdef LINUX
#define  __voinline  static __inline__
#else
#define  __voinline  static __inline
#endif 


//avoid QCELP Decoder iOS duplicate 
#define		InitFrameBuffer		voQCELPDec_Fun01     
#define         UpdateFrameBuffer       voQCELPDec_Fun02       
#define         FlushFrameBuffer        voQCELPDec_Fun03        
#define		gen_params              voQCELPDec_Fun11
#define		Sqrt                    voQCELPDec_Fun12
#define		agc_prefilter           voQCELPDec_Fun13
#define		agc                     voQCELPDec_Fun14
#define		QC13_DecBlank           voQCELPDec_Fun15
#define		QC13_DecErase           voQCELPDec_Fun16
#define		QC13_DecEighth          voQCELPDec_Fun17
#define		QC13_DecQuarter         voQCELPDec_Fun18
#define		QC13_DecPSF             voQCELPDec_Fun19
#define		QC13_Decode             voQCELPDec_Fun20
#define		make_cw                 voQCELPDec_Fun21
#define		make_qc13_qtr_cw        voQCELPDec_Fun22
#define		make_qc13_full_cw       voQCELPDec_Fun23
#define		fir_filt                voQCELPDec_Fun24
#define		dp_polefilt             voQCELPDec_Fun25
#define		wghtfilt                voQCELPDec_Fun26
#define		postfilt                voQCELPDec_Fun27
#define		pitch_filter_13         voQCELPDec_Fun28
#define		update_acbk_16          voQCELPDec_Fun29
#define		interp_lpcs             voQCELPDec_Fun30
#define		wght_lpcs               voQCELPDec_Fun31
#define		lsp2lpc                 voQCELPDec_Fun32
#define		smooth_lsps             voQCELPDec_Fun33
#define		gen_qlsp                voQCELPDec_Fun34
#define		unquantize_G_8th        voQCELPDec_Fun35
#define		unquantize_G_13         voQCELPDec_Fun36
#define		unvq_lsp                voQCELPDec_Fun37
#define		unquantize_lag          voQCELPDec_Fun38
#define		unquantize_b            voQCELPDec_Fun39
#define		check_lsp_stab          voQCELPDec_Fun40
#define		unquantize_i            voQCELPDec_Fun41
#define		unquantize_lsp          voQCELPDec_Fun42
#define		unpack_cb13             voQCELPDec_Fun43
#define		clear_qc13_packet_params  voQCELPDec_Fun44
#define		putbit13                voQCELPDec_Fun45
#define		G_erasure_check         voQCELPDec_Fun46
#define		unpack_frame13          voQCELPDec_Fun47


/*___________________________________________________________________________
|                                                                           |
|   Prototypes for basic arithmetic operators                               |
|___________________________________________________________________________|
*/
#define L_add_MACRO(a, b) ((a) + (b))

__voinline Int32 L_add(Int32 L_var1, Int32 L_var2);    /* Long add,        2 */
__voinline Int32 L_sub(Int32 L_var1, Int32 L_var2);    /* Long sub,        2 */
__voinline Int32 L_shl(Int32 L_var1, Int16 var2);      /* Long shift left, 2 */
__voinline Int32 L_shl2(Int32 L_var1, Int16 var2);      /* Long shift left, 2 */
__voinline Int32 L_shr(Int32 L_var1, Int16 var2);      /* Long shift right, 2*/
__voinline Int16 saturate(Int32 L_var1);

__voinline Int32 L_add(Int32 L_var1, Int32 L_var2)
{
	Int32 L_var_out;
	L_var_out = L_var1 + L_var2;
	if (((L_var1 ^ L_var2) & MIN_32) == 0)
	{
		if ((L_var_out ^ L_var1) & MIN_32)
		{
			L_var_out = (L_var1 < 0) ? MIN_32 : MAX_32;
		}
	}
	return (L_var_out);
}

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_sub                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   32 bits subtraction of the two 32 bits variables (L_var1-L_var2) with   |
|   overflow control and saturation; the result is set at +2147483647 when  |
|   overflow occurs or at -2147483648 when underflow occurs.                |
|                                                                           |
|   Complexity weight : 2                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1   32 bit long signed integer (Int32) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    L_var2   32 bit long signed integer (Int32) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (Int32) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
__voinline Int32 L_sub (Int32 L_var1, Int32 L_var2)
{
	Int32 L_var_out;
	L_var_out = L_var1 - L_var2;

	if (((L_var1 ^ L_var2) & MIN_32) != 0)
	{
		if ((L_var_out ^ L_var1) & MIN_32)
		{
			L_var_out = (L_var1 < 0L) ? MIN_32 : MAX_32;
		}
	}
	return (L_var_out);
}

__voinline Int32 L_shl (Int32 L_var1, Int16 var2)
{
	Int32 L_var_out = 0L;
	if (var2 <= 0)
	{
		if (var2 < -32)
			var2 = -32;
		L_var_out = L_shr (L_var1, (Int16)-var2);
	}
	else
	{
		for (; var2 > 0; var2--)
		{
			if (L_var1 > (Int32) 0X3fffffffL)
			{
				L_var_out = MAX_32;
				break;
			}
			else
			{
				if (L_var1 < (Int32) 0xc0000000L)
				{
					L_var_out = MIN_32;
					break;
				}
			}
			L_var1 <<= 1;
			L_var_out = L_var1;
		}
	}
	return (L_var_out);
}

__voinline Int32 L_shl2 (Int32 L_var1, Int16 var2)
{
	Int32 L_var_out = 0L;
	for (; var2 > 0; var2--)
	{
		if (L_var1 > (Int32) 0X3fffffffL)
		{
			L_var_out = MAX_32;
			break;
		}
		else
		{
			if (L_var1 < (Int32) 0xc0000000L)
			{
				L_var_out = MIN_32;
				break;
			}
		}
		L_var1 <<= 1;
		L_var_out = L_var1;
	}
	return (L_var_out);
}

/*___________________________________________________________________________
|                                                                           |
|   Function Name : L_shr                                                   |
|                                                                           |
|   Purpose :                                                               |
|                                                                           |
|   Arithmetically shift the 32 bit input L_var1 right var2 positions with  |
|   sign extension. If var2 is negative, arithmetically shift L_var1 left   |
|   by -var2 and zero fill the -var2 LSB of the result. Saturate the result |
|   in case of underflows or overflows.                                     |
|                                                                           |
|   Complexity weight : 2                                                   |
|                                                                           |
|   Inputs :                                                                |
|                                                                           |
|    L_var1   32 bit long signed integer (Int32) whose value falls in the  |
|             range : 0x8000 0000 <= L_var3 <= 0x7fff ffff.                 |
|                                                                           |
|    var2                                                                   |
|             16 bit short signed integer (Int16) whose value falls in the |
|             range : 0xffff 8000 <= var1 <= 0x0000 7fff.                   |
|                                                                           |
|   Outputs :                                                               |
|                                                                           |
|    none                                                                   |
|                                                                           |
|   Return Value :                                                          |
|                                                                           |
|    L_var_out                                                              |
|             32 bit long signed integer (Int32) whose value falls in the  |
|             range : 0x8000 0000 <= L_var_out <= 0x7fff ffff.              |
|___________________________________________________________________________|
*/
__voinline Int32 L_shr (Int32 L_var1, Int16 var2)
{
	Int32 L_var_out;
	if (var2 < 0)
	{
		if (var2 < -32)
			var2 = -32;
		L_var_out = L_shl (L_var1, (Int16)-var2);
	}
	else
	{
		if (var2 >= 31)
		{
			L_var_out = (L_var1 < 0L) ? -1 : 0;
		}
		else
		{
			if (L_var1 < 0)
			{
				L_var_out = ~((~L_var1) >> var2);
			}
			else
			{
				L_var_out = L_var1 >> var2;
			}
		}
	}
	return (L_var_out);
}

__voinline Int16 saturate(Int32 L_var1)
{
	Int16 var_out;

	if (L_var1 > MAX_16)
	{
		var_out = MAX_16;
	}
	else if (L_var1 < MIN_16)
	{
		var_out = MIN_16;
	}
	else
	{
		var_out = (Int16)L_var1;
	}
	return (var_out);
}

#endif //__BASIC_OP_H__
