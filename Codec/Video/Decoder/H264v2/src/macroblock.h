#ifndef _MACROBLOCK_H_
#define _MACROBLOCK_H_

// #include "global.h"
#include "mbuffer.h"
// #include "block.h"

extern void setup_slice_methods(H264DEC_G *pDecGlobal,Slice *currSlice);
extern void start_macroblock     (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Slice *currSlice);
extern int  decode_one_macroblock(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture);
extern void  exit_macroblock  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Slice *currSlice);
extern void update_qp            (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int qp);


#endif

