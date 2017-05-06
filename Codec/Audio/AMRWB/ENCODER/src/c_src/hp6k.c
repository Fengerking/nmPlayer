/***********************************************************************
*                                                                      *
*        VisualOn, Inc. Confidential and Proprietary, 2003-2010        *
*                                                                      *
************************************************************************/
/***********************************************************************
*       File: hp6k.c                                                    *
*                                                                       *
*	    Description:15th order band pass 6kHz to 7kHz FIR filter        *
*       frequency: 4kHz   5kHz  5.5kHz  6kHz  6.5kHz  7kHz 7.5kHz 8kHz  *
*	    dB loss:  -60dB  -45dB  -13dB   -3dB   0dB    -3dB -13dB  -45dB *
*	                                                                    *                                                                 
************************************************************************/

#include "typedef.h"
#include "basic_op.h"
#include "acelp.h"
#include "cnst.h"

#define L_FIR 31

/* filter coefficients (gain=4.0) */

Word16 voAMRWBEnc_fir_6k_7k[L_FIR] =
{
	-32, 47, 32, -27, -369,
	1122, -1421, 0, 3798, -8880,
	12349, -10984, 3548, 7766, -18001,
	22118, -18001, 7766, 3548, -10984,
	12349, -8880, 3798, 0, -1421,
	1122, -369, -27, 32, 47,
	-32
};


void Init_Filt_6k_7k(Word16 mem[])         /* mem[30] */
{
	Set_zero(mem, L_FIR - 1);
	return;
}

void Filt_6k_7k(
				Word16 signal[],                      /* input:  signal                  */
				Word16 lg,                            /* input:  length of input         */
				Word16 mem[]                          /* in/out: memory (size=30)        */
)
{
	Word16 x[L_SUBFR16k + (L_FIR - 1)];
	Word32 i, L_tmp;
#if 1 //C_OPT
	voAMRWBEnc_Copy(mem, x, L_FIR - 1);
	for (i = lg - 1; i >= 0; i--)
	{
		x[i + L_FIR - 1] = signal[i] >> 2;                         /* gain of filter = 4 */
	}	
	for (i = 0; i < lg; i++)
	{
		L_tmp =  (x[i] + x[i+ 30]) * voAMRWBEnc_fir_6k_7k[0];
		L_tmp += (x[i+1] + x[i + 29]) * voAMRWBEnc_fir_6k_7k[1];
		L_tmp += (x[i+2] + x[i + 28]) * voAMRWBEnc_fir_6k_7k[2];
		L_tmp += (x[i+3] + x[i + 27]) * voAMRWBEnc_fir_6k_7k[3];
		L_tmp += (x[i+4] + x[i + 26]) * voAMRWBEnc_fir_6k_7k[4];
		L_tmp += (x[i+5] + x[i + 25]) * voAMRWBEnc_fir_6k_7k[5];
		L_tmp += (x[i+6] + x[i + 24]) * voAMRWBEnc_fir_6k_7k[6];
		L_tmp += (x[i+7] + x[i + 23]) * voAMRWBEnc_fir_6k_7k[7];
		L_tmp += (x[i+8] + x[i + 22]) * voAMRWBEnc_fir_6k_7k[8];
		L_tmp += (x[i+9] + x[i + 21]) * voAMRWBEnc_fir_6k_7k[9];
		L_tmp += (x[i+10] + x[i + 20]) * voAMRWBEnc_fir_6k_7k[10];
		L_tmp += (x[i+11] + x[i + 19]) * voAMRWBEnc_fir_6k_7k[11];
		L_tmp += (x[i+12] + x[i + 18]) * voAMRWBEnc_fir_6k_7k[12];
		L_tmp += (x[i+13] + x[i + 17]) * voAMRWBEnc_fir_6k_7k[13];
		L_tmp += (x[i+14] + x[i + 16]) * voAMRWBEnc_fir_6k_7k[14];
		L_tmp += (x[i+15]) * voAMRWBEnc_fir_6k_7k[15];
		signal[i] = (L_tmp + 0x4000) >> 15;
	}
	//memcpy(mem, x + lg, (L_FIR - 1)<<1);
	voAMRWBEnc_Copy(x + lg, mem, L_FIR - 1);
#else
	voAMRWBEnc_Copy(mem, x, L_FIR - 1);
	for (i = 0; i < lg; i++)
	{
		x[i + L_FIR - 1] = (signal[i] >> 2);     /* gain of filter = 4 */
	}
	for (i = 0; i < lg; i++)
	{
		L_tmp = 0;                        
		for (j = 0; j < L_FIR; j++)
			L_tmp = L_mac(L_tmp, x[i + j], voAMRWBEnc_fir_6k_7k[j]);
		signal[i] = voround(L_tmp);         
	}
	voAMRWBEnc_Copy(x + lg, mem, L_FIR - 1);
	return;
#endif
}
