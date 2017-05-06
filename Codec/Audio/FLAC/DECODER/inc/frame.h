#ifndef __FLAC_FRAME_H__
#define __FLAC_FRAME_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stream.h"

#define	MAX_CHANNEL						8
#define MAX_LPC_ORDER					32
#define MIN_QLP_COEFF_PRECISION			5
#define MAX_FIXED_ORDER					4
#define MAX_RICE_PARTITION_ORDER		15
extern void *g_hFLACDecInst;

#define CLIPTOSHORT(x)  ((((x) >> 31) == (x >> 15))?(x):((x) >> 31) ^ 0x7fff)

typedef enum {
    INDEPENDENT,
    LEFT_SIDE,
    RIGHT_SIDE,
    MID_SIDE
}CHANMIX;

typedef enum {
	SUBFRAME_CONSTANT,
	SUBFRAME_VERBATIM,
	SUBFRAME_FIXED,
	SUBFRAME_LPC
}SUBFRAME_TYPE;

typedef struct _StreamInfo{
	int MinBlockSize;
	int MaxBlockSize;
	int MinFrameSize;
	int MaxFrameSize;
	int SampleRate;
	int Channels;
	int BitsPerSample;	
	unsigned char md5sum[16];
}MStreamInfo;

typedef struct _FrameHeader{
	int BlockSize; 
	int SampleRate; 
	int Channels;
	CHANMIX ChanAssignment;
	int BitsperSample;
	unsigned char Crc;
	int64 nDecNum;
}FrameHeader;

typedef struct _SubFrame{
	SUBFRAME_TYPE SubType; 
	int WastedBits;
	int order;
	int ResType;
	int blockSize;
	int* sample;
}SubFrame;

typedef struct _FALCDecInfo {
	MStreamInfo		MetaInfo;
	FrameHeader		Fheader;
	SubFrame		Fsubframe[MAX_CHANNEL];
	FrameStream		*stream;
	VO_MEM_OPERATOR *pvoMemop;
	VO_MEM_OPERATOR voMemoprator;
	unsigned short	footerCrc;
	int				channel;
	int				samplerate;
	VO_PTR          hCheck;
}FLACDecInfo;

int voFLACDecReadMetaInfo(FLACDecInfo *decoder, unsigned char* indata, int length);
int voFLACDecUpdateFrameBuffer(SubFrame *fsubframe, int blockSize, VO_MEM_OPERATOR *pMemOP);
int voFLACDecHeaderDecode(FLACDecInfo *decoder);
int voFLACDecodeSubFrame(FLACDecInfo *decoder, int nch);
int voFLACDecChanAssignMix(FLACDecInfo *decoder, short *samples);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
