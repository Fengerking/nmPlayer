

#ifndef _QUANT_H_
#define _QUANT_H_

// exported functions
// quantization initialization
extern void init_qp_process (H264DEC_G *pDecGlobal);
extern void free_qp_matrices(H264DEC_G *pDecGlobal);

// For Q-matrix
extern void assign_quant_params   (H264DEC_G *pDecGlobal,Slice *currslice);
extern void CalculateQuant4x4Param(H264DEC_G *pDecGlobal,Slice *currslice);
extern void CalculateQuant8x8Param(H264DEC_G *pDecGlobal,Slice *currslice);
//add by Really Yang 20110321
extern void DeQuant4x4dc(VO_S16* temp,int invLevelScale,int qp_per);
extern void DeQuant2x2dc(VO_S16* temp,int invLevelScale,int qp_per);
extern void DeQuant4x4(VO_S32* cof,int* invLevelScale4x4,int qp_per);
//end of add

#endif

