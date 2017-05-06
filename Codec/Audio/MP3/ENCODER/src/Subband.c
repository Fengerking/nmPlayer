#include "Frame.h"

void window_subband1_2ch(short *wk, int sb0[SBLIMIT], int sb1[SBLIMIT])
{
	int   k, i, u, v;
	const short *wp;
	short *x1, *x2;
	int ch, s, t, *a;
	
	for(ch=0; ch<2; ch++)
	{
		a = ch ? sb1 : sb0;
		for(k=0; k<18; k++, wk+=64, a+=SBLIMIT)
		{
			wp = enwindow;
			x1 = wk;
			x2 = x1 - 124;
			
			for(i=-15; i<0; i++)
			{
				s  = (int)x2[-224*2] * wp[ 0];  t  = (int)x1[ 224*2] * wp[ 0];
				s += (int)x2[-160*2] * wp[ 1];  t += (int)x1[ 160*2] * wp[ 1];
				s += (int)x2[- 96*2] * wp[ 2];  t += (int)x1[  96*2] * wp[ 2];
				s += (int)x2[- 32*2] * wp[ 3];  t += (int)x1[  32*2] * wp[ 3];
				s += (int)x2[  32*2] * wp[ 4];  t += (int)x1[- 32*2] * wp[ 4];
				s += (int)x2[  96*2] * wp[ 5];  t += (int)x1[- 96*2] * wp[ 5];
				s += (int)x2[ 160*2] * wp[ 6];  t += (int)x1[-160*2] * wp[ 6];
				s += (int)x2[ 224*2] * wp[ 7];  t += (int)x1[-224*2] * wp[ 7];
				s += (int)x1[-256*2] * wp[ 8];  t += (int)x2[ 256*2] * wp[16];
				s += (int)x1[-192*2] * wp[ 9];  t += (int)x2[ 192*2] * wp[17];
				s += (int)x1[-128*2] * wp[10];  t += (int)x2[ 128*2] * wp[18];
				s += (int)x1[- 64*2] * wp[11];  t += (int)x2[  64*2] * wp[19];
				s += (int)x1[   0*2] * wp[12];  t += (int)x2[   0*2] * wp[20];
				s += (int)x1[  64*2] * wp[13];  t += (int)x2[- 64*2] * wp[21];
				s += (int)x1[ 128*2] * wp[14];  t += (int)x2[-128*2] * wp[22];
				s += (int)x1[ 192*2] * wp[15];  t += (int)x2[-192*2] * wp[23];
				
				a[30+i*2] =  shft4(t)          + shft13(s) * wp[24];
				a[31+i*2] = shft13(t) * wp[25] - shft13(s) * wp[26];
				wp += 27;
				x1 -=  2;
				x2 +=  2;
			}
			
			t  =  (int)x1[- 16*2]            * wp[ 8];  s  = (int)x1[ -32*2] * wp[0];
			t += ((int)x1[- 48*2]-x1[ 16*2]) * wp[10];  s += (int)x1[ -96*2] * wp[1];
			t += ((int)x1[- 80*2]+x1[ 48*2]) * wp[12];  s += (int)x1[-160*2] * wp[2];
			t += ((int)x1[-112*2]-x1[ 80*2]) * wp[14];  s += (int)x1[-224*2] * wp[3];
			t += ((int)x1[-144*2]+x1[112*2]) * wp[16];  s += (int)x1[  32*2] * wp[4];
			t += ((int)x1[-176*2]-x1[144*2]) * wp[18];  s += (int)x1[  96*2] * wp[5];
			t += ((int)x1[-208*2]+x1[176*2]) * wp[20];  s += (int)x1[ 160*2] * wp[6];
			t += ((int)x1[-240*2]-x1[208*2]) * wp[22];  s += (int)x1[ 224*2] * wp[7];
			
			u = shft4(s - t);
			v = shft4(s + t);
			t = a[14];
			s = a[15] - t;
			
			a[31] = v + t;   
			a[30] = u + s;   
			a[15] = u - s;   
			a[14] = v - t;   
		}
		wk -= 18 * 64 - 1; 
	}
}

void window_subband1_1ch(short *wk, int sb0[SBLIMIT], int sb1[SBLIMIT])
{
	int   k, i, u, v;
	const short *wp;
	short *x1, *x2;
	int s, t, *a;
	
	a = sb0;
	for(k=0; k<18; k++, wk+=32, a+=SBLIMIT)
	{
		wp = enwindow;
		x1 = wk;
		x2 = x1 - 64;

		for(i=-15; i<0; i++)
		{
			s  = (int)x2[-224] * wp[ 0];  t  = (int)x1[ 224] * wp[ 0];
			s += (int)x2[-160] * wp[ 1];  t += (int)x1[ 160] * wp[ 1];
			s += (int)x2[- 96] * wp[ 2];  t += (int)x1[  96] * wp[ 2];
			s += (int)x2[- 32] * wp[ 3];  t += (int)x1[  32] * wp[ 3];
			s += (int)x2[  32] * wp[ 4];  t += (int)x1[- 32] * wp[ 4];
			s += (int)x2[  96] * wp[ 5];  t += (int)x1[- 96] * wp[ 5];
			s += (int)x2[ 160] * wp[ 6];  t += (int)x1[-160] * wp[ 6];
			s += (int)x2[ 224] * wp[ 7];  t += (int)x1[-224] * wp[ 7];
			s += (int)x1[-256] * wp[ 8];  t += (int)x2[ 256] * wp[16];
			s += (int)x1[-192] * wp[ 9];  t += (int)x2[ 192] * wp[17];
			s += (int)x1[-128] * wp[10];  t += (int)x2[ 128] * wp[18];
			s += (int)x1[- 64] * wp[11];  t += (int)x2[  64] * wp[19];
			s += (int)x1[   0] * wp[12];  t += (int)x2[   0] * wp[20];
			s += (int)x1[  64] * wp[13];  t += (int)x2[- 64] * wp[21];
			s += (int)x1[ 128] * wp[14];  t += (int)x2[-128] * wp[22];
			s += (int)x1[ 192] * wp[15];  t += (int)x2[-192] * wp[23];

			a[30+i*2] =  shft4(t)          + shft13(s) * wp[24];
			a[31+i*2] = shft13(t) * wp[25] - shft13(s) * wp[26];
			wp += 27;
			x1 --;
			x2 ++;
		}

		t  =  (int)x1[- 16]          * wp[ 8];  s  = (int)x1[ -32] * wp[0];
		t += ((int)x1[- 48]-x1[ 16]) * wp[10];  s += (int)x1[ -96] * wp[1];
		t += ((int)x1[- 80]+x1[ 48]) * wp[12];  s += (int)x1[-160] * wp[2];
		t += ((int)x1[-112]-x1[ 80]) * wp[14];  s += (int)x1[-224] * wp[3];
		t += ((int)x1[-144]+x1[112]) * wp[16];  s += (int)x1[  32] * wp[4];
		t += ((int)x1[-176]-x1[144]) * wp[18];  s += (int)x1[  96] * wp[5];
		t += ((int)x1[-208]+x1[176]) * wp[20];  s += (int)x1[ 160] * wp[6];
		t += ((int)x1[-240]-x1[208]) * wp[22];  s += (int)x1[ 224] * wp[7];

		u = shft4(s - t);
		v = shft4(s + t);
		t = a[14];
		s = a[15] - t;

		a[31] = v + t;   
		a[30] = u + s;   
		a[15] = u - s;   
		a[14] = v - t;   
	}
}

void window_subband2(int a[SBLIMIT])
{
	int   xr;
	const short *wp = enwindow;

	wp += 27 * 15;
	
	xr = a[28] - a[0];  a[0] += a[28];  a[28] = shft9(xr) * wp[-2*27+25];
	xr = a[29] - a[1];  a[1] += a[29];  a[29] = shft9(xr) * wp[-2*27+25];
	xr = a[26] - a[2];  a[2] += a[26];  a[26] = shft9(xr) * wp[-4*27+25];
	xr = a[27] - a[3];  a[3] += a[27];  a[27] = shft9(xr) * wp[-4*27+25];
	xr = a[24] - a[4];  a[4] += a[24];  a[24] = shft9(xr) * wp[-6*27+25];
	xr = a[25] - a[5];  a[5] += a[25];  a[25] = shft9(xr) * wp[-6*27+25];
	xr = a[22] - a[6];  a[6] += a[22];  a[22] = shft9(xr) * SQRT        ;
	xr = a[23] - a[7];  a[7] += a[23];  a[23] = shft9(xr) * SQRT  - a[7];
	a[ 7] -= a[ 6];
	a[22] -= a[ 7];
	a[23] -= a[22];
    
	xr = a[ 6];  a[ 6] = a[31] - xr;  a[31] = a[31] + xr;
	xr = a[ 7];  a[ 7] = a[30] - xr;  a[30] = a[30] + xr;
	xr = a[22];  a[22] = a[15] - xr;  a[15] = a[15] + xr;
	xr = a[23];  a[23] = a[14] - xr;  a[14] = a[14] + xr;
	
	xr = a[20] - a[ 8];  a[ 8] += a[20];  a[20] = shft9(xr) * wp[-10*27+25];
	xr = a[21] - a[ 9];  a[ 9] += a[21];  a[21] = shft9(xr) * wp[-10*27+25];
	xr = a[18] - a[10];  a[10] += a[18];  a[18] = shft9(xr) * wp[-12*27+25];
	xr = a[19] - a[11];  a[11] += a[19];  a[19] = shft9(xr) * wp[-12*27+25];
	xr = a[16] - a[12];  a[12] += a[16];  a[16] = shft9(xr) * wp[-14*27+25];
	xr = a[17] - a[13];  a[13] += a[17];  a[17] = shft9(xr) * wp[-14*27+25];
	xr =-a[20] + a[24];  a[20] += a[24];  a[24] = shft9(xr) * wp[-12*27+25];
	xr =-a[21] + a[25];  a[21] += a[25];  a[25] = shft9(xr) * wp[-12*27+25];
	xr = a[ 4] - a[ 8];  a[ 4] += a[ 8];  a[ 8] = shft9(xr) * wp[-12*27+25];
	xr = a[ 5] - a[ 9];  a[ 5] += a[ 9];  a[ 9] = shft9(xr) * wp[-12*27+25];
	xr = a[ 0] - a[12];  a[ 0] += a[12];  a[12] = shft9(xr) * wp[ -4*27+25];
	xr = a[ 1] - a[13];  a[ 1] += a[13];  a[13] = shft9(xr) * wp[ -4*27+25];
	xr = a[16] - a[28];  a[16] += a[28];  a[28] = shft9(xr) * wp[ -4*27+25];
	xr =-a[17] + a[29];  a[17] += a[29];  a[29] = shft9(xr) * wp[ -4*27+25];
	
	xr = SQRT * shft9(a[ 2] - a[10]);  a[ 2] += a[10];  a[10] = xr;
	xr = SQRT * shft9(a[ 3] - a[11]);  a[ 3] += a[11];  a[11] = xr;
	xr = SQRT * shft9(a[26] - a[18]);  a[18] += a[26];  a[26] = xr - a[18];
	xr = SQRT * shft9(a[27] - a[19]);  a[19] += a[27];  a[27] = xr - a[19];
	
	xr = a[ 2];  a[19] -= a[ 3];  a[ 3] -= xr;  a[ 2] = a[31] - xr;  a[31] += xr;
	xr = a[ 3];  a[11] -= a[19];  a[18] -= xr;  a[ 3] = a[30] - xr;  a[30] += xr;
	xr = a[18];  a[27] -= a[11];  a[19] -= xr;  a[18] = a[15] - xr;  a[15] += xr;
	
	xr = a[19];  a[10] -= xr;  a[19] = a[14] - xr;  a[14] += xr;
	xr = a[10];  a[11] -= xr;  a[10] = a[23] - xr;  a[23] += xr;
	xr = a[11];  a[26] -= xr;  a[11] = a[22] - xr;  a[22] += xr;
	xr = a[26];  a[27] -= xr;  a[26] = a[ 7] - xr;  a[ 7] += xr;
	
	xr = a[27];  a[27] = a[6] - xr;  a[6] += xr;
	
	xr = SQRT * shft9(a[ 0] - a[ 4]);  a[ 0] += a[ 4];  a[ 4] = xr;
	xr = SQRT * shft9(a[ 1] - a[ 5]);  a[ 1] += a[ 5];  a[ 5] = xr;
	xr = SQRT * shft9(a[16] - a[20]);  a[16] += a[20];  a[20] = xr;
	xr = SQRT * shft9(a[17] - a[21]);  a[17] += a[21];  a[21] = xr;
	xr =-SQRT * shft9(a[ 8] - a[12]);  a[ 8] += a[12];  a[12] = xr - a[ 8];
	xr =-SQRT * shft9(a[ 9] - a[13]);  a[ 9] += a[13];  a[13] = xr - a[ 9];
	xr =-SQRT * shft9(a[25] - a[29]);  a[25] += a[29];  a[29] = xr - a[25];
	xr =-SQRT * shft9(a[24] + a[28]);  a[24] -= a[28];  a[28] = xr - a[24];
	
	xr = a[24] - a[16]; a[24] = xr;
	xr = a[20] - xr;    a[20] = xr;
	xr = a[28] - xr;    a[28] = xr;
	
	xr = a[25] - a[17]; a[25] = xr;
	xr = a[21] - xr;    a[21] = xr;
	xr = a[29] - xr;    a[29] = xr;
	
	xr = a[17] - a[1];  a[17] = xr;
	xr = a[ 9] - xr;    a[ 9] = xr;
	xr = a[25] - xr;    a[25] = xr;
	xr = a[ 5] - xr;    a[ 5] = xr;
	xr = a[21] - xr;    a[21] = xr;
	xr = a[13] - xr;    a[13] = xr;
	xr = a[29] - xr;    a[29] = xr;
	
	xr = a[ 1] - a[0];  a[ 1] = xr;
	xr = a[16] - xr;    a[16] = xr;
	xr = a[17] - xr;    a[17] = xr;
	xr = a[ 8] - xr;    a[ 8] = xr;
	xr = a[ 9] - xr;    a[ 9] = xr;
	xr = a[24] - xr;    a[24] = xr;
	xr = a[25] - xr;    a[25] = xr;
	xr = a[ 4] - xr;    a[ 4] = xr;
	xr = a[ 5] - xr;    a[ 5] = xr;
	xr = a[20] - xr;    a[20] = xr;
	xr = a[21] - xr;    a[21] = xr;
	xr = a[12] - xr;    a[12] = xr;
	xr = a[13] - xr;    a[13] = xr;
	xr = a[28] - xr;    a[28] = xr;
	xr = a[29] - xr;    a[29] = xr;
	
	xr = a[ 0];  a[ 0] += a[31];  a[31] -= xr;
	xr = a[ 1];  a[ 1] += a[30];  a[30] -= xr;
	xr = a[16];  a[16] += a[15];  a[15] -= xr;
	xr = a[17];  a[17] += a[14];  a[14] -= xr;
	xr = a[ 8];  a[ 8] += a[23];  a[23] -= xr;
	xr = a[ 9];  a[ 9] += a[22];  a[22] -= xr;
	xr = a[24];  a[24] += a[ 7];  a[ 7] -= xr;
	xr = a[25];  a[25] += a[ 6];  a[ 6] -= xr;
	xr = a[ 4];  a[ 4] += a[27];  a[27] -= xr;
	xr = a[ 5];  a[ 5] += a[26];  a[26] -= xr;
	xr = a[20];  a[20] += a[11];  a[11] -= xr;
	xr = a[21];  a[21] += a[10];  a[10] -= xr;
	xr = a[12];  a[12] += a[19];  a[19] -= xr;
	xr = a[13];  a[13] += a[18];  a[18] -= xr;
	xr = a[28];  a[28] += a[ 3];  a[ 3] -= xr;
	xr = a[29];  a[29] += a[ 2];  a[ 2] -= xr;
}