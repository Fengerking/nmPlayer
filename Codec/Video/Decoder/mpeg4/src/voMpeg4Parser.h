/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
	
#ifndef _MP4_HEADER_H_
#define _MP4_HEADER_H_
#include "voMpeg4DecGlobal.h"
#include "voMpeg4Dec.h"

#define VOL_START_CODE_MASK				0x0f

enum
{
	VOL_START_CODE = 0x120,
	VOP_START_CODE = 0x1b6,
	USERDATA_START_CODE = 0x1b2,
	S263_PIC_START_CODE = 0x00008
};

enum
{
	INTER =	0,
	INTER_Q,
	INTER4V,
	INTRA,	
	INTRA_Q
};

enum
{
	DIRECT = 0,
	INTERPOLATE,
	BACKWARD,
	FORWARD	
};

enum
{
	RECTANGULAR	= 0,
	BINARY,
	BINARY_SHAPE_ONLY, 
	GREY_SCALE
};

enum
{
	SPRITE_STATIC =	1,
	SPRITE_GMC = 2
};

enum
{
	ASPECT_SQUARE	=				1,
	ASPECT_625TYPE_43	=			2,
	ASPECT_525TYPE_43	=			3,
	ASPECT_625TYPE_169	=			8,
	ASPECT_525TYPE_169	=			9,
	ASPECT_CUSTOM		=			15
};

enum
{
	LEFT = 0,
	TOP
};

#define RESCUE	1

//#define _VOBASE_RC 0x80000000

#define CHECK_ERROR(result) {\
if((result &VO_ERR_BASE))\
	return result;\
	}\
	

extern VO_U32 DIVX3GetHdr(struct VO_MPEG4_DEC* , const VO_S32 nHeaderFlag);
extern VO_U32 MPEG4GetHdr(struct VO_MPEG4_DEC*, const VO_S32 nHeaderFlag);
extern VO_U32 H263GetHdr( VO_MPEG4_DEC*, const VO_S32 nHeaderFlag);
extern VO_U32 S263GetHdr( VO_MPEG4_DEC*, const VO_S32 nHeaderFlag);

extern VO_S32 H263GetMBA(VO_MPEG4_DEC* pDec, VO_U32 nWidth, VO_U32 nHeight);
extern VO_S32 Resync(VO_MPEG4_DEC *pDec, VO_S32 *pMBpos);

extern VOCONST VO_U8 def_quant_inter[64];
extern VOCONST VO_U8 def_quant_intra[64];

extern VOCONST VO_U8 default_h263_chroma_qscale_table[32];
extern VOCONST VO_U8 h263_chroma_qscale_table[32];

static INLINE VO_S32 _log2(VO_U32 data)
{
	VO_S32 i;
	if (!data) ++data;
	for (i=0;data;++i)
		data >>= 1;
    return i;
}

#endif

