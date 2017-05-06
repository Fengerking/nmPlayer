#include "decoder.h"
#ifdef  BSAC_DEC
#include "VOI_decode_bsac.h"
/*
#include "block.h"               / * handler, defines, enums * /
#include "tf_mainHandle.h"
#include "sam_tns.h"             / * struct * /
#include "sam_dec.h"*/

#if USE_EXTRA_BITBUFFER
#define  MINIMUM    3
#define  MAX_LENGTH  32
#define  ALIGNING  8

static unsigned char bs_buf[4096];  /* bit stream buffer */
static int  bs_buf_size;    /* size of buffer (in number of bytes) */
static int  bs_buf_byte_idx;  /* pointer to top byte in buffer */
static int  bs_buf_bit_idx;    /* pointer to top bit of top byte in buffer */
static int  bs_eob;      /* end of buffer index */
static int  bs_eobs;    /* end of bit stream flag */


/* open the device to read the bit stream from it */
void sam_init_layer_buf()
{
	int i;
	bs_buf_byte_idx=4095;
	bs_buf_bit_idx=0;
	bs_eob = 0;
	bs_eobs = 0;
	bs_buf_size = 4096;
	for(i = 0; i < bs_buf_size; i++)
		bs_buf[i] = 0;
}

void sam_setRBitBufPos(int pos)
{
	bs_buf_byte_idx = pos / 8;
	bs_buf_bit_idx  = 8 - (pos % 8);
}

int sam_getRBitBufPos()
{
	return ((bs_buf_byte_idx * 8) + (8 - bs_buf_bit_idx));
}

static int putmask[9]={0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff};

/*read N bit from the bit stream */
unsigned int sam_getbitsfrombuf(int N)
{
	unsigned long val=0;
	register int i;
	register int j = N;
	register int k, tmp;
	
	if(N <= 0) return 0;
	
	// if (N > MAX_LENGTH)
	//  printf("Cannot read or write more than %d bits at a time.\n", MAX_LENGTH);
	
	while (j > 0) {
		if (!bs_buf_bit_idx) {
			bs_buf_bit_idx = 8;
			bs_buf_byte_idx++;
			if ((bs_buf_byte_idx >= (bs_buf_size - MINIMUM)) || (bs_buf_byte_idx == bs_eob)) {
				if (bs_eob) {
					bs_eobs = 1;
					return 0;
				} else {
					for (i=bs_buf_byte_idx; i<bs_buf_size;i++)
						bs_buf[i-bs_buf_byte_idx] = bs_buf[i];
					//fprintf(stderr, "Bit buffer error!\n");
				}
			}
		}
		k = (j < bs_buf_bit_idx) ? j : bs_buf_bit_idx;
		tmp = bs_buf[bs_buf_byte_idx]&putmask[bs_buf_bit_idx];
		val |= (tmp >> (bs_buf_bit_idx-k)) << (j-k);
		bs_buf_bit_idx -= k;
		j -= k;
	}
	
	return(val);
}

/*write N bits into the bit stream */
int sam_putbits2buf(unsigned int val, int N)
{
	register int j = N;
	register int k, tmp;
	
	if (N < 1)
		return 0;
	if (N > MAX_LENGTH) {
		// printf("Cannot read or write more than %d bits[%d] at a time.\n", MAX_LENGTH, val);
		return 0;
	}
	
	while (j > 0) {
		k = (j < bs_buf_bit_idx) ? j : bs_buf_bit_idx;
		tmp = val >> (j-k);
		bs_buf[bs_buf_byte_idx] |= (tmp&putmask[k]) << (bs_buf_bit_idx-k);
		bs_buf_bit_idx -= k;
		if (!bs_buf_bit_idx) {
			bs_buf_bit_idx = 8;
			bs_buf_byte_idx++;
			if (bs_buf_byte_idx > bs_buf_size-1) {
				//fprintf(stderr, "SAM: bitstream buffer overflow!\n");
				return 0;
			}
		}
		j -= k;
	}
	return N;
}
#endif//USE_EXTRA_BITBUFFER
#endif  /* VERSION 2 */
