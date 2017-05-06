
/*!
 ************************************************************************
 * \file macroblock.h
 *
 * \author
 *  Inge Lille-Langøy               <inge.lille-langoy@telenor.com>
 *  Copyright (C) 1999  Telenor Satellite Services, Norway
 ************************************************************************
 */

#ifndef _MACROBLOCK_H_
#define _MACROBLOCK_H_
#include "defines.h"
//#define USE_AVG_ASM 0
#if defined(ARM_ASM)&&!defined(ARMv4)
#define USE_AVG_ASM SIAVG_ASM
#if USE_AVG_ASM
#define SIAvgBlock_C	SIAvgBlock_ASM
#endif//USE_AVG_ASM
#endif//ARM_ASM

#if (DUMP_VERSION & DUMP_SLICE)
extern StorablePicture* gRefFrame;
static int idx=0;
#define TURNOFF_LEVELRUN 0
#define TURNOFF_DEQ 1
#define OUTPUT_OFFSET 0
extern Bitstream* debugBitStream;
void DumpMBBeforeDeblock(ImageParameters *img,TMBsProcessor* info,int mbStart,int mbEnd);
void __inline checkbits(int bits,TMBsProcessor* info)
{
	int value = ShowBits(debugBitStream, bits);
	AvdLog(DUMP_SLICE,DUMP_DCORE"\ncheckbits=%X,offset=%d",value,debugBitStream->frame_bitoffset);
}
void __inline AvdLogDeq(int id, TMBsProcessor* info,char* format, int a,int b,int c,int d,int e)
{
	if(TURNOFF_DEQ)
		return;
	AvdLog(id,DUMP_DCORE "\n(%d,%d),deq=%d(%d,%d)",a,b,c,d,e);
}
void __inline AvdLogLevel(ImageParameters *img,TMBsProcessor* info,int id, char* format, int tmpLevarr, int tmpRunarr)
{
	if(TURNOFF_LEVELRUN)
		return;
#if OUTPUT_OFFSET
	/*static int preBitoffset = 0;
	int	   offset=0;

	offset = preBitoffset-img->currentSlice->partArr[0].bitstream->bBitToGo;
	if(offset<0)
	offset = offset+32;
	preBitoffset = img->currentSlice->partArr[0].bitstream->bBitToGo;*/

	AvdLog(id,DUMP_DCORE "\n%u:level(%d), run(%d) %x", idx++,tmpLevarr, tmpRunarr,ShowBits(debugBitStream,16));//offset);

#else
	//idx=0;
	AvdLog(id, DUMP_DCORE"\n%u:level(%d), run(%d)", idx++,tmpLevarr, tmpRunarr);
#endif
}

#define DUMP_MV 0
#define STORE_REF_FRAME(a) gRefFrame=(a);
#else
#define STORE_REF_FRAME(a)
#define checkbits(a)
#endif//DUMP_VERSION & DUMP_SLICE)

#if FEATURE_BFRAME
#define MEMSET_B8PDIR(val)	mbBits->uMBS.b8Info.b8Pdir[0] = mbBits->uMBS.b8Info.b8Pdir[1] = mbBits->uMBS.b8Info.b8Pdir[2] = mbBits->uMBS.b8Info.b8Pdir[3] = (avdUInt8)val; //memset(&mbBits->uMBS.b8Info.b8Pdir[0], val, 4 * sizeof(avdUInt8))
#else // VOI_H264D_NON_BASELINE
#define MEMSET_B8PDIR(val) // do nothing;
#endif //VOI_H264D_NON_BASELINE
#define MEMSET_B8MODE(val)		mbBits->uMBS.b8Info.b8Mode[0] = mbBits->uMBS.b8Info.b8Mode[1] = mbBits->uMBS.b8Info.b8Mode[2] = mbBits->uMBS.b8Info.b8Mode[3] = (avdUInt8)val; //memset(&mbBits->uMBS.b8Info.b8Mode[0], val, 4 * sizeof(avdUInt8));
#define TRACE_STRING(s) // do nothing
#define rshift_rnd_sf(x, a) (((x) + (1 << ((a)-1) )) >> (a))
#define CHECK_REF2(a,b,c) CheckRef(a,b,c);CHECK_ERR_RET_NULL

#define		AVD_6TAP(m2, m1, p0, p1, p2, p3) (m2+p3-5*(m1+p2)+20*(p0+p1)) 
#define M7_WIDTH		24


extern StorablePicture* gRefFrame;
typedef struct{
	avdUInt32 mv0;
	avdUInt32 mv1;
	avdNativeInt	ref0;
	avdNativeInt	ref1;
}TMVRef1;

#endif//MACRO_BLOCKH

