#ifndef __FRAME_H___
#define __FRAME_H___

#include "voMP3.h"
#include "config.h"
#include "Bits.h"
#include "Huffman.h"

typedef struct {
    int   type; /* 0=(22.05,24,16kHz) 1=(44.1,48,32kHz) */
	int   layer;
	int   mode; /* 0=stereo, 1=jstereo, 2=dual, 3=mono  */
    int   padding;
    int   channels;
    int   bitr_id;
    int   smpl_id;
} HeadInfo;

/* Side information */
typedef struct {
    int		part23Length;				
    int		nBigvals;		
	int		count1;
    int		globalGain;					
    int		sfCompress;					
    int		blockType;
    int		tableSelect[3];	
	int     region_0_1;
    int		region0Count;				
    int		region1Count;	
    int		count1TableSelect;
	int     quantStep;
	int		additStep;
	int     max_val;
	int		*factScale;
} SideInfoSub;

typedef struct _SideInfo {
	int mainDataBegin;
	int privateBits;
	int scfsi[MAX_NCHAN]; /* 4 scalefactor bands per channel */
	
	SideInfoSub	sis[MAX_NGRAN][MAX_NCHAN];
} SideInfo;

typedef struct {
	HeadInfo    header;		
	SideInfo	SideInfoPS;
	int			sbsample[MAX_NCHAN][MAX_NGRAN][18][32];
	int			xr[MAX_NCHAN][576];
	short		ix[576];
	int			si_len;
	int			ngrans;
    int         channels;
	int			samplerate;
	int			bitrate;
	int			num_bands;
    int         mean_bits;
    int         ResvSize;
} FrameInfo;

typedef struct _MP3DecInfo{
    Bitstream		*stream;
    FrameInfo		*frame;
	short			*encbuf;
	short			*inbuf;
	unsigned char   *headdata;
	unsigned char   *maindata;
	int				mainlen;
	int				enclen;
	int				inlen;
	int				frac_per_frame;
    int				byte_per_frame;
    int				slot_lag;

	int				initset;
	int				uselength;

	void			*hCheck;

	VO_MEM_OPERATOR *pvoMemop;
	VO_MEM_OPERATOR voMemoprator;
}MP3EncInfo;

void window_subband1_2ch(short *wk, int sb0[SBLIMIT], int sb1[SBLIMIT]);
void window_subband1_1ch(short *wk, int sb0[SBLIMIT], int sb1[SBLIMIT]);
void window_subband2(int a[SBLIMIT]);
void iteration_loop(FrameInfo *encInfo, Bitstream *stream, SideInfoSub *si, int *xr, int gr_cnt, int ch);
void Huffmancodebits(Bitstream *stream, short *ix, int *xr, SideInfoSub *gi);
int encHeadSideInfo(MP3EncInfo *encInfo);
void mdct_long(int *out, int *in);
int encMP3(MP3EncInfo *encInfo);
int choose_table( short *ix, unsigned int begin, unsigned int end, int *bits );
int encInit(MP3EncInfo *encInfo, int nch, int samplerate, int bitrate);

extern unsigned int sfBand[6][23];
extern const short int2idx[4096];
extern const int order[32];
extern const int bitr_index[2][16];
extern const int num_bands[3][15];
extern const int cx[9];
extern const int ca[8];
extern const int cs[8];
extern const int samTab[2][3];
extern const short enwindow[15*27+24];
extern const int win[18][4];


#endif