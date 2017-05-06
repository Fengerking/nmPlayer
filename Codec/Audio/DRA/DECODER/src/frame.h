#ifndef LIBDRA_FRAME_H
#define LIBDRA_FRAME_H

#include "config.h"
#include "global.h"
#include "stream.h"
#include "voMem.h"

typedef enum _DRA_WINDOWS_TYPE{
	WIN_LONG_LONG2LONG		= 0x0000,
	WIN_LONG_LONG2SHORT		= 0x0001,		
	WIN_LONG_SHORT2LONG		= 0x0002,
	WIN_LONG_SHORT2SHORT	= 0x0003,
	WIN_LONG_LONG2BRIEF		= 0x0004,
	WIN_LONG_BRIEF2LONG		= 0x0005,	
	WIN_LONG_BRIEF2BRIEF	= 0x0006,
	WIN_LONG_SHORT2BRIEF	= 0x0007,
	WIN_LONG_BRIEF2SHORT	= 0x0008,
		
	WIN_SHORT_SHORT2SHORT	= 0x0009,
	WIN_SHORT_SHORT2BRIEF	= 0x000a,
	WIN_SHORT_BRIEF2BRIEF	= 0x000b,
	WIN_SHORT_BRIEF2SHORT	= 0x000c,
}DRA_WINDOWS_TYPE;

typedef struct _FrameHeader{
	short nFrmHeaderType;	
	short nNumWord;	
	short nNumBlocksPerFrm;
	short nSamplerateRateIndex;
	short nNumNormalCh;
	short nNumLfeCh;
	short bAuxChCfg;
	short bUseSumdiff;
	short bUseJIC;
	short nJicCb;
} FrameHeader;

typedef struct _FrameDataInfo{
	short nWinTypeCurrent;	
	short nNumCluster;	
	short anNumBlocksPerFrmPerCluster[MAX_CLUSTER_NUM];
	short anHSNumBands[MAX_CLUSTER_NUM];
	unsigned char mnHSBandEdge[MAX_CLUSTER_NUM][MAX_HSBANDEDGE];
	unsigned char mnHS[MAX_CLUSTER_NUM][MAX_HSBANDEDGE];	
	unsigned char mnQStepIndex[MAX_CLUSTER_NUM][MAX_CBANDNUM];
} FrameDataInfo;

typedef struct _FrameSumDff{
	unsigned char anSumDffAllOff[MAX_FRAMELENGTH];
	unsigned char mnSumDffOn[MAX_CLUSTER_NUM][MAX_CBANDNUM];
} FrameSumDff;

typedef struct _FrameJicScale{
	unsigned char  mnQStepIndex[MAX_CLUSTER_NUM][MAX_CBANDNUM];
} FrameJicScale;

typedef struct _FrameCHInfo{
	int			  anQIndex[MAX_FRAMELENGTH];
	int			  overlap[MAX_FRAMELENGTH];
	FrameDataInfo frame;	
	short		  anMaxActcb[MAX_CLUSTER_NUM];
	short		  anClusterBin0[MAX_CLUSTER_NUM];
	short		  pnWinTypeShort[8];
	short		  preWinType;
	FrameSumDff	  *fSumDff;
	FrameJicScale *fJicScale;
	int			  *deinterleaved;
} FrameCHInfo;

typedef struct _Sfbwidth{
	unsigned short const *l;
	unsigned short const *s;
}Sfbwidth;

typedef struct _DraDecInfo{
	FrameStream* stream;
	FrameHeader* header;
	FrameCHInfo* chInfo[MAX_CHANNEL];
	int*		 afBinNatural;
	unsigned char *decoder_buf;
	Sfbwidth*	 sfbw;
	int			 channelNum;
	int			 LefNum;
	int			 channelSpec;
	int			 chanDecNum;
	int			 samplerate;
	int			 bitsPersample;

	void		*hCheck;

	VO_MEM_OPERATOR *pvoMemop;
	VO_MEM_OPERATOR voMemoprator;
} DraDecInfo;


int	 HeaderSync(unsigned char *, int );
int  HeaderDecode(FrameHeader *, FrameStream *);
int	 DecoderFrame(DraDecInfo *, short *);
int  InterToNatural(FrameCHInfo *, int *);
int  MaxActCB(FrameCHInfo *, Sfbwidth*);
int  MSdecode(FrameCHInfo *, FrameCHInfo *, short *, int);
int  ISdeocde(FrameCHInfo *, FrameCHInfo *, short *, int);
int  FilterBand(DraDecInfo *, short *, int);
int  UnpackFrameData(DraDecInfo*);
int	 UnpackWinSequence(DraDecInfo*, int nch);
int	 UnpackCodeBooks(FrameCHInfo*, Bitstream *);
int	 UnpackQIndex(FrameCHInfo*, Bitstream *);
int  UnpackStepIndex(FrameCHInfo*, Bitstream *);
int  UnpackSumDff(FrameCHInfo*, int , short *, Bitstream *);
int  UnpackJicScale(FrameCHInfo*, int , short *, Bitstream *);


extern const int samplerateTab[16];
extern const Sfbwidth sfbwidth_table[16]; 
extern const int nQStepSize[116];
extern const int nJicStepSize[116];
extern const int nWinShape[32 + 128 + 1024];
extern const int cos4sin4tab[128 + 1024];
extern const int twidTabEven[4*6 + 16*6 + 64*6];
extern const int twidTabOdd[8*6 + 32*6 + 128*6];
extern const int cos1sin1tab[514];
extern const unsigned char bitrevtab[17 + 129];
extern const int nfftTab[2];
extern const int cos4sin4tabOffset[2];
extern const int nfftlog2Tab[2];
extern const int nmdctTab[2];
extern const int postSkip[2];
extern const int bitrevtabOffset[2];
extern const int sinWindowOffset[3];


#endif




















