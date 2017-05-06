#include "config.h"
#include "global.h"
#include "frame.h"

#define COS0_00  0x4013c251	/* Q31 */
#define COS0_01  0x40b345bd	/* Q31 */
#define COS0_02  0x41fa2d6d	/* Q31 */
#define COS0_03  0x43f93421	/* Q31 */
#define COS0_04  0x46cc1bc4	/* Q31 */
#define COS0_05  0x4a9d9cf0	/* Q31 */
#define COS0_06  0x4fae3711	/* Q31 */
#define COS0_07  0x56601ea7	/* Q31 */
#define COS0_08  0x5f4cf6eb	/* Q31 */
#define COS0_09  0x6b6fcf26	/* Q31 */
#define COS0_10 0x7c7d1db3	/* Q31 */
#define COS0_11 0x4ad81a97	/* Q30 */
#define COS0_12 0x5efc8d96	/* Q30 */
#define COS0_13 0x41d95790	/* Q29 */
#define COS0_14 0x6d0b20cf	/* Q29 */
#define COS0_15 0x518522fb	/* Q27 */

#define COS1_00  0x404f4672	/* Q31 */
#define COS1_01  0x42e13c10	/* Q31 */
#define COS1_02  0x48919f44	/* Q31 */
#define COS1_03  0x52cb0e63	/* Q31 */
#define COS1_04  0x64e2402e	/* Q31 */
#define COS1_05  0x43e224a9	/* Q30 */
#define COS1_06  0x6e3c92c1	/* Q30 */
#define COS1_07  0x519e4e04	/* Q28 */

#define COS2_00  0x4140fb46	/* Q31 */
#define COS2_01  0x4cf8de88	/* Q31 */
#define COS2_02  0x73326bbf	/* Q31 */
#define COS2_03  0x52036742	/* Q29 */

#define COS3_00  0x4545e9ef	/* Q31 */
#define COS3_01  0x539eba45	/* Q30 */

#define COS4_00  0x5a82799a	/* Q31 */

#ifdef ARMV7
const int dcttab[48] = {
	 0x4013c251, 0x518522fb, 0x404f4672, 0x40b345bd,
	 0x6d0b20cf, 0x42e13c10, 0x41fa2d6d, 0x41d95790,
	 0x48919f44, 0x43f93421, 0x5efc8d96, 0x52cb0e63,
	 0x46cc1bc4, 0x4ad81a97, 0x64e2402e, 0x4a9d9cf0,
	 0x7c7d1db3, 0x43e224a9, 0x4fae3711, 0x6b6fcf26,
	 0x6e3c92c1, 0x56601ea7, 0x5f4cf6eb, 0x519e4e04,
	 0x4140fb46, 0x52036742, 0x4545e9ef, 0xbebf04ba,
	 0xadfc98be, 0x4545e9ef, 0x4140fb46, 0x52036742,
	 0x4545e9ef, 0xbebf04ba, 0xadfc98be, 0x4545e9ef,
	 0x4cf8de88, 0x73326bbf, 0x539eba45, 0xb3072178,
	 0x8ccd9441, 0x539eba45, 0x4cf8de88, 0x73326bbf,
	 0x539eba45, 0xb3072178, 0x8ccd9441, 0x539eba45
};

const int dctIdx[12] = {
	0x4, 0x2, 0x2, 0x1,
	0x1, 0x0, 0x0, 0x0,
	0x0, 0x1, 0x1, 0x3
};

#endif

//iOS ARMv6 asm table
const int dcttab_v6[48] = {
	0x4013c251, 0x518522fb, 0x404f4672, 0x40b345bd, 0x6d0b20cf, 0x42e13c10,
	0x41fa2d6d, 0x41d95790, 0x48919f44, 0x43f93421, 0x5efc8d96, 0x52cb0e63,
	0x46cc1bc4, 0x4ad81a97, 0x64e2402e, 0x4a9d9cf0, 0x7c7d1db3, 0x43e224a9,
	0x4fae3711, 0x6b6fcf26, 0x6e3c92c1, 0x56601ea7, 0x5f4cf6eb, 0x519e4e04,
	0x4140fb46, 0x52036742, 0x4545e9ef, 0x4cf8de88, 0x73326bbf, 0x539eba45,
	0xbebf04ba, 0xadfc98be, 0x4545e9ef, 0xb3072178, 0x8ccd9441, 0x539eba45,
	0x4140fb46, 0x52036742, 0x4545e9ef, 0x4cf8de88, 0x73326bbf, 0x539eba45,
	0xbebf04ba, 0xadfc98be, 0x4545e9ef, 0xb3072178, 0x8ccd9441, 0x539eba45,
};

#ifndef ARM
// faster in ROM
static const int dcttab[48] = {
	/* first pass */
	COS0_00, COS0_15, COS1_00,	/* 31, 27, 31 */
	COS0_01, COS0_14, COS1_01,	/* 31, 29, 31 */
	COS0_02, COS0_13, COS1_02,	/* 31, 29, 31 */
	COS0_03, COS0_12, COS1_03,	/* 31, 30, 31 */
	COS0_04, COS0_11, COS1_04,	/* 31, 30, 31 */
	COS0_05, COS0_10, COS1_05,	/* 31, 31, 30 */
	COS0_06, COS0_09, COS1_06,	/* 31, 31, 30 */
	COS0_07, COS0_08, COS1_07,	/* 31, 31, 28 */
	/* second pass */
	 COS2_00,  COS2_03, COS3_00,	/* 31, 29, 31 */
	 COS2_01,  COS2_02, COS3_01,	/* 31, 31, 30 */
	-COS2_00, -COS2_03, COS3_00, 	/* 31, 29, 31 */
	-COS2_01, -COS2_02, COS3_01, 	/* 31, 31, 30 */
	 COS2_00,  COS2_03, COS3_00, 	/* 31, 29, 31 */
	 COS2_01,  COS2_02, COS3_01, 	/* 31, 31, 30 */
	-COS2_00, -COS2_03, COS3_00, 	/* 31, 29, 31 */
	-COS2_01, -COS2_02, COS3_01, 	/* 31, 31, 30 */
};

#define D32FP(i, s0, s1, s2) { \
    a0 = buf[i];			a3 = buf[31-i]; \
	a1 = buf[15-i];			a2 = buf[16+i]; \
    b0 = a3 + a0;			b3 = MUL_32(*cptr++, a0 - a3) << (s0);	\
	b1 = a2 + a1;			b2 = MUL_32(*cptr++, a1 - a2) << (s1);	\
	buf[i] = b1 + b0;		buf[15-i] = MUL_32(*cptr,   b0 - b1) << (s2); \
	buf[16+i] = b3 + b2;    buf[31-i] = MUL_32(*cptr++, b3 - b2) << (s2); \
}

void voMP3DecDCT32(int *buf, int *dest, int offset, int oddBlock)
{
    int i, s, tmp, es;
    const int *cptr = dcttab;
    int a0, a1, a2, a3, a4, a5, a6, a7;
    int b0, b1, b2, b3, b4, b5, b6, b7;
	int *d;
	
	/* first pass */    
	D32FP(0, 1, 5, 1);
	D32FP(1, 1, 3, 1);
	D32FP(2, 1, 3, 1);
	D32FP(3, 1, 2, 1);
	D32FP(4, 1, 2, 1);
	D32FP(5, 1, 1, 2);
	D32FP(6, 1, 1, 2);
	D32FP(7, 1, 1, 4);

	/* second pass */
	for (i = 4; i > 0; i--) {
		a0 = buf[0]; 	    a7 = buf[7];		a3 = buf[3];	    a4 = buf[4];
		b0 = a0 + a7;	    b7 = MUL_32(*cptr++, a0 - a7) << 1;
		b3 = a3 + a4;	    b4 = MUL_32(*cptr++, a3 - a4) << 3;
		a0 = b0 + b3;	    a3 = MUL_32(*cptr,   b0 - b3) << 1;
		a4 = b4 + b7;		a7 = MUL_32(*cptr++, b7 - b4) << 1;

		a1 = buf[1];	    a6 = buf[6];	    a2 = buf[2];	    a5 = buf[5];
		b1 = a1 + a6;	    b6 = MUL_32(*cptr++, a1 - a6) << 1;
		b2 = a2 + a5;	    b5 = MUL_32(*cptr++, a2 - a5) << 1;
		a1 = b1 + b2;		a2 = MUL_32(*cptr,   b1 - b2) << 2;
		a5 = b5 + b6;	    a6 = MUL_32(*cptr++, b6 - b5) << 2;

		b0 = a0 + a1;	    b1 = MUL_32(COS4_00, a0 - a1) << 1;
		b2 = a2 + a3;	    b3 = MUL_32(COS4_00, a3 - a2) << 1;
		buf[0] = b0;	    buf[1] = b1;
		buf[2] = b2 + b3;	buf[3] = b3;

		b4 = a4 + a5;	    b5 = MUL_32(COS4_00, a4 - a5) << 1;
		b6 = a6 + a7;	    b7 = MUL_32(COS4_00, a7 - a6) << 1;
		b6 += b7;
		buf[4] = b4 + b6;	buf[5] = b5 + b7;
		buf[6] = b5 + b6;	buf[7] = b7;

		buf += 8;
	}
	buf -= 32;	/* reset */

	/* sample 0 - always delayed one block */
	d = dest + 64*16 + ((offset - oddBlock) & 7) + (oddBlock ? 0 : VBUF_LENGTH);
	s = buf[ 0];				d[0] = d[8] = s >> 7;
	/* samples 16 to 31 */
	d = dest + offset + (oddBlock ? VBUF_LENGTH  : 0);

	s = buf[ 1];				d[0] = d[8] = s >> 7;	d += 64;


	tmp = buf[25] + buf[29];
	s = buf[17] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[ 9] + buf[13];		d[0] = d[8] = s >> 7;	d += 64;
	s = buf[21] + tmp;			d[0] = d[8] = s >> 7;	d += 64;

	tmp = buf[29] + buf[27];
	s = buf[ 5];				d[0] = d[8] = s >> 7;	d += 64;
	s = buf[21] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[13] + buf[11];		d[0] = d[8] = s >> 7;	d += 64;
	s = buf[19] + tmp;			d[0] = d[8] = s >> 7;	d += 64;

	tmp = buf[27] + buf[31];
	s = buf[ 3];				d[0] = d[8] = s >> 7;	d += 64;
	s = buf[19] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[11] + buf[15];		d[0] = d[8] = s >> 7;	d += 64;
	s = buf[23] + tmp;			d[0] = d[8] = s >> 7;	d += 64;

	tmp = buf[31];
	s = buf[ 7];				d[0] = d[8] = s >> 7;	d += 64;
	s = buf[23] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[15];				d[0] = d[8] = s >> 7;	d += 64;
	s = tmp;					d[0] = d[8] = s >> 7;

	/* samples 16 to 1 (sample 16 used again) */
	d = dest + 16 + ((offset - oddBlock) & 7) + (oddBlock ? 0 : VBUF_LENGTH);

	s = buf[ 1];				d[0] = d[8] = s >> 7;	d += 64;

	tmp = buf[30] + buf[25];
	s = buf[17] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[14] + buf[ 9];		d[0] = d[8] = s >> 7;	d += 64;
	s = buf[22] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[ 6];				d[0] = d[8] = s >> 7;	d += 64;

	tmp = buf[26] + buf[30];
	s = buf[22] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[10] + buf[14];		d[0] = d[8] = s >> 7;	d += 64;
	s = buf[18] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[ 2];				d[0] = d[8] = s >> 7;	d += 64;

	tmp = buf[28] + buf[26];
	s = buf[18] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[12] + buf[10];		d[0] = d[8] = s >> 7;	d += 64;
	s = buf[20] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[ 4];				d[0] = d[8] = s >> 7;	d += 64;

	tmp = buf[24] + buf[28];
	s = buf[20] + tmp;			d[0] = d[8] = s >> 7;	d += 64;
	s = buf[ 8] + buf[12];		d[0] = d[8] = s >> 7;	d += 64;
	s = buf[16] + tmp;			d[0] = d[8] = s >> 7;
}
#endif
