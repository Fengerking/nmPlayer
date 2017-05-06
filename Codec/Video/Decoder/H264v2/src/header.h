#ifndef _HEADER_H_
#define _HEADER_H_

extern int FirstPartOfSliceHeader(H264DEC_G *pDecGlobal,Slice *currSlice);
extern int RestOfSliceHeader     (H264DEC_G *pDecGlobal,Slice *currSlice);

extern VO_S32 dec_ref_pic_marking(H264DEC_G *pDecGlobal, Slice *pSlice);

extern VO_S32 decode_poc(H264DEC_G *pDecGlobal, Slice *pSlice);
#endif

