#include "decoder.h"
#ifdef  BSAC_DEC
#include "decode_bsac.h"
/* ARITHMETIC DECODING ALGORITHM. */

/* DECLARATIONS USED FOR ARITHMETIC ENCODING AND DECODING */

/* SIZE OF ARITHMETIC CODE VALUES */
#define Code_value_bits     16          /* Number of bits in a code value */
typedef unsigned long code_value;       /* Type of an arithmetic code value */

#define Top_value 0x3fffffff

/* HALF AND QUARTER POINTS IN THE CODE VALUE RANGE. */
#define First_qtr 0x10000000        /* Point after first quarter */
#define Half      0x20000000        /* Point after frist half    */
#define Third_qtr 0x30000000        /* Point after third quarter */

/* CURRENT STATE OF THE DECODING */
static code_value value;  /* Currently-seen code value           */
static code_value range;  /* Size of the current code region */
static int est_cw_len=30;  /* estimated codeword length */

struct ArDecoderStr {
	code_value value;  /* Currently-seen code value           */
	code_value range;  /* Size of the current code region */
	int cw_len;      /* estimated codeword length */
};

typedef struct ArDecoderStr ArDecoder;

/* ARRAY FOR STORING STATES OF SEVERAL DECODING */
static ArDecoder arDec[64];

const code_value half[16] = 
{
	0x20000000, 0x10000000, 0x08000000, 0x04000000, 
	0x02000000, 0x01000000, 0x00800000, 0x00400000,  
	0x00200000, 0x00100000, 0x00080000, 0x00040000, 
	0x00020000, 0x00010000, 0x00008000, 0x00004000 
};


/* Initialize the Parameteres of the i-th Arithmetic Decoder */
void sam_initArDecode(int arDec_i)
{
	arDec[arDec_i].value = 0;
	arDec[arDec_i].range = 1;
	arDec[arDec_i].cw_len = 30;
}

/* Store the Working Parameters for the i-th Arithmetic Decoder */
void sam_storeArDecode(int arDec_i)
{
	arDec[arDec_i].value = value;
	arDec[arDec_i].range = range;
	arDec[arDec_i].cw_len = est_cw_len;
}

/* Set the i-th Arithmetic Decoder Working */
void sam_setArDecode(int arDec_i)
{
	value = arDec[arDec_i].value;
	range = arDec[arDec_i].range;
	est_cw_len = arDec[arDec_i].cw_len;
}

/* DECODE THE NEXT SYMBOL. */
int sam_decode_symbol (BitStream *bs, const short cum_freq[], int *symbol)
{
#if 1//hbftodo
	//int  cum;    /* Cumulative frequency calculated */
	int  sym;      /* Symbol decoded       */
	
	if (est_cw_len) {
		range = (range<<est_cw_len);
#if USE_EXTRA_BITBUFFER  
		value = (value<<est_cw_len) | BitStreamGetBits(bs, est_cw_len);
#else
		value = (value<<est_cw_len) | BitStreamGetBits(bs, est_cw_len);//sam_getbitsfrombuf(est_cw_len);
#endif
	}
	
	range >>= 14;
	//cum = value/range;        /* Find cum freq */ 
	
	/* Find symbol */
	for (sym=0; cum_freq[sym]*range>value; sym++);
	
	*symbol = sym;
	
	/* Narrow the code region to that allotted to this symbol. */ 
	value -= (range * cum_freq[sym]);
	
	if (sym > 0) {
		range = range * (cum_freq[sym-1]-cum_freq[sym]);
	}
	else {
		range = range * (16384-cum_freq[sym]);
	}
	
	for(est_cw_len=0; range<half[est_cw_len]; est_cw_len++);
#if DUMPBSAC
	{
		DPRINTF(DUMPBSAC_spectra,"\nsam_decode_symbol****range=%d,value=%d",range,value);
	}
#endif//
	return est_cw_len;
#else
	int  cum;    /* Cumulative frequency calculated */
	int  sym;      /* Symbol decoded       */
	
	if (est_cw_len) {
		range = (range<<est_cw_len);
		value = (value<<est_cw_len) | sam_getbitsfrombuf(est_cw_len);
	}
	
	range >>= 14;
	cum = value/range;        /* Find cum freq */ 
	
	for (sym=0; cum_freq[sym]>cum; sym++);
	
	*symbol = sym;
	
	/* Narrow the code region to that allotted to this symbol. */ 
	value -= (range * cum_freq[sym]);
	
	if (sym > 0) {
		range = range * (cum_freq[sym-1]-cum_freq[sym]);
	}
	else {
		range = range * (16384-cum_freq[sym]);
	}
	
	for(est_cw_len=0; range<half[est_cw_len]; est_cw_len++);
	
	return est_cw_len;
#endif
}

/* Binary Arithmetic decoder */
int sam_decode_symbol2 (BitStream *bs, int freq0, int *symbol)
{
	unsigned int temp=0;
	if (est_cw_len) {
		range = (range << est_cw_len);
#if USE_EXTRA_BITBUFFER  
		temp = BitStreamGetBits(bs, est_cw_len);
#else
		temp = BitStreamGetBits(bs, est_cw_len);//sam_getbitsfrombuf(est_cw_len);
#endif
		value = (value << est_cw_len) | temp;
	}

	range >>= 14;
	
	/* Find symbol */
	if ( (freq0 * range) <= value ) {
		*symbol = 1;
		
		value -= range * freq0;
		range =  range * (16384-freq0);
	}
	else {
		*symbol = 0;
		
		range = range * freq0;
	}
    
	for(est_cw_len=0; range<half[est_cw_len]; est_cw_len++);
	
	return est_cw_len;
}

#endif /* VERSION 2 */
