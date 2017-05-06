#ifndef _PARSET_H_
#define _PARSET_H_

#include "parsetcommon.h"
#include "nalucommon.h"


extern void Scaling_List(H264DEC_G *pDecGlobal,int *scalingList, int sizeOfScalingList, Boolean *UseDefaultScalingMatrix);

extern void InitVUI(seq_parameter_set_rbsp_t *sps);
extern int  ReadVUI(H264DEC_G *pDecGlobal,DataPartition *p, seq_parameter_set_rbsp_t *sps);
extern int  ReadHRDParameters(H264DEC_G *pDecGlobal,DataPartition *p, hrd_parameters_t *hrd);

extern void PPSConsistencyCheck (pic_parameter_set_rbsp_t *pps);
extern void SPSConsistencyCheck (seq_parameter_set_rbsp_t *sps);

extern void MakePPSavailable (H264DEC_G *pDecGlobal, int id, pic_parameter_set_rbsp_t *pps);
extern void MakeSPSavailable (H264DEC_G *pDecGlobal, int id, seq_parameter_set_rbsp_t *sps);

extern int ProcessSPS (H264DEC_G *pDecGlobal, NALU_t *nalu);
extern int ProcessPPS (H264DEC_G *pDecGlobal, NALU_t *nalu);

extern void CleanUpPPS(H264DEC_G *pDecGlobal);

extern void activate_sps (H264DEC_G *pDecGlobal, seq_parameter_set_rbsp_t *sps);
extern void activate_pps (H264DEC_G *pDecGlobal, pic_parameter_set_rbsp_t *pps);

#if (MVC_EXTENSION_ENABLE)
extern void SubsetSPSConsistencyCheck (subset_seq_parameter_set_rbsp_t *subset_sps);
extern void ProcessSubsetSPS (H264DEC_G *pDecGlobal, NALU_t *nalu);

extern void mvc_vui_parameters_extension(H264DEC_G *pDecGlobal,MVCVUI_t *pMVCVUI, Bitstream *s);
extern void seq_parameter_set_mvc_extension(H264DEC_G *pDecGlobal,subset_seq_parameter_set_rbsp_t *subset_sps, Bitstream *s);
extern void init_subset_sps_list(subset_seq_parameter_set_rbsp_t *subset_sps_list, int iSize);
extern void reset_subset_sps(subset_seq_parameter_set_rbsp_t *subset_sps);
extern int  GetBaseViewId(H264DEC_G *pDecGlobal, subset_seq_parameter_set_rbsp_t **subset_sps);
extern void get_max_dec_frame_buf_size(seq_parameter_set_rbsp_t *sps);
#endif

#endif
