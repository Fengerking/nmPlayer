/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*****************************************************************************/
/* bit stream manipulation routines                                          */
/*****************************************************************************/
#include "typedef.h"
#include "ld8a.h"
#include "tab_ld8a.h"

static void bit2byte(Word16 para,int bitlen,unsigned char * bits,int bitpos) ; 
static Word16 byte2bit(int bitlen,unsigned char * bits,int bitpos) ; 

void prm2bits_ld8k(Word16 *para,Word16 *bits, int flag)
{
	int i;
	int bitpos = 0;

    if(flag)
	{
    *bits++ = SYNC_WORD;
	*bits++ = RATE_8000;
	}
    
	for (i = 0;i<PRM_SIZE;i++) {
		bit2byte(*para++,bitsno[i],(unsigned char *)bits,bitpos);
		bitpos += bitsno[i];
	}
}

void bit2byte(Word16 para,int bitlen,unsigned char * bits,int bitpos)
{
	int i;
	int bit = 0;
	unsigned char newbyte = 0;
	unsigned char *p = bits + (bitpos / 8);
	for (i = 0 ;i < bitlen; i++) {
		bit = (para >> (bitlen - i -1) ) &0x01;
		newbyte = (1 << (7 - bitpos % 8));
		if (bit == 1)
			*p |= newbyte;
		else
			*p &= ~newbyte;
		bitpos++;
		if (bitpos % 8 == 0)
			p++;
	}
} 

void bits2prm_ld8k(unsigned char *bits,Word16 *para)
{
	int i;
	int bitpos = 0;
	for (i = 0;i<PRM_SIZE;i++) {
		*para++ = byte2bit(bitsno[i],bits,bitpos);
		bitpos += bitsno[i];
	}
}

Word16 byte2bit(int bitlen,unsigned char * bits,int bitpos)
{
	int i;
	int bit = 0;
	Word16 newbyte = 0;
	Word16 value = 0;
	unsigned char *p = bits + (bitpos / 8);
	for (i = 0 ;i < bitlen; i++) {
		bit = (*p >> (7 - bitpos % 8)) &0x01;
		if (bit == 1) {
			newbyte = (1 << (bitlen - i -1));
			value |= newbyte;
		}
		bitpos++;
		if (bitpos % 8 == 0)
			p++;
	}
	return value;
}


