 /******************************************************************************************MakeNode
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/*  \file     h265dec_list.h
    \brief    list for the pic
    \author   Jo Liu
	  \change
*/

#ifndef __H265_DEC_LIST__
#define __H265_DEC_LIST__

#include "h265_decoder.h"
#include "h265dec_config.h"


extern VO_VOID CreateDpbList(H265_DPB_LIST *p_dpb_list, TComPic* dpb_pool);
extern VO_VOID  PushDisPic(H265_DPB_LIST *p_dpb_list, TComPic*  p_dis_pic);
extern TComPic*  PopDisPic(H265_DEC *p_dec, H265_DPB_LIST *p_dpb_list, VO_S32* next_pic_ready);
extern VO_VOID sortPicList(H265_DPB_LIST* p_dpb_list);
extern VO_VOID ClearDpbList(H265_DPB_LIST *p_dpb_list);
extern VO_VOID xWriteOutput(/*H265_DEC *p_dec,*/ H265_DPB_LIST* pcListPic, VO_S32 reorder_num );

#endif
