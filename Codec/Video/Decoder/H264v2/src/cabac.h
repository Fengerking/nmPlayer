#ifndef _CABAC_H_
#define _CABAC_H_

#include "global.h"
extern void vo_init_cabac_states();
extern void cabac_new_slice(Slice *currSlice);
extern void vo_h264_init_cabac_states(Slice *currSlice); 
extern int readMB_typeInfo_CABAC_i_slice   (H264DEC_L *pDecLocal,VOCABACContext *dep_dp);
extern int readMB_typeInfo_CABAC_p_slice(H264DEC_L *pDecLocal,VOCABACContext *dep_dp,VO_S32* intra);
extern int readMB_typeInfo_CABAC_b_slice(H264DEC_L *pDecLocal,VOCABACContext *dep_dp,VO_S32* intra);
extern int readB8_typeInfo_CABAC_p_slice (H264DEC_L *pDecLocal,VOCABACContext *dep_dp);
extern int readB8_typeInfo_CABAC_b_slice (H264DEC_L *pDecLocal,VOCABACContext *dep_dp);
extern int readIntraPredMode_CABAC( H264DEC_L *pDecLocal,VOCABACContext *dep_dp);
extern int readRefFrame_CABAC(H264DEC_L *pDecLocal,VOCABACContext *dep_dp,int list,VO_S32 n);
extern VO_S32 readRefFrame_CABAC_p_slice(H264DEC_L *pDecLocal,VOCABACContext *dep_dp,int n,VO_S8 *ref_cache);
//extern void read_MVD_CABAC                  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, SyntaxElement *se, DecodingEnvironmentPtr dep_dp);
extern VO_S32 read_MVD_CABAC(H264DEC_L *pDecLocal, VOCABACContext *dep_dp,int n,short *mvd_cache,int k,short *);
extern int read_CBP_CABAC(H264DEC_L *pDecLocal,VOCABACContext *dep_dp);
extern void readRunLevel_CABAC (H264DEC_L *pDecLocal, int type,VOCABACContext *dep_dp,short *cof,int dc,int idx,int max,const VO_U8 *scan,int* inv,int qp_per);
extern void readRunLevel_CABAC_8x8 (H264DEC_L *pDecLocal,VOCABACContext *dep_dp,short *cof,int idx,const VO_U8 *scan,int* inv,int qp_per);
extern int read_dQuant_CABAC( Slice *currSlice,VOCABACContext *dep_dp);
extern int readCIPredMode_CABAC(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VOCABACContext *dep_dp);
extern int read_skip_flag_CABAC(  H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VOCABACContext *dep_dp,VO_S32 mbx,VO_S32 mby);
extern int readFieldModeInfo_CABAC         (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VOCABACContext *dep_dp);
extern int readMB_transform_size_flag_CABAC( H264DEC_L *pDecLocal,VOCABACContext *dep_dp);

extern void readIPCM_CABAC(H264DEC_G *pDecGlobal,Slice *currSlice, struct datapartition *dP);

extern int  cabac_startcode_follows(H264DEC_G *pDecGlobal,Slice *currSlice, int eos_bit);


//extern int check_next_mb_and_get_field_mode_CABAC_p_slice( H264DEC_G *pDecGlobal,Slice *currSlice, SyntaxElement *se, DataPartition  *act_dp);
//extern int check_next_mb_and_get_field_mode_CABAC_b_slice( H264DEC_G *pDecGlobal,Slice *currSlice, SyntaxElement *se, DataPartition  *act_dp);

//extern void CheckAvailabilityOfNeighborsCABAC(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Macroblock *currMB);

#endif  // _CABAC_H_

