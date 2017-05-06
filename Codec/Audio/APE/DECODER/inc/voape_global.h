/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2010 	    *
*									                                    *
************************************************************************/
/************************************************************************
File:		voAPE_Global.h

Contains:	APE Decoder global structure define		

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

**************************************************************************/
#ifndef __VOAPE_GLOBAL_H__
#define __VOAPE_GLOBAL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include  "voAPETag.h"
#include  "voAPE_Header.h"
#include  "voAPE_Predictor.h"
#include  "voAPEBits.h"
#include  "cmnMemory.h"
#include  "stream.h"

#define  BLOCKS_NUMBERS   1024

//typedef struct{
//	unsigned char  *input_buf;
//	unsigned char  *output_buf;
//	int            input_len;
//	int            output_len;
//	int            used_len;
//	int            over_byte;
//}Buffer_Op;

typedef struct{
	voAPE_DESCRIPTOR   *ape_desc;            
	voAPE_HEADER       *ape_hdr;
	voAPE_HEADER_OLD   *ape_hdr_old;
	voAPE_FILE_INFO    *ape_info;
	voCNNFState        *Xcnnf;
	voCNNFState        *Ycnnf;
	voDecodeBytes      *DecProcess;
	FrameStream        *stream_buf;
	int                PredFlag;                /* Predictor malloc flag */
	int                voAPED_State;
	VO_MEM_OPERATOR    *pvoMemop;
	VO_MEM_OPERATOR    voMemoprator;	
	void              *hCheck;
}voAPE_GData;

int  voDecodeValueRange(voAPE_GData *st, voUnbit_Array_State *BitArrayState);
int  voDecompressValue3930to3950(voAPE_GData *st, int nInput);

extern void *g_hAPEDecInst;

#define SPECIAL_FRAME_MONO_SILENCE              1
#define SPECIAL_FRAME_LEFT_SILENCE              1
#define SPECIAL_FRAME_RIGHT_SILENCE             2
#define SPECIAL_FRAME_PSEUDO_STEREO             4

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   //__VOAPE_GLOBAL_H__


