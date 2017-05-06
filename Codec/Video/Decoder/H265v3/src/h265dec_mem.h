 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_men.h
    \brief    Memory related stuff 
    \author   
	\change
*/


#ifndef _H265DEC_MEM_H_
#define _H265DEC_MEM_H_
#include "voType.h"
#include "voMem.h"
#include "h265dec_porting.h"
#include "h265dec_ID.h"

#define SAFETY		16
#define EDGE_SIZE	32
#define EDGE_SIZE2  (EDGE_SIZE/2)

//YU_TBD: fine tune cache line size
#define CACHE_LINE	64

#define ALIGN_MEM(out_mem){\
	if((VO_U32)(out_mem) & ~(CACHE_LINE - 1)){\
		out_mem = 	(VO_U8 *) ((VO_U32) (out_mem + CACHE_LINE - 1) &\
							 (~(VO_U32) (CACHE_LINE - 1)));\
	}\
}

/*
  For platform dependent align declaration.
*/
#if (defined(__GNUC__) && __GNUC__) || defined(__SUNPRO_C)
  #define DECLARE_ALIGNED(n,typ,val)  typ val __attribute__ ((aligned (n)))
#elif defined(_MSC_VER)
  #define DECLARE_ALIGNED(n,typ,val)  __declspec(align(n)) typ val
#else
  #warning No alignment directives known for this compiler.
  #define DECLARE_ALIGNED(n,typ,val)  typ val
#endif

extern void *AlignMalloc(VO_CODEC_INIT_USERDATA * p_user_op, VO_U32 codec_id, VO_U32 size, VO_U32 align);
extern void AlignFree(VO_CODEC_INIT_USERDATA * p_user_op, VO_U32 codec_id, VO_PTR p_dst);
extern void SetMem(VO_CODEC_INIT_USERDATA * p_user_op, VO_U32 codec_id, VO_PTR p_dst, VO_U8 value, VO_U32 size);
extern VOINLINE void CopyMem(VO_CODEC_INIT_USERDATA * p_user_op, VO_U32 codec_id, VO_PTR p_dst, VO_CPTR p_src, VO_U32 size);


#endif							/* _H265DEC_MEM_H_ */
