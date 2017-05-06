# ifndef LIBADPCM_FRAME_H
# define LIBADPCM_FRAME_H

#include "voMem.h"
#include "Stream.h"
#include "mem_align.h"

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */
#define INT_MAX       2147483647		/* maximum (signed) int value */

#define GET2BYTE(buf)  ((buf[1] << 8) | buf[0])
#define GET4BYTE(buf)  ((buf[3] << 24) | buf[2] << 16 | buf[1] << 8 | buf[0])

/* swf adpcm step table can reuse the "ima_step_size" table */
static const int step_table[89] = { 
	0x7,  
	0x8,  0x9,  0xa,  0xb,  0xc,  0xd,  0xe,  0x10,  
	0x11,  0x13,  0x15,  0x17,  0x19,  0x1c,  0x1f,  0x22,  
	0x25,  0x29,  0x2d,  0x32,  0x37,  0x3c,  0x42,  0x49,  
	0x50,  0x58,  0x61,  0x6b,  0x76,  0x82,  0x8f,  0x9d,  
	0xad,  0xbe,  0xd1,  0xe6,  0xfd,  0x117,  0x133,  0x151,  
	0x173,  0x198,  0x1c1,  0x1ee,  0x220,  0x256,  0x292,  0x2d4,  
	0x31c,  0x36c,  0x3c3,  0x424,  0x48e,  0x502,  0x583,  0x610,  
	0x6ab,  0x756,  0x812,  0x8e0,  0x9c3,  0xabd,  0xbd0,  0xcff,  
	0xe4c,  0xfba,  0x114c,  0x1307,  0x14ee,  0x1706,  0x1954,  0x1bdc,  
	0x1ea5,  0x21b6,  0x2515,  0x28ca,  0x2cdf,  0x315b,  0x364b,  0x3bb9,  
	0x41b2,  0x4844,  0x4f7e,  0x5771,  0x602f,  0x69ce,  0x7462,  0x7fff
};

typedef struct _MSADPCMInfo{
	int				numcoeffs ;
	struct
	{	short	coeff1;
		short	coeff2;
	}	coeffs [10] ;
}MSADPCMInfo;

typedef struct _IMAADPCMInfo{
	int				previous [2];
	int				stepindx [2];
}IMAADPCMInfo;

typedef struct _ITU_G726Info{
    int sr[2];      /**< prev. reconstructed samples */
    int dq[6];      /**< prev. difference */
    int a[2];           /**< second order predictor coeffs */
    int b[6];           /**< sixth order predictor coeffs */
    int pk[2];          /**< signs of prev. 2 sez + dq */

    int ap;             /**< scale factor control */
    int yu;             /**< fast scale factor */
    int yl;             /**< slow scale factor */
    int dms;            /**< short average magnitude of F[i] */
    int dml;            /**< long average magnitude of F[i] */
    int td;             /**< tone detect */

	int* iquant_tab;     /**< inverse quantization table */
    int* W_tab;          /**< special table #1 ;-) */
    int* F_tab;          /**< special table #2 */

	int bits_left;
    int bit_buffer;
    int code_size;
}ITU_G726Info;

typedef struct _ADPCMDecInfo{
    short			frametype;
	short			channel;
	int				samplerate;
	int				avgbytespersec;
	short			blockalign;
	short			BitsPerSample;
	short			extrabytes;
	int				framesperblock;
	unsigned char	*decoder_buf;
	void			*adpcm;
	FrameStream		*stream;

	void			*hCheck;

	VO_MEM_OPERATOR *pvoMemop;
	VO_MEM_OPERATOR voMemoprator;
}ADPCMDecInfo;

MSADPCMInfo *voADPCMDecInitMSInfo(VO_MEM_OPERATOR *pMemop, int blockalign, int framesperblock);
IMAADPCMInfo *voADPCMDecInitIMAInfo(VO_MEM_OPERATOR *pMemop, int blockalign, int framesperblock);
ITU_G726Info *voADPCMDecInitITUG726Info(VO_MEM_OPERATOR *pMemop, int avgbytespersec);

int voADPCMDecReadFMTChunk(ADPCMDecInfo *decoder, unsigned char* indata, int length);
int voADPCMDecADPCM(ADPCMDecInfo *decoder, short* samples);
int voADPCMDecMSADPCMDec(ADPCMDecInfo *decoder, short* samples);
int voADPCMDecIMAADPCMDec(ADPCMDecInfo *decoder, short* samples);
int voADPCMDecITUG726ADPCMDec(ADPCMDecInfo *decoder, short* samples);
int voADPCMDecuLawADPCMDec(ADPCMDecInfo *decoder, short* samples);
int voADPCMDecaLawADPCMDec(ADPCMDecInfo *decoder, short* samples);
int voADPCMDecSWFADPCMDec(ADPCMDecInfo *decoder, short* samples);

#endif
