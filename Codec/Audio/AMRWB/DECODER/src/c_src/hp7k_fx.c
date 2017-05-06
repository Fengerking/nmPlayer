/*-------------------------------------------------------------------*
 *                         HP6K.C									 *
 *-------------------------------------------------------------------*
 * 15th order high pass 7kHz FIR filter.                             *
 *                                                                   *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "cnst_wb_fx.h"
#include "acelp_fx.h"

#define L_FIR 31

//static 
Word16 fir_7k[L_FIR] =
{
    -21, 47, -89, 146, -203,
    229, -177, 0, 335, -839,
    1485, -2211, 2931, -3542, 3953,
    28682, 3953, -3542, 2931, -2211,
    1485, -839, 335, 0, -177,
    229, -203, 146, -89, 47,
    -21
};

void Init_Filt_7k(Word16 mem[])            /* mem[30] */
{
    AMRWBDecSetZero(mem, L_FIR - 1);

    return;
}


void Filt_7k(
     Word16 signal[],                      /* input:  signal                  */
     Word16 lg,                            /* input:  length of input         */
     Word16 mem[]                          /* in/out: memory (size=30)        */
)
{
#if 1 //(FUNC_FILT_7K_OPT)
    Word16 i,x[L_SUBFR16k + (L_FIR - 1)];
    Word32 L_tmp;

    Copy(mem, x, L_FIR - 1);
    //memcpy(x, mem, (L_FIR-1) << 1);

    Copy(signal, &x[30], lg);

	//memcpy(&x[30], signal, lg <<1);
    //for (i = 0; i < lg; i++)
    //{
    //    x[i + L_FIR - 1] = signal[i];
    //}
    for (i = 0; i < lg; i++)
    {
		L_tmp =  (x[i] + x[i+ 30]) * fir_7k[0];
        L_tmp += (x[i+1] + x[i + 29]) * fir_7k[1];
		L_tmp += (x[i+2] + x[i + 28]) * fir_7k[2];
        L_tmp += (x[i+3] + x[i + 27]) * fir_7k[3];
		L_tmp += (x[i+4] + x[i + 26]) * fir_7k[4];
        L_tmp += (x[i+5] + x[i + 25]) * fir_7k[5];
		L_tmp += (x[i+6] + x[i + 24]) * fir_7k[6];
        L_tmp += (x[i+7] + x[i + 23]) * fir_7k[7];
		L_tmp += (x[i+8] + x[i + 22]) * fir_7k[8];
        L_tmp += (x[i+9] + x[i + 21]) * fir_7k[9];
		L_tmp += (x[i+10] + x[i + 20]) * fir_7k[10];
        L_tmp += (x[i+11] + x[i + 19]) * fir_7k[11];
		L_tmp += (x[i+12] + x[i + 18]) * fir_7k[12];
        L_tmp += (x[i+13] + x[i + 17]) * fir_7k[13];
		L_tmp += (x[i+14] + x[i + 16]) * fir_7k[14];
		L_tmp += (x[i+15]) * fir_7k[15];
        signal[i] = (L_tmp + 0x4000) >> 15;
    }
    Copy(x + lg, mem, L_FIR - 1);
    //memcpy(mem, x + lg, (L_FIR - 1)<<1);
    return;
#else
    Word16 i, j, x[L_SUBFR16k + (L_FIR - 1)];
    Word32 L_tmp;

    Copy(mem, x, L_FIR - 1);

    for (i = 0; i < lg; i++)
    {
        x[i + L_FIR - 1] = signal[i];
    }

    for (i = 0; i < lg; i++)
    {
        L_tmp = 0;  
        for (j = 0; j < L_FIR; j++)
        L_tmp = L_mac(L_tmp, x[i + j], fir_7k[j]);
        signal[i] = vo_round(L_tmp); 
    }

    Copy(x + lg, mem, L_FIR - 1);

    return;
#endif
}
