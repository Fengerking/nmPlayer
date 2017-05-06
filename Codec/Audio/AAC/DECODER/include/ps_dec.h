        /************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#ifndef __PS_DEC_H__
#define __PS_DEC_H__

#ifdef PS_DEC

#include "bitstream.h"
#include "struct.h"

#define INLINE  __inline

#define MAX_PS_ENVELOPES 5
#define MAX_REV_LINK_DELAY 5 
#define NO_ALLPASS_LINKS 3

/* type definitaions */
typedef const char (*HuffPsTab)[2];

typedef struct
{
    unsigned char framelen;
    unsigned char resolution20[3];
	
    int WorkBuffer[32+12][2];
    int SubQMFBuffer[5][32][2];
    int TmpBuffer[32][12][2];

	int uBuffer[48];
} hyb_info;

typedef struct
{
    unsigned char bEnableIid;
    unsigned char bEnableIcc;
    unsigned char bEnableExt;

	unsigned char IidMode;
    unsigned char IccMode;
    unsigned char NrIidNum;
    unsigned char NrIccNum;

    unsigned char bFrameClass;
    unsigned char NumEnv;
    unsigned char bAvailblePsData;
	unsigned char bEnableHeader;

	unsigned char NumGroups;
    unsigned char NumHybridGroups;
    unsigned char NrParBands;
    unsigned char NrAllpassBands;
    unsigned char DecayCutoff;

    unsigned char aEnvStartStop[MAX_PS_ENVELOPES+1];

    unsigned char IidDtFlag[MAX_PS_ENVELOPES];
    unsigned char IccDtFlag[MAX_PS_ENVELOPES];

    /* indices */
    char IidIndexPrev[34];
    char IccIndexPrev[34];
    char IidIndex[MAX_PS_ENVELOPES][36];
    char IccIndex[MAX_PS_ENVELOPES][36];

    hyb_info *pHybrid;


    unsigned char *GroupBorder;
    unsigned short *MapGroup2bk;

    int alphaDecay;
    int alphaSmooth;

    unsigned char SavedDelay;
    unsigned char DelayBufIndexSer[NO_ALLPASS_LINKS];
    unsigned char DelayD[64];
    unsigned char DelayBufIndex[64];

    int  DelayBufferQmf[14][64][2]; 
    int  DelayBufferSubQmf[2][32][2]; 
    int  DelayBufferQmfSer[NO_ALLPASS_LINKS][MAX_REV_LINK_DELAY][64][2]; 
    int  DelayBufferSubQmfSer[NO_ALLPASS_LINKS][MAX_REV_LINK_DELAY][32][2]; 
	int  LBuf[38][64][2];
	int  RBuf[38][64][2];
	int  LHybrid[32][32][2];
    int  RHybrid[32][32][2];

    int  PeakDecayFast[34];
    int  PrevNrg[34];
    int  PrevPeakDiff[34];

    int  h11Prev[50];
    int  h12Prev[50];
    int  h21Prev[50];
    int  h22Prev[50];
} ps_info;


/* ps_syntax.c */
int ReadPsStream(ps_info *ps, BitStream *bs);

/* ps_dec.c */
ps_info *ps_init(VO_MEM_OPERATOR *voMemop, unsigned char sr_index);
void ps_free(VO_MEM_OPERATOR *voMemop, ps_info *ps);

unsigned char ps_decode(AACDecoder* decoder,ps_info *ps);
void psDataInit(ps_info *ps);
void deCorrelate(AACDecoder* decoder, ps_info *ps, int* X_left, int* X_right,
                           int* X_hybrid_left, int* X_hybrid_right);
void applyRotation(AACDecoder* decoder, ps_info *ps, int* X_left, int* X_right,
                         int* X_hybrid_left, int* X_hybrid_right);

hyb_info *hybrid_init();
void ChannelFilter2(unsigned char frame_len, const int *filter,
                            int *buffer, int *X_hybrid, int *tmp);
void ChannelFilter8(unsigned char frame_len, const int *filter,
                            int *buffer, int *X_hybrid, int *tmp);
void HybridAnalysis(VO_MEM_OPERATOR *voMemop, hyb_info *hyb, int *XBuf, 
							int *X_hybrid);
void HybridSynthesis(hyb_info *hyb, int *XBuf, int *X_hybrid);
void DeltaDecArray(unsigned char enable, char *index, char *index_prev,
                         unsigned char dt_flag, unsigned char nr_par, unsigned char stride,
                         char min_index, char max_index);


extern const unsigned char NrIidTab[8];
extern const unsigned char NrIccTab[8];
extern const unsigned char NrIpdOpdTab[8];
extern const unsigned char NumEnvTab[2][4];
extern const char fHuffIidBook[28][2];
extern const char tHuffIidBook[28][2];
extern const char fHuffIidFineBook[60][2];
extern const char tHuffIidFineBook[60][2];
extern const char fHuffIccBook[14][2];
extern const char tHuffIccBook[14][2];

#endif//PS_DEC

#endif
