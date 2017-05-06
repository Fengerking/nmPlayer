
#include "typedefs.h"
#include "basic_op.h"

#define			SIZE			128
#define			SIZE_BY_TWO		64
#define			NUM_STAGE		6
#define			TRUE			1
#define			FALSE			0

static Shortword csTable[] = { 
	0x7fff,  
	0x0,     0x7fd9,  0xf9b8,  0x7f62,  0xf374,  0x7e9d,  0xed38,  0x7d8a,  
	0xe707,  0x7c2a,  0xe0e6,  0x7a7d,  0xdad8,  0x7885,  0xd4e1,  0x7642,  
	0xcf04,  0x73b6,  0xc946,  0x70e3,  0xc3a9,  0x6dca,  0xbe32,  0x6a6e,  
	0xb8e3,  0x66d0,  0xb3c0,  0x62f2,  0xaecc,  0x5ed7,  0xaa0a,  0x5a82,  
	0xa57e,  0x55f6,  0xa129,  0x5134,  0x9d0e,  0x4c40,  0x9930,  0x471d,  
	0x9592,  0x41ce,  0x9236,  0x3c57,  0x8f1d,  0x36ba,  0x8c4a,  0x30fc,  
	0x89be,  0x2b1f,  0x877b,  0x2528,  0x8583,  0x1f1a,  0x83d6,  0x18f9,  
	0x8276,  0x12c8,  0x8163,  0xc8c,   0x809e,  0x648,   0x8027,  0x0,  
	0x8000,  0xf9b8,  0x8027,  0xf374,  0x809e,  0xed38,  0x8163,  0xe707,  
	0x8276,  0xe0e6,  0x83d6,  0xdad8,  0x8583,  0xd4e1,  0x877b,  0xcf04,  
	0x89be,  0xc946,  0x8c4a,  0xc3a9,  0x8f1d,  0xbe32,  0x9236,  0xb8e3,  
	0x9592,  0xb3c0,  0x9930,  0xaecc,  0x9d0e,  0xaa0a,  0xa129,  0xa57e,  
	0xa57e,  0xa129,  0xaa0a,  0x9d0e,  0xaecc,  0x9930,  0xb3c0,  0x9592,  
	0xb8e3,  0x9236,  0xbe32,  0x8f1d,  0xc3a9,  0x8c4a,  0xc946,  0x89be,  
	0xcf04,  0x877b,  0xd4e1,  0x8583,  0xdad8,  0x83d6,  0xe0e6,  0x8276,  
	0xe707,  0x8163,  0xed38,  0x809e,  0xf374,  0x8027,  0xf9b8
};

static Shortword ii_table[] =
//{SIZE / 2, SIZE / 4, SIZE / 8, SIZE / 16, SIZE / 32, SIZE / 64};
{(SIZE >> 1), (SIZE >> 2), (SIZE >> 3), (SIZE >> 4), (SIZE >> 5), (SIZE >> 6)};

/* FFT/IFFT function for complex sequences */
void c_fft(Shortword *pFarray, Shortword isign)
{
	Longword nReal, nImag;	
	int  nIndx, nStep, k, ii, jj, kk, jTmp, kTmp;
	Shortword tmp, tmp1, tmp2, nFtmp;
	Shortword chen_temp1, chen_temp2;

/* Rearrange the input array in bit reversed order */
	for (nIndx = 0, nStep = 0; nIndx < SIZE - 2; nIndx = nIndx + 2)
	{
		if (nStep > nIndx)
		{
			nFtmp = *(pFarray + nIndx);
			*(pFarray + nIndx) = *(pFarray + nStep);
			*(pFarray + nStep) = nFtmp;

			nFtmp = *(pFarray + nIndx + 1);
			*(pFarray + nIndx + 1) = *(pFarray + nStep + 1);
			*(pFarray + nStep + 1) = nFtmp;
		}

		k = SIZE_BY_TWO;
		while (nStep >= k)
		{
			nStep -= k;
			k >>= 1;
		}
		nStep += k;
	}

/* The FFT part */
	if (isign == 1)
	{
		for (nIndx = 0; nIndx < NUM_STAGE; nIndx++)
		{					
			jj = 2 << nIndx;
			kk = jj << 1;
			ii = ii_table[nIndx];

			for (nStep = 0; nStep < jj; nStep = nStep + 2)
			{				
				jTmp = nStep * ii;

				for (k = nStep; k < SIZE; k = k + kk)
				{			
					kTmp = k + jj;

					nReal = *(pFarray + kTmp) * csTable[jTmp] - *(pFarray + kTmp + 1) * csTable[jTmp + 1];
					nImag = *(pFarray + kTmp + 1) * csTable[jTmp] + *(pFarray + kTmp) * csTable[jTmp + 1];


					tmp1 = (Shortword)((nReal + 0x4000) >> 15);
					tmp2 = (Shortword)((nImag + 0x4000) >> 15);

					tmp = *(pFarray + k) - tmp1;
					*(pFarray + kTmp) = tmp >> 1;
					
					tmp = *(pFarray + k + 1) - tmp2;
					*(pFarray + kTmp + 1) = tmp >> 1;
					
					tmp = *(pFarray + k) + tmp1;
					*(pFarray + k) = tmp >> 1;
					
					tmp = *(pFarray + k + 1) + tmp2;
					*(pFarray + k + 1) = tmp >> 1;
				}
			}
		}
/* The IFFT part */
	}
	else
	{
		for (nIndx = 0; nIndx < NUM_STAGE; nIndx++)
		{					
			jj = 2 << nIndx;	
			kk = jj << 1;
			ii = ii_table[nIndx];

			for (nStep = 0; nStep < jj; nStep = nStep + 2)
			{				
				jTmp = nStep * ii;	

				for (k = nStep; k < SIZE; k = k + kk)
				{			
					kTmp = k + jj;	

					/* Butterfly computations */
					nReal = (*(pFarray + kTmp)) * csTable[jTmp] +
								(*(pFarray + kTmp + 1)) * csTable[jTmp + 1];
					nImag = (*(pFarray + kTmp + 1)) * csTable[jTmp] -
								(*(pFarray + kTmp)) * csTable[jTmp + 1];
					tmp1 = (nReal < 0x3fff8000L) ? (Shortword)(((nReal << 1) + 0x8000) >> 16) : (Shortword)0x7fff;
					tmp2 = (nImag < 0x3fff8000L) ? (Shortword)(((nImag << 1) + 0x8000) >> 16) : (Shortword)0x7fff;

                    chen_temp1 = *(pFarray + k);
                    chen_temp2 = chen_temp1 - tmp1;
					*(pFarray + kTmp) = chen_temp2;
                    if ((chen_temp1 ^ tmp1) & MIN_16)
                    {
                        if ((chen_temp2 ^ chen_temp1) & MIN_16)
                        {
                            *(pFarray + kTmp) = (chen_temp1 > 0) ? MAX_16 : MIN_16;
                        }
                    }

                    chen_temp1 = *(pFarray + k + 1);
                    chen_temp2 = chen_temp1 - tmp2;
					*(pFarray + kTmp + 1) = chen_temp2;
                    if ((chen_temp1 ^ tmp2) & MIN_16)
                    {
                        if ((chen_temp2 ^ chen_temp1) & MIN_16)
                        {
                            *(pFarray + kTmp + 1) = (chen_temp1 > 0) ? MAX_16 : MIN_16;
                        }
                    }
					
                    chen_temp1 = *(pFarray + k);
                    chen_temp2 = chen_temp1 + tmp1;
					*(pFarray + k) = chen_temp2;
                    if (((chen_temp1 ^ tmp1) & MIN_16) == 0)
                    {
                        if ((chen_temp2 ^ chen_temp1) & MIN_16)
                        {
                            *(pFarray + k) = (chen_temp1 > 0) ? MAX_16 : MIN_16;
                        }
                    }

                    chen_temp1 = *(pFarray + k + 1);
                    chen_temp2 = chen_temp1 + tmp2;
					*(pFarray + k + 1) = chen_temp2;
                    if (((chen_temp1 ^ tmp2) & MIN_16) == 0)
                    {
                        if ((chen_temp2 ^ chen_temp1) & MIN_16)
                        {
                            *(pFarray + k + 1) = (chen_temp1 > 0) ? MAX_16 : MIN_16;
                        }
                    }
				}
			}
		}
	}
}								/* end of c_fft () */

void r_fft(Shortword * pFarray, Shortword isign)
{
	short nFtmp1_real, nFtmp1_imag, nFtmp2_real, nFtmp2_imag;
	int LnFtmp1_real, LnFtmp1_imag, LnFtmp2_real, LnFtmp2_imag;
	int i, j;
	int Ltmp1, Ltmp2;

/* The FFT part */
	if (isign == 1)
	{
		/* Perform the complex FFT */
		c_fft(pFarray, isign);

		/* First, handle the DC and foldover frequencies */
		nFtmp1_real = *pFarray;
		nFtmp2_real = *(pFarray + 1);
		*pFarray = (nFtmp1_real + nFtmp2_real);
		*(pFarray + 1) = (nFtmp1_real - nFtmp2_real);

		/* Now, handle the remaining positive frequencies */
		for (i = 2, j = SIZE - i; i <= SIZE_BY_TWO; i = i + 2, j = SIZE - i)
		{
			nFtmp1_real = (*(pFarray + i) + *(pFarray + j));
			nFtmp1_imag = (*(pFarray + i + 1) - *(pFarray + j + 1));
			nFtmp2_real = (*(pFarray + i + 1) + *(pFarray + j + 1));
			nFtmp2_imag = (*(pFarray + j) - *(pFarray + i));

			LnFtmp1_real = L_deposit_h(nFtmp1_real);
			LnFtmp1_imag = L_deposit_h(nFtmp1_imag);
			LnFtmp2_real = L_deposit_h(nFtmp2_real);
			LnFtmp2_imag = L_deposit_h(nFtmp2_imag);

			Ltmp1 = L_sub((nFtmp2_real * csTable[i])<<1, (nFtmp2_imag * csTable[i + 1])<<1);
			*(pFarray + i) = round32(((L_add(LnFtmp1_real, Ltmp1)) >> 1));

			Ltmp1 = L_add((nFtmp2_imag * csTable[i])<<1, (nFtmp2_real * csTable[i + 1])<<1);
			*(pFarray + i + 1) = round32(((L_add(LnFtmp1_imag, Ltmp1)) >> 1));

			Ltmp1 = L_add((nFtmp2_real * csTable[j])<<1, (nFtmp2_imag * csTable[j + 1])<<1);
			*(pFarray + j) = round32(((L_add(LnFtmp1_real, Ltmp1)) >> 1));

			Ltmp1 = L_add(L_negate((nFtmp2_imag * csTable[j])<<1), (nFtmp2_real * csTable[j + 1])<<1);
			Ltmp2 = L_add(L_negate(LnFtmp1_imag), Ltmp1);
			*(pFarray + j + 1) = round32((Ltmp2 >> 1));
		}

	}
	else
	{
		/* First, handle the DC and foldover frequencies */
		nFtmp1_real = *pFarray;
		nFtmp2_real = *(pFarray + 1);
		*pFarray = ((nFtmp1_real + nFtmp2_real) >> 1);
		*(pFarray + 1) = ((nFtmp1_real - nFtmp2_real) >> 1);

		/* Now, handle the remaining positive frequencies */
		for (i = 2, j = SIZE - i; i <= SIZE_BY_TWO; i = i + 2, j = SIZE - i)
		{
			nFtmp1_real = (*(pFarray + i) + *(pFarray + j));
			nFtmp1_imag = (*(pFarray + i + 1) - *(pFarray + j + 1));
			nFtmp2_real = negate((*(pFarray + j + 1) + *(pFarray + i + 1)));
			nFtmp2_imag = negate((*(pFarray + j) - *(pFarray + i)));

			LnFtmp1_real = L_deposit_h(nFtmp1_real);
			LnFtmp1_imag = L_deposit_h(nFtmp1_imag);
			LnFtmp2_real = L_deposit_h(nFtmp2_real);
			LnFtmp2_imag = L_deposit_h(nFtmp2_imag);

			Ltmp1 = L_add((nFtmp2_real * csTable[i])<<1, (nFtmp2_imag * csTable[i + 1])<<1);
			*(pFarray + i) = round32(((L_add(LnFtmp1_real, Ltmp1)) >> 1));

			Ltmp1 = L_sub((nFtmp2_imag * csTable[i])<<1, (nFtmp2_real * csTable[i + 1])<<1);
			*(pFarray + i + 1) = round32(((L_add(LnFtmp1_imag, Ltmp1)) >> 1));

			Ltmp1 = L_sub((nFtmp2_real * csTable[j])<<1, (nFtmp2_imag * csTable[j + 1])<<1);
			*(pFarray + j) = round32(((L_add(LnFtmp1_real, Ltmp1)) >> 1));

			Ltmp1 = L_negate(L_add((nFtmp2_imag * csTable[j])<<1, (nFtmp2_real * csTable[j + 1])<<1));
			Ltmp2 = L_add(L_negate(LnFtmp1_imag), Ltmp1);
			*(pFarray + j + 1) = round32((Ltmp2 >> 1));
		}

		/* Perform the complex IFFT */
		c_fft(pFarray, isign);

	}

}								/* end r_fft () */



